#include <dolphin/types.h>
#include "manager/evtmgr.h"
#include "evt/evt_cmd.h"

// .sbss:0x0 | 0x80418A28
s32 mono_alpha;

USER_FUNC(mono_on) {
    if (isFirstCall) {
        mono_alpha = 0;
    }
    mono_alpha += 10;
    if (mono_alpha > 0xFF) {
        mono_alpha = 0xFF;
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}
