// eff_poison_breath, shared by the Poison Puff units (unit_dokugassun, both versions) and unit_poison_pakkun. Included
// where it sits in the unit's function order (it sets the .text layout).
#include "eff/n64/eff_vapor_n64.h"
#include "evt/evt_cmd.h"
#include "system.h"
#include <stdlib.h>

static USER_FUNC(eff_poison_breath) {
    effVaporN64Data* data;
    f32 x;
    f32 y;
    f32 z;

    if (evtGetValue(event, event->args[0]) == 0) {
        x = -125.0f;
    } else {
        x = 125.0f;
    }
    x += (f32)(irand(150) - 75);
    y = (f32)irand(70);
    z = (f32)(irand(80) - 40);
    data = effVaporN64Entry(0, 15, x, y, z, 2.5f + (f32)rand() / 32767.0f)->userdata;
    data->color1R = 32;
    data->color1G = 115;
    data->color1B = 100;
    data->color1A = 255;
    data->color2R = 32;
    data->color2G = 115;
    data->color2B = 100;
    data->color2A = 255;
    return EVT_RETURN_DONE;
}
