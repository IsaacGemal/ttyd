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
# Results must pass tools/collect_gate.py (ALLOW_UNAUDITED=<wave>/<tag> admits one folder from before the audit).
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

# only a clean, complete collect (tools/collect_gate.py: COLLECT-OK's manifest, no limits, a clean audit)
GATE=$(python3 tools/collect_gate.py "$D" 2>&1) || { echo "REJECT $TAG: $GATE"; exit 1; }

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
bad=$(git diff -U0 "$F" | grep -E '^\+\s*#\s*pragma' | grep -v -E 'pragma\s+(dont_inline|opt_propagation|opt_common_subs|pool_data|explicit_zero_data|force_active|opt_dead_assignments|optimize_for_size|section|opt_lifetimes|inline_max_total_size|inline_max_size|defer_codegen|opt_loop_invariants|inline_depth|push|pop)\b')
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
# objdiff never compares relocation targets by value, so a function can score 100% loading the wrong constant:
# any `value*` finding in this unit (tools/reloc_value_check.py) refuses the merge; `name` findings are shown only
# It fails closed: an exit status other than 0/1, a missing or unreadable JSON, a `skip` of this unit, or this unit not
# among the ones the checker selects (its objects not found) all mean the check didn't run, which refuses too.
RVJ=$(mktemp); RVE=$(mktemp)
python3 tools/reloc_value_check.py "$(basename "$UNIT")" --json "$RVJ" >/dev/null 2>"$RVE"; RVS=$?
RV=$(python3 - "$RVJ" "$RVE" "$RVS" "$UNIT" <<'PY' 2>&1
import json, os, sys
sys.path.insert(0, "tools")
jpath, epath, status, unit = sys.argv[1], sys.argv[2], int(sys.argv[3]), sys.argv[4]
mine = lambda n: n == unit or n.endswith("/" + unit)
def fail(why):
    print("FAIL " + why)
    sys.exit()
if status not in (0, 1):
    fail(f"reloc_value_check exited {status}: " + open(epath).read().strip()[-300:])
try:
    res = json.load(open(jpath))
except Exception as e:
    fail(f"no readable JSON from reloc_value_check ({e})")
skips = [l.strip() for l in open(epath) if l.startswith("skip ") and mine(l.split()[1].rstrip(":"))]
if skips:
    fail("it skipped this unit: " + skips[0])
import reloc_value_check as R
if not any(mine(n) for n, *_ in R.units(os.path.join(R.BUILD, "report.json"), [os.path.basename(unit)], True)):
    fail("this unit's objects weren't found, so nothing was checked")
for name, fs in res.items():
    if mine(name):
        for x in fs:
            print(("BLOCK " if x["kind"].startswith("value") else "note  ") + f"{x['fn']} +{x['off']:#x} {x['kind']}: original {x['target']} ours {x['ours']}")
PY
); rm -f "$RVJ" "$RVE"
[ -n "$RV" ] && echo "$RV"
if [ -n "$RV" ] && echo "$RV" | grep -qv '^\(BLOCK \|note  \)'; then  # a FAIL line or a traceback
  restore
  echo "REJECT $TAG: the value check did not run (tools/reloc_value_check.py):"; echo "$RV" | head -5; exit 1
fi
if echo "$RV" | grep -q '^BLOCK'; then
  restore
  echo "REJECT $TAG: loads a wrong constant (tools/reloc_value_check.py):"; echo "$RV" | grep '^BLOCK'; exit 1
fi
# near-miss functions: constants the original loads that ours never does, and the reverse (tools/reloc_value_check.py
# --partial). Advisory only: a legitimately computed or folded value (x - 150.0f vs x + -150.0f) is flagged too.
PV=$(timeout 300 python3 tools/reloc_value_check.py --partial "$(basename "$UNIT")" 2>/dev/null | grep -E '^\s+\S+ \+0x' | head -8)
if [ -n "$PV" ]; then echo "NOTE  $TAG: partial-function constant differences (advisory; check each is a spelling, not a wrong value):"; echo "$PV"; fi
# near-miss functions: struct stores and li constants only one side has, alignment-free (tools/store_sweep.py). Advisory.
SV=$(timeout 120 python3 tools/store_sweep.py "/$UNIT" 2>/dev/null | grep -E '^  \S+: ' | head -8)
if [ -n "$SV" ]; then echo "NOTE  $TAG: partial-function store/constant differences (advisory; a field the C never writes, or a wrong width or literal):"; echo "$SV"; fi

matched() { python3 -c "import json;print(json.load(open('build/ci/accepted.json'))['measures']['matched_functions'])"; }
n0=$(matched); python3 tools/ci.py --accept $FLAGS >/dev/null 2>&1; n1=$(matched)
if [ $NEW = 1 ]; then SUBJECT="$(basename $UNIT): new file, $((n1-n0)) functions matched"
elif [ $n1 = $n0 ]; then SUBJECT="$(basename $UNIT): changes toward linking, no change in functions matched"
else SUBJECT="$(basename $UNIT): match $((n1-n0)) more functions"; fi
PROV=""  # what the agent fetched or looked at outside its workspace (tools/provenance.py, run by the launcher)
# WARN on network evidence and reads outside the workspace; NOTE for the reference decomps (reading is allowed, copying
# is not). The sandbox's /tmp and ~/.cache are its own, and an area that isn't a real top-level folder is a sed or grep
# pattern provenance.py took for a path.
provclass() { awk '/^(fetch|clone) /{print "W " $0; next}
  /^path /{a=$0; sub(/^path +/, "", a); sub(/: .*/, "", a)
    if (a ~ /^\/home\/[^\/]+\/ttyd-refs(\/|$)/) print "N " $0
    else if (a == "/tmp" || a ~ /^\/home\/[^\/]+\/\.cache(\/|$)/) next
    else if (a ~ /^\/(home|mnt|srv|opt|media|root|data|var|etc)(\/|$)/) print "W " $0}' "$1"; }
if [ ! -f "$D/provenance.txt" ]; then echo "WARN $TAG: no provenance.txt; run tools/provenance.py on its events.jsonl by hand"
else PW=$(provclass "$D/provenance.txt" | sed -n 's/^W //p'); PN=$(provclass "$D/provenance.txt" | sed -n 's/^N //p')
  [ -n "$PW" ] && { echo "WARN $TAG: fetched or looked outside its workspace (check licenses):"; echo "$PW"; }
  [ -n "$PN" ] && { echo "NOTE  $TAG: read the reference decomps (fine; check nothing was copied):"; echo "$PN"; }
  PV2=$(printf '%s\n%s' "$PW" "$PN" | grep . | head -40)
  [ -n "$PV2" ] && PROV="Outside its workspace (tools/provenance.py):
$PV2"; fi
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
