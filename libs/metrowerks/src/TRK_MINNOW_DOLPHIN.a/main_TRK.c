#include "PowerPC_EABI_Support/MetroTRK/trk.h"

static DSError TRK_mainError;

/*
 * --INFO--
 * Address:	80220418
 * Size:	000048
 */
DSError TRK_main(void)
{
	MWTRACE(1, "TRK_Main \n");
	TRK_mainError = TRKInitializeNub();

	if (TRK_mainError == DS_NoError) {
		TRKNubWelcome();
		TRKNubMainLoop();
	}

	return TRK_mainError = TRKTerminateNub();
}
