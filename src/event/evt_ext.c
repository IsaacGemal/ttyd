#include "evt/evt_ext.h"
#include "driver/extdrv.h"

USER_FUNC(evt_ext_reset) {
	extReset();
	return EVT_RETURN_DONE;
}
