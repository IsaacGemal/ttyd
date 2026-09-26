#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "battle/battle_sub.h"
#include "driver/animdrv.h"
#include "driver/effdrv.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario_pouch.h"
#include "pmario_sound.h"
#include "system.h"

// Prototypes for helpers that have no header yet. See REPORT.md.
void BattleMajinaiDone(void);
void BattleMajinaiEndCheck(void);
void BattleAudience_Case_JumpNewRecord(void);
void BtlUnit_SetAnim(BattleUnitPart* part, const char* animName);
EffectEntry* effMajinaiEntry(s32 type, f32 x, f32 y, f32 z);
EffectEntry* effHibashiraEntry(s32 a, s32 b, s32 c, f32 x, f32 y, f32 z, f32 x2, f32 y2, f32 z2, f32 scale);
EffectEntry* effEnergyEntry(s32 a, s32 b, f32 p1, f32 p2, f32 p3, f32 p4, f32 p5, f32 p6);
EffectEntry* effFireEntry(s32 a, s32 b, f32 x, f32 y, f32 z, f32 scale);
EffectEntry* effWhirlwindN64Entry(s32 a, s32 b, s32 c, f32 x);
EffectEntry* effKamekiTornadeN64Entry(s32 a, s32 b, f32 x, f32 y, f32 z, f32 a4);
void psndSFX_pos(s32 id, Vec* position);
f32 sinfd(f32 angle);
f32 cosfd(f32 angle);

// .rodata 0x802EEFF0; referenced by _battle_majinai_effect.
const Vec vec3_802eeff0 = {0.0f, 10.0f, 0.0f};

USER_FUNC(_paper_light_off) {
    s32* args = event->args;
    BattleUnitPart* part;
    s32 id;
    s32 v0, v1;

    v0 = evtGetValue(event, args[0]);
    id = BattleTransID(event, v0);
    v1 = evtGetValue(event, args[1]);
    part = BattleGetUnitPartsPtr(id, v1);
    animPoseSetMaterialLightFlagOff(*(s32*)((u8*)part + 0x1C0), 1);
    return EVT_RETURN_DONE;
}

USER_FUNC(_battle_majinai_effect) {
    EffectEntry* effect;
    Vec pos;

    BattleGetMarioPtr(_battleWorkPointer);
    if (isFirstCall) {
        pos = vec3_802eeff0;
        event->userdata[0] = (s32)effMajinaiEntry(0, pos.x, pos.y, pos.z);
        effect = (EffectEntry*)event->userdata[0];
        effSetName(effect, "mjef_coinup");
    }
    if (event->userdata[0] != 0 && effNameToPtr("mjef_coinup") != NULL) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(_majinai_powerup_check) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 var = args[0];

    if (pouchGetPtr()->mMerleeNextCurseType != 1) {
        evtSetValue(event, var, 0);
    } else {
        evtSetValue(event, var, 1);
        *(u8*)((u8*)wp + 0x18FE1) = 1;
        BattleMajinaiDone();
        BattleMajinaiEndCheck();
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(_fire_wave) {
    s32* args = event->args;
    f32 x = (f32)evtGetValue(event, args[0]);
    f32 y = (f32)evtGetValue(event, args[1]);
    f32 z = (f32)evtGetValue(event, args[2]);
    f32 scale = 0.6f * (f32)evtGetValue(event, args[3]);

    if (isFirstCall) {
        event->userdata[0] = (s32)effHibashiraEntry(0, 1, 0x5A, x, y, z, x, y, z, scale);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(_mario_fire_ball_controll) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    EffectEntry* effect;
    s32 value0, value1;
    BattleUnitPart* part;

    value0 = evtGetValue(event, args[0]);
    value1 = evtGetValue(event, args[1]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, value0));
    part = BtlUnit_GetPartsPtr(unit, value1);
    if (isFirstCall) {
        event->userdata[0] = 0;
        event->userdata[1] = (s32)effFireEntry(5, 1000, 0.0f, 0.0f, 0.0f, 1.0f);
    }
    effect = (EffectEntry*)event->userdata[1];
    if (part->work[0] == 0) {
        effDelete(effect);
        return EVT_RETURN_DONE;
    }
    BtlUnit_GetPartsPos(part, (f32*)((u8*)effect->userdata + 4), (f32*)((u8*)effect->userdata + 8),
                        (f32*)((u8*)effect->userdata + 0xC));
    return EVT_RETURN_BLOCK;
}

USER_FUNC(_kaiten_hammer_acrobat_rotate) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    BattleUnitPart* part;
    s32 value0, value1;
    s32 count;
    s32 anim;
    f32 x, y, z;

    value0 = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, value0));
    value1 = evtGetValue(event, args[1]);
    part = BtlUnit_GetPartsPtr(unit, value1);
    count = evtGetValue(event, args[2]);
    anim = evtGetValue(event, args[3]);
    if (isFirstCall) {
        BtlUnit_GetRotate(unit, &x, &y, &z);
        event->userdata[0] = count;
        if (unit->faceDirection >= 0) {
            event->userdata[1] = (s32)(360.0f + (360.0f - reviseAngle(y)));
        } else {
            event->userdata[1] = (s32)(360.0f + reviseAngle(y));
        }
        event->userdata[2] = (unit->faceDirection * event->userdata[1]) / count;
        event->field_84 = 0;
    }
    BtlUnit_AddRotate(unit, 0.0f, (f32)event->userdata[2], 0.0f);
    event->userdata[0]--;
    if ((s32)event->field_84 == 0) {
        event->userdata[1] -= event->userdata[2];
        if (event->userdata[1] <= 0x10E) {
            event->field_84 = 1;
            BtlUnit_SetAnim(part, (const char*)anim);
        }
    }
    if (event->userdata[0] > 0) {
        return EVT_RETURN_BLOCK;
    }
    BtlUnit_SetRotate(unit, 0.0f, 0.0f, 0.0f);
    return EVT_RETURN_DONE;
}

// NONMATCHING (97.60%): the only difference is scheduling of the `lwz` that
// reads event->userdata[0] for the two intplGetValue calls: mwcc issues it first,
// the original issues it after the `lfs`/`li` argument setup. Everything else matches.
USER_FUNC(_hammer_star_effect) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 value0, value1;
    Vec pos;
    f32 angle;
    f32 dir;

    value0 = evtGetValue(event, args[0]);
    value1 = evtGetValue(event, args[1]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, value0));
    if (isFirstCall) {
        event->userdata[0] = 0;
    }
    {
    s32 frame = event->userdata[0];
    if (unit->faceDirection >= 0) {
        angle = intplGetValue(1, frame, 12, 210.0f, 420.0f);
        dir = -10.0f;
    } else {
        angle = intplGetValue(1, frame, 12, 150.0f, -60.0f);
        dir = 10.0f;
    }
    }
    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    pos.y += unit->mSizeMultiplier * (0.85f * (f32)((s32(*)(BattleUnit*))BtlUnit_GetHeight)(unit));
    pos.x += unit->mSizeMultiplier * (26.0f * -sinfd(angle));
    pos.y += unit->mSizeMultiplier * (26.0f * cosfd(angle));
    effEnergyEntry(value1, 30, pos.x, pos.y, pos.x, 0.08f * event->userdata[0] + 0.1f, dir, 0.05f);
    if (event->userdata[0] < 12) {
        event->userdata[0]++;
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(mario_get_renzoku_count_max) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 val0, val1;
    s32 dest;
    s32 max;
    BattleWork* wp;

    val0 = evtGetValue(event, args[0]);
    val1 = evtGetValue(event, args[1]);
    dest = args[2];
    wp = _battleWorkPointer;
    unit = BattleGetUnitPtr(wp, BattleTransID(event, val0));
    if (unit == NULL) {
        evtSetValue(event, dest, 0);
        return EVT_RETURN_DONE;
    }
    max = *(u16*)((u8*)unit->mKindParams + 0x14);
    if (val1 < max) {
        evtSetValue(event, dest, max);
        return EVT_RETURN_DONE;
    }
    if (val1 >= max * 2) {
        evtSetValue(event, dest, val1);
        return EVT_RETURN_DONE;
    }
    if (irand(100) < 33) {
        evtSetValue(event, dest, val1 + 1);
    } else {
        evtSetValue(event, dest, val1);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(_record_renzoku_count) {
    s32* args = event->args;
    s32 count;
    PouchData* pouch;
    u8* record;

    count = evtGetValue(event, args[0]);
    record = (u8*)_battleWorkPointer + 0x16F24;
    pouch = pouchGetPtr();
    if (count > record[0]) {
        if (count > *(u16*)&pouch->mPowerBounceRecord) {
            pouch->mPowerBounceRecord = count;
            BattleAudience_Case_JumpNewRecord();
        }
        if (count > 100) {
            count = 100;
        }
        record[0] = count;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(_tatsumaki_effect) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    EffectEntry* effect;
    s32 value;

    value = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, value));
    if (isFirstCall) {
        event->userdata[0] = 0;
    }
    switch (event->userdata[0]) {
    case 0: {
        Vec pos;

        event->userdata[0]++;
        event->userdata[1] = 0;
        event->userdata[2] = (s32)unit->mPosition.x;
        event->field_84 = (s32)unit->mPosition.y - 40;
        *(s32*)((u8*)event + 0x88) = (s32)unit->mPosition.z;
        *(s32*)((u8*)event + 0x8C) = (s32)effKamekiTornadeN64Entry(0, 0x5A,
            (f32)event->userdata[2], (f32)(s32)event->field_84, (f32)*(s32*)((u8*)event + 0x88),
            3.0f);
        *(s32*)((u8*)event + 0x90) = (s32)effKamekiTornadeN64Entry(0, 0x5A,
            (f32)event->userdata[2], (f32)(s32)event->field_84, (f32)*(s32*)((u8*)event + 0x88),
            3.0f);
        *(s32*)((u8*)event + 0x94) = psndSFXOn("SFX_BTL_JUMP_TATUMAKI2");
        pos.x = (f32)event->userdata[2];
        pos.y = (f32)(s32)event->field_84;
        pos.z = (f32)*(s32*)((u8*)event + 0x88);
        psndSFX_pos(*(s32*)((u8*)event + 0x94), &pos);
    }
    case 1: {
        f32 amp;
        f32 c;
        f32 s;
        f32 angle;

        event->userdata[1]++;
        amp = intplGetValue(5, event->userdata[1], 90, 0.0f, 80.0f);
        angle = (f32)(event->userdata[1] * 4);
        c = amp * cosfd(angle);
        s = amp * sinfd(angle);
        *(f32*)((u8*)((EffectEntry*)*(s32*)((u8*)event + 0x8C))->userdata + 4) =
            (f32)event->userdata[2] + c;
        *(f32*)((u8*)((EffectEntry*)*(s32*)((u8*)event + 0x8C))->userdata + 8) =
            (f32)(s32)event->field_84;
        *(f32*)((u8*)((EffectEntry*)*(s32*)((u8*)event + 0x8C))->userdata + 0xC) =
            (f32)*(s32*)((u8*)event + 0x88) + s;
        *(f32*)((u8*)((EffectEntry*)*(s32*)((u8*)event + 0x90))->userdata + 4) =
            (f32)event->userdata[2] - c;
        *(f32*)((u8*)((EffectEntry*)*(s32*)((u8*)event + 0x90))->userdata + 8) =
            (f32)(s32)event->field_84;
        *(f32*)((u8*)((EffectEntry*)*(s32*)((u8*)event + 0x90))->userdata + 0xC) =
            (f32)*(s32*)((u8*)event + 0x88) - s;
        if (event->userdata[1] >= 90) {
            psndSFXOff(*(s32*)((u8*)event + 0x94));
            return EVT_RETURN_DONE;
        }
        break;
    }
    default:
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(_wait_jyabara_hit_iron_frame) {
    s32* args = event->args;
    BattleUnit* unit;
    Vec pos;
    s32 value;

    value = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, value));
    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    if (pos.y + (f32)((s32(*)(BattleUnit*))BtlUnit_GetHeight)(unit) < 130.0f) {
        return EVT_RETURN_BLOCK;
    }
    BtlUnit_SetPos(unit, pos.x, (f32)(130 - ((s32(*)(BattleUnit*))BtlUnit_GetHeight)(unit)), pos.z);
    return EVT_RETURN_DONE;
}

USER_FUNC(_bgset_iron_frame_check) {
    s32* args = event->args;
    u8 flags = *(u8*)((u8*)_battleWorkPointer + 0x180DC);
    s32 value = args[0];

    if (flags & 0x20) {
        evtSetValue(event, value, 0);
    } else {
        evtSetValue(event, value, 1);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(_jump_star_effect) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 value0, value1;
    s32 step;
    Vec pos;
    f32 a, b;

    value0 = evtGetValue(event, args[0]);
    value1 = evtGetValue(event, args[1]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, value0));
    if (isFirstCall) {
        event->userdata[0] = 0;
    }
    step = event->userdata[0] / 4;
    if (event->userdata[0] % 4 == 0) {
        if (unit->faceDirection >= 0) {
            a = 10.0f;
            b = -10.0f;
        } else {
            a = -10.0f;
            b = 10.0f;
        }
        BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
        pos.x += b * unit->mSizeMultiplier;
        pos.y += (f32)(step * 8 / 10);
        {
            f32 scale = 0.1f * (f32)step + 0.8f;
            if (scale > 2.0f) {
                scale = 2.0f;
            }
            effEnergyEntry(value1, 50, pos.x, pos.y, pos.x, scale, a, 0.05f);
        }
    }
    if (event->userdata[0] <= 0x18) {
        event->userdata[0]++;
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(_whirlwind_effect) {
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
    effect = effWhirlwindN64Entry((u32)unit->faceDirection >> 31, 0, value1, unit->mSizeMultiplier);
    *(f32*)((u8*)effect->userdata + 8) = pos.x;
    *(f32*)((u8*)effect->userdata + 0xC) = pos.y;
    *(f32*)((u8*)effect->userdata + 0x10) = pos.z;
    return EVT_RETURN_DONE;
}

static USER_FUNC(_get_mario_hammer_lv) {
    s32* args = event->args;
    s32 value = args[0];

    evtSetValue(event, value, pouchGetPtr()->hammerLevel);
    return EVT_RETURN_DONE;
}
