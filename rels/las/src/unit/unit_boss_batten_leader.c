#include "unit_boss_batten_leader.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_msg.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"
#include "battle/battle_sub.h"
#include "manager/evtmgr_cmd.h"
#include "driver/animdrv.h"
#include "eff/n64/eff_pokopi_thunder_n64.h"
#include "eff/n64/eff_glass_n64.h"
#include "memory.h"
#include "driver/mapdrv.h"
#include "system.h"
#include "eff/eff_syuryou.h"
#include "math_ext.h"
#include <stdlib.h>

#pragma pool_data off

#pragma force_active on
extern s32 bts_x_flag_add[];
extern s32 bts_x_flag_set[];
extern s32 bts_x_flag_wait[];
extern BattleUnitKind unit_boss_batten_satellite;
static USER_FUNC(_set_draw_callback);
static USER_FUNC(enzan_lw_load);
static USER_FUNC(enzan_lw_save);
static USER_FUNC(eff_staff);
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[3];
static BattleUnitKindPart parts[7];
static s32 init_event[149];
static s32 entry_event[94];
static s32 damage_event[47];
s32 damage_sub_event[148];
static s32 phase_event[28];
s32 monster_before_phase_event[195];
static s32 dead_event[165];
s32 sub_call_satellite[87];
s32 player_before_phase_event[170];
static s32 attack_event[141];
s32 batten_leader_thunder_attack_event[377];
s32 batten_leader_fire_attack_event[376];
s32 batten_leader_ice_attack_event[321];
s32 batten_leader_shikaeshi_attack_event[176];
s32 batten_leader_yokeyoke_attack_event[176];
s32 batten_leader_stop_attack_event[328];
static s32 wait_event[16];

// clang-format off
extend_data(156,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(157,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

EVT_BEGIN(batten_leader_zoom)
    CALL(evt_btl_camera_set_mode, 0, 3)
    IF_INT_NOT_EQUAL(LW(0), 2)
        SET_VALUE(LW(1), 30)
    ELSE
        SET_VALUE(LW(1), 1)
    END_IF
    CALL(evt_btl_camera_set_moveto, 0, 83, 84, 349, 83, 60, 0, LW(1), 11)
    IF_INT_EQUAL(LW(0), 1)
        WAIT_MS(600)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(mario_party_zoom)
    CALL(evt_btl_camera_set_mode, 0, 3)
    IF_INT_NOT_EQUAL(LW(0), 2)
        SET_VALUE(LW(1), 30)
    ELSE
        SET_VALUE(LW(1), 1)
    END_IF
    CALL(evt_btl_camera_set_moveto, 0, -120, 58, 257, -120, 40, 0, LW(1), 11)
    IF_INT_EQUAL(LW(0), 1)
        WAIT_MS(600)
    END_IF
    RETURN
EVT_END()

BattleWeapon weapon_batten_leader_thunder = {
    NULL,                                     // name
    0,                                        // icon
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
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 7, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    4,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000047,                               // special
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
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // stageEffect
};

BattleWeapon weapon_batten_leader_fire = {
    NULL,                                     // name
    0,                                        // icon
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
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 6, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    1,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000047,                               // special
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
    20,                                       // burnChance
    3,                                        // burnTime
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
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // stageEffect
};

BattleWeapon weapon_batten_leader_ice = {
    NULL,                                     // name
    0,                                        // icon
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
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 5, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    2,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000047,                               // special
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
    30,                                       // freezeChance
    2,                                        // freezeTime
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
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // stageEffect
};

BattleWeapon weapon_batten_leader_shikaeshi = {
    NULL,                                     // name
    0,                                        // icon
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
    1,                                        // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01004020,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000008,                               // special
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
    3,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // stageEffect
};

BattleWeapon weapon_batten_leader_yokeyoke = {
    NULL,                                     // name
    0,                                        // icon
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
    1,                                        // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01004020,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000008,                               // special
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
    100,                                      // dodgyChance
    3,                                        // dodgyTime
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
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // stageEffect
};

BattleWeapon weapon_batten_leader_stop = {
    NULL,                                     // name
    0,                                        // icon
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
    1,                                        // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000007,                               // special
    0x000007FF,                               // counterResistanceFlags
    0,                                        // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    30,                                       // stopChance
    2,                                        // stopTime
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
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // stageEffect
};

BattleUnitKind unit_boss_batten_leader = {
    UNIT_GRODUS,                // mUnitType
    "btl_un_batten_leader",     // name
    50,                         // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    87,                         // mLevel
    0,                          // mBonusExp
    0,                          // bonusCoinRolls
    100,                        // bonusCoinRate
    0,                          // baseCoin
    50,                         // escapeRate
    6,                          // renzokuSoftCap
    68,                         // width
    66,                         // height
    { 7, 70 },                  // statusIconOffset
    { 0.0f, 33.0f, 0.0f },      // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },       // talkTogeBaseOffset
    { 34.0f, 0.0f, -10.0f },    // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // kagenukeOffsetX
    0.0f,                       // kagenukeOffsetZ
    { 34.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 20.0f, 42.9f, 10.0f },    // kissHitOffset
    { 0.0f, 33.0f, 0.0f },      // cutBaseOffset
    68.0f,                      // cutWidth
    66.0f,                      // cutHeight
    1,                          // turnOrder
    0,                          // turnOrderVariance
    0,                          // swallowRate
    2,                          // swallowAttribute
    100,                        // shotDamageRate
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_BOSS_SRYO_DAMAGED1",   // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0,                          // attributeFlags
    &regist,                    // vulnStatus
    7,                          // numParts
    parts,                      // mParts
    init_event,                 // initEvtScript
    data_table,                 // dataTable
};

static BattleUnitDefense defence = { { 1, 1, 1, 1, 1 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

BattleUnitDefenseAttr defence_attr_muteki = { { 2, 2, 2, 2, 2 } };

static BattleVulnerableStatus regist = {
    30,  // sleepRate
    30,  // stopRate
    30,  // dizzyRate
    100, // poisonRate
    30,  // confuseRate
    100, // electricRate
    100, // burnRate
    30,  // freezeRate
    100, // hugeRate
    60,  // tinyRate
    100, // attackUpRate
    60,  // attackDownRate
    100, // defenseUpRate
    75,  // defenseDownRate
    30,  // allergicRate
    0,   // frightRate
    0,   // galeForceRate
    100, // fastRate
    30,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    1,   // instaKillRate
};

// TODO: type
u8 defencecopy[8] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
};

static BattleDispPose pose_table[18] = {
    { 0x01, "N_1" },
    { 0x02, "Y_1" },
    { 0x09, "Y_1" },
    { 0x05, "K_1" },
    { 0x04, "X_1" },
    { 0x03, "K_1" },
    { 0x1C, "S_1" },
    { 0x1D, "Q_1" },
    { 0x1E, "Q_1" },
    { 0x1F, "D_1" },
    { 0x27, "D_1" },
    { 0x32, "A_2" },
    { 0x2A, "R_1" },
    { 0x28, "W_1" },
    { 0x38, "X_1" },
    { 0x39, "X_1" },
    { 0x41, "T_1" },
    { 0x45, "S_1" },
};

static BattleDispPose pose_table_furafura[18] = {
    { 0x01, "N_1" },
    { 0x02, "Y_1" },
    { 0x09, "Y_1" },
    { 0x05, "K_1" },
    { 0x04, "X_1" },
    { 0x03, "K_1" },
    { 0x1C, "X_1" },
    { 0x1D, "Q_1" },
    { 0x1E, "Q_1" },
    { 0x1F, "D_1" },
    { 0x27, "D_1" },
    { 0x32, "A_2" },
    { 0x2A, "R_1" },
    { 0x28, "W_1" },
    { 0x38, "X_1" },
    { 0x39, "X_1" },
    { 0x41, "T_3" },
    { 0x45, "X_1" },
};

static BattleDataEntry data_table[3] = {
    { 0x09, damage_sub_event },
    { 0x31, dead_event },
    { 0x00, NULL },
};

static BattleUnitKindPart parts[7] = {
    {
        1,                      // partNum
        "btl_un_batten_leader", // partName
        "c_shuryo",             // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 0.0f, 56.0f, 0.0f },  // hitBaseOffset
        { -6.0f, 55.0f, 0.0f }, // hitCursorBaseOffset
        53,                     // shadowSize
        69,                     // field_0x32
        255,                    // baseAlpha
        &defence,               // defense
        &defence_attr,          // defenseAttr
        0x00000009,             // attributes
        0,                      // counterAttributes
        pose_table,             // poseTable
    },
    {
        2,                      // partNum
        "btl_un_batten_leader", // partName
        "c_shuryo",             // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 18.0f, 9.0f, 0.0f },  // hitBaseOffset
        { 16.0f, 15.0f, 0.0f }, // hitCursorBaseOffset
        64,                     // shadowSize
        36,                     // field_0x32
        255,                    // baseAlpha
        &defence,               // defense
        &defence_attr,          // defenseAttr
        0x80800000,             // attributes
        0,                      // counterAttributes
        NULL,                   // poseTable
    },
    {
        3,                     // partNum
        "電源ランプ",               // partName
        "c_shuryo",            // modelName
        { 0.0f, 80.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f },  // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },  // hitCursorBaseOffset
        15,                    // shadowSize
        15,                    // field_0x32
        255,                   // baseAlpha
        &defence,              // defense
        &defence_attr,         // defenseAttr
        0x02010000,            // attributes
        0,                     // counterAttributes
        pose_table,            // poseTable
    },
    {
        4,                     // partNum
        "頭のランプ１",              // partName
        "c_shuryo",            // modelName
        { 0.0f, 80.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f },  // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },  // hitCursorBaseOffset
        15,                    // shadowSize
        15,                    // field_0x32
        255,                   // baseAlpha
        &defence,              // defense
        &defence_attr,         // defenseAttr
        0x02010000,            // attributes
        0,                     // counterAttributes
        pose_table,            // poseTable
    },
    {
        5,                     // partNum
        "頭のランプ２",              // partName
        "c_shuryo",            // modelName
        { 0.0f, 80.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f },  // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },  // hitCursorBaseOffset
        15,                    // shadowSize
        15,                    // field_0x32
        255,                   // baseAlpha
        &defence,              // defense
        &defence_attr,         // defenseAttr
        0x02010000,            // attributes
        0,                     // counterAttributes
        pose_table,            // poseTable
    },
    {
        6,                     // partNum
        "頭のランプ３",              // partName
        "c_shuryo",            // modelName
        { 0.0f, 80.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f },  // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },  // hitCursorBaseOffset
        15,                    // shadowSize
        15,                    // field_0x32
        255,                   // baseAlpha
        &defence,              // defense
        &defence_attr,         // defenseAttr
        0x02010000,            // attributes
        0,                     // counterAttributes
        pose_table,            // poseTable
    },
    {
        7,                     // partNum
        "杖のランプ１",              // partName
        "c_shuryo",            // modelName
        { 0.0f, 80.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f },  // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },  // hitCursorBaseOffset
        15,                    // shadowSize
        15,                    // field_0x32
        255,                   // baseAlpha
        &defence,              // defense
        &defence_attr,         // defenseAttr
        0x02010000,            // attributes
        0,                     // counterAttributes
        pose_table,            // poseTable
    },
};

EVT_BEGIN(hakai_snd_event)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_BARRIER_BREAK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    RETURN
EVT_END()

// MUST_MATCH: file-scope so MWCC emits it into .rodata before the btl_shuryolight_* label
// strings, matching the original VecData$634 offset.
static const Vec VecData[10] = {
    { 38.0f, 45.0f, 5.0f },
    { 38.0f, 45.0f, 5.0f },
    { -38.0f, 45.0f, 5.0f },
    { 0.0f, 83.0f, 5.0f },
    { 38.0f, 7.0f, 5.0f },
    { -38.0f, 7.0f, 5.0f },
    { 38.0f, 83.0f, 5.0f },
    { -38.0f, 83.0f, 5.0f },
    { 38.0f, 7.0f, 5.0f },
    { -38.0f, 7.0f, 5.0f },
};

BattleUnitSetup entry_batten_satellite[1] = {
    { &unit_boss_batten_satellite, 1, 0x04000004, { 1000.0f, 0.0f, 1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

// TODO: type
u32 btl_shuryolight_pow_normal[7] = {
    (u32)"L_1B", 0xFFFFFFFE, (u32)"L_1A", 0x00000050,
    (u32)"L_1B", 0x00000050, 0x00000000,
};

char* btl_shuryolight_pow_tenmetu2[3] = {
    "L_4E", NULL, NULL,
};

// TODO: type
u32 btl_shuryolight_pow_tenmetu[5] = {
    (u32)"L_1B", 0xFFFFFFFD, (u32)"L_1A", 0x000000C8,
    0x00000000,
};

// TODO: type
u32 btl_shuryolight_head_1_normal[9] = {
    (u32)"L_4A", 0x00000190, (u32)"L_4A", 0x00000190,
    (u32)"L_4B", 0x00000190, (u32)"L_4A", 0x00000190,
    0x00000000,
};

// TODO: type
u32 btl_shuryolight_head_2_normal[9] = {
    (u32)"L_3A", 0x00000190, (u32)"L_3B", 0x00000190,
    (u32)"L_3A", 0x00000190, (u32)"L_3A", 0x00000190,
    0x00000000,
};

// TODO: type
u32 btl_shuryolight_head_3_normal[9] = {
    (u32)"L_2B", 0x00000190, (u32)"L_2A", 0x00000190,
    (u32)"L_2A", 0x00000190, (u32)"L_2A", 0x00000190,
    0x00000000,
};

// TODO: type
u32 btl_shuryolight_head_1_quick[9] = {
    (u32)"L_4A", 0x000000C8, (u32)"L_4A", 0x000000C8,
    (u32)"L_4B", 0x000000C8, (u32)"L_4A", 0x000000C8,
    0x00000000,
};

// TODO: type
u32 btl_shuryolight_head_2_quick[9] = {
    (u32)"L_3A", 0x000000C8, (u32)"L_3B", 0x000000C8,
    (u32)"L_3A", 0x000000C8, (u32)"L_3A", 0x000000C8,
    0x00000000,
};

// TODO: type
u32 btl_shuryolight_head_3_quick[9] = {
    (u32)"L_2B", 0x000000C8, (u32)"L_2A", 0x000000C8,
    (u32)"L_2A", 0x000000C8, (u32)"L_2A", 0x000000C8,
    0x00000000,
};

// TODO: type
u32 btl_shuryolight_head_1_random[5] = {
    (u32)"L_4A", 0xFFFFFFFC, (u32)"L_4B", 0xFFFFFFFC,
    0x00000000,
};

// TODO: type
u32 btl_shuryolight_head_2_random[5] = {
    (u32)"L_3A", 0xFFFFFFFC, (u32)"L_3B", 0xFFFFFFFC,
    0x00000000,
};

// TODO: type
u32 btl_shuryolight_head_3_random[5] = {
    (u32)"L_2B", 0xFFFFFFFC, (u32)"L_2A", 0xFFFFFFFC,
    0x00000000,
};

char* btl_shuryolight_head_1_tenmetu[3] = {
    "L_4E", NULL, NULL,
};

char* btl_shuryolight_head_2_tenmetu[3] = {
    "L_3E", NULL, NULL,
};

char* btl_shuryolight_head_3_tenmetu[3] = {
    "L_2E", NULL, NULL,
};

// TODO: type
u32 btl_shuryolight_head_1_off[3] = {
    (u32)"L_4A", 0xFFFFFFFF, 0x00000000,
};

// TODO: type
u32 btl_shuryolight_head_2_off[3] = {
    (u32)"L_3A", 0xFFFFFFFF, 0x00000000,
};

// TODO: type
u32 btl_shuryolight_head_3_off[3] = {
    (u32)"L_2A", 0xFFFFFFFF, 0x00000000,
};

// TODO: type
u32 btl_shuryolight_head_1_on[3] = {
    (u32)"L_4B", 0xFFFFFFFF, 0x00000000,
};

// TODO: type
u32 btl_shuryolight_head_2_on[3] = {
    (u32)"L_3B", 0xFFFFFFFF, 0x00000000,
};

// TODO: type
u32 btl_shuryolight_head_3_on[3] = {
    (u32)"L_2B", 0xFFFFFFFF, 0x00000000,
};

// TODO: type
u32 btl_shuryolight_rod_normal[9] = {
    (u32)"L_3C", 0x00000000, (u32)"L_3B", 0x000003E8,
    (u32)"L_3D", 0x00000000, (u32)"L_3A", 0x000003E8,
    0x00000000,
};

// TODO: type
u32 btl_shuryolight_rod_off[3] = {
    (u32)"L_3A", 0xFFFFFFFF, 0x00000000,
};

EVT_BEGIN(light_anim_rtn)
    BEGIN_SIBLING_EVENT
        SET_INT_READ_ADDR(LW(6))
        LOOP(0)
            READ_INT(LW(1))
            IF_INT_EQUAL(LW(1), 0)
                SET_INT_READ_ADDR(LW(6))
                READ_INT(LW(1))
            END_IF
            CALL(btlevtcmd_AnimeChangePose, -2, 4, LW(1))
            READ_INT(LW(1))
            IF_INT_GREATER(LW(1), 0)
                WAIT_MS(LW(1))
            ELSE
                SWITCH(LW(1))
                    CASE_EQUAL(0)
                    WAIT_FRAMES(1)
                    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 4)
                    CASE_EQUAL(-1)
                    BREAK_LOOP
                    CASE_EQUAL(-2)
                    LOOP(0)
                        CALL(evt_sub_random, 3000, LW(0))
                        ADD(LW(0), 3000)
                        WAIT_MS(LW(0))
                        CALL(evt_sub_random, 100, LW(0))
                        DIVIDE(LW(0), 30)
                        IF_INT_GREATER(LW(0), 0)
                            BREAK_LOOP
                        END_IF
                    END_LOOP
                    READ_INT4(LW(1), LW(2), LW(3), LW(4))
                    LOOP(LW(0))
                        CALL(btlevtcmd_AnimeChangePose, -2, 4, LW(1))
                        WAIT_MS(LW(2))
                        CALL(btlevtcmd_AnimeChangePose, -2, 4, LW(3))
                        WAIT_MS(LW(4))
                    END_LOOP
                    CASE_EQUAL(-3)
                    CALL(evt_sub_random, 3000, LW(0))
                    CALL(evt_sub_random, 1000, LW(2))
                    ADD(LW(0), LW(2))
                    ADD(LW(0), 700)
                    WAIT_MS(LW(0))
                    CASE_EQUAL(-4)
                    CALL(evt_sub_random, 300, LW(0))
                    ADD(LW(0), 100)
                    WAIT_MS(LW(0))
                END_SWITCH
            END_IF
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        SET_INT_READ_ADDR(LW(7))
        LOOP(0)
            READ_INT(LW(1))
            IF_INT_EQUAL(LW(1), 0)
                SET_INT_READ_ADDR(LW(7))
                READ_INT(LW(1))
            END_IF
            CALL(btlevtcmd_AnimeChangePose, -2, 5, LW(1))
            READ_INT(LW(1))
            IF_INT_GREATER(LW(1), 0)
                WAIT_MS(LW(1))
            ELSE
                SWITCH(LW(1))
                    CASE_EQUAL(0)
                    WAIT_FRAMES(1)
                    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 5)
                    CASE_EQUAL(-1)
                    BREAK_LOOP
                    CASE_EQUAL(-2)
                    LOOP(0)
                        CALL(evt_sub_random, 3000, LW(0))
                        ADD(LW(0), 3000)
                        WAIT_MS(LW(0))
                        CALL(evt_sub_random, 100, LW(0))
                        DIVIDE(LW(0), 30)
                        IF_INT_GREATER(LW(0), 0)
                            BREAK_LOOP
                        END_IF
                    END_LOOP
                    READ_INT4(LW(1), LW(2), LW(3), LW(4))
                    LOOP(LW(0))
                        CALL(btlevtcmd_AnimeChangePose, -2, 5, LW(1))
                        WAIT_MS(LW(2))
                        CALL(btlevtcmd_AnimeChangePose, -2, 5, LW(3))
                        WAIT_MS(LW(4))
                    END_LOOP
                    CASE_EQUAL(-3)
                    CALL(evt_sub_random, 3000, LW(0))
                    CALL(evt_sub_random, 1000, LW(2))
                    ADD(LW(0), LW(2))
                    ADD(LW(0), 700)
                    WAIT_MS(LW(0))
                    CASE_EQUAL(-4)
                    CALL(evt_sub_random, 300, LW(0))
                    ADD(LW(0), 100)
                    WAIT_MS(LW(0))
                END_SWITCH
            END_IF
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        SET_INT_READ_ADDR(LW(8))
        LOOP(0)
            READ_INT(LW(1))
            IF_INT_EQUAL(LW(1), 0)
                SET_INT_READ_ADDR(LW(8))
                READ_INT(LW(1))
            END_IF
            CALL(btlevtcmd_AnimeChangePose, -2, 6, LW(1))
            READ_INT(LW(1))
            IF_INT_GREATER(LW(1), 0)
                WAIT_MS(LW(1))
            ELSE
                SWITCH(LW(1))
                    CASE_EQUAL(0)
                    WAIT_FRAMES(1)
                    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 6)
                    CASE_EQUAL(-1)
                    BREAK_LOOP
                    CASE_EQUAL(-2)
                    LOOP(0)
                        CALL(evt_sub_random, 3000, LW(0))
                        ADD(LW(0), 3000)
                        WAIT_MS(LW(0))
                        CALL(evt_sub_random, 100, LW(0))
                        DIVIDE(LW(0), 30)
                        IF_INT_GREATER(LW(0), 0)
                            BREAK_LOOP
                        END_IF
                    END_LOOP
                    READ_INT4(LW(1), LW(2), LW(3), LW(4))
                    LOOP(LW(0))
                        CALL(btlevtcmd_AnimeChangePose, -2, 6, LW(1))
                        WAIT_MS(LW(2))
                        CALL(btlevtcmd_AnimeChangePose, -2, 6, LW(3))
                        WAIT_MS(LW(4))
                    END_LOOP
                    CASE_EQUAL(-3)
                    CALL(evt_sub_random, 3000, LW(0))
                    CALL(evt_sub_random, 1000, LW(2))
                    ADD(LW(0), LW(2))
                    ADD(LW(0), 700)
                    WAIT_MS(LW(0))
                    CASE_EQUAL(-4)
                    CALL(evt_sub_random, 300, LW(0))
                    ADD(LW(0), 100)
                    WAIT_MS(LW(0))
                END_SWITCH
            END_IF
        END_LOOP
    END_SIBLING_EVENT
    SET_INT_READ_ADDR(LW(5))
    LOOP(0)
        READ_INT(LW(1))
        IF_INT_EQUAL(LW(1), 0)
            SET_INT_READ_ADDR(LW(5))
            READ_INT(LW(1))
        END_IF
        CALL(btlevtcmd_AnimeChangePose, -2, 3, LW(1))
        READ_INT(LW(1))
        IF_INT_GREATER(LW(1), 0)
            WAIT_MS(LW(1))
        ELSE
            SWITCH(LW(1))
                CASE_EQUAL(0)
                WAIT_FRAMES(1)
                CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 3)
                CASE_EQUAL(-1)
                BREAK_LOOP
                CASE_EQUAL(-2)
                LOOP(0)
                    CALL(evt_sub_random, 3000, LW(0))
                    ADD(LW(0), 3000)
                    WAIT_MS(LW(0))
                    CALL(evt_sub_random, 100, LW(0))
                    DIVIDE(LW(0), 30)
                    IF_INT_GREATER(LW(0), 0)
                        BREAK_LOOP
                    END_IF
                END_LOOP
                READ_INT4(LW(1), LW(2), LW(3), LW(4))
                LOOP(LW(0))
                    CALL(btlevtcmd_AnimeChangePose, -2, 3, LW(1))
                    WAIT_MS(LW(2))
                    CALL(btlevtcmd_AnimeChangePose, -2, 3, LW(3))
                    WAIT_MS(LW(4))
                END_LOOP
                CASE_EQUAL(-3)
                CALL(evt_sub_random, 3000, LW(0))
                CALL(evt_sub_random, 1000, LW(2))
                ADD(LW(0), LW(2))
                ADD(LW(0), 700)
                WAIT_MS(LW(0))
                CASE_EQUAL(-4)
                CALL(evt_sub_random, 300, LW(0))
                ADD(LW(0), 100)
                WAIT_MS(LW(0))
            END_SWITCH
        END_IF
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(light_rod_anim_rtn)
    SET_INT_READ_ADDR(LW(9))
    LOOP(0)
        READ_INT(LW(1))
        IF_INT_EQUAL(LW(1), 0)
            SET_INT_READ_ADDR(LW(9))
            READ_INT(LW(1))
        END_IF
        CALL(btlevtcmd_AnimeChangePose, -2, 7, LW(1))
        READ_INT(LW(1))
        IF_INT_GREATER(LW(1), 0)
            WAIT_MS(LW(1))
        ELSE
            SWITCH(LW(1))
                CASE_EQUAL(0)
                WAIT_FRAMES(1)
                CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 7)
                CASE_EQUAL(-1)
                BREAK_LOOP
                CASE_EQUAL(-2)
                LOOP(0)
                    CALL(evt_sub_random, 3000, LW(0))
                    ADD(LW(0), 3000)
                    WAIT_MS(LW(0))
                    CALL(evt_sub_random, 100, LW(0))
                    DIVIDE(LW(0), 30)
                    IF_INT_GREATER(LW(0), 0)
                        BREAK_LOOP
                    END_IF
                END_LOOP
                READ_INT4(LW(1), LW(2), LW(3), LW(4))
                LOOP(LW(0))
                    CALL(btlevtcmd_AnimeChangePose, -2, 7, LW(1))
                    WAIT_MS(LW(2))
                    CALL(btlevtcmd_AnimeChangePose, -2, 7, LW(3))
                    WAIT_MS(LW(4))
                END_LOOP
                CASE_EQUAL(-3)
                CALL(evt_sub_random, 3000, LW(0))
                CALL(evt_sub_random, 1000, LW(2))
                ADD(LW(0), LW(2))
                ADD(LW(0), 700)
                WAIT_MS(LW(0))
                CASE_EQUAL(-4)
                CALL(evt_sub_random, 300, LW(0))
                ADD(LW(0), 100)
                WAIT_MS(LW(0))
            END_SWITCH
        END_IF
    END_LOOP
    RETURN
EVT_END()






EVT_BEGIN(light_pose_change)
    CALL(enzan_lw_save2)
    CALL(btlevtcmd_AnimeGetPose, -2, 1, LW(0))
    IF_STR_EQUAL(LW(0), STRING("A_1B"))
        CALL(enzan_lw_save)
        SET_VALUE(LW(5), PTR(btl_shuryolight_pow_normal))
        SET_VALUE(LW(6), PTR(btl_shuryolight_head_1_tenmetu))
        SET_VALUE(LW(7), PTR(btl_shuryolight_head_2_tenmetu))
        SET_VALUE(LW(8), PTR(btl_shuryolight_head_3_tenmetu))
        SET_VALUE(LW(0), 2)
        GOTO(0)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("A_1C"))
        CALL(enzan_lw_save)
        SET_VALUE(LW(5), PTR(btl_shuryolight_pow_normal))
        SET_VALUE(LW(6), PTR(btl_shuryolight_head_1_tenmetu))
        SET_VALUE(LW(7), PTR(btl_shuryolight_head_2_tenmetu))
        SET_VALUE(LW(8), PTR(btl_shuryolight_head_3_tenmetu))
        SET_VALUE(LW(0), 2)
        GOTO(0)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("A_2"))
        CALL(enzan_lw_save)
        SET_VALUE(LW(5), PTR(btl_shuryolight_pow_normal))
        SET_VALUE(LW(6), PTR(btl_shuryolight_head_1_tenmetu))
        SET_VALUE(LW(7), PTR(btl_shuryolight_head_2_tenmetu))
        SET_VALUE(LW(8), PTR(btl_shuryolight_head_3_tenmetu))
        SET_VALUE(LW(0), 2)
        GOTO(0)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("H_1"))
        CALL(enzan_lw_save)
        SET_VALUE(LW(5), PTR(btl_shuryolight_pow_normal))
        SET_VALUE(LW(6), PTR(btl_shuryolight_head_1_quick))
        SET_VALUE(LW(7), PTR(btl_shuryolight_head_2_quick))
        SET_VALUE(LW(8), PTR(btl_shuryolight_head_3_quick))
        SET_VALUE(LW(0), 1)
        GOTO(0)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("D_1"))
        CALL(enzan_lw_save)
        SET_VALUE(LW(5), PTR(btl_shuryolight_pow_tenmetu2))
        SET_VALUE(LW(6), PTR(btl_shuryolight_head_1_tenmetu))
        SET_VALUE(LW(7), PTR(btl_shuryolight_head_2_tenmetu))
        SET_VALUE(LW(8), PTR(btl_shuryolight_head_3_tenmetu))
        SET_VALUE(LW(0), 3)
        GOTO(0)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("N_1"))
        CALL(enzan_lw_save)
        SET_VALUE(LW(5), PTR(btl_shuryolight_pow_normal))
        SET_VALUE(LW(6), PTR(btl_shuryolight_head_1_off))
        SET_VALUE(LW(7), PTR(btl_shuryolight_head_2_off))
        SET_VALUE(LW(8), PTR(btl_shuryolight_head_3_off))
        SET_VALUE(LW(0), 6)
        GOTO(0)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("K_1"))
        CALL(enzan_lw_save)
        SET_VALUE(LW(5), PTR(btl_shuryolight_pow_normal))
        SET_VALUE(LW(6), PTR(btl_shuryolight_head_1_random))
        SET_VALUE(LW(7), PTR(btl_shuryolight_head_2_random))
        SET_VALUE(LW(8), PTR(btl_shuryolight_head_3_random))
        SET_VALUE(LW(0), 4)
        GOTO(0)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("Q_1"))
        CALL(enzan_lw_save)
        SET_VALUE(LW(5), PTR(btl_shuryolight_pow_tenmetu2))
        SET_VALUE(LW(6), PTR(btl_shuryolight_head_1_tenmetu))
        SET_VALUE(LW(7), PTR(btl_shuryolight_head_2_tenmetu))
        SET_VALUE(LW(8), PTR(btl_shuryolight_head_3_tenmetu))
        SET_VALUE(LW(0), 3)
        GOTO(0)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("X_1"))
        CALL(enzan_lw_save)
        SET_VALUE(LW(5), PTR(btl_shuryolight_pow_tenmetu2))
        SET_VALUE(LW(6), PTR(btl_shuryolight_head_1_tenmetu))
        SET_VALUE(LW(7), PTR(btl_shuryolight_head_2_tenmetu))
        SET_VALUE(LW(8), PTR(btl_shuryolight_head_3_tenmetu))
        SET_VALUE(LW(0), 3)
        GOTO(0)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("Y_1"))
        CALL(enzan_lw_save)
        SET_VALUE(LW(5), PTR(btl_shuryolight_pow_normal))
        SET_VALUE(LW(6), PTR(btl_shuryolight_head_1_on))
        SET_VALUE(LW(7), PTR(btl_shuryolight_head_2_on))
        SET_VALUE(LW(8), PTR(btl_shuryolight_head_3_on))
        SET_VALUE(LW(0), 5)
        GOTO(0)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("D_2"))
        CALL(enzan_lw_save)
        SET_VALUE(LW(5), PTR(btl_shuryolight_pow_normal))
        SET_VALUE(LW(6), PTR(btl_shuryolight_head_1_tenmetu))
        SET_VALUE(LW(7), PTR(btl_shuryolight_head_2_tenmetu))
        SET_VALUE(LW(8), PTR(btl_shuryolight_head_3_tenmetu))
        SET_VALUE(LW(0), 2)
        GOTO(0)
    END_IF
    CALL(enzan_lw_save)
    SET_VALUE(LW(5), PTR(btl_shuryolight_pow_normal))
    SET_VALUE(LW(6), PTR(btl_shuryolight_head_1_normal))
    SET_VALUE(LW(7), PTR(btl_shuryolight_head_2_normal))
    SET_VALUE(LW(8), PTR(btl_shuryolight_head_3_normal))
    SET_VALUE(LW(0), 0)
    LABEL(0)
    CALL(btlevtcmd_GetUnitWork, -2, 6, LW(1))
    IF_INT_NOT_EQUAL(LW(0), LW(1))
        CALL(btlevtcmd_SetUnitWork, -2, 6, LW(0))
        CALL(btlevtcmd_GetUnitWork, -2, 4, LW(0))
        DELETE_EVENT(LW(0))
        SPAWN_EVENT_GET_ID(PTR(light_anim_rtn), LW(0))
        CALL(btlevtcmd_SetUnitWork, -2, 4, LW(0))
    END_IF
    CALL(enzan_lw_load)
    CALL(enzan_lw_load2)
    RETURN
EVT_END()


EVT_BEGIN(maiframeanimcheck)
    LOOP(0)
        CALL(btlevtcmd_AnimeGetPose, -2, 1, LW(1))
        IF_STR_NOT_EQUAL(LW(0), LW(1))
            CALL(datastrcopy, LW(1))
            CALL_CHILD(light_pose_change)
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()




static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetEventEntry, -2, PTR(entry_event))
    CALL(btl_hitoffsetx, -2, 2, -3)
    CALL(btlevtcmd_SetHpGaugeOffset, -2, -2, -6)
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("L_1B"))
    CALL(btlevtcmd_AnimeChangePose, -2, 4, STRING("L_4B"))
    CALL(btlevtcmd_AnimeChangePose, -2, 5, STRING("L_3B"))
    CALL(btlevtcmd_AnimeChangePose, -2, 6, STRING("L_2B"))
    CALL(btlevtcmd_AnimeChangePose, -2, 7, STRING("L_3B"))
    CALL(_set_draw_callback)
    CALL(enzan_lw_save)
    SET_VALUE(LW(5), PTR(btl_shuryolight_pow_normal))
    SET_VALUE(LW(6), PTR(btl_shuryolight_head_1_normal))
    SET_VALUE(LW(7), PTR(btl_shuryolight_head_2_normal))
    SET_VALUE(LW(8), PTR(btl_shuryolight_head_3_normal))
    SET_VALUE(LW(0), 0)
    CALL(btlevtcmd_SetUnitWork, -2, 6, LW(0))
    SPAWN_EVENT_GET_ID(PTR(light_anim_rtn), LW(0))
    CALL(btlevtcmd_SetUnitWork, -2, 4, LW(0))
    CALL(enzan_lw_load)
    CALL(enzan_lw_save)
    SET_VALUE(LW(9), PTR(btl_shuryolight_rod_normal))
    SET_VALUE(LW(0), 8)
    CALL(btlevtcmd_SetUnitWork, -2, 7, LW(0))
    SPAWN_EVENT_GET_ID(PTR(light_rod_anim_rtn), LW(0))
    CALL(btlevtcmd_SetUnitWork, -2, 5, LW(0))
    CALL(enzan_lw_load)
    CALL(pose_check_evt_run)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 3, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 2, -1)
    SET_VALUE(LW(0), 9)
    LOOP(4)
        CALL(btlevtcmd_SetUnitWork, -2, LW(0), -1)
        ADD(LW(0), 1)
    END_LOOP
    RETURN
EVT_END()



static EVT_BEGIN(entry_event)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    SET_VALUE(LW(0), 2)
    CALL_CHILD(batten_leader_zoom)
    WAIT_MS(1000)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("E_1"))
    WAIT_MS(1000)
    CALL(evt_msg_print, 2, STRING("stg8_las_65"), 0, -2)
    WAIT_MS(300)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_STUFF_PUTOUT1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1A"))
    WAIT_MS(1000)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_STUFF_PUTOUT2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(eff_staff, -2)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1B"))
    WAIT_MS(2000)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_STUFF_PISECE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_2"))
    WAIT_MS(270)
    SET_VALUE(LW(0), 0)
    SET_VALUE(LW(6), 4)
    CALL_CHILD(sub_call_satellite)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 69)
    CALL(check_satellite)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    CALL(btlevtcmd_GetUnitWork, -2, 3, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SWITCH(LW(0))
            CASE_EQUAL(4)
            WAIT_MS(1500)
            CALL(evt_btl_camera_set_mode, 0, 0)
            WAIT_MS(300)
            CASE_EQUAL(5)
            WAIT_MS(1500)
            CALL(evt_btl_camera_set_mode, 0, 0)
            WAIT_MS(300)
        END_SWITCH
        CALL(btlevtcmd_SetUnitWork, -2, 3, 0)
    END_IF
    RETURN
EVT_END()


EVT_BEGIN(damage_sub_event)
    CALL(btlevtcmd_CheckDamageCode, LW(10), 256, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(unk_las_0000d538, LW(10))
    END_IF
    CALL(btlevtcmd_GetDamage, LW(10), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetHitPos, LW(10), LW(11), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_BTL_ATTACK_MISS2"), LW(0), LW(1), LW(2), 0)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_GetDamagePartsId, LW(10), LW(11))
    IF_INT_EQUAL(LW(11), 1)
        CALL(btlevtcmd_AnimeChangePoseType, LW(10), 1, 39)
    ELSE
        CALL(enzan_lw_save)
        SET_VALUE(LW(9), PTR(btl_shuryolight_rod_off))
        SET_VALUE(LW(0), 9)
        CALL(btlevtcmd_GetUnitWork, -2, 7, LW(1))
        IF_INT_NOT_EQUAL(LW(0), LW(1))
            CALL(btlevtcmd_SetUnitWork, -2, 7, LW(0))
            CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
            DELETE_EVENT(LW(0))
            SPAWN_EVENT_GET_ID(PTR(light_rod_anim_rtn), LW(0))
            CALL(btlevtcmd_SetUnitWork, -2, 5, LW(0))
        END_IF
        CALL(enzan_lw_load)
        CALL(btlevtcmd_AnimeChangePose, LW(10), 1, STRING("D_2"))
    END_IF
    LABEL(90)
    CALL(btlevtcmd_CheckDamageCode, LW(10), 256, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        IF_INT_NOT_EQUAL(LW(11), 1)
            CALL(btlevtcmd_GetTotalDamage, LW(10), LW(0))
            CALL(btlevtcmd_GetUnitWork, -2, 2, LW(1))
            IF_INT_LESS(LW(1), 0)
                SET_VALUE(LW(1), 100)
            ELSE
                MULTIPLY(LW(0), 10)
                ADD(LW(1), LW(0))
                IF_INT_GREATER(LW(1), 100)
                    SET_VALUE(LW(1), 100)
                END_IF
            END_IF
            CALL(btlevtcmd_SetUnitWork, -2, 2, LW(1))
        END_IF
    END_IF
    WAIT_MS(500)
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    CALL(btlevtcmd_CheckPhase, LW(0), 67108865)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(player_before_phase_event)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CheckPhase, LW(0), 67108867)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(monster_before_phase_event)
        GOTO(99)
    END_IF
    LABEL(99)
    RETURN
EVT_END()

EVT_BEGIN(monster_before_phase_event)
    CALL(btlevtcmd_CheckActStatus, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(0)
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL(btlevtcmd_GetMaxHp, -2, LW(0))
        MULTIPLY(LW(0), 75)
        DIVIDE(LW(0), 100)
        CALL(btlevtcmd_GetHp, -2, LW(1))
        IF_INT_LESS_OR_EQUAL(LW(1), LW(0))
            CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
            CALL(btlevtcmd_StopWaitEvent, -2)
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            SET_VALUE(LW(0), 1)
            CALL_CHILD(batten_leader_zoom)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("E_1"))
            WAIT_MS(1000)
            CALL(evt_msg_print, 2, STRING("stg8_las_66"), 0, -2)
            WAIT_MS(300)
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            CALL(evt_btl_camera_set_mode, 0, 0)
        END_IF
        CASE_EQUAL(1)
        CALL(btlevtcmd_GetMaxHp, -2, LW(0))
        MULTIPLY(LW(0), 50)
        DIVIDE(LW(0), 100)
        CALL(btlevtcmd_GetHp, -2, LW(1))
        IF_INT_LESS_OR_EQUAL(LW(1), LW(0))
            CALL(btlevtcmd_SetUnitWork, -2, 1, 2)
            CALL(btlevtcmd_StopWaitEvent, -2)
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            SET_VALUE(LW(0), 1)
            CALL_CHILD(batten_leader_zoom)
            CALL(evt_msg_print, 2, STRING("stg8_las_67"), 0, -2)
            WAIT_MS(300)
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            CALL(evt_btl_camera_set_mode, 0, 0)
        END_IF
        CASE_EQUAL(2)
        CALL(btlevtcmd_GetMaxHp, -2, LW(0))
        MULTIPLY(LW(0), 25)
        DIVIDE(LW(0), 100)
        CALL(btlevtcmd_GetHp, -2, LW(1))
        IF_INT_LESS_OR_EQUAL(LW(1), LW(0))
            CALL(btlevtcmd_SetUnitWork, -2, 1, 3)
            CALL(btlevtcmd_StopWaitEvent, -2)
            CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_furafura))
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            SET_VALUE(LW(0), 1)
            CALL_CHILD(batten_leader_zoom)
            CALL(btlevtcmd_SetTalkPoseType, -2, 65)
            CALL(btlevtcmd_SetStayPoseType, -2, 43)
            CALL(evt_msg_print, 2, STRING("stg8_las_68"), 0, -2)
            WAIT_MS(300)
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            CALL(evt_btl_camera_set_mode, 0, 0)
        END_IF
    END_SWITCH
    LABEL(0)
    RETURN
EVT_END()


static EVT_BEGIN(dead_event)
    CALL(btlevtcmd_WaitAttackEnd)
    SET_VALUE(LW(0), 9)
    LOOP(4)
        CALL(btlevtcmd_GetUnitWork, -2, LW(0), LW(1))
        CALL(btlevtcmd_GetUnitId, LW(1), LW(1))
        IF_INT_NOT_EQUAL(LW(1), -1)
            CALL(btlevtcmd_CheckDamageCode, LW(1), 512, LW(2))
            IF_INT_EQUAL(LW(2), 0)
                SET_VALUE(LW(10), LW(1))
                BEGIN_INLINE_EVENT
                    CALL_CHILD(subsetevt_dead_core)
                    CALL(btlevtcmd_GetBodyId, LW(10), LW(11))
                    CALL(btlevtcmd_OnPartsAttribute, LW(10), LW(11), 16777216)
                    CALL(btlevtcmd_OnPartsAttribute, LW(10), LW(11), 33554432)
                END_INLINE_EVENT
                WAIT_MS(200)
            END_IF
        END_IF
        ADD(LW(0), 1)
    END_LOOP
    WAIT_MS(1000)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(pose_check_evt_stop)
    CALL(enzan_lw_save)
    SET_VALUE(LW(5), PTR(btl_shuryolight_pow_tenmetu))
    SET_VALUE(LW(6), PTR(btl_shuryolight_head_1_off))
    SET_VALUE(LW(7), PTR(btl_shuryolight_head_2_off))
    SET_VALUE(LW(8), PTR(btl_shuryolight_head_3_off))
    SET_VALUE(LW(0), 7)
    CALL(btlevtcmd_GetUnitWork, -2, 6, LW(1))
    IF_INT_NOT_EQUAL(LW(0), LW(1))
        CALL(btlevtcmd_SetUnitWork, -2, 6, LW(0))
        CALL(btlevtcmd_GetUnitWork, -2, 4, LW(0))
        DELETE_EVENT(LW(0))
        SPAWN_EVENT_GET_ID(PTR(light_anim_rtn), LW(0))
        CALL(btlevtcmd_SetUnitWork, -2, 4, LW(0))
    END_IF
    CALL(enzan_lw_load)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("D_3"))
    WAIT_MS(750)
    CALL(btlevtcmd_SetTalkPose, -2, STRING("T_5"))
    CALL(btlevtcmd_SetStayPose, -2, STRING("D_4"))
    CALL(evt_msg_print, 2, STRING("stg8_las_69"), 0, -2)
    WAIT_MS(300)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("D_4"))
    WAIT_MS(500)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(btlevtcmd_GetUnitWork, -2, 4, LW(0))
    DELETE_EVENT(LW(0))
    CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
    DELETE_EVENT(LW(0))
    SET_VALUE(LW(10), -2)
    CALL_CHILD(subsetevt_dead_core_nospin_norotate)
    RETURN
EVT_END()

// TODO: type
static u32 splineTableIn1_1[15] = {
    0x42180000, 0x42340000, 0x40A00000, 0x41F33333,
    0xBF1999C0, 0xC0A00000, 0x41980000, 0xC17CCCCC,
    0xC0A00000, 0x40F33333, 0xBF1999C0, 0xC0A00000,
    0x00000000, 0x42340000, 0xC0A00000,
};

// TODO: type
static u32 splineTableIn2_1[15] = {
    0x42180000, 0x42340000, 0x40A00000, 0x41F33333,
    0xBF1999C0, 0xC0A00000, 0x41980000, 0xC17CCCCC,
    0xC0A00000, 0x40F33333, 0xBF1999C0, 0xC0A00000,
    0x00000000, 0x42340000, 0xC0A00000,
};

// TODO: type
static u32 splineTableIn2_2[15] = {
    0xC2180000, 0x42340000, 0x40A00000, 0xC1F33333,
    0x42B53334, 0xC0A00000, 0xC1980000, 0x42B53334,
    0xC0A00000, 0xC0F33333, 0x42B53334, 0xC0A00000,
    0x00000000, 0x42340000, 0xC0A00000,
};

// TODO: type
static u32 splineTableIn3_1[15] = {
    0x00000000, 0x42A60000, 0x40A00000, 0x00000000,
    0x42E2CCCD, 0xC0A00000, 0x00000000, 0x42F20000,
    0xC0A00000, 0x00000000, 0x42A60000, 0xC0A00000,
    0x00000000, 0x42340000, 0xC0A00000,
};

// TODO: type
static u32 splineTableIn3_2[15] = {
    0x42180000, 0x40E00000, 0x40A00000, 0x42733333,
    0xC17CCCCC, 0xC0A00000, 0x42366667, 0xC1033334,
    0xC0A00000, 0x41F33333, 0x40E00000, 0xC0A00000,
    0x00000000, 0x42340000, 0xC0A00000,
};

// TODO: type
static u32 splineTableIn3_3[15] = {
    0xC2180000, 0x40E00000, 0x40A00000, 0xC2733333,
    0xC17CCCCC, 0xC0A00000, 0xC2366667, 0xC1033334,
    0xC0A00000, 0xC1F33333, 0x40E00000, 0xC0A00000,
    0x00000000, 0x42340000, 0xC0A00000,
};

// TODO: type
static u32 splineTableIn4_1[15] = {
    0x42180000, 0x42A60000, 0x40A00000, 0x42366667,
    0x42C46666, 0xC0A00000, 0x42733333, 0x42E2CCCD,
    0xC0A00000, 0x41F33333, 0x42A60000, 0xC0A00000,
    0x00000000, 0x42340000, 0xC0A00000,
};

// TODO: type
static u32 splineTableIn4_2[15] = {
    0xC2180000, 0x42A60000, 0x40A00000, 0xC2366667,
    0x42C46666, 0xC0A00000, 0xC2733333, 0x42E2CCCD,
    0xC0A00000, 0xC1F33333, 0x42A60000, 0xC0A00000,
    0x00000000, 0x42340000, 0xC0A00000,
};

// TODO: type
static u32 splineTableIn4_3[15] = {
    0x42180000, 0x40E00000, 0x40A00000, 0x42366667,
    0xC1033334, 0xC0A00000, 0x42733333, 0xC1BB3334,
    0xC0A00000, 0x41F33333, 0x40E00000, 0xC0A00000,
    0x00000000, 0x42340000, 0xC0A00000,
};

// TODO: type
static u32 splineTableIn4_4[15] = {
    0xC2180000, 0x40E00000, 0x40A00000, 0xC2366667,
    0xC1033334, 0xC0A00000, 0xC2733333, 0xC1BB3334,
    0xC0A00000, 0xC1F33333, 0x40E00000, 0xC0A00000,
    0x00000000, 0x42340000, 0xC0A00000,
};

// TODO: type
static u32 splineTableOut1_1[15] = {
    0x00000000, 0x42340000, 0xC0A00000, 0xC254CCCD,
    0xC1033334, 0xC0A00000, 0xC2733333, 0x42340000,
    0xC0A00000, 0x42366667, 0x42D3999A, 0xC0A00000,
    0x42180000, 0x42340000, 0x40A00000,
};

// TODO: type
static u32 splineTableOut2_1[15] = {
    0x00000000, 0x42340000, 0xC0A00000, 0x40F33333,
    0x42B53334, 0xC0A00000, 0x41980000, 0x42D3999A,
    0xC0A00000, 0x41F33333, 0x42B53334, 0xC0A00000,
    0x42180000, 0x42340000, 0x40A00000,
};

// TODO: type
static u32 splineTableOut2_2[15] = {
    0x00000000, 0x42340000, 0xC0A00000, 0xC0F33333,
    0xBF1999C0, 0xC0A00000, 0xC1980000, 0xC17CCCCC,
    0xC0A00000, 0xC1F33333, 0xBF1999C0, 0xC0A00000,
    0xC2180000, 0x42340000, 0x40A00000,
};

// TODO: type
static u32 splineTableOut3_1[15] = {
    0x00000000, 0x42340000, 0xC0A00000, 0x00000000,
    0x42A60000, 0xC0A00000, 0x00000000, 0x42F20000,
    0xC0A00000, 0x00000000, 0x42E2CCCD, 0xC0A00000,
    0x00000000, 0x42A60000, 0x40A00000,
};

// TODO: type
static u32 splineTableOut3_2[15] = {
    0x00000000, 0x42340000, 0xC0A00000, 0x42180000,
    0x4169999A, 0xC0A00000, 0x4288CCCD, 0xBF1999C0,
    0xC0A00000, 0x42366667, 0x40E00000, 0xC0A00000,
    0x42180000, 0x40E00000, 0x40A00000,
};

// TODO: type
static u32 splineTableOut3_3[15] = {
    0x00000000, 0x42340000, 0xC0A00000, 0xC2180000,
    0x4169999A, 0xC0A00000, 0xC288CCCD, 0xBF1999C0,
    0xC0A00000, 0xC2366667, 0x40E00000, 0xC0A00000,
    0xC2180000, 0x40E00000, 0x40A00000,
};

// TODO: type
static u32 splineTableOut4_1[15] = {
    0x00000000, 0x42340000, 0xC0A00000, 0x42180000,
    0x4296CCCD, 0xC0A00000, 0x4288CCCD, 0x42B53334,
    0xC0A00000, 0x42366667, 0x42A60000, 0xC0A00000,
    0x42180000, 0x42A60000, 0x40A00000,
};

// TODO: type
static u32 splineTableOut4_2[15] = {
    0x00000000, 0x42340000, 0xC0A00000, 0xC2180000,
    0x4296CCCD, 0xC0A00000, 0xC288CCCD, 0x42B53334,
    0xC0A00000, 0xC2366667, 0x42A60000, 0xC0A00000,
    0xC2180000, 0x42A60000, 0x40A00000,
};

// TODO: type
static u32 splineTableOut4_3[15] = {
    0x00000000, 0x42340000, 0xC0A00000, 0x42180000,
    0x4169999A, 0xC0A00000, 0x4288CCCD, 0xBF1999C0,
    0xC0A00000, 0x42366667, 0x40E00000, 0xC0A00000,
    0x42180000, 0x40E00000, 0x40A00000,
};

// TODO: type
static u32 splineTableOut4_4[15] = {
    0x00000000, 0x42340000, 0xC0A00000, 0xC2180000,
    0x4169999A, 0xC0A00000, 0xC288CCCD, 0xBF1999C0,
    0xC0A00000, 0xC2366667, 0x40E00000, 0xC0A00000,
    0xC2180000, 0x40E00000, 0x40A00000,
};

static EVT_BEGIN(satellite_blur_on)
    SET_VALUE(LW(2), 9)
    LOOP(4)
        CALL(btlevtcmd_GetUnitWork, -2, LW(2), LW(3))
        IF_INT_NOT_EQUAL(LW(3), -1)
            CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
            CALL(btlevtcmd_SetPartsBlur, LW(3), LW(4), 255, 255, 255, 255, 255, 255, 255, 100, 0)
            CALL(btlevtcmd_OnPartsAttribute, LW(3), LW(4), 67108864)
        END_IF
        ADD(LW(2), 1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(satellite_blur_off)
    SET_VALUE(LW(2), 9)
    LOOP(4)
        CALL(btlevtcmd_GetUnitWork, -2, LW(2), LW(3))
        IF_INT_NOT_EQUAL(LW(3), -1)
            CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
            CALL(btlevtcmd_OffPartsAttribute, LW(3), LW(4), 67108864)
        END_IF
        ADD(LW(2), 1)
    END_LOOP
    RETURN
EVT_END()


EVT_BEGIN(sub_call_satellite)
    CALL(btlevtcmd_GetUnitId, -2, LW(4))
    SET_VALUE(LW(1), 9)
    SET_VALUE(LW(5), 0)
    LOOP(4)
        CALL(btlevtcmd_GetUnitWork, -2, LW(1), LW(2))
        IF_INT_EQUAL(LW(2), -1)
            CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_batten_satellite), 0)
            CALL(btlevtcmd_AddPos, LW(3), 0, 45, -5)
            CALL(btlevtcmd_SetUnitWork, -2, LW(1), LW(3))
            CALL(btlevtcmd_SetUnitWork, LW(3), 0, LW(4))
            ADD(LW(0), 1)
            ADD(LW(5), 1)
            IF_INT_GREATER_OR_EQUAL(LW(5), LW(6))
                BREAK_LOOP
            END_IF
        END_IF
        ADD(LW(1), 1)
    END_LOOP
    WAIT_FRAMES(5)
    CALL_CHILD(satellite_blur_on)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_SPLIT2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_IF
    CALL(battenSatelliteToOutside, -2, LW(0))
    WAIT_MS(1000)
    CALL_CHILD(satellite_blur_off)
    CALL(btlevtcmd_SetUnitWork, -2, 0, LW(0))
    RETURN
EVT_END()


EVT_BEGIN(player_before_phase_event)
    CALL(btlevtcmd_StopWaitEvent, -2)
    CALL(btlevtcmd_get_turn, LW(0))
    IF_INT_LESS_OR_EQUAL(LW(0), 1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CheckActStatus, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(99)
    END_IF
    CALL(evt_sub_random, 10, LW(0))
    WAIT_FRAMES(LW(0))
    SET_VALUE(LW(0), 0)
    SET_VALUE(LW(1), 9)
    LOOP(4)
        CALL(btlevtcmd_GetUnitWork, -2, LW(1), LW(3))
        IF_INT_NOT_EQUAL(LW(3), -1)
            ADD(LW(0), 1)
        END_IF
        ADD(LW(1), 1)
    END_LOOP
    IF_INT_GREATER_OR_EQUAL(LW(0), 4)
        GOTO(99)
    END_IF
    CALL_CHILD(satellite_blur_on)
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_SPLIT1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(1))
    CALL(battenSatelliteToInside, -2, LW(1))
    CALL_CHILD(satellite_blur_off)
    SET_VALUE(LW(1), 9)
    LOOP(3)
        CALL(btlevtcmd_GetUnitWork, -2, LW(1), LW(2))
        IF_INT_EQUAL(LW(2), -1)
            SET_VALUE(LW(4), LW(1))
            SET_VALUE(LW(3), 999999999)
            LOOP(0)
                IF_INT_EQUAL(LW(3), 999999999)
                    SET_VALUE(LW(3), LW(1))
                    ADD(LW(3), 1)
                ELSE
                    ADD(LW(3), 1)
                    ADD(LW(3), 0)
                END_IF
                SUBTRACT(LW(3), 0)
                IF_INT_GREATER(LW(3), 12)
                    ADD(LW(3), 0)
                    BREAK_LOOP
                END_IF
                ADD(LW(3), 0)
                CALL(btlevtcmd_GetUnitWork, -2, LW(3), LW(2))
                CALL(btlevtcmd_SetUnitWork, -2, LW(4), LW(2))
                SET_VALUE(LW(4), LW(3))
            END_LOOP
            CALL(btlevtcmd_SetUnitWork, -2, LW(4), -1)
        END_IF
        ADD(LW(1), 1)
    END_LOOP
    SET_VALUE(LW(6), 2)
    CALL_CHILD(sub_call_satellite)
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_get_turn, LW(0))
    IF_INT_LESS_OR_EQUAL(LW(0), 1)
        SET_VALUE(LW(9), PTR(&weapon_batten_leader_thunder))
        CALL_CHILD(batten_leader_thunder_attack_event)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(2))
    IF_INT_LESS(LW(2), 2)
        CALL(btlevtcmd_DrawLots, LW(0), 6, 60, 40, 30, 20, 20, 20)
        SWITCH(LW(0))
            CASE_EQUAL(0)
            SET_VALUE(LW(9), PTR(&weapon_batten_leader_thunder))
            CALL_CHILD(batten_leader_thunder_attack_event)
            CASE_EQUAL(1)
            SET_VALUE(LW(9), PTR(&weapon_batten_leader_fire))
            CALL_CHILD(batten_leader_fire_attack_event)
            CASE_EQUAL(2)
            SET_VALUE(LW(9), PTR(&weapon_batten_leader_ice))
            CALL_CHILD(batten_leader_ice_attack_event)
            CASE_EQUAL(3)
            SET_VALUE(LW(9), PTR(&weapon_batten_leader_shikaeshi))
            CALL_CHILD(batten_leader_shikaeshi_attack_event)
            CASE_EQUAL(4)
            SET_VALUE(LW(9), PTR(&weapon_batten_leader_yokeyoke))
            CALL_CHILD(batten_leader_yokeyoke_attack_event)
            CASE_EQUAL(5)
            SET_VALUE(LW(9), PTR(&weapon_batten_leader_stop))
            CALL_CHILD(batten_leader_stop_attack_event)
        END_SWITCH
    ELSE
        CALL(btlevtcmd_DrawLots, LW(0), 6, 60, 40, 30, 20, 20, 20)
        SWITCH(LW(0))
            CASE_EQUAL(0)
            SET_VALUE(LW(9), PTR(&weapon_batten_leader_thunder))
            CALL_CHILD(batten_leader_thunder_attack_event)
            CASE_EQUAL(1)
            SET_VALUE(LW(9), PTR(&weapon_batten_leader_fire))
            CALL_CHILD(batten_leader_fire_attack_event)
            CASE_EQUAL(2)
            SET_VALUE(LW(9), PTR(&weapon_batten_leader_ice))
            CALL_CHILD(batten_leader_ice_attack_event)
            CASE_EQUAL(3)
            SET_VALUE(LW(9), PTR(&weapon_batten_leader_shikaeshi))
            CALL_CHILD(batten_leader_shikaeshi_attack_event)
            CASE_EQUAL(4)
            SET_VALUE(LW(9), PTR(&weapon_batten_leader_yokeyoke))
            CALL_CHILD(batten_leader_yokeyoke_attack_event)
            CASE_EQUAL(5)
            SET_VALUE(LW(9), PTR(&weapon_batten_leader_stop))
            CALL_CHILD(batten_leader_stop_attack_event)
        END_SWITCH
    END_IF
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()


EVT_BEGIN(batten_leader_thunder_attack_event)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_STUFF_PUTOUT1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1A"))
    WAIT_MS(170)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_STUFF_PUTOUT2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(eff_staff, -2)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1B"))
    WAIT_MS(2000)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_STUFF_PISECE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_2"))
    WAIT_MS(270)
    CALL(enzan_lw_save)
    SET_VALUE(LW(9), PTR(btl_shuryolight_rod_normal))
    SET_VALUE(LW(0), 8)
    CALL(btlevtcmd_GetUnitWork, -2, 7, LW(1))
    IF_INT_NOT_EQUAL(LW(0), LW(1))
        CALL(btlevtcmd_SetUnitWork, -2, 7, LW(0))
        CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
        DELETE_EVENT(LW(0))
        SPAWN_EVENT_GET_ID(PTR(light_rod_anim_rtn), LW(0))
        CALL(btlevtcmd_SetUnitWork, -2, 5, LW(0))
    END_IF
    CALL(enzan_lw_load)
    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(1))
    IF_INT_GREATER(LW(1), 0)
        CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
        CALL(btlevtcmd_GetRandomValue, 100, LW(2))
        IF_INT_LESS(LW(2), LW(1))
            CALL(btlevtcmd_GetPosFloat, -2, LW(0), LW(1), LW(2))
            ADD_FLOAT(LW(0), FLOAT(-52.0f))
            ADD_FLOAT(LW(1), FLOAT(60.0f))
            ADD_FLOAT(LW(2), FLOAT(20.0f))
            CALL(evt_eff, STRING("KEMURI_EFF_00"), STRING("kemuri_test"), 19, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
            WAIT_MS(600)
            RETURN
        END_IF
    END_IF
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
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
    WAIT_MS(500)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_THUNDER1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_eff_pokopi_thunder, -80, 0, 25)
    CALL(evt_eff_pokopi_thunder, -160, 0, -25)
    WAIT_MS(100)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_THUNDER1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_eff_pokopi_thunder, -100, 0, 35)
    CALL(evt_eff_pokopi_thunder, -140, 0, -35)
    WAIT_MS(100)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_THUNDER1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_eff_pokopi_thunder, -120, 0, 25)
    CALL(evt_eff_pokopi_thunder, -120, 0, -25)
    WAIT_MS(100)
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_THUNDER1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(evt_eff_pokopi_thunder, -140, 0, 35)
        CALL(evt_eff_pokopi_thunder, -100, 0, -35)
        WAIT_MS(100)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_THUNDER1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(evt_eff_pokopi_thunder, -160, 0, 25)
        CALL(evt_eff_pokopi_thunder, -80, 0, -25)
    END_INLINE_EVENT
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    SET_VALUE(LW(10), 0)
    LOOP(0)
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
        IF_INT_EQUAL(LW(10), 0)
            SET_VALUE(LW(10), 1)
            CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
        END_IF
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
        LABEL(98)
        CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
        IF_INT_EQUAL(LW(3), -1)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

EVT_BEGIN(batten_leader_fire_attack_event)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_STUFF_PUTOUT1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1A"))
    WAIT_MS(170)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_STUFF_PUTOUT2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(eff_staff, -2)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1B"))
    WAIT_MS(2000)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_STUFF_PISECE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_2"))
    WAIT_MS(270)
    CALL(enzan_lw_save)
    SET_VALUE(LW(9), PTR(btl_shuryolight_rod_normal))
    SET_VALUE(LW(0), 8)
    CALL(btlevtcmd_GetUnitWork, -2, 7, LW(1))
    IF_INT_NOT_EQUAL(LW(0), LW(1))
        CALL(btlevtcmd_SetUnitWork, -2, 7, LW(0))
        CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
        DELETE_EVENT(LW(0))
        SPAWN_EVENT_GET_ID(PTR(light_rod_anim_rtn), LW(0))
        CALL(btlevtcmd_SetUnitWork, -2, 5, LW(0))
    END_IF
    CALL(enzan_lw_load)
    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(1))
    IF_INT_GREATER(LW(1), 0)
        CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
        CALL(btlevtcmd_GetRandomValue, 100, LW(2))
        IF_INT_LESS(LW(2), LW(1))
            CALL(btlevtcmd_GetPosFloat, -2, LW(0), LW(1), LW(2))
            ADD_FLOAT(LW(0), FLOAT(-52.0f))
            ADD_FLOAT(LW(1), FLOAT(60.0f))
            ADD_FLOAT(LW(2), FLOAT(20.0f))
            CALL(evt_eff, STRING("KEMURI_EFF_00"), STRING("kemuri_test"), 19, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
            WAIT_MS(600)
            RETURN
        END_IF
    END_IF
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_FIRE1"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    CALL(btlevtcmd_GetPosFloat, -2, LW(0), LW(1), LW(2))
    SET_FLOAT_VALUE(LW(5), LW(0))
    ADD_FLOAT(LW(0), -40)
    ADD_FLOAT(LW(5), -400)
    CALL(evt_eff, STRING(""), STRING("hibashira"), 4, LW(0), LW(1), LW(2), LW(5), LW(1), LW(2), 20, FLOAT(1.0f), 60, 0, 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_snd_sfxoff, LW(15))
    END_INLINE_EVENT
    CALL(bts_x_flag_set, 0)
    SET_VALUE(LW(7), 1)
    BEGIN_SIBLING_EVENT
        CALL(bts_x_flag_add, 1)
    END_SIBLING_EVENT
    WAIT_MS(600)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    SET_VALUE(LW(10), 0)
    LOOP(0)
        CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
        SWITCH(LW(5))
            CASE_OR_EQUAL(4)
            SET_VALUE(LW(6), 90)
            CASE_END
            CASE_EQUAL(3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
            SET_VALUE(LW(6), 90)
            CASE_END
            CASE_EQUAL(6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
            SET_VALUE(LW(6), 90)
            CASE_EQUAL(2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
            SET_VALUE(LW(6), 90)
            CASE_EQUAL(1)
            SET_VALUE(LW(6), 91)
            CASE_END
            CASE_DEFAULT
            SET_VALUE(LW(6), 97)
            CASE_END
        END_SWITCH
        GOTO(LW(6))
        LABEL(90)
        GOTO(97)
        LABEL(91)
        IF_INT_EQUAL(LW(10), 0)
            SET_VALUE(LW(10), 1)
            CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
            ADD(LW(7), 1)
            BEGIN_SIBLING_EVENT
                CALL(evt_cam_shake, 4, FLOAT(0.006f), FLOAT(0.0f), 500)
                CALL(bts_x_flag_add, 1)
            END_SIBLING_EVENT
        END_IF
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
        LABEL(97)
        WAIT_MS(150)
        CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
        IF_INT_EQUAL(LW(3), -1)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(bts_x_flag_wait, LW(7))
    RETURN
EVT_END()

EVT_BEGIN(batten_leader_ice_attack_event)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_STUFF_PUTOUT1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1A"))
    WAIT_MS(170)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_STUFF_PUTOUT2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(eff_staff, -2)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1B"))
    WAIT_MS(2000)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_STUFF_PISECE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_2"))
    WAIT_MS(270)
    CALL(enzan_lw_save)
    SET_VALUE(LW(9), PTR(btl_shuryolight_rod_normal))
    SET_VALUE(LW(0), 8)
    CALL(btlevtcmd_GetUnitWork, -2, 7, LW(1))
    IF_INT_NOT_EQUAL(LW(0), LW(1))
        CALL(btlevtcmd_SetUnitWork, -2, 7, LW(0))
        CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
        DELETE_EVENT(LW(0))
        SPAWN_EVENT_GET_ID(PTR(light_rod_anim_rtn), LW(0))
        CALL(btlevtcmd_SetUnitWork, -2, 5, LW(0))
    END_IF
    CALL(enzan_lw_load)
    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(1))
    IF_INT_GREATER(LW(1), 0)
        CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
        CALL(btlevtcmd_GetRandomValue, 100, LW(2))
        IF_INT_LESS(LW(2), LW(1))
            CALL(btlevtcmd_GetPosFloat, -2, LW(0), LW(1), LW(2))
            ADD_FLOAT(LW(0), FLOAT(-52.0f))
            ADD_FLOAT(LW(1), FLOAT(60.0f))
            ADD_FLOAT(LW(2), FLOAT(20.0f))
            CALL(evt_eff, STRING("KEMURI_EFF_00"), STRING("kemuri_test"), 19, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
            WAIT_MS(600)
            RETURN
        END_IF
    END_IF
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
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_ICE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPosFloat, -2, LW(0), LW(1), LW(2))
    ADD_FLOAT(LW(0), FLOAT(-52.0f))
    ADD_FLOAT(LW(1), FLOAT(60.0f))
    ADD_FLOAT(LW(2), FLOAT(10.0f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(5), LW(6), LW(7))
    ADD_FLOAT(LW(7), FLOAT(10.0f))
    CALL(evt_eff64, STRING("ICE_EFF_00"), STRING("opuku_wgun_n64"), 0, LW(0), LW(1), LW(2), LW(5), LW(6), LW(7), FLOAT(1.0f), 9, 0, 0, 0)
    WAIT_MS(150)
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
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 69)
    WAIT_MS(300)
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 69)
    LABEL(98)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

EVT_BEGIN(batten_leader_shikaeshi_attack_event)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_STUFF_PUTOUT1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1A"))
    WAIT_MS(170)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_STUFF_PUTOUT2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(eff_staff, -2)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1B"))
    WAIT_MS(2000)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_STUFF_PISECE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1C"))
    WAIT_MS(140)
    CALL(enzan_lw_save)
    SET_VALUE(LW(9), PTR(btl_shuryolight_rod_normal))
    SET_VALUE(LW(0), 8)
    CALL(btlevtcmd_GetUnitWork, -2, 7, LW(1))
    IF_INT_NOT_EQUAL(LW(0), LW(1))
        CALL(btlevtcmd_SetUnitWork, -2, 7, LW(0))
        CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
        DELETE_EVENT(LW(0))
        SPAWN_EVENT_GET_ID(PTR(light_rod_anim_rtn), LW(0))
        CALL(btlevtcmd_SetUnitWork, -2, 5, LW(0))
    END_IF
    CALL(enzan_lw_load)
    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(1))
    IF_INT_GREATER(LW(1), 0)
        CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
        CALL(btlevtcmd_GetRandomValue, 100, LW(2))
        IF_INT_LESS(LW(2), LW(1))
            CALL(btlevtcmd_GetPosFloat, -2, LW(0), LW(1), LW(2))
            ADD_FLOAT(LW(0), FLOAT(-25.0f))
            ADD_FLOAT(LW(1), FLOAT(80.0f))
            ADD_FLOAT(LW(2), FLOAT(20.0f))
            CALL(evt_eff, STRING("KEMURI_EFF_00"), STRING("kemuri_test"), 19, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
            WAIT_MS(600)
            RETURN
        END_IF
    END_IF
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 400)
    CALL(btlevtcmd_SetUnitWork, -2, 3, 4)
    CALL(btlevtcmd_CheckDamage, -2, -2, 1, LW(9), 256, LW(5))
    RETURN
EVT_END()

EVT_BEGIN(batten_leader_yokeyoke_attack_event)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_STUFF_PUTOUT1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1A"))
    WAIT_MS(170)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_STUFF_PUTOUT2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(eff_staff, -2)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1B"))
    WAIT_MS(2000)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_STUFF_PISECE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1C"))
    WAIT_MS(140)
    CALL(enzan_lw_save)
    SET_VALUE(LW(9), PTR(btl_shuryolight_rod_normal))
    SET_VALUE(LW(0), 8)
    CALL(btlevtcmd_GetUnitWork, -2, 7, LW(1))
    IF_INT_NOT_EQUAL(LW(0), LW(1))
        CALL(btlevtcmd_SetUnitWork, -2, 7, LW(0))
        CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
        DELETE_EVENT(LW(0))
        SPAWN_EVENT_GET_ID(PTR(light_rod_anim_rtn), LW(0))
        CALL(btlevtcmd_SetUnitWork, -2, 5, LW(0))
    END_IF
    CALL(enzan_lw_load)
    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(1))
    IF_INT_GREATER(LW(1), 0)
        CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
        CALL(btlevtcmd_GetRandomValue, 100, LW(2))
        IF_INT_LESS(LW(2), LW(1))
            CALL(btlevtcmd_GetPosFloat, -2, LW(0), LW(1), LW(2))
            ADD_FLOAT(LW(0), FLOAT(-25.0f))
            ADD_FLOAT(LW(1), FLOAT(80.0f))
            ADD_FLOAT(LW(2), FLOAT(20.0f))
            CALL(evt_eff, STRING("KEMURI_EFF_00"), STRING("kemuri_test"), 19, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
            WAIT_MS(600)
            RETURN
        END_IF
    END_IF
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 400)
    CALL(btlevtcmd_SetUnitWork, -2, 3, 5)
    CALL(btlevtcmd_CheckDamage, -2, -2, 1, LW(9), 256, LW(5))
    RETURN
EVT_END()

EVT_BEGIN(batten_leader_stop_attack_event)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_STUFF_PUTOUT1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1A"))
    WAIT_MS(170)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_STUFF_PUTOUT2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(eff_staff, -2)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1B"))
    WAIT_MS(2000)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_STUFF_PISECE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_2"))
    WAIT_MS(270)
    CALL(enzan_lw_save)
    SET_VALUE(LW(9), PTR(btl_shuryolight_rod_normal))
    SET_VALUE(LW(0), 8)
    CALL(btlevtcmd_GetUnitWork, -2, 7, LW(1))
    IF_INT_NOT_EQUAL(LW(0), LW(1))
        CALL(btlevtcmd_SetUnitWork, -2, 7, LW(0))
        CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
        DELETE_EVENT(LW(0))
        SPAWN_EVENT_GET_ID(PTR(light_rod_anim_rtn), LW(0))
        CALL(btlevtcmd_SetUnitWork, -2, 5, LW(0))
    END_IF
    CALL(enzan_lw_load)
    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(1))
    IF_INT_GREATER(LW(1), 0)
        CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
        CALL(btlevtcmd_GetRandomValue, 100, LW(2))
        IF_INT_LESS(LW(2), LW(1))
            CALL(btlevtcmd_GetPosFloat, -2, LW(0), LW(1), LW(2))
            ADD_FLOAT(LW(0), FLOAT(-52.0f))
            ADD_FLOAT(LW(1), FLOAT(60.0f))
            ADD_FLOAT(LW(2), FLOAT(20.0f))
            CALL(evt_eff, STRING("KEMURI_EFF_00"), STRING("kemuri_test"), 19, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
            WAIT_MS(600)
            RETURN
        END_IF
    END_IF
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SRYO_STOP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_cam_get_at, 4, LW(0), LW(1), LW(2))
    ADD_FLOAT(LW(0), FLOAT(-25.0f))
    SET_FLOAT_VALUE(LW(2), FLOAT(100.0f))
    CALL(evt_eff64, STRING("TIMESTOP_EFF_00"), STRING("timestamp_n64"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 150, 0, 0, 0, 0, 0, 0)
    WAIT_MS(2500)
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
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    SET_VALUE(LW(10), 0)
    LOOP(0)
        CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
        SWITCH(LW(5))
            CASE_OR_EQUAL(4)
            SET_VALUE(LW(6), 90)
            CASE_END
            CASE_EQUAL(3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
            SET_VALUE(LW(6), 90)
            CASE_END
            CASE_EQUAL(6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
            SET_VALUE(LW(6), 90)
            CASE_EQUAL(2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
            SET_VALUE(LW(6), 90)
            CASE_EQUAL(1)
            SET_VALUE(LW(6), 91)
            CASE_END
            CASE_DEFAULT
            SET_VALUE(LW(6), 97)
            CASE_END
        END_SWITCH
        GOTO(LW(6))
        LABEL(90)
        GOTO(97)
        LABEL(91)
        IF_INT_EQUAL(LW(10), 0)
            SET_VALUE(LW(10), 1)
            CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
        END_IF
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
        LABEL(97)
        CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
        IF_INT_EQUAL(LW(3), -1)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(check_satellite)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    LOOP(0)
        WAIT_FRAMES(1)
        CALL(check_satellite)
    END_LOOP
    RETURN
EVT_END()

// .bss objects are emitted in reverse declaration order by MWCC; listed here so the linked layout is
// xevt, xmemo, xlw2, xlw, maer, nowv (matching the original object).
Vec nowv;

f32 maer;

static u32 xlw[16];

// TODO: type
u32 xlw2[16];

// TODO: type
u32 xmemo[5];

// TODO: type
EventEntry* xevt[1];




// clang-format on
USER_FUNC(evt_eff_pokopi_thunder) {
    s32* args = event->args;
    f32 f31;
    f32 f30 = evtGetFloat(event, args[0]);
    f32 f29 = evtGetFloat(event, args[1]);
    f32 f28 = evtGetFloat(event, args[2]);
    f31 = 250.0f + f29;
    effPokopiThunderN64Entry(0, 0x1E, f30, f31, f28, f30, f29, f28, 1.0f);
    effPokopiThunderN64Entry(1, 0x1E, f30, f31, f28, f30, f29, f28, 1.0f);
    effPokopiThunderN64Entry(1, 0x1E, f30, f31, f28, f30, f29, f28, 1.0f);
    return EVT_RETURN_DONE;
}

typedef struct BattenEffUserdata {
    u8 pad_0x0[0x8];
    f32 field_0x8;
    f32 field_0xC;
    f32 barrierSize;
    u8 pad_0x14[0x4];
    s32 field_0x18;
    s32 field_0x1C;
    s32 field_0x20;
    u8 pad_0x24[0x4];
    s32 field_0x28;
    s32 field_0x2C;
    s32 field_0x30;
    u8 pad_0x34[0x4];
    f32 posX;        // evt_eff_firechange00 moves it by its velocity and writes it back
    f32 posY;
} BattenEffUserdata;

USER_FUNC(evt_eff_colorchange00) {
    EffectEntry* effect = effNameToPtr((const char*)evtGetValue(event, event->args[0]));
    ((BattenEffUserdata*)effect->userdata)->field_0x18 = 0xFF;
    ((BattenEffUserdata*)effect->userdata)->field_0x1C = 0xFF;
    ((BattenEffUserdata*)effect->userdata)->field_0x20 = 0xFF;
    ((BattenEffUserdata*)effect->userdata)->field_0x28 = 0xFF;
    ((BattenEffUserdata*)effect->userdata)->field_0x2C = 0xF5;
    ((BattenEffUserdata*)effect->userdata)->field_0x30 = 0;
    ((BattenEffUserdata*)effect->userdata)->field_0x8 = 65.0f + ((BattenEffUserdata*)effect->userdata)->field_0x8;
    ((BattenEffUserdata*)effect->userdata)->field_0xC = 10.0f + ((BattenEffUserdata*)effect->userdata)->field_0xC;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_eff_barriersizechange) {
    s32* args = event->args;
    EffectEntry* effect = effNameToPtr((const char*)evtGetValue(event, args[0]));
    ((BattenEffUserdata*)effect->userdata)->barrierSize = evtGetFloat(event, args[1]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_eff_firechange00) {
    s32* args = event->args;
    EffectEntry* eff;
    s32 mode;
    Vec target;
    Vec pos;
    Vec v;
    Mtx mtx;
    BOOL flag;
    f32 angle;
    f32 tmp2;

    eff = effNameToPtr((const char*)evtGetValue(event, args[0]));
    mode = evtGetValue(event, args[1]);
    target.x = (f32)evtGetValue(event, args[2]);
    target.y = (f32)evtGetValue(event, args[3]);
    target.z = 0.0f;
    pos.x = ((BattenEffUserdata*)eff->userdata)->posX;
    pos.y = ((BattenEffUserdata*)eff->userdata)->posY;
    pos.z = 0.0f;
    if (isFirstCall) {
        event->field_0x84 = 0;
        if (mode == 0) {
            PSVECSubtract(&target, &pos, &nowv);
            PSVECNormalize(&nowv, &nowv);
            maer = (f32)((rand() % 60) - 30);
            maer += (f32)((maer < 0.0f) ? -30 : 31);
            ((BattenEffUserdata*)eff->userdata)->field_0x18 = 0xFF;
            ((BattenEffUserdata*)eff->userdata)->field_0x1C = 0xFF;
            ((BattenEffUserdata*)eff->userdata)->field_0x20 = 0xFF;
            ((BattenEffUserdata*)eff->userdata)->field_0x28 = 0xFF;
            ((BattenEffUserdata*)eff->userdata)->field_0x2C = 0x41;
            ((BattenEffUserdata*)eff->userdata)->field_0x30 = 0x14;
        }
    }
    switch (event->field_0x84) {
        case 0:
            break;
        case 1:
            target.x = -2700.0f;
            break;
    }
    PSVECSubtract(&target, &pos, &v);
    flag = 0;
    if (PSVECSquareMag(&v) > 2500.0f) {
        maer += (f32)((rand() % 41) - 20);
        if (maer > 80.0f || maer < -80.0f || (maer < 20.0f && maer > -20.0f)) {
            maer = (f32)((rand() % 60) - 30);
            maer += (f32)((maer < 0.0f) ? -30 : 31);
            flag = 1;
        }
        PSMTXRotRad(mtx, 'Z', 0.017453292f * maer);
        PSMTXMultVec(mtx, &v, &v);
    } else {
        flag = 1;
    }
    PSVECNormalize(&v, &v);
    angle = angleABf(0.0f, 0.0f, nowv.x, nowv.y);
    tmp2 = angleABf(0.0f, 0.0f, v.x, v.y);
    angle = compAngle(angle, tmp2);
    if (flag == 0) {
        f32 absangle = (angle >= 0.0f) ? angle : -angle;
        if (absangle > 15.0f) {
            angle = angle > 0.0f ? 15.0f : -15.0f;
        }
    }
    PSMTXRotRad(mtx, 'Z', 0.017453292f * angle);
    PSMTXMultVec(mtx, &nowv, &nowv);
    PSVECScale(&nowv, &v, 15.0f);
    PSVECAdd(&pos, &v, &pos);
    if (pos.y < 10.0f) {
        if (((BattenEffUserdata*)eff->userdata)->posY >= 10.0f) {
            nowv.y = -nowv.y;
        } else if (nowv.y < 0.0f) {
            nowv.y = -nowv.y;
        }
    }
    ((BattenEffUserdata*)eff->userdata)->posX = pos.x;
    ((BattenEffUserdata*)eff->userdata)->posY = pos.y;
    switch (event->field_0x84) {
        case 0:
            if (PSVECSquareDistance(&pos, &target) < 225.0f) {
                if (mode != 0) {
                    event->field_0x84 = 1;
                } else {
                    return 1;
                }
            }
            break;
        case 1:
            if (pos.x <= -270.0f) {
                effSoftDelete(eff);
                return 1;
            }
            break;
    }
    return 0;
}

static USER_FUNC(eff_staff) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 id;
    EffSyuryouData* data;
    f32 x, y, z;
    f32 faceDir;
    id = BattleTransID(event, evtGetValue(event, args[0]));
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    data = effSyuryouEntry(0, 0x5A, 0.0f, -1000.0f, 0.0f)->userdata;
    BtlUnit_GetPos(unit, &x, &y, &z);
    faceDir = (f32)unit->faceDirection;
    data->position.x = 6.0f * (unit->mSizeMultiplier * faceDir) + x;
    data->position.y = 68.0f * unit->mSizeMultiplier + y;
    data->position.z = 5.0f + z;
    data->scale = unit->mSizeMultiplier;
    return EVT_RETURN_DONE;
}

USER_FUNC(check_satellite) {
    BattleWork* wp;
    BattleUnit* unit;
    s32 count;
    s32 i;
    s32 haveAll;
    s32 id;
    id = BattleTransID(event, -2);
    wp = _battleWorkPointer;
    unit = BattleGetUnitPtr(wp, id);
    count = 0;
    haveAll = 0;
    if (unit == NULL) {
        return EVT_RETURN_DONE;
    }
    for (i = 9; i < 13; i++) {
        if (unit->work[i] != -1) {
            if (BattleGetUnitPtr(_battleWorkPointer, unit->work[i]) != NULL) {
                count++;
            } else {
                unit->work[i] = -1;
            }
        }
    }
    if (count >= 4) {
        haveAll = 1;
    }
    if (haveAll == 0) {
        BattleUnitPart* bodyPart = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
        BattleUnitPart* part2 = BtlUnit_GetPartsPtr(unit, 2);
        f32 x, y, z;
        if (bodyPart->attributes & 0x40000000) {
            EventEntry* evt = evtEntryType(hakai_snd_event, event->priority, 0, event->type);
            evt->unitId = event->unitId;
            evt->ownerNPC = event->ownerNPC;
            for (i = 0; i < 16; i++) {
                evt->lwData[i] = event->lwData[i];
            }
            for (i = 0; i < 3; i++) {
                evt->lfData[i] = event->lfData[i];
            }
            evt->uwBase = event->uwBase;
            evt->ufBase = event->ufBase;
            effDelete((EffectEntry*)unit->work[8]);
            unit->work[8] = 0;
            BtlUnit_GetPos(unit, &x, &y, &z);
            for (i = 0; i < 20; i++) {
                s32 w = BtlUnit_GetWidth(unit) + 15;
                s32 h = BtlUnit_GetHeight(unit) + 15;
                s32 rx = irand(w) - w / 2;
                s32 ry = irand(h) - h / 2;
                effGlassN64Entry(2, 0x3C, x + (f32)rx, y + (f32)ry, z, 1.0f);
            }
        }
        bodyPart->attributes &= ~0x40000000;
        part2->attributes &= ~0x20000000;
        bodyPart->defenseAttr = &defence_attr;
        for (i = 0; i < 5; i++) {
            bodyPart->defense->defenses[i] = defencecopy[i];
        }
        bodyPart->defense->defenses[0] += count;
    } else {
        BattleUnitPart* bodyPart = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
        BattleUnitPart* part2 = BtlUnit_GetPartsPtr(unit, 2);
        f32 a, b, c;
        f32 x, y, z;
        if (!(bodyPart->attributes & 0x40000000)) {
            BtlUnit_snd_se(unit, "SFX_BOSS_SRYO_LIGHT_ROUND1", 0xF1194D80, 0);
            BtlUnit_GetPos(unit, &a, &b, &c);
            unit->work[8] = (s32)effSyuryouEntry(1, 0x3E8, 0.0f, -1000.0f, 0.0f);
        }
        if (unit->work[8] != 0) {
            EffSyuryouData* data = ((EffectEntry*)unit->work[8])->userdata;
            BtlUnit_GetPos(unit, &x, &y, &z);
            data->position.x = x;
            data->position.y = y - 12.5f * unit->mSizeMultiplier;
            data->position.z = z + 10.0f;
            data->scale = 2.5f * unit->mSizeMultiplier;
        }
        bodyPart->attributes |= 0x40000000;
        part2->attributes |= 0x20000000;
        bodyPart->defenseAttr = &defence_attr_muteki;
        for (i = 0; i < 5; i++) {
            bodyPart->defense->defenses[i] = defencecopy[i];
        }
        bodyPart->defense->defenses[0] += count;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(battleGetBattenSatelliteInitPos) {
    s32* args;
    BattleUnit* unit;
    s32 id;
    s32 n;
    s32 m;
    s32 i;
    s32 j;
    s32 indexX;
    s32 indexY;
    s32 indexZ;
    const Vec* p;
    BattleWork* wp;
    args = event->args;
    wp = _battleWorkPointer;
    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(wp, id);
    n = evtGetValue(event, args[1]);
    m = evtGetValue(event, args[2]);
    indexX = args[3];
    indexY = args[4];
    indexZ = args[5];
    p = VecData;
    for (i = 0; i < n; i++) {
        for (j = 0; j < i; j++) {
            p++;
        }
    }
    for (j = 0; j < m; j++) {
        p++;
    }
    evtSetValue(event, indexX, (s32)(p->x * unit->faceDirection));
    evtSetValue(event, indexY, (s32)p->y);
    evtSetValue(event, indexZ, (s32)p->z);
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_AnimeGetPose) {
    s32 unitId;
    s32 arg0, arg1;
    s32* args;
    args = event->args;
    arg0 = evtGetValue(event, args[0]);
    unitId = BattleTransID(event, arg0);
    arg1 = evtGetValue(event, args[1]);
    evtSetValue(event, args[2],
                (s32)animPoseGetCurrentAnim(BattleGetUnitPartsPtr(unitId, arg1)->poseId));
    return EVT_RETURN_DONE;
}

static USER_FUNC(enzan_lw_save) {
    s32 i;
    for (i = 0; i < 16; i++) {
        xlw[i] = event->lwData[i];
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(enzan_lw_load) {
    s32 i;
    for (i = 0; i < 16; i++) {
        event->lwData[i] = xlw[i];
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(enzan_lw_save2) {
    s32 i;
    for (i = 0; i < 16; i++) {
        xlw2[i] = event->lwData[i];
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(enzan_lw_load2) {
    s32 i;
    for (i = 0; i < 16; i++) {
        event->lwData[i] = xlw2[i];
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(datastrcopy) {
    s8* src = (s8*)evtGetValue(event, event->args[0]);
    s8* dst = (s8*)xmemo;
    s32 i = 0;
    while (i < 16 && src[i] != 0) {
        dst[i] = src[i];
        i++;
    }
    ((s8*)xmemo)[i] = 0;
    return EVT_RETURN_DONE;
}

USER_FUNC(pose_check_evt_run) {
    xevt[0] = evtEntryType(maiframeanimcheck, event->priority, 0, event->type);
    xevt[0]->unitId = event->unitId;
    xevt[0]->ownerNPC = event->ownerNPC;
    ((u8*)xmemo)[0] = 0;
    xevt[0]->lwData[0] = (s32)xmemo;
    xevt[0]->uwBase = event->uwBase;
    xevt[0]->ufBase = event->ufBase;
    return EVT_RETURN_DONE;
}

USER_FUNC(pose_check_evt_stop) {
    evtDeleteID(xevt[0]->eventId);
    return EVT_RETURN_DONE;
}

USER_FUNC(btl_hitoffsetx) {
    s32 unitId;
    BattleUnitPart* part;
    s32 arg0, arg1, arg2;
    s32* args;
    args = event->args;
    arg0 = evtGetValue(event, args[0]);
    unitId = BattleTransID(event, arg0);
    arg1 = evtGetValue(event, args[1]);
    part = BattleGetUnitPartsPtr(unitId, arg1);
    arg2 = evtGetValue(event, args[2]);
    part->detectOffsetX = arg2;
    return EVT_RETURN_DONE;
}

USER_FUNC(_set_draw_callback) {
    BattleWork* wp;
    BattleUnit* unit;
    BattleUnitPart *part1, *part3, *part4, *part5, *part6, *part7;
    s32 id;
    wp = _battleWorkPointer;
    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(wp, id);
    part1 = BtlUnit_GetPartsPtr(unit, 1);
    animPoseSetDispCallBack(part1->poseId, _light_point_callback, part1);
    part3 = BtlUnit_GetPartsPtr(unit, 3);
    part4 = BtlUnit_GetPartsPtr(unit, 4);
    part5 = BtlUnit_GetPartsPtr(unit, 5);
    part6 = BtlUnit_GetPartsPtr(unit, 6);
    part7 = BtlUnit_GetPartsPtr(unit, 7);
    part3->attributes |= 0x01000000;
    part4->attributes |= 0x01000000;
    part5->attributes |= 0x01000000;
    part6->attributes |= 0x01000000;
    part7->attributes |= 0x01000000;
    return EVT_RETURN_DONE;
}

void _light_point_callback(void* userData, s32 group, Mtx mtx) {
    BattleUnitPart* part = userData;
    BattleUnit* unit = part->mOwner;
    s32 pose = part->poseId;
    s32 idx;
    if (strcmp("locator6", animPoseGetGroupName(pose, group)) == 0) {
        idx = 0;
    } else if (strcmp("locator9", animPoseGetGroupName(pose, group)) == 0) {
        idx = 1;
    } else if (strcmp("locator8", animPoseGetGroupName(pose, group)) == 0) {
        idx = 2;
    } else if (strcmp("locator7", animPoseGetGroupName(pose, group)) == 0) {
        idx = 3;
    } else if (strcmp("locator5", animPoseGetGroupName(pose, group)) == 0) {
        idx = 4;
    } else {
        idx = -1;
    }
    switch (idx) {
    case 0: {
        s32 pose3 = BtlUnit_GetPartsPtr(unit, 3)->poseId;
        s32 shape = animPoseGetShapeIdx(pose3, "pPlaneShape34");
        animPoseDrawShape(pose3, shape);
        break;
    }
    case 1: {
        s32 pose4 = BtlUnit_GetPartsPtr(unit, 4)->poseId;
        s32 shape = animPoseGetShapeIdx(pose4, "pPlaneShape37");
        animPoseDrawShape(pose4, shape);
        break;
    }
    case 2: {
        s32 pose5 = BtlUnit_GetPartsPtr(unit, 5)->poseId;
        s32 shape = animPoseGetShapeIdx(pose5, "pPlaneShape36");
        animPoseDrawShape(pose5, shape);
        break;
    }
    case 3: {
        s32 pose6 = BtlUnit_GetPartsPtr(unit, 6)->poseId;
        s32 shape = animPoseGetShapeIdx(pose6, "pPlaneShape35");
        animPoseDrawShape(pose6, shape);
        break;
    }
    case 4: {
        s32 pose7 = BtlUnit_GetPartsPtr(unit, 7)->poseId;
        s32 shape = animPoseGetShapeIdx(pose7, "pPlaneShape36");
        animPoseDrawShape(pose7, shape);
        break;
    }
    }
}

USER_FUNC(unk_las_0000d538) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32* args = event->args;
    s32 arg = evtGetValue(event, *args++);

    unit = BattleGetUnitPtr(wp, BattleTransID(event, arg));
    if (unit != NULL) {
        unit->hpGauge.flags |= 1;
    }
    return EVT_RETURN_DONE;
}

typedef struct BattenSatelliteSpline {
    s32 type;        // spline_getvalue's type (the point count, 5)
    Vec* points;     // the spline table spline_maketable builds from
    f32 knots[5];
    Vec tangents[5];
    s32 frame;       // intplGetValue frame (0..0x3C); the work is freed at 0x3C
} BattenSatelliteSpline;

u8 battenSatelliteTo_Init(struct BattleUnit* unit, s32 index, u32* table) {
    BattenSatelliteSpline* data;
    struct BattleUnit* sat;
    data = __memAlloc(4, 0x5C);
    if (unit->work[9 + index] == -1) {
        return 0;
    }
    unit->work[13 + index] = (s32)data;
    data->type = 5;
    data->points = (Vec*)table;
    data->frame = 0;
    spline_maketable(5, data->points, data->knots, data->tangents);
    sat = BattleGetUnitPtr(_battleWorkPointer, unit->work[9 + index]);
    BtlUnit_GetPartsPtr(sat, 1)->work[1] = (s32)table;
    return 1;
}

u8 battenSatelliteTo_Main(struct BattleUnit* unit, s32 index) {
    BattenSatelliteSpline* data;
    struct BattleUnit* sat;
    f32 pos[3];
    f32 spline[3];
    if (unit->work[9 + index] == -1) {
        return 0;
    }
    data = (BattenSatelliteSpline*)unit->work[13 + index];
    if (data == NULL) {
        return 0;
    }
    if (data->frame >= 0x3C) {
        if (data != NULL) {
            __memFree(4, data);
            unit->work[13 + index] = 0;
        }
        return 0;
    }
    spline_getvalue(spline, data->type, data->points, data->knots, data->tangents,
                    intplGetValue(4, data->frame, 0x3C, 0.0f, 1.0f));
    sat = BattleGetUnitPtr(_battleWorkPointer, unit->work[9 + index]);
    if (sat == NULL) {
        return 0;
    }
    BtlUnit_GetPos(unit, &pos[0], &pos[1], &pos[2]);
    BtlUnit_SetPos(sat, spline[0] * (f32)unit->faceDirection + pos[0], 5.0f + (pos[1] + spline[1]), pos[2] + spline[2]);
    BtlUnit_SetHomePos(sat, spline[0] * (f32)unit->faceDirection + pos[0], 5.0f + (pos[1] + spline[1]), pos[2] + spline[2]);
    data->frame += 1;
    return 1;
}

USER_FUNC(battenSatelliteToInside) {
    s32* args;
    BattleUnit* unit;
    BattleUnitPart* part;
    u8 haveAll;
    s32 id;
    s32 i;
    s32 table;
    s32 tid;
    args = event->args;
    id = evtGetValue(event, args[0]);
    evtGetValue(event, args[1]);
    tid = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, tid);
    if (isFirstCall) {
        for (i = 0; i < 4; i++) {
            if (unit->work[9 + i] == -1) {
                continue;
            }
            part = BtlUnit_GetPartsPtr(BattleGetUnitPtr(_battleWorkPointer, unit->work[9 + i]), 1);
            table = part->work[1];
            if (table == (s32)splineTableOut1_1) {
                battenSatelliteTo_Init(unit, i, splineTableIn1_1);
            }
            if (table == (s32)splineTableOut2_1) {
                battenSatelliteTo_Init(unit, i, splineTableIn2_1);
            }
            if (table == (s32)splineTableOut2_2) {
                battenSatelliteTo_Init(unit, i, splineTableIn2_2);
            }
            if (table == (s32)splineTableOut3_1) {
                battenSatelliteTo_Init(unit, i, splineTableIn3_1);
            }
            if (table == (s32)splineTableOut3_2) {
                battenSatelliteTo_Init(unit, i, splineTableIn3_2);
            }
            if (table == (s32)splineTableOut3_3) {
                battenSatelliteTo_Init(unit, i, splineTableIn3_3);
            }
            if (table == (s32)splineTableOut4_1) {
                battenSatelliteTo_Init(unit, i, splineTableIn4_1);
            }
            if (table == (s32)splineTableOut4_2) {
                battenSatelliteTo_Init(unit, i, splineTableIn4_2);
            }
            if (table == (s32)splineTableOut4_3) {
                battenSatelliteTo_Init(unit, i, splineTableIn4_3);
            }
            if (table == (s32)splineTableOut4_4) {
                battenSatelliteTo_Init(unit, i, splineTableIn4_4);
            }
        }
    }
    haveAll = 0;
    for (i = 0; i < 4; i++) {
        if (battenSatelliteTo_Main(unit, i) == 1) {
            haveAll = 1;
        }
    }
    return haveAll == 0 ? EVT_RETURN_DONE : 0;
}

USER_FUNC(battenSatelliteToOutside) {
    s32* args = event->args;
    s32 id;
    s32 type;
    s32 tid;
    BattleUnit* unit;
    s32 i;
    u8 haveAll;

    id = evtGetValue(event, args[0]);
    type = evtGetValue(event, args[1]);
    tid = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, tid);
    if (isFirstCall) {
        switch (type) {
            case 1:
                i = 0;
                for (;;) {
                    if (battenSatelliteTo_Init(unit, i, splineTableOut1_1) == 1) {
                        break;
                    }
                    i++;
                }
                break;
            case 2:
                i = 0;
                for (;;) {
                    if (battenSatelliteTo_Init(unit, i, splineTableOut2_1) == 1) {
                        break;
                    }
                    i++;
                }
                i++;
                for (;;) {
                    if (battenSatelliteTo_Init(unit, i, splineTableOut2_2) == 1) {
                        break;
                    }
                    i++;
                }
                break;
            case 3:
                i = 0;
                for (;;) {
                    if (battenSatelliteTo_Init(unit, i, splineTableOut3_1) == 1) {
                        break;
                    }
                    i++;
                }
                i++;
                for (;;) {
                    if (battenSatelliteTo_Init(unit, i, splineTableOut3_2) == 1) {
                        break;
                    }
                    i++;
                }
                i++;
                for (;;) {
                    if (battenSatelliteTo_Init(unit, i, splineTableOut3_3) == 1) {
                        break;
                    }
                    i++;
                }
                break;
            case 4:
                i = 0;
                for (;;) {
                    if (battenSatelliteTo_Init(unit, i, splineTableOut4_1) == 1) {
                        break;
                    }
                    i++;
                }
                i++;
                for (;;) {
                    if (battenSatelliteTo_Init(unit, i, splineTableOut4_2) == 1) {
                        break;
                    }
                    i++;
                }
                i++;
                for (;;) {
                    if (battenSatelliteTo_Init(unit, i, splineTableOut4_3) == 1) {
                        break;
                    }
                    i++;
                }
                i++;
                for (;;) {
                    if (battenSatelliteTo_Init(unit, i, splineTableOut4_4) == 1) {
                        break;
                    }
                    i++;
                }
                break;
            default:
                return EVT_RETURN_DONE;
        }
    }
    haveAll = 0;
    for (i = 0; i < 4; i++) {
        if (battenSatelliteTo_Main(unit, i) == 1) {
            haveAll = 1;
        }
    }
    return haveAll == 0 ? EVT_RETURN_DONE : 0;
}
