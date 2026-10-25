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
#include "battle/battle_attack_audience.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_seq_command.h"
#include "battle/battle_weapon_power.h"
#include "eff/eff_funemizu.h"
#include "eff/n64/eff_kameki_tornade_n64.h"
#include "evt/evt_audience.h"
#include "evt/evt_eff.h"
#include "evt/evt_env.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "party/party_motion.h"
#include "unit/party/unit_party_chuchurina.h"
#include "battle/battle_database_common.h"
#pragma pool_data on // the original addresses its .rodata through one base register

extern u32 lbl_80379A00[2];
#pragma force_active on
extern s32 negone_one[];
extern s32 one_negone[];
static void __makeTechMenuFunc(BattleCommand* cmd, s32* count);
static s32 init_event[54];
static s32 damage_event[16];
BattleWeapon partyWeapon_YoshiNormalAttack;
BattleWeapon partyWeapon_YoshiNomikomi_Shot;
BattleWeapon partyWeapon_YoshiEggAttack_Minimini;
BattleWeapon partyWeapon_YoshiCallGuard;
s32 partyYoshiAttack_NormalAttack[1025];
s32 partyYoshiAttack_Nomikomi[1000];
s32 partyYoshiAttack_EggAttack[1266];
s32 _egg_attack_event[195];
s32 partyYoshiAttack_CallGuard[553];
s32 yoshi_appeal_event[39];
static s32 party_win_reaction[25];
static s32 attack_audience_event[290];
static s32 attack_event[2];
static s32 wait_event[6];

// clang-format off


#define TECH_MENU_ENTRY(table)                                                        \
    cmd->weaponTable[*count].mIndex = -1;                                            \
    cmd->weaponTable[*count].mItem = 0;                                              \
    cmd->weaponTable[*count].mWeaponParams = (table);                                \
    cmd->weaponTable[*count].mEnabled = 0;                                           \
    cmd->weaponTable[*count].mIcon =                                                 \
        cmd->weaponTable[*count].mWeaponParams->icon;                                \
    cmd->weaponTable[*count].mName =                                                 \
        msgSearch(cmd->weaponTable[*count].mWeaponParams->name);                      \
    (*count)++;

__declspec(section ".data") u32 lbl_80379A00[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
static __declspec(section ".data") u32 negone_one_374[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_80379A10[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
static __declspec(section ".data") u32 one_negone_373[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_80379A20[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
static __declspec(section ".data") u32 negone_one_360[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_80379A30[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
static __declspec(section ".data") u32 one_negone_359[2] = {
    0x3F800000, 0xBF800000,
};

BattleDispPose pose_table_yoshi_stay[25] = {
    { 0x09, "PYS_D_5" },
    { 0x02, "PYS_D_5" },
    { 0x03, "PYS_X_1" },
    { 0x04, "PYS_D_2" },
    { 0x05, "PYS_X_1" },
    { 0x01, "PYS_N_1" },
    { 0x1B, "PYS_D_3" },
    { 0x1C, "PYS_S_1" },
    { 0x25, "PYS_G_2" },
    { 0x1F, "PYS_G_1" },
    { 0x27, "PYS_D_1" },
    { 0x28, "PYS_W_1" },
    { 0x2A, "PYS_R_1" },
    { 0x39, "PYS_D_2" },
    { 0x38, "PYS_D_2" },
    { 0x3C, "PYS_D_4" },
    { 0x3D, "PYS_D_2" },
    { 0x41, "PYS_T_1" },
    { 0x34, "PYS_I_1" },
    { 0x43, "PYS_C_1" },
    { 0x42, "PYS_O_1" },
    { 0x40, "PYS_F_1" },
    { 0x31, "PYS_F_1" },
    { 0x2D, "PYS_G_1" },
    { 0x45, "PYS_S_1" },
};

static BattleDataEntry data_table[8] = {
    { 0x05, yoshi_appeal_event },
    { 0x06, attack_audience_event },
    { 0x01, __makeTechMenuFunc },
    { 0x30, btldefaultevt_Dummy },
    { 0x40, party_win_reaction },
    { 0x07, subset_attack_ready_jump_event },
    { 0x2F, btldefaultevt_avoid_counter_event },
    { 0x00, NULL },
};

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

BattleDispPose pose_table_egg_g[4] = {
    { 0x1C, "CHU_C_1" },
    { 0x1F, "CHU_C_1" },
    { 0x27, "CHU_C_1" },
    { 0x45, "CHU_C_1" },
};

BattleDispPose pose_table_egg_y[4] = {
    { 0x1C, "CHU_C_2" },
    { 0x1F, "CHU_C_2" },
    { 0x27, "CHU_C_2" },
    { 0x45, "CHU_C_2" },
};

BattleDispPose pose_table_egg_p[4] = {
    { 0x1C, "CHU_C_3" },
    { 0x1F, "CHU_C_3" },
    { 0x27, "CHU_C_3" },
    { 0x45, "CHU_C_3" },
};

BattleUnitKindPart unitpartsdata_Party_Yoshi[7] = {
    {
        1,                                    // partNum
        "btl_un_yoshi",                       // partName
        "c_babyyoshi",                        // modelName
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
        pose_table_yoshi_stay,                // poseTable
    },
    {
        2,                                    // partNum
        "btl_un_yoshi",                       // partName
        "c_tamago",                           // modelName
        { 0.0f, 0.0f, 0.0f },                 // positionOffset
        { 16.0f, 24.0f, 5.0f },               // hitBaseOffset
        { 0.0f, 24.0f, 0.0f },                // hitCursorBaseOffset
        20,                                   // shadowSize
        24,                                   // field_0x32
        255,                                  // baseAlpha
        (BattleUnitDefense*)defence,          // defense
        (BattleUnitDefenseAttr*)defence_attr, // defenseAttr
        0x13010000,                           // attributes
        0,                                    // counterAttributes
        pose_table_egg_g,                     // poseTable
    },
    {
        3,                                    // partNum
        "btl_un_yoshi",                       // partName
        "c_tamago",                           // modelName
        { 0.0f, 0.0f, 0.0f },                 // positionOffset
        { 16.0f, 24.0f, 5.0f },               // hitBaseOffset
        { 0.0f, 24.0f, 0.0f },                // hitCursorBaseOffset
        20,                                   // shadowSize
        24,                                   // field_0x32
        255,                                  // baseAlpha
        (BattleUnitDefense*)defence,          // defense
        (BattleUnitDefenseAttr*)defence_attr, // defenseAttr
        0x13010000,                           // attributes
        0,                                    // counterAttributes
        pose_table_egg_g,                     // poseTable
    },
    {
        4,                                    // partNum
        "btl_un_yoshi",                       // partName
        "c_tamago",                           // modelName
        { 0.0f, 0.0f, 0.0f },                 // positionOffset
        { 16.0f, 24.0f, 5.0f },               // hitBaseOffset
        { 0.0f, 24.0f, 0.0f },                // hitCursorBaseOffset
        20,                                   // shadowSize
        24,                                   // field_0x32
        255,                                  // baseAlpha
        (BattleUnitDefense*)defence,          // defense
        (BattleUnitDefenseAttr*)defence_attr, // defenseAttr
        0x13010000,                           // attributes
        0,                                    // counterAttributes
        pose_table_egg_g,                     // poseTable
    },
    {
        5,                                    // partNum
        "btl_un_yoshi",                       // partName
        "c_tamago",                           // modelName
        { 0.0f, 0.0f, 0.0f },                 // positionOffset
        { 16.0f, 24.0f, 5.0f },               // hitBaseOffset
        { 0.0f, 24.0f, 0.0f },                // hitCursorBaseOffset
        20,                                   // shadowSize
        24,                                   // field_0x32
        255,                                  // baseAlpha
        (BattleUnitDefense*)defence,          // defense
        (BattleUnitDefenseAttr*)defence_attr, // defenseAttr
        0x13010000,                           // attributes
        0,                                    // counterAttributes
        pose_table_egg_g,                     // poseTable
    },
    {
        6,                                    // partNum
        "btl_un_yoshi",                       // partName
        "c_tamago",                           // modelName
        { 0.0f, 0.0f, 0.0f },                 // positionOffset
        { 16.0f, 24.0f, 5.0f },               // hitBaseOffset
        { 0.0f, 24.0f, 0.0f },                // hitCursorBaseOffset
        20,                                   // shadowSize
        24,                                   // field_0x32
        255,                                  // baseAlpha
        (BattleUnitDefense*)defence,          // defense
        (BattleUnitDefenseAttr*)defence_attr, // defenseAttr
        0x13010000,                           // attributes
        0,                                    // counterAttributes
        pose_table_egg_g,                     // poseTable
    },
    {
        7,                                    // partNum
        "btl_un_yoshi",                       // partName
        "c_tamago",                           // modelName
        { 0.0f, 0.0f, 0.0f },                 // positionOffset
        { 16.0f, 24.0f, 5.0f },               // hitBaseOffset
        { 0.0f, 24.0f, 0.0f },                // hitCursorBaseOffset
        20,                                   // shadowSize
        24,                                   // field_0x32
        255,                                  // baseAlpha
        (BattleUnitDefense*)defence,          // defense
        (BattleUnitDefenseAttr*)defence_attr, // defenseAttr
        0x13010000,                           // attributes
        0,                                    // counterAttributes
        pose_table_egg_g,                     // poseTable
    },
};

BattleUnitKind unitdata_Party_Yoshi = {
    UNIT_YOSHI,                      // mUnitType
    "btl_un_yoshi",                  // name
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
    { 8, 28 },                       // statusIconOffset
    { 0.0f, 12.0f, 0.0f },           // centerOffset
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
    7,                               // numParts
    unitpartsdata_Party_Yoshi,       // mParts
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
    CALL(btlevtcmd_SetMoveSoundDataWork, -2, STRING("SFX_PARTY_BATTLE_SUBMOVE1L"), STRING("SFX_PARTY_BATTLE_SUBMOVE1R"), 0, 6, 6, 0, 0)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -2, 0)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_PARTY_BATTLE_MOVE1L"), STRING("SFX_PARTY_BATTLE_MOVE1R"), 0, 15, 13)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_PARTY_BATTLE_MOVE1L"), STRING("SFX_PARTY_BATTLE_MOVE1R"), 0, 6, 6)
    CALL(btlevtcmd_SetJumpSound, -2, STRING("SFX_PARTY_BATTLE_JUMP1"), STRING("SFX_PARTY_BATTLE_LANDING1"))
    CALL(yoshi_original_color_anim_set, -2)
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

BattleWeapon partyWeapon_YoshiNormalAttack = {
    "btl_wn_pys_normal",                         // name
    336,                                         // icon
    ITEM_NULL,                                   // item
    "msg_pys_hip_drop",                          // description
    100,                                         // baseAccuracy
    0,                                           // baseFpCost
    0,                                           // baseSpCost
    0,                                           // superguardType
    1.0f,                                        // field_0x14
    1,                                           // mStylishCommandMultiplier
    1,                                           // field_0x19
    100,                                         // mBingoSlotChance
    50,                                          // field_0x1B
    weaponGetPowerFromPartyAttackLv,             // hpDamageFn
    { 1, 1, 1, 1, 1, 1, 0, 0 },                  // hpDamageWork
    NULL,                                        // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                  // fpDamageWork
    0x01001260,                                  // targetClassFlags
    0x20001004,                                  // targetPropertyFlags
    0,                                           // element
    0,                                           // damagePattern
    3,                                           // actionLevel
    2,                                           // field_0x6F
    "msg_ac_hip_drop",                           // acHelpMsg
    0x00035097,                                  // special
    0x0000000C,                                  // counterResistanceFlags
    0x80002004,                                  // targetWeightingFlags
    0,                                           // sleepChance
    0,                                           // sleepTime
    0,                                           // stopChance
    0,                                           // stopTime
    0,                                           // dizzyChance
    0,                                           // dizzyTime
    0,                                           // poisonChance
    0,                                           // poisonTime
    0,                                           // poisonStrength
    0,                                           // confuseChance
    0,                                           // confuseTime
    0,                                           // electricChance
    0,                                           // electricTime
    0,                                           // dodgyChance
    0,                                           // dodgyTime
    0,                                           // burnChance
    0,                                           // burnTime
    0,                                           // freezeChance
    0,                                           // freezeTime
    0,                                           // sizeChangeChance
    0,                                           // sizeChangeTime
    0,                                           // sizeChangeStrength
    0,                                           // atkChangeChance
    0,                                           // atkChangeTime
    0,                                           // atkChangeStrength
    0,                                           // defChangeChance
    0,                                           // defChangeTime
    0,                                           // defChangeStrength
    0,                                           // allergicChance
    0,                                           // allergicTime
    0,                                           // ohkoChance
    0,                                           // chargeStrength
    0,                                           // fastChance
    0,                                           // fastTime
    0,                                           // slowChance
    0,                                           // slowTime
    0,                                           // frightChance
    0,                                           // galeForceChance
    0,                                           // paybackTime
    0,                                           // holdFastTime
    0,                                           // invisibleChance
    0,                                           // invisibleTime
    0,                                           // hpRegenTime
    0,                                           // hpRegenStrength
    0,                                           // fpRegenTime
    0,                                           // fpRegenStrength
    partyYoshiAttack_NormalAttack,               // mAttackEvtScript
    { 0, 10, 10, 100, 10, 5, 5, 5, 5, 5, 0, 0 }, // field_B4
};

BattleWeapon partyWeapon_YoshiNomikomi_Shot = {
    "btl_wn_pys_lv1",                         // name
    337,                                      // icon
    ITEM_NULL,                                // item
    "msg_pys_nomikomi",                       // description
    100,                                      // baseAccuracy
    4,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPowerFromPartyAttackLv,          // hpDamageFn
    { 2, 4, 3, 5, 3, 6, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    6,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_nomikomi",                        // acHelpMsg
    0x00020047,                               // special
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
    partyYoshiAttack_Nomikomi,                // mAttackEvtScript
    { 0, 0, 0, 100, 0, 5, 5, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon partyWeapon_YoshiNomikomi_Spew = {
    "btl_wn_pys_lv1",                         // name
    337,                                      // icon
    ITEM_NULL,                                // item
    "msg_pys_nomikomi",                       // description
    100,                                      // baseAccuracy
    4,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPowerFromPartyAttackLv,          // hpDamageFn
    { 2, 4, 3, 5, 3, 6, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    5,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_nomikomi",                        // acHelpMsg
    0x00020047,                               // special
    0x000007FF,                               // counterResistanceFlags
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
    partyYoshiAttack_Nomikomi,                // mAttackEvtScript
    { 0, 0, 0, 100, 0, 5, 5, 5, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon partyWeapon_YoshiNomikomi_Dmg0 = {
    "btl_wn_pys_lv1",                         // name
    337,                                      // icon
    ITEM_NULL,                                // item
    "msg_pys_nomikomi",                       // description
    100,                                      // baseAccuracy
    2,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_nomikomi",                        // acHelpMsg
    0x00020040,                               // special
    0x000007FF,                               // counterResistanceFlags
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
    partyYoshiAttack_Nomikomi,                // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon partyWeapon_YoshiNomikomi_Fire = {
    "btl_wn_pys_lv1",                         // name
    337,                                      // icon
    ITEM_NULL,                                // item
    "msg_pys_nomikomi",                       // description
    100,                                      // baseAccuracy
    2,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 1, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    1,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_nomikomi",                        // acHelpMsg
    0x00020040,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80000004,                               // targetWeightingFlags
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
    partyYoshiAttack_Nomikomi,                // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon partyWeapon_YoshiNomikomi_Involved = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    0.0f,                                     // field_0x14
    0,                                        // mStylishCommandMultiplier
    0,                                        // field_0x19
    0,                                        // mBingoSlotChance
    0,                                        // field_0x1B
    weaponGetPowerFromPartyAttackLv,          // hpDamageFn
    { 1, 4, 2, 5, 3, 6, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02100260,                               // targetClassFlags
    0x20102000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00020040,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x00000004,                               // targetWeightingFlags
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

BattleWeapon partyWeapon_YoshiEggAttack_Minimini = {
    "btl_wn_pys_lv2",                         // name
    338,                                      // icon
    ITEM_NULL,                                // item
    "msg_pys_wonder_egg",                     // description
    100,                                      // baseAccuracy
    3,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    2,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 1, 1, 1, 1, 1, 1, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_wonder_egg",                      // acHelpMsg
    0x00020003,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80002000,                               // targetWeightingFlags
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
    100,                                      // sizeChangeChance
    3,                                        // sizeChangeTime
    -2,                                       // sizeChangeStrength
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
    partyYoshiAttack_EggAttack,               // mAttackEvtScript
    { 0, 0, 0, 100, 0, 3, 3, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon partyWeapon_YoshiCallGuard = {
    "btl_wn_pys_lv3",                              // name
    339,                                           // icon
    ITEM_NULL,                                     // item
    "msg_pys_taigun_yoshi",                        // description
    100,                                           // baseAccuracy
    6,                                             // baseFpCost
    0,                                             // baseSpCost
    0,                                             // superguardType
    1.0f,                                          // field_0x14
    1,                                             // mStylishCommandMultiplier
    5,                                             // field_0x19
    100,                                           // mBingoSlotChance
    50,                                            // field_0x1B
    weaponGetPowerFromPartyAttackLv,               // hpDamageFn
    { 1, 1, 1, 1, 1, 1, 0, 0 },                    // hpDamageWork
    NULL,                                          // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                    // fpDamageWork
    0x02101260,                                    // targetClassFlags
    0x20000000,                                    // targetPropertyFlags
    0,                                             // element
    0,                                             // damagePattern
    3,                                             // actionLevel
    2,                                             // field_0x6F
    "msg_ac_taigun_yoshi",                         // acHelpMsg
    0x00020017,                                    // special
    0x000007FF,                                    // counterResistanceFlags
    0x80002004,                                    // targetWeightingFlags
    0,                                             // sleepChance
    0,                                             // sleepTime
    0,                                             // stopChance
    0,                                             // stopTime
    0,                                             // dizzyChance
    0,                                             // dizzyTime
    0,                                             // poisonChance
    0,                                             // poisonTime
    0,                                             // poisonStrength
    0,                                             // confuseChance
    0,                                             // confuseTime
    0,                                             // electricChance
    0,                                             // electricTime
    0,                                             // dodgyChance
    0,                                             // dodgyTime
    0,                                             // burnChance
    0,                                             // burnTime
    0,                                             // freezeChance
    0,                                             // freezeTime
    0,                                             // sizeChangeChance
    0,                                             // sizeChangeTime
    0,                                             // sizeChangeStrength
    0,                                             // atkChangeChance
    0,                                             // atkChangeTime
    0,                                             // atkChangeStrength
    0,                                             // defChangeChance
    0,                                             // defChangeTime
    0,                                             // defChangeStrength
    0,                                             // allergicChance
    0,                                             // allergicTime
    0,                                             // ohkoChance
    0,                                             // chargeStrength
    0,                                             // fastChance
    0,                                             // fastTime
    0,                                             // slowChance
    0,                                             // slowTime
    0,                                             // frightChance
    0,                                             // galeForceChance
    0,                                             // paybackTime
    0,                                             // holdFastTime
    0,                                             // invisibleChance
    0,                                             // invisibleTime
    0,                                             // hpRegenTime
    0,                                             // hpRegenStrength
    0,                                             // fpRegenTime
    0,                                             // fpRegenStrength
    partyYoshiAttack_CallGuard,                    // mAttackEvtScript
    { 0, 20, 20, 100, 20, 20, 5, 5, 20, 5, 0, 0 }, // field_B4
};

BtlUnitPoseSound posesound_normal_attack[15] = {
    { "PYS_W_1", 0.01f, _yoshi_slide_move_sound, "SFX_PARTY_BATTLE_MOVE1L", 0, 1 },
    { "PYS_W_1", 0.25f, _yoshi_slide_move_sound, "SFX_PARTY_BATTLE_MOVE1R", 0, 1 },
    { "PYS_R_1", 0.01f, _yoshi_slide_move_sound, "SFX_PARTY_BATTLE_MOVE1L", 0, 1 },
    { "PYS_R_1", 0.1f, _yoshi_slide_move_sound, "SFX_PARTY_BATTLE_MOVE1R", 0, 1 },
    { "PYS_R_4", 0.01f, _yoshi_slide_move_sound, "SFX_PARTY_BATTLE_MOVE1L", 1364, 1 },
    { "PYS_R_4", 0.05f, _yoshi_slide_move_sound, "SFX_PARTY_BATTLE_MOVE1R", 1364, 1 },
    { "PYS_R_4", 0.1f, _yoshi_slide_move_sound, "SFX_PARTY_BATTLE_MOVE1L", 1364, 1 },
    { "PYS_R_4", 0.15f, _yoshi_slide_move_sound, "SFX_PARTY_BATTLE_MOVE1R", 1364, 1 },
    { "PYS_R_5", 0.01f, _yoshi_slide_move_sound, "SFX_PARTY_BATTLE_MOVE1L", 2387, 1 },
    { "PYS_R_5", 0.033333335f, _yoshi_slide_move_sound, "SFX_PARTY_BATTLE_MOVE1R", 2387, 1 },
    { "PYS_R_5", 0.083333336f, _yoshi_slide_move_sound, "SFX_PARTY_BATTLE_MOVE1L", 2387, 1 },
    { "PYS_R_5", 0.11666667f, _yoshi_slide_move_sound, "SFX_PARTY_BATTLE_MOVE1R", 2387, 1 },
    { "PYS_A_3", 0.01f, _yoshi_slide_move_sound, "SFX_PARTY_BATTLE_MOVE1L", 2387, 1 },
    { "PYS_A_3", 0.083333336f, _yoshi_slide_move_sound, "SFX_PARTY_BATTLE_MOVE1R", 2387, 1 },
    { NULL, 0.0f, _yoshi_slide_move_sound, NULL, 0, 1 },
};

EVT_BEGIN(partyYoshiAttack_NormalAttack)
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
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    WAIT_FRAMES(20)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_SetUnitWork, -2, 2, 1)
    CALL(btlevtcmd_SetUnitWork, -2, 3, -1)
    CALL(btlevtcmd_SetUnitWork, -2, 4, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 5, 100)
    CALL(btlevtcmd_AnimeSetPoseSoundTable, -2, 1, PTR(posesound_normal_attack))
    CALL(btlevtcmd_AcSetOutputParam, 1, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_W_1"))
    BEGIN_SIBLING_EVENT
        SET_VALUE(LW(0), 0)
        LOOP(25)
            ADD(LW(0), 4)
            SET_VALUE(LW(1), 2387)
            MULTIPLY(LW(1), LW(0))
            DIVIDE(LW(1), 100)
            CALL(btlevtcmd_SetUnitWork, -2, 4, LW(1))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_R_5"))
    WAIT_FRAMES(25)
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 350)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_GetTakeoffPosition, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_J_1A"))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.3f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 25)
    ADD(LW(1), 35)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL(btlevtcmd_CommandGetWeaponActionLv, LW(15))
        CALL(btlevtcmd_AcSetDifficulty, -2, LW(15))
        CALL(btlevtcmd_GetPartyTechLv, -2, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(2)
            CALL(btlevtcmd_AcSetParamAll, 5, 180, 178, 23, 25, 20, 61, 1)
            CALL(btlevtcmd_AcSetGaugeParam, 60, 80, 100, 100)
            CASE_EQUAL(1)
            CALL(btlevtcmd_AcSetParamAll, 5, 180, 178, 23, 25, 25, 76, 1)
            CALL(btlevtcmd_AcSetGaugeParam, 75, 100, 100, 100)
            CASE_DEFAULT
            CALL(btlevtcmd_AcSetParamAll, 5, 180, 178, 23, 25, 34, 100, 1)
            CALL(btlevtcmd_AcSetGaugeParam, 100, 100, 100, 100)
        END_SWITCH
        CALL(btlevtcmd_AcSetFlag, 72)
        CALL(btlevtcmd_SetupAC, -2, 6, 1, 0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 25, -1)
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_3"))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
    ADD(LW(7), 40)
    SET_VALUE(LW(10), FLOAT(1.0f))
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL(btlevtcmd_StartAC, 1)
        CALL(btlevtcmd_ResultAC)
    END_SIBLING_EVENT
    CALL(btl_yoshi_yoroyoro_jump_calc_param, -2, LW(6), LW(7), LW(8), LW(10), LW(9))
    CALL(btl_yoshi_yoroyoro_jump_move, -2, LW(6), LW(7), LW(8), LW(10), LW(9))
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    SET_VALUE(LW(13), 1)
    CALL(btlevtcmd_AcGetOutputParam, 2, LW(14))
    LABEL(50)
    IF_INT_GREATER_OR_EQUAL(LW(13), 2)
        CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 1048832, LW(5))
    ELSE
        CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(5))
    END_IF
    SWITCH(LW(5))
        CASE_OR_EQUAL(2)
        CASE_OR_EQUAL(3)
        CASE_OR_EQUAL(6)
        CASE_OR_EQUAL(4)
        CASE_END
        CASE_EQUAL(1)
        CASE_DEFAULT
    END_SWITCH
    CALL(btlevtcmd_AnimeSetPoseSoundTable, -2, 1, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    IF_INT_LESS(LW(13), 2)
        CALL(btlevtcmd_ACRStart, -2, 0, 20, 20, 0)
        CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
    ELSE
        SET_VALUE(LW(6), 1)
    END_IF
    IF_INT_EQUAL(LW(13), 1)
        CALL(btlevtcmd_GetFaceDirection, -2, LW(15))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_J_1B"))
        IF_INT_EQUAL(LW(6), 2)
            SET_VALUE(LW(0), 3)
            SET_VALUE(LW(1), 6)
            MULTIPLY(LW(15), -60)
        ELSE
            SET_VALUE(LW(0), 1)
            SET_VALUE(LW(1), 9)
            MULTIPLY(LW(15), -40)
        END_IF
        CALL(btlevtcmd_SetRotateOffset, -2, 0, 12, 0)
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        WAIT_FRAMES(15)
        IF_INT_EQUAL(LW(6), 2)
            CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, -60, -25, 0)
        END_IF
        LOOP(LW(0))
            SET_VALUE(LW(2), LW(1))
            CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_ATTACK1"), 0)
            LOOP(LW(2))
                CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(15))
                WAIT_FRAMES(1)
            END_LOOP
        END_LOOP
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CALL(btlevtcmd_SetRotateOffset, -2, 0, 0, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_1"))
        WAIT_FRAMES(15)
    ELSE
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_1"))
    END_IF
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 4, 0, 0, 0, -1)
    IF_INT_NOT_EQUAL(LW(5), 1)
        CALL(btlevtcmd_StopAC)
        IF_INT_EQUAL(LW(5), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(5), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(5), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        SET_VALUE(LW(15), LW(1))
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        SUBTRACT(LW(15), LW(1))
        DIVIDE(LW(15), 10)
        CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), LW(15), 0, 0, 0, -1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_D_1"))
        CALL(btlevtcmd_SetDispOffset, -2, 0, -7, 0)
        CALL(evt_btl_camera_shake_h, 0, 4, 0, 10, 13)
        WAIT_FRAMES(40)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_5"))
        CALL(btlevtcmd_SetDispOffset, -2, 0, 0, 0)
        WAIT_FRAMES(54)
        CALL(evt_audience_ap_recovery)
        CALL(btlevtcmd_InviteApInfoReport)
        GOTO(95)
    END_IF
    CALL(btlevtcmd_ac_timing_flag_onoff, 1, 1)
    WAIT_FRAMES(2)
    CALL(btlevtcmd_ac_timing_flag_onoff, 0, 1)
    CALL(btlevtcmd_GetResultAC, LW(0))
    IF_BITS_SET(LW(0), 2)
        IF_INT_GREATER_OR_EQUAL(LW(13), LW(14))
            CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 131328, LW(5))
        ELSE
            CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 131072, LW(5))
        END_IF
        CALL(btlevtcmd_GetPartyTechLv, -2, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(2)
            CALL(btlevtcmd_AcGetOutputParam, 2, LW(0))
            SWITCH(LW(0))
                CASE_GREATER_OR_EQUAL(6)
                SET_VALUE(LW(0), 2)
                CASE_GREATER_OR_EQUAL(4)
                SET_VALUE(LW(0), 1)
                CASE_GREATER_OR_EQUAL(2)
                SET_VALUE(LW(0), 0)
                CASE_DEFAULT
                SET_VALUE(LW(0), -1)
            END_SWITCH
            CASE_EQUAL(1)
            CALL(btlevtcmd_AcGetOutputParam, 2, LW(0))
            SWITCH(LW(0))
                CASE_GREATER_OR_EQUAL(4)
                SET_VALUE(LW(0), 1)
                CASE_GREATER_OR_EQUAL(2)
                SET_VALUE(LW(0), 0)
                CASE_DEFAULT
                SET_VALUE(LW(0), -1)
            END_SWITCH
            CASE_DEFAULT
            CALL(btlevtcmd_AcGetOutputParam, 2, LW(0))
            SWITCH(LW(0))
                CASE_GREATER_OR_EQUAL(4)
                SET_VALUE(LW(0), 1)
                CASE_GREATER_OR_EQUAL(2)
                SET_VALUE(LW(0), 0)
                CASE_DEFAULT
                SET_VALUE(LW(0), -1)
            END_SWITCH
        END_SWITCH
        IF_INT_GREATER_OR_EQUAL(LW(0), 0)
            SET_VALUE(LW(1), LW(0))
            SET_VALUE(LW(2), LW(13))
            SUBTRACT(LW(2), 1)
            IF_INT_GREATER(LW(1), LW(2))
                SET_VALUE(LW(1), LW(2))
            END_IF
            CALL(btlevtcmd_GetResultPrizeLv, LW(3), LW(1), LW(6))
            CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(9), LW(1), LW(2))
            CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(9), LW(1), LW(2))
            SET_VALUE(LW(1), LW(13))
            ADD(LW(1), 1)
            IF_INT_GREATER(LW(1), LW(14))
                CALL(btlevtcmd_GetResultPrizeLv, LW(3), LW(0), LW(6))
                CALL(btlevtcmd_AudienceDeclareACResult, LW(12), LW(6))
            END_IF
        ELSE
            CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
        END_IF
    ELSE
        IF_INT_GREATER_OR_EQUAL(LW(13), LW(14))
            CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
        ELSE
            CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 0, LW(5))
        END_IF
        CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
    END_IF
    LABEL(90)
    ADD(LW(13), 1)
    IF_INT_LESS_OR_EQUAL(LW(13), LW(14))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.2f))
        ADD(LW(1), 40)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
        GOTO(50)
    END_IF
    CALL(btlevtcmd_StopAC)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 60)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_ACRStart, -2, 0, 15, 15, 20)
    END_SIBLING_EVENT
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 25)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 16, -1)
    CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
    SWITCH(LW(6))
        CASE_GREATER_OR_EQUAL(2)
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(0), 1, 0, 0, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_Y_1"))
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_Y_1"))
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        CASE_DEFAULT
        CALL(evt_audience_acrobat_notry)
    END_SWITCH
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

EVT_BEGIN(partyYoshiAttack_Nomikomi)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(15))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(15))
    CALL(btlevtcmd_AcGetDifficulty, LW(0))
    SUBTRACT(LW(0), 3)
    SWITCH(LW(0))
        CASE_LESS_OR_EQUAL(-3)
        SET_VALUE(LW(0), 160)
        SET_VALUE(LW(1), 40)
        CASE_EQUAL(-2)
        SET_VALUE(LW(0), 140)
        SET_VALUE(LW(1), 30)
        CASE_EQUAL(-1)
        SET_VALUE(LW(0), 120)
        SET_VALUE(LW(1), 25)
        CASE_EQUAL(0)
        SET_VALUE(LW(0), 100)
        SET_VALUE(LW(1), 20)
        CASE_EQUAL(1)
        SET_VALUE(LW(0), 80)
        SET_VALUE(LW(1), 10)
        CASE_EQUAL(2)
        SET_VALUE(LW(0), 50)
        SET_VALUE(LW(1), 5)
        CASE_DEFAULT
        SET_VALUE(LW(0), 30)
        SET_VALUE(LW(1), 2)
    END_SWITCH
    CALL(btlevtcmd_AcSetParamAll, 3, LW(0), LW(1), 0, 100, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AcSetFlag, 15)
    CALL(btlevtcmd_SetupAC, -2, 15, 1, 0)
    CALL(evt_btl_camera_set_mode, 0, 10)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, -1, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    WAIT_FRAMES(22)
    CALL(btlevtcmd_StartAC, 1)
    LABEL(5)
    CALL(btlevtcmd_AcGetOutputParam, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        WAIT_FRAMES(1)
        GOTO(5)
    END_IF
    CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(5))
    IF_INT_EQUAL(LW(5), 5)
        CALL(btlevtcmd_GetStatusMg, -2, LW(6))
        MULTIPLY_FLOAT(LW(6), 40)
        CALL(_get_nomikomi_hit_position, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), LW(6))
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
        CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 80, -1, 0)
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
    END_IF
    CALL(btlevtcmd_GetStatusMg, -2, LW(6))
    MULTIPLY_FLOAT(LW(6), 40)
    CALL(_get_nomikomi_hit_position, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), LW(6))
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 80, -1, 0)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_2"))
    CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_SWALLOW1"), 0)
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
        WAIT_FRAMES(50)
        GOTO(90)
    END_IF
    WAIT_FRAMES(7)
    CALL(_get_swallow_param, LW(3), LW(7))
    IF_INT_EQUAL(LW(7), -1)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&partyWeapon_YoshiNomikomi_Dmg0), 0x80000100, LW(5))
        WAIT_FRAMES(43)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_GetResultAC, LW(0))
    IF_BITS_CLEAR(LW(0), 2)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&partyWeapon_YoshiNomikomi_Dmg0), 0x80000100, LW(5))
        WAIT_FRAMES(43)
        GOTO(90)
    END_IF
    CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_SWALLOW2"), 0)
    CALL(_check_swallow_attribute, LW(3), 8, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_DamageDirect, LW(3), LW(4), 0, 0, 7, 1)
        WAIT_FRAMES(13)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_7"))
        CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_SWALLOW5"), 0)
        WAIT_FRAMES(34)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_5"))
        WAIT_FRAMES(54)
        GOTO(90)
    END_IF
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 350)
    CALL(btlevtcmd_DamageDirect, LW(3), LW(4), 0, 0, 2, 1)
    WAIT_FRAMES(3)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 12)
    CALL(btlevtcmd_DivePosition, LW(3), LW(0), LW(1), LW(2), 5, 0, 0, 0, -1)
    CALL(btlevtcmd_OnAttribute, LW(3), 16777216)
    CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_SWALLOW3"), 0)
    WAIT_FRAMES(6)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_3"))
    WAIT_FRAMES(7)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_sub_random, 99, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), LW(7))
        BEGIN_INLINE_EVENT
            CALL(evt_env_blur_on, 0, 600)
        END_INLINE_EVENT
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_4"))
        CALL(btlevtcmd_ftomsec, 22, LW(0))
        WAIT_MS(LW(0))
        BEGIN_INLINE_EVENT_GET_ID(LW(15))
            CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_NOMIKOMI6"), 0)
            CALL(btlevtcmd_OffAttribute, LW(3), 16777216)
            CALL(_check_swallow_attribute, LW(3), 2, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&partyWeapon_YoshiNomikomi_Spew), 0x80000100, LW(5))
            ELSE
                CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&partyWeapon_YoshiNomikomi_Shot), 0x80000100, LW(5))
            END_IF
            CALL(btlevtcmd_GetResultAC, LW(0))
            IF_BITS_SET(LW(0), 2)
                CALL(btlevtcmd_GetResultPrizeLv, LW(3), 0, LW(6))
                CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
                CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
                CALL(btlevtcmd_AudienceDeclareACResult, LW(12), LW(6))
            ELSE
                CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
            END_IF
            CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
            CALL(btlevtcmd_ACRStart, -2, 0, 15, 15, 20)
            CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
            SWITCH(LW(6))
                CASE_GREATER_OR_EQUAL(2)
                CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
                CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(0), 1, 0, 0, 0)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_Y_1"))
                CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_Y_1"))
                CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
                CASE_DEFAULT
                CALL(evt_audience_acrobat_notry)
            END_SWITCH
        END_INLINE_EVENT
        CALL(btlevtcmd_WaitEventEnd, LW(15))
        GOTO(90)
    END_IF
    CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_GetResultPrizeLv, LW(3), 0, LW(6))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
    END_SIBLING_EVENT
    CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_SWALLOW5"), 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_7"))
    WAIT_FRAMES(26)
    CALL(_check_swallow_attribute, LW(3), 1, LW(1))
    CALL(_check_swallow_attribute, LW(3), 4, LW(2))
    CALL(btlevtcmd_GetExp, LW(3), LW(0))
    CALL(btlevtcmd_StoreExp, LW(0))
    CALL(btlevtcmd_KillUnit, LW(3), 0)
    WAIT_FRAMES(8)
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_6"))
        CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_SWALLOW7"), 0)
        WAIT_FRAMES(50)
        GOTO(90)
    ELSE
        IF_INT_EQUAL(LW(2), 0)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_5"))
            CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_SWALLOW4"), 0)
            WAIT_FRAMES(54)
            GOTO(90)
        END_IF
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_4"))
    WAIT_FRAMES(22)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(18)
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 15)
    ADD(LW(1), 15)
    SET_VALUE(LW(3), LW(0))
    SET_VALUE(LW(4), LW(1))
    SET_VALUE(LW(5), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(3), 240)
    CALL(evt_eff, STRING("eff"), STRING("breath_fire"), 1, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), 1, 2, 60, FLOAT(0.5f), FLOAT(30.0f))
    WAIT_FRAMES(60)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&partyWeapon_YoshiNomikomi_Fire))
    CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
    LABEL(10)
    IF_INT_EQUAL(LW(10), -1)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(10), LW(11), PTR(&partyWeapon_YoshiNomikomi_Fire), 0x80000100, LW(5))
    CALL(btlevtcmd_GetSelectNextEnemy, LW(10), LW(11))
    GOTO(10)
    LABEL(90)
    CALL(btlevtcmd_StopAC)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(5))
    IF_INT_EQUAL(LW(5), 0)
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetMoveFrame, -2, LW(0), LW(1), LW(2), FLOAT(4.0f), LW(5))
    IF_INT_LESS_OR_EQUAL(LW(5), 20)
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
        GOTO(99)
    END_IF
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        SUBTRACT(LW(5), 1)
        SET_VALUE(LW(6), LW(5))
        SUBTRACT(LW(6), 10)
        CALL(btlevtcmd_ACRStart, -2, LW(6), LW(5), LW(5), 0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), LW(5), -1, 0)
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
    IF_INT_EQUAL(LW(6), 2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_O_1"))
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, 0, 0, 0)
        WAIT_FRAMES(20)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_J_1B"))
        CALL(evt_sub_intpl_init, 4, 0, 360, 30)
        LOOP(30)
            CALL(evt_sub_intpl_get_value)
            CALL(btlevtcmd_SetRotate, -2, 0, LW(0), 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_IF
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(partyYoshiAttack_EggAttack)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
    CALL(btlevtcmd_WeaponAftereffect, LW(0))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&partyWeapon_YoshiEggAttack_Minimini))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&partyWeapon_YoshiEggAttack_Minimini), LW(3), LW(4))
    CALL(btlevtcmd_SetPartsWork, -2, 2, 3, LW(3))
    CALL(btlevtcmd_SetPartsWork, -2, 2, 4, LW(4))
    CALL(btlevtcmd_SetPartsWork, -2, 2, 5, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 2, 6, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 2, 7, 0)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&partyWeapon_YoshiEggAttack_Minimini))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&partyWeapon_YoshiEggAttack_Minimini), LW(10), LW(11))
    CALL(btlevtcmd_SetPartsWork, -2, 7, 3, LW(10))
    CALL(btlevtcmd_SetPartsWork, -2, 7, 4, LW(11))
    CALL(btlevtcmd_SetPartsWork, -2, 7, 5, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 7, 6, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 7, 7, 0)
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&partyWeapon_YoshiEggAttack_Minimini))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&partyWeapon_YoshiEggAttack_Minimini), LW(10), LW(11))
    CALL(btlevtcmd_SetPartsWork, -2, 6, 3, LW(10))
    CALL(btlevtcmd_SetPartsWork, -2, 6, 4, LW(11))
    CALL(btlevtcmd_SetPartsWork, -2, 6, 5, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 6, 6, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 6, 7, 0)
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&partyWeapon_YoshiEggAttack_Minimini))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&partyWeapon_YoshiEggAttack_Minimini), LW(10), LW(11))
    CALL(btlevtcmd_SetPartsWork, -2, 5, 3, LW(10))
    CALL(btlevtcmd_SetPartsWork, -2, 5, 4, LW(11))
    CALL(btlevtcmd_SetPartsWork, -2, 5, 5, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 5, 6, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 5, 7, 0)
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&partyWeapon_YoshiEggAttack_Minimini))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&partyWeapon_YoshiEggAttack_Minimini), LW(10), LW(11))
    CALL(btlevtcmd_SetPartsWork, -2, 4, 3, LW(10))
    CALL(btlevtcmd_SetPartsWork, -2, 4, 4, LW(11))
    CALL(btlevtcmd_SetPartsWork, -2, 4, 5, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 4, 6, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 4, 7, 0)
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&partyWeapon_YoshiEggAttack_Minimini))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&partyWeapon_YoshiEggAttack_Minimini), LW(10), LW(11))
    CALL(btlevtcmd_SetPartsWork, -2, 3, 3, LW(10))
    CALL(btlevtcmd_SetPartsWork, -2, 3, 4, LW(11))
    CALL(btlevtcmd_SetPartsWork, -2, 3, 5, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 3, 6, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 3, 7, 0)
    CALL(evt_btl_camera_set_mode, 0, 10)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, -1, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 300)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_GetPartyTechLv, -2, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(2)
        SET_VALUE(LW(1), 4)
        CASE_DEFAULT
        SET_VALUE(LW(1), 3)
    END_SWITCH
    CALL(btlevtcmd_AcGetDifficulty, LW(0))
    SUBTRACT(LW(0), 3)
    SWITCH(LW(0))
        CASE_LESS_OR_EQUAL(-3)
        SET_VALUE(LW(0), 480)
        CASE_EQUAL(-2)
        SET_VALUE(LW(0), 360)
        CASE_EQUAL(-1)
        SET_VALUE(LW(0), 240)
        CASE_EQUAL(0)
        SET_VALUE(LW(0), 180)
        CASE_EQUAL(1)
        SET_VALUE(LW(0), 150)
        CASE_EQUAL(2)
        SET_VALUE(LW(0), 100)
        CASE_DEFAULT
        SET_VALUE(LW(0), 60)
    END_SWITCH
    CALL(btlevtcmd_AcSetParamAll, 0, LW(0), LW(1), 0, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AcSetFlag, 0)
    CALL(btlevtcmd_AcSetOutputParam, 1, 0)
    CALL(btlevtcmd_SetupAC, -2, 20, 1, 60)
    WAIT_FRAMES(22)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 272, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_StartAC, 1)
    SET_VALUE(LW(12), 2)
    LABEL(5)
    CALL(btlevtcmd_AcGetOutputParam, 0, LW(13))
    ADD(LW(13), 1)
    IF_INT_GREATER(LW(12), LW(13))
        CALL(btlevtcmd_AcGetOutputParam, 1, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_StopAC)
            WAIT_MS(500)
            CALL(btlevtcmd_AcGetOutputParam, 0, LW(13))
            IF_INT_GREATER_OR_EQUAL(LW(13), 1)
                CALL(btlevtcmd_AcGetOutputParam, 0, LW(13))
                ADD(LW(13), 1)
                GOTO(25)
            ELSE
                CALL(btlevtcmd_AudienceDeclareACResult, PTR(&partyWeapon_YoshiEggAttack_Minimini), -1)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_5"))
                WAIT_FRAMES(54)
                CALL(evt_audience_ap_recovery)
                CALL(btlevtcmd_InviteApInfoReport)
                GOTO(95)
            END_IF
        END_IF
        WAIT_FRAMES(1)
        GOTO(5)
    END_IF
    SET_VALUE(LW(0), LW(12))
    SUBTRACT(LW(0), 1)
    SWITCH(LW(0))
        CASE_LESS_OR_EQUAL(2)
        SET_VALUE(LW(0), 0)
        CASE_LESS_OR_EQUAL(3)
        SET_VALUE(LW(0), 1)
        CASE_DEFAULT
        SET_VALUE(LW(0), 2)
    END_SWITCH
    CALL(btlevtcmd_GetResultPrizeLv, -5, LW(0), LW(6))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 40)
    CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_AudienceDeclareACResult, PTR(&partyWeapon_YoshiEggAttack_Minimini), LW(6))
    LABEL(10)
    CALL(evt_sub_random, 2, LW(0))
    CALL(btlevtcmd_SetPartsWork, -2, LW(12), 0, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, LW(12), PTR(pose_table_egg_y))
        CASE_EQUAL(2)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, LW(12), PTR(pose_table_egg_p))
        CASE_DEFAULT
        CALL(btlevtcmd_AnimeSetPoseTable, -2, LW(12), PTR(pose_table_egg_g))
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(12), 69)
    CALL(btlevtcmd_SetPartsWork, -2, LW(12), 1, 4)
    CALL(btlevtcmd_SetPartsWork, -2, LW(12), 2, PTR(&partyWeapon_YoshiEggAttack_Minimini))
    LABEL(20)
    CALL(btlevtcmd_GetStatusMg, -2, LW(6))
    CALL(btlevtcmd_SetPartsScale, -2, LW(12), FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_8"))
    WAIT_FRAMES(10)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_12"))
    CALL(btlevtcmd_GetStatusMg, -2, LW(5))
    MULTIPLY_FLOAT(LW(5), 10)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), LW(5))
    ADD(LW(1), LW(5))
    CALL(btlevtcmd_SetPartsPos, -2, LW(12), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPartsDispOffset, -2, LW(12), 0, 0, 0)
    CALL(btlevtcmd_SetPartsRotate, -2, LW(12), 0, 0, 0)
    CALL(btlevtcmd_SetPartsRotateOffset, -2, LW(12), 0, 0, 0)
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(12), 50331648)
    CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_EGG1"), 0)
    SET_VALUE(LW(15), 0)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_SetPartsRotateOffset, -2, LW(12), 0, 6, 0)
        LOOP(30)
            CALL(btlevtcmd_AddPartsRotate, -2, LW(12), 0, 0, 48)
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_SetPartsRotate, -2, LW(12), 0, 0, 0)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        SET_VALUE(LW(5), LW(12))
        SUBTRACT(LW(5), 2)
        MULTIPLY_FLOAT(LW(5), 10)
        ADD_FLOAT(LW(5), 15)
        CALL(btlevtcmd_GetStatusMg, -2, LW(0))
        MULTIPLY_FLOAT(LW(5), LW(0))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), LW(5))
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetPartsFallAccel, -2, LW(12), FLOAT(0.2f))
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(12), LW(0), LW(1), LW(2), 30, -1)
    END_SIBLING_EVENT
    WAIT_FRAMES(10)
    ADD(LW(12), 1)
    GOTO(5)
    LABEL(25)
    CALL(btlevtcmd_SetPartsWork, -2, 2, 6, -1)
    CALL(btlevtcmd_SetPartsWork, -2, 3, 6, -1)
    CALL(btlevtcmd_SetPartsWork, -2, 4, 6, -1)
    CALL(btlevtcmd_SetPartsWork, -2, 5, 6, -1)
    CALL(btlevtcmd_SetPartsWork, -2, 6, 6, -1)
    CALL(btlevtcmd_SetPartsWork, -2, 7, 6, -1)
    SET_VALUE(LW(12), LW(13))
    LABEL(26)
    CALL(btlevtcmd_GetPartsWork, -2, LW(12), 6, LW(0))
    IF_INT_EQUAL(LW(0), -1)
        CALL(btlevtcmd_SetPartsWork, -2, LW(12), 6, 1)
        SET_VALUE(LW(14), LW(12))
        LABEL(27)
        SUBTRACT(LW(14), 1)
        IF_INT_GREATER_OR_EQUAL(LW(14), 2)
            CALL(btlevtcmd_GetPartsWork, -2, LW(12), 3, LW(0))
            CALL(btlevtcmd_GetPartsWork, -2, LW(14), 3, LW(1))
            IF_INT_EQUAL(LW(0), LW(1))
                CALL(btlevtcmd_SetPartsWork, -2, LW(14), 6, 0)
            END_IF
            GOTO(27)
        END_IF
    END_IF
    SUBTRACT(LW(12), 1)
    IF_INT_GREATER_OR_EQUAL(LW(12), 2)
        GOTO(26)
    END_IF
    SET_VALUE(LW(12), 2)
    LABEL(28)
    CALL(btlevtcmd_GetPartsWork, -2, LW(12), 6, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL(btlevtcmd_GetPartsWork, -2, LW(12), 3, LW(0))
        CALL(btlevtcmd_GetPartsWork, -2, LW(12), 4, LW(1))
        CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(0), LW(1), 256, LW(6))
        CALL(btlevtcmd_SetPartsWork, -2, LW(12), 5, LW(6))
        IF_INT_EQUAL(LW(6), 1)
            SET_VALUE(LW(14), LW(12))
            LABEL(29)
            ADD(LW(14), 1)
            IF_INT_LESS_OR_EQUAL(LW(14), LW(13))
                CALL(btlevtcmd_GetPartsWork, -2, LW(12), 3, LW(0))
                CALL(btlevtcmd_GetPartsWork, -2, LW(14), 3, LW(1))
                IF_INT_EQUAL(LW(0), LW(1))
                    CALL(btlevtcmd_GetPartsWork, -2, LW(14), 6, LW(0))
                    IF_INT_NOT_EQUAL(LW(0), 0)
                        CALL(btlevtcmd_SetPartsWork, -2, LW(14), 6, 2)
                    END_IF
                END_IF
                GOTO(29)
            END_IF
        END_IF
        CASE_EQUAL(1)
        CALL(btlevtcmd_GetPartsWork, -2, LW(12), 3, LW(0))
        CALL(btlevtcmd_GetPartsWork, -2, LW(12), 4, LW(1))
        CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(0), LW(1), 256, LW(6))
        CALL(btlevtcmd_SetPartsWork, -2, LW(12), 5, LW(6))
        CASE_DEFAULT
        CALL(btlevtcmd_GetPartsWork, -2, LW(12), 3, LW(0))
        CALL(btlevtcmd_GetPartsWork, -2, LW(12), 4, LW(1))
        CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(0), LW(1), 1048832, LW(6))
        CALL(btlevtcmd_SetPartsWork, -2, LW(12), 5, LW(6))
    END_SWITCH
    IF_INT_LESS(LW(12), LW(13))
        ADD(LW(12), 1)
        GOTO(28)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    SET_VALUE(LW(12), 2)
    LABEL(30)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(12), 50331648)
    CALL(btlevtcmd_GetPartsWork, -2, LW(12), 0, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        SET_VALUE(LW(6), STRING("PYS_A3_1"))
        SET_VALUE(LW(7), STRING("PYS_A3_2"))
        SET_VALUE(LW(8), STRING("PYS_A3_3"))
        CASE_EQUAL(2)
        SET_VALUE(LW(6), STRING("PYS_A4_1"))
        SET_VALUE(LW(7), STRING("PYS_A4_2"))
        SET_VALUE(LW(8), STRING("PYS_A4_3"))
        CASE_DEFAULT
        SET_VALUE(LW(6), STRING("PYS_A2_1"))
        SET_VALUE(LW(7), STRING("PYS_A2_2"))
        SET_VALUE(LW(8), STRING("PYS_A2_3"))
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(6))
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(8))
    WAIT_FRAMES(7)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL_CHILD(_egg_attack_event)
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetPartsWork, -2, LW(12), 7, LW(15))
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    WAIT_FRAMES(10)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 69)
    LABEL(90)
    IF_INT_NOT_EQUAL(LW(12), LW(13))
        ADD(LW(12), 1)
        CALL(btlevtcmd_GetPartsWork, -2, LW(12), 3, LW(3))
        CALL(btlevtcmd_GetPartsWork, -2, LW(12), 4, LW(4))
        GOTO(30)
    END_IF
    CALL(btlevtcmd_StopAC)
    CALL(btlevtcmd_ACRStart, -2, 0, 15, 15, 20)
    CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
    SWITCH(LW(6))
        CASE_GREATER_OR_EQUAL(2)
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(0), 1, 0, 0, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_Y_1"))
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_Y_1"))
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        CASE_DEFAULT
        CALL(evt_audience_acrobat_notry)
    END_SWITCH
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    LABEL(95)
    SET_VALUE(LW(12), 2)
    LABEL(96)
    CALL(btlevtcmd_GetPartsWork, -2, LW(12), 7, LW(15))
    IF_INT_NOT_EQUAL(LW(15), 0)
        CALL(btlevtcmd_WaitEventEnd, LW(15))
        CALL(btlevtcmd_SetPartsWork, -2, LW(12), 7, 0)
    END_IF
    ADD(LW(12), 1)
    IF_INT_LESS_OR_EQUAL(LW(12), LW(13))
        GOTO(96)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    WAIT_MS(2000)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(_egg_attack_event)
    CALL(btlevtcmd_GetStatusMg, -2, LW(6))
    MULTIPLY_FLOAT(LW(6), 10)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), LW(6))
    CALL(btlevtcmd_SetPartsPos, -2, LW(12), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(12), 50331648)
    CALL(btlevtcmd_SetPartsMoveSpeed, -2, LW(12), FLOAT(5.0f))
    CALL(btlevtcmd_SetPartsFallAccel, -2, LW(12), FLOAT(0.1f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_GetMoveFrame, -2, LW(0), LW(1), LW(2), FLOAT(5.0f), LW(5))
        CALL(btlevtcmd_GetStatusMg, -2, LW(6))
        MULTIPLY_FLOAT(LW(6), 6)
        CALL(btlevtcmd_SetPartsRotateOffset, -2, LW(12), 0, LW(6), 0)
        LOOP(LW(5))
            CALL(btlevtcmd_AddPartsRotate, -2, LW(12), 0, 0, 48)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_EGG3"), 0)
    CALL(btlevtcmd_DivePartsPosition, -2, LW(12), LW(0), LW(1), LW(2), 0, 25, 0, 0, -1)
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(12), 50331648)
    CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_EGG2"), 0)
    CALL(evt_eff, STRING(""), STRING("bomb"), 0, LW(0), LW(1), LW(2), FLOAT(0.6f), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_GetPartsWork, -2, LW(12), 5, LW(6))
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
        GOTO(89)
    END_IF
    CALL(btlevtcmd_GetPartsWork, -2, LW(12), 2, LW(0))
    CALL(btlevtcmd_GetPartsWork, -2, LW(12), 6, LW(1))
    IF_INT_EQUAL(LW(1), 0)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(0), 1073872896, LW(5))
    ELSE
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(0), 1073873152, LW(5))
    END_IF
    LABEL(89)
    RETURN
EVT_END()

EVT_BEGIN(partyYoshiAttack_CallGuard)
    SET_VALUE(LW(12), PTR(&partyWeapon_YoshiCallGuard))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(12))
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_AcGetDifficulty, LW(0))
    SUBTRACT(LW(0), 3)
    SWITCH(LW(0))
        CASE_LESS_OR_EQUAL(-3)
        SET_VALUE(LW(0), 280)
        SET_VALUE(LW(1), 18)
        SET_VALUE(LW(2), 25)
        CASE_EQUAL(-2)
        SET_VALUE(LW(0), 250)
        SET_VALUE(LW(1), 17)
        SET_VALUE(LW(2), 27)
        CASE_EQUAL(-1)
        SET_VALUE(LW(0), 240)
        SET_VALUE(LW(1), 16)
        SET_VALUE(LW(2), 29)
        CASE_EQUAL(0)
        SET_VALUE(LW(0), 240)
        SET_VALUE(LW(1), 15)
        SET_VALUE(LW(2), 30)
        CASE_EQUAL(1)
        SET_VALUE(LW(0), 200)
        SET_VALUE(LW(1), 15)
        SET_VALUE(LW(2), 30)
        CASE_EQUAL(2)
        SET_VALUE(LW(0), 180)
        SET_VALUE(LW(1), 15)
        SET_VALUE(LW(2), 30)
        CASE_DEFAULT
        SET_VALUE(LW(0), 150)
        SET_VALUE(LW(1), 14)
        SET_VALUE(LW(2), 30)
    END_SWITCH
    CALL(btlevtcmd_AcSetParamAll, 15, LW(0), 178, LW(1), LW(2), 20, 61, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AcSetGaugeParam, 60, 80, 100, 100)
    CALL(btlevtcmd_AcSetFlag, 64)
    CALL(btlevtcmd_SetupAC, -2, 6, 1, 0)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, 1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 4)
    CALL(evt_btl_camera_set_zoom, 0, 400)
    WAIT_FRAMES(20)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_10"))
    CALL(btlevtcmd_ftof, 24, LW(0))
    WAIT_FRAMES(LW(0))
    CALL(btlevtcmd_StartAC, 1)
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_11"))
    CALL(btlevtcmd_GetHeight, -2, LW(6))
    CALL(btlevtcmd_GetStatusMg, -2, LW(0))
    MULTIPLY_FLOAT(LW(6), LW(0))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), LW(6))
    CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
    MULTIPLY(LW(5), 30)
    CALL(evt_eff, 0, STRING("kiss"), 3, LW(0), LW(1), LW(2), LW(5), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_ENM_TOGENOKO_CALL1"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_ftof, 33, LW(0))
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        WAIT_FRAMES(LW(0))
    END_SIBLING_EVENT
    CALL(btlevtcmd_ACRStart, -2, 0, LW(0), LW(0), 20)
    CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
    SWITCH(LW(6))
        CASE_GREATER_OR_EQUAL(2)
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, 0, 0, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_Y_1"))
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_Y_1"))
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
        CASE_DEFAULT
        CALL(evt_audience_acrobat_notry)
        CALL(btlevtcmd_WaitEventEnd, LW(15))
        BEGIN_SIBLING_EVENT
            WAIT_MS(166)
            CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
        END_SIBLING_EVENT
    END_SWITCH
    BEGIN_INLINE_EVENT_GET_ID(LW(9))
        WAIT_MS(1000)
        CALL(_gundan_yoshi_run_effect, -2)
    END_INLINE_EVENT
    WAIT_MS(166)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_shake_h, 0, 1, 0, 6000, 0)
    WAIT_MS(1000)
    CALL(_wait_yoshig_run, -2)
    SET_VALUE(LW(15), 0)
    LABEL(70)
    IF_INT_EQUAL(LW(3), -1)
        IF_INT_NOT_EQUAL(LW(15), 0)
            CALL(btlevtcmd_WaitEventEnd, LW(15))
        END_IF
        CALL(btlevtcmd_WaitEventEnd, LW(9))
        GOTO(90)
    END_IF
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(6))
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
        GOTO(75)
    END_IF
    CALL(btlevtcmd_AcGetOutputParam, 2, LW(14))
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        LOOP(LW(14))
            IF_INT_GREATER_OR_EQUAL(LW(14), 2)
                CALL(btlevtcmd_AcGetOutputParam, 2, LW(0))
                IF_INT_GREATER_OR_EQUAL(LW(0), 2)
                    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 131072, LW(5))
                ELSE
                    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 0, LW(5))
                END_IF
            ELSE
                CALL(btlevtcmd_AcGetOutputParam, 2, LW(0))
                SWITCH(LW(0))
                    CASE_GREATER_OR_EQUAL(6)
                    SET_VALUE(LW(0), 2)
                    CASE_GREATER_OR_EQUAL(4)
                    SET_VALUE(LW(0), 1)
                    CASE_GREATER_OR_EQUAL(2)
                    SET_VALUE(LW(0), 0)
                    CASE_DEFAULT
                    SET_VALUE(LW(0), -1)
                END_SWITCH
                IF_INT_GREATER_OR_EQUAL(LW(0), 0)
                    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 131328, LW(5))
                    CALL(btlevtcmd_GetResultPrizeLv, LW(3), LW(0), LW(6))
                    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
                    CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
                    CALL(btlevtcmd_AudienceDeclareACResult, LW(12), LW(6))
                ELSE
                    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
                    CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
                END_IF
            END_IF
            WAIT_FRAMES(10)
        END_LOOP
    END_SIBLING_EVENT
    LABEL(75)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    GOTO(70)
    LABEL(90)
    CALL(_wait_yoshig_complete, -2)
    CALL(evt_btl_camera_noshake, 1)
    WAIT_MS(500)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(yoshi_appeal_event)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 66)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_APPEAL1"), LW(0), LW(1), LW(2), 0)
    WAIT_FRAMES(40)
    CALL(btlevtcmd_AudienceAppeal, -2)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(party_win_reaction)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_Y_1"))
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_Y_1"))
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
    RETURN
EVT_END()

static EVT_BEGIN(attack_audience_event)
    CALL(_get_attack_aud_no, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    SET_VALUE(LW(12), 2)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 10)
    CALL(btlevtcmd_SetPartsPos, -2, LW(12), LW(0), LW(1), LW(2))
    BEGIN_SIBLING_EVENT
        CALL(evt_btl_camera_set_mode, 0, 13)
        CALL(evt_btl_camera_set_homing_unit_audience, 0, -2, LW(12), LW(3))
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
        CALL(evt_btl_camera_set_zoom, 0, 200)
    END_SIBLING_EVENT
    CALL(evt_sub_random, 2, LW(0))
    CALL(btlevtcmd_SetPartsWork, -2, LW(12), 0, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, LW(12), PTR(pose_table_egg_y))
        CASE_EQUAL(2)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, LW(12), PTR(pose_table_egg_p))
        CASE_DEFAULT
        CALL(btlevtcmd_AnimeSetPoseTable, -2, LW(12), PTR(pose_table_egg_g))
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(12), 69)
    CALL(btlevtcmd_SetPartsMoveSpeed, -2, LW(12), FLOAT(7.5f))
    CALL(btlevtcmd_SetPartsFallAccel, -2, LW(12), FLOAT(0.1f))
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(12), 50331648)
    CALL(btlevtcmd_GetPartsWork, -2, LW(12), 0, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        SET_VALUE(LW(6), STRING("PYS_A3_1"))
        SET_VALUE(LW(7), STRING("PYS_A3_2"))
        SET_VALUE(LW(8), STRING("PYS_A3_3"))
        CASE_EQUAL(2)
        SET_VALUE(LW(6), STRING("PYS_A4_1"))
        SET_VALUE(LW(7), STRING("PYS_A4_2"))
        SET_VALUE(LW(8), STRING("PYS_A4_3"))
        CASE_DEFAULT
        SET_VALUE(LW(6), STRING("PYS_A2_1"))
        SET_VALUE(LW(7), STRING("PYS_A2_2"))
        SET_VALUE(LW(8), STRING("PYS_A2_3"))
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(6))
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(8))
    BEGIN_SIBLING_EVENT
        WAIT_MS(1000)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
    END_SIBLING_EVENT
    WAIT_FRAMES(7)
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(12), 50331648)
    CALL(_get_attack_aud_no, LW(3), LW(0), LW(1), LW(2))
    ADD(LW(1), 28)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_GetMoveFrame, -2, LW(0), LW(1), LW(2), FLOAT(5.0f), LW(5))
        CALL(btlevtcmd_SetPartsRotateOffset, -2, LW(12), 0, 6, 0)
        LOOP(LW(5))
            CALL(btlevtcmd_AddPartsRotate, -2, LW(12), 0, 0, 48)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_EGG3"), 0)
    CALL(btlevtcmd_DivePartsPosition, -2, LW(12), LW(0), LW(1), LW(2), 0, 25, 0, 0, -1)
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(12), 50331648)
    CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_EGG2"), 0)
    CALL(evt_eff, STRING(""), STRING("bomb"), 0, LW(0), LW(1), LW(2), FLOAT(0.6f), 0, 0, 0, 0, 0, 0, 0)
    CALL(_attack_aud, LW(3), 0)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        WAIT_MS(1500)
        CALL(evt_btl_camera_set_mode, 0, 0)
        WAIT_MS(500)
    END_SIBLING_EVENT
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

// TODO: type
u8 _yoshig_tpl_no_tbl[8] = { 0x46, 0x49, 0x47, 0x46, 0x4A, 0x48, 0x00, 0x00 };
// clang-format on
#pragma force_active off

#ifdef MUST_MATCH
// An ordinary, immediately generated function pools the i2f double before the deferred functions' strings
// ("SFX_BTL_YOSHI_TAIGUN1"); nothing calls it and the linker strips the body.
#pragma defer_codegen off
static f32 yoshi_pool_i2f(s32 v) {
    return (f32)v;
}
#pragma defer_codegen reset
#endif

// Reusing one variable for both evtGetValue and BattleTransID (`id = evtGetValue(...);
// id = BattleTransID(event, id);`) removed the extra `mr r0` the two-variable form emitted.
USER_FUNC(yoshi_original_color_anim_set) {
    s32 id;
    const char* name;
    BattleWork* work;
    s32* args;
    BattleUnitPart* part;

    args = event->args;
    work = _battleWorkPointer;
    id = evtGetValue(event, *args++);
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
        animPoseGetAnimPosePtr(part->poseId);
        if (part->poseId != -1) {
            animPoseRelease(part->poseId);
            part->poseId = -1;
        }
    }
    if (animEffectAsync(name, 1)) {
        part = BattleGetUnitPartsPtr(id, 1);
        part->poseId = animPoseEntry(name, 1);
        return EVT_RETURN_DONE;
    }
    return 0;
}

// .sdata2 constants used by the yoroyoro jump functions.

// Matched: the `if (tx - px > 0.0f)` comparison must use the literal `0.0f` (pooled as its own
// symbol) rather than the `0.0f` extern; that flips the FP load/compare registers to
// the target's. The remaining `4503601774854144.0` relocation names are objdiff-normalised.

static void __makeTechMenuFunc(BattleCommand* cmd, s32* count) {
    s32 techLv = partyGetTechLv(
        BattleTransPartyId(BattleGetPartyPtr(_battleWorkPointer)->currentKind));

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
}


// One of the 256 particles drawn by _gundan_yoshi_run_effect: a 0x30-byte entry whose
// display callback argument is the entry's +4 sub-structure. No header declares it.
typedef struct YoshiGundanDispWork {
    Vec pos;  // 0x00
    f32 velX; // 0x0C
    f32 field_0x10; // 0x10
    f32 velZ; // 0x14
    u8  field_0x18[0x0c]; // 0x18
    u8  animTimer; // 0x24
    u8  tplIndex; // 0x25, index into _yoshig_tpl_no_tbl (0..5)
    u8  field_0x26[2]; // 0x26
    s32 startDelay; // 0x28
    s32 state; // 0x2C
} YoshiGundanDispWork;


// .sdata/.sdata2 constants this unit references by name.
extern u8 _yoshig_tpl_no_tbl[8];


// The 0x300C-byte allocation `_gundan_yoshi_run_effect` keeps at unit+0x310: a state word,
// 256 0x30-byte particles, and a trailing accumulator. No header declares it.
typedef struct YoshiGundanArea {
    s32 state;                          // 0x0
    YoshiGundanDispWork field_0x4[256]; // 0x4
    u8 pad_0x3004[0x3008 - 0x3004];     // 0x3004
    f32 avgPosX;                        // 0x3008
} YoshiGundanArea;

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
    if (speed > 0.0f) {
        unit->mMoveSpeedXZ = speed;
    }
    if (unit->mMoveSpeedXZ <= 0.0f) {
        unit->mMoveSpeedXZ = 1.0f;
    }
    BtlUnit_SetMoveTargetPos(unit, tx, ty, tz);
    BtlUnit_GetPos(unit, &px, &py, &pz);
    BtlUnit_SetMoveStartPos(unit, px, py, pz);
    BtlUnit_SetMoveCurrentPos(unit, px, py, pz);
    unit->moveAngle = angleABf(px, pz, tx, tz);
    unit->moveDist = distABf(px, pz, tx, tz);
    if (tx - px > 0.0f) {
        unit->moveDirection = 1;
    } else if (tx - px < 0.0f) {
        unit->moveDirection = -1;
    }
    unit->moveDist = __fabs(tx - px);
    unit->moveFrames = 180;
    unit->moveProgress = (f32)unit->moveFrames;
    unit->mMoveSpeedY = 0.0f;
    unit->mMoveSpeedXZ = unit->moveDist / (f32)unit->moveFrames;
    evtSetFloat(event, speedVar, unit->mMoveSpeedXZ);
    evtSetValue(event, frameVar, unit->moveFrames);
    return 2;
}


// Matched. The post-increment branch must be an `else { effect = ...; if (...) }` nest so the scheduler
// hoists the event->userdata[2] load between the `cmpwi 0x10` and `bge`. `height` must stay an f32 local
// for the first block to match.
USER_FUNC(btl_yoshi_yoroyoro_jump_move) {
    s32 id;
    BattleWork* work;
    BattleUnit* unit;
    s32* args;
    EffectEntry* effect;
    f32 newSpeedY;
    f32 height;
    f32 halfHeight;
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
        } else if (unit->mMoveTargetPos.z - unit->mMoveStartPos.z > 0.0f) {
            unit->mMoveCurrentPos.z += unit->mMoveSpeedXZ;
        } else {
            unit->mMoveCurrentPos.z -= unit->mMoveSpeedXZ;
        }
    }
    newSpeedY = 10.f *
                sinfd(180.f +
                      360.f *
                          (unit->moveProgress -
                           (f32)(unit->moveFrames % 90)) /
                          90.f);
    BtlUnit_GetPartsPtr(unit, 1);
    unit->mMoveCurrentPos.y =
        unit->mMoveSpeedY +
        (unit->mMoveStartPos.y +
         (unit->mMoveTargetPos.y - unit->mMoveStartPos.y) *
             (unit->moveProgress - (f32)unit->moveFrames) /
             unit->moveProgress);
    unit->mMoveSpeedY = newSpeedY;
    unit->work[5] =
        (s32)(75.0f -
              25.0f *
                  sinfd(360.f *
                            (unit->moveProgress -
                             (f32)(unit->moveFrames % 90)) /
                        90.f));
    BtlUnit_SetPos(unit, unit->mMoveCurrentPos.x, unit->mMoveCurrentPos.y, unit->mMoveCurrentPos.z);
    event->userdata[1]++;
    if (event->userdata[1] > 20) {
        event->userdata[1] = 0;
        if (unit->moveDirection >= 0) {
            newSpeedY = -50.f;
        } else {
            newSpeedY = 50.f;
        }
        height = BtlUnit_GetHeight(unit);
        event->userdata[2] = (s32)effSweatN64Entry(
            1, unit->mMoveCurrentPos.x,
            unit->mSizeMultiplier * (0.5f * height) + unit->mMoveCurrentPos.y,
            unit->mMoveCurrentPos.z, 5.0f * unit->mSizeMultiplier, newSpeedY, 16);
    } else {
        effect = (EffectEntry*)event->userdata[2];
        if (event->userdata[1] < 16) {
            ((effSweatData*)effect->userdata)->position.x = unit->mMoveCurrentPos.x;
            halfHeight = 0.5f * BtlUnit_GetHeight(unit);
            ((effSweatData*)effect->userdata)->position.y =
                unit->mSizeMultiplier * halfHeight + unit->mMoveCurrentPos.y;
            ((effSweatData*)effect->userdata)->position.z = unit->mMoveCurrentPos.z;
        }
    }
    unit->moveFrames -= 1;
    if (unit->moveFrames <= 0) {
        BtlUnit_SetPos(unit, unit->mMoveTargetPos.x, unit->mMoveTargetPos.y, unit->mMoveTargetPos.z);
        return 2;
    }
    return 0;
}


s32 _yoshi_slide_move_sound(BattleUnit* unit, BtlUnitPoseSound* entry, void* ps) {
    return BtlUnit_snd_se(unit, entry->soundName, 0xF1194D80, (s16)(unit->work[4] * unit->work[5] / 100));
}


// Matched by mirroring the identical, already-matched `_get_bomb_hit_position` in
// unit_party_sanders.c: same declaration order (the order fixes MWCC's callee-saved
// allocation: event r29, work r28, id1 r26, id2 r25, outX/outY/outZ r31/r30/r26) and the
// same `Vec pos` + inline `mSizeMultiplier` shape.
USER_FUNC(_get_nomikomi_hit_position) {
    BattleUnit* unit;
    BattleWork* work = _battleWorkPointer;
    Vec pos;
    s32* args = event->args;
    s32 id1;
    s32 id2;
    s32 dir;
    s32 outZ;
    s32 outX;
    s32 outY;

    id1 = evtGetValue(event, args[0]);
    id2 = evtGetValue(event, args[1]);
    evtGetValue(event, args[2]);
    outX = args[3];
    outY = args[4];
    outZ = args[5];
    BattleGetUnitPtr(work, BattleTransID(event, id1));
    unit = BattleGetUnitPtr(work, BattleTransID(event, id2));
    dir = unit->faceDirection;
    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    pos.x += (f32)dir * (unit->bintaHitOffset.x * unit->mSizeMultiplier);
    pos.y += unit->bintaHitOffset.y * unit->mSizeMultiplier;
    pos.z += unit->bintaHitOffset.z * unit->mSizeMultiplier;
    evtSetFloat(event, outX, pos.x);
    evtSetFloat(event, outY, pos.y);
    evtSetFloat(event, outZ, pos.z);
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
    YoshiGundanArea* area;

    val = evtGetValue(event, event->args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, val));
    area = unit->mExtraWork;
    if (area == NULL) {
        return 0;
    }
    if (area->avgPosX <= 0.0f) {
        return 0;
    }
    return EVT_RETURN_DONE;
}


USER_FUNC(_wait_yoshig_complete) {
    BattleUnit* unit;
    s32 val;

    val = evtGetValue(event, event->args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, val));
    return unit->mExtraWork ? 0 : EVT_RETURN_DONE;
}


// Matched. The zone constants must be literals (`80.0f`/`-80.0f`/`300.0f`) while `-1.0f` stays the
// `-1.0f` extern, and `active` must be reached with a `goto` label placed after the
// `default:` case so MWCC shares the r0 `li` block instead of tail-duplicating it.
USER_FUNC(_gundan_yoshi_run_effect) {
    Vec pos;
    f32 scale;
    BattleUnit* unit;
    s32 active;
    YoshiGundanArea* area;
    s32 val;
    YoshiGundanDispWork* work;
    s32 i;
    s32 done;
    s32 j;

    val = evtGetValue(event, event->args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, val));
    if (isFirstCall) {
        area = BattleAlloc(0x300C);
        unit->mExtraWork = area;
        memset(area, 0, 0x300C);
        area->state = 0;
        area->avgPosX = 0.0f;
        event->userdata[0] = 0;
        event->userdata[1] = psndSFXOn("SFX_BTL_YOSHI_TAIGUN1");
    } else {
        area = unit->mExtraWork;
    }
    switch (area->state) {
    case 0:
        area->state++;
        // fallthrough
    case 1:
        done = 1;
        area->avgPosX = 0.0f;
        for (i = 0; i < 256; i++) {
            work = &area->field_0x4[i];
            work->animTimer++;
            if (work->animTimer > 2) {
                work->animTimer = 0;
                work->tplIndex++;
                if (work->tplIndex >= 6) {
                    work->tplIndex = 0;
                }
            }
            switch (work->state) {
            case 0:
                work->state++;
                work->startDelay = irand(3);
                work->pos.x = (f32)(-(30 * (i / 16) + 300) + irand(10));
                work->pos.y = 0.0f;
                work->pos.z = (f32)((i % 16) * 10 - 80);
                work->tplIndex = irand(6);
                work->animTimer = irand(5);
                work->velX = 3.0f + 0.1f * (f32)irand(10);
                work->field_0x10 = 0.0f;
                work->velZ = 0.001f * (f32)(irand(2000) - 1000);
                // fallthrough
            case 1:
                if (work->startDelay > 0) {
                    work->startDelay--;
                    goto yoshi_active; // shares case 2's `active = 1` tail; a second store changes the object
                }
                // fallthrough
            case 2:
                work->pos.z += work->velZ;
                if (work->pos.z >= 80.0f) {
                    work->pos.z = 80.0f;
                    work->velZ *= -1.0f;
                }
                if (work->pos.z <= -80.0f) {
                    work->pos.z = -80.0f;
                    work->velZ *= -1.0f;
                }
                work->pos.x += work->velX;
                if (work->pos.x >= 300.0f) {
                    work->pos.x = 300.0f;
                    work->velX = 0.0f;
                    work->state = 999;
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
            area->avgPosX += work->pos.x;
        }
        if (event->userdata[0] > 0) {
            event->userdata[0]--;
        } else {
            work = &area->field_0x4[irand(256)];
            scale = 1.0f + 0.05f * (f32)irand(20);
            effKemuTestEntry(0, work->pos.x,
                             5.0f + work->pos.y + (f32)irand(15),
                             65.0f, scale);
        }
        area->avgPosX *= 0.00390625f;
        pos.x = area->avgPosX;
        pos.y = 0.0f;
        pos.z = 0.0f;
        psndSFX_pos(event->userdata[1], &pos);
        if (done) {
            area->state = 999;
        }
        break;
    case 999:
        psndSFXOff(event->userdata[1]);
        BattleFree(unit->mExtraWork);
        unit->mExtraWork = NULL;
        return 2;
    default:
        break;
    }
    for (j = 0; j < 256; j++) {
        dispEntry(4, dispCalcZ(&work->pos), 2, _btlYoshiDisp,
                  &area->field_0x4[j]);
    }
    return 0;
}


void _btlYoshiDisp(CameraId cameraId, void* param) {
    YoshiGundanDispWork* work = param;
    GXColor color;
    f32 x_scale;
    static const GXColor white = { 0xFF, 0xFF, 0xFF, 0xFF };

    x_scale = (work->velX < 0.0f) ? 0.4f : -0.4f;
    color = white;
    btlDispTexPlane(work->pos.x, 20.f + work->pos.y, work->pos.z, x_scale, 0.4f, _yoshig_tpl_no_tbl[work->tplIndex],
                    color, 0);
}

// TODO: type
