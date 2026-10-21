// _disp_icon (see rel/common/unit_boss_koopa.h). Included where it sits in the unit's function order (it sets the
// .text layout).
#include "driver/camdrv.h"
#include "evt/evt_cmd.h"
#include "rel/common/unit_boss_koopa.h"

static void _disp_icon(CameraId cameraId, void* param) {
    EventEntry* event = param;
    IconJump* obj = (IconJump*)event->userdata[0];

    iconDispGx(&obj->pos, 0, event->userdata[1], 1.0f);
}
