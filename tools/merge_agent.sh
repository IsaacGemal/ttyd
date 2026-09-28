#!/usr/bin/env bash
# Merge one agent's result into the current branch: apply it, run tools/ci.py and tools/lint_patch.py, commit on
# pass, then try to link the file with tools/autolink.py.
#
#   tools/merge_agent.sh RESULTS_DIR UNIT [AGENT_MODEL] [AGENT_TRAILER]
#
# UNIT may also be the source file's path, e.g. rels/pik/src/unit/unit_purple_teresa.c or a library's
# libs/metrowerks/src/MSL_C.PPCEABI.bare.H.a/printf.c: needed for a new file in
# a REL module, and for a unit that exists in several RELs.
# RESULTS_DIR is the agent's result folder: final.c (the agent's whole file; used for new files and finishers)
# or src.patch (a diff against the file on main), plus other_changes.txt (source files the agent changed besides
# its own) and include.patch (what the agent added to headers; tools/header_patch.py checks it only adds
# declarations and type definitions).
# UNIT is the source path without src/ and .c, e.g. event/evt_map. The wave name in the commit is RESULTS_DIR's
# parent folder name. ORCHESTRATOR (env) names the model doing the merge in the commit trailer.
# Prints MERGED, REJECT or SKIP. Run on a clean tree.
set -u
D=$(cd "$1" && pwd); UNIT=${2#./}
MODEL=${3:-"Pi + DeepSeek V4.1 Flash"}; TRAILER=${4:-"DeepSeek V4.1 Flash <noreply@deepseek.com>"}
ORCH=${ORCHESTRATOR:-"Claude Opus 5.5 <noreply@anthropic.com>"}
TAG=$(basename "$D"); WAVE=$(basename "$(dirname "$D")"); TOOLS=$(cd "$(dirname "$0")" && pwd)
cd "$TOOLS/.."
case $UNIT in /*) UNIT=${UNIT#"$PWD/"} ;; esac
case $UNIT in
  *.c|*.C) F=$UNIT; UNIT=$(echo "$F" | sed -E 's#^(rels/[^/]+/|libs/[^/]+/)?src/##; s#\.[cC]$##') ;;
  *) F=src/$UNIT.c
     [ -e "$F" ] || for c in libs/*/src/$UNIT.[cC] rels/*/src/$UNIT.c; do [ -e "$c" ] && F=$c; done ;;
esac

[ -z "$(git status --short -- src include rels libs)" ] || { echo "SKIP $TAG: tree not clean"; exit 1; }
# how `diff -rq` lists the agent's own new file: under its nearest folder that already exists
top=$F; while [ ! -d "$(dirname "$top")" ]; do top=$(dirname "$top"); done
own="Only in $(dirname "$top"): $(basename "$top")"
if [ -s "$D/other_changes.txt" ] && grep -v -x -F "$own" "$D/other_changes.txt" | grep -q .; then
  echo "REJECT $TAG: touched other files:"; cat "$D/other_changes.txt"; exit 1
fi

NEW=0
if [ -s "$D/src.patch" ]; then
  patch -s -p0 "$F" < "$D/src.patch" || { git checkout -q "$F"; echo "REJECT $TAG: patch failed"; exit 1; }
elif [ -s "$D/final.c" ]; then
  [ -e "$F" ] || NEW=1
  mkdir -p "$(dirname "$F")"; cp "$D/final.c" "$F"
  [ $NEW = 1 ] && git add -N "$F"
else
  echo "REJECT $TAG: no changes"; exit 1
fi
HDR=0; HDRS=""
if [ -s "$D/include.patch" ]; then
  BAD=$(python3 tools/header_patch.py "$D/include.patch") || {
    if [ $NEW = 1 ]; then git rm -q --cached "$F" 2>/dev/null; rm -f "$F"; else git checkout -q "$F"; fi
    echo "REJECT $TAG: header changes other than added declarations and type definitions:"; echo "$BAD"; exit 1; }
  # append-only: each added line goes to the end of its header unless the header already has it
  APPLIED=$(python3 tools/header_patch.py --apply "$D/include.patch") || {
    git checkout -q -- include; git ls-files --others --exclude-standard include | xargs rm -f
    if [ $NEW = 1 ]; then git rm -q --cached "$F" 2>/dev/null; rm -f "$F"; else git checkout -q "$F"; fi
    echo "REJECT $TAG: header patch failed"; exit 1; }
  HDRS=$(echo "$APPLIED" | cut -f2); HDRLINES=$(echo "$APPLIED" | awk -F'\t' '{s+=$1} END{print s+0}')
  HDR=1
fi
[ -z "$(git status --short -- "$F" include)" ] && { echo "SKIP $TAG: no change to $F"; exit 1; }
python3 configure.py >/dev/null   # a new source file needs a build rule

restore() {
  if [ $HDR = 1 ]; then git checkout -q -- include; git ls-files --others --exclude-standard include | xargs rm -f; fi  # tree was clean before the merge
  if [ $NEW = 1 ]; then git rm -q --cached "$F" 2>/dev/null; rm -f "$F"; else git checkout -q "$F"; fi
  python3 configure.py >/dev/null
}

FLAGS=""
bad=$(git diff -U0 "$F" | grep -E '^\+\s*#\s*pragma' | grep -v -E 'pragma\s+(dont_inline|opt_propagation|opt_common_subs|pool_data|explicit_zero_data|optimize_for_size|inline_depth|push|pop)\b')
[ -n "$bad" ] && { restore; echo "REJECT $TAG: unfamiliar pragma: $bad"; exit 1; }
git diff -U0 "$F" | grep -q -E '^\+\s*#\s*pragma' && FLAGS="--allow-pragma"

OUT=$(python3 tools/ci.py $FLAGS 2>&1)
echo "$OUT" | tail -9
echo "$OUT" | grep -q '^PASS' || { restore; echo "REJECT $TAG"; exit 1; }
LINT=$(python3 tools/lint_patch.py "$F" 2>&1) || {
  restore
  if echo "$LINT" | grep -q '^STUB'; then echo "REJECT $TAG: empty stubs:"; else echo "REJECT $TAG: lint failed:"; fi
  echo "$LINT"; exit 1; }
[ -n "$LINT" ] && { echo "$LINT"; echo "$LINT" > "$D/lint.txt"; }
if [ -n "$HDRS" ]; then  # main reports no problems, so anything here came from this agent's headers; check it in the asm
  PA=$(python3 tools/proto_asm.py) || { echo "WARN $TAG: proto_asm.py findings (check by hand):"; echo "$PA" | tee "$D/proto_asm.txt"; }
fi

matched() { python3 -c "import json;print(json.load(open('build/ci/accepted.json'))['measures']['matched_functions'])"; }
n0=$(matched); python3 tools/ci.py --accept $FLAGS >/dev/null 2>&1; n1=$(matched)
if [ $NEW = 1 ]; then SUBJECT="$(basename $UNIT): new file, $((n1-n0)) functions matched"
elif [ $n1 = $n0 ]; then SUBJECT="$(basename $UNIT): changes toward linking, no change in functions matched"
else SUBJECT="$(basename $UNIT): match $((n1-n0)) more functions"; fi
PROV=""  # what the agent fetched or looked at outside its workspace (tools/provenance.py, run by the launcher)
if [ ! -f "$D/provenance.txt" ]; then echo "WARN $TAG: no provenance.txt; run tools/provenance.py on its events.jsonl by hand"
elif [ -s "$D/provenance.txt" ]; then echo "WARN $TAG: fetched or looked outside its workspace (check licenses):"; cat "$D/provenance.txt"
  PROV="Outside its workspace (tools/provenance.py):
$(head -40 "$D/provenance.txt")"; [ $(grep -c . "$D/provenance.txt") -gt 40 ] && PROV="$PROV
... $(( $(grep -c . "$D/provenance.txt") - 40 )) more lines"; fi
HDRNOTE=""
[ -n "$HDRS" ] && HDRNOTE="Header additions (tools/header_patch.py: declarations and type definitions): $HDRLINES lines."
git add "$F" $HDRS && git commit -q -F - <<MSG
$SUBJECT

tools/ci.py${FLAGS:+ $FLAGS}: PASS, no regressions; $n0 -> $n1 functions matched.
$HDRNOTE
$PROV

Agent: $MODEL, TTYD $WAVE; verified and merged by the orchestrator.

Co-Authored-By: $TRAILER
Co-Authored-By: $ORCH
MSG
echo "MERGED $TAG: $n0 -> $n1 ($(git log --oneline -1))"
# likely wrong code left in partly matched functions, for the review (tools/semcheck.py; leads, not verdicts)
SEMUNIT=$UNIT; case $F in rels/*) SEMUNIT="$(echo "$F" | cut -d/ -f2)/$UNIT" ;; esac  # a REL unit names its module
if python3 tools/semcheck.py "$SEMUNIT" > "$D/semcheck.txt" 2>&1; then
  echo "SEMCHECK $TAG: $(tail -1 "$D/semcheck.txt") -> $D/semcheck.txt"
else echo "WARN $TAG: tools/semcheck.py failed: $(tail -1 "$D/semcheck.txt")"; fi

# Link the file if this merge finished it (tools/autolink.py keeps the flip only if the checksums pass).
if python3 tools/autolink.py --list | grep -qx "$UNIT.c"; then
  L=$(python3 tools/autolink.py "$UNIT.c" | head -1); echo "$L"
  if echo "$L" | grep -q '^LINKED'; then
    python3 tools/ci.py --accept >/dev/null 2>&1
    git add configure.py && git commit -q -F - <<MSG
Link $(basename $UNIT)

tools/autolink.py: the file is fully matched and the build's checksums still pass with it linked.

Co-Authored-By: $ORCH
MSG
    echo "LINKED $TAG"
  fi
fi
