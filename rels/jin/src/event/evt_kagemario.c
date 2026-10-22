#include "evt_kagemario.h"
#include "evt/evt_cmd.h"
#include "evt/evt_mario.h"
#include "evt/evt_party.h"
#include "forcedata.h"
#include "driver/animdrv.h"
#include "driver/mapdrv.h"
#include "driver/swdrv.h"
#include "eff/eff_mahojin.h"
#include "party.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(test);
static USER_FUNC(check);
static s32 color_change_evt(s32 arg);
static void callback(MapTevCallback* param);

// clang-format off
extend_data(52,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(53,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static void callback(MapTevCallback* param) {
    s32 stage = param->tevStage;
    s32 texMap = param->texMap;
    s32 texCoord = param->texCoord;
    s32 numIndStage = param->numIndStage;

    if (marioGetPtr()->flags & MARIO_FLAG_BG_MODE) {
        GXSetTevColor(GX_TEVREG0, (GXColor){ 0x00, 0x00, 0x00, 0xFF });
    } else {
        GXSetTevColor(GX_TEVREG0, (GXColor){ 0x4D, 0x09, 0x52, 0xFF });
    }
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

static s32 color_change_evt(s32 arg) {
    s32 i;

    if (arg == 0) {
        for (i = 0; i < 3; i++) {
            animPoseSetMaterialFlagOff(marioGetPtr()->paperAnimGroupId[i], 0xA000000);
        }
        marioGetPtr()->flags &= ~MARIO_FLAG_IS_SHADOW_MARIO;
    } else {
        mapSetTevCallback(10, callback);
        for (i = 0; i < 3; i++) {
            animPoseSetMaterialFlagOn(marioGetPtr()->paperAnimGroupId[i], 0xA000000);
        }
        marioGetPtr()->flags |= MARIO_FLAG_IS_SHADOW_MARIO;
    }
    return 0;
}

static USER_FUNC(check) {
    if (partyGetPtr(0) != NULL) {
        animPoseSetMaterialFlagOff(partyGetPtr(0)->paperAnimGroup, 0xA000000);
    }
    return 0;
}

static EVT_BEGIN(evt_check)
    CALL(check)
    RETURN
EVT_END()

USER_FUNC(evt_kagemario_init_14_text_4DB8) {
    s32 i = swByteGet(0);

    if (i >= 201 && i < 212) {
        mapSetTevCallback(10, callback);
        for (i = 0; i < 3; i++) {
            animPoseSetMaterialFlagOn(marioGetPtr()->paperAnimGroupId[i], 0xA000000);
        }
        marioGetPtr()->flags |= MARIO_FLAG_IS_SHADOW_MARIO;
        evtEntry(evt_check, 0, 0);
    } else {
        for (i = 0; i < 3; i++) {
            animPoseSetMaterialFlagOff(marioGetPtr()->paperAnimGroupId[i], 0xA000000);
        }
        marioGetPtr()->flags &= ~MARIO_FLAG_IS_SHADOW_MARIO;
    }
    return 2;
}

USER_FUNC(evt_kagemario_change_14_text_4CF4) {
    s32 i = event->args[0];

    if (i == 0) {
        for (i = 0; i < 3; i++) {
            animPoseSetMaterialFlagOff(marioGetPtr()->paperAnimGroupId[i], 0xA000000);
        }
        marioGetPtr()->flags &= ~MARIO_FLAG_IS_SHADOW_MARIO;
    } else {
        mapSetTevCallback(10, callback);
        for (i = 0; i < 3; i++) {
            animPoseSetMaterialFlagOn(marioGetPtr()->paperAnimGroupId[i], 0xA000000);
        }
        marioGetPtr()->flags |= MARIO_FLAG_IS_SHADOW_MARIO;
    }
    return 2;
}

static USER_FUNC(test) {
    marioGetPtr()->prevPartyMemberId[0] = 0;
    marioGetPtr()->prevPartyMemberId[1] = 0;
    return 2;
}

EVT_BEGIN(evt_usu_kagemario_party_kill_14_data_AF00)
    SWITCH(GSW(0))
        CASE_BETWEEN(201, 205)
        CALL(evt_party_stop, 0)
        CALL(evt_party_set_pos, 0, 0, -1000, 0)
        CALL(evt_party_cont_onoff, 0, 0)
        CALL(evt_mario_kill_party, 0)
        CALL(evt_party_stop, 1)
        CALL(evt_party_set_pos, 1, 0, -1000, 0)
        CALL(evt_party_cont_onoff, 1, 0)
        CALL(evt_mario_clear_party, 1)
        CALL(test)
    END_SWITCH
    RETURN
EVT_END()
// clang-format on
