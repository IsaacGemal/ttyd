#include "dolphin/types.h"
#include "dolphin/mtx.h"
#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "mario/mario_pouch.h"
#include "mario/mario_sbr.h"
#include "driver/hitdrv.h"
#include "driver/mobjdrv.h"
#include "driver/animdrv.h"
#include "eff/eff_ultra_hammer.h"
#include "eff/eff_kemutest.h"
#include "pmario_sound.h"
#include "system.h"
#include "manager/evtmgr.h"
#include "evt/evt_cmd.h"
#include "evt/evt_cam.h"
#include "motion/mot_stay.h"
#include "motion/mot_hammer.h"
#include "math_ext.h"
#include <string.h>

// Raw accessors for MarioWork scratch words this motion reads with a type no union member has.
#define M_S16(m, off) (*(s16*)((u8*)(m) + (off)))
#define M_U16(m, off) (*(u16*)((u8*)(m) + (off)))
#define M_S32(m, off) (*(s32*)((u8*)(m) + (off)))

// .rodata: vec3_802c5a80, then hitEstimate's Vec literals; the MOBJ names are defined after hitEstimate.
const Vec vec3_802c5a80 = { 0.0f, 40.0f, 0.0f };
extern const char str_MOBJ_Lv1Block[];
extern const char str_MOBJ_Lv1BigBlock[];
extern const char str_MOBJ_Lv2Block[];
extern const char str_MOBJ_Lv2BigBlock[];
extern const char str_MOBJ_Lv3Block[];
extern const char str_MOBJ_Lv3BigBlock[];

// .sdata
s32 se_data[10] = {
    0x13B, 0x165, 0x13C, 0x167, 0x13D, 0x166, 0x13E, 0x168, 0x13F, 0x169,
};
s32 lv1quake_evt[] = {
    EVT_CMD(5, OPCODE_USER_FUNC), (s32)evt_cam_shake, 4, 0xF24A7A80, 0xF24A7A8A, 0x5A,
    EVT_CMD(0, OPCODE_END_EVENT), OPCODE_END_SCRIPT
};
s32 lv2quake_evt[] = {
    EVT_CMD(5, OPCODE_USER_FUNC), (s32)evt_cam_shake, 4, 0xF24A7A80, 0xF24A7A94, 0x6E,
    EVT_CMD(0, OPCODE_END_EVENT), OPCODE_END_SCRIPT
};
s32 lv3quake_evt[] = {
    EVT_CMD(5, OPCODE_USER_FUNC), (s32)evt_cam_shake, 4, 0xF24A7A80, 0xF24A7A9E, 0x8C,
    EVT_CMD(0, OPCODE_END_EVENT), OPCODE_END_SCRIPT
};

// .sdata2
extern const f32 float_2_8041a9a0;
extern const f32 float_0p5_8041a9a4;
extern const f32 float_0_8041a9a8;

s32 marioGetHammerLevel(void) {
    return pouchGetPtr()->hammerLevel;
}

#ifdef MUST_MATCH
// Inline helper for the `type == 7 ? 3 : type` clamp: through a helper the type lands in r0 and the clamped
// value in r3, as in the target; the plain expression swaps the two.
static inline s32 clampHammerType(s32 t) {
    return (t == 7) ? 3 : t;
}
#endif

// The `type == 7 ? 3 : type` clamp, the se_data update order, the switches, the pool and every callee-saved
// register (base r31, mario r30, r28/r29, f30/f31) match. The pose-time clamp `b <= 10 ? b : 10` is branchy
// in the target (`cmpwi; bgt; mr r0,r3; b end; li r0,10`); the plain ternary is if-converted to the one
// instruction shorter `cmpwi; mr r0,r3; ble; li r0,10`. The clamp goes through a temp `t` whose condition and
// arm are syntactically different (the named field vs this file's raw accessor), which blocks that
// if-conversion, and `opt_dead_assignments off` keeps the `r27 = t` copy so the target's `r0`/`r27` split
// survives. Together they give the target's bytes.
// Common subexpression elimination off keeps the rodata base in r31 and the mario pointer in r30; with it on
// the two are swapped and every access thereafter shifts. Needed only for this function.
#pragma opt_common_subs off
void mot_hammer(void) {
#pragma opt_dead_assignments off // keep the `r27 = t` pose-time copy (target splits r0/r27)
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2;
    MarioWork* mario3;
    MapObjectEntry* mobj;
    s32 mt;
    s32 sound;
    s32 r28, rp;
    u32 i;
    s32 type;
    s32 r27;
    s32 eff;
    s32 lv;
    HitEntry* hit;
    f32 f31;
    f32 f30;
    f32 ang2;
    f32 px;
    f32 pz;

    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1;
        mario->flags &= ~0x000F0000;
        mario->flags |= 0x80;
        f30 = mario->dispDir;
        {
            f32 off;
            MarioWork* m = marioGetPtr();
            off = float_0_8041a9a8;
            if (m->dispFlags & 0x80000000) {
                f32 ang = m->dispDir;
                s32 facing = 0;
                if (ang >= 90.f && ang <= 270.f) {
                    facing = 1;
                }
                off = facing ? 30.f : -30.f;
            }
            if (f30 >= 90.f && f30 <= 270.f) {
                ang2 = revise360(180.f + off);
            } else {
                ang2 = revise360(off);
            }
            mario->field_0x2A4 = ang2;
        }
        mario->multiTimer = 0;
        mario->airTimer = 0;
        mario->currSubMotionId = 0;
        mario->baseSpeed = float_0_8041a9a8;
        mario->field_0x200 = NULL;
        hit = hitEstimate(&type);
        mario->field_0x200 = hit;
        if (hit != NULL) {
            f30 = mario->dispDir;
            {
                f32 v = (marioGetPtr()->dispFlags & 0x80000000) ? -30.f : float_0_8041a9a8;
                if (f30 >= 90.f && f30 <= 270.f) {
                    ang2 = toMovedir2(180.f, v);
                } else {
                    ang2 = toMovedir2(float_0_8041a9a8, v);
                }
                mario->directionView = ang2;
            }
            f30 = mario->dispDir;
            {
                f32 off;
                MarioWork* m = marioGetPtr();
                off = float_0_8041a9a8;
                if (m->dispFlags & 0x80000000) {
                    f32 ang = m->dispDir;
                    s32 facing = 0;
                    if (ang >= 90.f && ang <= 270.f) {
                        facing = 1;
                    }
                    off = facing ? 30.f : -30.f;
                }
                if (f30 >= 90.f && f30 <= 270.f) {
                    ang2 = revise360(180.f + off);
                } else {
                    ang2 = revise360(off);
                }
                mario->field_0x2A4 = ang2;
            }
        }
        if (hitGetAttr(hit) & 0x80000000) {
            mobj = mobjHitObjPtrToPtr(hit);
            r28 = 0;
            if (strcmp(mobj->animName, str_MOBJ_Lv1Block) == 0 ||
                strcmp(mobj->animName, str_MOBJ_Lv1BigBlock) == 0) {
                if (pouchGetPtr()->hammerLevel >= 1) {
                    r28 = 1;
                }
            } else if (strcmp(mobj->animName, str_MOBJ_Lv2Block) == 0 ||
                       strcmp(mobj->animName, str_MOBJ_Lv2BigBlock) == 0) {
                if (pouchGetPtr()->hammerLevel >= 2) {
                    r28 = 1;
                }
            } else if (strcmp(mobj->animName, str_MOBJ_Lv3Block) == 0 ||
                       strcmp(mobj->animName, str_MOBJ_Lv3BigBlock) == 0) {
                if (pouchGetPtr()->hammerLevel >= 3) {
                    r28 = 1;
                }
            }
            if (r28) {
                type = 0;
            }
        }
        switch (pouchGetHammerLv()) {
        case 1:
            marioChgPose(type == 7 ? "M_H_2" : "M_H_1");
            break;
        case 2:
            marioChgPose(type == 7 ? "M_H_5" : "M_H_4");
            break;
        case 3:
            marioChgPose(type == 7 ? "M_H_8" : "M_H_7");
            break;
        }
        if (type != 0) {
#ifdef MUST_MATCH
            M_S32(mario, 0x2A0) = clampHammerType(type);
#else
            M_S32(mario, 0x2A0) = (type == 7) ? 3 : type;
#endif
            mario->flags |= 0x800;
        } else {
            mario->flags &= ~0x800;
        }
        mario2 = marioGetPtr();
        sound = 0x15B;
        switch (pouchGetHammerLv()) {
        case 1:
            sound = 0x15B;
            break;
        case 2:
            sound = 0x15C;
            break;
        case 3:
            sound = 0x15D;
            break;
        }
        psndSFXOn_3D((const char*)sound, &mario2->position);
        mario->field_0x2A8 = type;
    }
    mario->dispDir = (mario->field_0x2A4 >= 90.f && mario->field_0x2A4 <= 270.f)
                              ? 180.f
                              : float_0_8041a9a8;
    mario->dispReqDir = mario->dispDir;
    mario->moveDir = toMovedir(mario->field_0x2A4);
    mario->directionView = mario->moveDir;
    rp = 0;
    if (mario->flags & 0x800) {
        s32 b = mario->multiTimer;
        s32 a = M_S32(mario, 0x2A0);
        r27 = (b <= a) ? b : a;
        if (b == a) {
            rp = 1;
        }
    } else {
        s32 b = mario->multiTimer;
        s32 t = (b <= 10) ? mario->multiTimer : 10;
        r27 = t;
        if (b == 10) {
            rp = 1;
        }
    }
    mario->poseTime = r27;
    animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], (f32)r27);
    if (rp != 0) {
        lv = pouchGetPtr()->hammerLevel;
        if (lv == 1) {
            evtEntry(lv1quake_evt, 0, 0);
        } else if (lv == 2) {
            evtEntry(lv2quake_evt, 0, 0);
        } else if (lv == 3) {
            evtEntry(lv3quake_evt, 0, 0);
        }
        mario->currSubMotionId = 1;
        mario->hitobjHammer = mario->field_0x200;
        px = mario->position.x;
        f30 = mario->position.y;
        pz = mario->position.z;
        if (!(mario->dispFlags & 0x80000000)) {
            eff = -1;
            switch (mario->field_0x2A8) {            case 7:
                movePos(&px, &pz, 16.0f, toMovedir2(mario->dispReqDir, float_0_8041a9a8));
                f31 = 53.0f;
                eff = 0x16;
                break;
                        case 0:
            case 6:
                movePos(&px, &pz, 25.0f, toMovedir2(mario->dispReqDir, float_0_8041a9a8));
                f31 = -3.0f;
                eff = 0x15;
                break;
            case 5:
                movePos(&px, &pz, 31.0f, toMovedir2(mario->dispReqDir, float_0_8041a9a8));
                f31 = 5.0f;
                eff = 0x15;
                break;
            case 4:
                movePos(&px, &pz, 30.f, toMovedir2(mario->dispReqDir, float_0_8041a9a8));
                f31 = 30.f;
                eff = 0x16;
                break;
}
        } else {
            eff = -1;
            switch (mario->field_0x2A8) {            case 7:
                movePos(&px, &pz, 16.0f, toMovedir2(mario->dispReqDir, -2.0f));
                f31 = 53.0f;
                eff = 0x16;
                break;
                        case 0:
            case 6:
                movePos(&px, &pz, 30.f, toMovedir2(mario->dispReqDir, -6.0f));
                f31 = -3.0f;
                eff = 0x15;
                break;
            case 5:
                movePos(&px, &pz, 36.0f, toMovedir2(mario->dispReqDir, -6.0f));
                f31 = 20.f;
                eff = 0x16;
                break;
            case 4:
                movePos(&px, &pz, 37.0f, toMovedir2(mario->dispReqDir, -6.0f));
                f31 = 28.0f;
                eff = 0x16;
                break;
}
        }
        if (eff > 0) {
            effKemuTestEntry(eff, px, f30 + f31, pz, 1.0f);
        }
        mario3 = marioGetPtr();
        sound = 0x161;
        switch (pouchGetHammerLv()) {
        case 1:
            psndSFXOn_3D((const char*)0x15E, &mario3->position);
            break;
        case 2:
            psndSFXOn_3D((const char*)0x15F, &mario3->position);
            break;
        case 3:
            psndSFXOn_3D((const char*)0x160, &mario3->position);
            break;
        }
        i = 0;
        while (i < 10) {
            if (pouchEquipCheckBadge(se_data[mario3->field_0x3A]) != 0) {
                sound = se_data[mario3->field_0x3A + 1];
                mario3->field_0x3A = mario3->field_0x3A + 2;
                if (mario3->field_0x3A >= 0xA) {
                    mario3->field_0x3A -= 0xA;
                }
                break;
            } else {
                mario3->field_0x3A = mario3->field_0x3A + 2;
                if (mario3->field_0x3A >= 0xA) {
                    mario3->field_0x3A -= 0xA;
                }
                i++;
            }
        }
        psndSFXOn_3D((const char*)sound, &mario3->position);
    }
    mt = mario->multiTimer + 1;
    mario->multiTimer = mt;
    if (mt > 0x14) {
        mario->flags &= ~0x80;
        marioChgMot(0);
    }
}
#pragma opt_dead_assignments reset
#pragma opt_common_subs reset

s32 marioChkHammer2(void) {
    MarioWork* mario;
    s32 sp10;
    s32 spC;
    f32 sp8;
    s32 ret;

    mario = marioGetPtr();
    if (pouchGetPtr()->hammerLevel < 2) {
        return 0;
    }
    if (!(mario->buttonHeld & 0x200)) {
        if (!(mario->buttonHeld & 0x100)) {
            if (strcmp(mario->animName, "M_I_Y") == 0 && mario->currMotionId == MARIO_MOTION_STAY) {
                marioChgPose("M_S_1");
                marioVoiceGlareOff();
            }
            mario->wMotionTimer = 0;
        }
        return 0;
    }
    mario->wMotionTimer++;
    if (mario->controlStickSensitivity != float_0_8041a9a8) {
        if (mario->wMotionTimer > 10 && mario->currMotionId == MARIO_MOTION_STAY &&
            !(mario->dispFlags & 0x40000000)) {
            marioChgPose("M_I_Y");
            marioVoiceGlareOn();
        }
    }
    if (mario->wMotionTimer >= 50) {
        mario->wMotionTimer = 50;
        if (mario->currMotionId == MARIO_MOTION_STAY && !(mario->dispFlags & 0x40000000)) {
            marioChgPose("M_I_Y");
            marioVoiceGlareOn();
        }
    }
    ret = marioGetRub(0x200, &spC, &sp10, &sp8);
    if (ret != 1) {
        return 0;
    }
    mario->baseSpeed = float_0_8041a9a8;
    if (sp10 < 1) {
        return 0;
    }
    mario->dispHalfTurn = spC;
    return 1;
}

// The original keeps a dead `hitGetAttr(hit) & 0x800005` comparison in this function (its branch was
// folded away), while the compiler eliminates it from the natural empty-body form. A no-op store to
// `hit` (below) keeps the comparison alive without adding instructions.
#pragma opt_propagation off // keep mot_hammer2's f0/f1/f2 load split
void mot_hammer2(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* m2;
    HitEntry* hit;
    f32 f30;
    f32 f31;
    s32 rub2;
    s32 rub1;
    f32 rub3;
    f32 hnz;
    f32 hny;
    f32 hnx;
    f32 dist;
    f32 hz;
    f32 hy;
    f32 hx;
    f32 ca;
    f32 sa;
    f32 px__;
    f32 py__;
    f32 pz__;
    f32 one__;
    s32 level;

    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1;
        mario->flags &= ~0x000F0000;
        mario->field_0x2B4 = 0;
        mario->currSubMotionId = 0;
        M_S32(mario, 0x2B8) = -1;
    }
    switch (mario->currSubMotionId) {
    case 0:
        mario->flags |= 0x80;
        mario->dispFlags |= 4;
        mario->field_0x41 = 0;
        mario->field_0x42 = 0;
        mario->baseSpeed = float_0_8041a9a8;
        mario->airTimer = 0;
        mario->currSubMotionId = 1;
        mario->multiTimer = 0x1E;
        if (mario->field_0x42 == 3) {
            mario->wCamVal1 = 38.0f;
        } else if (mario->field_0x42 <= 2) {
            mario->wCamVal1 = 15.0f;
        } else {
            mario->wCamVal1 = 10.f;
        }
        switch (pouchGetHammerLv()) {
        case 1:
            marioChgPose("M_H_3");
            break;
        case 2:
            marioChgPose("M_H_6");
            break;
        case 3:
            marioChgPose("M_H_9");
            break;
        }
        mario->field_0x200 = NULL;
        mario->currSubMotionId = 1;
        mario->multiTimer = 1;
        break;
    case 1:
        if (--mario->multiTimer > 0) {
            break;
        }
        if (180.f == mario->dispReqDir && mario->dispHalfTurn == 1) {
            mario->dispFlags |= 8;
        }
        if (float_0_8041a9a8 == mario->dispReqDir && mario->dispHalfTurn == 0) {
            mario->dispFlags |= 0x10;
        }
        marioPaperOn("p_kaiten_h");
        marioChgPaper("P_H_1C");
        marioPaperLightOff();
        M_S32(mario, 0x2B8) = psndSFXOn_3D((const char*)0x162, &mario->position);
        mario->field_0x2B0 = float_0_8041a9a8;
        mario->poseTime = 0;
        animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], mario->field_0x2B0);
        mario->currSubMotionId = 0xA;
        mario->multiTimer = 0x1E;
        /* fallthrough to case 0xA */
    case 0xA:
        if (marioGetRub(0x200, &rub1, &rub2, &rub3) == 1) {
            mario->field_0x2B4 = mario->field_0x2B4 & ~1;
            mario->field_0x41++;
            if (mario->field_0x41 >= 0x14) {
                mario->field_0x41 = 0x14;
            }
        }
        if (mario->field_0x41 >= 4) {
            mario->field_0x2B0 += 6.0f;
            if (mario->field_0x2B0 >= 72.0f) {
                mario->field_0x2B0 = 72.0f;
            }
            mario->currSubMotionId = 0xB;
            marioChgPaper("P_H_1B");
            mario->field_0x42 = 3;
            m2 = marioGetPtr();
            m2->unk148 = 0.4f;
            m2->unk150.z = 0.005f;
#ifdef MUST_MATCH
            *(volatile f32*)&m2->field_68 = 1.7f;
            *(volatile Vec*)&m2->field_6C = *(volatile Vec*)&vec3_802c5a80;
#else
            m2->field_68 = 1.7f;
            m2->field_6C = vec3_802c5a80;
#endif
            mario->field_0x2B4 |= 0x1000;
        } else if (mario->field_0x41 >= 3) {
            marioChgPaper("P_H_1A");
            level = mario->field_0x2B4;
            if (!(level & 1)) {
                mario->field_0x2B0 += 6.0f;
                if (mario->field_0x2B0 >= 54.0f) {
                    mario->field_0x2B0 = 54.0f;
                    mario->field_0x2B4 |= 1;
                    mario->field_0x42 = 3;
                    m2 = marioGetPtr();
                    m2->unk148 = 0.4f;
                    m2->unk150.z = 0.005f;
#ifdef MUST_MATCH
                    *(volatile f32*)&m2->field_68 = 1.7f;
                    *(volatile Vec*)&m2->field_6C = *(volatile Vec*)&vec3_802c5a80;
#else
                    m2->field_68 = 1.7f;
                    m2->field_6C = vec3_802c5a80;
#endif
                    mario->field_0x2B4 |= 0x1000;
                }
            } else if (!(level & 0x10)) {
                mario->field_0x2B0 += 1.0f;
                if (mario->field_0x2B0 >= 54.0f) {
                    mario->field_0x2B0 = 54.0f;
                    mario->field_0x2B4 |= 0x10;
                }
            } else {
                mario->field_0x2B0 -= 1.0f;
                if (mario->field_0x2B0 <= 42.0f) {
                    mario->field_0x2B0 = 42.0f;
                    mario->field_0x2B4 &= ~0x10;
                }
            }
        } else if (mario->field_0x41 >= 2) {
            marioChgPaper("P_H_1A");
            level = mario->field_0x2B4;
            if (!(level & 1)) {
                mario->field_0x2B0 += 6.0f;
                if (mario->field_0x2B0 >= 36.0f) {
                    mario->field_0x2B0 = 36.0f;
                    mario->field_0x2B4 |= 1;
                    mario->field_0x42 = 2;
                    m2 = marioGetPtr();
                    m2->unk148 = 0.3f;
                    m2->unk150.z = 0.005f;
#ifdef MUST_MATCH
                    *(volatile f32*)&m2->field_68 = 1.6f;
                    *(volatile Vec*)&m2->field_6C = *(volatile Vec*)&vec3_802c5a80;
#else
                    m2->field_68 = 1.6f;
                    m2->field_6C = vec3_802c5a80;
#endif
                    mario->field_0x2B4 |= 0x1000;
                }
            } else if (!(level & 0x10)) {
                mario->field_0x2B0 += 1.0f;
                if (mario->field_0x2B0 >= 36.0f) {
                    mario->field_0x2B0 = 36.0f;
                    mario->field_0x2B4 |= 0x10;
                }
            } else {
                mario->field_0x2B0 -= 1.0f;
                if (mario->field_0x2B0 <= 30.f) {
                    mario->field_0x2B0 = 30.f;
                    mario->field_0x2B4 &= ~0x10;
                }
            }
        } else {
            marioChgPaper("P_H_1A");
            level = mario->field_0x2B4;
            if (!(level & 1)) {
                mario->field_0x2B0 += 6.0f;
                if (mario->field_0x2B0 >= 18.0f) {
                    mario->field_0x2B0 = 18.0f;
                    mario->field_0x2B4 |= 1;
                    mario->field_0x42 = 1;
                    m2 = marioGetPtr();
                    m2->unk148 = 0.2f;
                    m2->unk150.z = 0.005f;
#ifdef MUST_MATCH
                    *(volatile f32*)&m2->field_68 = 1.5f;
                    *(volatile Vec*)&m2->field_6C = *(volatile Vec*)&vec3_802c5a80;
#else
                    m2->field_68 = 1.5f;
                    m2->field_6C = vec3_802c5a80;
#endif
                    mario->field_0x2B4 |= 0x1000;
                }
            } else if (!(level & 0x10)) {
                mario->field_0x2B0 += 1.0f;
                if (mario->field_0x2B0 >= 24.0f) {
                    mario->field_0x2B0 = 24.0f;
                    mario->field_0x2B4 |= 0x10;
                }
            } else {
                mario->field_0x2B0 -= 1.0f;
                if (mario->field_0x2B0 <= 18.0f) {
                    mario->field_0x2B0 = 18.0f;
                    mario->field_0x2B4 &= ~0x10;
                }
            }
        }
        if (mario->field_0x2B4 & 0x1000) {
            mario->field_0x2B4 &= ~0x1000;
            if (!(mario->field_0x2B4 & 0x2000)) {
                mario->field_0x2B4 |= 0x2000;
                switch (pouchGetHammerLv()) {
                case 1:
                    marioChgPose("M_H_3A");
                    break;
                case 2:
                    marioChgPose("M_H_6A");
                    break;
                case 3:
                    marioChgPose("M_H_9A");
                    break;
                }
            }
        }
        if (strcmp(mario->paperAnimName, "P_H_1A") == 0) {
            animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], mario->field_0x2B0 / 6.0f);
        }
    /* fallthrough to case 0xB */
    case 0xB:
        if (mario->buttonHeld & 0x200) {
            break;
        }
        if ((u32)M_S32(mario, 0x2B8) != 0xFFFFFFFFu) {
            psndSFXOff(M_S32(mario, 0x2B8));
            M_S32(mario, 0x2B8) = -1;
        }
        m2 = marioGetPtr();
        m2->unk148 = float_0_8041a9a8;
        m2->unk150.z = 0.01f;
        switch (pouchGetHammerLv()) {
        case 1:
            marioChgPose("M_H_3B");
            break;
        case 2:
            marioChgPose("M_H_6B");
            break;
        case 3:
            marioChgPose("M_H_9B");
            break;
        }
        marioChgPaper("P_H_1C");
        mario->currSubMotionId = 0x14;
        if (mario->field_0x42 == 3) {
            mario->multiTimer = 0x30;
        } else if (mario->field_0x42 == 2) {
            mario->multiTimer = 0x42;
        } else {
            mario->multiTimer = 0x48;
        }
        M_U16(mario, 0x2A4) = 0;
        if (mario->field_0x42 == 3) {
            mario->wCamVal1 = 38.0f;
        } else if (mario->field_0x42 == 2) {
            mario->wCamVal1 = 22.0f;
        } else if (mario->field_0x42 <= 1) {
            mario->wCamVal1 = 15.0f;
        }
        py__ = 20.f;
        px__ = mario->position.x;
        py__ += mario->position.y;
        pz__ = mario->position.z;
        one__ = 1.0f;
        effUltraHammerEntry(mario->dispHalfTurn != 0, mario->multiTimer, px__, py__, pz__, one__);
        break;
    case 0x14:
        f31 = mario->dispDir;
        M_S16(mario, 0x2A4) = 0xA;
        mario->dispFlags |= 0x100;
        f30 = revise360(270.f - mario->dispDir);
        m2 = marioGetPtr();
        sincosf(&sa, &ca, f30);
        dist = float_2_8041a9a0 * m2->wPlayerCollisionBox.x;
        hit = hitCheckFilter(NULL, m2->position.x, float_0p5_8041a9a4 * m2->wPlayerCollisionBox.y + m2->position.y, m2->position.z, sa, float_0_8041a9a8, ca, &hx,
                             &hy,
                             &hz,
                             &dist, &hnx, &hny, &hnz);
#ifdef MUST_MATCH
        // The original keeps the wall's attribute-mask comparison in the output even though its result
        // is never used (the branch on it was folded away). Written as an empty `if`, this compiler
        // dead-code-eliminates the whole comparison. A no-op store of `hit` back to itself keeps the
        // first `if` body non-empty so the comparison survives, and the store itself optimises to
        // nothing. The #else below is the natural (empty-body) form.
        {
            if (hit != NULL && (hitGetAttr(hit) & 0x800005)) {
                hit = (HitEntry*)((u32)hit | 0);
            }
            if (hit != NULL) {
                mario->hitobjHammer = hit;
            }
        }
#else
        // The original tests the wall's attributes here and does nothing with the result.
        if (hit != NULL && (hitGetAttr(hit) & 0x800005)) {
        }
        if (hit != NULL) {
            mario->hitobjHammer = hit;
        }
#endif
        f30 = mario->wCamVal1;
        mario->dispDir += f30 * (mario->dispHalfTurn ? -1.0f : 1.0f);
        if ((s32)f31 / 360 != (s32)mario->dispDir / 360) {
            psndSFXOn_3D((const char*)0x163, &mario->position);
        }
        if (M_S16(mario, 0x2A4) == 1) {
            psndSFXOn_3D((const char*)0x163, &mario->position);
        }
        mario->multiTimer--;
        if (mario->field_0x42 > 2 && mario->multiTimer == 0xA) {
            marioPaperOff();
            marioPaperLightOn();
            marioChgPose("M_S_1");
            mario->dispFlags &= ~4;
            mario->dispFlags &= ~8;
            mario->dispFlags &= ~0x10;
        }
        if (mario->multiTimer > 0) {
            break;
        }
        if (mario->field_0x42 <= 2) {
            mario->multiTimer = sysMsec2Frame(0x12C);
        } else {
            mario->dispDir = revise360(mario->dispDir);
            marioAdjustMoveDir();
            mario->multiTimer = 0x1E;
        }
        mario->currSubMotionId = 0x16;
        /* fallthrough to case 0x16 */
    case 0x16:
        if (--mario->multiTimer > 0) {
            break;
        }
        if (mario->field_0x42 <= 2) {
            marioPaperOff();
            marioPaperLightOn();
            marioChgPose("M_S_1");
            mario->dispFlags &= ~4;
            mario->dispFlags &= ~8;
            mario->dispFlags &= ~0x10;
            mario->dispDir = revise360(mario->dispDir);
            marioAdjustMoveDir();
        }
        mario->dispFlags &= ~0x100;
        mario->currSubMotionId = 0x17;
        mario->multiTimer = 1;
        /* fallthrough to case 0x17 */
    case 0x17:
        if (--mario->multiTimer > 0) {
            break;
        }
        mario->multiTimer = 1;
        /* fallthrough to case 0x18 */
    case 0x18:
        if (--mario->multiTimer > 0) {
            break;
        }
        mario->dispFlags &= ~4;
        mario->flags &= ~0x80;
        marioAdjustMoveDir();
        marioChgMotSub(0, 0);
        break;
    default:
        break;
    }
}
#pragma opt_propagation reset

f32 marioGetAngleKaitenHammer(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->currMotionId != MARIO_MOTION_HAMMER_2) {
        return -1.0f;
    }
    if (mario->currSubMotionId != 0x14) {
        return -2.0f;
    }
    return revise360(270.f - mario->dispDir);
}

void mot_ham2_post(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2;

    if (mario->dispFlags & 0x40000000) {
        marioPaperOff();
    }
    mario2 = marioGetPtr();
    mario2->unk148 = float_0_8041a9a8;
    mario2->unk150.z = 0.01f;
    marioPaperOff();
    marioPaperLightOn();
    mario->dispFlags &= ~0x100;
    mario->dispFlags &= ~0x8;
    mario->dispFlags &= ~0x10;
    mario->dispFlags &= ~0x4;
    mario->flags &= ~0x80;
    marioAdjustMoveDir();
}

HitEntry* hitEstimate(s32* type) {
    HitCheckQuery query;
    Vec pos;
    Vec dir3;
    MarioWork* mario;
    HitEntry* hit;
    f32 angle;
    f32 boxY;
    f32 sa;
    f32 ca;
    f32 off;
    f32 dy;
    f32 off2;
    f32 dist;
    f32 c;
    s32 i;

    mario = marioGetPtr();
    *type = 0;
    angle = mario->dispDir;
    if (marioGetPtr()->dispFlags & 0x80000000) {
        off = -20.f;
    } else {
        off = float_0_8041a9a8;
    }
    if (angle >= 90.f && angle <= 270.f) {
        angle = toMovedir2(180.f, off);
    } else {
        angle = toMovedir2(float_0_8041a9a8, off);
    }
    sincosf(&sa, &ca, angle);
    boxY = mario->wPlayerCollisionBox.y;
    { f32 p = 3.1415927f; c = cos(p); } // a float local: a bare literal makes MWCC pool a double and load it with lfd
    pos = mario->position;
    pos.y = 4.0f + (mario->position.y + boxY);
    query.targetPos = pos;
    query.targetDir = (Vec){ sa, -c, ca };
    query.targetDistance = 14.0f;
    hit = hitCheckVecFilter(&query, NULL);
    if (hit != NULL && (hitGetAttr(hit) & 0x800005) == 0) {
        *type = 7;
        return hit;
    }

    { f32 p = 1.2217306f; c = cos(p); } // a bare literal makes MWCC pool a double and load it with lfd
    { f32 five = 5.0f; // local keeps the 5.0f load after the stack load (target order)
    pos.x = mario->position.x + sa * five;
    pos.z = mario->position.z + ca * five; }
    pos.y = mario->position.y + boxY;
    query.targetPos = pos;
    query.targetDir = (Vec){ sa, -c, ca };
    query.targetDistance = 26.0f;
    hit = hitCheckVecFilter(&query, NULL);
    if (hit != NULL && (hitGetAttr(hit) & 0x800005) == 0) {
        *type = (query.hitPos.y - mario->position.y > 30.f) ? 4 : 5;
        return hit;
    }

    { f32 p = 0.7853982f; c = cos(p); } // a bare literal makes MWCC pool a double and load it with lfd
    { f32 five = 5.0f; // local keeps the 5.0f load after the stack load (target order)
    pos.x = mario->position.x + sa * five;
    pos.z = mario->position.z + ca * five; }
    pos.y = mario->position.y + boxY;
    query.targetPos = pos;
    query.targetDir = (Vec){ sa, -c, ca };
    query.targetDistance = 30.f;
    hit = hitCheckVecFilter(&query, NULL);
    if (hit != NULL && (hitGetAttr(hit) & 0x800005) == 0) {
        *type = 5;
        return hit;
    }

    c = cos(float_0_8041a9a8);
    dir3 = (Vec){ 0.0f, -c, 0.0f };
    for (i = 0; i < 2; i++) {
        if (i == 0) {
            dist = 36.0f;
            off2 = 20.f;
        } else {
            dist = 34.0f;
            if (mario->dispFlags & 0x80000000) {
                off2 = 37.0f;
            } else {
                off2 = dist;
            }
        }
        pos.x = mario->position.x + sa * off2;
        pos.z = mario->position.z + ca * off2;
        pos.y = mario->position.y + boxY;
        query.targetPos = pos;
        query.targetDir = dir3;
        query.targetDistance = dist;
        hit = hitCheckVecFilter(&query, NULL);
        if (hit != NULL && (hitGetAttr(hit) & 0x800005) == 0) {
            dy = query.hitPos.y - mario->position.y;
            *type = (dy > 13.0f) ? 5 : 6;
            return hit;
        }
    }
    return hit;
}

const char str_MOBJ_Lv1Block[] = "MOBJ_Lv1Block";
const char str_MOBJ_Lv1BigBlock[] = "MOBJ_Lv1BigBlock";
const char str_MOBJ_Lv2Block[] = "MOBJ_Lv2Block";
const char str_MOBJ_Lv2BigBlock[] = "MOBJ_Lv2BigBlock";
const char str_MOBJ_Lv3Block[] = "MOBJ_Lv3Block";
const char str_MOBJ_Lv3BigBlock[] = "MOBJ_Lv3BigBlock";

// Named .sdata2 prefix: the first three floats of the original's layout are real objects.
const f32 float_2_8041a9a0 = 2.0f;
const f32 float_0p5_8041a9a4 = 0.5f;
const f32 float_0_8041a9a8 = 0.0f;
