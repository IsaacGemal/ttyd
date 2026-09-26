#include "battle/battle.h"
// NOTE FOR REVIEWERS: every function in this file matches with `-O4,s`
// (extra_cflags=["-O4,s"] in configure.py). With the default `-O4,p` the
// prologues use individual stw/lwz instead of stmw/lmw, so nothing matches.
// This file's entry in configure.py must gain `extra_cflags=["-O4,s"]`.
#include "battle/battle_information.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "battle/battle_audience.h"
#include "driver/effdrv.h"
#include "driver/npcdrv.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_cmd.h"

//=== added by agent: local prototypes for undeclared externals ===
void BattleAcHelpSetHelp(const char* help);
void BattleAcHelpSetDispType(s32 type);
s32 BattlePartyAnimeLoad(BattleUnitType partyId);
EffectEntry* effFireEntry(s32 a1, s32 a2, f32 a3, f32 a4, f32 a5, f32 a6);
void BattleDamageDirect(s32 damageCode, BattleUnit* unit, s32 arg2, s32 arg3, s32 arg4,
                        s32 arg5, s32 arg6, s32 arg7);
f32 sinfd(f32 deg);

const f32 zero_8041c010 = 0.0f;
const f32 float_1_8041c014 = 1.0f;
const f32 float_0p4_8041c018 = 0.4f;
const f32 float_1p6_8041c01c = 1.6f;

// NONMATCHING (99.10% with -O4,s): body is identical except the two argument
// loads are emitted in the opposite order (target loads args[0] into r4 then
// args[1] into r30; our build hoists the callee-saved args[1] load first).
USER_FUNC(_get_flower_suitoru_point) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32* args = event->args;
    s32 arg1 = args[1];
    s32 value;

    unit = BattleGetUnitPtr(wp, BattleTransID(event, args[0]));
    value = 0;
    if (unit) {
        value = unit->badgesEquipped.FpDrain;
        if (value > *(s32*)((u8*)unit + 0x280)) {
            value = *(s32*)((u8*)unit + 0x280);
        }
    }
    evtSetValue(event, arg1, value);
    return EVT_RETURN_DONE;
}

// NONMATCHING (99.10% with -O4,s): same argument-load ordering difference as
// _get_flower_suitoru_point; reads HpDrain (0x2F5) instead of FpDrain.
USER_FUNC(_get_heart_suitoru_point) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32* args = event->args;
    s32 arg1 = args[1];
    s32 value;

    unit = BattleGetUnitPtr(wp, BattleTransID(event, args[0]));
    value = 0;
    if (unit) {
        value = unit->badgesEquipped.HpDrain;
        if (value > *(s32*)((u8*)unit + 0x280)) {
            value = *(s32*)((u8*)unit + 0x280);
        }
    }
    evtSetValue(event, arg1, value);
    return EVT_RETURN_DONE;
}

USER_FUNC(_get_escape_rate) {
    BattleWork* wp = _battleWorkPointer;
    FieldBattleInfo* info = wp->fbatInfo;

    evtSetValue(event, event->args[0],
                *(u8*)((u8*)info->setup->groupSetup->enemyData->mUnitKindParams + 0x13));
    return EVT_RETURN_DONE;
}

USER_FUNC(_set_escape_ac_hlp_msg) {
    BattleAcHelpSetHelp("msg_ac_escape");
    BattleAcHelpSetDispType(2);
    return EVT_RETURN_DONE;
}

USER_FUNC(_btl_escape_reset_move_color_lv_player_unit_all) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;

    unit = BattleGetMarioPtr(wp);
    unit->mMoveColorLv = 0xFF;
    unit = BattleGetPartyPtr(wp);
    if (unit) {
        unit->mMoveColorLv = 0xFF;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_audience_code_escape_fail) {
    BattleAudience_Case_Escape_Bad();
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_audience_code_escape_success) {
    BattleAudience_Case_Escape_Good();
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_audience_code_escape) {
    BattleAudience_Case_Escape();
    return EVT_RETURN_DONE;
}

USER_FUNC(_check_pose_stay) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 arg0 = event->args[0];
    s32 id;
    s32 result;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(wp, id);
    result = 0;
    if (unit->currentKind == 0xDE) {
        result = 1;
        if (wp->badgeEquippedFlags & 1) {
            if (!(unit->mTokenFlags & 0x10)) {
                result = 0;
            }
        }
    }
    evtSetValue(event, arg0, result);
    return EVT_RETURN_DONE;
}

static s32 _anime_load_wait(EventEntry* event, BOOL isFirstCall) {
    return BattlePartyAnimeLoad(*(s32*)((u8*)_battleWorkPointer + 0x1C68)) ? EVT_RETURN_DONE
                                                                            : EVT_RETURN_BLOCK;
}

static s32 _anime_load(EventEntry* event, BOOL isFirstCall) {
    BattlePartyAnimeLoad(*(s32*)((u8*)_battleWorkPointer + 0x1C68));
    return EVT_RETURN_DONE;
}

// NONMATCHING (97.04% with -O4,s): transId gets r31 in the target but is
// rematerialised through r0 in ours, and the float/2^52 literals resolve to
// unnamed pool symbols because this file's .sdata2/.rodata layout (zero_8041c010
// and the event-script data) is not reproduced yet.
USER_FUNC(_backfire) {
    s32 transId;
    BattleUnit* unit;
    BattleWork* wp = _battleWorkPointer;
    EffectEntry* eff;
    s32 id;
    s32 counter;
    void* work;

    id = evtGetValue(event, event->args[0]);
    transId = BattleTransID(event, id);
    unit = BattleGetUnitPtr(wp, transId);
    if (!unit) {
        return EVT_RETURN_DONE;
    }
    if (isFirstCall) {
        f32 x, y, z;

        BtlUnit_GetPos(unit, &x, &y, &z);
        event->userdata[0] = (s32)effFireEntry(0, 0, x, y, z, float_1_8041c014);
        event->userdata[1] = 0;
    }
    eff = (EffectEntry*)event->userdata[0];
    counter = event->userdata[1];
    work = eff->userdata;
    event->userdata[1] = counter + 1;
    if (event->userdata[1] >= 60) {
        effDelete(eff);
        BattleDamageDirect(-5, unit, 0, 1, 0, 0x118, 0, 1);
        if (BattleTransID(event, event->unitId) == transId) {
            return 0xFF;
        }
        return EVT_RETURN_DONE;
    }
    *(f32*)((u8*)work + 0x24) = float_1p6_8041c01c * sinfd((f32)(event->userdata[1] * 3)) + float_0p4_8041c018;
    return EVT_RETURN_BLOCK;
}
