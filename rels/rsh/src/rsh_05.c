#include "rsh_05.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_ext.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "driver/dispdrv.h"
#include "driver/extdrv.h"
#include "driver/hitdrv.h"
#include "driver/mapdrv.h"
#include "driver/npcdrv.h"
#include "driver/offscreendrv.h"
#include "mario/mariost.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <string.h>
#include <stdlib.h>

#pragma pool_data off

// Moamoa EXTask work entry, stride 0x50.
typedef struct MoamoaPose {
    u16 flags;          // 0x0
    u16 field_0x2;      // 0x2
    u16 field_0x4;      // 0x4
    u8 pad_0x6[0x1E];   // 0x6
    u16 field_0x24;     // 0x24
    u8 pad_0x26[0x16];  // 0x26
    s16 field_0x3C;     // 0x3C
    u8 pad_0x3E[2];     // 0x3E
    OSTime time;        // 0x40
    u8 pad_0x48[4];     // 0x48
    u8 field_0x4C;      // 0x4C
    u8 pad_0x4D[3];     // 0x4D
} MoamoaPose;

// Moamoa group table entry (ext_moamoa_data), stride 0xC.
typedef struct MoamoaGroupDef {
    char* name;     // 0x0
    char* name2;    // 0x4
    f32 field_0x8;  // 0x8
} MoamoaGroupDef;

// Moamoa name-group entry, stride 0x28, starting at MoamoaWork+0x50.
typedef struct MoamoaGroup {
    char name[16];      // 0x0
    char name2[8];      // 0x10
    u16 field_0x18;     // 0x18
    u8 pad_0x1A[2];     // 0x1A
    f32 field_0x1C;     // 0x1C
    s32 field_0x20;     // 0x20
    s32 field_0x24;     // 0x24
} MoamoaGroup;

// Moamoa EXTask work header (allocated as 0x870 bytes by ext_moamoa_init).
typedef struct MoamoaWork {
    u16 flags;          // 0x0
    u16 field_0x2;      // 0x2
    s32 poseNum;        // 0x4
    MoamoaPose* poses;  // 0x8
    void* field_0xC;    // 0xC
    f32 field_0x10;     // 0x10
    Vec pos[3];         // 0x14
    u16 field_0x38;     // 0x38
    u16 field_0x3A[2];  // 0x3A
    u16 field_0x3E;     // 0x3E
    u16 field_0x40;     // 0x40
    u16 field_0x42;     // 0x42
    s32 field_0x44;     // 0x44
    s32 field_0x48;     // 0x48
    s32 groupCount;     // 0x4C
    MoamoaGroup groups[1]; // 0x50
} MoamoaWork;

#pragma force_active on
// Constant spawn offset used by ext_moamoa_init (moamoa spawn vector).
static const Vec vec3_rsh_000136d4 = {-290.0f, 65.0f, -95.0f};
static USER_FUNC(npc_jump);
static void ext_moamoa_dispent(void);
static void ext_moamoa_main(void);
static void moamoa_hit_check(void);
static void moamoa_jump(void);
static void ext_shadow_disp(s32 camId);
static void ext_disp(s32 camId);
static void ext_moamoa_init(void);
static f32 randf(f32 min, f32 max);
static USER_FUNC(evt_moamoa_target_off);
static USER_FUNC(evt_moamoa_fusion_debug);
static USER_FUNC(evt_moamoa_blow);
static USER_FUNC(evt_moamoa_kind_onoff);
static USER_FUNC(evt_moamoa_fusion);
static USER_FUNC(evt_moamoa_end_wait);
static USER_FUNC(evt_moamoa_ry_reset);
static USER_FUNC(evt_moamoa_exit);
static USER_FUNC(evt_moamoa_flag_onoff);
static void moamoa_pose_change(MoamoaPose* pose, char* name1, char* name2);
static void moamoa_pose_main(void);  // TODO: real signature
static void moamoa_pose_init(void);  // TODO: real signature
static MoamoaWork* wp;
static s32 talk_syashou[19];
static s32 talk_waitless[19];

// clang-format off
extend_data(24,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(25,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// Moamoa group definitions, one row per distinctive group (stride 0xC).
static MoamoaGroupDef ext_moamoa_data[] = {
    {"c_n_moa_a", "S_1", 0.0f},
    {"c_n_moa_a", "S_1", 2.0f},
    {"c_n_moa_a", "S_1", 4.0f},
    {"c_n_moa_a", "S_1", 6.0f},
    {"c_n_moa_a", "S_1", 8.0f},
    {"c_n_moa_a", "S_1", 10.0f},
    {"c_n_moa_a", "S_1", 12.0f},
    {"c_n_moa_a", "S_1", 14.0f},
    {"c_n_moa_a", "S_1", 16.0f},
    {"c_n_moa_a", "S_1", 18.0f},
    {"c_n_moa_a", "S_1", 20.0f},
    {"c_n_moa_a", "S_1", 22.0f},
    {"c_n_moa_a", "S_1", 24.0f},
    {"c_n_moa_a", "S_1", 26.0f},
    {"c_n_moa_a", "S_1", 28.0f},
    {"c_n_moa_a", "S_1", 30.0f},
    {"c_n_moa_a", "S_1", 32.0f},
    {"c_n_moa_a", "S_1", 34.0f},
    {"c_n_moa_a", "S_1", 36.0f},
    {"c_n_moa_a", "S_1", 38.0f},
    {"c_n_moa_a", "D_1", 0.0f},
    {"c_n_moa_a", "D_1", 4.0f},
    {"c_n_moa_a", "D_1", 8.0f},
    {"c_n_moa_b", "S_1", 0.0f},
    {"c_n_moa_b", "S_1", 2.0f},
    {"c_n_moa_b", "S_1", 4.0f},
    {"c_n_moa_b", "S_1", 6.0f},
    {"c_n_moa_b", "S_1", 8.0f},
    {"c_n_moa_b", "S_1", 10.0f},
    {"c_n_moa_b", "S_1", 12.0f},
    {"c_n_moa_b", "S_1", 14.0f},
    {"c_n_moa_b", "S_1", 16.0f},
    {"c_n_moa_b", "S_1", 18.0f},
    {"c_n_moa_b", "S_1", 20.0f},
    {"c_n_moa_b", "S_1", 22.0f},
    {"c_n_moa_b", "S_1", 24.0f},
    {"c_n_moa_b", "S_1", 26.0f},
    {"c_n_moa_b", "S_1", 28.0f},
    {"c_n_moa_b", "S_1", 30.0f},
    {"c_n_moa_b", "S_1", 32.0f},
    {"c_n_moa_b", "S_1", 34.0f},
    {"c_n_moa_b", "S_1", 36.0f},
    {"c_n_moa_b", "S_1", 38.0f},
    {"c_n_moa_b", "D_1", 0.0f},
    {"c_n_moa_b", "D_1", 4.0f},
    {"c_n_moa_b", "D_1", 8.0f},
    {"00000000", "00000000", 0.0f},
};

// data for the moamoa group lookup table
static char* group_tbl[11][2] = {
    {"c_n_moa_a", "S_1"},
    {"c_n_moa_b", "S_1"},
    {"c_kinodebu", "G_1"},
    {"c_puku_c", "U_1"},
    {"c_powan", "O_1"},
    {"c_smoko", "T_2"},
    {"c_rose", "G_1"},
    {"c_papab", "G_1"},
    {"c_mamab", "G_1"},
    {"c_konari", "G_1"},
    {"c_n_moa_a", "S_1"},
};
static void ext_moamoa_dispent(void) {
    if (wp->flags & 1) {
        dispEntry(4, 0.0f, 1, (DispCallback)ext_disp, NULL);
        dispEntry(4, 0.0f, 2, (DispCallback)ext_shadow_disp, NULL);
    }
    if (wp->flags & 2) {
        dispEntry(1, -5000.0f, 1, (DispCallback)ext_disp, NULL);
        dispEntry(1, -5000.0f, 2, (DispCallback)ext_shadow_disp, NULL);
    }
}

// TODO: write ext_moamoa_main (static, 0x3220 bytes) here:
// its asm is in build/G8MJ01/rsh/asm/rsh_05.s; this is where the original has it, just before the first script that uses it

// Reconstructed logic: allocate/clear wp, copy both npc positions + spawn vector, build the
// pose array, random matrix translation + ground hitCheckFilter, then 600 warm-up ext_moamoa_main.
// NONMATCHING (50.01%): larger frame + different hitCheckFilter stack layout than the original.
static void ext_moamoa_init(void) {
    MapObject* mapObj;
    NpcEntry* npc;
    ExtPose* extPose;
    MoamoaPose* pose;
    s32 i;

    wp = _mapAlloc(0x870);
    memset(wp, 0, 0x870);

    npc = npcNameToPtr("ウェイトレス");
    wp->pos[0] = npc->position;
    npc = npcNameToPtr("車掌");
    wp->pos[1] = npc->position;
    wp->pos[2] = vec3_rsh_000136d4;

    wp->poseNum = extGetPoseNum();
    wp->poses = _mapAlloc(wp->poseNum * 0x50);
    memset(wp->poses, 0, wp->poseNum * 0x50);
    moamoa_pose_init();

    extPose = extGetPosePtr();
    pose = wp->poses;
    i = 0;
    while (i < wp->poseNum) {
        f32 hz;
        f32 hy;
        f32 hx;
        f32 dist;
        f32 nz;
        f32 ny;
        f32 nx;
        pose->flags = 0;
        pose->field_0x2 = 0;
        pose->field_0x4 = 0;
        moamoa_pose_change(pose, "c_n_moa_a", "S_1");
        extPose->flags |= 3;
        PSMTXIdentity(*extPose->matrix);
        (*extPose->matrix)[0][3] = randf(-290.0f, 240.0f);
        (*extPose->matrix)[1][3] = 0.0f;
        (*extPose->matrix)[2][3] = randf(-130.0f, 50.0f);
        hz = 0.0f;
        hy = 0.0f;
        hx = 0.0f;
        dist = 1000000.0f;
        if (hitCheckFilter(NULL, &hx, &hy, &hz, &dist, &nx, &ny, &nz,
                           (*extPose->matrix)[0][3], 100.0f + (*extPose->matrix)[1][3],
                           (*extPose->matrix)[2][3], 0.0f, -1.0f, 0.0f)) {
            (*extPose->matrix)[1][3] = hy;
        }
        i++;
        extPose++;
        pose++;
    }
    mapObj = mapGetMapObj("ressha");
    wp->field_0xC = mapObj;
    wp->field_0x10 = *(f32*)((u8*)mapObj + 0x98);
    for (i = 0; i < 600; i++) {
        ext_moamoa_main();
    }
}
static USER_FUNC(evt_moamoa_target_off) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);

    wp->pos[index] = wp->pos[1 - index];
    wp->field_0x3A[index] = 100;
    if (index == 0) {
        wp->flags |= 0x1000;
    }
    else {
        wp->flags |= 0x2000;
    }
    return EVT_RETURN_DONE;
}
static USER_FUNC(evt_moamoa_flag_onoff) {
    s32* args = event->args;
    s32 onoff = evtGetValue(event, args[0]);
    s32 bits = evtGetValue(event, args[1]);

    if (onoff != 0) {
        wp->flags |= bits;
    }
    else {
        wp->flags &= ~bits;
    }
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(evt_moamoa_init)
    CALL(evt_npc_entry, STRING("moa_a"), STRING("c_n_moa_a"))
    CALL(evt_npc_set_position, STRING("moa_a"), 0, -1000, 0)
    CALL(evt_npc_entry, STRING("moa_b"), STRING("c_n_moa_b"))
    CALL(evt_npc_set_position, STRING("moa_b"), 0, -1000, 0)
    CALL(evt_ext_entry, 400, PTR(ext_moamoa_data), PTR(ext_moamoa_init), PTR(ext_moamoa_main), PTR(ext_moamoa_dispent))
    CALL(evt_moamoa_flag_onoff, 1, 1)
    IF_INT_EQUAL(GSWF(3450), 1)
        CALL(evt_moamoa_target_off, 0)
        CALL(evt_npc_change_interrupt, STRING("ウェイトレス"), 6, PTR(talk_waitless))
    END_IF
    IF_INT_EQUAL(GSWF(3451), 1)
        CALL(evt_moamoa_target_off, 1)
        CALL(evt_npc_change_interrupt, STRING("車掌"), 6, PTR(talk_syashou))
    END_IF
    RETURN
EVT_END()
static USER_FUNC(evt_moamoa_ry_reset) {
    ExtPose* pose = extGetPosePtr();
    s32 i;

    for (i = 0; i < wp->poseNum; i++) {
        pose->field_0x14 = 0.0f;
        pose->field_0x1C = 0.0f;
        pose++;
    }
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(evt_meet_moamoa)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_seq_wait, 2)
    WAIT_MS(1500)
    CALL(evt_party_stop, 0)
    CALL(evt_npc_set_anim, STRING("ウェイトレス"), STRING("G_1"))
    CALL(evt_npc_set_anim, STRING("車掌"), STRING("G_1"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, STRING("eff0"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff_fukidashi, 3, STRING("eff1"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1000)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_print, 0, STRING("stg6_rsh_237"), 0, 0)
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_zoom_in, 25, 1500, 0, 11)
    WAIT_MS(2000)
    CALL(evt_cam3d_evt_set_at, -200, 60, -80, 0, 1000, 11)
    WAIT_MS(1000)
    WAIT_MS(2000)
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_moamoa_ry_reset)
    CALL(evt_cam3d_evt_off, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_mario_get_party, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_238_0"), 0, STRING("party"))
        CASE_EQUAL(2)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_238_1"), 0, STRING("party"))
        CASE_EQUAL(4)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_238_2"), 0, STRING("party"))
        CASE_EQUAL(5)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_238_3"), 0, STRING("party"))
        CASE_EQUAL(6)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_238_4"), 0, STRING("party"))
        CASE_EQUAL(3)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_238_5"), 0, STRING("party"))
        CASE_EQUAL(7)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_238_6"), 0, STRING("party"))
    END_SWITCH
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(GSW(0), 330)
    RETURN
EVT_END()

// TODO: write npc_jump (static, 0x484 bytes) here:
// its asm is in build/G8MJ01/rsh/asm/rsh_05.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(evt_moamoa_help)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_flag_onoff, 0, LW(0), 512)
        IF_STR_EQUAL(LW(0), STRING("車掌"))
            CALL(evt_msg_print, 0, STRING("stg6_rsh_249_00"), 0, LW(0))
        ELSE
            CALL(evt_msg_print, 0, STRING("stg6_rsh_249_01"), 0, LW(0))
        END_IF
        CALL(evt_npc_flag_onoff, 1, LW(0), 512)
    END_INLINE_EVENT
    CALL(evt_npc_flag_onoff, 1, LW(0), 32)
    CALL(evt_npc_flag_onoff, 0, LW(0), 1024)
    SET_VALUE(LW(10), LW(0))
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    CALL(evt_sub_random, 1000, LW(4))
    IF_INT_LESS(LW(4), 500)
        SET_VALUE(LW(4), 90)
    ELSE
        SET_VALUE(LW(4), 270)
    END_IF
    CALL(evt_sub_get_sincos, LW(4), LW(5), LW(6))
    MULTIPLY_FLOAT(LW(5), FLOAT(30.0f))
    MULTIPLY_FLOAT(LW(6), FLOAT(30.0f))
    ADD_FLOAT(LW(1), LW(5))
    SUBTRACT_FLOAT(LW(3), LW(6))
    CALL(npc_jump, LW(0), LW(1), LW(2), LW(3), FLOAT(0.0f), FLOAT(0.5f), 400)
    CALL(evt_sub_get_sincos, LW(4), LW(5), LW(6))
    MULTIPLY_FLOAT(LW(5), FLOAT(20.0f))
    MULTIPLY_FLOAT(LW(6), FLOAT(20.0f))
    ADD_FLOAT(LW(1), LW(5))
    SUBTRACT_FLOAT(LW(3), LW(6))
    CALL(npc_jump, LW(0), LW(1), LW(2), LW(3), FLOAT(0.0f), FLOAT(0.3f), 200)
    CALL(evt_sub_get_sincos, LW(4), LW(5), LW(6))
    MULTIPLY_FLOAT(LW(5), FLOAT(10.0f))
    MULTIPLY_FLOAT(LW(6), FLOAT(10.0f))
    ADD_FLOAT(LW(1), LW(5))
    SUBTRACT_FLOAT(LW(3), LW(6))
    CALL(npc_jump, LW(0), LW(1), LW(2), LW(3), FLOAT(0.0f), FLOAT(0.2f), 100)
    CALL(evt_npc_flag_onoff, 1, LW(0), 1024)
    CALL(evt_npc_flag_onoff, 0, LW(0), 32)
    IF_STR_EQUAL(LW(10), STRING("車掌"))
        CALL(evt_npc_set_anim, LW(10), STRING("O_1"))
    ELSE
        CALL(evt_npc_set_anim, LW(10), STRING("P_1"))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_moamoa_escape0)
    SET_VALUE(LW(0), STRING("ウェイトレス"))
    SPAWN_EVENT(evt_moamoa_help)
    SET_VALUE(GSWF(3450), 1)
    CALL(evt_npc_change_interrupt, STRING("ウェイトレス"), 6, PTR(talk_waitless))
    RETURN
EVT_END()

EVT_BEGIN(evt_moamoa_escape1)
    SET_VALUE(LW(0), STRING("車掌"))
    SPAWN_EVENT(evt_moamoa_help)
    SET_VALUE(GSWF(3451), 1)
    CALL(evt_npc_change_interrupt, STRING("車掌"), 6, PTR(talk_syashou))
    RETURN
EVT_END()
static USER_FUNC(evt_moamoa_end_wait) {
    s32 i;
    MoamoaPose* pose;

    extGetPosePtr();
    pose = wp->poses;
    for (i = wp->poseNum; i > 0; i--) {
        if (!(pose->flags & 0x20)) {
            return EVT_RETURN_BLOCK;
        }
        pose++;
    }
    return EVT_RETURN_DONE;
}
static USER_FUNC(evt_moamoa_exit) {
    extReset();
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(evt_moamoa_escape)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    WAIT_MS(2000)
    CALL(evt_cam3d_evt_set_xyz, -200, 0, 0, 0, 3000, 11)
    WAIT_MS(3000)
    CALL(evt_moamoa_end_wait)
    CALL(evt_moamoa_exit)
    CALL(evt_cam3d_evt_xyz_off, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_party_stop, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("車掌"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("ウェイトレス"), 32)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_status_onoff, 1, STRING("車掌"), 2)
        CALL(evt_npc_move_position, STRING("車掌"), -80, 22, 1000, 0, 1)
        CALL(evt_npc_set_ry, STRING("車掌"), 90)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_status_onoff, 1, STRING("ウェイトレス"), 2)
        CALL(evt_npc_move_position, STRING("ウェイトレス"), 0, 15, 1000, 0, 1)
        CALL(evt_npc_set_ry, STRING("ウェイトレス"), 270)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, -20, -10, FLOAT(80.0f))
        CALL(evt_party_set_dir, 0, 270, 300)
    END_INLINE_EVENT
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    IF_INT_LESS(LW(2), -140)
        CALL(evt_mario_mov_pos2, LW(0), -100, FLOAT(80.0f))
    END_IF
    CALL(evt_mario_flag_onoff, 1, 1024)
    CALL(evt_mario_mov_pos2, -45, 5, FLOAT(80.0f))
    CALL(evt_mario_set_dir, 270, 300, 0)
    CALL(evt_mario_flag_onoff, 0, 1024)
    CALL(evt_cam3d_evt_set, -40, 90, 283, -40, 38, -13, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_240_00"), 0, STRING("車掌"))
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_party_set_dir, 0, 90, 300)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir, 90, 300, 0)
    END_INLINE_EVENT
    CALL(evt_msg_print, 0, STRING("stg6_rsh_241"), 0, STRING("ウェイトレス"))
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_party_set_dir, 0, 270, 300)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir, 270, 300, 0)
    END_INLINE_EVENT
    CALL(evt_msg_print, 0, STRING("stg6_rsh_240_01"), 0, STRING("車掌"))
    WAIT_MS(500)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_party_run, 0)
    CALL(evt_npc_flag_onoff, 0, STRING("車掌"), 32)
    CALL(evt_npc_flag_onoff, 0, STRING("ウェイトレス"), 32)
    CALL(evt_npc_change_interrupt, STRING("車掌"), 6, PTR(talk_syashou))
    CALL(evt_npc_change_interrupt, STRING("ウェイトレス"), 6, PTR(talk_waitless))
    SET_VALUE(GSW(0), 331)
    SET_VALUE(LW(0), STRING("w_bero"))
    CALL_CHILD(bero_case_switch_on)
    RETURN
EVT_END()

static EVT_BEGIN(init_syashou)
    SWITCH(GSW(0))
        CASE_BETWEEN(329, 332)
        CALL(evt_npc_set_position, STRING("me"), -115, 0, 20)
        CALL(evt_npc_set_autotalkpose, STRING("me"), STRING("O_1"), STRING("T_3"))
        CALL(evt_npc_set_tribe, STRING("me"), STRING("車掌悲"))
        CASE_DEFAULT
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(talk_syashou)
    IF_INT_LESS(GSW(0), 331)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_249_01_01"), 0, 0)
    ELSE
        CALL(evt_msg_print, 0, STRING("stg6_rsh_242"), 0, 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(init_waitless)
    SWITCH(GSW(0))
        CASE_BETWEEN(329, 332)
        CALL(evt_npc_set_position, STRING("me"), 50, 0, 0)
        CALL(evt_npc_set_autotalkpose, STRING("me"), STRING("P_1"), STRING("T_5"))
        CALL(evt_npc_set_tribe, STRING("me"), STRING("ウェイトレス悲"))
        CASE_DEFAULT
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(talk_waitless)
    IF_INT_LESS(GSW(0), 331)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_249_01_02"), 0, 0)
    ELSE
        CALL(evt_msg_print, 0, STRING("stg6_rsh_243"), 0, 0)
    END_IF
    RETURN
EVT_END()

static BeroEntry bero_data_a[3] = {
    {"e_bero", 1, 8, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_04_a", "w_bero", 1, 1, "e_door_close", "e_door_open"},
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_06_a", "w_bero", 1, 1},
    {NULL},
};

static BeroEntry bero_data_b[3] = {
    {"e_bero", 1, 8, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_04_b", "w_bero", 1, 1, "e_door_close", "e_door_open"},
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_06_a", "w_bero", 1, 1},
    {NULL},
};

static BeroEntry bero_data_c[3] = {
    {"e_bero", 1, 8, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_04_c", "w_bero", 1, 1, "e_door_close", "e_door_open"},
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_06_a", "w_bero", 1, 1},
    {NULL},
};

// TODO: type
static u32 npc_data[69] = {
    (u32)"車掌", 0x00000600, 0x00000000, (u32)&init_syashou,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"ウェイトレス",
    0x00000600, 0x00000000, (u32)&init_waitless, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000,
};

EVT_BEGIN(evt_ghost_diary)
    SET_VALUE(GSW(0), 299)
    RETURN
EVT_END()

EVT_BEGIN(evt_ghost_diary_read)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_130"), 0, 0)
    CALL(evt_msg_select, 0, STRING("stg6_rsh_130_yn"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        RETURN
    END_IF
    CALL(evt_msg_print_add, 0, STRING("stg6_rsh_131"))
    CALL(evt_msg_select, 0, STRING("stg6_rsh_130_yn"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        RETURN
    END_IF
    CALL(evt_msg_print_add, 0, STRING("stg6_rsh_132"))
    CALL(evt_msg_select, 0, STRING("stg6_rsh_130_yn"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        RETURN
    END_IF
    CALL(evt_msg_print_add, 0, STRING("stg6_rsh_133"))
    CALL(evt_msg_print, 0, STRING("stg6_rsh_134"), 0, STRING("ゆうれい"))
    RETURN
EVT_END()

static char* access_tbl[13] = {
    "kihako_1", "stg6_rsh_275", "kibako_2", "stg6_rsh_275", "bin_1", "stg6_rsh_276", "wagon",
    "stg6_rsh_280", "hako_1", "stg6_rsh_281", "hako_2", "stg6_rsh_281", NULL,
};

static EVT_BEGIN(evt_access)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, LW(1), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_access2)
    IF_INT_EQUAL(LSWF(12), 0)
        SET_VALUE(LSWF(12), 1)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_277"), 0, 0)
        CALL(evt_item_entry, STRING("a"), 142, 0, -1000, 0, 17, -1, 0)
        CALL(evt_item_get_item, STRING("a"))
        CALL(evt_mario_key_onoff, 1)
    ELSE
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_279"), 0, 0)
        CALL(evt_mario_key_onoff, 1)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(evt_access_entry)
    SET_INT_READ_ADDR(PTR(access_tbl))
    LOOP(0)
        READ_INT2(LW(0), LW(1))
        IF_INT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        CALL(evt_run_case_evt, 9, 1, LW(0), 0, PTR(evt_access), 0)
    END_LOOP
    CALL(evt_run_case_evt, 9, 1, STRING("bin_2"), 0, PTR(evt_access2), 0)
    RETURN
EVT_END()

EVT_BEGIN(rsh_05_init_evt)
    SWITCH(GSW(0))
        CASE_BETWEEN(310, 325)
        CASE_DEFAULT
        CALL(evt_map_playanim, STRING("yure"), 1, 0)
    END_SWITCH
    CALL_CHILD(evt_access_entry)
    SWITCH(GSW(0))
        CASE_BETWEEN(284, 286)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_POWAN1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1_3"))
        CASE_BETWEEN(304, 309)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_POWAN1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1_3"))
        CASE_BETWEEN(310, 325)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_HOM1"))
        CALL(evt_snd_envoff, 512)
        CALL(evt_snd_bgmoff, 26624)
        CASE_BETWEEN(327, 328)
        CALL(evt_snd_bgmoff, 512)
        CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1_3"))
        CASE_BETWEEN(329, 331)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_DANGER4"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1_3"))
        CASE_BETWEEN(334, 344)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_PIK1"))
        CALL(evt_snd_envoff, 512)
        CALL(evt_snd_bgmoff, 26624)
        CASE_DEFAULT
        CALL(evt_snd_bgmoff, 512)
        CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1_3"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(rsh_05_a_init_evt)
    SWITCH(GSW(0))
        CASE_LESS(288)
        SET_VALUE(LW(0), PTR(bero_data_a))
        CASE_LESS(294)
        SET_VALUE(LW(0), PTR(bero_data_b))
        CASE_LESS(303)
        SET_VALUE(LW(0), PTR(bero_data_c))
        CASE_LESS(309)
        SET_VALUE(LW(0), PTR(bero_data_a))
        CASE_LESS(326)
        SET_VALUE(LW(0), PTR(bero_data_b))
        CASE_LESS(328)
        SET_VALUE(LW(0), PTR(bero_data_c))
        CASE_DEFAULT
        SET_VALUE(LW(0), PTR(bero_data_a))
    END_SWITCH
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(rsh_05_init_evt)
    CALL(evt_npc_setup, PTR(npc_data))
    SWITCH(GSW(0))
        CASE_LESS(329)
        SET_VALUE(LW(0), STRING("w_bero"))
        CALL_CHILD(bero_case_switch_off)
        CASE_EQUAL(329)
        CALL_CHILD(evt_moamoa_init)
        SPAWN_EVENT(evt_meet_moamoa)
        SET_VALUE(LW(0), STRING("w_bero"))
        CALL_CHILD(bero_case_switch_off)
        CASE_EQUAL(330)
        CALL_CHILD(evt_moamoa_init)
        SET_VALUE(LW(0), STRING("w_bero"))
        CALL_CHILD(bero_case_switch_off)
        CASE_DEFAULT
        CALL(evt_npc_change_interrupt, STRING("車掌"), 6, PTR(talk_syashou))
        CALL(evt_npc_change_interrupt, STRING("ウェイトレス"), 6, PTR(talk_waitless))
    END_SWITCH
    IF_INT_LESS(GSW(0), 299)
        CALL(evt_item_entry, STRING("diary"), 78, -90, 0, -140, 0, -1, PTR(evt_ghost_diary))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(rsh_05_init_evt_nidome)
    IF_INT_EQUAL(GSWF(3579), 0)
        IF_INT_EQUAL(GSWF(3580), 0)
            IF_INT_EQUAL(GSWF(3581), 0)
                CALL(evt_map_playanim, STRING("yure"), 1, 0)
            END_IF
        END_IF
    END_IF
    CALL(evt_npc_setup, PTR(npc_data))
    IF_INT_EQUAL(GSWF(3460), 1)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_PIK1"))
        CALL(evt_snd_envoff, 512)
        CALL(evt_snd_bgmoff, 26624)
    ELSE
        IF_INT_EQUAL(GSWF(3461), 1)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_GOR1"))
            CALL(evt_snd_envoff, 512)
            CALL(evt_snd_bgmoff, 26624)
        ELSE
            CALL(evt_snd_bgmoff, 512)
            CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1_3"))
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(rsh_05_a_init_evt_nidome)
    IF_INT_EQUAL(GSWF(3453), 1)
        SET_VALUE(LW(0), PTR(bero_data_a))
    END_IF
    IF_INT_EQUAL(GSWF(3454), 1)
        SET_VALUE(LW(0), PTR(bero_data_b))
    END_IF
    IF_INT_EQUAL(GSWF(3455), 1)
        SET_VALUE(LW(0), PTR(bero_data_c))
    END_IF
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(rsh_05_init_evt_nidome)
    RETURN
EVT_END()

// TODO: type
static u32 jumptable_00014F3C[66] = {
    (u32)((u8*)&ext_moamoa_main + 0xb70), (u32)((u8*)&ext_moamoa_main + 0x15f0), (u32)((u8*)&ext_moamoa_main + 0x18e8), (u32)((u8*)&ext_moamoa_main + 0x1b4c),
    (u32)((u8*)&ext_moamoa_main + 0x1da8), (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c),
    (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c),
    (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c),
    (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c),
    (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c),
    (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x25c4), (u32)((u8*)&ext_moamoa_main + 0x2648), (u32)((u8*)&ext_moamoa_main + 0x2674),
    (u32)((u8*)&ext_moamoa_main + 0x2994), (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c),
    (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c),
    (u32)((u8*)&ext_moamoa_main + 0x1dcc), (u32)((u8*)&ext_moamoa_main + 0x1dcc), (u32)((u8*)&ext_moamoa_main + 0x1dcc), (u32)((u8*)&ext_moamoa_main + 0x20d0),
    (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c),
    (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x2130),
    (u32)((u8*)&ext_moamoa_main + 0x2480), (u32)((u8*)&ext_moamoa_main + 0x2510), (u32)((u8*)&ext_moamoa_main + 0x1358), (u32)((u8*)&ext_moamoa_main + 0x1384),
    (u32)((u8*)&ext_moamoa_main + 0x144c), (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c),
    (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c),
    (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x302c), (u32)((u8*)&ext_moamoa_main + 0x2c88), (u32)((u8*)&ext_moamoa_main + 0x2cb0),
    (u32)((u8*)&ext_moamoa_main + 0x2f3c), (u32)((u8*)&ext_moamoa_main + 0x2f60),
};

// TODO: write moamoa_hit_check (static, 0xb28 bytes) here:
// its asm is in build/G8MJ01/rsh/asm/rsh_05.s; this is where the original has it, just before the first script that uses it

// TODO: write moamoa_jump (static, 0x270 bytes) here:
// its asm is in build/G8MJ01/rsh/asm/rsh_05.s; this is where the original has it, just before the first script that uses it
static void ext_shadow_disp(s32 camId) {
    static Mtx mt = {{8.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 8.0f, 0.0f, 0.5f}, {0.0f, 0.0f, 8.0f, 0.0f}};
    u16 left, top, right, bottom;

    if (camId == 1) {
        GXClearBoundingBox();
    }
    extLoadShadowMtx(mt);
    extLoadShadowRenderMode();
    extLoadShadowVertex();
    extLoadShadowTev();
    extDrawShadow();
    if (camId == 1) {
        sysWaitDrawSync();
        GXReadBoundingBox(&left, &top, &right, &bottom);
        offscreenAddBoundingBox(offscreenNameToId("offscreen"), left, top, right, bottom);
    }
}
static void ext_disp(s32 camId) {
    u16 left, top, right, bottom;

    if (camId == 1) {
        GXClearBoundingBox();
    }
    extLoadRenderMode();
    extLoadVertex();
    extLoadTexture();
    extLoadTev();
    extDraw();
    extLoadTextureExit();
    GXSetNumIndStages(0);
    GXSetTevDirect(0);
    if (camId == 1) {
        sysWaitDrawSync();
        GXReadBoundingBox(&left, &top, &right, &bottom);
        offscreenAddBoundingBox(offscreenNameToId("offscreen"), left, top, right, bottom);
    }
}
static f32 randf(f32 min, f32 max) {
    f32 value = (f32)rand() / 32767.0f;

    return value * (max - min) + min;
}
static USER_FUNC(evt_moamoa_fusion_debug) {
    s32 i;
    ExtPose* extPose = extGetPosePtr();
    MoamoaPose* pose = wp->poses;

    i = 0;
    while (i < wp->poseNum) {
        extPose->flags |= 0x4;
        pose->flags |= 0x400;
        pose->flags |= 0x100;
        pose->flags |= 0x200;
        pose->field_0x2 = 0x3D;
        pose->flags |= 0x800;
        moamoa_pose_change(pose, "c_n_moa_a", "S_1");
        i++;
        extPose++;
        pose++;
    }
    return EVT_RETURN_DONE;
}
static USER_FUNC(evt_moamoa_blow) {
    s32 i;
    MoamoaPose* pose;

    (void)evtGetValue(event, event->args[0]);
    extGetPosePtr();
    pose = wp->poses;
    for (i = 0; i < wp->poseNum; i++) {
        pose->flags &= ~0x400;
        pose->field_0x2 = 0x3F;
        pose++;
    }
    return EVT_RETURN_DONE;
}
static USER_FUNC(evt_moamoa_kind_onoff) {
    s32* args = event->args;
    s32 on = evtGetValue(event, args[0]);
    s32 off = evtGetValue(event, args[1]);
    s32 i;
    MoamoaPose* pose;

    extGetPosePtr();
    pose = wp->poses;
    i = 0;
    while (i < wp->poseNum) {
        if (on) {
            if (off == 0) {
                if (pose->field_0x4C == 1) {
                    pose->flags |= 0x800;
                }
            }
            else {
                pose->flags |= 0x800;
            }
        }
        else {
            if (off == 0) {
                if (pose->field_0x4C != 1) {
                    pose->flags &= ~0x800;
                }
            }
            else {
                pose->flags &= ~0x800;
            }
        }
        moamoa_pose_change(pose, "c_n_moa_a", "S_1");
        i++;
        pose++;
    }
    return EVT_RETURN_DONE;
}
static USER_FUNC(evt_moamoa_fusion) {
    ExtPose* extPose;
    s32 i;
    MoamoaPose* pose;

    if (isFirstCall) {
        extPose = extGetPosePtr();
        pose = wp->poses;
        i = 0;
        while (i < wp->poseNum) {
            (*extPose->matrix)[2][2] = 1.0f;
            (*extPose->matrix)[1][1] = 1.0f;
            (*extPose->matrix)[0][0] = 1.0f;
            pose->field_0x24 = 0xE;
            pose->field_0x2 = 0xE;
            pose->field_0x4 = (i < 100) ? (i % 100) : (i % 60 + 100);
            i++;
            extPose++;
            pose->flags |= 0x100;
            pose++;
        }
    }
    extGetPosePtr();
    pose = wp->poses;
    for (i = 0; i < wp->poseNum; i++) {
        if (!(pose->flags & 0x200)) {
            return EVT_RETURN_BLOCK;
        }
        pose++;
    }
    extPose = extGetPosePtr();
    pose = wp->poses;
    i = 0;
    while (i < wp->poseNum) {
        extPose->flags |= 0x4;
        pose->flags |= 0x400;
        pose->field_0x2 = 0x3D;
        moamoa_pose_change(pose, "c_n_moa_a", "S_1");
        i++;
        extPose++;
        pose++;
    }
    return EVT_RETURN_DONE;
}
static void moamoa_pose_change(MoamoaPose* pose, char* name1, char* name2) {
    MoamoaGroup* group = wp->groups;
    s32 i;

    if (pose->flags & 0x800) {
        name1 = group_tbl[pose->field_0x4C][0];
        if (pose->field_0x4C >= 2) {
            name2 = group_tbl[pose->field_0x4C][1];
        }
        if (pose->field_0x4C == 0xA) {
            name2 = "S_2";
        }
    }
    i = 0;
    while (i < wp->groupCount) {
        if (strcmp(group->name, name1) == 0 && strcmp(group->name2, name2) == 0) {
            break;
        }
        i++;
        group++;
    }
    if (i < wp->groupCount && pose->field_0x3C != i) {
        if ((pose->flags & 0x4) && strncmp(name2, "D", 1) != 0) {
            return;
        }
        if ((pose->flags & 0x10) && strncmp(name2, "R", 1) != 0) {
            return;
        }
        pose->field_0x3C = i;
        pose->time = gp->renderTime;
    }
}
static void moamoa_pose_main(void) {
    MoamoaGroup* group;
    s32 i;
    ExtPose* pose;
    MoamoaPose* work;
    f32 progress;

    pose = extGetPosePtr();
    work = wp->poses;
    i = 0;
    while (i < wp->poseNum) {
        group = &wp->groups[work->field_0x3C];
        progress = (f32)OSTicksToMilliseconds((u64)(gp->renderTime - work->time)) * 60.0f / 1000.0f
                   / group->field_0x1C;
        if (progress >= 1.0f) {
            work->time = gp->renderTime;
            progress = 0.0f;
        }
        pose->field_0x10 = group->field_0x20 + (s32)((f32)(group->field_0x24 - 1) * progress);
        i++;
        work++;
        pose++;
    }
}
static void moamoa_pose_init(void) {
    MoamoaGroupDef* def = ext_moamoa_data;
    char name[32];
    char groupName[32];
    s32 i;
    s32 start;

    i = 0;
    strcpy(name, "");
    strcpy(groupName, "");
    wp->groupCount = -1;
    do {
        if (strcmp(def->name, name) != 0) {
            strcpy(name, def->name);
            strcpy(groupName, "");
        }
        if (strcmp(def->name2, groupName) != 0) {
            strcpy(groupName, def->name2);
            start = i;
            wp->groupCount++;
        }
        strcpy(wp->groups[wp->groupCount].name, name);
        strcpy(wp->groups[wp->groupCount].name2, groupName);
        wp->groups[wp->groupCount].field_0x20 = start;
        wp->groups[wp->groupCount].field_0x24 = i - start + 1;
        wp->groups[wp->groupCount].field_0x1C = def->field_0x8;
        wp->groups[wp->groupCount].field_0x18 = 1;
        if (strncmp(groupName, "J", 1) == 0) {
            wp->groups[wp->groupCount].field_0x18 = 0;
        }
        def++;
        i++;
    } while (def->name != NULL);
    wp->groupCount++;
}
// clang-format on
