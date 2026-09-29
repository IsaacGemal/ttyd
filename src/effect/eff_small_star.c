#include "eff/eff_small_star.h"
#include "eff/n64/eff_small_star_n64.h"

void* effSmallStarEntry(s32 a, s32 b, f32 x, f32 y, f32 z, f32 f4, f32 f5, f32 f6) {
    return effSmallStarN64Entry(a, b, x, y, z, f4, f5, f6);
}
