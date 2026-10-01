#include "unit/party/unit_party_sanders.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_cmd.h"
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_stage.h"
#include "battle/battle_unit.h"
#include "system.h"
#include "driver/msgdrv.h"
#include "mario/mario_party.h"
#include <math.h>
#include <string.h>

// float constants defined in this file's .sdata2
extern const f32 float_40_8041de58;
extern const f32 float_0p1_8041de5c;
extern const f32 float_0_8041de60;
extern const f32 float_1_8041de64;
extern const f32 float_0p2_8041de68;
extern const f32 float_0p8_8041de6c;
extern const f32 float_0p9_8041de70;
extern const f32 float_0p75_8041de74;

USER_FUNC(_judge_on_stage) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    BattleStage* stage;
    BattleUnit* unit;
    f32 z;
    f32 y;
    f32 x;
    s32 id;
    s32 val;
    s32 outIndex;

    stage = BattleStageGetPtr();
    val = evtGetValue(event, args[0]);
    id = BattleTransID(event, val);
    outIndex = args[1];
    unit = BattleGetUnitPtr(work, id);
    BtlUnit_GetPos(unit, &x, &y, &z);
    if (fabs(x) < float_40_8041de58 + *(f32*)((u8*)stage + 0xB24)) {
        evtSetValue(event, outIndex, 1);
    } else {
        evtSetValue(event, outIndex, 0);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(_shot_move) {
    BattleWork* work = _battleWorkPointer;
    s32 id;
    s32* args = event->args;
    s32 angleArg;
    s32 outX;
    s32 outY;
    s32 outZ;
    BattleUnit* unit;
    s32 outSpeed;
    f32 x;
    f32 y;
    f32 z;
    f32 speed;
    f32 floor;
    s32 val;

    val = evtGetValue(event, args[0]);
    id = BattleTransID(event, val);
    angleArg = evtGetValue(event, args[1]);
    speed = float_0p1_8041de5c * (f32)evtGetValue(event, args[2]);
    outX = args[3];
    outY = args[4];
    outZ = args[5];
    outSpeed = args[6];
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    if (isFirstCall) {
        BtlUnit_GetPos(unit, &x, &y, &z);
        unit->mMoveCurrentPos.x = x;
        unit->mMoveCurrentPos.y = y;
        unit->mMoveCurrentPos.z = z;
        *(f32*)((u8*)unit + 0x178) =
            angleABf(float_0_8041de60, float_0_8041de60, float_1_8041de64, float_0_8041de60);
        unit->mFallAccel = float_0p2_8041de68;
        unit->mMoveSpeedY = float_0p8_8041de6c * (float_0p9_8041de70 * (speed * cosfd((f32)angleArg)));
        unit->mMoveSpeedXZ = float_0p8_8041de6c * (float_0p75_8041de74 * (speed * sinfd((f32)angleArg)));
    }
    unit->mMoveCurrentPos.y += unit->mMoveSpeedY;
    unit->mMoveSpeedY -= unit->mFallAccel;
    btlMovePos(&unit->mMoveCurrentPos.x, &unit->mMoveCurrentPos.z, unit->mMoveSpeedXZ, *(f32*)((u8*)unit + 0x178));
    BtlUnit_SetPos(unit, unit->mMoveCurrentPos.x, unit->mMoveCurrentPos.y, unit->mMoveCurrentPos.z);
    floor = BattleGetFloorHeight(work, unit->mMoveCurrentPos.x, unit->mMoveCurrentPos.y, unit->mMoveCurrentPos.z);
    if (unit->mMoveCurrentPos.y > floor) {
        return 0;
    }
    BtlUnit_SetPos(unit, unit->mMoveCurrentPos.x, floor, unit->mMoveCurrentPos.z);
    evtSetValue(event, outX, (s32)unit->mMoveCurrentPos.x);
    evtSetValue(event, outY, (s32)floor);
    evtSetValue(event, outZ, (s32)unit->mMoveCurrentPos.z);
    evtSetFloat(event, outSpeed, unit->mMoveSpeedXZ);
    return 1;
}

USER_FUNC(_make_counterset_weapon) {
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
    arg0 = args[0];
    area = *(void**)((u8*)unit + 0x310);
    value = evtGetValue(event, arg0);
    value2 = evtGetValue(event, args[1]);
    memcpy(area, (void*)value, 0xC0);
    *(u8*)((u8*)area + 0xA7) = value2;
    evtSetValue(event, arg0, (s32)area);
    return EVT_RETURN_DONE;
}

// NONMATCHING (92.86%): the callee-saved map is shifted (original event=r29/work=r28/unit=r28,
// ours event=r31/work=r30/unit=r30, so outX/outY get r29/r28 instead of r31/r30) and the compiler
// hoists the addi for BtlUnit_GetPos above the second BattleGetUnitPtr; the maths and calls match.
USER_FUNC(_get_bomb_hit_position) {
    s32 outX;
    s32 outY;
    s32 outZ;
    s32* args = event->args;
    BattleWork* work = _battleWorkPointer;
    s32 v0;
    s32 v1;
    BattleUnit* unit;
    f32 z;
    f32 y;
    f32 x;
    f32 size;
    s32 face;

    v0 = evtGetValue(event, args[0]);
    v1 = evtGetValue(event, args[1]);
    evtGetValue(event, args[2]);
    outX = args[3];
    outY = args[4];
    outZ = args[5];
    BattleGetUnitPtr(work, BattleTransID(event, v0));
    unit = BattleGetUnitPtr(work, BattleTransID(event, v1));
    face = unit->faceDirection;
    BtlUnit_GetPos(unit, &x, &y, &z);
    size = unit->mSizeMultiplier;
    x += (f32)face * (unit->bintaHitOffset.x * size);
    y += unit->bintaHitOffset.y * size;
    z += unit->bintaHitOffset.z * size;
    evtSetFloat(event, outX, x);
    evtSetFloat(event, outY, y);
    evtSetFloat(event, outZ, z);
    return EVT_RETURN_DONE;
}

// attack_audience_event is defined in unit_party_chuchurina.c; the original addresses the four
// sanders weapons from one base register (attack_audience_event + 0x330, then +0x650/+0x710/+0x7D0/+0x890).
extern u8 attack_audience_event_803845A8[];
extern BattleWeapon partyWeapon_SandersNormalAttack;
extern BattleWeapon partyWeapon_SandersTimeBombSet;
extern BattleWeapon partyWeapon_SandersCounterSet;
extern BattleWeapon partyWeapon_SandersSuperBombAttack;

#define TECH_MENU_ENTRY(table)                                                             \
    cmd->mWeaponTable[*count].mIndex = -1;                                                 \
    cmd->mWeaponTable[*count].mItem = 0;                                                   \
    cmd->mWeaponTable[*count].mWeaponParams = (BattleWeapon*)(table);                      \
    cmd->mWeaponTable[*count].field_0x4 = 0;                                               \
    cmd->mWeaponTable[*count].mIcon =                                                      \
        *(u16*)((u8*)cmd->mWeaponTable[*count].mWeaponParams + 4);                         \
    cmd->mWeaponTable[*count].mName =                                                      \
        (char*)msgSearch(*(char**)((u8*)cmd->mWeaponTable[*count].mWeaponParams + 0));     \
    (*count)++;

// NONMATCHING (99.98%): the only difference is where the +0x330 sits. The target materializes it in the base
// (`lis r5, attack_audience_event_803845A8+0x330@ha` / `addi r30, r5, ...+0x330@l`) and then uses the four
// +0x650/+0x710/+0x7d0/+0x890 offsets; MWCC 2.6 folds `sym + 0x330` into the later immediates
// (+0x980/+0xa40/+0xb00/+0xbc0) and leaves addend 0 in the @ha/@l reloc, so the base register and the four
// immediates differ even though every final address is right. 0x803848D8 is the `{ -1.0f, 1.0f }` pair that is
// the last 8 bytes of unit_party_chuchurina.c's attack_audience_event; sanders' `.data` split starts at
// 0x803848E0, so the pair has no symbol of its own and dtk names the target reference as
// attack_audience_event+0x330. The real fix is tools/split_scan.py's fix for sanders (move the
// chuchurina/sanders `.data` boundary down 8 bytes and let sanders own the pair), same as the nokotarou fix in
// Grind 28. unit_party_christine.c has the same shape (it scores 100 with an in-file `lbl_80390B70` base); the
// matching siblings (clauda/vivian/chuchurina/nokotarou/yoshi) have their own `lbl_XXXX` pair symbol directly
// on the base.
static void __makeTechMenuFunc(BattleWorkCommand* cmd, s32* count) {
#ifdef MUST_MATCH
    u8* weaponBase = attack_audience_event_803845A8 + 0x330;
#endif
    s32 techLv = partyGetTechLv(
        BattleTransPartyId(BattleGetPartyPtr(_battleWorkPointer)->currentKind));

#ifdef MUST_MATCH
    TECH_MENU_ENTRY(weaponBase + 0x650);
    if (techLv >= 0) {
        TECH_MENU_ENTRY(weaponBase + 0x710);
    }
    if (techLv >= 1) {
        TECH_MENU_ENTRY(weaponBase + 0x7D0);
    }
    if (techLv >= 2) {
        TECH_MENU_ENTRY(weaponBase + 0x890);
    }
#else
    TECH_MENU_ENTRY(&partyWeapon_SandersNormalAttack);
    if (techLv >= 0) {
        TECH_MENU_ENTRY(&partyWeapon_SandersTimeBombSet);
    }
    if (techLv >= 1) {
        TECH_MENU_ENTRY(&partyWeapon_SandersCounterSet);
    }
    if (techLv >= 2) {
        TECH_MENU_ENTRY(&partyWeapon_SandersSuperBombAttack);
    }
#endif
}

USER_FUNC(_sanders_make_extra_work_area) {
    BattleWork* work = _battleWorkPointer;
    BattleUnit* unit;
    s32 id;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(work, id);
    *(void**)((u8*)unit + 0x310) = BattleAlloc(0xC0);
    return EVT_RETURN_DONE;
}
