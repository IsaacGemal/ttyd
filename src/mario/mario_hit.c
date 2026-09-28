#include "mario/mario_hit.h"
#include "driver/hitdrv.h"
#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "party/party_vivian.h"
#include "system.h"
#include <string.h>
#include "koopa.h"
#include "party.h"
#include "driver/mobjdrv.h"
#include "mario/mario_cam.h"
#include "mario/mario_sbr.h"
#include "mario/mario_party.h"
#include "motion/mot_damage.h"
#include "koopa_hit.h"
#include "koopa_motion.h"
#include "motion/mot_jabara.h"
#include "pmario_sound.h"
#include "math_ext.h"
#include <dolphin/mtx.h>
#include "motion/mot_slit.h"
#include "mario/mariost.h"

// locals for symbols with no home header

// floats with no home header
extern const f32 float_0_8041a7a4;
extern const f32 float_1000_8041a7a0;
extern const f32 float_1_8041a7bc;
extern const f32 float_0p5_8041a7c4;
extern const f32 float_neg0p5_8041a7c8;
extern const f32 float_0p1_8041a7e4;
extern const f32 float_neg10_8041a804;
extern const f32 float_6_8041a88c;
extern const f32 float_18_8041a850;

void* marioChkLandon(f32* pos, f32 spd) {
    MarioWork* wp = marioGetPtr();
    f32 outY;
    f32 outX;

    if (wp->characterId != 2) {
        if ((wp->flags & 0x1000000) == 0) {
            return (void*)marioSearchGround(pos, &outY, &outX, spd, spd);
        } else {
            return (void*)marioSearchGroundRoll(pos, &outY, &outX, spd, spd);
        }
    } else {
        return marioLandonChk_old(pos, spd);
    }
}

void marioChkToge(void) {
    MarioWork* wp = marioGetPtr();
    HitEntry* hit;
    s32 attr;

    if (vivianGetStatus() == 2 || vivianGetStatus() == 1 || vivianGetStatus() == 3) {
        return;
    }
    hit = (HitEntry*)*(void**)((u8*)wp + 0x1E0);
    if (hit != NULL) {
        attr = hitGetAttr(hit);
        if (wp->currMotionId != 0x20 && (attr & 0x800) != 0 &&
            *(f32*)&wp->field_0x1F0[0x20] < float_0p5_8041a7c4 * wp->wPlayerCollisionBox.x) {
            set_damage_root_ypos(wp->position.y);
            marioChgMot(0x20);
        }
    }

    hit = (HitEntry*)*(void**)((u8*)wp + 0x1F0);
    if (hit != NULL) {
        attr = hitGetAttr(hit);
        if (wp->currMotionId != 0x20 && (attr & 0x800) != 0) {
            set_damage_root_ypos(wp->position.y);
            marioChgMot(0x20);
        }
    }

    hit = (HitEntry*)*(void**)((u8*)wp + 0x1F4);
    if (hit != NULL) {
        attr = hitGetAttr(hit);
        if (wp->currMotionId != 0x20 && (attr & 0x800) != 0) {
            set_damage_root_ypos(wp->position.y);
            marioChgMot(0x20);
        }
    }
}

void marioChkGnd2(void) {
    MarioWork* wp = marioGetPtr();
    HitEntry* hit;
    f32 outX;
    f32 outY;
    f32 outZ;
    f32 dy;

    if (wp->flags & 0x10000) {
        return;
    }

    if (wp->characterId != 2) {
        hit = (HitEntry*)marioSearchGround(&outY, &outX, &outZ, float_0p1_8041a7e4, *(f32*)((u8*)wp + 0x7C));
    } else {
        hit = (HitEntry*)kpaSearchGround(&outY, &outX, &outZ, float_0p1_8041a7e4);
    }

    if (hit == NULL) {
        return;
    }

    dy = wp->position.y - outY;
    if (dy < float_0_8041a7a4 && dy >= float_neg10_8041a804) {
        wp->hitobjStandOn = hit;
        wp->position.y = outY;
        wp->flags &= ~0xC0;
        wp->airTimer = 0;
        *(s16*)((u8*)wp + 0x52) = 0;

        if (wp->characterId == 2) {
            if (wp->controlStickSensitivity != float_0_8041a7a4 || kpaFireAttackCheck() == 0) {
                if (*(s8*)((u8*)wp + 0x24E) * *(s8*)((u8*)wp + 0x24E) +
                        *(s8*)((u8*)wp + 0x24F) * *(s8*)((u8*)wp + 0x24F) >
                    0xBD1) {
                    marioChgMot(2);
                } else {
                    marioChgMot(1);
                }
            } else {
                marioChgMot(0);
            }
            quake_kpaLandOn();
        } else {
            marioChgMot(0);
        }

        if (wp->characterId != 2) {
            if (marioBgmodeChk() == 0) {
                camFollowYOn();
            }
        } else {
            kpaCamFollow();
        }
    }
}

BOOL marioChkBero(HitEntry* hit) {
    s32 attr = hitGetAttr(hit);
    MarioWork* wp = marioGetPtr();

    if ((attr & 0x80000) != 0 && (wp->flags & 0x1000000) == 0) {
        return FALSE;
    }
    if ((attr & 0x80) == 0) {
        return (attr & 0x800000) != 0;
    }
    return wp->currMotionId == 0x19;
}
void clear_hitobj_ride(void) {
    MarioWork* wp = marioGetPtr();

    wp->hitobjStandOn = NULL;
    if (wp->characterId == 2) {
        kpaClearHitobjRide();
    }
}

void marioHitCheckWidth(void) {
    MarioWork* wp = marioGetPtr();

    if (!(wp->flags & 0x100000)) {
        f32 target;
        f32 z;

        if ((wp->flags & 0x1000000) == 0) {
            target = wp->wPlayerCollisionBox.x;
        } else {
            target = float_18_8041a850;
        }
        z = wp->wPlayerCollisionBox.z + float_1_8041a7bc;
        wp->wPlayerCollisionBox.z = z;
        if (z > target) {
            wp->wPlayerCollisionBox.z = target;
        }
    } else {
        if (wp->wPlayerCollisionBox.z > float_6_8041a88c) {
            f32 z = wp->wPlayerCollisionBox.z - float_1_8041a7bc;

            wp->wPlayerCollisionBox.z = z;
            if (z < float_6_8041a88c) {
                wp->wPlayerCollisionBox.z = float_6_8041a88c;
            }
        }
        if (wp->wPlayerCollisionBox.z < float_6_8041a88c) {
            f32 z = wp->wPlayerCollisionBox.z + float_1_8041a7bc;

            wp->wPlayerCollisionBox.z = z;
            if (z > float_6_8041a88c) {
                wp->wPlayerCollisionBox.z = float_6_8041a88c;
            }
        }
    }
}

BOOL chkFilterAttr2(HitCheckQuery* query, HitEntry* entry) {
    return (entry->attributes & query->user0) == 0 || (entry->attributes & query->user1) != 0;
}

// The original addresses these .rodata objects as offsets from one base symbol.
#ifdef MUST_MATCH
#define RO(base, off, sym) ((const char*)(base) + (off))
#else
extern const char str_802C5950[];
extern const char str_802C5964[];
extern const char str_802C5974[];
extern const char str_802C5988[];
extern const char str_802C59A4[];
extern const char str_802C59BC[];
extern const char str_802C59D4[];
extern const Vec vec3_802c588c;
extern const Vec vec3_802c58a4;
#define RO(base, off, sym) ((const char*)&(sym))
#endif


// strings/rodata with no home header (addressed from the rodata base symbol)
extern const u8 vec3_802c5730[];
extern const u8 vec3_802c573c[];

BOOL chkfilterVecVivian(HitCheckQuery* query, HitEntry* entry) {
    s32 attr = entry->attributes;
    const u8* base = vec3_802c5730;
    u8* obj;

    if ((attr & 0x80804) != 0 && (attr & 0x800000) == 0) {
        return FALSE;
    }
    if ((attr & 0x80000000) == 0) {
        return TRUE;
    }
    obj = (u8*)mobjHitObjPtrToPtr(entry);
    if (strcmp((const char*)(obj + 0x15), RO(base, 0x274, str_802C59A4)) == 0 ||
        strcmp((const char*)(obj + 0x15), RO(base, 0x28C, str_802C59BC)) == 0 ||
        strcmp((const char*)(obj + 0x15), RO(base, 0x2A4, str_802C59D4)) == 0) {
        mobjHitObjPtrToPtr(entry);
        return FALSE;
    }
    return TRUE;
}

BOOL chkfilterVecRoll(HitCheckQuery* query, HitEntry* entry) {
    s32 attr = entry->attributes;
    const u8* base = vec3_802c5730;
    u8* obj;

    if ((attr & 0x80) != 0) {
        return TRUE;
    }
    if ((attr & 0x800000) == 0 && (attr & 0x80004) != 0) {
        return FALSE;
    }
    if ((attr & 0x80000000) == 0) {
        return TRUE;
    }
    obj = (u8*)mobjHitObjPtrToPtr(entry);
    if (strcmp((const char*)(obj + 0x15), RO(base, 0x274, str_802C59A4)) == 0 ||
        strcmp((const char*)(obj + 0x15), RO(base, 0x28C, str_802C59BC)) == 0 ||
        strcmp((const char*)(obj + 0x15), RO(base, 0x2A4, str_802C59D4)) == 0) {
        mobjHitObjPtrToPtr(entry);
        return FALSE;
    }
    return TRUE;
}

BOOL chkfilterVec(HitCheckQuery* query, HitEntry* entry) {
    s32 attr = entry->attributes;
    const u8* base = vec3_802c5730;
    u8* obj;

    if ((attr & 0x80080) != 0) {
        return TRUE;
    }
    if ((attr & 0x800000) == 0 && (attr & 0x4) != 0) {
        return FALSE;
    }
    if ((attr & 0x80000000) == 0) {
        return TRUE;
    }
    obj = (u8*)mobjHitObjPtrToPtr(entry);
    if (strcmp((const char*)(obj + 0x15), RO(base, 0x274, str_802C59A4)) == 0 ||
        strcmp((const char*)(obj + 0x15), RO(base, 0x28C, str_802C59BC)) == 0 ||
        strcmp((const char*)(obj + 0x15), RO(base, 0x2A4, str_802C59D4)) == 0) {
        mobjHitObjPtrToPtr(entry);
        return FALSE;
    }
    return TRUE;
}

// prototypes for in-file functions

// NONMATCHING (99.23%): CSEs wp+0x210 into an extra callee-saved register (r30)
// where the original recomputes addi r3,r31,0x210 per dou10_yoko_yari, dragging
// r28/r29/r30 through the function.
void marioCheckWall(f32 a, f32 b) {
    MarioWork* wp = marioGetPtr();
    HitEntry* hit;
    Vec v;
    f32 f8;
    s32 r;

    *(f32*)&wp->field_0x1F0[0x20] = float_1000_8041a7a0;
    if (a != float_0_8041a7a4 && wp->airTimer == 0) {
        if (*(f32*)((u8*)wp + 0x1C8) < a) {
            *(f32*)((u8*)wp + 0x1C8) = a;
            *(f32*)((u8*)wp + 0x1CC) = b;
            *(s32*)((u8*)wp + 0x1D0) = 0xE;
        } else {
            s32 t = *(s32*)((u8*)wp + 0x1D0) - 1;

            *(s32*)((u8*)wp + 0x1D0) = t;
            if (t <= 0) {
                *(s32*)((u8*)wp + 0x1D0) = 0;
                *(f32*)((u8*)wp + 0x1C8) = a;
                *(f32*)((u8*)wp + 0x1CC) = b;
            }
        }
        *(f32*)((u8*)wp + 0x19C) = *(f32*)((u8*)wp + 0x1CC);
    } else {
        *(f32*)((u8*)wp + 0x1C8) = float_0_8041a7a4;
    }

    if (a == float_0_8041a7a4) {
        marioSearchFrontWall_staying(b);
        dou10_yoko_yari((f32*)&wp->field_0x1F0[0x20]);
        return;
    }

    v = wp->position;
    *(s32*)((u8*)wp + 0x20C) = 0;
    hit = searchFrontWall(&v, &f8, a, b, wp->wPlayerCollisionBox.z);
    if (hit != NULL) {
        if (*(void**)((u8*)wp + 0x20C) != NULL) {
            *(void**)((u8*)wp + 0x1E0) = *(void**)((u8*)wp + 0x20C);
        } else {
            *(void**)((u8*)wp + 0x1E0) = hit;
        }
        if (marioChkBero(hit)) {
            if (a != float_0_8041a7a4) {
                wp->hitobjPush = hit;
                *(void**)((u8*)wp + 0x1F0) = hit;
            }
            movePos(&wp->position.x, &wp->position.z, a, b);
        } else {
            s32 t;

            *(f32*)&wp->field_0x1F0[0x20] = f8;
            *(void**)((u8*)wp + 0x1F0) = hit;
            t = wp->field_0x3E[1] + 1;
            wp->field_0x3E[1] = t;
            if ((s8)t >= 0x14) {
                wp->field_0x3E[1] = 0x14;
                if ((hitGetAttr(hit) & 0x80) == 0) {
                    r = 1;
                } else {
                    MarioWork* wp2 = marioGetPtr();

                    r = wp2->currMotionId == 0x19;
                }
                if (r != 0) {
                    wp->hitobjPush = hit;
                }
            }
            wp->position = v;
        }
    } else {
        wp->position = v;
    }
    marioChkWallAround(&wp->position, 0, a, b, wp->wPlayerCollisionBox.z, wp->wPlayerCollisionBox.y);
    dou10_yoko_yari((f32*)&wp->field_0x1F0[0x20]);
}


extern const f32 float_5_8041a7b8;
extern const f32 float_neg1_8041a7dc;
extern const f32 float_0p1_8041a7e4;

f32 marioChkOverheadJabara(s32* out, f32* outF) {
    MarioWork* wp = marioGetPtr();
    Vec v;
    f32 f8;

    *out = 0;
    *outF = float_neg1_8041a7dc;
    v = wp->position;
    f8 = wp->wPlayerCollisionBox.y;
    if (wp->currMotionId == 8) {
        return v.y;
    }
    if (jabaraNoHitChk() == 0) {
        f8 = f8 + float_5_8041a7b8;
        *(void**)((u8*)wp + 0x1F4) = marioSearchHead_jabara(&v, &f8, out, float_0_8041a7a4);
        if (*(void**)((u8*)wp + 0x1F4) == NULL) {
            f8 = wp->wPlayerCollisionBox.y + float_5_8041a7b8;
            *(void**)((u8*)wp + 0x1F4) = marioSearchHead(&v, &f8, float_0_8041a7a4);
        }
    }
    wp->position.x = v.x;
    if (*(void**)((u8*)wp + 0x1F4) != NULL) {
        wp->flags |= 0x08000000;
        v.y = v.y - wp->wPlayerCollisionBox.y - float_0p1_8041a7e4;
        if (wp->currMotionId == 3) {
            marioChgMot(0);
        }
    } else {
        if (f8 > float_0_8041a7a4) {
            *outF = f8;
            wp->flags |= 0x08000000;
        } else {
            wp->flags &= ~0x08000000;
        }
    }
    return v.y;
}

extern const Vec vec3_802c5820;
extern const f32 float_180_8041a7cc;
extern const f32 float_3p1416_8041a7e0;
extern const f32 float_neg23p7_8041a834;
extern const f32 float_5p8_8041a838;

// NONMATCHING (87.30%): 2 instructions differ, 8 missing, 16 extra, plus register differences.
void* marioSearchHead_jabara(Vec* pos, f32* outY, s32* out, f32 a) {
    MarioWork* wp = marioGetPtr();
    Vec p;
    Vec up = vec3_802c5820;
    Vec out1;
    Vec out2;
    f32 outF;
    f32 a2;
    f32 y;
    f32 radius;
    HitEntry* hit;
    s32 i;

    *out = 0;
    a2 = *outY;
    radius = float_neg23p7_8041a834;
    *outY = float_neg1_8041a7dc;
    y = a2 + a;

    for (i = 0; i < 8; i++) {
        outF = y;
        p.x = pos->x + radius * (f32)sin(float_3p1416_8041a7e0 * wp->targetYaw / float_180_8041a7cc);
        p.z = pos->z - radius * (f32)cos(float_3p1416_8041a7e0 * wp->targetYaw / float_180_8041a7cc);
        p.y = float_0p5_8041a7c4 + pos->y;
        hit = marioHitCheck2(&p, &up, &out1, &out2, &outF);
        if (hit != NULL) {
            if (hitGetAttr(hit) & 0x40000) {
                if (pos->y + a2 >= out1.y) {
                    pos->y = out1.y;
                    *outY = float_0_8041a7a4;
                    *out = (p.z - pos->z < float_0_8041a7a4) ? -1 : 1;
                    return hit;
                } else {
                    *outY = (out1.y - pos->y) - a2;
                }
            } else if (i == 3) {
                if (pos->y + a2 >= out1.y) {
                    pos->y = out1.y;
                    *outY = float_0_8041a7a4;
                    *out = 0;
                    return hit;
                } else {
                    *outY = (out1.y - pos->y) - a2;
                }
            }
        }
        radius = radius + float_5p8_8041a838;
    }
    return NULL;
}

extern const f32 float_9_8041a83c;
extern const f32 float_3_8041a7f0;
extern const f32 float_90_8041a81c;
extern const Vec vec3_802c5808;
extern const Vec vec3_802c5814;

// NONMATCHING (83.62%): stack frame 0x100 vs 0x110; one extra callee-saved GPR; the dead
// ours and removed, and the query/out1 locals land at different offsets.
void* marioSearchHead_roll(Vec* pos, f32* outY, f32 a) {
    MarioWork* wp = marioGetPtr();
    f32 ox;
    f32 oz;
    Vec dir;
    Vec p;
    Vec out2;
    Vec out1;
    HitCheckQuery query;
    HitEntry* hit;
    f32 y;
    f32 dist;
    s32 i;

    *outY = float_neg1_8041a7dc;
    y = wp->wPlayerCollisionBox.y;
    dist = y + a;
    for (i = 0; i < 5; i++) {
        f32 rad;

        switch (i) {
        case 0:
            ox = float_0_8041a7a4;
            oz = float_0_8041a7a4;
            break;
        case 1:
            rad = float_3p1416_8041a7e0 * (float_90_8041a81c + wp->directionView) / float_180_8041a7cc;
            ox = float_9_8041a83c * (f32)sin(rad);
            oz = float_9_8041a83c * -(f32)cos(rad);
            movePos(&ox, &oz, float_3_8041a7f0, wp->directionView);
            break;
        case 2:
            rad = float_3p1416_8041a7e0 * (wp->directionView - float_90_8041a81c) / float_180_8041a7cc;
            ox = float_9_8041a83c * (f32)sin(rad);
            oz = float_9_8041a83c * -(f32)cos(rad);
            movePos(&ox, &oz, float_3_8041a7f0, wp->directionView);
            break;
        case 3:
            rad = float_3p1416_8041a7e0 * (float_90_8041a81c + wp->directionView) / float_180_8041a7cc;
            ox = float_9_8041a83c * (f32)sin(rad);
            oz = float_9_8041a83c * -(f32)cos(rad);
            movePos(&ox, &oz, float_3_8041a7f0, float_180_8041a7cc + wp->directionView);
            break;
        case 4:
            rad = float_3p1416_8041a7e0 * (wp->directionView - float_90_8041a81c) / float_180_8041a7cc;
            ox = float_9_8041a83c * (f32)sin(rad);
            oz = float_9_8041a83c * -(f32)cos(rad);
            movePos(&ox, &oz, float_3_8041a7f0, float_180_8041a7cc + wp->directionView);
            break;
        }
        dir = vec3_802c5814;
        p.x = pos->x + ox;
        p.y = pos->y + float_1_8041a7bc;
        p.z = pos->z + oz;
        query.targetPos = p;
        query.targetDir = dir;
        query.targetDistance = dist;

        if (marioGetPtr()->currMotionId == 0x1C) {
            hit = hitCheckVecFilter(&query, chkfilterVecVivian);
        } else if (marioGetPtr()->flags & 0x1000000) {
            hit = hitCheckVecFilter(&query, chkfilterVecRoll);
        } else {
            hit = hitCheckVecFilter(&query, chkfilterVec);
        }
        if (hit != NULL) {
            out1 = query.hitPos;
            out1.x = (f32)(s32)(float_1000_8041a7a0 * out1.x +
                                (out1.x >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
                      float_1000_8041a7a0;
            out1.y = (f32)(s32)(float_1000_8041a7a0 * out1.y +
                                (out1.y >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
                      float_1000_8041a7a0;
            out1.z = (f32)(s32)(float_1000_8041a7a0 * out1.z +
                                (out1.z >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
                      float_1000_8041a7a0;
            out2 = query.hitNormal;
            if (pos->y + y >= out1.y) {
                pos->y = out1.y - y - float_1_8041a7bc;
                *outY = float_0_8041a7a4;
                return hit;
            } else {
                f32 d = (out1.y - pos->y) - y;

                if (*outY < float_0_8041a7a4) {
                    *outY = d;
                } else if (*outY > d) {
                    *outY = d;
                }
            }
        }
    }
    return NULL;
}

extern const f32 float_100_8041a7f8;
extern const f32 float_0p05_8041a848;
extern const f32 float_75_8041a840;
extern const f32 float_50_8041a844;
extern const Vec vec3_802c57fc;

// NONMATCHING (56.43%): stack/register layout differs; the dead sqrtf block is omitted.
void* marioSearchHead(Vec* pos, f32* outY, f32 a) {
    MarioWork* wp = marioGetPtr();
    Vec p;
    Vec up;
    Vec hitPos;
    Vec normal;
    HitEntry* hit;
    f32 x0;
    f32 y0;
    f32 f25;
    f32 f24;
    f32 f27;
    f32 f28;
    f32 f26;
    f32 f22;
    f32 rad;
    f32 angle;
    f32 s;
    f32 c;
    f32 dist;
    s32 i;

    if (wp->baseSpeed != float_0_8041a7a4) {
        angle = wp->directionView;
    } else {
        angle = toMovedir(wp->unk1A8);
    }
    rad = float_3p1416_8041a7e0 * angle / float_180_8041a7cc;
    s = (f32)sin(rad);
    c = (f32)cos(rad);
    x0 = wp->wPlayerCollisionBox.x - float_0p5_8041a7c4;
    f25 = float_0p5_8041a7c4 * (x0 * s);
    f24 = float_0p5_8041a7c4 * (x0 * -c);
    f27 = -f25;
    f28 = -f24;
    y0 = *outY;
    f26 = float_0p5_8041a7c4 * y0;
    f22 = y0 + a;
    *outY = float_neg1_8041a7dc;
    up = vec3_802c57fc;
    for (i = 0; i < 5; i++) {
        f32 ox;
        f32 oz;

        switch (i) {
        case 0:
            ox = float_0_8041a7a4;
            oz = float_0_8041a7a4;
            break;
        case 1:
            ox = f25;
            oz = f24;
            break;
        case 2:
            ox = f28;
            oz = f25;
            break;
        case 3:
            ox = f24;
            oz = f27;
            break;
        case 4:
            ox = f27;
            oz = f28;
            break;
        }
        p.x = pos->x + ox;
        p.y = pos->y + f26;
        p.z = pos->z + oz;
        dist = f22;
        hit = marioHitCheck2(&p, &up, &hitPos, &normal, &dist);
        if (hit != NULL) {
            MarioWork* wp2 = marioGetPtr();
            f32 ang;

            *(f32*)&wp2->field_0x1F0[0x28] =
                (f32)(s32)(float_100_8041a7f8 * (angleABf(float_0_8041a7a4, float_0_8041a7a4,
                                                            float_100_8041a7f8 * normal.z,
                                                            float_100_8041a7f8 * normal.y) -
                                                  float_180_8041a7cc) +
                          float_0p5_8041a7c4) /
                float_100_8041a7f8;
            *(f32*)&wp2->field_0x1F0[0x2C] = float_0_8041a7a4;
            *(f32*)&wp2->field_0x1F0[0x30] =
                (f32)(s32)(float_100_8041a7f8 * (angleABf(float_0_8041a7a4, float_0_8041a7a4,
                                                            float_100_8041a7f8 * normal.x,
                                                            float_100_8041a7f8 * normal.y) -
                                                  float_180_8041a7cc) +
                          float_0p5_8041a7c4) /
                float_100_8041a7f8;
            ang = angleABf(float_0_8041a7a4, float_0_8041a7a4, normal.x, -normal.y);
            if (!(wp->flags & 0x1000000)) {
                f32 lim = (wp->dispFlags & 0x1000000) ? float_75_8041a840 : float_50_8041a844;

                if (ang >= float_180_8041a7cc - lim && ang <= lim) {
                    hit = NULL;
                }
            }
        }
        if (wp->flags & 0x1000000) {
            if (hit != NULL) {
                f32 y = (f32)(s32)(float_100_8041a7f8 * hitPos.y + f26) / float_100_8041a7f8;

                y = y - float_0p05_8041a848 * y0;
                hitPos.y = y;
                if (pos->y + y0 >= y) {
                    pos->y = y;
                    *outY = float_0_8041a7a4;
                    return hit;
                } else {
                    *outY = (y - pos->y) - y0;
                    return NULL;
                }
            }
        } else {
            if (hit != NULL) {
                f32 y = (f32)(s32)(float_100_8041a7f8 * hitPos.y + f26) / float_100_8041a7f8;

                hitPos.y = y;
                if (pos->y + y0 >= y) {
                    pos->y = y;
                    *outY = float_0_8041a7a4;
                    return hit;
                } else {
                    *outY = (y - pos->y) - y0;
                    return NULL;
                }
            }
        }
    }
    return NULL;
}

extern const char str_802C5940[];
extern const Vec vec3_802c5874;
extern const f32 float_60_8041a808;
extern const f32 float_100_8041a7f8;
extern const f32 float_0p8_8041a80c;
extern const f32 float_20_8041a810;
extern const f32 float_8_8041a814;
extern const f32 float_270_8041a818;
extern const f32 float_90_8041a81c;
extern const Vec vec3_802c5868;
extern const f32 float_2_8041a7e8;
extern const f32 float_10_8041a7fc;
extern const f32 float_40_8041a800;

// NONMATCHING (59.03%): original keeps v.y in f31 across movePos (0xa0 frame), builds
// targetPos before sincosf, query at 0x48; ours 0x80 and different layout. Dir is
// {0,0,1} (sincosf called with 0.0). 0x1E0 is unnamed MarioWork padding.
HitEntry* N_dou10_yoko_yari3(void) {
    const u8* base = vec3_802c5730;
    MarioWork* wp = marioGetPtr();
    Vec v;
    Vec dir;
    HitCheckQuery query;
    f32 sn;
    f32 cs;
    HitEntry* hit;

    if (strcmp(gp->currentMapName, RO(base, 0x210, str_802C5940)) != 0) {
        return NULL;
    }

    v = wp->position;
    movePos(&v.x, &v.z, float_10_8041a7fc, wp->directionView);
    v.y = v.y + float_5_8041a7b8;
    query.targetPos = v;
    sincosf(&sn, &cs, float_0_8041a7a4);
    dir = *(const Vec*)(RO(base, 0x15C, vec3_802c588c));
    dir.x = sn;
    dir.z = cs;
    query.targetDir = dir;
    query.targetDistance = float_40_8041a800;
    hit = hitCheckVecFilter(&query, NULL);
    if (hit != NULL) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        return hit;
    }
    v = wp->position;
    movePos(&v.x, &v.z, float_neg10_8041a804, wp->directionView);
    v.y = v.y + float_5_8041a7b8;
    query.targetPos = v;
    sincosf(&sn, &cs, float_0_8041a7a4);
    dir = *(const Vec*)(RO(base, 0x174, vec3_802c58a4));
    dir.x = sn;
    dir.z = cs;
    query.targetDir = dir;
    query.targetDistance = float_40_8041a800;
    hit = hitCheckVecFilter(&query, NULL);
    if (hit != NULL) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
    }
    return hit;
}

// NONMATCHING (78.97%): 0xc0 frame in the original vs 0xb0 here (toMovedir's result
// is dead in both); pos/hitPos copies and the round blocks schedule differently.
HitEntry* dou10_yoko_yari2(f32* out) {
    MarioWork* wp = marioGetPtr();
    Vec pos;
    Vec dir;
    Vec hitPos;
    Vec hitNormal;
    HitEntry* hit;
    f32 f8;

    toMovedir(wp->unk1A8);
    pos = wp->position;
    if (strcmp(gp->currentMapName, str_802C5940) != 0) {
        return NULL;
    }
    pos.y = pos.y + float_60_8041a808;
    dir = vec3_802c5874;
    f8 = float_100_8041a7f8;
    hit = marioHitCheckVec(&pos, &dir, &hitPos, &hitNormal, &f8);
    if (hit != NULL && (hitGetAttr(hit) & 0x100000)) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        *out = float_0p8_8041a80c;
    }

    pos = wp->position;
    hit = searchFrontWall(&pos, &f8, float_20_8041a810, float_0_8041a7a4, float_20_8041a810);
    if (hit != NULL && (hitGetAttr(hit) & 0x100000)) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        *out = float_0p8_8041a80c;
    }

    pos = wp->position;
    movePos(&pos.x, &pos.z, float_8_8041a814, float_270_8041a818);
    hit = searchFrontWall(&pos, &f8, float_20_8041a810, float_0_8041a7a4, float_20_8041a810);
    if (hit != NULL && (hitGetAttr(hit) & 0x100000)) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        *out = float_0p8_8041a80c;
    }

    pos = wp->position;
    movePos(&pos.x, &pos.z, float_8_8041a814, float_90_8041a81c);
    hit = searchFrontWall(&pos, &f8, float_20_8041a810, float_0_8041a7a4, float_20_8041a810);
    if (hit != NULL && (hitGetAttr(hit) & 0x100000)) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        *out = float_0p8_8041a80c;
    }
    return hit;
}

// NONMATCHING (79.39%): the remaining differences are not yet analysed (the frame is 0xc0 vs the original 0xd0).
HitEntry* dou10_yoko_yari(f32* out) {
    MarioWork* wp = marioGetPtr();
    Vec pos;
    Vec dir;
    Vec hitPos;
    Vec hitNormal;
    HitEntry* hit;
    f32 f8;
    f32 dist;

    if (wp->currMotionId == 0x16) {
        return NULL;
    }
    toMovedir(wp->unk1A8);
    hit = *(HitEntry**)((u8*)wp + 0x1E0);
    if (hit != NULL) {
        return hit;
    }
    pos = wp->position;
    if (strcmp(gp->currentMapName, str_802C5940) != 0) {
        return hit;
    }
    if (vivianGetStatus() == 2 || vivianGetStatus() == 1 || vivianGetStatus() == 3) {
        return hit;
    }
    dist = (wp->flags & 0x100000) ? float_2_8041a7e8 : float_8_8041a814;
    pos.y = pos.y + float_60_8041a808;
    dir = vec3_802c5868;
    f8 = float_100_8041a7f8;
    hit = marioHitCheckVec(&pos, &dir, &hitPos, &hitNormal, &f8);
    if (hit != NULL && (hitGetAttr(hit) & 0x100000)) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        *out = float_0p8_8041a80c;
    }

    pos = wp->position;
    hit = searchFrontWall(&pos, &f8, float_20_8041a810, float_0_8041a7a4, float_20_8041a810);
    if (hit != NULL && (hitGetAttr(hit) & 0x100000)) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        *out = float_0p8_8041a80c;
    }

    pos = wp->position;
    movePos(&pos.x, &pos.z, dist, float_270_8041a818);
    hit = searchFrontWall(&pos, &f8, float_20_8041a810, float_0_8041a7a4, float_20_8041a810);
    if (hit != NULL && (hitGetAttr(hit) & 0x100000)) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        *out = float_0p8_8041a80c;
    }

    pos = wp->position;
    movePos(&pos.x, &pos.z, dist, float_90_8041a81c);
    hit = searchFrontWall(&pos, &f8, float_20_8041a810, float_0_8041a7a4, float_20_8041a810);
    if (hit != NULL && (hitGetAttr(hit) & 0x100000)) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        *out = float_0p8_8041a80c;
    }
    return hit;
}

void marioSearchFrontWall_staying(f32 a) {
    MarioWork* wp = marioGetPtr();
    Vec v;
    f32 f8;
    f32 f8b;
    f32 halfx;
    f32 angle;
    HitEntry* hit;

    *(s8*)((u8*)wp + 0x3F) = 0;
    halfx = float_0p5_8041a7c4 * wp->wPlayerCollisionBox.x;
    if (wp->flags & 0x100000) {
        halfx = float_0_8041a7a4;
    }

    v = wp->position;
    hit = searchFrontWall(&v, &f8, halfx, wp->directionView - wp->targetYaw, wp->wPlayerCollisionBox.z);
    if (wp->currMotionId != 0x13) {
        *(void**)((u8*)wp + 0x1E0) = hit;
    } else {
        *(void**)((u8*)wp + 0x1E0) = NULL;
    }

    if (hit == NULL) {
        angle = toMovedir(wp->unk1A8);
        v = wp->position;
        hit = searchFrontWall(&v, &f8, halfx, angle, wp->wPlayerCollisionBox.z);
        if (wp->currMotionId != 0x13) {
            *(void**)((u8*)wp + 0x1E0) = hit;
        } else {
            *(void**)((u8*)wp + 0x1E0) = NULL;
        }
    } else {
        if (strcmp(gp->currentMapName, str_802C5940) == 0) {
            if (vivianGetStatus() != 2 && vivianGetStatus() != 1 && vivianGetStatus() != 3) {
                angle = toMovedir(wp->unk1A8);
                v = wp->position;
                movePos(&v.x, &v.z, float_0p5_8041a7c4 * wp->wPlayerCollisionBox.x, angle);
                angle = angle + float_180_8041a7cc;
                hit = searchFrontWall(&v, &f8b, wp->wPlayerCollisionBox.x, angle, wp->wPlayerCollisionBox.z);
                if (hit != NULL && wp->wPlayerCollisionBox.x >= f8b) {
                    *(void**)((u8*)wp + 0x1E0) = hit;
                    f8 = float_0p5_8041a7c4 * f8b;
                }
            }
        }
    }

    if (*(void**)((u8*)wp + 0x1E0) != NULL) {
        *(f32*)((u8*)wp + 0x210) = f8;
    }

    if (*(u16*)((u8*)wp + 0x248) & 0x100) {
        *(void**)((u8*)wp + 0x1DC) = NULL;
        v = wp->position;
        hit = searchFrontWall(&v, &f8, halfx, wp->directionView, wp->wPlayerCollisionBox.z);
        if (hit != NULL) {
            *(void**)((u8*)wp + 0x1DC) = *(void**)((u8*)wp + 0x1E0);
        }
    }

    v = wp->position;
    marioChkWallAround(&v, 0, float_0_8041a7a4, a, wp->wPlayerCollisionBox.z, wp->wPlayerCollisionBox.y);
    wp->position.x = v.x;
    wp->position.z = v.z;
}

extern const f32 float_4_8041a7a8;
extern const f32 float_10_8041a7fc;
extern const f32 float_40_8041a800;
extern const f32 float_47_8041a84c;
extern const Vec vec3_802c5838;

// NONMATCHING (74.47%): 0xf0 frame in the original vs 0xe0 here (one fewer stack
// Vec); dir/targetPos copies and rounding blocks scheduled differently; logic (sincos
// dir, rounded distance compare, distABf) is the same.
void* marioChkFront(f32* out, Vec* pos, f32 angle, f32 dist) {
    Vec dir;
    Vec hitPos;
    Vec hitNormal;
    HitEntry* hit;
    f32 distance;
    f32 s;
    f32 c;

    sincosf(&s, &c, angle);
    dir = vec3_802c5838;
    dir.x = s;
    dir.z = c;
    distance = dist;
    hit = marioHitCheckVec(pos, &dir, &hitPos, &hitNormal, &distance);
    if (hit != NULL && distance <= dist) {
        *out = distABf(pos->x, pos->z, hitPos.x, hitPos.z);
        return hit;
    }
    *out = float_neg1_8041a7dc;
    return NULL;
}

// NONMATCHING (99.35%): float_0/float_1000 are allocated f1/f0 instead of f0/f1,
// so the `else` reuses f1 (stfs f1,0x1c8) where the original reloads float_0.
void marioCheckWallShip(f32 a, f32 b) {
    MarioWork* wp = marioGetPtr();
    HitEntry* hit;
    Vec v;
    Vec v2;
    Vec v3;
    f32 f8;
    f32 d;
    f32 mag;
    s32 r;

    *(f32*)((u8*)wp + 0x210) = float_1000_8041a7a0;
    if (a != float_0_8041a7a4 && wp->airTimer == 0) {
        if (*(f32*)((u8*)wp + 0x1C8) < a) {
            *(f32*)((u8*)wp + 0x1C8) = a;
            *(f32*)((u8*)wp + 0x1CC) = b;
            *(s32*)((u8*)wp + 0x1D0) = 0xE;
        } else {
            s32 t = *(s32*)((u8*)wp + 0x1D0) - 1;

            *(s32*)((u8*)wp + 0x1D0) = t;
            if (t <= 0) {
                *(s32*)((u8*)wp + 0x1D0) = 0;
                *(f32*)((u8*)wp + 0x1C8) = a;
                *(f32*)((u8*)wp + 0x1CC) = b;
            }
        }
    } else {
        *(f32*)((u8*)wp + 0x1C8) = float_0_8041a7a4;
    }

    if (a != float_0_8041a7a4 && wp->airTimer == 0) {
        *(f32*)((u8*)wp + 0x19C) = *(f32*)((u8*)wp + 0x1CC);
    }

    if (a == float_0_8041a7a4) {
        marioSearchFrontWall_staying(b);
        return;
    }

    v = wp->position;
    hit = searchFrontWall(&v, &f8, a, b, wp->wPlayerCollisionBox.z);
    if (hit != NULL) {
        MarioWork* wp2;

        *(void**)((u8*)wp + 0x1E0) = hit;
        r = hitGetAttr(hit);
        wp2 = marioGetPtr();
        if ((r & 0x80000) && !(wp2->flags & 0x1000000)) {
            r = 0;
        } else if ((r & 0x80) == 0) {
            r = (r & 0x800000) >> 23;
        } else {
            r = (wp2->currMotionId == 0x19);
        }
        if (r != 0) {
            if (a != float_0_8041a7a4) {
                wp->hitobjPush = hit;
                *(void**)((u8*)wp + 0x1F0) = hit;
            }
            movePos(&wp->position.x, &wp->position.z, a, b);
            v = wp->position;
            return;
        } else {
            if (a == float_0_8041a7a4) {
                wp->field_0x3E[1] = 0;
            } else {
                s32 t;

                *(f32*)((u8*)wp + 0x210) = f8;
                *(void**)((u8*)wp + 0x1F0) = hit;
                t = wp->field_0x3E[1] + 1;
                wp->field_0x3E[1] = t;
                if ((s8)t >= 0x14) {
                    wp->field_0x3E[1] = 0x14;
                    if ((hitGetAttr(hit) & 0x80) == 0) {
                        r = 1;
                    } else {
                        MarioWork* wp3 = marioGetPtr();

                        r = (wp3->currMotionId == 0x19);
                    }
                    if (r != 0) {
                        wp->hitobjPush = hit;
                    }
                }
            }
            wp->position = v;
        }
    }
    v2 = v;
    wp->position = v;
    marioChkWallAround(&wp->position, 0, a, b, wp->wPlayerCollisionBox.z, float_4_8041a7a8);
    d = wp->mBaseDashSpeed;
    PSVECSubtract(&wp->position, &v2, &v3);
    mag = PSVECMag(&v3);
    if (mag > d) {
        PSVECScale(&v3, &v3, d / mag);
        PSVECAdd(&v2, &v3, &wp->position);
    }
}


// NONMATCHING (97.17%): the `beq else; bne then; b join` branch chain after the
// hit != NULL test is folded to one `beq`; the float->int magic double loads from an
// anonymous @594 symbol instead of 0x1f0(r30) / double_to_int_802c5920.
f32 marioChkOverhead(void) {
    const u8* base = vec3_802c5730;
    MarioWork* wp = marioGetPtr();
    Vec v;
    s32 out;
    f32 f8;
    f32 f31;
    HitEntry* hit;

    if (wp->currMotionId == 8) {
        return wp->position.y;
    }
    if (wp->currMotionId == 0x14) {
        v = wp->position;
        f8 = wp->wPlayerCollisionBox.y;
        if (jabaraNoHitChk() == 0) {
            f8 = f8 + float_5_8041a7b8;
            *(void**)((u8*)wp + 0x1F4) = marioSearchHead_jabara(&v, &f8, &out, float_0_8041a7a4);
            if (*(void**)((u8*)wp + 0x1F4) == NULL) {
                *(void**)((u8*)wp + 0x1F4) = marioSearchHead(&v, &f8, (wp->flags & 0x10000) ? *(f32*)((u8*)wp + 0x7C) : float_10_8041a7fc);
            }
        }
    } else if (wp->flags & 0x1000000) {
        v = wp->position;
        *(void**)((u8*)wp + 0x1F4) = marioSearchHead_roll(&v, &f8, float_40_8041a800);
    } else {
        v = wp->position;
        f8 = wp->wPlayerCollisionBox.y;
        f31 = (wp->flags & 0x10000) ? *(f32*)((u8*)wp + 0x7C) : float_10_8041a7fc;
        if (wp->characterId == 2) {
            *(void**)((u8*)wp + 0x1F4) = kpaSearchHead(&v, &f8, f31);
        } else {
            *(void**)((u8*)wp + 0x1F4) = marioSearchHead(&v, &f8, f31);
        }
    }
    wp->position.x = v.x;
    hit = *(HitEntry**)((u8*)wp + 0x1F4);
    if (hit != NULL) {
        MarioWork* wp2;
        s32 ok;

        if (hitGetAttr(hit) & 0x80000000) {
            u8* obj = (u8*)mobjHitObjPtrToPtr(hit);

            if (strcmp((const char*)(obj + 0x15), RO(base, 0x220, str_802C5950)) == 0 ||
                strcmp((const char*)(obj + 0x15), RO(base, 0x234, str_802C5964)) == 0 ||
                strcmp((const char*)(obj + 0x15), RO(base, 0x244, str_802C5974)) == 0 ||
                strcmp((const char*)(obj + 0x15), RO(base, 0x258, str_802C5988)) == 0) {
                ok = 1;
            } else {
                ok = 0;
            }
        } else {
            ok = 0;
        }
        if (ok) {
            wp->baseSpeed = float_0_8041a7a4;
            wp->flags |= 0x80;
        }
        wp->flags |= 0x08000000;
        if (wp->characterId == 2) {
            marioItemGetDisable();
            if (kpaGetBodyStatus() == 0) {
                marioClearJumpPara();
            }
        }
        wp2 = marioGetPtr();
        f31 = wp2->wPlayerCollisionBox.y;
        if (wp2->currMotionId == 0x1A) {
            void* p = partyGetPtr(marioGetPartyId());

            if (p != NULL) {
                f31 = partyGetHeight(p);
            }
        }
        v.y = (f32)(s32)(float_10_8041a7fc * (v.y - f31) + float_0p5_8041a7c4) / float_10_8041a7fc - float_1_8041a7bc;
        if (!(wp->flags & 0x20000) && v.y < *(f32*)((u8*)wp + 0x120)) {
            v.y = *(f32*)((u8*)wp + 0x120);
            if (!(wp->flags & 0x1000000) && wp->currMotionId != 0x19 &&
                wp->currMotionId != 0x18 && wp->currMotionId != 0x1C &&
                wp->characterId != 2 && marioChkSlitEnd() == 0) {
                if (float_0_8041a7a4 == wp->baseSpeed) {
                    if (wp->currMotionId != 0) {
                        marioChgStayMotion();
                    }
                } else {
                    marioChgMot(wp->baseSpeed > wp->mBaseWalkSpeed ? 2 : 1);
                }
            }
        }
        if (wp->currMotionId == 3 || wp->currMotionId == 6) {
            psndSFXOn_3D((const char*)0x156, &wp->position);
            marioClearJumpPara();
            wp->flags &= ~0x000F0000;
            if (wp->characterId != 2) {
                marioChgMot(0xA);
                marioSetFallPara();
            }
            wp->position.y = v.y;
        }
    } else {
        if (!(wp->flags & 0x1000000)) {
            if (f8 >= float_0_8041a7a4 && f8 <= float_5_8041a7b8) {
                wp->flags |= 0x08000000;
            } else {
                wp->flags &= ~0x08000000;
            }
        } else {
            if (f8 >= float_0_8041a7a4 && f8 + wp->wPlayerCollisionBox.y <= float_47_8041a84c) {
                wp->flags |= 0x08000000;
            } else {
                wp->flags &= ~0x08000000;
            }
        }
    }
    return v.y;
}

// NONMATCHING (72.55%): inlined marioHitCheckVec body keeps the distance in f31
// (saves f31, 0xf0 stack); ours keeps it in memory (0xd0 stack), so register and
// stack layout differ along with scheduling.
HitEntry* marioHitCheck(f32* outX, f32* outY, f32* outZ, f32* distance,
                        f32* outNX, f32* outNY, f32* outNZ,
                        f32 x, f32 y, f32 z, f32 nx, f32 ny, f32 nz) {
    Vec pos;
    Vec dir;
    Vec hitPos;
    Vec hitNormal;
    HitEntry* hit;

    pos = *(const Vec*)vec3_802c5730;
    dir = *(const Vec*)vec3_802c573c;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    dir.x = nx;
    dir.y = ny;
    dir.z = nz;
    hit = marioHitCheckVec(&pos, &dir, &hitPos, &hitNormal, distance);
    if (hit != NULL) {
        *outX = hitPos.x;
        *outY = hitPos.y;
        *outZ = hitPos.z;
        *outNX = hitNormal.x;
        *outNY = hitNormal.y;
        *outNZ = hitNormal.z;
    }
    return hit;
}

HitEntry* marioHitCheckVec(Vec* a, Vec* b, Vec* c, Vec* d, f32* distance) {
    HitCheckQuery query;
    HitEntry* hit;

    query.targetPos = *a;
    query.targetDir = *b;
    query.targetDistance = *distance;

    if (marioGetPtr()->currMotionId == 0x1C) {
        hit = hitCheckVecFilter(&query, chkfilterVecVivian);
    } else if (marioGetPtr()->flags & 0x1000000) {
        hit = hitCheckVecFilter(&query, chkfilterVecRoll);
    } else {
        hit = hitCheckVecFilter(&query, chkfilterVec);
    }

    if (hit == NULL) {
        return NULL;
    }

    *distance = (f32)(s32)(float_1000_8041a7a0 * query.targetDistance +
                           (query.targetDistance >= float_0_8041a7a4 ? float_0p5_8041a7c4
                                                                     : float_neg0p5_8041a7c8)) /
                float_1000_8041a7a0;
    if (c != NULL) {
        *c = query.hitPos;
        c->x = (f32)(s32)(float_1000_8041a7a0 * c->x +
                          (c->x >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
               float_1000_8041a7a0;
        c->y = (f32)(s32)(float_1000_8041a7a0 * c->y +
                          (c->y >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
               float_1000_8041a7a0;
        c->z = (f32)(s32)(float_1000_8041a7a0 * c->z +
                          (c->z >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
               float_1000_8041a7a0;
    }
    if (d != NULL) {
        *d = query.hitNormal;
    }
    return hit;
}

extern const Vec vec3_802c5754;
extern const f32 float_neg3000_8041a870;
extern const Vec vec3_802c5760;
extern const Vec vec3_802c576c;
extern const f32 float_37_8041a86c;
extern const f32 float_1037_8041a888;

// NONMATCHING (74.01%): the inlined marioHitCheckVec query sits at 0x38 in the
// original but 0x2c here (one fewer stack Vec); pos is built by loading
// vec3_802c5760 first there, plus register differences.
s32 searchUnder2(Vec* out, f32 x, f32 y, f32 z) {
    Vec pos;
    Vec dir;
    Vec hitPos;
    Vec hitNormal;
    HitEntry* hit;
    f32 dist;

    pos.x = x;
    pos.y = y + float_37_8041a86c;
    pos.z = z;
    dir = vec3_802c576c;
    dist = float_1037_8041a888;
    hit = marioHitCheckVec(&pos, &dir, &hitPos, &hitNormal, &dist);
    out->x = (hit != NULL) ? hitPos.y : float_neg1_8041a7dc;
    return (s32)hit;
}

// NONMATCHING (86.16%): 5 instructions differ, 10 missing, 8 extra, plus register differences.
HitEntry* marioSearchUnder(void) {
    MarioWork* wp = marioGetPtr();
    Vec pos;
    Vec dir;
    Vec hitPos;
    Vec hitNormal;
    HitEntry* hit;
    f32 dist;
    f32 y;

    y = wp->wPlayerCollisionBox.y;
    if (wp->flags & 0x1000000) {
        y = y * float_0p5_8041a7c4;
    }

    pos = wp->position;
    pos.y = pos.y + y;
    dir = vec3_802c5754;
    dist = float_1000_8041a7a0;
    hit = marioHitCheckVec(&pos, &dir, &hitPos, &hitNormal, &dist);
    *(f32*)wp->field_0x1C0 = (hit != NULL) ? hitPos.y : float_neg3000_8041a870;
    return hit;
}
