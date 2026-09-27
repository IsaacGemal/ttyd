#include "PowerPC_EABI_Support/MetroTRK/trktypes.h"

// TTYD's copy of this library has a newer DBCommTable than the ac-decomp header:
// 10 pointers, with shutdown at _08 and peek at _0C. The vendored header has 7
// with peek at _08, so declare the real layout here instead of editing it. The
// header's trk.h is not included because its InitializeProgramEndTrap
// prototype (DSError) does not fit TTYD's void version, so the TRK helpers and
// the OS/driver entry points used here are declared locally too. dolphin/
// os.h and DebuggerDriver.h are deliberately avoided: they pull in the root
// types.h and emit the sqrtf inline statics into .rodata ahead of our own data.
// Functions are written in the order the object's symbol table expects (the
// build compiles them in reverse, see reverse_fn_order in objdiff.json).
void OSReport(const char* fmt, ...);
s32 OSEnableScheduler(void);
void ICInvalidateRange(void* addr, u32 nBytes);
void DCFlushRange(void* addr, u32 nBytes);
int Hu_IsStub();
BOOL AMC_IsStub(void);

typedef struct TRKCommTable {
	DBCommInitFunc initialize_func;  // _00
	DBCommFunc init_interrupts_func; // _04
	DBCommFunc shutdown_func;        // _08
	DBCommFunc peek_func;            // _0C
	DBCommReadFunc read_func;        // _10
	DBCommWriteFunc write_func;      // _14
	DBCommFunc open_func;            // _18
	DBCommFunc close_func;           // _1C
	DBCommFunc pre_continue_func;    // _20
	DBCommFunc post_stop_func;       // _24
} TRKCommTable;

void TRKLoadContext(OSContext* ctx, u32 a);
void TRKInterruptHandler();
void* TRK_memcpy(void* dst, const void* src, size_t n);
void PPCHalt();
void InitializeProgramEndTrap(void);
DSError TRKInitializeIntDrivenUART(u32, u32, u32, volatile u8**);
int TRKPollUART(void);
UARTError TRKReadUARTN(void*, u32);
UARTError TRKWriteUARTN(const void*, u32);
void UnreserveEXI2Port(void);
void ReserveEXI2Port(void);
void TRK_board_display(char*);
void TRKEXICallBack(__OSInterrupt interrupt, OSContext* ctx);

int udp_cc_initialize(void* flagOut, __OSInterruptHandler handler);
int udp_cc_shutdown();
int udp_cc_open();
int udp_cc_close();
int udp_cc_read(u8* dest, int size);
int udp_cc_write(const u8* src, int size);
int udp_cc_pre_continue();
int udp_cc_post_stop();
int udp_cc_peek();
int udp_cc_initinterrupts();
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
int ddh_cc_initialize(void* flagOut, __OSInterruptHandler handler);
int ddh_cc_shutdown();
int ddh_cc_open();
int ddh_cc_close();
int ddh_cc_read(u8* dest, int size);
int ddh_cc_write(const u8* src, int size);
int ddh_cc_pre_continue();
int ddh_cc_post_stop();
int ddh_cc_peek();
int ddh_cc_initinterrupts();

u8 TRK_Use_BBA;
s32 _MetroTRK_Has_Framing;

TRKCommTable gDBCommTable = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

/*
 * --INFO--
 * Address:	80220460
 * Size:	000088
 */
asm void TRKLoadContext(OSContext* ctx, u32 a)
{
#ifdef __MWERKS__ // clang-format off
    nofralloc
    lwz r0, OSContext.gpr[0](r3)
    lwz r1, OSContext.gpr[1](r3)
    lwz r2, OSContext.gpr[2](r3)
    lhz r5, OSContext.state(r3)
    rlwinm. r6, r5, 0, 0x1e, 0x1e
    beq lbl_80371C1C
    rlwinm r5, r5, 0, 0x1f, 0x1d
    sth r5, OSContext.state(r3)
    lmw r5, OSContext.gpr[5](r3)
    b lbl_80371C20
lbl_80371C1C:
    lmw r13, OSContext.gpr[13](r3)
lbl_80371C20:
    mr r31, r3
    mr r3, r4
    lwz r4, OSContext.cr(r31)
    mtcrf 0xff, r4
    lwz r4, OSContext.lr(r31)
    mtlr r4
    lwz r4, OSContext.ctr(r31)
    mtctr r4
    lwz r4, OSContext.xer(r31)
    mtxer r4
    mfmsr r4
    rlwinm r4, r4, 0, 0x11, 0xf //Turn off external exceptions
    rlwinm r4, r4, 0, 0x1f, 0x1d //Turn off recoverable exception flag
    mtmsr r4
    mtsprg 1, r2
    lwz r4, OSContext.gpr[3](r31)
    mtsprg 2, r4
    lwz r4, OSContext.gpr[4](r31)
    mtsprg 3, r4
    lwz r2, OSContext.srr0(r31)
    lwz r4, OSContext.srr1(r31)
    lwz r31, OSContext.gpr[31](r31)
    b TRKInterruptHandler
#endif // clang-format on
}

/*
 * --INFO--
 * Address:	802204E8
 * Size:	000038
 */
void TRKEXICallBack(__OSInterrupt param_0, OSContext* ctx)
{
	OSEnableScheduler();
	TRKLoadContext(ctx, 0x500);
}

/*
 * --INFO--
 * Address:	80220520
 * Size:	0000E8
 */
int InitMetroTRKCommTable(int hwId)
{
	int result = 1;

	OSReport("Devkit set to : %ld\n", hwId);

	TRK_Use_BBA = 0;

	if (hwId == HARDWARE_BBA) {
		OSReport("MetroTRK : Set to BBA\n");

		TRK_Use_BBA = 1;

		gDBCommTable.initialize_func      = (DBCommInitFunc)udp_cc_initialize;
		gDBCommTable.open_func            = (DBCommFunc)udp_cc_open;
		gDBCommTable.close_func           = (DBCommFunc)udp_cc_close;
		gDBCommTable.read_func            = (DBCommReadFunc)udp_cc_read;
		gDBCommTable.write_func           = (DBCommWriteFunc)udp_cc_write;
		gDBCommTable.shutdown_func        = (DBCommFunc)udp_cc_shutdown;
		gDBCommTable.peek_func            = (DBCommFunc)udp_cc_peek;
		gDBCommTable.pre_continue_func    = (DBCommFunc)udp_cc_pre_continue;
		gDBCommTable.post_stop_func       = (DBCommFunc)udp_cc_post_stop;
		gDBCommTable.init_interrupts_func = NULL;

		return 0;
	} else if (hwId == HARDWARE_GDEV) {
		OSReport("MetroTRK : Set to GDEV hardware\n");
		result = Hu_IsStub();

		gDBCommTable.initialize_func      = (DBCommInitFunc)gdev_cc_initialize;
		gDBCommTable.open_func            = (DBCommFunc)gdev_cc_open;
		gDBCommTable.close_func           = (DBCommFunc)gdev_cc_close;
		gDBCommTable.read_func            = (DBCommReadFunc)gdev_cc_read;
		gDBCommTable.write_func           = (DBCommWriteFunc)gdev_cc_write;
		gDBCommTable.shutdown_func        = (DBCommFunc)gdev_cc_shutdown;
		gDBCommTable.peek_func            = (DBCommFunc)gdev_cc_peek;
		gDBCommTable.pre_continue_func    = (DBCommFunc)gdev_cc_pre_continue;
		gDBCommTable.post_stop_func       = (DBCommFunc)gdev_cc_post_stop;
		gDBCommTable.init_interrupts_func = (DBCommFunc)gdev_cc_initinterrupts;
	} else if (hwId == HARDWARE_AMC_DDH) {
		OSReport("MetroTRK : Set to AMC DDH hardware\n");
		result = AMC_IsStub();

		gDBCommTable.initialize_func      = (DBCommInitFunc)ddh_cc_initialize;
		gDBCommTable.open_func            = (DBCommFunc)ddh_cc_open;
		gDBCommTable.close_func           = (DBCommFunc)ddh_cc_close;
		gDBCommTable.read_func            = (DBCommReadFunc)ddh_cc_read;
		gDBCommTable.write_func           = (DBCommWriteFunc)ddh_cc_write;
		gDBCommTable.shutdown_func        = (DBCommFunc)ddh_cc_shutdown;
		gDBCommTable.peek_func            = (DBCommFunc)ddh_cc_peek;
		gDBCommTable.pre_continue_func    = (DBCommFunc)ddh_cc_pre_continue;
		gDBCommTable.post_stop_func       = (DBCommFunc)ddh_cc_post_stop;
		gDBCommTable.init_interrupts_func = (DBCommFunc)ddh_cc_initinterrupts;
	} else {
		OSReport("MetroTRK : Set to UNKNOWN hardware. (%ld)\n", hwId);
		OSReport("MetroTRK : Invalid hardware ID passed from OS\n");
		OSReport("MetroTRK : Defaulting to GDEV Hardware\n");
	}

	return result;
}

/*
 * --INFO--
 * Address:	8022060C
 * Size:	000040
 */
DSError TRKInitializeIntDrivenUART(u32 param_0, u32 param_1, u32 param_2, volatile u8** param_3)
{
	gDBCommTable.initialize_func(param_3, TRKEXICallBack);
	gDBCommTable.open_func();
	return DS_NoError;
}

/*
 * --INFO--
 * Address:	8022064C
 * Size:	000030
 */
void EnableEXI2Interrupts(void)
{
	if (TRK_Use_BBA == 0 && gDBCommTable.init_interrupts_func != NULL) {
		gDBCommTable.init_interrupts_func();
	}
}

/*
 * --INFO--
 * Address:	8022067C
 * Size:	000030
 */
int TRKPollUART(void)
{
	return gDBCommTable.peek_func();
}

/*
 * --INFO--
 * Address:	802206AC
 * Size:	000044
 */
UARTError TRKReadUARTN(void* bytes, u32 length)
{
	int readErr = gDBCommTable.read_func(bytes, length);
	return readErr == 0 ? 0 : -1;
}

/*
 * --INFO--
 * Address:	802206F0
 * Size:	000044
 */
UARTError TRKWriteUARTN(const void* bytes, u32 length)
{
	int writeErr = gDBCommTable.write_func(bytes, length);
	return writeErr == 0 ? 0 : -1;
}

/*
 * --INFO--
 * Address:	80220734
 * Size:	000030
 */
void ReserveEXI2Port(void)
{
	gDBCommTable.post_stop_func();
}

/*
 * --INFO--
 * Address:	80220764
 * Size:	000030
 */
void UnreserveEXI2Port(void)
{
	gDBCommTable.pre_continue_func();
}

/*
 * --INFO--
 * Address:	80220794
 * Size:	000024
 */
void TRK_board_display(char* str)
{
	OSReport("%s\n", str);
}

/*
 * --INFO--
 * Address:	8022060C
 * Size:	000058
 */
void InitializeProgramEndTrap(void)
{
	static const u8 EndofProgramInstruction[4] = { 0x00, 'E', 'N', 'D' };
	u32* halt = (u32*)PPCHalt;

	TRK_memcpy(halt + 1, EndofProgramInstruction, 4);
	ICInvalidateRange(halt + 1, 4);
	DCFlushRange(halt + 1, 4);
}

/*
 * --INFO--
 * Address:	80220608
 * Size:	000004
 */
void TRKUARTInterruptHandler(void)
{
}
