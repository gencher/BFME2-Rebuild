"""Keep normal state work on current main and authorize explicit protected seeds."""
import os
from pathlib import Path
import sys
from subprocess import CalledProcessError
import time
import urllib.error
import urllib.parse
import urllib.request

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent / "managed-check"))
from common import HEX40, REPO, REPO_ID, encoded, read
from github_status import GitHub
from state_delta import COMPACT_SCOPE, GRAPH_PATHS, REPORT_DIRS, graph_change, predecessors, source_changes
from state_snapshot import git
from state_fetch import WORKFLOW, select_artifact, select_run, verified_run

SEED_TITLE = "Seed protected main state "
MEASURING = {"requested", "queued", "pending", "waiting", "in_progress"}


class Held(ValueError):
    pass


def is_seed(run, revisions):
    sha = run.get("head_sha", "")
    return (isinstance(sha, str) and HEX40.fullmatch(sha)
            and sha in revisions and run.get("head_branch") == "main"
            and run.get("event") == "workflow_dispatch" and run.get("path") == WORKFLOW
            and (run.get("repository") or {}).get("id") == REPO_ID
            and run.get("display_title") == SEED_TITLE + sha)


def active_seeds(api, revisions):
    filters = urllib.parse.urlencode({"branch": "main", "event": "workflow_dispatch", "per_page": 20})
    runs = api.call("actions/workflows/state.yml/runs?" + filters)["workflow_runs"]
    return [run for run in runs if is_seed(run, revisions) and run.get("status") in MEASURING]


def running_seed(api, revisions):
    return bool(active_seeds(api, revisions))


def cancel_run(api, run_id, *, wait_seconds=4):
    endpoint = f"https://api.github.com/repos/{REPO}/actions/runs/{run_id}/cancel"
    request = urllib.request.Request(endpoint, data=b"", method="POST",
                                     headers={"Authorization": "Bearer " + api.token,
                                              "Accept": "application/vnd.github+json"})
    try:
        with api.api.open(request, timeout=25) as response:
            if response.status not in (202, 204):
                return False
    except urllib.error.HTTPError as err:
        if err.code in (404, 409):
            return False
        raise
    deadline = time.monotonic() + wait_seconds
    while time.monotonic() < deadline:
        try:
            if api.call(f"actions/runs/{run_id}").get("status") == "completed":
                return True
        except Exception:
            break
        time.sleep(0.5)
    return True


def main_ref(api):
    current = api.call("git/ref/heads/main")["object"]["sha"]
    if not isinstance(current, str) or not HEX40.fullmatch(current):
        raise Held("Expected the full current main object. Found invalid GitHub ref metadata.")
    return current


def report_only_push(root, sha, event):
    """Classify the complete push diff; missing or untrusted evidence never skips."""
    before = event.get("before")
    if (event.get("ref") != "refs/heads/main" or event.get("after") != sha
            or not isinstance(before, str) or not HEX40.fullmatch(before) or before == "0" * 40):
        return False
    try:
        diff = git(root, "diff", "--name-only", "--no-renames", "--no-ext-diff", "-z",
                   before, sha, "--")
    except (OSError, CalledProcessError):
        return False
    names = diff.split(b"\0")
    if names.pop() != b"":
        return False
    prefixes = tuple(prefix.encode("ascii") for prefix in REPORT_DIRS)
    scope_paths = {path.encode("ascii") for path in COMPACT_SCOPE}
    unclassified = {name for name in names if name != b"ci/README.md"
                    and not (b"/" not in name and name.endswith(b".md"))
                    and not name.startswith(prefixes)}
    if not unclassified:
        return bool(names)
    graph_paths = {path for path in GRAPH_PATHS if path.encode("ascii") in unclassified}
    return (unclassified <= {path.encode("ascii") for path in graph_paths} | scope_paths
            and graph_change(root, before, sha, graph_paths) is not None)


def eligible(api, sha, seed, phase, *, root=None):
    current = main_ref(api)
    if not seed and current != sha:
        detail = f"Expected current main {sha[:12]}. Found newer main {current[:12]}."
        if phase == "admit":
            print(detail)
            return False
        raise Held(detail)
    if phase == "admit" and not seed:
        run = select_run(api, sha)
        if run and select_artifact(api, *run):
            print(f"Main {sha[:12]} already has a successful exact state. No duplicate refresh.")
            return False
        ancestors = predecessors(root, sha)
        seeds = active_seeds(api, {sha, *ancestors})
        obsolete = []
        for srun in seeds:
            seed_sha = srun.get("head_sha", "")
            if seed_sha == sha or (root is not None and source_changes(root, seed_sha, sha) is not None):
                print("A protected seed is already measuring a compatible main-line revision. Current refresh deferred.")
                return False
            obsolete.append(srun)
        for srun in obsolete:
            rid = srun.get("id")
            seed_sha = srun.get("head_sha", "")
            if rid is not None:
                cancel_run(api, rid)
                print(f"Cancelled obsolete protected seed {rid} ({seed_sha[:12]}) that cannot be carried to {sha[:12]}.")
    return True


def _dispatch_refresh(api):
    endpoint = f"https://api.github.com/repos/{REPO}/actions/workflows/state.yml/dispatches"
    request = urllib.request.Request(endpoint, data=encoded({"ref": "main", "inputs": {"seed": "false"}}),
                                     headers={"Authorization": "Bearer " + api.token,
                                              "Accept": "application/vnd.github+json",
                                              "Content-Type": "application/json"})
    with api.api.open(request, timeout=25) as response:
        if response.status != 204:
            raise Held("Expected GitHub to accept one current-main refresh. Found a different response.")
    print("Requested one current-main refresh after the exact protected seed completed.")


def catch_up(api, event):
    run = event.get("workflow_run") or {}
    sha = run.get("head_sha", "")
    if (not is_seed(run, {sha}) or not verified_run(run, sha)
            or not select_artifact(api, run["id"], run["run_attempt"])):
        raise Held("Expected a completed protected seed with its exact evidence. Found incompatible run metadata.")
    if main_ref(api) == sha:
        print("The successful seed still describes current main.")
        return
    _dispatch_refresh(api)


def after_seed(api, sha):
    if main_ref(api) == sha:
        print("The successful seed still describes current main.")
        return
    _dispatch_refresh(api)


def run(phase):
    env = os.environ
    sha, event = env.get("GITHUB_SHA", ""), env.get("GITHUB_EVENT_NAME")
    if (env.get("GITHUB_REPOSITORY") != REPO or env.get("GITHUB_REPOSITORY_ID") != str(REPO_ID)
            or env.get("GITHUB_REF") != "refs/heads/main" or not HEX40.fullmatch(sha)):
        raise Held("Expected the exact protected repository and main ref. Found another execution origin.")
    if phase == "admit" and event == "push":
        payload = read(env["GITHUB_EVENT_PATH"], 32 * 1024**2)
        if report_only_push(Path.cwd(), sha, payload):
            print("Only workflow, reporting, or documentation paths changed. Skipping state measurement.")
            with Path(env["GITHUB_OUTPUT"]).open("a") as stream:
                stream.write("active=no\n")
            return
    api = GitHub()
    if phase == "catch-up":
        if event != "workflow_run":
            raise Held("Expected a completed protected seed event. Found a different trigger.")
        catch_up(api, read(env["GITHUB_EVENT_PATH"], 128 * 1024))
        return
    if phase == "after-seed":
        if event != "workflow_dispatch" or env.get("BFME_STATE_SEED") != "true":
            raise Held("Expected a completed protected seed dispatch. Found a different trigger.")
        after_seed(api, sha)
        return
    if phase not in {"admit", "check"} or event not in {"push", "repository_dispatch", "workflow_dispatch"}:
        raise Held("Expected a protected main state entry point. Found another phase or trigger.")
    if event == "repository_dispatch" and env.get("GITHUB_ACTOR_ID") != "41898282":
        raise Held("Expected an automatic landing dispatch. Found a different dispatch identity.")
    seed = event == "workflow_dispatch" and env.get("BFME_STATE_SEED") == "true"
    result = eligible(api, sha, seed, phase, root=Path.cwd())
    if phase == "admit":
        with Path(env["GITHUB_OUTPUT"]).open("a") as stream:
            stream.write(f"active={'yes' if result else 'no'}\n")


if __name__ == "__main__":
    try:
        run(*sys.argv[1:])
    except Held as error:
        print(str(error), file=sys.stderr)
        raise SystemExit(1) from None
    except (OSError, ValueError, TypeError, KeyError) as error:
        print(f"Expected fresh protected state metadata from GitHub. Found {type(error).__name__}.", file=sys.stderr)
        raise SystemExit(1) from None

