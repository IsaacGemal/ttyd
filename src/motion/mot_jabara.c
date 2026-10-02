#include "motion/mot_jabara.h"
#include "dolphin/types.h"
#include "dolphin/mtx.h"
#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "mario/mario_pouch.h"
#include "mario/mario_sbr.h"
#include "motion/mot_stay.h"
#include "party.h"
#include "memory.h"
#include "matching.h"
#include <math.h>
#include <string.h>

// Backing struct for `mario->motStruct` while the jabara motion runs; the fields
// are written by swingMain and read here. Not described by any other header.
typedef struct JabaraWork {
    u32 flags;   // 0x0
    f32 unk4;    // 0x4
    f32 unk8;    // 0x8
    u8 pad[4];   // 0xC
    f32 unk10;   // 0x10
} JabaraWork;

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
const u32 gap_04_802C5C24_rodata = 0;
const f64 double_1p5708_802c5c28 = 1.5707963705062866;
const char str_802C5C30[] = "M_J_1B";
const f64 double_to_int_802c5c38 = 4503601774854144.0;

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
const char str_8041AB70[] = "M_S_1";
const f32 float_0p6_8041ab78 = 0.6f;
const f32 float_neg0p6_8041ab7c = -0.6f;
const f32 float_3p1416_8041ab80 = 3.1415927f;
const f32 float_neg1_8041ab84 = -1.0f;
const char str_8041AB88[] = "M_S_4";
const f32 float_10_8041ab90 = 10.0f;
const f32 float_1_8041ab94 = 1.0f;
const char str_8041AB98[] = "M_W_6";
const f32 float_90_8041aba0 = 90.0f;
const f32 float_270_8041aba4 = 270.0f;
const f32 float_neg0p5_8041aba8 = -0.5f;
const char str_8041ABAC[] = "M_I_Y";
const u32 gap_09_8041ABB4_sdata2 = 0;

void mot_grasp(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1u;
        mario->flags &= ~(MARIO_FLAG_IS_JUMPING | MARIO_FLAG_IS_FALLING | MARIO_FLAG_IS_STEPPING |
                          MARIO_FLAG_HAS_INPUT_JUMP);
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

