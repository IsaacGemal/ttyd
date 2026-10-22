// barriern_wawawa_set_target, barriern_wawawa_get_target, barriern_wawawa_get_next_target and _check_satellite.
// Included where they sit in the unit's function order (it sets the .text layout); see rel/common/unit_barriern.h.
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "rel/common/unit_barriern.h"
#include "driver/effdrv.h"
#include "system.h"

// The args increment is written before the second call so the scheduler emits the addi there,
// as the original does; args[-1] is the pre-increment args[1].
static USER_FUNC(barriern_wawawa_set_target) {
    s32* args;
    s32 xs[12];
    s32 ys[10];
    s32 i;
    s32 count;
    s32 n;

    args = event->args;
    i = 0;
    _battleWorkPointer->sampling.selectIdx = 0;
    barriern_wawawa_get_target(&xs[0], &ys[0]);
    for (;;) {
        if (xs[i] == -1) {
            count = i;
            break;
        }
        i++;
        barriern_wawawa_get_next_target(&xs[i], &ys[i]);
    }
    i = 0;
    while (i < 5) {
        n = irand(count);

        evtSetValue(event, args[0], xs[n]);
        args += 2;
        evtSetValue(event, args[-1], ys[n]);
        i++;
    }
    return EVT_RETURN_DONE;
}

static void barriern_wawawa_get_target(s32* out1, s32* out2) {
    BattleSampling* sampling = &_battleWorkPointer->sampling;
    BattleSamplingEntry* entry = &sampling->entries[sampling->order[sampling->selectIdx]];

    *out1 = entry->unitId;
    *out2 = entry->partId;
}

static void barriern_wawawa_get_next_target(s32* out1, s32* out2) {
    BattleSampling* sampling = &_battleWorkPointer->sampling;
    s32 idx;

    sampling->selectIdx++;
    idx = sampling->selectIdx;
    if (idx < sampling->count) {
        BattleSamplingEntry* entry = &sampling->entries[sampling->order[idx]];
        *out1 = entry->unitId;
        *out2 = entry->partId;
    } else {
        *out1 = -1;
        *out2 = 0;
    }
}

static USER_FUNC(_check_satellite) {
    BattleUnit* unit;
    s32* args;
    BattleUnit* satellite;
    BattleUnitPart* part;
    s32 i;
    s32 hasSat;
    f32 x;
    f32 y;
    f32 z;
    f32 ox;
    f32 oy;
    f32 oz;
    s32 id;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    args = event->args;
    hasSat = 0;
    if (unit == NULL) {
        return EVT_RETURN_DONE;
    }
    for (i = 4; i < 8; i++) {
        if (unit->work[i] != -1) {
            satellite = BattleGetUnitPtr(_battleWorkPointer, unit->work[i]);
            if (satellite != NULL) {
                hasSat = 1;
            } else {
                unit->work[i] = -1;
            }
        }
    }
    if (hasSat == 0) {
        part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
        if (part->attributes & 0x20000000) {
            s32 j;
            effDelete((EffectEntry*)unit->work[3]);
            unit->work[3] = 0;
            BtlUnit_snd_se(unit, "SFX_ENM_BARRI_BARRIER_BREAK1", -250000000, 0);
            BtlUnit_GetPos(unit, &x, &y, &z);
            for (j = 0; j < 20; j++) {
                s32 w = BtlUnit_GetWidth(unit) + 0xf;
                s32 h = BtlUnit_GetHeight(unit) + 0xf;
                s32 dx = irand(w) - w / 2;
                s32 dy = irand(h) - h / 2;
                effGlassN64Entry(2, 0x3c, x + dx, y + dy, z, 1.0f);
            }
        }
        part->attributes &= ~0x20000000;
        evtSetValue(event, args[0], 0);
    } else {
        part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
        if (!(part->attributes & 0x20000000)) {
            BtlUnit_snd_se(unit, "SFX_ENM_BARRI_BARRIER1", -250000000, 0);
            BtlUnit_GetPos(unit, &ox, &oy, &oz);
            unit->work[3] = (s32)effSyuryouEntry(1, 0x3e8, 0.0f, -1000.0f, 0.0f);
        }
        part->attributes |= 0x20000000;
        evtSetValue(event, args[0], 1);
        if (unit->work[3] != 0) {
            BarriernSatelliteToData* data = ((EffectEntry*)unit->work[3])->userdata;
            BtlUnit_GetPos(unit, &x, &y, &z);
            data->x = x;
            data->y = y - 32.0f * unit->mSizeMultiplier;
            data->z = z + 10.0f;
            data->scale = 1.8f * unit->mSizeMultiplier;
        }
    }
    return EVT_RETURN_DONE;
}
