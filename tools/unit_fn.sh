#!/bin/bash
# usage: tools/unit_fn.sh unit/unit_x FUNC BODY.c MOD [MOD ...]  (MOD list = every REL with the unit; the first one holds the rel_gen source)   insert BODY at rel_gen's hint, add includes, share, configure
set -e
unit=$1; fn=$2; body=$3; first=$4; shift 3
src=rels/$first/src/$unit.c
python3 tools/fn_hint.py $src $fn $body
sed -i '' '1i\
#include "battle/battle.h"\
#include "battle/battle_sub.h"\
#include "manager/evtmgr_cmd.h"
' $src
python3 tools/share_unit.py $unit "$@"
python3 configure.py >/dev/null 2>&1
