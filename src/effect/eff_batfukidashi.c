#include "eff/eff_batfukidashi.h"
#include "driver/dispdrv.h"
#include "memory.h"

extern const f32 float_0_80420478;
extern const f32 float_1_8042047c;
extern const f32 float_100_80420494;

const Vec vec3_802fd498 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_mask_802fd4a8 = 4503599627370496.0;

const char str_802FD4B0[] = "CoinFukidashi";

EffectEntry* effCoinFukidashiEntry(s32 type, s32 coinNum, s32 count, f32 x, f32 y, f32 z) {
    EffectEntry* entry;
    CoinFukidashiData* data;

    entry = effEntry();
    entry->type = str_802FD4B0;
    entry->count = 1;
    data = __memAlloc(HEAP_EFFECT, entry->count * sizeof(CoinFukidashiData));
    entry->userdata = data;
    entry->callback = effCoinFukidashiMain;
    entry->flags |= 2;

    data->type = type;
    data->position.x = x;
    data->position.y = y;
    data->position.z = z;
    data->field_0x10 = float_0_80420478;
    data->scale = float_1_8042047c;
    data->field_0x14 = coinNum;

    if (count <= 0) {
        data->field_0x20 = 1000;
    } else {
        data->field_0x20 = count;
    }
    data->field_0x24 = 0;
    data->alpha = 0;

    return entry;
}

// .data table driving the coin's vertical rise (asm: .obj offset_tbl, local)
static u8 offset_tbl[] = {
    0x28, 0x23, 0x1E, 0x19, 0x14, 0x0F, 0x0A, 0x06,
    0x02, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x04,
    0x06, 0x08, 0x0A, 0x0C, 0x0F, 0x11, 0x14, 0x16,
    0x19, 0x1C, 0x1E, 0x21, 0x23, 0x25, 0x28, 0x28,
    0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28,
    0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28,
    0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28,
    0x28, 0x28, 0x28, 0x28, 0x28, 0x00, 0x00, 0x00,
};

void effCoinFukidashiMain(EffectEntry* effect) {
    CoinFukidashiData* data;
#ifdef MUST_MATCH
    /* MUST_MATCH: retail builds the Vec in a scratch and copies it to a second local before
       dispCalcZ; a single local folds the copy away. */
    Vec position;
#endif
    Vec disp = { 0.0f, 0.0f, 0.0f };

    data = effect->userdata;
    disp.x = data->position.x;
    disp.y = data->position.y;
    disp.z = data->position.z;
#ifdef MUST_MATCH
    position = disp;
#endif

    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->field_0x20 = 16;
    }

    if (data->field_0x20 < 1000) {
        data->field_0x20--;
    }

    if (data->field_0x20 < 16) {
        data->alpha = data->field_0x20 * 16;
    } else {
        data->alpha += 16;
        if (data->alpha > 255) {
            data->alpha = 255;
        }
    }

    if (data->field_0x20 < 0) {
        effDelete(effect);
        return;
    }

    data->field_0x10 = (f32)offset_tbl[data->field_0x24] / float_100_80420494;
    data->field_0x24++;
    if ((u32)data->field_0x24 >= 61) {
        data->field_0x24 = 0;
    }

#ifdef MUST_MATCH
    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effCoinFukidashiDisp, effect);
#else
    dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effCoinFukidashiDisp, effect);
#endif
}
