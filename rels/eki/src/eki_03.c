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

static BeroEntry bero_data[3] = {
    {"w_bero_1", 1, 3, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "eki_01", "e_bero", 5, 5, w1_close, w1_open},
    {"w_bero_2", 1, 3, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "eki_04", "e_bero", 5, 5, w2_close, w2_open},
    {NULL},
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

// The original `envGlareFilter` prototype takes `s16` y0/x0/y1/x1, so the caller sign-extends the four
// values as part of the call and MWCC groups the four `extsh` after the four sign adjustments. The shared
// `include/driver/envdrv.h` declares them `s32` (its `src/driver/envdrv.c` definition is `s32`), which
// folds each conversion into its expression and schedules the x0 `subi` before y0's. The cast restores
// the original call signature for this call only; the values fit in 16 bits, so the code is unchanged.
static USER_FUNC(test_glare) {
    MapObject* obj = mapGetMapObj("sun");

    marioGetPtr();
    if (isFirstCall) {
        obj->flags |= 0x20000000;
        return 0;
    }

    if (*(u16*)obj->field_0xEC == 0x3FF || *(u16*)(obj->field_0xEC + 2) == 0x3FF
        || *(u16*)(obj->field_0xEC + 4) == 0x3FF || *(u16*)(obj->field_0xEC + 6) == 0x3FF) {
        envGlareFilter(0, 0, 0, 0, 0);
    }
    else if (*(u16*)obj->field_0xEC == 0 && *(u16*)(obj->field_0xEC + 2) == 0 && *(u16*)(obj->field_0xEC + 4) == 0
             && *(u16*)(obj->field_0xEC + 6) == 0) {
        envGlareFilter(0, 0, 0, 0, 0);
    }
    else {
#ifdef MUST_MATCH
        // MUST_MATCH: the `s16` call-site signature is what fixes the schedule (see the comment above
        // the function); passing the same values through the header's `s32` prototype loses it.
        ((void (*)(s32, s16, s16, s16, s16))envGlareFilter)(
            1, *(u16*)(obj->field_0xEC + 2) - 0x32, *(u16*)obj->field_0xEC - 0x32,
            *(u16*)(obj->field_0xEC + 6) + 0x32, *(u16*)(obj->field_0xEC + 4) + 0x32);
#else
        envGlareFilter(1, (s16)(*(u16*)(obj->field_0xEC + 2) - 0x32), (s16)(*(u16*)obj->field_0xEC - 0x32),
                       (s16)(*(u16*)(obj->field_0xEC + 6) + 0x32), (s16)(*(u16*)(obj->field_0xEC + 4) + 0x32));
#endif
        envGlareFilterZ(1.148f);
    }
    return 0;
}

static void sun_tev(EnvironmentYami* yami) {
    s32 tevStage = yami->tevStage;
    s32 texMap = yami->texMap;
    s32 texCoord = yami->texCoord;
    s32 indStage = yami->unk18;
    u8* image;
    GXTexObj texObj;
    f32 indMtx[2][3];
    s32 i, j, row;

    if (ind_lp == NULL) {
        ind_lp = smartAlloc(GXGetTexBufferSize(0x40, 0x40, GX_TF_IA8, GX_FALSE, GX_FALSE), 1);
    }

    image = (u8*)ind_lp->address;
    for (i = 0; i < 0x40; i++) {
#ifdef MUST_MATCH
        // MUST_MATCH: reusing `q` as the row base (instead of `row = (i / 4) * 32;`) makes MWCC schedule the
        // `(i / 4) * 32` slwi after the `i % 4` add and puts row/gp/0x4330/i%4 in r31/r30/r29/r28; the
        // natural one-bank form rotates them to r29/r31/r30/r28.
        s32 q = i / 4;
        q = q * 32;
        row = q;
#else
        row = (i / 4) * 32;
#endif
        for (j = 0; j < 0x40; j++) {
            f32 s = (f32)sin(31.41592f * (f32)j / 64.0f - (f32)gp->retraceCount / 8.0f);
            s32 off = row + (j / 4) * 512;
            off += ((i % 4) + (j % 4) * 4) * 2;
            image[off] = (s32)(3.0f * s + 128.0f);
            image[off + 1] = 0x80;
        }
    }

    DCFlushRange(image, 0x2000);
    GXInitTexObj(&texObj, image, 0x40, 0x40, GX_TF_IA8, GX_REPEAT, GX_REPEAT, GX_FALSE);
    GXInitTexObjLOD(&texObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&texObj, texMap);

    indMtx[0][0] = 0.5f;
    indMtx[0][1] = 0.0f;
    indMtx[0][2] = 0.0f;
    indMtx[1][0] = 0.0f;
    indMtx[1][1] = 0.5f;
    indMtx[1][2] = 0.0f;
    GXSetIndTexMtx(GX_ITM_0, indMtx, 1);
    GXSetIndTexOrder(indStage, GX_TEXCOORD0, texMap);
    GXSetIndTexCoordScale(indStage, GX_ITS_1, GX_ITS_1);
    GXSetTevIndWarp(GX_TEVSTAGE0, indStage, 1, 0, GX_ITM_0);

    yami->tevStage = tevStage;
    yami->texCoord = texCoord;
    yami->unk18 = indStage + 1;
    yami->texMap = texMap + 1;
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

// clang-format on
