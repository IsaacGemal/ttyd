#include "dolphin/types.h"
#include "dolphin/os/OSInterrupt.h"

void* memcpy(void* dest, const void* src, size_t n);
void MWTRACE(u8, char*, ...);

int gdev_cc_initialize(void* flagOut, __OSInterruptHandler handler);
int gdev_cc_shutdown();
int gdev_cc_open();
int gdev_cc_close();
int gdev_cc_read(u8* dest, int size);
int gdev_cc_write(const u8* src, int size);
int gdev_cc_pre_continue();
int gdev_cc_post_stop();
int gdev_cc_peek();
int gdev_cc_initinterrupts();

#define GDEV_ERR_NOT_INITIALIZED     -0x2711
#define GDEV_ERR_ALREADY_INITIALIZED -0x2715
#define GDEV_ERR_READ_ERROR          -0x2719

#define GDEV_BUF_SIZE 0x500

typedef struct CircleBuffer {
	u8* read_ptr;      // _00
	u8* write_ptr;     // _04
	u8* start_ptr;     // _08
	u32 size;          // _0C
	u32 mBytesToRead;  // _10
	u32 mBytesToWrite; // _14
	u32 mCriticalSection; // _18
} CircleBuffer;

typedef struct GdevRecvCB {
	CircleBuffer cb;
} GdevRecvCB;

typedef struct GdevInitFlag {
	BOOL value;
} GdevInitFlag;

void MWInitializeCriticalSection(u32* section);
void MWEnterCriticalSection(u32* section);
void MWExitCriticalSection(u32* section);

void DBInitComm(volatile u8** param_1, __OSInterruptHandler param_2);
void DBInitInterrupts(void);
u32 DBQueryData(void);
int DBRead(void* data, u32 size);
int DBWrite(const void* data, u32 size);
void DBOpen(void);
void DBClose(void);

u32 CBGetBytesAvailableForRead(CircleBuffer* cb);
void CircleBufferInitialize(CircleBuffer* cb, u8* buf, s32 size);
int CircleBufferWriteBytes(CircleBuffer* cb, u8* buf, u32 size);
int CircleBufferReadBytes(CircleBuffer* cb, u8* buf, u32 size);

static const char str_803022E0[20] = "cc not initialized\n";
static const char str_803022F4[44] = "cc_write : Output data 0x%08x %ld bytes\n";
static const char str_80302320[28] = "cc_write sending %ld bytes\n";
static const char str_8030233C[40] = "Expected packet size : 0x%08x (%ld)\n";
static const char str_80302364[48] = "cc_read : error reading bytes from EXI2 %ld\n";
static const char str_80302394[20] = "CALLING EXI2_Init\n";
static const char str_803023A8[24] = "DONE CALLING EXI2_Init\n";

static GdevRecvCB gRecvCB ATTRIBUTE_ALIGN(8);
static u8 gRecvBuf[GDEV_BUF_SIZE];
__declspec(section ".sdata") static GdevInitFlag gIsInitialized;

int gdev_cc_initialize(void* inputPendingPtrRef, __OSInterruptHandler monitorCallback) {
	MWTRACE(1, (char*)str_80302394);
	DBInitComm(inputPendingPtrRef, monitorCallback);
	MWTRACE(1, (char*)str_803023A8);
	CircleBufferInitialize(&gRecvCB.cb, gRecvBuf, GDEV_BUF_SIZE);
	return 0;
}

int gdev_cc_shutdown() {
	return 0;
}

int gdev_cc_open() {
	if (gIsInitialized.value != 0) {
		return GDEV_ERR_ALREADY_INITIALIZED;
	}

	gIsInitialized.value = TRUE;
	return 0;
}

int gdev_cc_close() {
	return 0;
}

int gdev_cc_read(u8* data, int size) {
	u8 buff[GDEV_BUF_SIZE];
	int originalDataSize;
	u32 result;
	int expectedDataSize;
	int poll;

	result = 0;
	if (!gIsInitialized.value) {
		return GDEV_ERR_NOT_INITIALIZED;
	}

	MWTRACE(1, (char*)str_8030233C, size, size);
	originalDataSize = expectedDataSize = size;

	while ((u32)CBGetBytesAvailableForRead(&gRecvCB.cb) < expectedDataSize) {
		result = 0;
		poll = DBQueryData();

		if (poll != 0) {
			result = DBRead(buff, expectedDataSize);
			if (result == 0) {
				CircleBufferWriteBytes(&gRecvCB.cb, buff, poll);
			}
		}
	}

	if (result == 0) {
		CircleBufferReadBytes(&gRecvCB.cb, data, originalDataSize);
	} else {
		MWTRACE(8, (char*)str_80302364, result);
	}

	return result;
}

int gdev_cc_write(const u8* bytes, int length) {
	int exi2Len;
	int nCopy;
	u32 bytesAddr;

	bytesAddr = (u32)bytes;
	nCopy = length;

	if (gIsInitialized.value == FALSE) {
		MWTRACE(8, (char*)str_803022E0);
		return GDEV_ERR_NOT_INITIALIZED;
	}

	MWTRACE(8, (char*)str_803022F4, bytes, length);

	while (nCopy > 0) {
		MWTRACE(1, (char*)str_80302320, nCopy);
		exi2Len = DBWrite((const void*)bytesAddr, nCopy);
		if (exi2Len == 0) {
			break;
		}

		bytesAddr += exi2Len;
		nCopy -= exi2Len;
	}

	return 0;
}

int gdev_cc_pre_continue() {
	DBClose();
	return 0;
}

int gdev_cc_post_stop() {
	DBOpen();
	return 0;
}

int gdev_cc_peek() {
	int poll;
	u8 buff[GDEV_BUF_SIZE];

	poll = DBQueryData();
	if (poll <= 0) {
		return 0;
	}

	if (DBRead(buff, poll) == 0) {
		CircleBufferWriteBytes(&gRecvCB.cb, buff, poll);
	} else {
		return GDEV_ERR_READ_ERROR;
	}

	return poll;
}

int gdev_cc_initinterrupts() {
	DBInitInterrupts();
	return 0;
}

// dont_inline keeps the CircleBuffer calls above as real `bl`s (the entry points only
// see the prototypes, and the compiler is told not to inline the definitions below).
#pragma dont_inline on

u32 CBGetBytesAvailableForRead(CircleBuffer* cb) {
	return cb->mBytesToRead;
}

void CircleBufferInitialize(CircleBuffer* cb, u8* buf, s32 size) {
	cb->start_ptr = buf;
	cb->size = size;
	cb->read_ptr = cb->start_ptr;
	cb->write_ptr = cb->start_ptr;
	cb->mBytesToRead = 0;
	cb->mBytesToWrite = cb->size;
	MWInitializeCriticalSection(&cb->mCriticalSection);
}

int CircleBufferWriteBytes(CircleBuffer* cb, u8* buf, u32 size) {
	int availSize;

	if (size > cb->mBytesToWrite) {
		return -1;
	}

	MWEnterCriticalSection(&cb->mCriticalSection);
	availSize = cb->size - (cb->write_ptr - cb->start_ptr);
	if (availSize >= size) {
		memcpy(cb->write_ptr, buf, size);
		cb->write_ptr += size;
	} else {
		memcpy(cb->write_ptr, buf, availSize);
		memcpy(cb->start_ptr, buf + availSize, size - availSize);
		cb->write_ptr = cb->start_ptr + size - availSize;
	}

	if (cb->size == (cb->write_ptr - cb->start_ptr)) {
		cb->write_ptr = cb->start_ptr;
	}

	cb->mBytesToWrite -= size;
	cb->mBytesToRead += size;
	MWExitCriticalSection(&cb->mCriticalSection);
	return 0;
}

int CircleBufferReadBytes(CircleBuffer* cb, u8* buf, u32 size) {
	int availSize;

	if (size > cb->mBytesToRead) {
		return -1;
	}

	MWEnterCriticalSection(&cb->mCriticalSection);
	availSize = cb->size - (cb->read_ptr - cb->start_ptr);
	if (size < availSize) {
		memcpy(buf, cb->read_ptr, size);
		cb->read_ptr += size;
	} else {
		memcpy(buf, cb->read_ptr, availSize);
		memcpy(buf + availSize, cb->start_ptr, size - availSize);
		cb->read_ptr = cb->start_ptr + size - availSize;
	}

	if (cb->size == (cb->read_ptr - cb->start_ptr)) {
		cb->read_ptr = cb->start_ptr;
	}

	cb->mBytesToWrite += size;
	cb->mBytesToRead -= size;
	MWExitCriticalSection(&cb->mCriticalSection);
	return 0;
}

#pragma dont_inline reset
