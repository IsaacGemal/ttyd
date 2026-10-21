#include "data/battle/battle_item_data.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_audience.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_env.h"
#include "evt/evt_mobj.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "manager/evtmgr_cmd.h"
#include "party/party_motion.h"
#include "battle/battle_database_common.h"
#include "battle/battle.h"
#include "battle/battle_detect.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "data/item_icon_view.h"
#include "driver/animdrv.h"
#include "driver/effdrv.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "eff/eff_naniga.h"
#include "mario/mario_pouch.h"
#include "pmario_sound.h"
#include "statuswindow.h"
#include "system.h"
#include <dolphin/mtx.h>


BattleWeapon ItemWeaponData_Pow_Block;
BattleWeapon ItemWeaponData_Fire_Flower;
BattleWeapon ItemWeaponData_Kirakira_Otoshi;
BattleWeapon ItemWeaponData_Koorino_Ibuki;
BattleWeapon ItemWeaponData_Yurayura_Jishin;
BattleWeapon ItemWeaponData_Teresano_Fuku;
BattleWeapon ItemWeaponData_Biribiri_Kinoko;
BattleWeapon ItemWeaponData_Teki_Yokeeru;
BattleWeapon ItemWeaponData_Nemure_Yoikoyo;
BattleWeapon ItemWeaponData_Stop_Watch;
BattleWeapon ItemWeaponData_Guruguru_Memawashi;
BattleWeapon ItemWeaponData_Suitooru;
BattleWeapon ItemWeaponData_Teki_Kyouka;
BattleWeapon ItemWeaponData_Jiwajiwa_Kinoko;
BattleWeapon ItemWeaponData_Jiwajiwa_Syrup;
BattleWeapon ItemWeaponData_Irekaeeru;
BattleWeapon ItemWeaponData_Attiike_Shisshi;
u32 ItemWeaponData_Naniga_Okorukana[48];
BattleWeapon ItemWeaponData_FirstLovePuddingBiribiri;
BattleWeapon ItemWeaponData_FirstLovePuddingTrans;
BattleWeapon ItemWeaponData_FirstLovePuddingSleep;
BattleWeapon ItemWeaponData_PeachTaltBiribiri;
BattleWeapon ItemWeaponData_PeachTaltDodge;
BattleWeapon ItemWeaponData_PeachTaltSleep;
BattleWeapon ItemWeaponData_PoisonKinoko;
s32 ItemEvent_Kaminari_Dokkan[7];
s32 ItemEvent_Kaminari_Gorogoro[7];
s32 ItemEvent_Madowaseno_Kona[7];
s32 ItemEvent_Dekadeka_Drink[7];
s32 ItemEvent_Katikati_Koura[7];
s32 ItemEvent_Minimini_Kun[7];
s32 ItemEvent_Funyafunya_Kun[7];
s32 ItemEvent_Sukkiri_Drink[7];
s32 ItemEvent_Shikaeshino_Kona[7];
s32 ItemEvent_Kameno_Noroi[7];

// clang-format off
EVT_BEGIN(_return_home_event)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    LABEL(95)
    CALL(btlevtcmd_GetPos, -2, LW(3), LW(4), LW(5))
    IF_INT_NOT_EQUAL(LW(0), LW(3))
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_GetBodyId, -2, LW(0))
            CALL(btlevtcmd_AnimeChangePoseType, -2, LW(0), 42)
        END_SIBLING_EVENT
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(5.0f))
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Automatic_Target)
    CALL(btlevtcmd_CheckCommandUnit, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(8))
        CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
    ELSE
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(8))
        CALL(btlevtcmd_ChoiceSamplingEnemy, LW(8), LW(10), LW(11))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Recovery_Core_Effect)
    CALL(evt_btl_camera_set_homing_unit, 0, LW(10), -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_GetItemRecoverParam, LW(9), LW(5), LW(6), LW(7))
    IF_INT_GREATER_OR_EQUAL(LW(5), 1)
        CALL(btlevtcmd_GetPartsPos, LW(10), LW(11), LW(12), LW(13), LW(14))
        CALL(btlevtcmd_GetStatusMg, LW(10), LW(0))
        CALL(btlevtcmd_GetHeight, LW(10), LW(1))
        MULTIPLY_FLOAT(LW(0), LW(1))
        ADD(LW(13), LW(0))
        CALL(evt_eff, 0, STRING("recovery"), 0, LW(12), LW(13), LW(14), LW(5), 0, 0, 0, 0, 0, 0, 0)
        WAIT_FRAMES(120)
        CALL(btlevtcmd_RecoverHp, LW(10), LW(11), LW(5))
        WAIT_FRAMES(60)
    END_IF
    IF_INT_GREATER_OR_EQUAL(LW(6), 1)
        CALL(btlevtcmd_GetPartsPos, LW(10), LW(11), LW(12), LW(13), LW(14))
        CALL(btlevtcmd_GetStatusMg, LW(10), LW(0))
        CALL(btlevtcmd_GetHeight, LW(10), LW(1))
        MULTIPLY_FLOAT(LW(0), LW(1))
        ADD(LW(13), LW(0))
        CALL(evt_eff, 0, STRING("recovery"), 1, LW(12), LW(13), LW(14), LW(6), 0, 0, 0, 0, 0, 0, 0)
        WAIT_FRAMES(120)
        CALL(btlevtcmd_RecoverFp, LW(10), LW(11), LW(6))
        WAIT_FRAMES(60)
    END_IF
    IF_INT_GREATER_OR_EQUAL(LW(7), 1)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Recovery_Core)
    CALL_CHILD(ItemEvent_Recovery_Core_Effect)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    WAIT_FRAMES(60)
    CALL_CHILD(_return_home_event)
    CALL(btlevtcmd_StartWaitEvent, LW(10))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Recovery)
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(8))
    CALL(btlevtcmd_WeaponAftereffect, LW(8))
    CALL(btlevtcmd_CheckCommandUnit, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
    ELSE
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(8))
        CALL(btlevtcmd_ChoiceSamplingEnemy, LW(8), LW(10), LW(11))
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    WAIT_MS(1000)
    CALL(btlevtcmd_GetFriendBelong, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetBodyId, -2, LW(9))
        CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 40)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        CALL(btlevtcmd_MovePosition, -2, -60, LW(1), LW(2), 0, -1, 0)
    END_IF
    CALL(btlevtcmd_GetBodyId, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 52)
    CALL(btlevtcmd_CommandFlyItem, -2, 60)
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 69)
    CALL(btlevtcmd_ConsumeItem, -2)
    CALL(btlevtcmd_CommandGetWeaponItemId, LW(9), LW(8))
    IF_INT_EQUAL(LW(9), 0)
        CALL(btlevtcmd_GetConsumeItem, LW(9))
    END_IF
    CALL_CHILD(ItemEvent_Recovery_Core)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Support_Sub_UseDeclere)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    WAIT_MS(600)
    CALL(btlevtcmd_GetBodyId, -2, LW(9))
    CALL(btlevtcmd_GetFriendBelong, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetBodyId, -2, LW(9))
        CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 40)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        CALL(btlevtcmd_MovePosition, -2, -60, LW(1), LW(2), 0, -1, 0)
    END_IF
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 52)
    CALL(btlevtcmd_CommandFlyItem, -2, 40)
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 69)
    CALL(btlevtcmd_ConsumeItem, -2)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_GetTarget)
    CALL(btlevtcmd_CheckCommandUnit, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
        END_IF
        CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
    ELSE
        CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
            CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
        ELSE
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
            CALL(btlevtcmd_ChoiceSamplingEnemy, LW(12), LW(10), LW(11))
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Support_Sub_Effect)
    CALL_CHILD(ItemEvent_GetTarget)
    LABEL(10)
    IF_INT_EQUAL(LW(10), -1)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_PreCheckDamage, -2, LW(10), LW(11), LW(12), 256, LW(5))
    SWITCH(LW(5))
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(10), 38)
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(10), 39)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(10), 40)
        CASE_EQUAL(1)
        CALL(btlevtcmd_CheckDamage, -2, LW(10), LW(11), LW(12), 256, LW(5))
    END_SWITCH
    CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetSelectNextEnemy, LW(10), LW(11))
    ELSE
        SET_VALUE(LW(10), -1)
    END_IF
    GOTO(10)
    LABEL(90)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Support_NoEffect)
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL_CHILD(ItemEvent_Support_Sub_UseDeclere)
    WAIT_FRAMES(30)
    CALL_CHILD(ItemEvent_Support_Sub_Effect)
    CALL(evt_audience_ap_recovery)
    WAIT_FRAMES(150)
    CALL_CHILD(_return_home_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Support_RefreshEffect)
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL_CHILD(ItemEvent_Support_Sub_UseDeclere)
    WAIT_FRAMES(30)
    CALL_CHILD(ItemEvent_GetTarget)
    LABEL(10)
    IF_INT_EQUAL(LW(10), -1)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_PreCheckDamage, -2, LW(10), LW(11), LW(12), 256, LW(5))
    SWITCH(LW(5))
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(10), 38)
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(10), 39)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(10), 40)
        CASE_EQUAL(1)
        CALL(btlevtcmd_GetHitPos, LW(10), LW(11), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_REST_CONDITION1"), LW(0), LW(1), LW(2), 0)
        CALL(btlevtcmd_CheckDamage, -2, LW(10), LW(11), LW(12), 256, LW(5))
    END_SWITCH
    CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetSelectNextEnemy, LW(10), LW(11))
    ELSE
        SET_VALUE(LW(10), -1)
    END_IF
    GOTO(10)
    LABEL(90)
    CALL(evt_audience_ap_recovery)
    WAIT_FRAMES(150)
    CALL_CHILD(_return_home_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

// TODO: type
u32 ItemWeaponData_CookingItem[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030008, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Recovery, 0x00000064, 0x00000000, 0x00000000,
};

BattleWeapon ItemWeaponData_Kinoko = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_MUSHROOM,                            // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030148,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x00001110,                               // targetWeightingFlags
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
    ItemEvent_Recovery,                       // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_Kinkyuu_Kinoko = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_LIFE_SHROOM,                         // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030148,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x00001110,                               // targetWeightingFlags
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
    ItemEvent_Recovery,                       // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_Super_Kinoko = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_SUPER_SHROOM,                        // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030148,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x00001110,                               // targetWeightingFlags
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
    ItemEvent_Recovery,                       // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_Ultra_Kinoko = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_ULTRA_SHROOM,                        // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030148,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x00001110,                               // targetWeightingFlags
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
    ItemEvent_Recovery,                       // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_Sinabita_Kinoko = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_DRIED_SHROOM,                        // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030148,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x00001110,                               // targetWeightingFlags
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
    ItemEvent_Recovery,                       // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_Honey_Syrup = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_HONEY_SYRUP,                         // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030148,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80000002,                               // targetWeightingFlags
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
    ItemEvent_Recovery,                       // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_Maple_Syrup = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_MAPLE_SYRUP,                         // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030148,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80000002,                               // targetWeightingFlags
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
    ItemEvent_Recovery,                       // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_Royal_Jelly = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_JAMMIN_JELLY,                        // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030148,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80000002,                               // targetWeightingFlags
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
    ItemEvent_Recovery,                       // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_Tankobu = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_WHACKA_BUMP,                         // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030148,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80000002,                               // targetWeightingFlags
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
    ItemEvent_Recovery,                       // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

EVT_BEGIN(ItemEvent_Pow_Block)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    WAIT_MS(1000)
    CALL(btlevtcmd_GetFriendBelong, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetBodyId, -2, LW(9))
        CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 40)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        CALL(btlevtcmd_MovePosition, -2, -60, LW(1), LW(2), 0, -1, 0)
    END_IF
    CALL(btlevtcmd_GetBodyId, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 52)
    CALL(btlevtcmd_CommandFlyItem, -2, 40)
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 69)
    CALL(btlevtcmd_ConsumeItem, -2)
    CALL(btlevtcmd_CheckCommandUnit, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetHeight, -2, LW(5))
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 30)
        ADD(LW(1), 16)
        ADD(LW(1), LW(5))
    ELSE
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetHeight, -2, LW(5))
        ADD(LW(1), 16)
        ADD(LW(1), LW(5))
    END_IF
    BEGIN_SIBLING_EVENT
        WAIT_MS(100)
        CALL(evt_mobj_entry, STRING("_BtCIPw"), STRING("MOBJ_PowerBlock"))
        CALL(evt_mobj_set_position, STRING("_BtCIPw"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_ITEM_POW_APPEAR1"), LW(0), LW(1), LW(2), 0)
    END_SIBLING_EVENT
    ADD(LW(1), 10)
    ADD(LW(2), 25)
    CALL(evt_eff, STRING(""), STRING("kemuri_test"), 2, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(2050)
        CALL(evt_eff, STRING(""), STRING("kemuri_test"), 2, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    END_INLINE_EVENT
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 69)
    WAIT_FRAMES(5)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    CALL(btlevtcmd_GetPos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 43)
    WAIT_FRAMES(30)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.8f))
        CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 44)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 14, -1)
        CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 43)
        WAIT_FRAMES(60)
        CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 42)
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
        CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 43)
    END_SIBLING_EVENT
    BEGIN_INLINE_EVENT
        WAIT_FRAMES(5)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_POW_BLOCK1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mobj_set_anim, STRING("_BtCIPw"), STRING("A_1"))
        WAIT_FRAMES(3)
        CALL(evt_btl_camera_shake_h, 0, 8, 0, 10, 13)
        WAIT_FRAMES(50)
        CALL(evt_mobj_delete, STRING("_BtCIPw"))
    END_INLINE_EVENT
    CALL(evt_audience_reflesh_natural_all, 100)
    WAIT_FRAMES(7)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&ItemWeaponData_Pow_Block))
    CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
    LABEL(10)
    IF_INT_EQUAL(LW(10), -1)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_PreCheckDamage, -2, LW(10), LW(11), PTR(&ItemWeaponData_Pow_Block), 256, LW(5))
    SWITCH(LW(5))
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(10), 38)
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(10), 39)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(10), 40)
        CASE_EQUAL(1)
        CALL(btlevtcmd_CheckDamage, -2, LW(10), LW(11), PTR(&ItemWeaponData_Pow_Block), 256, LW(5))
    END_SWITCH
    CALL(btlevtcmd_GetSelectNextEnemy, LW(10), LW(11))
    GOTO(10)
    LABEL(90)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&ItemWeaponData_Pow_Block))
    WAIT_FRAMES(150)
    CALL_CHILD(_return_home_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

BattleWeapon ItemWeaponData_Pow_Block = {
    NULL,                                           // name
    0,                                              // icon
    ITEM_POW_BLOCK,                                 // item
    NULL,                                           // description
    100,                                            // baseAccuracy
    0,                                              // baseFpCost
    0,                                              // baseSpCost
    0,                                              // superguardType
    1.0f,                                           // field_0x14
    1,                                              // mStylishCommandMultiplier
    1,                                              // field_0x19
    1,                                              // mBingoSlotChance
    1,                                              // field_0x1B
    weaponGetPowerDefault,                          // hpDamageFn
    { 2, 0, 0, 0, 0, 0, 0, 0 },                     // hpDamageWork
    NULL,                                           // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                     // fpDamageWork
    0x02101260,                                     // targetClassFlags
    0x20000000,                                     // targetPropertyFlags
    0,                                              // element
    0,                                              // field_0x6D
    3,                                              // actionLevel
    2,                                              // field_0x6F
    NULL,                                           // acHelpMsg
    0x00033040,                                     // special
    0x000007FF,                                     // counterResistanceFlags
    0x80000000,                                     // targetWeightingFlags
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
    ItemEvent_Pow_Block,                            // mAttackEvtScript
    { 0, 100, 100, 100, 75, 10, 5, 5, 5, 5, 0, 0 }, // field_B4
};

EVT_BEGIN(ItemEvent_Fire_Flower_Core)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    WAIT_MS(100)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 40)
    SET_VALUE(LW(3), LW(0))
    SET_VALUE(LW(4), LW(1))
    SET_VALUE(LW(5), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(3), 240)
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    IF_INT_LESS(LW(0), LW(3))
        CALL(evt_eff64, STRING(""), STRING("fireflower_n64"), 0, LW(0), LW(1), LW(2), 180, 0, 0, 0, 0, 0, 0, 0)
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(45)
            LOOP(36)
                CALL(evt_snd_sfxon_3d, STRING("SFX_ITEM_FIRE_FLOWER1"), LW(0), LW(1), LW(2), 0)
                WAIT_FRAMES(5)
            END_LOOP
        END_SIBLING_EVENT
    ELSE
        CALL(evt_eff64, STRING(""), STRING("fireflower_n64"), 1, LW(0), LW(1), LW(2), 180, 0, 0, 0, 0, 0, 0, 0)
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(45)
            LOOP(36)
                CALL(evt_snd_sfxon_3d, STRING("SFX_ITEM_FIRE_FLOWER1"), LW(0), LW(1), LW(2), 0)
                WAIT_FRAMES(5)
            END_LOOP
        END_SIBLING_EVENT
    END_IF
    WAIT_MS(2800)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&ItemWeaponData_Fire_Flower))
    CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
    LABEL(10)
    IF_INT_EQUAL(LW(10), -1)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_PreCheckDamage, -2, LW(10), LW(11), PTR(&ItemWeaponData_Fire_Flower), 256, LW(5))
    SWITCH(LW(5))
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(10), 38)
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(10), 39)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(10), 40)
        CASE_EQUAL(1)
        CALL(btlevtcmd_CheckDamage, -2, LW(10), LW(11), PTR(&ItemWeaponData_Fire_Flower), 256, LW(5))
    END_SWITCH
    CALL(btlevtcmd_GetSelectNextEnemy, LW(10), LW(11))
    WAIT_MS(80)
    GOTO(10)
    LABEL(90)
    CALL(evt_audience_ap_recovery)
    WAIT_FRAMES(150)
    CALL_CHILD(_return_home_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Fire_Flower)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_Fire_Flower))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL_CHILD(ItemEvent_Support_Sub_UseDeclere)
    WAIT_FRAMES(30)
    CALL_CHILD(ItemEvent_Fire_Flower_Core)
    RETURN
EVT_END()

BattleWeapon ItemWeaponData_Fire_Flower = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_FIRE_FLOWER,                         // item
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
    { 3, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    1,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030040,                               // special
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
    ItemEvent_Fire_Flower,                    // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

EVT_BEGIN(ItemEvent_Kaminari_Core)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_CheckCommandUnit, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
        END_IF
        CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
    ELSE
        CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
            CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
        ELSE
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
            CALL(btlevtcmd_ChoiceSamplingEnemy, LW(12), LW(10), LW(11))
        END_IF
    END_IF
    LABEL(10)
    IF_INT_EQUAL(LW(10), -1)
        GOTO(90)
    END_IF
    CALL(evt_sub_random, 2, LW(4))
    ADD(LW(4), 3)
    CALL(btlevtcmd_GetHitPos, LW(10), LW(11), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetWidth, LW(10), LW(5))
    CALL(btlevtcmd_GetHeight, LW(10), LW(6))
    CALL(evt_eff64, STRING(""), STRING("thunder_n64"), LW(4), LW(0), LW(1), LW(2), LW(5), LW(6), 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_ITEM_ICE_THUNDER_FALL1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_btl_camera_shake_h, 0, 3, 0, 25, 13)
    CALL(evt_sub_rumble_onoff, 0, 0)
    WAIT_MS(83)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(10), LW(11), LW(12), 256, LW(5))
    SWITCH(LW(5))
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(10), 38)
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(10), 39)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(10), 40)
        CASE_EQUAL(1)
        CALL(btlevtcmd_CheckDamage, -2, LW(10), LW(11), LW(12), 256, LW(5))
    END_SWITCH
    WAIT_MS(167)
    CALL(evt_sub_rumble_onoff, 1, 0)
    WAIT_MS(167)
    CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetSelectNextEnemy, LW(10), LW(11))
    ELSE
        SET_VALUE(LW(10), -1)
    END_IF
    GOTO(10)
    LABEL(90)
    CALL(evt_audience_ap_recovery)
    WAIT_FRAMES(30)
    CALL_CHILD(_return_home_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Kaminari)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    WAIT_MS(1000)
    CALL(btlevtcmd_GetFriendBelong, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetBodyId, -2, LW(9))
        CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 40)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        CALL(btlevtcmd_MovePosition, -2, -60, LW(1), LW(2), 0, -1, 0)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_GetBodyId, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 52)
    CALL(btlevtcmd_CommandFlyItem, -2, 40)
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 69)
    CALL(btlevtcmd_ConsumeItem, -2)
    CALL_CHILD(ItemEvent_Kaminari_Core)
    RETURN
EVT_END()

BattleWeapon ItemWeaponData_Kaminari_Dokkan = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_THUNDER_RAGE,                        // item
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
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    4,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030040,                               // special
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
    ItemEvent_Kaminari_Dokkan,                // mAttackEvtScript
    { 0, 0, 0, 100, 0, 5, 5, 5, 5, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_Kaminari_Gorogoro = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_THUNDER_BOLT,                        // item
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
    4,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030040,                               // special
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
    ItemEvent_Kaminari_Gorogoro,              // mAttackEvtScript
    { 0, 0, 0, 100, 0, 5, 5, 5, 5, 0, 0, 0 }, // field_B4
};

// The sampling entry is 0x24 bytes, so the original computes the byte offset once (`mulli 0x24`)
// and keeps it in a callee-saved register while recomputing `base + ofs` for each read. Writing the
// address as `*(s16*)(samp + ofs + 4)` makes MWCC fold `ofs` into the base (`samp += ofs`); indexing
// an `s16*` view with `e * 0x12 + 2` (e = entry index) yields the original `mulli 0x24`.
s32 _sampling_random_target(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleWeapon* weapon = (BattleWeapon*)evtGetValue(event, args[0]);
    u8 sampling[0xACC];
    s8 belong;
    s32 id;
    s32 outIdx1 = args[1];
    s32 outIdx2 = args[2];
    u8* samp;

    belong = (s8)evtGetValue(event, args[3]);
    samp = sampling;
    id = BattleTransID(event, -2);
    BattleSamplingEnemy(samp, weapon, id, belong, weapon->targetClassFlags,
                        weapon->targetPropertyFlags,
                        -wp->alliances[belong].attackDirection);
    if ((s8)samp[0xA6C] <= 0) {
        evtSetValue(event, outIdx1, -1);
        evtSetValue(event, outIdx2, -1);
    } else {
        s32 idx = irand((s8)samp[0xA6C]);
        s32 e = (s8)samp[idx + 0xA6D];
        s16* sp = (s16*)samp;

        evtSetValue(event, outIdx1, sp[e * 0x12 + 2]);
        evtSetValue(event, outIdx2, sp[e * 0x12 + 3]);
    }
    return 2;
}

EVT_BEGIN(ItemEvent_Kirakira_Otoshi_Core)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_CheckCommandUnit, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(15), 0)
        CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
            SET_VALUE(LW(15), 1)
        END_IF
        CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
    ELSE
        CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            SET_VALUE(LW(15), 1)
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
            CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
        ELSE
            SET_VALUE(LW(15), 0)
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
            CALL(btlevtcmd_ChoiceSamplingEnemy, LW(12), LW(10), LW(11))
        END_IF
    END_IF
    CALL(evt_snd_sfxon, STRING("SFX_ITEM_KIRAKIRA_FALL1"), 0)
    IF_INT_EQUAL(LW(15), 0)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetStageSize, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_GetHitPos, LW(10), LW(11), LW(3), LW(4), LW(5))
        CALL(evt_eff64, STRING(""), STRING("sstar_n64"), 2, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), FLOAT(10.0f), 0, 0, 0, 0)
        CALL(evt_snd_sfxon_3d, STRING("SFX_ITEM_KIRAKIRA_FALL1"), LW(0), LW(1), LW(2), 0)
        WAIT_FRAMES(50)
    ELSE
        BEGIN_SIBLING_EVENT
            WAIT_MS(500)
            LOOP(5)
                CALL(evt_btl_camera_shake_h, 0, 5, 0, 20, 12)
                WAIT_MS(400)
                WAIT_MS(200)
            END_LOOP
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT_GET_ID(LW(15))
            LOOP(5)
                CALL(evt_sub_random, 6, LW(0))
                ADD(LW(0), 27)
                WAIT_FRAMES(LW(0))
                CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
                CALL(_sampling_random_target, LW(12), LW(10), LW(11), LW(0))
                IF_INT_NOT_EQUAL(LW(10), -1)
                    CALL(btlevtcmd_GetHitPos, LW(10), LW(11), LW(0), LW(1), LW(2))
                    CALL(evt_eff, 0, STRING("hit"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 3, 0, 0, 0, 0, 0, 0)
                END_IF
            END_LOOP
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            LOOP(7)
                CALL(evt_snd_sfxon_3d, STRING("SFX_ITEM_KIRAKIRA_FALL1"), LW(0), LW(1), LW(2), 0)
                WAIT_MS(500)
            END_LOOP
        END_SIBLING_EVENT
        LOOP(60)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_GetStageSize, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
            CALL(evt_sub_random, 200, LW(0))
            CALL(evt_sub_random, 320, LW(3))
            CALL(btlevtcmd_GetFriendBelong, LW(10), LW(6))
            IF_INT_EQUAL(LW(6), 0)
                MULTIPLY(LW(3), -1)
                SET_VALUE(LW(4), 0)
            ELSE
                MULTIPLY(LW(0), -1)
                SET_VALUE(LW(4), 0)
            END_IF
            CALL(btlevtcmd_check_battleflag, LW(8), 256)
            IF_INT_EQUAL(LW(8), 0)
                SET_VALUE(LW(2), 0)
                SET_VALUE(LW(5), 0)
            ELSE
                SET_VALUE(LW(2), 200)
                SET_VALUE(LW(5), 200)
            END_IF
            CALL(evt_sub_random, 120, LW(8))
            SUBTRACT(LW(8), 60)
            ADD(LW(5), LW(8))
            CALL(evt_sub_random, 120, LW(8))
            SUBTRACT(LW(8), 60)
            ADD(LW(2), LW(8))
            CALL(btlevtcmd_TransStageFloorPosition, LW(3), LW(4), LW(5))
            CALL(evt_eff64, STRING(""), STRING("sstar_n64"), 2, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), FLOAT(8.0f), 0, 0, 0, 0)
            WAIT_FRAMES(3)
        END_LOOP
        WAIT_FRAMES(50)
        CALL(btlevtcmd_WaitEventEnd, LW(15))
        CALL(btlevtcmd_CheckCommandUnit, -2, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
            CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
        ELSE
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
            CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
        END_IF
    END_IF
    LABEL(10)
    IF_INT_EQUAL(LW(10), -1)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_PreCheckDamage, -2, LW(10), LW(11), LW(12), 256, LW(5))
    SWITCH(LW(5))
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(10), 38)
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(10), 39)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(10), 40)
        CASE_EQUAL(1)
        CALL(btlevtcmd_CheckDamage, -2, LW(10), LW(11), LW(12), 256, LW(5))
    END_SWITCH
    CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetSelectNextEnemy, LW(10), LW(11))
    ELSE
        SET_VALUE(LW(10), -1)
    END_IF
    WAIT_MS(80)
    GOTO(10)
    LABEL(90)
    CALL(evt_audience_ap_recovery)
    WAIT_FRAMES(150)
    CALL_CHILD(_return_home_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Kirakira_Otoshi)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_Kirakira_Otoshi))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL_CHILD(ItemEvent_Support_Sub_UseDeclere)
    CALL_CHILD(ItemEvent_Kirakira_Otoshi_Core)
    RETURN
EVT_END()

BattleWeapon ItemWeaponData_Kirakira_Otoshi = {
    NULL,                                        // name
    0,                                           // icon
    ITEM_SHOOTING_STAR,                          // item
    NULL,                                        // description
    100,                                         // baseAccuracy
    0,                                           // baseFpCost
    0,                                           // baseSpCost
    0,                                           // superguardType
    1.0f,                                        // field_0x14
    1,                                           // mStylishCommandMultiplier
    1,                                           // field_0x19
    1,                                           // mBingoSlotChance
    1,                                           // field_0x1B
    weaponGetPowerDefault,                       // hpDamageFn
    { 6, 0, 0, 0, 0, 0, 0, 0 },                  // hpDamageWork
    NULL,                                        // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                  // fpDamageWork
    0x02101260,                                  // targetClassFlags
    0x20000000,                                  // targetPropertyFlags
    0,                                           // element
    0,                                           // field_0x6D
    3,                                           // actionLevel
    2,                                           // field_0x6F
    NULL,                                        // acHelpMsg
    0x00030040,                                  // special
    0x000007FF,                                  // counterResistanceFlags
    0x80000000,                                  // targetWeightingFlags
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
    ItemEvent_Kirakira_Otoshi,                   // mAttackEvtScript
    { 0, 10, 10, 100, 10, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

EVT_BEGIN(ItemEvent_Koorino_Ibuki)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_Koorino_Ibuki))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    WAIT_MS(1000)
    CALL(btlevtcmd_GetFriendBelong, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetBodyId, -2, LW(9))
        CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 40)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        CALL(btlevtcmd_MovePosition, -2, -60, LW(1), LW(2), 0, -1, 0)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_GetBodyId, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 52)
    CALL(btlevtcmd_CommandFlyItem, -2, 40)
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 69)
    CALL(btlevtcmd_ConsumeItem, -2)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_CheckCommandUnit, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
        END_IF
        CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
    ELSE
        CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
            CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
        ELSE
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
            CALL(btlevtcmd_ChoiceSamplingEnemy, LW(12), LW(10), LW(11))
        END_IF
    END_IF
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_ITEM_ICE_WIND1"), LW(0), LW(1), LW(2), LW(15))
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
    ADD(LW(1), 10)
    ADD(LW(2), 20)
    IF_INT_GREATER_OR_EQUAL(LW(5), 0)
        CALL(evt_eff, 0, STRING("ibuki"), 0, LW(0), LW(1), LW(2), 180, 0, 0, 0, 0, 0, 0, 0)
    ELSE
        CALL(evt_eff, 0, STRING("ibuki"), 1, LW(0), LW(1), LW(2), 180, 0, 0, 0, 0, 0, 0, 0)
    END_IF
    WAIT_MS(500)
    CALL(btlevtcmd_StageDispellFog)
    WAIT_MS(2000)
    CALL(evt_snd_sfxoff, LW(15))
    LABEL(10)
    IF_INT_EQUAL(LW(10), -1)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_PreCheckDamage, -2, LW(10), LW(11), LW(12), 256, LW(5))
    SWITCH(LW(5))
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(10), 38)
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(10), 39)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(10), 40)
        CASE_EQUAL(1)
        CALL(btlevtcmd_CheckDamage, -2, LW(10), LW(11), LW(12), 256, LW(5))
    END_SWITCH
    CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetSelectNextEnemy, LW(10), LW(11))
    ELSE
        SET_VALUE(LW(10), -1)
    END_IF
    GOTO(10)
    LABEL(90)
    CALL(evt_audience_ap_recovery)
    WAIT_FRAMES(150)
    CALL_CHILD(_return_home_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

BattleWeapon ItemWeaponData_Koorino_Ibuki = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_ICE_STORM,                           // item
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
    { 3, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    2,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030040,                               // special
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
    95,                                       // freezeChance
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
    ItemEvent_Koorino_Ibuki,                  // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

EVT_BEGIN(ItemEvent_Yurayura_Jishin)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_Yurayura_Jishin))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    WAIT_MS(1000)
    CALL(btlevtcmd_GetFriendBelong, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetBodyId, -2, LW(9))
        CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 40)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        CALL(btlevtcmd_MovePosition, -2, -60, LW(1), LW(2), 0, -1, 0)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_GetBodyId, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 52)
    CALL(btlevtcmd_CommandFlyItem, -2, 40)
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 69)
    CALL(btlevtcmd_ConsumeItem, -2)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    WAIT_FRAMES(30)
    BEGIN_SIBLING_EVENT
        CALL(evt_eff64, 0, STRING("hokori_n64"), 0, 0, 300, 0, 150, 0, 0, 0, 0, 0, 0, 0)
        WAIT_MS(600)
        CALL(evt_eff64, 0, STRING("hokori_n64"), 0, 0, 300, 0, 60, 0, 0, 0, 0, 0, 0, 0)
        WAIT_MS(600)
        CALL(evt_eff64, 0, STRING("hokori_n64"), 0, 0, 300, 0, 60, 0, 0, 0, 0, 0, 0, 0)
        WAIT_MS(600)
        CALL(evt_eff64, 0, STRING("hokori_n64"), 0, 0, 300, 0, 60, 0, 0, 0, 0, 0, 0, 0)
    END_SIBLING_EVENT
    CALL(evt_snd_sfxon, STRING("SFX_ITEM_QUAKE1"), LW(10))
    CALL(evt_btl_camera_shake_h, 0, 0, 5, 90, 0)
    WAIT_MS(1500)
    CALL(evt_btl_camera_shake_h, 0, 5, 5, 40, 0)
    WAIT_MS(667)
    CALL(evt_btl_camera_shake_h, 0, 5, 0, 30, 13)
    WAIT_MS(333)
    CALL(evt_snd_sfxoff, LW(10))
    CALL(btlevtcmd_CheckCommandUnit, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
        END_IF
        CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
    ELSE
        CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
            CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
        ELSE
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
            CALL(btlevtcmd_ChoiceSamplingEnemy, LW(12), LW(10), LW(11))
        END_IF
    END_IF
    LABEL(10)
    IF_INT_EQUAL(LW(10), -1)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_PreCheckDamage, -2, LW(10), LW(11), LW(12), 256, LW(5))
    SWITCH(LW(5))
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(10), 38)
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(10), 39)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(10), 40)
        CASE_EQUAL(1)
        CALL(btlevtcmd_CheckDamage, -2, LW(10), LW(11), LW(12), 256, LW(5))
    END_SWITCH
    CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetSelectNextEnemy, LW(10), LW(11))
    ELSE
        SET_VALUE(LW(10), -1)
    END_IF
    GOTO(10)
    LABEL(90)
    CALL(evt_audience_ap_recovery)
    WAIT_FRAMES(150)
    CALL_CHILD(_return_home_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

BattleWeapon ItemWeaponData_Yurayura_Jishin = {
    NULL,                                           // name
    0,                                              // icon
    ITEM_EARTH_QUAKE,                               // item
    NULL,                                           // description
    100,                                            // baseAccuracy
    0,                                              // baseFpCost
    0,                                              // baseSpCost
    0,                                              // superguardType
    1.0f,                                           // field_0x14
    1,                                              // mStylishCommandMultiplier
    1,                                              // field_0x19
    1,                                              // mBingoSlotChance
    1,                                              // field_0x1B
    weaponGetPowerDefault,                          // hpDamageFn
    { 5, 0, 0, 0, 0, 0, 0, 0 },                     // hpDamageWork
    NULL,                                           // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                     // fpDamageWork
    0x02101260,                                     // targetClassFlags
    0x20000008,                                     // targetPropertyFlags
    0,                                              // element
    0,                                              // field_0x6D
    3,                                              // actionLevel
    2,                                              // field_0x6F
    NULL,                                           // acHelpMsg
    0x00030040,                                     // special
    0x000007FF,                                     // counterResistanceFlags
    0x80000000,                                     // targetWeightingFlags
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
    ItemEvent_Yurayura_Jishin,                      // mAttackEvtScript
    { 0, 100, 100, 100, 75, 10, 5, 5, 5, 5, 0, 0 }, // field_B4
};

EVT_BEGIN(ItemEvent_Teresano_Fuku)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_Teresano_Fuku))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    WAIT_MS(1000)
    CALL(btlevtcmd_GetFriendBelong, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetBodyId, -2, LW(9))
        CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 40)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        CALL(btlevtcmd_MovePosition, -2, -60, LW(1), LW(2), 0, -1, 0)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_GetBodyId, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 52)
    CALL(btlevtcmd_CommandFlyItem, -2, 40)
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 69)
    CALL(btlevtcmd_ConsumeItem, -2)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_CheckCommandUnit, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
        END_IF
        CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
    ELSE
        CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
            CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
        ELSE
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
            CALL(btlevtcmd_ChoiceSamplingEnemy, LW(12), LW(10), LW(11))
        END_IF
    END_IF
    LABEL(10)
    IF_INT_EQUAL(LW(10), -1)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_PreCheckDamage, -2, LW(10), LW(11), LW(12), 256, LW(5))
    SWITCH(LW(5))
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(10), 38)
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(10), 39)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(10), 40)
        CASE_EQUAL(1)
        CALL(btlevtcmd_CheckDamage, -2, LW(10), LW(11), LW(12), 256, LW(5))
    END_SWITCH
    CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetSelectNextEnemy, LW(10), LW(11))
    ELSE
        SET_VALUE(LW(10), -1)
    END_IF
    GOTO(10)
    LABEL(90)
    CALL(evt_audience_ap_recovery)
    WAIT_FRAMES(150)
    CALL_CHILD(_return_home_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

BattleWeapon ItemWeaponData_Teresano_Fuku = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_BOOS_SHEET,                          // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
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
    100,                                      // invisibleChance
    2,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    ItemEvent_Teresano_Fuku,                  // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

EVT_BEGIN(ItemEvent_Biribiri_Kinoko_Core)
    CALL_CHILD(ItemEvent_Support_Sub_Effect)
    CALL(evt_audience_ap_recovery)
    WAIT_FRAMES(150)
    CALL_CHILD(_return_home_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Biribiri_Kinoko)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_Biribiri_Kinoko))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL_CHILD(ItemEvent_Support_Sub_UseDeclere)
    WAIT_FRAMES(30)
    CALL_CHILD(ItemEvent_Biribiri_Kinoko_Core)
    RETURN
EVT_END()

BattleWeapon ItemWeaponData_Biribiri_Kinoko = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_VOLT_SHROOM,                         // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
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
    100,                                      // electricChance
    5,                                        // electricTime
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
    ItemEvent_Biribiri_Kinoko,                // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

EVT_BEGIN(ItemEvent_Teki_Yokeeru_Core)
    CALL_CHILD(ItemEvent_Support_Sub_Effect)
    CALL(evt_audience_ap_recovery)
    WAIT_FRAMES(150)
    CALL_CHILD(_return_home_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Teki_Yokeeru)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_Teki_Yokeeru))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL_CHILD(ItemEvent_Support_Sub_UseDeclere)
    WAIT_FRAMES(30)
    CALL_CHILD(ItemEvent_Teki_Yokeeru_Core)
    RETURN
EVT_END()

BattleWeapon ItemWeaponData_Teki_Yokeeru = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_REPEL_CAPE,                          // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
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
    100,                                      // dodgyChance
    4,                                        // dodgyTime
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
    ItemEvent_Teki_Yokeeru,                   // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_Madowaseno_Kona = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_RUIN_POWDER,                         // item
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
    0x00030000,                               // special
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
    100,                                      // confuseChance
    3,                                        // confuseTime
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
    ItemEvent_Madowaseno_Kona,                // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

EVT_BEGIN(ItemEvent_Nemure_Yoikoyo)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_Nemure_Yoikoyo))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL_CHILD(ItemEvent_Support_Sub_UseDeclere)
    WAIT_FRAMES(30)
    CALL(evt_btl_camera_shake_h, 0, 1, 0, 10000, 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_eff, 0, STRING("sheep"), 0, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_FRAMES(300)
    CALL(evt_btl_camera_noshake, 0)
    CALL_CHILD(ItemEvent_Support_Sub_Effect)
    CALL(evt_audience_ap_recovery)
    WAIT_FRAMES(150)
    CALL_CHILD(_return_home_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

BattleWeapon ItemWeaponData_Nemure_Yoikoyo = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_SLEEPY_SHEEP,                        // item
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
    0x00030000,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80000000,                               // targetWeightingFlags
    95,                                       // sleepChance
    5,                                        // sleepTime
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
    ItemEvent_Nemure_Yoikoyo,                 // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

EVT_BEGIN(ItemEvent_Stop_Watch_Core)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    WAIT_FRAMES(30)
    CALL(evt_snd_sfxon, STRING("SFX_ITEM_STOP_WATCH1"), 0)
    CALL(btlevtcmd_check_battleflag, LW(0), 256)
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_eff64, 0, STRING("timestamp_n64"), 0, 0, 0, 103, FLOAT(1.0f), 200, 0, 0, 0, 0, 0, 0)
    ELSE
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(2), 30)
        CALL(evt_eff64, 0, STRING("timestamp_n64"), 0, 0, 0, LW(2), FLOAT(1.0f), 200, 0, 0, 0, 0, 0, 0)
    END_IF
    WAIT_FRAMES(200)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
    CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
    LABEL(10)
    IF_INT_EQUAL(LW(10), -1)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_PreCheckDamage, -2, LW(10), LW(11), LW(12), 256, LW(5))
    SWITCH(LW(5))
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(10), 38)
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(10), 39)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(10), 40)
        CASE_EQUAL(1)
        CALL(btlevtcmd_CheckDamage, -2, LW(10), LW(11), LW(12), 256, LW(5))
    END_SWITCH
    CALL(btlevtcmd_GetSelectNextEnemy, LW(10), LW(11))
    GOTO(10)
    LABEL(90)
    CALL(evt_audience_ap_recovery)
    CALL_CHILD(_return_home_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Stop_Watch)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_Stop_Watch))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    WAIT_MS(1000)
    CALL(btlevtcmd_GetFriendBelong, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetBodyId, -2, LW(9))
        CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 40)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        CALL(btlevtcmd_MovePosition, -2, -60, LW(1), LW(2), 0, -1, 0)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_GetBodyId, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 52)
    CALL(btlevtcmd_CommandFlyItem, -2, 40)
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 69)
    CALL(btlevtcmd_ConsumeItem, -2)
    CALL_CHILD(ItemEvent_Stop_Watch_Core)
    RETURN
EVT_END()

BattleWeapon ItemWeaponData_Stop_Watch = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_STOPWATCH,                           // item
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
    0x00030000,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80000000,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    95,                                       // stopChance
    3,                                        // stopTime
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
    ItemEvent_Stop_Watch,                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

s32 _cam_rot(EventEntry* event, BOOL isFirstCall) {
    f32 angle;

    if (isFirstCall) {
        event->userdata[0] = sysMsec2Frame(2000);
        event->userdata[1] = event->userdata[0];
    }
    angle = intplGetValue(4, event->userdata[0], event->userdata[1], 720.0f, 0.0f);
    camGetPtr(CAMERA_3D)->rollAngle = angle;
    event->userdata[0] -= 1;
    if (event->userdata[0] <= 0) {
        angle = 0.0f;
        camGetPtr(CAMERA_3D)->rollAngle = angle;
        return 1;
    }
    return 0;
}

EVT_BEGIN(ItemEvent_Guruguru_Memawashi)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_Guruguru_Memawashi))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL_CHILD(ItemEvent_Support_Sub_UseDeclere)
    WAIT_FRAMES(30)
    CALL(evt_snd_sfxon, STRING("SFX_ITEM_GURUGURU1"), 0)
    BEGIN_SIBLING_EVENT
        CALL(evt_env_blur_on, 0, 2000)
    END_SIBLING_EVENT
    CALL(_cam_rot)
    CALL_CHILD(ItemEvent_Support_Sub_Effect)
    CALL(evt_audience_ap_recovery)
    WAIT_FRAMES(150)
    CALL_CHILD(_return_home_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

BattleWeapon ItemWeaponData_Guruguru_Memawashi = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_DIZZY_DIAL,                          // item
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
    0x00030000,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80000000,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    100,                                      // dizzyChance
    3,                                        // dizzyTime
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
    ItemEvent_Guruguru_Memawashi,             // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_Dekadeka_Drink = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_POWER_PUNCH,                         // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
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
    100,                                      // sizeChangeChance
    3,                                        // sizeChangeTime
    2,                                        // sizeChangeStrength
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
    ItemEvent_Dekadeka_Drink,                 // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_Katikati_Koura = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_COURAGE_SHELL,                       // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
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
    100,                                      // defChangeChance
    3,                                        // defChangeTime
    3,                                        // defChangeStrength
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
    ItemEvent_Katikati_Koura,                 // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

EVT_BEGIN(ItemEvent_Suitooru)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_Suitooru))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    WAIT_MS(1000)
    CALL(btlevtcmd_GetFriendBelong, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetBodyId, -2, LW(9))
        CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 40)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        CALL(btlevtcmd_MovePosition, -2, -60, LW(1), LW(2), 0, -1, 0)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_GetBodyId, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 52)
    CALL(btlevtcmd_CommandFlyItem, -2, 40)
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 69)
    CALL(btlevtcmd_ConsumeItem, -2)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_CheckCommandUnit, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
        END_IF
        CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
    ELSE
        CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
            CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
        ELSE
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
            CALL(btlevtcmd_ChoiceSamplingEnemy, LW(12), LW(10), LW(11))
        END_IF
    END_IF
    LABEL(10)
    IF_INT_EQUAL(LW(10), -1)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_PreCheckDamage, -2, LW(10), LW(11), LW(12), 256, LW(5))
    SWITCH(LW(5))
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(10), 38)
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(10), 39)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(10), 40)
        CASE_EQUAL(1)
        CALL(btlevtcmd_CheckDamage, -2, LW(10), LW(11), LW(12), 256, LW(5))
    END_SWITCH
    CALL(btlevtcmd_GetDamage, LW(10), LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        CALL(btlevtcmd_RecoverHp, -2, LW(9), LW(0))
        CALL(btlevtcmd_GetPos, -2, LW(5), LW(6), LW(7))
        CALL(btlevtcmd_GetHeight, -2, LW(8))
        ADD(LW(6), LW(8))
        ADD(LW(7), 3)
        CALL(evt_eff, 0, STRING("recovery"), 0, LW(5), LW(6), LW(7), LW(0), 0, 0, 0, 0, 0, 0, 0)
    END_IF
    CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetSelectNextEnemy, LW(10), LW(11))
    ELSE
        SET_VALUE(LW(10), -1)
    END_IF
    GOTO(10)
    LABEL(90)
    CALL(evt_audience_ap_recovery)
    WAIT_FRAMES(60)
    CALL_CHILD(_return_home_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

BattleWeapon ItemWeaponData_Suitooru = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_HP_DRAIN_ITEM,                       // item
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
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030140,                               // special
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
    ItemEvent_Suitooru,                       // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

s32 _rank_up(EventEntry* event, BOOL isFirstCall) {
    s32 type = evtGetValue(event, event->args[0]);
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, type));

    if (unit != NULL) {
        unit->mLevel += 5;
        if (unit->mLevel > 0x63) {
            unit->mLevel = 0x63;
        }
    }
    return 2;
}

EVT_BEGIN(ItemEvent_Teki_Kyouka)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_Teki_Kyouka))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    WAIT_MS(1000)
    CALL(btlevtcmd_GetFriendBelong, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetBodyId, -2, LW(9))
        CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 40)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        CALL(btlevtcmd_MovePosition, -2, -60, LW(1), LW(2), 0, -1, 0)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_GetBodyId, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 52)
    CALL(btlevtcmd_CommandFlyItem, -2, 40)
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 69)
    CALL(btlevtcmd_ConsumeItem, -2)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_CheckCommandUnit, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
        END_IF
        CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
    ELSE
        CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
            CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
        ELSE
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
            CALL(btlevtcmd_ChoiceSamplingEnemy, LW(12), LW(10), LW(11))
        END_IF
    END_IF
    LABEL(10)
    IF_INT_EQUAL(LW(10), -1)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_snd_se, LW(10), STRING("SFX_ITEM_RANK_UP_ENEMY1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetHitPos, LW(10), LW(11), LW(0), LW(1), LW(2))
    ADD(LW(1), 20)
    ADD(LW(2), 5)
    CALL(evt_eff, STRING(""), STRING("rankup"), 0, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(_rank_up, LW(10))
    CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetSelectNextEnemy, LW(10), LW(11))
    ELSE
        SET_VALUE(LW(10), -1)
    END_IF
    GOTO(10)
    LABEL(90)
    CALL(evt_audience_ap_recovery)
    WAIT_FRAMES(150)
    CALL_CHILD(_return_home_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

BattleWeapon ItemWeaponData_Teki_Kyouka = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_TRADE_OFF,                           // item
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
    0x00030108,                               // special
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
    ItemEvent_Teki_Kyouka,                    // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_Minimini_Kun = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_MINI_MR_MINI,                        // item
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
    0x00030000,                               // special
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
    ItemEvent_Minimini_Kun,                   // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_Funyafunya_Kun = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_MR_SOFTENER,                         // item
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
    0x00030000,                               // special
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
    100,                                      // defChangeChance
    3,                                        // defChangeTime
    -3,                                       // defChangeStrength
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
    ItemEvent_Funyafunya_Kun,                 // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_Sukkiri_Drink = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_TASTY_TONIC,                         // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80000000,                               // targetWeightingFlags
    100,                                      // sleepChance
    0,                                        // sleepTime
    100,                                      // stopChance
    0,                                        // stopTime
    100,                                      // dizzyChance
    0,                                        // dizzyTime
    100,                                      // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    100,                                      // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    100,                                      // burnChance
    0,                                        // burnTime
    100,                                      // freezeChance
    0,                                        // freezeTime
    100,                                      // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    100,                                      // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    100,                                      // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    100,                                      // slowChance
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
    ItemEvent_Sukkiri_Drink,                  // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

EVT_BEGIN(ItemEvent_Jiwajiwa_Kinoko_Core)
    CALL_CHILD(ItemEvent_Support_Sub_Effect)
    CALL(evt_audience_ap_recovery)
    WAIT_FRAMES(150)
    CALL_CHILD(_return_home_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Jiwajiwa_Kinoko)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_Jiwajiwa_Kinoko))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL_CHILD(ItemEvent_Support_Sub_UseDeclere)
    WAIT_FRAMES(30)
    CALL_CHILD(ItemEvent_Jiwajiwa_Kinoko_Core)
    RETURN
EVT_END()

BattleWeapon ItemWeaponData_Jiwajiwa_Kinoko = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_SLOW_SHROOM,                         // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x00001110,                               // targetWeightingFlags
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
    10,                                       // hpRegenTime
    2,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    ItemEvent_Jiwajiwa_Kinoko,                // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

EVT_BEGIN(ItemEvent_Jiwajiwa_Syrup_Core)
    CALL_CHILD(ItemEvent_Support_Sub_Effect)
    CALL(evt_audience_ap_recovery)
    WAIT_FRAMES(150)
    CALL_CHILD(_return_home_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Jiwajiwa_Syrup)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_Jiwajiwa_Syrup))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL_CHILD(ItemEvent_Support_Sub_UseDeclere)
    WAIT_FRAMES(30)
    CALL_CHILD(ItemEvent_Jiwajiwa_Syrup_Core)
    RETURN
EVT_END()

BattleWeapon ItemWeaponData_Jiwajiwa_Syrup = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_GRADUAL_SYRUP,                       // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
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
    10,                                       // fpRegenTime
    2,                                        // fpRegenStrength
    ItemEvent_Jiwajiwa_Syrup,                 // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

s32 _hpfp_change_declare_1(EventEntry* event, BOOL isFirstCall) {
    BattleWork* wp = _battleWorkPointer;
    s32 type = evtGetValue(event, event->args[0]);
    BattleUnit* unit = BattleGetUnitPtr(wp, BattleTransID(event, type));

    if (unit != NULL) {
        if (unit->currentKind == UNIT_MARIO) {
            BattleStatusWindowSystemOn();
        } else if (unit->currentKind >= TYPE_PARTNER_MIN && unit->currentKind < TYPE_PARTNER_MAX) {
            BattleStatusWindowSystemOn();
        }
    }
    return 2;
}

s32 _hpfp_change_declare_2(EventEntry* event, BOOL isFirstCall) {
    BattleWork* wp = _battleWorkPointer;
    s32 type = evtGetValue(event, event->args[0]);
    BattleUnit* unit = BattleGetUnitPtr(wp, BattleTransID(event, type));

    if (unit != NULL) {
        if (unit->currentKind == UNIT_MARIO) {
            N_statusClearBlink();
            statusMarioHPBlink(0x78);
            statusFPBlink(0x78);
        } else if (unit->currentKind >= TYPE_PARTNER_MIN && unit->currentKind < TYPE_PARTNER_MAX) {
            N_statusClearBlink();
            statusPartyHPBlink(0x78);
            statusFPBlink(0x78);
        }
    }
    return 2;
}

EVT_BEGIN(ItemEvent_Irekaeeru)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_Irekaeeru))
    CALL_CHILD(ItemEvent_GetTarget)
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL_CHILD(ItemEvent_Support_Sub_UseDeclere)
    WAIT_FRAMES(30)
    CALL(_hpfp_change_declare_1, LW(10))
    CALL(btlevtcmd_GetHitPos, LW(10), LW(11), LW(0), LW(1), LW(2))
    ADD(LW(1), 20)
    ADD(LW(2), 5)
    CALL(evt_eff, STRING(""), STRING("irekae"), 0, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_snd_se, LW(10), STRING("SFX_ITEM_HP_FP_CHANGE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(120)
    CALL(btlevtcmd_CheckAttribute, LW(10), 1048576, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetMaxHp, LW(10), LW(5))
        CALL(btlevtcmd_GetHp, LW(10), LW(6))
        CALL(btlevtcmd_GetMaxFp, LW(10), LW(7))
        CALL(btlevtcmd_GetFp, LW(10), LW(8))
        IF_INT_GREATER(LW(8), LW(5))
            SET_VALUE(LW(8), LW(5))
        END_IF
        CALL(btlevtcmd_SetHp, LW(10), LW(8))
        IF_INT_GREATER(LW(6), LW(7))
            SET_VALUE(LW(6), LW(7))
        END_IF
        CALL(btlevtcmd_SetFp, LW(10), LW(6))
        CALL(_hpfp_change_declare_2, LW(10))
        IF_INT_EQUAL(LW(8), 0)
            CALL(btlevtcmd_PreCheckDamage, -2, LW(10), LW(11), LW(12), 256, LW(5))
            SWITCH(LW(5))
                CASE_EQUAL(3)
                CALL(btlevtcmd_StartAvoid, LW(10), 38)
                CASE_EQUAL(6)
                CALL(btlevtcmd_StartAvoid, LW(10), 39)
                CASE_EQUAL(2)
                CALL(btlevtcmd_StartAvoid, LW(10), 40)
                CASE_EQUAL(1)
                CALL(btlevtcmd_CheckDamage, -2, LW(10), LW(11), LW(12), 256, LW(5))
            END_SWITCH
        END_IF
    END_IF
    CALL(evt_audience_ap_recovery)
    WAIT_FRAMES(30)
    CALL_CHILD(_return_home_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

BattleWeapon ItemWeaponData_Irekaeeru = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_POINT_SWAP,                          // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    27,                                       // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
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
    ItemEvent_Irekaeeru,                      // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

EVT_BEGIN(ItemEvent_Attiike_Shisshi)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_Attiike_Shisshi))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL_CHILD(ItemEvent_Support_Sub_UseDeclere)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ITEM_HORROR1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(60)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ITEM_HORROR2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_eff64, 0, STRING("holography_n64"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 120, 0, 0, 0, 0, 0, 0)
    BEGIN_SIBLING_EVENT
        WAIT_MS(400)
        CALL(evt_env_blur_on, 0, 1600)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        WAIT_MS(400)
        CALL(evt_btl_camera_set_mode, 0, 3)
        CALL(evt_btl_camera_set_moveto, 0, 0, 90, 450, 0, 60, 0, 30, 11)
        WAIT_MS(250)
        CALL(evt_btl_camera_set_moveto, 0, 0, 110, 750, 0, 60, 0, 30, 11)
        WAIT_MS(250)
        CALL(evt_btl_camera_set_moveto, 0, 0, 90, 450, 0, 60, 0, 30, 11)
        WAIT_MS(250)
        CALL(evt_btl_camera_set_moveto, 0, 0, 110, 750, 0, 60, 0, 30, 11)
        WAIT_MS(250)
        CALL(evt_btl_camera_set_moveto, 0, 0, 90, 450, 0, 60, 0, 30, 11)
        WAIT_MS(250)
        CALL(evt_btl_camera_set_moveto, 0, 0, 110, 750, 0, 60, 0, 30, 11)
        WAIT_MS(250)
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    END_SIBLING_EVENT
    WAIT_MS(2000)
    CALL_CHILD(ItemEvent_Support_Sub_Effect)
    CALL(evt_audience_ap_recovery)
    WAIT_MS(2500)
    CALL_CHILD(_return_home_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

BattleWeapon ItemWeaponData_Attiike_Shisshi = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_FRIGHT_MASK,                         // item
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
    0x00030000,                               // special
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
    100,                                      // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    ItemEvent_Attiike_Shisshi,                // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

// Hoisting the effect lookup into `EffectEntry* e` fixes both the `lmw` placement (which was
// hoisted before the return-value computation when the call fed the ternary directly) and the
// boolean idiom's operand order (`or r3, r4, r3`). The `R_mystery_item_table` scan must use the
// index form `((s32*)R_mystery_item_table)[count] != 0` to avoid an extra `mr r4, r0`.
s32 _naniga_okorukana_check(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 type = evtGetValue(event, args[0]);
    s32 outIdx = args[1];
    BattleUnit* unit = BattleGetUnitPtr(wp, BattleTransID(event, type));
    Vec pos;

    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    if (!(unit->mAttributeFlags & kVeiled)) {
        if (unit->mAttributeFlags & kCeiling) {
            pos.y -= 5.0f;
        } else {
            pos.y += (f32)(BtlUnit_GetHeight(unit) + 0x14);
        }
    }
    if (isFirstCall) {
        s32 count = 0;
        s32 item;
        u8* countA;
        u8* countB;

        while (((s32*)R_mystery_item_table)[count] != 0) {
            count++;
        }
        item = (s32)R_mystery_item_table[irand(count)];
        event->userdata[0] = (s32)effNanigaEntry(0, (s32*)R_mystery_item_table, item, pos.x, pos.y,
                                                 pos.z);
        effSetName((EffectEntry*)event->userdata[0], "effNaniga");
        evtSetValue(event, outIdx, item);
        if (unit->currentKind == UNIT_MARIO) {
            countA = &wp->field_16F1C[0x25 - 0x1C];
            countB = &wp->field_16F1C[0x26 - 0x1C];
        } else if (unit->currentKind >= TYPE_PARTNER_MIN && unit->currentKind < TYPE_PARTNER_MAX) {
            countA = &wp->field_16F1C[0x27 - 0x1C];
            countB = &wp->field_16F1C[0x28 - 0x1C];
        } else {
            goto done;
        }
        switch (item) {
        case 0x80:
        case 0x81:
        case 0x82:
        case 0x84:
        case 0x8C:
            (*countA)++;
            break;
        default:
            (*countB)++;
            break;
        }
    }
 done:
    {
        EffectEntry* e = (EffectEntry*)effNameToPtr("effNaniga");

        return e ? 0 : 2;
    }
}

EVT_BEGIN(ItemEvent_Naniga_Okorukana)
    SET_VALUE(LW(12), PTR(ItemWeaponData_Naniga_Okorukana))
    CALL_CHILD(ItemEvent_Support_Sub_UseDeclere)
    WAIT_FRAMES(30)
    CALL(_naniga_okorukana_check, -2, LW(13))
    SWITCH(LW(13))
        CASE_EQUAL(148)
        SET_VALUE(LW(12), PTR(&ItemWeaponData_Kinoko))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        SET_VALUE(LW(9), 148)
        CALL_CHILD(ItemEvent_Recovery_Core)
        CASE_EQUAL(150)
        SET_VALUE(LW(12), PTR(&ItemWeaponData_Ultra_Kinoko))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        SET_VALUE(LW(9), 150)
        CALL_CHILD(ItemEvent_Recovery_Core)
        CASE_EQUAL(152)
        SET_VALUE(LW(12), PTR(&ItemWeaponData_Sinabita_Kinoko))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        SET_VALUE(LW(9), 152)
        CALL_CHILD(ItemEvent_Recovery_Core)
        CASE_EQUAL(154)
        SET_VALUE(LW(12), PTR(&ItemWeaponData_Honey_Syrup))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        SET_VALUE(LW(9), 154)
        CALL_CHILD(ItemEvent_Recovery_Core)
        CASE_EQUAL(156)
        SET_VALUE(LW(12), PTR(&ItemWeaponData_Royal_Jelly))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        SET_VALUE(LW(9), 156)
        CALL_CHILD(ItemEvent_Recovery_Core)
        CASE_EQUAL(135)
        SET_VALUE(LW(12), PTR(&ItemWeaponData_Biribiri_Kinoko))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        CALL_CHILD(ItemEvent_Biribiri_Kinoko_Core)
        CASE_EQUAL(136)
        SET_VALUE(LW(12), PTR(&ItemWeaponData_Teki_Yokeeru))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        CALL_CHILD(ItemEvent_Teki_Yokeeru_Core)
        CASE_EQUAL(157)
        SET_VALUE(LW(12), PTR(&ItemWeaponData_Jiwajiwa_Kinoko))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        CALL_CHILD(ItemEvent_Jiwajiwa_Kinoko_Core)
        CASE_EQUAL(158)
        SET_VALUE(LW(12), PTR(&ItemWeaponData_Jiwajiwa_Syrup))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        CALL_CHILD(ItemEvent_Jiwajiwa_Syrup_Core)
        CASE_EQUAL(128)
        SET_VALUE(LW(12), PTR(&ItemWeaponData_Kaminari_Gorogoro))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        CALL_CHILD(ItemEvent_Kaminari_Core)
        CASE_EQUAL(129)
        SET_VALUE(LW(12), PTR(&ItemWeaponData_Kaminari_Dokkan))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        CALL_CHILD(ItemEvent_Kaminari_Core)
        CASE_EQUAL(132)
        SET_VALUE(LW(12), PTR(&ItemWeaponData_Fire_Flower))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        CALL_CHILD(ItemEvent_Fire_Flower_Core)
        CASE_EQUAL(130)
        SET_VALUE(LW(12), PTR(&ItemWeaponData_Kirakira_Otoshi))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        CALL_CHILD(ItemEvent_Kirakira_Otoshi_Core)
        CASE_EQUAL(140)
        SET_VALUE(LW(12), PTR(&ItemWeaponData_Stop_Watch))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        CALL_CHILD(ItemEvent_Stop_Watch_Core)
        CASE_DEFAULT
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_SWITCH
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

// TODO: type
u32 ItemWeaponData_Naniga_Okorukana[48] = {
    0x00000000, 0x00000000, 0x000000A3, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02100060, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Naniga_Okorukana, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 R_mystery_item_table[14] = {
    0x00000094, 0x0000009A, 0x0000009C, 0x00000087,
    0x00000088, 0x0000009D, 0x0000009E, 0x00000096,
    0x00000098, 0x00000081, 0x00000084, 0x00000082,
    0x0000008C, 0x00000000,
};

BattleWeapon ItemWeaponData_Shikaeshino_Kona = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_SPITE_POUCH,                         // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
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
    3,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    ItemEvent_Shikaeshino_Kona,               // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_Kameno_Noroi = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_KOOPA_CURSE,                         // item
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
    0x01101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030008,                               // special
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
    100,                                      // slowChance
    3,                                        // slowTime
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
    ItemEvent_Kameno_Noroi,                   // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

EVT_BEGIN(ItemEvent_RecoveryAndStatus)
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(8))
    CALL(btlevtcmd_WeaponAftereffect, LW(8))
    CALL(btlevtcmd_CheckCommandUnit, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
    ELSE
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(8))
        CALL(btlevtcmd_ChoiceSamplingEnemy, LW(8), LW(10), LW(11))
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    WAIT_MS(1000)
    CALL(btlevtcmd_GetFriendBelong, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetBodyId, -2, LW(9))
        CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 40)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        CALL(btlevtcmd_MovePosition, -2, -60, LW(1), LW(2), 0, -1, 0)
    END_IF
    CALL(btlevtcmd_GetBodyId, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 52)
    CALL(btlevtcmd_CommandFlyItem, -2, 60)
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 69)
    CALL(btlevtcmd_ConsumeItem, -2)
    CALL(btlevtcmd_CommandGetWeaponItemId, LW(9), LW(8))
    IF_INT_EQUAL(LW(9), 0)
        CALL(btlevtcmd_GetConsumeItem, LW(9))
    END_IF
    CALL_CHILD(ItemEvent_Recovery_Core_Effect)
    SWITCH(LW(9))
        CASE_EQUAL(216)
        CALL(evt_sub_random, 100, LW(0))
        IF_INT_LESS(LW(0), 40)
            SET_VALUE(LW(8), PTR(&ItemWeaponData_FirstLovePuddingBiribiri))
        ELSE
            IF_INT_LESS(LW(0), 80)
                SET_VALUE(LW(8), PTR(&ItemWeaponData_FirstLovePuddingTrans))
            ELSE
                SET_VALUE(LW(8), PTR(&ItemWeaponData_FirstLovePuddingSleep))
            END_IF
        END_IF
        CASE_EQUAL(226)
        CALL(evt_sub_random, 100, LW(0))
        IF_INT_LESS(LW(0), 40)
            SET_VALUE(LW(8), PTR(&ItemWeaponData_PeachTaltBiribiri))
        ELSE
            IF_INT_LESS(LW(0), 80)
                SET_VALUE(LW(8), PTR(&ItemWeaponData_PeachTaltDodge))
            ELSE
                SET_VALUE(LW(8), PTR(&ItemWeaponData_PeachTaltSleep))
            END_IF
        END_IF
        CASE_OR_EQUAL(233)
        CASE_OR_EQUAL(235)
        CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_REST_CONDITION1"), LW(0), LW(1), LW(2), 0)
        CASE_END
    END_SWITCH
    SET_VALUE(LW(12), LW(8))
    CALL(btlevtcmd_PreCheckDamage, -2, LW(10), LW(11), LW(12), 256, LW(5))
    SWITCH(LW(5))
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(10), 38)
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(10), 39)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(10), 40)
        CASE_EQUAL(1)
        CALL(btlevtcmd_CheckDamage, -2, LW(10), LW(11), LW(12), 256, LW(5))
    END_SWITCH
    CALL_CHILD(_return_home_event)
    CALL(btlevtcmd_StartWaitEvent, LW(10))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

BattleWeapon ItemWeaponData_SpaceFood = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_SPACE_FOOD,                          // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
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
    50,                                       // allergicChance
    3,                                        // allergicTime
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
    ItemEvent_RecoveryAndStatus,              // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_IceCandy = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_ICICLE_POP,                          // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
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
    20,                                       // freezeChance
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
    ItemEvent_RecoveryAndStatus,              // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_NancyFrappe = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_ZESS_FRAPPE,                         // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
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
    20,                                       // freezeChance
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
    ItemEvent_RecoveryAndStatus,              // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_SnowRabbit = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_SNOW_BUNNY,                          // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
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
    50,                                       // freezeChance
    3,                                        // freezeTime
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
    ItemEvent_RecoveryAndStatus,              // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_TeaKinoko = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_SHROOM_BROTH,                        // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
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
    100,                                      // defChangeChance
    3,                                        // defChangeTime
    -1,                                       // defChangeStrength
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
    5,                                        // hpRegenTime
    2,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    ItemEvent_RecoveryAndStatus,              // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_FirstLovePuddingBiribiri = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_LOVE_PUDDING,                        // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
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
    100,                                      // electricChance
    2,                                        // electricTime
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
    ItemEvent_RecoveryAndStatus,              // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_FirstLovePuddingTrans = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_LOVE_PUDDING,                        // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
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
    100,                                      // invisibleChance
    2,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    ItemEvent_RecoveryAndStatus,              // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_FirstLovePuddingSleep = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_LOVE_PUDDING,                        // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80000000,                               // targetWeightingFlags
    100,                                      // sleepChance
    2,                                        // sleepTime
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
    ItemEvent_RecoveryAndStatus,              // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_StarryDinner = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_METEOR_MEAL,                         // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
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
    3,                                        // hpRegenTime
    2,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    ItemEvent_RecoveryAndStatus,              // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_KararinaPasta = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_SPICY_PASTA,                         // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
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
    1,                                        // chargeStrength
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
    ItemEvent_RecoveryAndStatus,              // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_MeromeroCake = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_HEARTFUL_CAKE,                       // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
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
    100,                                      // defChangeChance
    3,                                        // defChangeTime
    -1,                                       // defChangeStrength
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
    ItemEvent_RecoveryAndStatus,              // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_PeachTaltBiribiri = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_PEACH_TART,                          // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
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
    100,                                      // electricChance
    2,                                        // electricTime
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
    ItemEvent_RecoveryAndStatus,              // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_PeachTaltDodge = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_PEACH_TART,                          // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
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
    100,                                      // dodgyChance
    2,                                        // dodgyTime
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
    ItemEvent_RecoveryAndStatus,              // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_PeachTaltSleep = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_PEACH_TART,                          // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80000000,                               // targetWeightingFlags
    100,                                      // sleepChance
    2,                                        // sleepTime
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
    ItemEvent_RecoveryAndStatus,              // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_BiribiriCandy = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_ELECTRO_POP,                         // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
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
    100,                                      // electricChance
    4,                                        // electricTime
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
    ItemEvent_RecoveryAndStatus,              // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_HealthySalad = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_HEALTHY_SALAD,                       // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80000000,                               // targetWeightingFlags
    100,                                      // sleepChance
    0,                                        // sleepTime
    100,                                      // stopChance
    0,                                        // stopTime
    100,                                      // dizzyChance
    0,                                        // dizzyTime
    100,                                      // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    100,                                      // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    100,                                      // burnChance
    0,                                        // burnTime
    100,                                      // freezeChance
    0,                                        // freezeTime
    100,                                      // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    100,                                      // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    100,                                      // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    100,                                      // slowChance
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
    ItemEvent_RecoveryAndStatus,              // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_FreshJuice = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_FRESH_JUICE,                         // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80000000,                               // targetWeightingFlags
    100,                                      // sleepChance
    0,                                        // sleepTime
    100,                                      // stopChance
    0,                                        // stopTime
    100,                                      // dizzyChance
    0,                                        // dizzyTime
    100,                                      // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    100,                                      // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    100,                                      // burnChance
    0,                                        // burnTime
    100,                                      // freezeChance
    0,                                        // freezeTime
    100,                                      // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    100,                                      // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    100,                                      // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    100,                                      // slowChance
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
    ItemEvent_RecoveryAndStatus,              // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_RedKararing = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_HOT_SAUCE,                           // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
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
    1,                                        // chargeStrength
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
    ItemEvent_RecoveryAndStatus,              // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_FutarideForever = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_COUPLES_CAKE,                        // item
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
    0x01101160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
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
    10,                                       // hpRegenTime
    2,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    ItemEvent_RecoveryAndStatus,              // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

s32 _get_throw_first_pos(EventEntry* event, BOOL isFirstCall) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    BattleUnit* unit;
    Vec pos;
    s32 outIdx1 = args[1];
    s32 outIdx2 = args[2];
    s32 outIdx3 = args[3];

    unit = BattleGetUnitPtr(wp, BattleTransID(event, type));
    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    pos.y += 10.0f;
    if (!(unit->mAttributeFlags & kVeiled)) {
        if (unit->mAttributeFlags & kCeiling) {
            pos.y -= 5.0f;
        } else {
            pos.y += unit->mSizeMultiplier * (f32)BtlUnit_GetHeight(unit);
        }
    }
    evtSetFloat(event, outIdx1, pos.x);
    evtSetFloat(event, outIdx2, pos.y);
    evtSetFloat(event, outIdx3, pos.z);
    return 2;
}

s32 _throw_item_icon(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 item = evtGetValue(event, args[0]);
    f32 x1 = evtGetFloat(event, args[1]);
    f32 y1 = evtGetFloat(event, args[2]);
    f32 z1 = evtGetFloat(event, args[3]);
    f32 x2 = evtGetFloat(event, args[4]);
    f32 y2 = evtGetFloat(event, args[5]);
    f32 z2 = evtGetFloat(event, args[6]);
    ThrowItemWork* work;
    Vec pos;

    if (isFirstCall) {
        event->userdata[0] = (s32)BattleAlloc(0x7c);
        work = (ThrowItemWork*)event->userdata[0];
        work->target.x = x2;
        work->target.y = y2;
        work->target.z = z2;
        pos.x = x1;
        pos.y = y1;
        pos.z = z1;
        work->pos = pos;
        work->start = pos;
        work->counter = (s32)(distABf(work->start.x, work->start.z, work->target.x, work->target.z) /
                                     15.0f) +
                        30;
        work->gravity = 0.5f;
        work->velY = work->gravity * (f32)work->counter / 2.0f +
                     (work->target.y - work->start.y) / (f32)work->counter;
        work->speed = distABf(work->start.x, work->start.z, work->target.x, work->target.z) /
                      (f32)work->counter;
        event->userdata[1] = item;
        event->userdata[2] = 0;
        event->field_84 = psndSFXOn_3D("SFX_ITEM_COOKING_THROW1", &pos);
    }
    work = (ThrowItemWork*)event->userdata[0];
    if (event->userdata[2] != 0) {
        psndSFXOff(event->field_84);
        BattleFree(work);
        return 2;
    }
    work->pos.y += work->velY;
    work->velY -= work->gravity;
    if (work->velY < 0.0f && work->pos.y < work->target.y) {
        work->pos.y = work->target.y;
    }
    btlMovePos(&work->pos.x, &work->pos.z, work->speed,
               angleABf(work->start.x, work->start.z, work->target.x, work->target.z));
    work->counter -= 1;
    if (work->counter <= 0) {
        event->userdata[2] = 1;
        work->pos = work->target;
    }
    dispEntry(CAMERA_3D, 900.0f, 1, _disp_item_icon, event);
    return 0;
}

void _disp_item_icon(CameraId camera, void* param) {
    EventEntry* event = param;

    iconDispGx(1.0f, &((ThrowItemWork*)event->userdata[0])->pos, 0,
               itemDataTable[event->userdata[1]].icon_id);
}

EVT_BEGIN(ItemEvent_ThrowAttack)
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    WAIT_MS(1000)
    CALL(btlevtcmd_GetFriendBelong, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetBodyId, -2, LW(9))
        CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 40)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        CALL(btlevtcmd_MovePosition, -2, -60, LW(1), LW(2), 0, -1, 0)
    END_IF
    CALL(btlevtcmd_GetBodyId, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 52)
    CALL(btlevtcmd_CommandFlyItem, -2, 40)
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(9), 69)
    CALL(btlevtcmd_CommandGetWeaponItemId, LW(9), LW(12))
    IF_INT_EQUAL(LW(9), 0)
        CALL(btlevtcmd_GetConsumeItem, LW(9))
    END_IF
    CALL(btlevtcmd_ConsumeItem, -2)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_CheckCommandUnit, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
        END_IF
        CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
    ELSE
        CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
            CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
        ELSE
            CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
            CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
            CALL(btlevtcmd_ChoiceSamplingEnemy, LW(12), LW(10), LW(11))
        END_IF
    END_IF
    CALL(btlevtcmd_GetUnitKind, -2, LW(0))
    IF_INT_EQUAL(LW(0), 222)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_C_1"))
        BEGIN_SIBLING_EVENT
            WAIT_MS(500)
            CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
        END_SIBLING_EVENT
    END_IF
    CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetHitPos, LW(10), LW(11), LW(5), LW(6), LW(7))
    ELSE
        CALL(btlevtcmd_GetFriendBelong, LW(10), LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetPos, -2, LW(5), LW(6), LW(7))
            SET_VALUE(LW(5), -120)
        ELSE
            CALL(btlevtcmd_GetPos, -2, LW(5), LW(6), LW(7))
            SET_VALUE(LW(5), 100)
        END_IF
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    END_IF
    CALL(_get_throw_first_pos, -2, LW(0), LW(1), LW(2))
    CALL(_throw_item_icon, LW(9), LW(0), LW(1), LW(2), LW(5), LW(6), LW(7))
    SWITCH(LW(9))
        CASE_EQUAL(196)
        CALL(evt_snd_sfxon_3d, STRING("SFX_BTL_THUNDERS_ATTACK4"), LW(5), LW(6), LW(7), 0)
        SET_VALUE(LW(0), LW(5))
        SET_VALUE(LW(1), LW(6))
        SET_VALUE(LW(2), LW(7))
        ADD(LW(0), 100)
        CALL(evt_eff, 0, STRING("hibashira"), 5, LW(5), LW(6), LW(6), LW(0), LW(1), LW(2), 6, FLOAT(1.4f), 60, 0, 0)
        SUBTRACT(LW(0), 200)
        CALL(evt_eff, 0, STRING("hibashira"), 5, LW(5), LW(6), LW(6), LW(0), LW(1), LW(2), 6, FLOAT(1.4f), 60, 0, 0)
        BEGIN_INLINE_EVENT
            ADD(LW(7), 20)
            CALL(_nancy_mirage, LW(5), LW(6), LW(7))
        END_INLINE_EVENT
        WAIT_MS(500)
        CASE_OR_EQUAL(202)
        CASE_OR_EQUAL(209)
        CALL(evt_eff, 0, STRING("bomb"), 0, LW(5), LW(6), LW(7), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
        CALL(evt_snd_sfxon_3d, STRING("SFX_BTL_THUNDERS_ATTACK3"), LW(5), LW(6), LW(7), 0)
        CASE_END
    END_SWITCH
    LABEL(10)
    IF_INT_EQUAL(LW(10), -1)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_PreCheckDamage, -2, LW(10), LW(11), LW(12), 256, LW(5))
    SWITCH(LW(5))
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(10), 38)
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(10), 39)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(10), 40)
        CASE_EQUAL(1)
        CALL(btlevtcmd_CheckDamage, -2, LW(10), LW(11), LW(12), 256, LW(5))
    END_SWITCH
    CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetSelectNextEnemy, LW(10), LW(11))
    ELSE
        SET_VALUE(LW(10), -1)
    END_IF
    GOTO(10)
    LABEL(90)
    CALL(evt_audience_ap_recovery)
    WAIT_FRAMES(30)
    CALL_CHILD(_return_home_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

BattleWeapon ItemWeaponData_NancyDynamite = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_ZESS_DYNAMITE,                       // item
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
    3,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030040,                               // special
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
    ItemEvent_ThrowAttack,                    // mAttackEvtScript
    { 0, 0, 0, 100, 0, 5, 5, 5, 5, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_CoconutsBomb = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_COCONUT_BOMB,                        // item
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
    0x01101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    3,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030040,                               // special
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
    ItemEvent_ThrowAttack,                    // mAttackEvtScript
    { 0, 0, 0, 100, 0, 5, 5, 5, 5, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_KachikachiDish = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_COURAGE_MEAL,                        // item
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
    0x00030040,                               // special
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
    ItemEvent_ThrowAttack,                    // mAttackEvtScript
    { 0, 0, 0, 100, 0, 5, 5, 5, 5, 0, 0, 0 }, // field_B4
};

BattleWeapon ItemWeaponData_BomberEgg = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_EGG_BOMB,                            // item
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
    0x01101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    3,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030040,                               // special
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
    ItemEvent_ThrowAttack,                    // mAttackEvtScript
    { 0, 0, 0, 100, 0, 5, 5, 5, 5, 0, 0, 0 }, // field_B4
};

EVT_BEGIN(ItemEvent_Poison_Kinoko)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_PoisonKinoko))
    CALL_CHILD(ItemEvent_Support_Sub_UseDeclere)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(evt_sub_random, 100, LW(0))
    IF_INT_LESS(LW(0), 80)
        CALL_CHILD(ItemEvent_Support_Sub_Effect)
    ELSE
        CALL_CHILD(ItemEvent_GetTarget)
        CALL(evt_btl_camera_set_homing_unit, 0, LW(10), -1)
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
        CALL(evt_btl_camera_set_zoom, 0, 200)
        CALL(btlevtcmd_GetPartsPos, LW(10), LW(11), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetStatusMg, LW(10), LW(5))
        MULTIPLY_FLOAT(LW(5), 50)
        ADD(LW(1), LW(5))
        CALL(evt_eff, 0, STRING("recovery"), 4, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
        WAIT_FRAMES(60)
        CALL(btlevtcmd_RecoverHp, LW(10), LW(11), 999)
    END_IF
END_IF
CALL(evt_audience_ap_recovery)
CALL_CHILD(_return_home_event)
CALL(btlevtcmd_StartWaitEvent, -2)
RETURN
EVT_END()

BattleWeapon ItemWeaponData_PoisonKinoko = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_POISON_SHROOM,                       // item
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
    weaponGetPowerHPHalf2,                    // hpDamageFn
    { 7, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030048,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80000000,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    100,                                      // poisonChance
    3,                                        // poisonTime
    1,                                        // poisonStrength
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
    ItemEvent_Poison_Kinoko,                  // mAttackEvtScript
    { 0, 0, 0, 100, 0, 5, 5, 5, 5, 0, 0, 0 }, // field_B4
};

s32 _nancy_mirage(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);

    if (isFirstCall) {
        event->userdata[0] = 60;
        event->userdata[1] = animPoseEntry("c_cook", 1);
        event->userdata[2] = 0;
        event->field_84 = (s32)x;
        ((s32*)&event->wInterpRelatedTime)[0] = (s32)y;
        ((s32*)&event->wInterpRelatedTime)[1] = (s32)z;
        animPoseSetAnim(event->userdata[1], "A_2", 1);
        animPoseSetEffectAnim(event->userdata[1], "PM_Y_1A", 1);
    }
    event->userdata[0] -= 1;
    if (event->userdata[0] < 0) {
        animPoseSetEffect(event->userdata[1], NULL, 0);
        animPoseRelease(event->userdata[1]);
        return 2;
    }
    animPoseMain(event->userdata[1]);
    dispEntry(CAMERA_3D, z, 2, _nancy_disp, event);
    return 0;
}

__declspec(section ".sdata2") const GXColor dat_8041c504 = { 0xFF, 0xFF, 0xFF, 0x00 };

EVT_BEGIN(ItemEvent_LastDinner)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_PoisonKinoko))
    CALL_CHILD(ItemEvent_Support_Sub_UseDeclere)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL_CHILD(ItemEvent_GetTarget)
    CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHeight, LW(10), LW(5))
    CALL(btlevtcmd_GetStatusMg, LW(10), LW(6))
    MULTIPLY_FLOAT(LW(5), LW(6))
    DIVIDE(LW(5), 2)
    ADD(LW(1), LW(5))
    ADD(LW(2), 10)
    CALL(evt_eff, 0, STRING("charge"), 1, LW(0), LW(1), LW(2), FLOAT(1.0f), 120, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_ITEM_COOK_LAST1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff, 0, STRING("mist"), 6, LW(0), LW(1), LW(2), 0, 180, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_ITEM_COOK_LAST2"), LW(0), LW(1), LW(2), LW(7))
    WAIT_MS(2000)
    CALL(evt_snd_sfxoff, LW(7))
    CALL(btlevtcmd_SetHp, LW(10), 1)
    CALL(btlevtcmd_SetFp, LW(10), 0)
    CALL(_full_ap_recover, LW(10))
    CALL(btlevtcmd_StatusWindowOnOff, 2)
    CALL(evt_eff, 0, STRING("indirect"), 0, LW(0), LW(1), LW(2), 60, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff, 0, STRING("hit"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 3, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_ITEM_COOK_LAST3"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(250)
    CALL(evt_eff, 0, STRING("hit"), 1, LW(0), LW(1), LW(2), FLOAT(1.0f), 3, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_ITEM_COOK_LAST3"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(250)
    CALL(evt_eff, 0, STRING("hit"), 2, LW(0), LW(1), LW(2), FLOAT(1.0f), 3, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_ITEM_COOK_LAST3"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(250)
    CALL(evt_eff, 0, STRING("hit"), 3, LW(0), LW(1), LW(2), FLOAT(1.0f), 3, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_ITEM_COOK_LAST3"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(1750)
    CALL(evt_audience_ap_recovery)
    CALL_CHILD(_return_home_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

BattleWeapon ItemWeaponData_LastDinner = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_TRIAL_STEW,                          // item
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
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00030108,                               // special
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
    ItemEvent_LastDinner,                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

EVT_BEGIN(ItemEvent_Kaminari_Dokkan)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_Kaminari_Dokkan))
    CALL_CHILD(ItemEvent_Kaminari)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Kaminari_Gorogoro)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_Kaminari_Gorogoro))
    CALL_CHILD(ItemEvent_Kaminari)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Madowaseno_Kona)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_Madowaseno_Kona))
    CALL_CHILD(ItemEvent_Support_NoEffect)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Dekadeka_Drink)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_Dekadeka_Drink))
    CALL_CHILD(ItemEvent_Support_NoEffect)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Katikati_Koura)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_Katikati_Koura))
    CALL_CHILD(ItemEvent_Support_NoEffect)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Minimini_Kun)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_Minimini_Kun))
    CALL_CHILD(ItemEvent_Support_NoEffect)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Funyafunya_Kun)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_Funyafunya_Kun))
    CALL_CHILD(ItemEvent_Support_NoEffect)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Sukkiri_Drink)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_Sukkiri_Drink))
    CALL_CHILD(ItemEvent_Support_RefreshEffect)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Shikaeshino_Kona)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_Shikaeshino_Kona))
    CALL_CHILD(ItemEvent_Support_NoEffect)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Kameno_Noroi)
    SET_VALUE(LW(12), PTR(&ItemWeaponData_Kameno_Noroi))
    CALL_CHILD(ItemEvent_Support_NoEffect)
    RETURN
EVT_END()

#ifdef MUST_MATCH
// Match-only: the colour built through an inline lands the local in the original's stack slot (brief code shape).
static inline GXColor nancyColor(u8 alpha) {
    GXColor c = dat_8041c504;
    c.a = alpha;
    return c;
}
#endif

void _nancy_disp(CameraId camera, void* param) {
    EventEntry* event = param;
    GXColor color;
    Mtx mtx;
    Mtx scale;
    Mtx trans;
    f32 scaleValue;
    u8 alpha;
    s32 timer = event->userdata[0];

    if (timer < 30) {
        scaleValue = 0.05f * (f32)(30 - timer) + 4.0f;
        alpha = 0x80 - (30 - timer) * 4;
    } else {
        scaleValue = 4.0f;
        alpha = 0x80;
    }
    PSMTXTrans(trans, (f32)(s32)event->field_84,
               (f32)((s32*)&event->wInterpRelatedTime)[0],
               (f32)((s32*)&event->wInterpRelatedTime)[1]);
    PSMTXScale(scale, scaleValue, scaleValue, 1.0f);
    PSMTXIdentity(mtx);
    PSMTXConcat(scale, mtx, mtx);
    PSMTXConcat(trans, mtx, mtx);
#ifdef MUST_MATCH
    color = nancyColor(alpha);
#else
    color = dat_8041c504;
    color.a = alpha;
#endif
    animPoseSetMaterialEvtColor(event->userdata[1], color);
    animPoseSetMaterialFlagOn(event->userdata[1], 0x40);
    animPoseDrawMtx(event->userdata[1], mtx, 1, 0.0f, 2.0f);
    animPoseDrawMtx(event->userdata[1], mtx, 2, 0.0f, 2.0f);
    animPoseDrawMtx(event->userdata[1], mtx, 3, 0.0f, 2.0f);
}

s32 _full_ap_recover(EventEntry* event, BOOL isFirstCall) {
    BattleWork* wp = _battleWorkPointer;
    s32 type = evtGetValue(event, event->args[0]);
    BattleUnit* unit = BattleGetUnitPtr(wp, BattleTransID(event, type));

    if (BtlUnit_GetBelong(unit) == 0) {
        pouchSetAP(pouchGetMaxAP());
    }
    return 2;
}
// clang-format on
