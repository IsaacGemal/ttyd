#include "evt/evt_ext.h"
#include "driver/extdrv.h"
#include "manager/evtmgr_cmd.h"

USER_FUNC(evt_ext_entry) {
	s32* args = event->args;
	s32 a0 = evtGetValue(event, args[0]);
	s32 a1 = evtGetValue(event, args[1]);
	s32 a2 = evtGetValue(event, args[2]);
	s32 a3 = evtGetValue(event, args[3]);
	s32 a4 = evtGetValue(event, args[4]);

	extEntry(a0, (void*)a1, (void (*)(void))a2, (void (*)(void))a3, (void (*)(void))a4);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_ext_reset) {
	extReset();
	return EVT_RETURN_DONE;
}
