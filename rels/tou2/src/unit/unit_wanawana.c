#include "unit_wanawana.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "evt/evt_cmd.h"
#include "evt/evt_map.h"
#include "forcedata.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"
#include "battle/battle_sub.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/mapdrv.h"
#include "memory.h"
#include "system.h"
#include "math_ext.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <string.h>

#pragma pool_data off

#pragma force_active on
// This unit has its own copy of tanf (see its asm); math.h declares it as f32 tanf(f32).
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[1];
static BattleUnitKindPart parts[1];
static s32 init_event[29];
static s32 damage_event[10];
static s32 attack_event[19];
static s32 wait_event[6];

// clang-format off
extend_data(148,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(149,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_wanawana = {
    UNIT_UNUSED_WANAWANA,       // mUnitType
    "btl_un_wanawana",          // name
    2,                          // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    1,                          // mLevel
    0,                          // mBonusExp
    1,                          // bonusCoinRolls
    0,                          // bonusCoinRate
    0,                          // baseCoin
    100,                        // escapeRate
    9,                          // field_0x14
    24,                         // width
    24,                         // height
    { 24, 24 },                 // statusIconOffset
    { 0.0f, 0.0f, 0.0f },       // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },       // talkTogeBaseOffset
    { 0.0f, 0.0f, 0.0f },       // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // field_0x54
    0.0f,                       // field_0x58
    { 12.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 12.0f, 15.6f, 0.0f },     // kissHitOffset
    { 0.0f, 0.0f, 0.0f },       // cutBaseOffset
    0.0f,                       // cutWidth
    0.0f,                       // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    0,                          // swallowRate
    0,                          // swallowAttribute
    100,                        // field_0x8C
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_BTL_DAMAGE1",          // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0,                          // attributeFlags
    &regist,                    // vulnStatus
    1,                          // numParts
    parts,                      // mParts
    init_event,                 // initEvtScript
    data_table,                 // dataTable
};

static BattleUnitDefense defence = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    100, // sleepRate
    100, // stopRate
    100, // dizzyRate
    100, // poisonRate
    100, // confuseRate
    100, // electricRate
    100, // burnRate
    100, // freezeRate
    100, // hugeRate
    100, // tinyRate
    100, // attackUpRate
    100, // attackDownRate
    100, // defenseUpRate
    100, // defenseDownRate
    100, // allergicRate
    100, // frightRate
    100, // galeForceRate
    100, // fastRate
    100, // slowRate
    100, // dodgyRate
    100, // invisibleRate
    100, // instaKillRate
};

static BattleDispPose pose_table[4] = {
    { 0x1C, "S_1" },
    { 0x1F, "S_1" },
    { 0x27, "S_1" },
    { 0x45, "S_1" },
};

static BattleDataEntry data_table[1] = {
    { 0x00, NULL },
};

static BattleUnitKindPart parts[1] = {
    {
        1,                     // partNum
        "btl_un_wanawana",     // partName
        "nakayama_test4",      // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 40.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 40.0f, 0.0f }, // hitCursorBaseOffset
        20,                    // shadowSize
        30,                    // field_0x32
        255,                   // baseAlpha
        &defence,              // defense
        &defence_attr,         // defenseAttr
        0x00000009,            // attributes
        0,                     // counterAttributes
        pose_table,            // poseTable
    },
};

// .text:0x680 | 0x8ACC | size: 0x24
// The original has its own static tanf (the object defines a local tanf that
// calls the double tan and narrows the result), so include math_ext.h, not math.h.
static f32 tanf(f32 x) {
    return tan(x);
}

// .text:0x324 | 0x8770 | size: 0x35C
void replace_disp(CameraId cameraId, void* param) {
    WanawanaWork* p = param;
    CameraEntry save;
    CameraEntry* cam;
    AnimationPoseData* animData;
    Vec vmin;
    Vec vmax;
    Vec pos;
    s32 width;
    s32 height;

    memcpy(&save, camGetPtr(cameraId), 0x260);
    animData = animPoseGetAnimBaseDataPtr(p->poseId);
    vmin = (Vec){ animData->bboxMin.x, animData->bboxMin.y, animData->bboxMin.z };
    vmax = (Vec){ animData->bboxMax.x, animData->bboxMax.y, animData->bboxMax.z };
    width = (s32)(2.0f * (vmax.x - vmin.x));
    height = (s32)(2.0f * (vmax.z - vmin.z));
    cam = camGetPtr(cameraId);
    width += width & 1;
    height += height & 1;
    BtlUnit_GetPartsWorldPos(p->part, &pos.x, &pos.y, &pos.z);
    cam->cameraUp = (Vec){ 0.0f, 0.0f, -1.0f };
    cam->cameraAt = pos;
    cam->cameraPos = pos;
    cam->cameraPos.y += (f32)height / 2.0f / tanf(0.21816616f);
    C_MTXPerspective(cam->projection, 25.0f, (f32)width / (f32)height, 1.0f, 32767.0f);
    cam->projType = GX_PERSPECTIVE;
    C_MTXLookAt(cam->view, &cam->cameraPos, &cam->cameraUp, &cam->cameraAt);
    GXSetProjection(cam->projection, cam->projType);
    GXSetViewport(0.0f, 0.0f, (f32)width, (f32)height, 0.0f, 1.0f);
    GXSetScissor(0, 0, width, height);
    test_kururing_mapdisp(cameraId);
    sysWaitDrawSync();
    GXSetTexCopySrc(0, 0, width, height);
    GXSetTexCopyDst(width / 2, height / 2, GX_TF_RGB5A3, GX_TRUE);
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GXCopyTex(p->tex->address, GX_TRUE);
    GXInvalidateTexAll();
    GXPixModeSync();
    GXTexModeSync();
    memcpy(camGetPtr(cameraId), &save, 0x260);
    p->captured = 1;
}

// .text:0x26C | 0x86B8 | size: 0xB8
void replace_callback(BattleUnitPart* part, s32 arg) {
    WanawanaWork* p = part->mOwner->mExtraWork;
    Mtx mtx;

    if (p->captured == 0) {
        return;
    }
    PSMTXIdentity(mtx);
    if (arg == 0) {
        animSetPaperTexMtx(mtx, NULL, NULL);
        animSetPaperTexObj(&p->texObj, NULL, NULL, p->tex, NULL, NULL, 1, 1);
    } else {
        animSetPaperTexMtx(NULL, NULL, NULL);
        animSetPaperTexObj(NULL, NULL, NULL, NULL, NULL, NULL, 1, 1);
    }
}

// .text:0x0 | 0x844C | size: 0x26C
USER_FUNC(replace_evt) {
    WanawanaWork* p;
    BattleUnitPart* part;
    BattleUnit* unit;
    AnimationPoseData* animData;
    Vec vmin;
    Vec vmax;
    s32 width;
    s32 height;
    u32 size;

    part = BattleGetUnitPartsPtr(BattleTransID(event, -2), 1);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, -2));
    animData = animPoseGetAnimBaseDataPtr(part->poseId);
    vmin = (Vec){ animData->bboxMin.x, animData->bboxMin.y, animData->bboxMin.z };
    vmax = (Vec){ animData->bboxMax.x, animData->bboxMax.y, animData->bboxMax.z };
    width = (s32)(2.0f * (vmax.x - vmin.x));
    height = (s32)(2.0f * (vmax.z - vmin.z));
    width += width & 1;
    height += height & 1;
    p = __memAlloc(HEAP_BATTLE, 0x60);
    width /= 2;
    height /= 2;
    size = GXGetTexBufferSize(width, height, GX_TF_RGB5A3, GX_FALSE, 0);
    p->tex = smartAlloc(size, 1);
    p->poseId = part->poseId;
    p->part = part;
    GXInitTexObj(&p->texObj, p->tex->address, width, height, GX_TF_RGB5A3, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GXInitTexObjLOD(&p->texObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    PSMTXIdentity(p->mtx);
    animGetPtr()->poses[part->poseId].flags |= 0x10;
    part->dispCallback = replace_callback;
    unit->mExtraWork = p;
    unit->mSmartEntry = p->tex;
    p->captured = 0;
    dispEntry(CAMERA_SHADOW, 0.0f, 0, replace_disp, p);
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_yuka"), 4194304)
    CALL(replace_evt)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

// clang-format on
