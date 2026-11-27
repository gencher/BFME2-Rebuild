"""Tests for state carry across ci/queue/ and ci/control-plane/ changes,
the R1 overcap-exit state.yml migration, and the artifact-first ancestor lookup.

Run from the repository root:  python3 -B -m unittest discover -s ci/state/tests -v
"""
from pathlib import Path
import sys
import unittest

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[2]
sys.path.insert(0, str(ROOT / "ci/managed-state"))
sys.path.insert(0, str(ROOT / "ci/managed-check"))

import state_delta  # noqa: E402
import state_fetch  # noqa: E402


class ReportDirsTest(unittest.TestCase):
    def test_queue_and_control_plane_in_report_dirs(self):
        self.assertIn("ci/queue/", state_delta.REPORT_DIRS)
        self.assertIn("ci/control-plane/", state_delta.REPORT_DIRS)

    def test_queue_path_is_publication(self):
        self.assertTrue("ci/queue/enqueue.py".startswith(state_delta.REPORT_DIRS))

    def test_control_plane_path_is_publication(self):
        self.assertTrue("ci/control-plane/dispatch.py".startswith(state_delta.REPORT_DIRS))


class OvercapExitMigrationTest(unittest.TestCase):
    def test_overcap_exit_migration_registered(self):
        pair = state_delta.OVERCAP_EXIT_MIGRATION
        self.assertIn(pair, state_delta.STATE_FLOW_MIGRATIONS)
        self.assertEqual(pair[0], "ec1ec3a1489c459a4513cfef8937101998a740667c0662d15e4df00f102bce61")
        self.assertEqual(pair[1], "ad0a65303832e04fc813d9a49d875b645f47e6d70b3422ba50d141914487e62d")

    def test_overcap_exit_chains_from_hydration(self):
        self.assertEqual(state_delta.OVERCAP_EXIT_MIGRATION[0], state_delta.HYDRATION_MIGRATION[1])


class AutoSeedMigrationTest(unittest.TestCase):
    def test_auto_seed_migration_matches_the_workflow(self):
        import hashlib
        pair = state_delta.AUTO_SEED_MIGRATION
        self.assertIn(pair, state_delta.STATE_FLOW_MIGRATIONS)
        self.assertEqual(pair[1], state_delta.STATE_RUNNER_MIGRATION[0])

    def test_state_runner_migration_matches_the_workflow(self):
        pair = state_delta.STATE_RUNNER_MIGRATION
        self.assertIn(pair, state_delta.STATE_FLOW_MIGRATIONS)
        self.assertEqual(pair[1], state_delta.SEED_LIFECYCLE_MIGRATION[0])

    def test_seed_lifecycle_migration_matches_the_workflow(self):
        import hashlib
        pair = state_delta.SEED_LIFECYCLE_MIGRATION
        self.assertIn(pair, state_delta.STATE_FLOW_MIGRATIONS)
        self.assertEqual(pair[1], state_delta.SEED_YAML_FIX_MIGRATION[0])
        pair2 = state_delta.SEED_YAML_FIX_MIGRATION
        self.assertIn(pair2, state_delta.STATE_FLOW_MIGRATIONS)
        text = (Path(__file__).resolve().parents[3] / ".github/workflows/state.yml").read_bytes()
        self.assertEqual(hashlib.sha256(text).hexdigest(), pair2[1])


class ArtifactFirstLookupTest(unittest.TestCase):
    def _repo_id(self):
        from common import REPO_ID
        return REPO_ID

    def test_artifact_first_fallback_finds_artifact_past_run_window(self):
        REPO_ID = self._repo_id()
        target_sha = "b" * 40

        class FakeApi:
            def __init__(self):
                self.calls = []

            def call(self, path):
                self.calls.append(path)
                if path.startswith("actions/workflows/state.yml/runs?"):
                    return {"workflow_runs": []}
                if "actions/artifacts?" in path:
                    return {"artifacts": [
                        {"name": "bfme-exact-main-state-1", "expired": False, "id": 999,
                         "size_in_bytes": 1000, "digest": "sha256:" + "a" * 64,
                         "workflow_run": {"id": 100, "head_branch": "main"}},
                    ]}
                if path == "actions/runs/100":
                    return {"id": 100, "run_attempt": 1, "path": ".github/workflows/state.yml",
                            "head_sha": target_sha, "head_branch": "main", "status": "completed",
                            "event": "push", "conclusion": "success",
                            "repository": {"id": REPO_ID}}
                if path.startswith("actions/runs/") and "/artifacts" in path:
                    return {"artifacts": []}
                return {"workflow_runs": []}

        api = FakeApi()
        shas = ["c" * 40] * 30 + [target_sha]
        result = state_fetch.select_ancestor_artifact(api, shas)
        self.assertIsNotNone(result)
        self.assertEqual(result[0], 999)

    def test_no_artifacts_returns_none(self):
        class EmptyApi:
            def call(self, path):
                if "actions/artifacts?" in path:
                    return {"artifacts": []}
                return {"workflow_runs": []}

        result = state_fetch.select_ancestor_artifact(EmptyApi(), ["a" * 40])
        self.assertIsNone(result)

    def test_attempt_mismatch_rejected(self):
        REPO_ID = self._repo_id()
        target_sha = "b" * 40

        class FakeApi:
            def call(self, path):
                if path.startswith("actions/workflows/state.yml/runs?"):
                    return {"workflow_runs": [
                        {"id": 100, "run_attempt": 2, "path": ".github/workflows/state.yml",
                         "head_sha": target_sha, "head_branch": "main", "status": "completed",
                         "event": "push", "conclusion": "success",
                         "repository": {"id": REPO_ID}},
                    ]}
                if "actions/artifacts?" in path and "name=bfme-exact-main-state-2" in path:
                    return {"artifacts": []}
                if "actions/artifacts?" in path and "name=bfme-exact-main-state-1" in path:
                    return {"artifacts": [
                        {"name": "bfme-exact-main-state-1", "expired": False, "id": 888,
                         "size_in_bytes": 500, "digest": "sha256:" + "b" * 64,
                         "workflow_run": {"id": 100, "head_branch": "main"}},
                    ]}
                if path.startswith("actions/runs/") and "/artifacts" in path:
                    return {"artifacts": [
                        {"name": "bfme-exact-main-state-1", "expired": False, "id": 888,
                         "size_in_bytes": 500, "digest": "sha256:" + "b" * 64,
                         "workflow_run": {"id": 100, "head_branch": "main"}},
                    ]}
                if "actions/artifacts?" in path:
                    return {"artifacts": []}
                return {"workflow_runs": []}

        result = state_fetch.select_ancestor_artifact(FakeApi(), [target_sha])
        self.assertIsNone(result)

    def test_nearer_sha_preferred_over_older(self):
        REPO_ID = self._repo_id()
        near_sha = "1" * 40
        far_sha = "2" * 40

        class FakeApi:
            def call(self, path):
                if path.startswith("actions/workflows/state.yml/runs?"):
                    return {"workflow_runs": [
                        {"id": 200, "run_attempt": 1, "path": ".github/workflows/state.yml",
                         "head_sha": far_sha, "head_branch": "main", "status": "completed",
                         "event": "push", "conclusion": "success",
                         "repository": {"id": REPO_ID}},
                        {"id": 300, "run_attempt": 1, "path": ".github/workflows/state.yml",
                         "head_sha": near_sha, "head_branch": "main", "status": "completed",
                         "event": "push", "conclusion": "success",
                         "repository": {"id": REPO_ID}},
                    ]}
                if "actions/artifacts?" in path:
                    return {"artifacts": [
                        {"name": "bfme-exact-main-state-1", "expired": False, "id": 701,
                         "size_in_bytes": 500, "digest": "sha256:" + "c" * 64,
                         "workflow_run": {"id": 200, "head_branch": "main"}},
                        {"name": "bfme-exact-main-state-1", "expired": False, "id": 702,
                         "size_in_bytes": 500, "digest": "sha256:" + "d" * 64,
                         "workflow_run": {"id": 300, "head_branch": "main"}},
                    ]}
                if path.startswith("actions/runs/") and "/artifacts" in path:
                    return {"artifacts": []}
                return {"workflow_runs": []}

        result = state_fetch.select_ancestor_artifact(FakeApi(), [near_sha, far_sha])
        self.assertIsNotNone(result)
        self.assertEqual(result[0], 702)

    def test_page2_artifact_discovery(self):
        REPO_ID = self._repo_id()
        target_sha = "d" * 40

        class FakeApi:
            def call(self, path):
                if path.startswith("actions/workflows/state.yml/runs?"):
                    return {"workflow_runs": []}
                if "actions/artifacts?" in path and "&page=1" in path:
                    return {"artifacts": [{"name": "other", "expired": False, "id": i,
                                           "size_in_bytes": 100, "digest": "sha256:" + "0" * 64,
                                           "workflow_run": {"id": i, "head_branch": "main"}}
                                          for i in range(100)]}
                if "actions/artifacts?" in path and "&page=2" in path:
                    return {"artifacts": [
                        {"name": "bfme-exact-main-state-1", "expired": False, "id": 555,
                         "size_in_bytes": 800, "digest": "sha256:" + "e" * 64,
                         "workflow_run": {"id": 400, "head_branch": "main"}},
                    ]}
                if path == "actions/runs/400":
                    return {"id": 400, "run_attempt": 1, "path": ".github/workflows/state.yml",
                            "head_sha": target_sha, "head_branch": "main", "status": "completed",
                            "event": "push", "conclusion": "success",
                            "repository": {"id": REPO_ID}}
                if "actions/artifacts?" in path:
                    return {"artifacts": []}
                return {"workflow_runs": []}

        result = state_fetch.select_ancestor_artifact(FakeApi(), [target_sha])
        self.assertIsNotNone(result)
        self.assertEqual(result[0], 555)


class MultiStepComposeAndSeedOriginTest(unittest.TestCase):
    def test_compose_transition_chains_multiple_tu_move_migrations(self):
        import hashlib, subprocess, tempfile
        from unittest import mock

        with tempfile.TemporaryDirectory() as tmp:
            repo = Path(tmp)
            def git_cmd(*args):
                subprocess.check_call(["git", "-C", str(repo), *args],
                                      stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            git_cmd("init", "-q")
            git_cmd("config", "user.email", "t@example.com")
            git_cmd("config", "user.name", "t")
            for p, content in [
                ("config/map.json", b'{"v":0}\n'),
                ("src/unk/A.cpp", b"int a() { return 0; }\n"),
                ("src/unk/B.cpp", b"int b() { return 0; }\n"),
                ("src/unk/C.cpp", b"int c() { return 0; }\n"),
            ]:
                fp = repo / p
                fp.parent.mkdir(parents=True, exist_ok=True)
                fp.write_bytes(content)
            git_cmd("add", "-A"); git_cmd("commit", "-q", "-m", "base")
            c0 = subprocess.check_output(["git", "-C", str(repo), "rev-parse", "HEAD"], text=True).strip()

            # Step 1: migration 1 edits map.json and removes A.cpp
            (repo / "config/map.json").write_bytes(b'{"v":1}\n')
            (repo / "src/unk/A.cpp").unlink()
            git_cmd("add", "-A"); git_cmd("commit", "-q", "-m", "mig1")
            c1 = subprocess.check_output(["git", "-C", str(repo), "rev-parse", "HEAD"], text=True).strip()

            # Step 2: ordinary source edit on C.cpp
            (repo / "src/unk/C.cpp").write_bytes(b"int c() { return 1; }\n")
            git_cmd("add", "-A"); git_cmd("commit", "-q", "-m", "src")
            c2 = subprocess.check_output(["git", "-C", str(repo), "rev-parse", "HEAD"], text=True).strip()

            # Step 3: migration 2 edits map.json again and removes B.cpp
            (repo / "config/map.json").write_bytes(b'{"v":2}\n')
            (repo / "src/unk/B.cpp").unlink()
            git_cmd("add", "-A"); git_cmd("commit", "-q", "-m", "mig2")
            c3 = subprocess.check_output(["git", "-C", str(repo), "rev-parse", "HEAD"], text=True).strip()

            def bsha(rev, path):
                r = subprocess.run(["git", "-C", str(repo), "show", f"{rev}:{path}"], capture_output=True)
                return hashlib.sha256(r.stdout).hexdigest() if r.returncode == 0 else None

            mig1 = {
                "files": {"config/map.json": (bsha(c0, "config/map.json"), bsha(c1, "config/map.json")),
                          "src/unk/A.cpp": (bsha(c0, "src/unk/A.cpp"), None)},
                "old_tus": frozenset({"src/unk/A.cpp"}),
                "new_tus": frozenset(),
                "force_changed": frozenset({"src/unk/c.cpp"}),
            }
            mig2 = {
                "files": {"config/map.json": (bsha(c1, "config/map.json"), bsha(c3, "config/map.json")),
                          "src/unk/B.cpp": (bsha(c1, "src/unk/B.cpp"), None)},
                "old_tus": frozenset({"src/unk/B.cpp"}),
                "new_tus": frozenset(),
                "force_changed": frozenset(),
            }
            with mock.patch.object(state_delta, "TU_MOVE_MIGRATIONS", (mig1, mig2)):
                composed = state_delta.compose_transition(repo, c0, c3)
                self.assertIsNotNone(composed)
                changed, old_tus, new_tus, _bundle = composed
                self.assertEqual(changed, {"src/unk/c.cpp"})
                self.assertEqual(old_tus, frozenset({"src/unk/A.cpp", "src/unk/B.cpp"}))
                self.assertEqual(new_tus, frozenset())

    def test_eligible_cancels_obsolete_seed_and_defers_compatible_seed(self):
        from unittest import mock
        import state_origin
        from common import REPO_ID

        old_sha = "1" * 40
        new_sha = "2" * 40
        seed_run = {
            "id": 777,
            "head_sha": old_sha,
            "head_branch": "main",
            "event": "workflow_dispatch",
            "path": ".github/workflows/state.yml",
            "status": "in_progress",
            "repository": {"id": REPO_ID},
            "display_title": "Seed protected main state " + old_sha,
        }

        class FakeApi:
            token = "tok"
            def call(self, path):
                if path == "git/ref/heads/main":
                    return {"object": {"sha": new_sha}}
                if path.startswith("actions/workflows/state.yml/runs?"):
                    return {"workflow_runs": [seed_run]}
                return {"workflow_runs": [], "artifacts": []}

        # Compatible seed -> defers without cancelling
        with mock.patch.object(state_origin, "select_run", return_value=None), \
             mock.patch.object(state_origin, "predecessors", return_value=[old_sha]), \
             mock.patch.object(state_origin, "source_changes", return_value={"src/a.cpp"}), \
             mock.patch.object(state_origin, "cancel_run") as cancel_mock:
            self.assertFalse(state_origin.eligible(FakeApi(), new_sha, False, "admit", root=Path(".")))
            cancel_mock.assert_not_called()

        # Obsolete seed (source_changes is None) -> cancels and returns True
        with mock.patch.object(state_origin, "select_run", return_value=None), \
             mock.patch.object(state_origin, "predecessors", return_value=[old_sha]), \
             mock.patch.object(state_origin, "source_changes", return_value=None), \
             mock.patch.object(state_origin, "cancel_run", return_value=True) as cancel_mock:
            self.assertTrue(state_origin.eligible(FakeApi(), new_sha, False, "admit", root=Path(".")))
            cancel_mock.assert_called_once_with(mock.ANY, 777)

    def test_after_seed_dispatches_only_when_main_moved(self):
        from unittest import mock
        import state_origin

        sha1 = "a" * 40
        sha2 = "b" * 40
        class FakeApi:
            def __init__(self, cur): self.cur = cur
            def call(self, path):
                return {"object": {"sha": self.cur}}

        with mock.patch.object(state_origin, "_dispatch_refresh") as disp:
            state_origin.after_seed(FakeApi(sha1), sha1)
            disp.assert_not_called()
            state_origin.after_seed(FakeApi(sha2), sha1)
            disp.assert_called_once()


if __name__ == "__main__":
    unittest.main()
