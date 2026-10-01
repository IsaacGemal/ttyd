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
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "evt/evt_audience.h"
#include "evt/evt_eff.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "party/party_motion.h"
#include "battle/battle_attack_audience.h"

// First object of this unit's .data (the float pair that used to end the previous unit's .data).

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

extern s32 _check_aud_item_type[];
extern s32 btldefaultevt_avoid_counter_event[];
extern s32 evt_audience_acrobat_notry[];
extern s32 negone_one[];
extern s32 one_negone[];
extern s32 subset_attack_ready_jump_event[];
extern s32 weaponGetACOutputParam[];
extern s32 weaponGetPowerFromPartyAttackLv[];
static void __makeTechMenuFunc(BattleWorkCommand* cmd, s32* count);
static s32 _clauda_breath_effect_ready(EventEntry* event, BOOL isFirstCall);
static s32 _clauda_breath_effect_fire(EventEntry* event, BOOL isFirstCall);
u32 partyWeapon_ClaudaNormalAttack[48];
u32 partyWeapon_ClaudaBreathAttack[48];
u32 partyWeapon_ClaudaPredationAttack[48];
u32 partyWeapon_ClaudaKumogakureAttack[48];
static s32 init_event[60];
static s32 damage_event[16];
s32 partyClaudaAttack_NormalAttack[700];
s32 partyClaudaAttack_BreathAttack[520];
s32 partyClaudaAttack_PredationAttack[875];
s32 partyClaudaAttack_KumoGuard[290];
s32 clauda_appeal_event[71];
static s32 attack_audience_event[15];
s32 attack_bad_audience_event[348];
s32 attack_good_audience_event[216];
static u32 defence[2];
static u32 defence_attr[2];
static u32 regist[6];
static s32 attack_event[2];
static s32 wait_event[6];
static s32 party_win_reaction[11];

// clang-format off
USER_FUNC(_clauda_make_extra_work_area) {
    BattleWork* work = _battleWorkPointer;
    BattleUnit* unit;
    s32 id;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(work, id);
    *(void**)((u8*)unit + 0x310) = BattleAlloc(0xC0);
    return EVT_RETURN_DONE;
}


static void __makeTechMenuFunc(BattleWorkCommand* cmd, s32* count) {
    s32 techLv = partyGetTechLv(
        BattleTransPartyId(BattleGetPartyPtr(_battleWorkPointer)->currentKind));

    TECH_MENU_ENTRY((BattleWeapon*)partyWeapon_ClaudaNormalAttack);
    if (techLv >= 0) {
        TECH_MENU_ENTRY((BattleWeapon*)partyWeapon_ClaudaBreathAttack);
    }
    if (techLv >= 1) {
        TECH_MENU_ENTRY((BattleWeapon*)partyWeapon_ClaudaPredationAttack);
    }
    if (techLv >= 2) {
        TECH_MENU_ENTRY((BattleWeapon*)partyWeapon_ClaudaKumogakureAttack);
    }
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

USER_FUNC(_get_clauda_kiss_hit_position) {
    s32* args = event->args;
    BattleWork* work = _battleWorkPointer;
    s32 id0;
    s32 id1;
    s32 out0;
    s32 out1;
    s32 out2;
    BattleUnit* unit1;
    BattleUnit* unit2;
    s32 dir;
    Vec pos;
    Vec hit;

    id0 = evtGetValue(event, args[0]);
    id1 = evtGetValue(event, args[1]);
    evtGetValue(event, args[2]);
    out0 = args[3];
    out1 = args[4];
    out2 = args[5];
    unit1 = BattleGetUnitPtr(work, BattleTransID(event, id0));
    unit2 = BattleGetUnitPtr(work, BattleTransID(event, id1));

    dir = unit2->faceDirection;
    BtlUnit_GetPos(unit2, &pos.x, &pos.y, &pos.z);
    pos.x += (f32)dir * (unit2->kissHitOffset.x * unit2->mSizeMultiplier);
    pos.y += unit2->kissHitOffset.y * unit2->mSizeMultiplier;
    pos.z += unit2->kissHitOffset.z * unit2->mSizeMultiplier;
    pos.y -= float_30_8041dcfc * unit1->mSizeMultiplier;
    hit = pos;
    hit.y = BattleGetFloorHeight(work, hit.x, hit.y, hit.z);
    if (hit.y > pos.y) {
        pos.y = hit.y;
    }
    evtSetFloat(event, out0, pos.x);
    evtSetFloat(event, out1, pos.y);
    evtSetFloat(event, out2, pos.z);
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

// TODO: type
__declspec(section ".data") u32 lbl_80376320[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
static __declspec(section ".data") u32 negone_one_374[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_80376330[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
static __declspec(section ".data") u32 one_negone_373[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_80376340[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
static __declspec(section ".data") u32 negone_one_360[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_80376350[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
static __declspec(section ".data") u32 one_negone_359[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
u32 partyWeapon_ClaudaNormalAttack[48] = {
    (u32)"btl_wn_pwd_normal", 0x01500000, 0x00000000, (u32)"msg_pwd_body_press",
    0x64000000, 0x3F800000, 0x01016432, (u32)weaponGetPowerFromPartyAttackLv,
    0x00000001, 0x00000002, 0x00000002, 0x00000004,
    0x00000003, 0x00000006, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01001260, 0x20000000, 0x000A0302,
    (u32)"msg_ac_body_press", 0x00035087, 0x0000000C, 0x80002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&partyClaudaAttack_NormalAttack, 0x00141464, 0x140A0205, 0x0A000000,
};

// TODO: type
u32 partyWeapon_ClaudaBreathAttack[48] = {
    (u32)"btl_wn_pwd_lv1", 0x01510000, 0x00000000, (u32)"msg_pwd_breath",
    0x64040000, 0x3F800000, 0x01056432, 0x00000000,
    0x00000001, 0x00000002, 0x00000002, 0x00000003,
    0x00000003, 0x00000004, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x00190302,
    (u32)"msg_ac_breath", 0x00020287, 0x000007FF, 0x80002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00640000, 0x00000000, 0x00000000,
    (u32)&partyClaudaAttack_BreathAttack, 0x00000064, 0x00050500, 0x00000000,
};

// TODO: type
u32 partyWeapon_ClaudaPredationAttack[48] = {
    (u32)"btl_wn_pwd_lv2", 0x01520000, 0x00000000, (u32)"msg_pwd_sexy_kiss",
    0x64030000, 0x3F800000, 0x02056432, (u32)weaponGetACOutputParam,
    0x00000000, 0x00000001, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000003, 0x00000003, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20000000, 0x00000302,
    (u32)"msg_ac_sexy_kiss", 0x000200C7, 0x00000002, 0x80002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&partyClaudaAttack_PredationAttack, 0x00000064, 0x00000500, 0x00000000,
};

// TODO: type
u32 partyWeapon_ClaudaKumogakureAttack[48] = {
    (u32)"btl_wn_pwd_lv3", 0x01530000, 0x00000000, (u32)"msg_pwd_kumogakure",
    0x64040000, 0x3F800000, 0x01056432, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101160, 0x10000000, 0x00000302,
    (u32)"msg_ac_kumogakure", 0x00020008, 0x000007FF, 0x80002004,
    0x00000000, 0x00000000, 0x00000000, 0x00640200,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&partyClaudaAttack_KumoGuard, 0x00000064, 0x00000500, 0x00000000,
};

// TODO: type
u32 pose_table_clauda_stay[50] = {
    0x00000009, (u32)"PWD_D_5", 0x00000002, (u32)"PWD_D_5",
    0x00000003, (u32)"PWD_X_1", 0x00000004, (u32)"PWD_D_2",
    0x00000005, (u32)"PWD_X_1", 0x00000001, (u32)"PWD_N_1",
    0x0000001B, (u32)"PWD_D_3", 0x0000001C, (u32)"PWD_S_1",
    0x00000025, (u32)"PWD_G_2", 0x0000001F, (u32)"PWD_G_1",
    0x00000027, (u32)"PWD_D_1", 0x00000028, (u32)"PWD_W_1",
    0x0000002A, (u32)"PWD_R_1", 0x00000039, (u32)"PWD_D_2",
    0x00000038, (u32)"PWD_D_2", 0x0000003C, (u32)"PWD_D_3",
    0x0000003D, (u32)"PWD_D_2", 0x00000041, (u32)"PWD_T_1",
    0x00000034, (u32)"PWD_I_1", 0x00000043, (u32)"PWD_C_1",
    0x00000042, (u32)"PWD_O_1", 0x00000040, (u32)"PWD_X_1",
    0x00000031, (u32)"PWD_A4_3", 0x0000002D, (u32)"PWD_G_1",
    0x00000045, (u32)"PWD_S_1",
};

// TODO: type
static u32 data_table[16] = {
    0x00000005, (u32)&clauda_appeal_event, 0x00000006, (u32)&attack_audience_event,
    0x00000001, (u32)__makeTechMenuFunc, 0x00000030, (u32)btldefaultevt_Dummy,
    0x00000040, (u32)&party_win_reaction, 0x00000007, (u32)subset_attack_ready_jump_event,
    0x0000002F, (u32)btldefaultevt_avoid_counter_event, 0x00000000, 0x00000000,
};

// TODO: type
u32 unitpartsdata_Party_Clauda[19] = {
    0x00000001, (u32)"btl_un_clauda", (u32)"c_windy", 0x00000000,
    0x00000000, 0x00000000, 0x41800000, 0x42300000,
    0x40A00000, 0x00000000, 0x42300000, 0x00000000,
    0x001A0032, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00000009, 0x00000000, (u32)&pose_table_clauda_stay,
};

// TODO: type
u32 unitdata_Party_Clauda[49] = {
    0x000000E3, (u32)"btl_un_clauda", 0x000A0005, 0x05010100,
    0x00000064, 0x00090014, 0x00320010, 0x002C0000,
    0x00000000, 0x41400000, 0x00000000, 0x00000000,
    0x00000000, 0x42200000, 0x00000000, 0x00000000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41200000,
    0x00000000, 0x00000000, 0x41200000, 0x42020000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0000FF00, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_PARTY_DAMAGE1",
    (u32)"SFX_MARIO_DAMAGE_FIRE1", (u32)"SFX_MARIO_DAMAGE_ICE1", (u32)"SFX_MARIO_DAMAGE_BIRIBIRI1", 0x02000004,
    (u32)&regist, 0x01000000, (u32)&unitpartsdata_Party_Clauda, (u32)&init_event,
    (u32)&data_table,
};

static EVT_BEGIN(battle_entry_event)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 250)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    WAIT_FRAMES(40)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_SetMoveSoundDataWork, -2, STRING("SFX_PARTY_BATTLE_SUBHANG1"), STRING("SFX_PARTY_BATTLE_SUBHANG1"), 0, 9, 3, 0, 0)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -2, 0)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

static EVT_BEGIN(init_event)
    CALL(_clauda_make_extra_work_area)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_PARTY_BATTLE_HANG1"), STRING("SFX_PARTY_BATTLE_HANG1"), 6, 12, 16)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_PARTY_BATTLE_HANG1"), STRING("SFX_PARTY_BATTLE_HANG1"), 0, 9, 8)
    CALL(btlevtcmd_SetDiveSound, -2, STRING("SFX_PARTY_BATTLE_HANG1"), STRING("SFX_PARTY_BATTLE_HANG1"), 0, 9, 8)
    CALL(btlevtcmd_SetJumpSound, -2, STRING("SFX_PARTY_BATTLE_JUMP1"), STRING("SFX_PARTY_BATTLE_LANDING1"))
    CALL(btlevtcmd_SetEventEntry, -2, PTR(battle_entry_event))
    CALL(btlevtcmd_OnUnitFlag, -2, 3145728)
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

EVT_BEGIN(partyClaudaAttack_NormalAttack)
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
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_AcGetDifficulty, LW(0))
    CALL(btlevtcmd_AcSetParamAll, LW(3), LW(4), 180, LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AcSetFlag, 0)
    CALL(btlevtcmd_SetupAC, -2, 8, 1, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    WAIT_FRAMES(20)
    CALL(btlevtcmd_StartAC, 1)
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_StopAC)
    CALL(btlevtcmd_OffUnitFlag, -2, 2097152)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_A1_1"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_CLAUD_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.2f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    ADD(LW(2), 5)
    CALL(btlevtcmd_GetResultAC, LW(5))
    IF_BITS_CLEAR(LW(5), 2)
        CALL(evt_sub_random, 40, LW(5))
        SUBTRACT(LW(5), 20)
        ADD(LW(0), LW(5))
        CALL(evt_sub_random, 20, LW(5))
        SUBTRACT(LW(5), 10)
        ADD(LW(1), LW(5))
    END_IF
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 60, -1)
    CALL(btlevtcmd_OnUnitFlag, -2, 2097152)
    CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(5))
    IF_INT_NOT_EQUAL(LW(5), 1)
        GOTO(10)
    END_IF
    CALL(btlevtcmd_GetResultAC, LW(0))
    IF_BITS_SET(LW(0), 2)
        GOTO(20)
    END_IF
    SET_VALUE(LW(5), 1)
    LABEL(10)
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
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_A1_2"))
    CALL(btlevtcmd_JumpContinue, -2)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_D_3"))
    CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL(evt_sub_intpl_init_float, 11, FLOAT(1.0f), FLOAT(0.4f), 10)
        LOOP(10)
            CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetScale, -2, LW(0), LW(5), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_sub_intpl_init_float, 11, FLOAT(0.4f), FLOAT(1.0f), 6)
        LOOP(6)
            CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetScale, -2, LW(0), LW(5), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        CALL(evt_sub_intpl_init_float, 11, FLOAT(1.0f), FLOAT(1.5f), 10)
        LOOP(10)
            CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetScale, -2, LW(5), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_sub_intpl_init_float, 11, FLOAT(1.5f), FLOAT(1.0f), 6)
        LOOP(6)
            CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetScale, -2, LW(5), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    WAIT_FRAMES(30)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    GOTO(95)
    LABEL(20)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_CLAUD_ATT2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_CLAUD_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetResultAC, LW(0))
    IF_BITS_SET(LW(0), 2)
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 131328, LW(5))
        CALL(btlevtcmd_GetResultPrizeLv, LW(3), 0, LW(6))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(12), LW(6))
    ELSE
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
    END_IF
    CALL(btlevtcmd_JumpContinue, -2)
    BEGIN_SIBLING_EVENT_GET_ID(LW(14))
        CALL(btlevtcmd_ACRStart, -2, 0, 16, 16, 20)
    END_SIBLING_EVENT
    CALL(evt_btl_camera_shake_h, 0, 8, 0, 30, 13)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL(evt_sub_intpl_init_float, 11, FLOAT(1.0f), FLOAT(0.4f), 10)
        LOOP(10)
            CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetScale, -2, LW(0), LW(5), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_sub_intpl_init_float, 11, FLOAT(0.4f), FLOAT(1.0f), 6)
        LOOP(6)
            CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetScale, -2, LW(0), LW(5), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        CALL(evt_sub_intpl_init_float, 11, FLOAT(1.0f), FLOAT(1.5f), 10)
        LOOP(10)
            CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetScale, -2, LW(5), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_sub_intpl_init_float, 11, FLOAT(1.5f), FLOAT(1.0f), 6)
        LOOP(6)
            CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetScale, -2, LW(5), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_WaitEventEnd, LW(14))
    CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
    IF_INT_NOT_EQUAL(LW(6), 2)
        CALL(btlevtcmd_WaitEventEnd, LW(15))
        CALL(evt_audience_acrobat_notry)
        GOTO(95)
    END_IF
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 20)
    ADD(LW(1), 20)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.3f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 18, -1)
    CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, FLOAT(-60.0f), 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_Y_1"))
    WAIT_MS(500)
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetMoveFrame, -2, LW(0), LW(1), LW(2), FLOAT(6.0f), LW(15))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), LW(15), 0, 4, 0, -1)
    LABEL(99)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(partyClaudaAttack_BreathAttack)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(12))
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    WAIT_FRAMES(20)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -2)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 300)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_AcGetDifficulty, LW(1))
    SUBTRACT(LW(1), 3)
    SWITCH(LW(1))
        CASE_LESS_OR_EQUAL(-3)
        SET_VALUE(LW(0), 300)
        CASE_EQUAL(-2)
        SET_VALUE(LW(0), 300)
        CASE_EQUAL(-1)
        SET_VALUE(LW(0), 300)
        CASE_EQUAL(0)
        SET_VALUE(LW(0), 300)
        CASE_EQUAL(1)
        SET_VALUE(LW(0), 300)
        CASE_EQUAL(2)
        SET_VALUE(LW(0), 300)
        CASE_DEFAULT
        SET_VALUE(LW(0), 300)
    END_SWITCH
    CALL(btlevtcmd_AcGetDifficulty, LW(1))
    CALL(btlevtcmd_AcSetParamAll, 1, LW(0), 4, 40, 60, 80, 100, LW(1))
    CALL(btlevtcmd_SetupAC, -2, 14, 1, 0)
    CALL(_clauda_breath_effect_ready)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_A2_1"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_CLAUD_BREATH1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_A2_2"))
    CALL(evt_btl_camera_nomove_x_onoff, 0, 1)
    CALL(evt_btl_camera_nomove_y_onoff, 0, 1)
    CALL(evt_btl_camera_nomove_z_onoff, 0, 1)
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 0)
    CALL(evt_btl_camera_nomove_x_onoff, 0, 0)
    CALL(evt_btl_camera_nomove_y_onoff, 0, 0)
    CALL(evt_btl_camera_nomove_z_onoff, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_A2_3"))
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(btlevtcmd_StageDispellFog)
    END_INLINE_EVENT
    CALL(_clauda_breath_effect_fire, LW(13))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_CLAUD_BREATH2"), EVTDAT_POINTER_MAX, 0, LW(14))
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        SET_VALUE(LW(14), -1)
        LABEL(5)
        CALL(btlevtcmd_AcGetOutputParam, 1, LW(0))
        WAIT_FRAMES(1)
        GOTO(5)
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    LABEL(8)
    IF_INT_NOT_EQUAL(LW(3), -1)
        CALL(_check_blow_rate, LW(3), LW(0))
        IF_INT_GREATER_OR_EQUAL(LW(0), 1)
            CALL(btlevtcmd_DamageDirect, LW(3), LW(4), 0, 0, 3, 1)
        END_IF
        CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
        GOTO(8)
    END_IF
    CALL(btlevtcmd_StartAC, 1)
    CALL(btlevtcmd_ResultAC)
    DELETE_EVENT(LW(15))
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_StopAC)
    SET_VALUE(LW(11), 99)
    SET_VALUE(LW(12), PTR(partyWeapon_ClaudaBreathAttack))
    CALL(btlevtcmd_AcGetOutputParam, 2, LW(0))
    CALL(_make_breath_weapon, LW(12), LW(0))
    WAIT_FRAMES(60)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    SET_VALUE(LW(10), 0)
    LABEL(10)
    CALL(btlevtcmd_SetDispOffset, LW(3), 0, 0, 0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(6))
    IF_INT_NOT_EQUAL(LW(6), 1)
        IF_INT_EQUAL(LW(6), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
            GOTO(51)
        END_IF
        IF_INT_EQUAL(LW(6), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
            GOTO(51)
        END_IF
        IF_INT_EQUAL(LW(6), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
            GOTO(51)
        END_IF
        GOTO(50)
    END_IF
    CALL(btlevtcmd_GetResultAC, LW(0))
    IF_BITS_SET(LW(0), 2)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 131328, LW(5))
        CALL(btlevtcmd_GetDamageCode, LW(3), LW(0))
        IF_INT_EQUAL(LW(0), 29)
            CALL(btlevtcmd_GetResultPrizeLv, LW(3), 0, LW(6))
            CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
            CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
            CALL(btlevtcmd_AudienceDeclareACResult, LW(12), LW(6))
        END_IF
    ELSE
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
    END_IF
    LABEL(50)
    WAIT_MS(300)
    LABEL(51)
    SUBTRACT(LW(11), 1)
    IF_INT_GREATER_OR_EQUAL(LW(11), 1)
        LABEL(52)
        CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
        IF_INT_NOT_EQUAL(LW(3), -1)
            ADD(LW(10), 1)
            GOTO(10)
        END_IF
    END_IF
    LABEL(80)
    WAIT_MS(1500)
    LABEL(90)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_snd_sfxoff, LW(14))
    CALL(evt_eff_delete_ptr, LW(13))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
    CALL(btlevtcmd_ACRStart, -2, 0, 25, 25, 20)
    CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
    IF_INT_NOT_EQUAL(LW(6), 2)
        CALL(evt_audience_acrobat_notry)
        WAIT_MS(500)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_Y_1"))
    WAIT_MS(1000)
    LABEL(99)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(partyClaudaAttack_PredationAttack)
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
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 300)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_GetPartyTechLv, -2, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(2)
        CALL(btlevtcmd_AcGetDifficulty, LW(0))
        SUBTRACT(LW(0), 3)
        SWITCH(LW(0))
            CASE_LESS_OR_EQUAL(-3)
            SET_VALUE(LW(0), 120)
            SET_VALUE(LW(1), 0)
            CASE_EQUAL(-2)
            SET_VALUE(LW(0), 100)
            SET_VALUE(LW(1), 0)
            CASE_EQUAL(-1)
            SET_VALUE(LW(0), 80)
            SET_VALUE(LW(1), 1)
            CASE_EQUAL(0)
            SET_VALUE(LW(0), 70)
            SET_VALUE(LW(1), 2)
            CASE_EQUAL(1)
            SET_VALUE(LW(0), 66)
            SET_VALUE(LW(1), 2)
            CASE_EQUAL(2)
            SET_VALUE(LW(0), 62)
            SET_VALUE(LW(1), 2)
            CASE_DEFAULT
            SET_VALUE(LW(0), 58)
            SET_VALUE(LW(1), 2)
        END_SWITCH
        CALL(btlevtcmd_AcSetParamAll, 0, LW(0), LW(1), 5, 3, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AcSetGaugeParam, 60, 80, 100, 100)
        CASE_DEFAULT
        CALL(btlevtcmd_AcGetDifficulty, LW(0))
        SUBTRACT(LW(0), 3)
        SWITCH(LW(0))
            CASE_LESS_OR_EQUAL(-3)
            SET_VALUE(LW(0), 120)
            SET_VALUE(LW(1), 0)
            CASE_EQUAL(-2)
            SET_VALUE(LW(0), 100)
            SET_VALUE(LW(1), 0)
            CASE_EQUAL(-1)
            SET_VALUE(LW(0), 80)
            SET_VALUE(LW(1), 1)
            CASE_EQUAL(0)
            SET_VALUE(LW(0), 70)
            SET_VALUE(LW(1), 2)
            CASE_EQUAL(1)
            SET_VALUE(LW(0), 66)
            SET_VALUE(LW(1), 2)
            CASE_EQUAL(2)
            SET_VALUE(LW(0), 62)
            SET_VALUE(LW(1), 2)
            CASE_DEFAULT
            SET_VALUE(LW(0), 58)
            SET_VALUE(LW(1), 2)
        END_SWITCH
        CALL(btlevtcmd_AcSetParamAll, 0, LW(0), LW(1), 3, 2, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AcSetGaugeParam, 34, 68, 100, 100)
    END_SWITCH
    CALL(btlevtcmd_AcSetFlag, 0)
    CALL(btlevtcmd_SetupAC, -2, 17, 1, 60)
    WAIT_FRAMES(22)
    CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(5))
    IF_INT_EQUAL(LW(5), 5)
        CALL(btlevtcmd_GetStatusMg, LW(3), LW(6))
        MULTIPLY_FLOAT(LW(6), 40)
        CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), LW(6))
        CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
        CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 40, 0, 5, 0, -1)
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
    END_IF
    CALL(_get_clauda_kiss_hit_position, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetStatusMg, -2, LW(6))
    MULTIPLY_FLOAT(LW(6), 20)
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), LW(6))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    ADD(LW(2), 5)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 40, 0, 5, 0, -1)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_A3_1"))
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(5))
    SWITCH(LW(5))
        CASE_OR_EQUAL(2)
        CASE_OR_EQUAL(3)
        CASE_OR_EQUAL(6)
        CASE_OR_EQUAL(4)
        CASE_END
        CASE_EQUAL(1)
        CASE_DEFAULT
    END_SWITCH
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
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_A3_2"))
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_CommandPreCheckCounter, -2, LW(3), LW(4), 256, LW(0))
    SWITCH(LW(0))
        CASE_OR_EQUAL(7)
        CASE_OR_EQUAL(8)
        CASE_OR_EQUAL(9)
        CASE_OR_EQUAL(10)
        CASE_OR_EQUAL(11)
        CASE_OR_EQUAL(12)
        CASE_OR_EQUAL(13)
        CASE_OR_EQUAL(14)
        CASE_OR_EQUAL(15)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_A3_2"))
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
        CASE_END
    END_SWITCH
    CALL(btlevtcmd_OffUnitFlag, -2, 2097152)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_CLAUD_KISS1"), EVTDAT_POINTER_MAX, 0, LW(14))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_A3_2"))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(evt_eff, STRING(""), STRING("kiss"), 2, LW(0), LW(1), LW(2), LW(6), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_DamageDirect, LW(3), LW(4), 0, 0, 2, 1)
    CALL(btlevtcmd_StartAC, 1)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL(btlevtcmd_SetRGB, LW(3), LW(4), 255, 255, 255)
        LOOP(45)
            CALL(btlevtcmd_GetRGB, LW(3), LW(4), LW(0), LW(1), LW(2))
            SUBTRACT(LW(1), 2)
            IF_INT_LESS(LW(1), 190)
                SET_VALUE(LW(1), 190)
            END_IF
            SUBTRACT(LW(2), 4)
            CALL(btlevtcmd_SetRGB, LW(3), LW(4), LW(0), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
        LOOP(45)
            CALL(btlevtcmd_GetRGB, LW(3), LW(4), LW(0), LW(1), LW(2))
            SUBTRACT(LW(0), 2)
            SUBTRACT(LW(1), 2)
            ADD(LW(2), 3)
            IF_INT_LESS(LW(1), 0)
                SET_VALUE(LW(1), 0)
            END_IF
            CALL(btlevtcmd_SetRGB, LW(3), LW(4), LW(0), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_ResultAC)
    CALL(evt_snd_sfxoff, LW(14))
    DELETE_EVENT(LW(15))
    CALL(btlevtcmd_GetResultAC, LW(9))
    IF_BITS_SET(LW(9), 2)
        CALL(btlevtcmd_GetPartyTechLv, -2, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(2)
            SET_VALUE(LW(7), 3)
            CASE_DEFAULT
            SET_VALUE(LW(7), 2)
        END_SWITCH
        CALL(btlevtcmd_AcGetOutputParam, 0, LW(8))
        IF_INT_GREATER_OR_EQUAL(LW(8), LW(7))
            SUBTRACT(LW(8), LW(7))
            CALL(btlevtcmd_GetResultPrizeLv, LW(3), LW(8), LW(6))
            CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
            CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
            CALL(btlevtcmd_AudienceDeclareACResult, LW(12), LW(6))
        ELSE
            CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
        END_IF
    ELSE
        CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
    END_IF
    CALL(btlevtcmd_OnUnitFlag, -2, 2097152)
    CALL(btlevtcmd_SetRGB, LW(3), LW(4), 255, 255, 255)
    CALL(btlevtcmd_AcGetOutputParam, 0, LW(0))
    CALL(_make_kiss_weapon, LW(12), LW(0))
    IF_BITS_SET(LW(9), 2)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 131328, LW(5))
    ELSE
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    END_IF
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_GetHpDamage, LW(3), LW(0))
        WAIT_FRAMES(50)
        IF_INT_GREATER_OR_EQUAL(LW(0), 1)
            CALL(btlevtcmd_GetPos, -2, LW(5), LW(6), LW(7))
            CALL(btlevtcmd_GetStatusMg, -2, LW(8))
            MULTIPLY_FLOAT(LW(8), 40)
            ADD(LW(6), LW(8))
            CALL(btlevtcmd_RecoverHp, -2, 1, LW(0))
            CALL(evt_eff, STRING(""), STRING("recovery"), 7, LW(5), LW(6), LW(7), LW(0), 0, 0, 0, 0, 0, 0, 0)
        END_IF
    END_SIBLING_EVENT
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_CLAUD_KISS2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_A3_3"))
    WAIT_FRAMES(5)
    CALL(btlevtcmd_ACRStart, -2, 0, 29, 29, 0)
    CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
    IF_INT_EQUAL(LW(6), 2)
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, 0, 0, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_A_7"))
        WAIT_FRAMES(56)
    ELSE
        CALL(evt_audience_acrobat_notry)
        WAIT_FRAMES(60)
    END_IF
    LABEL(90)
    CALL(btlevtcmd_StopAC)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 40, 0, 5, 0, -1)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(partyClaudaAttack_KumoGuard)
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
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_GetPos, -2, LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, LW(3), LW(1), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    ADD(LW(0), LW(1))
    DIVIDE(LW(0), 2)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 1, LW(0), 72, 400, LW(0), 42, 0, 30, 5)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_AcSetParamAll, 3, 0, 40, 0, 1, 1, 2, 8)
    CALL(btlevtcmd_AcSetFlag, 5)
    CALL(btlevtcmd_SetupAC, -2, 18, 1, 0)
    WAIT_FRAMES(1)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_A4_2"))
    CALL(btlevtcmd_StartAC, 1)
    CALL(btlevtcmd_ResultAC)
    WAIT_FRAMES(60)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_A4_3"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_CLAUD_HIDE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_MS(150)
    CALL(btlevtcmd_GetResultAC, LW(6))
    CALL(btlevtcmd_StopAC)
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHeight, LW(3), LW(7))
    DIVIDE(LW(7), 2)
    ADD(LW(1), LW(7))
    ADD(LW(2), 20)
    CALL(evt_eff, STRING(""), STRING("kemuri_test"), 8, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_AcGetOutputParam, 0, LW(0))
    ADD(LW(0), 1)
    CALL(_make_kumoguard_weapon, LW(12), LW(0))
    CALL(btlevtcmd_AcGetOutputParam, 0, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 131328, LW(5))
        SUBTRACT(LW(0), 1)
        CALL(btlevtcmd_GetResultPrizeLv, -2, LW(0), LW(6))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(12), LW(6))
    ELSE
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
    END_IF
    CALL(btlevtcmd_ACRStart, -2, 0, 29, 29, 20)
    CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
    IF_INT_EQUAL(LW(6), 2)
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, 0, 0, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_A_7"))
        WAIT_MS(1000)
    ELSE
        CALL(evt_audience_acrobat_notry)
        WAIT_MS(1000)
    END_IF
    LABEL(99)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(clauda_appeal_event)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 66)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_APPEAL1"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 15)
    ADD(LW(1), 30)
    CALL(btlevtcmd_GetFaceDirection, -2, LW(14))
    MULTIPLY(LW(14), 45)
    CALL(evt_eff, STRING(""), STRING("kiss"), 2, LW(0), LW(1), LW(2), LW(14), 0, 0, 0, 0, 0, 0, 0)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_AudienceAppeal, -2)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(attack_audience_event)
    CALL(_check_aud_item_type, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL_CHILD(attack_bad_audience_event)
        CASE_DEFAULT
        CALL_CHILD(attack_good_audience_event)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(attack_bad_audience_event)
    CALL(btlevtcmd_OffUnitFlag, -2, 2097152)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_A1_1"))
    CALL(_get_attack_aud_no, LW(3), LW(0), LW(1), LW(2))
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(15)
        CALL(evt_btl_camera_set_mode, 0, 13)
        CALL(evt_btl_camera_set_homing_unit_audience, 0, -2, 1, LW(3))
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
        CALL(evt_btl_camera_set_zoom, 0, 200)
    END_SIBLING_EVENT
    SET_VALUE(LW(5), LW(1))
    ADD(LW(1), 28)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 5)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
    SET_VALUE(LW(6), 36)
    CALL(btlevtcmd_MarioJumpParam, -2, LW(0), LW(1), LW(2), LW(6))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_A1_1"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_CLAUD_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        DIVIDE(LW(6), 2)
        WAIT_FRAMES(LW(6))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_A1_2"))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_CLAUD_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SIBLING_EVENT
    CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), LW(6), 0, 0)
    CALL(_attack_aud, LW(3), 0)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(btlevtcmd_JumpContinue, -2)
    CALL(evt_btl_camera_shake_h, 0, 8, 0, 30, 13)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        WAIT_MS(1500)
        CALL(evt_btl_camera_set_mode, 0, 0)
        WAIT_MS(1000)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT_GET_ID(LW(14))
        CALL(evt_sub_intpl_init_float, 11, FLOAT(1.0f), FLOAT(0.4f), 10)
        LOOP(10)
            CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetScale, -2, LW(0), LW(5), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_sub_intpl_init_float, 11, FLOAT(0.4f), FLOAT(1.0f), 6)
        LOOP(6)
            CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetScale, -2, LW(0), LW(5), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        CALL(evt_sub_intpl_init_float, 11, FLOAT(1.0f), FLOAT(1.5f), 10)
        LOOP(10)
            CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetScale, -2, LW(5), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_sub_intpl_init_float, 11, FLOAT(1.5f), FLOAT(1.0f), 6)
        LOOP(6)
            CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetScale, -2, LW(5), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_WaitEventEnd, LW(14))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 20)
    ADD(LW(1), 20)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.3f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 18, -1)
    CALL(btlevtcmd_OnUnitFlag, -2, 2097152)
    WAIT_MS(400)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), 0, LW(2), 16, 0, 4, 0, 0)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetMoveFrame, -2, LW(0), LW(1), LW(2), FLOAT(6.0f), LW(15))
    ADD(LW(15), 30)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), LW(15), 20, 4, 0, -1)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(attack_good_audience_event)
    CALL(btlevtcmd_OffUnitFlag, -2, 2097152)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_A1_1"))
    CALL(_get_attack_aud_no, LW(3), LW(0), LW(1), LW(2))
    BEGIN_SIBLING_EVENT
        CALL(evt_btl_camera_set_mode, 0, 13)
        CALL(evt_btl_camera_set_homing_unit_audience, 0, -2, 1, LW(3))
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
        CALL(evt_btl_camera_set_zoom, 0, 200)
    END_SIBLING_EVENT
    SET_VALUE(LW(5), LW(1))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 30)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
    CALL(btlevtcmd_DivePosition, -2, LW(0), 0, LW(2), 40, 15, 4, 0, 0)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 20, 0, 4, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_A3_1"))
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_CLAUD_KISS1"), EVTDAT_POINTER_MAX, 0, LW(14))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_A3_2"))
    CALL(_get_attack_aud_no, LW(3), LW(0), LW(1), LW(2))
    ADD(LW(1), 25)
    CALL(evt_eff, STRING(""), STRING("kiss"), 2, LW(0), LW(1), LW(2), LW(6), 0, 0, 0, 0, 0, 0, 0)
    CALL(_attack_aud, LW(3), 1)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        WAIT_MS(1500)
        CALL(evt_btl_camera_set_mode, 0, 0)
        WAIT_MS(1000)
    END_SIBLING_EVENT
    WAIT_MS(500)
    CALL(evt_snd_sfxoff, LW(14))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_CLAUD_KISS2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_A3_3"))
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    WAIT_MS(300)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), 0, LW(2), 16, 0, 4, 0, 0)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetMoveFrame, -2, LW(0), LW(1), LW(2), FLOAT(6.0f), LW(15))
    ADD(LW(15), 30)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), LW(15), 20, 4, 0, -1)
    CALL(btlevtcmd_StartWaitEvent, -2)
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

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

static EVT_BEGIN(party_win_reaction)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_Y_1"))
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    RETURN
EVT_END()
// clang-format on

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_30_8041dcfc = 30.0f;
const f32 float_15_8041dd00 = 15.0f;
const f32 float_0_8041dd04 = 0.0f;
const f32 float_neg2_8041dd08 = -2.0f;
const f32 float_1_8041dd0c = 1.0f;
const f32 float_180_8041dd10 = 180.0f;
const f32 float_neg10_8041dd14 = -10.0f;
const f32 float_25_8041dd18 = 25.0f;
const f32 float_2_8041dd1c = 2.0f;

// 4-byte zero word that ends the unit's .sdata: an all-zero object needs explicit_zero_data to stay in .sdata and force_active to survive the linker.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_804122AC_sdata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset
