#include "unit/party/unit_party_yoshi.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_cmd.h"
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "battle/battle_disp.h"
#include "driver/animdrv.h"
#include "driver/msgdrv.h"
#include "mario/mario_pouch.h"
#include "mario/mario_party.h"
#include "system.h"
#include "eff/n64/eff_sweat_n64.h"
#include "eff/eff_kemutest.h"
#include "driver/dispdrv.h"
#include "pmario_sound.h"
#include <string.h>

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u8 lbl_80379A00[8] = { 0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00 };

// A pose-sound table entry. No header declares this type; the layout matches the
// 0x14-byte entries of `posesound_normal_attack` and the fields BtlUnit_ControlPoseSoundMain
// reads (pose name 0x0, volume 0x4, callback 0x8, sound name 0xC, pitch 0x10, flags 0x12).
typedef struct PoseSoundEntry {
    const char* poseName;                                   // 0x0
    f32 volume;                                             // 0x4
    s32 (*callback)(BattleUnit*, struct PoseSoundEntry*, void*); // 0x8
    const char* soundName;                                  // 0xC
    s16 pitch;                                              // 0x10
    u16 flags;                                              // 0x12
} PoseSoundEntry;

// Reusing one variable for both evtGetValue and BattleTransID (`id = evtGetValue(...);
// id = BattleTransID(event, id);`) removed the extra `mr r0` the two-variable form emitted.
// What still differs is the pool: the string pool base is `r6`/`...rodata.0` (original `r5`/`str_802F6F38`),
// so args goes through r5 not r6, the six strings sit at +0x8.. not +0x4F8.., and the jump-table base is
// `@NNN` not `jumptable_8037EBF4`; those bytes belong to the unit's not-yet-written functions.
// NONMATCHING (99.70%): only the pool base register/symbol and the pool offsets still differ.
USER_FUNC(yoshi_original_color_anim_set) {
    s32 id;
    const char* name;
    BattleWork* work;
    s32* args;
    BattleUnitPart* part;

    args = event->args;
    work = _battleWorkPointer;
    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    BattleGetUnitPtr(work, id);
    switch (pouchGetPartyColor(4)) {
    case 1:
        name = "c_babyyoshi2";
        break;
    case 2:
        name = "c_babyyoshi3";
        break;
    case 3:
        name = "c_babyyoshi4";
        break;
    case 4:
        name = "c_babyyoshi5";
        break;
    case 5:
        name = "c_babyyoshi6";
        break;
    case 6:
        name = "c_babyyoshi7";
        break;
    case 0:
    default:
        name = NULL;
        break;
    }
    if (isFirstCall) {
        if (name == NULL) {
            return EVT_RETURN_DONE;
        }
        part = BattleGetUnitPartsPtr(id, 1);
        animPoseGetAnimPosePtr(*(s32*)((u8*)part + 0x1C0));
        if (*(s32*)((u8*)part + 0x1C0) != -1) {
            animPoseRelease(*(s32*)((u8*)part + 0x1C0));
            *(s32*)((u8*)part + 0x1C0) = -1;
        }
    }
    if (animEffectAsync(name, 1)) {
        part = BattleGetUnitPartsPtr(id, 1);
        *(s32*)((u8*)part + 0x1C0) = animPoseEntry(name, 1);
        return EVT_RETURN_DONE;
    }
    return 0;
}

extern BattleWeapon partyWeapon_YoshiNormalAttack;
extern BattleWeapon partyWeapon_YoshiNomikomi_Shot;
extern BattleWeapon partyWeapon_YoshiEggAttack_Minimini;
extern BattleWeapon partyWeapon_YoshiCallGuard;

#define TECH_MENU_ENTRY(table)                                                        \
    cmd->mWeaponTable[*count].mIndex = -1;                                            \
    cmd->mWeaponTable[*count].mItem = 0;                                              \
    cmd->mWeaponTable[*count].mWeaponParams = (BattleWeapon*)(table);                 \
    cmd->mWeaponTable[*count].field_0x4 = 0;                                          \
    cmd->mWeaponTable[*count].mIcon =                                                 \
        *(u16*)((u8*)cmd->mWeaponTable[*count].mWeaponParams + 4);                    \
    cmd->mWeaponTable[*count].mName =                                                 \
        (char*)msgSearch(*(char**)((u8*)cmd->mWeaponTable[*count].mWeaponParams + 0)); \
    (*count)++;

// The original addresses the four weapons from one base register (addi r30, r5, lbl_80379A00@l, then
// +0x624/+0x6E4/+0xAA4/+0xB64); naming each weapon directly does not reproduce that.
static void __makeTechMenuFunc(BattleWorkCommand* cmd, s32* count) {
#ifdef MUST_MATCH
    u8* const weaponBase = lbl_80379A00;
#endif
    s32 techLv = partyGetTechLv(
        BattleTransPartyId(BattleGetPartyPtr(_battleWorkPointer)->currentKind));

#ifdef MUST_MATCH
    TECH_MENU_ENTRY(weaponBase + 0x624);  // partyWeapon_YoshiNormalAttack
    if (techLv >= 0) {
        TECH_MENU_ENTRY(weaponBase + 0x6E4); // partyWeapon_YoshiNomikomi_Shot
    }
    if (techLv >= 1) {
        TECH_MENU_ENTRY(weaponBase + 0xAA4); // partyWeapon_YoshiEggAttack_Minimini
    }
    if (techLv >= 2) {
        TECH_MENU_ENTRY(weaponBase + 0xB64); // partyWeapon_YoshiCallGuard
    }
#else
    TECH_MENU_ENTRY(&partyWeapon_YoshiNormalAttack);
    if (techLv >= 0) {
        TECH_MENU_ENTRY(&partyWeapon_YoshiNomikomi_Shot);
    }
    if (techLv >= 1) {
        TECH_MENU_ENTRY(&partyWeapon_YoshiEggAttack_Minimini);
    }
    if (techLv >= 2) {
        TECH_MENU_ENTRY(&partyWeapon_YoshiCallGuard);
    }
#endif
}

// .sdata2 constants used by the yoroyoro jump functions.
extern const f32 float_0_8041dd44;
extern const f32 float_1_8041dd68;
extern const f32 float_5_8041dd70;
extern const f32 float_10_8041dd7c;
extern const f32 float_180_8041dd80;
extern const f32 float_360_8041dd84;
extern const f32 float_90_8041dd88;
extern const f32 float_75_8041dd8c;
extern const f32 float_25_8041dd90;
extern const f32 float_neg50_8041dd94;
extern const f32 float_50_8041dd98;
extern const f32 float_0p5_8041dd9c;

// Matched: the `if (tx - px > 0.0f)` comparison must use the literal `0.0f` (pooled as its own
// symbol) rather than the `float_0_8041dd44` extern; that flips the FP load/compare registers to
// the target's. The remaining `double_to_int_802f7410` relocation names are objdiff-normalised.
USER_FUNC(btl_yoshi_yoroyoro_jump_calc_param) {
    BattleWork* work;
    s32* args;
    f32 tx;
    f32 ty;
    f32 tz;
    f32 px;
    f32 py;
    f32 pz;
    f32 speed;
    s32 id;
    BattleUnit* unit;
    s32 val;
    s32 speedVar;
    s32 frameVar;

    work = _battleWorkPointer;
    args = event->args;
    val = evtGetValue(event, args[0]);
    id = BattleTransID(event, val);
    tx = (f32)evtGetValue(event, args[1]);
    ty = (f32)evtGetValue(event, args[2]);
    tz = (f32)evtGetValue(event, args[3]);
    speedVar = args[4];
    speed = evtGetFloat(event, speedVar);
    unit = BattleGetUnitPtr(work, id);
    frameVar = args[5];
    if (speed > float_0_8041dd44) {
        unit->mMoveSpeedXZ = speed;
    }
    if (unit->mMoveSpeedXZ <= float_0_8041dd44) {
        unit->mMoveSpeedXZ = float_1_8041dd68;
    }
    BtlUnit_SetMoveTargetPos(unit, tx, ty, tz);
    BtlUnit_GetPos(unit, &px, &py, &pz);
    BtlUnit_SetMoveStartPos(unit, px, py, pz);
    BtlUnit_SetMoveCurrentPos(unit, px, py, pz);
    *(f32*)((u8*)unit + 0x178) = angleABf(px, pz, tx, tz);
    *(f32*)((u8*)unit + 0x17C) = distABf(px, pz, tx, tz);
    if (tx - px > 0.0f) {
        unit->moveDirection = 1;
    } else if (tx - px < 0.0f) {
        unit->moveDirection = -1;
    }
    *(f32*)((u8*)unit + 0x17C) = __fabs(tx - px);
    *(s32*)((u8*)unit + 0x168) = 180;
    *(f32*)((u8*)unit + 0x180) = (f32)*(s32*)((u8*)unit + 0x168);
    unit->mMoveSpeedY = float_0_8041dd44;
    unit->mMoveSpeedXZ = *(f32*)((u8*)unit + 0x17C) / (f32)*(s32*)((u8*)unit + 0x168);
    evtSetFloat(event, speedVar, unit->mMoveSpeedXZ);
    evtSetValue(event, frameVar, *(s32*)((u8*)unit + 0x168));
    return 2;
}

// NONMATCHING (98.89%): left is the second block's `0.5f`/`mSizeMultiplier` FP registers swapped
// (target f2/f1, ours f1/f2) and one `lwz effect` / `mr r3,unit` scheduling slot around the
// `userdata[1] < 16` branch. `height` must stay an `f32` local for the first block to match.
USER_FUNC(btl_yoshi_yoroyoro_jump_move) {
    s32 id;
    BattleWork* work;
    BattleUnit* unit;
    s32* args;
    EffectEntry* effect;
    f32 newSpeedY;
    f32 height;
    s32 val;

    work = _battleWorkPointer;
    args = event->args;
    val = evtGetValue(event, args[0]);
    id = BattleTransID(event, val);
    evtGetValue(event, args[1]);
    evtGetValue(event, args[2]);
    evtGetValue(event, args[3]);
    evtGetFloat(event, args[4]);
    unit = BattleGetUnitPtr(work, id);
    evtGetValue(event, args[5]);
    if (isFirstCall) {
        event->blocked = 1;
        event->userdata[1] = 20;
        event->userdata[2] = 0;
    }
    if (unit->mMoveCurrentPos.x != unit->mMoveTargetPos.x) {
        if (__fabs(unit->mMoveTargetPos.x - unit->mMoveCurrentPos.x) < unit->mMoveSpeedXZ) {
            unit->mMoveCurrentPos.x = unit->mMoveTargetPos.x;
        } else {
            unit->mMoveCurrentPos.x += unit->mMoveSpeedXZ * (f32)unit->moveDirection;
        }
    }
    if (unit->mMoveCurrentPos.z != unit->mMoveTargetPos.z) {
        if (__fabs(unit->mMoveTargetPos.z - unit->mMoveCurrentPos.z) < unit->mMoveSpeedXZ) {
            unit->mMoveCurrentPos.z = unit->mMoveTargetPos.z;
        } else if (unit->mMoveTargetPos.z - unit->mMoveStartPos.z > float_0_8041dd44) {
            unit->mMoveCurrentPos.z += unit->mMoveSpeedXZ;
        } else {
            unit->mMoveCurrentPos.z -= unit->mMoveSpeedXZ;
        }
    }
    newSpeedY = float_10_8041dd7c *
                sinfd(float_180_8041dd80 +
                      float_360_8041dd84 *
                          (*(f32*)((u8*)unit + 0x180) -
                           (f32)(*(s32*)((u8*)unit + 0x168) % 90)) /
                          float_90_8041dd88);
    BtlUnit_GetPartsPtr(unit, 1);
    unit->mMoveCurrentPos.y =
        unit->mMoveSpeedY +
        (unit->mMoveStartPos.y +
         (unit->mMoveTargetPos.y - unit->mMoveStartPos.y) *
             (*(f32*)((u8*)unit + 0x180) - (f32)*(s32*)((u8*)unit + 0x168)) /
             *(f32*)((u8*)unit + 0x180));
    unit->mMoveSpeedY = newSpeedY;
    unit->work[5] =
        (s32)(float_75_8041dd8c -
              float_25_8041dd90 *
                  sinfd(float_360_8041dd84 *
                            (*(f32*)((u8*)unit + 0x180) -
                             (f32)(*(s32*)((u8*)unit + 0x168) % 90)) /
                        float_90_8041dd88));
    BtlUnit_SetPos(unit, unit->mMoveCurrentPos.x, unit->mMoveCurrentPos.y, unit->mMoveCurrentPos.z);
    event->userdata[1]++;
    if (event->userdata[1] > 20) {
        event->userdata[1] = 0;
        if (unit->moveDirection >= 0) {
            newSpeedY = float_neg50_8041dd94;
        } else {
            newSpeedY = float_50_8041dd98;
        }
        height = BtlUnit_GetHeight(unit);
        event->userdata[2] = (s32)effSweatN64Entry(
            1, 16, unit->mMoveCurrentPos.x,
            unit->mSizeMultiplier * (float_0p5_8041dd9c * height) + unit->mMoveCurrentPos.y,
            unit->mMoveCurrentPos.z, float_5_8041dd70 * unit->mSizeMultiplier, newSpeedY);
    } else if (event->userdata[1] < 16) {
        effect = (EffectEntry*)event->userdata[2];
        ((effSweatData*)effect->userdata)->position.x = unit->mMoveCurrentPos.x;
        height = BtlUnit_GetHeight(unit);
        ((effSweatData*)effect->userdata)->position.y =
            unit->mSizeMultiplier * (float_0p5_8041dd9c * height) + unit->mMoveCurrentPos.y;
        ((effSweatData*)effect->userdata)->position.z = unit->mMoveCurrentPos.z;
    }
    *(s32*)((u8*)unit + 0x168) -= 1;
    if (*(s32*)((u8*)unit + 0x168) <= 0) {
        BtlUnit_SetPos(unit, unit->mMoveTargetPos.x, unit->mMoveTargetPos.y, unit->mMoveTargetPos.z);
        return 2;
    }
    return 0;
}

s32 _yoshi_slide_move_sound(BattleUnit* unit, PoseSoundEntry* entry) {
    return BtlUnit_snd_se(unit, entry->soundName, 0xF1194D80, (s16)(unit->work[4] * unit->work[5] / 100));
}

// NONMATCHING (92.80%): same instruction sequence; the callee-saved registers are assigned in a different order
// (target: event r29, args r27, work r28; ours r31, r29, r30).
USER_FUNC(_get_nomikomi_hit_position) {
    f32 y;
    s32 outY;
    f32 scale;
    s32 outX;
    s32 outZ;
    f32 z;
    f32 x;
    BattleWork* work;
    s32* args;
    s32 id1;
    s32 id2;
    s32 dir;
    BattleUnit* unit;

    args = event->args;
    work = _battleWorkPointer;
    id1 = evtGetValue(event, args[0]);
    id2 = evtGetValue(event, args[1]);
    evtGetValue(event, args[2]);
    outX = args[3];
    outY = args[4];
    outZ = args[5];
    BattleGetUnitPtr(work, BattleTransID(event, id1));
    unit = BattleGetUnitPtr(work, BattleTransID(event, id2));
    dir = unit->faceDirection;
    BtlUnit_GetPos(unit, &x, &y, &z);
    scale = unit->mSizeMultiplier;
    x += (f32)dir * (unit->bintaHitOffset.x * scale);
    y += unit->bintaHitOffset.y * scale;
    z += unit->bintaHitOffset.z * scale;
    evtSetFloat(event, outX, x);
    evtSetFloat(event, outY, y);
    evtSetFloat(event, outZ, z);
    return EVT_RETURN_DONE;
}

USER_FUNC(_get_swallow_param) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 val;
    s32 id;
    s32 index;

    val = evtGetValue(event, args[0]);
    id = BattleTransID(event, val);
    index = args[1];
    unit = BattleGetUnitPtr(work, id);
    evtSetValue(event, index, unit->swallowRate);
    return EVT_RETURN_DONE;
}

USER_FUNC(_check_swallow_attribute) {
    BattleWork* work;
    BattleUnit* unit;
    s32 val;
    u16 mask;
    s32* args;
    s32 index;
    s32 id;

    args = event->args;
    work = _battleWorkPointer;
    val = evtGetValue(event, args[0]);
    id = BattleTransID(event, val);
    mask = args[1];
    index = args[2];
    unit = BattleGetUnitPtr(work, id);
    if ((u8)unit->swallowAttribute & mask) {
        evtSetValue(event, index, 1);
    } else {
        evtSetValue(event, index, 0);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(_wait_yoshig_run) {
    BattleUnit* unit;
    s32 val;
    f32* area;

    val = evtGetValue(event, event->args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, val));
    area = *(f32**)((u8*)unit + 0x310);
    if (area == NULL) {
        return 0;
    }
    if (*(f32*)((u8*)area + 0x3008) <= 0.0f) {
        return 0;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(_wait_yoshig_complete) {
    BattleUnit* unit;
    s32 val;

    val = evtGetValue(event, event->args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, val));
    return *(void**)((u8*)unit + 0x310) ? 0 : EVT_RETURN_DONE;
}

// One of the 256 particles drawn by _gundan_yoshi_run_effect: a 0x30-byte entry whose
// display callback argument is the entry's +4 sub-structure. No header declares it.
typedef struct YoshiGundanDispWork {
    f32 field_0x00; // 0x00
    f32 field_0x04; // 0x04
    f32 field_0x08; // 0x08
    f32 field_0x0c; // 0x0C
    f32 field_0x10; // 0x10
    f32 field_0x14; // 0x14
    u8  field_0x18[0x0c]; // 0x18
    u8  field_0x24; // 0x24
    u8  field_0x25; // 0x25, index into _yoshig_tpl_no_tbl (0..5)
    u8  field_0x26[2]; // 0x26
    s32 field_0x28; // 0x28
    s32 field_0x2c; // 0x2C
} YoshiGundanDispWork;

// .sdata/.sdata2 constants this unit references by name.
extern u8 _yoshig_tpl_no_tbl[8];
extern const GXColor dat_8041dd38;       // 0xFFFFFFFF
extern const f32 float_0p4_8041dd3c;
extern const f32 float_neg0p4_8041dd40;
extern const f32 float_0_8041dd44;
extern const f32 float_20_8041dd48;

extern const f32 float_0p1_8041dd50;
extern const f32 float_3_8041dd4c;
extern const f32 float_0p001_8041dd54;
extern const f32 float_80_8041dd58;
extern const f32 float_neg1_8041dd5c;
extern const f32 float_neg80_8041dd60;
extern const f32 float_300_8041dd64;
extern const f32 float_0p05_8041dd6c;
extern const f32 float_65_8041dd74;
extern const f32 float_0p0039062_8041dd78;

// The 0x300C-byte allocation `_gundan_yoshi_run_effect` keeps at unit+0x310: a state word,
// 256 0x30-byte particles, and a trailing accumulator. No header declares it.
typedef struct YoshiGundanArea {
    s32 state;                // 0x0
    u8 pad_0x4[0x3008 - 0x4]; // 0x4
    f32 field_0x3008;         // 0x3008
} YoshiGundanArea;

// Matched. The zone constants must be literals (`80.0f`/`-80.0f`/`300.0f`) while `-1.0f` stays the
// `float_neg1_8041dd5c` extern, and `active` must be reached with a `goto` label placed after the
// `default:` case so MWCC shares the r0 `li` block instead of tail-duplicating it.
USER_FUNC(_gundan_yoshi_run_effect) {
    BattleUnit* unit;
    YoshiGundanArea* area;
    YoshiGundanDispWork* work;
    Vec pos;
    f32 scale;
    s32 val;
    s32 active;
    s32 i;
    s32 done;

    val = evtGetValue(event, event->args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, val));
    if (isFirstCall) {
        area = BattleAlloc(0x300C);
        *(YoshiGundanArea**)((u8*)unit + 0x310) = area;
        memset(area, 0, 0x300C);
        area->state = 0;
        area->field_0x3008 = 0.0f;
        event->userdata[0] = 0;
        event->userdata[1] = psndSFXOn("SFX_BTL_YOSHI_TAIGUN1");
    } else {
        area = *(YoshiGundanArea**)((u8*)unit + 0x310);
    }
    switch (area->state) {
    case 0:
        area->state++;
        // fallthrough
    case 1:
        done = 1;
        area->field_0x3008 = 0.0f;
        for (i = 0; i < 256; i++) {
            work = (YoshiGundanDispWork*)((u8*)area + 4 + i * 0x30);
            work->field_0x24++;
            if (work->field_0x24 > 2) {
                work->field_0x24 = 0;
                work->field_0x25++;
                if (work->field_0x25 >= 6) {
                    work->field_0x25 = 0;
                }
            }
            switch (work->field_0x2c) {
            case 0:
                work->field_0x2c++;
                work->field_0x28 = irand(3);
                work->field_0x00 = (f32)(-(30 * (i / 16) + 300) + irand(10));
                work->field_0x04 = 0.0f;
                work->field_0x08 = (f32)((i % 16) * 10 - 80);
                work->field_0x25 = irand(6);
                work->field_0x24 = irand(5);
                work->field_0x0c = float_3_8041dd4c + float_0p1_8041dd50 * (f32)irand(10);
                work->field_0x10 = 0.0f;
                work->field_0x14 = float_0p001_8041dd54 * (f32)(irand(2000) - 1000);
                // fallthrough
            case 1:
                if (work->field_0x28 > 0) {
                    work->field_0x28--;
                    goto yoshi_active;
                }
                // fallthrough
            case 2:
                work->field_0x08 += work->field_0x14;
                if (work->field_0x08 >= 80.0f) {
                    work->field_0x08 = 80.0f;
                    work->field_0x14 = work->field_0x14 * float_neg1_8041dd5c;
                }
                if (work->field_0x08 <= -80.0f) {
                    work->field_0x08 = -80.0f;
                    work->field_0x14 = work->field_0x14 * float_neg1_8041dd5c;
                }
                work->field_0x00 += work->field_0x0c;
                if (work->field_0x00 >= 300.0f) {
                    work->field_0x00 = 300.0f;
                    work->field_0x0c = 0.0f;
                    work->field_0x2c = 999;
                }
                goto yoshi_active;
            default:
                active = 0;
                break;
            yoshi_active:
                active = 1;
                break;
            }
            if (active) {
                done = 0;
            }
            area->field_0x3008 += work->field_0x00;
        }
        if (event->userdata[0] > 0) {
            event->userdata[0]--;
        } else {
            work = (YoshiGundanDispWork*)((u8*)area + 4 + irand(256) * 0x30);
            scale = float_1_8041dd68 + float_0p05_8041dd6c * (f32)irand(20);
            effKemuTestEntry(0, work->field_0x00,
                             float_5_8041dd70 + work->field_0x04 + (f32)irand(15),
                             float_65_8041dd74, scale);
        }
        area->field_0x3008 = area->field_0x3008 * float_0p0039062_8041dd78;
        pos.x = area->field_0x3008;
        pos.y = 0.0f;
        pos.z = 0.0f;
        psndSFX_pos(event->userdata[1], &pos);
        if (done) {
            area->state = 999;
        }
        break;
    case 999:
        psndSFXOff(event->userdata[1]);
        BattleFree(*(YoshiGundanArea**)((u8*)unit + 0x310));
        *(YoshiGundanArea**)((u8*)unit + 0x310) = NULL;
        return 2;
    default:
        break;
    }
    for (i = 0; i < 256; i++) {
        dispEntry(4, dispCalcZ((Vec*)work), 2, _btlYoshiDisp,
                  (YoshiGundanDispWork*)((u8*)area + 4 + i * 0x30));
    }
    return 0;
}

void _btlYoshiDisp(CameraId cameraId, void* param) {
    YoshiGundanDispWork* work = param;
    GXColor color;
    f32 x_scale;

    x_scale = (work->field_0x0c < float_0_8041dd44) ? float_0p4_8041dd3c : float_neg0p4_8041dd40;
    color = dat_8041dd38;
#ifdef MUST_MATCH
    // btlDispTexPlane's real signature is
    // (f32 x, f32 y, f32 z, f32 sx, f32 sy, s32 frame, GXColor* color, s32 unused);
    // include/battle/battle_disp.h declares it with the decomp's (u32 id, GXColor color, ...)
    // order and omits the trailing unused s32. Call through the real signature so the original's
    // argument setup (color pointer in r4, `li r5, 0`) is emitted. The natural decomp-form call
    // is under #else.
    ((void (*)(f32, f32, f32, f32, f32, s32, GXColor*, s32))btlDispTexPlane)(
        work->field_0x00, float_20_8041dd48 + work->field_0x04, work->field_0x08, x_scale,
        float_0p4_8041dd3c, _yoshig_tpl_no_tbl[work->field_0x25], &color, 0);
#else
    btlDispTexPlane(_yoshig_tpl_no_tbl[work->field_0x25], color, work->field_0x00,
                    float_20_8041dd48 + work->field_0x04, work->field_0x08, x_scale,
                    float_0p4_8041dd3c);
#endif
}
