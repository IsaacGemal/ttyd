#include "eki_03.h"
#include "driver/envdrv.h"
#include "driver/mapdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cloud.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mobj.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "matching.h"
#include "math_ext.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
static smartEntry* ind_lp;
static void sun_tev(EnvironmentYami* yami);
static USER_FUNC(test_glare);

// clang-format off
extend_data(16,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(17,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(w1_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door_1"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w1_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door_1"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w2_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door_2"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w2_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door_2"), 0, LW(0), 0)
    RETURN
EVT_END()

// TODO: type
static u32 bero_data[45] = {
    (u32)"w_bero_1", 0x00010003, 0x00004E20, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)"eki_01", (u32)"e_bero",
    0x00050005, (u32)&w1_close, (u32)&w1_open, (u32)"w_bero_2",
    0x00010003, 0x00004E20, 0x000186A0, 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)"eki_04", (u32)"e_bero", 0x00050005,
    (u32)&w2_close, (u32)&w2_open, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000,
};

EVT_BEGIN(evt_poster)
    SET_VALUE(GSW(0), 316)
    SET_VALUE(LW(0), 1)
    CALL_CHILD(bero_case_switch_on)
    RETURN
EVT_END()

static EVT_BEGIN(evt_chk)
    IF_INT_EQUAL(GSWF(3733), 0)
        RETURN
    END_IF
    IF_INT_EQUAL(GSWF(3734), 0)
        RETURN
    END_IF
    IF_INT_EQUAL(GSWF(3735), 0)
        RETURN
    END_IF
    IF_INT_EQUAL(GSWF(3736), 0)
        RETURN
    END_IF
    IF_INT_EQUAL(GSWF(3737), 0)
        RETURN
    END_IF
    IF_INT_EQUAL(GSWF(3738), 0)
        RETURN
    END_IF
    IF_INT_EQUAL(GSWF(3739), 0)
        RETURN
    END_IF
    IF_INT_EQUAL(GSWF(3740), 0)
        RETURN
    END_IF
    IF_INT_EQUAL(GSWF(3741), 0)
        RETURN
    END_IF
    IF_INT_EQUAL(GSWF(3742), 0)
        RETURN
    END_IF
    IF_INT_EQUAL(GSWF(3743), 0)
        RETURN
    END_IF
    SET_VALUE(LW(0), 1)
    CALL_CHILD(bero_case_switch_on)
    CALL(evt_hitobj_onoff, STRING("a_kabe"), 1, 0)
    IF_INT_LESS(GSW(0), 316)
        SET_VALUE(GSW(0), 316)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(evt_poster_1)
    SET_VALUE(GSWF(3729), 1)
    CALL_CHILD(evt_chk)
    RETURN
EVT_END()

static EVT_BEGIN(evt_poster_2)
    SET_VALUE(GSWF(3730), 1)
    CALL_CHILD(evt_chk)
    RETURN
EVT_END()

static EVT_BEGIN(evt_poster_3)
    SET_VALUE(GSWF(3731), 1)
    CALL_CHILD(evt_chk)
    RETURN
EVT_END()

static EVT_BEGIN(evt_poster_4)
    SET_VALUE(GSWF(3732), 1)
    CALL_CHILD(evt_chk)
    RETURN
EVT_END()

static EVT_BEGIN(evt_poster_5)
    SET_VALUE(GSWF(3733), 1)
    CALL_CHILD(evt_chk)
    RETURN
EVT_END()

static EVT_BEGIN(evt_poster_6)
    SET_VALUE(GSWF(3734), 1)
    CALL_CHILD(evt_chk)
    RETURN
EVT_END()

static EVT_BEGIN(evt_poster_7)
    SET_VALUE(GSWF(3735), 1)
    CALL_CHILD(evt_chk)
    RETURN
EVT_END()

static EVT_BEGIN(evt_poster_8)
    SET_VALUE(GSWF(3736), 1)
    CALL_CHILD(evt_chk)
    RETURN
EVT_END()

static EVT_BEGIN(evt_poster_9)
    SET_VALUE(GSWF(3737), 1)
    CALL_CHILD(evt_chk)
    RETURN
EVT_END()

EVT_BEGIN(evt_poster_10)
    SET_VALUE(GSWF(3738), 1)
    CALL(evt_hitobj_onoff, STRING("a_kami_13"), 1, 0)
    CALL_CHILD(evt_chk)
    RETURN
EVT_END()

EVT_BEGIN(evt_poster_11)
    SET_VALUE(GSWF(3739), 1)
    CALL(evt_hitobj_onoff, STRING("a_kami_14"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("a_kami_15"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("a_kami_6"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("a_kami_7"), 1, 0)
    CALL_CHILD(evt_chk)
    RETURN
EVT_END()

EVT_BEGIN(evt_poster_12)
    SET_VALUE(GSWF(3740), 1)
    CALL(evt_hitobj_onoff, STRING("a_kami_8"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("a_kami_9"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("a_kami_5"), 1, 0)
    CALL_CHILD(evt_chk)
    RETURN
EVT_END()

EVT_BEGIN(evt_poster_13)
    SET_VALUE(GSWF(3741), 1)
    CALL_CHILD(evt_chk)
    RETURN
EVT_END()

EVT_BEGIN(evt_poster_14)
    SET_VALUE(GSWF(3742), 1)
    CALL_CHILD(evt_chk)
    RETURN
EVT_END()

EVT_BEGIN(evt_poster_15)
    SET_VALUE(GSWF(3743), 1)
    CALL_CHILD(evt_chk)
    RETURN
EVT_END()

// TODO: write test_glare (static, 0x118 bytes) here:
extern const f32 float_31p416_eki_000091e0;
extern const f32 float_0p015625_eki_000091e4;
extern const f32 float_0p125_eki_000091e8;
extern const f32 float_128_eki_000091ec;
extern const f32 float_3_eki_000091f0;
extern const f32 zero_eki_000091f4;
extern const f32 float_0p5_eki_000091f8;

static USER_FUNC(test_glare) {
    MapObject* obj = mapGetMapObj("sun");

    marioGetPtr();
    if (isFirstCall) {
        *(u32*)obj->field_0x0 |= 0x20000000;
        return 0;
    }

    if (*(u16*)((u8*)obj + 0xEC) == 0x3FF || *(u16*)((u8*)obj + 0xEE) == 0x3FF || *(u16*)((u8*)obj + 0xF0) == 0x3FF
        || *(u16*)((u8*)obj + 0xF2) == 0x3FF) {
        envGlareFilter(0, 0, 0, 0, 0);
    }
    else if (*(u16*)((u8*)obj + 0xEC) == 0 && *(u16*)((u8*)obj + 0xEE) == 0 && *(u16*)((u8*)obj + 0xF0) == 0
             && *(u16*)((u8*)obj + 0xF2) == 0) {
        envGlareFilter(0, 0, 0, 0, 0);
    }
    else {
        s16 y0 = *(u16*)((u8*)obj + 0xEE) - 0x32;
        s16 x0 = *(u16*)((u8*)obj + 0xEC) - 0x32;
        s16 y1 = *(u16*)((u8*)obj + 0xF2) + 0x32;
        s16 x1 = *(u16*)((u8*)obj + 0xF0) + 0x32;

        envGlareFilter(1, y0, x0, y1, x1);
        envGlareFilterZ(1.148f);
    }
    return 0;
}

// TODO: write sun_tev (static, 0x308 bytes) here:
static void sun_tev(EnvironmentYami* yami) {
    GXTexObj texObj;
    u8* image;
    s32 i;
    s32 j;
    s32 tevStage = yami->tevStage;
    s32 texMap = yami->texMap;
    s32 texCoord = yami->texCoord;
    s32 indStage = yami->unk18;
    f32 indMtx[2][3];

    if (ind_lp == NULL) {
        ind_lp = smartAlloc(GXGetTexBufferSize(0x40, 0x40, GX_TF_IA8, GX_FALSE, GX_FALSE), 1);
    }

    image = (u8*)ind_lp->address;
    for (i = 0; i < 0x40; i++) {
        for (j = 0; j < 0x40; j++) {
            image[(i / 4) * 32 + (j / 4) * 512 + ((i % 4) + (j % 4) * 4) * 2]
                = (s32)(float_3_eki_000091f0 * sin((float_31p416_eki_000091e0 * (f32)j) * float_0p015625_eki_000091e4 - (f32)gp->retraceCount * float_0p125_eki_000091e8)
                        + float_128_eki_000091ec);
            image[(i / 4) * 32 + (j / 4) * 512 + ((i % 4) + (j % 4) * 4) * 2 + 1] = 0x80;
        }
    }

    DCFlushRange(image, 0x2000);
    GXInitTexObj(&texObj, image, 0x40, 0x40, GX_TF_IA8, GX_REPEAT, GX_REPEAT, GX_FALSE);
    GXInitTexObjLOD(&texObj, GX_NEAR, GX_NEAR, zero_eki_000091f4, zero_eki_000091f4, zero_eki_000091f4, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&texObj, texMap);

    indMtx[0][0] = float_0p5_eki_000091f8;
    indMtx[0][1] = zero_eki_000091f4;
    indMtx[0][2] = zero_eki_000091f4;
    indMtx[1][0] = zero_eki_000091f4;
    indMtx[1][1] = float_0p5_eki_000091f8;
    indMtx[1][2] = zero_eki_000091f4;
    GXSetIndTexMtx(GX_ITM_0, indMtx, 1);
    GXSetIndTexOrder(indStage, GX_TEXCOORD0, texMap);
    GXSetIndTexCoordScale(indStage, GX_ITS_1, GX_ITS_1);
    GXSetTevIndWarp(GX_TEVSTAGE0, indStage, 1, 0, GX_ITM_0);

    yami->tevStage = tevStage;
    yami->texMap = texMap + 1;
    yami->texCoord = texCoord;
    yami->unk18 = indStage + 1;
}

EVT_BEGIN(eki_03_init_evt)
    SET_VALUE(LW(0), PTR(bero_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_cam_road_reset)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_HOM1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG6_EKI4"))
    CALL(evt_snd_bgmoff, 26624)
    CALL(evt_item_entry, STRING("item"), 278, -550, 220, -50, 16, GSWF(3746), 0)
    CALL(evt_mobj_powerupblk, STRING("pblk01"), 450, 100, 10, 0, GSWF(5561), 0)
    CALL(evt_mobj_brick, STRING("blk01"), 250, 300, -100, 0, 0, 0, LSWF(1))
    CALL(evt_mobj_brick, STRING("blk02"), 250, 385, -100, 129, 11, 0, GSWF(3754))
    CALL(evt_map_set_tevcallback, 2, PTR(sun_tev))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("sun"), 33554432)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("sun"), 256)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("sun"), 268435456)
    BEGIN_INLINE_EVENT
        CALL(test_glare)
    END_INLINE_EVENT
    CALL(evt_cloud_init)
    IF_INT_EQUAL(GSWF(3729), 0)
        CALL(evt_cloud_ent, 0, STRING("kami_0"), STRING("a_kami_1"), PTR(evt_poster_1))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kami_0"), 1)
        CALL(evt_hitobj_onoff, STRING("a_kami_1"), 1, 0)
    END_IF
    IF_INT_EQUAL(GSWF(3730), 0)
        CALL(evt_cloud_ent, 0, STRING("kami_17"), STRING("a_kami_2"), PTR(evt_poster_2))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kami_17"), 1)
        CALL(evt_hitobj_onoff, STRING("a_kami_2"), 1, 0)
    END_IF
    IF_INT_EQUAL(GSWF(3731), 0)
        CALL(evt_cloud_ent, 0, STRING("kami_16"), STRING("a_kami_3"), PTR(evt_poster_3))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kami_16"), 1)
        CALL(evt_hitobj_onoff, STRING("a_kami_3"), 1, 0)
    END_IF
    IF_INT_EQUAL(GSWF(3732), 0)
        CALL(evt_cloud_ent, 0, STRING("kami_4"), STRING("a_kami_4"), PTR(evt_poster_4))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kami_4"), 1)
        CALL(evt_hitobj_onoff, STRING("a_kami_4"), 1, 0)
    END_IF
    IF_INT_EQUAL(GSWF(3733), 0)
        CALL(evt_cloud_ent, 0, STRING("kami_5"), STRING("a_kami_12"), PTR(evt_poster_5))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kami_5"), 1)
        CALL(evt_hitobj_onoff, STRING("a_kami_12"), 1, 0)
    END_IF
    IF_INT_EQUAL(GSWF(3734), 0)
        CALL(evt_cloud_ent, 0, STRING("kami_6"), STRING("a_kami_11"), PTR(evt_poster_6))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kami_6"), 1)
        CALL(evt_hitobj_onoff, STRING("a_kami_11"), 1, 0)
    END_IF
    IF_INT_EQUAL(GSWF(3735), 0)
        CALL(evt_cloud_ent, 0, STRING("kami_7"), STRING("a_kami_11"), PTR(evt_poster_7))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kami_7"), 1)
        CALL(evt_hitobj_onoff, STRING("a_kami_11"), 1, 0)
    END_IF
    IF_INT_EQUAL(GSWF(3736), 0)
        CALL(evt_cloud_ent, 0, STRING("kami_8"), STRING("a_kami_12"), PTR(evt_poster_8))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kami_8"), 1)
        CALL(evt_hitobj_onoff, STRING("a_kami_12"), 1, 0)
    END_IF
    IF_INT_EQUAL(GSWF(3737), 0)
        CALL(evt_cloud_ent, 0, STRING("kami_9"), STRING("a_kami_12"), PTR(evt_poster_9))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kami_9"), 1)
        CALL(evt_hitobj_onoff, STRING("a_kami_12"), 1, 0)
    END_IF
    IF_INT_EQUAL(GSWF(3738), 0)
        CALL(evt_cloud_ent, 0, STRING("kami_10"), STRING("a_kami_10"), PTR(evt_poster_10))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kami_10"), 1)
        CALL(evt_hitobj_onoff, STRING("a_kami_10"), 1, 0)
        CALL(evt_hitobj_onoff, STRING("a_kami_13"), 1, 0)
    END_IF
    IF_INT_EQUAL(GSWF(3739), 0)
        CALL(evt_cloud_ent, 0, STRING("kami_11"), STRING("a_kami_10"), PTR(evt_poster_11))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kami_11"), 1)
        CALL(evt_hitobj_onoff, STRING("a_kami_10"), 1, 0)
        CALL(evt_hitobj_onoff, STRING("a_kami_14"), 1, 0)
        CALL(evt_hitobj_onoff, STRING("a_kami_15"), 1, 0)
        CALL(evt_hitobj_onoff, STRING("a_kami_6"), 1, 0)
        CALL(evt_hitobj_onoff, STRING("a_kami_7"), 1, 0)
    END_IF
    IF_INT_EQUAL(GSWF(3740), 0)
        CALL(evt_cloud_ent, 0, STRING("kami_12"), STRING("a_kami_10"), PTR(evt_poster_12))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kami_12"), 1)
        CALL(evt_hitobj_onoff, STRING("a_kami_10"), 1, 0)
        CALL(evt_hitobj_onoff, STRING("a_kami_8"), 1, 0)
        CALL(evt_hitobj_onoff, STRING("a_kami_9"), 1, 0)
        CALL(evt_hitobj_onoff, STRING("a_kami_5"), 1, 0)
    END_IF
    IF_INT_EQUAL(GSWF(3741), 0)
        CALL(evt_cloud_ent, 0, STRING("kami_13"), STRING("a_kami_10"), PTR(evt_poster_13))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kami_13"), 1)
        CALL(evt_hitobj_onoff, STRING("a_kami_10"), 1, 0)
    END_IF
    IF_INT_EQUAL(GSWF(3742), 0)
        CALL(evt_cloud_ent, 0, STRING("kami_14"), STRING("a_kami_11"), PTR(evt_poster_14))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kami_14"), 1)
        CALL(evt_hitobj_onoff, STRING("a_kami_11"), 1, 0)
    END_IF
    IF_INT_EQUAL(GSWF(3743), 0)
        CALL(evt_cloud_ent, 0, STRING("kami_15"), STRING("a_kami_11"), PTR(evt_poster_15))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kami_15"), 1)
        CALL(evt_hitobj_onoff, STRING("a_kami_11"), 1, 0)
    END_IF
    CALL(evt_hitobj_onoff, STRING("a_kabe"), 1, 0)
    SET_VALUE(LW(0), 1)
    CALL_CHILD(bero_case_switch_off)
    CALL_CHILD(evt_chk)
    RETURN
EVT_END()

static const s32 texmtx_tbl[] = { 30, 33, 36, 39, 42, 45, 48, 51, 54, 57 };
const f32 float_31p416_eki_000091e0 = 31.41592f;
const f32 float_0p015625_eki_000091e4 = 0.015625f;
const f32 float_0p125_eki_000091e8 = 0.125f;
const f32 float_128_eki_000091ec = 128.0f;
const f32 float_3_eki_000091f0 = 3.0f;
const f32 zero_eki_000091f4 = 0.0f;
const f32 float_0p5_eki_000091f8 = 0.5f;

// clang-format on
