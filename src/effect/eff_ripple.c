#include <dolphin/types.h>
#include "driver/effdrv.h"
#include "eff/eff_ripple.h"
#include "eff/n64/eff_ripple_n64.h"

EffectEntry* effRippleEntry(s32 type, f32 x, f32 y, f32 z) {
    return effRippleN64Entry(type, x, y, z, 0.0f);
}

void effRippleSetRxRz(EffectEntry* effect, f32 rx, f32 rz) {
    effRippleN64SetRxRz(effect, rx, rz);
}

void effRippleSetCamId(EffectEntry* effect, s32 camId) {
    ((EffRippleData*)effect->userdata)->camId = camId;
}
