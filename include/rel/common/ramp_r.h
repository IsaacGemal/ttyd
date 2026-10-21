// ramp_r, shared by gor_04, hom_10 and pik_00. Included where it sits in the unit's function order (it sets the .text
// layout).
#include "driver/envdrv.h"
#include "evt/evt_cmd.h"
#include "mario/mariost.h"

#include "math_ext.h"

static USER_FUNC(ramp_r) {
    f32 value = __fabsf(evtGetFloat(event, GW(0)));
    f32 wave = (f32)sin((f32)gp->retraceCount / 15.0f);
    evtSetFloat(event, LW(10), 1.8f * value * wave);
    return EVT_RETURN_DONE;
}
