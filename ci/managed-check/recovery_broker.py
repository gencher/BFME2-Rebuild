"""Stage exact-attempt recovery checkpoints with a read-only GitHub credential."""

import os
from pathlib import Path
import signal
import subprocess
import sys
from time import sleep, time

from common import HEX40, HEX64, REPO_ID, read, write
from recovery_artifacts import Actions, ArtifactPending
from wide_plan import identity
from wide_recovery import (
    GROUP_LIMIT, RESERVE_SECONDS, checked_lease, freeze, get_matrix, groups, label,
)

PRIMARY_SECONDS = 1200
MAX_RUN_AGE = 3600
MAX_PLAN_DELAY = 600
LEASE_LIMIT = 64 * 1024
POLL = 10


def output(values):
    with Path(os.environ["GITHUB_OUTPUT"]).open("a") as target:
        for key, value in values.items():
            target.write(f"{key}={value}\n")


def source_free_decision(source):
    decision = read(Path(source) / "decision.json", 4096)
    base_keys = {"version", "repo", "number", "base", "head", "tree", "kind", "plan"}
    auto_keys = base_keys | {"automatic_author", "automatic_kind"}
    keys = set(decision)
    auto_ok = (keys == base_keys
               or (keys == auto_keys and type(decision.get("automatic_author")) is int
                   and decision["automatic_author"] > 0 and decision.get("automatic_kind") == "wide"))
    if (not auto_ok
            or decision["version"] != 1 or decision["repo"] != REPO_ID
            or type(decision["number"]) is not int or decision["number"] <= 0
            or decision["kind"] != "wide" or decision["base"] != os.environ["GITHUB_SHA"]
            or not all(HEX40.fullmatch(decision[key]) for key in ("base", "head", "tree"))
            or not HEX64.fullmatch(decision["plan"])):
        raise ValueError("Expected source-free protected wide authorization; found an invalid decision.")
    return decision


def anchor(source):
    decision = source_free_decision(source)
    run_start, plan_start = Actions(decision["head"], base=decision["base"]).plan_anchor()
    now = time()
    if now - run_start > MAX_RUN_AGE or now - plan_start > MAX_PLAN_DELAY:
        raise ValueError("This attempt waited too long before planning; aborting before wide Boat fan-out.")
    cutoff = plan_start + PRIMARY_SECONDS
    output({"started": plan_start, "primary_cutoff": cutoff, "reserve_cutoff": cutoff + RESERVE_SECONDS})


def recovery_inputs(source, matrix, checksum, started, cutoff):
    decision = source_free_decision(source)
    doc = get_matrix(Path(matrix), checksum)
    if doc["partition"]["binding"] != {key: decision[key] for key in ("base", "head", "tree")}:
        raise ValueError("Expected the authorized wide matrix; found different revisions.")
    start, cutoff = int(started), int(cutoff)
    if cutoff - start != PRIMARY_SECONDS or start > time() or start + MAX_RUN_AGE < time():
        raise ValueError("A recovery job has an unavailable attempt window.")
    api = Actions(decision["head"], base=decision["base"])
    _run_start, plan_start = api.plan_anchor()
    if plan_start != start:
        raise ValueError("The recovery clock is not the service's wide planner clock.")
    return doc, api, start, cutoff


def collect_primary(doc, api, start, cutoff, directory, finish):
    prefix = "bfme-recovery-primary-" + api.run["attempt"] + "-"
    names = {prefix + label(pair): "primary-" + label(pair) for pair in groups(doc)}
    root = Path(directory)
    root.mkdir(mode=0o700)
    seen = {}
    while True:
        observed = api.inventory(names, earliest=start, cutoff=cutoff)
        if any(observed.get(name, observed_id) != observed_id for name, observed_id in seen.items()):
            raise ValueError("A primary checkpoint changed after its service timestamp was verified.")
        todo = {name: value for name, value in observed.items() if name not in seen}
        if todo:
            transferred = api.primary(todo, names, root)
            seen.update({name: todo[name] for name in transferred})
        if set(seen) == set(names):
            return True
        if time() >= finish:
            return False
        sleep(max(0, min(POLL, finish - time())))


def freeze_for_reserves(source, matrix, checksum, directory, target, started, cutoff):
    doc, api, start, cutoff = recovery_inputs(source, matrix, checksum, started, cutoff)
    collect_primary(doc, api, start, cutoff, directory, cutoff)
    lease = freeze(doc, checksum, directory, cutoff)
    write(target, lease)
    slots = sorted(set(lease["reserve"].values()))
    output({
        "lease_sha": identity(lease),
        "reserve_needed": str(bool(lease["reserve"])).lower(),
        "reserve_slots": "[" + ",".join(map(str, slots)) + "]",
        "has_primaries": str(bool(lease["primary"])).lower(),
    })
    print(f"Frozen {len(lease['primary'])} paired groups; leased {len(lease['reserve'])} groups.")


def fallback_primary(source, matrix, checksum, directory, target, started, cutoff):
    doc, api, start, cutoff = recovery_inputs(source, matrix, checksum, started, cutoff)
    # Allow the artifact service to settle after the original primary deadline.
    # Inventory still accepts only checkpoints published before the original cutoff.
    finish = max(cutoff + 60, min(cutoff + 320, time() + 30))
    if not collect_primary(doc, api, start, cutoff, directory, finish):
        raise ValueError("Expected every on-time primary receipt after recovery failed; found incomplete work.")
    lease = freeze(doc, checksum, directory, cutoff)
    if lease["reserve"]:
        raise ValueError("Primary-only fallback must never lease reserve work.")
    write(target, lease)
    output({"lease_sha": identity(lease), "reserve_needed": "false"})
    print(f"Verified {len(lease['primary'])} on-time paired groups after collector failure.")


def wait_for_lease(source, matrix, checksum, target, started, cutoff):
    doc, api, start, cutoff = recovery_inputs(source, matrix, checksum, started, cutoff)
    end = cutoff + RESERVE_SECONDS
    name = "bfme-recovery-lease-" + api.run["attempt"]
    while time() < end:
        if api.reserve_should_stop():
            print("The exact attempt's protected reporter or collector finished; reserve stopped.", flush=True)
            output({"reserve_needed": "false"})
            return
        records = api.inventory({name}, earliest=start, cutoff=end)
        if records:
            dest = Path(target)
            try:
                api.extract(records[name], {"lease.json"}, LEASE_LIMIT, dest)
            except ArtifactPending as exc:
                print(f"Expected the protected lease; found {exc} Retrying before reserve cutoff.", flush=True)
                sleep(max(0, min(POLL, end - time())))
                continue
            lease = read(dest / "lease.json", LEASE_LIMIT)
            digest = identity(lease)
            checked_lease(doc, checksum, lease, digest)
            if lease["primary_cutoff"] != cutoff:
                raise ValueError("The reserve lease changed the protected cutoff.")
            output({"lease_sha": digest, "reserve_needed": str(bool(lease["reserve"])).lower()})
            return
        # Poll infrequently before the recovery window. Four reserves can
        # wait concurrently without making one request per second each.
        sleep(max(0, min(POLL if time() >= cutoff else 20, end - time())))
    raise ValueError("The protected recovery lease missed its deadline.")


def gather_reserves(source, matrix, checksum, lease_path, lease_sha, directory, started, cutoff):
    doc, api, _start, cutoff = recovery_inputs(source, matrix, checksum, started, cutoff)
    lease = read(lease_path, LEASE_LIMIT)
    checked_lease(doc, checksum, lease, lease_sha)
    if lease["primary_cutoff"] != cutoff:
        raise ValueError("The recovery lease changed the protected cutoff.")
    allowed = {}
    for name, slot in lease["reserve"].items():
        artifact = f"bfme-recovery-reserve-{api.run['attempt']}-{slot}"
        allowed.setdefault(artifact, set()).add("reserve-" + name + "/group.json")
    if not allowed:
        return
    end = lease["reserve_cutoff"]
    seen = set()
    while True:
        current = api.inventory(allowed, earliest=cutoff, cutoff=end)
        for name, artifact in current.items():
            if name in seen:
                continue
            try:
                api.extract(artifact, allowed[name], GROUP_LIMIT, Path(directory))
            except ArtifactPending as exc:
                print(f"Expected leased output {name}; found {exc} Retrying before reserve cutoff.", flush=True)
                continue
            seen.add(name)
        if seen == allowed.keys():
            return
        if time() >= end:
            raise ValueError("A leased paired group missed the reserve deadline.")
        sleep(max(0, min(POLL, end - time())))


def stop_reserve(worker):
    # The process group belongs only to the child started by this invocation.
    if worker.poll() is not None:
        return
    try:
        os.killpg(worker.pid, signal.SIGTERM)
        worker.wait(timeout=2)
    except subprocess.TimeoutExpired:
        try:
            os.killpg(worker.pid, signal.SIGKILL)
        except ProcessLookupError:
            pass
    except ProcessLookupError:
        pass
    worker.wait()


def supervised_reserve(decision, source, matrix, checksum, lease_path, lease_sha, slot, directory, started, cutoff):
    # The supervisor receives only the decision. The scrubbed compiler
    # is the only process that opens the separate source plan.
    doc, api, _start, cutoff = recovery_inputs(decision, matrix, checksum, started, cutoff)
    lease = read(lease_path, LEASE_LIMIT)
    checked_lease(doc, checksum, lease, lease_sha)
    if lease["primary_cutoff"] != cutoff or slot not in ("0", "1", "2", "3"):
        raise ValueError("Foreign reserve cutoff or slot.")
    if int(slot) not in lease["reserve"].values() or api.reserve_should_stop():
        print("The protected collector needs no work from this reserve slot.", flush=True)
        return
    end = lease["reserve_cutoff"] - 5
    remaining = int(end - time())
    if remaining <= 0:
        raise ValueError("Expected time left in the lease; found the reserve cutoff.")
    env = {key: os.environ[key] for key in (
        "PATH", "HOME", "LANG", "LC_ALL", "TMPDIR", "GITHUB_SHA", "GITHUB_RUN_ID", "GITHUB_RUN_ATTEMPT",
    )}
    argv = ["timeout", "--kill-after=2", str(remaining), sys.executable, "-B",
            str(Path(__file__).with_name("wide_recovery.py")), "reserve", source, matrix, checksum,
            lease_path, lease_sha, slot, directory]
    with subprocess.Popen(argv, env=env, start_new_session=True) as worker:
        try:
            while True:
                try:
                    if worker.wait(timeout=POLL) != 0:
                        raise ValueError("The isolated reserve compile did not finish successfully.")
                    break
                except subprocess.TimeoutExpired:
                    if api.reserve_should_stop():
                        print("Stopped this exact attempt's isolated reserve compile after terminal report or collector.", flush=True)
                        return
        finally:
            stop_reserve(worker)
    if time() >= lease["reserve_cutoff"] or api.reserve_should_stop():
        print("Reserve output retired before upload.", flush=True)
        return
    if any(Path(directory).glob("reserve-*/group.json")):
        output({"complete": "true"})


def main(mode, *args):
    os.umask(0o077)
    if mode == "clock":
        anchor(*args)
    elif mode == "freeze":
        freeze_for_reserves(*args)
    elif mode == "fallback":
        fallback_primary(*args)
    elif mode == "lease":
        wait_for_lease(*args)
    elif mode == "gather":
        gather_reserves(*args)
    elif mode == "reserve":
        supervised_reserve(*args)
    else:
        raise ValueError("Unknown managed recovery step.")


if __name__ == "__main__":
    main(*sys.argv[1:])
