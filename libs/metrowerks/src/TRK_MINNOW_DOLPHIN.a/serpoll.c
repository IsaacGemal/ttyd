/* FIX_SQRT_LINKAGE: w_math.h's extern-inline sqrtf carries initialized static data, which turns off
 * data pooling for the whole translation unit (documented at the top of w_math.h). The original
 * serpoll.o pools the trace strings (base + raw offsets), so sqrtf must stay static/unemitted here. */
#include "PowerPC_EABI_Support/MetroTRK/trk.h"

/*
 * TTYD's TRKFramingState stores receiveState as the ReceiverState enum (4 bytes), not as a u8 like
 * the vendored ac-decomp header. The `stw r0, 0x8(r6)` in TRKInitializeSerialHandler proves it.
 * gTRKFramingState is only used by this file, so a file-local (correctly typed) struct is used here.
 * Offsets are unchanged (msgBufID 0x0, buffer 0x4, receiveState 0x8, isEscape 0xC, fcsType 0x10).
 */
typedef struct TRKFramingStateLocal {
    TRKBufferID msgBufID;       // _00
    TRKBuffer* buffer;          // _04
    ReceiverState receiveState; // _08
    BOOL isEscape;              // _0C
    u8 fcsType;                 // _10
} TRKFramingStateLocal;

TRKFramingStateLocal gTRKFramingState;

void* gTRKInputPendingPtr;

// TTYD's TRKTestForPacket is a UART packet reader, not the ac-decomp TRKFramingState state machine.
// (Declared here because the vendored header's prototype is misspelled "TRKTestForPacker".)
int TRKTestForPacket(void);

int TRKTestForPacket(void)
{
    int bufID;
    TRKBuffer* buffer;
    u8 header[0x40];
    u8 payload[0x880];
    int var_r30;
    UARTError err;

    if (TRKPollUART() <= 0)
        return -1;

    var_r30 = TRKGetFreeBuffer(&bufID, &buffer);
    MWTRACE(4, "TestForPacket : FreeBuffer is  %ld\n", var_r30);
    TRKSetBufferPosition(buffer, 0);
    err = TRKReadUARTN(header, 0x40);
    if (err == DS_NoError) {
        TRKAppendBuffer_ui8(buffer, header, 0x40);
        var_r30 = bufID;
        if ((s32)(*(u32*)header - 0x40) > 0) {
            MWTRACE(1, "Reading payload %ld bytes\n", *(u32*)header - 0x40);
            err = TRKReadUARTN(payload, *(u32*)header - 0x40);
            if (err == DS_NoError) {
                TRKAppendBuffer_ui8(buffer, payload, *(u32*)header);
            } else {
                MWTRACE(8, "TestForPacket : Invalid size of packet hdr.size\n");
                TRKReleaseBuffer(var_r30);
                var_r30 = -1;
            }
        }
    } else {
        MWTRACE(8, "TestForPacket : Invalid size of packet\n");
        TRKReleaseBuffer(var_r30);
        var_r30 = -1;
    }
    MWTRACE(1, "TestForPacket returning %ld\n", var_r30);
    return var_r30;
}

void TRKGetInput(void)
{
    int id;

    id = TRKTestForPacket();
    if (id == -1)
        return;

    TRKGetBuffer(id);
    TRKProcessInput(id);
}

void TRKProcessInput(int bufferIdx)
{
    TRKEvent event;

    TRKConstructEvent(&event, NUBEVENT_Request);
    gTRKFramingState.msgBufID = -1;
    event.msgBufID            = bufferIdx;
    TRKPostEvent(&event);
}

DSError TRKInitializeSerialHandler(void)
{
    gTRKFramingState.msgBufID     = -1;
    gTRKFramingState.receiveState = DSRECV_Wait;
    gTRKFramingState.isEscape     = FALSE;

    MWTRACE(1, "TRK_Packet_Header \t    %ld bytes\n", 0x40);
    MWTRACE(1, "TRK_CMD_ReadMemory     %ld bytes\n", 0x40);
    MWTRACE(1, "TRK_CMD_WriteMemory    %ld bytes\n", 0x40);
    MWTRACE(1, "TRK_CMD_Connect \t    %ld bytes\n", 0x40);
    MWTRACE(1, "TRK_CMD_ReplyAck\t    %ld bytes\n", 0x40);
    MWTRACE(1, "TRK_CMD_ReadRegisters\t%ld bytes\n", 0x40);

    return DS_NoError;
}

DSError TRKTerminateSerialHandler(void) { return DS_NoError; }
