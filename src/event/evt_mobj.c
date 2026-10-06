#ifdef MUST_MATCH
// Retail pools function literals relative to the unit read-only data base.
#pragma pool_data on
#endif

#include "mario/mario_motion.h"
#include "math_ext.h"
#include "party.h"
#include "mario/mario_motion.h"
#include "pmario_sound.h"
#include "party.h"
#include "driver/mobjdrv.h"
#include "driver/animdrv.h"
#include "driver/hitdrv.h"
#include "driver/msgdrv.h"
#include "driver/windowdrv.h"
#include "driver/dispdrv.h"
#include "driver/itemdrv.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "manager/fontmgr.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "memory.h"
#include <string.h>
#include "koopa.h"
#include "driver/npcdrv.h"
#include "system.h"
#include "mario/mario_party.h"
#include "evt/evt_mobj.h"
#include "kpa_act.h"
#include "koopa_motion.h"
#include "eff/eff_fire.h"
#include "eff/eff_small_star.h"
#include "eff/eff_mobj_broken.h"
#include "eff/eff_batfukidashi.h"
#include "eff/eff_stardust.h"
#include "driver/offscreendrv.h"
#include "driver/imgdrv.h"
#include "driver/swdrv.h"
#include "statuswindow.h"
#include "evt/evt_img.h"
#include "evt/evt_snd.h"
#include "evt/evt_mario.h"
#include "evt/evt_cam.h"
#include "evt/evt_msg.h"
#include "evt/evt_memcard.h"
#include "evt/evt_sub.h"
#include "evt/evt_map.h"
#include "party/party_motion.h"
#include <dolphin/mtx.h>

extern Vec mobj_vec3_802c4050_template[];
extern Vec mobj_vec3_802c405c_template[];
extern Vec mobj_vec3_802c40b8_template[];
static const Vec vec3_802c3fd8[1];
static const Vec vec3_802c3fe4[1];
extern Vec vec3_802c4020[];
extern Vec vec3_802c402c[];
extern Vec vec3_802c4038[];
extern Vec vec3_802c4044[];
extern const char str_802C3EC8[];
extern const char str_802C3ED8[];
#ifdef MUST_MATCH
// A typed aggregate copy emits the original four word loads and stores.
typedef struct MobjNameTemplate {
    u32 words[4];
} MobjNameTemplate;
#endif
extern Vec lv_block_position[];

// Data objects defined later in this file or in other units, declared before their first use.
extern const char str_802C3DE0[];
extern const char str_802C3DF8[];
extern const char str_802C3E68[];
extern const char str_802C3E7C[];
extern const char str_802C3E98[];
extern const char str_802C3EAC[];

// Prototypes for functions in other, not-yet-decompiled units (koopa / hit / etc.)

// Read-only constants defined in this unit's .rodata (to be defined locally later)
extern f32 float_0_8041a150;
extern f32 float_12p5_8041a178;
extern f32 float_1_8041a160;
extern f32 float_0p5_8041a15c;
extern f32 float_neg1_8041a214;
extern f32 float_10_8041a1a4;
extern f32 float_0p8_8041a1b0;
extern f32 float_0p125_8041a1b4;
extern f32 float_0p99_8041a1b8;
extern const f32 float_1p4_8041a208;
extern const f32 float_1p2_8041a20c;
extern f32 float_5_8041a210;
extern Vec vec3_802c3dc8[];
extern Vec vec3_802c3dd4[];
extern Vec vec3_802c40ac[];
extern Vec vec3_802c40b8[];
extern Vec vec3_802c40c4[];
extern Vec vec3_802c4050[];
extern Vec vec3_802c405c[];
extern u32 dat_8041a134;
extern u32 dat_8041a130;
extern f32 float_0p45_8041a1c4;
extern f32 float_0p7_8041a1c8;
extern f32 float_500_8041a1cc;
extern f32 float_120_8041a1d0;
extern f32 float_25_8041a140;
extern f32 float_7p5_8041a1bc;
extern f32 float_40_8041a1ec;
extern f32 float_32767_8041a1e4;

extern const char str_8041A12C[4];
extern s32 evt_lock_sound[];
extern f32 float_1024_8041a1a0;
extern const f32 float_neg0p8_8041a220;
extern const f32 float_4_8041a224;
extern f32 float_30_8041a1a8;
extern f32 float_60_8041a1ac;
extern f32 float_1E05_8041a1d8;
extern f32 float_42p42_8041a1dc;
extern f32 float_0p6_8041a1e0;
extern f32 float_300_8041a158;
extern f32 float_2_8041a190;

// Prototypes missing from manager/fontmgr.h


// Prototypes missing from driver/mobjdrv.h (to move to the header later)

// Prototypes missing from mario/party/sound headers (to move later)

// ---------------------------------------------------------------------------
// Pole score work (sbss `psw`).  The first bytes are the ASCII score string,
// then three floats holding the on-screen position.
// ---------------------------------------------------------------------------
typedef struct PoleWork {
    char str[4];    // 0x0
    u8 unk4;        // 0x4, string NULL terminator
    u8 unk5[3];     // 0x5
    f32 x;          // 0x8
    f32 y;          // 0xC
    f32 z;          // 0x10
} PoleWork;

PoleWork* psw;

// trailing zero word after psw in the unit's .sbss (declared first: MWCC lays .sbss out last-declared-first)
#pragma force_active on
u32 gap_08_80418714_sbss;
#pragma force_active reset

// Anchor for the .data addresses below (kpa_pole and friends live at fixed offsets past it).
#ifdef MUST_MATCH
__declspec(section ".data") static f32 negone_one_374[2] = { -1.0f, 1.0f };
#endif

#ifdef MUST_MATCH
// MUST_MATCH: retail reads these objects through the vec3_802c3dc8 / negone_one_374 base registers.
#define IB_R(off, sym, val) ((const char*)(rbase + (off)))
#define IB_D(off, sym) ((s32*)(dbase + (off)))
#define IB_VEC(off, sym, val) (*(Vec*)(rbase + (off)))
#define IB_F64(off, sym, val) (*(f64*)(rbase + (off)))
#else
#define IB_R(off, sym, val) (val)
#define IB_D(off, sym) ((s32*)(sym))
#define IB_VEC(off, sym, val) (val)
#define IB_F64(off, sym, val) (val)
#endif
// .rodata (part A)
#pragma force_active on
// vec3_802c3dc8: placeholder for the object the code copies (defining it hangs MWCC)
__declspec(section ".rodata") Vec mobj_vec3_802c3dc8_template[1] = {{0.0f, 0.0f, 0.0f}};
// vec3_802c3dd4: placeholder for the object the code copies (defining it hangs MWCC)
__declspec(section ".rodata") Vec mobj_vec3_802c3dd4_template[1] = {{0.0f, 0.0f, 0.0f}};
#pragma force_active reset
const char str_802C3DE0[0x18] = "MOBJ_BlueTimerSwitch";
const char str_802C3DF8[0x14] = "MOBJ_RedTimerSwitch";
static u32 evt_shake[] = {
    0x0005005B, (u32)evt_cam_shake, 0x00000004, 0xF24A7A80,
    0xF24A7A8A, 0x000000C8, 0x00000002, 0x00000001,
};
extern s32 mobj_save_blk_sysevt[];
extern Vec vec3_802c40a0[];
extern Vec vec3_802c3ef4[];
#ifdef MUST_MATCH
// Copy all 16 template bytes; strcpy is sufficient in the natural build.
#define LVB_TEMPLATE(dst, src) (*(MobjNameTemplate*)(dst) = *(const MobjNameTemplate*)(src))
#else
#define LVB_TEMPLATE(dst, src) strcpy((dst), (src))
#endif
extern const char str_8041A11C[4];
s32 psndSFXOnEx_3D(const char* name, Vec* position, u8 vol, u8 pan, u16 pitch, u16 flags);
extern const char* _switch_anim[6];
extern const char str_8041A120[4];

s32 evt_mobj_entry(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    const char* animName;

    name = (const char*)evtGetValue(event, args[0]);
    animName = (const char*)evtGetValue(event, args[1]);
    if (!animGroupBaseAsync(animName, gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, animName);
    return 2;
}

s32 evt_mobj_delete(EventEntry* event, BOOL isFirstCall) {
    mobjDelete((const char*)evtGetValue(event, event->args[0]));
    return 2;
}

s32 evt_mobj_check(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;

    name = (const char*)evtGetValue(event, args[0]);
    evtSetValue(event, args[1], (s32)mobjNameToPtrNoAssert(name));
    return 2;
}

s32 evt_mobj_flag_onoff(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 on;
    const char* name;
    s32 flag;
    MapObjectEntry* obj;

    on = evtGetValue(event, args[0]);
    name = (const char*)evtGetValue(event, args[1]);
    flag = args[2];
    obj = mobjNameToPtr(name);

    if (on != 0) {
        obj->flags |= flag;
    } else {
        obj->flags &= ~flag;
    }
    return 2;
}

s32 evt_mobj_get_kindname(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;

    name = (const char*)evtGetValue(event, args[0]);
    obj = mobjNameToPtr(name);
    evtSetValue(event, args[1], (s32)obj->animName);
    return 2;
}

s32 evt_mobj_set_scale(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    f32 x;
    f32 y;
    f32 z;
    Vec scale;

    name = (const char*)evtGetValue(event, args[0]);
    if (strcmp(name, "me") == 0) {
        obj = event->thisMapObj;
    } else {
        obj = mobjNameToPtr(name);
    }
    x = evtGetFloat(event, args[1]);
    y = evtGetFloat(event, args[2]);
    z = evtGetFloat(event, args[3]);
    scale = vec3_802c3dc8[0];
    scale.x = x;
    scale.y = y;
    scale.z = z;
    obj->scale = scale;
    obj->flags |= 4;
    return 2;
}

s32 evt_mobj_get_position(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;

    name = (const char*)evtGetValue(event, args[0]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    evtSetFloat(event, args[1], obj->position.x);
    evtSetFloat(event, args[2], obj->position.y);
    evtSetFloat(event, args[3], obj->position.z);
    return 2;
}

s32 evt_mobj_get_x_position(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;

    name = (const char*)evtGetValue(event, args[0]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    evtSetFloat(event, args[1], obj->position.x);
    return 2;
}

s32 evt_mobj_get_y_position(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;

    name = (const char*)evtGetValue(event, args[0]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    evtSetFloat(event, args[1], obj->position.y);
    return 2;
}

s32 evt_mobj_get_z_position(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;

    name = (const char*)evtGetValue(event, args[0]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    evtSetFloat(event, args[1], obj->position.z);
    return 2;
}

s32 evt_mobj_set_position(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    f32 x;
    f32 y;
    f32 z;
    Vec pos;

    name = (const char*)evtGetValue(event, args[0]);
    x = evtGetFloat(event, args[1]);
    y = evtGetFloat(event, args[2]);
    z = evtGetFloat(event, args[3]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    pos = vec3_802c3dd4[0];
    pos.x = x;
    pos.y = y;
    pos.z = z;
    obj->position = pos;
    obj->flags |= 4;
    return 2;
}

s32 evt_mobj_set_x_position(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    Vec pos;

    name = (const char*)evtGetValue(event, args[0]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    pos = obj->position;
    pos.x = evtGetFloat(event, args[1]);
    obj->position = pos;
    obj->flags |= 4;
    return 2;
}

s32 evt_mobj_set_y_position(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    Vec pos;

    name = (const char*)evtGetValue(event, args[0]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    pos = obj->position;
    pos.y = evtGetFloat(event, args[1]);
    obj->position = pos;
    obj->flags |= 4;
    return 2;
}

s32 evt_mobj_set_z_position(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    Vec pos;

    name = (const char*)evtGetValue(event, args[0]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    pos = obj->position;
    pos.z = evtGetFloat(event, args[1]);
    obj->position = pos;
    obj->flags |= 4;
    return 2;
}

s32 evt_mobj_exec_cancel(EventEntry* event, BOOL isFirstCall) {
    const char* name;
    MapObjectEntry* obj;

    name = (const char*)evtGetValue(event, event->args[0]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    obj->flags |= 0x1000;
    return 2;
}

s32 evt_mobj_set_gravity_bound(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    f32 f1;
    f32 f2;

    name = (const char*)evtGetValue(event, args[0]);
    f1 = evtGetFloat(event, args[1]);
    f2 = evtGetFloat(event, args[2]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    obj->unk68 = f1;
    obj->unk6C = f2;
    return 2;
}

s32 evt_mobj_set_anim(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    const char* animName;
    MapObjectEntry* obj;

    name = (const char*)evtGetValue(event, args[0]);
    animName = (const char*)evtGetValue(event, args[1]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    animPoseSetAnim(obj->poseId, animName, 1);
    return 2;
}

s32 evt_mobj_wait_animation_end(EventEntry* event, BOOL isFirstCall) {
    const char* name;
    MapObjectEntry* obj;

    name = (const char*)evtGetValue(event, event->args[0]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    if (animPoseGetLoopTimes(obj->poseId) > 1.0f) {
        return 2;
    }
    return 0;
}

s32 evt_mobj_set_camid(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    u8 camId;

    name = (const char*)evtGetValue(event, args[0]);
    camId = (u8)evtGetValue(event, args[1]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    obj->camId = camId;
    return 2;
}

s32 evt_mobj_hitevt_onoff(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    s32 on;

    name = (const char*)evtGetValue(event, args[0]);
    on = evtGetValue(event, args[1]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }

    if (on != 0) {
        obj->flags &= ~0x10;
    } else {
        obj->flags |= 0x10;
    }
    return 2;
}

// ---------------------------------------------------------------------------
// Generic MOBJ event commands
// ---------------------------------------------------------------------------

s32 evt_mobj_hit_onoff(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    s32 on;
    s32 i;

    name = (const char*)evtGetValue(event, args[0]);
    on = evtGetValue(event, args[1]);
    obj = mobjNameToPtr(name);

    if (on != 0) {
        for (i = 0; i < 2; i++) {
            if (obj->hitObj[i].name != NULL) {
                hitGrpFlagOff(obj->hitObj[i].name, 1);
            }
        }
    } else {
        for (i = 0; i < 2; i++) {
            if (obj->hitObj[i].name != NULL) {
                hitGrpFlagOn(obj->hitObj[i].name, 1);
            }
        }
    }
    return 2;
}

s32 mobj_switch_blue(MapObjectEntry* obj) {
    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                mobjRunEvent(obj, obj->unk1D4);
                if (obj->unk1E0 == 0x200000 || obj->unk1E0 == 0x1000000) {
                    marioChgMot(5);
                }
                psndSFXOn_3D((const char*)0x1d2, &obj->position);
                animPoseSetAnim(obj->poseId, "A_1", 1);
                obj->unk1DC++;
            }
            break;
        case 1:
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            animPoseSetAnim(obj->poseId, "S_1", 1);
            effSmallStarCamChg(effSmallStarEntry(0, 4, obj->position.x,
                                                 obj->position.y + 12.5f, obj->position.z,
                                                 0.0f, -1.0f, 0.0f),
                               obj->camId);
            evtSetValue(NULL, obj->unk1E4, 1);
            obj->unk1DC = 0x63;
            /* fall through */
        case 0x63:
            mobjDelete(obj->name);
            break;
    }
    return 0;
}

s32 evt_mobj_switch_blue(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    const char* name = (const char*)evtGetValue(event, args[1]);
    f32 x = evtGetFloat(event, args[2]);
    f32 y = evtGetFloat(event, args[3]);
    f32 z = evtGetFloat(event, args[4]);
    s32 unk1D4 = evtGetValue(event, args[5]);
    s32 unk1E4 = args[6];
    const char* asyncName;
    MapObjectEntry* obj;
    BOOL inBattle = gp->inBattle != 0;

    switch (type) {
        case 0:
            asyncName = "MOBJ_BlueSwitch";
            break;
        case 1:
            asyncName = "MOBJ_BlackSwitch";
            break;
        case 2:
        default:
#ifdef MUST_MATCH
            // Include the original section's four trailing alignment bytes in its final literal.
            asyncName = "HOBJ_BlueSwitch\0\0\0\0";
#else
            asyncName = "HOBJ_BlueSwitch";
#endif
            break;
    }
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, x, y, z);
    obj->unk1D0 = mobj_switch_blue;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 0x44;
    obj->flags |= 0xa10000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    if (type == 2) {
        obj->flags |= 2;
    }
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x63;
    }
    mobjCalcMtx(obj);
    return 2;
}

// NONMATCHING (99.78%): only the gp/string-address temp colouring is swapped (retail gp=r5/str=r4,
// ours gp=r4/str=r5); `!!gp->inBattle` fixed the scheduling.
s32 evt_mobj_floatswitch_blue(EventEntry* event, BOOL isFirstCall) {
    f32 x;
    s32* args = event->args;
    const char* name;
    const char* asyncName;
    f32 y;
    f32 z;
    s32 unk1E4;
    MapObjectEntry* obj;
    s32 unk1D4;

    evtGetValue(event, args[0]);
    name = (const char*)evtGetValue(event, args[1]);
    x = evtGetFloat(event, args[2]);
    y = evtGetFloat(event, args[3]);
    z = evtGetFloat(event, args[4]);
    unk1D4 = evtGetValue(event, args[5]);
    unk1E4 = args[6];
    asyncName = "MOBJ_BlueSwitchBlock";
    if (!animGroupBaseAsync(asyncName, !!gp->inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, x, y, z);
    obj->unk1D0 = mobj_switch_blue;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 4;
    obj->flags |= 0x400000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x63;
    }
    mobjCalcMtx(obj);
    return 2;
}

// NONMATCHING (99.78%): only the gp/string-address temp colouring is swapped (retail gp=r5/str=r4,
// ours gp=r4/str=r5); `!!gp->inBattle` fixed the scheduling.
s32 evt_mobj_tornadoswitch_blue(EventEntry* event, BOOL isFirstCall) {
    f32 x;
    s32* args = event->args;
    const char* name;
    const char* asyncName;
    f32 y;
    f32 z;
    s32 unk1E4;
    MapObjectEntry* obj;
    s32 unk1D4;

    evtGetValue(event, args[0]);
    name = (const char*)evtGetValue(event, args[1]);
    x = evtGetFloat(event, args[2]);
    y = evtGetFloat(event, args[3]);
    z = evtGetFloat(event, args[4]);
    unk1D4 = evtGetValue(event, args[5]);
    unk1E4 = args[6];
    asyncName = "MOBJ_BlueTornadoSwitch";
    if (!animGroupBaseAsync(asyncName, !!gp->inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, x, y, z);
    obj->unk1D0 = mobj_switch_blue;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 4;
    obj->flags |= 0x1000000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x63;
    }
    mobjCalcMtx(obj);
    return 2;
}

s32 mobj_switch_red(MapObjectEntry* obj) {
    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                mobjRunEvent(obj, obj->unk1D4);
                if (obj->unk1E0 == 0x200000 || obj->unk1E0 == 0x1000000) {
                    marioChgMot(5);
                }
                psndSFXOn_3D((const char*)0x1d2, &obj->position);
                animPoseSetAnim(obj->poseId, "A_1", 1);
                obj->flags &= ~8;
            }
            break;
        case 1:
            if (obj->flags & 8) {
                psndSFXOn_3D((const char*)0x1d2, &obj->position);
                if (obj->unk1E0 == 0x200000) {
                    marioChgMot(5);
                }
                animPoseSetAnim(obj->poseId, "A_1", 1);
                obj->flags &= ~8;
            }
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            animPoseSetAnim(obj->poseId, "S_1", 1);
            obj->flags &= ~8;
            obj->unk1DC = 0;
            break;
    }
    return 0;
}

s32 evt_mobj_switch_red(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    const char* name = (const char*)evtGetValue(event, args[1]);
    f32 x = evtGetFloat(event, args[2]);
    f32 y = evtGetFloat(event, args[3]);
    f32 z = evtGetFloat(event, args[4]);
    s32 unk1D4 = evtGetValue(event, args[5]);
    s32 unk1E4 = args[6];
    const char* asyncName;
    MapObjectEntry* obj;
    BOOL inBattle = gp->inBattle != 0;

    /* MUST_MATCH: the empty-through case 1 forces MWCC's switch decision tree; default alone folds to `beq`. */
#ifdef MUST_MATCH
    switch (type) {
        case 0:
            asyncName = "MOBJ_RedSwitch";
            break;
        case 1:
        default:
            asyncName = "MOBJ_WhiteSwitch";
            break;
    }
#else
    asyncName = "MOBJ_WhiteSwitch";
    if (type == 0) {
        asyncName = "MOBJ_RedSwitch";
    }
#endif
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, x, y, z);
    obj->unk1D0 = mobj_switch_red;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 0x44;
    obj->flags |= 0xa10000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    mobjCalcMtx(obj);
    return 2;
}

// NONMATCHING (99.78%): only the gp/string-address temp colouring is swapped (retail gp=r5/str=r4,
// ours gp=r4/str=r5); `!!gp->inBattle` fixed the scheduling.
s32 evt_mobj_floatswitch_red(EventEntry* event, BOOL isFirstCall) {
    f32 x;
    s32* args = event->args;
    const char* name;
    const char* asyncName;
    f32 y;
    f32 z;
    s32 unk1E4;
    MapObjectEntry* obj;
    s32 unk1D4;

    evtGetValue(event, args[0]);
    name = (const char*)evtGetValue(event, args[1]);
    x = evtGetFloat(event, args[2]);
    y = evtGetFloat(event, args[3]);
    z = evtGetFloat(event, args[4]);
    unk1D4 = evtGetValue(event, args[5]);
    unk1E4 = args[6];
    asyncName = "MOBJ_RedSwitchBlock";
    if (!animGroupBaseAsync(asyncName, !!gp->inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, x, y, z);
    obj->unk1D0 = mobj_switch_red;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 4;
    obj->flags |= 0x400000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x63;
    }
    mobjCalcMtx(obj);
    return 2;
}

#ifdef MUST_MATCH
// Retail retains these unused animation names between function literal pools.
static const char* pool_redTornado(void) { return "MOBJ_RedTornadoSwitch"; }
#endif

s32 mobj_timerswitch(MapObjectEntry* obj) {
    MapObjectEntry* child = mobjNameToPtr((char*)((u8*)obj + 0x1B8));

    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                if (obj->unk1E0 == 0x200000) {
                    marioChgMot(5);
                }
                if (child->unk1DC != 2) {
                    psndSFXOn_3D((const char*)0x1C1, &obj->position);
                } else {
                    psndSFXOn_3D((const char*)0x1C2, &obj->position);
                }
                evtEntryType(evt_shake, 0, 0, 0);
                animPoseSetAnim(obj->poseId, "A_1", 1);
                obj->unk1DC++;
                obj->flags &= ~8;
            }
            break;
        case 1:
            if (obj->flags & 8) {
                animPoseSetAnim(obj->poseId, "S_1", 1);
                animPoseSetAnim(child->poseId, "S_1", 1);
                obj->unk1DC = 0;
                break;
            }
            if (animPoseGetLoopTimes(obj->poseId) < 0.3f) {
                break;
            }
            animPoseSetAnim(obj->poseId, "A_2", 1);
            animPoseSetAnim(child->poseId, "A_2", 1);
            obj->unk1DC++;
            *(s32*)((u8*)obj + 0x1B0) = *(s32*)((u8*)obj + 0x1B4);
            obj->flags &= ~8;
            break;
        case 2:
            if (obj->flags & 8) {
                animPoseSetAnim(obj->poseId, "S_1", 1);
                animPoseSetAnim(child->poseId, "S_1", 1);
                obj->unk1DC = 0;
                break;
            }
            (*(s32*)((u8*)obj + 0x1B0))--;
            if (*(s32*)((u8*)obj + 0x1B0) <= 0) {
                obj->flags &= ~8;
                animPoseSetAnim(obj->poseId, "S_1", 1);
                animPoseSetAnim(child->poseId, "S_1", 1);
                obj->unk1DC = 0;
                break;
            }
            if (child->unk1DC == 2) {
                if (marioKeyOffChk() != 0 || marioCtrlOffChk() != 0) {
                    obj->unk1DC = 0;
                    break;
                }
                mobjRunEvent(obj, obj->unk1D4);
                if (strcmp(obj->animName, str_802C3DF8) == 0) {
                    obj->flags &= ~8;
                    child->flags &= ~8;
                    animPoseSetAnim(obj->poseId, "S_1", 1);
                    animPoseSetAnim(child->poseId, "S_1", 1);
                    obj->unk1DC = 0;
                    child->unk1DC = 0;
                    break;
                }
                effSmallStarEntry(0, 8, obj->position.x, obj->position.y + 12.5f, obj->position.z,
                                  0.0f, -1.0f, 0.0f);
                effSmallStarEntry(0, 8, child->position.x, child->position.y + 12.5f,
                                  child->position.z, 0.0f, -1.0f, 0.0f);
                evtSetValue(NULL, obj->unk1E4, 1);
                evtSetValue(NULL, child->unk1E4, 1);
                mobjDelete(obj->name);
                mobjDelete(child->name);
            }
            break;
    }
    return 0;
}

s32 evt_mobj_timerswitch(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    const char* names[2];
    f32 xs[2];
    f32 ys[2];
    f32 zs[2];
    MapObjectEntry* obj[2];
    s32 unk1B4;
    s32 unk1D4;
    s32 unk1E4;
    BOOL inBattle = gp->inBattle != 0;
    s32 i;
    const char* asyncName;

    {
        const char* jumptable[2] = {str_802C3DE0, str_802C3DF8};
        asyncName = jumptable[type];
    }
    names[0] = (const char*)evtGetValue(event, args[1]);
    xs[0] = evtGetFloat(event, args[2]);
    ys[0] = evtGetFloat(event, args[3]);
    zs[0] = evtGetFloat(event, args[4]);
    names[1] = (const char*)evtGetValue(event, args[5]);
    xs[1] = evtGetFloat(event, args[6]);
    ys[1] = evtGetFloat(event, args[7]);
    zs[1] = evtGetFloat(event, args[8]);
    unk1B4 = evtGetValue(event, args[9]);
    unk1D4 = evtGetValue(event, args[10]);
    unk1E4 = args[11];
    if (type == 0 && evtGetValue(event, unk1E4) != 0) {
        return 2;
    }
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    for (i = 0; i < 2; i++) {
        mobjEntry(names[i], asyncName);
        obj[i] = mobjNameToPtr(names[i]);
        strcpy((char*)((u8*)obj[i] + 0x1B8), (i == 0) ? names[1] : names[0]);
        mobjSetPosition(names[i], xs[i], ys[i], zs[i]);
        obj[i]->unk1D0 = mobj_timerswitch;
        obj[i]->unk1D4 = (s32*)unk1D4;
        obj[i]->flags |= 0x44;
        obj[i]->flags |= 0xa10000;
        obj[i]->unk1E4 = unk1E4;
        obj[i]->unk1DC = 0;
        *(s32*)((u8*)obj[i] + 0x1B4) = unk1B4;
        if (evtGetValue(event, unk1E4) != 0) {
            obj[i]->unk1DC = 0x63;
        }
        mobjCalcMtx(obj[i]);
    }
    return 2;
}

s32 mobj_rideswitch_lightblue(MapObjectEntry* obj) {
    Vec* vec = vec3_802c3dc8;

    switch (obj->unk1DC) {
#ifdef MUST_MATCH
        // MUST_MATCH: case 2's temporary declared at switch scope gets retail's stack slot (permuter).
        Vec v;
#endif
        case 0:
            if (obj->flags & 8) {
                mobjRunEvent(obj, obj->unk1D4);
                psndSFXOn_3D((const char*)0x1d3, &obj->position);
                animPoseSetAnim(obj->poseId, "A_1", 1);
                obj->unk1DC = 1;
            }
            break;
        case 1: {
            f32 x = obj->scale2.x;
            f32 y = obj->scale2.y - 0.9f;
            f32 z = obj->scale2.z;
            Vec v;

            if (y <= 0.2f) {
                obj->unk1DC = 2;
                obj->flags &= ~8;
                y = 0.2f;
            }
            v = *(Vec*)((u8*)vec + 0x7C);
            v.x = x;
            v.y = y;
            v.z = z;
            obj->scale2 = v;
            mobjCalcMtx2(obj);
            break;
        }
        case 2: {
            AnimationPoseData* data = animPoseGetAnimBaseDataPtr(obj->poseId);
#ifdef MUST_MATCH
            // MUST_MATCH: route the products through a temporary `w` (copied to `v`) and move the
            // vec+0x88 copy after them. This reproduces retail's FP scheduling (products in f2/f0/f1
            // with 10.0 kept live) and the retail temp-copy of the Vec. The natural form is under #else.
            // The named float_0p8_8041a1b0/float_10_8041a1a4 loads (rather than literals) keep both
            // constants live, as in retail.
            Vec w;
            f32 vx;
            f32 vy;
            f32 vz;
            f32 mag;
            f32 dist;

            vx = float_0p8_8041a1b0 * (float_10_8041a1a4 * data->bboxMax.x);
            vy = float_10_8041a1a4 * data->bboxMax.y;
            vz = float_0p8_8041a1b0 * (float_10_8041a1a4 * data->bboxMax.z);
            w = *(Vec*)((u32*)vec + 0x22);
            w.x = vx; w.y = vy; w.z = vz;
            v = w;
#else
            Vec v = *(Vec*)((u8*)vec + 0x88);
            f32 mag;
            f32 dist;

            v.x = float_0p8_8041a1b0 * (float_10_8041a1a4 * data->bboxMax.x);
            v.z = float_0p8_8041a1b0 * (float_10_8041a1a4 * data->bboxMax.z);
            v.y = float_10_8041a1a4 * data->bboxMax.y;
#endif
            mag = PSVECMag(&v);
            dist = PSVECDistance(&obj->position, &marioGetPtr()->position);
            if (dist > mag) {
                if (!(obj->flags & 8)) {
                    obj->unk1DC = 3;
                    animPoseSetAnim(obj->poseId, "A_2", 1);
                    psndSFXOn_3D((const char*)0x1d4, &obj->position);
                    mobjRunEvent(obj, obj->unk1D8);
                } else {
                    obj->flags &= ~8;
                }
            }
            break;
        }
        case 3: {
            f32 x = obj->scale2.x;
            f32 y = obj->scale2.y + float_0p9_8041a228;
            f32 z = obj->scale2.z;
            Vec v;

            if (y >= 15.0f) {
                obj->unk1DC = 0;
                y = 15.0f;
            }
            v = *(Vec*)((u8*)vec + 0x94);
            v.x = x;
            v.y = y;
            v.z = z;
            obj->scale2 = v;
            mobjCalcMtx2(obj);
            obj->flags &= ~8;
            break;
        }
        case 0x63:
            break;
    }
    return 0;
}

#ifdef MUST_MATCH
// Retail retains these unused animation names between function literal pools.
static const char* pool_rideA(void) { return "MOBJ_RideSwitchA"; }
static const char* pool_rideB(void) { return "MOBJ_RideSwitchB"; }
#endif

// NONMATCHING (99.79%): only the gp/string-address temp colouring is swapped (retail gp=r5/str=r4,
// ours gp=r4/str=r5); `!!gp->inBattle` fixed the scheduling.
s32 evt_mobj_rideswitch_lightblue(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 unk1E4;
    const char* name;
    MapObjectEntry* obj;
    f32 f31;
    f32 f30;
    f32 f29;
    s32 unk1D4;
    s32 unk1D8;
    const char* asyncName;

    name = (const char*)evtGetValue(event, args[0]);
    f31 = evtGetFloat(event, args[1]);
    f30 = evtGetFloat(event, args[2]);
    f29 = evtGetFloat(event, args[3]);
    unk1D4 = evtGetValue(event, args[4]);
    unk1D8 = evtGetValue(event, args[5]);
    asyncName = "MOBJ_RideSwitchC";
    unk1E4 = args[6];
    if (!animGroupBaseAsync(asyncName, !!gp->inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    obj->scale2.x *= 0.8f;
    obj->scale2.z *= 0.8f;
    mobjSetPosition(name, f31, f30, f29);
    obj->unk1D0 = mobj_rideswitch_lightblue;
    obj->unk1D4 = (s32*)unk1D4;
    obj->unk1D8 = (s32*)unk1D8;
    obj->flags |= 4;
    obj->flags |= 0x200000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    mobjCalcMtx2(obj);
    return 2;
}

s32 mobj_jumpstand_red(MapObjectEntry* obj) {
    MarioWork* mario = marioGetPtr();
    s32 partyId = marioGetPartyId();

    partyGetPtr(partyId);
    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                EventEntry* evt;
                gp->systemLevelFlags |= 0x10;
                evt = mobjRunEvent(obj, obj->unk1D4);
                if (evt != NULL) {
                    s32 flag;
                    if (*(f32*)((u8*)mario + 0xD8) < mario->position.y) {
                        flag = 0;
                    } else if (*(f32*)((u8*)mario + 0xD8) <
                               obj->position.y + mario->wPlayerCollisionBox.y) {
                        flag = 0;
                    } else {
                        flag = 1;
                    }
                    evt->lwData[0] = flag;
                }
                if (mario->characterId != 2) {
                    if (strcmp(obj->animName, str_802C3E68) == 0) {
                        psndSFXOn_3D((const char*)0x1eb, &obj->position);
                    } else {
                        psndSFXOn_3D((const char*)0x1ec, &obj->position);
                    }
                    switch (irand(4)) {
                        case 0:
                            psndSFXOn_3D((const char*)0xab, &obj->position);
                            break;
                        case 1:
                            psndSFXOn_3D((const char*)0xac, &obj->position);
                            break;
                        case 2:
                            psndSFXOn_3D((const char*)0xad, &obj->position);
                            break;
                        case 3:
                            psndSFXOn_3D((const char*)0xae, &obj->position);
                            break;
                        case 4:
                            psndSFXOn_3D((const char*)0xaf, &obj->position);
                            break;
                    }
                } else if (strcmp(obj->animName, str_802C3E68) == 0) {
                    psndSFXOn((const char*)0x1eb);
                } else {
                    psndSFXOn((const char*)0x1ec);
                }
                animPoseSetAnim(obj->poseId, "A_1", 1);
                obj->flags &= ~8;
                obj->unk1DC++;
            }
            break;
        case 1:
            if (obj->flags & 8) {
                if (evtCheckID(obj->eventId) == 0) {
                    animPoseSetAnim(obj->poseId, "S_1", 1);
                    obj->unk1DC = 0;
                    break;
                }
            }
            if (evtCheckID(obj->eventId) != 1) {
                gp->systemLevelFlags &= ~0x10;
                if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                    break;
                }
                animPoseSetAnim(obj->poseId, "S_1", 1);
                obj->unk1DC = 0;
            }
            break;
    }
    return 0;
}

// .rodata (part C)
#pragma force_active on
__declspec(section ".rodata") Vec vec3_802c3e14[1] = {{0.0f, 0.0f, 0.0f}};
__declspec(section ".rodata") Vec vec3_802c3e20[1] = {{0.0f, 0.0f, 0.0f}};
__declspec(section ".rodata") Vec vec3_802c3e2c[1] = {{0.0f, 0.0f, 0.0f}};
__declspec(section ".rodata") Vec vec3_802c3e38[1] = {{0.0f, 0.0f, 0.0f}};
__declspec(section ".rodata") Vec vec3_802c3e44[1] = {{0.0f, 0.0f, 0.0f}};
__declspec(section ".rodata") Vec vec3_802c3e50[1] = {{0.0f, 0.0f, 0.0f}};
__declspec(section ".rodata") Vec vec3_802c3e5c[1] = {{0.0f, 0.0f, 0.0f}};
const char str_802C3E68[0x14] = "MOBJ_RedJumpStand";
const char str_802C3E7C[0x14] = "MOBJ_RedJumpStand2";
#pragma force_active reset

s32 evt_mobj_jumpstand_red(EventEntry* event, BOOL isFirstCall) {
    MapObjectEntry* obj;
    const char* asyncName;
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    const char* name = (const char*)evtGetValue(event, args[1]);
    f32 f31 = evtGetFloat(event, args[2]);
    f32 f30 = evtGetFloat(event, args[3]);
    f32 f29 = evtGetFloat(event, args[4]);
    s32 unk1D4 = evtGetValue(event, args[5]);
    s32 unk1E4 = args[6];
    s32 inBattle;
    inBattle = gp->inBattle != 0;
    {
        const char* jumptable[2] = {str_802C3E68, str_802C3E7C};
#ifdef MUST_MATCH
        // match-only: the byte-offset indexing reproduces retail's gp-before-slwi schedule
        asyncName = *(const char**)((u8*)jumptable + (type << 2));
#else
        asyncName = jumptable[type];
#endif
    }
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, f31, f30, f29);
    obj->unk1D0 = mobj_jumpstand_red;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 0x44;
    obj->flags |= 0x200000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    mobjCalcMtx(obj);
    return 2;
}

s32 mobj_jumpstand_blue(MapObjectEntry* obj) {
    MarioWork* mario = marioGetPtr();

    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                gp->systemLevelFlags |= 0x10;
                mobjRunEvent(obj, obj->unk1D4);
                if (obj->unk1E0 == 0x200000) {
                    marioChgJumpStandMotion(*(f32*)((u8*)obj + 0x1AC));
                }
                if (mario->characterId != 2) {
                    if (strcmp(obj->animName, str_802C3E98) == 0) {
                        psndSFXOn_3D((const char*)0x1eb, &obj->position);
                    } else {
                        psndSFXOn_3D((const char*)0x1ec, &obj->position);
                    }
                    switch (irand(4)) {
                        case 0:
                            psndSFXOn_3D((const char*)0xab, &obj->position);
                            break;
                        case 1:
                            psndSFXOn_3D((const char*)0xac, &obj->position);
                            break;
                        case 2:
                            psndSFXOn_3D((const char*)0xad, &obj->position);
                            break;
                        case 3:
                            psndSFXOn_3D((const char*)0xae, &obj->position);
                            break;
                        case 4:
                            psndSFXOn_3D((const char*)0xaf, &obj->position);
                            break;
                    }
                } else if (strcmp(obj->animName, str_802C3E98) == 0) {
                    psndSFXOn((const char*)0x1eb);
                } else {
                    psndSFXOn((const char*)0x1ec);
                }
                animPoseSetAnim(obj->poseId, "A_1", 1);
                obj->flags &= ~8;
                obj->unk1DC++;
            }
            break;
        case 1:
            if (obj->flags & 8) {
                animPoseSetAnim(obj->poseId, "S_1", 1);
                obj->unk1DC = 0;
            } else if (evtCheckID(obj->eventId) != 1) {
                gp->systemLevelFlags &= ~0x10;
                if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                    break;
                }
                animPoseSetAnim(obj->poseId, "S_1", 1);
                obj->unk1DC = 0;
            }
            break;
    }
    return 0;
}

// .rodata (part B)
#pragma force_active on
const char str_802C3E98[0x14] = "MOBJ_BlueJumpStand";
const char str_802C3EAC[0x14] = "MOBJ_BlueJumpStand2";
#pragma force_active reset

s32 evt_mobj_jumpstand_blue(EventEntry* event, BOOL isFirstCall) {
    MapObjectEntry* obj;
    const char* asyncName;
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    const char* name = (const char*)evtGetValue(event, args[1]);
    f32 f31 = evtGetFloat(event, args[2]);
    f32 f30 = evtGetFloat(event, args[3]);
    f32 f29 = evtGetFloat(event, args[4]);
    f32 f28 = evtGetFloat(event, args[5]);
    s32 unk1D4 = evtGetValue(event, args[6]);
    s32 unk1E4 = args[7];
    s32 inBattle;
    inBattle = gp->inBattle != 0;
    {
        const char* jumptable[2] = {str_802C3E98, str_802C3EAC};
#ifdef MUST_MATCH
        // match-only: the byte-offset indexing reproduces retail's gp-before-slwi schedule
        asyncName = *(const char**)((u8*)jumptable + (type << 2));
#else
        asyncName = jumptable[type];
#endif
    }
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, f30, f29, f28);
    obj->unk1D0 = mobj_jumpstand_blue;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 0x44;
    obj->flags |= 0x200000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    *(f32*)((u8*)obj + 0x1AC) = f31;
    mobjCalcMtx(obj);
    return 2;
}

// Level-block callback: literal animation names share the compiler's read-only pool base.
s32 mobj_lv_blk_evt(MapObjectEntry* obj) {
    char offName[16];
    char imgName[16];
    ImageEntry* img;
    MarioWork* mario;
    BOOL inBattle;

    LVB_TEMPLATE(offName, str_802C3EC8);
    LVB_TEMPLATE(imgName, str_802C3ED8);
    inBattle = gp->inBattle != 0;
    mario = marioGetPtr();
    strcat(offName, obj->name);
    strcat(imgName, obj->name);
    switch (obj->unk1DC) {
        case 0:
            animGroupBaseAsync("P_box_y", inBattle, NULL);
            if (obj->flags & 8) {
                if (strcmp(obj->animName, "MOBJ_Lv1Block") == 0 ||
                    strcmp(obj->animName, "MOBJ_Lv1BigBlock") == 0) {
                    goto run_event;
                }
                if (strcmp(obj->animName, "MOBJ_Lv1BigBigBlock") == 0) {
                    if (pouchGetHammerLv() >= 2 && mario->currMotionId == MARIO_MOTION_HAMMER_2) {
                        goto run_event;
                    }
                    obj->flags &= ~8;
                    break;
                }
                if (strcmp(obj->animName, "MOBJ_Lv2Block") == 0 ||
                    strcmp(obj->animName, "MOBJ_Lv2BigBlock") == 0) {
                    if (pouchGetHammerLv() >= 3) {
                        goto run_event;
                    }
                    obj->flags &= ~8;
                    break;
                }
                if (strcmp(obj->animName, "MOBJ_Lv2BigBigBlock") == 0) {
                    if (pouchGetHammerLv() >= 3 && mario->currMotionId == MARIO_MOTION_HAMMER_2) {
                        goto run_event;
                    }
                    obj->flags &= ~8;
                    break;
                }
                obj->flags &= ~8;
                break;
            run_event:
                mobjRunEvent(obj, obj->unk1D4);
                evtSetValue(NULL, obj->unk1E4, 1);
                obj->unk1DC = 0xa;
            }
            break;
        case 0xa: {
            s32 captureArgs1[10];
            EventEntry tmpEvent1;
            s32 captureArgs2[10];
            EventEntry tmpEvent2;

            animPaperPoseEntry("P_box_y", inBattle);
            offscreenEntry(offName);
            imgEntry(imgName, inBattle);
            obj->offscreenId = offscreenNameToId(offName);
            tmpEvent1.args = captureArgs1;
            captureArgs1[0] = (s32)imgName;
            captureArgs1[1] = 0;
            captureArgs1[2] = (s32)offName;
            captureArgs1[3] = 1;
            captureArgs1[4] = 0;
            captureArgs1[5] = 0;
            captureArgs1[6] = 0;
            captureArgs1[7] = 0;
            evt_img_alloc_capture(&tmpEvent1, 1);
            tmpEvent2.args = captureArgs2;
            captureArgs2[0] = (s32)imgName;
            captureArgs2[1] = 1;
            captureArgs2[2] = (s32)offName;
            captureArgs2[3] = 1;
            captureArgs2[4] = 0;
            captureArgs2[5] = 0;
            captureArgs2[6] = 0;
            captureArgs2[7] = 0;
            evt_img_alloc_capture(&tmpEvent2, 1);
            img = imgNameToPtr(imgName, inBattle);
            img->poseId = animPaperPoseGetId("P_box_y", inBattle);
            img->animName = str_8041A118;
            // ImageEntry declares no field at 0x110 (it falls 4 bytes into the field_0x10C padding,
            // 0x10C..0x118); the same location is written by evt_img.c, evt_cloud.c and fadedrv.c.
            *(OSTime*)(img->field_0x10C + 4) = animTimeGetTime(inBattle);
            imgSetShadow(img, 2);
            if (strcmp(obj->animName, "MOBJ_Lv1Block") == 0 ||
                strcmp(obj->animName, "MOBJ_Lv2Block") == 0) {
                psndSFXOn_3D((const char*)0x1C7, &obj->position);
            } else if (strcmp(obj->animName, "MOBJ_Lv1BigBlock") == 0 ||
                       strcmp(obj->animName, "MOBJ_Lv2BigBlock") == 0) {
                psndSFXOn_3D((const char*)0x1C8, &obj->position);
            } else if (strcmp(obj->animName, "MOBJ_Lv1BigBigBlock") == 0 ||
                       strcmp(obj->animName, "MOBJ_Lv2BigBigBlock") == 0) {
                psndSFXOn_3D((const char*)0x1C9, &obj->position);
            }
            obj->unk1DC = 0xb;
            break;
        }
        case 0xb:
            img = imgNameToPtr(imgName, inBattle);
            if (img->field_0x118 >= float_1_8041a160) {
                obj->unk1DC = 0xc;
            }
            if (img->flags & 0x10000000) {
                obj->unk1DC = 0xc;
            }
            obj->position = lv_block_position[0];
            obj->flags |= 4;
            obj->flags |= 8;
            break;
        case 0xc:
            img = imgNameToPtr(imgName, inBattle);
            imgFreeCapture(img, 0);
            imgFreeCapture(img, 1);
            imgSetShadow(img, 0);
            imgRelease(img, inBattle);
            offscreenDelete(offName);
            obj->offscreenId = -1;
            obj->unk1DC = 0x63;
            break;
        case 0x63:
            mobjDelete(obj->name);
            break;
    }
    return 0;
}

s32 evt_mobj_lv_blk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f29 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f31 = evtGetFloat(event, args[3]);
    s32 unk1D4 = evtGetValue(event, args[4]);
    s32 unk1E4 = args[5];
    const char* asyncName = (const char*)evtGetValue(event, args[6]);
    MapObjectEntry* obj;
    Vec pos;
    BOOL inBattle = gp->inBattle != 0;

    if (evtGetValue(event, unk1E4) != 0) {
        return 2;
    }
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    if (!animGroupBaseAsync("P_box_y", inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    obj->flags |= 0x44;
    pos = vec3_802c3ef4[0];
    pos.x = f29;
    pos.y = f30;
    pos.z = f31;
    obj->position = pos;
    obj->unk1D0 = mobj_lv_blk_evt;
    obj->unk1D4 = (s32*)unk1D4;
    obj->unk1D8 = 0;
    obj->flags |= 0x10000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    return 2;
}

s32 mobj_float_blk(MapObjectEntry* obj) {
    switch (obj->unk1DC) {
        case 0x63:
            if (*(s8*)&obj->unk1A0[0x18] == 0) {
                f32 hitNZ, hitNY, hitNX, distance, hitZ, hitY, hitX;

                distance = float_1_8041a160 + (f32)__fabs(obj->unk5C.y);
                if ((s32)hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ,
                                        obj->position.x, float_0p5_8041a15c + obj->position.y, obj->position.z,
                                        float_0_8041a150, float_neg1_8041a214, float_0_8041a150) != 0) {
                    if (strcmp(animPoseGetCurrentAnim(obj->poseId)->name, "S_1") != 0) {
                        animPoseSetAnim(obj->poseId, "S_1", 1);
                        obj->flags |= 2;
                    }
                } else {
                    if (strcmp(animPoseGetCurrentAnim(obj->poseId)->name, "A_1") != 0) {
                        animPoseSetAnim(obj->poseId, "A_1", 1);
                        obj->flags &= ~2;
                    }
                }
            } else {
                AnimTableEntry* anim = animPoseGetCurrentAnim(mobjNameToPtr((const char*)&obj->unk1A0[0x18])->poseId);

                if ((s8)anim->name[0] == 'A' && (s8)anim->name[2] >= '1' && (s8)anim->name[2] <= '3') {
                    animPoseSetAnim(obj->poseId, "A_1", 0);
                } else {
                    animPoseSetAnim(obj->poseId, "S_1", 1);
                }
            }
            break;
    }
    return 0;
}

s32 evt_mobj_float_blk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f31 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f29 = evtGetFloat(event, args[3]);
    s32 type = args[4];
    s32 sub = args[5];
    s32 unk1E4;
    const char* asyncName;
    MapObjectEntry* obj;
    BOOL inBattle;

    evtGetValue(event, args[6]);
    unk1E4 = args[7];
    inBattle = gp->inBattle != 0;
    switch (type) {
        case 0:
            if (sub == 0) {
                asyncName = "MOBJ_green_FlowBlock";
            } else if (sub == 1) {
                asyncName = "MOBJ_green_big_FlowBlock";
            } else {
                asyncName = "MOBJ_green_big02_FlowBlock";
            }
            break;
        case 1:
            if (sub == 0) {
                asyncName = "MOBJ_purple_FlowBlock";
            } else if (sub == 1) {
                asyncName = "MOBJ_purple_big_FlowBlock";
            } else {
                asyncName = "MOBJ_purple_big02_FlowBlock";
            }
            break;
        case 2:
            if (sub == 0) {
                asyncName = "MOBJ_orange_FlowBlock";
            } else if (sub == 1) {
                asyncName = "MOBJ_orange_big_FlowBlock";
            } else {
                asyncName = "MOBJ_orange_big02_FlowBlock";
            }
            break;
        case 3:
            asyncName = "MOBJ_red_big02_FlowBlock";
            break;
        default:
            if (sub == 0) {
                asyncName = "MOBJ_green_FlowBlock";
            } else if (sub == 1) {
                asyncName = "MOBJ_green_big_FlowBlock";
            } else {
                asyncName = "MOBJ_green_big02_FlowBlock";
            }
            break;
    }
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, f31, f30, f29);
    obj->unk1D0 = mobj_float_blk;
    obj->flags |= 4;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0x63;
    mobjCalcMtx(obj);
    {
        f32 hitNZ, hitNY, hitNX, distance, hitZ, hitY, hitX;

        distance = float_1_8041a160 + (f32)__fabs(obj->unk5C.y);
        if ((s32)hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ,
                                obj->position.x, float_0p5_8041a15c + obj->position.y, obj->position.z,
                                float_0_8041a150, float_neg1_8041a214, float_0_8041a150) != 0) {
            animPoseSetAnim(obj->poseId, "S_1", 1);
            obj->flags |= 2;
        } else {
            animPoseSetAnim(obj->poseId, "A_1", 1);
        }
    }
    return 2;
}

// Data for mobj_switch_float_blk only (unit .rodata).

// Switch-block state machine: picks the child/partner anim based on whether the linked block
// is hit, runs the event, and opens/closes its animation.  `_switch_anim` is the six anim-name
// table in .sdata.
// One residual MWCC non-determinism remains: the case-0 `_switch_anim` scan initialises i with
// a fresh `li r30,0` where the target copies the already-zero idx (`mr r30,r29`).
// opt_common_subs off: with CSE on, MWCC reorders the float_0/float_1 loads, the fmr and the
// fabs around the child-pointer copy in the two hitCheckFilter calls (target
// `mr r11,r3; lfs f4; lfs f0; lfs f1; fmr; fabs` vs CSE-on
// `lfs f0; mr r11,r3; lfs f4; fabs; lfs f1; lfs f2; fmr`); off reproduces the retail order.
// Scoped to this function by the reset below.
// NONMATCHING (99.85%)
#pragma opt_common_subs off
s32 mobj_switch_float_blk(MapObjectEntry* obj) {
    switch (obj->unk1DC) {
        case 0: {
            s32 i;
            s32 idx;
            AnimTableEntry* anim;
            const char** p;
            s32 animId;

            if (!(obj->flags & 8)) {
                break;
            }
            anim = animPoseGetCurrentAnim(obj->poseId);
            idx = 0;
            p = _switch_anim;
            for (i = 0; i < 6; i++, p++) {
                if (strcmp(*p, anim->name) == 0) {
                    idx = i;
                    break;
                }
            }
            *(s32*)((u8*)obj + 0x1A4) = (*(s32*)((u8*)obj + 0x1A4) & 0x40000000) | idx;
            if (*(s8*)&obj->unk1A0[0x18] != 0) {
                MapObjectEntry* child = mobjNameToPtr((const char*)&obj->unk1A0[0x18]);
                f32 hitNZ, hitNY, hitNX, distance, hitZ, hitY, hitX;

                distance = float_1_8041a160 + (f32)__fabs(child->unk5C.y);
                if ((s32)hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ,
                                        child->position.x, float_0p5_8041a15c + child->position.y,
                                        child->position.z, float_0_8041a150, float_neg1_8041a214,
                                        float_0_8041a150) != 0) {
                    if (evtCheckID(obj->eventId) != 0) {
                        anim = animPoseGetCurrentAnim(obj->poseId);
                        {
                            BOOL isA;
                            if ((s8)anim->name[0] == 'A' && (s8)anim->name[2] >= '1' &&
                                (s8)anim->name[2] <= '3') {
                                isA = 1;
                            } else {
                                isA = 0;
                            }
                            if (isA) {
                                animId = 0;
                            } else {
                                animId = 3;
                            }
                        }
                    } else {
                        *(s32*)((u8*)obj + 0x1A4) |= 0x80000000;
                        animId = 0;
                    }
                } else {
                    if (evtCheckID(obj->eventId) != 0) {
                        anim = animPoseGetCurrentAnim(obj->poseId);
                        {
                            s8* n = (s8*)anim->name;
                            s8 c = n[0];
                            if (c == 'A' && n[2] >= '1' && n[2] <= '2') {
                                animId = 0;
                            } else if (c == 'A' && n[2] == '3') {
                                *(s32*)((u8*)obj + 0x1A4) |= 0x40000000;
                                animId = 0;
                            } else if (c == 'A' && n[2] >= '4' && n[2] <= '5') {
                                animId = 3;
                            } else {
                                animId = 3;
                            }
                        }
                    } else {
                        *(s32*)((u8*)obj + 0x1A4) |= 0x80000000;
                        animId = 3;
                    }
                }
            } else {
                anim = animPoseGetCurrentAnim(obj->poseId);
                {
                    BOOL isA;
                    if ((s8)anim->name[0] == 'A' && (s8)anim->name[2] >= '1' &&
                        (s8)anim->name[2] <= '3') {
                        isA = 1;
                    } else {
                        isA = 0;
                    }
                    if (isA) {
                        animId = 3;
                    } else {
                        animId = 0;
                    }
                }
                *(s32*)((u8*)obj + 0x1A4) |= 0x80000000;
            }
            mobjRunEvent(obj, obj->unk1D4);
            if (obj->unk1E0 == 0x200000) {
                marioChgMot(5);
            }
            psndSFXOn_3D((const char*)0x1b5, &obj->position);
            switch (obj->unk1E0) {
            case 0x400000:
                animPoseSetAnim(obj->poseId, _switch_anim[animId + 1], 1);
                break;
            default:
                animPoseSetAnim(obj->poseId, _switch_anim[animId], 1);
                break;
            }
            obj->flags &= ~8;
            if (*(s32*)((u8*)obj + 0x1B0) != 0) {
                obj->unk1DC = 0xa;
            } else {
                obj->unk1DC++;
            }
            break;
        }
        case 1:
            if (obj->flags & 8) {
                obj->unk1DC = 0;
                break;
            }
            if (obj->flags & 0x1000) {
                obj->flags &= ~0x1000;
                obj->unk1DC = 0xa;
                break;
            }
            if (animPoseGetLoopTimes(obj->poseId) < float_1_8041a160) {
                break;
            }
            if (*(s8*)&obj->unk1A0[0x18] != 0) {
                mobjNameToPtr((const char*)&obj->unk1A0[0x18]);
            }
            {
                const char* nm;
                AnimTableEntry* a = animPoseGetCurrentAnim(obj->poseId);
                BOOL isA;
                if ((s8)a->name[0] == 'A' && (s8)a->name[2] >= '1' && (s8)a->name[2] <= '3') {
                    isA = 1;
                } else {
                    isA = 0;
                }
                if (isA) {
                    nm = str_8041A120;
                } else {
                    nm = str_8041A12C;
                }
                animPoseSetAnim(obj->poseId, nm, 1);
            }
            obj->unk1DC++;
            break;
        case 2:
            if (obj->flags & 8) {
                obj->unk1DC = 0;
                break;
            }
            if (obj->flags & 0x1000) {
                obj->flags &= ~0x1000;
                obj->unk1DC = 0xa;
            }
            if (evtCheckID(obj->eventId) != 0) {
                break;
            }
            obj->unk1DC = 0;
            *(s32*)((u8*)obj + 0x1A4) = 0;
            break;
        case 0xa:
            if (obj->flags & 8) {
                obj->unk1DC = 0;
                break;
            }
            if (animPoseGetLoopTimes(obj->poseId) < float_1_8041a160) {
                break;
            }
            {
                AnimTableEntry* anim = animPoseGetCurrentAnim(obj->poseId);
                BOOL isA;
                if (((s8)anim->name[0] == 'A' && (s8)anim->name[2] >= '1' && (s8)anim->name[2] <= '3')) {
                    isA = 1;
                } else {
                    isA = 0;
                }
                if (isA) {
                    animPoseSetAnim(obj->poseId, str_8041A120, 1);
                } else {
                    animPoseSetAnim(obj->poseId, str_8041A12C, 1);
                }
            }
            *(s32*)((u8*)obj + 0x1B0) = 0x78;
            obj->unk1DC++;
        case 0xb:
            if (obj->flags & 8) {
                obj->unk1DC = 0;
                break;
            }
            (*(s32*)((u8*)obj + 0x1B0))--;
            if (*(s32*)((u8*)obj + 0x1B0) != 0) {
                break;
            }
            if (*(s8*)&obj->unk1A0[0x18] != 0) {
                MapObjectEntry* child = mobjNameToPtr((const char*)&obj->unk1A0[0x18]);
                f32 hitNZ, hitNY, hitNX, distance, hitZ, hitY, hitX;

                distance = float_1_8041a160 + (f32)__fabs(child->unk5C.y);
                if ((s32)hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ,
                                        child->position.x, float_0p5_8041a15c + child->position.y,
                                        child->position.z, float_0_8041a150, float_neg1_8041a214,
                                        float_0_8041a150) != 0) {
                    animPoseSetAnim(obj->poseId, str_8041A12C, 0);
                } else {
                    animPoseSetAnim(obj->poseId, str_8041A120, 0);
                }
            } else {
                AnimTableEntry* anim = animPoseGetCurrentAnim(obj->poseId);
                BOOL isA;
                if (((s8)anim->name[0] == 'A' && (s8)anim->name[2] >= '1' && (s8)anim->name[2] <= '3')) {
                    isA = 1;
                } else {
                    isA = 0;
                }
                if (isA) {
                    animPoseSetAnim(obj->poseId, str_8041A120, 1);
                } else {
                    animPoseSetAnim(obj->poseId, str_8041A12C, 1);
                }
            }
            obj->unk1DC = 0;
            break;
    }
    return 0;
}
#pragma opt_common_subs reset

// The float-block switch animation names pool in their original codegen order.
s32 evt_mobj_switch_float_blk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f31 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f29 = evtGetFloat(event, args[3]);
    const char* childName = (const char*)evtGetValue(event, args[4]);
    s32 type = args[5];
    s32 unk1D4 = evtGetValue(event, args[6]);
    s32 unk1E4 = args[7];
    BOOL inBattle = gp->inBattle != 0;
    const char* asyncName;
    MapObjectEntry* obj;

    switch (type) {
        case 0:
            asyncName = "MOBJ_green_FlowBlockSwitch";
            break;
        case 1:
            asyncName = "MOBJ_purple_FlowBlockSwitch";
            break;
        case 2:
            asyncName = "MOBJ_orange_FlowBlockSwitch";
            break;
        case 3:
            asyncName = "MOBJ_red_FlowBlockSwitch";
            break;
        default:
            asyncName = "MOBJ_green_FlowBlockSwitch";
            break;
    }
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, f31, f30, f29);
    obj->unk1D0 = mobj_switch_float_blk;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 4;
    obj->flags |= 0xe10000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    *(s32*)&obj->unk1A0[0x10] = 0;
    mobjCalcMtx(obj);
    strcpy((char*)&obj->unk1A0[0x18], childName);
    if (*(s8*)&obj->unk1A0[0x18] != 0) {
        f32 hitNZ, hitNY, hitNX, distance, hitZ, hitY, hitX;
        MapObjectEntry* obj2 = mobjNameToPtr((char*)&obj->unk1A0[0x18]);
        HitEntry* hit;
        strcpy((char*)&obj2->unk1A0[0x18], obj->name);
        distance = float_1_8041a160 + (f32)__fabs(obj2->unk5C.y);
        hit = hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ,
                             obj2->position.x, float_0p5_8041a15c + obj2->position.y,
                             obj2->position.z, float_0_8041a150, float_neg1_8041a214,
                             float_0_8041a150);
        if ((s32)hit != 0) {
            animPoseSetAnim(obj->poseId, "S_1", 1);
        } else {
            animPoseSetAnim(obj->poseId, "A_3", 1);
        }
    }
    return 2;
}

s32 evt_mobj_lock_unlock(EventEntry* event, BOOL isFirstCall) {
    const char* name = (const char*)evtGetValue(event, event->args[0]);
    MapObjectEntry* obj = mobjNameToPtr(name);

    if (strcmp(obj->animName, "MOBJ_Lock") != 0) {
        return 2;
    }
    if (obj->unk1D0 == NULL) {
        obj->unk1D0 = mobj_lock;
    }
    obj->flags |= 8;
    marioKeyOff();
    partyKeyOff();
    marioStSystemLevel(1);
    obj->unk1DC = 3;
    return 2;
}

s32 mobj_lock(MapObjectEntry* obj) {
    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                marioKeyOff();
                partyKeyOff();
                marioStSystemLevel(1);
                mobjRunEvent(obj, obj->unk1D4);
                obj->unk1DC++;
            }
            break;
        case 1:
            if (evtCheckID(obj->eventId) == 1) {
                break;
            }
            if (obj->flags & 0x1000) {
                obj->flags &= ~0x1000;
                obj->flags &= ~0x8;
                marioKeyOn();
                partyKeyOn();
                marioStSystemLevel(0);
                obj->eventId = 0;
                obj->unk1DC = 0;
            } else {
                obj->eventId = 0;
                obj->unk1DC++;
            }
            break;
        case 2: {
            s32 item;

            if (*(s32*)obj->unk1A0 & 0x80000000) {
                s32 i;
                s32 check;

                item = 0;
                i = 0;
                while (1) {
                    check = ((s32*)*(s32*)obj->unk1A0)[i];
                    if (check == -1) {
                        break;
                    }
                    check = pouchCheckItem(check);
                    if (check != 0) {
                        item = check;
                        break;
                    }
                    i++;
                }
            } else {
                s32 check = pouchCheckItem(*(s32*)obj->unk1A0);

                item = (check != 0) ? *(s32*)obj->unk1A0 : 0;
            }
            if (item != 0) {
                pouchRemoveItem(item);
                obj->unk1DC++;
            } else {
                *(s32*)&obj->unk1A0[8] = msgWindow_Entry(msgSearch("sys_no_key"), 0, 0);
                obj->unk1DC = 0xa;
            }
            break;
        }
        case 3: {
            EventEntry* event = evtEntryType(evt_lock_sound, 0, 0, 0);
            f32 scale = float_1024_8041a1a0;

            event->lwData[0] = (s32)(scale * obj->position.x) + 0xF24A7A80;
            event->lwData[1] = (s32)(scale * obj->position.y) + 0xF24A7A80;
            event->lwData[2] = (s32)(scale * obj->position.z) + 0xF24A7A80;
            animPoseSetAnim(obj->poseId, str_8041A12C, 1);
            obj->unk1DC++;
            break;
        }
        case 4:
            if (animPoseGetLoopTimes(obj->poseId) < float_1_8041a160) {
                break;
            }
            obj->unk5C.y = 0.0f;
            obj->unk68 = 0.5f;
            obj->unk6C = 0.0f;
            evtSetValue(NULL, obj->unk1E4, 1);
            mobjRunEvent(obj, obj->unk1D8);
            obj->unk1DC++;
            break;
        case 5: {
            f32 hitX, hitY, hitZ, distance, hitNX, hitNY, hitNZ;

            distance = float_1_8041a160 + (f32)__fabs(obj->unk5C.y);
            if (hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ,
                               obj->position.x, float_1_8041a160 + obj->position.y, obj->position.z,
                               float_0_8041a150, obj->unk5C.y, float_0_8041a150) != NULL) {
                obj->position.y = hitY;
                obj->unk5C.y *= float_neg0p8_8041a220;
                obj->unk6C += 1.0f;
                if (obj->unk6C < float_4_8041a224) {
                    psndSFXOn_3D((const char*)0x1e6, &obj->position);
                }
            } else {
                obj->position.y += obj->unk5C.y;
                obj->unk5C.y -= obj->unk68;
            }
            if (obj->unk6C < float_4_8041a224) {
                break;
            }
            if (evtCheckID(obj->eventId) != 1) {
                obj->unk1DC++;
                *(s32*)&obj->unk1A0[0x10] = 0x14;
            }
            break;
        }
        case 6:
            (*(s32*)&obj->unk1A0[0x10])--;
            if (*(s32*)&obj->unk1A0[0x10] & 2) {
                obj->flags |= 0x4000;
            } else {
                obj->flags &= ~0x4000;
            }
            if (*(s32*)&obj->unk1A0[0x10] == 0) {
                obj->unk1DC = 0x63;
            }
            break;
        case 10:
            if (windowCheckID(*(s32*)&obj->unk1A0[8]) == 0) {
                obj->flags &= ~0x8;
                marioKeyOn();
                partyKeyOn();
                marioStSystemLevel(0);
                obj->unk1DC = 0;
            }
            break;
        case 0x63:
            marioKeyOn();
            partyKeyOn();
            marioStSystemLevel(0);
            mobjDelete(obj->name);
            break;
    }
    return 0;
}

s32 evt_mobj_lock(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    s32 unk1A0 = evtGetValue(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f29 = evtGetFloat(event, args[3]);
    f32 f28 = evtGetFloat(event, args[4]);
    f32 f31 = evtGetFloat(event, args[5]);
    s32 unk1D4 = evtGetValue(event, args[6]);
    s32 unk1D8 = evtGetValue(event, args[7]);
    s32 unk1E4 = args[8];
    BOOL inBattle = gp->inBattle != 0;
    MapObjectEntry* obj;
    const char* asyncName = "MOBJ_Lock";

    if (evtGetValue(event, unk1E4) != 0) {
        return 2;
    }
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjHitEntry(obj, 2);
    *(s32*)obj->unk1A0 = unk1A0;
    mobjSetPosition(name, f30, f29, f28);
    obj->rotation.y = f31;
    obj->unk1D0 = mobj_lock;
    obj->unk1D4 = (s32*)unk1D4;
    obj->unk1D8 = (s32*)unk1D8;
    obj->flags |= 0x26;
    obj->flags |= 0x40000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "Z_1", 1);
    return 2;
}

// Itembox per-frame update callback (obj->unk1D0). Installed by evt_mobj_itembox.
// MUST_MATCH: the named rodata base keeps the object in r31 and the pool in r30.
// The literal form alone swaps those registers (97.58%). All addressed strings and
// the double are now compiler-owned literals, first pooled by evt_mobj_itembox;
// IB_* selects their natural values without MUST_MATCH.
s32 mobj_itembox(MapObjectEntry* obj) {
#ifdef MUST_MATCH
    u8* dbase = (u8*)negone_one_374;
    u8* rbase = (u8*)vec3_802c3dc8;

#endif
    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                marioKeyOff();
                partyKeyOff();
                marioStSystemLevel(1);
                obj->eventId = 0;
                mobjRunEvent(obj, obj->unk1D4);
                obj->unk1DC++;
            }
            break;
        case 1:
            if (obj->eventId != 0) {
                if (evtCheckID(obj->eventId) == 1) {
                    break;
                }
                if (obj->flags & 0x1000) {
                    obj->flags &= ~0x1000;
                    obj->flags &= ~8;
                    marioKeyOn();
                    partyKeyOn();
                    marioStSystemLevel(0);
                    obj->eventId = 0;
                    obj->unk1DC = 0;
                    break;
                }
            }
            obj->unk1DC++;
            break;
        case 2: {
            EventEntry* event;

            if (strcmp(obj->animName, IB_R(0x6A0, str_802C4468, "MOBJ_TreasureBox")) == 0 ||
                strcmp(obj->animName, IB_R(0x6C8, str_802C4490, "MOBJ_GrayTreasureBox")) == 0) {
                event = evtEntryType(IB_D(0x90, evt_box_sound1), 0, 0, 0);
                event->lwData[0] = (s32)(1024.0f * obj->position.x) + 0xF24A7A80;
                event->lwData[1] = (s32)(1024.0f * obj->position.y) + 0xF24A7A80;
                event->lwData[2] = (s32)(1024.0f * obj->position.z) + 0xF24A7A80;
            } else if (strcmp(obj->animName, IB_R(0x6E0, str_802C44A8, "MOBJ_BlackTreasureBox")) == 0) {
                event = evtEntryType(IB_D(0x130, evt_box_sound3), 0, 0, 0);
                event->lwData[0] = (s32)(1024.0f * obj->position.x) + 0xF24A7A80;
                event->lwData[1] = (s32)(1024.0f * obj->position.y) + 0xF24A7A80;
                event->lwData[2] = (s32)(1024.0f * obj->position.z) + 0xF24A7A80;
            } else {
                event = evtEntryType(IB_D(0xE0, evt_box_sound2), 0, 0, 0);
                event->lwData[0] = (s32)(1024.0f * obj->position.x) + 0xF24A7A80;
                event->lwData[1] = (s32)(1024.0f * obj->position.y) + 0xF24A7A80;
                event->lwData[2] = (s32)(1024.0f * obj->position.z) + 0xF24A7A80;
            }
            animPoseSetAnim(obj->poseId, str_8041A118, 1);
            obj->eventId = 0;
            mobjRunEvent(obj, obj->unk1D8);
            obj->unk1DC++;
            break;
        }
        case 3: {
            AnimationPoseData* animData;
            Vec maxTmp;
            Vec minTmp;
            Vec bboxMax;
            Vec bboxMin;

            if (animPoseGetLoopTimes(obj->poseId) < float_1_8041a160) {
                break;
            }
            if (obj->eventId != 0 && evtCheckID(obj->eventId) == 1) {
                break;
            }
            obj->flags &= ~0x20;
            animData = animPoseGetAnimBaseDataPtr(obj->poseId);
            minTmp = IB_VEC(0x210, vec3_802c3fd8, ((Vec){0.0f, 0.0f, 0.0f}));
            minTmp.x = animData->bboxMin.x;
            minTmp.y = animData->bboxMin.y;
            minTmp.z = animData->bboxMin.z;
            bboxMin = minTmp;
            maxTmp = IB_VEC(0x21C, vec3_802c3fe4, ((Vec){0.0f, 0.0f, 0.0f}));
            maxTmp.x = animData->bboxMax.x;
            maxTmp.y = animData->bboxMax.y;
            maxTmp.z = animData->bboxMax.z;
            bboxMax = maxTmp;
            bboxMin.z = bboxMin.z - IB_F64(0x710, double_0p2_802c44d8, ((f64)0.2f)) * __fabsf(bboxMax.y - bboxMin.y);
            PSVECAdd(&bboxMax, &bboxMin, &obj->translation);
            PSVECScale(&obj->translation, &obj->translation, float_0p5_8041a15c);
            PSVECScale(&obj->translation, &obj->translation, float_10_8041a1a4);
            obj->translation.y = float_0_8041a150;
            obj->scale2.y = obj->scale2.y * float_1p4_8041a208;
            obj->scale2.z = obj->scale2.z * float_1p2_8041a20c;
            mobjCalcMtx2(obj);
            marioKeyOn();
            partyKeyOn();
            marioStSystemLevel(0);
            obj->eventId = 0;
            obj->unk1DC = 0x63;
            break;
        }
        case 0x63:
            evtSetValue(NULL, obj->unk1E4, 1);
            obj->flags |= 8;
            obj->flags &= ~0x20;
            if (strcmp(obj->animName, IB_R(0x6E0, str_802C44A8, "MOBJ_BlackTreasureBox")) == 0) {
                animPoseSetAnim(obj->poseId, str_8041A218, 1);
            } else {
                animPoseSetAnim(obj->poseId, str_8041A1C0, 1);
            }
            break;
    }
    return 0;
}

s32 evt_mobj_itembox(EventEntry* event, BOOL isFirstCall) {

    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f31 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f29 = evtGetFloat(event, args[3]);
    s32 type = evtGetValue(event, args[4]);
    s32 unk1D4 = evtGetValue(event, args[5]);
    s32 unk1D8 = evtGetValue(event, args[6]);
    s32 unk1E4 = args[7];
    BOOL inBattle = gp->inBattle != 0;
    const char* modelName;
    MapObjectEntry* obj;

    switch (type) {
        case 0:
            modelName = "MOBJ_TreasureBox";
            break;
        case 1:
            modelName = "MOBJ_BigTreasureBox";
            break;
        case 2:
            modelName = "MOBJ_GrayTreasureBox";
            break;
        case 3:
            modelName = "MOBJ_BlackTreasureBox";
            break;
        case 4:
        default:
            modelName = "MOBJ_GoldenTreasureBox";
            break;
    }
    if (!animGroupBaseAsync(modelName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, modelName);
    obj = mobjNameToPtr(name);
    mobjHitEntry(obj, 2);
    mobjSetPosition(name, f31, f30, f29);
    obj->unk1D0 = mobj_itembox;
    obj->unk1D4 = (s32*)unk1D4;
    obj->unk1D8 = (s32*)unk1D8;
    obj->flags |= 0x64;
    obj->flags |= 0x40000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    mobjCalcMtx(obj);
    if (evtGetValue(event, unk1E4) != 0) {
        AnimationPoseData* animData = animPoseGetAnimBaseDataPtr(obj->poseId);
        Vec maxTmp;
        Vec minTmp;
        Vec bboxMax;
        Vec bboxMin;

        minTmp = vec3_802c3fd8[0];
        minTmp.x = animData->bboxMin.x;
        minTmp.y = animData->bboxMin.y;
        minTmp.z = animData->bboxMin.z;
        bboxMin = minTmp;
        maxTmp = vec3_802c3fe4[0];
        maxTmp.x = animData->bboxMax.x;
        maxTmp.y = animData->bboxMax.y;
        maxTmp.z = animData->bboxMax.z;
        bboxMax = maxTmp;
        bboxMin.z = bboxMin.z - (f64)0.2f * __fabsf(bboxMax.y - bboxMin.y);
        PSVECAdd(&bboxMax, &bboxMin, &obj->translation);
        PSVECScale(&obj->translation, &obj->translation, float_0p5_8041a15c);
        PSVECScale(&obj->translation, &obj->translation, float_10_8041a1a4);
        obj->translation.y = float_0_8041a150;
        obj->scale2.y = obj->scale2.y * float_1p4_8041a208;
        obj->scale2.z = obj->scale2.z * float_1p2_8041a20c;
        mobjCalcMtx2(obj);
        obj->unk1DC = 0x63;
    }
    {
        f32 hitX;
        f32 hitY;
        f32 hitZ;
        f32 distance = float_5_8041a210 + (f32)__fabs(obj->unk5C.y);
        f32 hitNX;
        f32 hitNY;
        f32 hitNZ;

        if (hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ,
                           obj->position.x, float_1_8041a160 + obj->position.y, obj->position.z,
                           float_0_8041a150, float_neg1_8041a214, float_0_8041a150) != NULL) {
            obj->flags |= 2;
            obj->flags &= ~0x40;
        }
    }
    return 2;
}

s32 mobj_signboard(MapObjectEntry* obj) {
    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                marioKeyOff();
                mobjRunEvent(obj, obj->unk1D4);
                obj->unk1DC++;
            }
            break;
        case 1:
            if (evtCheckID(obj->eventId) != 1) {
                marioKeyOn();
                obj->eventId = 0;
                obj->flags &= ~8;
                obj->unk1DC = 0;
            }
            break;
    }
    return 0;
}

// NONMATCHING (99.74%): the gp pointer and the string-address high-part temp are coloured the other
// way round (retail `lwz r5, gp` + `lis r4, str`; ours `lwz r4, gp` + `lis r5, str`). Everything else
// byte-matches; `!!gp->inBattle` fixed the unk1E4 load scheduling.
s32 evt_mobj_signboard(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    f32 f1;
    f32 f2;
    f32 f3;
    s32 unk1D4;
    s32 unk1E4;
    const char* asyncName;

    name = (const char*)evtGetValue(event, args[0]);
    f1 = evtGetFloat(event, args[1]);
    f2 = evtGetFloat(event, args[2]);
    f3 = evtGetFloat(event, args[3]);
    unk1D4 = evtGetValue(event, args[4]);
    asyncName = "MOBJ_Signboard";
    unk1E4 = args[5];
    if (!animGroupBaseAsync(asyncName, !!gp->inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, f1, f2, f3);
    obj->unk1D0 = mobj_signboard;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 0x24;
    obj->flags |= 0x40000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    return 2;
}

#ifdef MUST_MATCH
// Retail retains these unused animation names between function literal pools.
static const char* pool_arrow(void) { return "MOBJ_Arrow"; }
#endif

// Recovery block (heart / coin-taken). MapObjectEntry does not declare the fields this object stores in
// its 0x1A0 padding, so they are read/written through the declared array: *(s32*)&obj->unk1A0[4] (0x1A4
// coinNeeded), (u8*)obj + 0x1A8 msgWindowId, +0x1B0 timer, +0x1C8 EffectEntry* effect. The
// effCoinFukidashiEntry calls pass the header's (type, coinNum, count, x, y, z) order; y is accumulated
// through a local (`f32 y2 = float_25; y2 += obj->position.y;`) so mwcc emits `fadds f2,f2,f1` like retail.
// The residue is register allocation: retail keeps the y operand in f1 and the z operand in f3, ours uses
// f0/f1, and retail loads the coin argument last.
// NONMATCHING (98.21%)
s32 mobj_recovery_blk(MapObjectEntry* obj) {
    s32 i;

    switch (obj->unk1DC) {
        case -1:
            {
                f32 y2 = float_25_8041a140;
                y2 += obj->position.y;
                *(EffectEntry**)((u8*)obj + 0x1C8) = effCoinFukidashiEntry(
                    0, *(s32*)&obj->unk1A0[4], 0,
                    obj->position.x, y2, obj->position.z - float_10_8041a1a4);
            }
            obj->unk1DC++;
            break;
        case 0:
            if (!(obj->flags & 8)) {
                break;
            }
            marioKeyOff();
            camGetPtr(8)->flags |= 0x200;
            psndSFXOn_3D((const char*)0x1D9, &obj->position);
            obj->unk1DC++;
            // fallthrough
        case 1:
            if (!(gp->flags & 0x1000) && swGet(0xEA) == 0) {
                swSet(0xEA);
                obj->unk1DC = 0x14;
                break;
            }
            obj->unk1DC++;
            // fallthrough
        case 2:
            if (pouchGetCoin() < *(s32*)((u8*)obj + 0x1A4)) {
                obj->unk1DC = 0xA;
                break;
            }
            if (*(EffectEntry**)((u8*)obj + 0x1C8) != NULL) {
                effSoftDelete(*(EffectEntry**)((u8*)obj + 0x1C8));
                *(EffectEntry**)((u8*)obj + 0x1C8) = NULL;
            }
            psndSFXOn_3D((const char*)0x1DA, &obj->position);
            animPoseSetAnim(obj->poseId, str_8041A11C, 1);
            if (*(s32*)((u8*)obj + 0x1A4) == 0) {
                obj->unk1DC = 3;
            } else {
                obj->unk1DC = 4;
                statusWinForceOpen();
            }
            *(s32*)((u8*)obj + 0x1B0) = 0x78;
            break;
        case 3:
            if (*(s32*)((u8*)obj + 0x1B0) == 100) {
                effStardustEntry(1, 0x1E, 0x3C, obj->position.x,
                                 float_45_8041a1f4 + obj->position.y, obj->position.z,
                                 float_50_8041a14c, float_50_8041a14c);
                psndSFXOn_3D((const char*)0x1DB, &obj->position);
            }
            if (*(s32*)((u8*)obj + 0x1B0) == 100) {
                MarioWork* mario = marioGetPtr();
                PouchData* pouch = pouchGetPtr();
                PouchPartyData* party = pouch->partyData;

                effStardustEntry(2, 0x32, 0x64, mario->position.x, mario->position.y, mario->position.z,
                                 float_50_8041a14c, float_50_8041a14c);
                marioChgPose(str_8041A1F8);
                pouch->currentHP = pouch->maximumHP;
                pouch->currentFP = pouch->maximumFP;
                for (i = 0; i < 8; i++) {
                    if ((party->flags & 1) && !(party->flags & 2)) {
                        party->currentHP = party->maximumHP;
                    }
                    party++;
                }
                pouchAddCoin(0);
            }
            *(s32*)((u8*)obj + 0x1B0) -= 1;
            if (*(s32*)((u8*)obj + 0x1B0) <= 0) {
                obj->unk1DC = 5;
            }
            break;
        case 4:
            if (*(s32*)((u8*)obj + 0x1B0) == 100) {
                effStardustEntry(1, 0x1E, 0x3C, obj->position.x,
                                 float_45_8041a1f4 + obj->position.y, obj->position.z,
                                 float_50_8041a14c, float_50_8041a14c);
                psndSFXOn_3D((const char*)0x1DB, &obj->position);
            }
            if (*(s32*)((u8*)obj + 0x1B0) == 100) {
                s32 coin = *(s32*)((u8*)obj + 0x1A4);
                MarioWork* mario = marioGetPtr();
                PouchData* pouch = pouchGetPtr();
                PouchPartyData* party = pouch->partyData;

                effStardustEntry(2, 0x32, 0x64, mario->position.x, mario->position.y, mario->position.z,
                                 float_50_8041a14c, float_50_8041a14c);
                marioChgPose(str_8041A1F8);
                pouch->currentHP = pouch->maximumHP;
                pouch->currentFP = pouch->maximumFP;
                for (i = 0; i < 8; i++) {
                    if ((party->flags & 1) && !(party->flags & 2)) {
                        party->currentHP = party->maximumHP;
                    }
                    party++;
                }
                pouchAddCoin(-coin);
            }
            *(s32*)((u8*)obj + 0x1B0) -= 1;
            if (*(s32*)((u8*)obj + 0x1B0) <= 0) {
                obj->unk1DC = 5;
            }
            break;
        case 5:
            if (animPoseGetLoopTimes(obj->poseId) < float_1_8041a160) {
                break;
            }
            animPoseSetAnim(obj->poseId, str_8041A118, 1);
            marioChgPose(str_8041A200);
            marioKeyOn();
            statusWinForceOff();
            camGetPtr(8)->flags &= ~0x200;
            obj->unk1DC++;
            break;
        case 6:
            if (*(EffectEntry**)((u8*)obj + 0x1C8) == NULL) {
                f32 y2 = float_25_8041a140;
                y2 += obj->position.y;
                *(EffectEntry**)((u8*)obj + 0x1C8) = effCoinFukidashiEntry(
                    0, *(s32*)&obj->unk1A0[4], 0,
                    obj->position.x, y2, obj->position.z - float_10_8041a1a4);
            }
            if (animPoseGetLoopTimes(obj->poseId) < float_1_8041a160) {
                break;
            }
            animPoseSetAnim(obj->poseId, str_8041A12C, 1);
            obj->flags &= ~8;
            obj->unk1DC = 0;
            break;
        case 10:
            *(s32*)((u8*)obj + 0x1A8) = msgWindow_Entry(msgSearch("sys_no_coin"), 0, 0);
            obj->unk1DC++;
            break;
        case 11:
            if (windowCheckID(*(s32*)((u8*)obj + 0x1A8)) == 0) {
                marioKeyOn();
                camGetPtr(8)->flags &= ~0x200;
                obj->flags &= ~8;
                obj->unk1DC = 0;
            }
            break;
        case 20:
            *(s32*)((u8*)obj + 0x1A8) = msgWindow_Entry(msgSearch("kaifuku_first_hit"), 0, 0);
            obj->unk1DC++;
            break;
        case 21:
            if (windowCheckID(*(s32*)((u8*)obj + 0x1A8)) == 0) {
                marioKeyOn();
                camGetPtr(8)->flags &= ~0x200;
                obj->flags &= ~8;
                obj->unk1DC = 0;
            }
            break;
    }
    return 0;
}

s32 evt_mobj_recovery_blk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    s32 type = evtGetValue(event, args[1]);
    f32 f31 = evtGetFloat(event, args[2]);
    f32 f30 = evtGetFloat(event, args[3]);
    f32 f29 = evtGetFloat(event, args[4]);
    s32 unk1E4;
    BOOL inBattle;
    const char* asyncName;
    MapObjectEntry* obj;

    evtGetValue(event, args[5]);
    unk1E4 = args[6];
    inBattle = gp->inBattle != 0;
    asyncName = (type == 0) ? "MOBJ_RecoveryBlock" : "MOBJ_CoinTakenRecoveryBlock";
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    *(s32*)((u8*)obj + 0x1A4) = type;
    mobjSetPosition(name, f31, f30, f29);
    obj->flags |= 4;
    obj->unk1D0 = mobj_recovery_blk;
    obj->unk1D4 = 0;
    obj->unk1D8 = 0;
    obj->flags |= 4;
    obj->flags |= 0x400000;
    obj->unk1E4 = unk1E4;
    if (type == 0) {
        obj->unk1DC = 0;
    } else {
        obj->unk1DC = -1;
    }
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    return 2;
}

s32 mobj_save_blk(MapObjectEntry* obj) {
    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                marioKeyOff();
                marioStSystemLevel(1);
                psndSFXOn_3D((const char*)0x1AF, &obj->position);
                animPoseSetAnim(obj->poseId, "A_1", 1);
                camGetPtr(8)->flags |= 0x200;
                obj->unk1DC++;
            }
            break;
        case 1:
            if (animPoseGetLoopTimes(obj->poseId) < float_1_8041a160) {
                break;
            }
            animPoseSetAnim(obj->poseId, "S_1", 1);
            if (obj->unk1D4 != NULL) {
                mobjRunEvent(obj, obj->unk1D4);
                obj->unk1DC++;
            } else {
                obj->unk1DC = 3;
            }
            break;
        case 2:
            if (evtCheckID(obj->eventId) != 1) {
                obj->eventId = 0;
                obj->unk1DC++;
            }
            break;
        case 3:
            mobjRunEvent(obj, obj->unk1D8);
            obj->unk1DC++;
            break;
        case 4:
            if (evtCheckID(obj->eventId) != 1) {
                marioStSystemLevel(0);
                camGetPtr(8)->flags &= ~0x200;
                obj->flags &= ~8;
                marioKeyOn();
                obj->unk1DC = 0;
            }
            break;
    }
    return 0;
}

s32 evt_mobj_save_blk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f31 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f29 = evtGetFloat(event, args[3]);
    s32 unk1D4 = evtGetValue(event, args[4]);
    s32 unk1E4 = args[5];
    const char* asyncName = "MOBJ_SaveBlock";
    MapObjectEntry* obj;
    BOOL inBattle = gp->inBattle != 0;

    if (gp->flags & 0x20) {
        return 2;
    }
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, f31, f30, f29);
    obj->unk1D0 = mobj_save_blk;
    obj->unk1D4 = (s32*)unk1D4;
    obj->unk1D8 = mobj_save_blk_sysevt;
    obj->flags |= 0x84;
    obj->flags |= 0x400000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    return 2;
}

// NONMATCHING (99.35%): the itemEntry argument setup schedules the evtId load before the fadds and the x
// load after it (target: x load, fadds, z load, evtId load); 2 replaced instructions, otherwise identical.
// The two-step `y` (below) already forces the 0.04 constant into f2, the target's register. The remaining
// reorder is the global optimizer hoisting the evtId load, not source-order scheduling: tried with no change
// -- direct/inline/commuted y sums, x/z/evtId locals (function and block scope, every declaration/assignment
// order), returning-inline helpers, comma-sequence forms, nested blocks, raw-offset and volatile reads, the
// opt_propagation/opt_lifetimes/opt_common_subs/opt_dead_assignments/scheduling/optimize_for_size/
// global_optimizer/defer_codegen pragmas, -O4,s, and MWCC 2.5/2.7/3.0a3/3.0a3p1/3.0a5 (all give this
// schedule). Same known-hard load-hoist class as _hammer_star_effect (battle_mario.c) and
// DrawSubMenuCommonProcess (battle_menu_disp.c).
s32 mobj_badgeblk(MapObjectEntry* obj) {


    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                if (strcmp(obj->animName, "MOBJ_BadgeBlock") == 0 ||
                    strcmp(obj->animName, "MOBJ_BrickBadgeBlock") == 0 ||
                    strcmp(obj->animName, "MOBJ_HiddenBadgeBlock") == 0) {
                    psndSFXOn_3D((const char*)0x1BB, &obj->position);
                } else {
                    psndSFXOn_3D((const char*)0x1BA, &obj->position);
                }
                if (obj->flags & 0x800) {
                    obj->flags &= ~0x800;
                }
                animPoseSetAnim(obj->poseId, "A_1", 1);
                if (obj->unk1D4 != NULL) {
                    mobjRunEvent(obj, obj->unk1D4);
                } else if (*(s32*)((u8*)obj + 0x1A0) != 0x79) {
                    ItemEntry* item;
#ifdef MUST_MATCH
                    // Two-step sum loads the 0.04 constant into f2 (the target's register) instead of f1.
                    f32 y = float_40_8041a1ec;

                    y += obj->position.y;
#else
                    f32 y = float_40_8041a1ec + obj->position.y;
#endif
                    itemEntry(obj->name, (ItemType)(*(s32*)((u8*)obj + 0x1A0)), 4, obj->unk1E4, 0,
                              obj->position.x, y, obj->position.z);
                    item = itemNameToPtr(obj->name);
                    itemFlagOn(item, 0x10);
                    *(f32*)((u8*)item + 0x80) = float_0p7_8041a1c8;
                    *(f32*)((u8*)item + 0x84) = 1.0f;
                    *(s32*)((u8*)item + 0x88) = 0;
                    *(f32*)((u8*)item + 0x58) = 360.0f;
                    psndSFXOn_3D((const char*)0x1BC, &obj->position);
                } else {
                    pouchAddCoin(1);
                    itemEntry(NULL, (ItemType)0x79, 0xf, -1, 0, obj->position.x, obj->position.y,
                              obj->position.z);
                    evtSetValue(NULL, obj->unk1E4, 1);
                    psndSFXOn_3D((const char*)0x5B, &obj->position);
                }
                obj->unk1DC++;
            }
            break;
        case 1:
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            animPoseSetAnim(obj->poseId, "S_2", 1);
            obj->flags &= ~8;
            obj->unk1DC = 0x5a;
            if (strcmp(obj->animName, "MOBJ_HatenaBlock") == 0 ||
                strcmp(obj->animName, "MOBJ_BrickHatenaBlock") == 0 ||
                strcmp(obj->animName, "MOBJ_HiddenHatenaBlock") == 0) {
                strcpy(obj->animName, "MOBJ_Block");
            } else {
                strcpy(obj->animName, "MOBJ_PinkBlock");
            }
            break;
        case 0x5a:
            if (obj->flags & 8) {
                obj->flags &= ~8;
                animPoseSetAnim(obj->poseId, "A_2", 1);
                psndSFXOn_3D((const char*)0x1B7, &obj->position);
                obj->unk1DC++;
            }
            break;
        case 0x5b:
            if (!(obj->flags & 8)) {
                obj->unk1DC = 0x5a;
                break;
            }
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            animPoseSetAnim(obj->poseId, "S_2", 1);
            obj->unk1DC = 0x5a;
            obj->flags &= ~8;
            break;
    }
    return 0;
}

s32 evt_mobj_badgeblk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f31 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f29 = evtGetFloat(event, args[3]);
    s32 unk1A0 = evtGetValue(event, args[4]);
    s32 unk1D4 = evtGetValue(event, args[5]);
    s32 unk1E4 = args[6];
    s32 var = args[7];
    BOOL inBattle;
    const char* asyncName;
    MapObjectEntry* obj;

    inBattle = gp->inBattle != 0;
    asyncName = (var != 0) ? "MOBJ_BadgeBlock" : "MOBJ_HatenaBlock";
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, f31, f30, f29);
    obj->unk1D0 = mobj_badgeblk;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 4;
    obj->flags |= 0xc10000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    *(s32*)obj->unk1A0 = unk1A0;
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x5a;
        animPoseSetAnim(obj->poseId, "S_2", 1);
        strcpy(obj->animName, (var != 0) ? "MOBJ_PinkBlock" : "MOBJ_Block");
    }
    return 2;
}

s32 mobj_powerupblk(MapObjectEntry* obj) {
#define PB_DOU02 "dou_02"
#define PB_DOU05 "dou_05"
#define PB_DOU10 "dou_10"
#define PB_BLK1 "pwrblk1"
#define PB_BLK2 "pwrblk2"

    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                if (marioKeyOffChk() != 0 || marioCtrlOffChk() != 0) {
                    obj->flags &= ~8;
                } else {
                    marioKeyOff();
                    if (strcmp(gp->currentMapName, PB_DOU02) == 0) {
                        marioCtrlOff();
                    } else if (strcmp(gp->currentMapName, PB_DOU05) == 0 ||
                               strcmp(gp->currentMapName, PB_DOU10) == 0) {
                        if (obj->unk1E0 == 0x800000) {
                            marioCtrlOff();
                        }
                    }
                    gp->systemLevelFlags |= 3;
                    psndSFXOn_3D((const char*)0x201C0, &obj->position);
                    animPoseSetAnim(obj->poseId, "A_1", 1);
                    itemEntry(PB_BLK1, (ItemType)0x57, 0xf, -1, 0, obj->position.x,
                              obj->position.y, obj->position.z);
                    obj->unk1DC++;
                }
            }
            break;
        case 1:
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            animPoseSetAnim(obj->poseId, "A_2", 1);
            obj->flags &= ~8;
            *(s32*)((u8*)obj + 0x1A4) = 1;
            obj->unk1DC++;
            break;
        case 2:
            if (*(s32*)((u8*)obj + 0x1A4) != 0) {
                (*(s32*)((u8*)obj + 0x1A4))--;
            } else {
                itemForceGet(itemEntry(PB_BLK2, (ItemType)0x57, 0x11, -1, 0,
                                       obj->position.x, float_40_8041a1ec + obj->position.y,
                                       obj->position.z));
                evtSetValue(NULL, obj->unk1E4, 1);
                marioKeyOn();
                if (strcmp(gp->currentMapName, PB_DOU02) == 0) {
                    marioCtrlOn();
                } else if (strcmp(gp->currentMapName, PB_DOU05) == 0 ||
                           strcmp(gp->currentMapName, PB_DOU10) == 0) {
                    if (obj->unk1E0 == 0x800000) {
                        marioCtrlOn();
                    }
                }
                obj->unk1DC = 0x5a;
            }
            break;
        case 0x5a:
            mobjDelete(obj->name);
            break;
    }
    return 0;
}
#undef PB_DOU02
#undef PB_DOU05
#undef PB_DOU10
#undef PB_BLK1
#undef PB_BLK2

s32 evt_mobj_powerupblk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f30 = evtGetFloat(event, args[1]);
    f32 f29 = evtGetFloat(event, args[2]);
    f32 f28 = evtGetFloat(event, args[3]);
    s32 unk1D4 = evtGetValue(event, args[4]);
    s32 unk1E4 = args[5];
    f32 f31 = evtGetFloat(event, args[6]);
    const char* asyncName = "MOBJ_PowerUpBlock";
    MapObjectEntry* obj;

    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, f30, f29, f28);
    obj->unk1D0 = mobj_powerupblk;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 4;
    obj->flags |= 0xc10000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    obj->rotation.y = f31;
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x5a;
        animPoseSetAnim(obj->poseId, "A_2", 1);
    }
    return 2;
}

// The name-buffer initializers emit their templates here, after save_first_hit.
// Prefix data precedes the following callback's compiler-generated name buffers.
#pragma force_active on
const char str_802C3EC8[0x10] = "o_";
const char str_802C3ED8[0x10] = "i_";
__declspec(section ".rodata") Vec lv_block_position[1] = {{0.0f, -2000.0f, 0.0f}};
// vec3_802c3ef4: placeholder for the object the code copies (defining it hangs MWCC)
__declspec(section ".rodata") Vec mobj_vec3_802c3ef4_template[1] = {{0.0f, 0.0f, 0.0f}};
__declspec(section ".rodata") char str_802C3F00[0x18] = "SFX_MOBJ_LOCK_SWING1";
__declspec(section ".rodata") char str_802C3F18[0x14] = "SFX_MOBJ_UNLOCK1";
__declspec(section ".rodata") char str_802C3F2C[0x20] = "SFX_MOBJ_TREASURE_BOX_SWING1";
__declspec(section ".rodata") char str_802C3F4C[0x1C] = "SFX_MOBJ_TREASURE_BOX_OPEN1";
__declspec(section ".rodata") char str_802C3F68[0x20] = "SFX_MOBJ_TREASURE_BOX_SWING2";
__declspec(section ".rodata") char str_802C3F88[0x1C] = "SFX_MOBJ_TREASURE_BOX_OPEN2";
__declspec(section ".rodata") char str_802C3FA4[0x1C] = "SFX_MOBJ_BLACKBOX_SWING1";
__declspec(section ".rodata") char str_802C3FC0[0x18] = "SFX_MOBJ_BLACKBOX_OPEN1";
__declspec(section ".rodata") static const Vec vec3_802c3fd8[1] = {{0.0f, 0.0f, 0.0f}};
__declspec(section ".rodata") static const Vec vec3_802c3fe4[1] = {{0.0f, 0.0f, 0.0f}};
__declspec(section ".rodata") char str_802C3FF0[0x10] = "save_first_hit";
#pragma force_active reset

s32 mobj_brickblk(MapObjectEntry* obj) {

    char offName[16] = "o_";
    char imgName[16] = "i_";
    BOOL inBattle;
    ImageEntry* img;

    inBattle = gp->inBattle != 0;

    strcat(offName, obj->name);
    strcat(imgName, obj->name);
    switch (obj->unk1DC) {
        case 0:
            animGroupBaseAsync("P_box_y", inBattle, NULL);
            if (obj->flags & 8) {
                mobjRunEvent(obj, obj->unk1D4);
                evtSetValue(NULL, obj->unk1E4, 1);
                obj->unk1DC = 0xa;
            }
            break;
        case 0xa: {
            s32 captureArgs1[10];
            EventEntry tmpEvent1;
            s32 captureArgs2[10];
            EventEntry tmpEvent2;

            animPaperPoseEntry("P_box_y", inBattle);
            offscreenEntry(offName);
            imgEntry(imgName, inBattle);
            *(s32*)((u8*)obj + 0x19C) = offscreenNameToId(offName);
            tmpEvent1.args = captureArgs1;
            captureArgs1[0] = (s32)imgName;
            captureArgs1[1] = 0;
            captureArgs1[2] = (s32)offName;
            captureArgs1[3] = 1;
            captureArgs1[4] = 0;
            captureArgs1[5] = 0;
            captureArgs1[6] = 0;
            captureArgs1[7] = 0;
            evt_img_alloc_capture(&tmpEvent1, 1);
            tmpEvent2.args = captureArgs2;
            captureArgs2[0] = (s32)imgName;
            captureArgs2[1] = 1;
            captureArgs2[2] = (s32)offName;
            captureArgs2[3] = 1;
            captureArgs2[4] = 0;
            captureArgs2[5] = 0;
            captureArgs2[6] = 0;
            captureArgs2[7] = 0;
            evt_img_alloc_capture(&tmpEvent2, 1);
            img = imgNameToPtr(imgName, inBattle);
            img->poseId = animPaperPoseGetId("P_box_y", inBattle);
            img->animName = "A_1";
            *(OSTime*)((u8*)img + 0x110) = animTimeGetTime(inBattle);
            imgSetShadow(img, 2);
            psndSFXOn_3D((const char*)0x1B8, &obj->position);
            obj->unk1DC = 0xb;
            break;
        }
        case 0xb:
            img = imgNameToPtr(imgName, inBattle);
            if (img->field_0x118 >= 1.0f) {
                obj->unk1DC = 0xc;
            }
            if (img->flags & 0x10000000) {
                obj->unk1DC = 0xc;
            }
            obj->position = vec3_802c4020[0];
            obj->flags |= 4;
            obj->flags |= 8;
            break;
        case 0xc:
            img = imgNameToPtr(imgName, inBattle);
            imgFreeCapture(img, 0);
            imgFreeCapture(img, 1);
            imgSetShadow(img, 0);
            imgRelease(img, inBattle);
            offscreenDelete(offName);
            *(s32*)((u8*)obj + 0x19C) = -1;
            obj->unk1DC = 0x63;
            break;
        case 0x63:
            mobjDelete(obj->name);
            break;
    }
    return 0;
}

s32 mobj_10countblk(MapObjectEntry* obj) {
    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                if (obj->flags & 0x800) {
                    obj->flags &= ~0x800;
                    strcpy(obj->animName, "MOBJ_Block");
                }
                animPoseSetAnim(obj->poseId, "A_1", 1);
                psndSFXOn_3D((const char*)0x1BE, &obj->position);
                evtSetValue(NULL, obj->unk1E4, 1);
                if (obj->unk1D4 != NULL) {
                    mobjRunEvent(obj, obj->unk1D4);
                } else {
                    pouchAddCoin(1);
                    itemEntry(NULL, (ItemType)0x79, 0xf, -1, 0, obj->position.x, obj->position.y,
                              obj->position.z);
                    *(s32*)((u8*)obj + 0x1B0) = 0xC8;
                }
                obj->unk1DC++;
                obj->flags &= ~8;
            }
            break;
        case 1:
            if (*(s32*)((u8*)obj + 0x1B0) > 0) {
                (*(s32*)((u8*)obj + 0x1B0))--;
            }
            if (*(s32*)((u8*)obj + 0x1B0) <= 0) {
                obj->unk1DC = 0x5a;
                animPoseSetAnim(obj->poseId, "S_2", 1);
                strcpy(obj->animName, "MOBJ_Block");
            } else if (obj->flags & 8) {
                psndSFXOn_3D((const char*)0x1BF, &obj->position);
                pouchAddCoin(1);
                itemEntry(NULL, (ItemType)0x79, 0xf, -1, 0, obj->position.x, obj->position.y,
                          obj->position.z);
                obj->flags &= ~8;
                animPoseSetAnim(obj->poseId, "A_1", 1);
            }
            break;
        case 0x5a:
            if (obj->flags & 8) {
                psndSFXOn_3D((const char*)0x1B7, &obj->position);
                animPoseSetAnim(obj->poseId, "A_3", 1);
                obj->flags &= ~8;
                obj->unk1DC++;
            }
            break;
        case 0x5b:
            if (!(obj->flags & 8)) {
                obj->unk1DC = 0x5a;
            } else {
                if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                    break;
                }
                animPoseSetAnim(obj->poseId, "S_2", 1);
                obj->unk1DC = 0x5a;
                obj->flags &= ~8;
            }
            break;
    }
    return 0;
}

s32 evt_mobj_brick(EventEntry* event, BOOL isFirstCall) {

    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    s32 unk1A0 = evtGetValue(event, args[4]);
    s32 type = args[5];
    s32 unk1D4 = evtGetValue(event, args[6]);
    s32 evtId = args[7];
    BOOL inBattle = gp->inBattle != 0;
    const char* animName;
    MapObjectEntry* obj;

    switch (type) {
        case 0:
            animName = "MOBJ_FlowBrickBlock";
            break;
        case 1:
            animName = "MOBJ_BrickHatenaBlock";
            break;
        case 2:
            animName = "MOBJ_BrickBadgeBlock";
            break;
        case 3:
            animName = "MOBJ_10CountBlock";
            break;
        case 0xa:
            animName = "MOBJ_HiddenHatenaBlock";
            break;
        case 0xb:
            animName = "MOBJ_HiddenHatenaBlock";
            break;
        case 0xc:
            animName = "MOBJ_HiddenBadgeBlock";
            break;
        case 0xd:
            animName = "MOBJ_Hidden10CountBlock";
            break;
    }
    if (!animGroupBaseAsync(animName, inBattle, NULL)) {
        return 0;
    }
    if (!animGroupBaseAsync("P_box_y", inBattle, NULL)) {
        return 0;
    }
    mobjEntry((const char*)name, animName);
    obj = mobjNameToPtr((const char*)name);
    mobjSetPosition((const char*)name, x, y, z);
    if (type >= 0xa) {
        obj->flags |= 0x800;
    }
    switch (type) {
        case 0:
        case 0xa:
            obj->unk1D0 = mobj_brickblk;
            break;
        case 1:
        case 0xb:
            obj->unk1D0 = mobj_badgeblk;
            break;
        case 2:
        case 0xc:
            obj->unk1D0 = mobj_badgeblk;
            break;
        case 3:
        case 0xd:
            obj->unk1D0 = mobj_10countblk;
            break;
    }
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 4;
    obj->flags |= 0xC10000;
    obj->unk1E4 = evtId;
    obj->unk1DC = 0;
    *(s32*)((u8*)obj + 0x1A0) = unk1A0;
    mobjCalcMtx(obj);
    if (type < 0xa) {
        animPoseSetAnim(obj->poseId, "S_1", 1);
    } else if (type == 0xd) {
        animPoseSetAnim(obj->poseId, "S_3", 1);
    } else {
        animPoseSetAnim(obj->poseId, "S_1", 1);
    }
    if (evtGetValue(event, evtId) != 0) {
        const char* newName = NULL;

        switch (type) {
            case 0:
            case 0xa:
                animPoseSetAnim(obj->poseId, "S_1", 1);
                obj->unk1DC = 0x63;
                break;
            case 1:
            case 0xb:
                animPoseSetAnim(obj->poseId, "S_2", 1);
                obj->unk1DC = 0x5a;
                newName = "MOBJ_Block";
                break;
            case 2:
            case 0xc:
                animPoseSetAnim(obj->poseId, "S_2", 1);
                obj->unk1DC = 0x5a;
                newName = "MOBJ_PinkBlock";
                break;
            case 3:
            case 0xd:
                animPoseSetAnim(obj->poseId, "S_2", 1);
                obj->unk1DC = 0x5a;
                newName = "MOBJ_Block";
                break;
        }
        if (newName != NULL) {
            strcpy(obj->animName, newName);
        }
        obj->flags &= ~0x800;
    }
    return 2;
}

// The original's `psndSFXOn_3D` argument is the literal 0x401E8.
s32 mobj_breaking_floor(MapObjectEntry* obj) {
    GXColor color2;
    GXColor color;

    marioGetPtr();
    color = *(GXColor*)&dat_8041a130;
    color2 = *(GXColor*)&dat_8041a130;
    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                psndSFXOn_3D((const char*)0x401E8, &obj->position);
                animPoseSetAnim(obj->poseId, "A_1", 1);
                if (obj->unk1D4 != NULL) {
                    mobjRunEvent(obj, obj->unk1D4);
                }
                if (obj->hitObj[0].hitObject != NULL) {
                    *(u16*)obj->hitObj[0].hitObject |= 1;
                }
                obj->flags |= 2;
                evtSetValue(NULL, obj->unk1E4, 1);
                obj->unk1DC = 1;
                animPoseSetMaterialFlagOn(obj->poseId, 0x40);
                animPoseSetMaterialEvtColor(obj->poseId, color);
                *(s32*)((u8*)obj + 0x1B0) = 0x40;
            }
            break;
        case 1: {

            *(u32*)&color2 = animPoseGetMaterialEvtColor(obj->poseId);
            if (color2.a >= 4) {
                color2.a -= 4;
            } else {
                color2.a = 0;
                obj->unk1DC = 0x63;
            }
            animPoseSetMaterialEvtColor(obj->poseId, color2);
            *(s32*)((u8*)obj + 0x1B0) -= 1;
            if (*(s32*)((u8*)obj + 0x1B0) <= 0) {
                obj->unk1DC = 0x63;
            }
            break;
        }
        case 0x63:
            mobjDelete(obj->name);
            break;
    }
    return 0;
}

s32 evt_mobj_breaking_floor(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f31 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f29 = evtGetFloat(event, args[3]);
    s32 type = evtGetValue(event, args[4]);
    s32 unk1D4 = evtGetValue(event, args[5]);
    s32 unk1E4 = args[6];
    BOOL inBattle = gp->inBattle != 0;
    char animName[0x80];
    MapObjectEntry* obj;

    switch (type) {
        case 0:
            strcpy(animName, "MOBJ_BreakingFloor");
            break;
        case 1:
            strcpy(animName, "MOBJ_BlackBreakingFloor");
            break;
        default:
            strcpy(animName, "MOBJ_BreakingFloor");
            break;
    }
    if (!animGroupBaseAsync(animName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, animName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, f31, f30, f29);
    obj->unk1D0 = mobj_breaking_floor;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 4;
    obj->flags |= 0x1000000;
    obj->unk1E4 = unk1E4;
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x63;
    } else {
        obj->unk1DC = 0;
    }
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    return 2;
}

// NONMATCHING (90.09%): the compiler now shares both conversion doubles with the
// read-only pool; floating-point register allocation and scheduling still differ.
void kururing_capture(CameraId cameraId, void* param) {
    MapObjectEntry* obj = param;

    CameraEntry save;
    CameraEntry* cam;
    AnimationPoseData* animData;
    Vec bboxMin;
    Vec bboxMax;
    Vec minTmp;
    Vec maxTmp;
    u16 texWidth;
    u16 texHeight;
    f32 t;
    f32 ten = 10.0f; // local keeps the float_10 load in the target's f3 (the named extern hoists to a callee-saved FPR)

    memcpy(&save, camGetPtr(cameraId), 0x260);
    animData = animPoseGetAnimBaseDataPtr(obj->poseId);
    minTmp = vec3_802c402c[0];
    minTmp.x = animData->bboxMin.x;
    minTmp.y = animData->bboxMin.y;
    minTmp.z = animData->bboxMin.z;
    bboxMin = minTmp;
    maxTmp = vec3_802c4038[0];
    maxTmp.x = animData->bboxMax.x;
    maxTmp.y = animData->bboxMax.y;
    maxTmp.z = animData->bboxMax.z;
    bboxMax = maxTmp;
    texWidth = (bboxMax.x - bboxMin.x) * ten;
    texHeight = (bboxMax.z - bboxMin.z) * ten;
    cam = camGetPtr(cameraId);
    texWidth = (texWidth + 1) & ~1;
    texHeight = (texHeight + 1) & ~1;
    cam->cameraUp = vec3_802c4044[0];
    cam->cameraAt = obj->position;
    cam->cameraPos = obj->position;
    t = (f32)tan(0.21816615760326385);
    cam->cameraPos.y += 0.5f * (f32)texHeight / t;
    C_MTXPerspective(cam->projection, float_25_8041a140, (f32)(texWidth / texHeight),
                     float_1_8041a160, float_32767_8041a1e4);
    cam->projType = GX_PERSPECTIVE;
    C_MTXLookAt(cam->view, &cam->cameraPos, &cam->cameraUp, &cam->cameraAt);
    GXSetProjection(cam->projection, cam->projType);
    GXSetViewport(float_0_8041a150, float_0_8041a150, (f32)texWidth, (f32)texHeight, float_0_8041a150,
                  float_1_8041a160);
    GXSetScissor(0, 0, texWidth, texHeight);
    test_kururing_mapdisp(cameraId);
    sysWaitDrawSync();
    obj->unk208 = smartAlloc(GXGetTexBufferSize(texWidth / 2, texHeight / 2, 6, 0, 0), 3);
    GXSetTexCopySrc(0, 0, texWidth, texHeight);
    GXSetTexCopyDst(texWidth / 2, texHeight / 2, 6, 1);
    GXSetZMode(1, 3, 1);
    GXCopyTex(((smartEntry*)obj->unk208)->address, 1);
    GXInvalidateTexAll();
    GXPixModeSync();
    GXTexModeSync();
    GXInitTexObj(&obj->paperTexObj, ((smartEntry*)obj->unk208)->address, texWidth / 2,
                 texHeight / 2, 6, 0, 0, 0);
    GXInitTexObjLOD(&obj->paperTexObj, 0, 0, float_0_8041a150, float_0_8041a150, float_0_8041a150, 0,
                    0, 0);
    PSMTXIdentity(obj->paperTexMtx);
    memcpy(camGetPtr(cameraId), &save, 0x260);
}

// Data externs / prototypes for this function only.

static inline BOOL mobjIsInRange(f32 cx, f32 cz, f32 range, f32 x, f32 z) {
    if (cx - range <= x && x <= cx + range && cz - range <= z && z <= cz + range) {
        return TRUE;
    }
    return FALSE;
}

// NONMATCHING (99.94%): range-box checks, state machine, literals and the itemEntry schedule match. Residue is
// the global allocator colouring posBuf r28 where the target uses r30 (6 instructions).
s32 mobj_kururing_floor(MapObjectEntry* obj) {
    MarioWork* mario = marioGetPtr();
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    s32 flag;
    f32 f31;
    s32 i;
    ItemEntry* item;
    NpcWork* npcWork;
    NpcEntry* entry;
    u8* posBuf;
    PartyEntry* party2;

    switch (obj->unk1DC) {
        case 0:
            if (!(obj->flags & 8)) {
                break;
            }
            f31 = distABf(mario->position.x, mario->position.z, obj->position.x, obj->position.z);
            if (__fabsf(mario->position.y - obj->position.y) > float_10_8041a1a4) {
                f31 = float_1E05_8041a1d8;
            }
            if (obj->unk1E0 == 0x01000000) {
                if (pouchGetJumpLv() < 2) {
                    flag = 0;
                } else if (f31 < float_60_8041a1ac) {
                    flag = 2;
                } else if (f31 < float_100_8041a148) {
                    flag = 1;
                } else {
                    flag = 0;
                }
            }
            if (obj->unk1E0 == 0x00010000) {
                flag = 1;
            }
            if (flag == 0) {
                obj->flags &= ~8;
                break;
            }
            {
                f32 objx = obj->position.x;
                if (mobjIsInRange(objx, obj->position.z, 30.0f, mario->position.x, mario->position.z)) {
                    flag = 1;
                }
                if (party != NULL &&
                    mobjIsInRange(objx, obj->position.z, 30.0f, party->position.x, party->position.z)) {
                    flag = 1;
                }
            }
            if (itemHitCheck(obj->position.x, obj->position.y, obj->position.z, float_42p42_8041a1dc) != NULL) {
                flag = 1;
            }
            if (npcNearDistCheck(obj->position.x, obj->position.y, obj->position.z, float_42p42_8041a1dc) != NULL) {
                flag = 1;
            }
            if (flag == 0) {
                obj->flags &= ~8;
                break;
            }
            if (flag == 1) {
                animPoseSetAnim(obj->poseId, str_8041A118, 1);
                psndSFXOnEx_3D((const char*)0x1ee, &obj->position, 0xff, 0xff, 0x64, 0x10);
                obj->unk1DC = 0xa;
            } else if (flag == 2) {
                animPoseSetAnim(obj->poseId, str_8041A11C, 1);
                psndSFXOn_3D((const char*)0x1ef, &obj->position);
                obj->unk1DC = 1;
                item = itemNameToPtr(obj->name);
                if (item == NULL || !(item->flags & 1)) {
                    f32 y = float_30_8041a1a8;

                    y += obj->position.y;
#ifdef MUST_MATCH
                    // The target evaluates the floats before evtId: `itemEntry`'s declared parameter order must be
                    // (name, type, a3, f32, f32, f32, evtId, a5), not the header's (name, type, a3, evtId, a5, f32...).
                    // The registers are identical either way; only the evaluation order (and so the schedule)
                    // differs. Reported in REPORT.md; this cast reproduces the target order until the header is fixed.
                    if (((ItemEntry* (*)(const char*, ItemType, u16, f32, f32, f32, s32, s32))itemEntry)(
                            obj->name, 0x7d, 4, obj->position.x, y, obj->position.z, obj->unk1E4, 0) != NULL) {
#else
                    if (itemEntry(obj->name, 0x7d, 4, obj->unk1E4, 0, obj->position.x,
                                  y, obj->position.z) != NULL) {
#endif
                        item = itemNameToPtr(obj->name);
                        itemFlagOn(item, 0x10);
                        *(f32*)&item->field_0x70[0x10] = 0.6f;
                        *(f32*)&item->field_0x70[0x14] = 1.0f;
                        *(s32*)&item->field_0x70[0x18] = 5;
                        *(f32*)&item->field_0x4D[0xB] = 300.0f;
                    }
                }
            }
            animGetPtr()->poses[obj->poseId].flags |= 0x10;
            obj->flags |= 0x400;
            dispEntry(2, 0.0f, 0, kururing_capture, obj);
            break;

        case 1:
            animGetPtr()->poses[obj->poseId].flags |= 0x10;
            obj->flags |= 0x400;
            dispEntry(2, 0.0f, 0, kururing_capture, obj);
            if (animPoseGetLoopTimes(obj->poseId) < float_1_8041a160) {
                break;
            }
            animGetPtr()->poses[obj->poseId].flags &= ~0x10;
            obj->flags &= ~0x400;
            obj->flags &= ~8;
            animPoseSetAnim(obj->poseId, str_8041A12C, 1);
            psndSFXOn_3D((const char*)0x1f0, &obj->position);
            obj->unk1DC = 0;
            break;

        case 0xa:
            animGetPtr()->poses[obj->poseId].flags |= 0x10;
            obj->flags |= 0x400;
            dispEntry(2, 0.0f, 0, kururing_capture, obj);
            posBuf = (u8*)((smartEntry*)*(void**)((u8*)animPoseGetAnimPosePtr(obj->poseId) + 0x15C))->address;
            mario = marioGetPtr();
            if (mobjIsInRange(obj->position.x, obj->position.z, 30.0f, mario->position.x, mario->position.z)) {
                mario->position.y = *(f32*)(posBuf + 0x10);
            }
            party2 = partyGetPtr(marioGetPartyId());
            if (party2 != NULL &&
                mobjIsInRange(obj->position.x, obj->position.z, 30.0f, party2->position.x, party2->position.z)) {
                party2->position.y = *(f32*)(posBuf + 0x10);
            }
            party2 = partyGetPtr(marioGetExtraPartyId());
            if (party2 != NULL &&
                mobjIsInRange(obj->position.x, obj->position.z, 30.0f, party2->position.x, party2->position.z)) {
                party2->position.y = *(f32*)(posBuf + 0x10);
            }
            npcWork = npcGetWorkPtr();
            entry = npcWork->entries;
            for (i = 0; i < npcWork->total; i++, entry++) {
                if (entry->flags & 1) {
                    if (mobjIsInRange(obj->position.x, obj->position.z, 42.420002f, entry->position.x, entry->position.z)) {
                        entry->position.y = *(f32*)(posBuf + 0x10);
                    }
                }
            }
            item = itemNearDistCheck(obj->position.x, obj->position.y, obj->position.z, float_42p42_8041a1dc);
            if (item != NULL) {
                item->position.y = *(f32*)(posBuf + 0x10);
            }
            if (animPoseGetLoopTimes(obj->poseId) < float_2_8041a190) {
                break;
            }
            animGetPtr()->poses[obj->poseId].flags &= ~0x10;
            obj->flags &= ~0x400;
            animPoseSetAnim(obj->poseId, str_8041A12C, 1);
            psndSFXOn_3D((const char*)0x1f0, &obj->position);
            *(s32*)&obj->unk1A0[0x10] = 0x14;
            obj->unk1DC++;
            /* fallthrough */
        case 0xb:
            (*(s32*)&obj->unk1A0[0x10])--;
            if (*(s32*)&obj->unk1A0[0x10] == 0) {
                obj->unk1DC++;
            }
            break;

        case 0xc:
            obj->flags &= ~8;
            obj->unk1DC = 0;
            break;
    }
    return 0;
}

s32 evt_mobj_kururing_floor(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f29 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f31 = evtGetFloat(event, args[3]);
    s32 r30 = evtGetValue(event, args[4]);
    s32 unk1E4;
    MapObjectEntry* obj;
    Vec pos;
    Vec scale;
    const char* asyncName;

    evtGetValue(event, args[5]);
    asyncName = "MOBJ_KururinFloor";
    unk1E4 = args[6];
    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
#ifdef MUST_MATCH
    pos = mobj_vec3_802c4050_template[0];
#else
    pos = vec3_802c4050[0];
#endif
    pos.x = f29;
    pos.y = f30 + 0.1f;
    pos.z = f31;
    obj->position = pos;
    obj->unk1D0 = mobj_kururing_floor;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    obj->flags |= 0x1010000;
    obj->flags |= 2;
    mobjCalcMtx(obj);
#ifdef MUST_MATCH
    scale = mobj_vec3_802c405c_template[0];
#else
    scale = vec3_802c405c[0];
#endif
    scale.y = obj->scale2.y;
    obj->scale2 = scale;
    mobjCalcMtx2(obj);
    mapGrpFlagOn((const char*)r30, 0x400000);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    animPoseWorldPositionEvalOn(obj->poseId);
    return 2;
}

#ifdef MUST_MATCH
// Retail retains these unused animation names between function literal pools.
static const char* pool_trap(void) { return "MOBJ_trap"; }
static const char* pool_offBomb(void) { return "OFF_d_bom"; }
static const char* pool_bombRock(void) { return "MOBJ_BombRock"; }
#endif

s32 kpa_powerup(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    Vec pos;

    pos.x = evtGetFloat(event, args[0]);
    pos.y = evtGetFloat(event, args[1]);
    pos.z = evtGetFloat(event, args[2]);
    kpaAddScorePos(500, &pos);
    if (kpaGetLevel() == 3) {
        kpaAddScore(1000);
    }
    kpaPowUp();
    return 2;
}

s32 mobj_koopa_ojama_blk_evt(MapObjectEntry* obj) {
    s32 flag = 0;

    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                switch (kpaGetLevel()) {
                    case 0:
                    case 1:
                        if (obj->unk1E0 == 0x20000000 || obj->unk1E0 == 0x4000000) {
                            flag = 1;
                        }
                        break;
                    case 2:
                        if (obj->unk1E0 == 0x20000000 || obj->unk1E0 == 0x10000000 ||
                            obj->unk1E0 == 0x4000000) {
                            flag = 1;
                        }
                        break;
                    case 3:
                        flag = 1;
                        break;
                }
                if (flag == 0) {
                    obj->flags &= ~8;
                } else {
                    mobjRunEvent(obj, obj->unk1D4);
                    evtSetValue(NULL, obj->unk1E4, 1);
                    if (obj->unk1E0 == 0x4000000) {
                        npcKoopaModeMobjBoundDeadCheck(obj->hitObj[0].hitObject);
                    }
                    if (obj->hitObj[0].hitObject != NULL) {
                        *(u16*)obj->hitObj[0].hitObject |= 1;
                    }
                    obj->flags |= 2;
                    kpaAddScore(kpaMutekiCheck() ? 500 : 400);
                    obj->unk1DC = 0xa;
                }
            }
            break;
        case 0xa: {
            ItemEntry* item;

            psndSFXOn_3D((const char*)0x1C4, &obj->position);
            effMObjBrokenEntry(5, obj->position.x, float_25_8041a140 + obj->position.y,
                               float_25_8041a140 + obj->position.z);
            if ((item = itemEntry(obj->name, (ItemType)0x59, 4, -1, (s32)kpa_powerup_evt,
                                  obj->position.x, float_25_8041a140 + obj->position.y,
                                  obj->position.z)) != NULL) {
                itemStatusOn(item, 0x10);
                itemFlagOn(item, 0x14);
                *(s32*)((u8*)item + 0x7C) = 0x2710;
                *(s32*)((u8*)item + 0x78) = 0;
                *(f32*)((u8*)item + 0x80) = 0.45f;
                *(f32*)((u8*)item + 0x84) = 0.7f;
                *(s32*)((u8*)item + 0x88) = 0;
                *(f32*)((u8*)item + 0x58) = 500.0f;
                *(f32*)((u8*)item + 0x50) = 120.0f;
                *(s32*)((u8*)item + 0x54) = 0x5a;
                psndSFXOn_3D((const char*)0x836, &obj->position);
            }
            obj->unk1DC = 0x63;
            break;
        }
        case 0x63:
            mobjDelete(obj->name);
            break;
    }
    return 0;
}

s32 evt_mobj_koopa_ojama_blk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f29 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f31 = evtGetFloat(event, args[3]);
    s32 unk1D4 = evtGetValue(event, args[4]);
    s32 unk1E4 = args[5];
    const char* asyncName = (const char*)evtGetValue(event, args[6]);
    MapObjectEntry* obj;
    Vec pos;

    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    obj->flags |= 4;
    pos = vec3_802c40a0[0];
    pos.x = f29;
    pos.y = f30;
    pos.z = f31;
    obj->position = pos;
    obj->unk1D0 = mobj_koopa_ojama_blk_evt;
    obj->unk1D4 = (s32*)unk1D4;
    obj->unk1D8 = 0;
    obj->flags |= 0x3c000000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x63;
    }
    return 2;
}

s32 mobj_koopa_stone_blk_evt(MapObjectEntry* obj) {
    return 0;
}

s32 evt_mobj_koopa_stone_blk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    s32 unk1D4 = evtGetValue(event, args[4]);
    s32 unk1E4 = args[5];
    const char* asyncName = (const char*)evtGetValue(event, args[6]);
    MapObjectEntry* obj;
    Vec pos;

    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    obj->flags |= 4;
    pos = vec3_802c40ac[0];
    pos.x = x;
    pos.y = y;
    pos.z = z;
    obj->position = pos;
    obj->unk1D0 = mobj_koopa_stone_blk_evt;
    obj->unk1D4 = (s32*)unk1D4;
    obj->unk1D8 = 0;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0x63;
    return 2;
}

s32 mobj_koopa_brickblk(MapObjectEntry* obj) {
    BOOL flag = FALSE;


    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                switch (kpaGetLevel()) {
                    case 0:
                        if (obj->unk1E0 == 0x20000000) {
                            flag = TRUE;
                        }
                        break;
                    case 1:
                        if (obj->unk1E0 == 0x20000000 || obj->unk1E0 == 0x4000000) {
                            flag = TRUE;
                        }
                        break;
                    case 2:
                        if (obj->unk1E0 == 0x20000000 || obj->unk1E0 == 0x10000000 ||
                            obj->unk1E0 == 0x4000000) {
                            flag = TRUE;
                        }
                        break;
                    case 3:
                        flag = TRUE;
                        break;
                }
                if (!flag) {
                    if (obj->unk1E0 == 0x4000000) {
                        npcKoopaModeMobjBoundDeadCheck(obj->hitObj[0].hitObject);
                        obj->unk1DC = 0x14;
                    } else {
                        obj->flags &= ~8;
                    }
                } else {
                    mobjRunEvent(obj, obj->unk1D4);
                    evtSetValue(NULL, obj->unk1E4, 1);
                    if (obj->unk1E0 == 0x4000000) {
                        npcKoopaModeMobjBoundDeadCheck(obj->hitObj[0].hitObject);
                    }
                    if (obj->hitObj[0].hitObject != NULL) {
                        obj->hitObj[0].hitObject->flags |= 1;
                    }
                    obj->flags |= 2;
                    kpaAddScore(kpaMutekiCheck() ? 500 : 400);
                    obj->unk1DC = 0xa;
                }
            }
            break;
        case 0xa:
            psndSFXOn_3D((const char*)0x1B8, &obj->position);
            if (strcmp(obj->animName, "MOBJ_FlowBrickBlock") == 0) {
                effMObjBrokenEntry(0, obj->position.x, obj->position.y + 12.5f,
                                   obj->position.z + 12.5f);
            } else if (strcmp(obj->animName, "MOBJ_block_ky") == 0) {
                effMObjBrokenEntry(1, obj->position.x, obj->position.y + 12.5f,
                                   obj->position.z + 12.5f);
            } else if (strcmp(obj->animName, "MOBJ_bigblock_01") == 0) {
                effMObjBrokenEntry(3, obj->position.x, obj->position.y + 25.0f,
                                   obj->position.z + 25.0f);
            } else if (strcmp(obj->animName, "MOBJ_bigblock_02") == 0) {
                effMObjBrokenEntry(4, obj->position.x, obj->position.y + 25.0f,
                                   obj->position.z + 25.0f);
            }
            obj->unk1DC = 0x63;
            break;
        case 0x14:
            psndSFXOn_3D((const char*)0x1B7, &obj->position);
            animPoseSetAnim(obj->poseId, "A_1", 1);
            obj->unk1DC = 0x15;
            break;
        case 0x15:
            if (!(obj->flags & 8)) {
                obj->unk1DC = 0;
                break;
            }
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            animPoseSetAnim(obj->poseId, "S_1", 1);
            obj->unk1DC = 0;
            obj->flags &= ~8;
            break;
        case 0x63:
            mobjDelete(obj->name);
            break;
    }
    return 0;
}

s32 evt_mobj_koopa_brick(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f31 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f29 = evtGetFloat(event, args[3]);
    s32 unk1A0 = evtGetValue(event, args[4]);
    s32 type = args[5];
    s32 unk1D4 = evtGetValue(event, args[6]);
    s32 unk1E4 = args[7];
    BOOL inBattle = gp->inBattle != 0;
    const char* asyncName;
    MapObjectEntry* obj;

    switch (type) {
        case 0:
            asyncName = "MOBJ_FlowBrickBlock";
            break;
        case 1:
            asyncName = "MOBJ_block_ky";
            break;
        case 2:
            asyncName = "MOBJ_bigblock_01";
            break;
        case 3:
        default:
            asyncName = "MOBJ_bigblock_02";
            break;
    }
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, f31, f30, f29);
    obj->unk1D0 = mobj_koopa_brickblk;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 4;
    obj->flags |= 0x3c000000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    *(s32*)obj->unk1A0 = unk1A0;
    mobjCalcMtx(obj);
#ifdef MUST_MATCH
    animPoseSetAnim(obj->poseId, str_8041A12C, 1);
#else
    animPoseSetAnim(obj->poseId, "S_1", 1);
#endif
    if (evtGetValue(event, unk1E4) != 0) {
#ifdef MUST_MATCH
        animPoseSetAnim(obj->poseId, str_8041A12C, 1);
#else
        animPoseSetAnim(obj->poseId, "S_1", 1);
#endif
        obj->unk1DC = 0x63;
    }
    return 2;
}

s32 mobj_koopa_badgeblk(MapObjectEntry* obj) {
    BOOL flag;

    marioGetPtr();
    flag = FALSE;
    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                if (obj->unk1E0 != 0x4000000) {
                    obj->flags &= ~8;
                } else {
                    if (obj->flags & 0x800) {
                        mobjHitEntry(obj, 0);
                    }
                    animPoseSetAnim(obj->poseId, "A_1", 1);
                    if (obj->unk1D4 != NULL) {
                        mobjRunEvent(obj, obj->unk1D4);
                    } else {
                        if (*(s32*)((u8*)obj + 0x1A0) == 0x59) {
                            ItemEntry* item;
                            itemEntry(obj->name, (ItemType)(*(s32*)((u8*)obj + 0x1A0)), 4, -1,
                                      (s32)kpa_powerup_evt, obj->position.x,
                                      float_12p5_8041a178 + obj->position.y, obj->position.z);
                            item = itemNameToPtr(obj->name);
                            itemStatusOn(item, 0x10);
                            itemFlagOn(item, 0x14);
                            *(s32*)((u8*)item + 0x7C) = 0x2710;
                            *(s32*)((u8*)item + 0x78) = 0;
                            *(f32*)((u8*)item + 0x80) = 0.45f;
                            *(f32*)((u8*)item + 0x84) = 0.7f;
                            *(s32*)((u8*)item + 0x88) = 0;
                            *(f32*)((u8*)item + 0x58) = 500.0f;
                            *(f32*)((u8*)item + 0x50) = 120.0f;
                            *(s32*)((u8*)item + 0x54) = 0x5a;
                            psndSFXOn_3D((const char*)0x836, &obj->position);
                        } else {
                            kpaAddCoin(1);
                            itemEntry(NULL, (ItemType)0x79, 0xf, -1, 0, obj->position.x, obj->position.y,
                                      obj->position.z);
                            evtSetValue(NULL, obj->unk1E4, 1);
                            psndSFXOn_3D((const char*)0x5B, &obj->position);
                        }
                    }
                    if (obj->unk1E0 == 0x4000000) {
                        npcKoopaModeMobjBoundDeadCheck(obj->hitObj[0].hitObject);
                    }
                    obj->unk1DC++;
                }
            }
            break;
        case 1:
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            animPoseSetAnim(obj->poseId, "S_2", 1);
            obj->flags &= ~8;
            obj->unk1DC = 0x5a;
            strcpy(obj->animName, "MOBJ_Block");
            break;
        case 0x5a:
            if (obj->flags & 8) {
                // MUST_MATCH: the empty case 2 makes MWCC emit the retail switch decision tree
                // (cmpwi/beq/bge/b) instead of folding the two default edges together.
#ifdef MUST_MATCH
                switch (kpaGetLevel()) {
                    case 2:
                        break;
                    case 3:
                        flag = TRUE;
                        break;
                }
#else
                if (kpaGetLevel() == 3) {
                    flag = TRUE;
                }
#endif
                if (!flag) {
                    obj->flags &= ~8;
                } else {
                    if (obj->unk1E0 == 0x4000000) {
                        npcKoopaModeMobjBoundDeadCheck(obj->hitObj[0].hitObject);
                    }
                    if (obj->hitObj[0].hitObject != NULL) {
                        obj->hitObj[0].hitObject->flags |= 1;
                    }
                    obj->flags |= 2;
                    obj->unk1DC++;
                }
            }
            break;
        case 0x5b:
            psndSFXOn_3D((const char*)0x1B8, &obj->position);
            effMObjBrokenEntry(2, obj->position.x, float_12p5_8041a178 + obj->position.y,
                               float_12p5_8041a178 + obj->position.z);
            obj->unk1DC = 0x63;
            break;
        case 0x63:
            mobjDelete(obj->name);
            break;
    }
    return 0;
}

// NONMATCHING (99.80%): only the gp/string-address temp colouring is swapped (retail gp=r5/str=r4,
// ours gp=r4/str=r5); `!!gp->inBattle` fixed the scheduling.
s32 evt_mobj_koopa_badgeblk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f31 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f29 = evtGetFloat(event, args[3]);
    s32 unk1A0 = evtGetValue(event, args[4]);
    s32 unk1D4 = evtGetValue(event, args[5]);
    s32 unk1E4 = args[6];
    const char* asyncName = "MOBJ_HatenaBlock";
    MapObjectEntry* obj;

    if (!animGroupBaseAsync(asyncName, !!gp->inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, f31, f30, f29);
    obj->unk1D0 = mobj_koopa_badgeblk;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 4;
    obj->flags |= 0x3c000000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    *(s32*)obj->unk1A0 = unk1A0;
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x5a;
        animPoseSetAnim(obj->poseId, "S_2", 1);
    }
    return 2;
}

// NONMATCHING (99.81%): only the gp/string-address temp colouring is swapped (retail gp=r5/str=r4,
// ours gp=r4/str=r5); `!!gp->inBattle` fixed the scheduling.
s32 evt_mobj_koopa_hidden_blk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f31 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f29 = evtGetFloat(event, args[3]);
    s32 unk1A0 = evtGetValue(event, args[4]);
    s32 unk1D4 = evtGetValue(event, args[6]);
    s32 unk1E4 = args[7];
    const char* asyncName = "MOBJ_HiddenHatenaBlock";
    MapObjectEntry* obj;

    if (!animGroupBaseAsync(asyncName, !!gp->inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, f31, f30, f29);
    obj->flags |= 0x800;
    mobjHitEntry(obj, 1);
    obj->unk1D0 = mobj_koopa_badgeblk;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 4;
    obj->flags |= 0x3c000000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    *(s32*)obj->unk1A0 = unk1A0;
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x5a;
        animPoseSetAnim(obj->poseId, "S_2", 1);
    }
    return 2;
}

s32 mobj_koopa_blk(MapObjectEntry* obj) {
    s32 flag = 0;

    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                // MUST_MATCH: the empty case 2 makes MWCC emit the retail switch decision tree
                // (cmpwi/beq/bge/b) instead of folding the two default edges together.
#ifdef MUST_MATCH
                switch (kpaGetLevel()) {
                    case 2:
                        break;
                    case 3:
                        flag = 1;
                        break;
                }
#else
                if (kpaGetLevel() == 3) {
                    flag = 1;
                }
#endif
                if (flag == 0) {
                    obj->flags &= ~8;
                } else {
                    mobjRunEvent(obj, obj->unk1D4);
                    evtSetValue(NULL, obj->unk1E4, 1);
                    if (obj->unk1E0 == 0x4000000) {
                        npcKoopaModeMobjBoundDeadCheck(obj->hitObj[0].hitObject);
                    }
                    if (obj->hitObj[0].hitObject != NULL) {
                        *(u16*)obj->hitObj[0].hitObject |= 1;
                    }
                    obj->flags |= 2;
                    obj->unk1DC = 0xa;
                }
            }
            break;
        case 0xa:
            psndSFXOn_3D((const char*)0x1b8, &obj->position);
            effMObjBrokenEntry(2, obj->position.x, float_12p5_8041a178 + obj->position.y,
                                 float_12p5_8041a178 + obj->position.z);
            obj->unk1DC = 0x63;
            /* fall through */
        case 0x63:
            mobjDelete(obj->name);
            break;
    }
    return 0;
}

s32 evt_mobj_koopa_blk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f31 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f29 = evtGetFloat(event, args[3]);
    s32 unk1D4 = evtGetValue(event, args[5]);
    s32 unk1E4 = args[6];
    MapObjectEntry* obj;

    if (!animGroupBaseAsync("MOBJ_Block", gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, "MOBJ_Block");
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, f31, f30, f29);
    obj->unk1D0 = mobj_koopa_blk;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 4;
    obj->flags |= 0x3c000000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    return 2;
}

// opt_common_subs off: with CSE on, MWCC hoists the pooled 25.0f load above the position.x load in
// the final effMObjBrokenEntry call; off keeps the original load order.
#pragma opt_common_subs off
s32 mobj_koopa_dokan_evt(MapObjectEntry* obj) {
    BOOL flag = FALSE;

    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                switch (kpaGetLevel()) {
                    case 0:
                        break;
                    case 1:
                    case 2:
                        if (obj->unk1E0 == 0x20000000) {
                            flag = TRUE;
                        }
                        break;
                    case 3:
                        flag = TRUE;
                        break;
                }
                if (!flag) {
                    obj->flags &= ~8;
                } else {
                    mobjRunEvent(obj, obj->unk1D4);
                    evtSetValue(NULL, obj->unk1E4, 1);
                    if (obj->unk1E0 == 0x4000000) {
                        npcKoopaModeMobjBoundDeadCheck(obj->hitObj[0].hitObject);
                    }
                    if (obj->hitObj[0].hitObject != NULL) {
                        obj->hitObj[0].hitObject->flags |= 1;
                    }
                    obj->flags |= 2;
                    kpaAddScore(kpaMutekiCheck() ? 500 : 400);
                    obj->unk1DC = 0xa;
                }
            }
            break;
        case 0xa:
            if (strcmp(obj->animName, "MOBJ_GreenPipe_ks") == 0) {
                effMObjBrokenEntry(6, obj->position.x, float_25_8041a140 + obj->position.y,
                                   float_25_8041a140 + obj->position.z);
            } else if (strcmp(obj->animName, "MOBJ_GreenPipe_kl") == 0) {
                effMObjBrokenEntry(6, obj->position.x,
                                   float_7p5_8041a1bc + (float_25_8041a140 + obj->position.y),
                                   float_25_8041a140 + obj->position.z);
            } else {
                effMObjBrokenEntry(6, obj->position.x - float_25_8041a140,
                                   float_25_8041a140 + obj->position.y,
                                   float_25_8041a140 + obj->position.z);
            }
            psndSFXOn_3D((const char*)0x834, &obj->position);
            obj->unk1DC = 0x63;
            break;
        case 0x63:
            mobjDelete(obj->name);
            break;
    }
    return 0;
}
#pragma opt_common_subs reset

// Pipe model names are literals; the position retains its prefix Vec template.
s32 evt_mobj_koopa_dokan(EventEntry* event, BOOL isFirstCall) {
#ifdef MUST_MATCH

#endif
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    const char* name = (const char*)evtGetValue(event, args[1]);
    f32 f29 = evtGetFloat(event, args[2]);
    f32 f30 = evtGetFloat(event, args[3]);
    f32 f31 = evtGetFloat(event, args[4]);
    s32 unk1D4 = evtGetValue(event, args[5]);
    s32 unk1E4 = args[6];
    BOOL inBattle = gp->inBattle != 0;
    const char* asyncName;
    MapObjectEntry* obj;
    Vec pos;

    switch (type) {
        case 0:
            asyncName = "MOBJ_GreenPipe_ks";
            break;
        case 1:
            asyncName = "MOBJ_GreenPipe_kl";
            break;
        case 2:
            asyncName = "MOBJ_GreenPipe_ky";
            break;
    }
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    obj->flags |= 6;
#ifdef MUST_MATCH
    // The original addresses vec3_802c40b8 from the vec3_802c3dc8 base.
    pos = mobj_vec3_802c40b8_template[0];
#else
    pos = vec3_802c40b8[0];
#endif
    pos.x = f29;
    pos.y = f30;
    pos.z = f31;
    obj->position = pos;
    obj->unk1D0 = mobj_koopa_dokan_evt;
    obj->unk1D4 = (s32*)unk1D4;
    obj->unk1D8 = 0;
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    obj->flags |= 0x3c000000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x63;
    }
    return 2;
}

s32 fire_func(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    f32 f31 = evtGetFloat(event, args[0]);
    f32 f30 = evtGetFloat(event, args[1]);
    f32 f29 = evtGetFloat(event, args[2]);
    s32 v = evtGetValue(event, args[3]);
    EffectEntry* eff;
    f32 unk7C;
    f32 unk80;
    f32 n5;
    f32 n6;

    if (isFirstCall) {
        event->userdata[0] =
            (s32)effFireEntry(5, v, f31, f30, f29, 0.8f + (f32)irand(10) / 10.0f);
    }
    unk7C = event->userdataf[1];
    unk80 = event->userdataf[2];
    eff = (EffectEntry*)event->userdata[0];
    {
        f32 k = 0.125f;
        unk7C = (1.0f - unk7C) * k + unk7C;
        unk80 = (1.0f - unk80) * k + unk80;
    }
    *(f32*)((u8*)eff->userdata + 0x78) = unk7C;
    *(f32*)((u8*)eff->userdata + 0x7C) = unk80;
    if (unk7C >= 0.99f) {
        *(f32*)((u8*)eff->userdata + 0x78) = 1.0f;
        *(f32*)((u8*)eff->userdata + 0x7C) = 1.0f;
        return 2;
    }
    event->userdataf[1] = unk7C;
    event->userdataf[2] = unk80;
    return 0;
}

s32 mobj_koopa_sango_evt(MapObjectEntry* obj) {
    s32 sp10 = 0;
    EventEntry* event;

    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                switch (kpaGetLevel()) {
                    case 0:
                    case 1:
                    case 2:
                        if (obj->unk1E0 == 0x20000000) {
                            sp10 = 1;
                        }
                        break;
                    case 3:
                        sp10 = 1;
                        break;
                }
                if (sp10 == 0) {
                    obj->flags &= ~0x8;
                    break;
                }
                mobjRunEvent(obj, obj->unk1D4);
                evtSetValue(NULL, obj->unk1E4, 1);
                if (obj->unk1E0 == 0x04000000) {
                    npcKoopaModeMobjBoundDeadCheck(obj->hitObj[0].hitObject);
                }
                if (obj->hitObj[0].hitObject != NULL) {
                    obj->hitObj[0].hitObject->flags |= 1;
                }
                obj->flags |= 2;
                kpaAddScore(kpaMutekiCheck() ? 0x1f4 : 0x190);
                obj->unk1DC = 0xa;
            }
            break;
        case 0xa:
            if (obj->unk1E0 == 0x20000000) {
                psndSFXOn_3D((const char*)0xb67, &obj->position);
            }
            event = evtEntry(evt_fire, 0, 0);
            event->lwData[0] = (s32)(1024.0f * obj->position.x) + 0xF24A7A80;
            event->lwData[1] = (s32)(1024.0f * obj->position.y) + 0xF24A7A80;
            event->lwData[2] = (s32)(1024.0f * (obj->position.z + 10.0f)) + 0xF24A7A80;
            event->lwData[3] = 0x5a;
            obj->unk1DC++;
            break;
        case 0x14:
            event = evtEntry(evt_fire, 0, 0);
            event->lwData[0] = (s32)(1024.0f * (obj->position.x + 10.0f)) + 0xF24A7A80;
            event->lwData[1] = (s32)(1024.0f * (obj->position.y + 30.0f)) + 0xF24A7A80;
            event->lwData[2] = (s32)(1024.0f * (obj->position.z + 10.0f)) + 0xF24A7A80;
            event->lwData[3] = 0x50;
            obj->unk1DC++;
            break;
        case 0x1e:
            event = evtEntry(evt_fire, 0, 0);
            event->lwData[0] = (s32)(1024.0f * (obj->position.x - 10.0f)) + 0xF24A7A80;
            event->lwData[1] = (s32)(1024.0f * (obj->position.y + 60.0f)) + 0xF24A7A80;
            event->lwData[2] = (s32)(1024.0f * (obj->position.z + 10.0f)) + 0xF24A7A80;
            event->lwData[3] = 0x46;
            obj->unk1DC++;
            break;
        case 0x62:
            effMObjBrokenEntry(7, obj->position.x, obj->position.y, obj->position.z + 10.0f);
            obj->unk1DC++;
            break;
        case 0x63:
            mobjDelete(obj->name);
            break;
        default:
            obj->unk1DC++;
            break;
    }
    return 0;
}

s32 evt_mobj_koopa_sango(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f29 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f31 = evtGetFloat(event, args[3]);
    s32 unk1D4 = evtGetValue(event, args[4]);
    s32 unk1E4 = args[5];
    MapObjectEntry* obj;
    Vec pos;

    if (!animGroupBaseAsync("MOBJ_sango", gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, "MOBJ_sango");
    obj = mobjNameToPtr(name);
    obj->flags |= 0x84;
    pos = vec3_802c40c4[0];
    pos.x = f29;
    pos.y = f30;
    pos.z = f31;
    obj->position = pos;
    obj->unk1D0 = mobj_koopa_sango_evt;
    obj->unk1D4 = (s32*)unk1D4;
    obj->unk1D8 = 0;
    obj->flags |= 0x3c000000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x63;
    }
    return 2;
}

s32 mobj_koopa_fireber_dodai_evt(MapObjectEntry* obj) {
    s32 pass = 0;
    MarioWork* mario = marioGetPtr();
    s32 i;
    s32 j;
    NpcEntry* npc;
    Mtx mtx;
    Mtx rotMtx;
    char buf[32];
    char buf2[32];

    if (obj->unk1DC < 0xa) {
        if (distABf(mario->position.x, mario->position.y, obj->position.x, obj->position.y) < 150.0f) {
            if (psndSFXChk(*(s32*)((u8*)obj + 0x1B4)) != 0) {
                *(s32*)((u8*)obj + 0x1B4) = psndSFXOn_3D((const char*)0x1cd, &obj->position);
            }
        } else if (psndSFXChk(*(s32*)((u8*)obj + 0x1B4)) == 0) {
            psndSFXOff(*(s32*)((u8*)obj + 0x1B4));
            *(s32*)((u8*)obj + 0x1B4) = -1;
        }
        PSMTXIdentity(mtx);
        PSMTXTransApply(mtx, mtx, -obj->position.x, -obj->position.y, -obj->position.z);
        PSMTXRotRad(rotMtx, 'Z', 0.017453292f * *(f32*)((u8*)obj + 0x1AC));
        PSMTXConcat(rotMtx, mtx, mtx);
        PSMTXTransApply(mtx, mtx, obj->position.x, obj->position.y, obj->position.z);
        for (i = 0; i < *(s32*)((u8*)obj + 0x1A4); i++) {
            sprintf(buf, "%s%2x", obj->name, i);
            npc = npcNameToPtr(buf);
            PSMTXMultVec(mtx, &npc->position, &npc->position);
        }
    }
    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                switch (kpaGetLevel()) {
                    case 0:
                    case 1:
                    case 2:
                        if (obj->unk1E0 == 0x20000000) {
                            pass = 1;
                        }
                        break;
                    case 3:
                        pass = 1;
                        break;
                }
                if (pass == 0) {
                    obj->flags &= ~8;
                    break;
                }
                mobjRunEvent(obj, obj->unk1D4);
                evtSetValue(NULL, obj->unk1E4, 1);
                if (obj->hitObj[0].hitObject != NULL) {
                    obj->hitObj[0].hitObject->flags |= 1;
                }
                obj->flags |= 2;
                kpaAddScore(kpaMutekiCheck() ? 0x1f4 : 0x190);
                obj->unk1DC = 0xa;
            }
            break;
        case 0xa:
            psndSFXOn_3D((const char*)0x1b8, &obj->position);
            effMObjBrokenEntry(0, obj->position.x, obj->position.y + 12.5f, obj->position.z + 12.5f);
            obj->unk1DC++;
            break;
        case 0xb:
            obj->position.y = -1000.0f;
            for (i = 0; i < *(s32*)((u8*)obj + 0x1A4); i++) {
                sprintf(buf, "%s%2x", obj->name, i);
                npc = npcNameToPtr(buf);
                npc->field_188 = 0;
                npc->jumpStartPos = npc->position;
                npc->jumpTargetPos.x = (f32)irand(0x65) / 200.0f;
                npc->jumpTargetPos.y = 6.0f - (f32)irand(0x65) / 75.0f;
                npc->jumpTargetPos.z = 4.8f;
                npc->moveTime = 2.0f;
                npc->jumpFlags |= 4;
            }
            if (psndSFXChk(*(s32*)((u8*)obj + 0x1B4)) == 0) {
                psndSFXOff(*(s32*)((u8*)obj + 0x1B4));
                *(s32*)((u8*)obj + 0x1B4) = -1;
            }
            obj->unk1DC++;
            break;
        case 0xc:
            for (i = 0; i < *(s32*)((u8*)obj + 0x1A4); i++) {
                sprintf(buf, "%s%2x", obj->name, i);
                npc = npcNameToPtr(buf);
                npc->moveOffset.x = npc->moveOffset.x + npc->jumpTargetPos.x;
                npc->position.y = npc->position.y + npc->jumpTargetPos.y;
                npc->moveOffset.z = npc->moveOffset.z + npc->jumpTargetPos.z;
                npc->scale.x += 0.04f;
                if (npc->scale.x > 3.0f) {
                    npc->scale.x = 3.0f;
                }
                {
                    f32 s = npc->scale.x;
                    npc->scale.y = s;
                    npc->scale.z = s;
                }
                npc->jumpTargetPos.y = npc->jumpTargetPos.y - 0.2f;
            }
            if (npc->moveTime <= npc->field_1A0) {
                obj->unk1DC = 0x63;
            }
            break;
        case 0x63:
            for (j = 0; j < *(s32*)((u8*)obj + 0x1A4); j++) {
                sprintf(buf2, "%s%2x", obj->name, j);
                npcDelete(npcNameToPtr(buf2));
            }
            mobjDelete(obj->name);
            break;
    }
    return 0;
}

// NONMATCHING (98.83%): 2 instructions differ, 1 extra, plus register differences.
s32 evt_mobj_koopa_fireber_dodai(EventEntry* event, BOOL isFirstCall) {
    u8* base = (u8*)vec3_802c3dc8;
    s32* args = event->args;
    s32 sign = evtGetValue(event, args[0]);
    const char* name = (const char*)evtGetValue(event, args[1]);
    f32 x = evtGetFloat(event, args[2]);
    f32 y = evtGetFloat(event, args[3]);
    f32 z = evtGetFloat(event, args[4]);
    s32 count = evtGetValue(event, args[5]);
    f32 f27 = evtGetFloat(event, args[6]);
    s32 unk1D4 = evtGetValue(event, args[7]);
    s32 unk1E4 = args[8];
    BOOL inBattle = gp->inBattle != 0;
    MapObjectEntry* obj;
    Vec pos;
    s32 i;
    char buf[32];

    if (!animGroupBaseAsync((const char*)(base + 0x3D8), inBattle, NULL)) {
        return 0;
    }
    if (!animGroupBaseAsync((const char*)(base + 0x3E8), inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, (const char*)(base + 0x3D8));
    obj = mobjNameToPtr(name);
    obj->flags |= 4;
    pos = *(Vec*)(base + 0x308);
    pos.x = x;
    pos.y = y;
    pos.z = z;
    obj->position = pos;
    obj->unk1D0 = mobj_koopa_fireber_dodai_evt;
    obj->unk1D4 = (s32*)unk1D4;
    obj->unk1D8 = NULL;
    obj->flags |= 0x3c000000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x63;
    }
    if (f27 == 0.0f) {
        f27 = 1.0f;
    }
    *(s32*)((u8*)obj + 0x1A4) = count;
    *(f32*)((u8*)obj + 0x1AC) = (sign != 0) ? __fabsf(f27) : -__fabsf(f27);
    *(s32*)((u8*)obj + 0x1B4) = 0;
    y = 25.0f + y;
    z = 175.0f + z;
    for (i = 0; i < count; i++) {
        NpcEntry* npc;

        sprintf(buf, "%s%2x", name, i);
        npcEntry(buf, (const char*)(base + 0x3E8));
        npc = npcNameToPtr(buf);
        npc->flags |= 0x08000000;
        *(u16*)((u8*)npc + 0x31E) |= 0xF;
        npc->position.x = x;
        npc->position.y = 25.0f * (f32)i + y;
        npc->position.z = z;
        animPoseSetAnim(npc->poseId, (const char*)(base + 0x3F4), 1);
        npc->flags |= 0x40000;
    }
    return 2;
}

s32 kpa_get_pole_name(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;

    if (event->thisMapObj != NULL) {
        evtSetValue(event, args[0], (s32)event->thisMapObj->name);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return 2;
}

s32 kpa_get_hata_name(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;

    if (event->thisMapObj != NULL) {
        evtSetValue(event, args[0], (s32)((u8*)event->thisMapObj + 0x1B8));
    } else {
        evtSetValue(event, args[0], 0);
    }
    return 2;
}

static s32 kpa_get_level(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;

    evtSetValue(event, args[0], kpaGetLevel());
    return 2;
}

s32 kpa_get_width(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;

    evtSetValue(event, args[0], (s32)marioGetPtr()->wPlayerCollisionBox.x);
    return 2;
}

// NONMATCHING (94.91%): `rem` (rather than mutating `value`) gets the divisions right but the four
// digit registers are still coloured wrongly (retail wants hundreds=r6/tens=r5/ones=r3; ours r7/r5/r6)
// and retail's per-digit `add; mr` coalescing is absent. Tried: all declaration orders, explicit
// temps, `ones = rem - tens*10`, least-significant-digit-first, digit widths and every pragma.
s32 kpa_score_to_str(EventEntry* event, BOOL isFirstCall) {
    s32 value = evtGetValue(event, event->args[0]);
    s32 i = 0;
    s32 thousands;
    s32 hundreds;
    s32 tens;
    s32 ones;
    s32 rem;

    thousands = value / 1000;
    rem = value % 1000;
    hundreds = rem / 100;
    rem %= 100;
    tens = rem / 10;
    ones = rem % 10;

    if (i != 0 || thousands != 0) {
        psw->str[i] = thousands + '0';
        i = 1;
    }
    if (i != 0 || hundreds != 0) {
        psw->str[i++] = hundreds + '0';
    }
    if (i != 0 || tens != 0) {
        psw->str[i++] = tens + '0';
    }
    if (i != 0 || ones != 0) {
        psw->str[i] = ones + '0';
    }
    psw->unk4 = 0;
    return 2;
}

void kpa_disp_pole_score(CameraId camera, void* param) {
    Mtx trans;
    Mtx scale;
    GXColor color;

    PSMTXTrans(trans, psw->x, psw->y, 300.0f);
    PSMTXScale(scale, 0.5f, 0.5f, 0.5f);
    PSMTXConcat(trans, scale, trans);
    FontDrawStart();
    *(u32*)&color = dat_8041a134;
    FontDrawColor(&color);
    FontDrawStringMtx(trans, psw->str);
}

s32 kpa_disp_pole_score_main(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    f32 f29;
    f32 f30;
    f32 f31;
    f32 f1;

    f29 = evtGetFloat(event, args[0]);
    f30 = evtGetFloat(event, args[1]);
    f31 = evtGetFloat(event, args[2]);
    f1 = evtGetFloat(event, args[3]);
    if (isFirstCall) {
        psw->x = f29;
        psw->y = f30;
        psw->z = f31;
    }
    if (psw->y < 230.0f) {
        psw->y = psw->y + f1;
    } else {
        psw->y = 230.0f;
    }
    dispEntry(4, 0.0f, 2, kpa_disp_pole_score, NULL);
    return 0;
}

s32 kpa_disp_pole_init(EventEntry* event, BOOL isFirstCall) {
    f32 zero;

    psw = _mapAlloc(0x14);
    zero = float_0_8041a150;
    psw->str[0] = 0;
    psw->z = zero;
    psw->y = zero;
    psw->x = zero;
    return 2;
}

s32 kpa_clear_jump_data(EventEntry* event, BOOL isFirstCall) {
    MarioWork* mario = marioGetPtr();
    mario->airTimer = 0;
    return 2;
}

s32 mobj_koopa_pole_evt(MapObjectEntry* obj) {
#ifdef MUST_MATCH
    u8* base = (u8*)negone_one_374;
#endif
    MarioWork* mario = marioGetPtr();
    NpcWork* wp;
    NpcEntry* entry;
    s32 next = 0;
    s32 score;
    s32 i;

    switch (obj->unk1DC) {
        case 0:
            if (!(obj->flags & 8)) {
                break;
            }
            if (mario->characterId != 2) {
                obj->flags &= ~8;
                break;
            }
            switch (kpaGetLevel()) {
                case 0:
                case 1:
                    if (mario->position.y < float_25_8041a140) {
                        obj->flags &= ~8;
                    } else {
                        next = 0xa;
                    }
                    break;
                case 2:
                case 3:
                    next = 0x32;
                    break;
            }
            if (next == 0) {
                break;
            }
            for (i = 0; i < npcGetWorkPtr()->total; i++) {
                entry = &npcGetWorkPtr()->entries[i];
                if (!(entry->flags & 1)) {
                    continue;
                }
                if (entry->jumpFlags & 4) {
                    continue;
                }
                if (!(entry->flags & 0x08000000)) {
                    continue;
                }
                if (!(entry->flags & 8)) {
                    continue;
                }
                npcGroupDead(entry, 0x20);
            }
            marioKeyOff();
            evtSetValue(NULL, obj->unk1E4, 1);
            *(s16*)mario->unk78 = 0;
            obj->unk1DC = next;
            break;
        case 0xa:
            mario = marioGetPtr();
            score = 0;
            if (kpaGetLevel() == 3) {
                kpaAddScore(0x7d0);
            } else if (kpaGetLevel() >= 2) {
                kpaAddScore(0x3e8);
            } else {
                if (mario->position.y >= float_170_8041a144) {
                    score = 1000;
                } else if (mario->position.y >= float_100_8041a148) {
                    score = 500;
                } else if (mario->position.y >= float_50_8041a14c) {
                    score = 200;
                } else {
                    score = 100;
                }
                kpaAddScore(score);
            }
#ifdef MUST_MATCH
            mobjRunEvent(obj, (s32*)(base + 0x230))->lwData[0] = score;
#else
            mobjRunEvent(obj, kpa_pole)->lwData[0] = score;
#endif
            obj->unk1DC++;
            break;
        case 0xb:
            if (evtCheckID(obj->eventId) == 1) {
                break;
            }
            obj->unk1DC = 0x5a;
            break;
        case 0x32:
            mario = marioGetPtr();
            if (kpaGetLevel() == 3) {
                kpaAddScore(0x7d0);
            } else if (kpaGetLevel() >= 2) {
                kpaAddScore(0x3e8);
            } else {
                if (mario->position.y >= float_170_8041a144) {
                    score = 1000;
                } else if (mario->position.y >= float_100_8041a148) {
                    score = 500;
                } else if (mario->position.y >= float_50_8041a14c) {
                    score = 200;
                } else {
                    score = 100;
                }
                kpaAddScore(score);
            }
#ifdef MUST_MATCH
            mobjRunEvent(obj, (s32*)(base + 0x690));
#else
            mobjRunEvent(obj, kpa_pole_move);
#endif
            obj->unk1DC++;
            break;
        case 0x33:
            if (evtCheckID(obj->eventId) == 1) {
                break;
            }
            obj->unk1DC = 0x5a;
            break;
        case 0x5a:
#ifdef MUST_MATCH
            mobjRunEvent(obj, (s32*)(base + 0x8ec));
#else
            mobjRunEvent(obj, kpa_move_to_toride);
#endif
            obj->unk1DC++;
            break;
        case 0x5b:
            if (evtCheckID(obj->eventId) == 1) {
                break;
            }
            obj->eventId = 0;
            obj->unk1DC++;
            break;
        case 0x5c:
            mobjRunEvent(obj, obj->unk1D8);
            obj->unk1DC++;
            break;
        case 0x5d:
            if (evtCheckID(obj->eventId) == 1) {
                break;
            }
            obj->eventId = 0;
            obj->unk1DC++;
            // fallthrough
        case 0x5e:
            mobjRunEvent(obj, obj->unk1D4);
            obj->unk1DC++;
            break;
#ifdef MUST_MATCH
        case 0x5f:
#endif
        default:
            break;
    }
    return 0;
}

// `obj+0x1B8` is reached as `&obj->unk1A0[0x18]` (the declared array), so MWCC recomputes the
// `addi r3, r31, 0x1b8` per use; the raw `(char*)((u8*)obj + 0x1B8)` form CSEs the address into r25.
s32 evt_mobj_koopa_pole(EventEntry* event, BOOL isFirstCall) {
    u8* base = (u8*)vec3_802c3dc8;
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    s32 unk1D4 = evtGetValue(event, args[4]);
    s32 unk1D8 = evtGetValue(event, args[5]);
    s32 unk1E4 = args[6];
    BOOL inBattle = gp->inBattle != 0;
    MapObjectEntry* obj;
    MapObjectEntry* child;
    Vec pos;
    Vec pos2;

    if (!animGroupBaseAsync((const char*)(base + 0x3B8), inBattle, NULL)) {
        return 0;
    }
    if (!animGroupBaseAsync((const char*)(base + 0x3C8), inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, (const char*)(base + 0x3B8));
    obj = mobjNameToPtr(name);
    obj->flags |= 0x84;
    pos = *(Vec*)(base + 0x3A0);
    pos.x = x;
    pos.y = y;
    pos.z = z;
    obj->position = pos;
    obj->unk1D0 = mobj_koopa_pole_evt;
    obj->unk1D4 = (s32*)unk1D4;
    obj->unk1D8 = (s32*)unk1D8;
    obj->flags |= 0x1c280000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x63;
    }
    strcpy((char*)&obj->unk1A0[0x18], name);
    strcat((char*)&obj->unk1A0[0x18], "_h");
    mobjEntry((char*)&obj->unk1A0[0x18], (const char*)(base + 0x3C8));
    child = mobjNameToPtr((char*)&obj->unk1A0[0x18]);
    pos2 = *(Vec*)(base + 0x3AC);
    pos2.x = x;
    pos2.y = 180.0f + y;
    pos2.z = z;
    child->position = pos2;
    child->unk1D0 = NULL;
    child->unk1D4 = NULL;
    child->unk1D8 = NULL;
    child->unk1E4 = 0;
    child->unk1DC = 0;
    strcpy((char*)((u8*)child + 0x1B8), name);
    if (child->hitObj[0].hitObject != NULL) {
        child->hitObj[0].hitObject->flags |= 1;
    }
    return 2;
}

// ROData pool objects
__declspec(section ".sdata2") const char str_8041A118[4] = "A_1";
__declspec(section ".sdata2") const char str_8041A11C[4] = "A_2";
__declspec(section ".sdata2") const char str_8041A120[4] = "A_3";
__declspec(section ".sdata2") const char str_8041A124[4] = "A_4";
__declspec(section ".sdata2") const char str_8041A128[4] = "A_5";
__declspec(section ".sdata2") const char str_8041A12C[4] = "S_1";
__declspec(section ".sdata2") u32 dat_8041a130 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041a134 = 0xFFFFFFFF;
__declspec(section ".sdata2") const char str_8041A138[4] = "_h\000";
const f32 float_180_8041a13c = 180.0f;
__declspec(section ".sdata2") f32 float_25_8041a140 = 25.0f;
__declspec(section ".sdata2") f32 float_170_8041a144 = 170.0f;
__declspec(section ".sdata2") f32 float_100_8041a148 = 100.0f;
__declspec(section ".sdata2") f32 float_50_8041a14c = 50.0f;
#pragma explicit_zero_data on  // keep the zero float_0 in .sdata2 (otherwise MWCC puts it in bss)
__declspec(section ".sdata2") f32 float_0_8041a150 = 0.0f;
#pragma explicit_zero_data reset
const f32 float_230_8041a154 = 230.0f;
__declspec(section ".sdata2") f32 float_300_8041a158 = 300.0f;
__declspec(section ".sdata2") f32 float_0p5_8041a15c = 0.5f;
__declspec(section ".sdata2") f32 float_1_8041a160 = 1.0f;
const f32 float_175_8041a164 = 175.0f;
__declspec(section ".sdata2") const char str_8041A168[8] = "%s%2x\000\000";
const f32 float_150_8041a170 = 150.0f;
const f32 float_deg2rad_8041a174 = 0.017453292f;
__declspec(section ".sdata2") f32 float_12p5_8041a178 = 12.5f;
const f32 float_neg1000_8041a17c = -1000.0f;
const f32 float_200_8041a180 = 200.0f;
const f32 float_6_8041a184 = 6.0f;
const f32 float_75_8041a188 = 75.0f;
const f32 float_4p8_8041a18c = 4.8f;
__declspec(section ".sdata2") f32 float_2_8041a190 = 2.0f;
const f32 float_0p04_8041a194 = 0.04f;
const f32 float_3_8041a198 = 3.0f;
const f32 float_0p2_8041a19c = 0.2f;
__declspec(section ".sdata2") f32 float_1024_8041a1a0 = 1024.0f;
__declspec(section ".sdata2") f32 float_10_8041a1a4 = 10.0f;
__declspec(section ".sdata2") f32 float_30_8041a1a8 = 30.0f;
__declspec(section ".sdata2") f32 float_60_8041a1ac = 60.0f;
__declspec(section ".sdata2") f32 float_0p8_8041a1b0 = 0.8f;
__declspec(section ".sdata2") f32 float_0p125_8041a1b4 = 0.125f;
__declspec(section ".sdata2") f32 float_0p99_8041a1b8 = 0.99f;
__declspec(section ".sdata2") f32 float_7p5_8041a1bc = 7.5f;
__declspec(section ".sdata2") const char str_8041A1C0[4] = "S_2";
__declspec(section ".sdata2") f32 float_0p45_8041a1c4 = 0.45f;
__declspec(section ".sdata2") f32 float_0p7_8041a1c8 = 0.7f;
__declspec(section ".sdata2") f32 float_500_8041a1cc = 500.0f;
__declspec(section ".sdata2") f32 float_120_8041a1d0 = 120.0f;
const f32 float_0p1_8041a1d4 = 0.1f;
__declspec(section ".sdata2") f32 float_1E05_8041a1d8 = 100000.0f;
__declspec(section ".sdata2") f32 float_42p42_8041a1dc = 42.420002f;
__declspec(section ".sdata2") f32 float_0p6_8041a1e0 = 0.6f;
__declspec(section ".sdata2") f32 float_32767_8041a1e4 = 32767.0f;
__declspec(section ".sdata2") const char str_8041A1E8[4] = "S_3";
__declspec(section ".sdata2") f32 float_40_8041a1ec = 40.0f;
const f32 float_360_8041a1f0 = 360.0f;
__declspec(section ".sdata2") f32 float_45_8041a1f4 = 45.0f;
__declspec(section ".sdata2") const char str_8041A1F8[8] = "M_I_1";
__declspec(section ".sdata2") const char str_8041A200[8] = "M_S_1";
const f32 float_1p4_8041a208 = 1.4f;
const f32 float_1p2_8041a20c = 1.2f;
__declspec(section ".sdata2") f32 float_5_8041a210 = 5.0f;
__declspec(section ".sdata2") f32 float_neg1_8041a214 = -1.0f;
__declspec(section ".sdata2") const char str_8041A218[4] = "S_4";
__declspec(section ".sdata2") const char str_8041A21C[4] = "Z_1";
const f32 float_neg0p8_8041a220 = -0.8f;
const f32 float_4_8041a224 = 4.0f;
__declspec(section ".sdata2") f32 float_0p9_8041a228 = 0.9f;
const f32 float_15_8041a22c = 15.0f;
const f32 float_0p3_8041a230 = 0.3f;
__declspec(section ".sdata2") const char str_8041A234[4] = "me\000";

// .sdata
const char* _switch_anim[6] = {
    str_8041A118, str_8041A11C, str_8041A120, str_8041A124, str_8041A128, str_8041A12C,
};
s32 kpa_powerup_evt[7] = {
    0x0004005B, (s32)kpa_powerup, 0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000002, 0x00000001,
};
s32 evt_fire[8] = {
    0x0005005B, (s32)fire_func, 0xFE363C80, 0xFE363C81, 0xFE363C82, 0xFE363C83, 0x00000002, 0x00000001,
};
// trailing zero word that ends the unit's .sdata: needs explicit_zero_data to be emitted and force_active to survive the linker.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_80410304_sdata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset


// .rodata (part D)
#pragma force_active on
__declspec(section ".rodata") Vec vec3_802c4020[1] = {{0.0f, -2000.0f, 0.0f}};
__declspec(section ".rodata") Vec vec3_802c402c[1] = {{0.0f, 0.0f, 0.0f}};
__declspec(section ".rodata") Vec vec3_802c4038[1] = {{0.0f, 0.0f, 0.0f}};
__declspec(section ".rodata") Vec vec3_802c4044[1] = {{0.0f, 0.0f, -1.0f}};
// vec3_802c4050: placeholder for the object the code copies (defining it hangs MWCC)
__declspec(section ".rodata") Vec mobj_vec3_802c4050_template[1] = {{0.0f, 0.0f, 0.0f}};
// vec3_802c405c: placeholder for the object the code copies (defining it hangs MWCC)
__declspec(section ".rodata") Vec mobj_vec3_802c405c_template[1] = {{200.0f, 0.0f, 200.0f}};
__declspec(section ".rodata") Vec vec3_802c4068[1] = {{0.0f, 0.0f, 0.0f}};
const char str_802C4074[0x10] = "o_";
const char str_802C4084[0x10] = "i_";
__declspec(section ".rodata") Vec vec3_802c4094[1] = {{0.0f, -2000.0f, 0.0f}};
// vec3_802c40a0: placeholder for the object the code copies (defining it hangs MWCC)
__declspec(section ".rodata") Vec mobj_vec3_802c40a0_template[1] = {{0.0f, 0.0f, 0.0f}};
// vec3_802c40ac: placeholder for the object the code copies (defining it hangs MWCC)
__declspec(section ".rodata") Vec mobj_vec3_802c40ac_template[1] = {{0.0f, 0.0f, 0.0f}};
// vec3_802c40b8: placeholder for the object the code copies (defining it hangs MWCC)
__declspec(section ".rodata") Vec mobj_vec3_802c40b8_template[1] = {{0.0f, 0.0f, 0.0f}};
// vec3_802c40c4: placeholder for the object the code copies (defining it hangs MWCC)
__declspec(section ".rodata") Vec mobj_vec3_802c40c4_template[1] = {{0.0f, 0.0f, 0.0f}};
__declspec(section ".rodata") Vec vec3_802c40d0[1] = {{0.0f, 0.0f, 0.0f}};
__declspec(section ".rodata") char str_802C40DC[0xC] = "KPA2_G_1";
__declspec(section ".rodata") char str_802C40E8[0x18] = "SFX_KUPPA_POST_JUMP1";
__declspec(section ".rodata") char str_802C4100[0x18] = "SFX_KUPPA_POST_SLIDE1";
__declspec(section ".rodata") char str_802C4118[0x18] = "SFX_KUPPA_POST_THROW1";
__declspec(section ".rodata") char str_802C4130[0x8] = "kpa_06";
__declspec(section ".rodata") char str_802C4138[0x8] = "toride";
__declspec(section ".rodata") char str_802C4140[0x8] = "toride_";
__declspec(section ".rodata") char str_802C4148[0x14] = "SFX_KUPPA_STRUGGLE1";
__declspec(section ".rodata") char str_802C415C[0xC] = "KPA2_S_1";
__declspec(section ".rodata") Vec vec3_802c4168[1] = {{0.0f, 0.0f, 0.0f}};
__declspec(section ".rodata") Vec vec3_802c4174[1] = {{0.0f, 0.0f, 0.0f}};
const char str_802C4180[0x10] = "MOBJ_goal_pole";
const char str_802C4190[0x10] = "MOBJ_goal_hata";
const char str_802C41A0[0x10] = "MOBJ_fire_dodai";
const char str_802C41B0[0xC] = "c_fireber";
const char str_802C41BC[0x8] = "FRB_S_1";
// double_to_int_802c41c8: compiler int->float conversion constant, emitted at the first conversion
// double_to_int_mask_802c42e0: compiler int->float conversion constant, emitted at the first conversion
#pragma force_active reset

// .data
#pragma force_active on
__declspec(section ".data") f32 lbl_80309CF0[2] = {-1.0f, 1.0f};
__declspec(section ".data") static f32 one_negone_373[2] = {1.0f, -1.0f};
__declspec(section ".data") f32 lbl_80309D00[2] = {1.0f, -1.0f};
__declspec(section ".data") static f32 negone_one_360[2] = {-1.0f, 1.0f};
__declspec(section ".data") f32 lbl_80309D10[2] = {-1.0f, 1.0f};
__declspec(section ".data") static f32 one_negone_359[2] = {1.0f, -1.0f};
__declspec(section ".data") f32 lbl_80309D20[2] = {1.0f, -1.0f};
s32 evt_lock_sound[20] = {
    0x0001000A, 0x00000064, 0x0006005B, (s32)evt_snd_sfxon_3d,
    (s32)str_802C3F00, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000000, 0x0001000A, 0x000000C8, 0x0006005B,
    (s32)evt_snd_sfxon_3d, (s32)str_802C3F18, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000000, 0x00000002, 0x00000001,
};
s32 evt_box_sound1[20] = {
    0x0001000A, 0x00000064, 0x0006005B, (s32)evt_snd_sfxon_3d,
    (s32)str_802C3F2C, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000000, 0x0001000A, 0x000000C8, 0x0006005B,
    (s32)evt_snd_sfxon_3d, (s32)str_802C3F4C, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000000, 0x00000002, 0x00000001,
};
s32 evt_box_sound2[20] = {
    0x0001000A, 0x00000064, 0x0006005B, (s32)evt_snd_sfxon_3d,
    (s32)str_802C3F68, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000000, 0x0001000A, 0x000000C8, 0x0006005B,
    (s32)evt_snd_sfxon_3d, (s32)str_802C3F88, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000000, 0x00000002, 0x00000001,
};
s32 evt_box_sound3[23] = {
    0x0001000A, 0x00000064, 0x0006005B, (s32)evt_snd_sfxon_3d,
    (s32)str_802C3FA4, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0xFE363C8F, 0x0001000A, 0x000000C8, 0x0002005B,
    (s32)evt_snd_sfxoff, 0xFE363C8F, 0x0006005B, (s32)evt_snd_sfxon_3d,
    (s32)str_802C3FC0, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000000, 0x00000002, 0x00000001,
};
s32 mobj_save_blk_sysevt[41] = {
    0x00010063, 0x00000000, 0x0002005B, (s32)evt_mario_key_onoff,
    0x00000000, 0x00020032, 0xFBD3E28A, 0x00000000,
    0x00020018, 0xF8405C69, 0x00000000, 0x00020032,
    0xF8405C69, 0x00000001, 0x0005005B, (s32)evt_msg_print,
    0x00000000, (s32)str_802C3FF0, 0x00000000, 0x00000000,
    0x00020032, 0xFBD3E28A, 0x00000001, 0x00000021,
    0x0002001D, 0xF5DE0180, 0x00000007, 0x00020032,
    0xF8405C6C, 0x00000001, 0x00000021, 0x00020032,
    0xFE363C80, 0x00000000, 0x0001005E, (s32)evt_memcard_save,
    0x0002005B, (s32)evt_mario_key_onoff, 0x00000001, 0x00000002,
    0x00000001,
};
s32 kpa_pole[280] = {
    0x0002005B, (s32)evt_cam_letter_box_disable, 0x00000001, 0x0003005B,
    (s32)evt_cam_letter_box_onoff, 0x00000000, 0x00000000, 0x0002005B,
    (s32)evt_mario_cont_onoff, 0x00000000, 0x0001005B, (s32)evt_mario_cam_y_on,
    0x0001005B, (s32)kpa_disp_pole_init, 0x0001005B, (s32)kpa_clear_jump_data,
    0x0002005B, (s32)kpa_get_hata_name, 0xFE363C8F, 0x0004005B,
    (s32)evt_mario_set_dir, 0x0000005A, 0x00000000, 0x00000000,
    0x0002005B, (s32)kpa_chg_pose, (s32)str_802C40DC, 0x0005005B,
    (s32)evt_mario_get_pos, 0x00000000, 0xFE363C81, 0xFE363C82,
    0xFE363C83, 0x0006005B, (s32)evt_snd_sfxon_3d, (s32)str_802C40E8,
    0xFE363C81, 0xFE363C82, 0xFE363C83, 0x00000000,
    0x0003005B, (s32)evt_mobj_get_x_position, 0xFE363C8F, 0xFE363C84,
    0x0002005B, (s32)kpa_get_width, 0xFE363C85, 0x0002003C,
    0xFE363C85, 0xF24A7CE6, 0x0002003B, 0xFE363C84,
    0xFE363C85, 0x0004005B, (s32)evt_mario_set_pos, 0xFE363C84,
    0xFE363C82, 0xFE363C83, 0x0002005B, (s32)kpa_score_to_str,
    0xFE363C80, 0x00020034, 0xFE363C8E, 0xF24A8280,
    0x00020034, 0xFE363C8D, 0xF24A8280, 0x00020034,
    0xFE363C87, 0xF24B4280, 0x0000006B, 0x0005005B,
    (s32)evt_mobj_get_position, 0xFE363C8F, 0xFE363C83, 0xFE363C84,
    0xFE363C85, 0x0002003A, 0xFE363C83, 0xF24A9880,
    0x0005005B, (s32)kpa_disp_pole_score_main, 0xFE363C83, 0xFE363C87,
    0xFE363C85, 0xFE363C8D, 0x0000006D, 0x0005005B,
    (s32)evt_mario_get_pos, 0x00000000, 0xFE363C81, 0xFE363C82,
    0xFE363C83, 0x0006005B, (s32)evt_snd_sfxon_3d, (s32)str_802C4100,
    0xFE363C81, 0xFE363C82, 0xFE363C83, 0xFE363C8A,
    0x0001006C, 0xFE363C86, 0x00010005, 0x00000000,
    0x0003005B, (s32)evt_mobj_get_y_position, 0xFE363C8F, 0xFE363C84,
    0x0002003B, 0xFE363C84, 0xFE363C8E, 0x0003005B,
    (s32)evt_mobj_set_y_position, 0xFE363C8F, 0xFE363C84, 0x00010009,
    0x00000001, 0x0002001C, 0xFE363C84, 0x00000019,
    0x0003005B, (s32)evt_mobj_set_y_position, 0xFE363C8F, 0xF24ADE80,
    0x00000007, 0x00000021, 0x00000006, 0x0000006D,
    0x00010005, 0x00000000, 0x0005005B, (s32)evt_mario_get_pos,
    0x00000000, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0002003B, 0xFE363C81, 0xF24A8280, 0x0004005B,
    (s32)evt_mario_set_pos, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0002001C, 0xFE363C81, 0xF24ADE80, 0x0004005B,
    (s32)evt_mario_set_pos, 0xFE363C80, 0xF24ADE80, 0xFE363C82,
    0x00000007, 0x00000021, 0x00010009, 0x00000001,
    0x00000006, 0x00010005, 0x00000000, 0x0002006A,
    0xFE363C86, 0xFE363C87, 0x00020018, 0xFE363C87,
    0x00000000, 0x00000007, 0x00000021, 0x00010009,
    0x00000001, 0x00000006, 0x0002005B, (s32)evt_snd_sfxoff,
    0xFE363C8A, 0x0001000A, 0x000000C8, 0x0005005B,
    (s32)evt_cam_get_pos, 0x00000004, 0xFE363C84, 0xFE363C85,
    0xFE363C86, 0x0005005B, (s32)evt_cam_get_at, 0x00000004,
    0xFE363C87, 0xFE363C88, 0xFE363C89, 0x0009005B,
    (s32)evt_cam3d_evt_set, 0xFE363C84, 0xFE363C85, 0xFE363C86,
    0xFE363C87, 0xFE363C88, 0xFE363C89, 0x00000000,
    0x0000000B, 0x00010009, 0x00000001, 0x0002005B,
    (s32)kpa_get_width, 0xFE363C83, 0x0005005B, (s32)evt_mario_get_pos,
    0x00000000, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0002003C, 0xFE363C83, 0xF24A7F4C, 0x0002003A,
    0xFE363C80, 0xFE363C83, 0x0005005B, (s32)evt_cam3d_get_shift,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0xFE363C83,
    0x000B005B, (s32)evt_cam3d_event_from_road, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0xFE363C83, 0xFE363C84, 0xFE363C85,
    0xFE363C86, 0xFE363C87, 0xFE363C88, 0xFE363C89,
    0x0009005B, (s32)evt_cam3d_evt_set, 0xFE363C84, 0xFE363C85,
    0xFE363C86, 0xFE363C87, 0xFE363C88, 0xFE363C89,
    0x0000012C, 0x00000000, 0x0001000A, 0x000000C8,
    0x0004005B, (s32)evt_mario_set_pos, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0004005B, (s32)evt_mario_set_dir, 0x0000010E,
    0x00000000, 0x00000000, 0x0002005B, (s32)evt_mario_set_mov_spd,
    0x00000000, 0x0001000A, 0x000000C8, 0x00020035,
    0xFE363C80, 0x00000014, 0x0004005B, (s32)evt_mario_set_pos,
    0xFE363C80, 0x00000000, 0x0000000A, 0x00010009,
    0x00000001, 0x0002005B, (s32)evt_mario_cont_onoff, 0x00000001,
    0x0003005B, (s32)evt_cam3d_evt_off, 0x000000C8, 0x0000000B,
    0x0001000A, 0x000000C8, 0x00000002, 0x00000001,
};
s32 kpa_pole_move[151] = {
    0x0002005B, (s32)evt_cam_letter_box_disable, 0x00000001, 0x0003005B,
    (s32)evt_cam_letter_box_onoff, 0x00000000, 0x00000000, 0x0002005B,
    (s32)evt_mario_key_onoff, 0x00000000, 0x0002005B, (s32)kpa_get_pole_name,
    0xFE363C8E, 0x0002005B, (s32)kpa_get_hata_name, 0xFE363C8F,
    0x0003005B, (s32)evt_mobj_get_x_position, 0xFE363C8E, 0xFE363C80,
    0x0003005B, (s32)evt_mobj_get_y_position, 0xFE363C8E, 0xFE363C81,
    0x0003005B, (s32)evt_mobj_get_z_position, 0xFE363C8E, 0xFE363C82,
    0x00020034, 0xFE363C86, 0xF24A8280, 0x00020034,
    0xFE363C87, 0xF24A9E80, 0x00020034, 0xFE363C88,
    0xF24A91B3, 0x0003005B, (s32)evt_mobj_get_x_position, 0xFE363C8F,
    0xFE363C83, 0x0003005B, (s32)evt_mobj_get_y_position, 0xFE363C8F,
    0xFE363C84, 0x0003005B, (s32)evt_mobj_get_z_position, 0xFE363C8F,
    0xFE363C85, 0x00020034, 0xFE363C89, 0xF24A8280,
    0x00020034, 0xFE363C8B, 0xF24A91B3, 0x00020034,
    0xFE363C8C, 0xF24A7E80, 0x0006005B, (s32)evt_snd_sfxon_3d,
    (s32)str_802C4118, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0xFE363C8D, 0x00010005, 0x00000064, 0x0002003A,
    0xFE363C80, 0xFE363C86, 0x0002003A, 0xFE363C81,
    0xFE363C87, 0x0002003A, 0xFE363C82, 0xFE363C88,
    0x0002003A, 0xFE363C83, 0xFE363C89, 0x00020034,
    0xFE363C84, 0xF24D4A80, 0x0002003C, 0xFE363C84,
    0xFE363C8C, 0x0002003A, 0xFE363C84, 0xFE363C81,
    0x0002003A, 0xFE363C85, 0xFE363C8B, 0x0005005B,
    (s32)evt_mobj_set_position, 0xFE363C8E, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)evt_mobj_set_scale, 0xFE363C8E,
    0xFE363C8C, 0xFE363C8C, 0xFE363C8C, 0x0005005B,
    (s32)evt_mobj_set_position, 0xFE363C8F, 0xFE363C83, 0xFE363C84,
    0xFE363C85, 0x0005005B, (s32)evt_mobj_set_scale, 0xFE363C8F,
    0xFE363C8C, 0xFE363C8C, 0xFE363C8C, 0x0002003A,
    0xFE363C86, 0xF24A7A80, 0x0002003A, 0xFE363C87,
    0xF24A774D, 0x0002003A, 0xFE363C88, 0xF24A7A80,
    0x0002003A, 0xFE363C89, 0xF24A7A80, 0x0002003A,
    0xFE363C8B, 0xF24A7A80, 0x0002003A, 0xFE363C8C,
    0xF24A7AA8, 0x0002001B, 0xFE363C8C, 0x00000003,
    0x00020034, 0xFE363C8C, 0xF24A8680, 0x00000021,
    0x00010009, 0x00000001, 0x00000006, 0x0002005B,
    (s32)evt_snd_sfxoff, 0xFE363C8D, 0x0002005B, (s32)evt_mario_key_onoff,
    0x00000001, 0x00000002, 0x00000001,
};
s32 kpa_move_to_toride[110] = {
    0x0002005B, (s32)evt_mario_key_onoff, 0x00000000, 0x0001005B,
    (s32)evt_mario_wait_landon, 0x00020032, 0xF9718885, 0x00000001,
    0x0002005B, (s32)evt_sub_get_mapname, 0xFE363C80, 0x0002000C,
    0xFE363C80, (s32)str_802C4130, 0x0005005B, (s32)evt_mapobj_get_position,
    (s32)str_802C4138, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000020, 0x0005005B, (s32)evt_mapobj_get_position, (s32)str_802C4140,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000021,
    0x0002005B, (s32)kpa_get_level, 0xFE363C86, 0x00010022,
    0xFE363C86, 0x00010028, 0x00000001, 0x00020035,
    0xFE363C80, 0x00000064, 0x00010024, 0x00000002,
    0x0002005B, (s32)kpa_get_width, 0xFE363C87, 0x0002003C,
    0xFE363C87, 0xF24A7F4C, 0x0002003B, 0xFE363C80,
    0xFE363C87, 0x00010024, 0x00000003, 0x0002005B,
    (s32)kpa_get_width, 0xFE363C87, 0x0002003C, 0xFE363C87,
    0xF24A7F4C, 0x0002003B, 0xFE363C80, 0xFE363C87,
    0x00000031, 0x0005005B, (s32)evt_mario_get_pos, 0x00000000,
    0xFE363C83, 0xFE363C84, 0xFE363C85, 0x0004005B,
    (s32)evt_mario_mov_pos2, 0xFE363C80, 0xFE363C85, 0x0000005A,
    0x0002001D, 0xFE363C86, 0x00000002, 0x0005005B,
    (s32)evt_mario_get_pos, 0x00000000, 0xFE363C81, 0xFE363C82,
    0xFE363C83, 0x0006005B, (s32)evt_snd_sfxon_3d, (s32)str_802C4148,
    0xFE363C81, 0xFE363C82, 0xFE363C83, 0xFE363C8A,
    0x0003005B, (s32)evt_mario_dispflag_onoff, 0x00000001, 0x00000002,
    0x0001000A, 0x000007D0, 0x0003005B, (s32)evt_mario_dispflag_onoff,
    0x00000000, 0x00000002, 0x0002005B, (s32)evt_snd_sfxoff,
    0xFE363C8A, 0x0002005B, (s32)kpa_chg_pose, (s32)str_802C415C,
    0x00000021, 0x0002005B, (s32)evt_mario_key_onoff, 0x00000001,
    0x00000002, 0x00000001,
};
// jumptable_8030A78C: generated by the switch in the referencing function
// jumptable_8030A7C4: generated by the switch in the referencing function
// jumptable_8030A7FC: generated by the switch in the referencing function
// jumptable_8030A834: generated by the switch in the referencing function

#pragma force_active reset
