#include "eff/n64/eff_opuku_jetw_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// Local copy of system.c's inline helper so it inlines here (as in eff_kemuri11.c).
inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

void effOpukuJetwMain(EffectEntry* effect) {
    effOpukuJetwData* data = effect->userdata;
    Vec position = initializeVec(&data->position);

    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->lifetime = 0x10;
    }
    if (data->lifetime < 1000) {
        data->lifetime--;
    }
    data->counter++;
    if (data->lifetime < 0) {
        effDelete(effect);
    } else {
        if (data->counter < 8) {
            data->field_0x24 = data->counter * 32 + 31;
        }
        dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effOpukuJetwDisp, effect);
    }
}
