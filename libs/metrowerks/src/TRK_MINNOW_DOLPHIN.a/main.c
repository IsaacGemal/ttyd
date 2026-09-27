#include "PowerPC_EABI_Support/MetroTRK/trk.h"
#include "amcstubs/AmcExi2Stubs.h"

#define DDH_BUF_SIZE 0x800

// CircleBuffer lives in CircleBuffer.c and has no header here. TTYD's copy is 0x1C bytes: the six pointer/count fields
// followed by a 4-byte critical section at 0x18 (CircleBuffer.c calls
// MWInitializeCriticalSection on offset 0x18). We only ever pass its address, so declare
// the callees with an opaque pointer and keep the buffer as raw bytes.
void CircleBufferInitialize(void* cb, u8* buf, u32 size);
u32 CBGetBytesAvailableForRead(void* cb);
int CircleBufferReadBytes(void* cb, u8* data, u32 size);
int CircleBufferWriteBytes(void* cb, u8* data, u32 size);

static u8 gRecvCB[0x1C] ATTRIBUTE_ALIGN(8);
static u8 gRecvBuf[DDH_BUF_SIZE];
// TTYD keeps gIsInitialized in .sbss and reads it with @sda21; with this lib's
// -sdata 0 the only way to get that section/addressing is to name the small-data section.
__declspec(section ".sdata") static BOOL gIsInitialized;

static const char ddh_cc_write_not_initialized[] = "cc not initialized\n";
static const char ddh_cc_write_output_data[] = "cc_write : Output data 0x%08x %ld bytes\n";
static const char ddh_cc_write_sending[] = "cc_write sending %ld bytes\n";
static const char ddh_cc_expected_packet_size[] = "Expected packet size : 0x%08x (%ld)\n";
static const char ddh_cc_read_error[] = "cc_read : error reading bytes from EXI2 %ld\n";
static const char ddh_cc_initialize_calling_exi2_init[] = "CALLING EXI2_Init\n";
static const char ddh_cc_initialize_done_calling_exi2_init[] = "DONE CALLING EXI2_Init\n";

int ddh_cc_initinterrupts(void) {
    EXI2_EnableInterrupts();
    return 0;
}

int ddh_cc_peek(void) {
    int poll;
    u8 buff[DDH_BUF_SIZE];

    poll = EXI2_Poll();
    if (poll <= 0) {
        return 0;
    }

    if (EXI2_ReadN(buff, poll) == 0) {
        CircleBufferWriteBytes(&gRecvCB, buff, poll);
    } else {
        return DDH_ERR_READ_ERROR;
    }

    return poll;
}

int ddh_cc_post_stop(void) {
    EXI2_Reserve();
    return 0;
}

int ddh_cc_pre_continue(void) {
    EXI2_Unreserve();
    return 0;
}

int ddh_cc_write(const u8* bytes, int length) {
    int exi2Len;
    int nCopy;
    u32 bytesAddr;

    bytesAddr = (u32)bytes;
    nCopy = length;

    if (gIsInitialized == FALSE) {
        MWTRACE(8, (char*)ddh_cc_write_not_initialized);
        return DDH_ERR_NOT_INITIALIZED;
    }

    MWTRACE(8, (char*)ddh_cc_write_output_data, bytes, length);

    while (nCopy > 0) {
        MWTRACE(1, (char*)ddh_cc_write_sending, nCopy);
        exi2Len = EXI2_WriteN((const void*)bytesAddr, nCopy);

        if (exi2Len == AMC_EXI_NO_ERROR) {
            break;
        }

        bytesAddr += exi2Len;
        nCopy -= exi2Len;
    }

    return 0;
}

int ddh_cc_read(u8* data, int size) {
    u8 buff[DDH_BUF_SIZE];
    int originalDataSize;
    u32 result;
    int expectedDataSize;
    int poll;

    result = 0;
    if (!gIsInitialized) {
        return DDH_ERR_NOT_INITIALIZED;
    }

    MWTRACE(1, (char*)ddh_cc_expected_packet_size, size, size);
    originalDataSize = expectedDataSize = size;

    while ((u32)CBGetBytesAvailableForRead(&gRecvCB) < expectedDataSize) {
        result = 0;
        poll = EXI2_Poll();

        if (poll != 0) {
            result = EXI2_ReadN(buff, poll);
            if (result == 0) {
                CircleBufferWriteBytes(&gRecvCB, buff, poll);
            }
        }
    }

    if (result == 0) {
        CircleBufferReadBytes(&gRecvCB, data, originalDataSize);
    } else {
        MWTRACE(8, (char*)ddh_cc_read_error, result);
    }

    return result;
}

int ddh_cc_close(void) {
    return 0;
}

int ddh_cc_open(void) {
    if (gIsInitialized != 0) {
        return DDH_ERR_ALREADY_INITIALIZED;
    }

    gIsInitialized = TRUE;
    return 0;
}

int ddh_cc_shutdown(void) {
    return 0;
}

int ddh_cc_initialize(void* inputPendingPtrRef, __OSInterruptHandler monitorCallback) {
    MWTRACE(1, (char*)ddh_cc_initialize_calling_exi2_init);
    EXI2_Init(inputPendingPtrRef, monitorCallback);
    MWTRACE(1, (char*)ddh_cc_initialize_done_calling_exi2_init);
    CircleBufferInitialize(&gRecvCB, gRecvBuf, DDH_BUF_SIZE);
    return 0;
}
