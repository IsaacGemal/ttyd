#include "eff/eff_hit.h"
#include "eff/n64/eff_hit_n64.h"

EffectEntry* effHitEntry(s32 a, s32 b, f32 c, f32 d, f32 e, f32 f) {
    return effHitN64Entry(a, b, c, d, e, f);
}
