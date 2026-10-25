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
#include "math_ext.h"
// Local intrinsic form: the SDK math.h substitutes fabs as a macro, but
// also drags in extern-inline sqrtf/sqrt whose localstatics shift this unit's .rodata.
#define fabs(x) __fabs(x)
#include <string.h>
#include "battle/battle_attack_audience.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "evt/evt_audience.h"
#include "evt/evt_eff.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "party/party_motion.h"
#include "battle/battle_weapon_power.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"

extern s32 btldefaultevt_avoid_counter_event[];
extern s32 evt_audience_acrobat_notry[];
extern s32 subset_attack_ready_jump_event[];
extern BattleUnitKind unit_bomzou;
static void __makeTechMenuFunc(BattleCommand* cmd, s32* count);
static s32 init_event[63];
static s32 damage_event[16];
static s32 wait_event[26];
BattleWeapon partyWeapon_SandersNormalAttack;
BattleWeapon partyWeapon_SandersTimeBombSet;
BattleWeapon partyWeapon_SandersCounterSet;
BattleWeapon partyWeapon_SandersSuperBombAttack;
s32 partySandersAttack_FirstAttack[321];
s32 partySandersAttack_NormalAttack[970];
s32 partySandersAttack_SuperBombAttack[1116];
s32 partySandersAttack_CounterSet[201];
s32 partySandersAttack_TimeBombSet[290];
s32 _shot_bomb_event[155];
s32 sanders_appeal_event[121];
static s32 attack_audience_event[284];
s32 unk_evt_80388830[14];
static u32 defence[2];
static u32 defence_attr[2];
static u32 regist[6];
static s32 attack_event[2];
BattleUnitSetup entry_bomzou[1];
static s32 party_win_reaction[9];

// clang-format off

#define TECH_MENU_ENTRY(table)                                                             \
    cmd->weaponTable[*count].mIndex = -1;                                                 \
    cmd->weaponTable[*count].mItem = 0;                                                   \
    cmd->weaponTable[*count].mWeaponParams = (table);                                     \
    cmd->weaponTable[*count].mEnabled = 0;                                                \
    cmd->weaponTable[*count].mIcon =                                                      \
        cmd->weaponTable[*count].mWeaponParams->icon;                                     \
    cmd->weaponTable[*count].mName =                                                      \
        msgSearch(cmd->weaponTable[*count].mWeaponParams->name);                          \
    (*count)++;

USER_FUNC(_sanders_make_extra_work_area) {
    BattleWork* work = _battleWorkPointer;
    BattleUnit* unit;
    s32 id;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(work, id);
    unit->mExtraWork = BattleAlloc(0xC0);
    return EVT_RETURN_DONE;
}

static void __makeTechMenuFunc(BattleCommand* cmd, s32* count) {
    s32 techLv = partyGetTechLv(
        BattleTransPartyId(BattleGetPartyPtr(_battleWorkPointer)->currentKind));

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
}


// The declaration order below is what gives the original's callee-saved allocation
// (event r29, work r28, v0 r26, v1 r25, outX/outY/outZ r31/r30/r26).
USER_FUNC(_get_bomb_hit_position) {
    BattleUnit* unit;
    BattleWork* work = _battleWorkPointer;
    Vec pos;
    s32* args = event->args;
    s32 v0;
    s32 v1;
    s32 face;
    s32 outZ;
    s32 outX;
    s32 outY;

    v0 = evtGetValue(event, args[0]);
    v1 = evtGetValue(event, args[1]);
    evtGetValue(event, args[2]);
    outX = args[3];
    outY = args[4];
    outZ = args[5];
    BattleGetUnitPtr(work, BattleTransID(event, v0));
    unit = BattleGetUnitPtr(work, BattleTransID(event, v1));
    face = unit->faceDirection;
    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    pos.x += (f32)face * (unit->bintaHitOffset.x * unit->mSizeMultiplier);
    pos.y += unit->bintaHitOffset.y * unit->mSizeMultiplier;
    pos.z += unit->bintaHitOffset.z * unit->mSizeMultiplier;
    evtSetFloat(event, outX, pos.x);
    evtSetFloat(event, outY, pos.y);
    evtSetFloat(event, outZ, pos.z);
    return EVT_RETURN_DONE;
}


USER_FUNC(_make_counterset_weapon) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 arg0;
    BattleWeapon* area;
    s32 value;
    s32 value2;
    s32 id;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(work, id);
    arg0 = args[0];
    area = unit->mExtraWork;
    value = evtGetValue(event, arg0);
    value2 = evtGetValue(event, args[1]);
    memcpy(area, (void*)value, 0xC0);
    area->holdFastTime = value2;
    evtSetValue(event, arg0, (s32)area);
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
    speed = 0.1f * (f32)evtGetValue(event, args[2]);
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
        unit->moveAngle =
            angleABf(0.0f, 0.0f, 1.0f, 0.0f);
        unit->mFallAccel = 0.2f;
        unit->mMoveSpeedY = 0.8f * (0.9f * (speed * cosfd((f32)angleArg)));
        unit->mMoveSpeedXZ = 0.8f * (0.75f * (speed * sinfd((f32)angleArg)));
    }
    unit->mMoveCurrentPos.y += unit->mMoveSpeedY;
    unit->mMoveSpeedY -= unit->mFallAccel;
    btlMovePos(&unit->mMoveCurrentPos.x, &unit->mMoveCurrentPos.z, unit->mMoveSpeedXZ, unit->moveAngle);
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
    if (fabs(x) < 40.0f + stage->stageSize.x) {
        evtSetValue(event, outIndex, 1);
    } else {
        evtSetValue(event, outIndex, 0);
    }
    return EVT_RETURN_DONE;
}

// TODO: type
__declspec(section ".data") u32 lbl_803848D8[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
static __declspec(section ".data") u32 negone_one_374[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_803848E8[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
static __declspec(section ".data") u32 one_negone_373[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_803848F8[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
static __declspec(section ".data") u32 negone_one_360[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_80384908[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
static __declspec(section ".data") u32 one_negone_359[2] = {
    0x3F800000, 0xBF800000,
};

BattleDispPose pose_table_sanders_stay[25] = {
    { 0x09, "D_4" },
    { 0x02, "D_4" },
    { 0x03, "X_1" },
    { 0x04, "D_2" },
    { 0x05, "X_1" },
    { 0x01, "N_1" },
    { 0x1B, "D_3" },
    { 0x1C, "S_1" },
    { 0x25, "G_2" },
    { 0x1F, "G_1" },
    { 0x27, "D_1" },
    { 0x28, "W_1" },
    { 0x2A, "R_1" },
    { 0x39, "D_2" },
    { 0x38, "D_2" },
    { 0x3C, "D_4" },
    { 0x3D, "D_2" },
    { 0x41, "T_1" },
    { 0x34, "I_1" },
    { 0x43, "C_1" },
    { 0x42, "O_1" },
    { 0x40, "R_3" },
    { 0x31, "J_1A" },
    { 0x2D, "G_1" },
    { 0x45, "S_1" },
};

BattleDispPose pose_table_counter_sanders[20] = {
    { 0x01, "N_2" },
    { 0x1B, "D_3" },
    { 0x1C, "A_3" },
    { 0x25, "G_2" },
    { 0x1F, "G_1" },
    { 0x27, "D_1" },
    { 0x28, "W_1" },
    { 0x2A, "R_1" },
    { 0x39, "D_2" },
    { 0x38, "D_2" },
    { 0x3C, "D_4" },
    { 0x3D, "D_2" },
    { 0x41, "T_1" },
    { 0x34, "I_1" },
    { 0x43, "C_1" },
    { 0x42, "O_1" },
    { 0x40, "R_3" },
    { 0x31, "J_1A" },
    { 0x2D, "G_1" },
    { 0x45, "A_3" },
};

static BattleDataEntry data_table[9] = {
    { 0x05, sanders_appeal_event },
    { 0x06, attack_audience_event },
    { 0x01, __makeTechMenuFunc },
    { 0x30, btldefaultevt_Dummy },
    { 0x40, party_win_reaction },
    { 0x07, subset_attack_ready_jump_event },
    { 0x42, unk_evt_80388830 },
    { 0x2F, btldefaultevt_avoid_counter_event },
    { 0x00, NULL },
};

BattleUnitKindPart unitpartsdata_Party_Sanders[1] = {
    {
        1,                                    // partNum
        "btl_un_sanders",                     // partName
        "c_bomt_n",                           // modelName
        { 0.0f, 0.0f, 0.0f },                 // positionOffset
        { 16.0f, 24.0f, 5.0f },               // hitBaseOffset
        { 0.0f, 24.0f, 0.0f },                // hitCursorBaseOffset
        20,                                   // shadowSize
        24,                                   // field_0x32
        255,                                  // baseAlpha
        (BattleUnitDefense*)defence,          // defense
        (BattleUnitDefenseAttr*)defence_attr, // defenseAttr
        0x00000009,                           // attributes
        0,                                    // counterAttributes
        pose_table_sanders_stay,              // poseTable
    },
};

BattleUnitKind unitdata_Party_Sanders = {
    UNIT_BOBBERY,                    // mUnitType
    "btl_un_sanders",                // name
    10,                              // maxHp
    5,                               // maxFp
    5,                               // mDangerHP
    1,                               // mPerilHP
    1,                               // mLevel
    0,                               // mBonusExp
    0,                               // bonusCoinRolls
    0,                               // bonusCoinRate
    0,                               // baseCoin
    100,                             // escapeRate
    9,                               // renzokuSoftCap
    20,                              // width
    33,                              // height
    { 11, 28 },                      // statusIconOffset
    { 0.0f, 16.0f, 0.0f },           // centerOffset
    { 0, 0 },                        // hpGaugeOffset
    { 0.0f, 40.0f, 0.0f },           // talkTogeBaseOffset
    { 0.0f, 40.0f, 0.0f },           // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },            // burnFlameOffset
    0.0f,                            // kagenukeOffsetX
    0.0f,                            // kagenukeOffsetZ
    { 10.0f, 0.0f, 0.0f },           // bintaHitOffset
    { 10.0f, 21.45f, 0.0f },         // kissHitOffset
    { 0.0f, 0.0f, 0.0f },            // cutBaseOffset
    0.0f,                            // cutWidth
    0.0f,                            // cutHeight
    0,                               // turnOrder
    0,                               // turnOrderVariance
    -1,                              // swallowRate
    0,                               // swallowAttribute
    100,                             // shotDamageRate
    20,                              // itemStealParam
    { 0.0f, 0.0f, 0.0f },            // starPointDispOffset
    "SFX_PARTY_DAMAGE1",             // damageSfxName
    "SFX_MARIO_DAMAGE_FIRE1",        // fireDamageSfxName
    "SFX_MARIO_DAMAGE_ICE1",         // iceDamageSfxName
    "SFX_MARIO_DAMAGE_BIRIBIRI1",    // electricDamageSfxName
    0x02000000,                      // attributeFlags
    (BattleVulnerableStatus*)regist, // vulnStatus
    1,                               // numParts
    unitpartsdata_Party_Sanders,     // mParts
    init_event,                      // initEvtScript
    data_table,                      // dataTable
};

static EVT_BEGIN(battle_entry_event)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 250)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    WAIT_FRAMES(40)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_SetMoveSoundDataWork, -2, STRING("SFX_PARTY_BATTLE_SUBMOVE1L"), STRING("SFX_PARTY_BATTLE_SUBMOVE1R"), 0, 9, 9, 0, 0)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -2, 0)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_PARTY_BATTLE_MOVE1L"), STRING("SFX_PARTY_BATTLE_MOVE1R"), 0, 18, 18)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_PARTY_BATTLE_MOVE1L"), STRING("SFX_PARTY_BATTLE_MOVE1R"), 0, 9, 9)
    CALL(btlevtcmd_SetJumpSound, -2, STRING("SFX_PARTY_BATTLE_JUMP1"), STRING("SFX_PARTY_BATTLE_LANDING1"))
    CALL(btlevtcmd_SetUnitWork, -2, 0, -1)
    CALL(btlevtcmd_SetUnitWork, -2, 1, -1)
    CALL(btlevtcmd_SetUnitWork, -2, 2, -1)
    CALL(_sanders_make_extra_work_area)
    CALL(btlevtcmd_SetEventEntry, -2, PTR(battle_entry_event))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_CheckStatus, -2, 22, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_counter_sanders))
    ELSE
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_sanders_stay))
    END_IF
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

BattleWeapon partyWeapon_SandersFirstAttack = {
    NULL,                                     // name
    336,                                      // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPowerFromPartyAttackLv,          // hpDamageFn
    { 4, 4, 5, 5, 6, 6, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    3,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00026087,                               // special
    0x000007FB,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    partySandersAttack_FirstAttack,           // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon partyWeapon_SandersNormalAttack = {
    "btl_wn_pbm_normal",                            // name
    336,                                            // icon
    ITEM_NULL,                                      // item
    "msg_pbm_bakuhatsu",                            // description
    100,                                            // baseAccuracy
    0,                                              // baseFpCost
    0,                                              // baseSpCost
    0,                                              // superguardType
    1.0f,                                           // field_0x14
    1,                                              // mStylishCommandMultiplier
    1,                                              // field_0x19
    100,                                            // mBingoSlotChance
    50,                                             // field_0x1B
    weaponGetPowerFromPartyAttackLv,                // hpDamageFn
    { 1, 4, 2, 5, 3, 6, 0, 0 },                     // hpDamageWork
    NULL,                                           // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                     // fpDamageWork
    0x01101260,                                     // targetClassFlags
    0x21002000,                                     // targetPropertyFlags
    3,                                              // element
    0,                                              // damagePattern
    3,                                              // actionLevel
    2,                                              // field_0x6F
    "msg_ac_bakuhatsu",                             // acHelpMsg
    0x00036087,                                     // special
    0x000007FB,                                     // counterResistanceFlags
    0x80002004,                                     // targetWeightingFlags
    0,                                              // sleepChance
    0,                                              // sleepTime
    0,                                              // stopChance
    0,                                              // stopTime
    0,                                              // dizzyChance
    0,                                              // dizzyTime
    0,                                              // poisonChance
    0,                                              // poisonTime
    0,                                              // poisonStrength
    0,                                              // confuseChance
    0,                                              // confuseTime
    0,                                              // electricChance
    0,                                              // electricTime
    0,                                              // dodgyChance
    0,                                              // dodgyTime
    0,                                              // burnChance
    0,                                              // burnTime
    0,                                              // freezeChance
    0,                                              // freezeTime
    0,                                              // sizeChangeChance
    0,                                              // sizeChangeTime
    0,                                              // sizeChangeStrength
    0,                                              // atkChangeChance
    0,                                              // atkChangeTime
    0,                                              // atkChangeStrength
    0,                                              // defChangeChance
    0,                                              // defChangeTime
    0,                                              // defChangeStrength
    0,                                              // allergicChance
    0,                                              // allergicTime
    0,                                              // ohkoChance
    0,                                              // chargeStrength
    0,                                              // fastChance
    0,                                              // fastTime
    0,                                              // slowChance
    0,                                              // slowTime
    0,                                              // frightChance
    0,                                              // galeForceChance
    0,                                              // paybackTime
    0,                                              // holdFastTime
    0,                                              // invisibleChance
    0,                                              // invisibleTime
    0,                                              // hpRegenTime
    0,                                              // hpRegenStrength
    0,                                              // fpRegenTime
    0,                                              // fpRegenStrength
    partySandersAttack_NormalAttack,                // mAttackEvtScript
    { 0, 10, 10, 100, 10, 10, 10, 5, 10, 0, 0, 0 }, // field_B4
};

BattleWeapon partyWeapon_SandersTimeBombSet = {
    "btl_wn_pbm_lv1",                       // name
    337,                                    // icon
    ITEM_NULL,                              // item
    "msg_pbm_jigen_bakudan",                // description
    100,                                    // baseAccuracy
    3,                                      // baseFpCost
    0,                                      // baseSpCost
    0,                                      // superguardType
    1.0f,                                   // field_0x14
    1,                                      // mStylishCommandMultiplier
    5,                                      // field_0x19
    100,                                    // mBingoSlotChance
    50,                                     // field_0x1B
    NULL,                                   // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },             // hpDamageWork
    NULL,                                   // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },             // fpDamageWork
    0x02101260,                             // targetClassFlags
    0x20000000,                             // targetPropertyFlags
    0,                                      // element
    0,                                      // damagePattern
    3,                                      // actionLevel
    2,                                      // field_0x6F
    "msg_ac_jigen_bakudan",                 // acHelpMsg
    0x00020000,                             // special
    0x000007FF,                             // counterResistanceFlags
    0x80002004,                             // targetWeightingFlags
    0,                                      // sleepChance
    0,                                      // sleepTime
    0,                                      // stopChance
    0,                                      // stopTime
    0,                                      // dizzyChance
    0,                                      // dizzyTime
    0,                                      // poisonChance
    0,                                      // poisonTime
    0,                                      // poisonStrength
    0,                                      // confuseChance
    0,                                      // confuseTime
    0,                                      // electricChance
    0,                                      // electricTime
    0,                                      // dodgyChance
    0,                                      // dodgyTime
    0,                                      // burnChance
    0,                                      // burnTime
    0,                                      // freezeChance
    0,                                      // freezeTime
    0,                                      // sizeChangeChance
    0,                                      // sizeChangeTime
    0,                                      // sizeChangeStrength
    0,                                      // atkChangeChance
    0,                                      // atkChangeTime
    0,                                      // atkChangeStrength
    0,                                      // defChangeChance
    0,                                      // defChangeTime
    0,                                      // defChangeStrength
    0,                                      // allergicChance
    0,                                      // allergicTime
    0,                                      // ohkoChance
    0,                                      // chargeStrength
    0,                                      // fastChance
    0,                                      // fastTime
    0,                                      // slowChance
    0,                                      // slowTime
    0,                                      // frightChance
    0,                                      // galeForceChance
    0,                                      // paybackTime
    0,                                      // holdFastTime
    0,                                      // invisibleChance
    0,                                      // invisibleTime
    0,                                      // hpRegenTime
    0,                                      // hpRegenStrength
    0,                                      // fpRegenTime
    0,                                      // fpRegenStrength
    partySandersAttack_TimeBombSet,         // mAttackEvtScript
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon partyWeapon_SandersCounterSet = {
    "btl_wn_pbm_lv2",                          // name
    338,                                       // icon
    ITEM_NULL,                                 // item
    "msg_pbm_counter",                         // description
    100,                                       // baseAccuracy
    4,                                         // baseFpCost
    0,                                         // baseSpCost
    0,                                         // superguardType
    1.0f,                                      // field_0x14
    1,                                         // mStylishCommandMultiplier
    5,                                         // field_0x19
    100,                                       // mBingoSlotChance
    50,                                        // field_0x1B
    NULL,                                      // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                // hpDamageWork
    NULL,                                      // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                // fpDamageWork
    0x01004020,                                // targetClassFlags
    0x10000000,                                // targetPropertyFlags
    0,                                         // element
    0,                                         // damagePattern
    3,                                         // actionLevel
    2,                                         // field_0x6F
    "msg_ac_bom_counter",                      // acHelpMsg
    0x00020108,                                // special
    0x000007FF,                                // counterResistanceFlags
    0,                                         // targetWeightingFlags
    0,                                         // sleepChance
    0,                                         // sleepTime
    0,                                         // stopChance
    0,                                         // stopTime
    0,                                         // dizzyChance
    0,                                         // dizzyTime
    0,                                         // poisonChance
    0,                                         // poisonTime
    0,                                         // poisonStrength
    0,                                         // confuseChance
    0,                                         // confuseTime
    0,                                         // electricChance
    0,                                         // electricTime
    0,                                         // dodgyChance
    0,                                         // dodgyTime
    0,                                         // burnChance
    0,                                         // burnTime
    0,                                         // freezeChance
    0,                                         // freezeTime
    0,                                         // sizeChangeChance
    0,                                         // sizeChangeTime
    0,                                         // sizeChangeStrength
    0,                                         // atkChangeChance
    0,                                         // atkChangeTime
    0,                                         // atkChangeStrength
    0,                                         // defChangeChance
    0,                                         // defChangeTime
    0,                                         // defChangeStrength
    0,                                         // allergicChance
    0,                                         // allergicTime
    0,                                         // ohkoChance
    0,                                         // chargeStrength
    0,                                         // fastChance
    0,                                         // fastTime
    0,                                         // slowChance
    0,                                         // slowTime
    0,                                         // frightChance
    0,                                         // galeForceChance
    0,                                         // paybackTime
    1,                                         // holdFastTime
    0,                                         // invisibleChance
    0,                                         // invisibleTime
    0,                                         // hpRegenTime
    0,                                         // hpRegenStrength
    0,                                         // fpRegenTime
    0,                                         // fpRegenStrength
    partySandersAttack_CounterSet,             // mAttackEvtScript
    { 0, 5, 5, 100, 20, 5, 5, 5, 5, 5, 0, 0 }, // field_B4
};

BattleWeapon partyWeapon_SandersSuperBombAttack = {
    "btl_wn_pbm_lv3",                                // name
    339,                                             // icon
    ITEM_NULL,                                       // item
    "msg_pbm_sungoi_bakuhatsu",                      // description
    100,                                             // baseAccuracy
    9,                                               // baseFpCost
    0,                                               // baseSpCost
    0,                                               // superguardType
    1.0f,                                            // field_0x14
    2,                                               // mStylishCommandMultiplier
    5,                                               // field_0x19
    100,                                             // mBingoSlotChance
    50,                                              // field_0x1B
    weaponGetACOutputParam,                          // hpDamageFn
    { 2, 0, 0, 0, 0, 0, 0, 0 },                      // hpDamageWork
    NULL,                                            // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                      // fpDamageWork
    0x02101260,                                      // targetClassFlags
    0x20000000,                                      // targetPropertyFlags
    3,                                               // element
    0,                                               // damagePattern
    3,                                               // actionLevel
    2,                                               // field_0x6F
    "msg_ac_super_bakuhatsu",                        // acHelpMsg
    0x00026087,                                      // special
    0x000007FF,                                      // counterResistanceFlags
    0x80002004,                                      // targetWeightingFlags
    0,                                               // sleepChance
    0,                                               // sleepTime
    0,                                               // stopChance
    0,                                               // stopTime
    0,                                               // dizzyChance
    0,                                               // dizzyTime
    0,                                               // poisonChance
    0,                                               // poisonTime
    0,                                               // poisonStrength
    0,                                               // confuseChance
    0,                                               // confuseTime
    0,                                               // electricChance
    0,                                               // electricTime
    0,                                               // dodgyChance
    0,                                               // dodgyTime
    0,                                               // burnChance
    0,                                               // burnTime
    0,                                               // freezeChance
    0,                                               // freezeTime
    0,                                               // sizeChangeChance
    0,                                               // sizeChangeTime
    0,                                               // sizeChangeStrength
    0,                                               // atkChangeChance
    0,                                               // atkChangeTime
    0,                                               // atkChangeStrength
    0,                                               // defChangeChance
    0,                                               // defChangeTime
    0,                                               // defChangeStrength
    0,                                               // allergicChance
    0,                                               // allergicTime
    0,                                               // ohkoChance
    0,                                               // chargeStrength
    0,                                               // fastChance
    0,                                               // fastTime
    0,                                               // slowChance
    0,                                               // slowTime
    0,                                               // frightChance
    0,                                               // galeForceChance
    0,                                               // paybackTime
    0,                                               // holdFastTime
    0,                                               // invisibleChance
    0,                                               // invisibleTime
    0,                                               // hpRegenTime
    0,                                               // hpRegenStrength
    0,                                               // fpRegenTime
    0,                                               // fpRegenStrength
    partySandersAttack_SuperBombAttack,              // mAttackEvtScript
    { 0, 50, 50, 100, 20, 25, 25, 10, 10, 5, 0, 0 }, // field_B4
};

EVT_BEGIN(partySandersAttack_FirstAttack)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    SET_VALUE(LW(12), PTR(&partyWeapon_SandersFirstAttack))
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 250)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    WAIT_FRAMES(50)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("R_2"))
    CALL(btlevtcmd_GetStatusMg, -2, LW(6))
    MULTIPLY_FLOAT(LW(6), 20)
    CALL(_get_bomb_hit_position, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), LW(6))
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSoundDataWork, -2, STRING("SFX_BTL_THUNDERS_ATTACK1"), 0, 0, -1, -1, 0, 0)
    ADD(LW(2), 5)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(5.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, -2, 1)
    CALL(evt_snd_sfxon, STRING("SFX_BTL_THUNDERS_ATTACK2"), LW(14))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3"))
    WAIT_FRAMES(60)
    CALL(evt_snd_sfxoff, LW(14))
    CALL(evt_snd_sfxon, STRING("SFX_BTL_THUNDERS_ATTACK3"), 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHeight, -2, LW(6))
    DIVIDE(LW(6), 2)
    ADD(LW(1), LW(6))
    CALL(evt_eff64, STRING(""), STRING("bomb_n64"), 1, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_btl_camera_shake_h, 0, 5, 0, 10, 13)
    CALL(btlevtcmd_StageDispellFog)
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 131328, LW(5))
    LABEL(90)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    LABEL(92)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1A"))
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(30)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1B"))
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.5f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 40)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 90, -1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1A"))
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(8)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1B"))
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 16, -1)
    WAIT_FRAMES(20)
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(partySandersAttack_NormalAttack)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_AcGetDifficulty, LW(0))
    SUBTRACT(LW(0), 3)
    SWITCH(LW(0))
        CASE_LESS_OR_EQUAL(-3)
        SET_VALUE(LW(0), 120)
        SET_VALUE(LW(1), 60)
        CASE_EQUAL(-2)
        SET_VALUE(LW(0), 120)
        SET_VALUE(LW(1), 45)
        CASE_EQUAL(-1)
        SET_VALUE(LW(0), 120)
        SET_VALUE(LW(1), 30)
        CASE_EQUAL(0)
        SET_VALUE(LW(0), 120)
        SET_VALUE(LW(1), 25)
        CASE_EQUAL(1)
        SET_VALUE(LW(0), 120)
        SET_VALUE(LW(1), 20)
        CASE_EQUAL(2)
        SET_VALUE(LW(0), 120)
        SET_VALUE(LW(1), 12)
        CASE_DEFAULT
        SET_VALUE(LW(0), 120)
        SET_VALUE(LW(1), 6)
    END_SWITCH
    CALL(btlevtcmd_AcSetParamAll, 2, LW(0), LW(1), 0, 100, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AcSetFlag, 15)
    CALL(btlevtcmd_SetupAC, -2, 15, 1, 0)
    WAIT_FRAMES(22)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        SET_VALUE(LW(0), 5)
        LOOP(LW(0))
            CALL(btlevtcmd_AcGetOutputParam, 0, LW(1))
            IF_INT_EQUAL(LW(1), 0)
                SET_VALUE(LW(0), 5)
            ELSE
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_StartAC, 1)
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    CALL(btlevtcmd_AcGetOutputParam, 0, LW(1))
    IF_INT_EQUAL(LW(1), 2)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 20)
        CALL(btlevtcmd_SetMoveSoundDataWork, -2, STRING("SFX_BTL_THUNDERS_ATTACK1"), 0, 0, -1, -1, 0, 0)
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 20, -2, 1)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("F_1"))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_THUNDERS_DOWN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 20)
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.5f))
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
        CALL(btlevtcmd_StopAC)
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(0), 0)
        WAIT_FRAMES(60)
        CALL(evt_audience_ap_recovery)
        CALL(btlevtcmd_InviteApInfoReport)
        GOTO(95)
    END_IF
    CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(5))
    IF_INT_EQUAL(LW(5), 5)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("R_2"))
        CALL(btlevtcmd_GetStatusMg, -2, LW(5))
        MULTIPLY_FLOAT(LW(5), 35)
        CALL(_get_bomb_hit_position, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), LW(5))
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetMoveSoundDataWork, -2, STRING("SFX_BTL_THUNDERS_ATTACK1"), 0, 0, -1, -1, 0, 0)
        CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 60, -2, 1)
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("R_2"))
    CALL(btlevtcmd_GetStatusMg, -2, LW(6))
    MULTIPLY_FLOAT(LW(6), 20)
    CALL(_get_bomb_hit_position, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), LW(6))
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSoundDataWork, -2, STRING("SFX_BTL_THUNDERS_ATTACK1"), 0, 0, -1, -1, 0, 0)
    ADD(LW(2), 5)
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 90, -2, 1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 300)
    CALL(evt_snd_sfxon, STRING("SFX_BTL_THUNDERS_ATTACK2"), LW(14))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3"))
    WAIT_FRAMES(60)
    SWITCH(LW(5))
        CASE_OR_EQUAL(2)
        CASE_OR_EQUAL(3)
        CASE_OR_EQUAL(6)
        CASE_OR_EQUAL(4)
        CASE_END
        CASE_EQUAL(1)
        CASE_DEFAULT
        WAIT_MS(1000)
    END_SWITCH
    CALL(btlevtcmd_ResultAC)
    CALL(evt_snd_sfxoff, LW(14))
    CALL(evt_snd_sfxon, STRING("SFX_BTL_THUNDERS_ATTACK3"), 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHeight, -2, LW(6))
    DIVIDE(LW(6), 2)
    ADD(LW(1), LW(6))
    CALL(btlevtcmd_GetResultAC, LW(6))
    IF_BITS_SET(LW(6), 2)
        CALL(evt_eff64, STRING(""), STRING("bomb_n64"), 1, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
    ELSE
        CALL(evt_eff64, STRING(""), STRING("bomb_n64"), 0, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
    END_IF
    CALL(evt_btl_camera_shake_h, 0, 5, 0, 10, 13)
    CALL(btlevtcmd_StageDispellFog)
    IF_INT_NOT_EQUAL(LW(5), 1)
        IF_INT_EQUAL(LW(5), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(5), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(5), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        GOTO(90)
    END_IF
    CALL(btlevtcmd_GetResultAC, LW(6))
    IF_BITS_SET(LW(6), 2)
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 131328, LW(5))
        CALL(btlevtcmd_GetResultPrizeLv, LW(3), 0, LW(6))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(12), LW(6))
    ELSE
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
    END_IF
    LABEL(90)
    CALL(btlevtcmd_StopAC)
    LABEL(92)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1A"))
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(30)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1B"))
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.4f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 90, -1)
    END_SIBLING_EVENT
    WAIT_FRAMES(60)
    DELETE_EVENT(LW(15))
    CALL(btlevtcmd_GetPos, -2, LW(5), LW(6), LW(7))
    CALL(btlevtcmd_GetStageSize, EVTDAT_POINTER_MAX, LW(8), EVTDAT_POINTER_MAX)
    ADD(LW(6), LW(8))
    DIVIDE(LW(6), 2)
    CALL(btlevtcmd_SetPos, -2, LW(5), LW(6), LW(7))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.25f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 16, -1)
    END_SIBLING_EVENT
    CALL(btlevtcmd_ACRStart, -2, 0, 15, 15, 15)
    CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    IF_INT_GREATER_OR_EQUAL(LW(6), 2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("Y_1"))
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(0), 1, 0, 0, 0)
        BEGIN_SIBLING_EVENT_GET_ID(LW(15))
            CALL(evt_sub_intpl_init_float, 11, FLOAT(1.0f), FLOAT(0.6f), 8)
            LOOP(8)
                CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
                CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
                CALL(btlevtcmd_SetScale, -2, LW(0), LW(5), LW(2))
                WAIT_FRAMES(1)
            END_LOOP
            CALL(evt_sub_intpl_init_float, 11, FLOAT(0.6f), FLOAT(1.2f), 8)
            LOOP(8)
                CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
                CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
                CALL(btlevtcmd_SetScale, -2, LW(0), LW(5), LW(2))
                WAIT_FRAMES(1)
            END_LOOP
            CALL(evt_sub_intpl_init_float, 11, FLOAT(1.2f), FLOAT(1.0f), 6)
            LOOP(6)
                CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
                CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
                CALL(btlevtcmd_SetScale, -2, LW(0), LW(5), LW(2))
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            CALL(evt_sub_intpl_init_float, 11, FLOAT(1.0f), FLOAT(1.5f), 8)
            LOOP(8)
                CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
                CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
                CALL(btlevtcmd_SetScale, -2, LW(5), LW(1), LW(2))
                WAIT_FRAMES(1)
            END_LOOP
            CALL(evt_sub_intpl_init_float, 11, FLOAT(1.5f), FLOAT(0.6f), 8)
            LOOP(8)
                CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
                CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
                CALL(btlevtcmd_SetScale, -2, LW(5), LW(1), LW(2))
                WAIT_FRAMES(1)
            END_LOOP
            CALL(evt_sub_intpl_init_float, 11, FLOAT(0.6f), FLOAT(1.0f), 6)
            LOOP(6)
                CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
                CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
                CALL(btlevtcmd_SetScale, -2, LW(5), LW(1), LW(2))
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
        CALL(btlevtcmd_WaitEventEnd, LW(15))
        WAIT_FRAMES(40)
    ELSE
        CALL(evt_audience_acrobat_notry)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("F_1"))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_THUNDERS_DOWN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_FRAMES(40)
    END_IF
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(partySandersAttack_SuperBombAttack)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("Y_1"))
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_AcGetDifficulty, LW(0))
    SUBTRACT(LW(0), 3)
    SWITCH(LW(0))
        CASE_LESS_OR_EQUAL(-3)
        SET_VALUE(LW(0), 300)
        SET_VALUE(LW(1), 20)
        SET_VALUE(LW(2), 20)
        CASE_EQUAL(-2)
        SET_VALUE(LW(0), 370)
        SET_VALUE(LW(1), 18)
        SET_VALUE(LW(2), 22)
        CASE_EQUAL(-1)
        SET_VALUE(LW(0), 255)
        SET_VALUE(LW(1), 17)
        SET_VALUE(LW(2), 23)
        CASE_EQUAL(0)
        SET_VALUE(LW(0), 225)
        SET_VALUE(LW(1), 16)
        SET_VALUE(LW(2), 24)
        CASE_EQUAL(1)
        SET_VALUE(LW(0), 210)
        SET_VALUE(LW(1), 15)
        SET_VALUE(LW(2), 26)
        CASE_EQUAL(2)
        SET_VALUE(LW(0), 180)
        SET_VALUE(LW(1), 14)
        SET_VALUE(LW(2), 28)
        CASE_DEFAULT
        SET_VALUE(LW(0), 150)
        SET_VALUE(LW(1), 13)
        SET_VALUE(LW(2), 28)
    END_SWITCH
    CALL(btlevtcmd_AcSetParamAll, 1, LW(0), 178, LW(1), LW(2), 15, 46, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AcSetGaugeParam, 45, 75, 100, 100)
    CALL(btlevtcmd_AcSetFlag, 64)
    CALL(btlevtcmd_SetupAC, -2, 6, 1, 0)
    WAIT_FRAMES(22)
    CALL(btlevtcmd_StartAC, 1)
    CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(5))
    IF_INT_EQUAL(LW(5), 5)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("R_2"))
        CALL(btlevtcmd_GetStatusMg, -2, LW(6))
        MULTIPLY_FLOAT(LW(6), 35)
        CALL(_get_bomb_hit_position, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), LW(6))
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetMoveSoundDataWork, -2, STRING("SFX_BTL_THUNDERS_ATTACK1"), 0, 0, -1, -1, 0, 0)
        CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 60, -2, 1)
        CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
        RETURN
    END_IF
    BEGIN_SIBLING_EVENT
        SUBTRACT(LW(0), 85)
        WAIT_FRAMES(LW(0))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(evt_eff, STRING(""), STRING("sandars"), 5, LW(0), LW(1), LW(2), 60, 0, 0, 0, 0, 0, 0, 0)
        WAIT_FRAMES(15)
        CALL(evt_eff, STRING(""), STRING("sandars"), 5, LW(0), LW(1), LW(2), 150, 0, 0, 0, 0, 0, 0, 0)
        WAIT_FRAMES(15)
        CALL(evt_eff, STRING(""), STRING("sandars"), 5, LW(0), LW(1), LW(2), 30, 0, 0, 0, 0, 0, 0, 0)
        WAIT_FRAMES(15)
        CALL(evt_eff, STRING(""), STRING("sandars"), 5, LW(0), LW(1), LW(2), 120, 0, 0, 0, 0, 0, 0, 0)
        WAIT_FRAMES(40)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        SUBTRACT(LW(0), 85)
        WAIT_FRAMES(LW(0))
        CALL(evt_snd_sfxon, STRING("SFX_BTL_THUNDERS_ATTACK2"), LW(14))
        SET_VALUE(LW(0), 0)
        LOOP(80)
            ADD(LW(0), 1)
            IF_INT_GREATER(LW(0), 80)
                SET_VALUE(LW(0), 80)
            END_IF
            SET_VALUE(LW(1), 4096)
            MULTIPLY(LW(1), LW(0))
            DIVIDE(LW(1), 80)
            CALL(evt_snd_sfx_pit, LW(14), LW(1))
            WAIT_FRAMES(1)
        END_LOOP
        WAIT_FRAMES(3)
        CALL(evt_snd_sfxoff, LW(14))
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        LOOP(LW(0))
            CALL(btlevtcmd_AcGetOutputParam, 1, LW(0))
            ADD(LW(0), 100)
            MULTIPLY_FLOAT(LW(0), FLOAT(0.01f))
            CALL(btlevtcmd_SetScale, -2, LW(0), LW(0), FLOAT(1.0f))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    SET_VALUE(LW(13), LW(0))
    SUBTRACT(LW(13), 85)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("R_2"))
    CALL(btlevtcmd_GetStatusMg, -2, LW(6))
    MULTIPLY_FLOAT(LW(6), 20)
    CALL(_get_bomb_hit_position, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), LW(6))
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSoundDataWork, -2, STRING("SFX_BTL_THUNDERS_ATTACK1"), 0, 0, -1, -1, 0, 0)
    ADD(LW(2), 5)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), LW(13), -2, 1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 300)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3"))
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_StopAC)
    SWITCH(LW(5))
        CASE_OR_EQUAL(2)
        CASE_OR_EQUAL(3)
        CASE_OR_EQUAL(6)
        CASE_OR_EQUAL(4)
        CASE_END
        CASE_EQUAL(1)
        CASE_DEFAULT
        WAIT_MS(1000)
    END_SWITCH
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
    CALL(evt_snd_sfxon, STRING("SFX_BTL_THUNDERS_ATTACK4"), 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetResultAC, LW(6))
    IF_BITS_SET(LW(6), 2)
        CALL(evt_eff, STRING(""), STRING("sandars"), 3, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
    ELSE
        CALL(evt_eff, STRING(""), STRING("sandars"), 4, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
    END_IF
    CALL(evt_btl_camera_shake_h, 0, 10, 0, 20, 13)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    SET_VALUE(LW(10), 0)
    LABEL(10)
    CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(6))
    IF_INT_NOT_EQUAL(LW(6), 1)
        IF_INT_EQUAL(LW(6), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(6), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(6), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        GOTO(50)
    END_IF
    CALL(btlevtcmd_GetResultAC, LW(0))
    IF_BITS_SET(LW(0), 2)
        CALL(btlevtcmd_AcGetOutputParam, 2, LW(0))
        SWITCH(LW(0))
            CASE_LESS_OR_EQUAL(3)
            SET_VALUE(LW(0), -1)
            CASE_LESS_OR_EQUAL(5)
            SET_VALUE(LW(0), 0)
            CASE_LESS_OR_EQUAL(7)
            SET_VALUE(LW(0), 1)
            CASE_DEFAULT
            SET_VALUE(LW(0), 2)
        END_SWITCH
        IF_INT_GREATER_OR_EQUAL(LW(0), 0)
            CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 131328, LW(5))
            CALL(btlevtcmd_GetResultPrizeLv, LW(3), LW(0), LW(6))
            CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
            CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
            CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
            CALL(btlevtcmd_AudienceDeclareACResult, LW(0), LW(6))
        ELSE
            CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
            CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
            CALL(btlevtcmd_AudienceDeclareACResult, LW(0), -1)
        END_IF
    ELSE
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(0), -1)
    END_IF
    LABEL(50)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        ADD(LW(10), 1)
        GOTO(10)
    END_IF
    LABEL(90)
    CALL(btlevtcmd_StopAC)
    LABEL(92)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1A"))
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(30)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1B"))
        CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.4f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 90, -1)
    END_SIBLING_EVENT
    WAIT_FRAMES(60)
    DELETE_EVENT(LW(15))
    CALL(btlevtcmd_GetPos, -2, LW(5), LW(6), LW(7))
    CALL(btlevtcmd_GetStageSize, EVTDAT_POINTER_MAX, LW(8), EVTDAT_POINTER_MAX)
    ADD(LW(6), LW(8))
    DIVIDE(LW(6), 2)
    CALL(btlevtcmd_SetPos, -2, LW(5), LW(6), LW(7))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.25f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 16, -1)
    END_SIBLING_EVENT
    CALL(btlevtcmd_ACRStart, -2, 0, 15, 15, 15)
    CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    IF_INT_GREATER_OR_EQUAL(LW(6), 2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("Y_1"))
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(0), 1, 0, 0, 0)
        BEGIN_SIBLING_EVENT_GET_ID(LW(15))
            CALL(evt_sub_intpl_init_float, 11, FLOAT(1.0f), FLOAT(0.6f), 8)
            LOOP(8)
                CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
                CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
                CALL(btlevtcmd_SetScale, -2, LW(0), LW(5), LW(2))
                WAIT_FRAMES(1)
            END_LOOP
            CALL(evt_sub_intpl_init_float, 11, FLOAT(0.6f), FLOAT(1.2f), 8)
            LOOP(8)
                CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
                CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
                CALL(btlevtcmd_SetScale, -2, LW(0), LW(5), LW(2))
                WAIT_FRAMES(1)
            END_LOOP
            CALL(evt_sub_intpl_init_float, 11, FLOAT(1.2f), FLOAT(1.0f), 6)
            LOOP(6)
                CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
                CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
                CALL(btlevtcmd_SetScale, -2, LW(0), LW(5), LW(2))
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            CALL(evt_sub_intpl_init_float, 11, FLOAT(1.0f), FLOAT(1.5f), 8)
            LOOP(8)
                CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
                CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
                CALL(btlevtcmd_SetScale, -2, LW(5), LW(1), LW(2))
                WAIT_FRAMES(1)
            END_LOOP
            CALL(evt_sub_intpl_init_float, 11, FLOAT(1.5f), FLOAT(0.6f), 8)
            LOOP(8)
                CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
                CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
                CALL(btlevtcmd_SetScale, -2, LW(5), LW(1), LW(2))
                WAIT_FRAMES(1)
            END_LOOP
            CALL(evt_sub_intpl_init_float, 11, FLOAT(0.6f), FLOAT(1.0f), 6)
            LOOP(6)
                CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
                CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
                CALL(btlevtcmd_SetScale, -2, LW(5), LW(1), LW(2))
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
        CALL(btlevtcmd_WaitEventEnd, LW(15))
        WAIT_FRAMES(40)
    ELSE
        CALL(evt_audience_acrobat_notry)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("F_1"))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_THUNDERS_DOWN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_FRAMES(40)
    END_IF
    LABEL(95)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(partySandersAttack_CounterSet)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    SET_VALUE(LW(12), PTR(&partyWeapon_SandersCounterSet))
    CALL(btlevtcmd_PayWeaponCost, -2, LW(12))
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    WAIT_FRAMES(20)
    CALL(btlevtcmd_GetPos, -2, LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, -3, LW(1), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    ADD(LW(0), LW(1))
    DIVIDE(LW(0), 2)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 1, LW(0), 73, 400, LW(0), 43, 0, 30, 5)
    CALL(btlevtcmd_GetBodyId, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePose, -2, LW(9), STRING("O_1"))
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_AcSetParamAll, 3, 0, 40, 1, 0, 1, 2, 0)
    CALL(btlevtcmd_AcSetFlag, 8)
    CALL(btlevtcmd_SetupAC, -2, 18, 1, 0)
    WAIT_FRAMES(52)
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_2"))
    CALL(btlevtcmd_StartAC, 1)
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_GetResultAC, LW(0))
    CALL(btlevtcmd_StopAC)
    IF_BITS_CLEAR(LW(0), 2)
        CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
    ELSE
        CALL(btlevtcmd_AcGetOutputParam, 0, LW(0))
        SUBTRACT(LW(0), 1)
        CALL(btlevtcmd_GetResultPrizeLv, -1, LW(0), LW(6))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(12), LW(6))
    END_IF
    CALL(btlevtcmd_AcGetOutputParam, 0, LW(0))
    ADD(LW(0), 1)
    CALL(_make_counterset_weapon, LW(12), LW(0))
    WAIT_FRAMES(20)
    BEGIN_INLINE_EVENT
        WAIT_FRAMES(30)
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    END_INLINE_EVENT
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    WAIT_FRAMES(80)
    LABEL(99)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(partySandersAttack_TimeBombSet)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_bomzou), 0)
    CALL(btlevtcmd_OnAttribute, LW(3), 16777216)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    SET_VALUE(LW(10), LW(3))
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_bomzou), 0)
    CALL(btlevtcmd_OnAttribute, LW(3), 16777216)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    SET_VALUE(LW(11), LW(3))
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_bomzou), 0)
    CALL(btlevtcmd_OnAttribute, LW(3), 16777216)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    SET_VALUE(LW(12), LW(3))
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_AcGetDifficulty, LW(0))
    SUBTRACT(LW(0), 3)
    SWITCH(LW(0))
        CASE_LESS_OR_EQUAL(-3)
        SET_VALUE(LW(1), 80)
        SET_VALUE(LW(2), 12)
        CASE_EQUAL(-2)
        SET_VALUE(LW(1), 80)
        SET_VALUE(LW(2), 15)
        CASE_EQUAL(-1)
        SET_VALUE(LW(1), 80)
        SET_VALUE(LW(2), 20)
        CASE_EQUAL(0)
        SET_VALUE(LW(1), 80)
        SET_VALUE(LW(2), 22)
        CASE_EQUAL(1)
        SET_VALUE(LW(1), 80)
        SET_VALUE(LW(2), 32)
        CASE_EQUAL(2)
        SET_VALUE(LW(1), 90)
        SET_VALUE(LW(2), 50)
        CASE_DEFAULT
        SET_VALUE(LW(1), 120)
        SET_VALUE(LW(2), 80)
    END_SWITCH
    CALL(btlevtcmd_GetUnitId, -2, LW(0))
    CALL(btlevtcmd_AcSetParamAll, LW(0), 3, LW(1), LW(1), 0, 90, LW(2), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetupAC, -2, 7, 1, 0)
    CALL(btlevtcmd_AcSetFlag, 1)
    WAIT_FRAMES(20)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_4A"))
    CALL(btlevtcmd_StartAC, 1)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        SET_VALUE(LW(13), 0)
        SET_VALUE(LW(14), 5)
        LOOP(LW(14))
            CALL(btlevtcmd_AcGetOutputParam, 0, LW(0))
            IF_INT_NOT_EQUAL(LW(0), 0)
                SWITCH(LW(13))
                    CASE_EQUAL(0)
                    SET_VALUE(LW(3), LW(10))
                    CASE_EQUAL(1)
                    SET_VALUE(LW(3), LW(11))
                    CASE_DEFAULT
                    SET_VALUE(LW(3), LW(12))
                END_SWITCH
                CALL(btlevtcmd_OffAttribute, LW(3), 16777216)
                CALL(btlevtcmd_AcGetOutputParam, 2, LW(0))
                CALL(btlevtcmd_AcGetOutputParam, 1, LW(1))
                ADD(LW(1), 20)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_4B"))
                SPAWN_EVENT_GET_ID(PTR(_shot_bomb_event), LW(9))
                ADD(LW(13), 1)
                IF_INT_GREATER_OR_EQUAL(LW(13), 3)
                    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
                    CALL(btlevtcmd_WaitEventEnd, LW(9))
                    BREAK_LOOP
                END_IF
                CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_4A"))
                CALL(btlevtcmd_AcSetOutputParam, 0, 0)
            END_IF
            WAIT_FRAMES(1)
            SET_VALUE(LW(14), 5)
        END_LOOP
        DEBUG_PUT_MSG(STRING("イベント終了"))
    END_SIBLING_EVENT
    CALL(btlevtcmd_ResultAC)
    WAIT_FRAMES(60)
    CALL(btlevtcmd_StopAC)
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    LABEL(90)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(_shot_bomb_event)
    CALL(_shot_move, LW(3), LW(0), LW(1), LW(0), LW(1), LW(2), LW(5))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    SET_FLOAT_VALUE(LW(6), LW(5))
    MULTIPLY_FLOAT(LW(6), FLOAT(6.4f))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), LW(6))
    CALL(btlevtcmd_SetFallAccel, LW(3), FLOAT(0.5f))
    CALL(btlevtcmd_JumpPosition, LW(3), LW(0), LW(1), LW(2), 16, -1)
    SET_FLOAT_VALUE(LW(6), LW(5))
    MULTIPLY_FLOAT(LW(6), FLOAT(3.6f))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), LW(6))
    CALL(btlevtcmd_JumpPosition, LW(3), LW(0), LW(1), LW(2), 12, -1)
    SET_FLOAT_VALUE(LW(6), LW(5))
    MULTIPLY_FLOAT(LW(6), FLOAT(4.0f))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), LW(6))
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_GetFaceDirection, LW(3), LW(0))
        MULTIPLY(LW(0), -18)
        CALL(btlevtcmd_SetRotateOffset, LW(3), 0, 7, 0)
        LOOP(20)
            CALL(btlevtcmd_AddRotate, LW(3), 0, 0, LW(0))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_SetRotate, LW(3), 0, 0, 0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_MovePosition, LW(3), LW(0), LW(1), LW(2), 20, -1, 0)
    CALL(_judge_on_stage, LW(3), LW(5))
    IF_INT_EQUAL(LW(5), 0)
        CALL(btlevtcmd_KillUnit, LW(3), 0)
        RETURN
    END_IF
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_AnimeChangePose, LW(3), 1, STRING("B_2"))
    CALL(evt_snd_sfxon, STRING("SFX_BTL_THUNDERS_BOMB1"), LW(0))
    CALL(btlevtcmd_SetUnitWork, LW(3), 1, LW(0))
    WAIT_MS(400)
    CALL(btlevtcmd_StartWaitEvent, LW(3))
    RETURN
EVT_END()

EVT_BEGIN(sanders_appeal_event)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 66)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_APPEAL1"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(btlevtcmd_AudienceAppeal, -2)
    CALL(evt_snd_sfxon, STRING("SFX_BTL_THUNDERS_ATTACK3"), 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHeight, -2, LW(6))
    DIVIDE(LW(6), 2)
    ADD(LW(1), LW(6))
    CALL(evt_eff64, STRING(""), STRING("bomb_n64"), 0, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_btl_camera_shake_h, 0, 5, 0, 10, 13)
    CALL(btlevtcmd_StageDispellFog)
    BEGIN_INLINE_EVENT
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    END_INLINE_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1A"))
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(30)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1B"))
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.4f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 90, -1)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(attack_audience_event)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetStageSize, LW(3), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetFaceDirection, -2, LW(4))
    SET_VALUE(LW(8), LW(3))
    MULTIPLY(LW(8), LW(4))
    MULTIPLY(LW(8), -1)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_MovePosition, -2, LW(8), LW(1), LW(2), 0, -1, 0)
    WAIT_FRAMES(20)
    CALL(_get_attack_aud_no, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetStageAudienceAreaSize, LW(4), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
    SET_VALUE(LW(6), LW(4))
    MULTIPLY(LW(6), LW(5))
    MULTIPLY(LW(6), -1)
    CALL(btlevtcmd_SetPos, -2, LW(6), LW(1), LW(2))
    CALL(evt_btl_camera_set_mode, 0, 13)
    CALL(evt_btl_camera_set_homing_unit_audience, 0, -2, 1, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 100)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 30)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(evt_snd_sfxon, STRING("SFX_BTL_THUNDERS_ATTACK2"), LW(14))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3"))
    WAIT_MS(1000)
    CALL(evt_snd_sfxoff, LW(14))
    CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
    CALL(evt_snd_sfxon, STRING("SFX_BTL_THUNDERS_ATTACK3"), 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_eff64, STRING(""), STRING("bomb_n64"), 1, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_btl_camera_shake_h, 0, 10, 0, 20, 13)
    CALL(_attack_aud, LW(3), 0)
    CALL(evt_btl_camera_set_mode, 0, 3)
    BEGIN_SIBLING_EVENT
        WAIT_MS(1500)
        CALL(evt_btl_camera_set_mode, 0, 0)
        WAIT_MS(500)
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1A"))
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(30)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1B"))
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.4f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 90, -1)
    END_SIBLING_EVENT
    WAIT_FRAMES(60)
    DELETE_EVENT(LW(15))
    CALL(btlevtcmd_GetPos, -2, LW(5), LW(6), LW(7))
    CALL(btlevtcmd_GetStageSize, EVTDAT_POINTER_MAX, LW(8), EVTDAT_POINTER_MAX)
    ADD(LW(6), LW(8))
    DIVIDE(LW(6), 2)
    CALL(btlevtcmd_SetPos, -2, LW(5), LW(6), LW(7))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.25f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 16, -1)
    END_SIBLING_EVENT
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(unk_evt_80388830)
    CALL(btlevtcmd_OnOffStatus, -2, 22, 0, 0, 0)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_sanders_stay))
    RETURN
EVT_END()

// TODO: type
static u32 defence[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 defence_attr[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 regist[6] = {
    0x64646464, 0x64646464, 0x64646464, 0x64646464,
    0x64646464, 0x64640000,
};

static EVT_BEGIN(attack_event)
    RETURN
EVT_END()

BattleUnitSetup entry_bomzou[1] = {
    { &unit_bomzou, 2, 67108865, { 1000.0f, 0.0f, 1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

static EVT_BEGIN(party_win_reaction)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("Y_1"))
    WAIT_FRAMES(48)
    RETURN
EVT_END()
// clang-format on

