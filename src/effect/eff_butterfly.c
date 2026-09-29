#include "eff/eff_butterfly.h"
#include "eff/n64/eff_butterfly_n64.h"

EffectEntry* effButterflyEntry(s32 a, f32 x, f32 y, f32 z) {
    return effButterflyN64Entry(a, x, y, z);
}
