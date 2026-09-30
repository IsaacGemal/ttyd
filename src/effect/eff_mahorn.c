#include "eff/eff_mahorn.h"
#include "driver/dispdrv.h"
#include "memory.h"

// .sdata2 constants (defined by the matched data objects; named so loads match retail).
extern const f32 float_100_80422260;
extern const f32 float_20_80422264;
extern const f32 float_0p125_80422268;

// Scale curves for the two particle banks; each holds 21 entries (the rest is padding).
static u8 scale_dt[] = {
    0x00, 0x07, 0x0E, 0x16, 0x1E, 0x26, 0x2F, 0x37, 0x3F, 0x47, 0x4E,
    0x54, 0x5A, 0x5E, 0x62, 0x64, 0x64, 0x55, 0x33, 0x10, 0x00,
};
static u8 scale_dt2[] = {
    0x00, 0x13, 0x29, 0x3F, 0x52, 0x5F, 0x64, 0x63, 0x5F, 0x59, 0x51,
    0x47, 0x3D, 0x33, 0x28, 0x1E, 0x14, 0x0C, 0x06, 0x02, 0x00,
};

EffectEntry* effMahornEntry(BOOL unused, s32 count, f32 posX, f32 posY, f32 posZ, f32 scale, s32 unk) {
    EffectEntry* effect;
    effMahornData* data;
    s32 i;
    s32 newCount;

    effect = effEntry();
    effect->type = "Mahorn";
    newCount = count + 1;
    effect->count = newCount;
    data = __memAlloc(HEAP_EFFECT, newCount * sizeof(effMahornData));
    effect->userdata = data;
    effect->callback = effMahornMain;
    data->unused = unused;
    data->counter = 0;
    if (unk <= 0) {
        data->timer = 1000;
    }
    else {
        data->timer = unk;
    }
    data->position.x = posX;
    data->position.y = posY;
    data->position.z = posZ;
    data->scale = scale;
    data->alpha = 0xff;
    data++;
    for (i = 1; i < effect->count; i++, data++) {
        data->position.x = 0.0f;
        data->position.y = 0.0f;
        data->position.z = 0.0f;
        data->scale = 0.0f;
        data->field_0x14 = 0.0f;
        data->alpha = 0xff;
        data->field_0x24 = i * 4;
    }
    return effect;
}

// NONMATCHING (99.85%): in the field_0x14 decay retail loads float_20 into f1 and float_0p125 into
// f0, so the subtraction reuses f1; this build swaps the pair (f0/f1), which is the only remaining
// difference (the two lfs and the fsubs). Every other instruction, including the data symbols, matches.
void effMahornMain(EffectEntry* effect) {
    effMahornData* data = effect->userdata;
#ifdef MUST_MATCH
    Vec position;
#endif
    Vec disp = { 0 };
    f32 scale;
    s32 i;

    disp.x = data->position.x;
    disp.y = data->position.y;
    disp.z = data->position.z;
#ifdef MUST_MATCH
    position = disp;
#endif

    if (data->timer < 1000) {
        data->timer--;
    }
    data->counter++;
    if (data->timer < 16) {
        data->alpha = data->timer << 4;
    }
    if (data->timer < 0) {
        effDelete(effect);
        return;
    }

    data++;
    for (i = 1; i < effect->count; i++, data++) {
        if (data->field_0x24 != 0) {
            data->field_0x24--;
        }
        else {
            data->counter++;
            if (i & 1) {
                scale = (f32)scale_dt[(u32)data->counter % 21] / float_100_80422260;
            }
            else {
                scale = (f32)scale_dt2[(u32)data->counter % 21] / float_100_80422260;
            }
            data->scale = scale;
            data->field_0x14 = (float_20_80422264 - data->field_0x14) * float_0p125_80422268 + data->field_0x14;
            data->alpha += -data->alpha / 64;
        }
    }
#ifdef MUST_MATCH
    dispEntry(4, dispCalcZ(&position), 2, effMahornDisp, effect);
#else
    dispEntry(4, dispCalcZ(&disp), 2, effMahornDisp, effect);
#endif
}
