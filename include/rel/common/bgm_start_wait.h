// bgm_start_wait, as the lecture units (rel/common/evt_lect.h) and several map units have it. Included where it sits
// in the unit's function order (it sets the .text layout).
#include "evt/evt_cmd.h"
#include "pmario_sound.h"

static USER_FUNC(bgm_start_wait) {
    return psndBGMStartCheck(1) ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}
