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
