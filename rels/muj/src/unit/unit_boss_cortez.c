#include "unit_boss_cortez.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_batstage.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_msg.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"
#include "battle/battle.h"
#include "battle/battle_audience.h"
#include "battle/battle_sub.h"
#include "driver/effdrv.h"
#include "eff/eff_spirit.h"
#include "eff/n64/eff_kumokumo_n64.h"
#include "manager/evtmgr_cmd.h"
#include "system.h"

// Work area of the cortez unit, allocated by _make_cortez_work_area (size 0xE14).
typedef struct CortezWork {
    s16 state;                 // 0x0
    s16 count;                 // 0x2
    s16 audienceId[100];       // 0x4
    s16 timer[100];            // 0xCC
    s16 field_194[100];        // 0x194
    s16 field_25C[100];        // 0x25C
    EffectEntry* effects[100]; // 0x324
    Vec pos1[100];             // 0x4B4
    Vec pos2[100];             // 0x964
} CortezWork;

// psndSFX_get_vol has no header prototype; this file masks the u8 result (clrlwi 24).
extern u8 psndSFX_get_vol(s32 id);

#pragma pool_data off

#pragma force_active on
extern s32 cortez_weapon_get_home_height[];
extern s32 cortez_weapon_set_formation_attribute[];
extern BattleUnitKind unit_boss_cortez_claw;
extern BattleUnitKind unit_boss_cortez_rapier;
extern BattleUnitKind unit_boss_cortez_saber;
extern BattleUnitKind unit_boss_cortez_sword;
static USER_FUNC(_check_zenei_kouei);
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[7];
static BattleUnitKindPart parts[19];
static s32 init_event[260];
static s32 damage_event[70];
s32 cortez_ac_counter_damage_event[98];
static s32 avoid_thorn_event[89];
static s32 phase_event[196];
s32 attack_event_mode1[44];
s32 at2_head_bat_event[202];
s32 at2_hone_charge_event[146];
s32 at2_hone_rain_attack_event[605];
s32 _bone_rain_event[114];
static s32 attack_audience_event[247];
s32 at3_kamitsuki_attack[264];
s32 at1_normal_attack[205];
s32 at1_midaregiri_attack[251];
static s32 wait_event[6];
BattleUnitSetup entry_claw[1];
BattleUnitSetup entry_rapier[1];
BattleUnitSetup entry_sword[1];
BattleUnitSetup entry_saber[1];
s32 cortez_reset_pose_table[450];
static s32 cortez_damage_sub_event[153];
static s32 dead_event[1033];
s32 mode2_rib_open_event[54];
s32 mode2_rib_close_event[54];
s32 mode2_honeduka_damage_event[76];
s32 mode_change_event[111];

// clang-format off
extend_data(48,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(49,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_boss_cortez = {
    UNIT_CORTEZ,                // mUnitType
    "btl_un_cortez",            // name
    20,                         // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    77,                         // mLevel
    0,                          // mBonusExp
    0,                          // bonusCoinRolls
    100,                        // bonusCoinRate
    0,                          // baseCoin
    50,                         // field_0x13
    6,                          // field_0x14
    250,                        // width
    170,                        // height
    { -10, 90 },                // statusIconOffset
    { 0.0f, 0.0f, 0.0f },       // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },       // talkTogeBaseOffset
    { 125.0f, 0.0f, -10.0f },   // field_0x3C
    { 0.0f, 0.0f, 25.0f },      // burnFlameOffset
    0.0f,                       // field_0x54
    0.0f,                       // field_0x58
    { 32.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 0.0f, 0.0f, 0.0f },       // kissHitOffset
    { -70.0f, 65.0f, 0.0f },    // cutBaseOffset
    180.0f,                     // cutWidth
    130.0f,                     // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    -1,                         // swallowRate
    0,                          // swallowAttribute
    100,                        // field_0x8C
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_BOSS_KOR_DAMAGED1",    // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0x00200000,                 // attributeFlags
    &regist,                    // vulnStatus
    19,                         // numParts
    parts,                      // mParts
    init_event,                 // initEvtScript
    data_table,                 // dataTable
};

static BattleVulnerableStatus regist = {
    40,  // sleepRate
    40,  // stopRate
    50,  // dizzyRate
    100, // poisonRate
    50,  // confuseRate
    100, // electricRate
    100, // burnRate
    25,  // freezeRate
    0,   // hugeRate
    0,   // tinyRate
    100, // attackUpRate
    0,   // attackDownRate
    100, // defenseUpRate
    90,  // defenseDownRate
    50,  // allergicRate
    0,   // frightRate
    0,   // galeForceRate
    100, // fastRate
    50,  // slowRate
    0,   // dodgyRate
    0,   // invisibleRate
    0,   // instaKillRate
};

BattleUnitDefense defence_mode1 = { { 1, 1, 1, 1, 1 } };

BattleUnitDefenseAttr defence_attr_mode1 = { { 0, 0, 0, 0, 0 } };

BattleUnitDefense defence_mode2_upper_head = { { 1, 1, 1, 1, 1 } };

BattleUnitDefenseAttr defence_attr_mode2_upper_head = { { 0, 0, 0, 0, 0 } };

BattleUnitDefense defence_mode2_upper_rib_open = { { 0, 0, 0, 0, 0 } };

BattleUnitDefenseAttr defence_attr_mode2_upper_rib_open = { { 1, 1, 1, 1, 1 } };

BattleUnitDefense defence_mode2_upper_rib_close = { { 3, 3, 3, 3, 3 } };

BattleUnitDefenseAttr defence_attr_mode2_upper_rib_close = { { 0, 0, 0, 0, 0 } };

BattleUnitDefense defence_mode2_middle_head = { { 1, 1, 1, 1, 1 } };

BattleUnitDefenseAttr defence_attr_mode2_middle_head = { { 0, 0, 0, 0, 0 } };

BattleUnitDefense defence_mode2_middle_rib_open = { { 0, 0, 0, 0, 0 } };

BattleUnitDefenseAttr defence_attr_mode2_middle_rib_open = { { 1, 1, 1, 1, 1 } };

BattleUnitDefense defence_mode2_middle_rib_close = { { 3, 3, 3, 3, 3 } };

BattleUnitDefenseAttr defence_attr_mode2_middle_rib_close = { { 0, 0, 0, 0, 0 } };

BattleUnitDefense defence_mode2_lower_head = { { 1, 1, 1, 1, 1 } };

BattleUnitDefenseAttr defence_attr_mode2_lower_head = { { 0, 0, 0, 0, 0 } };

BattleUnitDefense defence_mode2_lower_rib_open = { { 0, 0, 0, 0, 0 } };

BattleUnitDefenseAttr defence_attr_mode2_lower_rib_open = { { 1, 1, 1, 1, 1 } };

BattleUnitDefense defence_mode2_lower_rib_close = { { 3, 3, 3, 3, 3 } };

BattleUnitDefenseAttr defence_attr_mode2_lower_rib_close = { { 0, 0, 0, 0, 0 } };

BattleUnitDefense defence_mode3 = { { 1, 1, 1, 1, 1 } };

BattleUnitDefenseAttr defence_attr_mode3 = { { 0, 0, 0, 0, 0 } };

BattleUnitDefense defence_honeduka = { { 1, 1, 1, 1, 1 } };

BattleUnitDefenseAttr defence_honeduka_attr = { { 0, 0, 0, 0, 0 } };

BattleWeapon weapon_cortez_normal_front_attack = {
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
    { 4, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
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

BattleWeapon weapon_cortez_normal_rear_attack = {
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
    { 4, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x00000008,                               // targetWeightingFlags
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

BattleWeapon weapon_cortez_midaregiri_attack = {
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
    { 2, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80000000,                               // targetWeightingFlags
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

BattleWeapon weapon_cortez_mode2_head_attack = {
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
    0x01101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00001087,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80000000,                               // targetWeightingFlags
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

BattleWeapon weapon_cortez_mode2_charge = {
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
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000008,                               // special
    0x000007FF,                               // counterResistanceFlags
    0,                                        // targetWeightingFlags
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
    4,                                        // chargeStrength
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

BattleWeapon weapon_cortez_mode2_bone_attack = {
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
    { 4, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
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

BattleWeapon weapon_cortez_mode3_kamitsuki_attack = {
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
    0x01101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80000000,                               // targetWeightingFlags
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

// TODO: type
static u32 pose_sound_timing_table[365] = {
    (u32)"KRT_1S_1", 0x3C23D70A, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1",
    0x00000001, (u32)"KRT_1S_1", 0x3E4CCCCD, 0x00000000,
    (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_1S_1", 0x3ECCCCCD,
    0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_1S_1",
    0x3F19999A, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001,
    (u32)"KRT_1S_1", 0x3F4CCCCD, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1",
    0x00000001, (u32)"KRT_1S_1", 0x3F800000, 0x00000000,
    (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_1S2_1", 0x3C23D70A,
    0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_1S2_1",
    0x3E4CCCCD, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001,
    (u32)"KRT_1S2_1", 0x3ECCCCCD, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1",
    0x00000001, (u32)"KRT_1S2_1", 0x3F19999A, 0x00000000,
    (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_1S2_1", 0x3F4CCCCD,
    0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_1S2_1",
    0x3F800000, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001,
    (u32)"KRT_1S_2", 0x3C23D70A, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1",
    0x00000001, (u32)"KRT_1S_2", 0x3E4CCCCD, 0x00000000,
    (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_1S_2", 0x3ECCCCCD,
    0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_1S_2",
    0x3F19999A, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001,
    (u32)"KRT_1S_2", 0x3F4CCCCD, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1",
    0x00000001, (u32)"KRT_1S_2", 0x3F800000, 0x00000000,
    (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_1S2_2", 0x3C23D70A,
    0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_1S2_2",
    0x3E4CCCCD, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001,
    (u32)"KRT_1S2_2", 0x3ECCCCCD, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1",
    0x00000001, (u32)"KRT_1S2_2", 0x3F19999A, 0x00000000,
    (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_1S2_2", 0x3F4CCCCD,
    0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_1S2_2",
    0x3F800000, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001,
    (u32)"KRT_2S_1", 0x3C23D70A, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1",
    0x00000001, (u32)"KRT_2S_1", 0x3E4CCCCD, 0x00000000,
    (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_2S_1", 0x3ECCCCCD,
    0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_2S_1",
    0x3F19999A, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001,
    (u32)"KRT_2S_1", 0x3F4CCCCD, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1",
    0x00000001, (u32)"KRT_2S_1", 0x3F800000, 0x00000000,
    (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_2S2_1", 0x3C23D70A,
    0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_2S2_1",
    0x3E4CCCCD, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001,
    (u32)"KRT_2S2_1", 0x3ECCCCCD, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1",
    0x00000001, (u32)"KRT_2S2_1", 0x3F19999A, 0x00000000,
    (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_2S2_1", 0x3F4CCCCD,
    0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_2S2_1",
    0x3F800000, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001,
    (u32)"KRT_2S_2", 0x3C23D70A, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1",
    0x00000001, (u32)"KRT_2S_2", 0x3E4CCCCD, 0x00000000,
    (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_2S_2", 0x3ECCCCCD,
    0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_2S_2",
    0x3F19999A, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001,
    (u32)"KRT_2S_2", 0x3F4CCCCD, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1",
    0x00000001, (u32)"KRT_2S_2", 0x3F800000, 0x00000000,
    (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_2S2_2", 0x3C23D70A,
    0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_2S2_2",
    0x3E4CCCCD, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001,
    (u32)"KRT_2S2_2", 0x3ECCCCCD, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1",
    0x00000001, (u32)"KRT_2S2_2", 0x3F19999A, 0x00000000,
    (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_2S2_2", 0x3F4CCCCD,
    0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_2S2_2",
    0x3F800000, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001,
    (u32)"KRT_3S_1", 0x3C23D70A, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1",
    0x00000001, (u32)"KRT_3S_1", 0x3E4CCCCD, 0x00000000,
    (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_3S_1", 0x3ECCCCCD,
    0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_3S_1",
    0x3F19999A, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001,
    (u32)"KRT_3S_1", 0x3F4CCCCD, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1",
    0x00000001, (u32)"KRT_3S_1", 0x3F800000, 0x00000000,
    (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_3S3_1", 0x3C23D70A,
    0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_3S3_1",
    0x3E4CCCCD, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001,
    (u32)"KRT_3S3_1", 0x3ECCCCCD, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1",
    0x00000001, (u32)"KRT_3S3_1", 0x3F19999A, 0x00000000,
    (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_3S3_1", 0x3F4CCCCD,
    0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_3S3_1",
    0x3F800000, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001,
    (u32)"KRT_3S_2", 0x3C23D70A, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1",
    0x00000001, (u32)"KRT_3S_2", 0x3E4CCCCD, 0x00000000,
    (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_3S_2", 0x3ECCCCCD,
    0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_3S_2",
    0x3F19999A, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001,
    (u32)"KRT_3S_2", 0x3F4CCCCD, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1",
    0x00000001, (u32)"KRT_3S_2", 0x3F800000, 0x00000000,
    (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_3S3_2", 0x3C23D70A,
    0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_3S3_2",
    0x3E4CCCCD, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001,
    (u32)"KRT_3S3_2", 0x3ECCCCCD, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1",
    0x00000001, (u32)"KRT_3S3_2", 0x3F19999A, 0x00000000,
    (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_3S3_2", 0x3F4CCCCD,
    0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001, (u32)"KRT_3S3_2",
    0x3F800000, 0x00000000, (u32)"SFX_BOSS_KOR_WAIT1", 0x00000001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000001,
};

BattleDispPose pose_table_mode_1[10] = {
    { 0x01, "KRT_N_1" },
    { 0x05, "KRT_K_1" },
    { 0x02, "KRT_Z_1" },
    { 0x09, "KRT_Z_1" },
    { 0x04, "KRT_X_1" },
    { 0x03, "KRT_K_1" },
    { 0x1C, "KRT_S_1" },
    { 0x1F, "KRT_S_1" },
    { 0x27, "KRT_D1_1" },
    { 0x45, "KRT_S_1" },
};

BattleDispPose pose_table_mode_1_hone[10] = {
    { 0x01, "KRT_N_1" },
    { 0x05, "KRT_K_1" },
    { 0x02, "KRT_Z_1" },
    { 0x09, "KRT_Z_1" },
    { 0x04, "KRT_X_1" },
    { 0x03, "KRT_K_1" },
    { 0x1C, "KRT_S_1" },
    { 0x1F, "KRT_S_1" },
    { 0x27, "KRT_D1_2" },
    { 0x45, "KRT_S_1" },
};

BattleDispPose pose_table_mode_2_normal[4] = {
    { 0x1C, "KRT_1S_1" },
    { 0x1F, "KRT_1S_1" },
    { 0x27, "KRT_1D_1" },
    { 0x45, "KRT_1S_1" },
};

BattleDispPose pose_table_mode_2_upper_normal[4] = {
    { 0x1C, "KRT_1S_1" },
    { 0x1F, "KRT_1S_1" },
    { 0x27, "KRT_1D_1" },
    { 0x45, "KRT_1S_1" },
};

BattleDispPose pose_table_mode_2_upper_normal_counter[4] = {
    { 0x1C, "KRT_1S2_1" },
    { 0x1F, "KRT_1S2_1" },
    { 0x27, "KRT_1D2_1" },
    { 0x45, "KRT_1S2_1" },
};

BattleDispPose pose_table_mode_2_upper_charge[4] = {
    { 0x1C, "KRT_1S_2" },
    { 0x1F, "KRT_1S_2" },
    { 0x27, "KRT_1D_2" },
    { 0x45, "KRT_1S_2" },
};

BattleDispPose pose_table_mode_2_upper_charge_counter[4] = {
    { 0x1C, "KRT_1S2_2" },
    { 0x1F, "KRT_1S2_2" },
    { 0x27, "KRT_1D2_2" },
    { 0x45, "KRT_1S2_2" },
};

BattleDispPose pose_table_mode_2_middle_normal[4] = {
    { 0x1C, "KRT_2S_1" },
    { 0x1F, "KRT_2S_1" },
    { 0x27, "KRT_2D_1" },
    { 0x45, "KRT_2S_1" },
};

BattleDispPose pose_table_mode_2_middle_normal_counter[4] = {
    { 0x1C, "KRT_2S2_1" },
    { 0x1F, "KRT_2S2_1" },
    { 0x27, "KRT_2D2_1" },
    { 0x45, "KRT_2S2_1" },
};

BattleDispPose pose_table_mode_2_middle_charge[4] = {
    { 0x1C, "KRT_2S_2" },
    { 0x1F, "KRT_2S_2" },
    { 0x27, "KRT_2D_2" },
    { 0x45, "KRT_2S_2" },
};

BattleDispPose pose_table_mode_2_middle_charge_counter[4] = {
    { 0x1C, "KRT_2S2_2" },
    { 0x1F, "KRT_2S2_2" },
    { 0x27, "KRT_2D2_2" },
    { 0x45, "KRT_2S2_2" },
};

BattleDispPose pose_table_mode_2_lower_normal[4] = {
    { 0x1C, "KRT_3S_1" },
    { 0x1F, "KRT_3S_1" },
    { 0x27, "KRT_3D_2" },
    { 0x45, "KRT_3S_1" },
};

BattleDispPose pose_table_mode_2_lower_normal_counter[4] = {
    { 0x1C, "KRT_3S3_1" },
    { 0x1F, "KRT_3S3_1" },
    { 0x27, "KRT_3D3_2" },
    { 0x45, "KRT_3S3_1" },
};

BattleDispPose pose_table_mode_2_lower_charge[4] = {
    { 0x1C, "KRT_3S_2" },
    { 0x1F, "KRT_3S_2" },
    { 0x27, "KRT_3D_2" },
    { 0x45, "KRT_3S_2" },
};

BattleDispPose pose_table_mode_2_lower_charge_counter[4] = {
    { 0x1C, "KRT_3S3_2" },
    { 0x1F, "KRT_3S3_2" },
    { 0x27, "KRT_3D3_2" },
    { 0x45, "KRT_3S3_2" },
};

BattleDispPose pose_table_fly_bone[4] = {
    { 0x1C, "KRT_Z_1" },
    { 0x1F, "KRT_Z_1" },
    { 0x27, "KRT_Z_1" },
    { 0x45, "KRT_Z_1" },
};

BattleDispPose pose_table_mode_3[12] = {
    { 0x01, "KRT_N_1" },
    { 0x02, "KRT_Z_1" },
    { 0x09, "KRT_Z_1" },
    { 0x05, "KRT_K_1" },
    { 0x04, "KRT_X_1" },
    { 0x03, "KRT_K_1" },
    { 0x1C, "KRT_S_1" },
    { 0x1D, "KRT_D_2" },
    { 0x1E, "KRT_D_2" },
    { 0x1F, "KRT_S_1" },
    { 0x27, "KRT_D_1" },
    { 0x45, "KRT_S_1" },
};

BattleDispPose pose_table_honeduka[4] = {
    { 0x1C, "KRT_Z_1" },
    { 0x1F, "KRT_Z_1" },
    { 0x27, "KRT_Z_1" },
    { 0x45, "KRT_Z_1" },
};

static BattleDataEntry data_table[7] = {
    { 0x09, cortez_damage_sub_event },
    { 0x31, dead_event },
    { 0x47, mode2_honeduka_damage_event },
    { 0x24, cortez_ac_counter_damage_event },
    { 0x27, avoid_thorn_event },
    { 0x25, avoid_thorn_event },
    { 0x00, NULL },
};

static BattleUnitKindPart parts[19] = {
    {
        1,                        // partNum
        "btl_un_cortez",          // partName
        "c_korutesu1",            // modelName
        { 0.0f, 0.0f, 0.0f },     // positionOffset
        { 145.0f, 90.0f, 30.0f }, // hitBaseOffset
        { 145.0f, 90.0f, 30.0f }, // hitCursorBaseOffset
        20,                       // field_0x30
        30,                       // field_0x32
        255,                      // baseAlpha
        &defence_honeduka,        // defense
        &defence_honeduka_attr,   // defenseAttr
        0x00600009,               // attributes
        0,                        // counterAttributes
        pose_table_mode_1,        // poseTable
    },
    {
        2,                                   // partNum
        "btl_un_cortez",                     // partName
        NULL,                                // modelName
        { 0.0f, 0.0f, 0.0f },                // positionOffset
        { 60.0f, 110.0f, 20.0f },            // hitBaseOffset
        { 60.0f, 110.0f, 20.0f },            // hitCursorBaseOffset
        20,                                  // field_0x30
        30,                                  // field_0x32
        255,                                 // baseAlpha
        &defence_mode2_upper_rib_close,      // defense
        &defence_attr_mode2_upper_rib_close, // defenseAttr
        0x03310004,                          // attributes
        0,                                   // counterAttributes
        pose_table_mode_1,                   // poseTable
    },
    {
        3,                    // partNum
        "btl_un_cortez",      // partName
        "c_otitabuki",        // modelName
        { 0.0f, 0.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f }, // hitCursorBaseOffset
        20,                   // field_0x30
        30,                   // field_0x32
        255,                  // baseAlpha
        &defence_mode1,       // defense
        &defence_attr_mode1,  // defenseAttr
        0x03010000,           // attributes
        0,                    // counterAttributes
        pose_table_mode_1,    // poseTable
    },
    {
        4,                       // partNum
        "btl_un_honeduka",       // partName
        "c_honezuka",            // modelName
        { 0.0f, 0.0f, 0.0f },    // positionOffset
        { 15.0f, 25.0f, 10.0f }, // hitBaseOffset
        { 0.0f, 30.0f, 10.0f },  // hitCursorBaseOffset
        45,                      // field_0x30
        30,                      // field_0x32
        255,                     // baseAlpha
        &defence_mode1,          // defense
        &defence_attr_mode1,     // defenseAttr
        0x02000006,              // attributes
        0x00000001,              // counterAttributes
        pose_table_honeduka,     // poseTable
    },
    {
        5,                      // partNum
        "btl_un_honeduka",      // partName
        "c_hone2",              // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 15.0f, 25.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 30.0f, 0.0f },  // hitCursorBaseOffset
        20,                     // field_0x30
        30,                     // field_0x32
        255,                    // baseAlpha
        &defence_mode1,         // defense
        &defence_attr_mode1,    // defenseAttr
        0x03010000,             // attributes
        0,                      // counterAttributes
        pose_table_fly_bone,    // poseTable
    },
    {
        6,                    // partNum
        "btl_un_honeduka",    // partName
        "c_hone",             // modelName
        { 0.0f, 0.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f }, // hitCursorBaseOffset
        20,                   // field_0x30
        30,                   // field_0x32
        255,                  // baseAlpha
        &defence_mode1,       // defense
        &defence_attr_mode1,  // defenseAttr
        0x13010000,           // attributes
        0,                    // counterAttributes
        pose_table_fly_bone,  // poseTable
    },
    {
        7,                    // partNum
        "btl_un_honeduka",    // partName
        "c_hone",             // modelName
        { 0.0f, 0.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f }, // hitCursorBaseOffset
        20,                   // field_0x30
        30,                   // field_0x32
        255,                  // baseAlpha
        &defence_mode1,       // defense
        &defence_attr_mode1,  // defenseAttr
        0x13010000,           // attributes
        0,                    // counterAttributes
        pose_table_fly_bone,  // poseTable
    },
    {
        8,                    // partNum
        "btl_un_honeduka",    // partName
        "c_hone",             // modelName
        { 0.0f, 0.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f }, // hitCursorBaseOffset
        20,                   // field_0x30
        30,                   // field_0x32
        255,                  // baseAlpha
        &defence_mode1,       // defense
        &defence_attr_mode1,  // defenseAttr
        0x13010000,           // attributes
        0,                    // counterAttributes
        pose_table_fly_bone,  // poseTable
    },
    {
        9,                    // partNum
        "btl_un_honeduka",    // partName
        "c_hone",             // modelName
        { 0.0f, 0.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f }, // hitCursorBaseOffset
        20,                   // field_0x30
        30,                   // field_0x32
        255,                  // baseAlpha
        &defence_mode1,       // defense
        &defence_attr_mode1,  // defenseAttr
        0x13010000,           // attributes
        0,                    // counterAttributes
        pose_table_fly_bone,  // poseTable
    },
    {
        10,                   // partNum
        "btl_un_honeduka",    // partName
        "c_hone",             // modelName
        { 0.0f, 0.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f }, // hitCursorBaseOffset
        20,                   // field_0x30
        30,                   // field_0x32
        255,                  // baseAlpha
        &defence_mode1,       // defense
        &defence_attr_mode1,  // defenseAttr
        0x13010000,           // attributes
        0,                    // counterAttributes
        pose_table_fly_bone,  // poseTable
    },
    {
        11,                   // partNum
        "btl_un_honeduka",    // partName
        "c_hone",             // modelName
        { 0.0f, 0.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f }, // hitCursorBaseOffset
        20,                   // field_0x30
        30,                   // field_0x32
        255,                  // baseAlpha
        &defence_mode1,       // defense
        &defence_attr_mode1,  // defenseAttr
        0x13010000,           // attributes
        0,                    // counterAttributes
        pose_table_fly_bone,  // poseTable
    },
    {
        12,                   // partNum
        "btl_un_honeduka",    // partName
        "c_hone",             // modelName
        { 0.0f, 0.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f }, // hitCursorBaseOffset
        20,                   // field_0x30
        30,                   // field_0x32
        255,                  // baseAlpha
        &defence_mode1,       // defense
        &defence_attr_mode1,  // defenseAttr
        0x13010000,           // attributes
        0,                    // counterAttributes
        pose_table_fly_bone,  // poseTable
    },
    {
        13,                   // partNum
        "btl_un_honeduka",    // partName
        "c_hone",             // modelName
        { 0.0f, 0.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f }, // hitCursorBaseOffset
        20,                   // field_0x30
        30,                   // field_0x32
        255,                  // baseAlpha
        &defence_mode1,       // defense
        &defence_attr_mode1,  // defenseAttr
        0x13010000,           // attributes
        0,                    // counterAttributes
        pose_table_fly_bone,  // poseTable
    },
    {
        14,                   // partNum
        "btl_un_honeduka",    // partName
        "c_hone",             // modelName
        { 0.0f, 0.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f }, // hitCursorBaseOffset
        20,                   // field_0x30
        30,                   // field_0x32
        255,                  // baseAlpha
        &defence_mode1,       // defense
        &defence_attr_mode1,  // defenseAttr
        0x13010000,           // attributes
        0,                    // counterAttributes
        pose_table_fly_bone,  // poseTable
    },
    {
        15,                   // partNum
        "btl_un_honeduka",    // partName
        "c_hone",             // modelName
        { 0.0f, 0.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f }, // hitCursorBaseOffset
        20,                   // field_0x30
        30,                   // field_0x32
        255,                  // baseAlpha
        &defence_mode1,       // defense
        &defence_attr_mode1,  // defenseAttr
        0x13010000,           // attributes
        0,                    // counterAttributes
        pose_table_fly_bone,  // poseTable
    },
    {
        16,                   // partNum
        "btl_un_honeduka",    // partName
        "c_hone",             // modelName
        { 0.0f, 0.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f }, // hitCursorBaseOffset
        20,                   // field_0x30
        30,                   // field_0x32
        255,                  // baseAlpha
        &defence_mode1,       // defense
        &defence_attr_mode1,  // defenseAttr
        0x13010000,           // attributes
        0,                    // counterAttributes
        pose_table_fly_bone,  // poseTable
    },
    {
        17,                   // partNum
        "btl_un_honeduka",    // partName
        "c_hone",             // modelName
        { 0.0f, 0.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f }, // hitCursorBaseOffset
        20,                   // field_0x30
        30,                   // field_0x32
        255,                  // baseAlpha
        &defence_mode1,       // defense
        &defence_attr_mode1,  // defenseAttr
        0x13010000,           // attributes
        0,                    // counterAttributes
        pose_table_fly_bone,  // poseTable
    },
    {
        18,                   // partNum
        "btl_un_honeduka",    // partName
        "c_hone",             // modelName
        { 0.0f, 0.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f }, // hitCursorBaseOffset
        20,                   // field_0x30
        30,                   // field_0x32
        255,                  // baseAlpha
        &defence_mode1,       // defense
        &defence_attr_mode1,  // defenseAttr
        0x13010000,           // attributes
        0,                    // counterAttributes
        pose_table_fly_bone,  // poseTable
    },
    {
        19,                   // partNum
        "btl_un_honeduka",    // partName
        "c_hone",             // modelName
        { 0.0f, 0.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f }, // hitCursorBaseOffset
        20,                   // field_0x30
        30,                   // field_0x32
        255,                  // baseAlpha
        &defence_mode1,       // defense
        &defence_attr_mode1,  // defenseAttr
        0x13010000,           // attributes
        0,                    // counterAttributes
        pose_table_fly_bone,  // poseTable
    },
};

USER_FUNC(_make_cortez_work_area) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 id;
    void* area;

    id = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(work, BattleTransID(event, id));
    args = BattleAlloc(0xE14);
    area = args;
    *(void**)((u8*)unit + 0x310) = area;
    return EVT_RETURN_DONE;
}

USER_FUNC(unk_muj_0000652c) {
    s32* args = event->args;
    s32 val = evtGetValue(event, args[0]);
    s32 arg1 = args[1];

    evtSetValue(event, arg1, psndSFX_get_vol(val));
    return EVT_RETURN_DONE;
}

USER_FUNC(_cortez_check_drain_audience) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    s32 arg1;
    BattleUnit* unit;
    CortezWork* area;
    s32 id;
    s32 i;

    id = evtGetValue(event, args[0]);
    arg1 = args[1];
    unit = BattleGetUnitPtr(work, BattleTransID(event, id));
    area = *(CortezWork**)((u8*)unit + 0x310);
    area->count = 0;
    for (i = 0; i < 100; i++) {
        s32 audienceId;
        area->audienceId[i] = -1;
        area->effects[i] = NULL;
        area->field_25C[i] = 0;
        audienceId = (i / 10) * 20 + 10 + i % 10;
        if ((u8)BattleAudience_GetExist(audienceId) != 0) {
            area->audienceId[i] = audienceId;
            area->count++;
        }
    }
    evtSetValue(event, arg1, area->count);
    return EVT_RETURN_DONE;
}

USER_FUNC(_cortez_collect_energy) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    CortezWork* area;
    s32 id;
    s32 i;
    s32 flag;
    s32 ret;
    f32 x, y, z;

    id = evtGetValue(event, args[0]);
    args = (s32*)BattleGetUnitPtr(work, BattleTransID(event, id));
    unit = (BattleUnit*)args;
    area = *(CortezWork**)((u8*)unit + 0x310);
    if (isFirstCall) {
        area->state = 0;
    }
    switch (area->state) {
    case 0:
        flag = 0;
        for (i = 0; i < 100; i++) {
            s32 audienceId = area->audienceId[i];
            if (audienceId != -1) {
                BattleAudienceMember* member = BattleAudienceGetPtr(audienceId);
                member->flags |= 0x12;
                BattleAudience_SetAnim(audienceId, 9, 1);
                area->timer[i] = irand(60) + 30;
                area->pos1[i] = member->position;
                BtlUnit_GetPos(unit, &area->pos2[i].x, &area->pos2[i].y, &area->pos2[i].z);
                area->pos2[i].y += 20.0f;
                effSpiritEntry(0, area->timer[i] + 60, area->pos1[i].x, area->pos1[i].y, area->pos1[i].z,
                               area->pos2[i].x, area->pos2[i].y, area->pos2[i].z, 2.0f);
                if (flag == 0) {
                    flag = 1;
                    effKumokumoN64Entry(1, 60, area->pos2[i].x, area->pos2[i].y, area->pos2[i].z, 180.0f, -1.0f,
                                        1.0f);
                }
            }
        }
        area->state++;
        // fallthrough
    case 1:
        ret = 1;
        for (i = 0; i < 100; i++) {
            s32 audienceId = area->audienceId[i];
            if (audienceId == -1) {
                continue;
            }
            switch (area->field_25C[i]) {
            case 0:
                if (area->timer[i] > 0) {
                    area->timer[i]--;
                    ret = 0;
                    break;
                }
                area->field_25C[i]++;
                area->field_194[i] = 0;
                // fallthrough
            case 1:
                area->field_194[i]++;
                BattleAudience_GetRotate(audienceId, &x, &y, &z);
                x = intplGetValue(0, area->field_194[i], 30, 0.0f, -90.0f);
                BattleAudience_SetRotate(audienceId, x, y, z);
                if (area->field_194[i] >= 30) {
                    area->field_25C[i]++;
                } else {
                    ret = 0;
                    break;
                }
                // fallthrough
            case 2:
                area->field_194[i] = 0;
                area->field_25C[i]++;
                // fallthrough
            case 3:
                area->field_194[i]++;
                if (area->field_194[i] >= 60) {
                    area->field_25C[i]++;
                }
                ret = 0;
                break;
            case 4:
            default:
                break;
            }
        }
        if (ret == 0) {
            break;
        }
        // fallthrough
    default:
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(_cortez_drain_energy) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    s32 arg1;
    BattleUnit* unit;
    CortezWork* area;
    s32 id;
    s32 i;

    id = evtGetValue(event, args[0]);
    arg1 = args[1];
    unit = BattleGetUnitPtr(work, BattleTransID(event, id));
    area = *(CortezWork**)((u8*)unit + 0x310);
    for (i = 0; i < 100; i++) {
        if (area->effects[i] != NULL) {
            effDelete(area->effects[i]);
            area->effects[i] = NULL;
        }
    }
    evtSetValue(event, arg1, area->count);
    return EVT_RETURN_DONE;
}

USER_FUNC(_cortez_recovery_audience) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    CortezWork* area;
    s32 id;
    s32 flag;
    s32 i;

    id = evtGetValue(event, args[0]);
    args = (s32*)BattleGetUnitPtr(work, BattleTransID(event, id));
    unit = (BattleUnit*)args;
    area = *(CortezWork**)((u8*)unit + 0x310);
    if (isFirstCall) {
        area->state = 0;
    }
    switch (area->state) {
    case 0:
        for (i = 0; i < 100; i++) {
            if (area->audienceId[i] != -1) {
                area->field_25C[i] = 0;
            }
        }
        area->state++;
        // fallthrough
    case 1:
        flag = 1;
        for (i = 0; i < 100; i++) {
            if (area->audienceId[i] != -1) {
                switch (area->field_25C[i]) {
                case 0:
                    area->field_25C[i]++;
                    area->field_194[i] = 0;
                    // fallthrough
                case 1:
                    area->field_194[i]++;
                    flag = 1;
                    break;
                }
            }
        }
        if (!flag) {
            break;
        }
        area->state++;
        // fallthrough
    case 2:
        for (i = 0; i < 100; i++) {
            flag = area->audienceId[i];
            if (flag != -1) {
                BattleAudience_Delete(area->audienceId[i]);
                BattleAudienceAddTargetNum(0.0f, -1.0f);
            }
        }
        area->state++;
        return EVT_RETURN_DONE;
    default:
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(_check_zenei_kouei) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 id;
    s32 arg1;
    f32 x, y, z;

    id = evtGetValue(event, args[0]);
    arg1 = args[1];
    unit = BattleGetUnitPtr(work, BattleTransID(event, id));
    BtlUnit_GetPos(unit, &x, &y, &z);
    if (x >= -120.0f) {
        evtSetValue(event, arg1, 0);
    } else {
        evtSetValue(event, arg1, 1);
    }
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(init_event)
    CALL(_make_cortez_work_area, -2)
    CALL(btlevtcmd_OnUnitFlag, -2, 16777216)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event_mode1))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetUnitWork, -2, 5, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 6, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 7, 1)
    CALL(btlevtcmd_SetUnitWork, -2, 9, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 10, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 11, 0)
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_claw), 0)
    CALL(btlevtcmd_SetUnitWork, -2, 3, LW(3))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 39)
    ADD(LW(1), 41)
    SUBTRACT(LW(2), 45)
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetUnitId, -2, LW(0))
    CALL(btlevtcmd_SetUnitWork, LW(3), 0, LW(0))
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_rapier), 0)
    CALL(btlevtcmd_SetUnitWork, -2, 4, LW(3))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 2)
    ADD(LW(1), 6)
    ADD(LW(2), 60)
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetUnitId, -2, LW(0))
    CALL(btlevtcmd_SetUnitWork, LW(3), 0, LW(0))
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_sword), 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, LW(3))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 154)
    ADD(LW(1), 11)
    SUBTRACT(LW(2), 65)
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetUnitId, -2, LW(0))
    CALL(btlevtcmd_SetUnitWork, LW(3), 0, LW(0))
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_saber), 0)
    CALL(btlevtcmd_SetUnitWork, -2, 2, LW(3))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 116)
    ADD(LW(1), 28)
    ADD(LW(2), 73)
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetUnitId, -2, LW(0))
    CALL(btlevtcmd_SetUnitWork, LW(3), 0, LW(0))
    CALL(btlevtcmd_SetKissHitOffset, -2, 160, 85, 50)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
    IF_INT_LESS_OR_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetDamagePartsId, -2, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 4)
            CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_mode_1))
            CALL_CHILD(btldefaultevt_Damage)
        ELSE
            CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_mode_1_hone))
            CALL_CHILD(btldefaultevt_Damage)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_GetDamagePartsId, -2, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL_CHILD(btldefaultevt_Damage)
        CASE_EQUAL(2)
        CALL(btlevtcmd_SetUnitWork, -2, 7, 1)
        CALL(btlevtcmd_SetUnitWork, -2, 11, 2)
        CALL_CHILD(btldefaultevt_Damage)
        CASE_EQUAL(4)
        CALL_CHILD(mode2_honeduka_damage_event)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(cortez_ac_counter_damage_event)
    CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
    IF_INT_LESS(LW(0), 4)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 39)
        WAIT_MS(500)
        RETURN
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_A3_1"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 50)
    ADD(LW(1), 10)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 30, 0, 2, 0, -1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_S_1"))
    CALL(btlevtcmd_CheckDamageCode, -2, 512, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckDamageCode, -2, 2048, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            RETURN
        END_IF
    END_IF
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetMoveFrame, -2, LW(0), LW(1), LW(2), FLOAT(6.0f), LW(15))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), LW(15), 15, 11, 0, -1)
    RETURN
EVT_END()

static EVT_BEGIN(avoid_thorn_event)
    CALL(btlevtcmd_GetDamagePartsId, -2, LW(0))
    IF_INT_EQUAL(LW(0), 4)
        CALL(btlevtcmd_GetHitPos, -2, 4, LW(0), LW(1), LW(2))
        CALL(evt_eff, STRING(""), STRING("toge_flush"), 0, LW(0), LW(1), LW(2), 60, 0, 0, 0, 0, 0, 0, 0)
    ELSE
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_RIB_COUNTER1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(1)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_1C_3"))
            CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
            CASE_EQUAL(2)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_2C_3"))
            CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
            CASE_EQUAL(3)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_3C_3"))
            CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        END_SWITCH
        CALL(btlevtcmd_SetUnitWork, -2, 11, 0)
        CALL_CHILD(cortez_reset_pose_table)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    CALL(btlevtcmd_CheckPhase, LW(0), 67108869)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_sub_random, 3, LW(0))
        CALL(btlevtcmd_SetUnitWork, -2, 8, LW(0))
    END_IF
    CALL(btlevtcmd_CheckPhase, LW(0), 67108865)
    IF_INT_EQUAL(LW(0), 0)
        GOTO(50)
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
    SWITCH(LW(0))
        CASE_OR_EQUAL(2)
        CASE_OR_EQUAL(3)
        CASE_END
        CASE_DEFAULT
        GOTO(50)
    END_SWITCH
    CALL(btlevtcmd_GetUnitWork, -2, 7, LW(0))
    CALL(btlevtcmd_SetUnitWork, -2, 7, 0)
    IF_INT_NOT_EQUAL(LW(0), 0)
        GOTO(50)
    END_IF
    CALL(btlevtcmd_StopWaitEvent, -2)
    CALL(btlevtcmd_GetUnitWork, -2, 11, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(mode2_rib_close_event)
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        SET_VALUE(LW(15), -1)
        SWITCH(LW(0))
            CASE_EQUAL(2)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_2B_1"))
            CALL(btlevtcmd_ftomsec, 80, LW(0))
            WAIT_MS(LW(0))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_MOVE2"), EVTDAT_POINTER_MAX, 0, LW(15))
            CASE_EQUAL(3)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_3B_1"))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_MOVE2"), EVTDAT_POINTER_MAX, 0, LW(15))
        END_SWITCH
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        IF_INT_NOT_EQUAL(LW(15), -1)
            CALL(evt_snd_sfxoff, LW(15))
        END_IF
    END_SIBLING_EVENT
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    CALL(btlevtcmd_SetUnitWork, -2, 5, 1)
    CALL_CHILD(cortez_reset_pose_table)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
    LABEL(50)
    CALL(btlevtcmd_CheckPhase, LW(0), 67108865)
    IF_INT_EQUAL(LW(0), 0)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 11, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(99)
    END_IF
    SUBTRACT(LW(0), 1)
    CALL(btlevtcmd_SetUnitWork, -2, 11, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_StopWaitEvent, -2)
    CALL_CHILD(mode2_rib_close_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
    LABEL(99)
    RETURN
EVT_END()

EVT_BEGIN(attack_event_mode1)
    CALL(btlevtcmd_GetUnitWork, -2, 6, LW(0))
    IF_INT_LESS(LW(0), 3)
        SET_VALUE(LW(1), LW(0))
        ADD(LW(1), 1)
        IF_INT_GREATER_OR_EQUAL(LW(1), 3)
            SET_VALUE(LW(1), 0)
        END_IF
        CALL(btlevtcmd_SetUnitWork, -2, 6, LW(1))
    END_IF
    SWITCH(LW(0))
        CASE_OR_EQUAL(0)
        CASE_OR_EQUAL(1)
        CALL_CHILD(at1_normal_attack)
        CASE_END
        CASE_EQUAL(2)
        CALL_CHILD(at1_midaregiri_attack)
        CASE_DEFAULT
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(attack_event_mode2)
    CALL(btlevtcmd_GetUnitWork, -2, 10, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(at2_hone_rain_attack_event)
        RETURN
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
    SWITCH(LW(0))
        CASE_OR_EQUAL(2)
        CASE_OR_EQUAL(3)
        CALL_CHILD(at2_hone_charge_event)
        RETURN
        CASE_END
    END_SWITCH
    CALL(btlevtcmd_DrawLots, LW(0), 2, 25, 75)
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL_CHILD(at2_hone_charge_event)
        CASE_EQUAL(1)
        CALL_CHILD(at2_head_bat_event)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(at2_head_bat_event)
    SET_VALUE(LW(12), PTR(&weapon_cortez_mode2_head_attack))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(12), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(12))
    CALL(_check_zenei_kouei, LW(3), LW(13))
    IF_INT_EQUAL(LW(13), 0)
        CALL(btlevtcmd_GetUnitWork, -2, 11, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_1A_1"))
        ELSE
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_1A_3"))
        END_IF
    ELSE
        CALL(btlevtcmd_GetUnitWork, -2, 11, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_1A_2"))
        ELSE
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_1A_4"))
        END_IF
    END_IF
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_HEAD_ATTACK1"), EVTDAT_POINTER_MAX, 0, LW(15))
        WAIT_MS(667)
        CALL(evt_snd_sfxoff, LW(15))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_ftomsec, 50, LW(0))
        WAIT_MS(LW(0))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_HEAD_ATTACK2"), EVTDAT_POINTER_MAX, 0, LW(15))
        CALL(btlevtcmd_ftomsec, 18, LW(0))
        WAIT_MS(LW(0))
        CALL(evt_snd_sfxoff, LW(15))
    END_INLINE_EVENT
    WAIT_MS(1116)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    SWITCH(LW(5))
        CASE_OR_EQUAL(4)
        GOTO(90)
        CASE_END
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(3), 38)
        GOTO(90)
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(3), 39)
        GOTO(90)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(3), 40)
        GOTO(90)
        CASE_DEFAULT
        GOTO(91)
    END_SWITCH
    LABEL(90)
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    GOTO(99)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(12))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(at2_hone_charge_event)
    CALL(btlevtcmd_SetUnitWork, -2, 10, 1)
    CALL(btlevtcmd_GetUnitWork, -2, 11, LW(1))
    CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL(evt_btl_camera_set_mode, 0, 3)
        CALL(evt_btl_camera_set_moveto, 0, 50, 140, 520, 50, 90, -260, 60, 4)
        IF_INT_EQUAL(LW(1), 0)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_2A_1"))
        ELSE
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_2A1_1"))
        END_IF
        CASE_EQUAL(2)
        CALL(evt_btl_camera_set_mode, 0, 3)
        CALL(evt_btl_camera_set_moveto, 0, 50, 140, 520, 50, 90, -260, 60, 4)
        IF_INT_EQUAL(LW(1), 0)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_2A_2"))
        ELSE
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_2A2_2"))
        END_IF
        CASE_EQUAL(3)
        CALL(evt_btl_camera_set_mode, 0, 3)
        CALL(evt_btl_camera_set_moveto, 0, 50, 140, 520, 50, 90, -260, 60, 4)
        IF_INT_EQUAL(LW(1), 0)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_2A_3"))
        ELSE
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_2A3_3"))
        END_IF
    END_SWITCH
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_INHALE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_MS(2000)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL_CHILD(cortez_reset_pose_table)
    CALL(btlevtcmd_CheckDamage, -2, -2, 1, PTR(&weapon_cortez_mode2_charge), 256, LW(5))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(at2_hone_rain_attack_event)
    CALL(btlevtcmd_SetUnitWork, -2, 10, 0)
    SET_VALUE(LW(12), PTR(&weapon_cortez_mode2_bone_attack))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(12))
    CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_3A_1"))
        CASE_EQUAL(2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_3A_2"))
        CASE_EQUAL(3)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_3A_3"))
    END_SWITCH
    WAIT_MS(1700)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    END_SIBLING_EVENT
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_BONE_FLY1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_OffPartsAttribute, -2, 5, 16777216)
    CALL(btlevtcmd_AnimeChangePose, -2, 5, STRING("KRT_A_1"))
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 5)
        WAIT_MS(500)
        CALL(btlevtcmd_OnPartsAttribute, -2, 5, 16777216)
    END_INLINE_EVENT
    SET_VALUE(LW(5), 12)
    SPAWN_EVENT(_bone_rain_event)
    SET_VALUE(LW(5), 13)
    SPAWN_EVENT(_bone_rain_event)
    SET_VALUE(LW(5), 14)
    SPAWN_EVENT(_bone_rain_event)
    SET_VALUE(LW(5), 15)
    SPAWN_EVENT(_bone_rain_event)
    SET_VALUE(LW(5), 16)
    SPAWN_EVENT(_bone_rain_event)
    SET_VALUE(LW(5), 17)
    SPAWN_EVENT(_bone_rain_event)
    SET_VALUE(LW(5), 18)
    SPAWN_EVENT(_bone_rain_event)
    SET_VALUE(LW(5), 19)
    SPAWN_EVENT(_bone_rain_event)
    BEGIN_INLINE_EVENT_GET_ID(LW(15))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetPartsPos, -2, 6, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_OffPartsAttribute, -2, 6, 16777216)
        CALL(btlevtcmd_SetPartsFallAccel, -2, 6, FLOAT(0.9f))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_JumpPartsPosition, -2, 6, LW(0), LW(1), LW(2), 30, -1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_SetPartsRotateOffset, -2, 6, 0, 0, 0)
        LOOP(30)
            CALL(btlevtcmd_AddPartsRotate, -2, 6, 0, 0, 30)
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        BEGIN_INLINE_EVENT
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetPartsPos, -2, 9, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_OffPartsAttribute, -2, 9, 16777216)
            CALL(btlevtcmd_SetPartsFallAccel, -2, 9, FLOAT(0.9f))
            CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
            CALL(btlevtcmd_JumpPartsPosition, -2, 9, LW(0), LW(1), LW(2), 30, -1)
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            CALL(btlevtcmd_SetPartsRotateOffset, -2, 9, 0, 0, 0)
            LOOP(30)
                CALL(btlevtcmd_AddPartsRotate, -2, 9, 0, 0, 30)
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
    END_IF
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    SET_VALUE(LW(10), 0)
    LABEL(10)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    SWITCH(LW(5))
        CASE_OR_EQUAL(4)
        GOTO(90)
        CASE_END
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(3), 38)
        GOTO(90)
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(3), 39)
        GOTO(90)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(3), 40)
        GOTO(90)
        CASE_EQUAL(1)
        GOTO(91)
        CASE_DEFAULT
        GOTO(90)
    END_SWITCH
    LABEL(90)
    IF_INT_EQUAL(LW(10), 0)
        BEGIN_INLINE_EVENT
            CALL(btlevtcmd_JumpPartsContinue, -2, 6)
            CALL(btlevtcmd_GetPartsPos, -2, 6, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 40)
            CALL(btlevtcmd_SetPartsFallAccel, -2, 6, FLOAT(0.7f))
            CALL(btlevtcmd_JumpPartsPosition, -2, 6, LW(0), LW(1), LW(2), 24, -1)
            CALL(btlevtcmd_OnPartsAttribute, -2, 6, 16777216)
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            LOOP(24)
                CALL(btlevtcmd_AddPartsRotate, -2, 6, 0, 0, 30)
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
    ELSE
        BEGIN_INLINE_EVENT
            CALL(btlevtcmd_JumpPartsContinue, -2, 9)
            CALL(btlevtcmd_GetPartsPos, -2, 9, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 40)
            CALL(btlevtcmd_SetPartsFallAccel, -2, 9, FLOAT(0.7f))
            CALL(btlevtcmd_JumpPartsPosition, -2, 9, LW(0), LW(1), LW(2), 24, -1)
            CALL(btlevtcmd_OnPartsAttribute, -2, 9, 16777216)
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            LOOP(24)
                CALL(btlevtcmd_AddPartsRotate, -2, 9, 0, 0, 30)
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
    END_IF
    GOTO(97)
    LABEL(91)
    IF_INT_EQUAL(LW(10), 0)
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(12))
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    IF_INT_EQUAL(LW(10), 0)
        BEGIN_INLINE_EVENT
            CALL(btlevtcmd_GetPartsPos, -2, 6, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 80)
            CALL(btlevtcmd_SetPartsFallAccel, -2, 6, FLOAT(0.6f))
            CALL(btlevtcmd_JumpPartsPosition, -2, 6, LW(0), LW(1), LW(2), 26, -1)
            CALL(btlevtcmd_OnPartsAttribute, -2, 6, 16777216)
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            LOOP(26)
                CALL(btlevtcmd_AddPartsRotate, -2, 6, 0, 0, 30)
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
    ELSE
        BEGIN_INLINE_EVENT
            CALL(btlevtcmd_GetPartsPos, -2, 9, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 80)
            CALL(btlevtcmd_SetPartsFallAccel, -2, 9, FLOAT(0.6f))
            CALL(btlevtcmd_JumpPartsPosition, -2, 9, LW(0), LW(1), LW(2), 26, -1)
            CALL(btlevtcmd_OnPartsAttribute, -2, 9, 16777216)
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            LOOP(26)
                CALL(btlevtcmd_AddPartsRotate, -2, 9, 0, 0, 30)
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
    END_IF
    LABEL(97)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        WAIT_MS(60)
        ADD(LW(10), 1)
        GOTO(10)
    END_IF
    WAIT_FRAMES(84)
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(_bone_rain_event)
    CALL(evt_sub_random, 20, LW(10))
    ADD(LW(10), 20)
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(evt_sub_random, 90, LW(7))
        SUBTRACT(LW(7), 45)
        ADD(LW(0), LW(7))
        CALL(evt_sub_random, 60, LW(7))
        SUBTRACT(LW(7), 30)
        ADD(LW(2), LW(7))
        CALL(btlevtcmd_SetPartsPos, -2, LW(5), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_OffPartsAttribute, -2, LW(5), 16777216)
        CALL(btlevtcmd_SetPartsFallAccel, -2, LW(5), FLOAT(1.4f))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(evt_sub_random, 120, LW(7))
        SUBTRACT(LW(7), 60)
        ADD(LW(0), LW(7))
        CALL(evt_sub_random, 60, LW(7))
        SUBTRACT(LW(7), 30)
        ADD(LW(2), LW(7))
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(5), LW(0), LW(1), LW(2), LW(10), -1)
        CALL(btlevtcmd_OnPartsAttribute, -2, LW(5), 16777216)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(LW(10))
            CALL(btlevtcmd_AddPartsRotate, -2, LW(5), 0, 0, 30)
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    RETURN
EVT_END()

EVT_BEGIN(attack_event_mode3)
    CALL(btlevtcmd_GetUnitWork, -2, 9, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetMaxHp, -2, LW(1))
        DIVIDE(LW(1), 2)
        CALL(btlevtcmd_GetHp, -2, LW(0))
        IF_INT_LESS_OR_EQUAL(LW(0), LW(1))
            CALL(_cortez_check_drain_audience, -2, LW(0))
            IF_INT_GREATER_OR_EQUAL(LW(0), 1)
                CALL(btlevtcmd_GetUnitWork, -2, 9, LW(0))
                ADD(LW(0), 1)
                CALL(btlevtcmd_SetUnitWork, -2, 9, LW(0))
                CALL_CHILD(attack_audience_event)
                RETURN
            END_IF
        END_IF
    END_IF
    CALL_CHILD(at3_kamitsuki_attack)
    RETURN
EVT_END()

static EVT_BEGIN(attack_audience_event)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, 140, 120, 500, 140, 85, 9, 60, 11)
    WAIT_FRAMES(60)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(btlevtcmd_SetTalkPose, -2, STRING("KRT_T_1"))
    CALL(btlevtcmd_SetStayPose, -2, STRING("KRT_S_1"))
    CALL(evt_msg_print, 2, STRING("stg5_dou_24_bt01"), 0, -2)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, 80, 115, 820, 80, 60, 6, 60, 11)
    CALL(evt_batstage_set_aud_dark, 0, 60, 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), 120, 60, -10, 4, 0, -1)
    CALL(evt_msg_print, 2, STRING("stg5_dou_24_bt02"), 0, -2)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_A2_1"))
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_INHALE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        CALL(evt_snd_sfxon, STRING("SFX_AUDIENCE_PANIC_SCREAM1"), LW(15))
        WAIT_FRAMES(120)
        CALL(unk_muj_0000652c, LW(15), LW(14))
        SET_VALUE(LW(13), 120)
        LOOP(120)
            WAIT_FRAMES(1)
            IF_INT_GREATER(LW(13), 0)
                SUBTRACT(LW(13), 1)
            END_IF
            SET_VALUE(LW(0), LW(14))
            MULTIPLY(LW(0), LW(13))
            DIVIDE(LW(0), 120)
            CALL(evt_snd_sfx_vol, LW(15), LW(0))
            DEBUG_PUT_REG(LW(14))
            DEBUG_PUT_REG(LW(0))
        END_LOOP
        CALL(evt_snd_sfxoff, LW(15))
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_A2_2"))
    CALL(_cortez_collect_energy, -2)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_A2_3"))
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_S_1"))
    END_SIBLING_EVENT
    WAIT_MS(125)
    CALL(_cortez_drain_energy, -2, LW(5))
    CALL(btlevtcmd_RecoverHp, -2, 1, LW(5))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 100)
    ADD(LW(2), 10)
    CALL(evt_eff, 0, STRING("recovery"), 0, LW(0), LW(1), LW(2), LW(5), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1500)
    CALL(evt_btl_camera_set_mode, 0, 0)
    BEGIN_INLINE_EVENT_GET_ID(LW(15))
        CALL(_cortez_recovery_audience, -2)
    END_INLINE_EVENT
    CALL(evt_batstage_return_aud_dark_base, 60, 0)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 60, -10, 4, 0, -1)
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(at3_kamitsuki_attack)
    SET_VALUE(LW(12), PTR(&weapon_cortez_mode3_kamitsuki_attack))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(12), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(12))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_W_1"))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 40)
    CALL(btlevtcmd_GetMoveFrame, -2, LW(0), LW(1), LW(2), FLOAT(6.0f), LW(15))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), LW(15), -12, 5, 0, -1)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_ROUND1"), EVTDAT_POINTER_MAX, 0, LW(15))
        CALL(btlevtcmd_ftomsec, 32, LW(0))
        WAIT_MS(LW(0))
        CALL(evt_snd_sfxoff, LW(15))
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_ftomsec, 46, LW(0))
        WAIT_MS(LW(0))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_BITE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_A3_3"))
    CALL(btlevtcmd_ftomsec, 50, LW(0))
    WAIT_MS(LW(0))
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    SWITCH(LW(5))
        CASE_OR_EQUAL(4)
        GOTO(90)
        CASE_END
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(3), 38)
        GOTO(90)
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(3), 39)
        GOTO(90)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(3), 40)
        GOTO(90)
        CASE_DEFAULT
        GOTO(91)
    END_SWITCH
    LABEL(90)
    GOTO(97)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(12))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    IF_INT_EQUAL(LW(5), 21)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_A3_1"))
        GOTO(98)
    END_IF
    LABEL(97)
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_Z_1"))
    LABEL(98)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 50)
    ADD(LW(1), 10)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 30, -10, 2, 0, -1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_S_1"))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetMoveFrame, -2, LW(0), LW(1), LW(2), FLOAT(6.0f), LW(15))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), LW(15), 15, 11, 0, -1)
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(at1_normal_attack)
    IF_INT_EQUAL(LW(0), 0)
        SET_VALUE(LW(12), PTR(&weapon_cortez_normal_front_attack))
    ELSE
        SET_VALUE(LW(12), PTR(&weapon_cortez_normal_rear_attack))
    END_IF
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(12), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(12))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_SWORD_READY1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(_check_zenei_kouei, LW(3), LW(13))
    IF_INT_EQUAL(LW(13), 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_A1_1"))
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_ftomsec, 95, LW(0))
            WAIT_MS(LW(0))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_SWORD_STICK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        END_SIBLING_EVENT
        CALL(btlevtcmd_ftomsec, 99, LW(0))
        WAIT_MS(LW(0))
    ELSE
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_A1_2"))
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_ftomsec, 95, LW(0))
            WAIT_MS(LW(0))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_SWORD_STICK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        END_SIBLING_EVENT
        CALL(btlevtcmd_ftomsec, 100, LW(0))
        WAIT_MS(LW(0))
    END_IF
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    SWITCH(LW(5))
        CASE_OR_EQUAL(4)
        GOTO(90)
        CASE_END
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(3), 38)
        GOTO(90)
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(3), 39)
        GOTO(90)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(3), 40)
        GOTO(90)
        CASE_DEFAULT
        GOTO(91)
    END_SWITCH
    LABEL(90)
    GOTO(97)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(12))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    LABEL(97)
    WAIT_FRAMES(3)
    CALL(evt_btl_camera_shake_h, 0, 2, 0, 10, 13)
    WAIT_FRAMES(7)
    CALL(evt_btl_camera_shake_h, 0, 8, 0, 30, 13)
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(at1_midaregiri_attack)
    SET_VALUE(LW(12), PTR(&weapon_cortez_midaregiri_attack))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(12), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(12))
    CALL(_check_zenei_kouei, LW(3), LW(13))
    IF_INT_EQUAL(LW(13), 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_A2_1"))
    ELSE
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_A2_2"))
    END_IF
    SET_VALUE(LW(0), 0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_ftomsec, 33, LW(0))
        WAIT_MS(LW(0))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_SWORD_SHAKE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_ftomsec, 84, LW(0))
        WAIT_MS(LW(0))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_SWORD_SHAKE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_ftomsec, 145, LW(0))
        WAIT_MS(LW(0))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_SWORD_SHAKE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_ftomsec, 198, LW(0))
        WAIT_MS(LW(0))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_SWORD_SHAKE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SIBLING_EVENT
    LABEL(10)
    SWITCH(LW(10))
        CASE_EQUAL(0)
        CALL(btlevtcmd_ftomsec, 38, LW(0))
        WAIT_MS(LW(0))
        CASE_EQUAL(1)
        CALL(btlevtcmd_ftomsec, 60, LW(0))
        WAIT_MS(LW(0))
        CASE_EQUAL(2)
        CALL(btlevtcmd_ftomsec, 54, LW(0))
        WAIT_MS(LW(0))
        CASE_DEFAULT
        CALL(btlevtcmd_ftomsec, 56, LW(0))
        WAIT_MS(LW(0))
    END_SWITCH
    SWITCH(LW(5))
        CASE_OR_EQUAL(4)
        GOTO(90)
        CASE_END
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(3), 38)
        GOTO(90)
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(3), 39)
        GOTO(90)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(3), 40)
        GOTO(90)
        CASE_DEFAULT
        GOTO(91)
    END_SWITCH
    LABEL(90)
    GOTO(97)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(12))
    IF_INT_GREATER_OR_EQUAL(LW(10), 3)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    ELSE
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 0, LW(5))
    END_IF
    LABEL(97)
    ADD(LW(10), 1)
    IF_INT_LESS_OR_EQUAL(LW(10), 3)
        GOTO(10)
    END_IF
    LABEL(99)
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

BattleUnitSetup entry_claw[1] = {
    { &unit_boss_cortez_claw, 1, 0x04000004, { 1000.0f, 0.0f, 1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleUnitSetup entry_rapier[1] = {
    { &unit_boss_cortez_rapier, 1, 0x04000004, { 1000.0f, 0.0f, 1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleUnitSetup entry_sword[1] = {
    { &unit_boss_cortez_sword, 1, 0x04000004, { 1000.0f, 0.0f, 1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleUnitSetup entry_saber[1] = {
    { &unit_boss_cortez_saber, 1, 0x04000004, { 1000.0f, 0.0f, 1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

EVT_BEGIN(cortez_reset_pose_table)
    CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_mode_1))
        CASE_EQUAL(1)
        CALL(btlevtcmd_SetKissHitOffset, -2, 160, 120, 50)
        CALL(btlevtcmd_GetUnitWork, -2, 11, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetUnitWork, -2, 10, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_mode_2_upper_normal))
            ELSE
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_mode_2_upper_charge))
            END_IF
            CALL(btlevtcmd_SetPartsDefenceTable, -2, 2, PTR(&defence_mode2_upper_rib_close))
            CALL(btlevtcmd_SetPartsDefenceAttrTable, -2, 2, PTR(&defence_attr_mode2_upper_rib_close))
        ELSE
            CALL(btlevtcmd_GetUnitWork, -2, 10, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_mode_2_upper_normal_counter))
            ELSE
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_mode_2_upper_charge_counter))
            END_IF
            CALL(btlevtcmd_SetPartsDefenceTable, -2, 2, PTR(&defence_mode2_upper_rib_open))
            CALL(btlevtcmd_SetPartsDefenceAttrTable, -2, 2, PTR(&defence_attr_mode2_upper_rib_open))
        END_IF
        CALL(btlevtcmd_SetPartsDefenceTable, -2, 1, PTR(&defence_mode2_upper_head))
        CALL(btlevtcmd_SetPartsDefenceAttrTable, -2, 1, PTR(&defence_attr_mode2_upper_head))
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 6291456)
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 7340032)
        CALL(btlevtcmd_SetHitOffset, -2, 1, 20, 15, 0)
        CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 20, 15, 0)
        CALL(btlevtcmd_SetHitOffset, -2, 2, 0, 0, 0)
        CALL(btlevtcmd_SetHitCursorOffset, -2, 2, 0, 0, 0)
        CASE_EQUAL(2)
        CALL(btlevtcmd_SetKissHitOffset, -2, 160, 85, 50)
        CALL(btlevtcmd_GetUnitWork, -2, 11, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetUnitWork, -2, 10, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_mode_2_middle_normal))
            ELSE
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_mode_2_middle_charge))
            END_IF
            CALL(btlevtcmd_SetPartsDefenceTable, -2, 2, PTR(&defence_mode2_middle_rib_close))
            CALL(btlevtcmd_SetPartsDefenceAttrTable, -2, 2, PTR(&defence_attr_mode2_middle_rib_close))
        ELSE
            CALL(btlevtcmd_GetUnitWork, -2, 10, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_mode_2_middle_normal_counter))
            ELSE
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_mode_2_middle_charge_counter))
            END_IF
            CALL(btlevtcmd_SetPartsDefenceTable, -2, 2, PTR(&defence_mode2_middle_rib_open))
            CALL(btlevtcmd_SetPartsDefenceAttrTable, -2, 2, PTR(&defence_attr_mode2_middle_rib_open))
        END_IF
        CALL(btlevtcmd_SetPartsDefenceTable, -2, 1, PTR(&defence_mode2_middle_head))
        CALL(btlevtcmd_SetPartsDefenceAttrTable, -2, 1, PTR(&defence_attr_mode2_middle_head))
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 6291456)
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 7340032)
        CALL(btlevtcmd_SetHitOffset, -2, 1, 10, 10, 0)
        CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 10, 10, 0)
        CALL(btlevtcmd_SetHitOffset, -2, 2, -7, -20, 0)
        CALL(btlevtcmd_SetHitCursorOffset, -2, 2, -7, -20, 0)
        CASE_EQUAL(3)
        CALL(btlevtcmd_SetKissHitOffset, -2, 150, 85, 50)
        CALL(btlevtcmd_GetUnitWork, -2, 11, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetUnitWork, -2, 10, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_mode_2_lower_normal))
            ELSE
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_mode_2_lower_charge))
            END_IF
            CALL(btlevtcmd_SetPartsDefenceTable, -2, 2, PTR(&defence_mode2_lower_rib_close))
            CALL(btlevtcmd_SetPartsDefenceAttrTable, -2, 2, PTR(&defence_attr_mode2_lower_rib_close))
        ELSE
            CALL(btlevtcmd_GetUnitWork, -2, 10, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_mode_2_lower_normal_counter))
            ELSE
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_mode_2_lower_charge_counter))
            END_IF
            CALL(btlevtcmd_SetPartsDefenceTable, -2, 2, PTR(&defence_mode2_lower_rib_open))
            CALL(btlevtcmd_SetPartsDefenceAttrTable, -2, 2, PTR(&defence_attr_mode2_lower_rib_open))
            CALL(btlevtcmd_OnPartsAttribute, -2, 2, 6291456)
        END_IF
        CALL(btlevtcmd_SetPartsDefenceTable, -2, 1, PTR(&defence_mode2_lower_head))
        CALL(btlevtcmd_SetPartsDefenceAttrTable, -2, 1, PTR(&defence_attr_mode2_lower_head))
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 6291456)
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 6291456)
        CALL(btlevtcmd_OffPartsAttribute, -2, 2, 1048576)
        CALL(btlevtcmd_SetHitOffset, -2, 1, -15, -5, 0)
        CALL(btlevtcmd_SetHitCursorOffset, -2, 1, -15, -5, 0)
        CALL(btlevtcmd_SetHitOffset, -2, 2, -20, -35, 0)
        CALL(btlevtcmd_SetHitCursorOffset, -2, 2, -20, -35, 0)
        CASE_DEFAULT
        CALL(btlevtcmd_SetKissHitOffset, -2, 30, 50, 50)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_mode_3))
        CALL(btlevtcmd_SetPartsDefenceTable, -2, 1, PTR(&defence_mode3))
        CALL(btlevtcmd_SetPartsDefenceAttrTable, -2, 1, PTR(&defence_attr_mode3))
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 6291456)
        CALL(btlevtcmd_OnAttribute, -2, 4)
        CALL(btlevtcmd_SetHitOffset, -2, 1, -135, -64, 0)
        CALL(btlevtcmd_SetHitCursorOffset, -2, 1, -135, -65, 0)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(cortez_damage_sub_event)
    CALL_CHILD(cortez_reset_pose_table)
    CALL(btlevtcmd_GetDamage, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetDamagePartsId, -2, LW(0))
        CALL(btlevtcmd_GetHitPos, -2, LW(0), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_BTL_ATTACK_MISS2"), LW(0), LW(1), LW(2), 0)
        GOTO(50)
    END_IF
    CALL(btlevtcmd_GetDamagePartsId, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 2)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 39)
    ELSE
        CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(1)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_1D_3"))
            CASE_EQUAL(2)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_2D_3"))
            CASE_EQUAL(3)
            CALL(btlevtcmd_GetUnitWork, -2, 11, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_3D_3"))
            ELSE
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_3D3_1"))
            END_IF
        END_SWITCH
    END_IF
    WAIT_FRAMES(30)
    GOTO(60)
    LABEL(50)
    LABEL(60)
    CALL(btlevtcmd_GetDamagePartsId, -2, LW(0))
    IF_INT_EQUAL(LW(0), 2)
        CALL(btlevtcmd_CheckDamageCode, -2, 256, LW(1))
        IF_INT_NOT_EQUAL(LW(1), 0)
            CALL(btlevtcmd_CheckDamageCode, -2, 1024, LW(1))
            IF_INT_EQUAL(LW(1), 0)
                CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
                SWITCH(LW(0))
                    CASE_OR_EQUAL(1)
                    CASE_OR_EQUAL(2)
                    CASE_OR_EQUAL(3)
                    CALL(btlevtcmd_GetUnitWork, -2, 11, LW(0))
                    IF_INT_EQUAL(LW(0), 0)
                        CALL_CHILD(mode2_rib_open_event)
                    END_IF
                    CASE_END
                END_SWITCH
            END_IF
        END_IF
    END_IF
END_IF
LABEL(99)
RETURN
EVT_END()

static EVT_BEGIN(dead_event)
    CALL(btlevtcmd_ClearAllStatus, -2)
    CALL(btlevtcmd_WaitAttackEnd)
    CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL(btlevtcmd_SetUnitWork, -2, 5, 1)
        CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event_mode2))
        CALL(btlevtcmd_RecoverHp, -2, 1, 999)
        CALL(btlevtcmd_OffPartsAttribute, -2, 4, 65536)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_D2_1"))
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_ftomsec, 125, LW(0))
            WAIT_MS(LW(0))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_SWORD_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_ftomsec, 120, LW(0))
            WAIT_MS(LW(0))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_SWORD_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_ftomsec, 118, LW(0))
            WAIT_MS(LW(0))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_SWORD_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_ftomsec, 117, LW(0))
            WAIT_MS(LW(0))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_SWORD_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            WAIT_MS(2000)
            CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_Z_1"))
            CALL(btlevtcmd_OffPartsAttribute, -2, 3, 16777216)
        END_SIBLING_EVENT
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_D2_2"))
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_ftomsec, 33, LW(0))
            WAIT_MS(LW(0))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_MOVE2"), EVTDAT_POINTER_MAX, 0, LW(7))
            CALL(btlevtcmd_ftomsec, 36, LW(0))
            WAIT_MS(LW(0))
            CALL(evt_snd_sfxoff, LW(7))
        END_SIBLING_EVENT
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        CALL(btlevtcmd_GetMaxHp, -2, LW(0))
        CALL(btlevtcmd_SetHp, -2, LW(0))
        CALL(evt_fade_entry, 24, 1000, 0, 0, 0)
        CALL(evt_fade_set_anim_ofs_pos, 0, 130)
        CALL(evt_fade_set_anim_virtual_pos, 0, 0, 80)
        CALL(evt_fade_end_wait)
        CALL(btlevtcmd_AnimeChangeGroup, -2, 1, STRING("c_korutesu2"))
        CALL_CHILD(cortez_reset_pose_table)
        CALL(btlevtcmd_AnimeSetPoseSoundTable, -2, 1, PTR(pose_sound_timing_table))
        CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
        CALL(evt_fade_entry, 25, 1000, 0, 0, 0)
        CALL(evt_fade_set_anim_ofs_pos, 0, 130)
        CALL(evt_fade_set_anim_virtual_pos, 0, 0, 80)
        CALL(evt_fade_end_wait)
        CALL(btlevtcmd_SetCutBaseOffset, -2, -80, 120, 0)
        CALL(btlevtcmd_SetCutWidth, -2, 200)
        CALL(btlevtcmd_SetCutHeight, -2, 100)
        CALL(btlevtcmd_reset_turn)
        CASE_OR_EQUAL(1)
        CASE_OR_EQUAL(2)
        CASE_OR_EQUAL(3)
        CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event_mode3))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_BONE_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(1)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_1B_1"))
            CASE_EQUAL(2)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_2B_1"))
            CALL(btlevtcmd_ftomsec, 80, LW(0))
            WAIT_MS(LW(0))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_MOVE2"), EVTDAT_POINTER_MAX, 0, LW(15))
            CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
            CALL(evt_snd_sfxoff, LW(15))
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_1B_1"))
            CASE_EQUAL(3)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_MOVE2"), EVTDAT_POINTER_MAX, 0, LW(15))
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_3B_1"))
            CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
            CALL(evt_snd_sfxoff, LW(15))
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_1B_1"))
        END_SWITCH
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_BONE_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_ftomsec, 7, LW(0))
            WAIT_MS(LW(0))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_BONE_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_ftomsec, 8, LW(0))
            WAIT_MS(LW(0))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_BONE_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_ftomsec, 4, LW(0))
            WAIT_MS(LW(0))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_BONE_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_ftomsec, 6, LW(0))
            WAIT_MS(LW(0))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_BONE_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_ftomsec, 7, LW(0))
            WAIT_MS(LW(0))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_BONE_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_ftomsec, 8, LW(0))
            WAIT_MS(LW(0))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_BONE_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_ftomsec, 8, LW(0))
            WAIT_MS(LW(0))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_BONE_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_ftomsec, 5, LW(0))
            WAIT_MS(LW(0))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_BONE_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_ftomsec, 7, LW(0))
            WAIT_MS(LW(0))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_BONE_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_ftomsec, 5, LW(0))
            WAIT_MS(LW(0))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_BONE_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_ftomsec, 7, LW(0))
            WAIT_MS(LW(0))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_BONE_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_ftomsec, 14, LW(0))
            WAIT_MS(LW(0))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_BONE_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_ftomsec, 30, LW(0))
            WAIT_MS(LW(0))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_BONE_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        END_SIBLING_EVENT
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        CALL(btlevtcmd_SetUnitWork, -2, 5, 4)
        WAIT_MS(1000)
        CALL(btlevtcmd_AnimeSetPoseSoundTable, -2, 1, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_A_1"))
        CALL(btlevtcmd_OffPartsAttribute, -2, 3, 16777216)
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 3)
        CALL(btlevtcmd_GetUnitWork, -2, 1, LW(3))
        CALL(btlevtcmd_AnimeChangePoseFromTable, LW(3), 1)
        CALL(btlevtcmd_OffPartsAttribute, LW(3), 1, 50397184)
        CALL(btlevtcmd_OffAttribute, LW(3), 33554432)
        CALL(btlevtcmd_SetMaxMoveCount, LW(3), 1)
        CALL(btlevtcmd_GetUnitWork, -2, 2, LW(3))
        CALL(btlevtcmd_AnimeChangePoseFromTable, LW(3), 1)
        CALL(btlevtcmd_OffPartsAttribute, LW(3), 1, 50397184)
        CALL(btlevtcmd_OffAttribute, LW(3), 33554432)
        CALL(btlevtcmd_SetMaxMoveCount, LW(3), 1)
        CALL(btlevtcmd_GetUnitWork, -2, 4, LW(3))
        CALL(btlevtcmd_AnimeChangePoseFromTable, LW(3), 1)
        CALL(btlevtcmd_OffPartsAttribute, LW(3), 1, 50397184)
        CALL(btlevtcmd_OffAttribute, LW(3), 33554432)
        CALL(btlevtcmd_SetMaxMoveCount, LW(3), 1)
        CALL(btlevtcmd_GetUnitWork, -2, 3, LW(3))
        CALL(btlevtcmd_AnimeChangePoseFromTable, LW(3), 1)
        CALL(btlevtcmd_OffPartsAttribute, LW(3), 1, 50397184)
        CALL(btlevtcmd_OffAttribute, LW(3), 33554432)
        CALL(btlevtcmd_SetMaxMoveCount, LW(3), 1)
        CALL(btlevtcmd_OnPartsAttribute, -2, 3, 50331648)
        CALL(btlevtcmd_OnPartsAttribute, -2, 4, 50397184)
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 65536)
        SET_VALUE(LW(11), 1)
        LOOP(4)
            CALL(btlevtcmd_GetUnitWork, -2, LW(11), LW(9))
            BEGIN_INLINE_EVENT
                CALL_CHILD(cortez_weapon_get_home_height)
                CALL(btlevtcmd_GetUnitWork, LW(9), 2, LW(6))
                IF_INT_EQUAL(LW(6), 2)
                    CALL(btlevtcmd_FaceDirectionAdd, LW(9), LW(0), 15)
                END_IF
                CALL(btlevtcmd_PhaseEventStartDeclare, LW(9))
                CALL(btlevtcmd_DivePosition, LW(9), LW(0), LW(5), LW(2), 60, 0, 5, 0, -1)
                CALL(btlevtcmd_SetHomePos, LW(9), LW(0), LW(5), LW(2))
                CALL_CHILD(cortez_weapon_set_formation_attribute)
                CALL(btlevtcmd_StartWaitEvent, LW(9))
            END_INLINE_EVENT
            ADD(LW(11), 1)
        END_LOOP
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(1), 40)
        CALL(btlevtcmd_SetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(evt_fade_entry, 24, 1000, 0, 0, 0)
        CALL(evt_fade_set_anim_ofs_pos, 0, 130)
        CALL(evt_fade_set_anim_virtual_pos, 0, 0, 80)
        CALL(evt_fade_end_wait)
        CALL(btlevtcmd_AnimeChangeGroup, -2, 1, STRING("c_korutesu3"))
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_mode_3))
        CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
        CALL(btlevtcmd_SetDiveSound, -2, STRING("SFX_BOSS_KOR_MOVE3"), 0, 0, 0, 0)
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
        CALL(evt_fade_entry, 25, 1000, 0, 0, 0)
        CALL(evt_fade_set_anim_ofs_pos, 0, 130)
        CALL(evt_fade_set_anim_virtual_pos, 0, 0, 80)
        CALL(evt_fade_end_wait)
        CALL(btlevtcmd_GetMaxHp, -2, LW(0))
        CALL(btlevtcmd_SetHp, -2, LW(0))
        CALL_CHILD(cortez_reset_pose_table)
        CALL(btlevtcmd_SetCutBaseOffset, -2, 0, 48, 0)
        CALL(btlevtcmd_SetCutWidth, -2, 90)
        CALL(btlevtcmd_SetCutHeight, -2, 96)
        CALL(btlevtcmd_reset_turn)
        CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
        CASE_END
        CASE_DEFAULT
        CALL(btlevtcmd_GetUnitWork, -2, 1, LW(3))
        IF_INT_NOT_EQUAL(LW(3), -1)
            CALL(btlevtcmd_SetUnitWork, LW(3), 0, -1)
            CALL(btlevtcmd_SetUnitWork, -2, 1, -1)
            CALL(btlevtcmd_GetDataFromDataTable, LW(3), LW(0), 71)
            CALL(btlevtcmd_RunHitEventDirect, LW(3), 0, LW(0), 0)
        END_IF
        CALL(btlevtcmd_GetUnitWork, -2, 2, LW(3))
        IF_INT_NOT_EQUAL(LW(3), -1)
            CALL(btlevtcmd_SetUnitWork, LW(3), 0, -1)
            CALL(btlevtcmd_SetUnitWork, -2, 2, -1)
            CALL(btlevtcmd_GetDataFromDataTable, LW(3), LW(0), 71)
            CALL(btlevtcmd_RunHitEventDirect, LW(3), 0, LW(0), 0)
        END_IF
        CALL(btlevtcmd_GetUnitWork, -2, 3, LW(3))
        IF_INT_NOT_EQUAL(LW(3), -1)
            CALL(btlevtcmd_SetUnitWork, LW(3), 0, -1)
            CALL(btlevtcmd_SetUnitWork, -2, 3, -1)
            CALL(btlevtcmd_GetDataFromDataTable, LW(3), LW(0), 71)
            CALL(btlevtcmd_RunHitEventDirect, LW(3), 0, LW(0), 0)
        END_IF
        CALL(btlevtcmd_GetUnitWork, -2, 4, LW(3))
        IF_INT_NOT_EQUAL(LW(3), -1)
            CALL(btlevtcmd_SetUnitWork, LW(3), 0, -1)
            CALL(btlevtcmd_SetUnitWork, -2, 4, -1)
            CALL(btlevtcmd_GetDataFromDataTable, LW(3), LW(0), 71)
            CALL(btlevtcmd_RunHitEventDirect, LW(3), 0, LW(0), 0)
        END_IF
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_D_3"))
        BEGIN_SIBLING_EVENT
            WAIT_MS(1500)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_BONE_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        END_SIBLING_EVENT
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 50331648)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_ENEMY_DIE1_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        ADD(LW(2), 10)
        CALL(evt_eff, 0, STRING("kemuri_test"), 4, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
        CALL(btlevtcmd_GetCoin, -2, LW(0))
        CALL(btlevtcmd_StoreCoin, LW(0))
        CALL(btlevtcmd_GetExp, -2, LW(0))
        CALL(btlevtcmd_StoreExp, LW(0))
        CALL(btlevtcmd_GetPos, -2, LW(3), LW(1), LW(2))
        ADD(LW(2), 10)
        ADD(LW(1), 30)
        CALL(evt_eff, STRING(""), STRING("star_point"), 0, LW(3), LW(1), LW(2), LW(0), 0, 0, 0, 0, 0, 0, 0)
        WAIT_MS(1000)
        CALL(btlevtcmd_KillUnit, -2, 1)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(mode2_rib_open_event)
    CALL(btlevtcmd_SetUnitWork, -2, 11, 2)
    CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_1D2_3"))
        CASE_EQUAL(2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_2D2_3"))
        CASE_EQUAL(3)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_3D3_3"))
    END_SWITCH
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_INHALE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(btlevtcmd_OffPartsAttribute, -2, 2, 65536)
    CALL_CHILD(cortez_reset_pose_table)
    RETURN
EVT_END()

EVT_BEGIN(mode2_rib_close_event)
    CALL(btlevtcmd_SetUnitWork, -2, 11, 0)
    CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_1D2_4"))
        CASE_EQUAL(2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_2D2_4"))
        CASE_EQUAL(3)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_3D3_4"))
    END_SWITCH
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_RIB_COUNTER1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(btlevtcmd_OnPartsAttribute, -2, 2, 65536)
    CALL_CHILD(cortez_reset_pose_table)
    RETURN
EVT_END()

EVT_BEGIN(mode2_honeduka_damage_event)
    CALL(btlevtcmd_OnUnitFlag, -2, 1)
    SET_VALUE(LW(8), 0)
    CALL(btlevtcmd_SetUnitWork, -2, 7, 1)
    CALL(btlevtcmd_CheckDamageCode, -2, 256, LW(1))
    IF_INT_NOT_EQUAL(LW(1), 0)
        BEGIN_SIBLING_EVENT_GET_ID(LW(8))
            CALL_CHILD(mode_change_event)
        END_SIBLING_EVENT
    END_IF
    BEGIN_SIBLING_EVENT_GET_ID(LW(9))
        CALL(evt_sub_intpl_init, 8, 6, 0, 30)
        LOOP(30)
            CALL(evt_sub_intpl_get_value)
            CALL(btlevtcmd_SetPartsDispOffset, -2, 4, 0, LW(0), 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_WaitEventEnd, LW(9))
    IF_INT_NOT_EQUAL(LW(8), 0)
        CALL(btlevtcmd_WaitEventEnd, LW(8))
    END_IF
    CALL(btlevtcmd_CheckDamageCode, LW(10), 1024, LW(1))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL_CHILD(dead_event)
        RETURN
    END_IF
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(mode_change_event)
    CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL(btlevtcmd_GetUnitWork, -2, 11, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_1H_1"))
        ELSE
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_1H2_1"))
        END_IF
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_MOVE2"), EVTDAT_POINTER_MAX, 0, LW(15))
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        CALL(evt_snd_sfxoff, LW(15))
        CALL(btlevtcmd_SetUnitWork, -2, 5, 2)
        CALL_CHILD(cortez_reset_pose_table)
        CASE_EQUAL(2)
        CALL(btlevtcmd_GetUnitWork, -2, 11, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_2H_1"))
        ELSE
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_2H2_1"))
        END_IF
        BEGIN_SIBLING_EVENT_GET_ID(LW(15))
            CALL(btlevtcmd_ftomsec, 80, LW(0))
            WAIT_MS(LW(0))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_MOVE2"), EVTDAT_POINTER_MAX, 0, LW(15))
            CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
            CALL(evt_snd_sfxoff, LW(15))
        END_SIBLING_EVENT
        CALL(btlevtcmd_WaitEventEnd, LW(15))
        CALL(btlevtcmd_SetUnitWork, -2, 5, 3)
        CALL_CHILD(cortez_reset_pose_table)
        CALL_CHILD(mode2_rib_open_event)
        CASE_DEFAULT
    END_SWITCH
    RETURN
EVT_END()

BattleUnitSetup entry_honeduka[1] = {
    { NULL, 0, 0, { 0.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, NULL },
};
// clang-format on
