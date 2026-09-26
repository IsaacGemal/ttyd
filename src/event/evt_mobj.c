#include "mario/mario_motion.h"
#include "party.h"
#include "mario/mario_motion.h"
#include "pmario_sound.h"
#include "party.h"
#include "driver/mobjdrv.h"
#include "driver/animdrv.h"
#include "driver/hitdrv.h"
#include "driver/dispdrv.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "manager/fontmgr.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "memory.h"
#include <string.h>

// Prototypes for functions in other, not-yet-decompiled units (koopa / hit / etc.)
extern s32 kpaGetLevel(void);
extern void kpaAddScore(s32 score);
extern void kpaAddScorePos(s32 score, Vec* pos);
extern void kpaPowUp(void);
extern void kpaAddCoin(s32 coins);
extern s32 kpaMutekiCheck(void);
extern void kpaChgPose(const char* pose);
extern void hitGrpFlagOn(const char* name, s32 flag);
extern void hitGrpFlagOff(const char* name, s32 flag);

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
extern EffectEntry* effFireEntry(s32 a, s32 b, f32 c, f32 d, f32 e, f32 f);
extern const char str_802C46E4[];
extern Vec vec3_802c3dc8[];
extern Vec vec3_802c3dd4[];
extern Vec vec3_802c40ac[];
extern u32 dat_8041a134;

// Prototypes missing from manager/fontmgr.h
extern void FontDrawColor(GXColor* color);
extern void FontDrawStringMtx(Mtx mtx, const char* msg);

void kpa_disp_pole_score(CameraId camera, void* param);

// Prototypes missing from driver/mobjdrv.h (to move to the header later)
extern s32 mobjEntry(const char* name, const char* animPoseName);
extern void mobjDelete(const char* name);
extern MapObjectEntry* mobjNameToPtr(const char* name);
extern MapObjectEntry* mobjNameToPtrNoAssert(const char* name);
extern void mobjSetPosition(const char* name, f32 x, f32 y, f32 z);
extern EventEntry* mobjRunEvent(MapObjectEntry* entry, s32* eventCode);
s32 mobj_signboard(MapObjectEntry* obj);
s32 mobj_lock(MapObjectEntry* obj);
s32 mobj_switch_red(MapObjectEntry* obj);
s32 mobj_switch_blue(MapObjectEntry* obj);

// Prototypes missing from mario/party/sound headers (to move later)
extern void partyKeyOff(void);
extern void mobjCalcMtx(MapObjectEntry* entry);
extern void* effSmallStarEntry(s32 a, s32 b, f32 x, f32 y, f32 z, f32 f4, f32 f5, f32 f6);
extern void effSmallStarCamChg(void* entry, s32 camId);
extern void marioChgJumpStandMotion(f32 a);
extern s32 irand(s32 max);
s32 mobj_jumpstand_blue(MapObjectEntry* obj);
s32 mobj_jumpstand_red(MapObjectEntry* obj);
extern s32 marioGetPartyId(void);
extern void mobjCalcMtx2(MapObjectEntry* entry);
extern f32 PSVECMag(const Vec* v);
extern f32 PSVECDistance(const Vec* a, const Vec* b);
s32 mobj_rideswitch_lightblue(MapObjectEntry* obj);
extern void npcKoopaModeMobjBoundDeadCheck(HitEntry* hit);
extern void effMObjBrokenEntry(s32 type, f32 x, f32 y, f32 z);
s32 mobj_koopa_blk(MapObjectEntry* obj);
s32 mobj_float_blk(MapObjectEntry* obj);
s32 fire_func(EventEntry* event, BOOL isFirstCall);

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

s32 kpa_clear_jump_data(EventEntry* event, BOOL isFirstCall) {
    MarioWork* mario = marioGetPtr();
    mario->airTimer = 0;
    return 2;
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

s32 kpa_get_width(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;

    evtSetValue(event, args[0], (s32)marioGetPtr()->wPlayerCollisionBox.x);
    return 2;
}

static s32 kpa_get_level(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;

    evtSetValue(event, args[0], kpaGetLevel());
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

s32 kpa_get_pole_name(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;

    if (event->thisMapObj != NULL) {
        evtSetValue(event, args[0], (s32)event->thisMapObj->name);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return 2;
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

// NONMATCHING (94%): register allocation of the 4 extracted digits differs.

s32 kpa_score_to_str(EventEntry* event, BOOL isFirstCall) {
    s32 value = evtGetValue(event, event->args[0]);
    s32 i = 0;
    s32 thousands = value / 1000;
    s32 hundreds;
    s32 tens;
    s32 ones;

    value %= 1000;
    hundreds = value / 100;
    value %= 100;
    tens = value / 10;
    ones = value % 10;

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
            (s32)effFireEntry(5, v, f31, f30, f29, float_0p8_8041a1b0 + (f32)irand(10) / float_10_8041a1a4);
    }
    unk7C = event->userdataf[1];
    unk80 = event->userdataf[2];
    eff = (EffectEntry*)event->userdata[0];
    {
        f32 one = float_1_8041a160;
        n5 = (one - unk7C) * float_0p125_8041a1b4 + unk7C;
        n6 = (one - unk80) * float_0p125_8041a1b4 + unk80;
    }
    *(f32*)((u8*)eff->userdata + 0x78) = n5;
    *(f32*)((u8*)eff->userdata + 0x7C) = n6;
    if (n5 >= float_0p99_8041a1b8) {
        *(f32*)((u8*)eff->userdata + 0x78) = float_1_8041a160;
        *(f32*)((u8*)eff->userdata + 0x7C) = float_1_8041a160;
        return 2;
    }
    event->userdataf[1] = n5;
    event->userdataf[2] = n6;
    return 0;
}

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

s32 evt_mobj_get_kindname(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;

    name = (const char*)evtGetValue(event, args[0]);
    obj = mobjNameToPtr(name);
    evtSetValue(event, args[1], (s32)obj->animName);
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

s32 evt_mobj_check(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;

    name = (const char*)evtGetValue(event, args[0]);
    evtSetValue(event, args[1], (s32)mobjNameToPtrNoAssert(name));
    return 2;
}

s32 evt_mobj_delete(EventEntry* event, BOOL isFirstCall) {
    mobjDelete((const char*)evtGetValue(event, event->args[0]));
    return 2;
}

// NONMATCHING (97%): retail schedules the `unk1E4 = args[5]` load two instructions later (after the
// string address is materialised); otherwise byte-identical.

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
    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
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

// NONMATCHING (95%): one extra callee-saved register (register-allocation mismatch)

s32 evt_mobj_floatswitch_red(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    f32 x;
    f32 y;
    f32 z;
    s32 unk1D4;
    s32 unk1E4;
    const char* asyncName;

    evtGetValue(event, args[0]);
    name = (const char*)evtGetValue(event, args[1]);
    x = evtGetFloat(event, args[2]);
    y = evtGetFloat(event, args[3]);
    z = evtGetFloat(event, args[4]);
    unk1D4 = evtGetValue(event, args[5]);
    unk1E4 = args[6];
    asyncName = "MOBJ_RedSwitchBlock";
    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
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

// NONMATCHING (70%): register mapping now matches; the switch/string-address block is scheduled a few
// instructions earlier than retail (retail evaluates gp->inBattle and loads args[6] first).

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

// NONMATCHING (95%): one extra callee-saved register (register-allocation mismatch)

s32 evt_mobj_tornadoswitch_blue(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    f32 x;
    f32 y;
    f32 z;
    s32 unk1D4;
    s32 unk1E4;
    const char* asyncName;

    evtGetValue(event, args[0]);
    name = (const char*)evtGetValue(event, args[1]);
    x = evtGetFloat(event, args[2]);
    y = evtGetFloat(event, args[3]);
    z = evtGetFloat(event, args[4]);
    unk1D4 = evtGetValue(event, args[5]);
    unk1E4 = args[6];
    asyncName = "MOBJ_BlueTornadoSwitch";
    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
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

// NONMATCHING (95%): one extra callee-saved register (register-allocation mismatch)

s32 evt_mobj_floatswitch_blue(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    f32 x;
    f32 y;
    f32 z;
    s32 unk1D4;
    s32 unk1E4;
    const char* asyncName;

    evtGetValue(event, args[0]);
    name = (const char*)evtGetValue(event, args[1]);
    x = evtGetFloat(event, args[2]);
    y = evtGetFloat(event, args[3]);
    z = evtGetFloat(event, args[4]);
    unk1D4 = evtGetValue(event, args[5]);
    unk1E4 = args[6];
    asyncName = "MOBJ_BlueSwitchBlock";
    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
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

// NONMATCHING (75.04%): switch dispatch and the MOBJ_* pool addresses are laid out differently

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
        default:
            asyncName = "HOBJ_BlueSwitch";
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

// NONMATCHING (93.34%): register allocation (r28/r30 swapped) around the local jumptable

s32 evt_mobj_jumpstand_blue(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    const char* name = (const char*)evtGetValue(event, args[1]);
    f32 f31 = evtGetFloat(event, args[2]);
    f32 f30 = evtGetFloat(event, args[3]);
    f32 f29 = evtGetFloat(event, args[4]);
    f32 f28 = evtGetFloat(event, args[5]);
    s32 unk1D4 = evtGetValue(event, args[6]);
    s32 unk1E4 = args[7];
    const char* asyncName;
    MapObjectEntry* obj;

    {
        const char* jumptable[2] = {"MOBJ_BlueJumpStand", "MOBJ_BlueJumpStand2"};
        asyncName = jumptable[type];
    }
    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
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

// NONMATCHING (91.16%): register allocation around the local jumptable

s32 evt_mobj_jumpstand_red(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    const char* name = (const char*)evtGetValue(event, args[1]);
    f32 f31 = evtGetFloat(event, args[2]);
    f32 f30 = evtGetFloat(event, args[3]);
    f32 f29 = evtGetFloat(event, args[4]);
    s32 unk1D4 = evtGetValue(event, args[5]);
    s32 unk1E4 = args[6];
    const char* asyncName;
    MapObjectEntry* obj;

    {
        const char* jumptable[2] = {"MOBJ_RedJumpStand", "MOBJ_RedJumpStand2"};
        asyncName = jumptable[type];
    }
    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
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

// NONMATCHING (96.42%): one extra callee-saved register

s32 evt_mobj_rideswitch_lightblue(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    f32 f31;
    f32 f30;
    f32 f29;
    s32 unk1D4;
    s32 unk1D8;
    s32 unk1E4;
    const char* asyncName;

    name = (const char*)evtGetValue(event, args[0]);
    f31 = evtGetFloat(event, args[1]);
    f30 = evtGetFloat(event, args[2]);
    f29 = evtGetFloat(event, args[3]);
    unk1D4 = evtGetValue(event, args[4]);
    unk1D8 = evtGetValue(event, args[5]);
    unk1E4 = args[6];
    asyncName = str_802C46E4;
    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
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

// NONMATCHING (84%): case 2's bbox float computation is scheduled with different FP registers
// (and the .sdata2/rodata constant symbols are anonymous @NNN here).

s32 mobj_rideswitch_lightblue(MapObjectEntry* obj) {
    Vec* vec = vec3_802c3dc8;

    switch (obj->unk1DC) {
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
            Vec v = *(Vec*)((u8*)vec + 0x88);
            f32 mag;
            f32 dist;

            v.x = 0.8f * (10.0f * data->bboxMax.x);
            v.z = 0.8f * (10.0f * data->bboxMax.z);
            v.y = 10.0f * data->bboxMax.y;
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
            f32 y = obj->scale2.y + 0.9f;
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
