#include "cam_road.h"
#include "cam_shift.h"
#include "driver/arcdrv.h"
#include "driver/mapdrv.h"
#include "manager/dvdmgr.h"
#include "memory.h"
#include "mario/mariost.h"
#include "system.h"
#include <stdlib.h>
#include <dolphin/os.h>
#include <dolphin/mtx.h>
#include <stdio.h>
#include <string.h>
#include "math_ext.h"

#define CAM_ROAD_MAP_101 0x65 // the map named "1.01"; gates the height-lock and curve-disable paths
#define CAM_ROAD_MAP_OTHER 0x64

typedef struct CamRoadMapEntry {
    /* 0x00 */ char name[0x28];
    /* 0x28 */ s32 disabled;
    /* 0x2C */ u8 pad_2C[0x8C];
} CamRoadMapEntry;

typedef struct CamRoadMap {
    /* 0x000 */ u8 pad_000[0x44];
    /* 0x044 */ char name[0x84];
    /* 0x0C8 */ s32 count;
    /* 0x0CC */ u8 pad_0CC[0x1C];
    /* 0x0E8 */ u32 unk_E8;
    /* 0x0EC */ CamRoadMapEntry* entries;
    /* 0x0F0 */ u32 subs;
    /* 0x0F4 */ u32 lists;
    /* 0x0F8 */ u32 curvePts;
    /* 0x0FC */ u32 verts;
    /* 0x100 */ u32 indices;
    /* 0x104 */ u32 curveRanges;
    /* 0x108 */ u32 unk_108;
} CamRoadMap;

// The 0xB8-byte curve entries of CamRoadMap::entries (CamRoadMapEntry is the same record seen from the name side).
typedef struct CamRoadCurve {
    /* 0x00 */ u8 pad_00[0x20];
    /* 0x20 */ u32 heightLock;
    /* 0x24 */ f32 lockedHeight;
    /* 0x28 */ u32 disabled;
    /* 0x2C */ u8 pad_2C[0x14];
    /* 0x40 */ s32 firstSeg;
    /* 0x44 */ s32 lastSeg;
    /* 0x48 */ u8 pad_48[0x10];
    /* 0x58 */ f32 radius;
    /* 0x5C */ u8 pad_5C[0x8];
    /* 0x64 */ f32 yaw;
    /* 0x68 */ u8 pad_68[0x8];
    /* 0x70 */ f32 roll;
    /* 0x74 */ u8 pad_74[0x8];
    /* 0x7C */ f32 shiftXRate;
    /* 0x80 */ u8 pad_80[0xC];
    /* 0x8C */ f32 topHeight;
    /* 0x90 */ u8 pad_90[0x18];
    /* 0xA8 */ s32 subStart;
    /* 0xAC */ s32 subCount;
    /* 0xB0 */ s32 curveIdx;
    /* 0xB4 */ s32 hasCurve;
} CamRoadCurve;

typedef struct CamRoadSub {
    /* 0x00 */ u8 pad_00[0x58];
    /* 0x58 */ s32 vertStart;
    /* 0x5C */ u8 pad_5C[0x4];
    /* 0x60 */ s32 listStart;
    /* 0x64 */ s32 listCount;
} CamRoadSub;

typedef struct CamRoadList {
    /* 0x0 */ s32 idxStart;
    /* 0x4 */ s32 idxCount;
} CamRoadList;



//.bss
static CamRoadWork work;

//.sdata
static CamRoadWork* wp = &work;

//.rodata
const Vec vec3_802c12d0 = { 0.0f, 1.0f, 0.0f };
const Vec vec3_802c12dc = { 90.0f, 110.0f, 13.0f };
const Vec vec3_802c12e8 = { -110.0f, -70.0f, -190.0f };
const Vec vec3_802c12f4 = { 0.0f, -1.0f, 0.0f };

//.sdata2

void camRoadCurveOff(const char* name) {
    CamRoadMapEntry* entry = wp->map->entries;
    s32 i;

    if (wp->mapKind == CAM_ROAD_MAP_101) {
        for (i = 0; i < wp->map->count; i++, entry++) {
            if (strcmp(entry->name, name) == 0) {
                entry->disabled = 1;
                break;
            }
        }
    }
}

void camRoadCurveOn(const char* name) {
    CamRoadMapEntry* entry = wp->map->entries;
    s32 i;

    if (wp->mapKind == CAM_ROAD_MAP_101) {
        for (i = 0; i < wp->map->count; i++, entry++) {
            if (strcmp(entry->name, name) == 0) {
                entry->disabled = 0;
                break;
            }
        }
    }
}

void camRoadReset(void) {
    s32 f204 = wp->unk_204;
    void* map = wp->map;
    s32 f1d0 = wp->mapKind;

    memset(wp, 0, sizeof(CamRoadWork));
    wp->hokanActive = 0;
    wp->previous.valid = 0;
    wp->previous.valid = 0;
    wp->distRate = 0.005f;
    wp->velRate = 0.0f;
    wp->sideRecoverA = 0.004f;
    wp->sideRecoverB = 0.008f;
    wp->sideWeightA = 1.0f;
    wp->sideWeightB = 1.0f;
    wp->cmpHeadingW = 2.0f;
    wp->cmpYawW = 1.0f;
    wp->cmpRollW = 1.0f;
    wp->cmpRadiusW = 3.0f;
    wp->cmpDistW = 1.0f;
    wp->unk_204 = -1;
    wp->unk_208 = 0;
    wp->unk_204 = f204;
    wp->map = map;
    wp->mapKind = f1d0;
}

f32 camRoadGetCurrentShiftXRate(void) {
    if (wp->current.valid != 0) {
        return wp->current.shiftXRate;
    }
    return 0.0f;
}

CamRoadWork* camRoadGetWorkPtr(void) {
    return wp;
}

// The clamp's literal restores the original's f0 constant / f2 accumulator / f1 result;
// the named `1.0f` instead puts the constant in f2.
void camRoadMain(f32* dst, f32 x, f32 y, f32 z, f32 ax, f32 ay, f32 az, f32 bank) {
    u32 result;

    wp->camX = x;
    wp->camY = y;
    wp->camZ = z;
    wp->camAx = ax;
    wp->camAy = ay;
    wp->camAz = az;
    calcCamRoad(&wp->current, &wp->previous, x / 10.0f, y / 10.0f,
                z / 10.0f, ax / 10.0f, ay / 10.0f,
                az / 10.0f, bank);

    if (wp->hokanActive != 0) {
        result = compareCamRoad(&wp->previous, &wp->current, &wp->hokanRate);
        if (result != 0) {
            wp->changeFlags = result;
            wp->hokanActive = 1;
            wp->hokanTime = gp->renderFieldTime;
            wp->hokanT = 0.0f;
            wp->hokanFrom = wp->blended;
            wp->previous = wp->current;
            if (wp->previous.flags != 0 || wp->hokanFrom.flags != 0 ||
                (wp->mapKind == CAM_ROAD_MAP_101 &&
                 ((wp->previous.road != NULL && ((CamRoadCurve*)wp->previous.road)->heightLock != 0) ||
                  (wp->hokanFrom.road != NULL && ((CamRoadCurve*)wp->hokanFrom.road)->heightLock != 0))) ||
                (wp->previous.road != NULL && ((CamRoadCurve*)wp->previous.road)->shiftXRate) ||
                (wp->hokanFrom.road != NULL && ((CamRoadCurve*)wp->hokanFrom.road)->shiftXRate)) {
                wp->hokanBlendPos = 1;
            } else {
                wp->hokanBlendPos = 0;
            }
        } else {
            wp->previous = wp->current;
        }
    }
    if (wp->hokanActive == 0) {
        result = compareCamRoad(&wp->current, &wp->previous, &wp->hokanRate);
        if (result != 0) {
            wp->changeFlags = result;
            wp->hokanActive = 1;
            wp->hokanTime = gp->renderFieldTime;
            wp->hokanT = 0.0f;
            wp->hokanFrom = wp->previous;
            wp->previous = wp->current;
            if (wp->previous.flags != 0 || wp->hokanFrom.flags != 0 ||
                (wp->mapKind == CAM_ROAD_MAP_101 &&
                 ((wp->previous.road != NULL && ((CamRoadCurve*)wp->previous.road)->heightLock != 0) ||
                  (wp->hokanFrom.road != NULL && ((CamRoadCurve*)wp->hokanFrom.road)->heightLock != 0))) ||
                (wp->previous.road != NULL && ((CamRoadCurve*)wp->previous.road)->shiftXRate) ||
                (wp->hokanFrom.road != NULL && ((CamRoadCurve*)wp->hokanFrom.road)->shiftXRate)) {
                wp->hokanBlendPos = 1;
            } else {
                wp->hokanBlendPos = 0;
            }
        }
    }

    if (wp->hokanActive != 0) {
        wp->hokanT += getV60FPS(wp->hokanRate, wp->hokanTime, gp->renderFieldTime);
        if (wp->hokanT > 1.0f) {
            wp->hokanT = 1.0f;
        }
        wp->hokanTime = gp->renderFieldTime;
        calcHokanCamRoad(wp->hokanBlendPos, &wp->hokanFrom, &wp->previous, wp->hokanT, &wp->current, &wp->blended);
        if (wp->hokanBlendPos != 0) {
            wp->blended.flags = 1;
        } else {
            wp->blended.flags = 0;
        }
        if (1.0f == wp->hokanT) {
            wp->blended = wp->previous;
            wp->hokanActive = 0;
        }
    } else {
        wp->blended = wp->previous = wp->current;
    }

    calcCurrentCamState(dst);
    dst[6] *= 10.0f;
    dst[7] *= 10.0f;
    dst[8] *= 10.0f;
    dst[3] *= 10.0f;
    dst[4] *= 10.0f;
    dst[5] *= 10.0f;
}

void calcHokanCamRoad(s32 flag, CamRoadState* a, CamRoadState* b, f32 t, CamRoadState* c, CamRoadState* out) {
    f32 f31;
    f32 f30;
    f32 f6;
    f32 t0;
    f32 angle;
    f32 angleB;
    f32 fa;
    f32 start = 0.0f;
    f32 range = 1.0f;

    f31 = (f32)cos(start);
    f30 = (f32)cos(start);
    // temp for the cos argument: the original pools its constants through this temp's
    // register assignment (inlining it back swaps f1/f2 in the f6 fmadds)
    fa = 3.141592f * t;
    f6 = (f32)cos(fa * range + start);
    t0 = 2.0f / (1.0f + f31);
    f6 = t0 * (0.5f * (1.0f - f30) + f6);
    f31 = 0.500001f * (2.0f - (1.0f + f6));
    if (f31 > 1.0f) {
        f31 = 1.0f;
    }
    if (flag != 0) {
        out->x = a->x + f31 * (b->x - a->x);
        out->y = a->y + f31 * (b->y - a->y);
        out->z = a->z + f31 * (b->z - a->z);
    } else {
        out->x = b->x;
        out->y = b->y;
        out->z = b->z;
    }
#ifdef MUST_MATCH
    // the original converts the first atan2 result in a scratch then copies it to angle
    t0 = (f32)atan2(a->dirX, a->dirZ);
    angle = t0;
#else
    angle = (f32)atan2(a->dirX, a->dirZ);
#endif
    angleB = (f32)atan2(b->dirX, b->dirZ);
    while (angle - angleB > 3.1415927f) {
        angle -= 6.2831855f;
    }
    while (angle - angleB < -3.1415927f) {
        angle += 6.2831855f;
    }
    angle = angle + f31 * (angleB - angle);
    out->dirX = (f32)sin(angle);
    out->dirZ = (f32)cos(angle);
    out->dirY = a->dirY + f31 * (b->dirY - a->dirY);
    out->radius = a->radius + f31 * (b->radius - a->radius);
    out->yaw = a->yaw + f31 * (b->yaw - a->yaw);
    out->roll = a->roll + f31 * (b->roll - a->roll);
    out->shiftXRate = a->shiftXRate + f31 * (b->shiftXRate - a->shiftXRate);
}

int __float_nan[];
#define NAN (*(f32*)__float_nan)

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

static inline s32 __fpclassifyf(f32 x) {
    switch (*(s32*)&x & 0x7F800000) {
        case 0x7F800000:
            if (*(s32*)&x & 0x007FFFFF) {
                return FP_NAN;
            } else {
                return FP_INFINITE;
            }
            break;
        case 0:
            if (*(s32*)&x & 0x007FFFFF) {
                return FP_SUBNORMAL;
            } else {
                return FP_ZERO;
            }
            break;
        default:
            return FP_NORMAL;
    }
}

#define fpclassify(x) __fpclassifyf(x)
#define isnan(x) (fpclassify(x) == FP_NAN)

static inline f32 sqrtf(f32 x) {
    const f64 _half = .5;
    const f64 _three = 3.0;

    if (x > 0.0f) {
        f64 guess = __frsqrte(x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        return x * guess;
    } else if (x < 0.0) {
        return NAN;
    } else if (isnan(x)) {
        return NAN;
    }
    return x;
}

u32 compareCamRoad(CamRoadState* cur, CamRoadState* prev, f32* out) {
    u32 flags = 0;
    s32 d;
    s32 range;
    s32 lim;
    f32 dot;
    f32 deg;
    f32 maxVal;
    f32 diff;
    f32 dist;
    f32 cand;

    if (prev->valid == 0) {
        return 0;
    }
    if (cur->road == prev->road) {
        if (!(cur->onRoad == 1 && prev->onRoad == 0) && !(cur->onRoad == 0 && prev->flags == 1)) {
            d = (s32)cur->segment - (s32)prev->segment;
            if (abs(d) <= 1) {
                return 0;
            }
            range = ((CamRoadCurve*)cur->road)->lastSeg - ((CamRoadCurve*)cur->road)->firstSeg;
            lim = range;
            if (abs(d) >= --lim) {
                return 0;
            }
            if (abs(d) >= (range -= 2)) {
                flags |= 0x20;
            }
        }
    }
    if (cur->road != prev->road) {
        flags |= 0x1;
    }
    if (cur->onRoad == 1 && prev->onRoad == 0) {
        flags |= 0x2;
    }
    if (cur->onRoad == 0 && prev->flags == 1) {
        flags |= 0x4;
    }
#ifdef MUST_MATCH
    // the compiler materialises the negated `<=` here; the natural `> 1.0` emits a plain branch.
    if (!(__fabsf(cur->segment - prev->segment) <= 1.0)) {
#else
    if (__fabsf(cur->segment - prev->segment) > 1.0) {
#endif
        flags |= 0x8;
    }
    dot = cur->dirX * prev->dirX + cur->dirZ * prev->dirZ;
    if (dot > 1.0f) {
        dot = 1.0f;
    }
    deg = (f32)acos(dot) * 57.29578f;
    deg = __fabsf(deg);
    if (deg > 180.0f) {
        deg = 360.0f - deg;
    }
    cand = 6.2831855f * cur->radius * deg / 360.0f;
    maxVal = cand * wp->cmpHeadingW;
    diff = __fabsf(cur->yaw - prev->yaw);
    if (diff > 180.0f) {
        diff = 360.0f - diff;
    }
    cand = 6.2831855f * cur->radius * diff / 360.0f;
    cand = cand * wp->cmpYawW;
    if (cand > maxVal) {
        maxVal = cand;
    }
    diff = __fabsf(cur->roll - prev->roll);
    if (diff > 180.0f) {
        diff = 360.0f - diff;
    }
    cand = 6.2831855f * cur->radius * diff / 360.0f;
    cand = cand * wp->cmpRollW;
    if (cand > maxVal) {
        maxVal = cand;
    }
    cand = __fabsf(cur->radius - prev->radius);
    cand = cand * wp->cmpRadiusW;
    if (cand > maxVal) {
        maxVal = cand;
    }
    {
        f32 dxx = (cur->x - prev->x) * (cur->x - prev->x);
        f32 dyy = (cur->y - prev->y) * (cur->y - prev->y);
        f32 dzz = (cur->z - prev->z) * (cur->z - prev->z);

        dot = dxx;
        dot = dot + dyy;
        dot = dot + dzz;
        if (dot) {
            dot = sqrtf(dot);
        } else {
            dot = 0.0f;
        }
        dist = dot;
    }
    if (maxVal < 0.01f && dist < 0.3f) {
        return 0;
    }
    cand = dist * wp->cmpDistW;
    if (cand > maxVal) {
        maxVal = cand;
    }
    if (maxVal > 90.0f) {
        maxVal = 90.0f;
    }
    if (maxVal < 20.0f) {
        maxVal = 20.0f;
    }
    *out = 1.0f / maxVal;
    if (flags == 0) {
        flags |= 0x10;
    }
    return flags;
}

// Splits d into a * u + b * w for the perpendicular xz axes u and w, eliminating through the larger u component.
static inline void splitOnAxes(Vec* d, Vec* u, Vec* w, f32* a, f32* b) {
    f32 ka, kb;

    if (__fabsf(u->z) > __fabsf(u->x)) {
        kb = (d->x - u->x * (d->z / u->z)) / (w->x - u->x * (w->z / u->z));
        ka = (d->z - kb * w->z) / u->z;
    } else {
        kb = (d->z - u->z * (d->x / u->x)) / (w->z - u->z * (w->x / u->x));
        ka = (d->x - kb * w->x) / u->x;
    }
    *a = ka;
    *b = kb;
}

// Finds the camera road under the player (ray tests down the road triangles, the curve collision) and eases the
// look-at point along it. The seventh float arg (f7) is the bank.
// collisionCurve/collisionTri_simple take their Vec arguments by value (the caller makes the stack copies).
// t1/t2 start as struct copies of pos (the original copies words, then overwrites y).
void calcCamRoad(CamRoadState* a, CamRoadState* b, f32 x, f32 y, f32 z, f32 ax, f32 ay, f32 az, f32 bank) {
    s32 m;
    s32* idxp;
    s32 k;
    s32 found;
    CamRoadCurve* entry = (CamRoadCurve*)wp->map->entries;
    void* hit = NULL;
    s32 subCount;
    CamRoadSub* sub;
    Vec* verts;
    s32 n;
    CamRoadList* list;
    s32 j;
    s32* idx;
    s32 i;
    CamRoadCurve* cur;
    s32 flags;
    s32 found1;
    s32 found2;
    f32 tmin = -100000000.0f;
    f32 s1B0;
    f32 f31;
    f32 top;
    f32 fz;
    f32 gx;
    f32 f30;
    f32 s1;
    f32 f29;
    f32 c1;
    f32 m2;
    f32 hx;
    f32 s1AC;
    f32 qq;
    f32 s1A8;
    f32 nzv;
    f32 gz;
    f32 px;
    f32 fx;
    f32 k2;
    f32 lim2;
    f32 py;
    f32 sgnq;
    f32 len;
    f32 vel1;
    f32 pz;
    f32 m1;
    f32 f24;
    f32 vel2;
    f32 rad;
    f32 sgnp;
    f32 lim1;
    f32 k1;
    f32 hz;
    f32 bk;
    f32 pp;
    CollisionCurveData* curve;
    Vec pos;
    Vec ang;
    Vec q;
    CollisionCurveHit h1;
    CollisionCurveHit h2;
    CollisionCurveHit h3;

    pos.x = x;
    pos.y = y;
    pos.z = z;
    ang.x = ax;
    ang.y = ay;
    ang.z = az;

    for (i = 0; i < wp->map->count; i++, entry++) {
        subCount = entry->subCount;
        sub = (CamRoadSub*)wp->map->subs + entry->subStart;
        if (wp->mapKind != CAM_ROAD_MAP_101 || entry->disabled == 0) {
            if (subCount == 0) {
                top = entry->topHeight;

                if (!(top > ay) && top > tmin) {
                    tmin = top;
                    hit = entry;
                }
            } else {
                for (k = 0; k < subCount; k++, sub++) {
                    CamRoadMap* map = wp->map;

                    verts = (Vec*)map->verts + sub->vertStart;
                    n = sub->listCount;
                    list = (CamRoadList*)map->lists + sub->listStart;
                    for (j = 0; j < n; j++, list++) {
                        idx = (s32*)wp->map->indices + list->idxStart;
                        idxp = idx + 1;
                        m = list->idxCount;
                        for (found = 1; found < m - 1; idxp++, found++) {
                            if (collisionTri_simple(ang, verts[idx[0]], verts[idxp[0]], verts[idxp[1]], &tmin)) {
                                hit = entry;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    if (hit != NULL) {
        h1.startY = -1000000.0f;
        h1.normX = 0.0f;
        h1.normY = 0.0f;
        h1.normZ = 0.0f;
        if (!collisionCurve(0, hit, pos, &h1)) {
            hit = NULL;
        }
    }

    cur = (CamRoadCurve*)hit;
    if (hit == NULL) {
        if (b->valid != 0) {
            cur = (CamRoadCurve*)b->road;
        } else {
            cur = NULL;
        }
        if (cur == NULL) {
            wp->noRoadX = x;
            wp->noRoadY = y;
            wp->noRoadZ = z;
            wp->noRoadAx = ax;
            wp->noRoadAy = ay;
            wp->noRoadAz = az;
            return;
        }
        px = x;
        if (wp->mapKind == CAM_ROAD_MAP_101 && cur != NULL && cur->heightLock != 0) {
            py = cur->lockedHeight;
        } else {
            py = y;
        }
        pz = z;
        flags = 0;
        f31 = -b->dirX;
        f30 = -b->dirY;
        f29 = -b->dirZ;
        f24 = b->radius;
        s1B0 = b->yaw;
        s1AC = b->roll;
        s1A8 = b->segment;
    } else {

        if (h1.param < 0.0f) {
            h1.param = 0.0f;
        }
        if (h1.param > 1.0f) {
            h1.param = 1.0f;
        }
        s1B0 = cur->yaw;
        f24 = cur->radius;
        s1AC = cur->roll;
        px = h1.x;
        if (wp->mapKind == CAM_ROAD_MAP_101 && cur->heightLock != 0) {
            py = cur->lockedHeight;
        } else {
            py = y;
        }
        f31 = -h1.normZ;
        f29 = h1.normX;
        len = f31 * f31 + h1.normX * h1.normX;
        pz = h1.z;
        flags = h1.flags;
        s1A8 = (f32)h1.segment;
        f30 = h1.normY;
        if (len != 0.0) {
            len = sqrtf(len);
            f31 /= len;
            f30 = 0.0f;
            f29 /= len;
        }
        f31 *= -1.0f;
        f30 *= -1.0f;
        f29 *= -1.0f;
    }

    if (cur->hasCurve != 0) {
        curve = (CollisionCurveData*)((CamRoadSub*)wp->map->curveRanges + cur->curveIdx);
        q.x = px;
        q.y = 100000000.0f;
        q.z = pz;
        h1.startY = -1000000.0f;
        if (collisionCurve(1, curve, q, &h1)) {
            px = h1.x;
            pz = h1.z;
            flags |= h1.flags;
        }
    }

    if (cur != NULL) {
        rad = 3.141592f * (*(f32*)wp->map->unk_E8 / 2.0f / 180.0f);

        if (bank && hit != NULL && b->valid != 0) {
            f32 scale = 1.0f;
            f32 ox;
            f32 oz;
            Vec t1;
            Vec t2;
            Vec dA;
            Vec dB;
            Vec u;
            Vec w;
            Vec w1;
            Vec w2;

            c1 = (f32)cos(rad);
            s1 = (f32)sin(rad);
            nzv = -f29;
            t1 = pos;
            bk = b->bank;
            k1 = 640.0f * (f24 * s1 / c1) / 480.0f;
            k2 = 0.01f * (bk * (scale * k1));
            ox = k2 * nzv;
            oz = k2 * f31;
            t1.x = t1.x + ox;
            t1.y = 1000000.0f;
            t1.z = t1.z + oz;
            h2.startY = -1000000.0f;
            h2.normX = h2.normY = h2.normZ = 0.0f;
            found1 = 0;
            if (collisionCurve(0, (CollisionCurveData*)cur, t1, &h2)) {
                found1 = 1;
                t1.x = h2.x;
                t1.y = py;
                t1.z = h2.z;
            }
            if (cur->hasCurve != 0) {
                curve = (CollisionCurveData*)((CamRoadSub*)wp->map->curveRanges + cur->curveIdx);
                h2.startY = -1000000.0f;
                if (collisionCurve(1, curve, t1, &h2)) {
                    found1 = 1;
                    t1.x = h2.x;
                    t1.y = py;
                    t1.z = h2.z;
                }
            }
            if (found1 != 0) {
                gx = t1.x;
                gz = t1.z;
            } else {
                gx = px;
                gz = pz;
            }

            c1 = (f32)cos(rad);
            s1 = (f32)sin(rad);
            k1 = 640.0f * (f24 * s1 / c1) / 480.0f;
            k2 = 0.01f * (bank * (scale * k1));
            ox = k2 * nzv;
            t2 = pos;
            t2.x = t2.x + ox;
            t2.y = 1000000.0f;
            h3.startY = -1000000.0f;
            oz = k2 * f31;
            t2.z = t2.z + oz;
            h3.normX = h3.normY = h3.normZ = 0.0f;
            found2 = 0;
            flags = 0;
            if (collisionCurve(0, (CollisionCurveData*)cur, t2, &h3)) {
                found2 = 1;
                flags |= h3.flags;
                t2.x = h3.x;
                t2.y = py;
                t2.z = h3.z;
            }
            if (cur->hasCurve != 0) {
                curve = (CollisionCurveData*)((CamRoadSub*)wp->map->curveRanges + cur->curveIdx);
                h3.startY = -1000000.0f;
                if (collisionCurve(1, curve, t2, &h3)) {
                    found2 = 1;
                    t2.x = h3.x;
                    flags |= h3.flags;
                    t2.z = h3.z;
                }
            }
            if (found2 != 0) {
                hx = t2.x;
                hz = t2.z;
                if (!(h3.flags & 1) && (b->flags & 1)) {
                    wp->sideWeightA = 0.0f;
                }
                if (!(h3.flags & 2) && (b->flags & 2)) {
                    wp->sideWeightB = 0.0f;
                }
            } else {
                hx = px;
                hz = pz;
            }

            dA.x = gx - b->x;
            dA.y = 0.0f;
            dA.z = gz - b->z;
            dB.x = hx - b->x;
            dB.y = 0.0f;
            dB.z = hz - b->z;
            m1 = PSVECMag(&dA);
            PSVECMag(&dB);
            m1 = m1 + getV60FPS(wp->distRate, b->time, gp->renderFieldTime);
            m2 = getV60FPS(wp->velRate, b->time, gp->renderFieldTime);
            lim1 = m1 * wp->sideWeightA;
            lim2 = m1 * wp->sideWeightB;
            vel1 = m2 * wp->sideWeightA;
            vel2 = m2 * wp->sideWeightB;
            u.x = nzv;
            u.z = f31;
            w.x = -u.z;
            w.z = u.x;
            u.y = 0.0f;
            w.y = 0.0f;
            w1.y = 0.0f;
            w2.y = 0.0f;
            splitOnAxes(&dB, &u, &w, &qq, &pp);
            w1.x = qq * u.x;
            w1.z = qq * u.z;
            w2.x = pp * w.x;
            w2.z = pp * w.z;
            if (qq < 0.0f) {
                sgnq = -1.0f;
            } else {
                sgnq = 1.0f;
            }
            if (pp < 0.0f) {
                sgnp = -1.0f;
            } else {
                sgnp = 1.0f;
            }
            fx = PSVECMag(&w1);
            fz = PSVECMag(&w2);
            if (fx > lim1 && fx > vel1) {
                PSVECScale(&u, &w1, lim1 * sgnq);
            }
            if (fz > lim2 && fz > vel2) {
                PSVECScale(&w, &w2, lim2 * sgnp);
            }
            px = b->x;
            pz = b->z;
            px += w1.x + w2.x;
            pz += w1.z + w2.z;
        }
    }

    wp->sideWeightA = wp->sideWeightA + wp->sideRecoverA;
    if (wp->sideWeightA > 1.0f) {
        wp->sideWeightA = 1.0f;
    }
    wp->sideWeightB = wp->sideWeightB + wp->sideRecoverB;
    if (wp->sideWeightB > 1.0f) {
        wp->sideWeightB = 1.0f;
    }

    a->valid = 1;
    a->onRoad = hit != NULL;
    a->flags = flags;
    a->inX = x;
    a->inY = y;
    a->inZ = z;
    a->inAx = ax;
    a->inAy = ay;
    a->inAz = az;
    a->road = cur;
    a->x = px;
    a->y = py;
    a->z = pz;
    a->dirX = -f31;
    a->dirY = -f30;
    a->dirZ = -f29;
    a->radius = f24;
    a->yaw = s1B0;
    a->roll = s1AC;
    a->segment = s1A8;
    a->time = gp->renderFieldTime;
    a->bank = bank;
    a->shiftXRate = cur->shiftXRate;
}


// The `if (x != 0.0) x = sqrtf(x);` guards are intentional: removing them drops the pool's
// `fcmpu 0.0,x; beq` skip that the target has. Register allocation follows declaration order: the
// second line of floats is declared in the order of their saved registers (rx = f31 .. c1 = f14), the
// intersection x (`ix`) sits above them so it wins the spill-cost tie against c1..iz yet takes the stack
// slot after `dir`, and the spilled floats ang0, yLo, yHi, best come first so their slots ascend. The
// pointer locals are declared pts..pHi (r30..r25) before the counters found..flags (r24..r18), and the
// loop bound stays `half - 1` (a hoisted temp, not a local).
BOOL collisionCurve(s32 mode, CollisionCurveData* data, Vec p, CollisionCurveHit* out) {
    f32 ang0, yLo, yHi, best, ix;
    f32 rx, rz, side, tl, limA, limB, x0, h0, z0, x1, z1, iz, r0, r1, rp, vx0, vz0, c1;
    f32 dir;
    f32 d0;
    Vec* base;
    Vec *pts, *nrm, *nLo, *pLo, *nHi, *pHi;
    s32 found, i, idxLo, idxHi, clampEnds, openEnds, flags;
    s32 half;
    Vec *ptBase, *nBase;
    f32 ux, y, sx, uz, wx, wz, sz, nx1, nz1, s, t, d, px, offx, qx, pz, qz, dx, cross, offz, nx, dz, fx, fz, c0, ratio, dist, h, xc;

    best = 100000000.0f;
    found = 0;
    d0 = 0.0f;

    if (mode == 0) {
        limA = data->limitA;
        limB = data->limitB;
        openEnds = 0;
        half = data->ptsCount / 2;
        base = (Vec*)wp->map->curvePts;
        ptBase = &base[data->ptsStart];
        nBase = &base[data->ptsStart + half];
        idxLo = data->firstSeg;
        idxHi = data->lastSeg;
        clampEnds = data->clampEnds;
    } else {
        idxLo = 0;
        limA = 0.0f;
        base = (Vec*)wp->map->curvePts;
        limB = 0.0f;
        ptBase = &base[data->curvePtsStart];
        idxHi = 1;
        clampEnds = 0;
        openEnds = 1;
        half = data->curvePtsCount / 2;
        nBase = &base[data->curvePtsStart + half];
    }

    yLo = nBase[idxLo].y;
    yHi = nBase[idxHi].y;
    if (__fabsf(yHi - yLo) < 0.00009999999747378752) {
        yLo = yHi;
    }
    if (openEnds != 0) {
        clampEnds = 0;
    }

    pts = ptBase;
    nrm = nBase;
    nLo = &nBase[idxLo];
    pLo = &ptBase[idxLo];
    nHi = &nBase[idxHi];
    pHi = &ptBase[idxHi];
    for (i = 0; i < half - 1; pts++, nrm++, i++) {
        z0 = pts[0].z;
        flags = 0;
        nx1 = -nrm[1].x;
        x0 = pts[0].x;
        nz1 = -nrm[1].z;
        h0 = pts[0].y;
        x1 = pts[1].x;
        z1 = pts[1].z;
        if (nrm[0].x * (p.x - x0) + nrm[0].z * (p.z - z0) < d0) continue;
        if (nx1 * (p.x - x1) + nz1 * (p.z - z1) < d0) continue;

        ux = -nrm[0].z;
        uz = nrm[0].x;
        wx = nz1;
        wz = -nx1;
        if (__fabs(ux - wx) < 0.01f && __fabs(uz - wz) < 0.01f) {
            wx = 0.5f * (ux + wx);
            wz = 0.5f * (uz + wz);
            ux = wx;
            uz = wz;
        }
        if (!(ux == wx && uz == wz)) {
            side = 1.0f;
            dir = side;
            if (wx != 0.0) {
                t = (wz * ((x0 - x1) / wx) + z1 - z0) / (uz - wz * (ux / wx));
            } else if (wz != 0.0) {
                t = (wx * ((z0 - z1) / wz) + x1 - x0) / (ux - wx * (uz / wz));
            } else {
                t = 0.0f;
            }
            iz = t * uz + z0;
            ix = t * ux + x0;
            vz0 = z0 - iz;
            vx0 = x0 - ix;
            r0 = vx0 * vx0 + vz0 * vz0;
            if (r0 != 0.0) {
                r0 = sqrtf(r0);
            }
            rz = p.z - iz;
            rx = p.x - ix;
            rp = rx * rx + rz * rz;
            if (rp != 0.0) {
                rp = sqrtf(rp);
            }
            fz = z1 - iz;
            fx = x1 - ix;
            r1 = fx * fx + fz * fz;
            if (r1 != 0.0) {
                r1 = sqrtf(r1);
            }
            c0 = (vx0 * rx + vz0 * rz) / (r0 * rp);
            c1 = (fx * rx + fz * rz) / (r1 * rp);
            if (c0 < -1.0f) {
                c0 = -1.0f;
            } else if (c0 > 1.0f) {
                c0 = 1.0f;
            }
            if (c1 < -1.0f) {
                c1 = -1.0f;
            } else if (c1 > 1.0f) {
                c1 = 1.0f;
            }
            ang0 = acos(c0);
            ratio = ang0 / (ang0 + (f32)acos(c1));
            dist = ratio * ((rp - r1) - (rp - r0)) + (rp - r0);
            if (dist < 0.0f) {
                side = -1.0f;
                dist = -dist;
            }
            if (dist >= best) {
                continue;
            }
            best = dist;
            xc = vx0 * -(z1 - z0) + vz0 * (x1 - x0);
            h = ratio * (nrm[1].y - nrm[0].y) + nrm[0].y;
            if (clampEnds) {
                if (h < yLo) {
                    if (xc > 0.0) {
                        if (side < 0.0f) {
                            if (dist > limA) {
                                dist = limA;
                                flags |= 2;
                            }
                        } else {
                            if (dist > limB) {
                                dist = limB;
                                flags |= 2;
                            }
                            dist = -dist;
                        }
                    } else {
                        if (side < 0.0f) {
                            if (dist > limB) {
                                dist = limB;
                                flags |= 2;
                            }
                            dist = -dist;
                        } else {
                            if (dist > limA) {
                                dist = limA;
                                flags |= 2;
                            }
                        }
                    }
                    found = 1;
                    out->param = yLo;
                    out->x = dist * nLo->z + pLo->x;
                    out->y = 0.0f;
                    out->z = pLo->z - dist * nLo->x;
                    out->normX = nLo->x;
                    out->normY = 0.0f;
                    out->normZ = nLo->z;
                    out->startY = h0;
                    out->segment = idxLo;
                    out->flags = flags | 1;
                    continue;
                }
                if (h > yHi) {
                    if (xc > 0.0) {
                        if (side < 0.0f) {
                            if (dist > limA) {
                                dist = limA;
                                flags |= 2;
                            }
                        } else {
                            if (dist > limB) {
                                dist = limB;
                                flags |= 2;
                            }
                            dist = -dist;
                        }
                    } else {
                        if (side < 0.0f) {
                            if (dist > limB) {
                                dist = limB;
                                flags |= 2;
                            }
                            dist = -dist;
                        } else {
                            if (dist > limA) {
                                dist = limA;
                                flags |= 2;
                            }
                        }
                    }
                    found = 1;
                    out->param = yHi;
                    out->x = dist * nHi->z + pHi->x;
                    out->y = 0.0f;
                    out->z = pHi->z - dist * nHi->x;
                    out->normX = nHi->x;
                    out->normY = 0.0f;
                    out->normZ = nHi->z;
                    out->startY = h0;
                    out->segment = idxHi;
                    out->flags = flags | 1;
                    continue;
                }
                if (xc > 0.0) {
                    if (side < 0.0f) {
                        if (dist > limA) {
                            rx /= rp;
                            flags |= 2;
                            rz /= rp;
                            dist -= limA;
                            rp += dist;
                            rx *= rp;
                            rz *= rp;
                        }
                    } else {
                        if (dist > limB) {
                            rx /= rp;
                            flags |= 2;
                            rz /= rp;
                            dist -= limB;
                            rp -= dist;
                            rx *= rp;
                            rz *= rp;
                        }
                    }
                } else {
                    if (side < 0.0f) {
                        if (dist > limB) {
                            rx /= rp;
                            flags |= 2;
                            rz /= rp;
                            dist -= limB;
                            rp += dist;
                            rx *= rp;
                            rz *= rp;
                        }
                    } else {
                        if (dist > limA) {
                            rx /= rp;
                            flags |= 2;
                            rz /= rp;
                            dist -= limA;
                            rp -= dist;
                            rx *= rp;
                            rz *= rp;
                        }
                    }
                    dir = -1.0f;
                }
            } else if (openEnds) {
                if (xc > 0.0) {
                    if (!(side < 0.0f) && dist > 0.0f) {
                        rx /= rp;
                        flags = 3;
                        rz /= rp;
                        rp -= dist;
                        rx *= rp;
                        rz *= rp;
                    }
                } else {
                    if (side < 0.0f && dist > 0.0f) {
                        rx /= rp;
                        flags = 3;
                        rz /= rp;
                        rp += dist;
                        rx *= rp;
                        rz *= rp;
                    }
                    dir = -1.0f;
                }
            } else if (!(xc > 0.0)) {
                dir = -1.0f;
            }
            out->x = ix + rx;
            out->y = 0.0f;
            out->z = iz + rz;
            if (dir < 0.0f) {
                out->normX = -rz / rp;
                out->normY = 0.0f;
                out->normZ = rx / rp;
            } else {
                out->normX = rz / rp;
                out->normY = 0.0f;
                out->normZ = -rx / rp;
            }
            out->param = h;
            found = 1;
            out->startY = h0;
            out->segment = i;
            out->flags = flags;
        } else {
            sz = pts[0].z;
            dz = pts[1].z - sz;
            sx = pts[0].x;
            dx = pts[1].x - sx;
            px = p.x;
            pz = p.z;
            nx = -dz;
            if (__fabsf(nx) > __fabsf(dx)) {
                s = (dx * ((sx - px) / nx) + pz - sz) / (dz - dx * (dx / nx));
            } else if (0.0 != dx) {
                s = (nx * ((sz - pz) / dx) + px - sx) / (dx - nx * (dz / dx));
            } else {
                s = 0.0f;
            }
            qx = s * dx + sx;
            qz = s * dz + sz;
            if (__fabsf(dx) > __fabsf(dz)) {
                tl = (qx - sx) / dx;
                if (tl < -0.0000001) {
                    goto L_CE48;
                }
                // dead like the compare above; it pools the double the original has at +0x70
                if (tl > 1.0000001) {
                    goto L_CE48;
                }
            } else {
                if (dz != 0.0) {
                    tl = (qz - sz) / dz;
                }
            }
        L_CE48:
            offz = pz - qz;
            offx = px - qx;
            y = tl * (nrm[1].y - nrm[0].y) + nrm[0].y;
            d = offx * offx + offz * offz;
            if (d > 0.0) {
                d = sqrtf(d);
            }
            if (d >= best) continue;
            best = d;
            if (clampEnds != 0) {
                if (y < yLo) {
                    cross = offx * nx + offz * dx;
                    if (cross < 0.0f) {
                        if (d > limA) {
                            d = limA;
                            flags |= 2;
                        }
                    } else {
                        if (d > limB) {
                            d = limB;
                            flags |= 2;
                        }
                        d = -d;
                    }
                    found = 1;
                    out->param = yLo;
                    out->x = d * nLo[0].z + pLo[0].x;
                    out->y = 0.0f;
                    out->z = pLo[0].z - d * nLo[0].x;
                    out->normX = nLo[0].x;
                    out->normY = 0.0f;
                    out->normZ = nLo[0].z;
                    out->startY = h0;
                    out->segment = idxLo;
                    out->flags = flags | 1;
                    continue;
                }
                if (y > yHi) {
                    cross = offx * nx + offz * dx;
                    if (cross < 0.0f) {
                        if (d > limA) {
                            d = limA;
                            flags |= 2;
                        }
                    } else {
                        if (d > limB) {
                            d = limB;
                            flags |= 2;
                        }
                        d = -d;
                    }
                    found = 1;
                    out->param = yHi;
                    out->x = d * nHi[0].z + pHi[0].x;
                    out->y = 0.0f;
                    out->z = pHi[0].z - d * nHi[0].x;
                    out->normX = nHi[0].x;
                    out->normY = 0.0f;
                    out->normZ = nHi[0].z;
                    out->startY = h0;
                    out->segment = idxHi;
                    out->flags = flags | 1;
                    continue;
                }
                cross = offx * nx + offz * dx;
                if (cross < 0.0f) {
                    if (d > limA) {
                        flags |= 2;
                        offx = limA * (offx / d);
                        offz = limA * (offz / d);
                    }
                } else {
                    if (d > limB) {
                        flags |= 2;
                        offx = limB * (offx / d);
                        offz = limB * (offz / d);
                    }
                }
            }
            out->param = y;
            out->x = qx + offx;
            out->y = 0.0f;
            out->z = qz + offz;
            found = 1;
            out->normX = tl * (nrm[1].x - nrm[0].x) + nrm[0].x;
            out->normY = 0.0f;
            out->normZ = tl * (nrm[1].z - nrm[0].z) + nrm[0].z;
            out->startY = h0;
            out->segment = i;
            out->flags = flags;
        }
    }
    return found;
}

// Match-only helpers: the original materialises sub/cross/dot results as Vec locals and
// copies them into by-value argument slots, which only happens when these are inline functions
// taking Vec by value.
static inline Vec collisionTriSub(Vec a, Vec b) {
    Vec r;
    r.x = a.x - b.x;
    r.y = a.y - b.y;
    r.z = a.z - b.z;
    return r;
}

static inline Vec collisionTriCross(Vec a, Vec b) {
    Vec r;
    r.x = a.y * b.z - a.z * b.y;
    r.y = a.z * b.x - a.x * b.z;
    r.z = a.x * b.y - a.y * b.x;
    return r;
}

static inline f32 collisionTriDot(Vec a, Vec b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline Vec collisionTriScale(Vec v, f32 s) {
    Vec r;
    r.x = v.x * s;
    r.y = v.y * s;
    r.z = v.z * s;
    return r;
}

static inline Vec collisionTriAdd(Vec a, Vec b) {
    Vec r;
    r.x = a.x + b.x;
    r.y = a.y + b.y;
    r.z = a.z + b.z;
    return r;
}

// The `dir` vector must be the anonymous literal `{ 0.0f, -1.0f, 0.0f }`, not a copy of the named
// `vec3_802c12f4`: copied from the named global MWCC hoists all three loads to the prologue (98.0%);
// the literal pools to the same .rodata Vec and restores the target's interleaved load schedule.
// Remaining diff: the first two vertex loads are swapped -- the target issues `lwz v1.x` before the `stmw`
// (this build issues `lwz v0.x` after it). The `dot(...) > 0.0` edge guards must be `0.0` (double, reusing the
// guard block's already-resident `0.0` double), not `0.0f`; the float form keeps a second 0.0f live and
// shifts the `collisionTriDot(n, dir)` guard accumulators one FP register higher (f2..f5 vs f0,f2,f3,f4).
// This is the same known-hard load-hoist class as `marioOfsRotReset` (the scheduler moves a load across the
// prologue store in the opposite direction; `scheduling off` collapses the whole function). Everything else --
// instructions, slots, constants and all FP/GP register assignments -- matches.
BOOL collisionTri_simple(Vec p, Vec v0, Vec v1, Vec v2, f32* out) {
    Vec dir = vec3_802c12f4;
    Vec bc = collisionTriSub(v2, v0);
    Vec ab = collisionTriSub(v0, v1);
    Vec ca = collisionTriSub(v1, v2);
    Vec n = collisionTriCross(bc, ab);
    f32 d;
    f32 s;
    f32 y;

    if (n.x == 0.0f && n.y == 0.0f && n.z == 0.0f) {
        return FALSE;
    }
    d = collisionTriDot(n, collisionTriSub(p, v0));
    if (d > 0.0f) {
        if (collisionTriDot(n, dir) >= 0.0) {
            return FALSE;
        }
        if (collisionTriDot(collisionTriSub(p, v0), collisionTriCross(bc, dir)) > 0.0) {
            return FALSE;
        }
        if (collisionTriDot(collisionTriSub(p, v1), collisionTriCross(ab, dir)) > 0.0) {
            return FALSE;
        }
        if (collisionTriDot(collisionTriSub(p, v2), collisionTriCross(ca, dir)) > 0.0) {
            return FALSE;
        }
    } else {
        if (collisionTriDot(n, dir) <= 0.0) {
            return FALSE;
        }
        if (collisionTriDot(collisionTriSub(p, v0), collisionTriCross(bc, dir)) < 0.0) {
            return FALSE;
        }
        if (collisionTriDot(collisionTriSub(p, v1), collisionTriCross(ab, dir)) < 0.0) {
            return FALSE;
        }
        if (collisionTriDot(collisionTriSub(p, v2), collisionTriCross(ca, dir)) < 0.0) {
            return FALSE;
        }
    }
    s = -d / collisionTriDot(n, dir);
    {
        Vec hit = collisionTriAdd(p, collisionTriScale(dir, s));
        if (hit.y < *out) {
            return FALSE;
        }
        *out = hit.y;
        return TRUE;
    }
}

void calcCurrentCamState(f32* dst) {
    Vec* src = (Vec*)wp->map->unk_E8;
    f32 f31;
    f32 sx;
    f32 sy;
    f32 sz;
    f32 f27;
    f32 f26;
    f32 f25;
    f32 f24;
    f32 f23;
    f32 f25b;
    f32 angle;
    Mtx mtx;
    Vec temp;
    Vec out;
    Vec axis;

    dst[0] = src->x;
    dst[1] = src->y;
    dst[2] = src->z;
    if (wp->blended.valid == 0) {
        dst[3] = 0.0f;
        dst[4] = 100.0f;
        dst[5] = 100.0f;
        dst[6] = 0.0f;
        dst[7] = 0.0f;
        dst[8] = 0.0f;
        return;
    }
    sx = wp->blended.x;
    sy = wp->blended.y;
    sz = wp->blended.z;
    f27 = wp->blended.dirX;
    f26 = wp->blended.dirZ;
    f25 = wp->blended.radius;
    f31 = wp->blended.roll;
    angle = wp->blended.yaw * 3.141592f / 180.0f;
    f23 = f27 * f25 * (f32)cos(angle);
    f24 = f25 * (f32)sin(angle);
    f25b = f26 * f25 * (f32)cos(angle);
    dst[3] = sx + f23;
    dst[4] = sy + f24;
    dst[5] = sz + f25b;
    axis.x = -f26;
    axis.y = 0.0f;
    axis.z = f27;
    PSMTXRotAxisRad(mtx, &axis, f31 * 0.017453292f);
    temp.x = -f23;
    temp.y = -f24;
    temp.z = -f25b;
    PSMTXMultVec(mtx, &temp, &out);
    dst[6] = dst[3] + out.x;
    dst[7] = dst[4] + out.y;
    dst[8] = dst[5] + out.z;
}

// Debug dump; nothing in the retail game calls it (mwld strips it), but its strings stay in the pool.
static void camRoadPrint(const char* prefix, s32 cam, s32 curve) {
    OSReport("%scam = %d\n", prefix, cam);
    OSReport("%scurve = %d\n", prefix, curve);
}

BOOL camRoadSetup(const char* filename) {
    u32 length;
    char path[0x80];
    DVDEntry* entry;

    memset(wp, 0, sizeof(CamRoadWork));
    wp->hokanActive = 0;
    wp->previous.valid = 0;
    wp->previous.valid = 0;
    wp->distRate = 0.005f;
    wp->velRate = 0.0f;
    wp->sideRecoverA = 0.004f;
    wp->sideRecoverB = 0.008f;
    wp->sideWeightA = 1.0f;
    wp->sideWeightB = 1.0f;
    wp->cmpHeadingW = 2.0f;
    wp->cmpYawW = 1.0f;
    wp->cmpRollW = 1.0f;
    wp->cmpRadiusW = 3.0f;
    wp->cmpDistW = 1.0f;
    wp->unk_204 = -1;
    wp->unk_208 = 0;
    camShiftInit();
    arcOpen(filename, (void**)&wp->map, &length);
    if (wp->map == NULL) {
        sprintf(path, "%s/%s", getMapDataDvdRoot(), filename);
        entry = DVDMgrOpen(path, 2, 0);
        if (entry != NULL) {
            length = DVDMgrGetLength(entry);
            wp->map = _mapAlloc(OSRoundUp32B(length));
            DVDMgrRead(entry, wp->map, OSRoundUp32B(length), 0);
            DVDMgrClose(entry);
        } else {
            return FALSE;
        }
    }
    if (wp->map->unk_E8 < (u32)wp->map) {
        wp->map->unk_E8 += (u32)wp->map;
        wp->map->entries = (CamRoadMapEntry*)((u32)wp->map->entries + (u32)wp->map);
        wp->map->subs += (u32)wp->map;
        wp->map->lists += (u32)wp->map;
        wp->map->curvePts += (u32)wp->map;
        wp->map->verts += (u32)wp->map;
        wp->map->indices += (u32)wp->map;
        wp->map->curveRanges += (u32)wp->map;
        wp->map->unk_108 += (u32)wp->map;
    }
    if (strcmp(wp->map->name, "1.01") == 0) {
        wp->mapKind = CAM_ROAD_MAP_101;
    } else {
        wp->mapKind = CAM_ROAD_MAP_OTHER;
    }
    return TRUE;
}

// 4-byte zero word that ends the unit's .sdata: all-zero objects need explicit_zero_data to be emitted and force_active to survive the linker.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_8040F2EC_sdata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset
