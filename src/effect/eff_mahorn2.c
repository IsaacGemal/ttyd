/* Mahon = Wizzerd */

#include "eff/eff_mahorn2.h"
#include "driver/dispdrv.h"
#include "eff/eff_hit.h"
#include "memory.h"
#include "system.h"
#include <math.h>
#include <string.h>

#define TWO_PI 6.283185307179586f

// MUST_MATCH: retail's compiler addressed the three zero Vecs below off one base register
// (offset 0, 0xC, 0x18); MWCC here emits a separate base per `{ 0 }` literal, which changes
// every GPR in the function. Declaring them as one array reproduces the shared base.
#ifdef MUST_MATCH
static const Vec vec3_zero[3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
#endif

// NONMATCHING (97.30%): 1 instruction differs, 4 missing, 1 extra, plus register differences.
EffectEntry *effMahorn2Entry(BOOL unused, f32 posX, f32 posY, f32 posZ, f32 dstX, f32 dstY, f32 dstZ, f32 scale, f32 radius) {
    EffectEntry *effect;
    effMahorn2Data *data;
    f32 temp;
    int i;
    
    effect = effEntry();
    effect->type = "Mahorn2";
    effect->count = 14;
    data = __memAlloc(HEAP_EFFECT, sizeof(effMahorn2Data) * 14);
    effect->userdata = data;
    //effect->callback = effMahorn2Main;
    data->unused = unused;
    data->unused2 = 0;
    data->intplStep = 0;
    data->position.x = posX;
    data->position.y = posY;
    data->position.z = posZ;
    data->target.x = dstX;
    data->target.y = dstY;
    data->target.z = dstZ;
    data->scale = scale;
    data->hitDistance = radius;
    data->alpha = 255;
    data->type = 0;
    data->hasHit = FALSE;
    data++;
    
    for (i = 1; i < 4; i++, data++) {
        memset(data, 0, sizeof(effMahorn2Data));
        data->type = 1;
        data->position.x = 0.0f;
        data->position.y = 0.0f;
        data->position.z = 0.0f;
        data->scale = 6.0f;
        data->alpha = 0;
        data->counter = 15 * (i - 1);
    }
    
    for (i = 4; i < 5; i++, data++) {
        memset(data, 0, sizeof(effMahorn2Data));
        data->type = 2;
        data->position.x = 0.0f;
        data->position.y = 0.0f;
        data->position.z = 0.0f;
        data->scale = 0.0f;
        data->alpha = 153;
        data->counter = 15;
    }
    
    for (i = 5; i < 6; i++, data++) {
        memset(data, 0, sizeof(effMahorn2Data));
        data->type = 3;
        data->position.x = 0.0f;
        data->position.y = 0.0f;
        data->position.z = 0.0f;
        data->scale = 0.0f;
        data->alpha = 153;
        data->counter = 60;
    }
    
    for (i = 6; i < effect->count; i++, data++) {
        memset(data, 0, sizeof(effMahorn2Data));
        data->type = 4;
        data->position.x = dstX - posX;
        data->position.y = dstY - posY;
        data->position.z = dstZ - posZ;
        temp = TWO_PI * (f32)(i - 6);
        data->target.x = 6.0f * (f32)sin(temp / (f32)(effect->count - 6));
        data->target.y = 6.0f * (f32)cos(temp / (f32)(effect->count - 6));
        data->target.z = 0.0f;
        data->scale = 0.0f;
        data->alpha = 255;
        data->counter = 0;
    }
    
    return effect;
}

// NONMATCHING (99.46%): all logic matches. Remaining: (1) the first zero-Vec copy picks scratch
// registers r4/r3/r0 and r7/r6/r5 where retail uses r3/r0/r5 and r4/r3/r0; (2) three fneg/fsubs
// results land in f0 where retail puts them in f1. Same values and opcodes.
void effMahorn2Main(EffectEntry* effect) {
    effMahorn2Data* data;
    Vec position;
    Vec target;
#ifdef MUST_MATCH
    Vec disp = vec3_zero[0];
    Vec targetDisp = vec3_zero[1];
#else
    Vec disp = { 0 };
    Vec targetDisp = { 0 };
#endif
    f32 hitDistance;
    f32 scale;
    f32 angle;
    s32 i;

    data = effect->userdata;
    disp.x = data->position.x;
    disp.y = data->position.y;
    disp.z = data->position.z;
    position = disp;
    targetDisp.x = data->target.x;
    targetDisp.y = data->target.y;
    targetDisp.z = data->target.z;
    target = targetDisp;
    hitDistance = data->hitDistance;
    scale = data->scale;

    for (i = 0; i < effect->count; i++, data++) {
        if (data->counter != 0) {
            data->counter--;
            continue;
        }
        switch (data->type) {
        case 0:
            break;
        case 1:
            data->alpha += 4;
            if (data->alpha > 255) {
                data->alpha = 255;
            }
            data->scale = (0.0f - data->scale) * 0.0625f + data->scale;
            break;
        case 2:
            if (data->intplStep < 90) {
                s32 step = data->intplStep + 1;

                data->intplStep = step;
                data->scale = intplGetValue(11, step, 45, 0.0f, 3.0f);
            }
            break;
        case 3: {
#ifdef MUST_MATCH
            Vec v = vec3_zero[2];
#else
            Vec v = { 0 };
#endif

            v.x = position.x + data->position.x;
            v.y = position.y + data->position.y;
            v.z = position.z + data->position.z;
            if (PSVECDistance(&target, &v) > hitDistance) {
                angle = 6.2831855f * angleABf(position.x + data->position.x, position.y + data->position.y, target.x, target.y) / 360.0f;
                data->position.x += hitDistance * (f32)sin(angle);
                data->position.y += hitDistance * (f32)cos(angle);
                data->scale = (4.0f - data->scale) * 0.125f + data->scale;
            }
            else {
                data->scale = (0.0f - data->scale) * 0.125f + data->scale;
                if (!((effMahorn2Data*)effect->userdata)->hasHit) {
                    ((effMahorn2Data*)effect->userdata)->hasHit = TRUE;
                    effHitEntry(0, 4, target.x, target.y, target.z, scale);
                }
            }
            break;
        }
        case 4:
            if (((effMahorn2Data*)effect->userdata)->hasHit) {
                data->position.x += data->target.x;
                data->position.y += data->target.y;
                data->position.z += data->target.z;
                data->target.x *= 0.88f;
                data->target.y *= 0.88f;
                data->target.z *= 0.88f;
                data->scale = 1.0f;
                data->alpha -= 4;
                if (data->alpha < 0) {
                    data->alpha = 0;
                    effDelete(effect);
                    return;
                }
            }
            break;
        default:
            break;
        }
    }
    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effMahorn2Disp, effect);
}