// randf, shared by hei_10 and jin_07. Included where it sits in the unit's function order (it sets the .text layout).
#include <stdlib.h>

static f32 randf(f32 min, f32 max) {
    f32 r = (f32)rand() / 32767.0f;

    return r * (max - min) + min;
}
