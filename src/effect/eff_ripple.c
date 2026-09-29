#include <dolphin/types.h>
#include "driver/effdrv.h"
#include "eff/eff_ripple.h"

void effRippleSetCamId(EffectEntry* effect, s32 camId) {
    ((EffRippleData*)effect->userdata)->camId = camId;
}
