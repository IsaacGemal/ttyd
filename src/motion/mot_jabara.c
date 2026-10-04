#include "motion/mot_jabara.h"
#include "dolphin/types.h"
#include "dolphin/mtx.h"
#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "mario/mario_pouch.h"
#include "mario/mario_sbr.h"
#include "motion/mot_stay.h"
#include "motion/mot_slit.h"
#include "mario/mario_hit.h"
#include "mario/mario_cam.h"
#include "mario/mario_kemuri.h"
#include "mario/mariost.h"
#include "driver/hitdrv.h"
#include "driver/animdrv.h"
#include "manager/evtmgr.h"
#include "evt/evt_cam.h"
#include "pmario_sound.h"
#include "system.h"
#include "party.h"
#include "memory.h"
#include "matching.h"
#include "math_ext.h"
// fabs/fabsf must lower to the FPU instruction, not a call to the C library.
#define fabs(x) __fabs(x)
#define fabsf(x) __fabsf(x)
#include <string.h>

// Backing struct for `mario->motStruct` while the jabara motion runs; the fields
// are written by swingMain and read here. Not described by any other header.
typedef struct JabaraWork {
    u32 flags;   // 0x0
    f32 unk4;    // 0x4
    f32 unk8;    // 0x8
    f32 unkC;    // 0xC
    f32 unk10;   // 0x10
    s32 unk14;   // 0x14
    f32 unk18;   // 0x18
    s32 unk1C;   // 0x1C
} JabaraWork;

// The original reaches all of the unit's rodata through one base register. Naming
// the string symbols (the natural form) lets the compiler establish that base; the
// match-only RDBL form reads the 1.5708 double through the same base register.
#define RSTR(off, sym) (sym)
#ifdef MUST_MATCH
#define RDBL(off, sym) (*(volatile const f64*)&(sym))
#else
#define RDBL(off, sym) (sym)
#endif

// Local declarations for functions whose headers don't exist yet. Each one is
// listed in REPORT.md so it can move to a header later.

// Raw accessors for MarioWork fields that are not named in include/mario/mario.h.
// Each offset is listed in REPORT.md.
#define MW_S8(m, off) (*(s8*)((u8*)(m) + (off)))
#define MW_U8(m, off) (*(u8*)((u8*)(m) + (off)))
#define MW_U16(m, off) (*(u16*)((u8*)(m) + (off)))
#define MW_S16(m, off) (*(s16*)((u8*)(m) + (off)))
#define MW_S32(m, off) (*(s32*)((u8*)(m) + (off)))
#define MW_U32(m, off) (*(u32*)((u8*)(m) + (off)))
#define MW_F32(m, off) (*(f32*)((u8*)(m) + (off)))
#define MW_VEC(m, off) (*(Vec*)((u8*)(m) + (off)))

// .rodata
const Vec vec3_802c5b88 = { 0.0f, 40.0f, 0.0f };
const Vec vec3_802c5b94 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5ba0 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5bac = { 0.0f, 0.0f, 0.0f };
const char str_802C5BB8[] = "M_J_1A";
const char str_802C5BC0[] = "p_jyabara";
const char str_802C5BCC[] = "PM_J_1B";
const char str_802C5BD4[] = "PM_J_3B";
const char str_802C5BDC[] = "PM_J_2B";
const char str_802C5BE4[] = "M_J_5A";
const char str_802C5BEC[] = "PM_J_1C";
const char str_802C5BF4[] = "M_J_1C";
const char str_802C5BFC[] = "PM_J_1D";
const char str_802C5C04[] = "eki_02";
const char str_802C5C0C[] = "PM_J_1F";
const char str_802C5C14[] = "PM_J_1G";
const char str_802C5C1C[] = "PM_J_1H";
__declspec(section ".rodata") const u32 gap_04_802C5C24_rodata = 0;

// .data trailing zero word after the two switch jump tables.
#pragma force_active on
#pragma explicit_zero_data on
__declspec(section ".data") u32 gap_05_8030E97C_data = 0;
#pragma explicit_zero_data reset
#pragma force_active reset
const f64 double_1p5708_802c5c28 = 1.5707963705062866;
const char str_802C5C30[] = "M_J_1B";

// .sdata2
const f32 float_40_8041aac0 = 40.0f;
const f32 float_0_8041aac4 = 0.0f;
const f32 float_0p1_8041aac8 = 0.1f;
const f32 float_180_8041aacc = 180.0f;
const f32 float_1p4_8041aad0 = 1.4f;
const f32 float_0p8_8041aad4 = 0.8f;
const f32 float_5_8041aad8 = 5.0f;
const f32 float_neg0p1_8041aadc = -0.1f;
const f32 float_0p18_8041aae0 = 0.18f;
const f32 float_24_8041aae4 = 24.0f;
const f32 float_2p4_8041aae8 = 2.4f;
const f32 float_neg0p4_8041aaec = -0.4f;
const f32 float_0p01_8041aaf0 = 0.01f;
const f32 float_20_8041aaf4 = 20.0f;
const f32 float_37_8041aaf8 = 37.0f;
const f32 float_0p2_8041aafc = 0.2f;
const f32 float_0p005_8041ab00 = 0.005f;
const f32 float_1p5_8041ab04 = 1.5f;
const f32 float_0p4_8041ab08 = 0.4f;
const f32 float_1p7_8041ab0c = 1.7f;
const f32 float_0p3_8041ab10 = 0.3f;
const f32 float_1p6_8041ab14 = 1.6f;
const f32 float_480_8041ab18 = 480.0f;
const f32 float_50_8041ab1c = 50.0f;
const f32 float_25_8041ab20 = 25.0f;
const f32 float_200_8041ab24 = 200.0f;
const f32 float_10p45_8041ab28 = 10.45f;
const f32 float_0p278_8041ab2c = 0.278f;
const f32 float_neg0p031_8041ab30 = -0.031f;
const f32 float_8_8041ab34 = 8.0f;
const f32 float_neg0p84_8041ab38 = -0.84f;
const f32 float_0p08_8041ab3c = 0.08f;
const f32 float_neg0p015_8041ab40 = -0.015f;
const f32 float_4p3_8041ab44 = 4.3f;
const f32 float_0p5_8041ab48 = 0.5f;
const f32 float_100_8041ab4c = 100.0f;
const f32 float_3p5_8041ab50 = 3.5f;
const f32 float_9_8041ab54 = 9.0f;
const f32 float_neg6_8041ab58 = -6.0f;
const f32 float_neg450_8041ab5c = -450.0f;
const f32 float_neg190_8041ab60 = -190.0f;
const f32 float_neg8_8041ab64 = -8.0f;
const f32 float_4_8041ab68 = 4.0f;
const f32 float_2_8041ab6c = 2.0f;
__declspec(section ".sdata2") const char str_8041AB70[8] = "M_S_1";
const f32 float_0p6_8041ab78 = 0.6f;
const f32 float_neg0p6_8041ab7c = -0.6f;
const f32 float_3p1416_8041ab80 = 3.1415927f;
const f32 float_neg1_8041ab84 = -1.0f;
__declspec(section ".sdata2") const char str_8041AB88[8] = "M_S_4";
const f32 float_10_8041ab90 = 10.0f;
const f32 float_1_8041ab94 = 1.0f;
__declspec(section ".sdata2") const char str_8041AB98[8] = "M_W_6";
const f32 float_90_8041aba0 = 90.0f;
const f32 float_270_8041aba4 = 270.0f;
const f32 float_neg0p5_8041aba8 = -0.5f;
__declspec(section ".sdata2") const char str_8041ABAC[8] = "M_I_Y";
const u32 gap_09_8041ABB4_sdata2 = 0;

// Local quake script table (`.sdata`), referenced by mot_jabara.
static s32 quake_evt[8] = { 0x0005005B, (s32)evt_cam_shake, 4, (s32)0xF24A7A80,
                            (s32)0xF24A7A8F, 0xF0, 2, 1 };

s32 marioGetJabaraState(void) {
    s32 state = 7;
    MarioWork* mario = marioGetPtr();

    if (mario->currMotionId != MARIO_MOTION_JABARA) {
        state = 0;
    } else {
        s32 sub = mario->currSubMotionId;
        if (sub == 0x14) {
            state = 1;
        } else if (sub >= 0x46 && sub <= 0x47) {
            state = 2;
        } else if (sub == 0x50) {
            state = 3;
        } else if (sub >= 0x28 && sub <= 0x2B) {
            state = 4;
        } else if (sub >= 0x32 && sub <= 0x34) {
            state = 5;
        } else if (sub >= 0x1E && sub <= 0x1F) {
            state = 6;
        }
    }
    return state;
}

s32 marioPriCheckJabara(void) {
    MarioWork* mario = marioGetPtr();

    if ((mario->flags & MARIO_FLAG_IS_8_BIT) || (mario->flags & MARIO_FLAG_PAPER_MODE)) {
        mario->wMotionTimer = 0;
        return 0;
    }
    return pouchCheckItem(8) != 0;
}

BOOL marioChkJabara(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2 = marioGetPtr();
    s32 check;
    s32 out2;
    s32 out1;
    f32 out3;

#ifdef MUST_MATCH
    // Chained assignment keeps `check` and wMotionTimer in the same register (r0);
    // the inverted equality makes 1 the out-of-line branch, as in the original.
    if ((mario2->flags & MARIO_FLAG_IS_8_BIT) || (mario2->flags & MARIO_FLAG_PAPER_MODE)) {
        mario2->wMotionTimer = check = 0;
    } else {
        if ((s32)pouchCheckItem(8) == 0) {
            check = 0;
        } else {
            check = 1;
        }
    }
#else
    if ((mario2->flags & MARIO_FLAG_IS_8_BIT) || (mario2->flags & MARIO_FLAG_PAPER_MODE)) {
        mario2->wMotionTimer = 0;
        check = 0;
    } else if ((s32)pouchCheckItem(8) != 0) {
        check = 1;
    } else {
        check = 0;
    }
#endif
    if (check == 0) {
        return FALSE;
    }
    if (!(MW_U16(mario, 0x246) & 0x100)) {
        if (!(MW_U16(mario, 0x246) & 0x200)) {
            if (strcmp(mario->animName, str_8041ABAC) == 0 && mario->currMotionId == 0) {
                marioChgPose(str_8041AB70);
                marioVoiceGlareOff();
            }
            mario->wMotionTimer = 0;
        }
        return FALSE;
    }
    mario->wMotionTimer = mario->wMotionTimer + 1;
    if (mario->controlStickSensitivity != MATCH_F32(float_0_8041aac4)) {
        if (mario->wMotionTimer > 10 && mario->currMotionId == 0 && !(mario->dispFlags & 0x40000000)) {
            marioChgPose(str_8041ABAC);
            marioVoiceGlareOn();
        }
    }
    if (mario->wMotionTimer >= 0x32) {
        mario->wMotionTimer = 0x32;
        if (mario->currMotionId == 0 && !(mario->dispFlags & 0x40000000)) {
            marioChgPose(str_8041ABAC);
            marioVoiceGlareOn();
        }
    }
    if (marioGetRub(0x100, &out1, &out2, &out3) == 0) {
        return FALSE;
    }
    return out2 >= 1;
}

// The original keeps one register (r30) as both the `.rodata` base and the `vec`
// pointer; here the compiler copies `vec` into a second callee-saved register
// (`addi r29, r30, 0x0`), so the frame is `stwu r1,-0x100` + `stmw r27` where the
// original is `stwu r1,-0xf0` + four `stw r28..r31` (and `lmw`/`lwz r0,0xf4(r1)` in
// the mirror). Every string/Vec read then comes out as `0x..(r29)` where the
// original has `0x..(r30)`, and the allocator shifts mario2 (ours r28, original
// r29) and var_r28 (ours r27, original r28) down a register. Expressing the Vecs
// with the named `vec3_*` symbols frees r29 and restores the original frame, but
// makes MWCC hoist the three-word copies out of the original
// load/load/store/store/load/store order.
// NONMATCHING (98.86%): only the prologue/epilogue frame differs.
#pragma opt_common_subs off // keep the 0x2BA state byte sign-extended again for the second compare
void mot_jabara(void) {
    const Vec* vec = &vec3_802c5b88;
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2;
    HitEntry* hit;
    JabaraWork* jw;
    f32 f31, f30, f29, f28;
    Vec sp74;
    Vec sp68;
    Vec sp50;
    Vec sp5C;
    s32 sp4C, sp48;
    f32 sp44;
    s32 sp40;
    f32 sp3C, sp38;
    f32 sp34;
    s32 sp30;
    f32 sp2C;
    f32 sp28, sp24, sp20, sp1C, sp18, sp14, sp10;
    f32 spC, sp8;
    s32 var_r28;
    s32 var_sp40;
    f32 var_f28;

    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1u;
        MW_S32(mario, 0x2A0) = 0;
        MW_S32(mario, 0x2A4) = 0;
        MW_S32(mario, 0x2A8) = 0;
        MW_S32(mario, 0x2AC) = 0;
        MW_S32(mario, 0x2B0) = 0;
        MW_S32(mario, 0x2B4) = 0;
        MW_S32(mario, 0x2B8) = 0;
        if (mario->dispFlags & (1 << 24)) {
            mot_slit_post();
        }
        mario->flags &= 0xFFF0FFFF;
        mario->currSubMotionId = 0;
        jw = (JabaraWork*)__memAlloc(HEAP_DEFAULT, 0x20);
        marioGetPtr()->motStruct = jw;
        memset(marioGetPtr()->motStruct, 0, 0x20);
    }
    MW_S32(mario, 0x1F4) = 0;

    switch (mario->currSubMotionId) {
    case 0:
        mario->dispFlags |= 0x100;
        MW_U8(mario, 0x41) = 1;
        MW_U8(mario, 0x42) = 0;
        mario->airTimer = 0;
        MW_S16(mario, 0x52) = 0;
        mario->baseSpeed = 0.0f;
        mario->currSubMotionId = 0xA;
        marioChgPose(RSTR(0x30, str_802C5BB8));
        marioPaperOn(RSTR(0x38, str_802C5BC0));
        marioChgPaper(RSTR(0x44, str_802C5BCC));
        MW_VEC(mario, 0x11C) = mario->position;
        marioChkGnd();
        if (mario->currMotionId != MARIO_MOTION_JABARA) {
            mario->currSubMotionId = 0x64;
        }
        goto block_72;
    case 10:
        if (mario->currMotionId != MARIO_MOTION_JABARA) {
            mario->currSubMotionId = 0x64;
            goto block_72;
        }
        L_allPartyRideOn();
        psndSFXOn_3D((const char*)0x182, &mario->position);
        mario2 = marioGetPtr();
        mario2->unk148 = 0.2f;
        mario2->unk150.z = 0.005f;
        mario2->field_68 = 1.5f;
        mario2->field_6C = (&vec3_802c5b88)[0];
        mario->multiTimer = 8;
        mario->currSubMotionId = 0xB;
        /* fallthrough */
    case 11:
        if (--mario->multiTimer <= 0) {
            mario->currSubMotionId = 0xC;
            if (!(((JabaraWork*)marioGetPtr()->motStruct)->flags & 0x10000)) {
                ((JabaraWork*)marioGetPtr()->motStruct)->flags |= 0x10000;
            }
        }
        /* fallthrough */
    case 12:
        sp3C = marioChkOverheadJabara(&sp40, &sp38);
        if (marioGetRub(0x100, &sp48, &sp4C, &sp44) == 1) {
            MW_U8(mario, 0x41) = MW_U8(mario, 0x41) + 1;
            if ((s8)MW_U8(mario, 0x41) >= 0x14) {
                MW_U8(mario, 0x41) = 0x14;
            }
        }
        {
            s8 jt = MW_S8(mario, 0x41);
            if (jt == 3) {
                if (!(((JabaraWork*)marioGetPtr()->motStruct)->flags & 0x40000)) {
                    ((JabaraWork*)marioGetPtr()->motStruct)->flags |= 0x40000;
                    psndSFXOn_3D((const char*)0x184, &mario->position);
                }
                marioChgPaper(RSTR(0x4C, str_802C5BD4));
                MW_U8(mario, 0x42) = 3;
                mario2 = marioGetPtr();
                mario2->unk148 = 0.4f;
                mario2->unk150.z = 0.005f;
                mario2->field_68 = 1.7f;
                mario2->field_6C = (&vec3_802c5b88)[0];
            } else if (jt == 2) {
                if (!(((JabaraWork*)marioGetPtr()->motStruct)->flags & 0x20000)) {
                    ((JabaraWork*)marioGetPtr()->motStruct)->flags |= 0x20000;
                    psndSFXOn_3D((const char*)0x183, &mario->position);
                }
                marioChgPaper(RSTR(0x54, str_802C5BDC));
                MW_U8(mario, 0x42) = 2;
                mario2 = marioGetPtr();
                mario2->unk148 = 0.3f;
                mario2->unk150.z = 0.005f;
                mario2->field_68 = 1.6f;
                mario2->field_6C = (&vec3_802c5b88)[0];
            }
        }
        marioChkGnd();
        if (mario->currMotionId != MARIO_MOTION_JABARA) {
            mario->currSubMotionId = 0x64;
            return;
        }
        if ((f32)MW_S32(mario, 0x16C) > 480.0f || (f32)MW_S32(mario, 0x16C) < 50.0f) {
            camFollowYOn();
        }
        if (mario->position.y == MW_F32(mario, 0x114)) {
            camFollowYOff();
        }
        sp74 = mario->position;
        sp34 = 25.0f;
        var_r28 = 0;
        hit = (HitEntry*)marioSearchHead(&sp74, &sp34, 200.0f);
        if (hit != NULL) {
            if (!(MW_U16(mario, 0x246) & 0x100)) {
                ((JabaraWork*)marioGetPtr()->motStruct)->unk1C++;
                if ((s32)((JabaraWork*)marioGetPtr()->motStruct)->unk1C > sysMsec2Frame(0x1388)) {
                    var_r28 = 1;
                }
            }
            if (var_r28 != 0) {
                goto block_37;
            }
        } else {
        block_37:
            if (var_r28 != 0 || !(MW_U16(mario, 0x246) & 0x100) || (MW_U16(mario, 0x24C) & 0x100)) {
                mario2 = marioGetPtr();
                mario2->unk148 = 0.0f;
                mario2->unk150.z = 0.01f;
                camFollowYOff();
                marioChgPose(RSTR(0x5C, str_802C5BE4));
                marioChgPaper(RSTR(0x64, str_802C5BEC));
                psndSFXOn_3D((const char*)0x20185, &mario->position);
                psndSFXOn_3D((const char*)0x200B5, &mario->position);
                {
                    s8 jt = MW_S8(mario, 0x42);
                    if (jt == 3) {
                        MW_F32(mario, 0x7C) = 10.45f;
                        MW_F32(mario, 0x80) = 0.278f;
                        MW_F32(mario, 0x84) = -0.031f;
                        MW_F32(mario, 0x88) = 0.0f;
                    } else if (jt == 2) {
                        MW_F32(mario, 0x7C) = 8.0f;
                        MW_F32(mario, 0x80) = -0.84f;
                        MW_F32(mario, 0x84) = 0.08f;
                        MW_F32(mario, 0x88) = -0.015f;
                    } else {
                        MW_F32(mario, 0x7C) = 4.3f;
                        MW_F32(mario, 0x80) = -0.84f;
                        MW_F32(mario, 0x84) = 0.08f;
                        MW_F32(mario, 0x88) = -0.015f;
                    }
                }
                if (MW_S8(mario, 0x42) == 3) {
                    unk_80130374();
                }
                mario->hitobjJumpFrom = mario->hitobjStandOn;
                mario->hitobjStandOn = NULL;
                MW_F32(mario, 0x2A4) = MW_F32(mario, 0x1AC);
                mario->currSubMotionId = 0x14;
            }
        }
        goto block_72;
    case 20:
        marioMakeJumpPara();
        if (MW_S8(mario, 0x42) == 3) {
            sp3C = marioChkOverheadJabara(&sp40, &sp38);
        } else {
            sp3C = marioChkOverhead();
        }
        if ((HitEntry*)MW_S32(mario, 0x1F4) == NULL) {
            if (MW_F32(mario, 0x7C) <= 0.0f) {
                mario->currSubMotionId = 0x1E;
                marioSetPeakYpos();
            }
            MW_F32(mario, 0x90) = MW_F32(mario, 0x90) + MW_F32(mario, 0x7C);
            goto block_72;
        }
        f31 = sp3C;
        if (f31 < MW_F32(mario, 0x120)) {
            marioPaperOff();
            hit = (HitEntry*)mario->hitobjStandOn;
            f30 = MW_F32(mario, 0x120);
            mario2 = marioGetPtr();
            mario2->airTimer = 0;
            MW_S16(mario2, 0x52) = 0;
            mario2->hitobjStandOn = hit;
            mario2->hitobjJumpFrom = NULL;
            MW_F32(mario2, 0x90) = f30;
            mario2->flags &= 0xFFF0FFFF;
            camFollowYOn();
            marioGetPtr();
            allPartyRideOff();
            mario2 = marioGetPtr();
            mario2->unk1A8 = MW_F32(mario2, 0x2A4);
            MW_F32(mario2, 0x1AC) = MW_F32(mario2, 0x2A4);
            MW_VEC(mario2, 0xB0) = vec[1];
            MW_VEC(mario2, 0x98) = vec[2];
            MW_VEC(mario2, 0xBC) = vec[3];
            mario2->wPlayerCollisionBox.x = 20.0f;
            mario2->wPlayerCollisionBox.y = 37.0f;
            marioAdjustMoveDir();
            mario2->dispFlags &= ~4;
            mario2->dispFlags &= ~0x100;
            marioChgMotSub(0, 0);
            if (marioGetPtr()->motStruct != NULL) {
                __memFree(HEAP_DEFAULT, marioGetPtr()->motStruct);
                marioGetPtr()->motStruct = NULL;
                return;
            }
        } else {
            MW_F32(mario, 0x90) = f31;
            if (MW_S8(mario, 0x42) <= 2) {
                mario->currSubMotionId = 0x1E;
            } else if (hitGetAttr((HitEntry*)MW_S32(mario, 0x1F4)) & 0x40000) {
                MW_F32(mario, 0x2A4) = MW_F32(mario, 0x1AC);
                MW_F32(mario, 0x1A8) = MW_F32(mario, 0x1AC);
                mario->currSubMotionId = 0x46;
                mario->multiTimer = 0xA;
                MW_S32(mario, 0x2A8) = (s32)(sp3C + MW_F32(mario, 0x1B8));
                hit = (HitEntry*)MW_S32(mario, 0x1F4);
                marioGetPtr();
                hitObjGetPos(hitGetName(hit), &sp50);
                MW_F32(mario, 0x94) = (f32)(s32)((100.0f * sp50.z + 0.5f) / 100.0f);
                f30 = (f32)MW_S32(mario, 0x2A8);
                f28 = f30 - MW_F32(mario, 0x1B8);
                MW_F32(mario, 0x90) = f28 - 3.5f;
                MW_F32(mario, 0xB4) = 9.0f + MW_F32(mario, 0x1B8);
                marioPaperOff();
                camFollowYOn();
                mario->dispFlags |= 4;
                marioChgPose(RSTR(0x5C, str_802C5BE4));
                psndSFXOn_3D((const char*)0x186, &mario->position);
                var_sp40 = sp40;
                mario2 = marioGetPtr();
                MW_S16(mario2, 0x2B8) = 4;
                MW_F32(mario2, 0x2B0) = 24.0f;
                ((JabaraWork*)marioGetPtr()->motStruct)->unk4 = 2.4f;
                ((JabaraWork*)marioGetPtr()->motStruct)->unk8 = -0.4f;
                ((JabaraWork*)marioGetPtr()->motStruct)->unk10 = 0.18f;
                if (var_sp40 < 0) {
                    MW_S8(mario2, 0x2BA) = 0xB;
                } else {
                    MW_S8(mario2, 0x2BA) = 1;
                }
            } else {
                mario->currSubMotionId = 0x28;
            }
            goto block_72;
        }
        break;
    case 30:
        marioChgPose(RSTR(0x6C, str_802C5BF4));
        marioChgPaper(RSTR(0x74, str_802C5BFC));
        marioSetFallPara();
        mario->currSubMotionId = 0x1F;
        /* fallthrough */
    case 31:
        var_f28 = marioGetFallSpd();
        if (var_f28 < -6.0f) {
            var_f28 = -6.0f;
        }
        hit = (HitEntry*)marioChkLandon(&sp3C, var_f28);
        if (hit == NULL) {
            MW_F32(mario, 0x90) = MW_F32(mario, 0x90) + var_f28;
            goto block_72;
        }
        marioPaperOff();
        f30 = sp3C;
        mario2 = marioGetPtr();
        mario2->airTimer = 0;
        MW_S16(mario2, 0x52) = 0;
        mario2->hitobjStandOn = hit;
        mario2->hitobjJumpFrom = NULL;
        MW_F32(mario2, 0x90) = f30;
        mario2->flags &= 0xFFF0FFFF;
        camFollowYOn();
        marioGetPtr();
        allPartyRideOff();
        mario2 = marioGetPtr();
        mario2->unk1A8 = MW_F32(mario2, 0x2A4);
        MW_F32(mario2, 0x1AC) = MW_F32(mario2, 0x2A4);
        MW_VEC(mario2, 0xB0) = vec[1];
        MW_VEC(mario2, 0x98) = vec[2];
        MW_VEC(mario2, 0xBC) = vec[3];
        mario2->wPlayerCollisionBox.x = 20.0f;
        mario2->wPlayerCollisionBox.y = 37.0f;
        marioAdjustMoveDir();
        mario2->dispFlags &= ~4;
        mario2->dispFlags &= ~0x100;
        marioChgMotSub(0, 0);
        if (marioGetPtr()->motStruct != NULL) {
            __memFree(HEAP_DEFAULT, marioGetPtr()->motStruct);
            marioGetPtr()->motStruct = NULL;
            return;
        }
        break;
    default:
        goto block_72;
    }

block_72:
    switch (mario->currSubMotionId) {
    case 0x28:
        if (strcmp(gp->currentMapName, RSTR(0x7C, str_802C5C04)) == 0) {
            if (mario->position.x >= -450.0f && mario->position.x <= -190.0f) {
                mario->flags |= 0x400;
            }
        }
        psndSFXOn_3D((const char*)0x188, &mario->position);
        evtEntryType(quake_evt, 0, 0, 0);
        marioClearJumpPara();
        marioChgPose(RSTR(0x5C, str_802C5BE4));
        marioChgPaper(RSTR(0x84, str_802C5C0C));
        MW_U8(mario, 0x2BB) = 1;
        mario->poseTime = MW_S8(mario, 0x2BB);
        MW_F32(mario, 0x9C) = -8.0f;
        mario->multiTimer = 0x32;
        mario->currSubMotionId = 0x29;
        goto block_121;
    case 0x29:
        animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], (f32)(s8)MW_U8(mario, 0x2BB));
        MW_U8(mario, 0x2BB) = MW_U8(mario, 0x2BB) + 1;
        if ((s8)MW_U8(mario, 0x2BB) >= 8) {
            MW_U8(mario, 0x2BB) = 8;
            MW_F32(mario, 0x1B8) = 4.0f;
        }
        if (--mario->multiTimer <= 0) {
            mario->currSubMotionId = 0x2A;
        }
        goto block_121;
    case 0x2A:
        MW_F32(mario, 0x9C) = MW_F32(mario, 0x9C) + MATCH_F32(float_2_8041ab6c);
        if (MW_F32(mario, 0x9C) >= 0.0f) {
            MW_F32(mario, 0x9C) = 0.0f;
        }
        animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], (f32)(s8)MW_U8(mario, 0x2BB));
        {
            s8 b = MW_S8(mario, 0x2BB) + 1;
            MW_U8(mario, 0x2BB) = b;
            if (b >= 0x10) {
                MW_U8(mario, 0x2BB) = 0x10;
                marioChgPaper(RSTR(0x8C, str_802C5C14));
                marioPaperLightOff();
                mario->currSubMotionId = 0x2B;
                mario->multiTimer = 0x18;
            }
        }
        goto block_121;
    case 0x2B:
        if (--mario->multiTimer > 0) {
            goto block_121;
        }
        marioChgPose(str_8041AB70);
        marioChgPaper(RSTR(0x94, str_802C5C1C));
        marioPaperLightOff();
        mario->currSubMotionId = 0x32;
        MW_U8(mario, 0x2BB) = 0;
        ((JabaraWork*)marioGetPtr()->motStruct)->unkC = 0.0f;
        goto do_32;
    case 0x32:
    do_32:
        animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], (f32)(s8)MW_U8(mario, 0x2BB));
            var_r28 = 0;
            {
                s8 b = MW_S8(mario, 0x2BB);
                if (b >= 0x23 && b <= 0x62) {
                    if (MW_F32(mario, 0x1AC) == 180.0f) {
                        var_f28 = 0.6f;
                    } else {
                        var_f28 = -0.6f;
                    }
                    if (var_f28 != ((JabaraWork*)marioGetPtr()->motStruct)->unk18) {
                        ((JabaraWork*)marioGetPtr()->motStruct)->unk14++;
                        var_r28 = 1;
                    }
                    ((JabaraWork*)marioGetPtr()->motStruct)->unk18 = var_f28;
                } else {
                    if (MW_F32(mario, 0x1AC) == 180.0f) {
                        var_f28 = -0.6f;
                    } else {
                        var_f28 = 0.6f;
                    }
                    if (var_f28 != ((JabaraWork*)marioGetPtr()->motStruct)->unk18) {
                        ((JabaraWork*)marioGetPtr()->motStruct)->unk14++;
                        var_r28 = 1;
                    }
                    ((JabaraWork*)marioGetPtr()->motStruct)->unk18 = var_f28;
                }
            }
            if (var_r28 != 0) {
                if (((JabaraWork*)marioGetPtr()->motStruct)->unk14 == 1) {
                    psndSFXOn_3D((const char*)0x189, &mario->position);
                } else if (((JabaraWork*)marioGetPtr()->motStruct)->unk14 == 2) {
                    psndSFXOn_3D((const char*)0x18A, &mario->position);
                } else if (((JabaraWork*)marioGetPtr()->motStruct)->unk14 == 3) {
                    psndSFXOn_3D((const char*)0x18B, &mario->position);
                }
            }
            f29 = (f32)cos((3.1415927f * MW_F32(mario, 0x198)) / 180.0f);
            MW_F32(mario, 0x98) = var_f28 * f29 + MW_F32(mario, 0x98);
            f30 = (f32)sin((3.1415927f * MW_F32(mario, 0x198)) / 180.0f);
            MW_F32(mario, 0xA0) = var_f28 * f30 + MW_F32(mario, 0xA0);
            var_f28 = -1.0f * (f32)sin(RDBL(0xA0, double_1p5708_802c5c28));
            hit = (HitEntry*)marioChkLandon2(&sp3C, var_f28, MW_F32(mario, 0x9C));
            if (hit == NULL) {
                MW_F32(mario, 0x90) = MW_F32(mario, 0x90) + var_f28;
                if ((f32)MW_S32(mario, 0x16C) > 480.0f) {
                    camFollowYOn();
                }
                {
                    s32 t = MW_U8(mario, 0x2BB) + 1;
                    MW_U8(mario, 0x2BB) = t;
                    if ((s8)t > 0x78) {
                        MW_U8(mario, 0x2BB) = 0;
                    }
                }
                if (MW_U16(mario, 0x248) & 0x200) {
                    mario->flags |= 0x80;
                    mario->flags &= ~0x400;
                    marioPaperOff();
                    marioGetPtr();
                    allPartyRideOff();
                    mario2 = marioGetPtr();
                    mario2->unk1A8 = MW_F32(mario2, 0x2A4);
                    MW_F32(mario2, 0x1AC) = MW_F32(mario2, 0x2A4);
                    MW_VEC(mario2, 0xB0) = vec[1];
                    MW_VEC(mario2, 0x98) = vec[2];
                    MW_VEC(mario2, 0xBC) = vec[3];
                    mario2->wPlayerCollisionBox.x = 20.0f;
                    mario2->wPlayerCollisionBox.y = 37.0f;
                    marioAdjustMoveDir();
                    mario2->dispFlags &= ~4;
                    mario2->dispFlags &= ~0x100;
                    marioChgMotSub(0, 0);
                    if (marioGetPtr()->motStruct != NULL) {
                        __memFree(HEAP_DEFAULT, marioGetPtr()->motStruct);
                        marioGetPtr()->motStruct = NULL;
                        return;
                    }
                } else {
                    goto block_121;
                }
            } else {
                mario->hitobjStandOn = hit;
                mario->hitobjJumpFrom = NULL;
                MW_F32(mario, 0x90) = sp3C;
                mario->currSubMotionId = 0x34;
                MW_U8(mario, 0x2BB) = 0;
                mario->multiTimer = 0x14;
                goto block_121;
            }
        break;
    case 0x34:
        animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], (f32)(s8)MW_U8(mario, 0x2BB));
        if (--mario->multiTimer <= 0) {
            mario->currSubMotionId = 0x3C;
        }
        goto block_121;
    }

block_121:
    {
        s32 sm = mario->currSubMotionId;
        switch (sm) {
        case 0x3C:
            allPartyRideOff();
            mario->currSubMotionId = 0x3D;
            mario->hitobjJumpFrom = mario->hitobjStandOn;
            mario->hitobjStandOn = NULL;
            MW_F32(mario, 0x180) = 0.0f;
            mario->flags &= ~0x400;
            mario2 = marioGetPtr();
            mario2->unk1A8 = MW_F32(mario2, 0x2A4);
            MW_F32(mario2, 0x1AC) = MW_F32(mario2, 0x2A4);
            MW_VEC(mario2, 0xB0) = vec[1];
            MW_VEC(mario2, 0x98) = vec[2];
            MW_VEC(mario2, 0xBC) = vec[3];
            mario2->wPlayerCollisionBox.x = 20.0f;
            mario2->wPlayerCollisionBox.y = 37.0f;
            marioAdjustMoveDir();
            mario2->dispFlags &= ~4;
            mario2->dispFlags &= ~0x100;
            if (marioGetPtr()->motStruct != NULL) {
                __memFree(HEAP_DEFAULT, marioGetPtr()->motStruct);
                marioGetPtr()->motStruct = NULL;
            }
            marioPaperOff();
            camFollowYOff();
            marioChgSmallJumpMotion();
            MW_F32(mario, 0x180) = 0.0f;
            return;
        }
        if (sm < 0x3C || sm >= 0x46) {
            switch (sm) {
                case 0x46:
                    {
                        s32 t = mario->multiTimer;
                        mario->multiTimer = t - 1;
                        if (t <= 0) {
                            marioChgPose(str_8041AB88);
                            mario->currSubMotionId = 0x47;
                        }
                    }
                    /* fallthrough */
                case 0x47:
                    {
                        s8 st = MW_S8(mario, 0x2BA);
                        if ((st == 5 || st == 0xF) &&
                            ((s8)MW_U8(mario, 0x24E) != 0 || (s8)MW_U8(mario, 0x24F) != 0)) {
                            mario->currSubMotionId = 0x50;
                            MW_F32(mario, 0xBC) = 0.0f;
                            MW_F32(mario, 0xB0) = 0.0f;
                            MW_S8(mario, 0x2BA) = 0;
                            MW_F32(mario, 0x2B0) = 0.0f;
                            MW_F32(mario, 0x90) = (f32)MW_S32(mario, 0x2A8) - MW_F32(mario, 0x1B8) - 3.5f;
                            MW_F32(mario, 0xB4) = 9.0f + MW_F32(mario, 0x1B8);
                        }
                    }
                    {
                        u8 st = MW_U8(mario, 0x2BA);
                        if ((s8)st == 0 || (s8)st == 0xA) {
                            mario->currSubMotionId = 0x50;
                            MW_F32(mario, 0xBC) = 0.0f;
                            MW_F32(mario, 0xB0) = 0.0f;
                            MW_S8(mario, 0x2BA) = 0;
                            MW_F32(mario, 0x2B0) = 0.0f;
                            MW_F32(mario, 0x90) = (f32)MW_S32(mario, 0x2A8) - MW_F32(mario, 0x1B8) - 3.5f;
                            MW_F32(mario, 0xB4) = 9.0f + MW_F32(mario, 0x1B8);
                        }
                    }
                    goto block_177;
                case 0x50:
                    if (MW_U16(mario, 0x248) & 0x100) {
                        mario->flags |= 0x80;
                        MW_F32(mario, 0x180) = 0.0f;
                        MW_F32(mario, 0xBC) = 0.0f;
                        mario->currSubMotionId = 0x5A;
                        goto block_177;
                    }
                    if (MW_U16(mario, 0x248) & 0x200) {
                        mario->flags |= 0x80;
                        marioGetPtr();
                        allPartyRideOff();
                        mario2 = marioGetPtr();
                        mario2->unk1A8 = MW_F32(mario2, 0x2A4);
                        MW_F32(mario2, 0x1AC) = MW_F32(mario2, 0x2A4);
                        MW_VEC(mario2, 0xB0) = vec[1];
                        MW_VEC(mario2, 0x98) = vec[2];
                        MW_VEC(mario2, 0xBC) = vec[3];
                        mario2->wPlayerCollisionBox.x = 20.0f;
                        mario2->wPlayerCollisionBox.y = 37.0f;
                        marioAdjustMoveDir();
                        mario2->dispFlags &= ~4;
                        mario2->dispFlags &= ~0x100;
                        marioChgMotSub(0, 0);
                        if (marioGetPtr()->motStruct != NULL) {
                            __memFree(HEAP_DEFAULT, marioGetPtr()->motStruct);
                            marioGetPtr()->motStruct = NULL;
                            return;
                        }
                    } else {
                        animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], (f32)(s8)MW_U8(mario, 0x2BB));
                        sp68 = mario->position;
                        sp2C = 10.0f + MW_F32(mario, 0x1B8);
                        sp68.x = MATCH_F32(float_1_8041ab94) * (f32)sin((3.1415927f * (MW_F32(mario, 0x194) + MW_F32(mario, 0x198))) / 180.0f) + sp68.x;
                        sp68.z = sp68.z - MATCH_F32(float_1_8041ab94) * (f32)cos((3.1415927f * (MW_F32(mario, 0x194) + MW_F32(mario, 0x198))) / 180.0f);
                        MW_S32(mario, 0x1F4) = (s32)marioSearchHead_jabara(&sp68, &sp2C, &sp30, 0.0f);
                        if ((HitEntry*)MW_S32(mario, 0x1F4) == NULL) {
                            sp2C = 5.0f + MW_F32(mario, 0x1B8);
                            MW_S32(mario, 0x1F4) = (s32)marioSearchHead(&sp68, &sp2C, 0.0f);
                        }
                        if (MW_F32(mario, 0x190) != MATCH_F32(float_0_8041aac4)) {
                            marioChgPose(str_8041AB98);
                            mario2 = marioGetPtr();
                            if (MW_S8(mario2, 0x2BA) == 0) {
                                MW_S8(mario2, 0x2BA) = 1;
                                MW_S16(mario2, 0x2B8) = 0xA;
                                MW_F32(mario2, 0x2B0) = 0.0f;
                                ((JabaraWork*)marioGetPtr()->motStruct)->unk4 = 0.1f;
                                ((JabaraWork*)marioGetPtr()->motStruct)->unk8 = -0.1f;
                                ((JabaraWork*)marioGetPtr()->motStruct)->unk10 = 0.18f;
                            }
                            var_r28 = 0;
                            f31 = 1.0f;
                            if ((HitEntry*)MW_S32(mario, 0x1F4) != NULL &&
                                (hitGetAttr((HitEntry*)MW_S32(mario, 0x1F4)) & 0x40000)) {
                                var_r28 = 1;
                            }
                            if (var_r28 != 0) {
                                f32 angle = MW_F32(mario, 0x194);
                                mario2 = marioGetPtr();
                                f28 = mario2->position.x;
                                f30 = mario2->position.z;
                                f29 = 2.0f + (mario2->position.y + MW_F32(mario2, 0x1B8));
                                sp1C = 0.5f * mario2->wPlayerCollisionBox.x;
                                sincosf(&sp8, &spC, angle);
                                hit = hitCheckFilter(NULL, &sp10, &sp14, &sp18, &sp1C, &sp20, &sp24, &sp28,
                                                     f28, f29, f30, sp8, 0.0f, spC);
                                if (hit == NULL) {
                                    f31 = f31 * (f32)sin((3.1415927f * MW_F32(mario, 0x194)) / 180.0f);
                                    MW_F32(mario, 0x8C) = MW_F32(mario, 0x8C) + f31;
                                    if (f31 > 0.0f) {
                                        if (MW_F32(mario, 0x1A8) >= 90.0f && MW_F32(mario, 0x1A8) <= 270.0f) {
                                            MW_U8(mario, 0x2BB) = MW_U8(mario, 0x2BB) + 1;
                                            if ((s8)MW_U8(mario, 0x2BB) > 0x20) {
                                                MW_U8(mario, 0x2BB) = 0;
                                            }
                                        } else {
                                            MW_U8(mario, 0x2BB) = MW_U8(mario, 0x2BB) - 1;
                                            if ((s8)MW_U8(mario, 0x2BB) < 0) {
                                                MW_U8(mario, 0x2BB) = 0x20;
                                            }
                                        }
                                    } else {
                                        if (MW_F32(mario, 0x1A8) >= 90.0f && MW_F32(mario, 0x1A8) <= 270.0f) {
                                            MW_U8(mario, 0x2BB) = MW_U8(mario, 0x2BB) - 1;
                                            if ((s8)MW_U8(mario, 0x2BB) < 0) {
                                                MW_U8(mario, 0x2BB) = 0x20;
                                            }
                                        } else {
                                            MW_U8(mario, 0x2BB) = MW_U8(mario, 0x2BB) + 1;
                                            if ((s8)MW_U8(mario, 0x2BB) > 0x20) {
                                                MW_U8(mario, 0x2BB) = 0;
                                            }
                                        }
                                    }
                                    if ((s8)MW_U8(mario, 0x2BB) == 0) {
                                        psndSFXOn_3D((const char*)0x187, &mario->position);
                                    }
                                    if ((s32)MW_U8(mario, 0x2BB) == 0x10) {
                                        psndSFXOn_3D((const char*)0x187, &mario->position);
                                    }
                                }
                            }
                            sp5C = mario->position;
                            marioUpdateCamCollision(sp5C);
                        }
                    }
                    break;
                }
block_177:
                        switch (mario->currSubMotionId) {
                        case 0x5A:
                            marioChgPose(RSTR(0xA8, str_802C5C30));
                            mario->currSubMotionId = 0x5B;
                            mario->dispFlags |= 4;
                            marioAdjustMoveDir();
                            MW_F32(mario, 0x7C) = 4.0f;
                            MW_F32(mario, 0x80) = -0.5f;
                            MW_F32(mario, 0x84) = 0.0f;
                            MW_F32(mario, 0x88) = 0.0f;
                            mario->currSubMotionId = 0x5B;
                            /* fallthrough */
                        case 0x5B:
                            MW_F32(mario, 0x90) = MW_F32(mario, 0x90) + MW_F32(mario, 0x7C);
                            marioMakeJumpPara();
                            if (MW_F32(mario, 0x7C) <= 0.0f) {
                                marioChgMot(0xA);
                                marioSetFallPara();
                            }
                            break;
                        }
                        if ((s32)mario->currSubMotionId >= 0x46) {
                            swingMain();
                        }
            } else {
                return;
            }
    }
}
#pragma opt_common_subs reset

s32 jabaraNoHitChk(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->currMotionId != MARIO_MOTION_JABARA) {
        return 0;
    }
    if ((s32)mario->currSubMotionId >= 0x28 && (s32)mario->currSubMotionId < 0x32) {
        return 1;
    }
    return 0;
}

// `switch` here, not an if-chain: the original's compare tree (9, then 3/12)
// is exactly CodeWarrior's codegen for a sparse switch.
void mot_jabara_post(void) {
    const Vec* vec = &vec3_802c5b88;
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2;

    mario->flags &= ~MARIO_FLAG_WALLS_DISABLED;
    mario2 = marioGetPtr();
    mario2->unk148 = MATCH_F32(float_0_8041aac4);
    mario2->unk150.z = MATCH_F32(float_0p01_8041aaf0);
    if (mario->dispFlags & 0x40000000) {
        marioPaperOff();
    }
    allPartyRideOff();
    mario2 = marioGetPtr();
    mario2->unk1A8 = MW_F32(mario2, 0x2A4);
    MW_F32(mario2, 0x1AC) = MW_F32(mario2, 0x2A4);
    MW_VEC(mario2, 0xB0) = vec[1];
    MW_VEC(mario2, 0x98) = vec[2];
    MW_VEC(mario2, 0xBC) = vec[3];
    mario2->wPlayerCollisionBox.x = MATCH_F32(float_20_8041aaf4);
    mario2->wPlayerCollisionBox.y = MATCH_F32(float_37_8041aaf8);
    marioAdjustMoveDir();
    mario2->dispFlags &= ~(1 << 2);
    mario2->dispFlags &= ~(1 << 8);
    if (marioGetPtr()->motStruct != NULL) {
        __memFree(HEAP_DEFAULT, marioGetPtr()->motStruct);
        marioGetPtr()->motStruct = NULL;
    }
    switch (mario->currMotionId) {
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 10:
    case 11:
        break;
    default:
        marioClearJumpPara();
        break;
    }
}

// State byte 0x2BA is signed (the original sign-extends it with `extsb`). The two
// otherwise single-case switch dispatches (state 1, state 11) only keep the original's
// `beq; bge; b` compare tree because states 0 and 10 are explicit cases that break to
// the same default; without them CodeWarrior folds the switch into a plain `if`.
// The clamp constants are plain globals rather than MATCH_F32: the volatile wrapper
// made the compiler reload the bound the original reuses from the compare.
void swingMain(void) {
    MarioWork* mario = marioGetPtr();
    
    f32 f31;
    s16 timer;

    if (marioGetPtr()->motStruct == NULL) {
        goto end;
    }
    if ((s32)mario->currSubMotionId < 0x5A && (MW_U16(mario, 0x248) & 0x300)) {
        if (!(((JabaraWork*)marioGetPtr()->motStruct)->flags & 1)) {
            MW_S16(mario, 0x2B8) = 0;
            ((JabaraWork*)marioGetPtr()->motStruct)->flags |= 1;
        }
    }
    switch (MW_S8(mario, 0x2BA)) {
    case 0:
        break;
    case 1:
    timer = MW_S16(mario, 0x2B8);
    if (timer > 0) {
        MW_S16(mario, 0x2B8) = timer - 1;
        MW_F32(mario, 0x2B0) += ((JabaraWork*)marioGetPtr()->motStruct)->unk4;
        if (MW_F32(mario, 0x2B0) >= float_40_8041aac0) {
            MW_F32(mario, 0x2B0) = float_40_8041aac0;
        }
    } else if (timer == 0) {
        MW_F32(mario, 0x2B0) += ((JabaraWork*)marioGetPtr()->motStruct)->unk8;
        if (MW_F32(mario, 0x2B0) <= float_0_8041aac4) {
            MW_F32(mario, 0x2B0) = float_0_8041aac4;
            MW_F32(mario, 0x2B4) = float_0p1_8041aac8;
            if (MW_F32(mario, 0xBC) >= MATCH_F32(float_180_8041aacc)) {
                MW_S8(mario, 0x2BA) = 2;
            } else {
                MW_S8(mario, 0x2BA) = 4;
            }
        }
    }
    if (((JabaraWork*)marioGetPtr()->motStruct)->flags & 1) {
        f31 = MW_F32(mario, 0xBC);
        MW_F32(mario, 0xBC) = revise360(f31 + MW_F32(mario, 0x2B0));
        if (MW_F32(mario, 0xBC) < f31) {
            MW_F32(mario, 0x2B0) = MATCH_F32(float_1p4_8041aad0);
            MW_F32(mario, 0x2B4) = MATCH_F32(float_0p1_8041aac8);
            if (MW_F32(mario, 0xBC) >= MATCH_F32(float_180_8041aacc)) {
                MW_S8(mario, 0x2BA) = 2;
            } else {
                MW_S8(mario, 0x2BA) = 4;
            }
        }
    } else {
        MW_F32(mario, 0xBC) = revise360(MW_F32(mario, 0xBC) + MW_F32(mario, 0x2B0));
    }
    break;
    }
state2:
    switch (MW_S8(mario, 0x2BA)) {
    case 2:
        MW_F32(mario, 0x2B0) += MW_F32(mario, 0x2B4);
        MW_F32(mario, 0xBC) = revise360(MW_F32(mario, 0xBC) + MW_F32(mario, 0x2B0));
        if (MW_F32(mario, 0xBC) < MATCH_F32(float_180_8041aacc)) {
            MW_S8(mario, 0x2BA) = 3;
            MW_F32(mario, 0x2B4) += ((JabaraWork*)marioGetPtr()->motStruct)->unk10;
        }
        break;
    case 3:
        MW_F32(mario, 0x2B0) -= MW_F32(mario, 0x2B4);
        if (MW_F32(mario, 0x2B0) <= float_0_8041aac4) {
            MW_F32(mario, 0x2B0) = float_0_8041aac4;
            MW_S8(mario, 0x2BA) = 4;
            MW_F32(mario, 0x2B4) -= ((JabaraWork*)marioGetPtr()->motStruct)->unk10;
            if (fabs(MW_F32(mario, 0xBC)) < MATCH_F32(float_0p8_8041aad4)) {
                MW_S8(mario, 0x2BA) = 0;
            }
        }
        MW_F32(mario, 0xBC) = revise360(MW_F32(mario, 0xBC) + MW_F32(mario, 0x2B0));
        break;
    case 4:
        MW_F32(mario, 0x2B0) -= MW_F32(mario, 0x2B4);
        MW_F32(mario, 0xBC) = revise360(MW_F32(mario, 0xBC) + MW_F32(mario, 0x2B0));
        if (MW_F32(mario, 0xBC) > MATCH_F32(float_180_8041aacc)) {
            MW_S8(mario, 0x2BA) = 5;
            MW_F32(mario, 0x2B4) += ((JabaraWork*)marioGetPtr()->motStruct)->unk10;
        }
        break;
    case 5:
        MW_F32(mario, 0x2B0) += MW_F32(mario, 0x2B4);
        if (MW_F32(mario, 0x2B0) >= float_0_8041aac4) {
            MW_F32(mario, 0x2B0) = float_0_8041aac4;
            MW_S8(mario, 0x2BA) = 2;
            MW_F32(mario, 0x2B4) -= ((JabaraWork*)marioGetPtr()->motStruct)->unk10;
            if (fabs(MW_F32(mario, 0xBC)) < MATCH_F32(float_0p8_8041aad4)) {
                MW_S8(mario, 0x2BA) = 0;
            }
        }
        MW_F32(mario, 0xBC) = revise360(MW_F32(mario, 0xBC) + MW_F32(mario, 0x2B0));
        break;
    }
    switch (MW_S8(mario, 0x2BA)) {
    case 10:
        break;
    case 11:
        timer = MW_S16(mario, 0x2B8);
        if (timer > 0) {
            MW_S16(mario, 0x2B8) = timer - 1;
            MW_F32(mario, 0x2B0) += ((JabaraWork*)marioGetPtr()->motStruct)->unk4;
            if (MW_F32(mario, 0x2B0) >= float_40_8041aac0) {
                MW_F32(mario, 0x2B0) = float_40_8041aac0;
            }
        } else if (timer == 0) {
            MW_F32(mario, 0x2B0) += ((JabaraWork*)marioGetPtr()->motStruct)->unk8;
            if (MW_F32(mario, 0x2B0) <= float_0_8041aac4) {
                MW_F32(mario, 0x2B0) = MATCH_F32(float_1p4_8041aad0);
                MW_F32(mario, 0x2B4) = MATCH_F32(float_0p1_8041aac8);
                if (MW_F32(mario, 0xBC) >= MATCH_F32(float_180_8041aacc)) {
                    MW_S8(mario, 0x2BA) = 12;
                } else {
                    MW_S8(mario, 0x2BA) = 14;
                }
            }
        }
        if (((JabaraWork*)marioGetPtr()->motStruct)->flags & 1) {
            f31 = MW_F32(mario, 0xBC);
            MW_F32(mario, 0xBC) = revise360(f31 - MW_F32(mario, 0x2B0));
            if (MW_F32(mario, 0xBC) > f31) {
                MW_F32(mario, 0x2B0) = MATCH_F32(float_5_8041aad8);
                MW_F32(mario, 0x2B4) = MATCH_F32(float_0p1_8041aac8);
                if (MW_F32(mario, 0xBC) >= MATCH_F32(float_180_8041aacc)) {
                    MW_S8(mario, 0x2BA) = 12;
                } else {
                    MW_S8(mario, 0x2BA) = 14;
                }
            }
        } else {
            MW_F32(mario, 0xBC) = revise360(MW_F32(mario, 0xBC) - MW_F32(mario, 0x2B0));
        }
        break;
    }
    switch (MW_S8(mario, 0x2BA)) {
    case 12:
        MW_F32(mario, 0x2B0) += MW_F32(mario, 0x2B4);
        MW_F32(mario, 0xBC) = revise360(MW_F32(mario, 0xBC) - MW_F32(mario, 0x2B0));
        if (MW_F32(mario, 0xBC) > MATCH_F32(float_180_8041aacc)) {
            MW_S8(mario, 0x2BA) = 13;
            MW_F32(mario, 0x2B4) += ((JabaraWork*)marioGetPtr()->motStruct)->unk10;
        }
        break;
    case 13:
        MW_F32(mario, 0x2B0) -= MW_F32(mario, 0x2B4);
        if (MW_F32(mario, 0x2B0) <= float_0_8041aac4) {
            MW_F32(mario, 0x2B0) = float_0_8041aac4;
            MW_S8(mario, 0x2BA) = 14;
            MW_F32(mario, 0x2B4) -= ((JabaraWork*)marioGetPtr()->motStruct)->unk10;
            if (fabs(MW_F32(mario, 0xBC)) < MATCH_F32(float_0p8_8041aad4)) {
                MW_S8(mario, 0x2BA) = 0;
            }
        }
        MW_F32(mario, 0xBC) = revise360(MW_F32(mario, 0xBC) - MW_F32(mario, 0x2B0));
        break;
    case 14:
        MW_F32(mario, 0x2B0) -= MW_F32(mario, 0x2B4);
        MW_F32(mario, 0xBC) = revise360(MW_F32(mario, 0xBC) - MW_F32(mario, 0x2B0));
        if (MW_F32(mario, 0xBC) < MATCH_F32(float_180_8041aacc)) {
            MW_S8(mario, 0x2BA) = 15;
            MW_F32(mario, 0x2B4) += ((JabaraWork*)marioGetPtr()->motStruct)->unk10;
        }
        break;
    case 15:
        MW_F32(mario, 0x2B0) += MW_F32(mario, 0x2B4);
        if (MW_F32(mario, 0x2B0) >= float_0_8041aac4) {
            MW_F32(mario, 0x2B0) = float_0_8041aac4;
            MW_S8(mario, 0x2BA) = 12;
            MW_F32(mario, 0x2B4) -= ((JabaraWork*)marioGetPtr()->motStruct)->unk10;
            if (fabs(MW_F32(mario, 0xBC)) < MATCH_F32(float_0p8_8041aad4)) {
                MW_S8(mario, 0x2BA) = 10;
            }
        }
        MW_F32(mario, 0xBC) = revise360(MW_F32(mario, 0xBC) - MW_F32(mario, 0x2B0));
        break;
    }
end:
    ;
}

void mot_grasp(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1u;
        mario->flags &= ~(MARIO_FLAG_IS_JUMPING | MARIO_FLAG_IS_FALLING | MARIO_FLAG_IS_STEPPING |
                          MARIO_FLAG_HAS_INPUT_JUMP);
    }
}

