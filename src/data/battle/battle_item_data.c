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
#include "party/party_motion.h"


u32 ItemWeaponData_Pow_Block[48];
u32 ItemWeaponData_Fire_Flower[48];
u32 ItemWeaponData_Kirakira_Otoshi[48];
u32 ItemWeaponData_Koorino_Ibuki[48];
u32 ItemWeaponData_Yurayura_Jishin[48];
u32 ItemWeaponData_Teresano_Fuku[48];
u32 ItemWeaponData_Biribiri_Kinoko[48];
u32 ItemWeaponData_Teki_Yokeeru[48];
u32 ItemWeaponData_Nemure_Yoikoyo[48];
u32 ItemWeaponData_Stop_Watch[48];
u32 ItemWeaponData_Guruguru_Memawashi[48];
u32 ItemWeaponData_Suitooru[48];
u32 ItemWeaponData_Teki_Kyouka[48];
u32 ItemWeaponData_Jiwajiwa_Kinoko[48];
u32 ItemWeaponData_Jiwajiwa_Syrup[48];
u32 ItemWeaponData_Irekaeeru[48];
u32 ItemWeaponData_Attiike_Shisshi[48];
u32 ItemWeaponData_Naniga_Okorukana[48];
u32 ItemWeaponData_FirstLovePuddingBiribiri[48];
u32 ItemWeaponData_FirstLovePuddingTrans[48];
u32 ItemWeaponData_FirstLovePuddingSleep[48];
u32 ItemWeaponData_PeachTaltBiribiri[48];
u32 ItemWeaponData_PeachTaltDodge[48];
u32 ItemWeaponData_PeachTaltSleep[48];
u32 ItemWeaponData_PoisonKinoko[48];
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

// TODO: type
u32 ItemWeaponData_Kinoko[48] = {
    0x00000000, 0x00000000, 0x00000094, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030148, 0x000007FF, 0x00001110,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Recovery, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_Kinkyuu_Kinoko[48] = {
    0x00000000, 0x00000000, 0x00000097, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030148, 0x000007FF, 0x00001110,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Recovery, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_Super_Kinoko[48] = {
    0x00000000, 0x00000000, 0x00000095, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030148, 0x000007FF, 0x00001110,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Recovery, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_Ultra_Kinoko[48] = {
    0x00000000, 0x00000000, 0x00000096, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030148, 0x000007FF, 0x00001110,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Recovery, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_Sinabita_Kinoko[48] = {
    0x00000000, 0x00000000, 0x00000098, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030148, 0x000007FF, 0x00001110,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Recovery, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_Honey_Syrup[48] = {
    0x00000000, 0x00000000, 0x0000009A, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030148, 0x000007FF, 0x80000002,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Recovery, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_Maple_Syrup[48] = {
    0x00000000, 0x00000000, 0x0000009B, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030148, 0x000007FF, 0x80000002,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Recovery, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_Royal_Jelly[48] = {
    0x00000000, 0x00000000, 0x0000009C, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030148, 0x000007FF, 0x80000002,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Recovery, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_Tankobu[48] = {
    0x00000000, 0x00000000, 0x000000A5, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030148, 0x000007FF, 0x80000002,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Recovery, 0x00000064, 0x00000000, 0x00000000,
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
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(ItemWeaponData_Pow_Block))
    CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
    LABEL(10)
    IF_INT_EQUAL(LW(10), -1)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_PreCheckDamage, -2, LW(10), LW(11), PTR(ItemWeaponData_Pow_Block), 256, LW(5))
    SWITCH(LW(5))
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(10), 38)
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(10), 39)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(10), 40)
        CASE_EQUAL(1)
        CALL(btlevtcmd_CheckDamage, -2, LW(10), LW(11), PTR(ItemWeaponData_Pow_Block), 256, LW(5))
    END_SWITCH
    CALL(btlevtcmd_GetSelectNextEnemy, LW(10), LW(11))
    GOTO(10)
    LABEL(90)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_WeaponAftereffect, PTR(ItemWeaponData_Pow_Block))
    WAIT_FRAMES(150)
    CALL_CHILD(_return_home_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

// TODO: type
u32 ItemWeaponData_Pow_Block[48] = {
    0x00000000, 0x00000000, 0x0000008B, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000002, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x00000302,
    0x00000000, 0x00033040, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Pow_Block, 0x00646464, 0x4B0A0505, 0x05050000,
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
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(ItemWeaponData_Fire_Flower))
    CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
    LABEL(10)
    IF_INT_EQUAL(LW(10), -1)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_PreCheckDamage, -2, LW(10), LW(11), PTR(ItemWeaponData_Fire_Flower), 256, LW(5))
    SWITCH(LW(5))
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(10), 38)
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(10), 39)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(10), 40)
        CASE_EQUAL(1)
        CALL(btlevtcmd_CheckDamage, -2, LW(10), LW(11), PTR(ItemWeaponData_Fire_Flower), 256, LW(5))
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
    SET_VALUE(LW(12), PTR(ItemWeaponData_Fire_Flower))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL_CHILD(ItemEvent_Support_Sub_UseDeclere)
    WAIT_FRAMES(30)
    CALL_CHILD(ItemEvent_Fire_Flower_Core)
    RETURN
EVT_END()

// TODO: type
u32 ItemWeaponData_Fire_Flower[48] = {
    0x00000000, 0x00000000, 0x00000084, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000003, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x01000302,
    0x00000000, 0x00030040, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Fire_Flower, 0x00000064, 0x00000000, 0x00000000,
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

// TODO: type
u32 ItemWeaponData_Kaminari_Dokkan[48] = {
    0x00000000, 0x00000000, 0x00000081, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000005, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x04000302,
    0x00000000, 0x00030040, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Kaminari_Dokkan, 0x00000064, 0x00050505, 0x05000000,
};

// TODO: type
u32 ItemWeaponData_Kaminari_Gorogoro[48] = {
    0x00000000, 0x00000000, 0x00000080, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000005, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20000000, 0x04000302,
    0x00000000, 0x00030040, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Kaminari_Gorogoro, 0x00000064, 0x00050505, 0x05000000,
};

// TODO: write _sampling_random_target (global, 0x10c bytes) here:
// its asm is in build/G8MJ01/asm/data/battle/battle_item_data.s

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
    SET_VALUE(LW(12), PTR(ItemWeaponData_Kirakira_Otoshi))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL_CHILD(ItemEvent_Support_Sub_UseDeclere)
    CALL_CHILD(ItemEvent_Kirakira_Otoshi_Core)
    RETURN
EVT_END()

// TODO: type
u32 ItemWeaponData_Kirakira_Otoshi[48] = {
    0x00000000, 0x00000000, 0x00000082, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000006, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x00000302,
    0x00000000, 0x00030040, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Kirakira_Otoshi, 0x000A0A64, 0x0A000000, 0x00000000,
};

EVT_BEGIN(ItemEvent_Koorino_Ibuki)
    SET_VALUE(LW(12), PTR(ItemWeaponData_Koorino_Ibuki))
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

// TODO: type
u32 ItemWeaponData_Koorino_Ibuki[48] = {
    0x00000000, 0x00000000, 0x00000083, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000003, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x02000302,
    0x00000000, 0x00030040, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x005F0200, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Koorino_Ibuki, 0x00000064, 0x00000000, 0x00000000,
};

EVT_BEGIN(ItemEvent_Yurayura_Jishin)
    SET_VALUE(LW(12), PTR(ItemWeaponData_Yurayura_Jishin))
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

// TODO: type
u32 ItemWeaponData_Yurayura_Jishin[48] = {
    0x00000000, 0x00000000, 0x00000085, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000005, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000008, 0x00000302,
    0x00000000, 0x00030040, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Yurayura_Jishin, 0x00646464, 0x4B0A0505, 0x05050000,
};

EVT_BEGIN(ItemEvent_Teresano_Fuku)
    SET_VALUE(LW(12), PTR(ItemWeaponData_Teresano_Fuku))
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

// TODO: type
u32 ItemWeaponData_Teresano_Fuku[48] = {
    0x00000000, 0x00000000, 0x00000086, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x64020000, 0x00000000,
    (u32)&ItemEvent_Teresano_Fuku, 0x00000064, 0x00000000, 0x00000000,
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
    SET_VALUE(LW(12), PTR(ItemWeaponData_Biribiri_Kinoko))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL_CHILD(ItemEvent_Support_Sub_UseDeclere)
    WAIT_FRAMES(30)
    CALL_CHILD(ItemEvent_Biribiri_Kinoko_Core)
    RETURN
EVT_END()

// TODO: type
u32 ItemWeaponData_Biribiri_Kinoko[48] = {
    0x00000000, 0x00000000, 0x00000087, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000064, 0x05000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Biribiri_Kinoko, 0x00000064, 0x00000000, 0x00000000,
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
    SET_VALUE(LW(12), PTR(ItemWeaponData_Teki_Yokeeru))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL_CHILD(ItemEvent_Support_Sub_UseDeclere)
    WAIT_FRAMES(30)
    CALL_CHILD(ItemEvent_Teki_Yokeeru_Core)
    RETURN
EVT_END()

// TODO: type
u32 ItemWeaponData_Teki_Yokeeru[48] = {
    0x00000000, 0x00000000, 0x00000088, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00640400,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Teki_Yokeeru, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_Madowaseno_Kona[48] = {
    0x00000000, 0x00000000, 0x00000089, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x00000302,
    0x00000000, 0x00030000, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00640300, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Madowaseno_Kona, 0x00000064, 0x00000000, 0x00000000,
};

EVT_BEGIN(ItemEvent_Nemure_Yoikoyo)
    SET_VALUE(LW(12), PTR(ItemWeaponData_Nemure_Yoikoyo))
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

// TODO: type
u32 ItemWeaponData_Nemure_Yoikoyo[48] = {
    0x00000000, 0x00000000, 0x0000008A, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x00000302,
    0x00000000, 0x00030000, 0x000007FF, 0x80000000,
    0x5F050000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Nemure_Yoikoyo, 0x00000064, 0x00000000, 0x00000000,
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
    SET_VALUE(LW(12), PTR(ItemWeaponData_Stop_Watch))
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

// TODO: type
u32 ItemWeaponData_Stop_Watch[48] = {
    0x00000000, 0x00000000, 0x0000008C, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x00000302,
    0x00000000, 0x00030000, 0x000007FF, 0x80000000,
    0x00005F03, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Stop_Watch, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: write _cam_rot (global, 0xb0 bytes) here:
// its asm is in build/G8MJ01/asm/data/battle/battle_item_data.s

EVT_BEGIN(ItemEvent_Guruguru_Memawashi)
    SET_VALUE(LW(12), PTR(ItemWeaponData_Guruguru_Memawashi))
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

// TODO: type
u32 ItemWeaponData_Guruguru_Memawashi[48] = {
    0x00000000, 0x00000000, 0x0000008D, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x00000302,
    0x00000000, 0x00030000, 0x000007FF, 0x80000000,
    0x00000000, 0x64030000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Guruguru_Memawashi, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_Dekadeka_Drink[48] = {
    0x00000000, 0x00000000, 0x0000008E, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000064, 0x03020000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Dekadeka_Drink, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_Katikati_Koura[48] = {
    0x00000000, 0x00000000, 0x0000008F, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00640303, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Katikati_Koura, 0x00000064, 0x00000000, 0x00000000,
};

EVT_BEGIN(ItemEvent_Suitooru)
    SET_VALUE(LW(12), PTR(ItemWeaponData_Suitooru))
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

// TODO: type
u32 ItemWeaponData_Suitooru[48] = {
    0x00000000, 0x00000000, 0x00000090, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000005, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20000000, 0x00000302,
    0x00000000, 0x00030140, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Suitooru, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: write _rank_up (global, 0x7c bytes) here:
// its asm is in build/G8MJ01/asm/data/battle/battle_item_data.s

EVT_BEGIN(ItemEvent_Teki_Kyouka)
    SET_VALUE(LW(12), PTR(ItemWeaponData_Teki_Kyouka))
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

// TODO: type
u32 ItemWeaponData_Teki_Kyouka[48] = {
    0x00000000, 0x00000000, 0x00000091, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Teki_Kyouka, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_Minimini_Kun[48] = {
    0x00000000, 0x00000000, 0x00000092, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x00000302,
    0x00000000, 0x00030000, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000064, 0x03FE0000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Minimini_Kun, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_Funyafunya_Kun[48] = {
    0x00000000, 0x00000000, 0x00000093, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x00000302,
    0x00000000, 0x00030000, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x006403FD, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Funyafunya_Kun, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_Sukkiri_Drink[48] = {
    0x00000000, 0x00000000, 0x00000099, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x64006400, 0x64006400, 0x00640000, 0x00000064,
    0x00640064, 0x00000000, 0x00640000, 0x64000000,
    0x00006400, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Sukkiri_Drink, 0x00000064, 0x00000000, 0x00000000,
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
    SET_VALUE(LW(12), PTR(ItemWeaponData_Jiwajiwa_Kinoko))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL_CHILD(ItemEvent_Support_Sub_UseDeclere)
    WAIT_FRAMES(30)
    CALL_CHILD(ItemEvent_Jiwajiwa_Kinoko_Core)
    RETURN
EVT_END()

// TODO: type
u32 ItemWeaponData_Jiwajiwa_Kinoko[48] = {
    0x00000000, 0x00000000, 0x0000009D, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x00001110,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000A02, 0x00000000,
    (u32)&ItemEvent_Jiwajiwa_Kinoko, 0x00000064, 0x00000000, 0x00000000,
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
    SET_VALUE(LW(12), PTR(ItemWeaponData_Jiwajiwa_Syrup))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL_CHILD(ItemEvent_Support_Sub_UseDeclere)
    WAIT_FRAMES(30)
    CALL_CHILD(ItemEvent_Jiwajiwa_Syrup_Core)
    RETURN
EVT_END()

// TODO: type
u32 ItemWeaponData_Jiwajiwa_Syrup[48] = {
    0x00000000, 0x00000000, 0x0000009E, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0A020000,
    (u32)&ItemEvent_Jiwajiwa_Syrup, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: write _hpfp_change_declare_2 (global, 0xb0 bytes) here:
// its asm is in build/G8MJ01/asm/data/battle/battle_item_data.s

// TODO: write _hpfp_change_declare_1 (global, 0x90 bytes) here:
// its asm is in build/G8MJ01/asm/data/battle/battle_item_data.s

EVT_BEGIN(ItemEvent_Irekaeeru)
    SET_VALUE(LW(12), PTR(ItemWeaponData_Irekaeeru))
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

// TODO: type
u32 ItemWeaponData_Irekaeeru[48] = {
    0x00000000, 0x00000000, 0x000000A1, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x001B0302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Irekaeeru, 0x00000064, 0x00000000, 0x00000000,
};

EVT_BEGIN(ItemEvent_Attiike_Shisshi)
    SET_VALUE(LW(12), PTR(ItemWeaponData_Attiike_Shisshi))
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

// TODO: type
u32 ItemWeaponData_Attiike_Shisshi[48] = {
    0x00000000, 0x00000000, 0x000000A2, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x00000302,
    0x00000000, 0x00030000, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x64000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Attiike_Shisshi, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: write _naniga_okorukana_check (global, 0x1e8 bytes) here:
// its asm is in build/G8MJ01/asm/data/battle/battle_item_data.s

EVT_BEGIN(ItemEvent_Naniga_Okorukana)
    SET_VALUE(LW(12), PTR(ItemWeaponData_Naniga_Okorukana))
    CALL_CHILD(ItemEvent_Support_Sub_UseDeclere)
    WAIT_FRAMES(30)
    CALL(_naniga_okorukana_check, -2, LW(13))
    SWITCH(LW(13))
        CASE_EQUAL(148)
        SET_VALUE(LW(12), PTR(ItemWeaponData_Kinoko))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        SET_VALUE(LW(9), 148)
        CALL_CHILD(ItemEvent_Recovery_Core)
        CASE_EQUAL(150)
        SET_VALUE(LW(12), PTR(ItemWeaponData_Ultra_Kinoko))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        SET_VALUE(LW(9), 150)
        CALL_CHILD(ItemEvent_Recovery_Core)
        CASE_EQUAL(152)
        SET_VALUE(LW(12), PTR(ItemWeaponData_Sinabita_Kinoko))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        SET_VALUE(LW(9), 152)
        CALL_CHILD(ItemEvent_Recovery_Core)
        CASE_EQUAL(154)
        SET_VALUE(LW(12), PTR(ItemWeaponData_Honey_Syrup))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        SET_VALUE(LW(9), 154)
        CALL_CHILD(ItemEvent_Recovery_Core)
        CASE_EQUAL(156)
        SET_VALUE(LW(12), PTR(ItemWeaponData_Royal_Jelly))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        SET_VALUE(LW(9), 156)
        CALL_CHILD(ItemEvent_Recovery_Core)
        CASE_EQUAL(135)
        SET_VALUE(LW(12), PTR(ItemWeaponData_Biribiri_Kinoko))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        CALL_CHILD(ItemEvent_Biribiri_Kinoko_Core)
        CASE_EQUAL(136)
        SET_VALUE(LW(12), PTR(ItemWeaponData_Teki_Yokeeru))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        CALL_CHILD(ItemEvent_Teki_Yokeeru_Core)
        CASE_EQUAL(157)
        SET_VALUE(LW(12), PTR(ItemWeaponData_Jiwajiwa_Kinoko))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        CALL_CHILD(ItemEvent_Jiwajiwa_Kinoko_Core)
        CASE_EQUAL(158)
        SET_VALUE(LW(12), PTR(ItemWeaponData_Jiwajiwa_Syrup))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        CALL_CHILD(ItemEvent_Jiwajiwa_Syrup_Core)
        CASE_EQUAL(128)
        SET_VALUE(LW(12), PTR(ItemWeaponData_Kaminari_Gorogoro))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        CALL_CHILD(ItemEvent_Kaminari_Core)
        CASE_EQUAL(129)
        SET_VALUE(LW(12), PTR(ItemWeaponData_Kaminari_Dokkan))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        CALL_CHILD(ItemEvent_Kaminari_Core)
        CASE_EQUAL(132)
        SET_VALUE(LW(12), PTR(ItemWeaponData_Fire_Flower))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        CALL_CHILD(ItemEvent_Fire_Flower_Core)
        CASE_EQUAL(130)
        SET_VALUE(LW(12), PTR(ItemWeaponData_Kirakira_Otoshi))
        SET_VALUE(LW(8), LW(12))
        CALL_CHILD(ItemEvent_Automatic_Target)
        CALL_CHILD(ItemEvent_Kirakira_Otoshi_Core)
        CASE_EQUAL(140)
        SET_VALUE(LW(12), PTR(ItemWeaponData_Stop_Watch))
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

// TODO: type
u32 ItemWeaponData_Shikaeshino_Kona[48] = {
    0x00000000, 0x00000000, 0x000000B1, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000300, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Shikaeshino_Kona, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_Kameno_Noroi[48] = {
    0x00000000, 0x00000000, 0x000000B2, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20000000, 0x00000302,
    0x00000000, 0x00030008, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00006403, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Kameno_Noroi, 0x00000064, 0x00000000, 0x00000000,
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
            SET_VALUE(LW(8), PTR(ItemWeaponData_FirstLovePuddingBiribiri))
        ELSE
            IF_INT_LESS(LW(0), 80)
                SET_VALUE(LW(8), PTR(ItemWeaponData_FirstLovePuddingTrans))
            ELSE
                SET_VALUE(LW(8), PTR(ItemWeaponData_FirstLovePuddingSleep))
            END_IF
        END_IF
        CASE_EQUAL(226)
        CALL(evt_sub_random, 100, LW(0))
        IF_INT_LESS(LW(0), 40)
            SET_VALUE(LW(8), PTR(ItemWeaponData_PeachTaltBiribiri))
        ELSE
            IF_INT_LESS(LW(0), 80)
                SET_VALUE(LW(8), PTR(ItemWeaponData_PeachTaltDodge))
            ELSE
                SET_VALUE(LW(8), PTR(ItemWeaponData_PeachTaltSleep))
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

// TODO: type
u32 ItemWeaponData_SpaceFood[48] = {
    0x00000000, 0x00000000, 0x000000C6, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x32030000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_RecoveryAndStatus, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_IceCandy[48] = {
    0x00000000, 0x00000000, 0x000000C7, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00140200, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_RecoveryAndStatus, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_NancyFrappe[48] = {
    0x00000000, 0x00000000, 0x000000C8, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00140200, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_RecoveryAndStatus, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_SnowRabbit[48] = {
    0x00000000, 0x00000000, 0x000000C9, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00320300, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_RecoveryAndStatus, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_TeaKinoko[48] = {
    0x00000000, 0x00000000, 0x000000D4, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x006403FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000502, 0x00000000,
    (u32)&ItemEvent_RecoveryAndStatus, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_FirstLovePuddingBiribiri[48] = {
    0x00000000, 0x00000000, 0x000000D8, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000064, 0x02000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_RecoveryAndStatus, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_FirstLovePuddingTrans[48] = {
    0x00000000, 0x00000000, 0x000000D8, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x64020000, 0x00000000,
    (u32)&ItemEvent_RecoveryAndStatus, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_FirstLovePuddingSleep[48] = {
    0x00000000, 0x00000000, 0x000000D8, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x64020000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_RecoveryAndStatus, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_StarryDinner[48] = {
    0x00000000, 0x00000000, 0x000000D9, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000302, 0x00000000,
    (u32)&ItemEvent_RecoveryAndStatus, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_KararinaPasta[48] = {
    0x00000000, 0x00000000, 0x000000E0, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_RecoveryAndStatus, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_MeromeroCake[48] = {
    0x00000000, 0x00000000, 0x000000E1, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x006403FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_RecoveryAndStatus, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_PeachTaltBiribiri[48] = {
    0x00000000, 0x00000000, 0x000000E2, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000064, 0x02000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_RecoveryAndStatus, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_PeachTaltDodge[48] = {
    0x00000000, 0x00000000, 0x000000E2, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00640200,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_RecoveryAndStatus, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_PeachTaltSleep[48] = {
    0x00000000, 0x00000000, 0x000000E2, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x64020000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_RecoveryAndStatus, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_BiribiriCandy[48] = {
    0x00000000, 0x00000000, 0x000000E3, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000064, 0x04000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_RecoveryAndStatus, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_HealthySalad[48] = {
    0x00000000, 0x00000000, 0x000000E9, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x64006400, 0x64006400, 0x00640000, 0x00000064,
    0x00640064, 0x00000000, 0x00640000, 0x64000000,
    0x00006400, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_RecoveryAndStatus, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_FreshJuice[48] = {
    0x00000000, 0x00000000, 0x000000EB, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x64006400, 0x64006400, 0x00640000, 0x00000064,
    0x00640064, 0x00000000, 0x00640000, 0x64000000,
    0x00006400, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_RecoveryAndStatus, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_RedKararing[48] = {
    0x00000000, 0x00000000, 0x000000AD, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_RecoveryAndStatus, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 ItemWeaponData_FutarideForever[48] = {
    0x00000000, 0x00000000, 0x000000DB, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000A02, 0x00000000,
    (u32)&ItemEvent_RecoveryAndStatus, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: write _nancy_mirage (global, 0x164 bytes) here:
// its asm is in build/G8MJ01/asm/data/battle/battle_item_data.s

// TODO: write _throw_item_icon (global, 0x34c bytes) here:
// its asm is in build/G8MJ01/asm/data/battle/battle_item_data.s

// TODO: write _get_throw_first_pos (global, 0x110 bytes) here:
// its asm is in build/G8MJ01/asm/data/battle/battle_item_data.s

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

// TODO: type
u32 ItemWeaponData_NancyDynamite[48] = {
    0x00000000, 0x00000000, 0x000000C4, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000007, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x03000302,
    0x00000000, 0x00030040, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_ThrowAttack, 0x00000064, 0x00050505, 0x05000000,
};

// TODO: type
u32 ItemWeaponData_CoconutsBomb[48] = {
    0x00000000, 0x00000000, 0x000000CA, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000007, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20000000, 0x03000302,
    0x00000000, 0x00030040, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_ThrowAttack, 0x00000064, 0x00050505, 0x05000000,
};

// TODO: type
u32 ItemWeaponData_KachikachiDish[48] = {
    0x00000000, 0x00000000, 0x000000CB, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000004, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20000000, 0x00000302,
    0x00000000, 0x00030040, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_ThrowAttack, 0x00000064, 0x00050505, 0x05000000,
};

// TODO: type
u32 ItemWeaponData_BomberEgg[48] = {
    0x00000000, 0x00000000, 0x000000D1, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000006, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20000000, 0x03000302,
    0x00000000, 0x00030040, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_ThrowAttack, 0x00000064, 0x00050505, 0x05000000,
};

EVT_BEGIN(ItemEvent_Poison_Kinoko)
    SET_VALUE(LW(12), PTR(ItemWeaponData_PoisonKinoko))
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

// TODO: type
u32 ItemWeaponData_PoisonKinoko[48] = {
    0x00000000, 0x00000000, 0x000000D5, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerHPHalf2,
    0x00000007, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030048, 0x000007FF, 0x80000000,
    0x00000000, 0x00006403, 0x01000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_Poison_Kinoko, 0x00000064, 0x00050505, 0x05000000,
};

// TODO: write _full_ap_recover (global, 0x74 bytes) here:
// its asm is in build/G8MJ01/asm/data/battle/battle_item_data.s

EVT_BEGIN(ItemEvent_LastDinner)
    SET_VALUE(LW(12), PTR(ItemWeaponData_PoisonKinoko))
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

// TODO: type
u32 ItemWeaponData_LastDinner[48] = {
    0x00000000, 0x00000000, 0x000000DA, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x10000000, 0x00000302,
    0x00000000, 0x00030108, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&ItemEvent_LastDinner, 0x00000064, 0x00000000, 0x00000000,
};

EVT_BEGIN(ItemEvent_Kaminari_Dokkan)
    SET_VALUE(LW(12), PTR(ItemWeaponData_Kaminari_Dokkan))
    CALL_CHILD(ItemEvent_Kaminari)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Kaminari_Gorogoro)
    SET_VALUE(LW(12), PTR(ItemWeaponData_Kaminari_Gorogoro))
    CALL_CHILD(ItemEvent_Kaminari)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Madowaseno_Kona)
    SET_VALUE(LW(12), PTR(ItemWeaponData_Madowaseno_Kona))
    CALL_CHILD(ItemEvent_Support_NoEffect)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Dekadeka_Drink)
    SET_VALUE(LW(12), PTR(ItemWeaponData_Dekadeka_Drink))
    CALL_CHILD(ItemEvent_Support_NoEffect)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Katikati_Koura)
    SET_VALUE(LW(12), PTR(ItemWeaponData_Katikati_Koura))
    CALL_CHILD(ItemEvent_Support_NoEffect)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Minimini_Kun)
    SET_VALUE(LW(12), PTR(ItemWeaponData_Minimini_Kun))
    CALL_CHILD(ItemEvent_Support_NoEffect)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Funyafunya_Kun)
    SET_VALUE(LW(12), PTR(ItemWeaponData_Funyafunya_Kun))
    CALL_CHILD(ItemEvent_Support_NoEffect)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Sukkiri_Drink)
    SET_VALUE(LW(12), PTR(ItemWeaponData_Sukkiri_Drink))
    CALL_CHILD(ItemEvent_Support_RefreshEffect)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Shikaeshino_Kona)
    SET_VALUE(LW(12), PTR(ItemWeaponData_Shikaeshino_Kona))
    CALL_CHILD(ItemEvent_Support_NoEffect)
    RETURN
EVT_END()

EVT_BEGIN(ItemEvent_Kameno_Noroi)
    SET_VALUE(LW(12), PTR(ItemWeaponData_Kameno_Noroi))
    CALL_CHILD(ItemEvent_Support_NoEffect)
    RETURN
EVT_END()

// TODO: write _nancy_disp (global, 0x1a0 bytes) here:
// its asm is in build/G8MJ01/asm/data/battle/battle_item_data.s

// TODO: write _disp_item_icon (global, 0x48 bytes) here:
// its asm is in build/G8MJ01/asm/data/battle/battle_item_data.s
// clang-format on
