#include "mri_puni.h"
#include "mri.h"
#include <string.h>
#include "memory.h"
#include "driver/camdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_npc.h"
#include "evt/evt_seq.h"
#include "evt/evt_sub.h"
#include "forcedata.h"

#pragma pool_data off

#pragma force_active on
extern void* Punimap;
extern PuniTexInfo* Punitex;
extern PuniWork* Punigp;
extern PuniData* Punidat;
static void randf(void);  // TODO: real signature

// clang-format off
extend_data(76,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(77,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

u16 puni_nom_patern_data[] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,    21, 22, 23, 24,
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 0,  1,  2,  3,  4,  5,     6,  7,  8,  9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,    31, 32, 33, 34,
    35, 36, 37, 38, 39, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,    16, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 51, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 65535,
};

u16 puni_wlk_patern_data[] = {
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 65535,
};

u16 puni_run_patern_data[] = { 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 65535 };

u16 puni_dmg_patern_data[] = { 72, 65535 };

u16 puni_dwn_patern_data[] = { 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 65535 };

u16 puni_rol_patern_data[] = { 83, 65535 };

u16 puni_hag_patern_data[] = { 0, 65535 };

u16 puni_jpu_patern_data[] = { 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 65534 };

u16 puni_jpd_patern_data[] = { 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 65534 };

u16 puni_wan_patern_data[] = { 104, 105, 106, 107, 108, 109, 110, 111, 65534 };

u16* puni_patern_table[] = { puni_nom_patern_data, puni_wlk_patern_data, puni_run_patern_data, puni_dmg_patern_data,
                             puni_dwn_patern_data, puni_rol_patern_data, puni_hag_patern_data, puni_jpu_patern_data,
                             puni_jpd_patern_data, puni_wan_patern_data };

u16 toge_nom_patern_data[25] = {
    84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 65535,
};

u16 toge_wlk_patern_data[] = { 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 65535 };

u16 toge_run_patern_data[] = { 108, 110, 112, 114, 116, 118, 65535 };

u16 toge_dmg_patern_data[] = { 120, 65535 };

u16 toge_dwn_patern_data[] = { 121, 122, 123, 124, 125, 126, 127, 128, 65535 };

u16 toge_rol_patern_data[] = { 129, 65535 };

u16 toge_hag_patern_data[] = { 65535 };

u16* toge_patern_table[] = { toge_nom_patern_data, toge_wlk_patern_data, toge_run_patern_data, toge_dmg_patern_data,
                             toge_dwn_patern_data, toge_rol_patern_data, toge_hag_patern_data, toge_hag_patern_data,
                             toge_hag_patern_data, toge_hag_patern_data };


// TODO: type
static u32 jumptable_00023FA0[7] = {
    (u32)((u8*)&puniSetYPosition + 0x280), (u32)((u8*)&puniSetYPosition + 0x50), (u32)((u8*)&puniSetYPosition + 0xcc), (u32)((u8*)&puniSetYPosition + 0x154),
    (u32)((u8*)&puniSetYPosition + 0x1e4), (u32)((u8*)&puniSetYPosition + 0x1ec), (u32)((u8*)&puniSetYPosition + 0x1f4),
};

// TODO: type
u32 puniMapdata_MRI_06[33] = {
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000,
    0x0000FFFF, 0x00000000, 0x0000FFFF, 0x00000000,
    0x0000FFFF, 0xFFFF0000, 0xFFFF0000, 0xFFFF0000,
    0xFFFF0000, 0xFFFF0000, 0xFFFF0000, 0xFFFFFFFF,
    0x0000FFFF, 0x00000000, 0x0000FFFF, 0x00000000,
    0x0000FFFF, 0x0000FFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFF0000,
};

// TODO: type
u32 puniMapdata_MRI_09[21] = {
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0x20002000, 0x20002000, 0x2000FFFF, 0xFFFF2000,
    0x00000000, 0x00002000, 0xFFFFFFFF, 0x20000000,
    0x00000000, 0x2000FFFF, 0xFFFF2000, 0x00000000,
    0x00001000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF,
};

// TODO: type
u32 puniMapdata_MRI_09_01[21] = {
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0x20002000, 0x00002000, 0x2000FFFF, 0xFFFF2000,
    0x00000000, 0x00002000, 0xFFFFFFFF, 0x20000000,
    0x00000000, 0x2000FFFF, 0xFFFF2000, 0x00000000,
    0x00001000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF,
};

// TODO: type
u32 puniMapdata_MRI_16[50] = {
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0x2000FFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0x2000FFFF, 0xFFFF2000,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF2000,
    0xFFFFFFFF, 0x20000300, 0x03000300, 0x03000300,
    0x03000300, 0x2000FFFF, 0xFFFF2000, 0x10001000,
    0x10001000, 0x10001000, 0x10002000, 0xFFFFFFFF,
    0x20000100, 0x01000100, 0x01000100, 0x01000100,
    0x2000FFFF, 0xFFFF2000, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFF2000, 0xFFFFFFFF, 0x2000FFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x2000FFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFF0000,
};


// TODO: type
static u32 jumptable_000241B0[12] = {
    (u32)((u8*)&puniMapdata_MRI_06_getpos + 0xd0), (u32)((u8*)&puniMapdata_MRI_06_getpos + 0x20), (u32)((u8*)&puniMapdata_MRI_06_getpos + 0x30), (u32)((u8*)&puniMapdata_MRI_06_getpos + 0x40),
    (u32)((u8*)&puniMapdata_MRI_06_getpos + 0x50), (u32)((u8*)&puniMapdata_MRI_06_getpos + 0x60), (u32)((u8*)&puniMapdata_MRI_06_getpos + 0x70), (u32)((u8*)&puniMapdata_MRI_06_getpos + 0x80),
    (u32)((u8*)&puniMapdata_MRI_06_getpos + 0x90), (u32)((u8*)&puniMapdata_MRI_06_getpos + 0xa0), (u32)((u8*)&puniMapdata_MRI_06_getpos + 0xb0), (u32)((u8*)&puniMapdata_MRI_06_getpos + 0xc0),
};


// TODO: type
static u32 jumptable_000241E0[8] = {
    (u32)((u8*)&puniMapdata_MRI_16_getpos + 0x14c), (u32)((u8*)&puniMapdata_MRI_16_getpos + 0xdc), (u32)((u8*)&puniMapdata_MRI_16_getpos + 0xec), (u32)((u8*)&puniMapdata_MRI_16_getpos + 0xfc),
    (u32)((u8*)&puniMapdata_MRI_16_getpos + 0x10c), (u32)((u8*)&puniMapdata_MRI_16_getpos + 0x11c), (u32)((u8*)&puniMapdata_MRI_16_getpos + 0x12c), (u32)((u8*)&puniMapdata_MRI_16_getpos + 0x13c),
};

// TODO: type
static u32 jumptable_00024200[10] = {
    (u32)((u8*)&puniMapdata_MRI_16_getpos + 0xb0), (u32)((u8*)&puniMapdata_MRI_16_getpos + 0x20), (u32)((u8*)&puniMapdata_MRI_16_getpos + 0x30), (u32)((u8*)&puniMapdata_MRI_16_getpos + 0x40),
    (u32)((u8*)&puniMapdata_MRI_16_getpos + 0x50), (u32)((u8*)&puniMapdata_MRI_16_getpos + 0x60), (u32)((u8*)&puniMapdata_MRI_16_getpos + 0x70), (u32)((u8*)&puniMapdata_MRI_16_getpos + 0x80),
    (u32)((u8*)&puniMapdata_MRI_16_getpos + 0x90), (u32)((u8*)&puniMapdata_MRI_16_getpos + 0xa0),
};


// TODO: type
static u32 jumptable_00024228[11] = {
    (u32)((u8*)&puniModeWind + 0x40), (u32)((u8*)&puniModeWind + 0x1bc), (u32)((u8*)&puniModeWind + 0x318), (u32)((u8*)&puniModeWind + 0x440),
    (u32)((u8*)&puniModeWind + 0x470), (u32)((u8*)&puniModeWind + 0x6bc), (u32)((u8*)&puniModeWind + 0x6bc), (u32)((u8*)&puniModeWind + 0x6bc),
    (u32)((u8*)&puniModeWind + 0x6bc), (u32)((u8*)&puniModeWind + 0x6bc), (u32)((u8*)&puniModeWind + 0x574),
};


// TODO: type
static u32 jumptable_00024254[35] = {
    (u32)((u8*)&puniModeHagure + 0x38), (u32)((u8*)&puniModeHagure + 0x88), (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218),
    (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218),
    (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0xf4), (u32)((u8*)&puniModeHagure + 0x218),
    (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218),
    (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218),
    (u32)((u8*)&puniModeHagure + 0x15c), (u32)((u8*)&puniModeHagure + 0x16c), (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218),
    (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218),
    (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x18c), (u32)((u8*)&puniModeHagure + 0x1fc),
    0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
static u32 shadow_dl[8] = {
    0x80000400, 0x00010102, 0x02030300, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
static u32 shadow_vtx[16] = {
    0xBF99999A, 0x00000000, 0xBF99999A, 0xBF99999A,
    0x00000000, 0x3F99999A, 0x3F99999A, 0x00000000,
    0x3F99999A, 0x3F99999A, 0x00000000, 0xBF99999A,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
static u32 shadow_coord[8] = {
    0x00000000, 0x3F800000, 0x00000000, 0x00000000,
    0x3F800000, 0x00000000, 0x3F800000, 0x3F800000,
};

// TODO: shadow_mt$4593 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u32 shadow_mt$4593[12] = {
//     0x41200000, 0x00000000, 0x00000000, 0x00000000,
//     0x00000000, 0x41200000, 0x00000000, 0x3DCCCCCD,
//     0x00000000, 0x00000000, 0x41200000, 0x00000000,
// };


// TODO: type
static u32 jumptable_00024390[30] = {
    (u32)((u8*)&unk_mri_00012d58 + 0x40), (u32)((u8*)&unk_mri_00012d58 + 0xb4), (u32)((u8*)&unk_mri_00012d58 + 0x1b4), (u32)((u8*)&unk_mri_00012d58 + 0x1c8),
    (u32)((u8*)&unk_mri_00012d58 + 0x1dc), (u32)((u8*)&unk_mri_00012d58 + 0x1f0), (u32)((u8*)&unk_mri_00012d58 + 0x204), (u32)((u8*)&unk_mri_00012d58 + 0x218),
    (u32)((u8*)&unk_mri_00012d58 + 0x4a8), (u32)((u8*)&unk_mri_00012d58 + 0x480), (u32)((u8*)&unk_mri_00012d58 + 0x494), (u32)((u8*)&unk_mri_00012d58 + 0x4bc),
    (u32)((u8*)&unk_mri_00012d58 + 0x4d0), (u32)((u8*)&unk_mri_00012d58 + 0x53c), (u32)((u8*)&unk_mri_00012d58 + 0xde0), (u32)((u8*)&unk_mri_00012d58 + 0x550),
    (u32)((u8*)&unk_mri_00012d58 + 0x804), (u32)((u8*)&unk_mri_00012d58 + 0xa68), (u32)((u8*)&unk_mri_00012d58 + 0x818), (u32)((u8*)&unk_mri_00012d58 + 0x900),
    (u32)((u8*)&unk_mri_00012d58 + 0x914), (u32)((u8*)&unk_mri_00012d58 + 0xa40), (u32)((u8*)&unk_mri_00012d58 + 0xa54), (u32)((u8*)&unk_mri_00012d58 + 0xa7c),
    (u32)((u8*)&unk_mri_00012d58 + 0xb44), (u32)((u8*)&unk_mri_00012d58 + 0xb58), (u32)((u8*)&unk_mri_00012d58 + 0xb6c), (u32)((u8*)&unk_mri_00012d58 + 0xbd4),
    (u32)((u8*)&unk_mri_00012d58 + 0xb80), (u32)((u8*)&unk_mri_00012d58 + 0xdc0),
};


// TODO: type
static u32 jumptable_00024408[30] = {
    (u32)((u8*)&fn_19_13A90 + 0x40), (u32)((u8*)&fn_19_13A90 + 0x40), (u32)((u8*)&fn_19_13A90 + 0x40), (u32)((u8*)&fn_19_13A90 + 0x40),
    (u32)((u8*)&fn_19_13A90 + 0x364), (u32)((u8*)&fn_19_13A90 + 0x900), (u32)((u8*)&fn_19_13A90 + 0x900), (u32)((u8*)&fn_19_13A90 + 0x40),
    (u32)((u8*)&fn_19_13A90 + 0x40), (u32)((u8*)&fn_19_13A90 + 0x628), (u32)((u8*)&fn_19_13A90 + 0x40), (u32)((u8*)&fn_19_13A90 + 0xc2c),
    (u32)((u8*)&fn_19_13A90 + 0xc2c), (u32)((u8*)&fn_19_13A90 + 0x900), (u32)((u8*)&fn_19_13A90 + 0x40), (u32)((u8*)&fn_19_13A90 + 0xc2c),
    (u32)((u8*)&fn_19_13A90 + 0x40), (u32)((u8*)&fn_19_13A90 + 0xc2c), (u32)((u8*)&fn_19_13A90 + 0xc2c), (u32)((u8*)&fn_19_13A90 + 0xc2c),
    (u32)((u8*)&fn_19_13A90 + 0xc2c), (u32)((u8*)&fn_19_13A90 + 0xc2c), (u32)((u8*)&fn_19_13A90 + 0xc2c), (u32)((u8*)&fn_19_13A90 + 0x40),
    (u32)((u8*)&fn_19_13A90 + 0x90c), (u32)((u8*)&fn_19_13A90 + 0x40), (u32)((u8*)&fn_19_13A90 + 0x40), (u32)((u8*)&fn_19_13A90 + 0x40),
    (u32)((u8*)&fn_19_13A90 + 0xc2c), (u32)((u8*)&fn_19_13A90 + 0x8e0),
};


EVT_BEGIN(puni_piders_check)
    CALL(evt_seq_wait, 2)
    LOOP(0)
        CALL(puni_piders_check_func)
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()


EVT_BEGIN(puni_disp_info)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 118)
        RETURN
    END_IF
    LOOP(0)
        CALL(puni_disp_info_sub, LW(0), LW(1))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()


EVT_BEGIN(mri_puninum_fadeout)
    WAIT_MS(1200)
    CALL(evt_sub_intpl_msec_init, 11, 255, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_puni_puninum_alpha, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(mri_puninum_fadein)
    CALL(evt_sub_intpl_msec_init, 11, 0, 255, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_puni_puninum_alpha, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    RETURN
EVT_END()






EVT_BEGIN(mri_puni_position_init)
    CALL(evt_puni_set_systemmode, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_puni_puninum_alpha, 0)
        WAIT_FRAMES(2)
        CALL(evt_bero_get_now_number, LW(10))
        CALL(evt_bero_get_info_kinddir, LW(10), LW(0), LW(1), LW(2))
        CALL(evt_bero_get_info_nextarea, LW(10), LW(3), LW(4))
        SET_VALUE(LF(0), 0)
        IF_INT_EQUAL(LW(0), 2)
            IF_INT_NOT_EQUAL(LW(3), 0)
                SET_VALUE(LF(0), 1)
            END_IF
        END_IF
        IF_INT_EQUAL(LF(0), 0)
            CALL(evt_puni_puninum_alpha, 255)
        END_IF
    END_INLINE_EVENT
    DEBUG_PUT_MSG(STRING("プニ関係：初期の実行中待ち"))
    CALL(evt_bero_exec_wait, 1)
    LOOP(0)
        CALL(evt_puni_set_systemmode, 1)
        DEBUG_PUT_MSG(STRING("プニ関係：座標セット待ち"))
        CALL(evt_bero_exec_wait, 2)
        DEBUG_PUT_MSG(STRING("プニ関係：小動物関係初期化"))
        DEBUG_PUT_REG(GSW(665))
        DEBUG_PUT_REG(GSW(666))
        CALL(evt_bero_get_now_number, LW(10))
        CALL(evt_bero_get_info_kinddir, LW(10), LW(0), LW(1), LW(2))
        CALL(evt_bero_get_info_nextarea, LW(10), LW(3), LW(4))
        IF_INT_EQUAL(LW(0), 2)
            IF_INT_NOT_EQUAL(LW(3), 0)
                SPAWN_EVENT(mri_puninum_fadein)
            END_IF
        END_IF
        CALL(evt_puni_setup, LW(0), LW(1), LW(2))
        CALL(evt_toge_setup, LW(6), LW(7), LW(8), LW(9))
        CALL(evt_puni_set_systemmode, 2)
        DEBUG_PUT_MSG(STRING("プニ関係：終了待ち"))
        CALL(evt_bero_exec_wait, 65536)
        CALL(evt_puni_set_systemmode, 3)
        DEBUG_PUT_MSG(STRING("プニ関係：次の実行中待ち"))
        CALL(evt_bero_exec_wait, 1)
        SET_VALUE(GSW(666), GSW(665))
        CALL(evt_bero_get_now_number, LW(10))
        CALL(evt_bero_get_info_kinddir, LW(10), LW(0), LW(1), LW(2))
        CALL(evt_bero_get_info_nextarea, LW(10), LW(3), LW(4))
        IF_INT_EQUAL(LW(0), 1)
            DEBUG_PUT_MSG(STRING("プニ関係：扉待ち"))
            CALL(evt_bero_exec_wait, 268435456)
            DEBUG_PUT_MSG(STRING("プニ関係：扉待ち終わり"))
            CALL(evt_bero_exec_onoff, 1, 268435456)
        END_IF
        IF_INT_EQUAL(LW(0), 2)
            IF_INT_NOT_EQUAL(LW(3), 0)
                SPAWN_EVENT(mri_puninum_fadeout)
            END_IF
        END_IF
        BEGIN_INLINE_EVENT
            DEBUG_PUT_MSG(STRING("プニ関係：エリアチェンジ直前待ち"))
            CALL(evt_bero_exec_wait, 8)
            DEBUG_PUT_MSG(STRING("プニ関係：プニ族トゲ族SE強制オフ"))
            CALL(evt_puni_force_se_off)
        END_INLINE_EVENT
        CALL(evt_puni_exit, LW(0), LW(1), LW(2), LW(3))
    END_LOOP
    RETURN
EVT_END()


EVT_BEGIN(mri_puni_main)
    SET_TYPE(0)
    CALL(evt_puni_main)
    RETURN
EVT_END()

EVT_BEGIN(mri_puni_init_dvd)
    CALL(evt_npc_entry, STRING("プニsp"), STRING("c_puni"))
    CALL(evt_npc_set_tribe, STRING("プニsp"), STRING("プニ族"))
    CALL(evt_npc_entry, STRING("トゲsp"), STRING("c_toge"))
    CALL(evt_npc_set_tribe, STRING("トゲsp"), STRING("トゲ族"))
    CALL(evt_npc_entry, STRING("ガイドsp"), STRING("c_pguide"))
    CALL(evt_npc_set_tribe, STRING("ガイドsp"), STRING("ガイド"))
    CALL(evt_npc_entry, STRING("ガイド妹sp"), STRING("c_psister"))
    CALL(evt_npc_set_tribe, STRING("ガイド妹sp"), STRING("ガイド妹"))
    CALL(evt_npc_entry, STRING("ちびトゲsp"), STRING("c_tchibi"))
    CALL(evt_npc_set_tribe, STRING("ちびトゲsp"), STRING("ちびトゲ"))
    CALL(evt_npc_entry, STRING("ライバルsp"), STRING("c_prival"))
    CALL(evt_npc_set_tribe, STRING("ライバルsp"), STRING("ライバル"))
    CALL(evt_npc_set_position, STRING("プニsp"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("トゲsp"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("ガイドsp"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("ガイド妹sp"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("ちびトゲsp"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("ライバルsp"), 0, -1000, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("プニsp"), 1074266144)
    CALL(evt_npc_flag_onoff, 1, STRING("トゲsp"), 1074266144)
    CALL(evt_npc_flag_onoff, 1, STRING("ガイドsp"), 1074266144)
    CALL(evt_npc_flag_onoff, 1, STRING("ガイド妹sp"), 1074266144)
    CALL(evt_npc_flag_onoff, 1, STRING("ちびトゲsp"), 1074266144)
    CALL(evt_npc_flag_onoff, 1, STRING("ライバルsp"), 1074266144)
    RETURN
EVT_END()







EVT_BEGIN(mri_puni_init)
    CALL_CHILD(mri_puni_init_dvd)
    SWITCH(GSW(0))
        CASE_LESS(88)
        RETURN
        CASE_GREATER_OR_EQUAL(403)
        RETURN
        CASE_EQUAL(400)
        CALL(evt_sub_get_mapname, LW(0))
        IF_STR_NOT_EQUAL(LW(0), STRING("mri_00"))
            RETURN
        END_IF
        SET_VALUE(LF(0), 0)
        CALL(evt_bero_get_entername, LW(0))
        IF_STR_EQUAL(LW(0), STRING("sekai_yami1"))
            SET_VALUE(LF(0), 1)
        END_IF
        IF_STR_EQUAL(LW(0), STRING("minnnanokoe"))
            SET_VALUE(LF(0), 1)
        END_IF
        IF_INT_EQUAL(LF(0), 0)
            RETURN
        END_IF
        CASE_BETWEEN(118, 399)
        CALL(evt_sub_get_mapname, LW(0))
        IF_STR_NOT_EQUAL(LW(0), STRING("mri_00"))
            RETURN
        END_IF
        IF_INT_EQUAL(GSWF(2884), 1)
            RETURN
        END_IF
        CASE_DEFAULT
    END_SWITCH
    CALL(evt_make_texbuf)
    CALL(evt_puni_init, 210, PTR(puni_puni_init), PTR(puni_puni_main), PTR(puni_puni_dispent))
    CALL(evt_puni_set_maze)
    SPAWN_EVENT(mri_puni_position_init)
    SPAWN_EVENT(mri_puni_main)
    RETURN
EVT_END()

EVT_BEGIN(mri_puni_init_none)
    CALL_CHILD(mri_puni_init_dvd)
    CALL(evt_make_texbuf)
    CALL(evt_puni_init, 210, PTR(puni_puni_init), PTR(puni_puni_main), PTR(puni_puni_dispent))
    CALL(evt_puni_set_maze)
    RETURN
EVT_END()

// TODO: h3$2957 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u8 h3$2957[2] = {
//     0x00, 0x00,
// };

// TODO: h2$2956 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u8 h2$2956[2] = {
//     0x00, 0x00,
// };

// TODO: h1$2955 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u8 h1$2955[2] = {
//     0x00, 0x00,
// };

// TODO: h0$2954 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u8 h0$2954[2] = {
//     0x00, 0x00,
// };

// TODO: m3$2953 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u8 m3$2953[2] = {
//     0x00, 0x00,
// };

// TODO: m2$2952 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u8 m2$2952[2] = {
//     0x00, 0x00,
// };

// TODO: m1$2951 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u8 m1$2951[2] = {
//     0x00, 0x00,
// };

// TODO: m0$2950 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u8 m0$2950[2] = {
//     0x00, 0x00,
// };

// TODO: find$2949 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u8 find$2949[2] = {
//     0x00, 0x00,
// };

// TODO: move$2948 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u8 move$2948[2] = {
//     0x00, 0x00,
// };

// TODO: takasa$2947 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u8 takasa$2947[2] = {
//     0x00, 0x00,
// };

// TODO: debug$2303 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u32 debug$2303[1] = {
//     0x00000000,
// };

void* Punimap;

PuniTexInfo* Punitex;

PuniWork* Punigp;

PuniData* Punidat;

PuniData* puniGetPunidat(void) {
    return Punidat;
}

PuniWork* puniGetPunigp(void) {
    return Punigp;
}

PuniTexInfo* puniGetTex(void) {
    return Punitex;
}

// TODO: write evt_icon_disp_func (global, 0x1c8 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

PuniTexInfo* _make_texbuf(PuniTexInfo* texbuf, s32 count, const char* a3, const char* a4, f32 a5) {
    s32 i;

    for (i = 0; i < count; i++) {
        texbuf->unk0 = _mapAlloc(0xC);
        texbuf->unk4 = _mapAlloc(0xC);
        strcpy(texbuf->unk0, a3);
        strcpy(texbuf->unk4, a4);
        texbuf->unk8 = a5;
        a5 += 1.0f;
        texbuf++;
    }
    return texbuf;
}

USER_FUNC(evt_make_texbuf) {
    PuniTexInfo* texbuf = Punitex;

    texbuf = Punitex;
    texbuf = _make_texbuf(texbuf, 40, "c_puni", "S_1", 0.0f);
    texbuf = _make_texbuf(texbuf, 12, "c_puni", "S_1", 97.0f);
    texbuf = _make_texbuf(texbuf, 20, "c_puni", "W_1", 0.0f);
    texbuf = _make_texbuf(texbuf, 1, "c_puni", "D_1", 0.0f);
    texbuf = _make_texbuf(texbuf, 10, "c_puni", "D_2", 0.0f);
    texbuf = _make_texbuf(texbuf, 1, "c_puni", "F_1", 0.0f);

    if (Punigp->unk328 == 9 || Punigp->unk328 == 12) {
        texbuf = _make_texbuf(texbuf, 24, "c_toge", "S_1", 0.0f);
        texbuf = _make_texbuf(texbuf, 12, "c_toge", "W_1", 0.0f);
        texbuf = _make_texbuf(texbuf, 1, "c_toge", "D_1", 0.0f);
        texbuf = _make_texbuf(texbuf, 8, "c_toge", "D_2", 0.0f);
        texbuf = _make_texbuf(texbuf, 1, "c_toge", "F_1", 0.0f);
    } else {
        texbuf = _make_texbuf(texbuf, 10, "c_puni", "J_1A", 0.0f);
        texbuf = _make_texbuf(texbuf, 10, "c_puni", "J_1B", 0.0f);
        texbuf = _make_texbuf(texbuf, 8, "c_puni", "B_1", 0.0f);
    }
    texbuf->unk0 = 0;
    texbuf->unk4 = 0;
    texbuf->unk8 = 0.0f;
    return EVT_RETURN_DONE;
}

// TODO: write puniAnimCount (global, 0x40 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniAnimationInit (global, 0xdc bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniAnimationInit2 (global, 0x2d4 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniAnimation (global, 0x464 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write _chukanhosei (global, 0x4c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write _areacheck (global, 0x48 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write _ymode_mri_09 (global, 0x450 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write _ymode_mri_16_00 (global, 0x4cc bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write _ymode_mri_16_01 (global, 0x51c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniSetYPosition (global, 0x2dc bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniMapdata_MRI_06_getidx (global, 0x214 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniMapdata_MRI_06_getpos (global, 0x140 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniMapdata_MRI_09_getidx (global, 0x174 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniMapdata_MRI_09_getpos (global, 0x100 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniMapdata_MRI_16_getidx (global, 0x254 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniMapdata_MRI_16_getpos (global, 0x15c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniMapPos2Blk (global, 0x68 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniMapBlk2Pos (global, 0x160 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniMapGetInfo (global, 0x38 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniMapInit (global, 0x3c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniMapDataSet (global, 0xc4 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniMapFindbuff (global, 0x2dc bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniTargetSearch (global, 0xe8 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniswGetindex (global, 0x13c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniswSetindex (global, 0xd8 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniswTrapGetindex (global, 0x70 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniswTrapSetindex (global, 0x7c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write randf (static, 0x7c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write randi (global, 0x48 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write qrndseed (global, 0x2c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write qrnd (global, 0x64 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniDeleteEffect (global, 0x40 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write _daime_change (global, 0x3b0 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write PuniDokuritsu (global, 0x158 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write PuniSakanobori (global, 0x18 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write PuniChildHagure (global, 0x230 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write searchParent (global, 0xb0 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write searchMario (global, 0x1c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniPositionCheck (global, 0x144 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniPositionRecover (global, 0x130 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniMoveAtari (global, 0x270 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_mri_000095f0 (global, 0x21c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_mri_0000980c (global, 0x258 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniMoveDecel (global, 0x350 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniMoveLimitedNormal (global, 0x2f0 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniMoveNormal (global, 0x2e4 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniMoveSimple (global, 0x50 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniMoveCustom (global, 0x30 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniParaAttack (global, 0x200 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniParaDamage (global, 0x40 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniParaRecover (global, 0xb0 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniParaDangerCheck (global, 0x28 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniParaSelfCheck (global, 0x28 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniParaRecoverCheck (global, 0x1c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniSearchRandom (global, 0xc8 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniSearchRandom_Mikata (global, 0xb8 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write fn_19_A820 (global, 0x4 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniSearchRandom_Teki (global, 0xb8 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write fn_19_A8DC (global, 0x4 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write searchBattle (global, 0xe0 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write searchMikata (global, 0xc4 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write searchTeki (global, 0xc8 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write searchBattleMikata (global, 0xc4 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write searchBattleTeki (global, 0xc8 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniSearch (global, 0x190 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeChangeAttack (global, 0x204 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeChangeAway (global, 0xd0 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeChangeFallCheck (global, 0x150 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniFukki (global, 0x308 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeALLCheck (global, 0x2d8 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeALLEndCheck (global, 0x50 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeNormal (global, 0x8c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeParentpre (global, 0x114 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeParent (global, 0x424 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeSearch (global, 0x1e88 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeMass (global, 0x40c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeAwayjump (global, 0x1b0 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeAway_sub (global, 0x1b8 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeAway (global, 0x28c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeAttack (global, 0x290 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeDamage (global, 0x268 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModePiyori (global, 0x1a8 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeEscape (global, 0x300 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeRecover (global, 0x500 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeDead (global, 0x84 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeWind_sub (global, 0x228 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeWind_kaiten (global, 0xa4 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeWind (global, 0x6d0 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeFall (global, 0x528 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModePreupdraft (global, 0x100 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeUpdraft (global, 0x4d8 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeWonder (global, 0x144 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeFunbari (global, 0x23c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeWater (global, 0x270 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModePrehagure (global, 0x2cc bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeHagure (global, 0x35c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeDodai (global, 0xe0 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeTrap_sub (global, 0x13c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeTrap (global, 0x3f8 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeEnterSubInit (global, 0x40 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeEnterSub (global, 0x64 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeEnter (global, 0x3ac bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeExitSubInit (global, 0x34 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeExitSub (global, 0x64 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeExit (global, 0x28c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModePoint (global, 0x204 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeDokan (global, 0x48 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeCustom (global, 0x30 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puni_puni_disp (global, 0xd0 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puni_puni_shadow_disp (global, 0x248 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puni_puni_init (global, 0x298 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_mri_00012d58 (global, 0xdfc bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write fn_19_13A90 (global, 0xc48 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniOption (global, 0x338 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_mri_00014ad4 (global, 0x314 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniSound (global, 0x500 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puni_puni_main (global, 0x6bc bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puni_puni_dispent (global, 0x100 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

USER_FUNC(evt_puni_mem_init) {
    Punigp = _mapAlloc(sizeof(PuniWork));
    memset(Punigp, 0, sizeof(PuniWork));
    Punidat = _mapAlloc(sizeof(PuniData) * PUNI_DATA_MAX);
    memset(Punidat, 0, sizeof(PuniData) * PUNI_DATA_MAX);
    Punitex = _mapAlloc(sizeof(PuniTexInfo) * PUNI_TEX_INFO_MAX);
    memset(Punitex, 0, sizeof(PuniTexInfo) * PUNI_TEX_INFO_MAX);
    return EVT_RETURN_DONE;
}

// TODO: write evt_puni_init (global, 0x184 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_set_yuka (global, 0x194 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_set_hamari (global, 0x1ec bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_set_maze (global, 0x394 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_main (global, 0x24 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puni_init_posy (global, 0x98 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puni_init_para (global, 0xf0 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write toge_init_para (global, 0xc0 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_rnd_pos (global, 0x2f8 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_get_systemmode (global, 0x38 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_set_systemmode (global, 0x3c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_wait_systemmode (global, 0x4c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_debug_info (global, 0x30c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_setup (global, 0xa10 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_toge_setup (global, 0x324 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write _puniVAtari (global, 0x114 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniHagurePos (global, 0x568 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_exit (global, 0x740 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_init_puni (global, 0x30c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_init_teki (global, 0x170 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_get_position (global, 0x9c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_set_position (global, 0xf0 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_get_dir (global, 0x64 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_set_dir (global, 0x78 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_get_anim (global, 0x58 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_set_anim (global, 0x60 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_korokoro (global, 0x124 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_move_position (global, 0x1e8 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_move_position2 (global, 0x258 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_jump_position (global, 0x28c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_get_mode (global, 0x58 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_set_mode (global, 0x8c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_setsudan (global, 0x70 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_get_flag (global, 0x58 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_flag_onoff (global, 0x88 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_dokan_onoff (global, 0x90 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_get_lsw_index (global, 0x1b4 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_set_lsw_index (global, 0x1a0 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_get_trap_index (global, 0x84 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_set_trap_index (global, 0x9c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_set_ymode (global, 0x3c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_all_fall_check (global, 0x154 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_all_movestop (global, 0x78 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_count (global, 0xa8 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_count_area (global, 0x1a8 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_zenmetsu_check (global, 0xc8 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_landon_check (global, 0xdc bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_pera_onoff (global, 0x7c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_fade_onoff (global, 0x30 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_attack_on (global, 0x1c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_puninum_onoff (global, 0x3c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_puninum_alpha (global, 0x3c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_winddisable_onoff (global, 0x3c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_force_se_off (global, 0x58 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_all_save (global, 0x1ec bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_target_flag_onoff (global, 0x90 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_target_set_position (global, 0xdc bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_target_set_soko (global, 0x60 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write npcGetTribeName (global, 0x8 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puni_piders_check_func (global, 0x1f8 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puni_disp_info_kassencheck (global, 0x54 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it

// TODO: write puniCountDisp (global, 0x22c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it
// clang-format on

// TODO: write puni_disp_info_sub (global, 0x25c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_puni.s; this is where the original has it, just before the first script that uses it
