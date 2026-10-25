
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
void FontDrawColor(GXColor* color);
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
extern Vec mobj_vec3_802c3ef4_template[];
extern Vec mobj_vec3_802c40a0_template[];
extern Vec mobj_vec3_802c40ac_template[];
extern Vec mobj_vec3_802c40c4_template[];
static const Vec vec3_802c3fd8[1];
extern Vec vec3_802c3e44[];
extern Vec vec3_802c3e50[];
extern Vec vec3_802c3e5c[];
static const Vec vec3_802c3fe4[1];
extern Vec vec3_802c4020[];
extern Vec vec3_802c402c[];
extern Vec vec3_802c4038[];
extern Vec vec3_802c4044[];
extern const char mobj_offscreen_prefix[];
extern const char mobj_image_prefix[];
// A typed aggregate copy emits the original four word loads and stores.
typedef struct MobjNameTemplate {
    u32 words[4];
} MobjNameTemplate;
extern Vec lv_block_position[];

// Data objects defined later in this file or in other units, declared before their first use.
extern Vec vec3_802c40d0[];
extern Vec vec3_802c4168[];
extern Vec vec3_802c4174[];
extern const char str_MOBJ_goal_pole[];
extern const char str_MOBJ_goal_hata[];

// Prototypes for functions in other, not-yet-decompiled units (koopa / hit / etc.)

extern u32 kpa_pole_score_color;
extern u32 dat_8041a130;

extern s32 evt_lock_sound[];

// Prototypes missing from manager/fontmgr.h


// Prototypes missing from driver/mobjdrv.h (to move to the header later)

// Prototypes missing from mario/party/sound headers (to move later)

// ---------------------------------------------------------------------------
// Pole score work (sbss `psw`).  The first bytes are the ASCII score string,
// then three floats holding the on-screen position.
// ---------------------------------------------------------------------------
typedef struct PoleWork {
    char str[4];    // 0x0
    u8 terminator;  // 0x4, set to 0 after the digits are written
    u8 pad_0x5[3];  // 0x5
    f32 x;          // 0x8
    f32 y;          // 0xC
    f32 z;          // 0x10
} PoleWork;

PoleWork* psw;

// trailing zero word after psw in the unit's .sbss (declared first: MWCC lays .sbss out last-declared-first)
#pragma force_active on
u32 gap_08_80418714_sbss;
#pragma force_active reset

// Opens retail's .data; no code reads it, but kpa_pole and the objects after it sit at fixed offsets past it.
#ifdef MUST_MATCH
__declspec(section ".data") static f32 negone_one_374[2] = { -1.0f, 1.0f };
#endif

// .rodata (part A)
#pragma force_active on
// vec3_802c3dc8: placeholder for the object the code copies (defining it hangs MWCC)
__declspec(section ".rodata") Vec mobj_vec3_802c3dc8_template[1] = {{0.0f, 0.0f, 0.0f}};
// vec3_802c3dd4: placeholder for the object the code copies (defining it hangs MWCC)
__declspec(section ".rodata") Vec mobj_vec3_802c3dd4_template[1] = {{0.0f, 0.0f, 0.0f}};
#pragma force_active reset
static EVT_BEGIN(evt_shake)
    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.01f), 200)
    RETURN
EVT_END()

extern s32 mobj_save_blk_sysevt[];
#define LVB_TEMPLATE(dst, src) (*(MobjNameTemplate*)(dst) = *(const MobjNameTemplate*)(src))
s32 psndSFXOnEx_3D(const char* name, Vec* position, u8 vol, u8 pan, u16 pitch, u16 flags);

const char* _switch_anim[6] = {
    "A_1", "A_2", "A_3", "A_4", "A_5", "S_1",
};

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
    if (strcmp(name, "me\0") == 0) {
        obj = event->thisMapObj;
    } else {
        obj = mobjNameToPtr(name);
    }
    x = evtGetFloat(event, args[1]);
    y = evtGetFloat(event, args[2]);
    z = evtGetFloat(event, args[3]);
    scale = mobj_vec3_802c3dc8_template[0];
    scale.x = x;
    scale.y = y;
    scale.z = z;
    obj->scale = scale;
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    return 2;
}

s32 evt_mobj_get_position(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;

    name = (const char*)evtGetValue(event, args[0]);
    if (strcmp(name, "me\0") != 0) {
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
    if (strcmp(name, "me\0") != 0) {
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
    if (strcmp(name, "me\0") != 0) {
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
    if (strcmp(name, "me\0") != 0) {
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
    if (strcmp(name, "me\0") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    pos = mobj_vec3_802c3dd4_template[0];
    pos.x = x;
    pos.y = y;
    pos.z = z;
    obj->position = pos;
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    return 2;
}

s32 evt_mobj_set_x_position(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    Vec pos;

    name = (const char*)evtGetValue(event, args[0]);
    if (strcmp(name, "me\0") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    pos = obj->position;
    pos.x = evtGetFloat(event, args[1]);
    obj->position = pos;
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    return 2;
}

s32 evt_mobj_set_y_position(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    Vec pos;

    name = (const char*)evtGetValue(event, args[0]);
    if (strcmp(name, "me\0") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    pos = obj->position;
    pos.y = evtGetFloat(event, args[1]);
    obj->position = pos;
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    return 2;
}

s32 evt_mobj_set_z_position(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    Vec pos;

    name = (const char*)evtGetValue(event, args[0]);
    if (strcmp(name, "me\0") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    pos = obj->position;
    pos.z = evtGetFloat(event, args[1]);
    obj->position = pos;
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    return 2;
}

s32 evt_mobj_exec_cancel(EventEntry* event, BOOL isFirstCall) {
    const char* name;
    MapObjectEntry* obj;

    name = (const char*)evtGetValue(event, event->args[0]);
    if (strcmp(name, "me\0") != 0) {
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
    if (strcmp(name, "me\0") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    obj->gravity = f1;
    obj->bounce = f2;
    return 2;
}

s32 evt_mobj_set_anim(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    const char* animName;
    MapObjectEntry* obj;

    name = (const char*)evtGetValue(event, args[0]);
    animName = (const char*)evtGetValue(event, args[1]);
    if (strcmp(name, "me\0") != 0) {
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
    if (strcmp(name, "me\0") != 0) {
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
    if (strcmp(name, "me\0") != 0) {
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
    if (strcmp(name, "me\0") != 0) {
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
    switch (obj->state) {
        case 0:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                mobjRunEvent(obj, obj->eventCode);
                if (obj->triggerFlag == MOBJ_FLAG_TRIGGER_STAND_ON || obj->triggerFlag == MOBJ_FLAG_TRIGGER_HIP_DROP) {
                    marioChgMot(5);
                }
                psndSFXOn_3D((const char*)0x1d2, &obj->position);
                animPoseSetAnim(obj->poseId, "A_1", 1);
                obj->state++;
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
            evtSetValue(NULL, obj->flagVar, 1);
            obj->state = 0x63;
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
    s32* eventCode = (s32*)evtGetValue(event, args[5]);
    s32 flagVar = args[6];
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
    obj->update = mobj_switch_blue;
    obj->eventCode = eventCode;
    obj->flags |= MOBJ_FLAG_RECALC_MTX | MOBJ_FLAG_GRAVITY;
    obj->flags |= MOBJ_FLAG_TRIGGER_STAND_ON | MOBJ_FLAG_TRIGGER_PARTY | 0x10000;
    obj->flagVar = flagVar;
    obj->state = 0;
    if (type == 2) {
        obj->flags |= 2;
    }
    if (evtGetValue(event, flagVar) != 0) {
        obj->state = 0x63;
    }
    mobjCalcMtx(obj);
    return 2;
}

// `opt_propagation off` keeps the `gp->inBattle` boolean in its own web and ahead of the asyncName
// address, so the two temps colour as retail's (`gp` r5, string address r4) instead of a swapped pair.
#pragma opt_propagation off
s32 evt_mobj_floatswitch_blue(EventEntry* event, BOOL isFirstCall) {
    f32 x;
    s32* args = event->args;
    const char* name;
    const char* asyncName;
    f32 y;
    f32 z;
    s32 flagVar;
    MapObjectEntry* obj;
    s32* eventCode;
    BOOL inBattle;

    evtGetValue(event, args[0]);
    name = (const char*)evtGetValue(event, args[1]);
    x = evtGetFloat(event, args[2]);
    y = evtGetFloat(event, args[3]);
    z = evtGetFloat(event, args[4]);
    eventCode = (s32*)evtGetValue(event, args[5]);
    flagVar = args[6];
    inBattle = !!gp->inBattle;
    asyncName = "MOBJ_BlueSwitchBlock";
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, x, y, z);
    obj->update = mobj_switch_blue;
    obj->eventCode = eventCode;
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    obj->flags |= MOBJ_FLAG_TRIGGER_HEAD;
    obj->flagVar = flagVar;
    obj->state = 0;
    if (evtGetValue(event, flagVar) != 0) {
        obj->state = 0x63;
    }
    mobjCalcMtx(obj);
    return 2;
}

// mwcc folds the one-use `inBattle` local back into the call, which then builds the string
// address before the gp load. Keeping it a real local makes the gp temp first, so the two
// share r5/r4 as in the original.
// `opt_propagation off` keeps `inBattle` a real local so the gp base-load temp is created before the string temp
s32 evt_mobj_tornadoswitch_blue(EventEntry* event, BOOL isFirstCall) {
    f32 x;
    s32* args = event->args;
    const char* name;
    const char* asyncName;
    f32 y;
    f32 z;
    s32 flagVar;
    MapObjectEntry* obj;
    s32* eventCode;
    BOOL inBattle;

    evtGetValue(event, args[0]);
    name = (const char*)evtGetValue(event, args[1]);
    x = evtGetFloat(event, args[2]);
    y = evtGetFloat(event, args[3]);
    z = evtGetFloat(event, args[4]);
    eventCode = (s32*)evtGetValue(event, args[5]);
    flagVar = args[6];
    inBattle = !!gp->inBattle;
    asyncName = "MOBJ_BlueTornadoSwitch";
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, x, y, z);
    obj->update = mobj_switch_blue;
    obj->eventCode = eventCode;
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    obj->flags |= MOBJ_FLAG_TRIGGER_HIP_DROP;
    obj->flagVar = flagVar;
    obj->state = 0;
    if (evtGetValue(event, flagVar) != 0) {
        obj->state = 0x63;
    }
    mobjCalcMtx(obj);
    return 2;
}
#pragma opt_propagation reset

s32 mobj_switch_red(MapObjectEntry* obj) {
    switch (obj->state) {
        case 0:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                mobjRunEvent(obj, obj->eventCode);
                if (obj->triggerFlag == MOBJ_FLAG_TRIGGER_STAND_ON || obj->triggerFlag == MOBJ_FLAG_TRIGGER_HIP_DROP) {
                    marioChgMot(5);
                }
                psndSFXOn_3D((const char*)0x1d2, &obj->position);
                animPoseSetAnim(obj->poseId, "A_1", 1);
                obj->flags &= ~MOBJ_FLAG_TRIGGERED;
            }
            break;
        case 1:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                psndSFXOn_3D((const char*)0x1d2, &obj->position);
                if (obj->triggerFlag == MOBJ_FLAG_TRIGGER_STAND_ON) {
                    marioChgMot(5);
                }
                animPoseSetAnim(obj->poseId, "A_1", 1);
                obj->flags &= ~MOBJ_FLAG_TRIGGERED;
            }
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            animPoseSetAnim(obj->poseId, "S_1", 1);
            obj->flags &= ~MOBJ_FLAG_TRIGGERED;
            obj->state = 0;
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
    s32* eventCode = (s32*)evtGetValue(event, args[5]);
    s32 flagVar = args[6];
    const char* asyncName;
    MapObjectEntry* obj;
    BOOL inBattle = gp->inBattle != 0;

    switch (type) {
        case 0:
            asyncName = "MOBJ_RedSwitch";
            break;
        case 1:
        default:
            asyncName = "MOBJ_WhiteSwitch";
            break;
    }
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, x, y, z);
    obj->update = mobj_switch_red;
    obj->eventCode = eventCode;
    obj->flags |= MOBJ_FLAG_RECALC_MTX | MOBJ_FLAG_GRAVITY;
    obj->flags |= MOBJ_FLAG_TRIGGER_STAND_ON | MOBJ_FLAG_TRIGGER_PARTY | 0x10000;
    obj->flagVar = flagVar;
    obj->state = 0;
    mobjCalcMtx(obj);
    return 2;
}

// Keep the gp->inBattle load ahead of the string-address temp (named local + no propagation): retail
// orders gp=r5 / string-high=r4, and with the load sunk to the call the two temps colour the other way.
#pragma opt_propagation off // keep the inBattle load ahead of the string-address temp so gp=r5, str-high=r4
s32 evt_mobj_floatswitch_red(EventEntry* event, BOOL isFirstCall) {
    f32 x;
    s32* args = event->args;
    const char* name;
    const char* asyncName;
    f32 y;
    f32 z;
    s32 flagVar;
    MapObjectEntry* obj;
    s32* eventCode;
    BOOL inBattle;

    evtGetValue(event, args[0]);
    name = (const char*)evtGetValue(event, args[1]);
    x = evtGetFloat(event, args[2]);
    y = evtGetFloat(event, args[3]);
    z = evtGetFloat(event, args[4]);
    eventCode = (s32*)evtGetValue(event, args[5]);
    flagVar = args[6];
    inBattle = !!gp->inBattle;
    asyncName = "MOBJ_RedSwitchBlock";
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, x, y, z);
    obj->update = mobj_switch_red;
    obj->eventCode = eventCode;
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    obj->flags |= MOBJ_FLAG_TRIGGER_HEAD;
    obj->flagVar = flagVar;
    obj->state = 0;
    if (evtGetValue(event, flagVar) != 0) {
        obj->state = 0x63;
    }
    mobjCalcMtx(obj);
    return 2;
}
#pragma opt_propagation reset

/// @todo reconstructed: retail's evt_mobj .rodata pool has "MOBJ_RedTornadoSwitch" (which no retail function
/// references) just ahead of evt_mobj_floatswitch_red's "MOBJ_RedSwitchBlock", and party_kuribo/party_bomhei test
/// animName against it, so the object existed. Invented: the name (the missing red member of the blue
/// switch/floatswitch/tornadoswitch set) and the body (evt_mobj_tornadoswitch_blue with mobj_switch_red).
#ifdef MUST_MATCH
s32 evt_mobj_tornadoswitch_red(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    const char* asyncName;
    f32 x;
    f32 y;
    f32 z;
    s32* eventCode;
    s32 flagVar;
    MapObjectEntry* obj;

    evtGetValue(event, args[0]);
    name = (const char*)evtGetValue(event, args[1]);
    x = evtGetFloat(event, args[2]);
    y = evtGetFloat(event, args[3]);
    z = evtGetFloat(event, args[4]);
    eventCode = (s32*)evtGetValue(event, args[5]);
    flagVar = args[6];
    asyncName = "MOBJ_RedTornadoSwitch";
    if (!animGroupBaseAsync(asyncName, !!gp->inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, x, y, z);
    obj->update = mobj_switch_red;
    obj->eventCode = eventCode;
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    obj->flags |= MOBJ_FLAG_TRIGGER_HIP_DROP;
    obj->flagVar = flagVar;
    obj->state = 0;
    mobjCalcMtx(obj);
    return 2;
}
#endif

s32 mobj_timerswitch(MapObjectEntry* obj) {
    MapObjectEntry* child = mobjNameToPtr(obj->childName);

    switch (obj->state) {
        case 0:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                if (obj->triggerFlag == MOBJ_FLAG_TRIGGER_STAND_ON) {
                    marioChgMot(5);
                }
                if (child->state != 2) {
                    psndSFXOn_3D((const char*)0x1C1, &obj->position);
                } else {
                    psndSFXOn_3D((const char*)0x1C2, &obj->position);
                }
                evtEntryType(evt_shake, 0, 0, 0);
                animPoseSetAnim(obj->poseId, "A_1", 1);
                obj->state++;
                obj->flags &= ~MOBJ_FLAG_TRIGGERED;
            }
            break;
        case 1:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                animPoseSetAnim(obj->poseId, "S_1", 1);
                animPoseSetAnim(child->poseId, "S_1", 1);
                obj->state = 0;
                break;
            }
            if (animPoseGetLoopTimes(obj->poseId) < 0.3f) {
                break;
            }
            animPoseSetAnim(obj->poseId, "A_2", 1);
            animPoseSetAnim(child->poseId, "A_2", 1);
            obj->state++;
            obj->timer = obj->timerLength;
            obj->flags &= ~MOBJ_FLAG_TRIGGERED;
            break;
        case 2:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                animPoseSetAnim(obj->poseId, "S_1", 1);
                animPoseSetAnim(child->poseId, "S_1", 1);
                obj->state = 0;
                break;
            }
            obj->timer--;
            if (obj->timer <= 0) {
                obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                animPoseSetAnim(obj->poseId, "S_1", 1);
                animPoseSetAnim(child->poseId, "S_1", 1);
                obj->state = 0;
                break;
            }
            if (child->state == 2) {
                if (marioKeyOffChk() != 0 || marioCtrlOffChk() != 0) {
                    obj->state = 0;
                    break;
                }
                mobjRunEvent(obj, obj->eventCode);
                if (strcmp(obj->animName, "MOBJ_RedTimerSwitch") == 0) {
                    obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                    child->flags &= ~MOBJ_FLAG_TRIGGERED;
                    animPoseSetAnim(obj->poseId, "S_1", 1);
                    animPoseSetAnim(child->poseId, "S_1", 1);
                    obj->state = 0;
                    child->state = 0;
                    break;
                }
                effSmallStarEntry(0, 8, obj->position.x, obj->position.y + 12.5f, obj->position.z,
                                  0.0f, -1.0f, 0.0f);
                effSmallStarEntry(0, 8, child->position.x, child->position.y + 12.5f,
                                  child->position.z, 0.0f, -1.0f, 0.0f);
                evtSetValue(NULL, obj->flagVar, 1);
                evtSetValue(NULL, child->flagVar, 1);
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
    s32 timerLength;
    s32* eventCode;
    s32 flagVar;
    BOOL inBattle = gp->inBattle != 0;
    s32 i;
    const char* asyncName;

    {
        const char* jumptable[2] = {"MOBJ_BlueTimerSwitch", "MOBJ_RedTimerSwitch"};
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
    timerLength = evtGetValue(event, args[9]);
    eventCode = (s32*)evtGetValue(event, args[10]);
    flagVar = args[11];
    if (type == 0 && evtGetValue(event, flagVar) != 0) {
        return 2;
    }
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    for (i = 0; i < 2; i++) {
        mobjEntry(names[i], asyncName);
        obj[i] = mobjNameToPtr(names[i]);
        strcpy(obj[i]->childName, (i == 0) ? names[1] : names[0]);
        mobjSetPosition(names[i], xs[i], ys[i], zs[i]);
        obj[i]->update = mobj_timerswitch;
        obj[i]->eventCode = eventCode;
        obj[i]->flags |= MOBJ_FLAG_RECALC_MTX | MOBJ_FLAG_GRAVITY;
        obj[i]->flags |= MOBJ_FLAG_TRIGGER_STAND_ON | MOBJ_FLAG_TRIGGER_PARTY | 0x10000;
        obj[i]->flagVar = flagVar;
        obj[i]->state = 0;
        obj[i]->timerLength = timerLength;
        if (evtGetValue(event, flagVar) != 0) {
            obj[i]->state = 0x63;
        }
        mobjCalcMtx(obj[i]);
    }
    return 2;
}

s32 mobj_rideswitch_lightblue(MapObjectEntry* obj) {
    switch (obj->state) {
        Vec v;
        case 0:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                mobjRunEvent(obj, obj->eventCode);
                psndSFXOn_3D((const char*)0x1d3, &obj->position);
                animPoseSetAnim(obj->poseId, "A_1", 1);
                obj->state = 1;
            }
            break;
        case 1: {
            f32 x = obj->scale2.x;
            f32 y = obj->scale2.y;
            f32 z = obj->scale2.z;
            Vec v;

            y -= 0.9f;

            if (y <= 0.2f) {
                obj->state = 2;
                obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                y = 0.2f;
            }
            v = vec3_802c3e44[0];
            v.x = x;
            v.y = y;
            v.z = z;
            obj->scale2 = v;
            mobjCalcMtx2(obj);
            break;
        }
        case 2: {
            AnimationPoseData* data = animPoseGetAnimBaseDataPtr(obj->poseId);
            Vec w = vec3_802c3e50[0];
            f32 mag;
            f32 dist;

            w.x = 0.8f * (10.0f * data->bboxMax.x);
            w.y = 10.0f * data->bboxMax.y;
            w.z = 0.8f * (10.0f * data->bboxMax.z);
            v = w;
            mag = PSVECMag(&v);
            dist = PSVECDistance(&obj->position, &marioGetPtr()->position);
            if (dist > mag) {
                if (!(obj->flags & MOBJ_FLAG_TRIGGERED)) {
                    obj->state = 3;
                    animPoseSetAnim(obj->poseId, "A_2", 1);
                    psndSFXOn_3D((const char*)0x1d4, &obj->position);
                    mobjRunEvent(obj, obj->eventCode2);
                } else {
                    obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                }
            }
            break;
        }
        case 3: {
            f32 x = obj->scale2.x;
            f32 y = obj->scale2.y;
            f32 z = obj->scale2.z;
            Vec v;

            y += 0.9f;

            if (y >= 15.0f) {
                obj->state = 0;
                y = 15.0f;
            }
            v = vec3_802c3e5c[0];
            v.x = x;
            v.y = y;
            v.z = z;
            obj->scale2 = v;
            mobjCalcMtx2(obj);
            obj->flags &= ~MOBJ_FLAG_TRIGGERED;
            break;
        }
        case 0x63:
            break;
    }
    return 0;
}

/// @todo reconstructed: retail's pool has "MOBJ_RideSwitchA" (unreferenced) after "MOBJ_RideSwitchB", both
/// following evt_mobj_rideswitch_lightblue's "MOBJ_RideSwitchC" (codegen runs in reverse source order), and
/// party_kuribo/party_bomhei test animName against it. Invented: the name and the body (rideswitch_lightblue's).
#ifdef MUST_MATCH
s32 evt_mobj_rideswitch_a(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    const char* asyncName;
    f32 x;
    f32 y;
    f32 z;
    s32* eventCode;
    s32* eventCode2;
    s32 flagVar;
    MapObjectEntry* obj;

    name = (const char*)evtGetValue(event, args[0]);
    x = evtGetFloat(event, args[1]);
    y = evtGetFloat(event, args[2]);
    z = evtGetFloat(event, args[3]);
    eventCode = (s32*)evtGetValue(event, args[4]);
    eventCode2 = (s32*)evtGetValue(event, args[5]);
    flagVar = args[6];
    asyncName = "MOBJ_RideSwitchA";
    if (!animGroupBaseAsync(asyncName, !!gp->inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, x, y, z);
    obj->update = mobj_rideswitch_lightblue;
    obj->eventCode = eventCode;
    obj->eventCode2 = eventCode2;
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    obj->flags |= MOBJ_FLAG_TRIGGER_STAND_ON;
    obj->flagVar = flagVar;
    obj->state = 0;
    mobjCalcMtx2(obj);
    return 2;
}
#endif

/// @todo reconstructed: retail's pool has "MOBJ_RideSwitchB" (unreferenced) between evt_mobj_rideswitch_lightblue's
/// "MOBJ_RideSwitchC" and "MOBJ_RideSwitchA", and party_kuribo/party_bomhei test animName against it. Invented: the
/// name and the body (rideswitch_lightblue's).
#ifdef MUST_MATCH
s32 evt_mobj_rideswitch_b(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    const char* asyncName;
    f32 x;
    f32 y;
    f32 z;
    s32* eventCode;
    s32* eventCode2;
    s32 flagVar;
    MapObjectEntry* obj;

    name = (const char*)evtGetValue(event, args[0]);
    x = evtGetFloat(event, args[1]);
    y = evtGetFloat(event, args[2]);
    z = evtGetFloat(event, args[3]);
    eventCode = (s32*)evtGetValue(event, args[4]);
    eventCode2 = (s32*)evtGetValue(event, args[5]);
    flagVar = args[6];
    asyncName = "MOBJ_RideSwitchB";
    if (!animGroupBaseAsync(asyncName, !!gp->inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, x, y, z);
    obj->update = mobj_rideswitch_lightblue;
    obj->eventCode = eventCode;
    obj->eventCode2 = eventCode2;
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    obj->flags |= MOBJ_FLAG_TRIGGER_STAND_ON;
    obj->flagVar = flagVar;
    obj->state = 0;
    mobjCalcMtx2(obj);
    return 2;
}
#endif

// The gp->inBattle read is kept in its own local ahead of the asyncName string address, so the gp
// load temp colours r5 and the string-address temp r4 as in the original.
#pragma opt_propagation off
s32 evt_mobj_rideswitch_lightblue(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 flagVar;
    const char* name;
    MapObjectEntry* obj;
    f32 x;
    f32 y;
    f32 z;
    s32* eventCode;
    s32* eventCode2;
    const char* asyncName;
    BOOL inBattle;

    name = (const char*)evtGetValue(event, args[0]);
    x = evtGetFloat(event, args[1]);
    y = evtGetFloat(event, args[2]);
    z = evtGetFloat(event, args[3]);
    eventCode = (s32*)evtGetValue(event, args[4]);
    eventCode2 = (s32*)evtGetValue(event, args[5]);
    inBattle = gp->inBattle != 0;
    asyncName = "MOBJ_RideSwitchC";
    flagVar = args[6];
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    obj->scale2.x *= 0.8f;
    obj->scale2.z *= 0.8f;
    mobjSetPosition(name, x, y, z);
    obj->update = mobj_rideswitch_lightblue;
    obj->eventCode = eventCode;
    obj->eventCode2 = eventCode2;
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    obj->flags |= MOBJ_FLAG_TRIGGER_STAND_ON;
    obj->flagVar = flagVar;
    obj->state = 0;
    mobjCalcMtx2(obj);
    return 2;
}
#pragma opt_propagation reset

s32 mobj_jumpstand_red(MapObjectEntry* obj) {
    MarioWork* mario = marioGetPtr();
    s32 partyId = marioGetPartyId();

    partyGetPtr(partyId);
    switch (obj->state) {
        case 0:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                EventEntry* evt;
                gp->systemLevelFlags |= 0x10;
                evt = mobjRunEvent(obj, obj->eventCode);
                if (evt != NULL) {
                    s32 flag;
                    if (mario->lastGroundPos.y < mario->position.y) {
                        flag = 0;
                    } else if (mario->lastGroundPos.y <
                               obj->position.y + mario->wPlayerCollisionBox.y) {
                        flag = 0;
                    } else {
                        flag = 1;
                    }
                    evt->lwData[0] = flag;
                }
                if (mario->characterId != 2) {
                    if (strcmp(obj->animName, "MOBJ_RedJumpStand") == 0) {
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
                } else if (strcmp(obj->animName, "MOBJ_RedJumpStand") == 0) {
                    psndSFXOn((const char*)0x1eb);
                } else {
                    psndSFXOn((const char*)0x1ec);
                }
                animPoseSetAnim(obj->poseId, "A_1", 1);
                obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                obj->state++;
            }
            break;
        case 1:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                if (evtCheckID(obj->eventId) == 0) {
                    animPoseSetAnim(obj->poseId, "S_1", 1);
                    obj->state = 0;
                    break;
                }
            }
            if (evtCheckID(obj->eventId) != 1) {
                gp->systemLevelFlags &= ~0x10;
                if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                    break;
                }
                animPoseSetAnim(obj->poseId, "S_1", 1);
                obj->state = 0;
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
#pragma force_active reset

s32 evt_mobj_jumpstand_red(EventEntry* event, BOOL isFirstCall) {
    MapObjectEntry* obj;
    const char* asyncName;
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    const char* name = (const char*)evtGetValue(event, args[1]);
    f32 x = evtGetFloat(event, args[2]);
    f32 y = evtGetFloat(event, args[3]);
    f32 z = evtGetFloat(event, args[4]);
    s32* eventCode = (s32*)evtGetValue(event, args[5]);
    s32 flagVar = args[6];
    s32 inBattle;
    inBattle = gp->inBattle != 0;
    {
        const char* jumptable[2] = {"MOBJ_RedJumpStand", "MOBJ_RedJumpStand2"};
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
    mobjSetPosition(name, x, y, z);
    obj->update = mobj_jumpstand_red;
    obj->eventCode = eventCode;
    obj->flags |= MOBJ_FLAG_RECALC_MTX | MOBJ_FLAG_GRAVITY;
    obj->flags |= MOBJ_FLAG_TRIGGER_STAND_ON;
    obj->flagVar = flagVar;
    obj->state = 0;
    mobjCalcMtx(obj);
    return 2;
}

s32 mobj_jumpstand_blue(MapObjectEntry* obj) {
    MarioWork* mario = marioGetPtr();

    switch (obj->state) {
        case 0:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                gp->systemLevelFlags |= 0x10;
                mobjRunEvent(obj, obj->eventCode);
                if (obj->triggerFlag == MOBJ_FLAG_TRIGGER_STAND_ON) {
                    marioChgJumpStandMotion(obj->jumpHeight);
                }
                if (mario->characterId != 2) {
                    if (strcmp(obj->animName, "MOBJ_BlueJumpStand") == 0) {
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
                } else if (strcmp(obj->animName, "MOBJ_BlueJumpStand") == 0) {
                    psndSFXOn((const char*)0x1eb);
                } else {
                    psndSFXOn((const char*)0x1ec);
                }
                animPoseSetAnim(obj->poseId, "A_1", 1);
                obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                obj->state++;
            }
            break;
        case 1:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                animPoseSetAnim(obj->poseId, "S_1", 1);
                obj->state = 0;
            } else if (evtCheckID(obj->eventId) != 1) {
                gp->systemLevelFlags &= ~0x10;
                if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                    break;
                }
                animPoseSetAnim(obj->poseId, "S_1", 1);
                obj->state = 0;
            }
            break;
    }
    return 0;
}

// .rodata (part B)
#pragma force_active on
#pragma force_active reset

s32 evt_mobj_jumpstand_blue(EventEntry* event, BOOL isFirstCall) {
    MapObjectEntry* obj;
    const char* asyncName;
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    const char* name = (const char*)evtGetValue(event, args[1]);
    f32 jumpHeight = evtGetFloat(event, args[2]);
    f32 x = evtGetFloat(event, args[3]);
    f32 y = evtGetFloat(event, args[4]);
    f32 z = evtGetFloat(event, args[5]);
    s32* eventCode = (s32*)evtGetValue(event, args[6]);
    s32 flagVar = args[7];
    s32 inBattle;
    inBattle = gp->inBattle != 0;
    {
        const char* jumptable[2] = {"MOBJ_BlueJumpStand", "MOBJ_BlueJumpStand2"};
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
    mobjSetPosition(name, x, y, z);
    obj->update = mobj_jumpstand_blue;
    obj->eventCode = eventCode;
    obj->flags |= MOBJ_FLAG_RECALC_MTX | MOBJ_FLAG_GRAVITY;
    obj->flags |= MOBJ_FLAG_TRIGGER_STAND_ON;
    obj->flagVar = flagVar;
    obj->state = 0;
    obj->jumpHeight = jumpHeight;
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

    LVB_TEMPLATE(offName, mobj_offscreen_prefix);
    LVB_TEMPLATE(imgName, mobj_image_prefix);
    inBattle = gp->inBattle != 0;
    mario = marioGetPtr();
    strcat(offName, obj->name);
    strcat(imgName, obj->name);
    switch (obj->state) {
        case 0:
            animGroupBaseAsync("P_box_y", inBattle, NULL);
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                if (strcmp(obj->animName, "MOBJ_Lv1Block") != 0 &&
                    strcmp(obj->animName, "MOBJ_Lv1BigBlock") != 0) {
                    if (strcmp(obj->animName, "MOBJ_Lv1BigBigBlock") == 0) {
                        if (pouchGetHammerLv() < 2 || mario->currMotionId != MARIO_MOTION_HAMMER_2) {
                            obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                            break;
                        }
                    } else if (strcmp(obj->animName, "MOBJ_Lv2Block") == 0 ||
                               strcmp(obj->animName, "MOBJ_Lv2BigBlock") == 0) {
                        if (pouchGetHammerLv() < 3) {
                            obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                            break;
                        }
                    } else if (strcmp(obj->animName, "MOBJ_Lv2BigBigBlock") == 0) {
                        if (pouchGetHammerLv() < 3 || mario->currMotionId != MARIO_MOTION_HAMMER_2) {
                            obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                            break;
                        }
                    } else {
                        obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                        break;
                    }
                }
                mobjRunEvent(obj, obj->eventCode);
                evtSetValue(NULL, obj->flagVar, 1);
                obj->state = 0xa;
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
            img->animName = "A_1";
            img->startTime = animTimeGetTime(inBattle);
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
            obj->state = 0xb;
            break;
        }
        case 0xb:
            img = imgNameToPtr(imgName, inBattle);
            if (img->loopTimes >= 1.0f) {
                obj->state = 0xc;
            }
            if (img->flags & 0x10000000) {
                obj->state = 0xc;
            }
            obj->position = lv_block_position[0];
            obj->flags |= MOBJ_FLAG_RECALC_MTX;
            obj->flags |= MOBJ_FLAG_TRIGGERED;
            break;
        case 0xc:
            img = imgNameToPtr(imgName, inBattle);
            imgFreeCapture(img, 0);
            imgFreeCapture(img, 1);
            imgSetShadow(img, 0);
            imgRelease(img, inBattle);
            offscreenDelete(offName);
            obj->offscreenId = -1;
            obj->state = 0x63;
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
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    s32* eventCode = (s32*)evtGetValue(event, args[4]);
    s32 flagVar = args[5];
    const char* asyncName = (const char*)evtGetValue(event, args[6]);
    MapObjectEntry* obj;
    Vec pos;
    BOOL inBattle = gp->inBattle != 0;

    if (evtGetValue(event, flagVar) != 0) {
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
    obj->flags |= MOBJ_FLAG_RECALC_MTX | MOBJ_FLAG_GRAVITY;
    pos = mobj_vec3_802c3ef4_template[0];
    pos.x = x;
    pos.y = y;
    pos.z = z;
    obj->position = pos;
    obj->update = mobj_lv_blk_evt;
    obj->eventCode = eventCode;
    obj->eventCode2 = NULL;
    obj->flags |= 0x10000;
    obj->flagVar = flagVar;
    obj->state = 0;
    return 2;
}

s32 mobj_float_blk(MapObjectEntry* obj) {
    switch (obj->state) {
        case 0x63:
            if (obj->childName[0] == 0) {
                f32 hitNZ, hitNY, hitNX, distance, hitZ, hitY, hitX;
                HitEntry* hit;

                distance = 1.0f + (f32)__fabs(obj->velocity.y);
                hit = hitCheckFilter(NULL, obj->position.x, 0.5f + obj->position.y, obj->position.z, 0.0f, (-1.0f), 0.0f, &hitX,
                                        &hitY, &hitZ, &distance,
                                        &hitNX, &hitNY, &hitNZ);
#ifdef MUST_MATCH
                // match-only: the int test is retail's cmpwi (the pointer test compiles to cmplwi)
                if ((s32)hit != 0) {
#else
                if (hit != NULL) {
#endif
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
                AnimTableEntry* anim = animPoseGetCurrentAnim(mobjNameToPtr(obj->childName)->poseId);

                if (anim->name[0] == 'A' && anim->name[2] >= '1' && anim->name[2] <= '3') {
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
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    s32 type = args[4];
    s32 sub = args[5];
    s32 flagVar;
    const char* asyncName;
    MapObjectEntry* obj;
    BOOL inBattle;

    evtGetValue(event, args[6]);
    flagVar = args[7];
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
    mobjSetPosition(name, x, y, z);
    obj->update = mobj_float_blk;
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    obj->flagVar = flagVar;
    obj->state = 0x63;
    mobjCalcMtx(obj);
    {
        f32 hitNZ, hitNY, hitNX, distance, hitZ, hitY, hitX;
        HitEntry* hit;

        distance = 1.0f + (f32)__fabs(obj->velocity.y);
        hit = hitCheckFilter(NULL, obj->position.x, 0.5f + obj->position.y, obj->position.z, 0.0f, (-1.0f), 0.0f, &hitX,
                                &hitY, &hitZ, &distance,
                                &hitNX, &hitNY, &hitNZ);
#ifdef MUST_MATCH
        // match-only: the int test is retail's cmpwi (the pointer test compiles to cmplwi)
        if ((s32)hit != 0) {
#else
        if (hit != NULL) {
#endif
            animPoseSetAnim(obj->poseId, "S_1", 1);
            obj->flags |= 2;
        } else {
            animPoseSetAnim(obj->poseId, "A_1", 1);
        }
    }
    return 2;
}

// Data for mobj_switch_float_blk only (unit .rodata).

#ifdef MUST_MATCH
// match-only, invented helper name: the case-0 `_switch_anim` scan. Written as its own function body, the frontend
// keeps the loop-init copy (`i = idx`) that the same code inline in the caller folds to `li`; the local declaration
// order below then gives retail's registers (p r27, anim r28, idx r29, i r30).
static inline s32 mobjSwitchAnimIndex(MapObjectEntry* o) {
    const char** p;
    AnimTableEntry* anim;
    s32 idx;
    s32 i;

    anim = animPoseGetCurrentAnim(o->poseId);
    idx = 0;
    p = _switch_anim;
    for (i = idx; i < 6; i++, p++) {
        if (strcmp(*p, anim->name) == 0) {
            idx = i;
            break;
        }
    }
    return idx;
}
#endif

// Switch-block state machine: picks the child/partner anim based on whether the linked block
// is hit, runs the event, and opens/closes its animation.  `_switch_anim` is the six anim-name
// table in .sdata.
s32 mobj_switch_float_blk(MapObjectEntry* obj) {
    switch (obj->state) {
        case 0: {
            AnimTableEntry* anim;
            s32 animId;

            if (!(obj->flags & MOBJ_FLAG_TRIGGERED)) {
                break;
            }
#ifdef MUST_MATCH
            obj->field_0x1A4 = (obj->field_0x1A4 & 0x40000000) | mobjSwitchAnimIndex(obj);
#else
            {
                const char** p;
                s32 idx;
                s32 i;

                anim = animPoseGetCurrentAnim(obj->poseId);
                idx = 0;
                p = _switch_anim;
                for (i = 0; i < 6; i++, p++) {
                    if (strcmp(*p, anim->name) == 0) {
                        idx = i;
                        break;
                    }
                }
                obj->field_0x1A4 = (obj->field_0x1A4 & 0x40000000) | idx;
            }
#endif
            if (obj->childName[0] != 0) {
                MapObjectEntry* child = mobjNameToPtr(obj->childName);
                f32 hitNZ, hitNY, hitNX, distance, hitZ, hitY, hitX;
                HitEntry* hit;

                distance = 1.0f + (f32)__fabs(child->velocity.y);
                hit = hitCheckFilter(NULL, child->position.x, 0.5f + child->position.y, child->position.z, 0.0f, (-1.0f), 0.0f, &hitX,
                                        &hitY, &hitZ,
                                        &distance, &hitNX, &hitNY,
                                        &hitNZ);
#ifdef MUST_MATCH
                // match-only: the int test is retail's cmpwi (the pointer test compiles to cmplwi)
                if ((s32)hit != 0) {
#else
                if (hit != NULL) {
#endif
                    if (evtCheckID(obj->eventId) != 0) {
                        anim = animPoseGetCurrentAnim(obj->poseId);
                        {
                            BOOL isA;
                            if (anim->name[0] == 'A' && anim->name[2] >= '1' &&
                                anim->name[2] <= '3') {
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
                        obj->field_0x1A4 |= 0x80000000;
                        animId = 0;
                    }
                } else {
                    if (evtCheckID(obj->eventId) != 0) {
                        anim = animPoseGetCurrentAnim(obj->poseId);
                        {
                            char* n = anim->name;
                            char c = n[0];
                            if (c == 'A' && n[2] >= '1' && n[2] <= '2') {
                                animId = 0;
                            } else if (c == 'A' && n[2] == '3') {
                                obj->field_0x1A4 |= 0x40000000;
                                animId = 0;
                            } else if (c == 'A' && n[2] >= '4' && n[2] <= '5') {
                                animId = 3;
                            } else {
                                animId = 3;
                            }
                        }
                    } else {
                        obj->field_0x1A4 |= 0x80000000;
                        animId = 3;
                    }
                }
            } else {
                anim = animPoseGetCurrentAnim(obj->poseId);
                {
                    BOOL isA;
                    if (anim->name[0] == 'A' && anim->name[2] >= '1' &&
                        anim->name[2] <= '3') {
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
                obj->field_0x1A4 |= 0x80000000;
            }
            mobjRunEvent(obj, obj->eventCode);
            if (obj->triggerFlag == MOBJ_FLAG_TRIGGER_STAND_ON) {
                marioChgMot(5);
            }
            psndSFXOn_3D((const char*)0x1b5, &obj->position);
            switch (obj->triggerFlag) {
            case 0x400000:
                animPoseSetAnim(obj->poseId, _switch_anim[animId + 1], 1);
                break;
            default:
                animPoseSetAnim(obj->poseId, _switch_anim[animId], 1);
                break;
            }
            obj->flags &= ~MOBJ_FLAG_TRIGGERED;
            if (obj->timer != 0) {
                obj->state = 0xa;
            } else {
                obj->state++;
            }
            break;
        }
        case 1:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                obj->state = 0;
                break;
            }
            if (obj->flags & 0x1000) {
                obj->flags &= ~0x1000;
                obj->state = 0xa;
                break;
            }
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            if (obj->childName[0] != 0) {
                mobjNameToPtr(obj->childName);
            }
            {
                const char* nm;
                AnimTableEntry* a = animPoseGetCurrentAnim(obj->poseId);
                BOOL isA;
                if (a->name[0] == 'A' && a->name[2] >= '1' && a->name[2] <= '3') {
                    isA = 1;
                } else {
                    isA = 0;
                }
                if (isA) {
                    nm = "A_3";
                } else {
                    nm = "S_1";
                }
                animPoseSetAnim(obj->poseId, nm, 1);
            }
            obj->state++;
            break;
        case 2:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                obj->state = 0;
                break;
            }
            if (obj->flags & 0x1000) {
                obj->flags &= ~0x1000;
                obj->state = 0xa;
            }
            if (evtCheckID(obj->eventId) != 0) {
                break;
            }
            obj->state = 0;
            obj->field_0x1A4 = 0;
            break;
        case 0xa:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                obj->state = 0;
                break;
            }
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            {
                AnimTableEntry* anim = animPoseGetCurrentAnim(obj->poseId);
                BOOL isA;
                if ((anim->name[0] == 'A' && anim->name[2] >= '1' && anim->name[2] <= '3')) {
                    isA = 1;
                } else {
                    isA = 0;
                }
                if (isA) {
                    animPoseSetAnim(obj->poseId, "A_3", 1);
                } else {
                    animPoseSetAnim(obj->poseId, "S_1", 1);
                }
            }
            obj->timer = 0x78;
            obj->state++;
        case 0xb:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                obj->state = 0;
                break;
            }
            obj->timer--;
            if (obj->timer != 0) {
                break;
            }
            if (obj->childName[0] != 0) {
                MapObjectEntry* child = mobjNameToPtr(obj->childName);
                f32 hitNZ, hitNY, hitNX, distance, hitZ, hitY, hitX;
                HitEntry* hit;

                distance = 1.0f + (f32)__fabs(child->velocity.y);
                hit = hitCheckFilter(NULL, child->position.x, 0.5f + child->position.y, child->position.z, 0.0f, (-1.0f), 0.0f, &hitX,
                                        &hitY, &hitZ,
                                        &distance, &hitNX, &hitNY,
                                        &hitNZ);
#ifdef MUST_MATCH
                // match-only: the int test is retail's cmpwi (the pointer test compiles to cmplwi)
                if ((s32)hit != 0) {
#else
                if (hit != NULL) {
#endif
                    animPoseSetAnim(obj->poseId, "S_1", 0);
                } else {
                    animPoseSetAnim(obj->poseId, "A_3", 0);
                }
            } else {
                AnimTableEntry* anim = animPoseGetCurrentAnim(obj->poseId);
                BOOL isA;
                if ((anim->name[0] == 'A' && anim->name[2] >= '1' && anim->name[2] <= '3')) {
                    isA = 1;
                } else {
                    isA = 0;
                }
                if (isA) {
                    animPoseSetAnim(obj->poseId, "A_3", 1);
                } else {
                    animPoseSetAnim(obj->poseId, "S_1", 1);
                }
            }
            obj->state = 0;
            break;
    }
    return 0;
}

// The float-block switch animation names pool in their original codegen order.
s32 evt_mobj_switch_float_blk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    const char* childName = (const char*)evtGetValue(event, args[4]);
    s32 type = args[5];
    s32* eventCode = (s32*)evtGetValue(event, args[6]);
    s32 flagVar = args[7];
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
    mobjSetPosition(name, x, y, z);
    obj->update = mobj_switch_float_blk;
    obj->eventCode = eventCode;
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    obj->flags |= MOBJ_FLAG_TRIGGER_STAND_ON | MOBJ_FLAG_TRIGGER_HEAD | MOBJ_FLAG_TRIGGER_PARTY | 0x10000;
    obj->flagVar = flagVar;
    obj->state = 0;
    obj->timer = 0;
    mobjCalcMtx(obj);
    strcpy(obj->childName, childName);
    if (obj->childName[0] != 0) {
        f32 hitNZ, hitNY, hitNX, distance, hitZ, hitY, hitX;
        MapObjectEntry* obj2 = mobjNameToPtr(obj->childName);
        HitEntry* hit;
        strcpy(obj2->childName, obj->name);
        distance = 1.0f + (f32)__fabs(obj2->velocity.y);
        hit = hitCheckFilter(NULL, obj2->position.x, 0.5f + obj2->position.y, obj2->position.z, 0.0f, (-1.0f), 0.0f, &hitX,
                             &hitY, &hitZ,
                             &distance, &hitNX, &hitNY,
                             &hitNZ);
#ifdef MUST_MATCH
        // match-only: the int test is retail's cmpwi (the pointer test compiles to cmplwi)
        if ((s32)hit != 0) {
#else
        if (hit != NULL) {
#endif
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
    if (obj->update == NULL) {
        obj->update = mobj_lock;
    }
    obj->flags |= MOBJ_FLAG_TRIGGERED;
    marioKeyOff();
    partyKeyOff();
    marioStSystemLevel(1);
    obj->state = 3;
    return 2;
}

s32 mobj_lock(MapObjectEntry* obj) {
    switch (obj->state) {
        case 0:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                marioKeyOff();
                partyKeyOff();
                marioStSystemLevel(1);
                mobjRunEvent(obj, obj->eventCode);
                obj->state++;
            }
            break;
        case 1:
            if (evtCheckID(obj->eventId) == 1) {
                break;
            }
            if (obj->flags & 0x1000) {
                obj->flags &= ~0x1000;
                obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                marioKeyOn();
                partyKeyOn();
                marioStSystemLevel(0);
                obj->eventId = 0;
                obj->state = 0;
            } else {
                obj->eventId = 0;
                obj->state++;
            }
            break;
        case 2: {
            s32 item;

            if (obj->itemId & 0x80000000) {
                s32 i;
                s32 check;

                item = 0;
                i = 0;
                while (1) {
                    check = obj->itemList[i];
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
                s32 check = pouchCheckItem(obj->itemId);

                item = (check != 0) ? obj->itemId : 0;
            }
            if (item != 0) {
                pouchRemoveItem(item);
                obj->state++;
            } else {
                obj->msgWindowId = msgWindow_Entry(msgSearch("sys_no_key"), 0, 0);
                obj->state = 0xa;
            }
            break;
        }
        case 3: {
            EventEntry* event = evtEntryType(evt_lock_sound, 0, 0, 0);

            event->lwData[0] = (s32)(1024.0f * obj->position.x) + 0xF24A7A80;
            event->lwData[1] = (s32)(1024.0f * obj->position.y) + 0xF24A7A80;
            event->lwData[2] = (s32)(1024.0f * obj->position.z) + 0xF24A7A80;
            animPoseSetAnim(obj->poseId, "S_1", 1);
            obj->state++;
            break;
        }
        case 4:
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            obj->velocity.y = 0.0f;
            obj->gravity = 0.5f;
            obj->bounce = 0.0f;
            evtSetValue(NULL, obj->flagVar, 1);
            mobjRunEvent(obj, obj->eventCode2);
            obj->state++;
            break;
        case 5: {
            f32 hitX, hitY, hitZ, distance, hitNX, hitNY, hitNZ;

            distance = 1.0f + (f32)__fabs(obj->velocity.y);
            if (hitCheckFilter(NULL, obj->position.x, 1.0f + obj->position.y, obj->position.z, 0.0f, obj->velocity.y, 0.0f, &hitX,
                               &hitY, &hitZ, &distance,
                               &hitNX, &hitNY, &hitNZ) != NULL) {
                obj->position.y = hitY;
                obj->velocity.y *= (-0.8f);
                obj->bounce += 1.0f;
                if (obj->bounce < 4.0f) {
                    psndSFXOn_3D((const char*)0x1e6, &obj->position);
                }
            } else {
                obj->position.y += obj->velocity.y;
                obj->velocity.y -= obj->gravity;
            }
            if (obj->bounce < 4.0f) {
                break;
            }
            if (evtCheckID(obj->eventId) != 1) {
                obj->state++;
                obj->timer = 0x14;
            }
            break;
        }
        case 6:
            obj->timer--;
            if (obj->timer & 2) {
                obj->flags |= MOBJ_FLAG_NO_DISP;
            } else {
                obj->flags &= ~MOBJ_FLAG_NO_DISP;
            }
            if (obj->timer == 0) {
                obj->state = 0x63;
            }
            break;
        case 10:
            if (windowCheckID(obj->msgWindowId) == 0) {
                obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                marioKeyOn();
                partyKeyOn();
                marioStSystemLevel(0);
                obj->state = 0;
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
    s32 itemId = evtGetValue(event, args[1]);
    f32 x = evtGetFloat(event, args[2]);
    f32 y = evtGetFloat(event, args[3]);
    f32 z = evtGetFloat(event, args[4]);
    f32 rotY = evtGetFloat(event, args[5]);
    s32* eventCode = (s32*)evtGetValue(event, args[6]);
    s32* eventCode2 = (s32*)evtGetValue(event, args[7]);
    s32 flagVar = args[8];
    BOOL inBattle = gp->inBattle != 0;
    MapObjectEntry* obj;
    const char* asyncName = "MOBJ_Lock";

    if (evtGetValue(event, flagVar) != 0) {
        return 2;
    }
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjHitEntry(obj, 2);
    obj->itemId = itemId;
    mobjSetPosition(name, x, y, z);
    obj->rotation.y = rotY;
    obj->update = mobj_lock;
    obj->eventCode = eventCode;
    obj->eventCode2 = eventCode2;
    obj->flags |= MOBJ_FLAG_RECALC_MTX | 0x22;
    obj->flags |= 0x40000;
    obj->flagVar = flagVar;
    obj->state = 0;
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "Z_1", 1);
    return 2;
}

// Itembox per-frame update callback (obj->update). Installed by evt_mobj_itembox.
s32 mobj_itembox(MapObjectEntry* obj) {
    switch (obj->state) {
        case 0:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                marioKeyOff();
                partyKeyOff();
                marioStSystemLevel(1);
                obj->eventId = 0;
                mobjRunEvent(obj, obj->eventCode);
                obj->state++;
            }
            break;
        case 1:
            if (obj->eventId != 0) {
                if (evtCheckID(obj->eventId) == 1) {
                    break;
                }
                if (obj->flags & 0x1000) {
                    obj->flags &= ~0x1000;
                    obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                    marioKeyOn();
                    partyKeyOn();
                    marioStSystemLevel(0);
                    obj->eventId = 0;
                    obj->state = 0;
                    break;
                }
            }
            obj->state++;
            break;
        case 2: {
            EventEntry* event;

            if (strcmp(obj->animName, "MOBJ_TreasureBox") == 0 ||
                strcmp(obj->animName, "MOBJ_GrayTreasureBox") == 0) {
                event = evtEntryType(evt_box_sound1, 0, 0, 0);
                event->lwData[0] = (s32)(1024.0f * obj->position.x) + 0xF24A7A80;
                event->lwData[1] = (s32)(1024.0f * obj->position.y) + 0xF24A7A80;
                event->lwData[2] = (s32)(1024.0f * obj->position.z) + 0xF24A7A80;
            } else if (strcmp(obj->animName, "MOBJ_BlackTreasureBox") == 0) {
                event = evtEntryType(evt_box_sound3, 0, 0, 0);
                event->lwData[0] = (s32)(1024.0f * obj->position.x) + 0xF24A7A80;
                event->lwData[1] = (s32)(1024.0f * obj->position.y) + 0xF24A7A80;
                event->lwData[2] = (s32)(1024.0f * obj->position.z) + 0xF24A7A80;
            } else {
                event = evtEntryType(evt_box_sound2, 0, 0, 0);
                event->lwData[0] = (s32)(1024.0f * obj->position.x) + 0xF24A7A80;
                event->lwData[1] = (s32)(1024.0f * obj->position.y) + 0xF24A7A80;
                event->lwData[2] = (s32)(1024.0f * obj->position.z) + 0xF24A7A80;
            }
            animPoseSetAnim(obj->poseId, "A_1", 1);
            obj->eventId = 0;
            mobjRunEvent(obj, obj->eventCode2);
            obj->state++;
            break;
        }
        case 3: {
            AnimationPoseData* animData;
            Vec maxTmp;
            Vec minTmp;
            Vec bboxMax;
            Vec bboxMin;

            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            if (obj->eventId != 0 && evtCheckID(obj->eventId) == 1) {
                break;
            }
            obj->flags &= ~0x20;
            animData = animPoseGetAnimBaseDataPtr(obj->poseId);
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
            PSVECScale(&obj->translation, &obj->translation, 0.5f);
            PSVECScale(&obj->translation, &obj->translation, 10.0f);
            obj->translation.y = 0.0f;
            obj->scale2.y *= 1.4f;
            obj->scale2.z *= 1.2f;
            mobjCalcMtx2(obj);
            marioKeyOn();
            partyKeyOn();
            marioStSystemLevel(0);
            obj->eventId = 0;
            obj->state = 0x63;
            break;
        }
        case 0x63:
            evtSetValue(NULL, obj->flagVar, 1);
            obj->flags |= MOBJ_FLAG_TRIGGERED;
            obj->flags &= ~0x20;
            if (strcmp(obj->animName, "MOBJ_BlackTreasureBox") == 0) {
                animPoseSetAnim(obj->poseId, "S_4", 1);
            } else {
                animPoseSetAnim(obj->poseId, "S_2", 1);
            }
            break;
    }
    return 0;
}

s32 evt_mobj_itembox(EventEntry* event, BOOL isFirstCall) {

    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    s32 type = evtGetValue(event, args[4]);
    s32* eventCode = (s32*)evtGetValue(event, args[5]);
    s32* eventCode2 = (s32*)evtGetValue(event, args[6]);
    s32 flagVar = args[7];
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
    mobjSetPosition(name, x, y, z);
    obj->update = mobj_itembox;
    obj->eventCode = eventCode;
    obj->eventCode2 = eventCode2;
    obj->flags |= MOBJ_FLAG_RECALC_MTX | MOBJ_FLAG_GRAVITY | 0x20;
    obj->flags |= 0x40000;
    obj->flagVar = flagVar;
    obj->state = 0;
    mobjCalcMtx(obj);
    if (evtGetValue(event, flagVar) != 0) {
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
        PSVECScale(&obj->translation, &obj->translation, 0.5f);
        PSVECScale(&obj->translation, &obj->translation, 10.0f);
        obj->translation.y = 0.0f;
        obj->scale2.y *= 1.4f;
        obj->scale2.z *= 1.2f;
        mobjCalcMtx2(obj);
        obj->state = 0x63;
    }
    {
        f32 hitX;
        f32 hitY;
        f32 hitZ;
        f32 distance = 5.0f + (f32)__fabs(obj->velocity.y);
        f32 hitNX;
        f32 hitNY;
        f32 hitNZ;

        if (hitCheckFilter(NULL, obj->position.x, 1.0f + obj->position.y, obj->position.z, 0.0f, (-1.0f), 0.0f, &hitX,
                           &hitY, &hitZ, &distance,
                           &hitNX, &hitNY, &hitNZ) != NULL) {
            obj->flags |= 2;
            obj->flags &= ~MOBJ_FLAG_GRAVITY;
        }
    }
    return 2;
}

s32 mobj_signboard(MapObjectEntry* obj) {
    switch (obj->state) {
        case 0:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                marioKeyOff();
                mobjRunEvent(obj, obj->eventCode);
                obj->state++;
            }
            break;
        case 1:
            if (evtCheckID(obj->eventId) != 1) {
                marioKeyOn();
                obj->eventId = 0;
                obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                obj->state = 0;
            }
            break;
    }
    return 0;
}

// The gp->inBattle read is kept in its own local ahead of the asyncName string address; under
// opt_propagation off the front end keeps that order, so the string's `lis` temp colours into r4 and
// the gp pointer into r5 as retail. Without the local/pragma the string address is created first and
// the two registers come out swapped.
#pragma opt_propagation off
s32 evt_mobj_signboard(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    f32 x;
    f32 y;
    f32 z;
    s32* eventCode;
    s32 flagVar;
    const char* asyncName;
    BOOL inBattle;

    name = (const char*)evtGetValue(event, args[0]);
    x = evtGetFloat(event, args[1]);
    y = evtGetFloat(event, args[2]);
    z = evtGetFloat(event, args[3]);
    eventCode = (s32*)evtGetValue(event, args[4]);
    inBattle = !!gp->inBattle;
    asyncName = "MOBJ_Signboard";
    flagVar = args[5];
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, x, y, z);
    obj->update = mobj_signboard;
    obj->eventCode = eventCode;
    obj->flags |= MOBJ_FLAG_RECALC_MTX | 0x20;
    obj->flags |= 0x40000;
    obj->flagVar = flagVar;
    obj->state = 0;
    return 2;
}
#pragma opt_propagation reset

/// @todo reconstructed: retail's pool has "MOBJ_Arrow" (unreferenced) just ahead of evt_mobj_signboard's
/// "MOBJ_Signboard", and party_kuribo/party_bomhei test animName against it. Invented: the name and the body
/// (evt_mobj_signboard's shape without its callback).
#ifdef MUST_MATCH
s32 evt_mobj_arrow(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    const char* asyncName;
    f32 x;
    f32 y;
    f32 z;
    s32* eventCode;
    s32 flagVar;
    MapObjectEntry* obj;

    name = (const char*)evtGetValue(event, args[0]);
    x = evtGetFloat(event, args[1]);
    y = evtGetFloat(event, args[2]);
    z = evtGetFloat(event, args[3]);
    eventCode = (s32*)evtGetValue(event, args[4]);
    asyncName = "MOBJ_Arrow";
    flagVar = args[5];
    if (!animGroupBaseAsync(asyncName, !!gp->inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, x, y, z);
    obj->eventCode = eventCode;
    obj->flags |= MOBJ_FLAG_RECALC_MTX | 0x20;
    obj->flagVar = flagVar;
    obj->state = 0;
    return 2;
}
#endif

// Recovery block (heart / coin-taken). The coins it takes are obj->coinCost; msgWindowId, timer and effect are
// named fields.
s32 mobj_recovery_blk(MapObjectEntry* obj) {
    s32 i;

    switch (obj->state) {
        case -1:
            obj->effect = effCoinFukidashiEntry(0, obj->position.x, obj->position.y + 25.0f, obj->position.z - 10.0f,
                                                obj->coinCost, 0);
            obj->state++;
            break;
        case 0:
            if (!(obj->flags & MOBJ_FLAG_TRIGGERED)) {
                break;
            }
            marioKeyOff();
            camGetPtr(8)->flags |= 0x200;
            psndSFXOn_3D((const char*)0x1D9, &obj->position);
            obj->state++;
            // fallthrough
        case 1:
            if (!(gp->flags & 0x1000) && swGet(0xEA) == 0) {
                swSet(0xEA);
                obj->state = 0x14;
                break;
            }
            obj->state++;
            // fallthrough
        case 2:
            if (pouchGetCoin() < obj->coinCost) {
                obj->state = 0xA;
                break;
            }
            if (obj->effect != NULL) {
                effSoftDelete(obj->effect);
                obj->effect = NULL;
            }
            psndSFXOn_3D((const char*)0x1DA, &obj->position);
            animPoseSetAnim(obj->poseId, "A_2", 1);
            if (obj->coinCost == 0) {
                obj->state = 3;
            } else {
                obj->state = 4;
                statusWinForceOpen();
            }
            obj->timer = 0x78;
            break;
        case 3:
            if (obj->timer == 100) {
                effStardustEntry(1, 0x1E, 0x3C, obj->position.x,
                                 45.0f + obj->position.y, obj->position.z,
                                 50.0f, 50.0f);
                psndSFXOn_3D((const char*)0x1DB, &obj->position);
            }
            if (obj->timer == 100) {
                MarioWork* mario = marioGetPtr();
                PouchData* pouch = pouchGetPtr();
                PouchPartyData* party = pouch->partyData;

                effStardustEntry(2, 0x32, 0x64, mario->position.x, mario->position.y, mario->position.z,
                                 50.0f, 50.0f);
                marioChgPose("M_I_1");
                pouch->currentHP = pouch->maximumHP;
                pouch->currentFP = pouch->maximumFP;
                for (i = 0; i < 8; i++) {
                    if ((party->flags & PARTY_FLAG_IS_ACTIVE) && !(party->flags & PARTY_FLAG_IS_GROUNDED)) {
                        party->currentHP = party->maximumHP;
                    }
                    party++;
                }
                pouchAddCoin(0);
            }
            obj->timer -= 1;
            if (obj->timer <= 0) {
                obj->state = 5;
            }
            break;
        case 4:
            if (obj->timer == 100) {
                effStardustEntry(1, 0x1E, 0x3C, obj->position.x,
                                 45.0f + obj->position.y, obj->position.z,
                                 50.0f, 50.0f);
                psndSFXOn_3D((const char*)0x1DB, &obj->position);
            }
            if (obj->timer == 100) {
                s32 coin = obj->coinCost;
                MarioWork* mario = marioGetPtr();
                PouchData* pouch = pouchGetPtr();
                PouchPartyData* party = pouch->partyData;

                effStardustEntry(2, 0x32, 0x64, mario->position.x, mario->position.y, mario->position.z,
                                 50.0f, 50.0f);
                marioChgPose("M_I_1");
                pouch->currentHP = pouch->maximumHP;
                pouch->currentFP = pouch->maximumFP;
                for (i = 0; i < 8; i++) {
                    if ((party->flags & PARTY_FLAG_IS_ACTIVE) && !(party->flags & PARTY_FLAG_IS_GROUNDED)) {
                        party->currentHP = party->maximumHP;
                    }
                    party++;
                }
                pouchAddCoin(-coin);
            }
            obj->timer -= 1;
            if (obj->timer <= 0) {
                obj->state = 5;
            }
            break;
        case 5:
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            animPoseSetAnim(obj->poseId, "A_1", 1);
            marioChgPose("M_S_1");
            marioKeyOn();
            statusWinForceOff();
            camGetPtr(8)->flags &= ~0x200;
            obj->state++;
            break;
        case 6:
            if (obj->effect == NULL) {
                obj->effect = effCoinFukidashiEntry(0, obj->position.x, obj->position.y + 25.0f, obj->position.z - 10.0f,
                                                    obj->coinCost, 0);
            }
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            animPoseSetAnim(obj->poseId, "S_1", 1);
            obj->flags &= ~MOBJ_FLAG_TRIGGERED;
            obj->state = 0;
            break;
        case 10:
            obj->msgWindowId = msgWindow_Entry(msgSearch("sys_no_coin"), 0, 0);
            obj->state++;
            break;
        case 11:
            if (windowCheckID(obj->msgWindowId) == 0) {
                marioKeyOn();
                camGetPtr(8)->flags &= ~0x200;
                obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                obj->state = 0;
            }
            break;
        case 20:
            obj->msgWindowId = msgWindow_Entry(msgSearch("kaifuku_first_hit"), 0, 0);
            obj->state++;
            break;
        case 21:
            if (windowCheckID(obj->msgWindowId) == 0) {
                marioKeyOn();
                camGetPtr(8)->flags &= ~0x200;
                obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                obj->state = 0;
            }
            break;
    }
    return 0;
}

s32 evt_mobj_recovery_blk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    s32 coinCost = evtGetValue(event, args[1]);
    f32 x = evtGetFloat(event, args[2]);
    f32 y = evtGetFloat(event, args[3]);
    f32 z = evtGetFloat(event, args[4]);
    s32 flagVar;
    BOOL inBattle;
    const char* asyncName;
    MapObjectEntry* obj;

    evtGetValue(event, args[5]);
    flagVar = args[6];
    inBattle = gp->inBattle != 0;
    asyncName = (coinCost == 0) ? "MOBJ_RecoveryBlock" : "MOBJ_CoinTakenRecoveryBlock";
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    obj->coinCost = coinCost;
    mobjSetPosition(name, x, y, z);
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    obj->update = mobj_recovery_blk;
    obj->eventCode = NULL;
    obj->eventCode2 = NULL;
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    obj->flags |= MOBJ_FLAG_TRIGGER_HEAD;
    obj->flagVar = flagVar;
    if (coinCost == 0) {
        obj->state = 0;
    } else {
        obj->state = -1;
    }
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    return 2;
}

s32 mobj_save_blk(MapObjectEntry* obj) {
    switch (obj->state) {
        case 0:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                marioKeyOff();
                marioStSystemLevel(1);
                psndSFXOn_3D((const char*)0x1AF, &obj->position);
                animPoseSetAnim(obj->poseId, "A_1", 1);
                camGetPtr(8)->flags |= 0x200;
                obj->state++;
            }
            break;
        case 1:
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            animPoseSetAnim(obj->poseId, "S_1", 1);
            if (obj->eventCode != NULL) {
                mobjRunEvent(obj, obj->eventCode);
                obj->state++;
            } else {
                obj->state = 3;
            }
            break;
        case 2:
            if (evtCheckID(obj->eventId) != 1) {
                obj->eventId = 0;
                obj->state++;
            }
            break;
        case 3:
            mobjRunEvent(obj, obj->eventCode2);
            obj->state++;
            break;
        case 4:
            if (evtCheckID(obj->eventId) != 1) {
                marioStSystemLevel(0);
                camGetPtr(8)->flags &= ~0x200;
                obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                marioKeyOn();
                obj->state = 0;
            }
            break;
    }
    return 0;
}

s32 evt_mobj_save_blk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    s32* eventCode = (s32*)evtGetValue(event, args[4]);
    s32 flagVar = args[5];
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
    mobjSetPosition(name, x, y, z);
    obj->update = mobj_save_blk;
    obj->eventCode = eventCode;
    obj->eventCode2 = mobj_save_blk_sysevt;
    obj->flags |= MOBJ_FLAG_RECALC_MTX | MOBJ_FLAG_NO_CULL;
    obj->flags |= MOBJ_FLAG_TRIGGER_HEAD;
    obj->flagVar = flagVar;
    obj->state = 0;
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    return 2;
}

s32 mobj_badgeblk(MapObjectEntry* obj) {


    switch (obj->state) {
        case 0:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
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
                if (obj->eventCode != NULL) {
                    mobjRunEvent(obj, obj->eventCode);
                } else if (obj->itemId != 0x79) {
                    ItemEntry* item;
                    f32 y = 40.0f + obj->position.y;
                    itemEntry(obj->name, (ItemType)obj->itemId, 4,
                        obj->position.x, y, obj->position.z, obj->flagVar, 0);
                    item = itemNameToPtr(obj->name);
                    itemFlagOn(item, 0x10);
                    item->boundRate = 0.7f;
                    item->gravity = 1.0f;
                    item->boundLimit = 0;
                    item->jumpPower = 360.0f;
                    psndSFXOn_3D((const char*)0x1BC, &obj->position);
                } else {
                    pouchAddCoin(1);
                    itemEntry(NULL, (ItemType)0x79, 0xf, obj->position.x, obj->position.y, obj->position.z, -1, 0);
                    evtSetValue(NULL, obj->flagVar, 1);
                    psndSFXOn_3D((const char*)0x5B, &obj->position);
                }
                obj->state++;
            }
            break;
        case 1:
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            animPoseSetAnim(obj->poseId, "S_2", 1);
            obj->flags &= ~MOBJ_FLAG_TRIGGERED;
            obj->state = 0x5a;
            if (strcmp(obj->animName, "MOBJ_HatenaBlock") == 0 ||
                strcmp(obj->animName, "MOBJ_BrickHatenaBlock") == 0 ||
                strcmp(obj->animName, "MOBJ_HiddenHatenaBlock") == 0) {
                strcpy(obj->animName, "MOBJ_Block");
            } else {
                strcpy(obj->animName, "MOBJ_PinkBlock");
            }
            break;
        case 0x5a:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                animPoseSetAnim(obj->poseId, "A_2", 1);
                psndSFXOn_3D((const char*)0x1B7, &obj->position);
                obj->state++;
            }
            break;
        case 0x5b:
            if (!(obj->flags & MOBJ_FLAG_TRIGGERED)) {
                obj->state = 0x5a;
                break;
            }
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            animPoseSetAnim(obj->poseId, "S_2", 1);
            obj->state = 0x5a;
            obj->flags &= ~MOBJ_FLAG_TRIGGERED;
            break;
    }
    return 0;
}

s32 evt_mobj_badgeblk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    s32 itemId = evtGetValue(event, args[4]);
    s32* eventCode = (s32*)evtGetValue(event, args[5]);
    s32 flagVar = args[6];
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
    mobjSetPosition(name, x, y, z);
    obj->update = mobj_badgeblk;
    obj->eventCode = eventCode;
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    obj->flags |= MOBJ_FLAG_TRIGGER_HEAD | MOBJ_FLAG_TRIGGER_PARTY | 0x10000;
    obj->flagVar = flagVar;
    obj->state = 0;
    obj->itemId = itemId;
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    if (evtGetValue(event, flagVar) != 0) {
        obj->state = 0x5a;
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

    switch (obj->state) {
        case 0:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                if (marioKeyOffChk() != 0 || marioCtrlOffChk() != 0) {
                    obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                } else {
                    marioKeyOff();
                    if (strcmp(gp->currentMapName, PB_DOU02) == 0) {
                        marioCtrlOff();
                    } else if (strcmp(gp->currentMapName, PB_DOU05) == 0 ||
                               strcmp(gp->currentMapName, PB_DOU10) == 0) {
                        if (obj->triggerFlag == MOBJ_FLAG_TRIGGER_PARTY) {
                            marioCtrlOff();
                        }
                    }
                    gp->systemLevelFlags |= 3;
                    psndSFXOn_3D((const char*)0x201C0, &obj->position);
                    animPoseSetAnim(obj->poseId, "A_1", 1);
                    itemEntry(PB_BLK1, (ItemType)0x57, 0xf, obj->position.x, obj->position.y, obj->position.z, -1, 0);
                    obj->state++;
                }
            }
            break;
        case 1:
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            animPoseSetAnim(obj->poseId, "A_2", 1);
            obj->flags &= ~MOBJ_FLAG_TRIGGERED;
            obj->delay = 1;
            obj->state++;
            break;
        case 2:
            if (obj->delay != 0) {
                obj->delay--;
            } else {
                itemForceGet(itemEntry(PB_BLK2, (ItemType)0x57, 0x11,
                    obj->position.x, 40.0f + obj->position.y, obj->position.z, -1, 0));
                evtSetValue(NULL, obj->flagVar, 1);
                marioKeyOn();
                if (strcmp(gp->currentMapName, PB_DOU02) == 0) {
                    marioCtrlOn();
                } else if (strcmp(gp->currentMapName, PB_DOU05) == 0 ||
                           strcmp(gp->currentMapName, PB_DOU10) == 0) {
                    if (obj->triggerFlag == MOBJ_FLAG_TRIGGER_PARTY) {
                        marioCtrlOn();
                    }
                }
                obj->state = 0x5a;
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
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    s32* eventCode = (s32*)evtGetValue(event, args[4]);
    s32 flagVar = args[5];
    f32 rotY = evtGetFloat(event, args[6]);
    const char* asyncName = "MOBJ_PowerUpBlock";
    MapObjectEntry* obj;

    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, x, y, z);
    obj->update = mobj_powerupblk;
    obj->eventCode = eventCode;
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    obj->flags |= MOBJ_FLAG_TRIGGER_HEAD | MOBJ_FLAG_TRIGGER_PARTY | 0x10000;
    obj->flagVar = flagVar;
    obj->state = 0;
    obj->rotation.y = rotY;
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    if (evtGetValue(event, flagVar) != 0) {
        obj->state = 0x5a;
        animPoseSetAnim(obj->poseId, "A_2", 1);
    }
    return 2;
}

// The name-buffer initializers emit their templates here, after save_first_hit.
// Prefix data precedes the following callback's compiler-generated name buffers.
#pragma force_active on
const char mobj_offscreen_prefix[0x10] = "o_";
const char mobj_image_prefix[0x10] = "i_";
__declspec(section ".rodata") Vec lv_block_position[1] = {{0.0f, -2000.0f, 0.0f}};
// vec3_802c3ef4: placeholder for the object the code copies (defining it hangs MWCC)
__declspec(section ".rodata") Vec mobj_vec3_802c3ef4_template[1] = {{0.0f, 0.0f, 0.0f}};
__declspec(section ".rodata") char sfx_mobj_lock_swing1[0x18] = "SFX_MOBJ_LOCK_SWING1";
__declspec(section ".rodata") char sfx_mobj_unlock1[0x14] = "SFX_MOBJ_UNLOCK1";
__declspec(section ".rodata") char sfx_mobj_treasure_box_swing1[0x20] = "SFX_MOBJ_TREASURE_BOX_SWING1";
__declspec(section ".rodata") char sfx_mobj_treasure_box_open1[0x1C] = "SFX_MOBJ_TREASURE_BOX_OPEN1";
__declspec(section ".rodata") char sfx_mobj_treasure_box_swing2[0x20] = "SFX_MOBJ_TREASURE_BOX_SWING2";
__declspec(section ".rodata") char sfx_mobj_treasure_box_open2[0x1C] = "SFX_MOBJ_TREASURE_BOX_OPEN2";
__declspec(section ".rodata") char sfx_mobj_blackbox_swing1[0x1C] = "SFX_MOBJ_BLACKBOX_SWING1";
__declspec(section ".rodata") char sfx_mobj_blackbox_open1[0x18] = "SFX_MOBJ_BLACKBOX_OPEN1";
__declspec(section ".rodata") static const Vec vec3_802c3fd8[1] = {{0.0f, 0.0f, 0.0f}};
__declspec(section ".rodata") static const Vec vec3_802c3fe4[1] = {{0.0f, 0.0f, 0.0f}};
__declspec(section ".rodata") char str_save_first_hit[0x10] = "save_first_hit";
#pragma force_active reset

s32 mobj_brickblk(MapObjectEntry* obj) {

    char offName[16] = "o_";
    char imgName[16] = "i_";
    BOOL inBattle;
    ImageEntry* img;

    inBattle = gp->inBattle != 0;

    strcat(offName, obj->name);
    strcat(imgName, obj->name);
    switch (obj->state) {
        case 0:
            animGroupBaseAsync("P_box_y", inBattle, NULL);
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                mobjRunEvent(obj, obj->eventCode);
                evtSetValue(NULL, obj->flagVar, 1);
                obj->state = 0xa;
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
            img->animName = "A_1";
            img->startTime = animTimeGetTime(inBattle);
            imgSetShadow(img, 2);
            psndSFXOn_3D((const char*)0x1B8, &obj->position);
            obj->state = 0xb;
            break;
        }
        case 0xb:
            img = imgNameToPtr(imgName, inBattle);
            if (img->loopTimes >= 1.0f) {
                obj->state = 0xc;
            }
            if (img->flags & 0x10000000) {
                obj->state = 0xc;
            }
            obj->position = vec3_802c4020[0];
            obj->flags |= MOBJ_FLAG_RECALC_MTX;
            obj->flags |= MOBJ_FLAG_TRIGGERED;
            break;
        case 0xc:
            img = imgNameToPtr(imgName, inBattle);
            imgFreeCapture(img, 0);
            imgFreeCapture(img, 1);
            imgSetShadow(img, 0);
            imgRelease(img, inBattle);
            offscreenDelete(offName);
            obj->offscreenId = -1;
            obj->state = 0x63;
            break;
        case 0x63:
            mobjDelete(obj->name);
            break;
    }
    return 0;
}

s32 mobj_10countblk(MapObjectEntry* obj) {
    switch (obj->state) {
        case 0:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                if (obj->flags & 0x800) {
                    obj->flags &= ~0x800;
                    strcpy(obj->animName, "MOBJ_Block");
                }
                animPoseSetAnim(obj->poseId, "A_1", 1);
                psndSFXOn_3D((const char*)0x1BE, &obj->position);
                evtSetValue(NULL, obj->flagVar, 1);
                if (obj->eventCode != NULL) {
                    mobjRunEvent(obj, obj->eventCode);
                } else {
                    pouchAddCoin(1);
                    itemEntry(NULL, (ItemType)0x79, 0xf, obj->position.x, obj->position.y, obj->position.z, -1, 0);
                    obj->timer = 0xC8;
                }
                obj->state++;
                obj->flags &= ~MOBJ_FLAG_TRIGGERED;
            }
            break;
        case 1:
            if (obj->timer > 0) {
                obj->timer--;
            }
            if (obj->timer <= 0) {
                obj->state = 0x5a;
                animPoseSetAnim(obj->poseId, "S_2", 1);
                strcpy(obj->animName, "MOBJ_Block");
            } else if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                psndSFXOn_3D((const char*)0x1BF, &obj->position);
                pouchAddCoin(1);
                itemEntry(NULL, (ItemType)0x79, 0xf, obj->position.x, obj->position.y, obj->position.z, -1, 0);
                obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                animPoseSetAnim(obj->poseId, "A_1", 1);
            }
            break;
        case 0x5a:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                psndSFXOn_3D((const char*)0x1B7, &obj->position);
                animPoseSetAnim(obj->poseId, "A_3", 1);
                obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                obj->state++;
            }
            break;
        case 0x5b:
            if (!(obj->flags & MOBJ_FLAG_TRIGGERED)) {
                obj->state = 0x5a;
            } else {
                if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                    break;
                }
                animPoseSetAnim(obj->poseId, "S_2", 1);
                obj->state = 0x5a;
                obj->flags &= ~MOBJ_FLAG_TRIGGERED;
            }
            break;
    }
    return 0;
}

s32 evt_mobj_brick(EventEntry* event, BOOL isFirstCall) {

    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    s32 itemId = evtGetValue(event, args[4]);
    s32 type = args[5];
    s32* eventCode = (s32*)evtGetValue(event, args[6]);
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
    mobjEntry(name, animName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, x, y, z);
    if (type >= 0xa) {
        obj->flags |= 0x800;
    }
    switch (type) {
        case 0:
        case 0xa:
            obj->update = mobj_brickblk;
            break;
        case 1:
        case 0xb:
            obj->update = mobj_badgeblk;
            break;
        case 2:
        case 0xc:
            obj->update = mobj_badgeblk;
            break;
        case 3:
        case 0xd:
            obj->update = mobj_10countblk;
            break;
    }
    obj->eventCode = eventCode;
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    obj->flags |= MOBJ_FLAG_TRIGGER_HEAD | MOBJ_FLAG_TRIGGER_PARTY | 0x10000;
    obj->flagVar = evtId;
    obj->state = 0;
    obj->itemId = itemId;
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
                obj->state = 0x63;
                break;
            case 1:
            case 0xb:
                animPoseSetAnim(obj->poseId, "S_2", 1);
                obj->state = 0x5a;
                newName = "MOBJ_Block";
                break;
            case 2:
            case 0xc:
                animPoseSetAnim(obj->poseId, "S_2", 1);
                obj->state = 0x5a;
                newName = "MOBJ_PinkBlock";
                break;
            case 3:
            case 0xd:
                animPoseSetAnim(obj->poseId, "S_2", 1);
                obj->state = 0x5a;
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
    switch (obj->state) {
        case 0:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                psndSFXOn_3D((const char*)0x401E8, &obj->position);
                animPoseSetAnim(obj->poseId, "A_1", 1);
                if (obj->eventCode != NULL) {
                    mobjRunEvent(obj, obj->eventCode);
                }
                if (obj->hitObj[0].hitObject != NULL) {
                    obj->hitObj[0].hitObject->flags |= 1;
                }
                obj->flags |= 2;
                evtSetValue(NULL, obj->flagVar, 1);
                obj->state = 1;
                animPoseSetMaterialFlagOn(obj->poseId, 0x40);
                animPoseSetMaterialEvtColor(obj->poseId, color);
                obj->timer = 0x40;
            }
            break;
        case 1: {

            *(u32*)&color2 = animPoseGetMaterialEvtColor(obj->poseId);
            if (color2.a >= 4) {
                color2.a -= 4;
            } else {
                color2.a = 0;
                obj->state = 0x63;
            }
            animPoseSetMaterialEvtColor(obj->poseId, color2);
            obj->timer -= 1;
            if (obj->timer <= 0) {
                obj->state = 0x63;
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
    s32* eventCode = (s32*)evtGetValue(event, args[5]);
    s32 flagVar = args[6];
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
    obj->update = mobj_breaking_floor;
    obj->eventCode = eventCode;
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    obj->flags |= MOBJ_FLAG_TRIGGER_HIP_DROP;
    obj->flagVar = flagVar;
    if (evtGetValue(event, flagVar) != 0) {
        obj->state = 0x63;
    } else {
        obj->state = 0;
    }
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    return 2;
}

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
    texWidth = (bboxMax.x - bboxMin.x) * 10.0f;
    texHeight = (bboxMax.z - bboxMin.z) * 10.0f;
    cam = camGetPtr(cameraId);
    texWidth = (texWidth + 1) & ~1;
    texHeight = (texHeight + 1) & ~1;
    cam->cameraUp = vec3_802c4044[0];
    cam->cameraAt = obj->position;
    cam->cameraPos = obj->position;
    t = (f32)tan(0.21816615760326385);
    cam->cameraPos.y += (f32)texHeight / 2.0f / t;
    C_MTXPerspective(cam->projection, 25.0f, (f32)(texWidth / texHeight),
                     1.0f, 32767.0f);
    cam->projType = GX_PERSPECTIVE;
    C_MTXLookAt(cam->view, &cam->cameraPos, &cam->cameraUp, &cam->cameraAt);
    GXSetProjection(cam->projection, cam->projType);
    GXSetViewport(0.0f, 0.0f, (f32)texWidth, (f32)texHeight, 0.0f,
                  1.0f);
    GXSetScissor(0, 0, texWidth, texHeight);
    test_kururing_mapdisp(cameraId);
    sysWaitDrawSync();
    obj->paperTex = smartAlloc(GXGetTexBufferSize(texWidth / 2, texHeight / 2, 6, 0, 0), 3);
    GXSetTexCopySrc(0, 0, texWidth, texHeight);
    GXSetTexCopyDst(texWidth / 2, texHeight / 2, 6, 1);
    GXSetZMode(1, 3, 1);
    GXCopyTex((obj->paperTex)->address, 1);
    GXInvalidateTexAll();
    GXPixModeSync();
    GXTexModeSync();
    GXInitTexObj(&obj->paperTexObj, (obj->paperTex)->address, texWidth / 2,
                 texHeight / 2, 6, 0, 0, 0);
    GXInitTexObjLOD(&obj->paperTexObj, 0, 0, 0.0f, 0.0f, 0.0f, 0,
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

s32 mobj_kururing_floor(MapObjectEntry* obj) {
    MarioWork* mario = marioGetPtr();
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    s32 flag;
    f32 dist;
    s32 i;
    ItemEntry* item;
    NpcWork* npcWork;
    NpcEntry* entry;
    Vec* vtx;
    PartyEntry* party2;

    switch (obj->state) {
        case 0:
            if (!(obj->flags & MOBJ_FLAG_TRIGGERED)) {
                break;
            }
            dist = distABf(mario->position.x, mario->position.z, obj->position.x, obj->position.z);
            if (__fabsf(mario->position.y - obj->position.y) > 10.0f) {
                dist = 100000.0f;
            }
            if (obj->triggerFlag == MOBJ_FLAG_TRIGGER_HIP_DROP) {
                if (pouchGetJumpLv() < 2) {
                    flag = 0;
                } else if (dist < 60.0f) {
                    flag = 2;
                } else if (dist < 100.0f) {
                    flag = 1;
                } else {
                    flag = 0;
                }
            }
            if (obj->triggerFlag == 0x00010000) {
                flag = 1;
            }
            if (flag == 0) {
                obj->flags &= ~MOBJ_FLAG_TRIGGERED;
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
            if (itemHitCheck(obj->position.x, obj->position.y, obj->position.z, 42.420002f) != NULL) {
                flag = 1;
            }
            if (npcNearDistCheck(obj->position.x, obj->position.y, obj->position.z, 42.420002f) != NULL) {
                flag = 1;
            }
            if (flag == 0) {
                obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                break;
            }
            if (flag == 1) {
                animPoseSetAnim(obj->poseId, "A_1", 1);
                psndSFXOnEx_3D((const char*)0x1ee, &obj->position, 0xff, 0xff, 0x64, 0x10);
                obj->state = 0xa;
            } else if (flag == 2) {
                animPoseSetAnim(obj->poseId, "A_2", 1);
                psndSFXOn_3D((const char*)0x1ef, &obj->position);
                obj->state = 1;
                item = itemNameToPtr(obj->name);
                if (item == NULL || !(item->flags & 1)) {
                    f32 y = 30.0f;

                    y += obj->position.y;
                    if (itemEntry(obj->name, 0x7d, 4, obj->position.x, y, obj->position.z, obj->flagVar, 0) != NULL) {
                        item = itemNameToPtr(obj->name);
                        itemFlagOn(item, 0x10);
                        item->boundRate = 0.6f;
                        item->gravity = 1.0f;
                        item->boundLimit = 5;
                        item->jumpPower = 300.0f;
                    }
                }
            }
            animGetPtr()->poses[obj->poseId].flags |= 0x10;
            obj->flags |= MOBJ_FLAG_PAPER_TEX;
            dispEntry(2, 0.0f, 0, kururing_capture, obj);
            break;

        case 1:
            animGetPtr()->poses[obj->poseId].flags |= 0x10;
            obj->flags |= MOBJ_FLAG_PAPER_TEX;
            dispEntry(2, 0.0f, 0, kururing_capture, obj);
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            animGetPtr()->poses[obj->poseId].flags &= ~0x10;
            obj->flags &= ~MOBJ_FLAG_PAPER_TEX;
            obj->flags &= ~MOBJ_FLAG_TRIGGERED;
            animPoseSetAnim(obj->poseId, "S_1", 1);
            psndSFXOn_3D((const char*)0x1f0, &obj->position);
            obj->state = 0;
            break;

        case 0xa:
            animGetPtr()->poses[obj->poseId].flags |= 0x10;
            obj->flags |= MOBJ_FLAG_PAPER_TEX;
            dispEntry(2, 0.0f, 0, kururing_capture, obj);
            vtx = &((Vec*)animPoseGetAnimPosePtr(obj->poseId)->groupWorldPos->address)[1];
            mario = marioGetPtr();
            if (mobjIsInRange(obj->position.x, obj->position.z, 30.0f, mario->position.x, mario->position.z)) {
                mario->position.y = vtx->y;
            }
            party2 = partyGetPtr(marioGetPartyId());
            if (party2 != NULL &&
                mobjIsInRange(obj->position.x, obj->position.z, 30.0f, party2->position.x, party2->position.z)) {
                party2->position.y = vtx->y;
            }
            party2 = partyGetPtr(marioGetExtraPartyId());
            if (party2 != NULL &&
                mobjIsInRange(obj->position.x, obj->position.z, 30.0f, party2->position.x, party2->position.z)) {
                party2->position.y = vtx->y;
            }
            npcWork = npcGetWorkPtr();
            entry = npcWork->entries;
            for (i = 0; i < npcWork->total; i++, entry++) {
                if (entry->flags & 1) {
                    if (mobjIsInRange(obj->position.x, obj->position.z, 42.420002f, entry->position.x, entry->position.z)) {
                        entry->position.y = vtx->y;
                    }
                }
            }
            item = itemNearDistCheck(obj->position.x, obj->position.y, obj->position.z, 42.420002f);
            if (item != NULL) {
                item->position.y = vtx->y;
            }
            if (animPoseGetLoopTimes(obj->poseId) < 2.0f) {
                break;
            }
            animGetPtr()->poses[obj->poseId].flags &= ~0x10;
            obj->flags &= ~MOBJ_FLAG_PAPER_TEX;
            animPoseSetAnim(obj->poseId, "S_1", 1);
            psndSFXOn_3D((const char*)0x1f0, &obj->position);
            obj->timer = 0x14;
            obj->state++;
            /* fallthrough */
        case 0xb:
            obj->timer--;
            if (obj->timer == 0) {
                obj->state++;
            }
            break;

        case 0xc:
            obj->flags &= ~MOBJ_FLAG_TRIGGERED;
            obj->state = 0;
            break;
    }
    return 0;
}

s32 evt_mobj_kururing_floor(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    const char* r30 = (const char*)evtGetValue(event, args[4]);
    s32 flagVar;
    MapObjectEntry* obj;
    Vec pos;
    Vec scale;
    const char* asyncName;

    evtGetValue(event, args[5]);
    asyncName = "MOBJ_KururinFloor";
    flagVar = args[6];
    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    pos = mobj_vec3_802c4050_template[0];
    pos.x = x;
    pos.y = y + 0.1f;
    pos.z = z;
    obj->position = pos;
    obj->update = mobj_kururing_floor;
    obj->flagVar = flagVar;
    obj->state = 0;
    obj->flags |= MOBJ_FLAG_TRIGGER_HIP_DROP | 0x10000;
    obj->flags |= 2;
    mobjCalcMtx(obj);
    scale = mobj_vec3_802c405c_template[0];
    scale.y = obj->scale2.y;
    obj->scale2 = scale;
    mobjCalcMtx2(obj);
    mapGrpFlagOn(r30, 0x400000);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    animPoseWorldPositionEvalOn(obj->poseId);
    return 2;
}

/// @todo reconstructed: retail's pool has "MOBJ_trap" (unreferenced) right after "MOBJ_BombRock" and "OFF_d_bom".
/// Invented: the name and the body (evt_mobj_signboard's shape).
#ifdef MUST_MATCH
s32 evt_mobj_trap(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    const char* asyncName;
    f32 x;
    f32 y;
    f32 z;
    s32* eventCode;
    s32 flagVar;
    MapObjectEntry* obj;

    name = (const char*)evtGetValue(event, args[0]);
    x = evtGetFloat(event, args[1]);
    y = evtGetFloat(event, args[2]);
    z = evtGetFloat(event, args[3]);
    eventCode = (s32*)evtGetValue(event, args[4]);
    asyncName = "MOBJ_trap";
    flagVar = args[5];
    if (!animGroupBaseAsync(asyncName, !!gp->inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, x, y, z);
    obj->eventCode = eventCode;
    obj->flags |= MOBJ_FLAG_RECALC_MTX | 0x20;
    obj->flagVar = flagVar;
    obj->state = 0;
    return 2;
}
#endif

/// @todo reconstructed: retail's pool has "MOBJ_BombRock" then "OFF_d_bom" (both unreferenced) at this slot, before
/// "MOBJ_trap", and party_kuribo/party_bomhei test animName against "MOBJ_BombRock". Invented: the name and the
/// body (evt_mobj_signboard's shape plus the hitObjFlagOn that uses "OFF_d_bom").
#ifdef MUST_MATCH
s32 evt_mobj_bombrock(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    const char* asyncName;
    f32 x;
    f32 y;
    f32 z;
    s32* eventCode;
    s32 flagVar;
    MapObjectEntry* obj;

    name = (const char*)evtGetValue(event, args[0]);
    x = evtGetFloat(event, args[1]);
    y = evtGetFloat(event, args[2]);
    z = evtGetFloat(event, args[3]);
    eventCode = (s32*)evtGetValue(event, args[4]);
    asyncName = "MOBJ_BombRock";
    flagVar = args[5];
    if (!animGroupBaseAsync(asyncName, !!gp->inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, x, y, z);
    obj->eventCode = eventCode;
    obj->flags |= MOBJ_FLAG_RECALC_MTX | 0x20;
    obj->flagVar = flagVar;
    obj->state = 0;
    hitObjFlagOn("OFF_d_bom", 1);
    return 2;
}
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

    switch (obj->state) {
        case 0:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                switch (kpaGetLevel()) {
                    case 0:
                    case 1:
                        if (obj->triggerFlag == MOBJ_FLAG_TRIGGER_KPA_FLAME || obj->triggerFlag == MOBJ_FLAG_TRIGGER_KPA_HEAD) {
                            flag = 1;
                        }
                        break;
                    case 2:
                        if (obj->triggerFlag == MOBJ_FLAG_TRIGGER_KPA_FLAME || obj->triggerFlag == MOBJ_FLAG_TRIGGER_KPA_HIP ||
                            obj->triggerFlag == MOBJ_FLAG_TRIGGER_KPA_HEAD) {
                            flag = 1;
                        }
                        break;
                    case 3:
                        flag = 1;
                        break;
                }
                if (flag == 0) {
                    obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                } else {
                    mobjRunEvent(obj, obj->eventCode);
                    evtSetValue(NULL, obj->flagVar, 1);
                    if (obj->triggerFlag == MOBJ_FLAG_TRIGGER_KPA_HEAD) {
                        npcKoopaModeMobjBoundDeadCheck(obj->hitObj[0].hitObject);
                    }
                    if (obj->hitObj[0].hitObject != NULL) {
                        obj->hitObj[0].hitObject->flags |= 1;
                    }
                    obj->flags |= 2;
                    kpaAddScore(kpaMutekiCheck() ? 500 : 400);
                    obj->state = 0xa;
                }
            }
            break;
        case 0xa: {
            ItemEntry* item;

            psndSFXOn_3D((const char*)0x1C4, &obj->position);
            effMObjBrokenEntry(5, obj->position.x, 25.0f + obj->position.y,
                               25.0f + obj->position.z);
            if ((item = itemEntry(obj->name, (ItemType)0x59, 4,
                obj->position.x, 25.0f + obj->position.y, obj->position.z, -1, (s32)kpa_powerup_evt)) != NULL) {
                itemStatusOn(item, 0x10);
                itemFlagOn(item, 0x14);
                item->lifeTime = 0x2710;
                item->boundRate = 0.45f;
                item->gravity = 0.7f;
                item->boundLimit = 0;
                item->jumpPower = 500.0f;
                item->moveSpeed = 120.0f;
                item->moveDir = 0x5a;
                psndSFXOn_3D((const char*)0x836, &obj->position);
            }
            obj->state = 0x63;
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
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    s32* eventCode = (s32*)evtGetValue(event, args[4]);
    s32 flagVar = args[5];
    const char* asyncName = (const char*)evtGetValue(event, args[6]);
    MapObjectEntry* obj;
    Vec pos;

    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    pos = mobj_vec3_802c40a0_template[0];
    pos.x = x;
    pos.y = y;
    pos.z = z;
    obj->position = pos;
    obj->update = mobj_koopa_ojama_blk_evt;
    obj->eventCode = eventCode;
    obj->eventCode2 = NULL;
    obj->flags |= MOBJ_FLAG_TRIGGER_KPA_HEAD | MOBJ_FLAG_TRIGGER_KPA_LV3 | MOBJ_FLAG_TRIGGER_KPA_HIP | MOBJ_FLAG_TRIGGER_KPA_FLAME;
    obj->flagVar = flagVar;
    obj->state = 0;
    if (evtGetValue(event, flagVar) != 0) {
        obj->state = 0x63;
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
    s32* eventCode = (s32*)evtGetValue(event, args[4]);
    s32 flagVar = args[5];
    const char* asyncName = (const char*)evtGetValue(event, args[6]);
    MapObjectEntry* obj;
    Vec pos;

    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    pos = mobj_vec3_802c40ac_template[0];
    pos.x = x;
    pos.y = y;
    pos.z = z;
    obj->position = pos;
    obj->update = mobj_koopa_stone_blk_evt;
    obj->eventCode = eventCode;
    obj->eventCode2 = NULL;
    obj->flagVar = flagVar;
    obj->state = 0x63;
    return 2;
}

s32 mobj_koopa_brickblk(MapObjectEntry* obj) {
    BOOL flag = FALSE;


    switch (obj->state) {
        case 0:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                switch (kpaGetLevel()) {
                    case 0:
                        if (obj->triggerFlag == MOBJ_FLAG_TRIGGER_KPA_FLAME) {
                            flag = TRUE;
                        }
                        break;
                    case 1:
                        if (obj->triggerFlag == MOBJ_FLAG_TRIGGER_KPA_FLAME || obj->triggerFlag == MOBJ_FLAG_TRIGGER_KPA_HEAD) {
                            flag = TRUE;
                        }
                        break;
                    case 2:
                        if (obj->triggerFlag == MOBJ_FLAG_TRIGGER_KPA_FLAME || obj->triggerFlag == MOBJ_FLAG_TRIGGER_KPA_HIP ||
                            obj->triggerFlag == MOBJ_FLAG_TRIGGER_KPA_HEAD) {
                            flag = TRUE;
                        }
                        break;
                    case 3:
                        flag = TRUE;
                        break;
                }
                if (!flag) {
                    if (obj->triggerFlag == MOBJ_FLAG_TRIGGER_KPA_HEAD) {
                        npcKoopaModeMobjBoundDeadCheck(obj->hitObj[0].hitObject);
                        obj->state = 0x14;
                    } else {
                        obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                    }
                } else {
                    mobjRunEvent(obj, obj->eventCode);
                    evtSetValue(NULL, obj->flagVar, 1);
                    if (obj->triggerFlag == MOBJ_FLAG_TRIGGER_KPA_HEAD) {
                        npcKoopaModeMobjBoundDeadCheck(obj->hitObj[0].hitObject);
                    }
                    if (obj->hitObj[0].hitObject != NULL) {
                        obj->hitObj[0].hitObject->flags |= 1;
                    }
                    obj->flags |= 2;
                    kpaAddScore(kpaMutekiCheck() ? 500 : 400);
                    obj->state = 0xa;
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
            obj->state = 0x63;
            break;
        case 0x14:
            psndSFXOn_3D((const char*)0x1B7, &obj->position);
            animPoseSetAnim(obj->poseId, "A_1", 1);
            obj->state = 0x15;
            break;
        case 0x15:
            if (!(obj->flags & MOBJ_FLAG_TRIGGERED)) {
                obj->state = 0;
                break;
            }
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            animPoseSetAnim(obj->poseId, "S_1", 1);
            obj->state = 0;
            obj->flags &= ~MOBJ_FLAG_TRIGGERED;
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
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    s32 itemId = evtGetValue(event, args[4]);
    s32 type = args[5];
    s32* eventCode = (s32*)evtGetValue(event, args[6]);
    s32 flagVar = args[7];
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
    mobjSetPosition(name, x, y, z);
    obj->update = mobj_koopa_brickblk;
    obj->eventCode = eventCode;
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    obj->flags |= MOBJ_FLAG_TRIGGER_KPA_HEAD | MOBJ_FLAG_TRIGGER_KPA_LV3 | MOBJ_FLAG_TRIGGER_KPA_HIP | MOBJ_FLAG_TRIGGER_KPA_FLAME;
    obj->flagVar = flagVar;
    obj->state = 0;
    obj->itemId = itemId;
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    if (evtGetValue(event, flagVar) != 0) {
        animPoseSetAnim(obj->poseId, "S_1", 1);
        obj->state = 0x63;
    }
    return 2;
}

s32 mobj_koopa_badgeblk(MapObjectEntry* obj) {
    BOOL flag;

    marioGetPtr();
    flag = FALSE;
    switch (obj->state) {
        case 0:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                if (obj->triggerFlag != MOBJ_FLAG_TRIGGER_KPA_HEAD) {
                    obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                } else {
                    if (obj->flags & 0x800) {
                        mobjHitEntry(obj, 0);
                    }
                    animPoseSetAnim(obj->poseId, "A_1", 1);
                    if (obj->eventCode != NULL) {
                        mobjRunEvent(obj, obj->eventCode);
                    } else {
                        if (obj->itemId == 0x59) {
                            ItemEntry* item;
                            itemEntry(obj->name, (ItemType)obj->itemId, 4,
                                obj->position.x, 12.5f + obj->position.y, obj->position.z, -1, (s32)kpa_powerup_evt);
                            item = itemNameToPtr(obj->name);
                            itemStatusOn(item, 0x10);
                            itemFlagOn(item, 0x14);
                            item->lifeTime = 0x2710;
                            item->boundRate = 0.45f;
                            item->gravity = 0.7f;
                            item->boundLimit = 0;
                            item->jumpPower = 500.0f;
                            item->moveSpeed = 120.0f;
                            item->moveDir = 0x5a;
                            psndSFXOn_3D((const char*)0x836, &obj->position);
                        } else {
                            kpaAddCoin(1);
                            itemEntry(NULL, (ItemType)0x79, 0xf,
                                obj->position.x, obj->position.y, obj->position.z, -1, 0);
                            evtSetValue(NULL, obj->flagVar, 1);
                            psndSFXOn_3D((const char*)0x5B, &obj->position);
                        }
                    }
                    if (obj->triggerFlag == MOBJ_FLAG_TRIGGER_KPA_HEAD) {
                        npcKoopaModeMobjBoundDeadCheck(obj->hitObj[0].hitObject);
                    }
                    obj->state++;
                }
            }
            break;
        case 1:
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            animPoseSetAnim(obj->poseId, "S_2", 1);
            obj->flags &= ~MOBJ_FLAG_TRIGGERED;
            obj->state = 0x5a;
            strcpy(obj->animName, "MOBJ_Block");
            break;
        case 0x5a:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                switch (kpaGetLevel()) {
                    case 2:
                        break;
                    case 3:
                        flag = TRUE;
                        break;
                }
                if (!flag) {
                    obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                } else {
                    if (obj->triggerFlag == MOBJ_FLAG_TRIGGER_KPA_HEAD) {
                        npcKoopaModeMobjBoundDeadCheck(obj->hitObj[0].hitObject);
                    }
                    if (obj->hitObj[0].hitObject != NULL) {
                        obj->hitObj[0].hitObject->flags |= 1;
                    }
                    obj->flags |= 2;
                    obj->state++;
                }
            }
            break;
        case 0x5b:
            psndSFXOn_3D((const char*)0x1B8, &obj->position);
            effMObjBrokenEntry(2, obj->position.x, 12.5f + obj->position.y,
                               12.5f + obj->position.z);
            obj->state = 0x63;
            break;
        case 0x63:
            mobjDelete(obj->name);
            break;
    }
    return 0;
}

// The gp->inBattle read is kept in its own local ahead of the asyncName string address, so the gp
// load temp colours r5 and the string-address temp r4 as in the original.
#pragma opt_propagation off
s32 evt_mobj_koopa_badgeblk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    s32 itemId = evtGetValue(event, args[4]);
    s32* eventCode = (s32*)evtGetValue(event, args[5]);
    s32 flagVar = args[6];
    BOOL inBattle = gp->inBattle != 0;
    const char* asyncName = "MOBJ_HatenaBlock";
    MapObjectEntry* obj;

    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, x, y, z);
    obj->update = mobj_koopa_badgeblk;
    obj->eventCode = eventCode;
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    obj->flags |= MOBJ_FLAG_TRIGGER_KPA_HEAD | MOBJ_FLAG_TRIGGER_KPA_LV3 | MOBJ_FLAG_TRIGGER_KPA_HIP | MOBJ_FLAG_TRIGGER_KPA_FLAME;
    obj->flagVar = flagVar;
    obj->state = 0;
    obj->itemId = itemId;
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    if (evtGetValue(event, flagVar) != 0) {
        obj->state = 0x5a;
        animPoseSetAnim(obj->poseId, "S_2", 1);
    }
    return 2;
}

// The original loads the gp pointer before the asyncName string address.  With the named
// inBattle web kept (opt_propagation off) the assignments happen in declaration order, giving
// the pointer register the original uses for each temp.
s32 evt_mobj_koopa_hidden_blk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    s32 itemId = evtGetValue(event, args[4]);
    s32* eventCode = (s32*)evtGetValue(event, args[6]);
    s32 flagVar = args[7];
    BOOL inBattle = gp->inBattle != 0;
    const char* asyncName = "MOBJ_HiddenHatenaBlock";
    MapObjectEntry* obj;

    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, x, y, z);
    obj->flags |= 0x800;
    mobjHitEntry(obj, 1);
    obj->update = mobj_koopa_badgeblk;
    obj->eventCode = eventCode;
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    obj->flags |= MOBJ_FLAG_TRIGGER_KPA_HEAD | MOBJ_FLAG_TRIGGER_KPA_LV3 | MOBJ_FLAG_TRIGGER_KPA_HIP | MOBJ_FLAG_TRIGGER_KPA_FLAME;
    obj->flagVar = flagVar;
    obj->state = 0;
    obj->itemId = itemId;
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    if (evtGetValue(event, flagVar) != 0) {
        obj->state = 0x5a;
        animPoseSetAnim(obj->poseId, "S_2", 1);
    }
    return 2;
}
#pragma opt_propagation reset

s32 mobj_koopa_blk(MapObjectEntry* obj) {
    s32 flag = 0;

    switch (obj->state) {
        case 0:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                switch (kpaGetLevel()) {
                    case 2:
                        break;
                    case 3:
                        flag = 1;
                        break;
                }
                if (flag == 0) {
                    obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                } else {
                    mobjRunEvent(obj, obj->eventCode);
                    evtSetValue(NULL, obj->flagVar, 1);
                    if (obj->triggerFlag == MOBJ_FLAG_TRIGGER_KPA_HEAD) {
                        npcKoopaModeMobjBoundDeadCheck(obj->hitObj[0].hitObject);
                    }
                    if (obj->hitObj[0].hitObject != NULL) {
                        obj->hitObj[0].hitObject->flags |= 1;
                    }
                    obj->flags |= 2;
                    obj->state = 0xa;
                }
            }
            break;
        case 0xa:
            psndSFXOn_3D((const char*)0x1b8, &obj->position);
            effMObjBrokenEntry(2, obj->position.x, 12.5f + obj->position.y,
                                 12.5f + obj->position.z);
            obj->state = 0x63;
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
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    s32* eventCode = (s32*)evtGetValue(event, args[5]);
    s32 flagVar = args[6];
    MapObjectEntry* obj;

    if (!animGroupBaseAsync("MOBJ_Block", gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, "MOBJ_Block");
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, x, y, z);
    obj->update = mobj_koopa_blk;
    obj->eventCode = eventCode;
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    obj->flags |= MOBJ_FLAG_TRIGGER_KPA_HEAD | MOBJ_FLAG_TRIGGER_KPA_LV3 | MOBJ_FLAG_TRIGGER_KPA_HIP | MOBJ_FLAG_TRIGGER_KPA_FLAME;
    obj->flagVar = flagVar;
    obj->state = 0;
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    return 2;
}

s32 mobj_koopa_dokan_evt(MapObjectEntry* obj) {
    BOOL flag = FALSE;

    switch (obj->state) {
        case 0:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                switch (kpaGetLevel()) {
                    case 0:
                        break;
                    case 1:
                    case 2:
                        if (obj->triggerFlag == MOBJ_FLAG_TRIGGER_KPA_FLAME) {
                            flag = TRUE;
                        }
                        break;
                    case 3:
                        flag = TRUE;
                        break;
                }
                if (!flag) {
                    obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                } else {
                    mobjRunEvent(obj, obj->eventCode);
                    evtSetValue(NULL, obj->flagVar, 1);
                    if (obj->triggerFlag == MOBJ_FLAG_TRIGGER_KPA_HEAD) {
                        npcKoopaModeMobjBoundDeadCheck(obj->hitObj[0].hitObject);
                    }
                    if (obj->hitObj[0].hitObject != NULL) {
                        obj->hitObj[0].hitObject->flags |= 1;
                    }
                    obj->flags |= 2;
                    kpaAddScore(kpaMutekiCheck() ? 500 : 400);
                    obj->state = 0xa;
                }
            }
            break;
        case 0xa:
            if (strcmp(obj->animName, "MOBJ_GreenPipe_ks") == 0) {
                effMObjBrokenEntry(6, obj->position.x, 25.0f + obj->position.y,
                                   25.0f + obj->position.z);
            } else if (strcmp(obj->animName, "MOBJ_GreenPipe_kl") == 0) {
                effMObjBrokenEntry(6, obj->position.x,
                                   7.5f + (25.0f + obj->position.y),
                                   25.0f + obj->position.z);
            } else {
                effMObjBrokenEntry(6, obj->position.x - 25.0f,
                                   25.0f + obj->position.y,
                                   25.0f + obj->position.z);
            }
            psndSFXOn_3D((const char*)0x834, &obj->position);
            obj->state = 0x63;
            break;
        case 0x63:
            mobjDelete(obj->name);
            break;
    }
    return 0;
}

// Pipe model names are literals; the position retains its prefix Vec template.
s32 evt_mobj_koopa_dokan(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    const char* name = (const char*)evtGetValue(event, args[1]);
    f32 x = evtGetFloat(event, args[2]);
    f32 y = evtGetFloat(event, args[3]);
    f32 z = evtGetFloat(event, args[4]);
    s32* eventCode = (s32*)evtGetValue(event, args[5]);
    s32 flagVar = args[6];
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
    obj->flags |= MOBJ_FLAG_RECALC_MTX | 0x2;
    pos = mobj_vec3_802c40b8_template[0];
    pos.x = x;
    pos.y = y;
    pos.z = z;
    obj->position = pos;
    obj->update = mobj_koopa_dokan_evt;
    obj->eventCode = eventCode;
    obj->eventCode2 = NULL;
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    obj->flags |= MOBJ_FLAG_TRIGGER_KPA_HEAD | MOBJ_FLAG_TRIGGER_KPA_LV3 | MOBJ_FLAG_TRIGGER_KPA_HIP | MOBJ_FLAG_TRIGGER_KPA_FLAME;
    obj->flagVar = flagVar;
    obj->state = 0;
    if (evtGetValue(event, flagVar) != 0) {
        obj->state = 0x63;
    }
    return 2;
}

s32 fire_func(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);
    s32 v = evtGetValue(event, args[3]);
    EffectEntry* eff;
    f32 scaleX;
    f32 scaleY;

    if (isFirstCall) {
        event->userdata[0] =
            (s32)effFireEntry(5, v, x, y, z, 0.8f + (f32)irand(10) / 10.0f);
    }
    scaleX = event->userdataf[1];
    scaleY = event->userdataf[2];
    eff = (EffectEntry*)event->userdata[0];
    {
        f32 k = 0.125f;
        scaleX = (1.0f - scaleX) * k + scaleX;
        scaleY = (1.0f - scaleY) * k + scaleY;
    }
    ((effFireData*)eff->userdata)->scaleX = scaleX;
    ((effFireData*)eff->userdata)->scaleY = scaleY;
    if (scaleX >= 0.99f) {
        ((effFireData*)eff->userdata)->scaleX = 1.0f;
        ((effFireData*)eff->userdata)->scaleY = 1.0f;
        return 2;
    }
    event->userdataf[1] = scaleX;
    event->userdataf[2] = scaleY;
    return 0;
}

s32 mobj_koopa_sango_evt(MapObjectEntry* obj) {
    s32 sp10 = 0;
    EventEntry* event;

    switch (obj->state) {
        case 0:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                switch (kpaGetLevel()) {
                    case 0:
                    case 1:
                    case 2:
                        if (obj->triggerFlag == MOBJ_FLAG_TRIGGER_KPA_FLAME) {
                            sp10 = 1;
                        }
                        break;
                    case 3:
                        sp10 = 1;
                        break;
                }
                if (sp10 == 0) {
                    obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                    break;
                }
                mobjRunEvent(obj, obj->eventCode);
                evtSetValue(NULL, obj->flagVar, 1);
                if (obj->triggerFlag == MOBJ_FLAG_TRIGGER_KPA_HEAD) {
                    npcKoopaModeMobjBoundDeadCheck(obj->hitObj[0].hitObject);
                }
                if (obj->hitObj[0].hitObject != NULL) {
                    obj->hitObj[0].hitObject->flags |= 1;
                }
                obj->flags |= 2;
                kpaAddScore(kpaMutekiCheck() ? 0x1f4 : 0x190);
                obj->state = 0xa;
            }
            break;
        case 0xa:
            if (obj->triggerFlag == MOBJ_FLAG_TRIGGER_KPA_FLAME) {
                psndSFXOn_3D((const char*)0xb67, &obj->position);
            }
            event = evtEntry(evt_fire, 0, 0);
            event->lwData[0] = (s32)(1024.0f * obj->position.x) + 0xF24A7A80;
            event->lwData[1] = (s32)(1024.0f * obj->position.y) + 0xF24A7A80;
            event->lwData[2] = (s32)(1024.0f * (obj->position.z + 10.0f)) + 0xF24A7A80;
            event->lwData[3] = 0x5a;
            obj->state++;
            break;
        case 0x14:
            event = evtEntry(evt_fire, 0, 0);
            event->lwData[0] = (s32)(1024.0f * (obj->position.x + 10.0f)) + 0xF24A7A80;
            event->lwData[1] = (s32)(1024.0f * (obj->position.y + 30.0f)) + 0xF24A7A80;
            event->lwData[2] = (s32)(1024.0f * (obj->position.z + 10.0f)) + 0xF24A7A80;
            event->lwData[3] = 0x50;
            obj->state++;
            break;
        case 0x1e:
            event = evtEntry(evt_fire, 0, 0);
            event->lwData[0] = (s32)(1024.0f * (obj->position.x - 10.0f)) + 0xF24A7A80;
            event->lwData[1] = (s32)(1024.0f * (obj->position.y + 60.0f)) + 0xF24A7A80;
            event->lwData[2] = (s32)(1024.0f * (obj->position.z + 10.0f)) + 0xF24A7A80;
            event->lwData[3] = 0x46;
            obj->state++;
            break;
        case 0x62:
            effMObjBrokenEntry(7, obj->position.x, obj->position.y, obj->position.z + 10.0f);
            obj->state++;
            break;
        case 0x63:
            mobjDelete(obj->name);
            break;
        default:
            obj->state++;
            break;
    }
    return 0;
}

s32 evt_mobj_koopa_sango(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    s32* eventCode = (s32*)evtGetValue(event, args[4]);
    s32 flagVar = args[5];
    MapObjectEntry* obj;
    Vec pos;

    if (!animGroupBaseAsync("MOBJ_sango", gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, "MOBJ_sango");
    obj = mobjNameToPtr(name);
    obj->flags |= MOBJ_FLAG_RECALC_MTX | MOBJ_FLAG_NO_CULL;
    pos = mobj_vec3_802c40c4_template[0];
    pos.x = x;
    pos.y = y;
    pos.z = z;
    obj->position = pos;
    obj->update = mobj_koopa_sango_evt;
    obj->eventCode = eventCode;
    obj->eventCode2 = NULL;
    obj->flags |= MOBJ_FLAG_TRIGGER_KPA_HEAD | MOBJ_FLAG_TRIGGER_KPA_LV3 | MOBJ_FLAG_TRIGGER_KPA_HIP | MOBJ_FLAG_TRIGGER_KPA_FLAME;
    obj->flagVar = flagVar;
    obj->state = 0;
    if (evtGetValue(event, flagVar) != 0) {
        obj->state = 0x63;
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

    if (obj->state < 0xa) {
        if (distABf(mario->position.x, mario->position.y, obj->position.x, obj->position.y) < 150.0f) {
            if (psndSFXChk(obj->sfxId) != 0) {
                obj->sfxId = psndSFXOn_3D((const char*)0x1cd, &obj->position);
            }
        } else if (psndSFXChk(obj->sfxId) == 0) {
            psndSFXOff(obj->sfxId);
            obj->sfxId = -1;
        }
        PSMTXIdentity(mtx);
        PSMTXTransApply(mtx, mtx, -obj->position.x, -obj->position.y, -obj->position.z);
        PSMTXRotRad(rotMtx, 'Z', 0.017453292f * obj->rotSpeed);
        PSMTXConcat(rotMtx, mtx, mtx);
        PSMTXTransApply(mtx, mtx, obj->position.x, obj->position.y, obj->position.z);
        for (i = 0; i < obj->npcCount; i++) {
            sprintf(buf, "%s%2x", obj->name, i);
            npc = npcNameToPtr(buf);
            PSMTXMultVec(mtx, &npc->position, &npc->position);
        }
    }
    switch (obj->state) {
        case 0:
            if (obj->flags & MOBJ_FLAG_TRIGGERED) {
                switch (kpaGetLevel()) {
                    case 0:
                    case 1:
                    case 2:
                        if (obj->triggerFlag == MOBJ_FLAG_TRIGGER_KPA_FLAME) {
                            pass = 1;
                        }
                        break;
                    case 3:
                        pass = 1;
                        break;
                }
                if (pass == 0) {
                    obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                    break;
                }
                mobjRunEvent(obj, obj->eventCode);
                evtSetValue(NULL, obj->flagVar, 1);
                if (obj->hitObj[0].hitObject != NULL) {
                    obj->hitObj[0].hitObject->flags |= 1;
                }
                obj->flags |= 2;
                kpaAddScore(kpaMutekiCheck() ? 0x1f4 : 0x190);
                obj->state = 0xa;
            }
            break;
        case 0xa:
            psndSFXOn_3D((const char*)0x1b8, &obj->position);
            effMObjBrokenEntry(0, obj->position.x, obj->position.y + 12.5f, obj->position.z + 12.5f);
            obj->state++;
            break;
        case 0xb:
            obj->position.y = -1000.0f;
            for (i = 0; i < obj->npcCount; i++) {
                sprintf(buf, "%s%2x", obj->name, i);
                npc = npcNameToPtr(buf);
                npc->moveElapsed = 0;
                npc->jumpStartPos = npc->position;
                npc->jumpTargetPos.x = (f32)irand(0x65) / 200.0f;
                npc->jumpTargetPos.y = 6.0f - (f32)irand(0x65) / 75.0f;
                npc->jumpTargetPos.z = 4.8f;
                npc->moveTime = 2.0f;
                npc->jumpFlags |= 4;
            }
            if (psndSFXChk(obj->sfxId) == 0) {
                psndSFXOff(obj->sfxId);
                obj->sfxId = -1;
            }
            obj->state++;
            break;
        case 0xc:
            for (i = 0; i < obj->npcCount; i++) {
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
            if (npc->moveTime <= npc->moveElapsedSec) {
                obj->state = 0x63;
            }
            break;
        case 0x63:
            for (j = 0; j < obj->npcCount; j++) {
                sprintf(buf2, "%s%2x", obj->name, j);
                npcDelete(npcNameToPtr(buf2));
            }
            mobjDelete(obj->name);
            break;
    }
    return 0;
}

// The read-only strings and Vec template are reached by name so that the unit's
// pool_data base covers the compiler's int->float conversion constant too.
s32 evt_mobj_koopa_fireber_dodai(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 sign = evtGetValue(event, args[0]);
    const char* name = (const char*)evtGetValue(event, args[1]);
    f32 x = evtGetFloat(event, args[2]);
    f32 y = evtGetFloat(event, args[3]);
    f32 z = evtGetFloat(event, args[4]);
    s32 count = evtGetValue(event, args[5]);
    f32 rotSpeed = evtGetFloat(event, args[6]);
    s32* eventCode = (s32*)evtGetValue(event, args[7]);
    s32 flagVar = args[8];
    BOOL inBattle = gp->inBattle != 0;
    MapObjectEntry* obj;
    Vec pos;
    s32 i;
    char buf[32];

    if (!animGroupBaseAsync("MOBJ_fire_dodai", inBattle, NULL)) {
        return 0;
    }
    if (!animGroupBaseAsync("c_fireber", inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, "MOBJ_fire_dodai");
    obj = mobjNameToPtr(name);
    obj->flags |= MOBJ_FLAG_RECALC_MTX;
    pos = vec3_802c40d0[0];
    pos.x = x;
    pos.y = y;
    pos.z = z;
    obj->position = pos;
    obj->update = mobj_koopa_fireber_dodai_evt;
    obj->eventCode = eventCode;
    obj->eventCode2 = NULL;
    obj->flags |= MOBJ_FLAG_TRIGGER_KPA_HEAD | MOBJ_FLAG_TRIGGER_KPA_LV3 | MOBJ_FLAG_TRIGGER_KPA_HIP | MOBJ_FLAG_TRIGGER_KPA_FLAME;
    obj->flagVar = flagVar;
    obj->state = 0;
    if (evtGetValue(event, flagVar) != 0) {
        obj->state = 0x63;
    }
    if (rotSpeed == 0.0f) {
        rotSpeed = 1.0f;
    }
    obj->npcCount = count;
    obj->rotSpeed = (sign != 0) ? __fabsf(rotSpeed) : -__fabsf(rotSpeed);
    obj->sfxId = 0;
    y = 25.0f + y;
    z = 175.0f + z;
    for (i = 0; i < count; i++) {
        NpcEntry* npc;

        sprintf(buf, "%s%2x", name, i);
        npcEntry(buf, "c_fireber");
        npc = npcNameToPtr(buf);
        npc->flags |= 0x08000000;
        npc->hazardImmuneFlags |= 0xF;
        npc->position.x = x;
        npc->position.y = 25.0f * (f32)i + y;
        npc->position.z = z;
        animPoseSetAnim(npc->poseId, "FRB_S_1", 1);
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
        evtSetValue(event, args[0], (s32)event->thisMapObj->childName);
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

s32 kpa_score_to_str(EventEntry* event, BOOL isFirstCall) {
    s32 n = evtGetValue(event, event->args[0]);
    s32 digits[4];
    s32 len;
    s32 j;

    digits[0] = n / 1000;
    n %= 1000;
    digits[1] = n / 100;
    n %= 100;
    digits[2] = n / 10;
    digits[3] = n % 10;
    len = 0;
    for (j = 0; j != 4; j++) {
        if (len != 0 || digits[j] != 0) {
            psw->str[len] = digits[j] + '0';
            len++;
        }
    }
    psw->terminator = 0;
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
    *(u32*)&color = kpa_pole_score_color;
    FontDrawColor(&color);
    FontDrawStringMtx(trans, psw->str);
}

s32 kpa_disp_pole_score_main(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    f32 x;
    f32 y;
    f32 z;
    f32 f1;

    x = evtGetFloat(event, args[0]);
    y = evtGetFloat(event, args[1]);
    z = evtGetFloat(event, args[2]);
    f1 = evtGetFloat(event, args[3]);
    if (isFirstCall) {
        psw->x = x;
        psw->y = y;
        psw->z = z;
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
    zero = 0.0f;
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
    MarioWork* mario = marioGetPtr();
    NpcEntry* entry;
    s32 next = 0;
    s32 score;
    s32 i;

    switch (obj->state) {
        case 0:
            if (!(obj->flags & MOBJ_FLAG_TRIGGERED)) {
                break;
            }
            if (mario->characterId != 2) {
                obj->flags &= ~MOBJ_FLAG_TRIGGERED;
                break;
            }
            switch (kpaGetLevel()) {
                case 0:
                case 1:
                    if (mario->position.y < 25.0f) {
                        obj->flags &= ~MOBJ_FLAG_TRIGGERED;
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
            evtSetValue(NULL, obj->flagVar, 1);
            mario->noHitEnemyCt = 0;
            obj->state = next;
            break;
        case 0xa:
            mario = marioGetPtr();
            score = 0;
            if (kpaGetLevel() == 3) {
                kpaAddScore(0x7d0);
            } else if (kpaGetLevel() >= 2) {
                kpaAddScore(0x3e8);
            } else {
                if (mario->position.y >= 170.0f) {
                    score = 1000;
                } else if (mario->position.y >= 100.0f) {
                    score = 500;
                } else if (mario->position.y >= 50.0f) {
                    score = 200;
                } else {
                    score = 100;
                }
                kpaAddScore(score);
            }
            mobjRunEvent(obj, kpa_pole)->lwData[0] = score;
            obj->state++;
            break;
        case 0xb:
            if (evtCheckID(obj->eventId) == 1) {
                break;
            }
            obj->state = 0x5a;
            break;
        case 0x32:
            mario = marioGetPtr();
            if (kpaGetLevel() == 3) {
                kpaAddScore(0x7d0);
            } else if (kpaGetLevel() >= 2) {
                kpaAddScore(0x3e8);
            } else {
                if (mario->position.y >= 170.0f) {
                    score = 1000;
                } else if (mario->position.y >= 100.0f) {
                    score = 500;
                } else if (mario->position.y >= 50.0f) {
                    score = 200;
                } else {
                    score = 100;
                }
                kpaAddScore(score);
            }
            mobjRunEvent(obj, kpa_pole_move);
            obj->state++;
            break;
        case 0x33:
            if (evtCheckID(obj->eventId) == 1) {
                break;
            }
            obj->state = 0x5a;
            break;
        case 0x5a:
            mobjRunEvent(obj, kpa_move_to_toride);
            obj->state++;
            break;
        case 0x5b:
            if (evtCheckID(obj->eventId) == 1) {
                break;
            }
            obj->eventId = 0;
            obj->state++;
            break;
        case 0x5c:
            mobjRunEvent(obj, obj->eventCode2);
            obj->state++;
            break;
        case 0x5d:
            if (evtCheckID(obj->eventId) == 1) {
                break;
            }
            obj->eventId = 0;
            obj->state++;
            // fallthrough
        case 0x5e:
            mobjRunEvent(obj, obj->eventCode);
            obj->state++;
            break;
        case 0x5f:
        default:
            break;
    }
    return 0;
}

s32 evt_mobj_koopa_pole(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    s32* eventCode = (s32*)evtGetValue(event, args[4]);
    s32* eventCode2 = (s32*)evtGetValue(event, args[5]);
    s32 flagVar = args[6];
    BOOL inBattle = gp->inBattle != 0;
    MapObjectEntry* obj;
    MapObjectEntry* child;
    Vec pos;
    Vec pos2;

    if (!animGroupBaseAsync(str_MOBJ_goal_pole, inBattle, NULL)) {
        return 0;
    }
    if (!animGroupBaseAsync(str_MOBJ_goal_hata, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, str_MOBJ_goal_pole);
    obj = mobjNameToPtr(name);
    obj->flags |= MOBJ_FLAG_RECALC_MTX | MOBJ_FLAG_NO_CULL;
    pos = vec3_802c4168[0];
    pos.x = x;
    pos.y = y;
    pos.z = z;
    obj->position = pos;
    obj->update = mobj_koopa_pole_evt;
    obj->eventCode = eventCode;
    obj->eventCode2 = eventCode2;
    obj->flags |= MOBJ_FLAG_TRIGGER_STAND_ON | MOBJ_FLAG_TRIGGER_KPA_HEAD | MOBJ_FLAG_TRIGGER_KPA_LV3 | MOBJ_FLAG_TRIGGER_KPA_HIP | 0x80000;
    obj->flagVar = flagVar;
    obj->state = 0;
    if (evtGetValue(event, flagVar) != 0) {
        obj->state = 0x63;
    }
    strcpy(obj->childName, name);
    strcat(obj->childName, "_h");
    mobjEntry(obj->childName, str_MOBJ_goal_hata);
    child = mobjNameToPtr(obj->childName);
    pos2 = vec3_802c4174[0];
    pos2.x = x;
    pos2.y = 180.0f + y;
    pos2.z = z;
    child->position = pos2;
    child->update = NULL;
    child->eventCode = NULL;
    child->eventCode2 = NULL;
    child->flagVar = 0;
    child->state = 0;
    strcpy(child->childName, name);
    if (child->hitObj[0].hitObject != NULL) {
        child->hitObj[0].hitObject->flags |= 1;
    }
    return 2;
}

// ROData pool objects
__declspec(section ".sdata2") u32 dat_8041a130 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 kpa_pole_score_color = 0xFFFFFFFF;

// .sdata
EVT_BEGIN(kpa_powerup_evt)
    CALL(kpa_powerup, LW(0), LW(1), LW(2))
    RETURN
EVT_END()

EVT_BEGIN(evt_fire)
    CALL(fire_func, LW(0), LW(1), LW(2), LW(3))
    RETURN
EVT_END()

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
const char str_o[0x10] = "o_";
const char str_i[0x10] = "i_";
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
__declspec(section ".rodata") char str_KPA2_G_1[0xC] = "KPA2_G_1";
__declspec(section ".rodata") char sfx_kuppa_post_jump1[0x18] = "SFX_KUPPA_POST_JUMP1";
__declspec(section ".rodata") char sfx_kuppa_post_slide1[0x18] = "SFX_KUPPA_POST_SLIDE1";
__declspec(section ".rodata") char sfx_kuppa_post_throw1[0x18] = "SFX_KUPPA_POST_THROW1";
__declspec(section ".rodata") char str_kpa_06[0x8] = "kpa_06";
__declspec(section ".rodata") char str_toride[0x8] = "toride";
__declspec(section ".rodata") char str_toride_[0x8] = "toride_";
__declspec(section ".rodata") char sfx_kuppa_struggle1[0x14] = "SFX_KUPPA_STRUGGLE1";
__declspec(section ".rodata") char str_KPA2_S_1[0xC] = "KPA2_S_1";
__declspec(section ".rodata") Vec vec3_802c4168[1] = {{0.0f, 0.0f, 0.0f}};
__declspec(section ".rodata") Vec vec3_802c4174[1] = {{0.0f, 0.0f, 0.0f}};
const char str_MOBJ_goal_pole[0x10] = "MOBJ_goal_pole";
const char str_MOBJ_goal_hata[0x10] = "MOBJ_goal_hata";
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
EVT_BEGIN(evt_lock_sound)
    WAIT_MS(100)
    CALL(evt_snd_sfxon_3d, PTR(sfx_mobj_lock_swing1), LW(0), LW(1), LW(2), 0)
    WAIT_MS(200)
    CALL(evt_snd_sfxon_3d, PTR(sfx_mobj_unlock1), LW(0), LW(1), LW(2), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_box_sound1)
    WAIT_MS(100)
    CALL(evt_snd_sfxon_3d, PTR(sfx_mobj_treasure_box_swing1), LW(0), LW(1), LW(2), 0)
    WAIT_MS(200)
    CALL(evt_snd_sfxon_3d, PTR(sfx_mobj_treasure_box_open1), LW(0), LW(1), LW(2), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_box_sound2)
    WAIT_MS(100)
    CALL(evt_snd_sfxon_3d, PTR(sfx_mobj_treasure_box_swing2), LW(0), LW(1), LW(2), 0)
    WAIT_MS(200)
    CALL(evt_snd_sfxon_3d, PTR(sfx_mobj_treasure_box_open2), LW(0), LW(1), LW(2), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_box_sound3)
    WAIT_MS(100)
    CALL(evt_snd_sfxon_3d, PTR(sfx_mobj_blackbox_swing1), LW(0), LW(1), LW(2), LW(15))
    WAIT_MS(200)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(evt_snd_sfxon_3d, PTR(sfx_mobj_blackbox_open1), LW(0), LW(1), LW(2), 0)
    RETURN
EVT_END()

EVT_BEGIN(mobj_save_blk_sysevt)
    SET_TYPE(0)
    CALL(evt_mario_key_onoff, 0)
    SET_VALUE(LF(10), 0)
    IF_INT_EQUAL(GSWF(233), 0)
        SET_VALUE(GSWF(233), 1)
        CALL(evt_msg_print, 0, PTR(str_save_first_hit), 0, 0)
        SET_VALUE(LF(10), 1)
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 7)
        SET_VALUE(GSWF(236), 1)
    END_IF
    SET_VALUE(LW(0), 0)
    CALL_CHILD(evt_memcard_save)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(kpa_pole)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_cam_letter_box_onoff, 0, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_mario_cam_y_on)
    CALL(kpa_disp_pole_init)
    CALL(kpa_clear_jump_data)
    CALL(kpa_get_hata_name, LW(15))
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(kpa_chg_pose, PTR(str_KPA2_G_1))
    CALL(evt_mario_get_pos, 0, LW(1), LW(2), LW(3))
    CALL(evt_snd_sfxon_3d, PTR(sfx_kuppa_post_jump1), LW(1), LW(2), LW(3), 0)
    CALL(evt_mobj_get_x_position, LW(15), LW(4))
    CALL(kpa_get_width, LW(5))
    MULTIPLY_FLOAT(LW(5), FLOAT(0.6f))
    SUBTRACT_FLOAT(LW(4), LW(5))
    CALL(evt_mario_set_pos, LW(4), LW(2), LW(3))
    CALL(kpa_score_to_str, LW(0))
    SET_FLOAT_VALUE(LW(14), FLOAT(2.0f))
    SET_FLOAT_VALUE(LW(13), FLOAT(2.0f))
    SET_FLOAT_VALUE(LW(7), FLOAT(50.0f))
    BEGIN_INLINE_EVENT
        CALL(evt_mobj_get_position, LW(15), LW(3), LW(4), LW(5))
        ADD_FLOAT(LW(3), FLOAT(7.5f))
        CALL(kpa_disp_pole_score_main, LW(3), LW(7), LW(5), LW(13))
    END_INLINE_EVENT
    CALL(evt_mario_get_pos, 0, LW(1), LW(2), LW(3))
    CALL(evt_snd_sfxon_3d, PTR(sfx_kuppa_post_slide1), LW(1), LW(2), LW(3), LW(10))
    BEGIN_INLINE_EVENT_GET_ID(LW(6))
        LOOP(0)
            CALL(evt_mobj_get_y_position, LW(15), LW(4))
            SUBTRACT_FLOAT(LW(4), LW(14))
            CALL(evt_mobj_set_y_position, LW(15), LW(4))
            WAIT_FRAMES(1)
            IF_INT_LESS_OR_EQUAL(LW(4), 25)
                CALL(evt_mobj_set_y_position, LW(15), FLOAT(25.0f))
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        SUBTRACT_FLOAT(LW(1), FLOAT(2.0f))
        CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
        IF_INT_LESS_OR_EQUAL(LW(1), FLOAT(25.0f))
            CALL(evt_mario_set_pos, LW(0), FLOAT(25.0f), LW(2))
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    LOOP(0)
        IS_EVENT_RUNNING(LW(6), LW(7))
        IF_INT_EQUAL(LW(7), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_snd_sfxoff, LW(10))
    WAIT_MS(200)
    CALL(evt_cam_get_pos, 4, LW(4), LW(5), LW(6))
    CALL(evt_cam_get_at, 4, LW(7), LW(8), LW(9))
    CALL(evt_cam3d_evt_set, LW(4), LW(5), LW(6), LW(7), LW(8), LW(9), 0, 11)
    WAIT_FRAMES(1)
    CALL(kpa_get_width, LW(3))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    MULTIPLY_FLOAT(LW(3), FLOAT(1.2f))
    ADD_FLOAT(LW(0), LW(3))
    CALL(evt_cam3d_get_shift, LW(0), LW(1), LW(2), LW(3))
    CALL(evt_cam3d_event_from_road, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7), LW(8), LW(9))
    CALL(evt_cam3d_evt_set, LW(4), LW(5), LW(6), LW(7), LW(8), LW(9), 300, 0)
    WAIT_MS(200)
    CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
    CALL(evt_mario_set_dir, 270, 0, 0)
    CALL(evt_mario_set_mov_spd, 0)
    WAIT_MS(200)
    ADD(LW(0), 20)
    CALL(evt_mario_set_pos, LW(0), 0, 10)
    WAIT_FRAMES(1)
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_cam3d_evt_off, 200, 11)
    WAIT_MS(200)
    RETURN
EVT_END()

EVT_BEGIN(kpa_pole_move)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_cam_letter_box_onoff, 0, 0)
    CALL(evt_mario_key_onoff, 0)
    CALL(kpa_get_pole_name, LW(14))
    CALL(kpa_get_hata_name, LW(15))
    CALL(evt_mobj_get_x_position, LW(14), LW(0))
    CALL(evt_mobj_get_y_position, LW(14), LW(1))
    CALL(evt_mobj_get_z_position, LW(14), LW(2))
    SET_FLOAT_VALUE(LW(6), FLOAT(2.0f))
    SET_FLOAT_VALUE(LW(7), FLOAT(9.0f))
    SET_FLOAT_VALUE(LW(8), FLOAT(5.8f))
    CALL(evt_mobj_get_x_position, LW(15), LW(3))
    CALL(evt_mobj_get_y_position, LW(15), LW(4))
    CALL(evt_mobj_get_z_position, LW(15), LW(5))
    SET_FLOAT_VALUE(LW(9), FLOAT(2.0f))
    SET_FLOAT_VALUE(LW(11), FLOAT(5.8f))
    SET_FLOAT_VALUE(LW(12), FLOAT(1.0f))
    CALL(evt_snd_sfxon_3d, PTR(sfx_kuppa_post_throw1), LW(0), LW(1), LW(2), LW(13))
    LOOP(100)
        ADD_FLOAT(LW(0), LW(6))
        ADD_FLOAT(LW(1), LW(7))
        ADD_FLOAT(LW(2), LW(8))
        ADD_FLOAT(LW(3), LW(9))
        SET_FLOAT_VALUE(LW(4), FLOAT(180.0f))
        MULTIPLY_FLOAT(LW(4), LW(12))
        ADD_FLOAT(LW(4), LW(1))
        ADD_FLOAT(LW(5), LW(11))
        CALL(evt_mobj_set_position, LW(14), LW(0), LW(1), LW(2))
        CALL(evt_mobj_set_scale, LW(14), LW(12), LW(12), LW(12))
        CALL(evt_mobj_set_position, LW(15), LW(3), LW(4), LW(5))
        CALL(evt_mobj_set_scale, LW(15), LW(12), LW(12), LW(12))
        ADD_FLOAT(LW(6), FLOAT(0.0f))
        ADD_FLOAT(LW(7), FLOAT(-0.8f))
        ADD_FLOAT(LW(8), FLOAT(0.0f))
        ADD_FLOAT(LW(9), FLOAT(0.0f))
        ADD_FLOAT(LW(11), FLOAT(0.0f))
        ADD_FLOAT(LW(12), FLOAT(0.04f))
        IF_INT_GREATER(LW(12), 3)
            SET_FLOAT_VALUE(LW(12), FLOAT(3.0f))
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_snd_sfxoff, LW(13))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(kpa_move_to_toride)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_wait_landon)
    SET_VALUE(LSWF(5), 1)
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), PTR(str_kpa_06))
        CALL(evt_mapobj_get_position, PTR(str_toride), LW(0), LW(1), LW(2))
    ELSE
        CALL(evt_mapobj_get_position, PTR(str_toride_), LW(0), LW(1), LW(2))
    END_IF
    CALL(kpa_get_level, LW(6))
    SWITCH(LW(6))
        CASE_LESS_OR_EQUAL(1)
        ADD(LW(0), 100)
        CASE_EQUAL(2)
        CALL(kpa_get_width, LW(7))
        MULTIPLY_FLOAT(LW(7), FLOAT(1.2f))
        SUBTRACT_FLOAT(LW(0), LW(7))
        CASE_EQUAL(3)
        CALL(kpa_get_width, LW(7))
        MULTIPLY_FLOAT(LW(7), FLOAT(1.2f))
        SUBTRACT_FLOAT(LW(0), LW(7))
    END_SWITCH
    CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
    CALL(evt_mario_mov_pos2, LW(0), LW(5), 90)
    IF_INT_GREATER_OR_EQUAL(LW(6), 2)
        CALL(evt_mario_get_pos, 0, LW(1), LW(2), LW(3))
        CALL(evt_snd_sfxon_3d, PTR(sfx_kuppa_struggle1), LW(1), LW(2), LW(3), LW(10))
        CALL(evt_mario_dispflag_onoff, 1, 2)
        WAIT_MS(2000)
        CALL(evt_mario_dispflag_onoff, 0, 2)
        CALL(evt_snd_sfxoff, LW(10))
        CALL(kpa_chg_pose, PTR(str_KPA2_S_1))
    END_IF
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

// jumptable_8030A78C: generated by the switch in the referencing function
// jumptable_8030A7C4: generated by the switch in the referencing function
// jumptable_8030A7FC: generated by the switch in the referencing function
// jumptable_8030A834: generated by the switch in the referencing function

#pragma force_active reset
