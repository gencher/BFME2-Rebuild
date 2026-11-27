"""Unit tests for the GitHub Native Merge Queue auto-enqueuer (ci/queue/enqueue.py)."""
import datetime
import io
from pathlib import Path
import sys
import tempfile
import unittest
from unittest import mock
import urllib.error

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "ci/queue"))

import enqueue  # noqa: E402

MAIN_SHA = "a" * 40
NOW = datetime.datetime(2026, 11, 23, 12, 0, tzinfo=datetime.timezone.utc)
OLD = "2026-11-23T10:00:00Z"      # two hours before NOW: outside the cooldown
RECENT = "2026-11-23T11:50:00Z"   # ten minutes before NOW: inside the cooldown
YESTERDAY = "2026-11-22T20:00:00Z"  # sixteen hours before NOW, on the previous UTC day


def make_pr(number, *, oid=None, draft=False, in_queue=False, mergeable="MERGEABLE",
            branch="feat/test", author_id=100, labels=(), files=("src/Foo.cpp",),
            total_files=None, statuses=None, reviews=(), removed=None):
    """statuses maps a context to a state, or to (state, description[, createdAt])."""
    oid = oid or f"{number:040x}"
    contexts = []
    for context, value in (statuses or {}).items():
        state, description, created = (value, "", OLD) if isinstance(value, str) or value is None else (
            tuple(value) + (OLD,))[:3]
        contexts.append({"context": context, "state": state, "description": description, "createdAt": created})
    return {
        "id": f"PR_node_{number}",
        "number": number,
        "isDraft": draft,
        "headRefOid": oid,
        "headRefName": branch,
        "mergeable": mergeable,
        "isInMergeQueue": in_queue,
        "author": {"login": f"user{author_id}", "databaseId": author_id},
        "labels": {"nodes": [{"name": lbl} for lbl in labels]},
        "files": {
            "totalCount": len(files) if total_files is None else total_files,
            "nodes": [{"path": p} for p in files],
        },
        "commits": {"nodes": [{"commit": {"status": {"contexts": contexts}}}]},
        "timelineItems": {"nodes": [{"createdAt": removed}] if removed else []},
        "latestOpinionatedReviews": {"nodes": list(reviews)},
    }


class FakeApi:
    def __init__(self, pages=(), *, active_in_progress=0, active_queued=0,
                 rules_in_progress=0, rules_queued=0,
                 admission_in_progress=0, admission_queued=0,
                 ratelimit=4000, fail_enqueue=(), fail_enqueue_transport=(),
                 fail_status=False, fail_quota=False,
                 merge_queue_entries=None, merge_queue_fail=False,
                 merge_queue_null_mq=False, merge_queue_total_override=None,
                 merge_queue_has_next_page=False,
                 merge_queue_entries_on_requery=None):
        self.pages = pages
        self.active_in_progress = active_in_progress
        self.active_queued = active_queued
        self.rules_in_progress = rules_in_progress
        self.rules_queued = rules_queued
        self.admission_in_progress = admission_in_progress
        self.admission_queued = admission_queued
        self.ratelimit_remaining = ratelimit
        self.graphql_ratelimit_remaining = ratelimit
        self.graphql_ratelimit_limit = None
        self.rest_ratelimit_remaining = None
        self.rest_ratelimit_limit = None
        self.fail_enqueue = set(fail_enqueue)
        self.fail_enqueue_transport = set(fail_enqueue_transport)
        self.fail_status = fail_status
        self.fail_quota = fail_quota
        self.merge_queue_entries = merge_queue_entries
        self.merge_queue_fail = merge_queue_fail
        self.merge_queue_null_mq = merge_queue_null_mq
        self.merge_queue_total_override = merge_queue_total_override
        self.merge_queue_has_next_page = merge_queue_has_next_page
        self.merge_queue_entries_on_requery = merge_queue_entries_on_requery
        self._mq_query_count = 0
        self.quota_reads = 0
        self.enqueued = []
        self.dispatches = []
        self.statuses = []
        self.gets = []

    def _auto_merge_queue_entries(self):
        """Build merge queue entries from isInMergeQueue PRs across all pages."""
        entries = []
        for page in self.pages:
            for pr in page:
                if pr.get("isInMergeQueue"):
                    entries.append({"number": pr["number"], "files": pr.get("files", {"totalCount": 0, "nodes": []})})
        return entries

    def graphql(self, query, variables):
        if "enqueuePullRequest" in query:
            if variables["id"] in self.fail_enqueue_transport:
                raise enqueue.ApiError("GraphQL HTTP 502")
            if variables["id"] in self.fail_enqueue:
                raise enqueue.ApiError("GraphQL error: Pull request is already in queue")
            self.enqueued.append((variables["id"], variables["oid"]))
            return {"enqueuePullRequest": {"mergeQueueEntry": {"id": "MQE_1", "position": 1}}}
        if "mergeQueue" in query and "pullRequests" not in query:
            self._mq_query_count += 1
            if self.merge_queue_fail:
                raise enqueue.ApiError("GraphQL HTTP 502")
            if self.merge_queue_null_mq:
                return {"repository": {"mergeQueue": None}}
            if self._mq_query_count > 1 and self.merge_queue_entries_on_requery is not None:
                entries = self.merge_queue_entries_on_requery
            elif self.merge_queue_entries is not None:
                entries = self.merge_queue_entries
            else:
                entries = self._auto_merge_queue_entries()
            total = self.merge_queue_total_override if self.merge_queue_total_override is not None else len(entries)
            return {"repository": {"mergeQueue": {"entries": {
                "totalCount": total,
                "pageInfo": {"hasNextPage": self.merge_queue_has_next_page},
                "nodes": [{"pullRequest": e} if e is not None else {"pullRequest": None} for e in entries],
            }}}}
        cursor = variables.get("cursor")
        idx = 0 if cursor is None else int(cursor)
        nodes = self.pages[idx]
        has_next = idx + 1 < len(self.pages)
        return {
            "repository": {
                "defaultBranchRef": {"target": {"oid": MAIN_SHA}},
                "pullRequests": {
                    "pageInfo": {"hasNextPage": has_next, "endCursor": str(idx + 1) if has_next else None},
                    "nodes": nodes,
                },
            }
        }

    def get(self, path):
        self.gets.append(path)
        if "workflows/managed-pr.yml/runs" in path:
            if "status=in_progress" in path:
                return {"total_count": self.active_in_progress}
            if "status=queued" in path:
                return {"total_count": self.active_queued}
        if "workflows/merge-queue-boat.yml/runs" in path:
            if "status=in_progress" in path:
                return {"total_count": getattr(self, "mq_in_progress", 0)}
            if "status=queued" in path:
                return {"total_count": getattr(self, "mq_queued", 0)}
        if "workflows/rules.yml/runs" in path:
            if "status=in_progress" in path:
                return {"total_count": self.rules_in_progress}
            if "status=queued" in path:
                return {"total_count": self.rules_queued}
        if "workflows/admission.yml/runs" in path:
            if "status=in_progress" in path:
                return {"total_count": self.admission_in_progress}
            if "status=queued" in path:
                return {"total_count": self.admission_queued}
        raise AssertionError(f"Unexpected GET {path}")

    def core_remaining(self):
        self.quota_reads += 1
        if self.fail_quota:
            raise enqueue.ApiError("GET rate_limit returned HTTP 500")
        return self.ratelimit_remaining

    def post(self, path, body):
        if path.startswith("statuses/"):
            if self.fail_status:
                raise enqueue.ApiError(f"POST {path} returned HTTP 403")
            self.statuses.append((path, body))
            return {}
        self.dispatches.append((path, body))
        return {}


def run(api, **kwargs):
    kwargs.setdefault("env", {})
    return enqueue.run(api, now=NOW, **kwargs)


class EnqueueTests(unittest.TestCase):
    def test_enqueues_ready_source_and_header_prs_and_skips_ineligible(self):
        oid_hdr_ok = "1" * 40
        oid_hdr_rev = "2" * 40
        oid_hdr_self = "3" * 40
        oid_hdr_stale = "4" * 40
        ok_src = {"BFME II / source rules": "SUCCESS", "BFME II / Boat": "SUCCESS", "BFME II / Inline virtual": "PENDING"}
        prs = [
            # Eligible narrow source PR
            make_pr(101, statuses=ok_src),
            # Eligible header PR via Header rules SUCCESS
            make_pr(102, oid=oid_hdr_ok, files=("include/A.h", "src/A.cpp"),
                    statuses=ok_src | {"BFME II / Header rules": "SUCCESS"}),
            # Eligible header PR via listed approver on exact headRefOid
            make_pr(103, oid=oid_hdr_rev, files=("include/B.h",), statuses=ok_src, reviews=[
                {"state": "APPROVED", "commit": {"oid": oid_hdr_rev}, "author": {"databaseId": 1760627}},
            ]),
            # Rejected: header PR self-approved by author
            make_pr(104, oid=oid_hdr_self, author_id=1760627, files=("include/C.h",), statuses=ok_src, reviews=[
                {"state": "APPROVED", "commit": {"oid": oid_hdr_self}, "author": {"databaseId": 1760627}},
            ]),
            # Rejected: header PR approved on stale commit
            make_pr(105, oid=oid_hdr_stale, files=("include/D.h",), statuses=ok_src, reviews=[
                {"state": "APPROVED", "commit": {"oid": "f" * 40}, "author": {"databaseId": 1760627}},
            ]),
            # Rejected: Inline virtual failure
            make_pr(106, statuses=ok_src | {"BFME II / Inline virtual": "FAILURE"}),
            # Rejected: draft, already queued, conflicting, held, canary, refused branch, bad paths, >60 files
            make_pr(107, draft=True, statuses=ok_src),
            make_pr(108, in_queue=True, statuses=ok_src),
            make_pr(109, mergeable="CONFLICTING", statuses=ok_src),
            make_pr(110, labels=("landing-hold",), statuses=ok_src),
            make_pr(21, statuses=ok_src),
            make_pr(111, branch="canary/test", statuses=ok_src),
            make_pr(112, files=("src/Code/Tools/exp.cpp",), statuses=ok_src),
            make_pr(113, files=("tools/gates/gate.py",), statuses=ok_src),
            make_pr(114, files=("src/A.cpp",), total_files=61, statuses=ok_src),
        ]
        api = FakeApi([prs[:8], prs[8:]])
        with tempfile.TemporaryDirectory() as td:
            summary = Path(td) / "summary.md"
            result = run(api, env={"GITHUB_STEP_SUMMARY": str(summary)})
            # 102 and 103 wait: a header change only enters an empty queue, after every ready narrow change.
            self.assertEqual(result["enqueued"], [101])
            # 104 and 105 passed Boat but have no Header rules yet: a wide check on the current main is requested.
            self.assertEqual(result["backfilled"], [104, 105])
            self.assertIn("**Enqueued:** 1 (#101)", summary.read_text(encoding="utf-8"))

    def test_caps_enqueue_at_16_and_survives_individual_rejection(self):
        ok_src = {"BFME II / source rules": "SUCCESS", "BFME II / Boat": "SUCCESS"}
        prs = [make_pr(200 + i, statuses=ok_src) for i in range(20)]
        api = FakeApi([prs], fail_enqueue={"PR_node_201"})
        result = run(api)
        # A rejected entry does not use one of the 16 places.
        self.assertEqual(len(result["enqueued"]), 16)
        self.assertNotIn(201, result["enqueued"])
        self.assertEqual(len(result["errors"]), 1)
        self.assertEqual(result["errors"][0][0], 201)

    def test_backfills_unmeasured_pending_and_error_prs_up_to_available_slots(self):
        boat_states = (None, "PENDING", "ERROR")
        prs = [
            make_pr(300 + i, statuses={"BFME II / Boat": boat_states[i % 3]} if i < 20 else {})
            for i in range(22)
        ]
        # PR with failed Boat status should NOT be backfilled.
        prs.insert(0, make_pr(297, statuses={"BFME II / Boat": "FAILURE"}))
        # PR with failed source rules should NOT be backfilled (even if Boat is ERROR).
        prs.insert(0, make_pr(299, statuses={"BFME II / source rules": "FAILURE", "BFME II / Boat": "ERROR"}))
        # PR touching disallowed path should NOT be backfilled.
        prs.insert(0, make_pr(298, files=("docs/x.md",), statuses={}))

        # With 0 active runs, fills up to MAX_BACKFILL (18), including ERROR Boat PRs; never-touched heads first.
        idle = FakeApi([prs], active_in_progress=0, active_queued=0, ratelimit=2500)
        idle_result = run(idle)
        self.assertEqual(idle_result["enqueued"], [])
        self.assertEqual(len(idle_result["backfilled"]), 18)
        self.assertEqual(idle_result["backfilled"], [320, 321] + [300 + i for i in range(16)])
        self.assertIn(302, idle_result["backfilled"])  # ERROR Boat status
        self.assertEqual(idle.dispatches[0][0], "dispatches")
        self.assertEqual(idle.dispatches[0][1]["event_type"], "bfme-source-refresh")
        self.assertEqual(idle.dispatches[0][1]["client_payload"]["base"], MAIN_SHA)

        # With 12 active runs (6 in_progress + 6 queued), only fills remaining 6 slots (18 - 12).
        partial = FakeApi([prs], active_in_progress=6, active_queued=6, ratelimit=2500)
        partial_result = run(partial)
        self.assertEqual(partial_result["enqueued"], [])
        self.assertEqual(len(partial_result["backfilled"]), 6)
        self.assertEqual(partial_result["backfilled"], [320, 321, 300, 301, 302, 303])

    def test_skips_backfill_when_ratelimit_low_or_boat_busy(self):
        prs = [make_pr(400, statuses={}), make_pr(401, statuses={"BFME II / Boat": "ERROR"})]
        low_rl = FakeApi([prs], active_in_progress=0, active_queued=0, ratelimit=500)
        self.assertEqual(run(low_rl)["backfilled"], [])
        self.assertEqual(low_rl.dispatches, [])

        at_cap = FakeApi([prs], active_in_progress=10, active_queued=8, ratelimit=3000)
        self.assertEqual(run(at_cap)["backfilled"], [])
        self.assertEqual(at_cap.dispatches, [])

        over_cap = FakeApi([prs], active_in_progress=12, active_queued=8, ratelimit=3000)
        self.assertEqual(run(over_cap)["backfilled"], [])
        self.assertEqual(over_cap.dispatches, [])

    def test_dispatch_api_receives_backfill_while_api_receives_enqueue(self):
        ok_src = {"BFME II / source rules": "SUCCESS", "BFME II / Boat": "SUCCESS"}
        prs = [
            make_pr(501, statuses=ok_src),
            make_pr(502, statuses={}),
            make_pr(503, statuses={"BFME II / Boat": "PENDING"}),
        ]
        queue_api = FakeApi([prs], ratelimit=3000)
        dispatch_api = FakeApi(ratelimit=800)
        result = run(queue_api, dispatch_api=dispatch_api)
        self.assertEqual(result["enqueued"], [501])
        self.assertEqual(result["backfilled"], [502, 503])
        self.assertEqual([node for node, _ in queue_api.enqueued], ["PR_node_501"])
        self.assertEqual(queue_api.dispatches, [])
        self.assertEqual(dispatch_api.enqueued, [])
        self.assertEqual(dispatch_api.gets, [])
        self.assertEqual(len(dispatch_api.dispatches), 2)
        self.assertEqual(dispatch_api.dispatches[0][0], "dispatches")
        self.assertEqual(dispatch_api.dispatches[0][1]["event_type"], "bfme-source-refresh")
        self.assertEqual(
            [d[1]["client_payload"]["number"] for d in dispatch_api.dispatches],
            [502, 503],
        )

    def test_env_dispatch_token_builds_separate_dispatch_api(self):
        prs = [make_pr(510, statuses={})]
        queue_api = FakeApi([prs], ratelimit=3000)
        dispatch_api = FakeApi(ratelimit=800)
        tokens = []

        def fake_github_api(token, token_name="BFME_QUEUE_TOKEN"):
            tokens.append((token, token_name))
            return queue_api if token_name == "BFME_QUEUE_TOKEN" else dispatch_api

        with mock.patch.object(enqueue, "GitHubApi", side_effect=fake_github_api):
            result = enqueue.run(env={
                "BFME_QUEUE_TOKEN": "pat_123456789012345",
                "BFME_DISPATCH_TOKEN": "ghs_123456789012345",
            })
        self.assertEqual(tokens, [
            ("pat_123456789012345", "BFME_QUEUE_TOKEN"),
            ("ghs_123456789012345", "BFME_DISPATCH_TOKEN"),
        ])
        self.assertEqual(result["backfilled"], [510])
        self.assertEqual(queue_api.dispatches, [])
        self.assertEqual(len(dispatch_api.dispatches), 1)

    def test_in_flight_rules_runs_count_toward_occupied_backfill_slots(self):
        prs = [make_pr(600 + i, statuses={}) for i in range(20)]
        # 4 boat in_progress + 2 boat queued + 5 rules in_progress + 3 rules queued = 14 occupied -> 4 slots left.
        partial = FakeApi(
            [prs],
            active_in_progress=4,
            active_queued=2,
            rules_in_progress=5,
            rules_queued=3,
            ratelimit=3000,
        )
        result = run(partial)
        self.assertEqual(result["backfilled"], [600, 601, 602, 603])
        self.assertEqual(len(partial.dispatches), 4)
        self.assertTrue(any("workflows/managed-pr.yml/runs" in p for p in partial.gets))
        self.assertTrue(any("workflows/rules.yml/runs" in p for p in partial.gets))

        # 0 boat runs, but 18 in-flight rules runs -> 0 slots left (prevents double-dispatch).
        rules_full = FakeApi(
            [prs],
            active_in_progress=0,
            active_queued=0,
            rules_in_progress=12,
            rules_queued=6,
            ratelimit=3000,
        )
        self.assertEqual(run(rules_full)["backfilled"], [])
        self.assertEqual(rules_full.dispatches, [])

    def test_backfill_writes_a_marker_first_and_skips_heads_touched_within_the_cooldown(self):
        prs = [
            make_pr(700, statuses={"BFME II / Boat": ("PENDING", "", RECENT)}),                # in flight
            make_pr(701, statuses={"BFME II / Queue backfill": ("SUCCESS", "", RECENT)}),      # just requested
            make_pr(702, statuses={"BFME II / Admission": ("SUCCESS", "", RECENT)}),           # just admitted
            make_pr(703, statuses={"BFME II / Boat": ("ERROR", "", OLD)}),
            make_pr(704, statuses={}),
        ]
        api = FakeApi([prs])
        result = run(api)
        self.assertEqual(result["backfilled"], [704, 703])
        self.assertEqual([path for path, _ in api.statuses], [f"statuses/{704:040x}", f"statuses/{703:040x}"])
        marker = api.statuses[0][1]
        self.assertEqual((marker["context"], marker["state"]), ("BFME II / Queue backfill", "success"))
        self.assertEqual(marker["description"], f"Requested bfme-source-refresh on main {MAIN_SHA[:12]}")

        # The next run sees the markers and requests nothing again.
        touched = [make_pr(704, statuses={"BFME II / Queue backfill": ("SUCCESS", "", RECENT)}),
                   make_pr(703, statuses={"BFME II / Boat": ("ERROR", "", OLD),
                                          "BFME II / Queue backfill": ("SUCCESS", "", RECENT)})]
        again = FakeApi([prs[:3] + touched])
        self.assertEqual(run(again)["backfilled"], [])
        self.assertEqual(again.dispatches, [])

    def test_no_dispatch_without_a_marker(self):
        api = FakeApi([[make_pr(710, statuses={})]], fail_status=True)
        self.assertEqual(run(api)["backfilled"], [])
        self.assertEqual(api.dispatches, [])

    def test_retries_rotate_least_recently_touched_first(self):
        prs = [
            make_pr(720, statuses={"BFME II / Boat": ("ERROR", "", "2026-11-23T11:00:00Z")}),
            make_pr(721, statuses={"BFME II / Boat": ("ERROR", "", "2026-11-23T09:00:00Z")}),
            make_pr(722, statuses={"BFME II / Boat": ("ERROR", "", "2026-11-23T10:00:00Z")}),
        ]
        api = FakeApi([prs], active_in_progress=16)
        self.assertEqual(run(api)["backfilled"], [721, 722])

    def test_passed_boat_with_unfinished_source_rules_gets_a_source_only_recheck(self):
        boat = {"BFME II / Boat": "SUCCESS"}
        conflict = "Conflicts with main: merge or rebase main into your branch and push (main x)"
        prs = [
            make_pr(730, statuses=boat | {"BFME II / source rules": ("ERROR", "Source rules could not finish (main x)")}),
            make_pr(731, statuses=boat),
            make_pr(732, statuses=boat | {"BFME II / source rules": "FAILURE"}),
            make_pr(733, statuses=boat | {"BFME II / source rules": ("ERROR", conflict)}),
            make_pr(734, statuses=boat | {"BFME II / Inline virtual": "FAILURE"}),
        ]
        api = FakeApi([prs])
        result = run(api)
        self.assertEqual((result["enqueued"], result["backfilled"]), ([], [730, 731]))
        self.assertEqual({body["event_type"] for _, body in api.dispatches}, {"bfme-source-recheck"})
        self.assertEqual(api.dispatches[0][1]["client_payload"],
                         {"number": 730, "head": f"{730:040x}", "base": MAIN_SHA})

    def test_passed_header_change_needs_a_wide_check_on_the_current_main_only_for_h1_or_h10(self):
        ok = {"BFME II / Boat": "SUCCESS", "BFME II / source rules": "SUCCESS"}
        # Last touched on the previous UTC day: the wide holds' daily budget has reset since.
        yesterday = {"BFME II / Boat": ("SUCCESS", "", YESTERDAY), "BFME II / source rules": ("SUCCESS", "", YESTERDAY)}
        hdr = ("include/A.h",)
        prs = [
            make_pr(740, files=hdr, statuses=yesterday | {"BFME II / Header rules": (
                "PENDING", "Held: H10 no wide check admitted for this commit yet (main x)")}),
            make_pr(741, files=hdr, statuses=yesterday | {"BFME II / Header rules": (
                "PENDING", "Held: H1 The wide comparison has not reported on this main. (main x)")}),
            make_pr(742, files=hdr, statuses=ok),
            make_pr(743, files=hdr, statuses=ok | {"BFME II / Header rules": (
                "PENDING", "Held: ledger request Foo::Bar (main x)")}),
            make_pr(744, files=hdr, statuses=ok | {"BFME II / Header rules": ("FAILURE", "H4: removes X (main x)")}),
        ]
        api = FakeApi([prs])
        result = run(api)
        # MAX_WIDE_BACKFILL=2: at most 2 wide refreshes per run; PR 742 waits for the next cycle.
        self.assertEqual((result["enqueued"], result["backfilled"]), ([], [740, 741]))
        self.assertEqual({body["event_type"] for _, body in api.dispatches}, {"bfme-source-refresh"})

    def test_merge_queue_removal_needs_a_newer_boat_verdict(self):
        ok = {"BFME II / Boat": ("SUCCESS", "", "2026-11-23T09:00:00Z"), "BFME II / source rules": "SUCCESS"}
        prs = [
            make_pr(750, statuses=ok, removed="2026-11-23T10:30:00Z"),   # removed after its Boat: measure again
            make_pr(751, statuses=ok, removed="2026-11-23T08:30:00Z"),   # measured again since: enqueue
        ]
        result = run(FakeApi([prs]))
        self.assertEqual((result["enqueued"], result["backfilled"]), ([751], [750]))

    def test_final_holds_are_left_alone(self):
        prs = [
            make_pr(760, statuses={"BFME II / Admission": (
                "PENDING", "Held: the compiler check failed to run five times; a maintainer will look")}),
            make_pr(761, statuses={"BFME II / Admission": (
                "PENDING", "Held: source-only admission covers at most eight paths")}),
            make_pr(762, statuses={"BFME II / Boat": "FAILURE"}),
            make_pr(763, statuses={"BFME II / Admission": (
                "PENDING", "Held: checking the source rules on main 0123456789ab, retried automatically")}),
        ]
        self.assertEqual(run(FakeApi([prs]))["backfilled"], [763])

    def test_enqueue_keeps_the_merge_queue_inside_the_group_planner_limits(self):
        ok = {"BFME II / Boat": "SUCCESS", "BFME II / source rules": "SUCCESS"}
        prs = [
            make_pr(800, in_queue=True, files=tuple(f"src/Q{i}.cpp" for i in range(50))),
            make_pr(801, files=tuple(f"src/A{i}.cpp" for i in range(8)), statuses=ok),   # 58: fits
            make_pr(802, files=tuple(f"src/B{i}.cpp" for i in range(3)), statuses=ok),   # 61: over the limit
            make_pr(803, files=("src/C.cpp", "src/D.cpp"), statuses=ok),                  # 60: fits
        ]
        self.assertEqual(run(FakeApi([prs]))["enqueued"], [801, 803])

        headers = [
            make_pr(810, in_queue=True, files=("include/Q.h",)),
            make_pr(811, files=("include/A.h",), statuses=ok | {"BFME II / Header rules": "SUCCESS"}),
            make_pr(812, statuses=ok),
        ]
        # Nothing is stacked behind a header change: each group behind it would recompile its wide shards.
        self.assertEqual(run(FakeApi([headers]))["enqueued"], [])

    def test_queue_wakeups_cover_every_completion_and_skip_needless_runs(self):
        managed = (ROOT / ".github/workflows/managed-pr.yml").read_text(encoding="utf-8")
        notify = managed.split("\n  notify-landing:\n")[1].split("\n  notify-unmeasured:\n")[0]
        self.assertIn("(needs.report.result != 'skipped' || needs.fallback-report.result != 'skipped')", notify)
        self.assertNotIn("outputs.passed", notify)
        rules = (ROOT / ".github/workflows/rules.yml").read_text(encoding="utf-8")
        self.assertIn("types: [bfme-source-refresh, bfme-source-recheck]\n", rules)
        admission = rules.split("\n  notify-admission:\n")[1].split("\n  notify-landing:\n")[0]
        self.assertIn('"bfme-admit-request"', admission)
        self.assertIn('"bfme-land-next"', admission)
        push = (ROOT / ".github/workflows/admission-push.yml").read_text(encoding="utf-8")
        self.assertIn("&& vars.BFME_MERGE_QUEUE_ENABLED != 'yes'", push)
        enq_wf = (ROOT / ".github/workflows/enqueue.yml").read_text(encoding="utf-8")
        self.assertIn("      statuses: write\n", enq_wf)

    def test_backfill_is_paced_by_the_actions_token_quota_not_the_queue_token(self):
        boat = {"BFME II / Boat": "SUCCESS"}
        refreshes = [make_pr(900 + i, statuses={}) for i in range(5)]
        rechecks = [make_pr(910 + i, statuses=boat | {"BFME II / source rules": ("ERROR", "", OLD)}) for i in range(3)]
        prs = refreshes + rechecks
        queue_api = FakeApi([prs], ratelimit=4900)

        # Rechecks sort first (15 each), then narrow refreshes (75 each). 445 spare = 445 - 400 reserve = 45.
        # 3 rechecks at 15 = 45 spare gone, no room for a refresh.
        actions = FakeApi(ratelimit=445)
        self.assertEqual(run(queue_api, dispatch_api=actions)["backfilled"], [910, 911, 912])
        self.assertEqual(actions.quota_reads, 1)

        # 475 spare fits 3 rechecks (45) + 0 refreshes (remaining 0 < 75).
        queue_api = FakeApi([prs], ratelimit=4900)
        self.assertEqual(run(queue_api, dispatch_api=FakeApi(ratelimit=445))["backfilled"], [910, 911, 912])

        # 490 spare = 90: 3 rechecks (45) + 1 refresh (75) = 120 > 90, so just 3 rechecks.
        queue_api = FakeApi([prs], ratelimit=4900)
        self.assertEqual(run(queue_api, dispatch_api=FakeApi(ratelimit=490))["backfilled"], [910, 911, 912])

        # 520 spare = 120: 3 rechecks (45) + 1 refresh (75) = 120, exactly fits.
        queue_api = FakeApi([prs], ratelimit=4900)
        self.assertEqual(run(queue_api, dispatch_api=FakeApi(ratelimit=520))["backfilled"], [910, 911, 912, 900])

        # Rechecks only: 415 spare = 15, fits exactly 1 recheck.
        rechecks_only = FakeApi([rechecks], ratelimit=4900)
        self.assertEqual(run(rechecks_only, dispatch_api=FakeApi(ratelimit=415))["backfilled"], [910])

        # At or under the reserve, or when the quota cannot be read, nothing is dispatched and no runs are listed.
        for actions in (FakeApi(ratelimit=400), FakeApi(ratelimit=0), FakeApi(ratelimit=4000, fail_quota=True)):
            with self.subTest(remaining=actions.ratelimit_remaining, fail=actions.fail_quota):
                queue_api = FakeApi([prs], ratelimit=4900)
                self.assertEqual(run(queue_api, dispatch_api=actions)["backfilled"], [])
                self.assertEqual((actions.dispatches, actions.statuses), ([], []))
                self.assertEqual(queue_api.gets, [])

    def test_in_flight_pipeline_runs_deduct_from_actions_token_budget(self):
        boat = {"BFME II / Boat": "SUCCESS"}
        refreshes = [make_pr(950 + i, statuses={}) for i in range(6)]
        rechecks = [make_pr(960 + i, statuses=boat | {"BFME II / source rules": ("ERROR", "", OLD)}) for i in range(4)]

        # 5 active runs: managed=4×65=260, rules=1×15=15 → inflight=275. 675 raw → 675-275=400 effective == reserve.
        queue_api = FakeApi([refreshes], active_in_progress=3, active_queued=1, rules_in_progress=1, ratelimit=4900)
        actions = FakeApi(ratelimit=675)
        with tempfile.TemporaryDirectory() as td:
            summary = Path(td) / "summary.md"
            result = run(queue_api, dispatch_api=actions, env={"GITHUB_STEP_SUMMARY": str(summary)})
            self.assertEqual(result["backfilled"], [])
            self.assertEqual((actions.dispatches, actions.statuses), ([], []))
            text = summary.read_text(encoding="utf-8")
            self.assertIn("-275 in-flight for 5 active runs", text)
            self.assertIn("= 400 effective; reserve 400)", text)

        # 2 active runs: managed=1×65=65, rules=1×15=15 → inflight=80. 900 raw → 820 effective → 420 spare → 5 refreshes.
        queue_api = FakeApi([refreshes], active_in_progress=1, rules_queued=1, ratelimit=4900)
        actions = FakeApi(ratelimit=900)
        with tempfile.TemporaryDirectory() as td:
            summary = Path(td) / "summary.md"
            result = run(queue_api, dispatch_api=actions, env={"GITHUB_STEP_SUMMARY": str(summary)})
            self.assertEqual(result["backfilled"], [950, 951, 952, 953, 954])
            self.assertEqual(len(actions.dispatches), 5)
            text = summary.read_text(encoding="utf-8")
            self.assertIn("-80 in-flight for 2 active runs", text)
            self.assertIn("= 820 effective; reserve 400)", text)

        # In-flight spend exceeding raw remaining clamps effective budget at 0 and dispatches nothing.
        queue_api = FakeApi([refreshes], active_in_progress=6, active_queued=6, ratelimit=4900)
        actions = FakeApi(ratelimit=900)
        with tempfile.TemporaryDirectory() as td:
            summary = Path(td) / "summary.md"
            result = run(queue_api, dispatch_api=actions, env={"GITHUB_STEP_SUMMARY": str(summary)})
            self.assertEqual(result["backfilled"], [])
            text = summary.read_text(encoding="utf-8")
            self.assertIn("-780 in-flight for 12 active runs", text)
            self.assertIn("= 120 effective; reserve 400)", text)

        # 4 active runs: managed=2×65=130, rules=2×15=30 → inflight=160. 575 raw → 415 effective → 15 spare.
        # 15 spare fits 1 recheck (15 each), not a refresh (75).
        queue_api = FakeApi([rechecks], active_in_progress=2, rules_in_progress=2, ratelimit=4900)
        actions = FakeApi(ratelimit=575)
        self.assertEqual(run(queue_api, dispatch_api=actions)["backfilled"], [960])
        queue_api = FakeApi([refreshes], active_in_progress=2, rules_in_progress=2, ratelimit=4900)
        actions = FakeApi(ratelimit=575)
        self.assertEqual(run(queue_api, dispatch_api=actions)["backfilled"], [])

    def test_marker_and_dispatch_use_the_actions_token(self):
        queue_api, actions = FakeApi([[make_pr(920, statuses={})]], ratelimit=4900), FakeApi(ratelimit=900)
        self.assertEqual(run(queue_api, dispatch_api=actions)["backfilled"], [920])
        self.assertEqual(([p for p, _ in actions.statuses], len(actions.dispatches)), ([f"statuses/{920:040x}"], 1))
        self.assertEqual((queue_api.statuses, queue_api.dispatches), ([], []))

    def test_workflows_have_atomic_cutover_guards(self):
        enq_wf = (ROOT / ".github/workflows/enqueue.yml").read_text(encoding="utf-8")
        land_wf = (ROOT / ".github/workflows/landing.yml").read_text(encoding="utf-8")
        self.assertIn("vars.BFME_MERGE_QUEUE_ENABLED == 'yes'", enq_wf)
        self.assertIn("BFME II Boat comparison", enq_wf)
        self.assertIn("BFME II header rules", enq_wf)
        self.assertNotIn("BFME II source rules", enq_wf)
        self.assertIn("bfme-land-next", enq_wf)
        self.assertIn("github.event.workflow_run.conclusion == 'failure'", enq_wf)
        self.assertIn("BFME_DISPATCH_TOKEN: '${{ github.token }}'", enq_wf)
        self.assertNotIn("\nconcurrency:\n", enq_wf)
        self.assertIn("\n    concurrency:\n", enq_wf)
        self.assertEqual(land_wf.count("vars.BFME_MERGE_QUEUE_ENABLED != 'yes'"), 2)

    def test_quota_holds_wait_for_a_new_utc_day_or_six_hours(self):
        hdr = ("include/A.h",)
        h10 = ("PENDING", "Held: H10 daily limit for wide checks reached; retried automatically (main x)")
        daily = ("PENDING", "Held: daily limit, retried automatically")

        def wide(number, touched):
            return make_pr(number, files=hdr, statuses={
                "BFME II / source rules": ("SUCCESS", "", touched), "BFME II / Boat": ("SUCCESS", "", touched),
                "BFME II / Header rules": h10})

        def narrow(number, touched):
            return make_pr(number, statuses={"BFME II / Admission": daily + (touched,)})

        prs = [
            wide(770, "2026-11-23T10:00:00Z"),     # 2h ago, same UTC day: waits
            wide(771, "2026-11-23T05:30:00Z"),     # 6.5h ago: retried
            narrow(772, "2026-11-23T09:00:00Z"),   # 3h ago, same UTC day: waits
            narrow(773, "2026-11-23T05:00:00Z"),   # 7h ago: retried
        ]
        with tempfile.TemporaryDirectory() as td:
            summary = Path(td) / "summary.md"
            result = run(FakeApi([prs]), env={"GITHUB_STEP_SUMMARY": str(summary)})
            self.assertEqual(result["backfilled"], [773, 771])
            self.assertIn("- **Backfill cooling down:** 2", summary.read_text(encoding="utf-8"))

        # A new UTC day resets the daily budgets, so the wait ends early; the 30-minute floor still applies.
        after_midnight = datetime.datetime(2026, 11, 24, 1, 0, tzinfo=datetime.timezone.utc)
        prs = [
            wide(774, "2026-11-23T23:00:00Z"),     # 2h ago, previous UTC day: retried
            narrow(775, "2026-11-23T22:00:00Z"),   # 3h ago, previous UTC day: retried
            wide(776, "2026-11-24T00:15:00Z"),     # 45 min ago, today: waits
            narrow(777, "2026-11-24T00:50:00Z"),   # 10 min ago: inside the floor
        ]
        result = enqueue.run(FakeApi([prs]), env={}, now=after_midnight)
        self.assertEqual(result["backfilled"], [775, 774])

    def test_runner_outage_holds_back_off_two_hours(self):
        outage = ("PENDING", "Held: compiler runners unavailable; retried automatically")
        prs = [
            make_pr(780, statuses={"BFME II / Admission": outage + ("2026-11-23T11:00:00Z",)}),  # 1h ago: waits
            make_pr(781, statuses={"BFME II / Admission": outage + ("2026-11-23T09:30:00Z",)}),  # 2.5h ago: retried
        ]
        self.assertEqual(run(FakeApi([prs]))["backfilled"], [781])

    def test_self_healing_holds_keep_the_30_minute_cooldown(self):
        boat = {"BFME II / Boat": "SUCCESS"}
        prs = [
            # A source rules ERROR on a passed Boat is rechecked as before.
            make_pr(790, statuses=boat | {"BFME II / source rules": ("ERROR", "Source rules could not finish (main x)",
                                                                       "2026-11-23T11:20:00Z")}),
            make_pr(791, statuses={"BFME II / Admission": (
                "PENDING", "Held: current source rules have not passed, retried automatically",
                "2026-11-23T11:20:00Z")}),
            # A header change without a Header rules verdict yet.
            make_pr(792, files=("include/A.h",), statuses=boat | {
                "BFME II / source rules": ("SUCCESS", "", "2026-11-23T11:20:00Z")}),
            # A settled admission status that is not pending is not a quota hold.
            make_pr(793, statuses={"BFME II / Admission": ("SUCCESS", "Held: daily limit, retried automatically",
                                                           "2026-11-23T11:20:00Z")}),
        ]
        # New backfill order: recheck (790), narrow refresh (791, 793), wide refresh (792).
        self.assertEqual(run(FakeApi([prs]))["backfilled"], [790, 791, 793, 792])
        self.assertEqual(enqueue.backoff({}), (enqueue.COOLDOWN_SECONDS, False))

    def test_a_header_change_enters_only_an_empty_queue_alone_after_narrow_changes(self):
        ok = {"BFME II / Boat": "SUCCESS", "BFME II / source rules": "SUCCESS"}
        wide_ok = ok | {"BFME II / Header rules": "SUCCESS"}

        # Narrow changes go first even when the header change was updated earlier; the header change waits.
        prs = [make_pr(820, files=("include/A.h",), statuses=wide_ok),
               make_pr(821, statuses=ok), make_pr(822, statuses=ok)]
        api = FakeApi([prs])
        self.assertEqual(run(api)["enqueued"], [821, 822])
        self.assertEqual([node for node, _ in api.enqueued], ["PR_node_821", "PR_node_822"])

        # Empty queue, no narrow change ready: exactly one header change, alone.
        prs = [make_pr(823, files=("include/A.h",), statuses=wide_ok),
               make_pr(824, files=("include/B.h", "src/B.cpp"), statuses=wide_ok)]
        self.assertEqual(run(FakeApi([prs]))["enqueued"], [823])

        # A rejected header change does not use the slot: the next one goes alone.
        self.assertEqual(run(FakeApi([prs], fail_enqueue={"PR_node_823"}))["enqueued"], [824])

        # A narrow change in the queue: more narrow changes join it, the header change waits for an empty queue.
        prs = [make_pr(825, in_queue=True), make_pr(826, files=("include/A.h",), statuses=wide_ok),
               make_pr(827, statuses=ok)]
        self.assertEqual(run(FakeApi([prs]))["enqueued"], [827])
        prs = [make_pr(825, in_queue=True), make_pr(826, files=("include/A.h",), statuses=wide_ok)]
        self.assertEqual(run(FakeApi([prs]))["enqueued"], [])

        # A header change in the queue: nothing narrow or wide is stacked behind it until it lands.
        prs = [make_pr(828, in_queue=True, files=("include/Q.h", "src/Q.cpp")),
               make_pr(829, statuses=ok), make_pr(830, files=("include/A.h",), statuses=wide_ok)]
        self.assertEqual(run(FakeApi([prs]))["enqueued"], [])

        # A header change approved by a listed approver is still a header change.
        oid = "9" * 40
        approved = make_pr(831, oid=oid, files=("include/C.h",), statuses=ok, reviews=[
            {"state": "APPROVED", "commit": {"oid": oid}, "author": {"databaseId": 1760627}}])
        self.assertEqual(run(FakeApi([[approved, make_pr(832, statuses=ok)]]))["enqueued"], [832])

    def test_summary_reports_both_tokens_remaining_and_limit(self):
        queue_api = FakeApi([[make_pr(840, statuses={})]], ratelimit=1918)
        queue_api.graphql_ratelimit_remaining = 1918
        queue_api.graphql_ratelimit_limit = 5000
        queue_api.rest_ratelimit_remaining = 1800
        queue_api.rest_ratelimit_limit = 5000
        actions = FakeApi(ratelimit=900)
        actions.rest_ratelimit_remaining = 900
        actions.rest_ratelimit_limit = 1000
        with tempfile.TemporaryDirectory() as td:
            summary = Path(td) / "summary.md"
            run(queue_api, dispatch_api=actions, env={"GITHUB_STEP_SUMMARY": str(summary)})
            text = summary.read_text(encoding="utf-8")
        self.assertIn("- **Queue token rate limit (GraphQL):** 1918/5000", text)
        self.assertIn("- **Queue token rate limit (REST):** 1800/5000", text)
        self.assertIn("- **Actions token rate limit:** 900/1000", text)
        self.assertEqual(enqueue.ratelimit_text(FakeApi(ratelimit=None)), "not read")
        self.assertEqual(enqueue.ratelimit_text(FakeApi(ratelimit=12)), "12/unknown")

    def test_open_pull_request_pages_are_small_but_reach_as_far(self):
        self.assertIn("first: 25, after: $cursor", enqueue.OPEN_PRS_QUERY)
        self.assertNotIn("%d", enqueue.OPEN_PRS_QUERY)
        self.assertGreaterEqual(enqueue.PAGE_SIZE * enqueue.MAX_PAGES, 1500)


class R2bTests(unittest.TestCase):
    """Tests for Hillclimb R2b: enqueue hardening, anti-starvation and narrow-first backfill."""

    def test_merge_queue_occupancy_via_graphql(self):
        ok = {"BFME II / Boat": "SUCCESS", "BFME II / source rules": "SUCCESS"}
        wide_ok = ok | {"BFME II / Header rules": "SUCCESS"}
        narrow = make_pr(850, statuses=ok)
        wide = make_pr(851, files=("include/A.h",), statuses=wide_ok)
        mq_entries = [{"number": 860, "files": {"totalCount": 1, "nodes": [{"path": "include/X.h"}]}}]
        api = FakeApi([[narrow, wide]], merge_queue_entries=mq_entries)
        result = run(api)
        self.assertEqual(result["enqueued"], [])

        api = FakeApi([[narrow, wide]], merge_queue_entries=[])
        result = run(api)
        self.assertEqual(result["enqueued"], [850])

    def test_merge_queue_fallback_on_null(self):
        ok = {"BFME II / Boat": "SUCCESS", "BFME II / source rules": "SUCCESS"}
        narrow = make_pr(852, statuses=ok)
        api = FakeApi([[narrow]])
        result = run(api)
        self.assertEqual(result["enqueued"], [852])

    def test_narrow_reject_falls_back_to_wide(self):
        ok = {"BFME II / Boat": "SUCCESS", "BFME II / source rules": "SUCCESS"}
        wide_ok = ok | {"BFME II / Header rules": "SUCCESS"}
        narrow1 = make_pr(853, statuses=ok)
        narrow2 = make_pr(854, statuses=ok)
        wide = make_pr(855, files=("include/A.h",), statuses=wide_ok)
        api = FakeApi([[narrow1, narrow2, wide]], fail_enqueue={"PR_node_853", "PR_node_854"})
        result = run(api)
        self.assertEqual(result["enqueued"], [855])

    def test_no_wide_fallback_when_narrow_succeeds(self):
        ok = {"BFME II / Boat": "SUCCESS", "BFME II / source rules": "SUCCESS"}
        wide_ok = ok | {"BFME II / Header rules": "SUCCESS"}
        narrow = make_pr(856, statuses=ok)
        wide = make_pr(857, files=("include/A.h",), statuses=wide_ok)
        api = FakeApi([[narrow, wide]])
        result = run(api)
        self.assertEqual(result["enqueued"], [856])
        self.assertNotIn(857, result["enqueued"])

    def test_wide_anti_starvation_yields_solo_slot(self):
        ok = {"BFME II / Boat": "SUCCESS", "BFME II / source rules": "SUCCESS"}
        wide_ok = ok | {"BFME II / Header rules": "SUCCESS"}
        very_old = "2026-11-23T09:00:00Z"
        narrow = make_pr(858, statuses=ok)
        wide = make_pr(859, files=("include/A.h",),
                       statuses={"BFME II / Boat": ("SUCCESS", "", very_old),
                                 "BFME II / source rules": ("SUCCESS", "", very_old),
                                 "BFME II / Header rules": ("SUCCESS", "", very_old)})
        api = FakeApi([[narrow, wide]])
        result = run(api)
        self.assertEqual(result["enqueued"], [859])

    def test_wide_at_exactly_threshold_does_not_trigger_starvation(self):
        ok = {"BFME II / Boat": "SUCCESS", "BFME II / source rules": "SUCCESS"}
        wide_ok = ok | {"BFME II / Header rules": "SUCCESS"}
        narrow = make_pr(860, statuses=ok)
        wide = make_pr(861, files=("include/A.h",),
                       statuses={"BFME II / Boat": ("SUCCESS", "", OLD),
                                 "BFME II / source rules": ("SUCCESS", "", OLD),
                                 "BFME II / Header rules": ("SUCCESS", "", OLD)})
        api = FakeApi([[narrow, wide]])
        result = run(api)
        self.assertEqual(result["enqueued"], [860])

    def test_wide_stays_solo_no_stacking(self):
        ok = {"BFME II / Boat": "SUCCESS", "BFME II / source rules": "SUCCESS"}
        wide_ok = ok | {"BFME II / Header rules": "SUCCESS"}
        narrow = make_pr(862, statuses=ok)
        wide1 = make_pr(863, files=("include/A.h",), statuses=wide_ok)
        wide2 = make_pr(864, files=("include/B.h",), statuses=wide_ok)
        api = FakeApi([[narrow, wide1, wide2]], fail_enqueue={"PR_node_862"})
        result = run(api)
        self.assertLessEqual(len(result["enqueued"]), 1)
        if result["enqueued"]:
            self.assertIn(result["enqueued"][0], [863, 864])

    def test_narrow_first_backfill_ordering(self):
        boat = {"BFME II / Boat": "SUCCESS"}
        narrow_refresh1 = make_pr(870, statuses={})
        narrow_refresh2 = make_pr(871, statuses={})
        recheck1 = make_pr(872, statuses=boat | {"BFME II / source rules": ("ERROR", "", OLD)})
        wide_refresh = make_pr(873, files=("include/A.h",), statuses={})
        api = FakeApi([[narrow_refresh1, narrow_refresh2, recheck1, wide_refresh]])
        result = run(api)
        backfilled = result["backfilled"]
        self.assertIn(872, backfilled)
        self.assertIn(870, backfilled)
        self.assertIn(873, backfilled)
        self.assertLess(backfilled.index(872), backfilled.index(870))
        self.assertLess(backfilled.index(870), backfilled.index(873))

    def test_max_wide_backfill_cap(self):
        wide_prs = [make_pr(880 + i, files=("include/A.h",), statuses={}) for i in range(5)]
        api = FakeApi([wide_prs])
        result = run(api)
        wide_in_backfill = [n for n in result["backfilled"]
                            if any(pr["number"] == n and enqueue.has_header(pr) for pr in wide_prs)]
        self.assertLessEqual(len(wide_in_backfill), enqueue.MAX_WIDE_BACKFILL)
        self.assertEqual(len(wide_in_backfill), 2)

    def test_narrow_refresh_not_displaced_by_wide(self):
        narrow_refreshes = [make_pr(1100 + i, statuses={}) for i in range(enqueue.MAX_BACKFILL)]
        wide_refresh = make_pr(1199, files=("include/A.h",), statuses={})
        api = FakeApi([narrow_refreshes + [wide_refresh]])
        result = run(api)
        self.assertNotIn(1199, result["backfilled"])
        self.assertEqual(len(result["backfilled"]), enqueue.MAX_BACKFILL)

    def test_per_workflow_inflight_cost_weights(self):
        prs = [make_pr(1000 + i, statuses={}) for i in range(5)]
        # managed-pr.yml: 2 runs × 65 = 130, rules.yml: 3 runs × 15 = 45, admission.yml: 1 run × 15 = 15
        # Total cost = 190. With 600 raw → 410 effective → 10 spare → 0 refreshes (each costs 75).
        api = FakeApi([prs], active_in_progress=1, active_queued=1,
                      rules_in_progress=2, rules_queued=1,
                      admission_in_progress=1, ratelimit=4900)
        actions = FakeApi(ratelimit=600)
        with tempfile.TemporaryDirectory() as td:
            summary = Path(td) / "summary.md"
            result = run(api, dispatch_api=actions, env={"GITHUB_STEP_SUMMARY": str(summary)})
            self.assertEqual(result["backfilled"], [])
            text = summary.read_text(encoding="utf-8")
            self.assertIn("managed-pr.yml=2", text)
            self.assertIn("rules.yml=3", text)
            self.assertIn("admission.yml=1", text)

    def test_graphql_vs_rest_ratelimit_separation(self):
        api = enqueue.GitHubApi.__new__(enqueue.GitHubApi)
        api._token = "ghs_123456789012345"
        api.ratelimit_remaining = None
        api.ratelimit_limit = None
        api.ratelimit_reset = None
        api.graphql_ratelimit_remaining = None
        api.graphql_ratelimit_limit = None
        api.rest_ratelimit_remaining = None
        api.rest_ratelimit_limit = None
        gql_headers = {"X-RateLimit-Remaining": "4800", "X-RateLimit-Limit": "5000"}
        api._record_ratelimit(gql_headers, kind="graphql")
        self.assertEqual(api.graphql_ratelimit_remaining, 4800)
        self.assertEqual(api.graphql_ratelimit_limit, 5000)
        self.assertIsNone(api.rest_ratelimit_remaining)
        rest_headers = {"X-RateLimit-Remaining": "900", "X-RateLimit-Limit": "1000"}
        api._record_ratelimit(rest_headers, kind="rest")
        self.assertEqual(api.rest_ratelimit_remaining, 900)
        self.assertEqual(api.rest_ratelimit_limit, 1000)
        self.assertEqual(api.graphql_ratelimit_remaining, 4800)

    def test_ratelimit_text_per_kind(self):
        api = FakeApi(ratelimit=1500)
        api.graphql_ratelimit_remaining = 4800
        api.graphql_ratelimit_limit = 5000
        api.rest_ratelimit_remaining = 900
        api.rest_ratelimit_limit = 1000
        self.assertEqual(enqueue.ratelimit_text(api, "graphql"), "4800/5000")
        self.assertEqual(enqueue.ratelimit_text(api, "rest"), "900/1000")
        self.assertEqual(enqueue.ratelimit_text(api), "1500/unknown")

    def test_h1_hold_uses_normal_cooldown_not_quota_hold(self):
        """H1 is a freshness hold (30m cooldown), not a daily quota like H10 (6h)."""
        hdr = ("include/A.h",)
        h1 = ("PENDING", "Held: H1 The wide comparison has not reported on this main. (main x)")
        h10 = ("PENDING", "Held: H10 daily limit for wide checks reached; retried automatically (main x)")
        h1_statuses = {"BFME II / Header rules": h1}
        h10_statuses = {"BFME II / Header rules": h10}
        self.assertEqual(enqueue.backoff(enqueue.status_map(make_pr(1, files=hdr, statuses=h1_statuses))),
                         (enqueue.COOLDOWN_SECONDS, False))
        self.assertEqual(enqueue.backoff(enqueue.status_map(make_pr(2, files=hdr, statuses=h10_statuses))),
                         (enqueue.QUOTA_HOLD_SECONDS, True))

        # H1 at 40 minutes ago (> 30m cooldown): retried. H10 at 40 minutes ago (< 6h): held.
        forty_min_ago = "2026-11-23T11:20:00Z"
        ok = {"BFME II / Boat": "SUCCESS", "BFME II / source rules": "SUCCESS"}
        prs = [
            make_pr(1400, files=hdr, statuses={k: (v, "", forty_min_ago) for k, v in ok.items()}
                    | {"BFME II / Header rules": h1 + (forty_min_ago,)}),
            make_pr(1401, files=hdr, statuses={k: (v, "", forty_min_ago) for k, v in ok.items()}
                    | {"BFME II / Header rules": h10 + (forty_min_ago,)}),
        ]
        result = run(FakeApi([prs]))
        self.assertEqual(result["backfilled"], [1400])

    def test_merge_queue_fail_closed_pauses_enqueue(self):
        """API error or truncated merge queue data pauses enqueue; null mergeQueue is a clean empty."""
        ok = {"BFME II / Boat": "SUCCESS", "BFME II / source rules": "SUCCESS"}
        narrow = make_pr(1250, statuses=ok)

        # API error: pauses enqueue, backfill still works
        api = FakeApi([[narrow]], merge_queue_fail=True)
        result = run(api)
        self.assertEqual(result["enqueued"], [])
        self.assertEqual(result["backfilled"], [])  # narrow is enqueue-eligible, not backfill

        # Null mergeQueue (no merge queue configured): proceeds cleanly
        api = FakeApi([[narrow]], merge_queue_null_mq=True)
        result = run(api)
        self.assertEqual(result["enqueued"], [1250])

        # Truncated entries (totalCount > nodes): pauses enqueue
        entry = {"number": 1260, "files": {"totalCount": 1, "nodes": [{"path": "src/X.cpp"}]}}
        api = FakeApi([[narrow]], merge_queue_entries=[entry], merge_queue_total_override=5)
        result = run(api)
        self.assertEqual(result["enqueued"], [])

        # hasNextPage: pauses enqueue
        api = FakeApi([[narrow]], merge_queue_entries=[entry], merge_queue_has_next_page=True)
        result = run(api)
        self.assertEqual(result["enqueued"], [])

        # Null pullRequest in an entry: pauses enqueue
        api = FakeApi([[narrow]], merge_queue_entries=[None])
        result = run(api)
        self.assertEqual(result["enqueued"], [])

    def test_merge_queue_derives_queue_files_from_native_snapshot(self):
        """queue_files comes from the native mergeQueue query, not the UPDATED_AT scan."""
        ok = {"BFME II / Boat": "SUCCESS", "BFME II / source rules": "SUCCESS"}
        # In-queue PR in the UPDATED_AT scan has 50 files, but merge queue snapshot says 55
        in_queue = make_pr(1260, in_queue=True, files=tuple(f"src/Q{i}.cpp" for i in range(50)))
        # Override merge queue entries to have 55 files
        mq_entry = {"number": 1260, "files": {
            "totalCount": 55, "nodes": [{"path": f"src/Q{i}.cpp"} for i in range(55)]}}
        candidate = make_pr(1261, files=tuple(f"src/A{i}.cpp" for i in range(6)), statuses=ok)
        api = FakeApi([[in_queue, candidate]], merge_queue_entries=[mq_entry])
        result = run(api)
        # 55 (from native) + 6 = 61 > MAX_FILES (60): should NOT fit
        self.assertEqual(result["enqueued"], [])

    def test_ambiguous_narrow_error_blocks_wide_fallback(self):
        """A transport-error narrow mutation may have committed; wide fallback is blocked
        unless a fresh merge queue re-query confirms the queue is empty."""
        ok = {"BFME II / Boat": "SUCCESS", "BFME II / source rules": "SUCCESS"}
        wide_ok = ok | {"BFME II / Header rules": "SUCCESS"}
        narrow = make_pr(1230, statuses=ok)
        wide = make_pr(1231, files=("include/A.h",), statuses=wide_ok)

        # Transport error on narrow; re-query shows it was committed
        requery_entries = [{"number": 1230, "files": {"totalCount": 1, "nodes": [{"path": "src/Foo.cpp"}]}}]
        api = FakeApi([[narrow, wide]],
                      fail_enqueue_transport={"PR_node_1230"},
                      merge_queue_entries_on_requery=requery_entries)
        result = run(api)
        self.assertNotIn(1231, result["enqueued"])
        self.assertEqual(result["enqueued"], [])

        # Transport error on narrow; re-query confirms empty → wide is allowed
        api = FakeApi([[narrow, wide]],
                      fail_enqueue_transport={"PR_node_1230"},
                      merge_queue_entries_on_requery=[])
        result = run(api)
        self.assertEqual(result["enqueued"], [1231])

    def test_starvation_drain_stops_narrow_topping(self):
        """When the queue is non-empty and a wide PR is starving, stop topping up narrow PRs."""
        ok = {"BFME II / Boat": "SUCCESS", "BFME II / source rules": "SUCCESS"}
        very_old = "2026-11-23T09:00:00Z"
        in_queue = make_pr(1200, in_queue=True)
        narrow = make_pr(1201, statuses=ok)
        wide = make_pr(1202, files=("include/A.h",),
                       statuses={"BFME II / Boat": ("SUCCESS", "", very_old),
                                 "BFME II / source rules": ("SUCCESS", "", very_old),
                                 "BFME II / Header rules": ("SUCCESS", "", very_old)})
        api = FakeApi([[in_queue, narrow, wide]])
        result = run(api)
        self.assertEqual(result["enqueued"], [])

    def test_starvation_wide_definite_rejection_resumes_narrow(self):
        """When a starving wide PR gets a definite GraphQL error, narrow PRs are tried."""
        ok = {"BFME II / Boat": "SUCCESS", "BFME II / source rules": "SUCCESS"}
        very_old = "2026-11-23T09:00:00Z"
        narrow = make_pr(1210, statuses=ok)
        wide = make_pr(1211, files=("include/A.h",),
                       statuses={"BFME II / Boat": ("SUCCESS", "", very_old),
                                 "BFME II / source rules": ("SUCCESS", "", very_old),
                                 "BFME II / Header rules": ("SUCCESS", "", very_old)})
        api = FakeApi([[narrow, wide]], fail_enqueue={"PR_node_1211"})
        result = run(api)
        self.assertEqual(result["enqueued"], [1210])

    def test_starving_wide_backfill_slot_reserved(self):
        """When narrow refreshes would fill all backfill slots, one is reserved for a starving wide refresh."""
        narrow_refreshes = [make_pr(1300 + i, statuses={}) for i in range(enqueue.MAX_BACKFILL)]
        starving_wide = make_pr(1399, files=("include/A.h",), statuses={
            "BFME II / Boat": ("ERROR", "", "2026-11-23T09:00:00Z")})
        api = FakeApi([narrow_refreshes + [starving_wide]])
        result = run(api)
        self.assertIn(1399, result["backfilled"])
        self.assertEqual(len(result["backfilled"]), enqueue.MAX_BACKFILL)

    def test_rest_ratelimit_cannot_trigger_graphql_gate(self):
        """A low REST X-RateLimit-Remaining cannot block GraphQL enqueue or backfill gates."""
        ok = {"BFME II / Boat": "SUCCESS", "BFME II / source rules": "SUCCESS"}
        prs = [make_pr(1050, statuses=ok)]
        # Generic/REST ratelimit low, but GraphQL ratelimit high
        api = FakeApi([prs], ratelimit=50)
        api.graphql_ratelimit_remaining = 4000
        result = run(api)
        self.assertEqual(result["enqueued"], [1050])

        # Also verify backfill proceeds with low REST but high GraphQL
        backfill_prs = [make_pr(1055, statuses={})]
        api = FakeApi([backfill_prs], ratelimit=50)
        api.graphql_ratelimit_remaining = 4000
        dispatch = FakeApi(ratelimit=900)
        result = run(api, dispatch_api=dispatch)
        self.assertEqual(result["backfilled"], [1055])



    def test_aged_wide_slot_promoted_inside_max_backfill_when_many_narrow(self):
        narrow = [make_pr(1400 + i, statuses={}) for i in range(enqueue.MAX_BACKFILL + 5)]
        aged_wide = make_pr(1499, files=("include/A.h",),
                            statuses={"BFME II / Queue backfill": ("SUCCESS", "old", "2026-04-01T00:00:00Z")})
        api = FakeApi([narrow + [aged_wide]])
        result = run(api)
        self.assertIn(1499, result["backfilled"])
        self.assertEqual(len(result["backfilled"]), enqueue.MAX_BACKFILL)

class FakeResponse:
    def __init__(self, code, body, headers):
        self.status, self._body, self.headers = code, body, headers

    def read(self, limit):
        return self._body[:limit]

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        return False


class FakeOpener:
    """Replays (code, body, headers) answers; 4xx/5xx answers raise HTTPError like urllib does."""

    def __init__(self, answers):
        self.answers = list(answers)
        self.requests = []

    def open(self, req, timeout):
        self.requests.append((req.get_method(), req.full_url))
        answer = self.answers.pop(0)
        if isinstance(answer, BaseException):
            raise answer
        code, body, headers = answer
        if code >= 400:
            raise urllib.error.HTTPError(req.full_url, code, "error", headers, io.BytesIO(body))
        return FakeResponse(code, body, headers)


HEADERS = {"X-RateLimit-Remaining": "1918", "X-RateLimit-Limit": "5000", "X-RateLimit-Reset": "1795000000"}
GRAPHQL_OK = (200, b'{"data": {"repository": {}}}', HEADERS)
BAD_GATEWAY = (502, b"<html><body>502 Bad Gateway</body></html>", {})


class GitHubApiTests(unittest.TestCase):
    def api(self, answers):
        self.opener, self.pauses = FakeOpener(answers), []
        return enqueue.GitHubApi("ghs_123456789012345", opener=self.opener, sleep=self.pauses.append)

    def test_graphql_query_retries_an_html_or_empty_5xx_body(self):
        api = self.api([BAD_GATEWAY, (504, b"", {}), GRAPHQL_OK])
        self.assertEqual(api.graphql(enqueue.OPEN_PRS_QUERY, {}), {"repository": {}})
        self.assertEqual(self.pauses, list(enqueue.RETRY_PAUSES))
        self.assertEqual((api.ratelimit_remaining, api.ratelimit_limit), (1918, 5000))

    def test_graphql_query_retries_an_empty_or_truncated_200_body(self):
        api = self.api([(200, b"", {}), (200, b'{"data": {', {}), GRAPHQL_OK])
        self.assertEqual(api.graphql(enqueue.OPEN_PRS_QUERY, {}), {"repository": {}})
        self.assertEqual(len(self.opener.requests), 3)

    def test_persistent_5xx_raises_an_api_error_not_a_decode_error(self):
        api = self.api([BAD_GATEWAY] * 3)
        with self.assertRaisesRegex(enqueue.ApiError, "HTTP 502"):
            api.graphql(enqueue.OPEN_PRS_QUERY, {})
        self.assertEqual(len(self.opener.requests), 3)

    def test_connection_failures_are_retried(self):
        api = self.api([urllib.error.URLError("reset"), (200, b'{"total_count": 2}', HEADERS)])
        self.assertEqual(api.get("actions/runs"), {"total_count": 2})
        self.assertEqual(len(self.pauses), 1)

    def test_mutations_and_rest_writes_are_sent_once(self):
        api = self.api([BAD_GATEWAY])
        with self.assertRaisesRegex(enqueue.ApiError, "HTTP 502"):
            api.graphql(enqueue.ENQUEUE_MUTATION, {"id": "PR_1", "oid": "a" * 40})
        self.assertEqual((len(self.opener.requests), self.pauses), (1, []))
        api = self.api([(502, b'{"message": "Server Error"}', {})])
        with self.assertRaisesRegex(enqueue.ApiError, "returned HTTP 502"):
            api.post("dispatches", {"event_type": "x"})
        self.assertEqual((len(self.opener.requests), self.pauses), (1, []))
        api = self.api([(204, b"", {})])
        self.assertIsNone(api.post("dispatches", {"event_type": "x"}))

    def test_core_remaining_records_the_limit(self):
        body = b'{"resources": {"core": {"remaining": 640, "limit": 1000}}}'
        api = self.api([BAD_GATEWAY, (200, body, {})])
        self.assertEqual(api.core_remaining(), 640)
        self.assertEqual((api.ratelimit_remaining, api.ratelimit_limit), (640, 1000))


if __name__ == "__main__":
    unittest.main()
