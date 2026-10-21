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

// Raw accessors for MarioWork scratch words this motion reads with a type no union member has.
#define MW_S8(m, off) (*(s8*)((u8*)(m) + (off)))
#define MW_S32(m, off) (*(s32*)((u8*)(m) + (off)))

// A nonconst view keeps these named pool words from folding into duplicate literals.
#ifdef MUST_MATCH
#define JABARA_F32(sym) (*(f32*)&(sym))
#else
#define JABARA_F32(sym) (sym)
#endif

// Local quake script table (`.sdata`), referenced by mot_jabara.
static s32 quake_evt[8] = { 0x0005005B, (s32)evt_cam_shake, 4, (s32)0xF24A7A80,
                            (s32)0xF24A7A8F, 0xF0, 2, 1 };

#ifdef MUST_MATCH
// Emit the real functions in retail order so their literals pool in that order.
#pragma defer_codegen off
#endif

void mot_grasp(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1u;
        mario->flags &= ~(MARIO_FLAG_IS_JUMPING | MARIO_FLAG_IS_FALLING | MARIO_FLAG_IS_STEPPING |
                          MARIO_FLAG_HAS_INPUT_JUMP);
    }
}

void swingMain(void) {
    MarioWork* mario = marioGetPtr();

    f32 f31;
    s16 timer;

    if (marioGetPtr()->motStruct == NULL) {
        return;
    }
    if (mario->currSubMotionId < 0x5A && (mario->buttonNew & 0x300)) {
        if (!(((JabaraWork*)marioGetPtr()->motStruct)->flags & 1)) {
            mario->jabaraTimer = 0;
            ((JabaraWork*)marioGetPtr()->motStruct)->flags |= 1;
        }
    }
    switch (mario->jabaraState) {
    case 0:
        break;
    case 1:
    timer = mario->jabaraTimer;
    if (timer > 0) {
        mario->jabaraTimer = timer - 1;
        mario->jabaraSpeed += ((JabaraWork*)marioGetPtr()->motStruct)->unk4;
        if (mario->jabaraSpeed >= 40.0f) {
            mario->jabaraSpeed = 40.0f;
        }
    } else if (timer == 0) {
        mario->jabaraSpeed += ((JabaraWork*)marioGetPtr()->motStruct)->unk8;
        if (mario->jabaraSpeed <= 0.0f) {
            mario->jabaraSpeed = 0.0f;
            mario->jabaraAccel = 0.1f;
            if (mario->dispRot.x >= 180.0f) {
                mario->jabaraState = 2;
            } else {
                mario->jabaraState = 4;
            }
        }
    }
    if (((JabaraWork*)marioGetPtr()->motStruct)->flags & 1) {
        f31 = mario->dispRot.x;
        mario->dispRot.x = revise360(f31 + mario->jabaraSpeed);
        if (mario->dispRot.x < f31) {
            mario->jabaraSpeed = 1.4f;
            mario->jabaraAccel = 0.1f;
            if (mario->dispRot.x >= 180.0f) {
                mario->jabaraState = 2;
            } else {
                mario->jabaraState = 4;
            }
        }
    } else {
        mario->dispRot.x = revise360(mario->dispRot.x + mario->jabaraSpeed);
    }
    break;
    }
    switch (mario->jabaraState) {
    case 2:
        mario->jabaraSpeed += mario->jabaraAccel;
        mario->dispRot.x = revise360(mario->dispRot.x + mario->jabaraSpeed);
        if (mario->dispRot.x < 180.0f) {
            mario->jabaraState = 3;
            mario->jabaraAccel += ((JabaraWork*)marioGetPtr()->motStruct)->unk10;
        }
        break;
    case 3:
        mario->jabaraSpeed -= mario->jabaraAccel;
        if (mario->jabaraSpeed <= 0.0f) {
            mario->jabaraSpeed = 0.0f;
            mario->jabaraState = 4;
            mario->jabaraAccel -= ((JabaraWork*)marioGetPtr()->motStruct)->unk10;
            if (fabsf(mario->dispRot.x) < 0.8f) {
                mario->jabaraState = 0;
            }
        }
        mario->dispRot.x = revise360(mario->dispRot.x + mario->jabaraSpeed);
        break;
    case 4:
        mario->jabaraSpeed -= mario->jabaraAccel;
        mario->dispRot.x = revise360(mario->dispRot.x + mario->jabaraSpeed);
        if (mario->dispRot.x > 180.0f) {
            mario->jabaraState = 5;
            mario->jabaraAccel += ((JabaraWork*)marioGetPtr()->motStruct)->unk10;
        }
        break;
    case 5:
        mario->jabaraSpeed += mario->jabaraAccel;
        if (mario->jabaraSpeed >= 0.0f) {
            mario->jabaraSpeed = 0.0f;
            mario->jabaraState = 2;
            mario->jabaraAccel -= ((JabaraWork*)marioGetPtr()->motStruct)->unk10;
            if (fabsf(mario->dispRot.x) < 0.8f) {
                mario->jabaraState = 0;
            }
        }
        mario->dispRot.x = revise360(mario->dispRot.x + mario->jabaraSpeed);
        break;
    }
    switch (mario->jabaraState) {
    case 10:
        break;
    case 11:
        timer = mario->jabaraTimer;
        if (timer > 0) {
            mario->jabaraTimer = timer - 1;
            mario->jabaraSpeed += ((JabaraWork*)marioGetPtr()->motStruct)->unk4;
            if (mario->jabaraSpeed >= 40.0f) {
                mario->jabaraSpeed = 40.0f;
            }
        } else if (timer == 0) {
            mario->jabaraSpeed += ((JabaraWork*)marioGetPtr()->motStruct)->unk8;
            if (mario->jabaraSpeed <= 0.0f) {
                mario->jabaraSpeed = 1.4f;
                mario->jabaraAccel = 0.1f;
                if (mario->dispRot.x >= 180.0f) {
                    mario->jabaraState = 12;
                } else {
                    mario->jabaraState = 14;
                }
            }
        }
        if (((JabaraWork*)marioGetPtr()->motStruct)->flags & 1) {
            f31 = mario->dispRot.x;
            mario->dispRot.x = revise360(f31 - mario->jabaraSpeed);
            if (mario->dispRot.x > f31) {
                mario->jabaraSpeed = 5.0f;
                mario->jabaraAccel = 0.1f;
                if (mario->dispRot.x >= 180.0f) {
                    mario->jabaraState = 12;
                } else {
                    mario->jabaraState = 14;
                }
            }
        } else {
            mario->dispRot.x = revise360(mario->dispRot.x - mario->jabaraSpeed);
        }
        break;
    }
    switch (mario->jabaraState) {
    case 12:
        mario->jabaraSpeed += mario->jabaraAccel;
        mario->dispRot.x = revise360(mario->dispRot.x - mario->jabaraSpeed);
        if (mario->dispRot.x > 180.0f) {
            mario->jabaraState = 13;
            mario->jabaraAccel += ((JabaraWork*)marioGetPtr()->motStruct)->unk10;
        }
        break;
    case 13:
        mario->jabaraSpeed -= mario->jabaraAccel;
        if (mario->jabaraSpeed <= 0.0f) {
            mario->jabaraSpeed = 0.0f;
            mario->jabaraState = 14;
            mario->jabaraAccel -= ((JabaraWork*)marioGetPtr()->motStruct)->unk10;
            if (fabsf(mario->dispRot.x) < 0.8f) {
                mario->jabaraState = 0;
            }
        }
        mario->dispRot.x = revise360(mario->dispRot.x - mario->jabaraSpeed);
        break;
    case 14:
        mario->jabaraSpeed -= mario->jabaraAccel;
        mario->dispRot.x = revise360(mario->dispRot.x - mario->jabaraSpeed);
        if (mario->dispRot.x < 180.0f) {
            mario->jabaraState = 15;
            mario->jabaraAccel += ((JabaraWork*)marioGetPtr()->motStruct)->unk10;
        }
        break;
    case 15:
        mario->jabaraSpeed += mario->jabaraAccel;
        if (mario->jabaraSpeed >= 0.0f) {
            mario->jabaraSpeed = 0.0f;
            mario->jabaraState = 12;
            mario->jabaraAccel -= ((JabaraWork*)marioGetPtr()->motStruct)->unk10;
            if (fabsf(mario->dispRot.x) < 0.8f) {
                mario->jabaraState = 10;
            }
        }
        mario->dispRot.x = revise360(mario->dispRot.x - mario->jabaraSpeed);
        break;
    }
}

// .sdata2
// The first seven words pool from swingMain. The words up to 37.0 are defined
// here, before mot_jabara_post; mot_jabara pools its seven camera constants
// itself, and the words after those are defined after mot_jabara.
const f32 float_neg0p1_8041aadc = -0.1f;
const f32 float_0p18_8041aae0 = 0.18f;
const f32 float_24_8041aae4 = 24.0f;
const f32 float_2p4_8041aae8 = 2.4f;
const f32 float_neg0p4_8041aaec = -0.4f;
const f32 float_0p01_8041aaf0 = 0.01f;
const f32 float_20_8041aaf4 = 20.0f;
const f32 float_37_8041aaf8 = 37.0f;


// Camera follow parameters for the stretched pose.
static inline void jabaraSetCamParam(f32 a, f32 b, f32 c) {
    MarioWork* mario = marioGetPtr();

    mario->unk148 = a;
    mario->unk150.z = b;
    mario->field_68 = c;
    mario->field_6C = (Vec){ 0.0f, 40.0f, 0.0f };
}

// Restores Mario's facing, movement vectors and collision box when the motion ends.
static inline void jabaraResetMario(void) {
    MarioWork* mario = marioGetPtr();

    mario->unk1A8 = mario->field_0x2A4;
    mario->dispReqDir = mario->field_0x2A4;
    mario->dispPivot = (Vec){ 0.0f, 0.0f, 0.0f };
    mario->dispOfs = (Vec){ 0.0f, 0.0f, 0.0f };
    mario->dispRot = (Vec){ 0.0f, 0.0f, 0.0f };
    mario->wPlayerCollisionBox.x = JABARA_F32(float_20_8041aaf4);
    mario->wPlayerCollisionBox.y = JABARA_F32(float_37_8041aaf8);
    marioAdjustMoveDir();
    mario->dispFlags &= ~4;
    mario->dispFlags &= ~0x100;
}

// Puts Mario on the ground he landed on.
static inline void jabaraLanding(f32 y, HitEntry* hit) {
    MarioWork* mario = marioGetPtr();

    mario->airTimer = 0;
    mario->airTimerPeak = 0;
    mario->hitobjStandOn = hit;
    mario->hitobjJumpFrom = NULL;
    mario->position.y = y;
    mario->flags &= 0xFFF0FFFF;
}

// Releases the motion's work buffer.
static inline void jabaraFreeWork(void) {
    if (marioGetPtr()->motStruct != NULL) {
        __memFree(HEAP_DEFAULT, marioGetPtr()->motStruct);
        marioGetPtr()->motStruct = NULL;
    }
}

// .rodata strings; the compound-literal templates above pool ahead of them.
const char str_802C5BB8[] = "M_J_1A";
const char str_p_jyabara[] = "p_jyabara";
const char str_PM_J_1B[] = "PM_J_1B";
const char str_PM_J_3B[] = "PM_J_3B";
const char str_PM_J_2B[] = "PM_J_2B";
const char str_802C5BE4[] = "M_J_5A";
const char str_PM_J_1C[] = "PM_J_1C";
const char str_802C5BF4[] = "M_J_1C";
const char str_PM_J_1D[] = "PM_J_1D";
const char str_eki_02[] = "eki_02";
const char str_PM_J_1F[] = "PM_J_1F";
const char str_PM_J_1G[] = "PM_J_1G";
const char str_PM_J_1H[] = "PM_J_1H";
__declspec(section ".rodata") const u32 gap_04_802C5C24_rodata = 0;
const f64 double_1p5708_802c5c28 = 1.5707963705062866;

// .sdata2 words defined after mot_jabara (they follow its pooled literals).
extern const f32 float_480_8041ab18;
extern const f32 float_50_8041ab1c;
extern const f32 float_25_8041ab20;
extern const f32 float_200_8041ab24;
extern const f32 float_10p45_8041ab28;
extern const f32 float_0p278_8041ab2c;
extern const f32 float_neg0p031_8041ab30;
extern const f32 float_8_8041ab34;
extern const f32 float_neg0p84_8041ab38;
extern const f32 float_0p08_8041ab3c;
extern const f32 float_neg0p015_8041ab40;
extern const f32 float_4p3_8041ab44;
extern const f32 float_0p5_8041ab48;
extern const f32 float_100_8041ab4c;
extern const f32 float_3p5_8041ab50;
extern const f32 float_9_8041ab54;
extern const f32 float_neg6_8041ab58;
extern const f32 float_neg450_8041ab5c;
extern const f32 float_neg190_8041ab60;
extern const f32 float_neg8_8041ab64;
extern const f32 float_4_8041ab68;
extern const f32 float_2_8041ab6c;
extern __declspec(section ".sdata2") const char str_8041AB70[8];
extern const f32 float_0p6_8041ab78;
extern const f32 float_neg0p6_8041ab7c;
extern const f32 float_3p1416_8041ab80;
extern const f32 float_neg1_8041ab84;
extern __declspec(section ".sdata2") const char str_M_S_4[8];
extern const f32 float_10_8041ab90;
extern const f32 float_1_8041ab94;
extern __declspec(section ".sdata2") const char str_M_W_6[8];
extern const f32 float_90_8041aba0;
extern const f32 float_270_8041aba4;
extern const f32 float_neg0p5_8041aba8;

void mot_jabara_post(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2;

    mario->flags &= ~MARIO_FLAG_WALLS_DISABLED;
    mario2 = marioGetPtr();
    mario2->unk148 = 0.0f;
    mario2->unk150.z = MATCH_F32(float_0p01_8041aaf0);
    if (mario->dispFlags & 0x40000000) {
        marioPaperOff();
    }
    allPartyRideOff();
    jabaraResetMario();
    jabaraFreeWork();
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

s32 jabaraNoHitChk(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->currMotionId != MARIO_MOTION_JABARA) {
        return 0;
    }
    if (mario->currSubMotionId >= 0x28 && mario->currSubMotionId < 0x32) {
        return 1;
    }
    return 0;
}

// Depth of the hit object Mario grabbed, rounded to two decimals.
static inline f32 jabaraGetHitDepth(HitEntry* hit) {
    Vec pos;

    marioGetPtr();
    hitObjGetPos(hitGetName(hit), &pos);
    return (f32)(s32)((JABARA_F32(float_100_8041ab4c) * pos.z + JABARA_F32(float_0p5_8041ab48)) / JABARA_F32(float_100_8041ab4c));
}

void mot_jabara(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2;
    HitEntry* hit;
    JabaraWork* jw;
    f32 f31, f30, f29, f28;
    Vec sp74;
    Vec sp68;
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
        mario->jabaraHitY = 0;
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
    mario->hitobjHead = NULL;

    switch (mario->currSubMotionId) {
    case 0:
        mario->dispFlags |= 0x100;
        mario->field_0x41 = 1;
        mario->field_0x42 = 0;
        mario->airTimer = 0;
        mario->airTimerPeak = 0;
        mario->baseSpeed = 0.0f;
        mario->currSubMotionId = 0xA;
        marioChgPose(RSTR(0x30, str_802C5BB8));
        marioPaperOn(RSTR(0x38, str_p_jyabara));
        marioChgPaper(RSTR(0x44, str_PM_J_1B));
        mario->field_0x11C = mario->position;
        marioChkGnd();
        if (mario->currMotionId != MARIO_MOTION_JABARA) {
            mario->currSubMotionId = 0x64;
        }
        break;
    case 10:
        if (mario->currMotionId != MARIO_MOTION_JABARA) {
            mario->currSubMotionId = 0x64;
            break;
        }
        L_allPartyRideOn();
        psndSFXOn_3D((const char*)0x182, &mario->position);
        jabaraSetCamParam(0.2f, 0.005f, 1.5f);
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
            mario->field_0x41 = mario->field_0x41 + 1;
            if (mario->field_0x41 >= 0x14) {
                mario->field_0x41 = 0x14;
            }
        }
        {
            s8 jt = mario->field_0x41;
            if (jt == 3) {
                if (!(((JabaraWork*)marioGetPtr()->motStruct)->flags & 0x40000)) {
                    ((JabaraWork*)marioGetPtr()->motStruct)->flags |= 0x40000;
                    psndSFXOn_3D((const char*)0x184, &mario->position);
                }
                marioChgPaper(RSTR(0x4C, str_PM_J_3B));
                mario->field_0x42 = 3;
                jabaraSetCamParam(0.4f, 0.005f, 1.7f);
            } else if (jt == 2) {
                if (!(((JabaraWork*)marioGetPtr()->motStruct)->flags & 0x20000)) {
                    ((JabaraWork*)marioGetPtr()->motStruct)->flags |= 0x20000;
                    psndSFXOn_3D((const char*)0x183, &mario->position);
                }
                marioChgPaper(RSTR(0x54, str_PM_J_2B));
                mario->field_0x42 = 2;
                jabaraSetCamParam(0.3f, 0.005f, 1.6f);
            }
        }
        marioChkGnd();
        if (mario->currMotionId != MARIO_MOTION_JABARA) {
            mario->currSubMotionId = 0x64;
            return;
        }
        if ((f32)mario->unk16C > JABARA_F32(float_480_8041ab18) || (f32)mario->unk16C < JABARA_F32(float_50_8041ab1c)) {
            camFollowYOn();
        }
        if (mario->position.y == mario->prevPosition.y) {
            camFollowYOff();
        }
        sp74 = mario->position;
        sp34 = JABARA_F32(float_25_8041ab20);
        var_r28 = 0;
        hit = (HitEntry*)marioSearchHead(&sp74, &sp34, JABARA_F32(float_200_8041ab24));
        if (hit != NULL) {
            if (!(mario->buttonHeld & 0x100)) {
                ((JabaraWork*)marioGetPtr()->motStruct)->unk1C++;
                if (((JabaraWork*)marioGetPtr()->motStruct)->unk1C > sysMsec2Frame(0x1388)) {
                    var_r28 = 1;
                }
            }
            if (var_r28 == 0) {
                break;
            }
        }
        if (var_r28 != 0 || !(mario->buttonHeld & 0x100) || (mario->buttonUp & 0x100)) {
            mario2 = marioGetPtr();
            mario2->unk148 = 0.0f;
            mario2->unk150.z = JABARA_F32(float_0p01_8041aaf0);
            camFollowYOff();
            marioChgPose(RSTR(0x5C, str_802C5BE4));
            marioChgPaper(RSTR(0x64, str_PM_J_1C));
            psndSFXOn_3D((const char*)0x20185, &mario->position);
            psndSFXOn_3D((const char*)0x200B5, &mario->position);
            {
                s8 jt = mario->field_0x42;
                if (jt == 3) {
                    mario->jumpVelY = JABARA_F32(float_10p45_8041ab28);
                    mario->jumpAccelY = JABARA_F32(float_0p278_8041ab2c);
                    mario->jumpJerkY = JABARA_F32(float_neg0p031_8041ab30);
                    mario->jumpJerkStepY = 0.0f;
                } else if (jt == 2) {
                    mario->jumpVelY = JABARA_F32(float_8_8041ab34);
                    mario->jumpAccelY = JABARA_F32(float_neg0p84_8041ab38);
                    mario->jumpJerkY = JABARA_F32(float_0p08_8041ab3c);
                    mario->jumpJerkStepY = JABARA_F32(float_neg0p015_8041ab40);
                } else {
                    mario->jumpVelY = JABARA_F32(float_4p3_8041ab44);
                    mario->jumpAccelY = JABARA_F32(float_neg0p84_8041ab38);
                    mario->jumpJerkY = JABARA_F32(float_0p08_8041ab3c);
                    mario->jumpJerkStepY = JABARA_F32(float_neg0p015_8041ab40);
                }
            }
            if (mario->field_0x42 == 3) {
                unk_80130374();
            }
            mario->hitobjJumpFrom = mario->hitobjStandOn;
            mario->hitobjStandOn = NULL;
            mario->field_0x2A4 = mario->dispReqDir;
            mario->currSubMotionId = 0x14;
        }
        break;
    case 20:
        marioMakeJumpPara();
        if (mario->field_0x42 == 3) {
            sp3C = marioChkOverheadJabara(&sp40, &sp38);
        } else {
            sp3C = marioChkOverhead();
        }
        if ((HitEntry*)mario->hitobjHead == NULL) {
            if (mario->jumpVelY <= 0.0f) {
                mario->currSubMotionId = 0x1E;
                marioSetPeakYpos();
            }
            mario->position.y = mario->position.y + mario->jumpVelY;
            break;
        }
        f31 = sp3C;
        if (f31 < mario->field_0x11C.y) {
            HitEntry* stand;

            marioPaperOff();
            stand = (HitEntry*)mario->hitobjStandOn;
            jabaraLanding(mario->field_0x11C.y, stand);
            camFollowYOn();
            marioGetPtr();
            allPartyRideOff();
            jabaraResetMario();
            marioChgMotSub(0, 0);
            jabaraFreeWork();
            return;
        } else {
            mario->position.y = f31;
            if (mario->field_0x42 <= 2) {
                mario->currSubMotionId = 0x1E;
            } else if (hitGetAttr((HitEntry*)mario->hitobjHead) & 0x40000) {
                mario->field_0x2A4 = mario->dispReqDir;
                mario->unk1A8 = mario->dispReqDir;
                mario->currSubMotionId = 0x46;
                mario->multiTimer = 0xA;
                mario->jabaraHitY = (s32)(sp3C + mario->wPlayerCollisionBox.y);
                mario->position.z = jabaraGetHitDepth((HitEntry*)mario->hitobjHead);
                f30 = (f32)mario->jabaraHitY;
                f28 = f30 - mario->wPlayerCollisionBox.y;
                mario->position.y = f28 - JABARA_F32(float_3p5_8041ab50);
                mario->dispPivot.y = JABARA_F32(float_9_8041ab54) + mario->wPlayerCollisionBox.y;
                marioPaperOff();
                camFollowYOn();
                mario->dispFlags |= 4;
                marioChgPose(RSTR(0x5C, str_802C5BE4));
                psndSFXOn_3D((const char*)0x186, &mario->position);
                var_sp40 = sp40;
                mario2 = marioGetPtr();
                mario2->jabaraTimer = 4;
                mario2->jabaraSpeed = JABARA_F32(float_24_8041aae4);
                ((JabaraWork*)marioGetPtr()->motStruct)->unk4 = JABARA_F32(float_2p4_8041aae8);
                ((JabaraWork*)marioGetPtr()->motStruct)->unk8 = JABARA_F32(float_neg0p4_8041aaec);
                ((JabaraWork*)marioGetPtr()->motStruct)->unk10 = JABARA_F32(float_0p18_8041aae0);
                if (var_sp40 < 0) {
                    mario2->jabaraState = 0xB;
                } else {
                    mario2->jabaraState = 1;
                }
            } else {
                mario->currSubMotionId = 0x28;
            }
        }
        break;
    case 30:
        marioChgPose(RSTR(0x6C, str_802C5BF4));
        marioChgPaper(RSTR(0x74, str_PM_J_1D));
        marioSetFallPara();
        mario->currSubMotionId = 0x1F;
        /* fallthrough */
    case 31:
        var_f28 = marioGetFallSpd();
        if (var_f28 < JABARA_F32(float_neg6_8041ab58)) {
            var_f28 = JABARA_F32(float_neg6_8041ab58);
        }
        hit = (HitEntry*)marioChkLandon(&sp3C, var_f28);
        if (hit == NULL) {
            mario->position.y = mario->position.y + var_f28;
            break;
        }
        marioPaperOff();
        jabaraLanding(sp3C, hit);
        camFollowYOn();
        marioGetPtr();
        allPartyRideOff();
        jabaraResetMario();
        marioChgMotSub(0, 0);
        jabaraFreeWork();
        return;
    default:
        break;
    }
    switch (mario->currSubMotionId) {
    case 0x28:
        if (strcmp(gp->currentMapName, RSTR(0x7C, str_eki_02)) == 0) {
            if (mario->position.x >= JABARA_F32(float_neg450_8041ab5c) && mario->position.x <= JABARA_F32(float_neg190_8041ab60)) {
                mario->flags |= 0x400;
            }
        }
        psndSFXOn_3D((const char*)0x188, &mario->position);
        evtEntryType(quake_evt, 0, 0, 0);
        marioClearJumpPara();
        marioChgPose(RSTR(0x5C, str_802C5BE4));
        marioChgPaper(RSTR(0x84, str_PM_J_1F));
        mario->jabaraAnimTime = 1;
        mario->poseTime = MW_S8(mario, 0x2BB);
        mario->dispOfs.y = JABARA_F32(float_neg8_8041ab64);
        mario->multiTimer = 0x32;
        mario->currSubMotionId = 0x29;
        break;
    case 0x29:
        animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], (f32)(s8)mario->jabaraAnimTime);
        mario->jabaraAnimTime = mario->jabaraAnimTime + 1;
        if ((s8)mario->jabaraAnimTime >= 8) {
            mario->jabaraAnimTime = 8;
            mario->wPlayerCollisionBox.y = JABARA_F32(float_4_8041ab68);
        }
        if (--mario->multiTimer <= 0) {
            mario->currSubMotionId = 0x2A;
        }
        break;
    case 0x2A:
        mario->dispOfs.y = mario->dispOfs.y + MATCH_F32(float_2_8041ab6c);
        if (mario->dispOfs.y >= 0.0f) {
            mario->dispOfs.y = 0.0f;
        }
        animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], (f32)(s8)mario->jabaraAnimTime);
        if (++MW_S8(mario, 0x2BB) >= 0x10) {
            mario->jabaraAnimTime = 0x10;
            marioChgPaper(RSTR(0x8C, str_PM_J_1G));
            marioPaperLightOff();
            mario->currSubMotionId = 0x2B;
            mario->multiTimer = 0x18;
        }
        break;
    case 0x2B:
        if (--mario->multiTimer > 0) {
            break;
        }
        marioChgPose(str_8041AB70);
        marioChgPaper(RSTR(0x94, str_PM_J_1H));
        marioPaperLightOff();
        mario->currSubMotionId = 0x32;
        mario->jabaraAnimTime = 0;
        ((JabaraWork*)marioGetPtr()->motStruct)->unkC = 0.0f;
        /* fallthrough */
    case 0x32:
    {
        s32 changed;
        f32 spd;
        f32 dy;
        animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], (f32)(s8)mario->jabaraAnimTime);
            changed = 0;
            {
                s8 b = MW_S8(mario, 0x2BB);
                if (b >= 0x23 && b <= 0x62) {
                    if (mario->dispReqDir == 180.0f) {
                        spd = JABARA_F32(float_0p6_8041ab78);
                    } else {
                        spd = JABARA_F32(float_neg0p6_8041ab7c);
                    }
                    if (spd != ((JabaraWork*)marioGetPtr()->motStruct)->unk18) {
                        ((JabaraWork*)marioGetPtr()->motStruct)->unk14++;
                        changed = 1;
                    }
                    ((JabaraWork*)marioGetPtr()->motStruct)->unk18 = spd;
                } else {
                    if (mario->dispReqDir == 180.0f) {
                        spd = JABARA_F32(float_neg0p6_8041ab7c);
                    } else {
                        spd = JABARA_F32(float_0p6_8041ab78);
                    }
                    if (spd != ((JabaraWork*)marioGetPtr()->motStruct)->unk18) {
                        ((JabaraWork*)marioGetPtr()->motStruct)->unk14++;
                        changed = 1;
                    }
                    ((JabaraWork*)marioGetPtr()->motStruct)->unk18 = spd;
                }
            }
            if (changed != 0) {
                if (((JabaraWork*)marioGetPtr()->motStruct)->unk14 == 1) {
                    psndSFXOn_3D((const char*)0x189, &mario->position);
                } else if (((JabaraWork*)marioGetPtr()->motStruct)->unk14 == 2) {
                    psndSFXOn_3D((const char*)0x18A, &mario->position);
                } else if (((JabaraWork*)marioGetPtr()->motStruct)->unk14 == 3) {
                    psndSFXOn_3D((const char*)0x18B, &mario->position);
                }
            }
            f29 = (f32)cos((JABARA_F32(float_3p1416_8041ab80) * mario->targetYaw) / 180.0f);
            mario->dispOfs.x = spd * f29 + mario->dispOfs.x;
            f30 = (f32)sin((JABARA_F32(float_3p1416_8041ab80) * mario->targetYaw) / 180.0f);
            mario->dispOfs.z = spd * f30 + mario->dispOfs.z;
            dy = JABARA_F32(float_neg1_8041ab84) * (f32)sin(RDBL(0xA0, double_1p5708_802c5c28));
            hit = (HitEntry*)marioChkLandon2(&sp3C, dy, mario->dispOfs.y);
            if (hit == NULL) {
                mario->position.y = mario->position.y + dy;
                if ((f32)mario->unk16C > JABARA_F32(float_480_8041ab18)) {
                    camFollowYOn();
                }
                {
                    s32 t = mario->jabaraAnimTime + 1;
                    mario->jabaraAnimTime = t;
                    if ((s8)t > 0x78) {
                        mario->jabaraAnimTime = 0;
                    }
                }
                if (mario->buttonNew & 0x200) {
                    mario->flags |= 0x80;
                    mario->flags &= ~0x400;
                    marioPaperOff();
                    marioGetPtr();
                    allPartyRideOff();
                    jabaraResetMario();
                    marioChgMotSub(0, 0);
                    jabaraFreeWork();
                    return;
                }
            } else {
                mario->hitobjStandOn = hit;
                mario->hitobjJumpFrom = NULL;
                mario->position.y = sp3C;
                mario->currSubMotionId = 0x34;
                mario->jabaraAnimTime = 0;
                mario->multiTimer = 0x14;
            }
    }
        break;
    case 0x34:
        animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], (f32)(s8)mario->jabaraAnimTime);
        if (--mario->multiTimer <= 0) {
            mario->currSubMotionId = 0x3C;
        }
        break;
    }

    {
        s32 sm = mario->currSubMotionId;
        switch (sm) {
        case 0x3C:
            allPartyRideOff();
            mario->currSubMotionId = 0x3D;
            mario->hitobjJumpFrom = mario->hitobjStandOn;
            mario->hitobjStandOn = NULL;
            mario->baseSpeed = 0.0f;
            mario->flags &= ~0x400;
            jabaraResetMario();
            jabaraFreeWork();
            marioPaperOff();
            camFollowYOff();
            marioChgSmallJumpMotion();
            mario->baseSpeed = 0.0f;
            return;
        }
        if (sm < 0x3C || sm >= 0x46) {
            switch (sm) {
                case 0x46:
                    {
                        s32 t = mario->multiTimer;
                        mario->multiTimer = t - 1;
                        if (t <= 0) {
                            marioChgPose(str_M_S_4);
                            mario->currSubMotionId = 0x47;
                        }
                    }
                    /* fallthrough */
                case 0x47:
                    {
                        s8 st = mario->jabaraState;
                        if ((st == 5 || st == 0xF) &&
                            (mario->wStickDir1 != 0 || mario->wStickDir2 != 0)) {
                            mario->currSubMotionId = 0x50;
                            mario->dispRot.x = 0.0f;
                            mario->dispPivot.x = 0.0f;
                            mario->jabaraState = 0;
                            mario->jabaraSpeed = 0.0f;
                            mario->position.y = (f32)mario->jabaraHitY - mario->wPlayerCollisionBox.y - JABARA_F32(float_3p5_8041ab50);
                            mario->dispPivot.y = JABARA_F32(float_9_8041ab54) + mario->wPlayerCollisionBox.y;
                        }
                    }
                    {
                        s8 st = mario->jabaraState;
                        if (st == 0 || st == 0xA) {
                            mario->currSubMotionId = 0x50;
                            mario->dispRot.x = 0.0f;
                            mario->dispPivot.x = 0.0f;
                            mario->jabaraState = 0;
                            mario->jabaraSpeed = 0.0f;
                            mario->position.y = (f32)mario->jabaraHitY - mario->wPlayerCollisionBox.y - JABARA_F32(float_3p5_8041ab50);
                            mario->dispPivot.y = JABARA_F32(float_9_8041ab54) + mario->wPlayerCollisionBox.y;
                        }
                    }
                    break;
                case 0x50:
                    if (mario->buttonNew & 0x100) {
                        mario->flags |= 0x80;
                        mario->baseSpeed = 0.0f;
                        mario->dispRot.x = 0.0f;
                        mario->currSubMotionId = 0x5A;
                        break;
                    }
                    if (mario->buttonNew & 0x200) {
                        mario->flags |= 0x80;
                        marioGetPtr();
                        allPartyRideOff();
                        jabaraResetMario();
                        marioChgMotSub(0, 0);
                        jabaraFreeWork();
                        return;
                    } else {
                        animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], (f32)(s8)mario->jabaraAnimTime);
                        sp68 = mario->position;
                        sp2C = JABARA_F32(float_10_8041ab90) + mario->wPlayerCollisionBox.y;
                        sp68.x = MATCH_F32(float_1_8041ab94) * (f32)sin((JABARA_F32(float_3p1416_8041ab80) * (mario->controlStickAngle + mario->targetYaw)) / 180.0f) + sp68.x;
                        sp68.z = sp68.z - MATCH_F32(float_1_8041ab94) * (f32)cos((JABARA_F32(float_3p1416_8041ab80) * (mario->controlStickAngle + mario->targetYaw)) / 180.0f);
                        mario->hitobjHead = marioSearchHead_jabara(&sp68, &sp2C, &sp30, 0.0f);
                        if ((HitEntry*)mario->hitobjHead == NULL) {
                            sp2C = 5.0f + mario->wPlayerCollisionBox.y;
                            mario->hitobjHead = marioSearchHead(&sp68, &sp2C, 0.0f);
                        }
                        if (mario->controlStickSensitivity) {
                            s32 wall;
                            f32 move;

                            marioChgPose(str_M_W_6);
                            mario2 = marioGetPtr();
                            if (mario2->jabaraState == 0) {
                                mario2->jabaraState = 1;
                                mario2->jabaraTimer = 0xA;
                                mario2->jabaraSpeed = 0.0f;
                                ((JabaraWork*)marioGetPtr()->motStruct)->unk4 = 0.1f;
                                ((JabaraWork*)marioGetPtr()->motStruct)->unk8 = JABARA_F32(float_neg0p1_8041aadc);
                                ((JabaraWork*)marioGetPtr()->motStruct)->unk10 = JABARA_F32(float_0p18_8041aae0);
                            }
                            wall = 0;
                            move = JABARA_F32(float_1_8041ab94);
                            if ((HitEntry*)mario->hitobjHead != NULL &&
                                (hitGetAttr((HitEntry*)mario->hitobjHead) & 0x40000)) {
                                wall = 1;
                            }
                            if (wall != 0) {
                                f32 angle = mario->controlStickAngle;
                                mario2 = marioGetPtr();
                                f28 = mario2->position.x;
                                f30 = mario2->position.z;
                                f29 = JABARA_F32(float_2_8041ab6c) + (mario2->position.y + mario2->wPlayerCollisionBox.y);
                                sp1C = JABARA_F32(float_0p5_8041ab48) * mario2->wPlayerCollisionBox.x;
                                sincosf(&sp8, &spC, angle);
                                hit = hitCheckFilter(NULL, f28, f29, f30, sp8, 0.0f, spC, &sp10,
                                                     &sp14, &sp18, &sp1C, &sp20, &sp24, &sp28);
                                if (hit == NULL) {
                                    move = move * (f32)sin((JABARA_F32(float_3p1416_8041ab80) * mario->controlStickAngle) / 180.0f);
                                    mario->position.x = mario->position.x + move;
                                    if (move > 0.0f) {
                                        if (mario->unk1A8 >= JABARA_F32(float_90_8041aba0) && mario->unk1A8 <= JABARA_F32(float_270_8041aba4)) {
                                            mario->jabaraAnimTime = mario->jabaraAnimTime + 1;
                                            if ((s8)mario->jabaraAnimTime > 0x20) {
                                                mario->jabaraAnimTime = 0;
                                            }
                                        } else {
                                            mario->jabaraAnimTime = mario->jabaraAnimTime - 1;
                                            if ((s8)mario->jabaraAnimTime < 0) {
                                                mario->jabaraAnimTime = 0x20;
                                            }
                                        }
                                    } else {
                                        if (mario->unk1A8 >= JABARA_F32(float_90_8041aba0) && mario->unk1A8 <= JABARA_F32(float_270_8041aba4)) {
                                            mario->jabaraAnimTime = mario->jabaraAnimTime - 1;
                                            if ((s8)mario->jabaraAnimTime < 0) {
                                                mario->jabaraAnimTime = 0x20;
                                            }
                                        } else {
                                            mario->jabaraAnimTime = mario->jabaraAnimTime + 1;
                                            if ((s8)mario->jabaraAnimTime > 0x20) {
                                                mario->jabaraAnimTime = 0;
                                            }
                                        }
                                    }
                                    if ((s8)mario->jabaraAnimTime == 0) {
                                        psndSFXOn_3D((const char*)0x187, &mario->position);
                                    }
                                    if ((s32)mario->jabaraAnimTime == 0x10) {
                                        psndSFXOn_3D((const char*)0x187, &mario->position);
                                    }
                                }
                            }
                            marioUpdateCamCollision(mario->position);
                        }
                    }
                    break;
                }
                        switch (mario->currSubMotionId) {
                        case 0x5A:
                            marioChgPose(RSTR(0xA8, "M_J_1B"));
                            mario->currSubMotionId = 0x5B;
                            mario->dispFlags |= 4;
                            marioAdjustMoveDir();
                            mario->jumpVelY = JABARA_F32(float_4_8041ab68);
                            mario->jumpAccelY = JABARA_F32(float_neg0p5_8041aba8);
                            mario->jumpJerkY = 0.0f;
                            mario->jumpJerkStepY = 0.0f;
                            mario->currSubMotionId = 0x5B;
                            /* fallthrough */
                        case 0x5B:
                            mario->position.y = mario->position.y + mario->jumpVelY;
                            marioMakeJumpPara();
                            if (mario->jumpVelY <= 0.0f) {
                                marioChgMot(0xA);
                                marioSetFallPara();
                            }
                            break;
                        }
                        if (mario->currSubMotionId >= 0x46) {
                            swingMain();
                        }
            } else {
                return;
            }
    }
}

// .sdata2, after mot_jabara's pooled camera constants
__declspec(section ".sdata2") const f32 float_480_8041ab18 = 480.0f;
__declspec(section ".sdata2") const f32 float_50_8041ab1c = 50.0f;
__declspec(section ".sdata2") const f32 float_25_8041ab20 = 25.0f;
__declspec(section ".sdata2") const f32 float_200_8041ab24 = 200.0f;
__declspec(section ".sdata2") const f32 float_10p45_8041ab28 = 10.45f;
__declspec(section ".sdata2") const f32 float_0p278_8041ab2c = 0.278f;
__declspec(section ".sdata2") const f32 float_neg0p031_8041ab30 = -0.031f;
__declspec(section ".sdata2") const f32 float_8_8041ab34 = 8.0f;
__declspec(section ".sdata2") const f32 float_neg0p84_8041ab38 = -0.84f;
__declspec(section ".sdata2") const f32 float_0p08_8041ab3c = 0.08f;
__declspec(section ".sdata2") const f32 float_neg0p015_8041ab40 = -0.015f;
__declspec(section ".sdata2") const f32 float_4p3_8041ab44 = 4.3f;
__declspec(section ".sdata2") const f32 float_0p5_8041ab48 = 0.5f;
__declspec(section ".sdata2") const f32 float_100_8041ab4c = 100.0f;
__declspec(section ".sdata2") const f32 float_3p5_8041ab50 = 3.5f;
__declspec(section ".sdata2") const f32 float_9_8041ab54 = 9.0f;
__declspec(section ".sdata2") const f32 float_neg6_8041ab58 = -6.0f;
__declspec(section ".sdata2") const f32 float_neg450_8041ab5c = -450.0f;
__declspec(section ".sdata2") const f32 float_neg190_8041ab60 = -190.0f;
__declspec(section ".sdata2") const f32 float_neg8_8041ab64 = -8.0f;
__declspec(section ".sdata2") const f32 float_4_8041ab68 = 4.0f;
__declspec(section ".sdata2") const f32 float_2_8041ab6c = 2.0f;
__declspec(section ".sdata2") const char str_8041AB70[8] = "M_S_1";
__declspec(section ".sdata2") const f32 float_0p6_8041ab78 = 0.6f;
__declspec(section ".sdata2") const f32 float_neg0p6_8041ab7c = -0.6f;
__declspec(section ".sdata2") const f32 float_3p1416_8041ab80 = 3.1415927f;
__declspec(section ".sdata2") const f32 float_neg1_8041ab84 = -1.0f;
__declspec(section ".sdata2") const char str_M_S_4[8] = "M_S_4";
__declspec(section ".sdata2") const f32 float_10_8041ab90 = 10.0f;
__declspec(section ".sdata2") const f32 float_1_8041ab94 = 1.0f;
__declspec(section ".sdata2") const char str_M_W_6[8] = "M_W_6";
__declspec(section ".sdata2") const f32 float_90_8041aba0 = 90.0f;
__declspec(section ".sdata2") const f32 float_270_8041aba4 = 270.0f;
__declspec(section ".sdata2") const f32 float_neg0p5_8041aba8 = -0.5f;
__declspec(section ".sdata2") const char str_8041ABAC[8] = "M_I_Y";
const u32 gap_09_8041ABB4_sdata2 = 0;

BOOL marioChkJabara(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2 = marioGetPtr();
    s32 check;
    s32 out2;
    s32 out1;
    f32 out3;

    // Chained assignment keeps `check` and wMotionTimer in the same register (r0);
    // the inverted equality makes 1 the out-of-line branch, as in the original.
    if ((mario2->flags & MARIO_FLAG_IS_8_BIT) || (mario2->flags & MARIO_FLAG_PAPER_MODE)) {
        mario2->wMotionTimer = check = 0;
    } else {
        if (pouchCheckItem(8) == 0) {
            check = 0;
        } else {
            check = 1;
        }
    }
    if (check == 0) {
        return FALSE;
    }
    if (!(mario->buttonHeld & 0x100)) {
        if (!(mario->buttonHeld & 0x200)) {
            if (strcmp(mario->animName, str_8041ABAC) == 0 && mario->currMotionId == 0) {
                marioChgPose(str_8041AB70);
                marioVoiceGlareOff();
            }
            mario->wMotionTimer = 0;
        }
        return FALSE;
    }
    mario->wMotionTimer = mario->wMotionTimer + 1;
    if (mario->controlStickSensitivity) {
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

s32 marioPriCheckJabara(void) {
    MarioWork* mario = marioGetPtr();

    if ((mario->flags & MARIO_FLAG_IS_8_BIT) || (mario->flags & MARIO_FLAG_PAPER_MODE)) {
        mario->wMotionTimer = 0;
        return 0;
    }
    return pouchCheckItem(8) != 0;
}

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

#ifdef MUST_MATCH
#pragma defer_codegen reset
#endif
