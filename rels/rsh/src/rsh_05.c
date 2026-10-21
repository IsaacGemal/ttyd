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
#include "party.h"
#include "party/party_nokonoko.h"
#include "party/party_cloud.h"
#include "pmario_sound.h"
#include "driver/camdrv.h"
#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "math_ext.h"
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
    s16 state;          // 0x2, switch key in ext_moamoa_main (0x0 wander, 0x1 settle, 0x32-0x34 fusion, ...)
    s16 stateTimer;     // 0x4, frames left in the current state (counts down or up per state)
    u8 pad_0x6[2];      // 0x6
    Vec target;         // 0x8, moamoa_jump's landing target; a per-frame velocity in the fly-off states
                        //      (0x30, 0x3F on), with 0x14/0x1C/0x20 as its per-frame change
    f32 field_0x14;     // 0x14
    s16 frames;         // 0x18, frames left in the current jump or glide (counts down to 0)
    u8 pad_0x1A[2];     // 0x1A
    f32 field_0x1C;     // 0x1C
    f32 field_0x20;     // 0x20
    s16 queuedState;    // 0x24, state to enter when the current jump lands (set by moamoa_jump)
    u8 pad_0x26[2];     // 0x26
    f32 savedX;         // 0x28, position saved when a glide starts
    f32 savedZ;         // 0x2C
    Vec offset;         // 0x30, unused here (rsh_06 adds it to the pose)
    s16 groupIndex;     // 0x3C, index into MoamoaWork.groups (the animation group)
    u8 pad_0x3E[2];     // 0x3E
    OSTime poseTime;    // 0x40, render time the current pose cycle started
    u8 pad_0x48[4];     // 0x48
    u8 kind;            // 0x4C, index into group_tbl (the moamoa's name group)
    u8 pad_0x4D[3];     // 0x4D
} MoamoaPose;

// Moamoa group table entry (ext_moamoa_data), stride 0xC.
typedef struct MoamoaGroupDef {
    char* name;     // 0x0
    char* anim;     // 0x4
    f32 cycleFrames; // 0x8, frames for one pass through the group's poses
} MoamoaGroupDef;

// Moamoa name-group entry, stride 0x28, starting at MoamoaWork+0x50.
typedef struct MoamoaGroup {
    char name[16];      // 0x0
    char anim[8];       // 0x10
    u16 enabled;        // 0x18, 0 for the J-prefixed animations
    u8 pad_0x1A[2];     // 0x1A
    f32 cycleFrames;    // 0x1C
    s32 firstPose;      // 0x20
    s32 numPoses;       // 0x24
} MoamoaGroup;

// Moamoa EXTask work header (allocated as 0x870 bytes by ext_moamoa_init).
typedef struct MoamoaWork {
    u16 flags;          // 0x0
    u16 field_0x2;      // 0x2
    s32 poseNum;        // 0x4
    MoamoaPose* poses;  // 0x8
    void* train;        // 0xC, the "ressha" map object the moamoa ride
    f32 prevTrainY;     // 0x10, train height last frame (the moamoa follow its change)
    Vec targets[3];     // 0x14, gathering points; moamoa i heads for targets[i % 3]
    s16 hitCount;       // 0x38, incremented on each player hit
    s16 nearHits[3];    // 0x3A, hits within 200 units of targets[i]; over 50 retires that target
    s16 hammerFrames; // 0x40, frames Mario has stayed in MARIO_MOTION_HAMMER
    s16 idleFrames;     // 0x42, frames Mario has stood still (capped at 1000)
    u32 escapeEvt;      // 0x44, event id of evt_moamoa_escape once started
    s32 field_0x48;     // 0x48
    s32 groupCount;     // 0x4C
    MoamoaGroup groups[32]; // 0x50
    f32 angleToMario;   // 0x550, angle from Mario to the moamoa in the current pose
    f32 distToMario;    // 0x554, distance from Mario to the current pose
    s32 bounceCount;    // 0x558, consecutive bounces off the moamoa while Mario is airborne (capped at 2)
    f32 breathDir;      // 0x55C, cloud breath direction (-1 when not in use)
    f32 breathDist;     // 0x560, cloud breath distance
    Vec breathPos;      // 0x564
} MoamoaWork;

#pragma force_active on
static USER_FUNC(npc_jump);
static void ext_moamoa_dispent(void);
static void ext_moamoa_main(void);
extern s32 evt_moamoa_escape[];
static s32 moamoa_hit_check(ExtPose* pose, MoamoaPose* entry);
static void moamoa_jump(ExtPose* pose, MoamoaPose* entry, f32 x, f32 y, f32 z, f32 p4, f32 p5, s32 field0x2, s32 flag);
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
    {NULL, NULL, 0.0f},
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

static Mtx mt = {
    {8.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 8.0f, 0.0f, 0.5f},
    {0.0f, 0.0f, 8.0f, 0.0f},
};

// Constant spawn offset used by ext_moamoa_init (moamoa spawn vector).
// The original keeps this global and read-only, but not `const`: MWCC then cannot hoist the
// copy in ext_moamoa_init past the wp->targets[1] stores (it emits the `lwzu` form).
__declspec(section ".rodata") Vec vec3_rsh_000136d4 = {-290.0f, 65.0f, -95.0f};
const Vec vec3_rsh_000136e0 = {0.0f, 0.0f, 0.0f};
const Vec vec3_rsh_000136ec = {0.0f, 0.0f, 0.0f};
const Vec vec3_rsh_000136f8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_rsh_00013704 = {0.0f, 0.0f, 0.0f};
const Vec vec3_rsh_00013710 = {0.0f, 0.0f, 0.0f};
const Vec vec3_rsh_0001371c = {0.0f, 0.0f, 0.0f};
const Vec vec3_rsh_00013728 = {0.0f, 0.0f, 0.0f};
const Vec vec3_rsh_00013734 = {0.0f, 0.0f, 0.0f};
__declspec(section ".rodata") Vec vec3_rsh_00013740 = {0.0f, 0.0f, 0.0f};
const Vec vec3_rsh_0001374c = {0.0f, 0.0f, 0.0f};
const Vec vec3_rsh_00013758 = {0.0f, 0.0f, 0.0f};
const Vec vec3_rsh_00013764 = {0.0f, 0.0f, 0.0f};
const Vec vec3_rsh_00013770 = {0.0f, 0.0f, 0.0f};
const Vec vec3_rsh_0001377c = {0.0f, 0.0f, 0.0f};
const Vec vec3_rsh_00013788 = {0.0f, 0.0f, 0.0f};
const Vec vec3_rsh_00013794 = {0.0f, 0.0f, 0.0f};

#pragma dont_inline on
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
        if (strcmp(def->anim, groupName) != 0) {
            strcpy(groupName, def->anim);
            start = i;
            wp->groupCount++;
        }
        strcpy(wp->groups[wp->groupCount].name, name);
        strcpy(wp->groups[wp->groupCount].anim, groupName);
        wp->groups[wp->groupCount].firstPose = start;
        wp->groups[wp->groupCount].numPoses = i - start + 1;
        wp->groups[wp->groupCount].cycleFrames = def->cycleFrames;
        wp->groups[wp->groupCount].enabled = 1;
        if (strncmp(groupName, "J", 1) == 0) {
            wp->groups[wp->groupCount].enabled = 0;
        }
        def++;
        i++;
    } while (def->name != NULL);
    wp->groupCount++;
}
#pragma dont_inline reset

// Inline sine/cosine approximations (the polynomials shared by the NPC blur path).
static inline f32 moamoa_sin(f32 angle) {
    f32 v = 0.00761f;
    f32 b = 0.16605f;
    f32 one = 1.0f;
    f32 u;
    if (angle <= 3.1415927f) {
        if (angle <= 1.5707964f) { u = angle * angle; v = v * u - b; v = v * u + one; v = v * angle; }
        else { angle = 1.5707964f - (angle - 1.5707964f); u = angle * angle; v = v * u - b; v = v * u + one; v = v * angle; }
    } else if (angle < 4.712389f) {
        angle = angle - 3.1415927f; u = angle * angle; v = v * u - b; v = v * u + one; v = v * angle; v = -v;
    } else {
        angle = 1.5707964f - (angle - 4.712389f); u = angle * angle; v = v * u - b; v = v * u + one; v = v * angle; v = -v;
    }
    return v;
}

static inline f32 moamoa_cos(f32 angle) {
    f32 v = 0.03705f;
    f32 b = 0.4967f;
    f32 one = 1.0f;
    f32 u;
    if (angle <= 3.1415927f) {
        if (angle <= 1.5707964f) { u = angle * angle; v = v * u - b; v = v * u + one; }
        else { angle = 1.5707964f - (angle - 1.5707964f); u = angle * angle; v = v * u - b; v = v * u + one; v = -v; }
    } else if (angle < 4.712389f) {
        angle = angle - 3.1415927f; u = angle * angle; v = v * u - b; v = v * u + one; v = -v;
    } else {
        angle = 1.5707964f - (angle - 4.712389f); u = angle * angle; v = v * u - b; v = v * u + one;
    }
    return v;
}

// The original builds each temporary Vec from a named zero vector and passes it by
// value through an inline; MWCC keeps the inline's return temp alongside the local.
static inline Vec moamoa_vec(f32 x, f32 y, f32 z, const Vec* base) {
    Vec v = *base;

    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

#ifdef MUST_MATCH
// Match-only: the volatile copy stops MWCC hoisting the base vector's lane loads above the two
// `pose->matrix` stores in the pvec rebuild (retail keeps each 12-byte copy serialized); the plain
// copy is `moamoa_vec`.
static inline Vec moamoa_vec_v(f32 x, f32 y, f32 z, const Vec* base) {
    Vec v = *(volatile Vec*)base;

    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}
#endif



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
        group = &wp->groups[work->groupIndex];
        progress = (f32)OSTicksToMilliseconds((u64)(gp->renderTime - work->poseTime)) * 60.0f / 1000.0f
                   / group->cycleFrames;
        if (progress >= 1.0f) {
            work->poseTime = gp->renderTime;
            progress = 0.0f;
        }
        pose->texId = group->firstPose + (s32)((f32)(group->numPoses - 1) * progress);
        i++;
        work++;
        pose++;
    }
}

static void moamoa_pose_change(MoamoaPose* pose, char* name1, char* name2) {
    MoamoaGroup* group = wp->groups;
    s32 i;

    if (pose->flags & 0x800) {
        name1 = group_tbl[pose->kind][0];
        if (pose->kind >= 2) {
            name2 = group_tbl[pose->kind][1];
        }
        if (pose->kind == 0xA) {
            name2 = "S_2";
        }
    }
    i = 0;
    while (i < wp->groupCount) {
        if (strcmp(group->name, name1) == 0 && strcmp(group->anim, name2) == 0) {
            break;
        }
        i++;
        group++;
    }
    if (i < wp->groupCount && pose->groupIndex != i) {
        if ((pose->flags & 0x4) && strncmp(name2, "D", 1) != 0) {
            return;
        }
        if ((pose->flags & 0x10) && strncmp(name2, "R", 1) != 0) {
            return;
        }
        pose->groupIndex = i;
        pose->poseTime = gp->renderTime;
    }
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

static USER_FUNC(evt_moamoa_exit) {
    extReset();
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
        pose->yaw = 0.0f;
        pose->targetYaw = 0.0f;
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
            pose->queuedState = 0xE;
            pose->state = 0xE;
            pose->stateTimer = (i < 100) ? (i % 100) : (i % 60 + 100);
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
        extPose->flags |= EXT_POSE_NO_SHADOW;
        pose->flags |= 0x400;
        pose->state = 0x3D;
        moamoa_pose_change(pose, "c_n_moa_a", "S_1");
        i++;
        extPose++;
        pose++;
    }
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
                if (pose->kind == 1) {
                    pose->flags |= 0x800;
                }
            }
            else {
                pose->flags |= 0x800;
            }
        }
        else {
            if (off == 0) {
                if (pose->kind != 1) {
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

static USER_FUNC(evt_moamoa_blow) {
    s32 i;
    MoamoaPose* pose;

    (void)evtGetValue(event, event->args[0]);
    extGetPosePtr();
    pose = wp->poses;
    for (i = 0; i < wp->poseNum; i++) {
        pose->flags &= ~0x400;
        pose->state = 0x3F;
        pose++;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_moamoa_fusion_debug) {
    s32 i;
    ExtPose* extPose = extGetPosePtr();
    MoamoaPose* pose = wp->poses;

    i = 0;
    while (i < wp->poseNum) {
        extPose->flags |= EXT_POSE_NO_SHADOW;
        pose->flags |= 0x400;
        pose->flags |= 0x100;
        pose->flags |= 0x200;
        pose->state = 0x3D;
        pose->flags |= 0x800;
        moamoa_pose_change(pose, "c_n_moa_a", "S_1");
        i++;
        extPose++;
        pose++;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_moamoa_target_off) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);

    wp->targets[index] = wp->targets[1 - index];
    wp->nearHits[index] = 100;
    if (index == 0) {
        wp->flags |= 0x1000;
    }
    else {
        wp->flags |= 0x2000;
    }
    return EVT_RETURN_DONE;
}

// randf's scaling step, for the sites in ext_moamoa_main that draw the rand() fraction ahead of its use.
static inline f32 randScale(f32 t, f32 min, f32 max) {
    return t * (max - min) + min;
}

static inline f32 randRange(f32 min, f32 max) {
    return max - min;
}

static f32 randf(f32 min, f32 max) {
    f32 value = (f32)rand() / 32767.0f;

    return value * (max - min) + min;
}

// Reconstructed logic: allocate/clear wp, copy both npc positions + spawn vector, build the
// pose array, random matrix translation + ground hitCheckFilter, then 600 warm-up ext_moamoa_main.
static void ext_moamoa_init(void) {
    MapObject* mapObj;
    NpcEntry* npc;
    s32 i;
    ExtPose* extPose;
    MoamoaPose* pose;

    wp = _mapAlloc(0x870);
    memset(wp, 0, 0x870);

    npc = npcNameToPtr("ウェイトレス");
    wp->targets[0] = npc->position;
    npc = npcNameToPtr("車掌");
    wp->targets[1] = npc->position;
    wp->targets[2] = vec3_rsh_000136d4;

    wp->poseNum = extGetPoseNum();
    wp->poses = _mapAlloc(wp->poseNum * 0x50);
    memset(wp->poses, 0, wp->poseNum * 0x50);
    moamoa_pose_init();

    extPose = extGetPosePtr();
    pose = wp->poses;
    i = 0;
    while (i < wp->poseNum) {
        f32 hx;
        f32 hy;
        f32 hz;
        f32 dist;
        f32 nx;
        f32 ny;
        f32 nz;
        pose->flags = 0;
        pose->state = 0;
        pose->stateTimer = 0;
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
        if (hitCheckFilter(NULL, (*extPose->matrix)[0][3], 100.0f + (*extPose->matrix)[1][3], (*extPose->matrix)[2][3], 0.0f, -1.0f, 0.0f, &hx,
                           &hy, &hz,
                           &dist, &nx, &ny, &nz)) {
            (*extPose->matrix)[1][3] = hy;
        }
        i++;
        extPose++;
        pose++;
    }
    mapObj = mapGetMapObj("ressha");
    wp->train = mapObj;
    wp->prevTrainY = ((f32*)((u8*)wp->train + 0x98))[0];
    for (i = 0; i < 600; i++) {
        ext_moamoa_main();
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

static void ext_shadow_disp(s32 camId) {
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

// The original calls moamoa_jump (a real bl) from ext_moamoa_main; the raised inline limits would inline it.
#pragma dont_inline on
static void moamoa_jump(ExtPose* pose, MoamoaPose* entry, f32 x, f32 y, f32 z, f32 p4, f32 p5, s32 field0x2, s32 flag) {
    if (flag) {
        f32 hx, hy, hz, dist, nx, ny, nz;

        hz = 0.0f;
        hy = 0.0f;
        hx = 0.0f;
        dist = 1000000.0f;
        if (hitCheckFilter(NULL, x, 100.0f + y, z, 0.0f, -1.0f, 0.0f, &hx, &hy, &hz, &dist, &nx, &ny, &nz)) {
            if (__fabsf(y - hy) > 30.0) {
                p4 -= 0.1f;
                p5 += 0.1f;
            }
            y = hy;
        }
    }
    {
        Vec pos = vec3_rsh_000136e0;

        pos.x = x;
        pos.y = y;
        pos.z = z;
        entry->target = pos;
    }
    entry->field_0x1C = p4;
    entry->field_0x20 = p5;
    {
        f32 dy = entry->target.y - (*pose->matrix)[1][3];
        f32 d = distABf((*pose->matrix)[0][3], (*pose->matrix)[2][3], entry->target.x, entry->target.z);
        f32 ang = angleABf((*pose->matrix)[0][3], (*pose->matrix)[2][3], entry->target.x, entry->target.z);
        s32 frames;

        frames = (s32)(d / entry->field_0x1C);
        entry->frames = frames;
        entry->field_0x14 = entry->field_0x20 * entry->frames / 2.0f + dy / entry->frames;
        entry->state = 3;
        entry->queuedState = field0x2;
        pose->heading = 180.0f + ang;
    }
    entry->flags |= 2;
    pose->flags |= EXT_POSE_SHADOW_AT_GROUND;
    pose->flags &= ~EXT_POSE_NO_SHADOW;
}
#pragma dont_inline reset

// Matches retail. The sine/cosine coefficients hoist and colour like retail (the pik/evt_sub helper
// shape and the two pragmas). Declaring the six Vec temporaries at function scope instead of
// per-block is load-bearing: with them block-local the pragma pushes the frame to 0x130; at function
// scope it is the target's 0xf0 and every stack slot lines up.
// Reusing the parameter as the sine/cosine half-angle (`angle = halfpi - (angle - halfpi)`) colours
// the reduced angle into the target's register. In the block whose cosine recomputes its argument,
// the sine's argument must NOT be named (the `rad` local is inlined into the call): keeping a `rad`
// local there leaves f5 live and pushes the reduced angle to f1, whereas retail coalesces it into f5.
// Naming the box-delta `d = f31 - f29` once (assigned after the sine, used by both riderDelta lanes)
// fixes the operand order of the two `d * s`/`d * c` fmuls. Giving the pvec's x lane a local `px`
// pins the mpos/pvec lane registers (retail pose x -> f3, z -> f1; without it they swap).
// The first ang/rad block matches by splitting `ang = reviseAngle(...)` from `rad = TWO_PI * ang / 360`
// and inlining `camGetPtr(4)` (check_hammer's shape), and the two `(20+box.x)*0.5` sites match by
// writing `(box.x + 20) / 2` (folded to the same fmuls).
#pragma opt_lifetimes off // keeps the sin/cos coefficients in the target's FPRs
#pragma opt_propagation off // hoist the sin/cos coefficient loads out of the inline branches
static s32 moamoa_hit_check(ExtPose* pose, MoamoaPose* entry) {
    MarioWork* mario = marioGetPtr();
    Vec v65, v55c, vnoko, vparty, mpos, pvec;
    PartyEntry* party;
    f32 f29, f30, f31;

    if (mario->position.y > 20.0f + (*pose->matrix)[1][3])
        return 0;
    if ((*pose->matrix)[1][3] > mario->position.y + mario->wPlayerCollisionBox.y)
        return 0;

    {
        f32 d = distABf((*pose->matrix)[0][3], (*pose->matrix)[2][3], mario->position.x, mario->position.z);

        // (x + 20) / 2 folds to the same * 0.5 but colours the operands like retail.
        if (d < (mario->wPlayerCollisionBox.x + 20.0f) / 2.0f) {
            if (mario->position.y > 10.0f + (*pose->matrix)[1][3] && (mario->flags & 0x20000) &&
                !(mario->flags & 0x10000) && wp->bounceCount < 2) {
                if (mario->currMotionId != 0x16 && mario->currMotionId != 0x1A)
                    marioChgMot(5);
                wp->bounceCount++;
                return 1;
            }
        }
    }

    if ((mario->flags & 0x30000) == 0)
        wp->bounceCount = 0;

    if (mario->currMotionId == MARIO_MOTION_HAMMER && wp->hammerFrames > 10) {
        f32 ang;
        f32 rad;
        f32 s;
        f32 c;
        f32 x;
        f32 z;

        ang = reviseAngle(mario->unk1A8 - 90.0f + camGetPtr(4)->viewYaw);
        rad = 6.2831855f * ang / 360.0f;
        s = moamoa_sin(rad);
        x = 35.0f * s + mario->position.x;
        c = moamoa_cos(rad);
        z = mario->position.z - 35.0f * c;
        if (distABf((*pose->matrix)[0][3], (*pose->matrix)[2][3], x, z) < 35.0f)
            return 2;
    }

    if (mario->currMotionId == 0x13 && (s32)mario->currSubMotionId == 0x14) {
        v65 = moamoa_vec((*pose->matrix)[0][3], (*pose->matrix)[1][3], (*pose->matrix)[2][3],
                             &vec3_rsh_000136ec);

        if (PSVECDistance(&mario->position, &v65) < 65.0f)
            return 2;
    }

    if (wp->breathDir >= 0.0f) {
        v55c = moamoa_vec((*pose->matrix)[0][3], (*pose->matrix)[1][3], (*pose->matrix)[2][3],
                              &vec3_rsh_000136f8);

        if (PSVECDistance(&wp->breathPos, &v55c) < 30.0f)
            return 3;
    }

    party = partyGetPtr(0);
    if (nokonokoGetStatus(party) == 2) {
        vnoko = moamoa_vec((*pose->matrix)[0][3], (*pose->matrix)[1][3], (*pose->matrix)[2][3],
                               &vec3_rsh_00013704);

        if (PSVECDistance(&partyGetPtr(0)->position, &vnoko) < 30.0f)
            return 2;
    }

    party = partyGetPtr(0);
    if (party->currentMemberId == PARTY_MEMBER_BOBBERY && partyGetPtr(0)->useMotionId == 0x46) {
        vparty = moamoa_vec((*pose->matrix)[0][3], (*pose->matrix)[1][3], (*pose->matrix)[2][3],
                                &vec3_rsh_00013710);

        if (PSVECDistance(&partyGetPtr(0)->position, &vparty) < 50.0f)
            return 4;
    }

    {
        mpos = moamoa_vec(mario->position.x, (*pose->matrix)[1][3], mario->position.z,
                              &vec3_rsh_0001371c);
        {
            // Giving the pvec's x lane a name pins the mpos/pvec lane registers
            // (retail pose.x -> f3, pose.z -> f1; without it MWCC loads them the other way round).
            f32 px = (*pose->matrix)[0][3];
            pvec = moamoa_vec(px, (*pose->matrix)[1][3], (*pose->matrix)[2][3], &vec3_rsh_00013728);
        }

        f31 = (mario->wPlayerCollisionBox.x + 20.0f) / 2.0f;
        if (PSVECDistance(&mpos, &pvec) < f31) {
#ifdef MUST_MATCH
            // Match-only volatile accesses: they keep the pvec base copy below the two matrix
            // stores, the order the target has.
            *(volatile f32*)&(*pose->matrix)[0][3] = entry->savedX;
            *(volatile f32*)&(*pose->matrix)[2][3] = entry->savedZ;
            {
                Mtx* pm = *(Mtx* volatile*)&pose->matrix;

                pvec = moamoa_vec_v((*pm)[0][3], (*pm)[1][3], (*pm)[2][3], &vec3_rsh_00013734);

                f29 = PSVECDistance(&mpos, &pvec);
            }
#else
            (*pose->matrix)[0][3] = entry->savedX;
            (*pose->matrix)[2][3] = entry->savedZ;
            {
                pvec = moamoa_vec((*pose->matrix)[0][3], (*pose->matrix)[1][3], (*pose->matrix)[2][3],
                                  &vec3_rsh_00013734);

                f29 = PSVECDistance(&mpos, &pvec);
            }
#endif
            if (f29 < f31) {
                f30 = angleABf(mario->position.x, mario->position.z, (*pose->matrix)[0][3],
                               (*pose->matrix)[2][3]);
                if (__fabsf(compAngle(f30, mario->directionView)) < 40.0f) {
                    // Inlining the sine argument (no `rad` local) lets the reduced
                    // angle coalesce into the argument FPR, and naming the box delta once fixes the
                    // operand order of the two `d * s` / `d * c` fmuls.
                    f32 s, c, d;

                    s = moamoa_sin(6.2831855f * mario->directionView / 360.0f);
                    d = f31 - f29;
                    mario->riderDelta.x = -(d * s);
                    c = moamoa_cos(6.2831855f * mario->directionView / 360.0f);
                    mario->riderDelta.z = d * c;
                } else {
                    f32 rad, s, c;

                    rad = 6.2831855f * f30 / 360.0f;
                    s = moamoa_sin(rad);
                    mario->riderDelta.x = ((*pose->matrix)[0][3] - f31 * s) - mario->position.x;
                    c = moamoa_cos(rad);
                    mario->riderDelta.z = ((*pose->matrix)[2][3] + f31 * c) - mario->position.z;
                }
            }
        }
    }

    return 0;
}

// Calls through the function pointer stay real `bl`s: the original does not inline pose_change/pose_main here.
#define MOAMOA_POSE_CHANGE(e, a, b) (((void (*)(MoamoaPose*, char*, char*))moamoa_pose_change)((e), (a), (b)))
// Keeping both sound vectors in inline scopes reproduces their stack-slot order (as in eki_00).
static inline Vec moaSoundVec(ExtPose* ep, const Vec* base) {
    Vec v = *base;
    v.x = (*ep->matrix)[0][3];
    v.y = (*ep->matrix)[1][3];
    v.z = (*ep->matrix)[2][3];
    return v;
}

#ifdef MUST_MATCH
static inline void moaPlayHit(ExtPose* ep) {
    Vec w = vec3_rsh_00013758;
    w.x = (*ep->matrix)[0][3];
    w.y = (*ep->matrix)[1][3];
    w.z = (*ep->matrix)[2][3];
    psndSFXOn_3D((const char*)0x6C3, &w);
}
#endif
#pragma inline_max_total_size(60000)
// Per-frame update of every moamoa (the ordinary variant, no follower).
static void ext_moamoa_main(void) {
#ifdef MUST_MATCH
    union { const char* p; u32 u; } voiceSfx;
#else
    const char* voiceSfx;
#endif
    MarioWork* mario;
    PartyEntry* party;
    ExtPose* ep;
    MoamoaPose* entry;
    s32 i;
    s32 j;
    s32 handled;
    s32 voicePlayed;
    f32 rad;
    f32 tx;
    f32 ty;
    f32 tz;
    f32 dist;
    f32 f;
    f32 r1;
    s32 res;
    Vec v;
    Vec b;

    mario = marioGetPtr();
    party = partyGetPtr(0);
    camGetPtr(CAMERA_3D);
    handled = 0;
    if (mario->currMotionId == 0x12) {
        wp->hammerFrames++;
    } else {
        wp->hammerFrames = 0;
    }
    if (mario->currMotionId == 0) {
        wp->idleFrames++;
        if (wp->idleFrames > 1000) {
            wp->idleFrames = 1000;
        }
    } else {
        wp->idleFrames = 0;
    }
    if (party->flags & PARTY_FLAG_IS_BEING_USED) {
        wp->flags |= 0x10;
    } else {
        wp->flags &= ~0x10;
    }
    wp->breathDir = -1.0f;
    wp->breathDist = -1.0f;
    if (party->currentMemberId == PARTY_MEMBER_FLURRIE && (party->flags & PARTY_FLAG_IS_BEING_USED)) {
        wp->breathDir = cloudGetBreathDir();
        wp->breathDist = cloudGetBreathDist();
        wp->breathPos.x =
            wp->breathDist * moamoa_sin(6.2831855f * wp->breathDir / 360.0f) + party->position.x;
        wp->breathPos.y = party->position.y;
        wp->breathPos.z = -(wp->breathDist * moamoa_cos(6.2831855f * wp->breathDir / 360.0f) - party->position.z);
    }
    if (wp->nearHits[0] > 0x32) {
        wp->targets[0] = wp->targets[1];
        if (!(wp->flags & 0x1000)) {
            wp->flags |= 0x1000;
            evtEntry(evt_moamoa_escape0, 0, 0);
        }
    }
    if (wp->nearHits[1] > 0x32) {
        wp->targets[1] = wp->targets[0];
        if (!(wp->flags & 0x2000)) {
            wp->flags |= 0x2000;
            evtEntry(evt_moamoa_escape1, 0, 0);
        }
    }
    if ((wp->flags & 0x1000) && (wp->flags & 0x2000)) {
        MoamoaPose* p;

        extGetPosePtr();
        for (j = 0, p = wp->poses; j < wp->poseNum; j++, p++) {
            if (!(p->flags & 0x4)) {
                if (p->flags & 0x10) {
                    break;
                }
                p->state = 0x19;
                p->stateTimer = 0;
                p->flags |= 0x10;
            }
        }
        if (wp->escapeEvt == 0) {
            wp->escapeEvt = (u32)evtEntry(evt_moamoa_escape, 0, 0);
        }
    }
    ep = extGetPosePtr();
    voicePlayed = 0;
    for (i = 0, entry = wp->poses; i < wp->poseNum; i++, ep++, entry++) {
#ifdef MUST_MATCH
        voiceSfx.p = "SFX_STG6_MOAMOA_VOICE1";
#else
        voiceSfx = "SFX_STG6_MOAMOA_VOICE1";
#endif
        wp->angleToMario = angleABf(mario->position.x, mario->position.z, (*ep->matrix)[0][3], (*ep->matrix)[2][3]);
        wp->distToMario = distABf(mario->position.x, mario->position.z, (*ep->matrix)[0][3], (*ep->matrix)[2][3]);
        if ((wp->flags & 0x1) && !(wp->flags & 0x4) && marioKeyOffChk() == 0 && marioCtrlOffChk() == 0
            && handled == 0 && !(entry->flags & 0x154)) {
            res = moamoa_hit_check(ep, entry);
            if (res == 1) {
                entry->flags |= 0x4;
                entry->state = 0x24;
                wp->hitCount++;
                if (distABf((*ep->matrix)[0][3], (*ep->matrix)[2][3], wp->targets[i % 3].x, wp->targets[i % 3].z) < 200.0f) {
                    wp->nearHits[i % 3]++;
                }
                MOAMOA_POSE_CHANGE(entry, "c_n_moa_a", "D_1");
                handled = 1;
            } else if (res == 2 || res == 4) {
                entry->flags |= 0xC;
                entry->state = 0x2F;
                wp->hitCount++;
                if (distABf((*ep->matrix)[0][3], (*ep->matrix)[2][3], wp->targets[i % 3].x, wp->targets[i % 3].z) < 200.0f) {
                    wp->nearHits[i % 3]++;
                }
                MOAMOA_POSE_CHANGE(entry, "c_n_moa_a", "D_1");
                if (res != 4) {
                    handled = 1;
                }
            } else if (res == 3) {
                f = (wp->breathDir + wp->angleToMario) / 2.0f;
                entry->flags |= 0x84;
                entry->state = 0x30;
                entry->target = vec3_rsh_00013740;
                dist = randf(0.1f, 0.2f);
                rad = 6.2831855f * f / 360.0f;
                entry->field_0x14 = dist * moamoa_sin(rad);
                entry->field_0x1C = randf(0.03f, 0.06f);
                entry->field_0x20 = dist * -moamoa_cos(rad);
                entry->frames = 0x78;
                wp->hitCount++;
                if (distABf((*ep->matrix)[0][3], (*ep->matrix)[2][3], wp->targets[i % 3].x, wp->targets[i % 3].z) < 200.0f) {
                    wp->nearHits[i % 3]++;
                }
                MOAMOA_POSE_CHANGE(entry, "c_n_moa_a", "D_1");
                handled = 1;
            }
        }
        if (wp->flags & 0x1) {
            if (!(entry->flags & 0x155) && wp->distToMario < 50.0f) {
                if (entry->flags & 0x2) {
                    entry->queuedState = 1;
                } else {
                    entry->state = 1;
                }
            }
        }
        if ((wp->flags & 0x1) && !(entry->flags & 0x444) && (u32)i == gp->retraceCount % wp->poseNum
            && irand(1000) < 200) {
            v = moaSoundVec(ep, &vec3_rsh_0001374c);
#ifdef MUST_MATCH
            psndSFXOn_3D((const char*)(u32)voiceSfx.p, &v);
#else
            psndSFXOn_3D(voiceSfx, &v);
#endif
        }
        switch (entry->state) {
        case 0x0:
            if (i % 133 < 100) {
                f32 dist, tx, ntx, ntz, ty, tz, rad, ar, br;
                f32 ang = randf(0.0f, 6.2831855f);
                f32 len = randf(0.0f, 50.0f);

                ntx = len * moamoa_sin(ang) + wp->targets[i % 3].x;
                ntz = -(len * moamoa_cos(ang) - wp->targets[i % 3].z);
                rad = 6.2831855f * angleABf((*ep->matrix)[0][3], (*ep->matrix)[2][3], ntx, ntz) / 360.0f;
                tx = 15.0f * moamoa_sin(rad) + (*ep->matrix)[0][3];
                ty = (*ep->matrix)[1][3];
                tz = -(15.0f * moamoa_cos(rad) - (*ep->matrix)[2][3]);
                ar = (f32)rand() / 32767.0f;
                br = (f32)rand() / 32767.0f;
                moamoa_jump(ep, entry, tx, ty, tz, ar * randRange(1.5f, 3.0f) + 1.5f, br * randRange(0.8f, 2.0f) + 0.8f, entry->state, 1);
                entry->flags |= 1;
            } else {
                f32 dist, rad, ang, tz, ar, br;

                dist = distABf((*ep->matrix)[0][3], (*ep->matrix)[2][3], wp->targets[i % 3].x, wp->targets[i % 3].z);
                ang = angleABf((*ep->matrix)[0][3], (*ep->matrix)[2][3], wp->targets[i % 3].x, wp->targets[i % 3].z);
                if (dist < 200.0f) {
                    ang = randf(0.0f, 360.0f);
                }
                rad = 6.2831855f * ang / 360.0f;
                tx = 5.0f * moamoa_sin(rad) + (*ep->matrix)[0][3];
                ty = (*ep->matrix)[1][3];
                tz = -(5.0f * moamoa_cos(rad) - (*ep->matrix)[2][3]);
                ar = (f32)rand() / 32767.0f;
                br = (f32)rand() / 32767.0f;
                moamoa_jump(ep, entry, tx, ty, tz, ar * randRange(0.3f, 0.5f) + 0.3f, br * randRange(0.5f, 0.8f) + 0.5f, entry->state, 1);
                entry->flags &= ~1;
            }
            break;
        case 0x32:
            if (--entry->stateTimer < 0) {
                entry->stateTimer = 0;
                entry->state++;
            }
            break;
        case 0x33: {
            f32 rad, tx, f, dist, ty;

            entry->field_0x1C = randf(0.0f, 200.0f);
            entry->field_0x20 = randf(0.0f, 300.0f * entry->field_0x1C / 200.0f);
            entry->state++;
            entry->stateTimer = 0;
            /* fallthrough */
        case 0x34: {
            // The branch keeps its result in a temp and copies it to f at the join (as in eki_00).
            f32 ft;

            if (entry->stateTimer < 200) {
                ft = 1.0f;
            } else {
                ft = (f32)(800 - entry->stateTimer) / 600.0f;
            }
            f = ft;
            if (ft < 0.0f) {
                f = 0.0f;
                entry->flags |= 0x200;
            }
            tx = wp->targets[i % 3].x;
            ty = wp->targets[i % 3].y;
            tz = wp->targets[i % 3].z;
            dist = entry->field_0x20 * (f32)sin((f32)(i + gp->retraceCount) / 4.0f);
            ty = entry->field_0x1C * f + ty;
            tx = f * dist + tx;
            tz = f * (-entry->field_0x20 * (f32)cos((f32)(i + gp->retraceCount) / 4.0f)) + tz;
            (*ep->matrix)[0][3] += (tx - (*ep->matrix)[0][3]) / 15.0f;
            (*ep->matrix)[1][3] += (ty - (*ep->matrix)[1][3]) / 15.0f;
            (*ep->matrix)[2][3] += (tz - (*ep->matrix)[2][3]) / 15.0f;
            entry->stateTimer++;
            break;
        }
        }
        case 0x1: {
            f32 dist, rad;

            {
                f32 rr, t;

                t = (f32)rand() / 32767.0f;
                rr = wp->angleToMario + (t * randRange(-45.0f, 45.0f) + -45.0f);
                rad = 6.2831855f * rr / 360.0f;
            }
            dist = wp->distToMario;
            tx = 15.0f * moamoa_sin(rad) + (*ep->matrix)[0][3];
            ty = (*ep->matrix)[1][3];
            tz = -(15.0f * moamoa_cos(rad) - (*ep->matrix)[2][3]);
            {
                f32 a = (f32)rand() / 32767.0f * randRange(1.0f, 1.0f) + 1.0f;
                f32 b = randf(0.4f, 0.4f);

                moamoa_jump(ep, entry, tx, ty, tz, a, b, (dist < 100.0f) ? entry->state : 0, 1);
            }
            break;
        }
        case 0x2: {
            f32 r1;

            {
                f32 rr;

                r1 = (f32)rand() / 32767.0f;
                rr = wp->angleToMario + (r1 * randRange(-45.0f, 45.0f) + -45.0f);
                rr += 180.0f;
                dist = wp->distToMario;
                tx = 30.0f * moamoa_sin(6.2831855f * rr / 360.0f) + (*ep->matrix)[0][3];
            }
            ty = (*ep->matrix)[1][3];
            tz = (*ep->matrix)[2][3];
            {
                f32 t, b;

                // The first jump speed is kept in the float temporary f across the second rand().
                t = (f32)rand() / 32767.0f;
                f = 2.0f * (t * randRange(1.0f, 1.0f) + 1.0f);
                t = (f32)rand() / 32767.0f;
                b = 2.0f * (t * randRange(0.4f, 0.4f) + 0.4f);
                moamoa_jump(ep, entry, tx, ty, tz, f, b, (dist < 100.0f) ? entry->state : 0, 1);
            }
            break;
        }
        case 0x3: {
            f32 rad;

            (*ep->matrix)[1][3] += entry->field_0x14;
            entry->field_0x14 -= entry->field_0x20;
            rad = 6.2831855f * angleABf((*ep->matrix)[0][3], (*ep->matrix)[2][3], entry->target.x, entry->target.z) /
                   360.0f;
            (*ep->matrix)[0][3] = entry->field_0x1C * moamoa_sin(rad) + (*ep->matrix)[0][3];
            (*ep->matrix)[2][3] = entry->field_0x1C * -moamoa_cos(rad) + (*ep->matrix)[2][3];
            if (--entry->frames < 0) {
                (*ep->matrix)[0][3] = entry->target.x;
                (*ep->matrix)[1][3] = entry->target.y;
                (*ep->matrix)[2][3] = entry->target.z;
                entry->flags &= ~0x2;
                ep->flags &= ~EXT_POSE_SHADOW_AT_GROUND;
                entry->stateTimer = 0;
                entry->state++;
            }
            break;
        }
        case 0x4:
            if (++entry->stateTimer > 4) {
                entry->state = entry->queuedState;
            }
            break;
        case 0x24:
        case 0x25:
        case 0x26: {
            f32 px, pz, tz, rad;

            if (!(entry->flags & 0x1)) {
                px = mario->position.x;
                pz = mario->position.z;
            } else {
                px = wp->targets[i % 3].x;
                pz = wp->targets[i % 3].z;
            }
            r1 = (f32)rand() / 32767.0f;
            {
                f32 rr = angleABf(px, pz, (*ep->matrix)[0][3], (*ep->matrix)[2][3]);

                rr = rr + randScale(r1, -45.0f, 45.0f);
                rad = 6.2831855f * rr / 360.0f;
            }
            tx = 15.0f * moamoa_sin(rad) + (*ep->matrix)[0][3];
            ty = (*ep->matrix)[1][3];
            tz = -(15.0f * moamoa_cos(rad) - (*ep->matrix)[2][3]);
            {
                f32 a, b;

                a = (f32)rand() / 32767.0f;
                b = (f32)rand() / 32767.0f;
                moamoa_jump(ep, entry, tx, ty, tz, a * randRange(1.0f, 1.0f) + 1.0f, b * randRange(1.0f, 1.0f) + 1.0f, entry->state + 1, 1);
            }
            break;
        }
        case 0x27:
            if (++entry->stateTimer < 0x1E) {
                (*ep->matrix)[0][0] *= 0.9f;
                (*ep->matrix)[1][1] *= 0.9f;
                (*ep->matrix)[2][2] *= 0.9f;
            } else {
                entry->state = 0x31;
            }
            break;
        case 0x2F: {
            f32 rad, dist, f, grav;

            // The jump's y is the -100 literal, so the float temporary ty carries its z target (retail's register).
            r1 = (f32)rand() / 32767.0f;
            dist = randf(300.0f, 500.0f);
            rad = 6.2831855f * (r1 * randRange(135.0f, 225.0f) + 135.0f) / 360.0f;
            tx = dist * moamoa_sin(rad) + (*ep->matrix)[0][3];
            ty = -(dist * moamoa_cos(rad) - (*ep->matrix)[2][3]);
            f = (f32)rand() / 32767.0f * randRange(5.0f, 6.0f) + 5.0f;
            grav = (f32)rand() / 32767.0f * randRange(0.2f, 0.3f) + 0.2f;
            if (voicePlayed == 0) {
#ifdef MUST_MATCH
                moaPlayHit(ep);
#else
                Vec w;

                w = vec3_rsh_00013758;
                w.x = (*ep->matrix)[0][3];
                w.y = (*ep->matrix)[1][3];
                w.z = (*ep->matrix)[2][3];
                psndSFXOn_3D((const char*)0x6C3, &w);
#endif
                voicePlayed = 1;
            }
            moamoa_jump(ep, entry, tx, -100.0f, ty, f, grav, 0x31, 0);
            break;
        }
        case 0x30:
            if (--entry->frames > 0) {
                entry->target.x += entry->field_0x14;
                entry->target.y += entry->field_0x1C;
                entry->target.z += entry->field_0x20;
                (*ep->matrix)[0][3] += entry->target.x;
                (*ep->matrix)[1][3] += entry->target.y;
                (*ep->matrix)[2][3] += entry->target.z;
            } else {
                entry->state = 0x31;
            }
            break;
        case 0x31:
            if ((wp->flags & 0x1000) && (wp->flags & 0x2000)) {
                entry->state = 0x3E;
            } else if (wp->hitCount < 500) {
                PSMTXIdentity(*ep->matrix);
                (*ep->matrix)[0][3] = wp->targets[i % 3].x;
                (*ep->matrix)[1][3] = wp->targets[i % 3].y;
                (*ep->matrix)[2][3] = wp->targets[i % 3].z;
                entry->flags = 0;
                entry->state = 0;
                entry->stateTimer = 0;
            } else {
                entry->state = 0x3E;
            }
            break;
        case 0x19:
            if (entry->stateTimer < 0x1E) {
                Mtx rot;

                PSMTXRotRad(rot, 'y', 0.20943952f);
                PSMTXConcat(*ep->matrix, rot, *ep->matrix);
                ep->flags &= ~EXT_POSE_BILLBOARD;
                entry->stateTimer++;
            } else {
                ep->flags |= EXT_POSE_BILLBOARD;
                entry->stateTimer = rand() % 30;
                entry->state++;
            }
            break;
        case 0x1A:
            if (--entry->stateTimer < 0) {
                entry->stateTimer = 0;
                entry->state++;
            }
            break;
        case 0x1B: {
            f32 dist1b, ang1b, rad1b, z1b;

            // Reuses the function's float temporaries (tz: the angle fraction, tx: the y, ty: the x), which gives
            // retail's registers.
            dist1b = distABf((*ep->matrix)[0][3], (*ep->matrix)[2][3], -275.0f, -100.0f);
            tz = (f32)rand() / 32767.0f;
            ang1b = angleABf((*ep->matrix)[0][3], (*ep->matrix)[2][3], -275.0f, -100.0f) + (tz * randRange(-45.0f, 45.0f) + -45.0f);
            if (dist1b > 6.0f && (tx = (*ep->matrix)[1][3]) < 60.0f) {
                rad1b = 6.2831855f * ang1b / 360.0f;
                ty = 15.0f * moamoa_sin(rad1b) + (*ep->matrix)[0][3];
                z1b = -(15.0f * moamoa_cos(rad1b) - (*ep->matrix)[2][3]);
                {
                    f32 b;

                    r1 = (f32)rand() / 32767.0f;
                    b = (f32)rand() / 32767.0f;
                    moamoa_jump(ep, entry, ty, tx, z1b, 3.0f * (r1 * randRange(1.0f, 1.0f) + 1.0f), b * randRange(1.0f, 1.0f) + 1.0f, entry->state, 1);
                }
            } else {
                entry->state++;
            }
            break;
        }
        case 0x1C:
            if ((*ep->matrix)[0][3] > -350.0f) {
                f32 tz, rad;

                r1 = (f32)rand() / 32767.0f;
                rad = 6.2831855f * (angleABf((*ep->matrix)[0][3], (*ep->matrix)[2][3], -350.0f, -100.0f) +
                                    (r1 * randRange(-45.0f, 45.0f) + -45.0f)) /
                        360.0f;
                tx = 15.0f * moamoa_sin(rad) + (*ep->matrix)[0][3];
                ty = (*ep->matrix)[1][3];
                tz = -(15.0f * moamoa_cos(rad) - (*ep->matrix)[2][3]);
                {
                    f32 a, b;

                    a = (f32)rand() / 32767.0f;
                    b = (f32)rand() / 32767.0f;
                    moamoa_jump(ep, entry, tx, ty, tz, 3.0f * (a * randRange(1.0f, 1.0f) + 1.0f), b * randRange(1.0f, 1.0f) + 1.0f, entry->state, 1);
                }
            } else {
                entry->state = 0x3E;
            }
            break;
        case 0x3E:
            entry->flags |= 0x20;
            ep->flags &= ~EXT_POSE_VISIBLE;
            ep->flags |= EXT_POSE_NO_SHADOW;
            break;
        case 0x3F: {
            r1 = (f32)rand() / 32767.0f;
            f = randf(2.0f, 3.0f);
            entry->stateTimer = irand(i / 2);
            rad = 6.2831855f * (r1 * randRange(225.0f, 315.0f) + 225.0f) / 360.0f;
            entry->target.x = f * moamoa_sin(rad);
            entry->target.y = randf(1.0f, 2.0f);
            entry->target.z = f * -moamoa_cos(rad);
            entry->state++;
            break;
        }
        case 0x40:
            if (--entry->stateTimer < 0) {
                entry->state++;
            }
            break;
        case 0x41: {
            Mtx rot41;

            entry->target.x *= 1.1f;
            entry->target.y *= 1.02f;
            entry->target.z *= 1.1f;
            (*ep->matrix)[0][3] += entry->target.x;
            (*ep->matrix)[1][3] += entry->target.y;
            (*ep->matrix)[2][3] += entry->target.z;
            PSMTXRotRad(rot41, 'y', 0.20943952f);
            PSMTXConcat(*ep->matrix, rot41, *ep->matrix);
            ep->flags &= ~EXT_POSE_BILLBOARD;
            if ((*ep->matrix)[0][3] < -1000.0f) {
                entry->state = 0x3E;
            }
            break;
        }
        }
        if (!(entry->flags & 0x98)) {
            if ((*ep->matrix)[0][3] > 290.0f) {
                (*ep->matrix)[0][3] = 290.0f;
            }
            if ((*ep->matrix)[0][3] < -290.0f) {
                (*ep->matrix)[0][3] = -290.0f;
            }
            if ((*ep->matrix)[2][3] > 50.0f) {
                (*ep->matrix)[2][3] = 50.0f;
            }
            if ((*ep->matrix)[2][3] < -130.0f) {
                (*ep->matrix)[2][3] = -130.0f;
            }
        }
        (*ep->matrix)[1][3] += *(f32*)((u8*)wp->train + 0x38) - wp->prevTrainY;
        entry->savedX = (*ep->matrix)[0][3];
        entry->savedZ = (*ep->matrix)[2][3];
    }
    ((void (*)(void))moamoa_pose_main)();
    wp->prevTrainY = *(f32*)((u8*)wp->train + 0x38);
    if (wp->flags & 0x10) {
        mario->riderDelta.x = 0.0f;
        mario->riderDelta.y = 0.0f;
        mario->riderDelta.z = 0.0f;
        return;
    }
    {
        // Declaration order sets the float registers; assignment order sets the load schedule (as in eki_00).
        f32 dz, dy, dx;
        dx = mario->riderDelta.x;
        dy = mario->riderDelta.y;
        dz = mario->riderDelta.z;

        b = moamoa_vec(dx, dy, dz, &vec3_rsh_00013764);
    }
    if (PSVECMag(&b) > 2.0f) {
        PSVECNormalize(&b, &b);
        mario->riderDelta.x = 2.0f * b.x;
        mario->riderDelta.y = 2.0f * b.y;
        mario->riderDelta.z = 2.0f * b.z;
    }
}
#pragma inline_max_total_size reset
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

// Hops an npc to (x, y, z) over a frame count: userdata holds the frames left, the vertical velocity and the speed.
static USER_FUNC(npc_jump) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    f32 speed = evtGetFloat(event, args[4]);
    f32 grav = evtGetFloat(event, args[5]);
    s32 ms = evtGetValue(event, args[6]);
    f32 frames = 60.0f * (f32)ms / 1000.0f;
    NpcEntry* npc = npcNameToPtr(name);
    f32 vy;
    f32 rad;

    if (isFirstCall) {
        Vec to;
        Vec from;
        f32 dist;
        f32 dy;

        from = vec3_rsh_00013788;
        from.x = npc->position.x;
        from.z = npc->position.z;
        to = vec3_rsh_0001377c;
        to.x = x;
        to.z = z;
        dist = PSVECDistance(&to, &from);
        dy = y - npc->position.y;
        if (0.0f == frames) {
            frames = dist / speed;
        } else {
            speed = dist / frames;
        }
        event->userdata[0] = *(s32*)&frames;
        vy = dy / frames + grav * frames / 2.0f;
        event->userdata[1] = *(s32*)&vy;
        event->userdata[2] = *(s32*)&speed;
    }
    frames = event->userdataf[0];
    vy = event->userdataf[1];
    speed = event->userdataf[2];
    rad = 6.2831855f * angleABf(npc->position.x, npc->position.z, x, z) / 360.0f;
    npc->position.x = speed * moamoa_sin(rad) + npc->position.x;
    npc->position.z = -(speed * moamoa_cos(rad) - npc->position.z);
    npc->position.y += vy;
    vy -= grav;
    frames -= 1.0f;
    if (frames < 0.0f) {
        Vec land;

        land = vec3_rsh_00013794;
        land.x = x;
        land.y = y;
        land.z = z;
        npc->position = land;
        return EVT_RETURN_YIELD;
    }
    event->userdata[0] = *(s32*)&frames;
    event->userdata[1] = *(s32*)&vy;
    return EVT_RETURN_BLOCK;
}
// clang-format on
