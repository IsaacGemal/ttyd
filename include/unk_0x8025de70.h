#ifndef __UNK_0X8025DE70_H__
#define __UNK_0X8025DE70_H__

#include <dolphin/types.h>

// The rodata blob at 0x803019E0. The compiler emitted this function's string literals, the
// copy-clear colour support and the u16->float mask double into one object; it is declared here as
// the single object it is so a caller can index it by offset (the original reaches every member
// with `addi rN, r29, <offset>` from the base symbol).
typedef struct N_ErrorData {
	char dsi[24];       // 0x00 str_803019E0 "---- OS_ERROR_DSI ----\n"
	char isi[24];       // 0x18 str_803019F8 "---- OS_ERROR_ISI ----\n"
	char program[28];   // 0x30 str_80301A10 "---- OS_ERROR_PROGRAM ----\n"
	char context[28];   // 0x4C str_80301A2C "---- Context 0x%08x ----\n"
	char gpr[36];       // 0x68 str_80301A48 "r%-2d  = 0x%08x r%-2d  = 0x%08x\n"
	char lrCr[32];      // 0x8C str_80301A6C "LR   = 0x%08x CR   = 0x%08x\n"
	char srr[32];       // 0xAC str_80301A8C "SRR0 = 0x%08x SRR1 = 0x%08x\n"
	char dsisr[32];     // 0xCC str_80301AAC "DSISR= 0x%08x DAR  = 0x%08x\n"
	char address[40];   // 0xEC str_80301ACC "Address:      Back Chain    LR Save\n"
	char backchain[28]; // 0x114 str_80301AF4 "0x%08x:   0x%08x    0x%08x\n"
	char instr[96];     // 0x130 str_80301B10 "Instruction at 0x%x\n..."
	f64 magic;          // 0x190 double_to_int_mask_80301b70 (u16->float conversion)
} N_ErrorData;

extern N_ErrorData str_803019E0;

#endif
