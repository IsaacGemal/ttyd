#!/usr/bin/env bash
# Gather one agent's results for the merge, after the agent has stopped.
#
#   tools/agent_collect.sh BASE WS OUT SRC [link]
#
# Run by tools/wave_launch.py from BASE (never from the workspace) inside the collect sandbox: WS (the agent's
# workspace) is mounted read-only and OUT (a directory the agent never had) is the only writable place. Everything
# here treats WS as untrusted: files are copied only if they are regular files whose real path is inside WS, and
# diffs never follow links. Writes REPORT.md, final.c, lastgood.c, link.txt, other_changes.txt, include.patch, configure.patch,
# provenance.txt and audit.txt to OUT; events.jsonl and stderr.log are already there (the launcher's pipe wrote them).
# Exit status: that of tools/agent_audit.py (0 clean, anything else flagged).
set -u
BASE=$1; WS=$2; OUT=$3; SRC=$4; MODE=${5:-}
WS=$(realpath -e -- "$WS") && cd "$WS" || exit 3

# copy WS/$1 to OUT/$2 only if it is a regular file inside WS
take() {
  local p; p=$(realpath -e -- "$WS/$1" 2>/dev/null) || return 0
  case $p in "$WS"/*) ;; *) echo "$1: outside the workspace, not taken" >> "$OUT/collect.txt"; return 0;; esac
  [ -f "$p" ] && [ ! -L "$WS/$1" ] && cp -- "$p" "$OUT/$2"
}
take "$SRC" final.c
take REPORT.md REPORT.md
take lastgood.c lastgood.c
take link.txt link.txt

D="diff --no-dereference"
{ $D -rq "$BASE/src" src; $D -rq "$BASE/libs" libs; $D -rq "$BASE/rels" rels
  [ "$MODE" = link ] || $D -q "$BASE/configure.py" configure.py; } 2>&1 \
  | grep -v -F "$SRC differ" | grep -v -E 'rels/[^/ ]+/include[/:]' > "$OUT/other_changes.txt"
$D -u "$BASE/configure.py" configure.py > "$OUT/configure.patch"
# header additions (include/ and each REL's rels/<mod>/include/) as one patch (a/... -> b/...) for tools/merge_agent.sh
{ $D -ruN "$BASE/include" include; for d in rels/*/include; do [ -d "$d" ] && $D -ruN "$BASE/$d" "$d"; done; } \
  | sed -e "s#^--- $BASE/#--- a/#" -e 's#^+++ include/#+++ b/include/#' -e 's#^+++ rels/#+++ b/rels/#' > "$OUT/include.patch"

python3 "$BASE/tools/provenance.py" "$OUT/events.jsonl" "$WS" > "$OUT/provenance.txt" 2>&1 \
  || echo 'ERROR tools/provenance.py failed; check events.jsonl by hand' >> "$OUT/provenance.txt"
python3 "$BASE/tools/agent_audit.py" "$OUT/events.jsonl" "$WS" > "$OUT/audit.txt" 2>&1
