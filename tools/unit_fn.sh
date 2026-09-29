#!/bin/bash
# usage: [INC="dir/a.h dir/b.h"] tools/unit_fn.sh unit/unit_x FUNC BODY.c MOD [MOD ...]
# MOD list = every REL that has the unit (the first holds rel_gen's source). Inserts BODY at rel_gen's hint, adds the
# battle/evt includes (plus $INC), moves the source to rels/common, registers the Objects and reconfigures.
set -e
unit=$1; fn=$2; body=$3; first=$4; shift 3
src=rels/$first/src/$unit.c
python3 tools/fn_hint.py $src $fn $body
inc='#include "battle/battle.h"\n#include "battle/battle_sub.h"\n#include "manager/evtmgr_cmd.h"\n'
for h in $INC; do inc="$inc#include \"$h\"\n"; done
python3 - "$src" "$inc" <<'P'
import sys
p, inc = sys.argv[1], sys.argv[2].replace('\\n', '\n')
open(p, 'w').write(inc + open(p).read())
P
python3 tools/share_unit.py $unit "$@"
python3 configure.py >/dev/null 2>&1
