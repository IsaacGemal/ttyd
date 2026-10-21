// barriern_zanzou_sub and barriern_satellite_zanzou. Included where they sit in the unit's function order (it sets the
// .text layout); see rel/common/unit_barriern.h.
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "rel/common/unit_barriern.h"
#include "system.h"

static s32 barriern_zanzou_sub(BOOL isFirstCall, BattleUnit* unit, BarriernZanzouScale scale, s32 mode) {
    BattleUnitPart* part;
    f32 temp2[5];
    f32 temp1[5];
    f32 x;
    f32 y;
    f32 z;
    f32 base;
    f32 step;
    f32 s;
    s32 num;
    u8 alpha1;
    u8 alpha2;
    s32 a = 4;
    s32 i;
    s32 j;

    switch (mode) {
    case 0:
        num = 0x24;
        base = 0.75f;
        for (i = 0; i < 5; i++) {
            temp2[i] = scale.value[i];
            temp1[i] = 0.75f;
        }
        step = 0.5f;
        alpha1 = 0;
        alpha2 = 0x40;
        a = 4;
        break;
    case 1:
    case 2:
        num = 8;
        base = 1.0f;
        for (i = 0; i < 5; i++) {
            temp2[i] = 1.0f;
            temp1[i] = scale.value[i];
        }
        step = -0.5f;
        alpha1 = 0x40;
        alpha2 = 0;
        a = 1;
        break;
    }
    if (mode == 2) {
        num = 0x24;
    }
    if (isFirstCall) {
        zanzou_count[0] = 0;
        BtlUnit_GetPos(unit, &x, &y, &z);
        BtlUnit_SetPartsScale(BtlUnit_GetPartsPtr(unit, 1), 1.0f, 1.0f, 1.0f);
        for (i = 2, j = 0; i <= 6; i++, j++) {
            part = BtlUnit_GetPartsPtr(unit, i);
            BtlUnit_SetPartsPos(part, x, y, z + (f32)i * step);
            BtlUnit_SetPartsScale(part, temp2[j], temp2[j], 1.0f);
            part->color.a = alpha2;
            part->attributes &= ~0x01000000;
        }
    }
    part = BtlUnit_GetPartsPtr(unit, 1);
    s = intplGetValue(a, zanzou_count[0], num, 1.0f, base);
    BtlUnit_SetPartsScale(part, s, s, 1.0f);
    for (i = 2, j = 0; i <= 6; i++, j++) {
        part = BtlUnit_GetPartsPtr(unit, i);
        s = intplGetValue(a, zanzou_count[0], num, temp2[j], temp1[j]);
        BtlUnit_SetPartsScale(part, s, s, 1.0f);
        part->color.a = (u8)intplGetValue(0, zanzou_count[0], num, (f32)alpha1, (f32)alpha2);
    }
    if (zanzou_count[0] >= num) {
        part = BtlUnit_GetPartsPtr(unit, 1);
        BtlUnit_SetPartsScale(part, 1.0f, 1.0f, 1.0f);
        part->color.a = 0xff;
        for (i = 2; i <= 6; i++) {
            BtlUnit_GetPartsPtr(unit, i)->attributes |= 0x01000000;
        }
        return 2;
    }
    zanzou_count[0]++;
    return 0;
}

static USER_FUNC(barriern_satellite_zanzou) {
    s32 index;
    BattleUnit* unit;
#ifdef MUST_MATCH
    BattleUnit* p;
#endif
    s32* args;
    s32 id;
    BattleWork* wp;

    args = event->args;
    id = BattleTransID(event, evtGetValue(event, args[0]));
    index = evtGetValue(event, args[1]);
    wp = _battleWorkPointer;
    unit = BattleGetUnitPtr(wp, id);
#ifdef MUST_MATCH
    return barriern_satellite_zanzou_sub(isFirstCall, (p = BattleGetUnitPtr(wp, unit->work[4 + index]), p),
                                         satellite_zanzou_scale, index, 0);
#else
    return barriern_satellite_zanzou_sub(isFirstCall, BattleGetUnitPtr(wp, unit->work[4 + index]),
                                         satellite_zanzou_scale, index, 0);
#endif
}
