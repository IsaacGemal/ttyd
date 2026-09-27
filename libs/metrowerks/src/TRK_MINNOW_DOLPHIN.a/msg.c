#include "PowerPC_EABI_Support/MetroTRK/trk.h"

DSError TRKMessageSend(TRKBuffer* msg)
{
    DSError err = TRKWriteUARTN(msg->data, msg->length);
    MWTRACE(1, "MessageSend : cc_write returned %ld\n", err);
    return DS_NoError;
}
