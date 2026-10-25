#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario_pouch.h"
#include "battle/battle_sub.h"
#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "driver/animdrv.h"
#include "dolphin/gx/GXTev.h"
#include "driver/mapdrv.h"
#include "unit/unit_mario.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "evt/evt_eff.h"
#include "evt/evt_map.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "party/party_motion.h"
#include "battle/battle_attack_audience.h"
#include "battle/battle_disp.h"
static s32 _mario_makkuro_set(EventEntry* event, BOOL isFirstCall);  // local to this file in the original
static s32 _get_mario_hammer_lv(EventEntry* event, BOOL isFirstCall);  // local to this file in the original

//local prototypes inherited from headers that do not declare these yet

//strings and constants are defined with the rest of the file's .rodata/.sdata2

#define ANIM_L "b_mario_l"
#define ANIM_W "b_mario_w"
#define ANIM_WL "b_mario_wl"

static void callback(MapTevCallback* param);

extern s32 btldefaultevt_avoid_counter_event[];
extern s32 subset_attack_ready_jump_event[];
s32 mario_ride_btl_entry_event[120];
static s32 init_event[75];
s32 mario_dead_event[57];
s32 mario_emergency_revival_event[130];
s32 mario_appeal_event[30];
static s32 attack_audience_event[15];
s32 btlataudevtMarioJump[237];
s32 btlataudevtMarioHammer[356];
static BtlUnitPoseSound pose_sound_timing_table[2];
static s32 damage_event[10];
static s32 attack_event[2];
static s32 wait_event[6];
s32 mario_win_reaction[9];

// clang-format off

BattleDispPose pose_table_mario_stay[34] = {
    { 0x04, "M_S_3" },
    { 0x01, "M_N_5" },
    { 0x02, "M_Z_10" },
    { 0x09, "M_Z_10" },
    { 0x05, "M_X_1" },
    { 0x03, "M_X_1" },
    { 0x1B, "M_Z_11" },
    { 0x1C, "M_S_1" },
    { 0x23, "M_V_2" },
    { 0x25, "M_B_2" },
    { 0x24, "M_D_6" },
    { 0x1F, "M_B_1" },
    { 0x20, "M_V_1" },
    { 0x26, "M_N_7" },
    { 0x27, "M_D_1" },
    { 0x2B, "M_S_1" },
    { 0x28, "M_W_1" },
    { 0x2A, "M_R_1" },
    { 0x2C, "M_J_1B" },
    { 0x34, "M_I_1" },
    { 0x39, "M_S_3" },
    { 0x38, "M_S_3" },
    { 0x3E, "M_V_2" },
    { 0x3F, "M_V_2" },
    { 0x3A, "M_A_3B" },
    { 0x3B, "M_A_3B" },
    { 0x3C, "M_D_2" },
    { 0x3D, "M_V_1" },
    { 0x43, "M_C_3" },
    { 0x42, "M_N_8" },
    { 0x40, "M_I_O" },
    { 0x31, "M_J_1B" },
    { 0x2D, "M_B_2" },
    { 0x45, "M_S_1" },
};

BattleDataEntry data_table_mario[10] = {
    { 0x05, mario_appeal_event },
    { 0x06, attack_audience_event },
    { 0x30, btldefaultevt_Dummy },
    { 0x33, mario_emergency_revival_event },
    { 0x31, mario_dead_event },
    { 0x40, mario_win_reaction },
    { 0x07, subset_attack_ready_jump_event },
    { 0x41, mario_ride_btl_entry_event },
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

BattleDispPose pose_table_object[3] = {
    { 0x1C, "KRN_H_1" },
    { 0x27, "KRN_H_1" },
    { 0x45, "KRN_H_1" },
};

BattleUnitKindPart unitpartsdata_Mario[3] = {
    {
        1,                                    // partNum
        "btl_un_mario",                       // partName
        "b_mario",                            // modelName
        { 0.0f, 0.0f, 0.0f },                 // positionOffset
        { 16.0f, 36.0f, 5.0f },               // hitBaseOffset
        { 0.0f, 40.0f, 0.0f },                // hitCursorBaseOffset
        20,                                   // shadowSize
        36,                                   // field_0x32
        255,                                  // baseAlpha
        (BattleUnitDefense*)defence,          // defense
        (BattleUnitDefenseAttr*)defence_attr, // defenseAttr
        0x00000009,                           // attributes
        0,                                    // counterAttributes
        pose_table_mario_stay,                // poseTable
    },
    {
        2,                                    // partNum
        "btl_un_mario",                       // partName
        "b_mario",                            // modelName
        { 0.0f, 0.0f, 0.0f },                 // positionOffset
        { 16.0f, 36.0f, 5.0f },               // hitBaseOffset
        { 0.0f, 40.0f, 0.0f },                // hitCursorBaseOffset
        20,                                   // shadowSize
        36,                                   // field_0x32
        255,                                  // baseAlpha
        (BattleUnitDefense*)defence,          // defense
        (BattleUnitDefenseAttr*)defence_attr, // defenseAttr
        0x13010000,                           // attributes
        0,                                    // counterAttributes
        pose_table_object,                    // poseTable
    },
    {
        3,                                    // partNum
        "btl_un_mario",                       // partName
        "b_mario",                            // modelName
        { 0.0f, 0.0f, 0.0f },                 // positionOffset
        { 16.0f, 36.0f, 5.0f },               // hitBaseOffset
        { 0.0f, 40.0f, 0.0f },                // hitCursorBaseOffset
        20,                                   // shadowSize
        36,                                   // field_0x32
        255,                                  // baseAlpha
        (BattleUnitDefense*)defence,          // defense
        (BattleUnitDefenseAttr*)defence_attr, // defenseAttr
        0x13010000,                           // attributes
        0,                                    // counterAttributes
        pose_table_object,                    // poseTable
    },
};

BattleUnitKind unitdata_Mario = {
    UNIT_MARIO,                      // mUnitType
    "btl_un_mario",                  // name
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
    22,                              // width
    36,                              // height
    { 11, 36 },                      // statusIconOffset
    { 0.0f, 0.0f, 0.0f },            // centerOffset
    { 0, 0 },                        // hpGaugeOffset
    { 0.0f, 40.0f, 0.0f },           // talkTogeBaseOffset
    { 0.0f, 40.0f, 0.0f },           // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },            // burnFlameOffset
    0.0f,                            // field_0x54
    0.0f,                            // field_0x58
    { 11.0f, 0.0f, 0.0f },           // bintaHitOffset
    { 11.0f, 23.4f, 0.0f },          // kissHitOffset
    { 0.0f, 0.0f, 0.0f },            // cutBaseOffset
    0.0f,                            // cutWidth
    0.0f,                            // cutHeight
    0,                               // turnOrder
    0,                               // turnOrderVariance
    -1,                              // swallowRate
    0,                               // swallowAttribute
    100,                             // field_0x8C
    20,                              // itemStealParam
    { 0.0f, 0.0f, 0.0f },            // starPointDispOffset
    "SFX_MARIO_DAMAGE1",             // damageSfxName
    "SFX_MARIO_DAMAGE_FIRE1",        // fireDamageSfxName
    "SFX_MARIO_DAMAGE_ICE1",         // iceDamageSfxName
    "SFX_MARIO_DAMAGE_BIRIBIRI1",    // electricDamageSfxName
    0x02040000,                      // attributeFlags
    (BattleVulnerableStatus*)regist, // vulnStatus
    3,                               // numParts
    unitpartsdata_Mario,             // mParts
    init_event,                      // initEvtScript
    data_table_mario,                // dataTable
};

static EVT_BEGIN(battle_entry_event)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 250)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_map_replayanim, 0, STRING("dontyo"))
    WAIT_FRAMES(40)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 61, -1, 0)
    LABEL(0)
    CALL(_get_local_frame, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(50)
    END_IF
    IF_INT_EQUAL(LW(0), 10)
        GOTO(50)
    END_IF
    WAIT_FRAMES(1)
    GOTO(0)
    LABEL(50)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_S_1"))
    RETURN
EVT_END()

s32 _get_local_frame(EventEntry* event, BOOL isFirstCall) {
    s32 arg = event->args[0];
    AnimationPose* pose =
        animPoseGetAnimPosePtr(BattleGetUnitPartsPtr(BattleTransID(event, -2), 1)->poseId);

    evtSetValue(event, arg, (s32)pose->clipTime);
    return EVT_RETURN_DONE;
}


s32 mario_pinch_pose_sound_callback(BattleUnit* unit, BtlUnitPoseSound* entry, void* ps) {
    s32 result;
    if (unit->work[8] & 1) {
        result = BtlUnit_snd_se(unit, "SFX_VOICE_MARIO_RELIEF2", 0xF1194D80, 0);
    }
    else {
        result = BtlUnit_snd_se(unit, "SFX_VOICE_MARIO_RELIEF1", 0xF1194D80, 0);
    }
    unit->work[8] = unit->work[8] + 1;
    return result;
}


static void callback(MapTevCallback* param) {
    GXColor color = (GXColor){ 0x4D, 0x09, 0x52, 0xFF };
    s32 stage = param->tevStage;
    s32 texMap = param->texMap;
    s32 texCoord = param->texCoord;
    s32 numIndStage = param->numIndStage;

    GXSetTevColor(GX_TEVREG0, color);
    GXSetTevOrder(stage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(stage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
    GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    GXSetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP0);

    param->tevStage = stage + 1;
    param->texCoord = texCoord;
    param->numIndStage = numIndStage;
    param->texMap = texMap;
}


void faker_mario_makkuro_set(void) {
    mapSetTevCallback(0xB, callback);
}

static s32 _mario_makkuro_set(EventEntry* event, BOOL isFirstCall) {
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);

    if (mario != NULL) {
        mapSetTevCallback(0xB, callback);
        animPoseSetMaterialFlagOn(BtlUnit_GetPartsPtr(mario, 1)->poseId, 0xB000000);
        animPoseSetMaterialFlagOn(BtlUnit_GetPartsPtr(mario, 2)->poseId, 0xB000000);
        animPoseSetMaterialFlagOn(BtlUnit_GetPartsPtr(mario, 3)->poseId, 0xB000000);
    }
    return EVT_RETURN_DONE;
}


s32 _mario_super_emblem_anim_set(EventEntry* event, s32 mode) {
    s32* args = event->args;
    const char* anim;
    BattleWork* work;
    s32 emblem;
    s32 id;

    work = _battleWorkPointer;
    id = evtGetValue(event, *args++);
    id = BattleTransID(event, id);

    BattleGetUnitPtr(work, id);

    emblem = 0;
    if (pouchEquipCheckBadge(ITEM_L_EMBLEM)) {
        emblem = 1;
    }
    if (pouchEquipCheckBadge(ITEM_W_EMBLEM)) {
        emblem += 2;
    }

    switch (emblem) {
    case 1:
        anim = ANIM_L;
        break;
    case 2:
        anim = ANIM_W;
        break;
    case 3:
        anim = ANIM_WL;
        break;
    default:
        anim = NULL;
        break;
    }

    if (mode != 0) {
        BattleUnitPart* part;
        if (anim == NULL) {
            return EVT_RETURN_DONE;
        }
        part = BattleGetUnitPartsPtr(id, 1);
        animPoseGetAnimPosePtr(part->poseId);
        if (part->poseId != -1) {
            animPoseRelease(part->poseId);
            part->poseId = -1;
        }
        part = BattleGetUnitPartsPtr(id, 2);
        animPoseGetAnimPosePtr(part->poseId);
        if (part->poseId != -1) {
            animPoseRelease(part->poseId);
            part->poseId = -1;
        }
        part = BattleGetUnitPartsPtr(id, 3);
        animPoseGetAnimPosePtr(part->poseId);
        if (part->poseId != -1) {
            animPoseRelease(part->poseId);
            part->poseId = -1;
        }
    }

    if (animEffectAsync(anim, 1)) {
        BattleUnitPart* part;
        part = BattleGetUnitPartsPtr(id, 1);
        part->poseId = animPoseEntry(anim, 1);
        part = BattleGetUnitPartsPtr(id, 2);
        part->poseId = animPoseEntry(anim, 1);
        part = BattleGetUnitPartsPtr(id, 3);
        part->poseId = animPoseEntry(anim, 1);
        return EVT_RETURN_DONE;
    }

    return 0;
}

USER_FUNC(_get_mario_hammer_lv) {
    s32 arg = event->args[0];

    evtSetValue(event, arg, pouchGetPtr()->hammerLevel);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(mario_ride_btl_entry_event)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 250)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_map_replayanim, 0, STRING("dontyo"))
    WAIT_FRAMES(60)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetHomePos, -4, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 30)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 50, -1, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 44)
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 60)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.5f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 24, -1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 10, -1, 0)
    LABEL(0)
    CALL(_get_local_frame, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(50)
    END_IF
    IF_INT_EQUAL(LW(0), 10)
        GOTO(50)
    END_IF
    WAIT_FRAMES(1)
    GOTO(0)
    LABEL(50)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_S_1"))
    RETURN
EVT_END()

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetJumpSound, -2, STRING("SFX_MARIO_BATTLE_JUMP1"), STRING("SFX_MARIO_JUMP_LANDING1"))
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_MARIO_BATTLE_MOVE1L"), STRING("SFX_MARIO_BATTLE_MOVE1R"), 0, 20, 20)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_MARIO_BATTLE_MOVE1L"), STRING("SFX_MARIO_BATTLE_MOVE1R"), 0, 10, 10)
    CALL(btlevtcmd_AnimeSetPoseSoundTable, -2, 1, PTR(pose_sound_timing_table))
    CALL(btlevtcmd_SetUnitWork, -2, 8, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 3, 0, 0)
    CALL(_mario_super_emblem_anim_set, -2)
    CALL(btlevtcmd_SetEventEntry, -2, PTR(battle_entry_event))
    IF_INT_GREATER_OR_EQUAL(GSW(0), 201)
        IF_INT_LESS(GSW(0), 212)
            CALL(_mario_makkuro_set)
        END_IF
    END_IF
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(mario_dead_event)
    CALL(evt_btl_camera_set_prilimit, 1)
    CALL(evt_btl_camera_set_mode, 1, 7)
    CALL(evt_btl_camera_set_homing_unit, 1, LW(10), -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 1, 2)
    CALL(evt_btl_camera_set_zoom, 1, 500)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_DOWN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, LW(10), LW(11), STRING("M_I_S"))
    WAIT_MS(2066)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_CONDITION_DOWN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, LW(10), LW(11), STRING("M_D_2"))
    WAIT_MS(2000)
    CALL(btlevtcmd_KillUnit, LW(10), 1)
    CALL(evt_btl_camera_set_prilimit, 0)
    RETURN
EVT_END()

EVT_BEGIN(mario_emergency_revival_event)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_DOWN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, LW(10), LW(11), STRING("M_I_S"))
    WAIT_MS(2066)
    CALL(btlevtcmd_ClearAllStatus, LW(10))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_CONDITION_DOWN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, LW(10), LW(11), STRING("M_D_2"))
    WAIT_MS(1000)
    CALL(btlevtcmd_ConsumeItemReserve, LW(10), 151)
    WAIT_MS(1000)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
        ADD(LW(1), 40)
        CALL(btlevtcmd_DispItemIcon, LW(0), LW(1), LW(2), 151, 120)
    END_SIBLING_EVENT
    WAIT_FRAMES(120)
    CALL(btlevtcmd_OnOffStatus, LW(10), 27, 0, 0, 0)
    CALL(btlevtcmd_SetHp, LW(10), 0)
    CALL(btlevtcmd_RecoverHp, LW(10), LW(11), 10)
    CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
    ADD(LW(1), 30)
    CALL(evt_eff, 0, STRING("recovery"), 0, LW(0), LW(1), LW(2), 10, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(2000)
    CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 44)
    CALL(btlevtcmd_JumpSetting, LW(10), 0, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(mario_appeal_event)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 66)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_APPEAL1"), LW(0), LW(1), LW(2), 0)
    WAIT_FRAMES(60)
    CALL(btlevtcmd_AudienceAppeal, -2)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(attack_audience_event)
    CALL(evt_sub_random, 1, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL_CHILD(btlataudevtMarioJump)
    ELSE
        CALL_CHILD(btlataudevtMarioHammer)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(btlataudevtMarioJump)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
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
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_BATTLE_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    SET_VALUE(LW(6), 36)
    CALL(btlevtcmd_MarioJumpParam, -2, LW(0), LW(1), LW(2), LW(6))
    CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), LW(6), 0, -1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1D"))
    WAIT_FRAMES(3)
    CALL(_attack_aud, LW(3), 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 40)
    SET_VALUE(LW(1), LW(5))
    ADD(LW(2), 5)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 10, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 10)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 6, -1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        WAIT_MS(500)
        CALL(evt_btl_camera_set_mode, 0, 0)
        WAIT_MS(1000)
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(8.0f))
    SET_VALUE(LW(0), 0)
    CALL(btlevtcmd_GetStageAudienceAreaSize, LW(4), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), LW(4))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    SET_VALUE(LW(0), 0)
    CALL(btlevtcmd_GetStageSize, LW(4), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), LW(4))
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(btlataudevtMarioHammer)
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
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(_get_mario_hammer_lv, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(2)
        SET_VALUE(LW(0), STRING("M_H_5A"))
        SET_VALUE(LW(1), STRING("M_H_5B"))
        SET_VALUE(LW(2), STRING("M_H_14"))
        CASE_EQUAL(3)
        SET_VALUE(LW(0), STRING("M_H_9A"))
        SET_VALUE(LW(1), STRING("M_H_9B"))
        SET_VALUE(LW(2), STRING("M_H_15"))
        CASE_DEFAULT
        SET_VALUE(LW(0), STRING("M_H_1A"))
        SET_VALUE(LW(1), STRING("M_H_1B"))
        SET_VALUE(LW(2), STRING("M_H_13"))
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(0))
    CALL(_get_mario_hammer_lv, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(2)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_MARIO_HAMMER_HOLD2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CASE_EQUAL(1)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_MARIO_HAMMER_HOLD1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CASE_DEFAULT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_MARIO_HAMMER_HOLD3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SWITCH
    WAIT_MS(500)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(1))
    WAIT_FRAMES(1)
    CALL(_get_mario_hammer_lv, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(2)
        CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_SWING2"), 0)
        CASE_EQUAL(1)
        CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_SWING1"), 0)
        CASE_DEFAULT
        CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_SWING3"), 0)
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(2))
    WAIT_FRAMES(7)
    CALL(_get_mario_hammer_lv, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(2)
        CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_HIT_GROUND3"), 0)
        CASE_EQUAL(1)
        CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_HIT_GROUND2"), 0)
        CASE_DEFAULT
        CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_HIT_GROUND1"), 0)
    END_SWITCH
    CALL(_attack_aud, LW(3), 0)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_shake_h, 0, 2, 0, 10, 13)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        WAIT_MS(1500)
        CALL(evt_btl_camera_set_mode, 0, 0)
        WAIT_MS(1000)
    END_SIBLING_EVENT
    WAIT_MS(500)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_H_16"))
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(8.0f))
    SET_VALUE(LW(0), 0)
    CALL(btlevtcmd_GetStageAudienceAreaSize, LW(4), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), LW(4))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, -2, LW(8), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static BtlUnitPoseSound pose_sound_timing_table[2] = {
    { "M_S_3", 0.25f, mario_pinch_pose_sound_callback, NULL, 0, 1 },
    { NULL, 0.0f, NULL, NULL, 0, 1 },
};

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

EVT_BEGIN(mario_win_reaction)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_V_2"))
    WAIT_MS(1000)
    RETURN
EVT_END()
// clang-format on

// 4-byte zero word that ends the unit's .sdata: an all-zero object needs explicit_zero_data to stay in .sdata and force_active to survive the linker.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_80411DB4_sdata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset
