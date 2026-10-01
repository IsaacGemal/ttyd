#include "battle/battle_event_default.h"
#include "battle/battle.h"
#include "battle/battle_information.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "battle/battle_audience.h"
#include "driver/effdrv.h"
#include "driver/npcdrv.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_cmd.h"
#include "battle/battle_ac_help.h"
#include "battle/battle_damage.h"
#include "eff/eff_fire.h"

//=== added by agent: local prototypes for undeclared externals ===

const f32 zero_8041c010 = 0.0f;
const f32 float_1_8041c014 = 1.0f;
const f32 float_0p4_8041c018 = 0.4f;
const f32 float_1p6_8041c01c = 1.6f;

USER_FUNC(_backfire) {
    BattleUnit* unit;
    BattleWork* wp = _battleWorkPointer;
    effFireData* data;
    s32 id;
    s32 result;

    id = evtGetValue(event, event->args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(wp, id);
    if (!unit) {
        return EVT_RETURN_DONE;
    }
    if (isFirstCall) {
        f32 x, y, z;

        BtlUnit_GetPos(unit, &x, &y, &z);
        event->userdata[0] = (s32)effFireEntry(0, 0, x, y, z, float_1_8041c014);
        event->userdata[1] = 0;
    }
    data = ((EffectEntry*)event->userdata[0])->userdata;
    event->userdata[1] = event->userdata[1] + 1;
    if (event->userdata[1] >= 60) {
        effDelete((EffectEntry*)event->userdata[0]);
        BattleDamageDirect(-5, unit, 0, 1, 0, 0x118, 0, 1);
        result = BattleTransID(event, event->unitId);
        if (result != id) {
            return EVT_RETURN_DONE;
        }
        return 0xFF;
    }
    data->scale = float_1p6_8041c01c * sinfd((f32)(event->userdata[1] * 3)) + float_0p4_8041c018;
    return EVT_RETURN_BLOCK;
}

static s32 _anime_load(EventEntry* event, BOOL isFirstCall) {
    BattlePartyAnimeLoad(*(s32*)((u8*)_battleWorkPointer + 0x1C68));
    return EVT_RETURN_DONE;
}

static s32 _anime_load_wait(EventEntry* event, BOOL isFirstCall) {
    return BattlePartyAnimeLoad(*(s32*)((u8*)_battleWorkPointer + 0x1C68)) ? EVT_RETURN_DONE
                                                                            : EVT_RETURN_BLOCK;
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

USER_FUNC(btlevtcmd_audience_code_escape) {
    BattleAudience_Case_Escape();
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_audience_code_escape_success) {
    BattleAudience_Case_Escape_Good();
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_audience_code_escape_fail) {
    BattleAudience_Case_Escape_Bad();
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

USER_FUNC(_set_escape_ac_hlp_msg) {
    BattleAcHelpSetHelp("msg_ac_escape");
    BattleAcHelpSetDispType(2);
    return EVT_RETURN_DONE;
}

USER_FUNC(_get_escape_rate) {
    BattleWork* wp = _battleWorkPointer;
    FieldBattleInfo* info = wp->fbatInfo;

    evtSetValue(event, event->args[0],
                *(u8*)((u8*)info->setup->groupSetup->enemyData->mUnitKindParams + 0x13));
    return EVT_RETURN_DONE;
}

// MUST_MATCH: reading args[0] through a volatile pointer forces MWCC to emit the
// args[0] load before the callee-saved args[1] load, as the original does.
// event->args is plain s32*, nothing here is actually volatile.
USER_FUNC(_get_heart_suitoru_point) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32* args = event->args;
    s32 arg0;
    s32 arg1;
    s32 value;

#ifdef MUST_MATCH
    arg0 = *(volatile s32*)&args[0];
#else
    arg0 = args[0];
#endif
    arg1 = args[1];

    unit = BattleGetUnitPtr(wp, BattleTransID(event, arg0));
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

// MUST_MATCH: reading args[0] through a volatile pointer forces MWCC to emit the
// args[0] load before the callee-saved args[1] load, as the original does.
// event->args is plain s32*, nothing here is actually volatile.
USER_FUNC(_get_flower_suitoru_point) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32* args = event->args;
    s32 arg0;
    s32 arg1;
    s32 value;

#ifdef MUST_MATCH
    arg0 = *(volatile s32*)&args[0];
#else
    arg0 = args[0];
#endif
    arg1 = args[1];

    unit = BattleGetUnitPtr(wp, BattleTransID(event, arg0));
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
