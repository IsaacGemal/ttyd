#include "evt/evt_env.h"

#include <dolphin/os.h>

#include "driver/envdrv.h"
#include "manager/evtmgr_cmd.h"

USER_FUNC(evt_env_blur_on) {
	s32* args = event->args;
	s32 flag = evtGetValue(event, args[0]);
	s32 value = evtGetValue(event, args[1]);
	u64 start = event->lifetime;

	if (isFirstCall) {
		envBlurOn(flag, value);
		event->userdata[0] = (start >> 32) & 0xFFFFFFFF;
		event->userdata[1] = start & 0xFFFFFFFF;
	}

	if (value != 0) {
		if (OSTicksToMilliseconds(start - *(OSTime*)&event->userdata[0]) >= value) {
			envBlurOff();
			return EVT_RETURN_DONE;
		}
	}
	return EVT_RETURN_BLOCK;
}
