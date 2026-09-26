#!/usr/bin/env bash
# Merge one agent's result into the current branch: apply it, run tools/ci.py and tools/lint_patch.py, commit on
# pass, then try to link the file with tools/autolink.py.
#
#   tools/merge_agent.sh RESULTS_DIR UNIT [AGENT_MODEL] [AGENT_TRAILER]
#
# RESULTS_DIR is the agent's result folder: final.c (the agent's whole file; used for new files and finishers)
# or src.patch (a diff against the file on main), plus other_changes.txt (files the agent changed besides its own).
# UNIT is the source path without src/ and .c, e.g. event/evt_map. The wave name in the commit is RESULTS_DIR's
# parent folder name. ORCHESTRATOR (env) names the model doing the merge in the commit trailer.
# Prints MERGED, REJECT or SKIP. Run on a clean tree.
set -u
D=$(cd "$1" && pwd); UNIT=$2
MODEL=${3:-"Pi + DeepSeek V4.1 Flash"}; TRAILER=${4:-"DeepSeek V4.1 Flash <noreply@deepseek.com>"}
ORCH=${ORCHESTRATOR:-"Claude Opus 5.5 <noreply@anthropic.com>"}
TAG=$(basename "$D"); WAVE=$(basename "$(dirname "$D")"); F=src/$UNIT.c; TOOLS=$(cd "$(dirname "$0")" && pwd)
cd "$TOOLS/.."

[ -z "$(git status --short -- src include)" ] || { echo "SKIP $TAG: tree not clean"; exit 1; }
own="Only in src$( [ "$(dirname $UNIT)" = . ] || echo /$(dirname $UNIT)): $(basename $UNIT).c"
if [ -s "$D/other_changes.txt" ] && grep -v -x -F "$own" "$D/other_changes.txt" | grep -q .; then
  echo "REJECT $TAG: touched other files:"; cat "$D/other_changes.txt"; exit 1
fi

NEW=0
if [ -s "$D/src.patch" ]; then
  patch -s -p0 "$F" < "$D/src.patch" || { git checkout -q "$F"; echo "REJECT $TAG: patch failed"; exit 1; }
elif [ -s "$D/final.c" ]; then
  [ -e "$F" ] || NEW=1
  cp "$D/final.c" "$F"
  [ $NEW = 1 ] && git add -N "$F"
else
  echo "REJECT $TAG: no changes"; exit 1
fi
[ -z "$(git status --short -- "$F")" ] && { echo "SKIP $TAG: no change to $F"; exit 1; }
python3 configure.py >/dev/null   # a new source file needs a build rule

restore() {
  if [ $NEW = 1 ]; then git rm -q --cached "$F" 2>/dev/null; rm -f "$F"; else git checkout -q "$F"; fi
  python3 configure.py >/dev/null
}

FLAGS=""
bad=$(git diff -U0 "$F" | grep -E '^\+\s*#\s*pragma' | grep -v -E 'pragma\s+(dont_inline|opt_propagation|pool_data|explicit_zero_data|optimize_for_size|inline_depth|push|pop)\b')
[ -n "$bad" ] && { restore; echo "REJECT $TAG: unfamiliar pragma: $bad"; exit 1; }
git diff -U0 "$F" | grep -q -E '^\+\s*#\s*pragma' && FLAGS="--allow-pragma"

OUT=$(python3 tools/ci.py $FLAGS 2>&1)
echo "$OUT" | tail -9
echo "$OUT" | grep -q '^PASS' || { restore; echo "REJECT $TAG"; exit 1; }
LINT=$(python3 tools/lint_patch.py "$UNIT") || { restore; echo "REJECT $TAG: empty stubs:"; echo "$LINT"; exit 1; }
[ -n "$LINT" ] && { echo "$LINT"; echo "$LINT" > "$D/lint.txt"; }

matched() { python3 -c "import json;print(json.load(open('build/ci/accepted.json'))['measures']['matched_functions'])"; }
n0=$(matched); python3 tools/ci.py --accept $FLAGS >/dev/null 2>&1; n1=$(matched)
if [ $NEW = 1 ]; then SUBJECT="$(basename $UNIT): new file, $((n1-n0)) functions matched"
else SUBJECT="$(basename $UNIT): match $((n1-n0)) more functions"; fi
git add "$F" && git commit -q -F - <<MSG
$SUBJECT

tools/ci.py${FLAGS:+ $FLAGS}: PASS, no regressions; $n0 -> $n1 functions matched.

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
