#include "unit/party/unit_party_clauda.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_cmd.h"
#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "battle/battle_sub.h"
#include "eff/n64/eff_kumokumo_n64.h"
#include "mario/mario_party.h"
#include "driver/msgdrv.h"
#include <string.h>

// sdata2 float constants live in this file's .sdata2 (see the original's data).
extern const f32 float_30_8041dcfc;
extern const f32 float_15_8041dd00;
extern const f32 float_0_8041dd04;
extern const f32 float_neg2_8041dd08;
extern const f32 float_1_8041dd0c;
extern const f32 float_180_8041dd10;
extern const f32 float_neg10_8041dd14;
extern const f32 float_25_8041dd18;
extern const f32 float_2_8041dd1c;

const f64 double_to_int_802f6f30 = 4503601774854144.0;

typedef struct ClaudaBreathEffectData {
    u8 pad_0x0[0x18];
    s32 field_0x18;
    s32 field_0x1C;
    s32 field_0x20;
    u8 pad_0x24[0x4];
    s32 field_0x28;
    s32 field_0x2C;
    s32 field_0x30;
    u8 pad_0x34[0x24];
    f32 field_0x58;
} ClaudaBreathEffectData;

extern BattleWeapon partyWeapon_ClaudaNormalAttack;
extern BattleWeapon partyWeapon_ClaudaBreathAttack;
extern BattleWeapon partyWeapon_ClaudaPredationAttack;
extern BattleWeapon partyWeapon_ClaudaKumogakureAttack;
extern u8 lbl_80376320[]; // this unit's .data weapon tables

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
USER_FUNC(_clauda_make_extra_work_area) {
    BattleWork* work = _battleWorkPointer;
    BattleUnit* unit;
    s32 id;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(work, id);
    *(void**)((u8*)unit + 0x310) = BattleAlloc(0xC0);
    return EVT_RETURN_DONE;
}

// The original addresses the four weapons from one base register (addi r30, r5, lbl_80376320@l, then
// +0x40/+0x100/+0x1C0/+0x280); naming each weapon directly does not reproduce that.
static void __makeTechMenuFunc(BattleWorkCommand* cmd, s32* count) {
#ifdef MUST_MATCH
    u8* const weaponBase = lbl_80376320;
#endif
    s32 techLv = partyGetTechLv(
        BattleTransPartyId(BattleGetPartyPtr(_battleWorkPointer)->currentKind));

#ifdef MUST_MATCH
    TECH_MENU_ENTRY(weaponBase + 0x40);  // partyWeapon_ClaudaNormalAttack
    if (techLv >= 0) {
        TECH_MENU_ENTRY(weaponBase + 0x100); // partyWeapon_ClaudaBreathAttack
    }
    if (techLv >= 1) {
        TECH_MENU_ENTRY(weaponBase + 0x1C0); // partyWeapon_ClaudaPredationAttack
    }
    if (techLv >= 2) {
        TECH_MENU_ENTRY(weaponBase + 0x280); // partyWeapon_ClaudaKumogakureAttack
    }
#else
    TECH_MENU_ENTRY(&partyWeapon_ClaudaNormalAttack);
    if (techLv >= 0) {
        TECH_MENU_ENTRY(&partyWeapon_ClaudaBreathAttack);
    }
    if (techLv >= 1) {
        TECH_MENU_ENTRY(&partyWeapon_ClaudaPredationAttack);
    }
    if (techLv >= 2) {
        TECH_MENU_ENTRY(&partyWeapon_ClaudaKumogakureAttack);
    }
#endif
}
USER_FUNC(_check_blow_rate) {
    s32* args = event->args;
    BattleWork* work = _battleWorkPointer;
    BattleUnit* unit;
    s32 arg1;
    s32 id;

    id = evtGetValue(event, args[0]);
    arg1 = args[1];
    unit = BattleGetUnitPtr(work, BattleTransID(event, id));
    evtSetValue(event, arg1, unit->vulnStatus->galeForceRate);
    return EVT_RETURN_DONE;
}
static s32 _clauda_breath_effect_ready(EventEntry* event, BOOL isFirstCall) {
    BattleWork* work = _battleWorkPointer;
    BattleUnit* unit;
    EffectEntry* effect;
    s8 dir;
    s32 id;
    Vec pos;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(work, id);
    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    dir = unit->faceDirection;
    pos.x += unit->mSizeMultiplier * (dir * 20);
    pos.y += float_25_8041dd18 * unit->mSizeMultiplier;
    if (dir >= 0) {
        effect = effKumokumoN64Entry(0, 0x3C, pos.x, pos.y, pos.z, float_0_8041dd04, float_2_8041dd1c,
                                     float_1_8041dd0c);
    } else {
        effect = effKumokumoN64Entry(0, 0x3C, pos.x, pos.y, pos.z, float_180_8041dd10, float_2_8041dd1c,
                                     float_1_8041dd0c);
    }
    ((ClaudaBreathEffectData*)effect->userdata)->field_0x18 = 0x4C;
    ((ClaudaBreathEffectData*)effect->userdata)->field_0x1C = 0xA7;
    ((ClaudaBreathEffectData*)effect->userdata)->field_0x20 = 0xCA;
    ((ClaudaBreathEffectData*)effect->userdata)->field_0x28 = 0x7D;
    ((ClaudaBreathEffectData*)effect->userdata)->field_0x2C = 0x1B;
    ((ClaudaBreathEffectData*)effect->userdata)->field_0x30 = 0xF5;
    return EVT_RETURN_DONE;
}
static s32 _clauda_breath_effect_fire(EventEntry* event, BOOL isFirstCall) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    EffectEntry* effect;
    s32 arg0;
    s8 dir;
    s32 id;
    Vec pos;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(work, id);
    arg0 = args[0];
    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    dir = unit->faceDirection;
    pos.x += unit->mSizeMultiplier * (dir * 25);
    pos.y += float_15_8041dd00 * unit->mSizeMultiplier;
    if (dir >= 0) {
        effect = effKumokumoN64Entry(0, 0, pos.x, pos.y, pos.z, float_0_8041dd04, float_neg2_8041dd08,
                                     float_1_8041dd0c);
    } else {
        effect = effKumokumoN64Entry(0, 0, pos.x, pos.y, pos.z, float_180_8041dd10, float_neg2_8041dd08,
                                     float_1_8041dd0c);
    }
    ((ClaudaBreathEffectData*)effect->userdata)->field_0x18 = 0x4C;
    ((ClaudaBreathEffectData*)effect->userdata)->field_0x1C = 0xA7;
    ((ClaudaBreathEffectData*)effect->userdata)->field_0x20 = 0xCA;
    ((ClaudaBreathEffectData*)effect->userdata)->field_0x28 = 0x7D;
    ((ClaudaBreathEffectData*)effect->userdata)->field_0x2C = 0x1B;
    ((ClaudaBreathEffectData*)effect->userdata)->field_0x30 = 0xF5;
    ((ClaudaBreathEffectData*)effect->userdata)->field_0x58 = float_neg10_8041dd14;
    evtSetValue(event, arg0, (s32)effect);
    return EVT_RETURN_DONE;
}
USER_FUNC(_make_breath_weapon) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 arg0;
    void* area;
    s32 value;
    s32 value2;
    s32 id;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(work, id);
    area = *(void**)((u8*)unit + 0x310);
    arg0 = args[0];
    value = evtGetValue(event, arg0);
    value2 = evtGetValue(event, args[1]);
    memcpy(area, (void*)value, 0xC0);
    *(u8*)((u8*)area + 0xA5) = value2;
    evtSetValue(event, arg0, (s32)area);
    return EVT_RETURN_DONE;
}
USER_FUNC(_make_kiss_weapon) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 arg0;
    void* area;
    s32 value;
    s32 id;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(work, id);
    area = *(void**)((u8*)unit + 0x310);
    arg0 = args[0];
    value = evtGetValue(event, arg0);
    evtGetValue(event, args[1]);
    memcpy(area, (void*)value, 0xC0);
    evtSetValue(event, arg0, (s32)area);
    return EVT_RETURN_DONE;
}
USER_FUNC(_get_clauda_kiss_hit_position) {
    s32* args = event->args;
    BattleWork* work = _battleWorkPointer;
    s32 id0;
    s32 id1;
    s32 out0;
    s32 out1;
    s32 out2;
    BattleUnit* unit1;
    BattleUnit* unit2;
    s32 dir;
    Vec pos;
    Vec hit;

    id0 = evtGetValue(event, args[0]);
    id1 = evtGetValue(event, args[1]);
    evtGetValue(event, args[2]);
    out0 = args[3];
    out1 = args[4];
    out2 = args[5];
    unit1 = BattleGetUnitPtr(work, BattleTransID(event, id0));
    unit2 = BattleGetUnitPtr(work, BattleTransID(event, id1));

    dir = unit2->faceDirection;
    BtlUnit_GetPos(unit2, &pos.x, &pos.y, &pos.z);
    pos.x += (f32)dir * (unit2->kissHitOffset.x * unit2->mSizeMultiplier);
    pos.y += unit2->kissHitOffset.y * unit2->mSizeMultiplier;
    pos.z += unit2->kissHitOffset.z * unit2->mSizeMultiplier;
    pos.y -= float_30_8041dcfc * unit1->mSizeMultiplier;
    hit = pos;
    hit.y = BattleGetFloorHeight(work, hit.x, hit.y, hit.z);
    if (hit.y > pos.y) {
        pos.y = hit.y;
    }
    evtSetFloat(event, out0, pos.x);
    evtSetFloat(event, out1, pos.y);
    evtSetFloat(event, out2, pos.z);
    return EVT_RETURN_DONE;
}
USER_FUNC(_make_kumoguard_weapon) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 arg0;
    void* area;
    s32 value;
    s32 value2;
    s32 id;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(work, id);
    area = *(void**)((u8*)unit + 0x310);
    arg0 = args[0];
    value = evtGetValue(event, arg0);
    value2 = evtGetValue(event, args[1]);
    memcpy(area, (void*)value, 0xC0);
    *(u8*)((u8*)area + 0x8E) = value2;
    evtSetValue(event, arg0, (s32)area);
    return EVT_RETURN_DONE;
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_30_8041dcfc = 30.0f;
const f32 float_15_8041dd00 = 15.0f;
const f32 float_0_8041dd04 = 0.0f;
const f32 float_neg2_8041dd08 = -2.0f;
const f32 float_1_8041dd0c = 1.0f;
const f32 float_180_8041dd10 = 180.0f;
const f32 float_neg10_8041dd14 = -10.0f;
const f32 float_25_8041dd18 = 25.0f;
const f32 float_2_8041dd1c = 2.0f;
