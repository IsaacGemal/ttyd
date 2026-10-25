// piders_draw_yarn_sub and piders_draw_yarn, shared by the Pider units (unit_piders, all three versions) and
// unit_churantalar. Included where they sit in the unit's function order (it sets the .text layout).
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "evt/evt_cmd.h"
#include "memory.h"
#include "rel/common/unit_piders.h"
#include "system.h"

static void piders_draw_yarn_sub(CameraId cameraId, void* param) {
    PidersYarnWork* yarn = param;
    CameraEntry* camera;

    camera = camGetPtr(CAMERA_3D);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    GXSetChanMatColor(GX_COLOR0A0, (GXColor){ 0xFF, 0xFF, 0xFF, yarn->alpha });
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXLoadPosMtxImm(camera->view, 0);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(yarn->top.x - 1.0f, yarn->top.y, yarn->top.z - 5.0f);
    GXPosition3f32(1.0f + yarn->top.x, yarn->top.y, yarn->top.z - 5.0f);
    GXPosition3f32(1.0f + yarn->bottom.x, yarn->bottom.y, yarn->bottom.z - 5.0f);
    GXPosition3f32(yarn->bottom.x - 1.0f, yarn->bottom.y, yarn->bottom.z - 5.0f);
}

static USER_FUNC(piders_draw_yarn) {
    u8 alpha = evtGetValue(event, event->args[0]);
    s32 id = BattleTransID(event, -2);
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, id);
    PidersYarnWork* yarn = unit->mExtraWork;

    BtlUnit_GetHomePos(unit, &yarn->bottom.x, &yarn->bottom.y, &yarn->bottom.z);
    yarn->bottom.y = (f32)unit->work[2];
    BtlUnit_GetHomePos(unit, &yarn->top.x, &yarn->top.y, &yarn->top.z);
    yarn->top.y = 400.0f;
    yarn->alpha = alpha;
    dispEntry(CAMERA_3D, 0.0f, 2, piders_draw_yarn_sub, yarn);
    return EVT_RETURN_DONE;
}
