#include "PowerPC_EABI_Support/MetroTRK/trk.h"

/*
 * --INFO--
 * Address:	80259738
 * Size:	000008
 */
DSError TRKInitializeDispatcher()
{
	return DS_NoError;
}

/*
 * --INFO--
 * Address:	802595C8
 * Size:	000170
 */
DSError TRKDispatchMessage(TRKBuffer* buffer)
{
	DSError error;

	error = DS_DispatchError;
	TRKSetBufferPosition(buffer, 0);
	MWTRACE(1, "Dispatch command 0x%08x\n", buffer->data[4]);
	switch (buffer->data[4]) {
	case DSMSG_Connect:
		error = TRKDoConnect(buffer);
		break;
	case DSMSG_Disconnect:
		error = TRKDoDisconnect(buffer);
		break;
	case DSMSG_Reset:
		error = TRKDoReset(buffer);
		break;
	case DSMSG_Override:
		error = TRKDoOverride(buffer);
		break;
	case DSMSG_Versions:
		error = TRKDoVersions(buffer);
		break;
	case DSMSG_SupportMask:
		error = TRKDoSupportMask(buffer);
		break;
	case DSMSG_ReadMemory:
		error = TRKDoReadMemory(buffer);
		break;
	case DSMSG_WriteMemory:
		error = TRKDoWriteMemory(buffer);
		break;
	case DSMSG_ReadRegisters:
		error = TRKDoReadRegisters(buffer);
		break;
	case DSMSG_WriteRegisters:
		error = TRKDoWriteRegisters(buffer);
		break;
	case DSMSG_Continue:
		error = TRKDoContinue(buffer);
		break;
	case DSMSG_Step:
		error = TRKDoStep(buffer);
		break;
	case DSMSG_Stop:
		error = TRKDoStop(buffer);
		break;
	case DSMSG_SetOption:
		error = TRKDoSetOption(buffer);
		break;
	}
	MWTRACE(1, "Dispatch complete err = %ld\n", error);
	return error;
}
