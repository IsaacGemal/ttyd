#include "unit/party/unit_party_vivian.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_cmd.h"
#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "battle/battle_sub.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "driver/msgdrv.h"
#include "pmario_sound.h"
#include "system.h"
#include "math_ext.h"
#include "mario/mario_party.h"
#include <dolphin/mtx.h>
#include <string.h>
#include "battle/ac/ac_table.h"
#include "battle/battle_ac.h"
#include "battle/battle_attack_audience.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_seq_command.h"
#include "driver/mobjdrv.h"
#include "eff/n64/eff_kameki_tornade_n64.h"
#include "evt/evt_audience.h"
#include "evt/evt_eff.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "party/party_motion.h"
extern u32 lbl_80388868[2];

// First object of this unit's .data (the float pair that used to end the previous unit's .data).


// sdata2 float constants live in this file's .sdata2 (see the original's data).






#define STR_P_BIBI "p_bibi"
#define STR_PM_B_1 "PM_B_1"
#define STR_SHADOW1_2 "SFX_BTL_VIVIAN_SHADOW1_2"
#define STR_SHADOW1_1 "SFX_BTL_VIVIAN_SHADOW1_1"
#define STR_PM_B_2 "PM_B_2\0"
#define VEC_POS1 ((Vec){ 0.0f, 0.0f, 0.0f })
#define VEC_POS2 ((Vec){ 0.0f, 0.0f, 0.0f })

extern s32 btldefaultevt_avoid_counter_event[];
extern s32 evt_audience_acrobat_notry[];

extern s32 one_negone[];
extern s32 subset_attack_ready_jump_event[];
extern s32 weaponGetACOutputParam[];
extern s32 weaponGetPowerFromPartyAttackLv[];
static void __makeTechMenuFunc(BattleWorkCommand* cmd, s32* count);
static s32 init_event[68];
static s32 damage_event[16];
s32 vivian_hide_event[209];
u32 partyWeapon_VivianNormalAttack[48];
u32 partyWeapon_VivianShadowGuard[48];
u32 partyWeapon_VivianMagicalPowder[48];
u32 partyWeapon_VivianCharmKissAttack[48];
s32 partyVivianAttack_NormalAttack[581];
s32 partyVivianAttack_ShadowGuard[430];
s32 partyVivianAttack_MagicalPowder[442];
s32 partyVivianAttack_CharmKissAttack[464];
s32 vivian_shadow_tail_event[78];
s32 vivian_counter_damage_event[177];
s32 vivian_appeal_event[80];
static s32 party_win_reaction[18];
static s32 attack_audience_event[189];
static u32 defence[2];
static u32 defence_attr[2];
static u32 regist[6];
static s32 attack_event[2];
static s32 wait_event[6];


#pragma force_active on
// ---- data region (before the functions: the original pools strings and templates in source order)
// TODO: type
__declspec(section ".data") u32 lbl_80388868[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
static __declspec(section ".data") u32 negone_one_374[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_80388878[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
static __declspec(section ".data") u32 one_negone_373[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_80388888[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
static __declspec(section ".data") u32 negone_one_360[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_80388898[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
static __declspec(section ".data") u32 one_negone_359[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
u32 pose_table_vivian_stay[48] = {
    0x00000009, (u32)"PTR_D_4", 0x00000002, (u32)"PTR_D_4",
    0x00000003, (u32)"PTR_X_1", 0x00000004, (u32)"PTR_D_2",
    0x00000005, (u32)"PTR_X_1", 0x00000001, (u32)"PTR_N_1",
    0x0000001B, (u32)"PTR_D_3", 0x0000001C, (u32)"PTR_S_1",
    0x00000025, (u32)"PTR_G_2", 0x0000001F, (u32)"PTR_G_1",
    0x00000027, (u32)"PTR_D_1", 0x00000028, (u32)"PTR_W_1",
    0x0000002A, (u32)"PTR_R_1", 0x00000039, (u32)"PTR_D_2",
    0x00000038, (u32)"PTR_D_2", 0x0000003C, (u32)"PTR_D_4",
    0x0000003D, (u32)"PTR_D_2", 0x00000041, (u32)"PTR_T_1",
    0x00000034, (u32)"PTR_I_1", 0x00000043, (u32)"PTR_C_1",
    0x00000040, (u32)"PTR_X_1", 0x00000031, (u32)"PTR_D_1",
    0x0000002D, (u32)"PTR_G_1", 0x00000045, (u32)"PTR_S_1",
};

// TODO: type
static u32 data_table[40] = {
    0x00000003, (u32)&vivian_hide_event, 0x00000005, (u32)&vivian_appeal_event,
    0x00000006, (u32)&attack_audience_event, 0x00000001, (u32)__makeTechMenuFunc,
    0x00000019, (u32)&vivian_counter_damage_event, 0x0000001A, (u32)&vivian_counter_damage_event,
    0x0000001B, (u32)&vivian_counter_damage_event, 0x0000001C, (u32)&vivian_counter_damage_event,
    0x0000001D, (u32)&vivian_counter_damage_event, 0x0000001E, (u32)&vivian_counter_damage_event,
    0x0000001F, (u32)&vivian_counter_damage_event, 0x00000020, (u32)&vivian_counter_damage_event,
    0x00000021, (u32)&vivian_counter_damage_event, 0x00000022, (u32)&vivian_counter_damage_event,
    0x00000023, (u32)&vivian_counter_damage_event, 0x00000030, (u32)btldefaultevt_Dummy,
    0x00000040, (u32)&party_win_reaction, 0x00000007, (u32)subset_attack_ready_jump_event,
    0x0000002F, (u32)btldefaultevt_avoid_counter_event, 0x00000000, 0x00000000,
};

// TODO: type
u32 unitpartsdata_Party_Vivian[19] = {
    0x00000001, (u32)"btl_un_vivian", (u32)"c_vivian", 0x00000000,
    0x00000000, 0x00000000, 0x41800000, 0x41C00000,
    0x40A00000, 0x00000000, 0x41C00000, 0x00000000,
    0x00140018, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00000009, 0x00000000, (u32)&pose_table_vivian_stay,
};

// TODO: type
u32 unitdata_Party_Vivian[49] = {
    0x000000E4, (u32)"btl_un_vivian", 0x000A0005, 0x05010100,
    0x00000064, 0x00090014, 0x0021000E, 0x00240000,
    0x00000000, 0x41400000, 0x00000000, 0x00000000,
    0x00000000, 0x42200000, 0x00000000, 0x00000000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41200000,
    0x00000000, 0x00000000, 0x41200000, 0x41AB999A,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0000FF00, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_PARTY_DAMAGE1",
    (u32)"SFX_MARIO_DAMAGE_FIRE1", (u32)"SFX_MARIO_DAMAGE_ICE1", (u32)"SFX_MARIO_DAMAGE_BIRIBIRI1", 0x02000004,
    (u32)&regist, 0x01000000, (u32)&unitpartsdata_Party_Vivian, (u32)&init_event,
    (u32)&data_table,
};

// Matched. `VEC_POS1` is a `const volatile` view of the zero vector and the two userdata stores are
// `volatile`: without that, MWCC hoists the zero-Vec copy above the stores and puts the constant 0 in
// r6 instead of r0 (96.01%). The `#else` form is the plain, natural source.
USER_FUNC(battle_evt_majo_disp_on) {
    s32* args = event->args;
    BattleUnit* unit;
    BattleUnitPart* parts;
    s32 pose;
    s32 id;
    s32 v1;
    s32 v5;
    f32 f2;
    f32 f3;
    f32 f4;

    id = BattleTransID(event, evtGetValue(event, args[0]));
    v1 = evtGetValue(event, args[1]);
    f2 = evtGetFloat(event, args[2]);
    f3 = evtGetFloat(event, args[3]);
    f4 = evtGetFloat(event, args[4]);
    v5 = evtGetValue(event, args[5]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    parts = BattleGetUnitPartsPtr(id, v1);
    pose = parts->poseId;

    if (isFirstCall) {
        event->userdata[0] = 0;
        event->userdata[1] = 0;
        {
            Vec pos = VEC_POS1;
            pos.x = f2;
            pos.y = f3 - 200.f;
            pos.z = f4;
            unit->mPosition = pos;
        }
        animEffectAsync(STR_P_BIBI, 1);
    }

    switch (event->userdata[0]) {
    case 0:
        parts->field_0x78 = intplGetValue(0, event->userdata[1], 30, 0.0f, 2.0f);
        if (event->userdata[1] == 0) {
            psndSFXOn_3D(STR_SHADOW1_1, &unit->mPosition);
        }
        event->userdata[1]++;
        if (event->userdata[1] > 15) {
            event->userdata[0]++;
        }
        break;
    case 1:
        if (event->userdata[1] <= 30) {
            parts->field_0x78 = intplGetValue(0, event->userdata[1], 30, 0.0f, 2.0f);
            event->userdata[1]++;
        }
        if (!animEffectAsync(STR_P_BIBI, 1)) {
            return 0;
        }
        {
            Vec pos = VEC_POS2;
            pos.x = f2;
            pos.y = f3;
            pos.z = f4;
            unit->mPosition = pos;
        }
        animPoseSetEffect(pose, STR_P_BIBI, 1);
        animPoseSetEffectAnim(pose, STR_PM_B_2, 1);
        event->userdata[0]++;
        break;
    case 2:
        if (event->userdata[1] <= 30) {
            parts->field_0x78 = intplGetValue(0, event->userdata[1], 30, 0.0f, 2.0f);
            event->userdata[1]++;
        }
        if (animPoseGetLoopTimes(animPoseGetAnimPosePtr(pose)->paperId) >= 0.6666667f) {
            animPoseSetPaperAnimGroup(pose, NULL, 1);
            event->userdata[0]++;
            event->userdata[1] = 0;
        }
        break;
    case 3:
        parts->field_0x78 = intplGetValue(0, event->userdata[1], 15, 2.0f, 1.0f);
        event->userdata[1]++;
        if (event->userdata[1] > 15) {
            parts->field_0x78 = 1.0f;
            animPoseSetEffect(pose, NULL, 1);
            if (v5 != 0) {
                unit->flags &= ~0x10000;
            }
            return EVT_RETURN_DONE;
        }
        break;
    }
    return 0;
}

// TODO: type
static s32 battle_entry_event[59] = {
    0x0005005B, (s32)btlevtcmd_GetHomePos, 0xFFFFFFFE, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0004005B, (s32)btlevtcmd_FaceDirectionSub,
    0xFFFFFFFE, 0xFE363C80, 0x000000FA, 0x0005005B,
    (s32)btlevtcmd_SetPos, 0xFFFFFFFE, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00010009, 0x00000028, 0x0004005B,
    (s32)btlevtcmd_AnimeChangePoseType, 0xFFFFFFFE, 0x00000001, 0x0000002A,
    0x0005005B, (s32)btlevtcmd_GetHomePos, 0xFFFFFFFE, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0003005B, (s32)btlevtcmd_SetMoveSpeed,
    0xFFFFFFFE, 0xF24A8A80, 0x0009005B, (s32)btlevtcmd_SetMoveSoundDataWork,
    0xFFFFFFFE, (s32)"SFX_PARTY_BATTLE_SUBHANG1", (s32)"SFX_PARTY_BATTLE_SUBHANG1", 0x00000000,
    0x00000006, 0x00000006, 0x00000000, 0x00000000,
    0x0008005B, (s32)btlevtcmd_MovePosition, 0xFFFFFFFE, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000000, 0xFFFFFFFE,
    0x00000000, 0x0003005B, (s32)btlevtcmd_AnimeChangePoseFromTable, 0xFFFFFFFE,
    0x00000001, 0x00000002, 0x00000001,
};

// TODO: type
static s32 init_event[68] = {
    0x0001005B, (s32)_vivian_make_extra_work_area, 0x0003005B, (s32)btlevtcmd_SetEventWait,
    0xFFFFFFFE, (s32)&wait_event, 0x0003005B, (s32)btlevtcmd_SetEventAttack,
    0xFFFFFFFE, (s32)&attack_event, 0x0003005B, (s32)btlevtcmd_SetEventDamage,
    0xFFFFFFFE, (s32)&damage_event, 0x0003005B, (s32)btlevtcmd_SetEventConfusion,
    0xFFFFFFFE, (s32)btldefaultevt_Confuse, 0x0004005B, (s32)btlevtcmd_SetUnitWork,
    0xFFFFFFFE, 0x00000004, 0x00000000, 0x0004005B,
    (s32)btlevtcmd_SetUnitWork, 0xFFFFFFFE, 0x00000003, 0x00000000,
    0x0001005C, (s32)&vivian_shadow_tail_event, 0x0007005B, (s32)btlevtcmd_SetWalkSound,
    0xFFFFFFFE, (s32)"SFX_PARTY_BATTLE_MOVE1L", (s32)"SFX_PARTY_BATTLE_MOVE1R", 0x00000000,
    0x00000014, 0x00000014, 0x0007005B, (s32)btlevtcmd_SetRunSound,
    0xFFFFFFFE, (s32)"SFX_PARTY_BATTLE_MOVE1L", (s32)"SFX_PARTY_BATTLE_MOVE1R", 0x00000000,
    0x0000000B, 0x0000000B, 0x0007005B, (s32)btlevtcmd_SetDiveSound,
    0xFFFFFFFE, (s32)"SFX_PARTY_BATTLE_HANG1", (s32)"SFX_PARTY_BATTLE_HANG1", 0x00000000,
    0x0000000B, 0x0000000B, 0x0004005B, (s32)btlevtcmd_SetJumpSound,
    0xFFFFFFFE, (s32)"SFX_PARTY_BATTLE_JUMP1", (s32)"SFX_PARTY_BATTLE_LANDING1", 0x0003005B,
    (s32)btlevtcmd_SetEventEntry, 0xFFFFFFFE, (s32)&battle_entry_event, 0x0002005B,
    (s32)btlevtcmd_StartWaitEvent, 0xFFFFFFFE, 0x00000002, 0x00000001,
};

// TODO: type
static s32 damage_event[16] = {
    0x0005005B, (s32)btlevtcmd_SetRotate, 0xFFFFFFFE, 0x00000000,
    0x00000000, 0x00000000, 0x00020032, 0xFE363C8A,
    0xFFFFFFFE, 0x00020032, 0xFE363C8B, 0x00000001,
    0x0001005E, (s32)btldefaultevt_Damage, 0x00000002, 0x00000001,
};

// TODO: type
s32 vivian_hide_event[209] = {
    0x0004005B, (s32)btlevtcmd_GetUnitWork, 0xFFFFFFFE, 0x00000004,
    0xFE363C80, 0x0002001D, 0xFE363C80, 0x00000001,
    0x00020036, 0xFE363C80, 0x00000001, 0x00000021,
    0x0004005B, (s32)btlevtcmd_SetUnitWork, 0xFFFFFFFE, 0x00000004,
    0xFE363C80, 0x0002001C, 0xFE363C80, 0x00000000,
    0x00020032, 0xFE363C83, 0xFFFFFFFD, 0x0002005B,
    (s32)btlevtcmd_PhaseEventStartDeclare, 0xFE363C83, 0x0005005B, (s32)btlevtcmd_GetPos,
    0xFE363C83, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0004005B, (s32)btlevtcmd_FaceDirectionSub, 0xFE363C83, 0xFE363C80,
    0x0000000F, 0x0005005B, (s32)btlevtcmd_SetPos, 0xFFFFFFFE,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0000006B,
    0x0003005B, (s32)btlevtcmd_OffAttribute, 0xFE363C83, 0x00000010,
    0x0004005B, (s32)btlevtcmd_OffPartsAttribute, 0xFE363C83, 0x00000001,
    0x03000000, 0x0007005B, (s32)battle_evt_majo_disp_on, 0xFE363C83,
    0x00000001, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000001, 0x0005005B, (s32)btlevtcmd_SetPos, 0xFE363C83,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)btlevtcmd_GetHomePos, 0xFE363C83, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0003005B, (s32)btlevtcmd_SetMoveSpeed, 0xFE363C83,
    0xF24A8280, 0x0008005B, (s32)btlevtcmd_MovePosition, 0xFE363C83,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000000,
    0xFFFFFFFF, 0x00000000, 0x0000006D, 0x0004005B,
    (s32)btlevtcmd_OffPartsAttribute, 0xFFFFFFFE, 0x00000001, 0x00010000,
    0x0004005B, (s32)btlevtcmd_OffPartsAttribute, 0xFFFFFFFE, 0x00000001,
    0x01000000, 0x0007005B, (s32)battle_evt_majo_disp_on, 0xFFFFFFFE,
    0x00000001, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000001, 0x0005005B, (s32)btlevtcmd_SetPos, 0xFFFFFFFE,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)btlevtcmd_GetHomePos, 0xFFFFFFFE, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0007005B, (s32)btlevtcmd_GetMoveFrame, 0xFFFFFFFE,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0xF24A8280,
    0xFE363C8F, 0x0001006F, 0xFE363C8E, 0x0008005B,
    (s32)btlevtcmd_MovePosition, 0xFFFFFFFE, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0xFE363C8F, 0xFFFFFFFF, 0x00000000,
    0x00000070, 0x0006005B, (s32)btlevtcmd_ACRStart, 0xFFFFFFFE,
    0x00000000, 0xFE363C8F, 0xFE363C8F, 0x0000000F,
    0x0003005B, (s32)btlevtcmd_ACRGetResult, 0xFE363C86, 0xFE363C87,
    0x00010022, 0xFE363C86, 0x00010029, 0x00000002,
    0x0001005F, 0xFE363C8E, 0x0006005B, (s32)btlevtcmd_AudienceDeclareAcrobatResult,
    (s32)&partyWeapon_VivianShadowGuard, 0x00000001, 0x00000000, 0x00000000,
    0x00000000, 0x0004005B, (s32)btlevtcmd_AnimeChangePose, 0xFFFFFFFE,
    0x00000001, (s32)"PTR_Y_1", 0x0005005B, (s32)btlevtcmd_GetHomePos,
    0xFFFFFFFE, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0003005B, (s32)btlevtcmd_SetMoveSpeed, 0xFFFFFFFE, 0xF24A8280,
    0x0008005B, (s32)btlevtcmd_MovePosition, 0xFFFFFFFE, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000000, 0xFFFFFFFF,
    0x00000000, 0x0003005B, (s32)btlevtcmd_AnimeWaitPlayComplete, 0xFFFFFFFE,
    0x00000001, 0x0000002A, 0x0001005B, (s32)evt_audience_acrobat_notry,
    0x0002005B, (s32)btlevtcmd_WaitEventEnd, 0xFE363C8E, 0x00000031,
    0x0001005B, (s32)evt_audience_ap_recovery, 0x0001005B, (s32)btlevtcmd_InviteApInfoReport,
    0x0002005B, (s32)btlevtcmd_StartWaitEvent, 0xFE363C83, 0x0002005B,
    (s32)btlevtcmd_StartWaitEvent, 0xFFFFFFFE, 0x00000021, 0x00000002,
    0x00000001,
};

// TODO: type
u32 partyWeapon_VivianNormalAttack[48] = {
    (u32)"btl_wn_ptr_normal", 0x01500000, 0x00000000, (u32)"msg_ptr_kagenuke",
    0x64000000, 0x3F800000, 0x01016432, (u32)weaponGetPowerFromPartyAttackLv,
    0x00000003, 0x00000003, 0x00000004, 0x00000004,
    0x00000005, 0x00000005, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20000000, 0x00000302,
    (u32)"msg_ac_kagenuke", 0x00030007, 0x00000002, 0x80002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000064,
    0x02000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&partyVivianAttack_NormalAttack, 0x00000064, 0x00020200, 0x00000000,
};

// TODO: type
u32 partyWeapon_VivianShadowGuard[48] = {
    (u32)"btl_wn_ptr_lv1", 0x01510000, 0x00000000, (u32)"msg_ptr_kagegakure",
    0x64010000, 0x3F800000, 0x02056432, 0x00000000,
    0x00000000, 0x00000002, 0x00000002, 0x00000003,
    0x00000003, 0x00000004, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101160, 0x10000002, 0x00000302,
    (u32)"msg_ac_kagegakure", 0x00020008, 0x000007FF, 0x80002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&partyVivianAttack_ShadowGuard, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 partyWeapon_VivianMagicalPowder[48] = {
    (u32)"btl_wn_ptr_lv2", 0x01520000, 0x00000000, (u32)"msg_ptr_mahou_no_kona",
    0x64060000, 0x3F800000, 0x01056432, (u32)weaponGetACOutputParam,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x01000302,
    (u32)"msg_ac_mahou_no_kona", 0x00020047, 0x000007FF, 0x80002000,
    0x00000000, 0x00000000, 0x00000000, 0x00000064,
    0x03000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&partyVivianAttack_MagicalPowder, 0x00000064, 0x00050500, 0x00000000,
};

// TODO: type
u32 partyWeapon_VivianCharmKissAttack[48] = {
    (u32)"btl_wn_ptr_lv3", 0x01530000, 0x00000000, (u32)"msg_ptr_meromero_kiss",
    0x64040000, 0x3F800000, 0x02056432, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x00000302,
    (u32)"msg_ac_meromero_kiss", 0x00020000, 0x000007FF, 0x80002004,
    0x00000000, 0x00000000, 0x00640300, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&partyVivianAttack_CharmKissAttack, 0x00000064, 0x00050500, 0x00000000,
};

// TODO: type
s32 partyVivianAttack_NormalAttack[581] = {
    0x0005005B, (s32)btlevtcmd_JumpSetting, 0xFFFFFFFE, 0x00000014,
    0xF24A7A80, 0xF24A7D4C, 0x0003005B, (s32)btlevtcmd_GetSelectEnemy,
    0xFE363C83, 0xFE363C84, 0x00020018, 0xFE363C83,
    0xFFFFFFFF, 0x00010004, 0x00000063, 0x00000021,
    0x0003005B, (s32)btlevtcmd_CommandGetWeaponAddress, 0xFFFFFFFE, 0xFE363C8C,
    0x0002005B, (s32)btlevtcmd_WeaponAftereffect, 0xFE363C8C, 0x0003005B,
    (s32)btlevtcmd_GetPartyTechLv, 0xFFFFFFFE, 0xFE363C80, 0x00010022,
    0xFE363C80, 0x00010024, 0x00000002, 0x0003005B,
    (s32)_make_kagenuke_weapon, 0xFE363C8C, 0x00000002, 0x00010024,
    0x00000001, 0x0003005B, (s32)_make_kagenuke_weapon, 0xFE363C8C,
    0x00000002, 0x0000002A, 0x0003005B, (s32)_make_kagenuke_weapon,
    0xFE363C8C, 0x00000002, 0x00000031, 0x0004005B,
    (s32)btlevtcmd_AttackDeclare, 0xFFFFFFFE, 0xFE363C83, 0xFE363C84,
    0x0001005B, (s32)btlevtcmd_WaitGuardMove, 0x0002005B, (s32)btlevtcmd_CommandPayWeaponCost,
    0xFFFFFFFE, 0x0003005B, (s32)btlevtcmd_RunDataEventChild, 0xFFFFFFFE,
    0x00000007, 0x0003005B, (s32)evt_btl_camera_set_mode, 0x00000000,
    0x0000000B, 0x0006005B, (s32)evt_btl_camera_set_homing_unitparts, 0x00000000,
    0xFFFFFFFE, 0x00000001, 0xFE363C83, 0xFE363C84,
    0x0003005B, (s32)evt_btl_camera_set_moveSpeedLv, 0x00000000, 0x00000002,
    0x0003005B, (s32)evt_btl_camera_set_zoom, 0x00000000, 0x000000C8,
    0x0002005B, (s32)btlevtcmd_CommandGetWeaponActionLv, 0xFE363C80, 0x0003005B,
    (s32)btlevtcmd_AcSetDifficulty, 0xFFFFFFFE, 0xFE363C80, 0x0009005B,
    (s32)btlevtcmd_AcSetParamAll, 0x00000001, 0x00000001, 0x00000028,
    0x00000001, 0x00000002, 0x00000000, 0x00000003,
    0x00000000, 0x0002005B, (s32)btlevtcmd_AcSetFlag, 0x00000017,
    0x0005005B, (s32)btlevtcmd_SetupAC, 0xFFFFFFFE, 0x00000012,
    0x00000001, 0x00000000, 0x0003005B, (s32)evt_snd_sfxon,
    (s32)"SFX_BTL_VIVIAN_ATTACK1", 0x00000000, 0x0005005B, (s32)battle_evt_majo_disp_off,
    0xFFFFFFFE, 0x00000001, 0x00000000, 0x00000000,
    0x0002005B, (s32)btlevtcmd_StartAC, 0x00000001, 0x0006005B,
    (s32)btlevtcmd_GetHitPos, 0xFE363C83, 0xFE363C84, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)unk_8017d03c,
    0xFE363C83, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)btlevtcmd_GetPos, 0xFFFFFFFE, 0xF1194D80,
    0xFE363C81, 0xF1194D80, 0x000A005B, (s32)btlevtcmd_DivePosition,
    0xFFFFFFFE, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000032, 0x00000000, 0x00000005, 0x00000000,
    0xFFFFFFFF, 0x0004005B, (s32)btlevtcmd_SetUnitWork, 0xFFFFFFFE,
    0x00000003, 0x00000001, 0x0004005B, (s32)btlevtcmd_TransStageFloorPosition,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0006005B,
    (s32)btlevtcmd_snd_se, 0xFFFFFFFE, (s32)"SFX_BTL_VIVIAN_ATTACK2", 0xF1194D80,
    0x00000000, 0xF1194D80, 0x0007005B, (s32)battle_evt_majo_disp_on,
    0xFFFFFFFE, 0x00000001, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000000, 0x0005005B, (s32)btlevtcmd_SetPos,
    0xFFFFFFFE, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0006005B, (s32)btlevtcmd_GetHitPos, 0xFE363C83, 0xFE363C84,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0006005B,
    (s32)btlevtcmd_GetHitPos, 0xFE363C83, 0xFE363C84, 0xFE363C86,
    0xFE363C87, 0xFE363C88, 0x0005005B, (s32)unk_8017d03c,
    0xFE363C83, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)unk_8017d03c, 0xFE363C83, 0xFE363C86,
    0xFE363C87, 0xFE363C88, 0x0004005B, (s32)btlevtcmd_TransStageFloorPosition,
    0xFE363C86, 0xFE363C87, 0xFE363C88, 0x0003005B,
    (s32)btlevtcmd_GetFaceDirection, 0xFE363C83, 0xFE363C8F, 0x00020037,
    0xFE363C8F, 0xFFFFFFFF, 0x0003005B, (s32)btlevtcmd_ChangeFaceDirection,
    0xFFFFFFFE, 0xFE363C8F, 0x0005005B, (s32)btlevtcmd_GetPos,
    0xFFFFFFFE, 0xFE363C86, 0xFE363C87, 0xFE363C88,
    0x0009005B, (s32)_get_move_frame, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0xFE363C86, 0xFE363C87, 0xFE363C88,
    0xF24A8A80, 0xFE363C86, 0x0002001D, 0xFE363C86,
    0x00000001, 0x0006005B, (s32)btlevtcmd_snd_se, 0xFFFFFFFE,
    (s32)"SFX_BTL_VIVIAN_ATTACK3", 0xF1194D80, 0x00000000, 0xF1194D80,
    0x000A005B, (s32)btlevtcmd_DivePosition, 0xFFFFFFFE, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0000000C, 0x00000000,
    0x00000005, 0x00000000, 0xFFFFFFFF, 0x00000021,
    0x00010003, 0x00000009, 0x0006005B, (s32)btlevtcmd_CommandPreCheckDamage,
    0xFFFFFFFE, 0xFE363C83, 0xFE363C84, 0x00000100,
    0xFE363C85, 0x00020018, 0xFE363C85, 0x00000005,
    0x0007005B, (s32)btlevtcmd_CheckDamage, 0xFFFFFFFE, 0xFE363C83,
    0xFE363C84, 0xFE363C8C, 0x00000100, 0xFE363C85,
    0x00000021, 0x0001005B, (s32)btlevtcmd_ResultAC, 0x00020032,
    0xFE363C8A, 0x00000000, 0x00010003, 0x0000000A,
    0x0002001D, 0xFE363C8A, 0x00000001, 0x0007005B,
    (s32)btlevtcmd_PreCheckDamage, 0xFFFFFFFE, 0xFE363C83, 0xFE363C84,
    0xFE363C8C, 0x00100100, 0xFE363C85, 0x00000020,
    0x0007005B, (s32)btlevtcmd_PreCheckDamage, 0xFFFFFFFE, 0xFE363C83,
    0xFE363C84, 0xFE363C8C, 0x00000100, 0xFE363C85,
    0x00000021, 0x00010022, 0xFE363C85, 0x0001002B,
    0x00000002, 0x0001002B, 0x00000003, 0x0001002B,
    0x00000006, 0x0001002B, 0x00000004, 0x0000002E,
    0x00010024, 0x00000001, 0x0000002A, 0x00000031,
    0x00020019, 0xFE363C85, 0x00000001, 0x0004005B,
    (s32)btlevtcmd_AnimeChangePose, 0xFFFFFFFE, 0x00000001, (s32)"PTR_A1_2",
    0x0001000A, 0x000000A6, 0x00020018, 0xFE363C85,
    0x00000003, 0x0003005B, (s32)btlevtcmd_StartAvoid, 0xFE363C83,
    0x00000026, 0x00000021, 0x00020018, 0xFE363C85,
    0x00000006, 0x0003005B, (s32)btlevtcmd_StartAvoid, 0xFE363C83,
    0x00000027, 0x00000021, 0x00020018, 0xFE363C85,
    0x00000002, 0x0003005B, (s32)btlevtcmd_StartAvoid, 0xFE363C83,
    0x00000028, 0x00000021, 0x0001000A, 0x000001F4,
    0x00010004, 0x0000005A, 0x00000021, 0x0004005B,
    (s32)btlevtcmd_AnimeChangePose, 0xFFFFFFFE, 0x00000001, (s32)"PTR_A1_2",
    0x0003005B, (s32)btlevtcmd_ftomsec, 0x00000004, 0xFE363C80,
    0x0001000A, 0xFE363C80, 0x0003005B, (s32)evt_snd_sfxon,
    (s32)"SFX_BTL_VIVIAN_ATTACK4", 0x00000000, 0x0002005B, (s32)btlevtcmd_GetResultAC,
    0xFE363C80, 0x0002001E, 0xFE363C80, 0x00000002,
    0x0007005B, (s32)btlevtcmd_CheckDamage, 0xFFFFFFFE, 0xFE363C83,
    0xFE363C84, 0xFE363C8C, 0x00020100, 0xFE363C85,
    0x00000020, 0x0007005B, (s32)btlevtcmd_CheckDamage, 0xFFFFFFFE,
    0xFE363C83, 0xFE363C84, 0xFE363C8C, 0x20000100,
    0xFE363C85, 0x00000021, 0x0002001E, 0xFE363C80,
    0x00000002, 0x0004005B, (s32)btlevtcmd_GetResultPrizeLv, 0xFE363C83,
    0x00000000, 0xFE363C86, 0x0006005B, (s32)btlevtcmd_GetHitPos,
    0xFE363C83, 0xFE363C84, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)btlevtcmd_ACSuccessEffect, 0xFE363C86,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0003005B,
    (s32)btlevtcmd_AudienceDeclareACResult, 0xFE363C8C, 0xFE363C86, 0x00000020,
    0x0003005B, (s32)btlevtcmd_AudienceDeclareACResult, 0xFE363C8C, 0xFFFFFFFF,
    0x00000021, 0x0001000A, 0x000001F4, 0x00010003,
    0x0000005A, 0x0001005B, (s32)btlevtcmd_StopAC, 0x00010003,
    0x0000005F, 0x0003005B, (s32)evt_btl_camera_set_mode, 0x00000000,
    0x00000000, 0x0003005B, (s32)evt_btl_camera_set_moveSpeedLv, 0x00000000,
    0x00000001, 0x0004005B, (s32)btlevtcmd_AnimeChangePose, 0xFFFFFFFE,
    0x00000001, (s32)"PTR_A1_1", 0x0005005B, (s32)btlevtcmd_GetPos,
    0xFFFFFFFE, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0004005B, (s32)btlevtcmd_TransStageFloorPosition, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x000A005B, (s32)btlevtcmd_DivePosition, 0xFFFFFFFE,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0000000C,
    0x00000000, 0x00000005, 0x00000000, 0xFFFFFFFF,
    0x0004005B, (s32)btlevtcmd_SetUnitWork, 0xFFFFFFFE, 0x00000003,
    0x00000000, 0x0005005B, (s32)battle_evt_majo_disp_off, 0xFFFFFFFE,
    0x00000001, 0x00000000, 0x00000000, 0x0005005B,
    (s32)btlevtcmd_GetHomePos, 0xFFFFFFFE, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)btlevtcmd_GetPos, 0xFFFFFFFE,
    0xF1194D80, 0xFE363C81, 0xF1194D80, 0x000A005B,
    (s32)btlevtcmd_DivePosition, 0xFFFFFFFE, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000028, 0x00000000, 0x00000005,
    0x00000000, 0xFFFFFFFF, 0x0000006E, 0x0006005B,
    (s32)btlevtcmd_ACRStart, 0xFFFFFFFE, 0x00000014, 0x0000001D,
    0x0000001D, 0x00000000, 0x00000070, 0x0005005B,
    (s32)btlevtcmd_GetHomePos, 0xFFFFFFFE, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0007005B, (s32)battle_evt_majo_disp_on, 0xFFFFFFFE,
    0x00000001, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000000, 0x0005005B, (s32)btlevtcmd_SetPos, 0xFFFFFFFE,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0003005B,
    (s32)btlevtcmd_ACRGetResult, 0xFE363C86, 0xFE363C87, 0x00020018,
    0xFE363C86, 0x00000002, 0x0003005B, (s32)btlevtcmd_CommandGetWeaponAddress,
    0xFFFFFFFE, 0xFE363C80, 0x0006005B, (s32)btlevtcmd_AudienceDeclareAcrobatResult,
    0xFE363C80, 0x00000001, 0x00000000, 0x00000000,
    0x00000000, 0x0004005B, (s32)btlevtcmd_AnimeChangePose, 0xFFFFFFFE,
    0x00000001, (s32)"PTR_Y_1", 0x00010009, 0x00000034,
    0x00000021, 0x00010003, 0x00000063, 0x0001005B,
    (s32)evt_audience_ap_recovery, 0x0001005B, (s32)btlevtcmd_InviteApInfoReport, 0x0002005B,
    (s32)btlevtcmd_ResetFaceDirection, 0xFFFFFFFE, 0x0001005E, (s32)btldefaultevt_SuitoruBadgeEffect,
    0x0002005B, (s32)btlevtcmd_StartWaitEvent, 0xFFFFFFFE, 0x00000002,
    0x00000001,
};

// TODO: type
s32 partyVivianAttack_ShadowGuard[430] = {
    0x00020032, 0xFE363C83, 0xFFFFFFFD, 0x0003005B,
    (s32)btlevtcmd_GetBodyId, 0xFE363C83, 0xFE363C84, 0x0002005B,
    (s32)btlevtcmd_CommandPayWeaponCost, 0xFFFFFFFE, 0x0003005B, (s32)btlevtcmd_CommandGetWeaponAddress,
    0xFFFFFFFE, 0xFE363C8C, 0x0003005B, (s32)btlevtcmd_RunDataEventChild,
    0xFFFFFFFE, 0x00000007, 0x0003005B, (s32)evt_btl_camera_set_mode,
    0x00000000, 0x00000007, 0x0004005B, (s32)evt_btl_camera_set_homing_unit,
    0x00000000, 0xFFFFFFFE, 0x00000001, 0x0003005B,
    (s32)evt_btl_camera_set_moveSpeedLv, 0x00000000, 0x00000002, 0x0003005B,
    (s32)evt_btl_camera_set_zoom, 0x00000000, 0x000000FA, 0x0005005B,
    (s32)evt_btl_camera_set_posoffset, 0x00000000, 0x00000014, 0x00000000,
    0x00000000, 0x0007005B, (s32)btlevtcmd_CalculateFaceDirection, 0xFFFFFFFE,
    0xFFFFFFFF, 0xFE363C83, 0xFFFFFFFF, 0x00000010,
    0xFE363C80, 0x0003005B, (s32)btlevtcmd_ChangeFaceDirection, 0xFFFFFFFE,
    0xFE363C80, 0x0004005B, (s32)btlevtcmd_AnimeChangePose, 0xFFFFFFFE,
    0x00000001, (s32)"PTR_A2_1", 0x0005005B, (s32)btlevtcmd_GetPos,
    0xFE363C83, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0004005B, (s32)btlevtcmd_FaceDirectionSub, 0xFE363C83, 0xFE363C80,
    0x0000000F, 0x00020035, 0xFE363C82, 0x00000005,
    0x0003005B, (s32)btlevtcmd_SetMoveSpeed, 0xFFFFFFFE, 0xF24A8280,
    0x0008005B, (s32)btlevtcmd_MovePosition, 0xFFFFFFFE, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000000, 0xFFFFFFFF,
    0x00000000, 0x0007005B, (s32)btlevtcmd_CalculateFaceDirection, 0xFFFFFFFE,
    0xFFFFFFFF, 0xFE363C83, 0xFFFFFFFF, 0x00000010,
    0xFE363C80, 0x0003005B, (s32)btlevtcmd_ChangeFaceDirection, 0xFFFFFFFE,
    0xFE363C80, 0x0002005B, (s32)btlevtcmd_CommandGetWeaponActionLv, 0xFE363C80,
    0x0003005B, (s32)btlevtcmd_AcSetDifficulty, 0xFFFFFFFE, 0xFE363C80,
    0x0002005B, (s32)btlevtcmd_CommandGetWeaponActionLv, 0xFE363C80, 0x0003005B,
    (s32)btlevtcmd_AcSetDifficulty, 0xFFFFFFFE, 0xFE363C80, 0x0002005B,
    (s32)btlevtcmd_AcGetDifficulty, 0xFE363C80, 0x00020036, 0xFE363C80,
    0x00000003, 0x00010022, 0xFE363C80, 0x00010028,
    0xFFFFFFFD, 0x00020032, 0xFE363C80, 0x000001A4,
    0x00010024, 0xFFFFFFFE, 0x00020032, 0xFE363C80,
    0x0000014A, 0x00010024, 0xFFFFFFFF, 0x00020032,
    0xFE363C80, 0x0000010E, 0x00010024, 0x00000000,
    0x00020032, 0xFE363C80, 0x000000F0, 0x00010024,
    0x00000001, 0x00020032, 0xFE363C80, 0x000000C8,
    0x00010024, 0x00000002, 0x00020032, 0xFE363C80,
    0x000000A0, 0x0000002A, 0x00020032, 0xFE363C80,
    0x00000078, 0x00000031, 0x0009005B, (s32)btlevtcmd_AcSetParamAll,
    0xFE363C80, 0x00000001, 0x00000005, 0xFFFFFFFE,
    0x00000005, 0xF1194D80, 0xF1194D80, 0xF1194D80,
    0x0002005B, (s32)btlevtcmd_AcSetFlag, 0x00000000, 0x0005005B,
    (s32)btlevtcmd_SetupAC, 0xFFFFFFFE, 0x0000000A, 0x00000001,
    0x00000000, 0x00010009, 0x00000016, 0x0002005B,
    (s32)btlevtcmd_StartAC, 0x00000001, 0x0001005B, (s32)btlevtcmd_ResultAC,
    0x0002005B, (s32)btlevtcmd_GetResultAC, 0xFE363C80, 0x0002001F,
    0xFE363C80, 0x00000002, 0x0003005B, (s32)btlevtcmd_AudienceDeclareACResult,
    0xFE363C8C, 0xFFFFFFFF, 0x0001005B, (s32)btlevtcmd_StopAC,
    0x0005005B, (s32)btlevtcmd_GetHomePos, 0xFFFFFFFE, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0003005B, (s32)btlevtcmd_SetMoveSpeed,
    0xFFFFFFFE, 0xF24A8280, 0x0008005B, (s32)btlevtcmd_MovePosition,
    0xFFFFFFFE, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000000, 0xFFFFFFFF, 0x00000000, 0x00010004,
    0x0000005A, 0x00000021, 0x0004005B, (s32)btlevtcmd_GetResultPrizeLv,
    0xFFFFFFFF, 0x00000000, 0xFE363C86, 0x0006005B,
    (s32)btlevtcmd_GetHitPos, 0xFE363C83, 0xFE363C84, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)btlevtcmd_ACSuccessEffect,
    0xFE363C86, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0003005B, (s32)btlevtcmd_AudienceDeclareACResult, 0xFE363C8C, 0xFE363C86,
    0x0001005B, (s32)btlevtcmd_StopAC, 0x0003005B, (s32)evt_btl_camera_set_moveSpeedLv,
    0x00000000, 0x00000002, 0x0003005B, (s32)evt_btl_camera_set_zoom,
    0x00000000, 0x0000015E, 0x0005005B, (s32)evt_btl_camera_set_posoffset,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0004005B, (s32)btlevtcmd_AnimeChangePose, 0xFFFFFFFE, 0x00000001,
    (s32)"PTR_A2_2", 0x0003005B, (s32)btlevtcmd_AnimeWaitPlayComplete, 0xFFFFFFFE,
    0x00000001, 0x0004005B, (s32)btlevtcmd_AnimeChangePose, 0xFFFFFFFE,
    0x00000001, (s32)"PTR_A2_3", 0x0003005B, (s32)btlevtcmd_AnimeWaitPlayComplete,
    0xFFFFFFFE, 0x00000001, 0x0000006E, 0x0004005B,
    (s32)btlevtcmd_AnimeChangePose, 0xFE363C83, 0x00000001, (s32)"M_B_3",
    0x0005005B, (s32)battle_evt_majo_disp_off, 0xFE363C83, 0x00000001,
    0x00000001, 0x00000000, 0x0003005B, (s32)btlevtcmd_OnAttribute,
    0xFE363C83, 0x00000010, 0x0004005B, (s32)btlevtcmd_OnPartsAttribute,
    0xFE363C83, 0x00000001, 0x03000000, 0x00010009,
    0x00000001, 0x0005005B, (s32)btlevtcmd_GetHomePos, 0xFE363C83,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)btlevtcmd_SetPos, 0xFE363C83, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000070, 0x0005005B, (s32)battle_evt_majo_disp_off,
    0xFFFFFFFE, 0x00000001, 0x00000001, 0x00000001,
    0x0005005B, (s32)btlevtcmd_GetPos, 0xFFFFFFFE, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)btlevtcmd_GetPos,
    0xFE363C83, 0xFE363C85, 0xFE363C81, 0xFE363C82,
    0x00020035, 0xFE363C80, 0xFE363C85, 0x00020038,
    0xFE363C80, 0x00000002, 0x0004005B, (s32)btlevtcmd_TransStageFloorPosition,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00020035,
    0xFE363C81, 0x00000001, 0x000F005B, (s32)evt_eff,
    0x00000000, (s32)"ripple", 0x00000001, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0001000A, 0x0000014D,
    0x0004005B, (s32)btlevtcmd_OnPartsAttribute, 0xFFFFFFFE, 0x00000001,
    0x01010000, 0x00010009, 0x00000001, 0x0004005B,
    (s32)btlevtcmd_SetUnitWork, 0xFFFFFFFE, 0x00000004, 0x00000001,
    0x0005005B, (s32)btlevtcmd_GetPos, 0xFFFFFFFE, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0004005B, (s32)btlevtcmd_TransStageFloorPosition,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)btlevtcmd_SetPos, 0xFFFFFFFE, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00010009, 0x00000001, 0x0007005B,
    (s32)btlevtcmd_CalculateFaceDirection, 0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0x00000004, 0xFE363C80, 0x0003005B,
    (s32)btlevtcmd_ChangeFaceDirection, 0xFFFFFFFE, 0xFE363C80, 0x0001000A,
    0x000001F4, 0x00010003, 0x0000005A, 0x0001005B,
    (s32)evt_audience_ap_recovery, 0x0001005B, (s32)btlevtcmd_InviteApInfoReport, 0x00010003,
    0x0000005F, 0x0003005B, (s32)evt_btl_camera_set_mode, 0x00000000,
    0x00000000, 0x0003005B, (s32)evt_btl_camera_set_moveSpeedLv, 0x00000000,
    0x00000001, 0x0005005B, (s32)evt_btl_camera_set_posoffset, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00010003,
    0x00000063, 0x0002005B, (s32)btlevtcmd_StartWaitEvent, 0xFFFFFFFE,
    0x00000002, 0x00000001,
};

// TODO: type
s32 partyVivianAttack_MagicalPowder[442] = {
    0x0003005B, (s32)btlevtcmd_GetSelectEnemy, 0xFE363C83, 0xFE363C84,
    0x00020018, 0xFE363C83, 0xFFFFFFFF, 0x00010004,
    0x00000063, 0x00000021, 0x0003005B, (s32)btlevtcmd_CommandGetWeaponAddress,
    0xFFFFFFFE, 0xFE363C8C, 0x0002005B, (s32)btlevtcmd_WeaponAftereffect,
    0xFE363C8C, 0x0002005B, (s32)btlevtcmd_AttackDeclareAll, 0xFFFFFFFE,
    0x0001005B, (s32)btlevtcmd_WaitGuardMove, 0x0002005B, (s32)btlevtcmd_CommandPayWeaponCost,
    0xFFFFFFFE, 0x0003005B, (s32)btlevtcmd_RunDataEventChild, 0xFFFFFFFE,
    0x00000007, 0x00010009, 0x00000014, 0x0002005B,
    (s32)btlevtcmd_CommandGetWeaponActionLv, 0xFE363C80, 0x0003005B, (s32)btlevtcmd_AcSetDifficulty,
    0xFFFFFFFE, 0xFE363C80, 0x0002005B, (s32)btlevtcmd_AcGetDifficulty,
    0xFE363C80, 0x00020036, 0xFE363C80, 0x00000003,
    0x00010022, 0xFE363C80, 0x00010028, 0xFFFFFFFD,
    0x00020032, 0xFE363C80, 0x00000168, 0x00010024,
    0xFFFFFFFE, 0x00020032, 0xFE363C80, 0x0000010E,
    0x00010024, 0xFFFFFFFF, 0x00020032, 0xFE363C80,
    0x000000D2, 0x00010024, 0x00000000, 0x00020032,
    0xFE363C80, 0x000000B4, 0x00010024, 0x00000001,
    0x00020032, 0xFE363C80, 0x000000A0, 0x00010024,
    0x00000002, 0x00020032, 0xFE363C80, 0x0000008C,
    0x0000002A, 0x00020032, 0xFE363C80, 0x00000078,
    0x00000031, 0x0009005B, (s32)btlevtcmd_AcSetParamAll, 0xFE363C80,
    0x00000001, 0x00000005, 0xFFFFFFFD, 0x00000001,
    0xF1194D80, 0xF1194D80, 0xF1194D80, 0x0002005B,
    (s32)btlevtcmd_AcSetFlag, 0x00000007, 0x0005005B, (s32)btlevtcmd_SetupAC,
    0xFFFFFFFE, 0x0000000A, 0x00000001, 0x00000000,
    0x00010009, 0x00000016, 0x0003005B, (s32)evt_btl_camera_set_mode,
    0x00000000, 0x00000007, 0x0004005B, (s32)evt_btl_camera_set_homing_unit,
    0x00000000, 0xFFFFFFFE, 0x00000001, 0x0003005B,
    (s32)evt_btl_camera_set_moveSpeedLv, 0x00000000, 0x00000004, 0x0003005B,
    (s32)evt_btl_camera_set_zoom, 0x00000000, 0x00000190, 0x0005005B,
    (s32)evt_btl_camera_set_posoffset, 0x00000000, 0x00000014, 0x00000000,
    0x00000000, 0x0002005B, (s32)btlevtcmd_StartAC, 0x00000001,
    0x0001005B, (s32)btlevtcmd_ResultAC, 0x0001005B, (s32)btlevtcmd_StopAC,
    0x0000006E, 0x0005005B, (s32)btlevtcmd_GetPos, 0xFFFFFFFE,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0003005B,
    (s32)btlevtcmd_GetWidth, 0xFFFFFFFE, 0xFE363C85, 0x0003005B,
    (s32)btlevtcmd_GetHeight, 0xFFFFFFFE, 0xFE363C86, 0x000F005B,
    (s32)evt_eff, 0x00000000, (s32)"stardust", 0x00000004,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0xFE363C85,
    0xFE363C86, 0x0000001E, 0x0000003C, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000070,
    0x0006005B, (s32)btlevtcmd_snd_se, 0xFFFFFFFE, (s32)"SFX_BTL_VIVIAN_MAGIC1",
    0xF1194D80, 0x00000000, 0xF1194D80, 0x0004005B,
    (s32)btlevtcmd_AnimeChangePose, 0xFFFFFFFE, 0x00000001, (s32)"PTR_A3_1",
    0x0003005B, (s32)btlevtcmd_ftof, 0x00000014, 0xFE363C80,
    0x00010009, 0x00000014, 0x0003005B, (s32)btlevtcmd_ftof,
    0x0000001A, 0xFE363C80, 0x0003005B, (s32)btlevtcmd_ftof,
    0x0000002E, 0xFE363C81, 0x0006005B, (s32)btlevtcmd_ACRStart,
    0xFFFFFFFE, 0xFE363C80, 0xFE363C81, 0xFE363C81,
    0x00000000, 0x0003005B, (s32)btlevtcmd_ACRGetResult, 0xFE363C86,
    0xFE363C87, 0x00010022, 0xFE363C86, 0x00010029,
    0x00000002, 0x0006005B, (s32)btlevtcmd_AudienceDeclareAcrobatResult, 0xFE363C8C,
    0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x0004005B, (s32)btlevtcmd_AnimeChangePose, 0xFFFFFFFE, 0x00000001,
    (s32)"PTR_Y_1", 0x0000002A, 0x0001005B, (s32)evt_audience_acrobat_notry,
    0x00000031, 0x0003005B, (s32)evt_btl_camera_set_mode, 0x00000000,
    0x00000000, 0x0003005B, (s32)evt_btl_camera_set_moveSpeedLv, 0x00000000,
    0x00000001, 0x00010009, 0x00000028, 0x00020032,
    0xFE363C8A, 0x00000000, 0x00010003, 0x0000000A,
    0x0006005B, (s32)btlevtcmd_GetHitPos, 0xFE363C83, 0xFE363C84,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x000F005B,
    (s32)evt_eff, 0x00000000, (s32)"kemuri_test", 0x00000006,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0xF24A7E80,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0006005B,
    (s32)btlevtcmd_CommandPreCheckDamage, 0xFFFFFFFE, 0xFE363C83, 0xFE363C84,
    0x00000100, 0xFE363C86, 0x00020019, 0xFE363C86,
    0x00000001, 0x00020018, 0xFE363C86, 0x00000003,
    0x0003005B, (s32)btlevtcmd_StartAvoid, 0xFE363C83, 0x00000026,
    0x00000021, 0x00020018, 0xFE363C86, 0x00000006,
    0x0003005B, (s32)btlevtcmd_StartAvoid, 0xFE363C83, 0x00000027,
    0x00000021, 0x00020018, 0xFE363C86, 0x00000002,
    0x0003005B, (s32)btlevtcmd_StartAvoid, 0xFE363C83, 0x00000028,
    0x00000021, 0x00010004, 0x00000032, 0x00000021,
    0x0003005B, (s32)btlevtcmd_AcGetOutputParam, 0x00000000, 0xFE363C80,
    0x0002001C, 0xFE363C80, 0x00000003, 0x0006005B,
    (s32)btlevtcmd_CommandCheckDamage, 0xFFFFFFFE, 0xFE363C83, 0xFE363C84,
    0x00020100, 0xFE363C85, 0x00000020, 0x0006005B,
    (s32)btlevtcmd_CommandCheckDamage, 0xFFFFFFFE, 0xFE363C83, 0xFE363C84,
    0x00000100, 0xFE363C85, 0x00000021, 0x00020018,
    0xFE363C85, 0x00000012, 0x0003005B, (s32)btlevtcmd_AcGetOutputParam,
    0x00000000, 0xFE363C80, 0x00010022, 0xFE363C80,
    0x00010028, 0x00000000, 0x00020032, 0xFE363C80,
    0xFFFFFFFF, 0x0001002B, 0x00000001, 0x0001002B,
    0x00000002, 0x00020032, 0xFE363C80, 0x00000000,
    0x0000002E, 0x0001002B, 0x00000003, 0x0001002B,
    0x00000004, 0x00020032, 0xFE363C80, 0x00000001,
    0x0000002E, 0x0000002A, 0x00020032, 0xFE363C80,
    0x00000002, 0x00000031, 0x0002001D, 0xFE363C80,
    0x00000000, 0x0004005B, (s32)btlevtcmd_GetResultPrizeLv, 0xFE363C83,
    0xFE363C80, 0xFE363C86, 0x0006005B, (s32)btlevtcmd_GetHitPos,
    0xFE363C83, 0xFE363C84, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)btlevtcmd_ACSuccessEffect, 0xFE363C86,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0003005B,
    (s32)btlevtcmd_AudienceDeclareACResult, 0xFE363C8C, 0xFE363C86, 0x00000020,
    0x0003005B, (s32)btlevtcmd_AudienceDeclareACResult, 0xFE363C8C, 0xFFFFFFFF,
    0x00000021, 0x00000021, 0x00010003, 0x00000032,
    0x0003005B, (s32)btlevtcmd_GetSelectNextEnemy, 0xFE363C83, 0xFE363C84,
    0x00020019, 0xFE363C83, 0xFFFFFFFF, 0x00020035,
    0xFE363C8A, 0x00000001, 0x00010004, 0x0000000A,
    0x00000021, 0x00010003, 0x00000050, 0x0001005B,
    (s32)evt_audience_ap_recovery, 0x0001005B, (s32)btlevtcmd_InviteApInfoReport, 0x00010003,
    0x0000005A, 0x0003005B, (s32)evt_btl_camera_set_mode, 0x00000000,
    0x00000000, 0x0003005B, (s32)evt_btl_camera_set_moveSpeedLv, 0x00000000,
    0x00000001, 0x0005005B, (s32)evt_btl_camera_set_posoffset, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0004005B,
    (s32)btlevtcmd_AnimeChangePose, 0xFFFFFFFE, 0x00000001, (s32)"PTR_A1_1",
    0x0001000A, 0x00000320, 0x00010003, 0x00000063,
    0x0002005B, (s32)btlevtcmd_ResetFaceDirection, 0xFFFFFFFE, 0x0001005E,
    (s32)btldefaultevt_SuitoruBadgeEffect, 0x0002005B, (s32)btlevtcmd_StartWaitEvent, 0xFFFFFFFE,
    0x00000002, 0x00000001,
};

// TODO: type
s32 partyVivianAttack_CharmKissAttack[464] = {
    0x0003005B, (s32)btlevtcmd_GetSelectEnemy, 0xFE363C83, 0xFE363C84,
    0x00020018, 0xFE363C83, 0xFFFFFFFF, 0x00010004,
    0x00000063, 0x00000021, 0x0003005B, (s32)btlevtcmd_CommandGetWeaponAddress,
    0xFFFFFFFE, 0xFE363C8C, 0x0002005B, (s32)btlevtcmd_WeaponAftereffect,
    0xFE363C8C, 0x0002005B, (s32)btlevtcmd_AttackDeclareAll, 0xFFFFFFFE,
    0x0001005B, (s32)btlevtcmd_WaitGuardMove, 0x0002005B, (s32)btlevtcmd_CommandPayWeaponCost,
    0xFFFFFFFE, 0x0003005B, (s32)btlevtcmd_RunDataEventChild, 0xFFFFFFFE,
    0x00000007, 0x00010009, 0x00000014, 0x00020032,
    0xFE363C8A, 0x00000000, 0x00010003, 0x0000000A,
    0x0004005B, (s32)btlevtcmd_AnimeChangePose, 0xFFFFFFFE, 0x00000001,
    (s32)"PTR_A4_1", 0x0003005B, (s32)evt_snd_sfxon, (s32)"SFX_PARTY_MOJIMOJI1",
    0x00000000, 0x0002005B, (s32)btlevtcmd_CommandGetWeaponActionLv, 0xFE363C80,
    0x0003005B, (s32)btlevtcmd_AcSetDifficulty, 0xFFFFFFFE, 0xFE363C80,
    0x0002005B, (s32)btlevtcmd_AcGetDifficulty, 0xFE363C80, 0x00020036,
    0xFE363C80, 0x00000003, 0x00010022, 0xFE363C80,
    0x00010028, 0xFFFFFFFD, 0x00020032, 0xFE363C85,
    0x0000003C, 0x00010024, 0xFFFFFFFE, 0x00020032,
    0xFE363C85, 0x0000002D, 0x00010024, 0xFFFFFFFF,
    0x00020032, 0xFE363C85, 0x0000001E, 0x00010024,
    0x00000000, 0x00020032, 0xFE363C85, 0x00000019,
    0x00010024, 0x00000001, 0x00020032, 0xFE363C85,
    0x00000014, 0x00010024, 0x00000002, 0x00020032,
    0xFE363C85, 0x0000000C, 0x0000002A, 0x00020032,
    0xFE363C85, 0x00000006, 0x00000031, 0x0003005B,
    (s32)evt_sub_random, 0xFE363C81, 0x00000059, 0x00020035,
    0xFE363C81, 0x0000001E, 0x00020032, 0xFE363C82,
    0xFE363C81, 0x00020036, 0xFE363C82, 0xFE363C85,
    0x0009005B, (s32)btlevtcmd_AcSetParamAll, 0x00000005, 0xFE363C81,
    0xFE363C82, 0xFE363C81, 0xF1194D80, 0xF1194D80,
    0xF1194D80, 0xF1194D80, 0x0002001B, 0xFE363C8A,
    0x00000000, 0x0002005B, (s32)btlevtcmd_AcSetFlag, 0x00000009,
    0x00000020, 0x0002005B, (s32)btlevtcmd_AcSetFlag, 0x00000008,
    0x00000021, 0x0005005B, (s32)btlevtcmd_SetupAC, 0xFFFFFFFE,
    0x0000000B, 0x00000001, 0x00000000, 0x00010009,
    0x00000016, 0x0002001B, 0xFE363C8A, 0x00000000,
    0x0002005B, (s32)btlevtcmd_StartAC, 0x00000000, 0x00000020,
    0x0002005B, (s32)btlevtcmd_StartAC, 0x00000001, 0x00000021,
    0x0001006F, 0xFE363C8F, 0x00020032, 0xFE363C80,
    0x00000005, 0x00010005, 0xFE363C80, 0x0003005B,
    (s32)btlevtcmd_AcGetOutputParam, 0x00000002, 0xFE363C81, 0x00020019,
    0xFE363C81, 0x00000000, 0x0006005B, (s32)_disp_heart_entry,
    0x0000000A, 0x00000014, 0x00000019, 0xFE363C83,
    0xFE363C84, 0x00010009, 0x00000001, 0x00000007,
    0x00000021, 0x0002005B, (s32)_disp_heart_entry_stop_check, 0xFE363C81,
    0x00020019, 0xFE363C81, 0x00000000, 0x00000007,
    0x00000021, 0x00020032, 0xFE363C80, 0x00000005,
    0x00010009, 0x00000001, 0x00000006, 0x00000070,
    0x0001005B, (s32)btlevtcmd_ResultAC, 0x0002005B, (s32)btlevtcmd_GetResultAC,
    0xFE363C87, 0x0002006A, 0xFE363C8F, 0xFE363C80,
    0x00020019, 0xFE363C80, 0x00000000, 0x0002005B,
    (s32)_disp_heart_entry_stop, 0xFE363C8F, 0x00000021, 0x0004005B,
    (s32)btlevtcmd_AnimeChangePose, 0xFFFFFFFE, 0x00000001, (s32)"PTR_A4_2",
    0x0001000A, 0x000000C8, 0x0006005B, (s32)btlevtcmd_snd_se,
    0xFFFFFFFE, (s32)"SFX_BTL_VIVIAN_MEROMERO1", 0xF1194D80, 0x00000000,
    0xF1194D80, 0x0005005B, (s32)btlevtcmd_GetPos, 0xFFFFFFFE,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0004005B,
    (s32)btlevtcmd_FaceDirectionAdd, 0xFFFFFFFE, 0xFE363C80, 0x0000000F,
    0x00020035, 0xFE363C81, 0x0000001E, 0x0003005B,
    (s32)btlevtcmd_GetFaceDirection, 0xFFFFFFFE, 0xFE363C86, 0x00020037,
    0xFE363C86, 0x0000002D, 0x000F005B, (s32)evt_eff,
    0x00000000, (s32)"kiss", 0x00000004, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0xFE363C86, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0006005B, (s32)btlevtcmd_CommandPreCheckDamage,
    0xFFFFFFFE, 0xFE363C83, 0xFE363C84, 0x00000100,
    0xFE363C86, 0x00020019, 0xFE363C86, 0x00000001,
    0x00020018, 0xFE363C86, 0x00000003, 0x0003005B,
    (s32)btlevtcmd_StartAvoid, 0xFE363C83, 0x00000026, 0x00000021,
    0x00020018, 0xFE363C86, 0x00000006, 0x0003005B,
    (s32)btlevtcmd_StartAvoid, 0xFE363C83, 0x00000027, 0x00000021,
    0x00020018, 0xFE363C86, 0x00000002, 0x0003005B,
    (s32)btlevtcmd_StartAvoid, 0xFE363C83, 0x00000028, 0x00000021,
    0x00010004, 0x00000032, 0x00000021, 0x0002001E,
    0xFE363C87, 0x00000002, 0x0006005B, (s32)btlevtcmd_CommandCheckDamage,
    0xFFFFFFFE, 0xFE363C83, 0xFE363C84, 0x00020100,
    0xFE363C85, 0x00000020, 0x0006005B, (s32)btlevtcmd_CommandCheckDamage,
    0xFFFFFFFE, 0xFE363C83, 0xFE363C84, 0x20000100,
    0xFE363C85, 0x00000021, 0x00020018, 0xFE363C85,
    0x00000012, 0x0002001E, 0xFE363C87, 0x00000002,
    0x0006005B, (s32)btlevtcmd_GetHitPos, 0xFE363C83, 0xFE363C84,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00020035,
    0xFE363C82, 0x0000000A, 0x000F005B, (s32)evt_eff,
    0x00000000, (s32)"kiss", 0x00000005, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0004005B, (s32)btlevtcmd_GetResultPrizeLv,
    0xFE363C83, 0x00000000, 0xFE363C86, 0x0006005B,
    (s32)btlevtcmd_GetHitPos, 0xFE363C83, 0xFE363C84, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)btlevtcmd_ACSuccessEffect,
    0xFE363C86, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0003005B, (s32)btlevtcmd_AudienceDeclareACResult, 0xFE363C8C, 0xFE363C86,
    0x00000020, 0x0003005B, (s32)btlevtcmd_AudienceDeclareACResult, 0xFE363C8C,
    0xFFFFFFFF, 0x00000021, 0x00000021, 0x0000006E,
    0x0006005B, (s32)btlevtcmd_ACRStart, 0xFFFFFFFE, 0x00000000,
    0x0000001E, 0x0000001E, 0x00000000, 0x00000070,
    0x0003005B, (s32)btlevtcmd_AnimeWaitPlayComplete, 0xFFFFFFFE, 0x00000001,
    0x0003005B, (s32)btlevtcmd_ACRGetResult, 0xFE363C86, 0xFE363C87,
    0x00020018, 0xFE363C86, 0x00000002, 0x0003005B,
    (s32)btlevtcmd_CommandGetWeaponAddress, 0xFFFFFFFE, 0xFE363C80, 0x0006005B,
    (s32)btlevtcmd_AudienceDeclareAcrobatResult, 0xFE363C80, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x0004005B, (s32)btlevtcmd_AnimeChangePose,
    0xFFFFFFFE, 0x00000001, (s32)"PTR_Y_1", 0x00010009,
    0x00000020, 0x00000021, 0x00010009, 0x00000005,
    0x00010003, 0x00000032, 0x0003005B, (s32)btlevtcmd_GetSelectNextEnemy,
    0xFE363C83, 0xFE363C84, 0x00020019, 0xFE363C83,
    0xFFFFFFFF, 0x00020035, 0xFE363C8A, 0x00000001,
    0x00010004, 0x0000000A, 0x00000021, 0x00010003,
    0x00000050, 0x0001005B, (s32)btlevtcmd_StopAC, 0x0001005B,
    (s32)evt_audience_ap_recovery, 0x0001005B, (s32)btlevtcmd_InviteApInfoReport, 0x00010003,
    0x0000005A, 0x0003005B, (s32)evt_btl_camera_set_mode, 0x00000000,
    0x00000000, 0x0003005B, (s32)evt_btl_camera_set_moveSpeedLv, 0x00000000,
    0x00000001, 0x0004005B, (s32)btlevtcmd_AnimeChangePose, 0xFFFFFFFE,
    0x00000001, (s32)"PTR_A1_1", 0x00010009, 0x00000030,
    0x00010003, 0x00000063, 0x0002005B, (s32)btlevtcmd_ResetFaceDirection,
    0xFFFFFFFE, 0x0001005E, (s32)btldefaultevt_SuitoruBadgeEffect, 0x0002005B,
    (s32)btlevtcmd_StartWaitEvent, 0xFFFFFFFE, 0x00000002, 0x00000001,
};

// TODO: type
s32 vivian_shadow_tail_event[78] = {
    0x0005005B, (s32)btlevtcmd_GetPos, 0xFFFFFFFE, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0004005B, (s32)btlevtcmd_TransStageFloorPosition,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0004005B,
    (s32)btlevtcmd_SetUnitWork, 0xFFFFFFFE, 0x00000000, 0xFE363C80,
    0x0004005B, (s32)btlevtcmd_SetUnitWork, 0xFFFFFFFE, 0x00000001,
    0xFE363C81, 0x0004005B, (s32)btlevtcmd_SetUnitWork, 0xFFFFFFFE,
    0x00000002, 0xFE363C82, 0x00010003, 0x00000000,
    0x0003005B, (s32)btlevtcmd_GetUnitId, 0xFFFFFFFE, 0xFE363C85,
    0x00020018, 0xFE363C85, 0xFFFFFFFF, 0x00000002,
    0x00000021, 0x0004005B, (s32)btlevtcmd_GetUnitWork, 0xFFFFFFFE,
    0x00000003, 0xFE363C85, 0x00020018, 0xFE363C85,
    0x00000000, 0x0005005B, (s32)btlevtcmd_GetPos, 0xFFFFFFFE,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0004005B,
    (s32)btlevtcmd_TransStageFloorPosition, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0004005B, (s32)btlevtcmd_SetUnitWork, 0xFFFFFFFE, 0x00000000,
    0xFE363C80, 0x0004005B, (s32)btlevtcmd_SetUnitWork, 0xFFFFFFFE,
    0x00000001, 0xFE363C81, 0x0004005B, (s32)btlevtcmd_SetUnitWork,
    0xFFFFFFFE, 0x00000002, 0xFE363C82, 0x00000021,
    0x00010009, 0x00000001, 0x00010004, 0x00000000,
    0x00000002, 0x00000001,
};

// TODO: type
s32 vivian_counter_damage_event[177] = {
    0x0005005B, (s32)btlevtcmd_SetScale, 0xFFFFFFFE, 0xF24A7E80,
    0xF24A7E80, 0xF24A7E80, 0x0003005B, (s32)btlevtcmd_OnUnitFlag,
    0xFFFFFFFE, 0x00000001, 0x00020018, 0xFE363C80,
    0x0000002B, 0x00020032, 0xFE363C8E, 0x00000024,
    0x00000020, 0x00020032, 0xFE363C8E, 0x00000027,
    0x00000021, 0x0004005B, (s32)btlevtcmd_AnimeChangePoseType, 0xFFFFFFFE,
    0xFE363C8B, 0xFE363C8E, 0x0003005B, (s32)btlevtcmd_SetFallAccel,
    0xFFFFFFFE, 0xF24A7E80, 0x0005005B, (s32)btlevtcmd_GetPos,
    0xFFFFFFFE, 0xFE363C82, 0xFE363C83, 0xFE363C84,
    0x0004005B, (s32)btlevtcmd_FaceDirectionSub, 0xFFFFFFFE, 0xFE363C82,
    0x00000019, 0x00020035, 0xFE363C83, 0x00000019,
    0x000A005B, (s32)btlevtcmd_DivePosition, 0xFFFFFFFE, 0xFE363C82,
    0xFE363C83, 0xFE363C84, 0x00000005, 0x00000000,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00010009,
    0x0000001E, 0x0004005B, (s32)btlevtcmd_GetUnitWork, 0xFFFFFFFE,
    0x00000003, 0xFE363C85, 0x00020019, 0xFE363C85,
    0x00000000, 0x0004005B, (s32)btlevtcmd_GetUnitWork, 0xFFFFFFFE,
    0x00000000, 0xFE363C80, 0x0004005B, (s32)btlevtcmd_GetUnitWork,
    0xFFFFFFFE, 0x00000001, 0xFE363C81, 0x0004005B,
    (s32)btlevtcmd_GetUnitWork, 0xFFFFFFFE, 0x00000002, 0xFE363C82,
    0x0004005B, (s32)btlevtcmd_TransStageFloorPosition, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x000A005B, (s32)btlevtcmd_DivePosition, 0xFFFFFFFE,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0000000C,
    0x00000000, 0x00000005, 0x00000000, 0xFFFFFFFF,
    0x0004005B, (s32)btlevtcmd_SetUnitWork, 0xFFFFFFFE, 0x00000003,
    0x00000000, 0x0005005B, (s32)battle_evt_majo_disp_off, 0xFFFFFFFE,
    0x00000001, 0x00000000, 0x00000000, 0x0005005B,
    (s32)btlevtcmd_GetHomePos, 0xFFFFFFFE, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)btlevtcmd_GetPos, 0xFFFFFFFE,
    0xF1194D80, 0xFE363C81, 0xF1194D80, 0x000A005B,
    (s32)btlevtcmd_DivePosition, 0xFFFFFFFE, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000050, 0x00000000, 0x00000005,
    0x00000000, 0xFFFFFFFF, 0x0005005B, (s32)btlevtcmd_GetHomePos,
    0xFFFFFFFE, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0007005B, (s32)battle_evt_majo_disp_on, 0xFFFFFFFE, 0x00000001,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000000,
    0x0005005B, (s32)btlevtcmd_SetPos, 0xFFFFFFFE, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000020, 0x0005005B,
    (s32)btlevtcmd_GetHomePos, 0xFFFFFFFE, 0xFE363C82, 0xFE363C83,
    0xFE363C84, 0x000A005B, (s32)btlevtcmd_DivePosition, 0xFFFFFFFE,
    0xFE363C82, 0xFE363C83, 0xFE363C84, 0x00000005,
    0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000021, 0x0002005B, (s32)btlevtcmd_ResetFaceDirection, 0xFFFFFFFE,
    0x0002005B, (s32)btlevtcmd_StartWaitEvent, 0xFFFFFFFE, 0x00000002,
    0x00000001,
};

// TODO: type
s32 vivian_appeal_event[80] = {
    0x0003005B, (s32)btlevtcmd_RunDataEventChild, 0xFFFFFFFE, 0x00000007,
    0x0004005B, (s32)btlevtcmd_AnimeChangePose, 0xFFFFFFFE, 0x00000001,
    (s32)"PTR_O_1", 0x0005005B, (s32)btlevtcmd_GetPos, 0xFFFFFFFE,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0006005B,
    (s32)evt_snd_sfxon_3d, (s32)"SFX_MARIO_APPEAL1", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000000, 0x00010009, 0x0000000A,
    0x0004005B, (s32)btlevtcmd_AnimeChangePose, 0xFFFFFFFE, 0x00000001,
    (s32)"PTR_O_2", 0x0001000A, 0x0000014D, 0x0005005B,
    (s32)btlevtcmd_GetPos, 0xFFFFFFFE, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0004005B, (s32)btlevtcmd_FaceDirectionAdd, 0xFFFFFFFE,
    0xFE363C80, 0x0000000F, 0x00020035, 0xFE363C81,
    0x0000001E, 0x0003005B, (s32)btlevtcmd_GetFaceDirection, 0xFFFFFFFE,
    0xFE363C8E, 0x00020037, 0xFE363C8E, 0x0000002D,
    0x000F005B, (s32)evt_eff, (s32)"", (s32)"kiss",
    0x00000000, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0xFE363C8E, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0001000A, 0x0000014D, 0x0002005B, (s32)btlevtcmd_AudienceAppeal,
    0xFFFFFFFE, 0x0001000A, 0x000001F4, 0x0002005B,
    (s32)btlevtcmd_StartWaitEvent, 0xFFFFFFFE, 0x00000002, 0x00000001,
};

// TODO: type
static s32 party_win_reaction[18] = {
    0x0004005B, (s32)btlevtcmd_AnimeChangePose, 0xFFFFFFFE, 0x00000001,
    (s32)"PTR_Y_1", 0x0003005B, (s32)btlevtcmd_AnimeWaitPlayComplete, 0xFFFFFFFE,
    0x00000001, 0x0001000A, 0x000001F4, 0x0004005B,
    (s32)btlevtcmd_AnimeChangePoseType, 0xFFFFFFFE, 0x00000001, 0x0000002B,
    0x00000002, 0x00000001,
};

// TODO: type
static s32 attack_audience_event[189] = {
    0x0005005B, (s32)btlevtcmd_GetPos, 0xFFFFFFFE, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0004005B, (s32)btlevtcmd_GetStageSize,
    0xFE363C83, 0xF1194D80, 0xF1194D80, 0x0003005B,
    (s32)btlevtcmd_GetFaceDirection, 0xFFFFFFFE, 0xFE363C84, 0x00020032,
    0xFE363C88, 0xFE363C83, 0x00020037, 0xFE363C88,
    0xFE363C84, 0x00020037, 0xFE363C88, 0xFFFFFFFF,
    0x0003005B, (s32)btlevtcmd_SetMoveSpeed, 0xFFFFFFFE, 0xF24A9280,
    0x0004005B, (s32)btlevtcmd_AnimeChangePoseType, 0xFFFFFFFE, 0x00000001,
    0x0000002A, 0x0008005B, (s32)btlevtcmd_MovePosition, 0xFFFFFFFE,
    0xFE363C88, 0xFE363C81, 0xFE363C82, 0x00000000,
    0xFFFFFFFF, 0x00000000, 0x00010009, 0x00000014,
    0x0005005B, (s32)_get_attack_aud_no, 0xFE363C83, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0004005B, (s32)btlevtcmd_GetStageAudienceAreaSize,
    0xFE363C84, 0xF1194D80, 0xF1194D80, 0x0003005B,
    (s32)btlevtcmd_GetFaceDirection, 0xFFFFFFFE, 0xFE363C85, 0x00020032,
    0xFE363C86, 0xFE363C84, 0x00020037, 0xFE363C86,
    0xFE363C85, 0x00020037, 0xFE363C86, 0xFFFFFFFF,
    0x0005005B, (s32)btlevtcmd_SetPos, 0xFFFFFFFE, 0xFE363C86,
    0xFE363C81, 0xFE363C82, 0x0003005B, (s32)evt_btl_camera_set_mode,
    0x00000000, 0x0000000D, 0x0005005B, (s32)evt_btl_camera_set_homing_unit_audience,
    0x00000000, 0xFFFFFFFE, 0x00000001, 0xFE363C83,
    0x0003005B, (s32)evt_btl_camera_set_moveSpeedLv, 0x00000000, 0x00000002,
    0x0003005B, (s32)evt_btl_camera_set_zoom, 0x00000000, 0x00000064,
    0x0004005B, (s32)btlevtcmd_FaceDirectionSub, 0xFFFFFFFE, 0xFE363C80,
    0x00000014, 0x0008005B, (s32)btlevtcmd_MovePosition, 0xFFFFFFFE,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000000,
    0xFFFFFFFF, 0x00000000, 0x0003005B, (s32)evt_btl_camera_set_zoom,
    0x00000000, 0x000000C8, 0x0004005B, (s32)btlevtcmd_AnimeChangePose,
    0xFFFFFFFE, 0x00000001, (s32)"PTR_A1_2", 0x0003005B,
    (s32)btlevtcmd_ftomsec, 0x00000004, 0xFE363C80, 0x0001000A,
    0xFE363C80, 0x0003005B, (s32)evt_snd_sfxon, (s32)"SFX_BTL_VIVIAN_ATTACK4",
    0x00000000, 0x0003005B, (s32)_attack_aud, 0xFE363C83,
    0x00000000, 0x0001006F, 0xFE363C8F, 0x0001000A,
    0x000005DC, 0x0003005B, (s32)evt_btl_camera_set_mode, 0x00000000,
    0x00000000, 0x0001000A, 0x000003E8, 0x00000070,
    0x0001000A, 0x000001F4, 0x0004005B, (s32)btlevtcmd_AnimeChangePoseType,
    0xFFFFFFFE, 0x00000001, 0x0000002A, 0x0003005B,
    (s32)btlevtcmd_SetMoveSpeed, 0xFFFFFFFE, 0xF24A8A80, 0x0008005B,
    (s32)btlevtcmd_MovePosition, 0xFFFFFFFE, 0xFE363C86, 0xFE363C81,
    0xFE363C82, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0x0005005B, (s32)btlevtcmd_GetHomePos, 0xFFFFFFFE, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)btlevtcmd_SetPos,
    0xFFFFFFFE, 0xFE363C88, 0xFE363C81, 0xFE363C82,
    0x0002005B, (s32)btlevtcmd_WaitEventEnd, 0xFE363C8F, 0x0008005B,
    (s32)btlevtcmd_MovePosition, 0xFFFFFFFE, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0x0002005B, (s32)btlevtcmd_StartWaitEvent, 0xFFFFFFFE, 0x00000002,
    0x00000001,
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

// TODO: type
static s32 attack_event[2] = {
    0x00000002, 0x00000001,
};

// TODO: type
static s32 wait_event[6] = {
    0x0003005B, (s32)btlevtcmd_AnimeChangePoseFromTable, 0xFFFFFFFE, 0x00000001,
    0x00000002, 0x00000001,
};

#pragma explicit_zero_data on
#pragma force_active on
u32 gap_07_80412488_sdata[6] = { 0 };
__declspec(section ".data") u32 gap_05_8038BA04_data = 0;
#pragma force_active reset
#pragma explicit_zero_data reset

#pragma force_active reset
USER_FUNC(battle_evt_majo_disp_off) {
    s32* args = event->args;
    BattleUnit* unit;
    BattleUnitPart* parts;
    s32 pose;
    s32 v1;
    s32 id;
    s32 v2;
    s32 v3;

    id = BattleTransID(event, evtGetValue(event, args[0]));
    v1 = evtGetValue(event, args[1]);
    v2 = evtGetValue(event, args[2]);
    v3 = evtGetValue(event, args[3]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    parts = BattleGetUnitPartsPtr(id, v1);
    pose = parts->poseId;

    if (isFirstCall) {
        event->userdata[0] = 0;
        event->userdata[1] = 0;
        animEffectAsync(STR_P_BIBI, 1);
    }

    switch (event->userdata[0]) {
    case 0:
        parts->field_0x78 = intplGetValue(0, event->userdata[1], 30, 1.0f, 2.0f);
        if (v2 != 0) {
            unit->flags |= 0x10000;
        }
        event->userdata[1]++;
        if (event->userdata[1] > 15) {
            event->userdata[0]++;
        }
        break;
    case 1:
        if (event->userdata[1] <= 30) {
            parts->field_0x78 = intplGetValue(0, event->userdata[1], 30, 1.0f, 2.0f);
            event->userdata[1]++;
        }
        if (!animEffectAsync(STR_P_BIBI, 1)) {
            return 0;
        }
        animPoseSetEffect(pose, STR_P_BIBI, 1);
        animPoseSetEffectAnim(pose, STR_PM_B_1, 1);
        animPoseSetLocalTime(animPoseGetAnimPosePtr(pose)->paperId, 8.0f);
        event->userdata[0]++;
        break;
    case 2:
        if (event->userdata[1] <= 30) {
            parts->field_0x78 = intplGetValue(0, event->userdata[1], 30, 1.0f, 2.0f);
            event->userdata[1]++;
        }
        if (animPoseGetLoopTimes(animPoseGetAnimPosePtr(pose)->paperId) >= 1.0f) {
            event->userdata[0]++;
            event->userdata[1] = 0;
            psndSFXOn_3D(STR_SHADOW1_2, &unit->mPosition);
            if (v3 != 0) {
                parts->attributes |= 0x01000000;
            }
        }
        break;
    case 3:
        parts->field_0x78 = intplGetValue(0, event->userdata[1], 20, 2.0f, 0.0f);
        event->userdata[1]++;
        if (event->userdata[1] > 20) {
            parts->field_0x78 = 0.0f;
            unit->mPosition.y -= 500.f;
            animPoseSetEffect(pose, NULL, 1);
            animPoseSetPaperAnimGroup(pose, NULL, 1);
            return EVT_RETURN_DONE;
        }
        break;
    }
    return 0;
}


USER_FUNC(_vivian_make_extra_work_area) {
    BattleWork* work = _battleWorkPointer;
    BattleUnit* unit;
    s32 id;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(work, id);
    unit->mExtraWork = BattleAlloc(0xC0);
    return EVT_RETURN_DONE;
}


// clang-format off

#define TECH_MENU_ENTRY(table)                                                             \
    cmd->mWeaponTable[*count].mIndex = -1;                                                 \
    cmd->mWeaponTable[*count].mItem = 0;                                                   \
    cmd->mWeaponTable[*count].mWeaponParams = (BattleWeapon*)(table);                      \
    cmd->mWeaponTable[*count].field_0x4 = 0;                                               \
    cmd->mWeaponTable[*count].mIcon =                                                      \
        *(u16*)((u8*)cmd->mWeaponTable[*count].mWeaponParams + 4);                         \
    cmd->mWeaponTable[*count].mName =                                                      \
        (char*)msgSearch(cmd->mWeaponTable[*count].mWeaponParams->name);                   \
    (*count)++;

static void __makeTechMenuFunc(BattleWorkCommand* cmd, s32* count) {
    s32 techLv = partyGetTechLv(
        BattleTransPartyId(BattleGetPartyPtr(_battleWorkPointer)->currentKind));

    TECH_MENU_ENTRY(&partyWeapon_VivianNormalAttack);
    if (techLv >= 0) {
        TECH_MENU_ENTRY(&partyWeapon_VivianShadowGuard);
    }
    if (techLv >= 1) {
        TECH_MENU_ENTRY(&partyWeapon_VivianMagicalPowder);
    }
    if (techLv >= 2) {
        TECH_MENU_ENTRY(&partyWeapon_VivianCharmKissAttack);
    }
}





s32 _get_move_frame(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    Vec a;
    Vec b;
    Vec d;
    f32 speed;
    f32 len;
    s32 frame;
    s32 frameIndex;

    a.x = evtGetFloat(event, args[0]);
    a.y = evtGetFloat(event, args[1]);
    a.z = evtGetFloat(event, args[2]);
    b.x = evtGetFloat(event, args[3]);
    b.y = evtGetFloat(event, args[4]);
    b.z = evtGetFloat(event, args[5]);
    speed = evtGetFloat(event, args[6]);
    frameIndex = args[7];
    PSVECSubtract(&a, &b, &d);
    len = sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
    frame = (s32)(len / speed);
    if (frame > 0xF) {
        frame = 0xF;
    }
    evtSetValue(event, frameIndex, frame);
    return EVT_RETURN_DONE;
}


// The three setValue arg indices stay live in callee-saved registers, one `base` temp holds
// `-30.f * unit1->mSizeMultiplier`, and outZ is computed between outX and outY.
// (The pooled int->float conversion double prints as an anonymous @NNN@ instead of double_to_int_802f8010.)
USER_FUNC(unk_8017d03c) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    s32 a1;
    s32 a2;
    s32 a3;
    BattleUnit* unit1;
    BattleUnit* unit2;
    f32 sx;
    f32 sy;
    f32 sz;
    f32 k54;
    f32 k58;
    f32 d1;
    f32 d2;
    f32 base;
    s32 t;
    s32 u;
    s32 v0;
    s32 v1;
    s32 v2;
    s32 v3;
    s32 outX;
    s32 outY;
    s32 outZ;
    f32 floor;

    v0 = evtGetValue(event, args[0]);
    a1 = args[1];
    v1 = evtGetValue(event, a1);
    a2 = args[2];
    v2 = evtGetValue(event, a2);
    a3 = args[3];
    v3 = evtGetValue(event, a3);

    unit1 = BattleGetUnitPtr(work, BattleTransID(event, -2));
    unit2 = BattleGetUnitPtr(work, BattleTransID(event, v0));
    BtlUnit_GetScale(unit2, &sx, &sy, &sz);

    k54 = unit2->mKindParams->field_0x54;
    k58 = unit2->mKindParams->field_0x58;
    d1 = (f32)unit1->faceDirection;
    d2 = (f32)unit2->faceDirection;
    t = (s32)(d2 * (k54 * sx));
    u = (s32)(k58 * sz);
    base = -30.f * unit1->mSizeMultiplier;
    outX = (s32)((f32)v1 + (base * d1 + (f32)t));
    outZ = v3 + u;
    outY = (s32)((f32)v2 - 20.f * unit1->mSizeMultiplier);
    floor = BattleGetFloorHeight(work, (f32)outX, (f32)outY, (f32)outZ);
    if (floor > (f32)outY) {
        outY = (s32)floor;
    }
    evtSetValue(event, a1, outX);
    evtSetValue(event, a2, outY);
    evtSetValue(event, a3, outZ);
    return EVT_RETURN_DONE;
}


USER_FUNC(_make_kagenuke_weapon) {
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
    area = unit->mExtraWork;
    arg0 = args[0];
    value = evtGetValue(event, arg0);
    value2 = evtGetValue(event, args[1]);
    memcpy(area, (void*)value, 0xC0);
    *(u8*)((u8*)area + 0x90) = value2;
    evtSetValue(event, arg0, (s32)area);
    return EVT_RETURN_DONE;
}


USER_FUNC(_disp_heart_entry) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 id;
    s32 frame;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(work, id);
    if (isFirstCall) {
        event->userdata[0] = (s32)unit;
        event->userdata[1] = 0;
        event->userdata[2] = evtGetValue(event, args[0]);
        event->field_84 = evtGetValue(event, args[1]);
        *(s32*)&event->wInterpRelatedTime = evtGetValue(event, args[2]);
        event->userdataEx[1] = 0;
        event->field_0x90[0] = evtGetValue(event, args[3]);
        event->field_0x90[1] = evtGetValue(event, args[4]);
    }
    if (event->userdataEx[1] != 0) {
        return EVT_RETURN_DONE;
    }
    event->userdata[1]++;
    dispEntry(CAMERA_3D, 0.0f, 1, _disp_heart, event);
    frame = *(s32*)&event->wInterpRelatedTime;
    if (event->userdata[1] < frame) {
        return 0;
    }
    return EVT_RETURN_DONE;
}


// Matched. The hit position must be copied through a separate `hitPos` local before `pos` (MWCC keeps
// the zero-filled `pos` plus a dead 12-byte temp); `iconPos = pos` followed by assignments from `pos`
// (not `+=`), with `float_5 + pos.z` commuted, gives the original's FP registers.
void _disp_heart(CameraId cameraId, void* param) {
    EventEntry* event;
    f32 scale;
    s32 a;
    s32 b;
    BattleUnit* unit;
    BattleUnitPart* part;
    Vec hitPos;
    Vec iconPos;
    Vec pos;

    if (param == NULL) {
        return;
    }
    event = param;
    if ((BattleUnit*)event->userdata[0] == NULL) {
        return;
    }
    a = event->userdata[1];
    b = event->userdata[2];
    if (a <= b) {
        scale = intplGetValue(5, a, b, 0.0f, 1.2f);
    } else if (a <= (s32)event->field_84) {
        scale = intplGetValue(0xB, a - b, (s32)event->field_84 - b, 1.2f, 1.0f);
    } else {
        scale = 1.0f;
    }

    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, event->field_0x90[0]));
    part = BtlUnit_GetPartsPtr(unit, event->field_0x90[1]);
    BtlUnit_GetHitPos(unit, part, &hitPos.x, &hitPos.y, &hitPos.z);
    hitPos.z += 10.f;
    pos = hitPos;
    iconPos = pos;
    iconPos.y = pos.y + 16.0f * scale;
    iconPos.z = 5.0f + pos.z;
    iconDispGx(scale, &pos, 0, 0x1F2);
    iconDispGx(scale, &iconPos, 0, 0x189);
}


USER_FUNC(_disp_heart_entry_stop) {
    EventEntry* entry = evtGetPtrID(evtGetValue(event, event->args[0]));
    entry->userdataEx[1] = 1;
    return EVT_RETURN_DONE;
}

USER_FUNC(_disp_heart_entry_stop_check) {
    evtSetValue(event, event->args[0], event->userdataEx[1]);
    return EVT_RETURN_DONE;
}

// clang-format on
