#include "unit_faker_mario.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_msg.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"
#include "battle/battle_sub.h"
#include "manager/evtmgr_cmd.h"

#include <string.h>

#pragma pool_data off

#pragma force_active on
extern BattleUnitSetup entryunit_party[7];
extern BattleUnitKind unit_gullible_christine;
extern BattleUnitKind unit_gullible_clauda;
extern BattleUnitKind unit_gullible_nokotarou;
extern BattleUnitKind unit_gullible_yoshi;
static USER_FUNC(_anime_load_wait);
static USER_FUNC(_anime_load);
static USER_FUNC(vivian_comeback);
static BattleVulnerableStatus regist_fmario;
static BattleDataEntry data_table_fmario[2];
static BattleUnitKindPart parts_fmario[1];
static s32 init_event_fmario[189];
s32 fmario_entry_event[24];
static s32 dead_event_fmario[198];
static s32 damage_event_fmario[22];
s32 phase_event_fmario[400];
static s32 attack_event_fmario[60];
static s32 marioAttackEvent_NormalJump[344];
static s32 marioAttackEvent_KururinJump[469];
static s32 marioAttackEvent_NormalHammer[299];
static s32 marioAttackEvent_KaitenHammer[299];
s32 party_change_event[196];
static s32 wait_event_fmario[6];

// clang-format off
extend_data(76,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(77,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_faker_mario = {
    UNIT_DOOPLISS_CHAPTER4_FIGHT2, // mUnitType
    "btl_un_rampell",              // name
    40,                            // maxHp
    0,                             // maxFp
    1,                             // mDangerHP
    1,                             // mPerilHP
    54,                            // mLevel
    0,                             // mBonusExp
    0,                             // bonusCoinRolls
    100,                           // bonusCoinRate
    0,                             // baseCoin
    70,                            // field_0x13
    7,                             // field_0x14
    24,                            // width
    40,                            // height
    { 12, 40 },                    // statusIconOffset
    { 0.0f, 20.0f, 0.0f },         // centerOffset
    { 0, 0 },                      // hpGaugeOffset
    { 0.0f, 40.0f, 0.0f },         // talkTogeBaseOffset
    { 12.0f, 0.0f, -10.0f },       // field_0x3C
    { 0.0f, 0.0f, 0.0f },          // burnFlameOffset
    0.0f,                          // field_0x54
    0.0f,                          // field_0x58
    { 12.0f, 0.0f, 0.0f },         // bintaHitOffset
    { 12.0f, 32.0f, 0.0f },        // kissHitOffset
    { 0.0f, 20.0f, 0.0f },         // cutBaseOffset
    24.0f,                         // cutWidth
    40.0f,                         // cutHeight
    0,                             // turnOrder
    0,                             // turnOrderVariance
    0,                             // swallowRate
    2,                             // swallowAttribute
    100,                           // field_0x8C
    20,                            // itemStealParam
    { 0.0f, 0.0f, 0.0f },          // starPointDispOffset
    "SFX_MARIO_DAMAGE1",           // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",        // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",         // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1",    // electricDamageSfxName
    0,                             // attributeFlags
    &regist_fmario,                // vulnStatus
    1,                             // numParts
    parts_fmario,                  // mParts
    init_event_fmario,             // initEvtScript
    data_table_fmario,             // dataTable
};

static BattleUnitDefense defence_fmario = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr_fmario = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist_fmario = {
    50,  // sleepRate
    50,  // stopRate
    70,  // dizzyRate
    50,  // poisonRate
    50,  // confuseRate
    50,  // electricRate
    100, // burnRate
    40,  // freezeRate
    100, // hugeRate
    70,  // tinyRate
    100, // attackUpRate
    70,  // attackDownRate
    100, // defenseUpRate
    90,  // defenseDownRate
    50,  // allergicRate
    0,   // frightRate
    0,   // galeForceRate
    100, // fastRate
    50,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    3,   // instaKillRate
};

static BattleDispPose pose_table_fmario_stay[15] = {
    { 0x01, "EM_N_1" },
    { 0x02, "EM_Y_2" },
    { 0x09, "EM_Y_2" },
    { 0x05, "EM_K_1" },
    { 0x04, "EM_K_1" },
    { 0x03, "EM_K_1" },
    { 0x1C, "EM_S_1" },
    { 0x27, "EM_D_1" },
    { 0x32, "EM_A_1" },
    { 0x2A, "EM_R_1" },
    { 0x28, "EM_W_1" },
    { 0x38, "EM_D_3" },
    { 0x39, "EM_D_3" },
    { 0x41, "EM_T_1" },
    { 0x45, "EM_S_1" },
};

static BattleDataEntry data_table_fmario[2] = {
    { 0x31, dead_event_fmario },
    { 0x00, NULL },
};

static BattleUnitKindPart parts_fmario[1] = {
    {
        1,                      // partNum
        "btl_un_rampell",       // partName
        "c_mario",              // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 3.0f, 27.0f, 0.0f },  // hitBaseOffset
        { 0.0f, 30.0f, 0.0f },  // hitCursorBaseOffset
        20,                     // field_0x30
        30,                     // field_0x32
        255,                    // baseAlpha
        &defence_fmario,        // defense
        &defence_attr_fmario,   // defenseAttr
        0x00000009,             // attributes
        0,                      // counterAttributes
        pose_table_fmario_stay, // poseTable
    },
};

static BattleWeapon weapon_masao_jump = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    1,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 2, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01001260,                               // targetClassFlags
    0x20001004,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00015807,                               // special
    0x0000000C,                               // counterResistanceFlags
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
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

static BattleWeapon weapon_masao_hammer = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    1,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 4, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01001260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00010807,                               // special
    0x0000020C,                               // counterResistanceFlags
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
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

static BattleWeapon weapon_masao_kururin_jump = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    1,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 2, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01001260,                               // targetClassFlags
    0x20001004,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00015807,                               // special
    0x0000000C,                               // counterResistanceFlags
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
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

static BattleWeapon weapon_masao_kaiten_hammer = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    1,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 4, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01001260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00010807,                               // special
    0x0000020C,                               // counterResistanceFlags
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
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleUnitSetup entry_gullible_christine[1] = {
    { &unit_gullible_christine, 1, 0x04000004, { 1000.0f, 0.0f, 1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleUnitSetup entry_gullible_nokotarou[1] = {
    { &unit_gullible_nokotarou, 1, 0x04000004, { 1000.0f, 0.0f, 1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleUnitSetup entry_gullible_clauda[1] = {
    { &unit_gullible_clauda, 1, 0x04000004, { 1000.0f, 0.0f, 1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleUnitSetup entry_gullible_yoshi[1] = {
    { &unit_gullible_yoshi, 1, 0x04000004, { 1000.0f, 0.0f, 1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

static EVT_BEGIN(mario_zoom)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, -110, 65, 332, -110, 30, -340, 30, 11)
    WAIT_FRAMES(37)
    RETURN
EVT_END()

static EVT_BEGIN(enemy_zoom)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, 115, 65, 332, 115, 30, -340, 30, 11)
    WAIT_FRAMES(37)
    RETURN
EVT_END()

static EVT_BEGIN(fmario_zoom)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, 90, 65, 332, 90, 30, -340, 30, 11)
    WAIT_FRAMES(37)
    RETURN
EVT_END()

static EVT_BEGIN(gparty_zoom)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, 130, 65, 332, 130, 30, -340, 30, 11)
    WAIT_FRAMES(37)
    RETURN
EVT_END()

// Source order of the six functions is the reverse of their order in the asm
// (-inline auto,deferred emits functions in reverse source order).

static USER_FUNC(vivian_comeback) {
    PouchData* pouch = pouchGetPtr();

    pouch->partyData[6].flags &= ~2;
    return EVT_RETURN_YIELD;
}

USER_FUNC(_gullible_party_info_init) {
    PouchData* pouch = pouchGetPtr();
    BattleUnit* unit;
    s32 id;
    BattleUnitParam* info;
    s32 fp;
    s32 i;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    info = (BattleUnitParam*)BattleAlloc(0xC0);
    unit->mExtraWork = info;
    memset(info, 0, 0xC0);
    for (i = 0; i < 4; i++) {
        BattleUnitParam* p = &info[i];
        p->mSizeMultiplier = 1.0f;
        p->mStatusFlags = 0;
        memset(p->mStatus.data, 0, 0x1E);
        switch (i) {
            case 0:
                p->maxHp = p->currentHp = pouch->partyData[1].baseMaxHP;
                break;
            case 1:
                p->maxHp = p->currentHp = pouch->partyData[2].baseMaxHP;
                break;
            case 2:
                p->maxHp = p->currentHp = pouch->partyData[5].baseMaxHP;
                break;
            default:
                p->maxHp = p->currentHp = pouch->partyData[4].baseMaxHP;
                break;
        }
        fp = pouch->baseMaxFP;
        p->maxFp = p->currentFp = fp;
    }
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(init_event_fmario)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event_fmario))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event_fmario))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event_fmario))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event_fmario))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetEventEntry, -2, PTR(fmario_entry_event))
    CALL(btlevtcmd_GetUnitId, -2, GW(16))
    SET_VALUE(GW(17), -1)
    CALL(_gullible_party_info_init)
    CALL(evt_sub_random, 3, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        SET_VALUE(LW(12), PTR(entry_gullible_christine))
        CASE_EQUAL(1)
        SET_VALUE(LW(12), PTR(entry_gullible_nokotarou))
        CASE_EQUAL(2)
        SET_VALUE(LW(12), PTR(entry_gullible_clauda))
        CASE_DEFAULT
        SET_VALUE(LW(12), PTR(entry_gullible_yoshi))
    END_SWITCH
    SET_VALUE(GW(18), 0)
    SET_VALUE(LW(12), PTR(entry_gullible_christine))
    CALL(btlevtcmd_SpawnUnit, LW(3), LW(12), 0)
    SET_VALUE(GW(17), LW(3))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 40)
    ADD(LW(2), 10)
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), LW(1), LW(2))
    CALL(_load_param, -2, LW(3))
    SET_VALUE(GW(20), 0)
    SET_VALUE(GW(19), 0)
    CALL(btlevtcmd_GetMaxHp, -2, LW(0))
    SET_VALUE(LW(1), LW(0))
    MULTIPLY(LW(1), 66)
    DIVIDE(LW(1), 100)
    CALL(btlevtcmd_SetUnitWork, -2, 1, LW(1))
    SET_VALUE(LW(1), LW(0))
    MULTIPLY(LW(1), 50)
    DIVIDE(LW(1), 100)
    CALL(btlevtcmd_SetUnitWork, -2, 2, LW(1))
    SET_VALUE(LW(1), LW(0))
    MULTIPLY(LW(1), 25)
    DIVIDE(LW(1), 100)
    CALL(btlevtcmd_SetUnitWork, -2, 3, LW(1))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    SET_VALUE(GW(21), 0)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_BOSS_NM_MOVE1L"), STRING("SFX_BOSS_NM_MOVE1R"), 0, 20, 20)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_BOSS_NM_MOVE1L"), STRING("SFX_BOSS_NM_MOVE1R"), 0, 10, 10)
    CALL(btlevtcmd_SetJumpSound, -2, STRING("SFX_BOSS_NM_JUMP1"), STRING("SFX_BOSS_NM_LANDING1"))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(fmario_entry_event)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    WAIT_MS(2000)
    CALL_CHILD(fmario_zoom)
    CALL(evt_msg_print, 2, STRING("stg4_jin_22"), 0, -2)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(250)
    RETURN
EVT_END()

EVT_BEGIN(dead_event_fmario_down)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_S_1"))
    WAIT_MS(300)
    CALL(btlevtcmd_SetTalkPose, -2, STRING("EM_T_1"))
    CALL(btlevtcmd_SetStayPose, -2, STRING("EM_S_1"))
    CALL(evt_msg_print, 2, STRING("stg4_jin_26_05"), 0, -2)
    WAIT_MS(300)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_K_1"))
    WAIT_FRAMES(152)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_NM_DOWN2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_NM_LAND_DAMAGE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_D_2"))
    WAIT_MS(1500)
    RETURN
EVT_END()

USER_FUNC(_load_param) {
    BattleWork* work = _battleWorkPointer;
    BattleUnit* src;
    BattleUnit* dst;
    BattleUnitParam* info;
    BattleUnitParam* slot;
    s32* args = event->args;
    s32 id0;
    s32 id1;

    id0 = evtGetValue(event, args[0]);
    id1 = evtGetValue(event, args[1]);
    src = BattleGetUnitPtr(work, BattleTransID(event, id0));
    dst = BattleGetUnitPtr(work, BattleTransID(event, id1));
    info = (BattleUnitParam*)src->mExtraWork;
    switch (dst->currentKind) {
        case UNIT_GOOMBELLA_CHAPTER4:
            slot = info;
            break;
        case UNIT_KOOPS_CHAPTER4:
            slot = info + 1;
            break;
        case UNIT_FLURRIE_CHAPTER4:
            slot = info + 2;
            break;
        case UNIT_YOSHI_CHAPTER4:
            slot = info + 3;
            break;
    }
    dst->mSizeMultiplier = slot->mSizeMultiplier;
    dst->mStatusFlags = slot->mStatusFlags;
    *(BattleUnitStatus*)&dst->mSleepTurns = slot->mStatus;
    dst->maxHp = slot->maxHp;
    dst->currentHp = slot->currentHp;
    dst->maxFp = slot->maxFp;
    dst->currentFp = slot->currentFp;
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(dead_event_fmario)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, -2, LW(3), LW(4), LW(5))
    SET_VALUE(LW(6), 1)
    IF_INT_EQUAL(LW(0), LW(3))
        IF_INT_EQUAL(LW(1), LW(4))
            IF_INT_EQUAL(LW(2), LW(5))
                SET_VALUE(LW(6), 0)
            END_IF
        END_IF
    END_IF
    IF_INT_EQUAL(LW(6), 1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_W_1"))
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
        CALL(btlevtcmd_ResetFaceDirection, -2)
    END_IF
    WAIT_FRAMES(1)
    CALL(btlevtcmd_WaitAttackEnd)
    IF_INT_EQUAL(GW(21), 0)
        SET_VALUE(GW(21), 1)
        CALL(vivian_comeback)
        CALL(btlevtcmd_SpawnUnit, LW(3), PTR(&entryunit_party[4]), 0)
        CALL(btlevtcmd_SetHomePos, LW(3), FLOAT(-140.0f), FLOAT(0.0f), FLOAT(5.0f))
        CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetUnitWork, LW(3), 0, LW(0))
        CALL(btlevtcmd_SetUnitWork, LW(3), 1, LW(1))
        CALL(btlevtcmd_SetUnitWork, LW(3), 2, LW(2))
        WAIT_FRAMES(1)
    END_IF
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    SWITCH(GW(20))
        CASE_EQUAL(0)
        CALL_CHILD(fmario_zoom)
        CALL_CHILD(dead_event_fmario_down)
        WAIT_MS(300)
        CALL_CHILD(gparty_zoom)
        SWITCH(GW(18))
            CASE_EQUAL(0)
            CALL(evt_msg_print, 2, STRING("stg4_jin_26_05_kur"), 0, GW(17))
            CASE_EQUAL(1)
            CALL(evt_msg_print, 2, STRING("stg4_jin_26_05_nok"), 0, GW(17))
            CASE_EQUAL(2)
            CALL(evt_msg_print, 2, STRING("stg4_jin_26_05_win"), 0, GW(17))
            CASE_EQUAL(3)
            CALL(evt_msg_print, 2, STRING("stg4_jin_26_05_yos"), 0, GW(17))
        END_SWITCH
        WAIT_MS(200)
        CASE_EQUAL(1)
        CALL_CHILD(enemy_zoom)
        CALL_CHILD(dead_event_fmario_down)
        WAIT_MS(200)
        CASE_EQUAL(2)
        CALL_CHILD(fmario_zoom)
        CALL_CHILD(dead_event_fmario_down)
        WAIT_MS(200)
    END_SWITCH
    SET_VALUE(GW(19), 2)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    SET_VALUE(LW(10), -2)
    CALL_CHILD(subsetevt_dead_core_nospin_norotate)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event_fmario)
    CALL(btlevtcmd_CheckDamageCode, -2, 1024, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(GW(19), 1)
    END_IF
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

EVT_BEGIN(phase_event_fmario)
    CALL(btlevtcmd_CheckPhase, LW(0), 67108865)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_get_turn, LW(0))
        IF_INT_EQUAL(LW(0), 3)
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            CALL(evt_msg_print, 0, STRING("stg4_jin_23"), 0, 0)
            WAIT_MS(200)
            SET_VALUE(GW(21), 1)
            CALL(vivian_comeback)
            CALL(btlevtcmd_SpawnUnit, LW(3), PTR(&entryunit_party[4]), 0)
            CALL(btlevtcmd_SetHomePos, LW(3), FLOAT(-140.0f), FLOAT(0.0f), FLOAT(5.0f))
            CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
            CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetUnitWork, LW(3), 0, LW(0))
            CALL(btlevtcmd_SetUnitWork, LW(3), 1, LW(1))
            CALL(btlevtcmd_SetUnitWork, LW(3), 2, LW(2))
            WAIT_FRAMES(1)
            CALL(btlevtcmd_GetMarioId, LW(3), LW(5))
            CALL(btlevtcmd_GetBodyId, LW(5), LW(6))
            CALL(btlevtcmd_StopWaitEvent, LW(3))
            CALL(btlevtcmd_StopWaitEvent, LW(5))
            WAIT_MS(700)
            SPAWN_EVENT(mario_zoom)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_VIVIAN_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_SetPos, LW(3), -140, -40, 5)
            CALL(btlevtcmd_DivePosition, LW(3), -140, 0, 5, 60, 0, 0, 4, 0)
            CALL(btlevtcmd_AnimeChangePose, LW(5), LW(6), STRING("M_Z_1"))
            BEGIN_SIBLING_EVENT_GET_ID(LW(0))
                CALL(evt_sub_intpl_init, 0, 0, 180, 15)
                LOOP(16)
                    CALL(evt_sub_intpl_get_value)
                    CALL(btlevtcmd_SetRotate, -3, 0, LW(0), 0)
                    WAIT_FRAMES(1)
                END_LOOP
            END_SIBLING_EVENT
            CALL(btlevtcmd_WaitEventEnd, LW(0))
            CALL(evt_msg_print, 2, STRING("stg4_jin_24"), 0, LW(3))
            WAIT_MS(200)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_FIND1_3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetPos, LW(5), LW(7), LW(8), LW(10))
            CALL(btlevtcmd_JumpSetting, LW(5), 12, FLOAT(1.0f), FLOAT(1.2f))
            CALL(btlevtcmd_JumpPosition, LW(5), LW(7), LW(8), LW(10), 0, -1)
            WAIT_MS(1000)
            CALL(evt_msg_print, 2, STRING("stg4_jin_25"), 0, LW(3))
            WAIT_MS(1000)
            BEGIN_SIBLING_EVENT_GET_ID(LW(0))
                CALL(evt_sub_intpl_init, 0, 180, 0, 15)
                LOOP(16)
                    CALL(evt_sub_intpl_get_value)
                    CALL(btlevtcmd_SetRotate, -3, 0, LW(0), 0)
                    WAIT_FRAMES(1)
                END_LOOP
            END_SIBLING_EVENT
            CALL(btlevtcmd_WaitEventEnd, LW(0))
            WAIT_MS(200)
            CALL(btlevtcmd_AnimeChangePose, LW(5), LW(6), STRING("M_I_Y"))
            WAIT_MS(1000)
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            CALL(evt_btl_camera_set_mode, 0, 0)
            CALL(btlevtcmd_StartWaitEvent, LW(3))
        END_IF
    END_IF
    CALL(btlevtcmd_CheckPhase, LW(0), 67108867)
    IF_INT_NOT_EQUAL(LW(0), 0)
        IF_INT_NOT_EQUAL(GW(20), 0)
            CALL_CHILD(party_change_event)
        END_IF
        CALL(btlevtcmd_GetHp, -2, LW(0))
        CALL(btlevtcmd_GetUnitWork, -2, 0, LW(2))
        IF_INT_NOT_EQUAL(GW(20), 0)
            IF_INT_EQUAL(LW(2), 0)
                CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
                SET_VALUE(LW(2), 1)
            END_IF
        END_IF
        SWITCH(LW(2))
            CASE_EQUAL(0)
            CALL(btlevtcmd_GetUnitWork, -2, 1, LW(1))
            IF_INT_LESS(LW(0), LW(1))
                CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
                CALL(btlevtcmd_StatusWindowOnOff, 0)
                CALL_CHILD(enemy_zoom)
                SWITCH(GW(18))
                    CASE_EQUAL(0)
                    CALL(evt_msg_print, 2, STRING("stg4_jin_26_kur"), 0, GW(17))
                    CASE_EQUAL(1)
                    CALL(evt_msg_print, 2, STRING("stg4_jin_26_nok"), 0, GW(17))
                    CASE_EQUAL(2)
                    CALL(evt_msg_print, 2, STRING("stg4_jin_26_win"), 0, GW(17))
                    CASE_EQUAL(3)
                    CALL(evt_msg_print, 2, STRING("stg4_jin_26_yos"), 0, GW(17))
                END_SWITCH
                WAIT_MS(200)
                CALL(btlevtcmd_StatusWindowOnOff, 1)
            END_IF
            CASE_EQUAL(1)
            CALL(btlevtcmd_GetUnitWork, -2, 2, LW(1))
            IF_INT_LESS(LW(0), LW(1))
                CALL(btlevtcmd_SetUnitWork, -2, 0, 2)
                CALL(btlevtcmd_StatusWindowOnOff, 0)
                CALL_CHILD(fmario_zoom)
                CALL(evt_msg_print, 2, STRING("stg4_jin_26_01"), 0, -2)
                WAIT_MS(200)
                CALL(btlevtcmd_StatusWindowOnOff, 1)
            END_IF
            CASE_EQUAL(2)
            CALL(btlevtcmd_GetUnitWork, -2, 3, LW(1))
            IF_INT_LESS(LW(0), LW(1))
                CALL(btlevtcmd_SetUnitWork, -2, 0, 3)
                CALL(btlevtcmd_StatusWindowOnOff, 0)
                CALL_CHILD(fmario_zoom)
                CALL(evt_msg_print, 2, STRING("stg4_jin_26_02"), 0, -2)
                WAIT_MS(200)
                CALL(btlevtcmd_StatusWindowOnOff, 1)
            END_IF
        END_SWITCH
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(attack_event_fmario)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_DrawLots, LW(0), 4, 50, 50, 20, 20)
    SWITCH(LW(0))
        CASE_EQUAL(0)
        SET_VALUE(LW(9), PTR(&weapon_masao_jump))
        CALL_CHILD(marioAttackEvent_NormalJump)
        CASE_EQUAL(1)
        SET_VALUE(LW(9), PTR(&weapon_masao_hammer))
        CALL_CHILD(marioAttackEvent_NormalHammer)
        CASE_EQUAL(2)
        SET_VALUE(LW(9), PTR(&weapon_masao_kururin_jump))
        CALL_CHILD(marioAttackEvent_KururinJump)
        CASE_EQUAL(3)
        SET_VALUE(LW(9), PTR(&weapon_masao_kaiten_hammer))
        CALL_CHILD(marioAttackEvent_KaitenHammer)
    END_SWITCH
    LABEL(90)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(jump_attack_voice)
    CALL(evt_sub_random, 2, LW(15))
    SWITCH(LW(15))
        CASE_EQUAL(0)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_VOICE_NM_AC1_1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CASE_EQUAL(1)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_VOICE_NM_AC1_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CASE_EQUAL(2)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_VOICE_NM_AC1_5"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(marioAttackEvent_NormalJump)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    CALL(btlevtcmd_GetTakeoffPosition, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(6))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(6))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    ADD(LW(2), 5)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_J_1B"))
    CALL_CHILD(jump_attack_voice)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_NM_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), 36, 0, -1)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    SWITCH(LW(5))
        CASE_OR_EQUAL(4)
        GOTO(90)
        CASE_END
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(3), 38)
        GOTO(90)
        CASE_END
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(3), 39)
        GOTO(90)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(3), 40)
        GOTO(90)
        CASE_EQUAL(1)
        GOTO(91)
        CASE_END
        CASE_DEFAULT
        GOTO(98)
        CASE_END
    END_SWITCH
    LABEL(90)
    CALL(btlevtcmd_JumpContinue, -2)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_NM_DOWN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_D_2"))
    WAIT_FRAMES(40)
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_J_1B"))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 60)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_W_1"))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(marioAttackEvent_KururinJump)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    CALL(btlevtcmd_GetTakeoffPosition, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(6))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(6))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    ADD(LW(2), 5)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_J_1B"))
    CALL_CHILD(jump_attack_voice)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_NM_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), 36, 0, -1)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    SWITCH(LW(5))
        CASE_OR_EQUAL(4)
        GOTO(90)
        CASE_END
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(3), 38)
        GOTO(90)
        CASE_END
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(3), 39)
        GOTO(90)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(3), 40)
        GOTO(90)
        CASE_EQUAL(1)
        GOTO(91)
        CASE_END
        CASE_DEFAULT
        GOTO(98)
        CASE_END
    END_SWITCH
    LABEL(90)
    CALL(btlevtcmd_JumpContinue, -2)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_NM_DOWN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_D_2"))
    WAIT_FRAMES(40)
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    BEGIN_SIBLING_EVENT_GET_ID(LW(14))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.5f))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        ADD(LW(2), 5)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_J_1B"))
        CALL_CHILD(jump_attack_voice)
        CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), 36, 0, -1)
    END_SIBLING_EVENT
    WAIT_FRAMES(15)
    DELETE_EVENT(LW(14))
    CALL(btlevtcmd_GetFaceDirection, -2, LW(15))
    MULTIPLY(LW(15), -30)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_Z_2"))
    CALL(btlevtcmd_SetRotateOffset, -2, 0, 12, 0)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    WAIT_FRAMES(15)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_NM_JUMP_KURURIN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    LOOP(12)
        CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(15))
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_SetRotateOffset, -2, 0, 0, 0)
    WAIT_FRAMES(15)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 4, 0, 0, 0, -1)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_J_1B"))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 27)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_W_1"))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(marioAttackEvent_NormalHammer)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 100)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 24)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    ADD(LW(2), 5)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_A_1A"))
    WAIT_MS(66)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_NM_HAMMER_FURU2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_MS(233)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_A_1B"))
    WAIT_MS(500)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_A_1C"))
    WAIT_MS(50)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_NM_HAMMER_DON2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_MS(100)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    SWITCH(LW(5))
        CASE_OR_EQUAL(4)
        GOTO(90)
        CASE_END
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(3), 38)
        GOTO(90)
        CASE_END
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(3), 39)
        GOTO(90)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(3), 40)
        GOTO(90)
        CASE_EQUAL(1)
        GOTO(91)
        CASE_END
        CASE_DEFAULT
        GOTO(98)
        CASE_END
    END_SWITCH
    LABEL(90)
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    LABEL(98)
    WAIT_MS(700)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_S_1"))
    WAIT_MS(200)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_W_1"))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(marioAttackEvent_KaitenHammer)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 100)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 24)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    ADD(LW(2), 5)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_A_1A"))
    WAIT_MS(66)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_NM_HAMMER_FURU2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_MS(233)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_A_1B"))
    WAIT_MS(500)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_A_1C"))
    WAIT_MS(50)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_NM_HAMMER_DON2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_MS(100)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    SWITCH(LW(5))
        CASE_OR_EQUAL(4)
        GOTO(90)
        CASE_END
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(3), 38)
        GOTO(90)
        CASE_END
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(3), 39)
        GOTO(90)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(3), 40)
        GOTO(90)
        CASE_EQUAL(1)
        GOTO(91)
        CASE_END
        CASE_DEFAULT
        GOTO(98)
        CASE_END
    END_SWITCH
    LABEL(90)
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    LABEL(98)
    WAIT_MS(700)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_S_1"))
    WAIT_MS(200)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_W_1"))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

USER_FUNC(_save_param) {
    BattleWork* work = _battleWorkPointer;
    BattleUnit* dst;
    BattleUnit* src;
    BattleUnitParam* info;
    BattleUnitParam* slot;
    s32* args = event->args;
    s32 id0;
    s32 id1;

    id0 = evtGetValue(event, args[0]);
    id1 = evtGetValue(event, args[1]);
    dst = BattleGetUnitPtr(work, BattleTransID(event, id0));
    src = BattleGetUnitPtr(work, BattleTransID(event, id1));
    info = (BattleUnitParam*)dst->mExtraWork;
    switch (src->currentKind) {
        case UNIT_GOOMBELLA_CHAPTER4:
            slot = info;
            break;
        case UNIT_KOOPS_CHAPTER4:
            slot = info + 1;
            break;
        case UNIT_FLURRIE_CHAPTER4:
            slot = info + 2;
            break;
        case UNIT_YOSHI_CHAPTER4:
            slot = info + 3;
            break;
    }
    slot->mSizeMultiplier = src->mSizeMultiplier;
    slot->mStatusFlags = src->mStatusFlags;
    slot->mStatus = *(BattleUnitStatus*)&src->mSleepTurns;
    slot->maxHp = src->maxHp;
    slot->currentHp = src->currentHp;
    slot->maxFp = src->maxFp;
    slot->currentFp = src->currentFp;
    return EVT_RETURN_DONE;
}

// Copy propagation off: with it on, _anime_load and _anime_load_wait both
// allocate the BattleGetUnitPtr scratch register differently from the target.
#pragma opt_propagation off
static USER_FUNC(_anime_load) {
    BattleWork* work = _battleWorkPointer;
    BattleUnit* unit;
    s32* args = event->args;
    s32 id;
    s32 partyId;

    id = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(work, BattleTransID(event, id));
    switch (unit->currentKind) {
        case UNIT_GOOMBELLA_CHAPTER4:
            partyId = UNIT_GOOMBELLA;
            break;
        case UNIT_KOOPS_CHAPTER4:
            partyId = UNIT_KOOPS;
            break;
        case UNIT_FLURRIE_CHAPTER4:
            partyId = UNIT_FLURRIE;
            break;
        case UNIT_YOSHI_CHAPTER4:
        default:
            partyId = UNIT_YOSHI;
            break;
    }
    BattlePartyAnimeLoad(partyId);
    return EVT_RETURN_DONE;
}

static USER_FUNC(_anime_load_wait) {
    BattleWork* work = _battleWorkPointer;
    BattleUnit* unit;
    s32* args = event->args;
    s32 id;
    s32 partyId;

    id = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(work, BattleTransID(event, id));
    switch (unit->currentKind) {
        case UNIT_GOOMBELLA_CHAPTER4:
            partyId = UNIT_GOOMBELLA;
            break;
        case UNIT_KOOPS_CHAPTER4:
            partyId = UNIT_KOOPS;
            break;
        case UNIT_FLURRIE_CHAPTER4:
            partyId = UNIT_FLURRIE;
            break;
        case UNIT_YOSHI_CHAPTER4:
        default:
            partyId = UNIT_YOSHI;
            break;
    }
    if (BattlePartyAnimeLoad(partyId)) {
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}
#pragma opt_propagation on

EVT_BEGIN(party_change_event)
    ADD(GW(18), 1)
    IF_INT_GREATER(GW(18), 4)
        RETURN
    END_IF
    SWITCH(GW(18))
        CASE_EQUAL(0)
        SET_VALUE(LW(12), PTR(entry_gullible_christine))
        CASE_EQUAL(1)
        SET_VALUE(LW(12), PTR(entry_gullible_nokotarou))
        CASE_EQUAL(2)
        SET_VALUE(LW(12), PTR(entry_gullible_clauda))
        CASE_EQUAL(3)
        SET_VALUE(LW(12), PTR(entry_gullible_yoshi))
        CASE_DEFAULT
        RETURN
    END_SWITCH
    SET_VALUE(GW(20), 0)
    SET_VALUE(LW(3), GW(17))
    CALL(btlevtcmd_SpawnUnit, LW(9), LW(12), 0)
    SET_VALUE(GW(17), LW(9))
    CALL(_anime_load, LW(9))
    CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
    CALL(btlevtcmd_SetFallAccel, LW(3), FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, LW(9), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, -2, LW(5), LW(6), LW(7))
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_SetScale, LW(3), FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
        LOOP(18)
            CALL(btlevtcmd_AddScale, LW(3), FLOAT(-0.05f), FLOAT(-0.05f), FLOAT(-0.05f))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(evt_snd_sfxon, STRING("SFX_PARTY_IN1"), 0)
    CALL(btlevtcmd_JumpPosition, LW(3), LW(5), LW(6), LW(7), 20, 0)
    CALL(_anime_load_wait, LW(9))
    CALL(_save_param, -2, LW(3))
    CALL(_load_param, -2, LW(9))
    CALL(btlevtcmd_SetPos, LW(9), LW(5), LW(6), LW(7))
    CALL(btlevtcmd_SetFallAccel, LW(9), FLOAT(0.7f))
    CALL(btlevtcmd_CheckDataOfDataTable, LW(9), LW(0), 4)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_RunDataEventChild, LW(9), 4)
    END_IF
    CALL(evt_snd_sfxon, STRING("SFX_PARTY_CHANGE1"), 0)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_SetScale, LW(9), FLOAT(0.1f), FLOAT(0.1f), FLOAT(0.1f))
        LOOP(18)
            CALL(btlevtcmd_AddScale, LW(9), FLOAT(0.05f), FLOAT(0.05f), FLOAT(0.05f))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_SetScale, LW(9), FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetHomePos, LW(9), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon, STRING("SFX_PARTY_OUT1"), 0)
    CALL(btlevtcmd_JumpPosition, LW(9), LW(0), LW(1), LW(2), 20, 0)
    CALL(btlevtcmd_KillUnit, LW(3), 0)
    CALL(btlevtcmd_SortPhaseMoveTable)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event_fmario)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on
