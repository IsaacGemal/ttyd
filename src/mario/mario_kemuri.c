#include "driver/effdrv.h"
#include "driver/hitdrv.h"
#include "eff/eff_kemuri.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "party.h"
#include "system.h"
#include <stdlib.h>
#include <string.h>
#include "koopa.h"
#include "mario/mario_sbr.h"
#include "mario/mario_kemuri.h"
#include "eff/eff_mugi.h"
#include "eff/n64/eff_kemuri7_n64.h"
#include "eff/n64/eff_water_damage_n64.h"
#include "eff/eff_ripple.h"

// The original build did not inline the effects helpers into their callers
// (kemuri_mugi/mario_kemuri_landon/kemuri are all large); force that here.
#pragma dont_inline on

// File-scope counters/state. Names are free: objdiff ignores local symbol names.
static s32 ct481 = 8;      // .sdata
static s32 cnt492 = 10;    // .sdata
static s32 ct520 = 4;      // .sdata
static s32 ct2521 = 10;    // .sdata
static s32 ct552 = 4;      // .sdata
static f32 dir551;         // .sbss
static s32 leftRight522;   // .sbss
static f32 dir519;         // .sbss
static s32 cnt491;         // .sbss
static s32 leftRight482;   // .sbss

// Prototypes not yet in headers.

// Forward declarations (functions are defined in reverse .text order so that
// Metrowerks emits them in the original order).

// The `flags` and `currMotionId == 0x1D` guards are one `||` condition in the
// original: mwcc then emits `bne body; b epilogue` for the second test instead
// of folding it to a single `beq epilogue`.
void mario_kemuri_80130474(void) {
    MarioWork* mario = marioGetPtr();
    s32 attr = 0;
    f32 cos;
    f32 sin;

    if ((u32)marioStGetSystemLevel() != 0) {
        return;
    }
    if ((mario->flags & 0x01000000) || mario->currMotionId == 0x1D) {
        return;
    }
    if (mario->currMotionId != 3) {
        if (mario->hitobjStandOn != NULL) {
            attr = hitGetAttr((HitEntry*)mario->hitobjStandOn);
        }
    } else {
        if (mario->hitobjJumpFrom != NULL) {
            attr = hitGetAttr((HitEntry*)mario->hitobjJumpFrom);
        }
    }
    if (attr & 0x1000) {
        kemuri_mugi();
        return;
    }
    if (attr & 0x100) {
        mario_kemuri_landon();
        return;
    }
    if (attr & 0x100000) {
        mario = marioGetPtr();
        if (mario->currMotionId != 2 && mario->currMotionId != 1) {
            ct481 = 8;
            return;
        }
        if (ct481++ > 8) {
            ct481 = 0;
            sincosf(&sin, &cos, 6.2831855f * reviseAngle(-mario->directionView) / 360.0f);
            effKemuri7N64Entry(
                mario->position.x + 0.2f * (mario->wPlayerCollisionBox.x * sin),
                mario->position.y + 0.5f,
                mario->position.z + 0.2f * (mario->wPlayerCollisionBox.x * cos),
                -mario->directionView, (f32)leftRight482);
            leftRight482 = !leftRight482;
        }
        return;
    }
    if (!(mario->flags & 0x01000000)) {
        if (kpaGetBodyStatus() <= 0 && mario->characterId != 1) {
            kemuri();
        }
    }
}

void mario_simple_kemuri(void) {
    MarioWork* mario = marioGetPtr();
    EffectEntry* effect = effKemuriEntry(0, mario->position.x, mario->position.y, mario->position.z, 0.0f);

    if (mario->flags & 0x02000000) {
        ((effKemuriData*)effect->userdata)->scale = 0.6f;
    }
}

void unk_80130374(void) {
    MarioWork* mario = marioGetPtr();
    HitEntry* hit;
    s32 attr;
    EffectEntry* effect;

    hit = (HitEntry*)mario->hitobjStandOn;
    if (hit == NULL) {
        return;
    }
    if (mario->trigFlags & 0x20000) {
        mario->trigFlags &= ~0x20000;
        return;
    }
    attr = hitGetAttr(hit);
    if (attr & 0x1000) {
        kemuri_mugi();
    } else if (attr & 0x100) {
        mario_kemuri_landon();
    } else {
        effect = effKemuriEntry(0, mario->position.x, mario->position.y, mario->position.z, 0.0f);
        if (mario->flags & 0x02000000) {
            ((effKemuriData*)effect->userdata)->scale = 0.6f;
        }
    }
}

void mario_kemuri_landon(void) {
    MarioWork* mario = marioGetPtr();
    s32 waterKind = 0;

    if (strcmp(gp->currentMapName, "mri_05") == 0) {
        waterKind = 10;
    }

    if (mario->currMotionId == 3 && mario->airTimer == 1 && cnt492 == 10) {
        effWaterDamageN64Entry(waterKind + 5, 30, mario->position.x, mario->position.y, mario->position.z, 1.0f);
        cnt492 = 0;
        effRippleEntry(0, mario->position.x, mario->position.y + 11.0f, mario->position.z);
    }

    cnt492++;
    if (cnt492 > 10) {
        cnt492 = 10;
    }

    // `!x`, not `== 0`: next to `== 1` that merges into `cmplwi 1; ble`.
    if (mario->currMotionId == 2 || mario->currMotionId == 1 || !mario->currMotionId || mario->currMotionId == 13) {
        if (mario->currMotionId == 13) {
            cnt491 = 0;
        }
        if (--cnt491 < 0) {
            cnt491 = rand() % 20 + 30;
            effRippleEntry(0, mario->position.x, mario->position.y + 11.0f, mario->position.z);
        }
        if (mario->currMotionId == 1 || mario->currMotionId == 2 || mario->currMotionId == 13) {
            if (gp->retraceCount % 20 == 0 || mario->currMotionId == 13) {
                effWaterDamageN64Entry(waterKind + 7, 30, mario->position.x, mario->position.y + 11.0f, mario->position.z, 1.0f);
            }
        }
    }
}

void kemuri_mugi(void) {
    MarioWork* mario = marioGetPtr();
    f32 sin;
    f32 cos;
    f32 x;
    f32 y;
    f32 z;
    f32 angle;

    if (strncmp(gp->currentMapName, "mri", 3) == 0) {
        if (mario->currMotionId == 0) {
            if (gp->retraceCount % 30 == 0) {
                effWaterDamageN64Entry(16, 30, mario->position.x, mario->position.y + 11.0f, mario->position.z, 0.8f);
            }
        }
        if (mario->currMotionId == 1 || mario->currMotionId == 2 || mario->currMotionId == 13) {
            if (gp->retraceCount % 20 == 0 || mario->currMotionId == 13) {
                effWaterDamageN64Entry(17, 30, mario->position.x, mario->position.y + 11.0f, mario->position.z, 0.8f);
            }
        }
        return;
    }

    if (mario->currMotionId == 3 && mario->airTimer == 1 && ct2521 == 10) {
        effMugiKemuriEntry(mario->position.x, mario->position.y + 14.0f, mario->position.z, dir519);
        dir519 = reviseAngle(35.0f + dir519);
        ct2521 = 0;
        return;
    }

    ct2521++;
    if (ct2521 > 10) {
        ct2521 = 10;
    }

    if (mario->currMotionId != 2 && mario->currMotionId != 1) {
        ct520 = 2;
        return;
    }

    if (ct520++ > 2) {
        ct520 = 0;
        sincosf(&sin, &cos, 6.2831855f * reviseAngle(-mario->directionView) / 360.0f);
        x = -0.4f * (mario->wPlayerCollisionBox.x * sin) + mario->position.x;
        y = mario->position.y + 3.0f;
        z = -0.4f * (mario->wPlayerCollisionBox.x * cos) + mario->position.z;
        angle = (-mario->directionView + (f32)(rand() % 10)) - 5.0f;
        effMugiKemuri2Entry(0, x, y, z, angle, (f32)leftRight522);
        leftRight522 = !leftRight522;
    }
}

// The two effKemuri2Entry calls must build their x/y/z arguments in separate
// locals in this order (x, y, z): folding them back into the call makes mwcc
// schedule the cos product before the sin product and pick different FPRs,
// which loses the match in both branches.
void kemuri(void) {
    MarioWork* mario = marioGetPtr();
    f32 sin;
    f32 cos;
    f32 fade;
    f32 dist;
    EffectEntry* effect;
    f32 x;
    f32 y;
    f32 z;

    if (mario->airTimerPrev != 0 && mario->hitobjStandOnPrev == NULL && mario->hitobjStandOn != NULL) {
        if (mario->trigFlags & 0x20000) {
            mario->trigFlags &= ~0x20000;
            return;
        }
        x = mario->position.x;
        y = mario->position.y;
        z = mario->position.z;
        if (mario->characterId == 2) {
            effect = effKemuriEntry(0, x, y, z + 10.0f, 0.0f);
            ((effKemuriData*)effect->userdata)->scale = kpaGetBaseScale(kpaGetLevel());
        } else {
            effKemuriEntry(0, x, y, z, dir551);
        }
        return;
    }

    if (mario->currMotionId != 2) {
        ct552 = 7;
        return;
    }

    if (mario->characterId != 2) {
        if (ct552++ > 6) {
            ct552 = 0;
            sincosf(&sin, &cos, 6.2831855f * reviseAngle(-mario->directionView) / 360.0f);
            x = mario->position.x + 0.2f * (mario->wPlayerCollisionBox.x * sin);
            y = mario->position.y + 1.5f;
            z = mario->position.z + 0.2f * (mario->wPlayerCollisionBox.x * cos);
            effect = effKemuri2Entry(0, x, y, z, sin, cos);
            if (mario->flags & 0x02000000) {
                ((effKemuri2Data*)effect->userdata)->scale = 0.6f;
            }
        }
    } else {
        if (--ct552 <= 0) {
            sincosf(&sin, &cos, 6.2831855f * reviseAngle(-mario->directionView) / 360.0f);
            sincosf(&sin, &cos, toMovedirSimple(180.0f + (kpaGetStageViewType() == 0 ? mario->dispReqDir : mario->unk1A8)));
            switch (kpaGetLevel()) {
                case 0:
                    ct552 = 4;
                    dist = 10.0f;
                    fade = 0.5f;
                    break;
                case 1:
                    ct552 = 7;
                    dist = 15.0f;
                    fade = 1.0f;
                    break;
                case 2:
                    ct552 = 10;
                    dist = 20.0f;
                    fade = 2.0f;
                    break;
                case 3:
                    ct552 = 10;
                    dist = 50.0f;
                    fade = 4.0f;
                    break;
            }
            x = mario->position.x + dist * sin;
            y = mario->position.y + 1.5f;
            z = mario->position.z + dist * cos;
            effect = effKemuri2Entry(0, x, y, z, sin, cos);
            ((effKemuri2Data*)effect->userdata)->scale = fade;
        }
    }
}

void party_kemuri(PartyEntry* party) {
    if (party->field_32 == 1) {
        return;
    }

    if (party->field_34 == 3 && party->currentMemberId == PARTY_MEMBER_YOSHI) {
        f32 cos;
        f32 sin;
        s8 count;

        if (party->groundHit != NULL) {
            if (party->speed < 2.0f) {
                party->field_2C = 7;
            } else {
                count = party->field_2C;
                party->field_2C = count + 1;
                if (count > 6) {
                    party->field_2C = 0;
                    sincosf(&sin, &cos, 6.2831855f * reviseAngle(-party->dir) / 360.0f);
                    effKemuri2Entry(0, party->position.x + 0.2f * ((party->radius) * sin), party->position.y + 1.5f,
                                    party->position.z + 0.2f * ((party->radius) * cos), sin, cos);
                }
            }
        } else {
            party->field_2C = 4;
        }
    } else if (party->field_36 == 1) {
        f32 cos;
        f32 sin;
        s8 count;

        if (party->groundHit != NULL) {
            if (party->speed < 2.0f) {
                party->field_2C = 7;
            } else {
                count = party->field_2C;
                party->field_2C = count + 1;
                if (count > 6) {
                    party->field_2C = 0;
                    sincosf(&sin, &cos, 6.2831855f * reviseAngle(-party->dir) / 360.0f);
                    effKemuri2Entry(0, party->position.x + 0.2f * ((party->radius) * sin), party->position.y + 1.5f,
                                    party->position.z + 0.2f * ((party->radius) * cos), sin, cos);
                }
            }
        } else {
            party->field_2C = 4;
        }
    }
}
