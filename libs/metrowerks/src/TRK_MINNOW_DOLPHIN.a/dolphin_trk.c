#include "PowerPC_EABI_Support/MetroTRK/trk.h"
#include "dolphin/os/OSReset.h"

#define EXCEPTIONMASK_ADDR 0x80000044

// ARAM helpers live in the Dolphin AR driver, which is not on this library's include path.
extern void ARStartDMA(u32 type, u32 mainmem_addr, u32 aram_addr, u32 length);
extern u32 ARGetDMAStatus(void);
extern u32 __ARGetInterruptStatus(void);
extern void __ARClearInterrupt(void);

u32 TRKTargetTranslate(u32 param_0);

static u32 lc_base;
extern u32 _db_stack_addr;

static u32 TRK_ISR_OFFSETS[15] = { PPC_SystemReset,
	                               PPC_MachineCheck,
	                               PPC_DataStorage,
	                               PPC_InstructionStorage,
	                               PPC_ExternalInterrupt,
	                               PPC_Alignment,
	                               PPC_Program,
	                               PPC_FloatingPointUnavaiable,
	                               PPC_Decrementer,
	                               PPC_SystemCall,
	                               PPC_Trace,
	                               PPC_PerformanceMonitor,
	                               PPC_InstructionAddressBreakpoint,
	                               PPC_SystemManagementInterrupt,
	                               PPC_ThermalManagementInterrupt };

__declspec(section ".init") void __TRK_reset()
{
	OSResetSystem(OS_RESET_RESTART, 0, FALSE);
}

asm void InitMetroTRK()
{
#ifdef __MWERKS__ // clang-format off
	nofralloc

	addi r1, r1, -4
	stw r3, 0(r1)
	lis r3, gTRKCPUState@h
	ori r3, r3, gTRKCPUState@l
	stmw r0, ProcessorState_PPC.Default.GPR(r3) //Save the gprs
	lwz r4, 0(r1)
	addi r1, r1, 4
	stw r1, ProcessorState_PPC.Default.GPR[1](r3)
	stw r4, ProcessorState_PPC.Default.GPR[3](r3)
	mflr r4
	stw r4, ProcessorState_PPC.Default.LR(r3)
	stw r4, ProcessorState_PPC.Default.PC(r3)
	mfcr r4
	stw r4, ProcessorState_PPC.Default.CR(r3)
	//???
	mfmsr r4
	ori r3, r4, (1 << (31 - 16))
	xori r3, r3, (1 << (31 - 16))
	mtmsr r3
	mtsrr1 r4 //Copy msr to srr1
	//Save misc registers to gTRKCPUState
	bl TRKSaveExtended1Block
	lis r3, gTRKCPUState@h
	ori r3, r3, gTRKCPUState@l
	lmw r0, ProcessorState_PPC.Default.GPR(r3) //Restore the gprs
	//Reset IABR and DABR
	li r0, 0
	mtspr  0x3f2, r0
	mtspr  0x3f5, r0
	//Restore stack pointer
	lis r1, _db_stack_addr@h
	ori r1, r1, _db_stack_addr@l
	mr r3, r5
	bl InitMetroTRKCommTable //Initialize comm table
	/*
	If InitMetroTRKCommTable returned 1 (failure), an invalid hardware
	id or the id for GDEV was somehow passed. Since only BBA or NDEV
	are supported, we return early. Otherwise, we proceed with
	starting up TRK.
	*/
	cmpwi r3, 1
	bne initCommTableSuccess
	/*
	BUG: The code probably orginally reloaded gTRKCPUState here, but
	as is it will read the returned value of InitMetroTRKCommTable
	as a TRKCPUState struct pointer, causing the CPU to return to
	a garbage code address.
	*/
	lwz r4, ProcessorState_PPC.Default.LR(r3)
	mtlr r4
	lmw r0, ProcessorState_PPC.Default.GPR(r3) //Restore the gprs
	blr
initCommTableSuccess:
	b TRK_main //Jump to TRK_main
	blr
#endif // clang-format on
}

asm void InitMetroTRK_BBA()
{
#ifdef __MWERKS__ // clang-format off
	nofralloc

	addi r1, r1, -4
	stw r3, 0(r1)
	lis r3, gTRKCPUState@h
	ori r3, r3, gTRKCPUState@l
	stmw r0, ProcessorState_PPC.Default.GPR(r3) //Save the gprs
	lwz r4, 0(r1)
	addi r1, r1, 4
	stw r1, ProcessorState_PPC.Default.GPR[1](r3)
	stw r4, ProcessorState_PPC.Default.GPR[3](r3)
	mflr r4
	stw r4, ProcessorState_PPC.Default.LR(r3)
	stw r4, ProcessorState_PPC.Default.PC(r3)
	mfcr r4
	stw r4, ProcessorState_PPC.Default.CR(r3)
	mfmsr r4
	ori r3, r4, (1 << (31 - 16))
	mtmsr r3
	mtsrr1 r4 //Copy msr to srr1
	//Save misc registers to gTRKCPUState
	bl TRKSaveExtended1Block
	lis r3, gTRKCPUState@h
	ori r3, r3, gTRKCPUState@l
	lmw r0, ProcessorState_PPC.Default.GPR(r3) //Restore the gprs
	//Reset IABR and DABR
	li r0, 0
	mtspr  0x3f2, r0
	mtspr  0x3f5, r0
	//Restore stack pointer
	lis r1, _db_stack_addr@h
	ori r1, r1, _db_stack_addr@l
	li r3, 2
	bl InitMetroTRKCommTable //Initialize comm table
	cmpwi r3, 1
	bne bbaCommTableSuccess
	lwz r4, ProcessorState_PPC.Default.LR(r3)
	mtlr r4
	lmw r0, ProcessorState_PPC.Default.GPR(r3) //Restore the gprs
	blr
bbaCommTableSuccess:
	b TRK_main //Jump to TRK_main
	blr
#endif // clang-format on
}

// NONMATCHING (87.28%): the compiler has no `__dcbi` intrinsic. The three data-cache invalidates compile as
// `dcbf` instead of `dcbi` (same operand form, different opcode); the remaining diff is register allocation
// in the cache loop. Everything else follows the original.
void TRK__write_aram(u8* buf, u32 aram, u32* size)
{
	u32 aramOff;
	u32 alignedAram;
	u32 len;
	u32 aramEnd;
	u32 i;
	u32 lastAram;
	u32 intr;
	u8 stack[32] __attribute__((aligned(32)));

	if (aram < 0x4000) {
		return;
	}
	if (aram + *size > 0x08000000U) {
		return;
	}

	aramOff = aram & 31;
	alignedAram = aram & ~31;
	len = *size + aramOff;
	len = (len + 31) & ~31;

	for (i = 0; i < len; i += 32) {
		__dcbf(buf, i);
	}

	while (ARGetDMAStatus() != 0) {
	}

	intr = __ARGetInterruptStatus();
	lastAram = 0x08000000;

	if (aramOff != 0) {
		lastAram = alignedAram;
		__dcbf(stack, 0);
		__ARClearInterrupt();
		ARStartDMA(1, (u32)stack, alignedAram, 32);
		while ((__ARGetInterruptStatus() & 0xffff) == 0) {
		}
		TRK_memcpy(buf, stack, aramOff);
		__dcbf(buf, 0);
	}

	aramEnd = aram + *size;
	if ((aramEnd & 31) != 0) {
		u32 tailAram = aramEnd & ~31;
		if (tailAram != lastAram) {
			__dcbf(stack, 0);
			__ARClearInterrupt();
			ARStartDMA(1, (u32)stack, tailAram, 32);
			while ((__ARGetInterruptStatus() & 0xffff) == 0) {
			}
		}
		TRK_memcpy(buf + aramEnd, stack + (aramEnd & 31), 32 - (aramEnd & 31));
		__dcbf(buf + aramEnd, 0);
	}

	__sync();
	__ARClearInterrupt();
	ARStartDMA(0, (u32)buf, alignedAram, len);
	if ((intr & 0xffff) == 0) {
		while ((__ARGetInterruptStatus() & 0xffff) == 0) {
		}
		__ARClearInterrupt();
	}
}

// NONMATCHING (87.34%): the compiler has no `__dcbi` intrinsic. The data-cache invalidates compile as `dcbf`
// instead of `dcbi`; the remaining diff is register allocation in the invalidate loop.
void TRK__read_aram(u8* buf, u32 aram, u32* size)
{
	u32 aramOff;
	u32 alignedAram;
	u32 len;
	u32 i;
	u32 intr;

	if (aram < 0x4000) {
		return;
	}
	if (aram + *size > 0x08000000U) {
		return;
	}

	aramOff = aram & 31;
	alignedAram = aram & ~31;
	len = *size + aramOff;
	len = (len + 31) & ~31;

	for (i = 0; i < len; i += 32) {
		__dcbf(buf, i);
	}

	while (ARGetDMAStatus() != 0) {
	}

	intr = __ARGetInterruptStatus();
	__ARClearInterrupt();
	ARStartDMA(1, (u32)buf, alignedAram, len);
	while ((__ARGetInterruptStatus() & 0xffff) == 0) {
	}
	if ((intr & 0xffff) == 0) {
		__ARClearInterrupt();
	}
}

/*
 * --INFO--
 * Address:	80220050
 * Size:	000050
 */
DSError TRKInitializeTarget()
{
	gTRKState.isStopped = TRUE;
	gTRKState.msr       = __TRK_get_MSR();
	lc_base             = 0xE0000000;
	return DS_NoError;
}

/*
 * --INFO--
 * Address:	8021FF5C
 * Size:	000060
 */
void TRK_copy_vector(u32 offset)
{
	void* destPtr = (void*)TRKTargetTranslate(offset);
	TRK_memcpy(destPtr, gTRKInterruptVectorTable + offset, 0x100);
	TRK_flush_cache(destPtr, 0x100);
}

/*
 * --INFO--
 * Address:	8021FFBC
 * Size:	000094
 */
void __TRK_copy_vectors(void)
{
	int i;
	u32 mask;

	mask = *(u32*)TRKTargetTranslate(0x44);

	for (i = 0; i <= 14; ++i) {
		if ((mask & (1 << i)) && i != 4) {
			TRK_copy_vector(TRK_ISR_OFFSETS[i]);
		}
	}
}

/*
 * --INFO--
 * Address:	8021FF14
 * Size:	000048
 */
u32 TRKTargetTranslate(u32 param_0)
{
	if (param_0 >= lc_base) {
		if ((param_0 < lc_base + 0x4000) && ((gTRKCPUState.Extended1.DBAT3U_ & 3) != 0)) {
			return param_0;
		}
	}

	if (param_0 >= 0x7E000000U && param_0 <= 0x80000000U) {
		return param_0;
	}

	return param_0 & 0x3FFFFFFF | 0x80000000;
}

/*
 * --INFO--
 * Address:	8021FEF4
 * Size:	000020
 */
void EnableMetroTRKInterrupts(void)
{
	EnableEXI2Interrupts();
}
