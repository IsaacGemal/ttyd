#include "battle/battle.h"
#include "battle/battle_information.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "driver/effdrv.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_cmd.h"

//=== added by agent: local prototypes for undeclared externals ===
EffectEntry* effWhirlwindN64Entry(s32 a1, s32 a2, s32 a3, f32 a4);

USER_FUNC(_binta_effect) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    EffectEntry* effect;
    s32 value0, value1;
    Vec pos;

    value0 = evtGetValue(event, args[0]);
    value1 = evtGetValue(event, args[1]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, value0));
    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    effect = effWhirlwindN64Entry((u32)value1 >> 31, 0, 0x5A, unit->mSizeMultiplier);
    *(f32*)((u8*)effect->userdata + 8) = pos.x;
    *(f32*)((u8*)effect->userdata + 0xC) = pos.y;
    *(f32*)((u8*)effect->userdata + 0x10) = pos.z;
    return EVT_RETURN_DONE;
}

USER_FUNC(_check_through_pos) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 value0, value1, value2, value3;
    s32 out;
    s32 min, max;

    value0 = evtGetValue(event, args[0]);
    value1 = evtGetValue(event, args[1]);
    value2 = evtGetValue(event, args[2]);
    value3 = evtGetValue(event, args[3]);
    out = args[4];
    unit = BattleGetUnitPtr(wp, BattleTransID(event, value0));
    if (value2 > value3) {
        max = value2;
        min = value3;
    } else {
        max = value3;
        min = value2;
    }
    if (value1 >= min && value1 <= max) {
        evtSetValue(event, out, 1);
    } else {
        evtSetValue(event, out, 0);
        if (unit->faceDirection >= 0) {
            if (value1 > max) {
                evtSetValue(event, out, 1);
            }
        } else {
            if (value1 < min) {
                evtSetValue(event, out, 1);
            }
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(unk_8010ee30) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 value;

    value = evtGetValue(event, event->args[0]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, value));
    if (unit) {
        unit->hpGauge.field_0x2 |= 1;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(_add_star_point_disp_offset) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32* args = event->args;
    s32 value;
    f32 x, y, zf;
    s32 arg1, arg2, arg3;

    value = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, value));
    arg1 = args[1];
    x = (f32)evtGetValue(event, arg1);
    arg2 = args[2];
    y = (f32)evtGetValue(event, arg2);
    arg3 = args[3];
    zf = (f32)evtGetValue(event, arg3);
    // BattleUnitKind::field_0x10 spans 0x10..0xB8, so the display-offset Vec at
    // 0x90 is not declared in the header and is read raw.
    x += *(f32*)((u8*)unit->mKindParams + 0x90);
    y += *(f32*)((u8*)unit->mKindParams + 0x94);
    zf += *(f32*)((u8*)unit->mKindParams + 0x98);
    evtSetValue(event, arg1, (s32)x);
    evtSetValue(event, arg2, (s32)y);
    evtSetValue(event, arg3, (s32)zf);
    return EVT_RETURN_DONE;
}

USER_FUNC(_check_at_dm_event_wait) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 value;

    value = evtGetValue(event, event->args[0]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, value));
    if (!unit) {
        return EVT_RETURN_DONE;
    }
    if (unit->attackEventId != 0 && evtCheckID(unit->attackEventId) != 0) {
        return EVT_RETURN_BLOCK;
    }
    if (unit->damageEventId != 0 && evtCheckID(unit->damageEventId) != 0) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(_disable_restore_command_cursor) {
    BattleWork* wp = _battleWorkPointer;

    // BattleCommand::unk540 spans 0x540..0x55C; commandMenu is at 0x171C, so
    // offset 0x554 (s32) / 0x558 (u8) into it have no named field yet.
    *(s32*)&wp->commandMenu.unk540[0x14] = 0;
    wp->commandMenu.unk540[0x18] = 0;
    return EVT_RETURN_DONE;
}
