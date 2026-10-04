#!/usr/bin/env bash
# Gather one agent's results for the merge, after the agent has stopped.
#
#   tools/agent_collect.sh BASE WS OUT SRC [MODE]
#
# Run by tools/wave_launch.py from BASE (never from the workspace) inside the collect sandbox: WS (the agent's
# workspace) is mounted read-only and OUT (a directory the agent never had) is the only writable place. Everything
# here treats WS as untrusted: files are copied only if they are regular files whose real path is inside WS and no
# larger than 16 MB, and diffs never follow links. Writes REPORT.md, final.c, lastgood.c, link.txt,
# other_changes.txt, include.patch, configure.patch, provenance.txt and audit.txt to OUT (events.jsonl and
# stderr.log and passes.txt are already there: the host wrote them), problems to collect.txt, and COLLECT-OK last if
# every step worked and no limit tripped (no limits.txt): a manifest (sha256, size, name) of every file the launcher
# copies back, which tools/collect_gate.py checks before a merge. MODE link, dataport or oneshort: the agent may flip its unit in configure.py (configure.patch carries
# it), so configure.py isn't listed as another file changed.
# Exit status: the audit's if it flagged (or failed), else 5 if a collect step failed, 6 if a limit tripped, else 0.
set -u
BASE=$1; WS=$2; OUT=$3; SRC=$4; MODE=${5:-}
FAIL=0
: > "$OUT/collect.txt"  # first: a collect killed from here on still leaves its trace (the gate refuses the folder)
note() { echo "$*" >> "$OUT/collect.txt"; FAIL=1; }
WS=$(realpath -e -- "$WS") && cd "$WS" || { note "workspace missing"; exit 5; }

# copy WS/$1 to OUT/$2 only if it is a regular file inside WS, not a link, at most 16 MB
take() {
  [ -e "$WS/$1" ] || return 0
  local p; p=$(realpath -e -- "$WS/$1" 2>/dev/null) || { note "$1: unresolvable, not taken"; return; }
  case $p in "$WS"/*) ;; *) note "$1: outside the workspace, not taken"; return;; esac
  if [ -L "$WS/$1" ] || [ ! -f "$p" ]; then note "$1: not a regular file, not taken"; return; fi
  [ "$(stat -c %s -- "$p")" -le 16777216 ] || { note "$1: over 16 MB, not taken"; return; }
  cp -- "$p" "$OUT/$2" || note "$1: copy failed"
}
take "$SRC" final.c
take REPORT.md REPORT.md
take lastgood.c lastgood.c
take link.txt link.txt

D="diff --no-dereference"
# diff exits 1 for "different" (expected) and 2 for trouble
run_diff() { "$@"; [ $? -le 1 ] || note "diff failed: $*"; }
{ run_diff $D -rq "$BASE/src" src; run_diff $D -rq "$BASE/libs" libs; run_diff $D -rq "$BASE/rels" rels
  case $MODE in link|dataport|oneshort) ;; *) run_diff $D -q "$BASE/configure.py" configure.py;; esac; } 2>&1 \
  | grep -v -F "$SRC differ" | grep -v -E 'rels/[^/ ]+/include[/:]' > "$OUT/other_changes.txt"
run_diff $D -u "$BASE/configure.py" configure.py > "$OUT/configure.patch"
# header additions (include/ and each REL's rels/<mod>/include/) as one patch (a/... -> b/...) for tools/merge_agent.sh
{ run_diff $D -ruN "$BASE/include" include; for d in rels/*/include; do [ -d "$d" ] && run_diff $D -ruN "$BASE/$d" "$d"; done; } \
  | sed -e "s#^--- $BASE/#--- a/#" -e 's#^+++ include/#+++ b/include/#' -e 's#^+++ rels/#+++ b/rels/#' > "$OUT/include.patch" \
  || note "include.patch: sed failed"
grep -q '^diff failed' "$OUT/collect.txt" 2>/dev/null && FAIL=1

python3 "$BASE/tools/provenance.py" "$OUT/events.jsonl" "$WS" > "$OUT/provenance.txt" 2>&1 \
  || { echo 'ERROR tools/provenance.py failed; check events.jsonl by hand' >> "$OUT/provenance.txt"; note "provenance failed"; }
# The audit reads the host's sibling passes.txt; the agent's stream cannot supply trusted pass boundaries.
python3 "$BASE/tools/agent_audit.py" "$OUT/events.jsonl" "$WS" "$BASE" > "$OUT/audit.txt" 2>&1
A=$?
[ $A -ne 0 ] && exit $A
[ $FAIL -ne 0 ] && exit 5
[ -e "$OUT/limits.txt" ] && exit 6
# the manifest, written last: every file the launcher copies back (all but events.jsonl), with its hash and size
cd "$OUT" || exit 5
for f in *; do
  case $f in events.jsonl|COLLECT-OK*) continue;; esac
  [ -f "$f" ] && [ ! -L "$f" ] || { note "$f: not a regular file in the output dir"; exit 5; }
  echo "$(sha256sum < "$f" | cut -d' ' -f1) $(stat -c %s -- "$f") $f"
done > COLLECT-OK.tmp || exit 5
mv COLLECT-OK.tmp COLLECT-OK
exit 0
