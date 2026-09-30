#include "eff/n64/eff_pturn_n64.h"
#include "memory.h"

// .rodata and .sdata2 constants (named after their original addresses in the asm).
extern const char str_802FB468[];
extern const f32 float_1_8041fa64;
extern const f32 float_0_8041fa68;

EffectEntry* effPturnN64Entry(s32 type, s32 a1, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    effPturnN64Data* data;

    effect = effEntry();
    effect->type = str_802FB468;
    effect->count = 2;
    data = __memAlloc(HEAP_EFFECT, 0x90);
    effect->userdata = data;
    effect->callback = effPturnMain;
    effect->flags |= 2;
    data->field_0x0 = type;
    data->field_0x34 = float_1_8041fa64;
    if (type == 1) {
        data->field_0x38 = 1;
        data->field_0x3C = 0;
    } else {
        data->field_0x38 = 0;
        data->field_0x3C = a1;
    }
    data->x = x;
    data->y = y;
    data->z = z;
    switch (type) {
    case 0:
        data->life = 100;
        break;
    case 1:
        data->life = 100;
        break;
    case 10:
        data->life = 0x15;
        break;
    }
    data->field_0x2C = 0;
    data->field_0x44 = 0;
    data->field_0x4 = 1;
    data->field_0x58 = float_0_8041fa68;
    data->field_0x54 = float_0_8041fa68;
    data->field_0x50 = float_0_8041fa68;
    data->field_0x68 = float_0_8041fa68;
    data->field_0x64 = float_0_8041fa68;
    data->field_0x60 = float_0_8041fa68;
    data->field_0x5C = float_0_8041fa68;
    data->field_0x88 = float_0_8041fa68;
    data->field_0x70 = 0;
    data->field_0x6C = 0xFF;
    return effect;
}

void effPturnDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    effPturnN64Data* data = effect->userdata;

    if (data->field_0x4 == 0) {
        return;
    }
    pturn_main_dispp(camId, param);
    if (data->field_0x0 == 1) {
        return;
    }
    pturn_num_dispp(camId, param);
}
