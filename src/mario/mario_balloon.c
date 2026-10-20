#include "mario/mario_balloon.h"
#include "driver/casedrv.h"
#include "driver/effdrv.h"
#include "driver/hitdrv.h"
#include "driver/mobjdrv.h"
#include "driver/npcdrv.h"
#include "mario/mario.h"
#include "mario/mario_hit.h"
#include <string.h>
#include "eff/eff_mario_balloon.h"

// Prototypes not yet in headers (these functions are defined in driver/mobjdrv.c and driver/npcdrv.c,
// but no included header declares them). See REPORT.md.

// Raw-offset accessors for MarioWork fields that are still padding in mario/mario.h:
//   0x1DC, 0x1E0: two unnamed HitEntry* fields before hitobjStandOn (0x1E4).
//   0x260: EffectEntry* for the balloon effect.
//   0x264: NpcEntry* the balloon is attached to.
#define MW_U32(m, off) (*(u32*)((u8*)(m) + (off)))

// opt_propagation off: keeps the 0 in r31 across the marioGetPtr call, as the original does,
// instead of rematerialising it after the call.
#pragma opt_propagation off
void marioBaloonInit(void) {
    MarioWork* mario;
    u32 zero = 0;

    mario = marioGetPtr();
    MW_U32(mario, 0x260) = zero;
}
#pragma opt_propagation reset

void marioBalloonOn(s32 type) {
    MarioWork* mario = marioGetPtr();

    if (!(mario->dispFlags & 0x02000000)) {
        mario->dispFlags |= 0x02000000;
        MW_U32(mario, 0x260) = (u32)effMarioBalloonEntry(type, NULL);
    }
}

void marioBalloonOff(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->dispFlags & 0x02000000) {
        mario->dispFlags &= ~0x02000000;
        effDelete((EffectEntry*)MW_U32(mario, 0x260));
        MW_U32(mario, 0x260) = 0;
        MW_U32(mario, 0x264) = 0;
    }
}

void marioBalloonMain(void) {
    MarioWork* mario = marioGetPtr();
    s32 valid;
    void* hit;
    void* hit1;
    void* hit2;
    void* hit3;
    void* hit4;
    // Not set on the item-box path, yet read after it: the original does the same (it compares r29, which
    // only the hit and NPC paths assign). Initializing it changes the code, so only the matching build
    // leaves it unset.
#ifdef MUST_MATCH
    s32 balloonType;
#else
    s32 balloonType = 0;
#endif
    BOOL ret;
    CaseEntry* ce;
    MapObjectEntry* obj;
    s32 attr;

    if (mario->dispFlags & 0x04000000) {
        return;
    }

    ret = FALSE;
    {
        BOOL noKey = !marioChkKey();

        if (noKey) {
            marioBalloonOff();
            return;
        }
    }

    hit1 = (void*)MW_U32(mario, 0x1DC);
    valid = 0;
    if (hit1 != NULL) {
        attr = hitGetAttr((HitEntry*)hit1);
        if (attr & 0x00800000) {
            valid = 0;
        } else if ((u32)attr & 0x80000000) {
            if (mobjGetHint(mobjHitObjPtrToPtr((HitEntry*)hit1)) == 1) {
                valid = 1;
            }
        } else {
            ce = caseCheckHitObj(hit1);
            if (ce != NULL) {
                if (ce->flags & 4) {
                    if (ce->activeConditionId == 9) {
                        valid = 1;
                    }
                }
            }
        }
    }
    if (valid != 0) {
        balloonType = 0;
        ret = TRUE;
    }

    hit2 = (void*)MW_U32(mario, 0x1E0);
    valid = 0;
    if (hit2 != NULL) {
        attr = hitGetAttr((HitEntry*)hit2);
        if ((u32)attr & 0x80000000) {
            if (mobjGetHint(mobjHitObjPtrToPtr((HitEntry*)hit2)) == 1) {
                valid = 1;
            }
        } else {
            ce = caseCheckHitObj(hit2);
            if (ce != NULL) {
                if (ce->flags & 4) {
                    if (ce->activeConditionId == 9 || ce->activeConditionId == 0xC) {
                        valid = 1;
                    }
                }
            } else {
                valid = 0;
            }
        }
        if (marioChkBero((HitEntry*)hit2)) {
            valid = 0;
        }
    }
    if (valid != 0) {
        balloonType = 0;
        ret = TRUE;
    }

    hit3 = mario->hitobjStandOn;
    valid = 0;
    if (hit3 != NULL) {
        attr = hitGetAttr((HitEntry*)hit3);
        if (attr & 0x00800000) {
            valid = 0;
        } else {
            if ((u32)attr & 0x80000000) {
                if (mobjGetHint(mobjHitObjPtrToPtr((HitEntry*)hit3)) == 1) {
                    valid = 1;
                }
            } else {
                ce = caseCheckHitObj(hit3);
                if (ce != NULL) {
                    if (ce->flags & 4) {
                        if (ce->activeConditionId == 0xC || ce->activeConditionId == 5) {
                            valid = 1;
                        }
                    }
                } else {
                    valid = 0;
                }
            }
            if (marioChkBero((HitEntry*)hit3)) {
                valid = 0;
            }
        }
    }
    if (valid != 0) {
        balloonType = 0;
        ret = TRUE;
    }

    hit4 = (void*)MW_U32(mario, 0x1E0);
    if (hit4 != NULL) {
        valid = -1;
#ifdef MUST_MATCH
        // The original calls marioGetPtr() and discards the result here (the pointer already lives in r31).
        marioGetPtr();
#endif
        if (!((u32)hitGetAttr((HitEntry*)hit4) & 0x80000000)) {
            valid = -1;
        } else {
            obj = mobjHitObjPtrToPtr((HitEntry*)hit4);
            if (strcmp(obj->animName, "MOBJ_TreasureBox") == 0 || strcmp(obj->animName, "MOBJ_BigTreasureBox") == 0) {
                valid = 1;
                if (mobjCheckItemboxOpen(obj) != 1) {
                    valid = 0;
                }
            }
        }
        if (valid >= 0) {
            hit4 = (void*)MW_U32(mario, 0x1E0);
            valid = -1;
#ifdef MUST_MATCH
            // Second copy: the original again calls marioGetPtr() and discards the result.
            marioGetPtr();
#endif
            if (!((u32)hitGetAttr((HitEntry*)hit4) & 0x80000000)) {
                valid = -1;
            } else {
                obj = mobjHitObjPtrToPtr((HitEntry*)hit4);
                if (strcmp(obj->animName, "MOBJ_TreasureBox") == 0 || strcmp(obj->animName, "MOBJ_BigTreasureBox") == 0) {
                    valid = 1;
                    if (mobjCheckItemboxOpen(obj) != 1) {
                        valid = 0;
                    }
                }
            }
            ret = (BOOL)valid;
        }
    }

    hit = fbatNpcTalkCheck();
    if (hit != NULL) {
        if (!(((NpcEntry*)hit)->flags & 0x80000000)) {
            balloonType = ((NpcEntry*)hit)->balloonType;
            ret = TRUE;
        }
        if (MW_U32(mario, 0x264) != (u32)hit) {
            MarioWork* m2 = marioGetPtr();

            if (m2->dispFlags & 0x02000000) {
                m2->dispFlags &= ~0x02000000;
                effDelete((EffectEntry*)MW_U32(m2, 0x260));
                MW_U32(m2, 0x260) = 0;
                MW_U32(m2, 0x264) = 0;
            }
        }
    }

    if (!(mario->airTimer == 0 && !(mario->flags & 0x00100000) && !(mario->dispFlags & 0x01000000) &&
          (mario->currMotionId == 0 || mario->currMotionId == 1 || mario->currMotionId == 2))) {
        ret = FALSE;
    }

    if (ret != 0) {
        if (balloonType == 1) {
            MarioWork* m2 = marioGetPtr();

            if (m2->dispFlags & 0x02000000) {
                return;
            }
            m2->dispFlags |= 0x02000000;
            MW_U32(m2, 0x260) = (u32)effMarioBalloonEntry(balloonType, (NpcEntry*)hit);
            MW_U32(m2, 0x264) = (u32)hit;
            return;
        } else {
            MarioWork* m2 = marioGetPtr();

            if (!(m2->dispFlags & 0x02000000)) {
                m2->dispFlags |= 0x02000000;
                MW_U32(m2, 0x260) = (u32)effMarioBalloonEntry(balloonType, NULL);
            }
            if (hit != NULL) {
                MW_U32(mario, 0x264) = (u32)hit;
            }
            return;
        }
    } else {
        MarioWork* m2 = marioGetPtr();

        if (m2->dispFlags & 0x02000000) {
            m2->dispFlags &= ~0x02000000;
            effDelete((EffectEntry*)MW_U32(m2, 0x260));
            MW_U32(m2, 0x260) = 0;
            MW_U32(m2, 0x264) = 0;
        }
    }
}

BOOL chkBalloon(HitEntry* hit) {
    BOOL ret = FALSE;

    if (hit != NULL) {
        s32 attr = hitGetAttr(hit);

        if ((u32)attr & 0x80000000) {
            if (mobjGetHint(mobjHitObjPtrToPtr(hit)) == 1) {
                ret = TRUE;
            }
        } else {
            CaseEntry* ce = caseCheckHitObj(hit);

            if (ce != NULL) {
                if (ce->flags & 4) {
                    if (ce->activeConditionId == 9 || ce->activeConditionId == 4 || ce->activeConditionId == 0xC ||
                        ce->activeConditionId == 5) {
                        ret = TRUE;
                    }
                }
            } else {
                ret = FALSE;
            }
        }
        if (marioChkBero(hit)) {
            ret = FALSE;
        }
    }
    return ret;
}
