"""Auto-enqueue ready pull requests into GitHub Native Merge Queue and backfill the rest.

Every open pull request lands in one bucket per run:
  enqueue  Boat and source rules passed (and Header rules, for include/), so it goes into the merge queue.
  recheck  Boat passed but source rules are missing or unfinished: bfme-source-recheck (hosted, no Boat VM).
  refresh  Boat is missing, unfinished or older than its last merge-queue removal, or a header change needs a
           wide check on the current main: bfme-source-refresh, then admission decides and measures.
  none     a verdict or hold only the author or a maintainer can change.
Backfill writes a BFME II / Queue backfill marker on the head before it dispatches, rotates least recently touched
first, and never touches the same head twice within COOLDOWN_SECONDS, so it always moves forward through the
backlog and stops by itself when nothing is left. A head held on a quota (a wide H1/H10 hold or the admission daily
limit) waits for the next UTC day or QUOTA_HOLD_SECONDS, and one held on a runner outage waits OUTAGE_HOLD_SECONDS:
admission's own hourly sweep already retries both, so a 30-minute refresh only spends the Actions token for nothing.

A wide (include/) change is compared again by every merge group stacked behind it, so it enters the merge queue
alone: narrow changes go first, a wide change is enqueued only into an empty queue when no narrow change is ready,
and nothing is stacked behind a wide change already in the queue.
"""
import datetime
import http.client
import json
import os
from pathlib import Path
import re
import sys
import time
import urllib.error
import urllib.request

OWNER = "Open-BFME"
NAME = "BFME2-Rebuild"
REPO = f"{OWNER}/{NAME}"
CANARY_PRS = frozenset({3, 4, 21, 27, 29})
DEFAULT_REFUSED_PREFIXES = ("canary/", "ci-corpus/", "corpus/")
HOLD_LABEL = "landing-hold"
MAX_FILES = 60
MAX_ENQUEUE = 16
MAX_BACKFILL = 18
MAX_ACTIVE_BOAT = 18
PIPELINE_WORKFLOWS = ("managed-pr.yml", "merge-queue-boat.yml", "rules.yml", "admission.yml")
COOLDOWN_SECONDS = 1800
SOURCE = "BFME II / source rules"
BOAT = "BFME II / Boat"
HEADER = "BFME II / Header rules"
INLINE = "BFME II / Inline virtual"
ADMISSION = "BFME II / Admission"
MARKER = "BFME II / Queue backfill"
TOUCHED = (SOURCE, BOAT, ADMISSION, MARKER)
# Holds on this exact head that no re-run changes (ci/admission/admit.py, tools/gates/workflow.py).
FINAL_HOLDS = {
    SOURCE: ("Conflicts with main", "No shared history with main"),
    ADMISSION: ("Held: source-only admission covers at most eight paths",
                "Held: the compiler check failed to run five times", "This conflicts with main",
                "This branch shares no history"),
}
# Header rules holds that only a wide check on the current main clears (ci/header/compose.py H1, H10).
WIDE_HOLDS = ("Held: H1 ", "Held: H10 ")
# Admission holds that its hourly retry sweep re-decides by itself (ci/admission/admit.py DAILY_LIMIT, OUTAGE).
QUOTA_HOLDS = ("Held: daily limit",)
OUTAGE_HOLDS = ("Held: compiler runners unavailable",)
QUOTA_HOLD_SECONDS = 6 * 3600
OUTAGE_HOLD_SECONDS = 2 * 3600
EVENTS = {"refresh": "bfme-source-refresh", "recheck": "bfme-source-recheck"}
MIN_RATELIMIT_BACKFILL = 500
MIN_RATELIMIT_ENQUEUE = 100
ACTIONS_TOKEN_RESERVE = 400
CALLS_PER_BACKFILL = {"refresh": 75, "recheck": 15}
MAX_WIDE_BACKFILL = 2
WIDE_STARVATION_SECONDS = 2 * 3600
INFLIGHT_COST = {"managed-pr.yml": 65, "merge-queue-boat.yml": 45, "rules.yml": 15, "admission.yml": 15}
LIMIT = 4 * 1024**2
HEX40 = re.compile(r"[0-9a-f]{40}\Z")
# Smaller pages keep the nested status/file/review query under GitHub's GraphQL timeout (502/504 at 50 a page);
# MAX_PAGES keeps the same 1,500 pull request reach.
PAGE_SIZE = 25
MAX_PAGES = 60
# Query and GET retries after a transient failure (5xx, an empty or non-JSON body, a dropped connection).
RETRY_PAUSES = (2, 5)
TRANSIENT = frozenset({500, 502, 503, 504})

OPEN_PRS_QUERY = """
query($owner: String!, $name: String!, $cursor: String) {
  repository(owner: $owner, name: $name) {
    defaultBranchRef {
      target { oid }
    }
    pullRequests(states: OPEN, baseRefName: "main", first: %d, after: $cursor,
                 orderBy: {field: UPDATED_AT, direction: ASC}) {
      pageInfo { hasNextPage endCursor }
      nodes {
        id
        number
        isDraft
        headRefOid
        headRefName
        mergeable
        isInMergeQueue
        author {
          login
          ... on User { databaseId }
        }
        labels(first: 20) {
          nodes { name }
        }
        files(first: 65) {
          totalCount
          nodes { path }
        }
        commits(last: 1) {
          nodes {
            commit {
              status {
                contexts {
                  context
                  state
                  description
                  createdAt
                }
              }
            }
          }
        }
        timelineItems(last: 1, itemTypes: [REMOVED_FROM_MERGE_QUEUE_EVENT]) {
          nodes {
            ... on RemovedFromMergeQueueEvent { createdAt }
          }
        }
        latestOpinionatedReviews(first: 20) {
          nodes {
            state
            commit { oid }
            author {
              ... on User { databaseId }
            }
          }
        }
      }
    }
  }
}
""" % PAGE_SIZE

MERGE_QUEUE_QUERY = """
query($owner: String!, $name: String!) {
  repository(owner: $owner, name: $name) {
    mergeQueue(branch: "main") {
      entries(first: 25) {
        totalCount
        pageInfo { hasNextPage }
        nodes {
          pullRequest {
            number
            files(first: 65) {
              totalCount
              nodes { path }
            }
          }
        }
      }
    }
  }
}
"""

ENQUEUE_MUTATION = """
mutation($id: ID!, $oid: GitObjectID!) {
  enqueuePullRequest(input: {pullRequestId: $id, expectedHeadOid: $oid}) {
    mergeQueueEntry {
      id
      position
    }
  }
}
"""


class ApiError(Exception):
    pass


class NoRedirect(urllib.request.HTTPRedirectHandler):
    def redirect_request(self, req, fp, code, msg, headers, newurl):
        return None


class GitHubApi:
    def __init__(self, token, token_name="BFME_QUEUE_TOKEN", *, opener=None, sleep=time.sleep):
        if not token or not re.fullmatch(r"[A-Za-z0-9_.-]{15,2048}", token):
            raise ApiError(f"Missing or malformed {token_name}.")
        self._token = token
        self._opener = opener or urllib.request.build_opener(urllib.request.ProxyHandler({}), NoRedirect())
        self._sleep = sleep
        self.ratelimit_remaining = None
        self.ratelimit_limit = None
        self.ratelimit_reset = None
        self.graphql_ratelimit_remaining = None
        self.graphql_ratelimit_limit = None
        self.rest_ratelimit_remaining = None
        self.rest_ratelimit_limit = None

    def _record_ratelimit(self, headers, *, kind=None):
        if not headers:
            return
        for header, base in (("X-RateLimit-Remaining", "ratelimit_remaining"),
                             ("X-RateLimit-Limit", "ratelimit_limit"),
                             ("X-RateLimit-Reset", "ratelimit_reset")):
            value = headers.get(header)
            if value is not None:
                try:
                    v = int(value)
                except (TypeError, ValueError):
                    continue
                setattr(self, base, v)
                if kind == "graphql" and base != "ratelimit_reset":
                    setattr(self, f"graphql_{base}", v)
                elif kind == "rest" and base != "ratelimit_reset":
                    setattr(self, f"rest_{base}", v)

    def _request(self, method, url, data, *, kind=None):
        req = urllib.request.Request(url, data=data, method=method, headers={
            "Authorization": "Bearer " + self._token,
            "Accept": "application/vnd.github+json",
            "X-GitHub-Api-Version": "2022-11-28",
            "Content-Type": "application/json",
        })
        try:
            with self._opener.open(req, timeout=30) as resp:
                code, raw = resp.status, resp.read(LIMIT + 1)
                self._record_ratelimit(resp.headers, kind=kind)
        except urllib.error.HTTPError as exc:
            code, raw = exc.code, exc.read(LIMIT + 1)
            self._record_ratelimit(exc.headers, kind=kind)
        except (urllib.error.URLError, http.client.HTTPException, OSError) as exc:
            raise ApiError(f"Connection failure on {method} {url}: {type(exc).__name__}") from None
        return code, raw

    def _send(self, method, url, body=None, *, retries=0, expect_json=False, kind=None):
        """Return (HTTP code, decoded JSON or None). A 5xx, a dropped connection, a body that is not JSON, or (with
        expect_json) a 200 without a JSON object is transient: it is tried again up to `retries` times."""
        data = json.dumps(body, sort_keys=True).encode("utf-8") if body is not None else None
        for attempt in range(retries + 1):
            last = attempt == retries
            try:
                code, raw = self._request(method, url, data, kind=kind)
            except ApiError:
                if last:
                    raise
                self._sleep(RETRY_PAUSES[min(attempt, len(RETRY_PAUSES) - 1)])
                continue
            if len(raw) > LIMIT:
                raise ApiError("GitHub API response exceeded size limit.")
            malformed = False
            try:
                payload = json.loads(raw) if raw.strip() else None
            except ValueError:      # json.JSONDecodeError and UnicodeDecodeError: an HTML or truncated 5xx page
                payload, malformed = None, True
            transient = (code in TRANSIENT or malformed
                         or (expect_json and code == 200 and not isinstance(payload, dict)))
            if transient and not last:
                self._sleep(RETRY_PAUSES[min(attempt, len(RETRY_PAUSES) - 1)])
                continue
            if malformed:
                raise ApiError(f"{method} {url} returned HTTP {code} without a JSON body")
            return code, payload
        raise AssertionError("unreachable")

    def graphql(self, query, variables):
        retries = 0 if query.lstrip().startswith("mutation") else len(RETRY_PAUSES)
        code, payload = self._send("POST", "https://api.github.com/graphql",
                                   {"query": query, "variables": variables}, retries=retries, expect_json=True,
                                   kind="graphql")
        if code != 200 or not isinstance(payload, dict):
            raise ApiError(f"GraphQL HTTP {code}")
        if payload.get("errors"):
            msg = (payload["errors"][0] or {}).get("message", "unknown error")
            raise ApiError(f"GraphQL error: {msg}")
        return payload.get("data") or {}

    def get(self, path):
        code, payload = self._send("GET", f"https://api.github.com/repos/{REPO}/{path}",
                                   retries=len(RETRY_PAUSES), expect_json=True, kind="rest")
        if code != 200:
            raise ApiError(f"GET {path} returned HTTP {code}")
        return payload

    def core_remaining(self):
        """REST calls this token has left this hour (GET /rate_limit itself costs none)."""
        code, payload = self._send("GET", "https://api.github.com/rate_limit",
                                   retries=len(RETRY_PAUSES), expect_json=True, kind="rest")
        try:
            core = payload["resources"]["core"]
            remaining = int(core["remaining"])
        except (TypeError, KeyError, ValueError):
            raise ApiError(f"GET rate_limit returned HTTP {code}") from None
        if code != 200:
            raise ApiError(f"GET rate_limit returned HTTP {code}")
        self.ratelimit_remaining = remaining
        try:
            self.ratelimit_limit = int(core["limit"])
        except (KeyError, TypeError, ValueError):
            pass
        return remaining

    def post(self, path, body):
        code, payload = self._send("POST", f"https://api.github.com/repos/{REPO}/{path}", body, kind="rest")
        if code not in (200, 201, 204):
            raise ApiError(f"POST {path} returned HTTP {code}")
        return payload


def load_config(path=None):
    config_path = Path(path) if path else Path(__file__).resolve().parents[1] / "landing/landing.json"
    data = json.loads(config_path.read_text(encoding="utf-8"))
    approvers = {int(uid) for uid in data.get("header_approvers", []) if type(uid) is int and uid > 0}
    refused = tuple(data.get("refused_branches", DEFAULT_REFUSED_PREFIXES))
    return approvers, refused


def allowed_file(path):
    if not isinstance(path, str) or not path or path.startswith("/") or ".." in path.split("/"):
        return False
    if path.casefold().startswith("src/code/tools/"):
        return False
    return path.startswith("src/") or path.startswith("include/")


def status_map(pr):
    commits = ((pr.get("commits") or {}).get("nodes") or [])
    if not commits:
        return {}
    status = ((commits[-1] or {}).get("commit") or {}).get("status") or {}
    contexts = status.get("contexts") or []
    return {c["context"]: c for c in contexts if isinstance(c, dict) and c.get("context")}


def state(statuses, context):
    return (statuses.get(context) or {}).get("state")


def created(statuses, context):
    return (statuses.get(context) or {}).get("createdAt") or ""


def removed_at(pr):
    nodes = ((pr.get("timelineItems") or {}).get("nodes") or [])
    return max((n.get("createdAt") or "" for n in nodes if isinstance(n, dict)), default="")


def header_approved(pr, approvers):
    head_oid = pr.get("headRefOid")
    author_id = (pr.get("author") or {}).get("databaseId")
    reviews = ((pr.get("latestOpinionatedReviews") or {}).get("nodes") or [])
    for rev in reviews:
        if not isinstance(rev, dict) or rev.get("state") != "APPROVED":
            continue
        if (rev.get("commit") or {}).get("oid") != head_oid:
            continue
        reviewer_id = (rev.get("author") or {}).get("databaseId")
        if reviewer_id in approvers and reviewer_id != author_id:
            return True
    return False


def base_eligible(pr, refused_prefixes):
    number = pr.get("number")
    if pr.get("isDraft") or pr.get("isInMergeQueue") or pr.get("mergeable") == "CONFLICTING":
        return None
    if number in CANARY_PRS:
        return None
    head_ref = pr.get("headRefName") or ""
    if any(head_ref.startswith(prefix) for prefix in refused_prefixes):
        return None
    labels = {node.get("name") for node in ((pr.get("labels") or {}).get("nodes") or []) if isinstance(node, dict)}
    if HOLD_LABEL in labels:
        return None
    files_obj = pr.get("files") or {}
    nodes = files_obj.get("nodes") or []
    total_count = files_obj.get("totalCount", len(nodes))
    paths = [n.get("path", "") for n in nodes if isinstance(n, dict)]
    if not paths or total_count > MAX_FILES or len(paths) > MAX_FILES:
        return None
    if not all(allowed_file(p) for p in paths):
        return None
    return paths


def classify(pr, paths, statuses, approvers):
    """Return "enqueue", "recheck", "refresh" or None (see the module docstring)."""
    if state(statuses, SOURCE) == "FAILURE" or state(statuses, BOAT) == "FAILURE":
        return None
    for context, prefixes in FINAL_HOLDS.items():
        if ((statuses.get(context) or {}).get("description") or "").startswith(prefixes):
            return None
    # A merge-queue removal (a failed group, a conflict, a maintainer) needs a Boat verdict newer than it, so a
    # change that keeps failing in the queue is measured again instead of being re-enqueued forever.
    if state(statuses, BOAT) != "SUCCESS" or removed_at(pr) > created(statuses, BOAT):
        return "refresh"
    if any(p.startswith("include/") for p in paths) and not header_approved(pr, approvers):
        header = statuses.get(HEADER)
        if header is None or (header.get("state") == "PENDING"
                              and (header.get("description") or "").startswith(WIDE_HOLDS)):
            return "refresh"
        if header.get("state") != "SUCCESS":
            return None
    if state(statuses, INLINE) == "FAILURE":
        return None
    return "enqueue" if state(statuses, SOURCE) == "SUCCESS" else "recheck"


def touched(statuses):
    return max(created(statuses, context) for context in TOUCHED)


def _held(statuses, context, prefixes):
    status = statuses.get(context) or {}
    return status.get("state") == "PENDING" and (status.get("description") or "").startswith(prefixes)


def backoff(statuses):
    """Return (seconds, daily): how long a head waits after it was last touched, and whether a new UTC day (the
    daily budgets' reset) ends the wait early. Quota and outage holds only clear on admission's hourly sweep or a new
    day, so refreshing them every COOLDOWN_SECONDS spends the Actions token and changes nothing."""
    if _held(statuses, HEADER, ("Held: H10 ",)) or _held(statuses, ADMISSION, QUOTA_HOLDS):
        return QUOTA_HOLD_SECONDS, True
    if _held(statuses, ADMISSION, OUTAGE_HOLDS):
        return OUTAGE_HOLD_SECONDS, False
    return COOLDOWN_SECONDS, False


def cooling(stamp, now, seconds=COOLDOWN_SECONDS, daily=False):
    if not stamp:
        return False
    try:
        when = datetime.datetime.fromisoformat(stamp.replace("Z", "+00:00"))
    except ValueError:
        return False
    elapsed = (now - when).total_seconds()
    if elapsed < COOLDOWN_SECONDS:
        return True
    if daily and when.astimezone(datetime.timezone.utc).date() < now.astimezone(datetime.timezone.utc).date():
        return False
    return elapsed < seconds


def file_count(pr):
    files = pr.get("files") or {}
    return int(files.get("totalCount", len(files.get("nodes") or [])) or 0)


def has_header(pr):
    return any(isinstance(n, dict) and (n.get("path") or "").startswith("include/")
               for n in ((pr.get("files") or {}).get("nodes") or []))


def list_open_prs(api):
    prs = []
    main_sha = None
    cursor = None
    for _ in range(MAX_PAGES):
        data = api.graphql(OPEN_PRS_QUERY, {"owner": OWNER, "name": NAME, "cursor": cursor})
        repo = data.get("repository") or {}
        if main_sha is None:
            main_sha = ((repo.get("defaultBranchRef") or {}).get("target") or {}).get("oid")
        conn = repo.get("pullRequests") or {}
        prs.extend(conn.get("nodes") or [])
        page = conn.get("pageInfo") or {}
        if not page.get("hasNextPage") or not page.get("endCursor"):
            break
        cursor = page["endCursor"]
    return main_sha, prs


def fetch_merge_queue(api):
    """Return (count, queue_files, has_wide) from the native merge queue.
    Returns (0, 0, False) if the repository has no merge queue configured.
    Returns (None, None, None) on API error or incomplete/truncated data (fail-closed)."""
    try:
        data = api.graphql(MERGE_QUEUE_QUERY, {"owner": OWNER, "name": NAME})
    except ApiError:
        return None, None, None
    repo = data.get("repository") or {}
    mq = repo.get("mergeQueue")
    if mq is None:
        return 0, 0, False
    entries = mq.get("entries")
    if entries is None:
        return None, None, None
    total = entries.get("totalCount", 0)
    nodes = entries.get("nodes") or []
    page_info = entries.get("pageInfo") or {}
    if total > len(nodes) or page_info.get("hasNextPage"):
        return None, None, None
    queue_files = 0
    has_wide = False
    for node in nodes:
        pr = (node or {}).get("pullRequest")
        if pr is None:
            return None, None, None
        files_obj = pr.get("files") or {}
        file_nodes = files_obj.get("nodes") or []
        file_total = files_obj.get("totalCount", len(file_nodes))
        queue_files += file_total
        pr_has_include = any(isinstance(f, dict) and (f.get("path") or "").startswith("include/")
                             for f in file_nodes)
        if pr_has_include:
            has_wide = True
        elif file_total > len(file_nodes):
            return None, None, None
    return total, queue_files, has_wide


def _oldest_boat_stamp(pr):
    """Return the Boat SUCCESS createdAt for a PR, or empty string if none."""
    statuses = status_map(pr)
    boat = statuses.get(BOAT) or {}
    if boat.get("state") == "SUCCESS":
        return boat.get("createdAt") or ""
    return ""


def wave(candidates, queued, *, mq_count=None, mq_has_wide=None, now=None):
    """Return (narrow, wide_fallback): narrow PRs to try first, then wide PRs to fall back to.

    A wide PR enters the queue only when the queue is empty and no narrow enqueue succeeds. The wide_fallback list
    contains at most 1 wide PR. Anti-starvation: if the oldest ready wide PR's Boat pass is older than
    WIDE_STARVATION_SECONDS and the queue is empty, it gets a solo slot even if narrow PRs exist."""
    queue_count = mq_count if mq_count is not None else len(queued)
    queue_has_wide = mq_has_wide if mq_has_wide is not None else any(has_header(pr) for pr in queued)
    if queue_has_wide:
        return [], []
    narrow = [pr for pr in candidates if not has_header(pr)]
    wide = [pr for pr in candidates if has_header(pr)]
    if queue_count > 0:
        if wide and now:
            wide_by_age = sorted(wide, key=lambda pr: _oldest_boat_stamp(pr) or "9")
            oldest_stamp = _oldest_boat_stamp(wide_by_age[0])
            if oldest_stamp:
                try:
                    when = datetime.datetime.fromisoformat(oldest_stamp.replace("Z", "+00:00"))
                    if (now - when).total_seconds() > WIDE_STARVATION_SECONDS:
                        return [], []
                except ValueError:
                    pass
        return narrow, []
    if not wide:
        return narrow, []
    wide_by_age = sorted(wide, key=lambda pr: _oldest_boat_stamp(pr) or "9")
    oldest_wide = wide_by_age[0]
    oldest_stamp = _oldest_boat_stamp(oldest_wide)
    if narrow and now and oldest_stamp:
        try:
            when = datetime.datetime.fromisoformat(oldest_stamp.replace("Z", "+00:00"))
            if (now - when).total_seconds() > WIDE_STARVATION_SECONDS:
                return [], wide_by_age
        except ValueError:
            pass
    if narrow:
        return narrow, wide_by_age
    return [], wide_by_age


def active_pipeline_runs(api):
    """Return (total_runs, weighted_cost, breakdown_str) with per-workflow REST cost weights."""
    total = 0
    cost = 0
    parts = []
    for workflow in PIPELINE_WORKFLOWS:
        wf_count = 0
        for status in ("in_progress", "queued"):
            data = api.get(f"actions/workflows/{workflow}/runs?status={status}&per_page=1")
            wf_count += int((data or {}).get("total_count", 0))
        total += wf_count
        wf_cost = wf_count * INFLIGHT_COST[workflow]
        cost += wf_cost
        if wf_count:
            parts.append(f"{workflow}={wf_count}×{INFLIGHT_COST[workflow]}")
    breakdown = ", ".join(parts) if parts else "none"
    return total, cost, breakdown



def ratelimit_text(api, kind=None):
    """remaining/limit from the last rate-limit headers this token saw, optionally filtered by kind."""
    if kind == "graphql":
        remaining = getattr(api, "graphql_ratelimit_remaining", None)
        limit = getattr(api, "graphql_ratelimit_limit", None)
    elif kind == "rest":
        remaining = getattr(api, "rest_ratelimit_remaining", None)
        limit = getattr(api, "rest_ratelimit_limit", None)
    else:
        remaining = getattr(api, "ratelimit_remaining", None)
        limit = getattr(api, "ratelimit_limit", None)
    if remaining is None:
        return "not read"
    return f"{remaining}/{limit}" if limit is not None else f"{remaining}/unknown"


def _backfill_sort_key(item):
    """Sort backfill candidates: recheck first, then narrow refresh, then wide refresh, oldest-touched within each."""
    stamp, pr, kind = item
    if kind == "recheck":
        tier = 0
    elif kind == "refresh" and not has_header(pr):
        tier = 1
    else:
        tier = 2
    return (tier, stamp)


def _reserve_starving_wide_slot(candidates, now):
    """Promote the oldest starving wide refresh ahead of the last narrow refresh when narrow
    refreshes would otherwise take all MAX_BACKFILL slots."""
    starving_idx = None
    for i, (stamp, pr, kind) in enumerate(candidates):
        if kind == "refresh" and has_header(pr) and starving_idx is None:
            if stamp:
                try:
                    when = datetime.datetime.fromisoformat(stamp.replace("Z", "+00:00"))
                    if (now - when).total_seconds() > WIDE_STARVATION_SECONDS:
                        starving_idx = i
                except ValueError:
                    pass
    if starving_idx is None or starving_idx < MAX_BACKFILL:
        return
    last_narrow_idx = None
    for i in range(min(starving_idx, MAX_BACKFILL)):
        _, pr, kind = candidates[i]
        if kind == "refresh" and not has_header(pr):
            last_narrow_idx = i
    if last_narrow_idx is None:
        return
    item = candidates.pop(starving_idx)
    candidates.insert(last_narrow_idx, item)


def run(api=None, *, dispatch_api=None, config_path=None, env=None, now=None):
    env = os.environ if env is None else env
    now = now or datetime.datetime.now(datetime.timezone.utc)
    api = GitHubApi(env.get("BFME_QUEUE_TOKEN", "")) if api is None else api
    if dispatch_api is None:
        dispatch_token = env.get("BFME_DISPATCH_TOKEN", "")
        dispatch_api = GitHubApi(dispatch_token, token_name="BFME_DISPATCH_TOKEN") if dispatch_token else api
    approvers, refused_prefixes = load_config(config_path)
    main_sha, prs = list_open_prs(api)

    queued = [pr for pr in prs if pr.get("isInMergeQueue")]
    mq_count, mq_files, mq_has_wide = fetch_merge_queue(api)
    enqueue_paused = mq_count is None
    if not enqueue_paused:
        queue_files = mq_files
        queue_header = mq_has_wide
    else:
        queue_files = sum(file_count(pr) for pr in queued)
        queue_header = any(has_header(pr) for pr in queued)
        print("Merge queue occupancy unknown; pausing enqueue for this run.", file=sys.stderr)

    enqueue_candidates = []
    backfill_candidates = []
    held_back = 0
    for pr in prs:
        paths = base_eligible(pr, refused_prefixes)
        if paths is None:
            continue
        statuses = status_map(pr)
        kind = classify(pr, paths, statuses, approvers)
        if kind == "enqueue":
            enqueue_candidates.append(pr)
        elif kind is not None:
            stamp = touched(statuses)
            if not cooling(stamp, now, *backoff(statuses)):
                backfill_candidates.append((stamp, pr, kind))
            else:
                held_back += 1
    backfill_candidates.sort(key=_backfill_sort_key)
    if now:
        _reserve_starving_wide_slot(backfill_candidates, now)

    enqueued = []
    enqueue_errors = []
    narrow_prs, wide_fallback = ([], []) if enqueue_paused else wave(
        enqueue_candidates, queued, mq_count=mq_count, mq_has_wide=mq_has_wide, now=now)

    ambiguous_narrow_error = False
    for pr in narrow_prs:
        if len(enqueued) >= MAX_ENQUEUE:
            break
        if api.graphql_ratelimit_remaining is not None and api.graphql_ratelimit_remaining <= MIN_RATELIMIT_ENQUEUE:
            break
        count, header = file_count(pr), has_header(pr)
        if queue_files + count > MAX_FILES or queue_header:
            continue
        num = pr["number"]
        try:
            api.graphql(ENQUEUE_MUTATION, {"id": pr["id"], "oid": pr["headRefOid"]})
            enqueued.append(num)
            queue_files += count
            print(f"Enqueued PR #{num} ({pr['headRefOid'][:12]})")
        except ApiError as exc:
            enqueue_errors.append((num, str(exc)))
            if not str(exc).startswith("GraphQL error:"):
                ambiguous_narrow_error = True
            print(f"Skipped enqueue for PR #{num}: {exc}", file=sys.stderr)

    attempt_wide = True
    if not enqueued and wide_fallback and ambiguous_narrow_error:
        fresh_count, _, _ = fetch_merge_queue(api)
        if fresh_count is None or fresh_count > 0:
            attempt_wide = False
            print("Ambiguous narrow mutation error; merge queue may not be empty; "
                  "skipping wide fallback.", file=sys.stderr)

    if not enqueued and wide_fallback and attempt_wide:
        for pr in wide_fallback:
            if len(enqueued) >= 1:
                break
            if api.graphql_ratelimit_remaining is not None and api.graphql_ratelimit_remaining <= MIN_RATELIMIT_ENQUEUE:
                break
            count = file_count(pr)
            if queue_files + count > MAX_FILES or queue_header:
                continue
            num = pr["number"]
            try:
                api.graphql(ENQUEUE_MUTATION, {"id": pr["id"], "oid": pr["headRefOid"]})
                enqueued.append(num)
                queue_files += count
                queue_header = True
                print(f"Enqueued wide PR #{num} ({pr['headRefOid'][:12]})")
            except ApiError as exc:
                enqueue_errors.append((num, str(exc)))
                print(f"Skipped enqueue for wide PR #{num}: {exc}", file=sys.stderr)

    starvation_mode = (not narrow_prs and wide_fallback
                       and any(not has_header(pr) for pr in enqueue_candidates))
    if starvation_mode and not enqueued:
        all_definite = all(err.startswith("GraphQL error:") for _, err in enqueue_errors)
        if all_definite:
            for pr in enqueue_candidates:
                if has_header(pr):
                    continue
                if len(enqueued) >= MAX_ENQUEUE:
                    break
                if api.graphql_ratelimit_remaining is not None and api.graphql_ratelimit_remaining <= MIN_RATELIMIT_ENQUEUE:
                    break
                count = file_count(pr)
                if queue_files + count > MAX_FILES or queue_header:
                    continue
                num = pr["number"]
                try:
                    api.graphql(ENQUEUE_MUTATION, {"id": pr["id"], "oid": pr["headRefOid"]})
                    enqueued.append(num)
                    queue_files += count
                    print(f"Enqueued PR #{num} ({pr['headRefOid'][:12]})")
                except ApiError as exc:
                    enqueue_errors.append((num, str(exc)))
                    print(f"Skipped enqueue for PR #{num}: {exc}", file=sys.stderr)

    backfilled = []
    budget = None
    active = None
    inflight_cost = None
    effective_budget = None
    inflight_breakdown = None
    if (backfill_candidates and main_sha and HEX40.fullmatch(main_sha)
            and (api.graphql_ratelimit_remaining is None or api.graphql_ratelimit_remaining > MIN_RATELIMIT_BACKFILL)):
        try:
            budget = dispatch_api.core_remaining()
        except ApiError as exc:
            print(f"Skipping backfill; could not read the Actions token quota: {exc}", file=sys.stderr)
    spare = -1 if budget is None else budget - ACTIONS_TOKEN_RESERVE
    if budget is not None and spare < CALLS_PER_BACKFILL[backfill_candidates[0][2]]:
        print(f"Backfill paused: the Actions token has {budget} REST calls left "
              f"({ACTIONS_TOKEN_RESERVE} are kept for checks and landings).", file=sys.stderr)
    elif budget is not None:
        try:
            active, inflight_cost, inflight_breakdown = active_pipeline_runs(api)
        except ApiError as exc:
            print(f"Skipping backfill; could not inspect active pipeline runs: {exc}", file=sys.stderr)
        if active is not None:
            effective_budget = max(0, budget - inflight_cost)
            spare = effective_budget - ACTIONS_TOKEN_RESERVE
            slots = min(MAX_BACKFILL, max(0, MAX_ACTIVE_BOAT - active))
            if spare < CALLS_PER_BACKFILL[backfill_candidates[0][2]]:
                print(f"Backfill paused: the Actions token has {budget} REST calls left "
                      f"(-{inflight_cost} in-flight for {active} active runs [{inflight_breakdown}] "
                      f"= {effective_budget} effective; "
                      f"{ACTIONS_TOKEN_RESERVE} are kept for checks and landings).", file=sys.stderr)
                slots = 0
            wide_dispatched = 0
            for _, pr, kind in backfill_candidates[:slots]:
                if CALLS_PER_BACKFILL[kind] > spare:
                    break
                dispatch_rl = getattr(dispatch_api, "ratelimit_remaining", None)
                if ((api.graphql_ratelimit_remaining is not None and api.graphql_ratelimit_remaining <= MIN_RATELIMIT_BACKFILL)
                        or (dispatch_rl is not None and dispatch_rl <= ACTIONS_TOKEN_RESERVE)):
                    break
                is_wide_refresh = (kind == "refresh" and has_header(pr))
                if is_wide_refresh and wide_dispatched >= MAX_WIDE_BACKFILL:
                    continue
                num = pr["number"]
                oid = pr.get("headRefOid", "")
                if not HEX40.fullmatch(oid):
                    continue
                event = EVENTS[kind]
                try:
                    dispatch_api.post(f"statuses/{oid}", {
                        "state": "success", "context": MARKER,
                        "description": f"Requested {event} on main {main_sha[:12]}",
                    })
                    dispatch_api.post("dispatches", {
                        "event_type": event,
                        "client_payload": {"number": num, "head": oid, "base": main_sha},
                    })
                    backfilled.append(num)
                    spare -= CALLS_PER_BACKFILL[kind]
                    if is_wide_refresh:
                        wide_dispatched += 1
                    print(f"Dispatched {event} for PR #{num} ({oid[:12]})")
                except ApiError as exc:
                    print(f"Skipped backfill for PR #{num}: {exc}", file=sys.stderr)

    summary_path = env.get("GITHUB_STEP_SUMMARY")
    if summary_path:
        if budget is None:
            quota_line = f"- **Actions token REST calls left:** not read (reserve {ACTIONS_TOKEN_RESERVE})"
        elif effective_budget is None:
            quota_line = f"- **Actions token REST calls left:** {budget} (reserve {ACTIONS_TOKEN_RESERVE})"
        else:
            quota_line = (
                f"- **Actions token REST calls left:** {budget} "
                f"(-{inflight_cost} in-flight for {active} active runs [{inflight_breakdown}] "
                f"= {effective_budget} effective; reserve {ACTIONS_TOKEN_RESERVE})"
            )
        lines = [
            "### BFME II Merge Queue Enqueue",
            "",
            f"- **Open PRs scanned:** {len(prs)}",
            f"- **Enqueued:** {len(enqueued)} ({', '.join(f'#{n}' for n in enqueued) or 'none'})",
            f"- **Backfill waiting:** {len(backfill_candidates)}",
            f"- **Backfill cooling down:** {held_back}",
            f"- **Backfill dispatched:** {len(backfilled)} ({', '.join(f'#{n}' for n in backfilled) or 'none'})",
            quota_line,
            f"- **Queue token rate limit (GraphQL):** {ratelimit_text(api, 'graphql')}",
            f"- **Queue token rate limit (REST):** {ratelimit_text(api, 'rest')}",
            f"- **Actions token rate limit:** {ratelimit_text(dispatch_api, 'rest')}",
        ]
        if enqueue_paused:
            lines.append("- **Enqueue paused:** merge queue state uncertain")
        if enqueue_errors:
            lines.append(f"- **Enqueue rejections:** {len(enqueue_errors)}")
        with Path(summary_path).open("a", encoding="utf-8", newline="\n") as stream:
            stream.write("\n".join(lines) + "\n")

    return {"enqueued": enqueued, "backfilled": backfilled, "errors": enqueue_errors}


def main():
    run()


if __name__ == "__main__":
    main()
