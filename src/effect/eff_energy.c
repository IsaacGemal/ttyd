#include "eff/eff_energy.h"
#include "driver/effdrv.h"
#include "memory.h"

typedef struct EnergyData {
    s32 type;       // 0x00
    f32 x;          // 0x04
    f32 y;          // 0x08
    f32 z;          // 0x0C
    f32 scale;      // 0x10
    f32 scaleAnim;  // 0x14
    f32 scaleSpeed; // 0x18
    f32 angle;      // 0x1C
    f32 angleSpeed; // 0x20
    s32 alpha;      // 0x24
    s32 lifetime;   // 0x28
    s32 alphaAnim;  // 0x2C
    s32 alphaSpeed; // 0x30
} EnergyData;

const char str_8030148C[] = "Energy";
extern const f32 float_0_80422470;

EffectEntry* effEnergyEntry(s32 a, s32 b, f32 p1, f32 p2, f32 p3, f32 p4, f32 p5, f32 p6) {
    EffectEntry* entry;
    EnergyData* data;

    entry = effEntry();
    entry->type = str_8030148C;
    entry->count = 1;
    data = __memAlloc(HEAP_EFFECT, entry->count * sizeof(EnergyData));
    entry->userdata = data;
    entry->callback = effEnergyMain;

    data->type = a;
    data->x = p1;
    data->y = p2;
    data->z = p3;
    data->angle = float_0_80422470;
    data->angleSpeed = p5;
    data->scale = p4;
    data->scaleAnim = float_0_80422470;
    data->scaleSpeed = p6;
    data->alpha = 0xFF;
    data->alphaAnim = 0;
    data->alphaSpeed = 0x20;
    if (b <= 0) {
        data->lifetime = 1000;
    } else {
        data->lifetime = b;
    }

    return entry;
}
