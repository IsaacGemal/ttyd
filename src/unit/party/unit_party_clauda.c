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

// NONMATCHING (75.23%): the callee-saved register allocation and the 0x40 vs 0x50 frame differ, and the
// target rounds one position value with an `frsp` that ours omits.
// Target: event=r27, work=r31, args=r25, id=r26/r24, out=r30/r29/r28, position spills at 0x14/0x18/0x1C.
// Ours: event=r30, work=r31, args=r25, id=r28/r29, out=r27/r26/r25, position at 0x8/0xC/0x10.
USER_FUNC(_get_clauda_kiss_hit_position) {
    BattleWork* work = _battleWorkPointer;
    s32 out[3];
    s32 id[2];
    s32* args = event->args;
    BattleUnit* unit1;
    BattleUnit* unit2;
    f32 x;
    f32 y;
    f32 z;
    f32 floor;

    id[0] = evtGetValue(event, args[0]);
    id[1] = evtGetValue(event, args[1]);
    evtGetValue(event, args[2]);
    out[0] = args[3];
    out[1] = args[4];
    out[2] = args[5];
    unit1 = BattleGetUnitPtr(work, BattleTransID(event, id[0]));
    unit2 = BattleGetUnitPtr(work, BattleTransID(event, id[1]));

    BtlUnit_GetPos(unit2, &x, &y, &z);
    x += (f32)unit2->faceDirection * (unit2->kissHitOffset.x * unit2->mSizeMultiplier);
    y += unit2->kissHitOffset.y * unit2->mSizeMultiplier;
    z += unit2->kissHitOffset.z * unit2->mSizeMultiplier;
    y -= float_30_8041dcfc * unit1->mSizeMultiplier;
    floor = BattleGetFloorHeight(work, x, y, z);
    if (floor > y) {
        y = floor;
    }
    evtSetFloat(event, out[0], x);
    evtSetFloat(event, out[1], y);
    evtSetFloat(event, out[2], z);
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
