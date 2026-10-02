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

// Raw accessors for MarioWork fields that are not named in mario.h yet; listed in REPORT.md.
#define M_S8(m, off) (*(s8*)((u8*)(m) + (off)))
#define M_S16(m, off) (*(s16*)((u8*)(m) + (off)))
#define M_U16(m, off) (*(u16*)((u8*)(m) + (off)))
#define M_S32(m, off) (*(s32*)((u8*)(m) + (off)))
#define M_F32(m, off) (*(f32*)((u8*)(m) + (off)))

// .rodata
const Vec vec3_802c5a80 = { 0.0f, 40.0f, 0.0f };
const Vec vec3_802c5a8c = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5a98 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5aa4 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5ab0 = { 0.0f, 0.0f, 0.0f };
const char str_802C5ABC[] = "MOBJ_Lv1Block";
const char str_802C5ACC[] = "MOBJ_Lv1BigBlock";
const char str_802C5AE0[] = "MOBJ_Lv2Block";
const char str_802C5AF0[] = "MOBJ_Lv2BigBlock";
const char str_802C5B04[] = "MOBJ_Lv3Block";
const char str_802C5B14[] = "MOBJ_Lv3BigBlock";
const char str_802C5B28[] = "p_kaiten_h";
const char str_802C5B34[] = "P_H_1C";
const char str_802C5B3C[] = "P_H_1B";
const char str_802C5B44[] = "P_H_1A";
const char str_802C5B4C[] = "M_H_3A";
const char str_802C5B54[] = "M_H_6A";
const char str_802C5B5C[] = "M_H_9A";
const char str_802C5B64[] = "M_H_3B";
const char str_802C5B6C[] = "M_H_6B";
const char str_802C5B74[] = "M_H_9B";
const f64 double_to_int_802c5b80 = 4503601774854144.0;

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
extern const f32 float_neg20_8041a9ac;
extern const f32 float_90_8041a9b0;
extern const f32 float_270_8041a9b4;
extern const f32 float_180_8041a9b8;
extern const f32 float_3p1416_8041a9bc;
extern const f32 float_4_8041a9c0;
extern const f32 float_14_8041a9c4;
extern const f32 float_1p2217_8041a9c8;
extern const f32 float_5_8041a9cc;
extern const f32 float_26_8041a9d0;
extern const f32 float_30_8041a9d4;
extern const f32 float_0p7854_8041a9d8;
extern const f32 float_36_8041a9dc;
extern const f32 float_20_8041a9e0;
extern const f32 float_34_8041a9e4;
extern const f32 float_37_8041a9e8;
extern const f32 float_13_8041a9ec;
extern const f32 float_0p01_8041a9f0;
extern const f32 float_neg1_8041a9f4;
extern const f32 float_neg2_8041a9f8;
extern const f32 float_38_8041a9fc;
extern const f32 float_15_8041aa00;
extern const f32 float_10_8041aa04;
const char str_8041AA08[] = "M_H_3";
const char str_8041AA10[] = "M_H_6";
const char str_8041AA18[] = "M_H_9";
extern const f32 float_6_8041aa20;
extern const f32 float_72_8041aa24;
extern const f32 float_0p4_8041aa28;
extern const f32 float_0p005_8041aa2c;
extern const f32 float_1p7_8041aa30;
extern const f32 float_54_8041aa34;
extern const f32 float_1_8041aa38;
extern const f32 float_42_8041aa3c;
extern const f32 float_0p3_8041aa40;
extern const f32 float_1p6_8041aa44;
extern const f32 float_18_8041aa48;
extern const f32 float_0p2_8041aa4c;
extern const f32 float_1p5_8041aa50;
extern const f32 float_24_8041aa54;
extern const f32 float_22_8041aa58;
const char str_8041AA5C[] = "M_S_1";
const char str_8041AA64[] = "M_I_Y";
extern const f32 float_neg30_8041aa6c;
const char str_8041AA70[] = "M_H_2";
const char str_8041AA78[] = "M_H_1";
const char str_8041AA80[] = "M_H_5";
const char str_8041AA88[] = "M_H_4";
const char str_8041AA90[] = "M_H_8";
const char str_8041AA98[] = "M_H_7";
extern const f32 float_16_8041aaa0;
extern const f32 float_53_8041aaa4;
extern const f32 float_25_8041aaa8;
extern const f32 float_neg3_8041aaac;
extern const f32 float_31_8041aab0;
extern const f32 float_neg6_8041aab4;
extern const f32 float_28_8041aab8;

// The original materialises dir3 into its own stack slot through a temp copy. A plain struct assignment is
// forwarded away by the compiler, so the copy is forced through a cast; the natural form is under #else.
HitEntry* hitEstimate(s32* type) {
    HitCheckQuery query;
    Vec pos;
    Vec dir3;
    Vec dir0;
    Vec dir1;
    Vec dir2;
    Vec dir3t;
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
    angle = mario->unk1A8;
    if (marioGetPtr()->dispFlags & 0x80000000) {
        off = float_neg20_8041a9ac;
    } else {
        off = float_0_8041a9a8;
    }
    if (angle >= float_90_8041a9b0 && angle <= float_270_8041a9b4) {
        angle = toMovedir2(float_180_8041a9b8, off);
    } else {
        angle = toMovedir2(float_0_8041a9a8, off);
    }
    sincosf(&sa, &ca, angle);
    boxY = mario->wPlayerCollisionBox.y;
    c = cos(float_3p1416_8041a9bc);
    pos = mario->position;
    pos.y = float_4_8041a9c0 + (mario->position.y + boxY);
    dir0 = vec3_802c5a8c;
    dir0.x = sa;
    dir0.y = -c;
    dir0.z = ca;
    query.targetPos = pos;
    query.targetDir = dir0;
    query.targetDistance = float_14_8041a9c4;
    hit = hitCheckVecFilter(&query, NULL);
    if (hit != NULL && (hitGetAttr(hit) & 0x800005) == 0) {
        *type = 7;
        return hit;
    }

    c = cos(float_1p2217_8041a9c8);
    pos.x = mario->position.x + sa * float_5_8041a9cc;
    pos.z = mario->position.z + ca * float_5_8041a9cc;
    pos.y = mario->position.y + boxY;
    dir1 = vec3_802c5a98;
    dir1.x = sa;
    dir1.y = -c;
    dir1.z = ca;
    query.targetPos = pos;
    query.targetDir = dir1;
    query.targetDistance = float_26_8041a9d0;
    hit = hitCheckVecFilter(&query, NULL);
    if (hit != NULL && (hitGetAttr(hit) & 0x800005) == 0) {
        *type = (query.hitPos.y - mario->position.y > float_30_8041a9d4) ? 4 : 5;
        return hit;
    }

    c = cos(float_0p7854_8041a9d8);
    pos.x = mario->position.x + sa * float_5_8041a9cc;
    pos.z = mario->position.z + ca * float_5_8041a9cc;
    pos.y = mario->position.y + boxY;
    dir2 = vec3_802c5aa4;
    dir2.x = sa;
    dir2.y = -c;
    dir2.z = ca;
    query.targetPos = pos;
    query.targetDir = dir2;
    query.targetDistance = float_30_8041a9d4;
    hit = hitCheckVecFilter(&query, NULL);
    if (hit != NULL && (hitGetAttr(hit) & 0x800005) == 0) {
        *type = 5;
        return hit;
    }

    c = cos(float_0_8041a9a8);
    dir3t = vec3_802c5ab0;
    dir3t.y = -c;
#ifdef MUST_MATCH
    *(Vec*)&dir3 = *(Vec*)&dir3t;
#else
    dir3 = dir3t;
#endif
    for (i = 0; i < 2; i++) {
        if (i == 0) {
            dist = float_36_8041a9dc;
            off2 = float_20_8041a9e0;
        } else {
            dist = float_34_8041a9e4;
            if (mario->dispFlags & 0x80000000) {
                off2 = float_37_8041a9e8;
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
            *type = (dy > float_13_8041a9ec) ? 5 : 6;
            return hit;
        }
    }
    return hit;
}

void mot_ham2_post(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2;

    if (mario->dispFlags & 0x40000000) {
        marioPaperOff();
    }
    mario2 = marioGetPtr();
    M_F32(mario2, 0x148) = float_0_8041a9a8;
    mario2->unk150.z = float_0p01_8041a9f0;
    marioPaperOff();
    marioPaperLightOn();
    mario->dispFlags &= ~0x100;
    mario->dispFlags &= ~0x8;
    mario->dispFlags &= ~0x10;
    mario->dispFlags &= ~0x4;
    mario->flags &= ~0x80;
    marioAdjustMoveDir();
}

f32 marioGetAngleKaitenHammer(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->currMotionId != MARIO_MOTION_HAMMER_2) {
        return float_neg1_8041a9f4;
    }
    if ((s32)mario->currSubMotionId != 0x14) {
        return float_neg2_8041a9f8;
    }
    return revise360(float_270_8041a9b4 - mario->unk1A8);
}

#pragma opt_propagation off // keep the effUltraHammerEntry and Vec-copy FP/load scheduling
// NONMATCHING (99.99%): the original's dead hitGetAttr mask comparison is optimized away by this
// compiler; a self-select ternary keeps it alive but leaves the first `if (hit != NULL)` branching
// to the second, redundant test instead of past the store (1 arg mismatch; all else matches).
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
#ifdef MUST_MATCH
    f32 px__;
    f32 py__;
    f32 pz__;
    f32 one__;
#endif
    s32 level;

    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1;
        mario->flags &= ~0x000F0000;
        M_S16(mario, 0x2B4) = 0;
        mario->currSubMotionId = 0;
        M_S32(mario, 0x2B8) = -1;
    }
    switch (mario->currSubMotionId) {
    case 0:
        mario->flags |= 0x80;
        mario->dispFlags |= 4;
        M_S8(mario, 0x41) = 0;
        M_S8(mario, 0x42) = 0;
        mario->baseSpeed = 0.0f;
        mario->airTimer = 0;
        mario->currSubMotionId = 1;
        mario->multiTimer = 0x1E;
        if (M_S8(mario, 0x42) == 3) {
            mario->wCamVal1 = 38.0f;
        } else if (M_S8(mario, 0x42) <= 2) {
            mario->wCamVal1 = 15.0f;
        } else {
            mario->wCamVal1 = 10.0f;
        }
        switch (pouchGetHammerLv()) {
        case 1:
            marioChgPose(str_8041AA08);
            break;
        case 2:
            marioChgPose(str_8041AA10);
            break;
        case 3:
            marioChgPose(str_8041AA18);
            break;
        }
        M_S32(mario, 0x200) = 0;
        mario->currSubMotionId = 1;
        mario->multiTimer = 1;
        break;
    case 1:
        if ((s32)--mario->multiTimer > 0) {
            break;
        }
        if (180.0f == M_F32(mario, 0x1AC) && M_S8(mario, 0x43) == 1) {
            mario->dispFlags |= 8;
        }
        if (0.0f == M_F32(mario, 0x1AC) && M_S8(mario, 0x43) == 0) {
            mario->dispFlags |= 0x10;
        }
        marioPaperOn(str_802C5B28);
        marioChgPaper(str_802C5B34);
        marioPaperLightOff();
        M_S32(mario, 0x2B8) = psndSFXOn_3D((const char*)0x162, &mario->position);
        M_F32(mario, 0x2B0) = 0.0f;
        mario->poseTime = 0;
        animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], M_F32(mario, 0x2B0));
        mario->currSubMotionId = 0xA;
        mario->multiTimer = 0x1E;
        /* fallthrough to case 0xA */
    case 0xA:
        if (marioGetRub(0x200, &rub1, &rub2, &rub3) == 1) {
            M_S16(mario, 0x2B4) = M_S16(mario, 0x2B4) & ~1;
            M_S8(mario, 0x41)++;
            if (M_S8(mario, 0x41) >= 0x14) {
                M_S8(mario, 0x41) = 0x14;
            }
        }
        if (M_S8(mario, 0x41) < 4) {
            goto level_low;
        }
        M_F32(mario, 0x2B0) += 6.0f;
        if (M_F32(mario, 0x2B0) >= 72.0f) {
            M_F32(mario, 0x2B0) = 72.0f;
        }
        mario->currSubMotionId = 0xB;
        marioChgPaper(str_802C5B3C);
        M_S8(mario, 0x42) = 3;
        m2 = marioGetPtr();
        M_F32(m2, 0x148) = 0.4f;
        M_F32(m2, 0x158) = 0.005f;
#ifdef MUST_MATCH
        *(volatile f32*)&m2->field_68 = 1.7f;
        *(volatile Vec*)&m2->field_6C = *(volatile Vec*)&vec3_802c5a80;
#else
        m2->field_68 = 1.7f;
        m2->field_6C = vec3_802c5a80;
#endif
        M_S16(mario, 0x2B4) |= 0x1000;
        goto common_6320;
    level_low:
        if (M_S8(mario, 0x41) < 3) {
            goto level_1;
        }
        marioChgPaper(str_802C5B44);
        level = M_S16(mario, 0x2B4);
        if (!(level & 1)) {
            M_F32(mario, 0x2B0) += 6.0f;
            if (M_F32(mario, 0x2B0) >= 54.0f) {
                M_F32(mario, 0x2B0) = 54.0f;
                M_S16(mario, 0x2B4) |= 1;
                M_S8(mario, 0x42) = 3;
                m2 = marioGetPtr();
                M_F32(m2, 0x148) = 0.4f;
                M_F32(m2, 0x158) = 0.005f;
#ifdef MUST_MATCH
                *(volatile f32*)&m2->field_68 = 1.7f;
                *(volatile Vec*)&m2->field_6C = *(volatile Vec*)&vec3_802c5a80;
#else
                m2->field_68 = 1.7f;
                m2->field_6C = vec3_802c5a80;
#endif
                M_S16(mario, 0x2B4) |= 0x1000;
            }
            goto common_6320;
        } else if (!(level & 0x10)) {
            M_F32(mario, 0x2B0) += 1.0f;
            if (M_F32(mario, 0x2B0) >= 54.0f) {
                M_F32(mario, 0x2B0) = 54.0f;
                M_S16(mario, 0x2B4) |= 0x10;
            }
            goto common_6320;
        } else {
            M_F32(mario, 0x2B0) -= 1.0f;
            if (M_F32(mario, 0x2B0) <= 42.0f) {
                M_F32(mario, 0x2B0) = 42.0f;
                M_S16(mario, 0x2B4) &= ~0x10;
            }
            goto common_6320;
        }
    level_1:
        if (M_S8(mario, 0x41) < 2) {
            goto level_0;
        }
        marioChgPaper(str_802C5B44);
        level = M_S16(mario, 0x2B4);
        if (!(level & 1)) {
            M_F32(mario, 0x2B0) += 6.0f;
            if (M_F32(mario, 0x2B0) >= 36.0f) {
                M_F32(mario, 0x2B0) = 36.0f;
                M_S16(mario, 0x2B4) |= 1;
                M_S8(mario, 0x42) = 2;
                m2 = marioGetPtr();
                M_F32(m2, 0x148) = 0.3f;
                M_F32(m2, 0x158) = 0.005f;
#ifdef MUST_MATCH
                *(volatile f32*)&m2->field_68 = 1.6f;
                *(volatile Vec*)&m2->field_6C = *(volatile Vec*)&vec3_802c5a80;
#else
                m2->field_68 = 1.6f;
                m2->field_6C = vec3_802c5a80;
#endif
                M_S16(mario, 0x2B4) |= 0x1000;
            }
            goto common_6320;
        } else if (!(level & 0x10)) {
            M_F32(mario, 0x2B0) += 1.0f;
            if (M_F32(mario, 0x2B0) >= 36.0f) {
                M_F32(mario, 0x2B0) = 36.0f;
                M_S16(mario, 0x2B4) |= 0x10;
            }
            goto common_6320;
        } else {
            M_F32(mario, 0x2B0) -= 1.0f;
            if (M_F32(mario, 0x2B0) <= 30.0f) {
                M_F32(mario, 0x2B0) = 30.0f;
                M_S16(mario, 0x2B4) &= ~0x10;
            }
            goto common_6320;
        }
    level_0:
        marioChgPaper(str_802C5B44);
        level = M_S16(mario, 0x2B4);
        if (!(level & 1)) {
            M_F32(mario, 0x2B0) += 6.0f;
            if (M_F32(mario, 0x2B0) >= 18.0f) {
                M_F32(mario, 0x2B0) = 18.0f;
                M_S16(mario, 0x2B4) |= 1;
                M_S8(mario, 0x42) = 1;
                m2 = marioGetPtr();
                M_F32(m2, 0x148) = 0.2f;
                M_F32(m2, 0x158) = 0.005f;
#ifdef MUST_MATCH
                *(volatile f32*)&m2->field_68 = 1.5f;
                *(volatile Vec*)&m2->field_6C = *(volatile Vec*)&vec3_802c5a80;
#else
                m2->field_68 = 1.5f;
                m2->field_6C = vec3_802c5a80;
#endif
                M_S16(mario, 0x2B4) |= 0x1000;
            }
            goto common_6320;
        } else if (!(level & 0x10)) {
            M_F32(mario, 0x2B0) += 1.0f;
            if (M_F32(mario, 0x2B0) >= 24.0f) {
                M_F32(mario, 0x2B0) = 24.0f;
                M_S16(mario, 0x2B4) |= 0x10;
            }
            goto common_6320;
        } else {
            M_F32(mario, 0x2B0) -= 1.0f;
            if (M_F32(mario, 0x2B0) <= 18.0f) {
                M_F32(mario, 0x2B0) = 18.0f;
                M_S16(mario, 0x2B4) &= ~0x10;
            }
            goto common_6320;
        }
    case 0xB:
        goto common_63C4;
    common_6320:
        if (M_S16(mario, 0x2B4) & 0x1000) {
            M_S16(mario, 0x2B4) &= ~0x1000;
            if (!(M_S16(mario, 0x2B4) & 0x2000)) {
                M_S16(mario, 0x2B4) |= 0x2000;
                switch (pouchGetHammerLv()) {
                case 1:
                    marioChgPose(str_802C5B4C);
                    break;
                case 2:
                    marioChgPose(str_802C5B54);
                    break;
                case 3:
                    marioChgPose(str_802C5B5C);
                    break;
                }
            }
        }
        if (strcmp(mario->paperAnimName, str_802C5B44) == 0) {
            animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], M_F32(mario, 0x2B0) / 6.0f);
        }
    common_63C4:
        if (M_U16(mario, 0x246) & 0x200) {
            break;
        }
        if ((u32)M_S32(mario, 0x2B8) != 0xFFFFFFFFu) {
            psndSFXOff(M_S32(mario, 0x2B8));
            M_S32(mario, 0x2B8) = -1;
        }
        m2 = marioGetPtr();
        M_F32(m2, 0x148) = 0.0f;
        M_F32(m2, 0x158) = 0.01f;
        switch (pouchGetHammerLv()) {
        case 1:
            marioChgPose(str_802C5B64);
            break;
        case 2:
            marioChgPose(str_802C5B6C);
            break;
        case 3:
            marioChgPose(str_802C5B74);
            break;
        }
        marioChgPaper(str_802C5B34);
        mario->currSubMotionId = 0x14;
        if (M_S8(mario, 0x42) == 3) {
            mario->multiTimer = 0x30;
        } else if (M_S8(mario, 0x42) == 2) {
            mario->multiTimer = 0x42;
        } else {
            mario->multiTimer = 0x48;
        }
        M_U16(mario, 0x2A4) = 0;
        if (M_S8(mario, 0x42) == 3) {
            mario->wCamVal1 = 38.0f;
        } else if (M_S8(mario, 0x42) == 2) {
            mario->wCamVal1 = 22.0f;
        } else if (M_S8(mario, 0x42) <= 1) {
            mario->wCamVal1 = 15.0f;
        }
#ifdef MUST_MATCH
        py__ = 20.0f; px__ = mario->position.x; py__ += mario->position.y;
        pz__ = mario->position.z; one__ = 1.0f;
        effUltraHammerEntry(M_S8(mario, 0x43) != 0, mario->multiTimer, px__, py__, pz__, one__);
#else
        effUltraHammerEntry(M_S8(mario, 0x43) != 0, mario->multiTimer, mario->position.x,
                            mario->position.y + 20.0f, mario->position.z, 1.0f);
#endif
        break;
    case 0x14:
        f31 = M_F32(mario, 0x1A8);
        M_S16(mario, 0x2A4) = 0xA;
        mario->dispFlags |= 0x100;
        f30 = revise360(270.0f - M_F32(mario, 0x1A8));
        m2 = marioGetPtr();
        sincosf(&sa, &ca, f30);
        dist = 2.0f * m2->wPlayerCollisionBox.x;
        hit = hitCheckFilter(NULL, &hx, &hy, &hz, &dist, &hnx, &hny, &hnz,
                             m2->position.x,
                             0.5f * m2->wPlayerCollisionBox.y + m2->position.y,
                             m2->position.z, sa, 0.0f, ca);
#ifdef MUST_MATCH
        // The original keeps the wall's attribute-mask comparison in the output even
        // though its result is never used (the branch on it was folded away).  A
        // self-select ternary on an unsigned copy of the pointer is the one shape this
        // compiler emits the comparison for while dropping the branch; without it the
        // whole comparison is dead-code-eliminated (as under #else).  The only residual
        // difference is that the original's first `if (hit != NULL)` branch lands on the
        // redundant second null test rather than on the end of the statement.
        {
            u32 hitBits = (u32)hit;
            if (hit != NULL && ((hitGetAttr(hit) & 0x800005) ? hitBits : hitBits)) {
                M_S32(mario, 0x1FC) = (s32)hit;
            }
        }
#else
        // The original tests the wall's attributes here and does nothing with the result.
        if (hit != NULL && (hitGetAttr(hit) & 0x800005)) {
        }
        if (hit != NULL) {
            M_S32(mario, 0x1FC) = (s32)hit;
        }
#endif
        f30 = M_F32(mario, 0x2A8);
        M_F32(mario, 0x1A8) += f30 * (M_S8(mario, 0x43) ? -1.0f : 1.0f);
        if ((s32)f31 / 360 != (s32)M_F32(mario, 0x1A8) / 360) {
            psndSFXOn_3D((const char*)0x163, &mario->position);
        }
        if (M_S16(mario, 0x2A4) == 1) {
            psndSFXOn_3D((const char*)0x163, &mario->position);
        }
        mario->multiTimer--;
        if (M_S8(mario, 0x42) > 2 && mario->multiTimer == 0xA) {
            marioPaperOff();
            marioPaperLightOn();
            marioChgPose(str_8041AA5C);
            mario->dispFlags &= ~4;
            mario->dispFlags &= ~8;
            mario->dispFlags &= ~0x10;
        }
        if (mario->multiTimer > 0) {
            break;
        }
        if (M_S8(mario, 0x42) <= 2) {
            mario->multiTimer = sysMsec2Frame(0x12C);
        } else {
            M_F32(mario, 0x1A8) = revise360(M_F32(mario, 0x1A8));
            marioAdjustMoveDir();
            mario->multiTimer = 0x1E;
        }
        mario->currSubMotionId = 0x16;
        /* fallthrough to case 0x16 */
    case 0x16:
        if ((s32)--mario->multiTimer > 0) {
            break;
        }
        if (M_S8(mario, 0x42) <= 2) {
            marioPaperOff();
            marioPaperLightOn();
            marioChgPose(str_8041AA5C);
            mario->dispFlags &= ~4;
            mario->dispFlags &= ~8;
            mario->dispFlags &= ~0x10;
            M_F32(mario, 0x1A8) = revise360(M_F32(mario, 0x1A8));
            marioAdjustMoveDir();
        }
        mario->dispFlags &= ~0x100;
        mario->currSubMotionId = 0x17;
        mario->multiTimer = 1;
        /* fallthrough to case 0x17 */
    case 0x17:
        if ((s32)--mario->multiTimer > 0) {
            break;
        }
        mario->multiTimer = 1;
        /* fallthrough to case 0x18 */
    case 0x18:
        if ((s32)--mario->multiTimer > 0) {
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
    if (!(M_U16(mario, 0x246) & 0x200)) {
        if (!(M_U16(mario, 0x246) & 0x100)) {
            if (strcmp(mario->animName, str_8041AA64) == 0 && mario->currMotionId == MARIO_MOTION_STAY) {
                marioChgPose(str_8041AA5C);
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
            marioChgPose(str_8041AA64);
            marioVoiceGlareOn();
        }
    }
    if (mario->wMotionTimer >= 50) {
        mario->wMotionTimer = 50;
        if (mario->currMotionId == MARIO_MOTION_STAY && !(mario->dispFlags & 0x40000000)) {
            marioChgPose(str_8041AA64);
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
    M_S8(mario, 0x43) = spC;
    return 1;
}

// NONMATCHING (88.29%): the directional/sfx blocks and clamp sequences differ in float register
// assignment and stack naming; the jump table and constants match.
void mot_hammer(void) {
    MarioWork* mario = marioGetPtr();
    MapObjectEntry* mobj;
    HitEntry* hit;
    s32 type;
    s32 r27;
    s32 r28;
    s32 i;
    s32 sound;
    f32 f30;
    f32 f31;
    f32 px;
    f32 pz;

    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1;
        mario->flags &= ~0x000F0000;
        mario->flags |= 0x80;
        f30 = M_F32(mario, 0x1A8);
        {
            MarioWork* m = marioGetPtr();
            if (m->dispFlags & 0x80000000) {
                f32 ang = m->unk1A8;
                s32 facing = 0;
                if (ang >= float_90_8041a9b0 && ang <= float_270_8041a9b4) {
                    facing = 1;
                }
                f31 = facing ? float_30_8041a9d4 : float_neg30_8041aa6c;
            } else {
                f31 = float_0_8041a9a8;
            }
        }
        if (f30 >= float_90_8041a9b0 && f30 <= float_270_8041a9b4) {
            M_F32(mario, 0x2A4) = revise360(float_180_8041a9b8 + f31);
        } else {
            M_F32(mario, 0x2A4) = revise360(f31);
        }
        mario->multiTimer = 0;
        mario->airTimer = 0;
        mario->currSubMotionId = 0;
        mario->baseSpeed = float_0_8041a9a8;
        M_S32(mario, 0x200) = 0;
        hit = hitEstimate(&type);
        M_S32(mario, 0x200) = (s32)hit;
        if (hit != NULL) {
            f30 = M_F32(mario, 0x1A8);
            if (marioGetPtr()->dispFlags & 0x80000000) {
                f31 = float_neg30_8041aa6c;
            } else {
                f31 = float_0_8041a9a8;
            }
            if (f30 >= float_90_8041a9b0 && f30 <= float_270_8041a9b4) {
                mario->directionView = toMovedir2(float_180_8041a9b8, f31);
            } else {
                mario->directionView = toMovedir2(float_0_8041a9a8, f31);
            }
            f30 = M_F32(mario, 0x1A8);
            {
                MarioWork* m = marioGetPtr();
                if (m->dispFlags & 0x80000000) {
                    f32 ang = m->unk1A8;
                    s32 facing = 0;
                    if (ang >= float_90_8041a9b0 && ang <= float_270_8041a9b4) {
                        facing = 1;
                    }
                    f31 = facing ? float_30_8041a9d4 : float_neg30_8041aa6c;
                } else {
                    f31 = float_0_8041a9a8;
                }
            }
            if (f30 >= float_90_8041a9b0 && f30 <= float_270_8041a9b4) {
                M_F32(mario, 0x2A4) = revise360(float_180_8041a9b8 + f31);
            } else {
                M_F32(mario, 0x2A4) = revise360(f31);
            }
        }
        if (hitGetAttr(hit) & 0x80000000) {
            mobj = mobjHitObjPtrToPtr(hit);
            r28 = 0;
            if (strcmp((char*)mobj + 0x15, str_802C5ABC) == 0 ||
                strcmp((char*)mobj + 0x15, str_802C5ACC) == 0) {
                if (pouchGetPtr()->hammerLevel >= 1) {
                    r28 = 1;
                }
            } else if (strcmp((char*)mobj + 0x15, str_802C5AE0) == 0 ||
                       strcmp((char*)mobj + 0x15, str_802C5AF0) == 0) {
                if (pouchGetPtr()->hammerLevel >= 2) {
                    r28 = 1;
                }
            } else if (strcmp((char*)mobj + 0x15, str_802C5B04) == 0 ||
                       strcmp((char*)mobj + 0x15, str_802C5B14) == 0) {
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
            marioChgPose(type == 7 ? str_8041AA70 : str_8041AA78);
            break;
        case 2:
            marioChgPose(type == 7 ? str_8041AA80 : str_8041AA88);
            break;
        case 3:
            marioChgPose(type == 7 ? str_8041AA90 : str_8041AA98);
            break;
        }
        if (type != 0) {
            s32 v = 3;
            if (type != 7) {
                v = type;
            }
            M_S32(mario, 0x2A0) = v;
            mario->flags |= 0x800;
        } else {
            mario->flags &= ~0x800;
        }
        mario = marioGetPtr();
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
        psndSFXOn_3D((const char*)sound, &mario->position);
        M_S32(mario, 0x2A8) = type;
    }
    M_F32(mario, 0x1A8) = (M_F32(mario, 0x2A4) >= float_90_8041a9b0 && M_F32(mario, 0x2A4) <= float_270_8041a9b4)
                              ? float_180_8041a9b8
                              : float_0_8041a9a8;
    M_F32(mario, 0x1AC) = M_F32(mario, 0x1A8);
    M_F32(mario, 0x19C) = toMovedir(M_F32(mario, 0x2A4));
    mario->directionView = M_F32(mario, 0x19C);
    r28 = 0;
    if (mario->flags & 0x800) {
        s32 a = M_S32(mario, 0x2A0);
        s32 b = mario->multiTimer;
        r27 = (b <= a) ? b : a;
        if (b == a) {
            r28 = 1;
        }
    } else {
        s32 b = mario->multiTimer;
        r27 = (b <= 10) ? b : 10;
        if (b == 10) {
            r28 = 1;
        }
    }
    mario->poseTime = r27;
    animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], (f32)r27);
    if (r28 != 0) {
        switch (pouchGetPtr()->hammerLevel) {
        case 1:
            evtEntry(lv1quake_evt, 0, 0);
            break;
        case 2:
            evtEntry(lv2quake_evt, 0, 0);
            break;
        case 3:
            evtEntry(lv3quake_evt, 0, 0);
            break;
        }
        mario->currSubMotionId = 1;
        M_S32(mario, 0x1FC) = M_S32(mario, 0x200);
        px = mario->position.x;
        f30 = mario->position.y;
        pz = mario->position.z;
        if (!(mario->dispFlags & 0x80000000)) {
            switch (M_S32(mario, 0x2A8)) {
            case 0:
                f31 = toMovedir2(M_F32(mario, 0x1AC), float_0_8041a9a8);
                movePos(&px, &pz, float_25_8041aaa8, f31);
                f31 = float_neg3_8041aaac;
                r27 = 0x15;
                break;
            case 4:
                f31 = toMovedir2(M_F32(mario, 0x1AC), float_0_8041a9a8);
                movePos(&px, &pz, float_30_8041a9d4, f31);
                f31 = float_30_8041a9d4;
                r27 = 0x16;
                break;
            case 5:
                f31 = toMovedir2(M_F32(mario, 0x1AC), float_0_8041a9a8);
                movePos(&px, &pz, float_31_8041aab0, f31);
                f31 = float_5_8041a9cc;
                r27 = 0x15;
                break;
            case 7:
                f31 = toMovedir2(M_F32(mario, 0x1AC), float_0_8041a9a8);
                movePos(&px, &pz, float_16_8041aaa0, f31);
                f31 = float_53_8041aaa4;
                r27 = 0x16;
                break;
            default:
                r27 = -1;
                break;
            }
        } else {
            switch (M_S32(mario, 0x2A8)) {
            case 0:
                f31 = toMovedir2(M_F32(mario, 0x1AC), float_neg6_8041aab4);
                movePos(&px, &pz, float_30_8041a9d4, f31);
                f31 = float_neg3_8041aaac;
                r27 = 0x15;
                break;
            case 4:
                f31 = toMovedir2(M_F32(mario, 0x1AC), float_neg6_8041aab4);
                movePos(&px, &pz, float_37_8041a9e8, f31);
                f31 = float_28_8041aab8;
                r27 = 0x16;
                break;
            case 5:
                f31 = toMovedir2(M_F32(mario, 0x1AC), float_neg6_8041aab4);
                movePos(&px, &pz, float_36_8041a9dc, f31);
                f31 = float_20_8041a9e0;
                r27 = 0x16;
                break;
            case 7:
                f31 = toMovedir2(M_F32(mario, 0x1AC), float_neg2_8041a9f8);
                movePos(&px, &pz, float_16_8041aaa0, f31);
                f31 = float_53_8041aaa4;
                r27 = 0x16;
                break;
            default:
                r27 = -1;
                break;
            }
        }
        if (r27 > 0) {
            effKemuTestEntry(r27, px, f30 + f31, pz, float_1_8041aa38);
        }
        mario = marioGetPtr();
        sound = 0x161;
        switch (pouchGetHammerLv()) {
        case 1:
            psndSFXOn_3D((const char*)0x15E, &mario->position);
            break;
        case 2:
            psndSFXOn_3D((const char*)0x15F, &mario->position);
            break;
        case 3:
            psndSFXOn_3D((const char*)0x160, &mario->position);
            break;
        }
        i = 0;
        while (i < 10) {
            if (pouchEquipCheckBadge(se_data[M_S8(mario, 0x3A)]) != 0) {
                s32 idx = M_S8(mario, 0x3A);
                sound = se_data[idx + 1];
                M_S8(mario, 0x3A) = idx + 2;
                if (M_S8(mario, 0x3A) >= 0xA) {
                    M_S8(mario, 0x3A) -= 0xA;
                }
                break;
            } else {
                M_S8(mario, 0x3A) = M_S8(mario, 0x3A) + 2;
                if (M_S8(mario, 0x3A) >= 0xA) {
                    M_S8(mario, 0x3A) -= 0xA;
                }
                i++;
            }
        }
        psndSFXOn_3D((const char*)sound, &mario->position);
    }
    mario->multiTimer++;
    if (mario->multiTimer > 0x14) {
        mario->flags &= ~0x80;
        marioChgMot(0);
    }
}

s32 marioGetHammerLevel(void) {
    return pouchGetPtr()->hammerLevel;
}
