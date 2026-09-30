#include <dolphin/types.h>
#include "driver/effdrv.h"
#include "eff/eff_ripple.h"
#include "eff/n64/eff_ripple_n64.h"

void effRippleSetRxRz(EffectEntry* effect, f32 rx, f32 rz) {
    effRippleN64SetRxRz(effect, rx, rz);
}

void effRippleSetCamId(EffectEntry* effect, s32 camId) {
    ((EffRippleData*)effect->userdata)->camId = camId;
}
