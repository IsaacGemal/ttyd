#include "driver/dispdrv.h"
#include "memory.h"
#include "system.h"
#include "seq/seq_title.h"
#include "pmario_sound.h"
#include <demo/DEMOInit.h>
#include <dolphin/os.h>
#include <dolphin/base/PPCArch.h>
#include <dolphin/vi.h>
#include <string.h>
#include <stdio.h>
#include "eff/eff_ultra_hammer.h"
#pragma force_active on


extern const char str_803019E0[24];
extern const char str_803019F8[24];
extern const char str_80301A10[28];
extern const char str_80301A2C[28];
extern const char str_80301A48[36];
extern const char str_80301A6C[32];
extern const char str_80301A8C[32];
extern const char str_80301AAC[32];
extern const char str_80301ACC[40];
extern const char str_80301AF4[28];
extern const char str_80301B10[96];

const char str_803019E0[24] = "---- OS_ERROR_DSI ----\n";

const char str_803019F8[24] = "---- OS_ERROR_ISI ----\n";

const char str_80301A10[28] = "---- OS_ERROR_PROGRAM ----\n";

const char str_80301A2C[28] = "---- Context 0x%08x ----\n\000\000";

const char str_80301A48[36] = "r%-2d  = 0x%08x r%-2d  = 0x%08x\n\000\000\000";

const char str_80301A6C[32] = "LR   = 0x%08x CR   = 0x%08x\n\000\000\000";

const char str_80301A8C[32] = "SRR0 = 0x%08x SRR1 = 0x%08x\n\000\000\000";

const char str_80301AAC[32] = "DSISR= 0x%08x DAR  = 0x%08x\n\000\000\000";

const char str_80301ACC[40] = "Address:      Back Chain    LR Save\n\000\000\000";

const char str_80301AF4[28] = "0x%08x:   0x%08x    0x%08x\n";

const char str_80301B10[96] = "Instruction at 0x%x\n(read from SRR0) attempted to access\ninvalid address 0x%x (read from DAR)\n\000";

#pragma force_active reset

void N_setupErrorHandler(void) {
	OSSetErrorHandler(OS_ERROR_DSI, (OSErrorHandler)N_systemErrorHandler);
	OSSetErrorHandler(OS_ERROR_ISI, (OSErrorHandler)N_systemErrorHandler);
	OSSetErrorHandler(OS_ERROR_PROGRAM, (OSErrorHandler)N_systemErrorHandler);
}

// 100%: the text loop indexes print_buffer by i; the two byte pointers the original walks are the compiler's
// strength-reduced induction variables, and &print_buffer[i] is its hoisted loop invariant.
void N_systemErrorHandler(OSError error, OSContext* context, u32 dsisr, u32 dar) {
	u32 count;
	int i;
	OSFontHeader* fontHeader;
	s32 width2, height2;
	u32 stack;
	u16 w1, w2, h1, h2, x1, x2, y1, y2;
	//stack vars
	char print_buffer2[0x400]; // 0x10E0
	char print_buffer[0x1000]; // 0xE0
	Mtx44 projection; // 0xA0
	Mtx matrix; // 0x70
	Mtx matrix2; // 0x40
	GXTexObj texObj; // 0x20
	void* image; // 0x1C
	s32 x; // 0x18
	s32 y; // 0x14
	s32 width; // 0x10

	if (N_getDebugMode() >= 0) {
		psndExit();
		while (1) {} //softlock
	}

	switch (error) {
		case OS_ERROR_DSI:
			strcpy(print_buffer, str_803019E0);
			break;
		case OS_ERROR_ISI:
			strcpy(print_buffer, str_803019F8);
			break;
		case OS_ERROR_PROGRAM:
			strcpy(print_buffer, str_80301A10);
			break;
	}
	sprintf(print_buffer2, str_80301A2C, context);
	strcat(print_buffer, print_buffer2);
	for (i = 0; i < 16; i++) {
		sprintf(print_buffer2, str_80301A48,
			i, context->gpr[i],
			i + 16, context->gpr[i + 16]
		);
		strcat(print_buffer, print_buffer2);
	}
	sprintf(print_buffer2, str_80301A6C, context->lr, context->cr);
	strcat(print_buffer, print_buffer2);
	sprintf(print_buffer2, str_80301A8C, context->srr0, context->srr1);
	strcat(print_buffer, print_buffer2);
	sprintf(print_buffer2, str_80301AAC, dsisr, dar);
	strcat(print_buffer, print_buffer2);
	strcat(print_buffer, "\n");
	sprintf(print_buffer2, str_80301ACC);
	strcat(print_buffer, print_buffer2);
	stack = context->gpr[1]; //get r1 (stack)
	i = 0;
	while ((stack != 0) && (stack != 0xFFFFFFFF) && (i++ < 16)) {
		sprintf(print_buffer2, str_80301AF4, stack, *(u32*)stack, *(u32*)(stack + 4));
		strcat(print_buffer, print_buffer2);
		stack = *(u32*)stack;
	}
	strcat(print_buffer, "\n");
	sprintf(print_buffer2, str_80301B10, context->srr0, dar);
	strcat(print_buffer, print_buffer2);
	psndExit();
	OSFillFPUContext(context);
	// The original re-enables FP and recoverable interrupts here with an inline asm block (the same one
	// src/error_handler.c's systemErrorHandler has); MWCC 2.6 has no mfmsr/mtmsr intrinsic, so the matching
	// build keeps the asm (tools/asm_exceptions.json, original-asm) and the natural build the equivalent call.
#ifdef MUST_MATCH
	asm {
		mfmsr	r5
		ori		r5, r5, 0x8000
		ori		r5, r5, 2
		mtmsr	r5
		isync
	};
#else
	PPCMtmsr(PPCMfmsr() | 0x8002);
#endif
	L_smartReInit();
	if (OSGetFontEncode() == OS_FONT_ENCODE_SJIS) {
		fontHeader = smartAlloc(OS_FONT_SIZE_SJIS, 0)->address;
	}
	else {
		fontHeader = smartAlloc(OS_FONT_SIZE_ANSI, 0)->address;
	}
	OSInitFont(fontHeader);
	GXSetCopyClear((GXColor){0, 0, 0, 0xFF}, 0xFFFFFF);
	GXCopyDisp((void*)DEMOGetCurrentBuffer(), 1);
	GXSetViewport(0.0f, 0.0f, 608.0f, 480.0f, 0.0f, 1.0f);
	GXSetScissor(0, 0, 608, 480);
	MTXOrtho(projection, 0.0f, 608.0f, 0.0f, 480.0f, 0.0f, 1.0f);
	GXSetProjection(projection, GX_ORTHOGRAPHIC);
	MTXScale(matrix, 0.5f, 0.5f, 1.0f);
	GXLoadPosMtxImm(matrix, 0);
	GXSetCurrentMtx(0);
	GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GXSetNumChans(0);
	GXSetNumTevStages(1);
	GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetNumTexGens(1);
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_LO_CLEAR);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, 1, 3, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, 1, 3, 0);
	while (1) {
		count = VIGetRetraceCount();
		width2 = 50;
		height2 = 50;
		i = 0;
		while ((s8)print_buffer[i] != '\0') {
			if ((s8)print_buffer[i] == '\n') {
				width2 = 50;
				height2 += 24;
				i++;
			}
			else {
				OSGetFontTexture(&print_buffer[i], &image, &x, &y, &width);
				GXInitTexObj(&texObj, image, fontHeader->sheetWidth, fontHeader->sheetHeight,
					(GXTexFmt)fontHeader->sheetFormat, GX_CLAMP, GX_CLAMP, GX_FALSE);
				GXLoadTexObj(&texObj, GX_TEXMAP0);
				MTXScale(matrix2, 1.0f / fontHeader->sheetWidth, 1.0f / fontHeader->sheetHeight, 1.0f);
				GXLoadTexMtxImm(matrix2, 0x1E, GX_MTX2x4);
				GXBegin(GX_QUADS, GX_VTXFMT0, 4);
				w1 = width2;
				w2 = (s16)width2 + fontHeader->cellWidth;
				h1 = height2;
				h2 = (s16)height2 + fontHeader->cellHeight;
				x1 = x;
				x2 = x + fontHeader->cellWidth;
				y1 = y;
				y2 = y + fontHeader->cellHeight;

				GXPosition1x16(w1);
				GXPosition1x16(h1);
				GXPosition1x16(0);
				GXPosition1x16(x1);
				GXPosition1x16(y1);

				GXPosition1x16(w2);
				GXPosition1x16(h1);
				GXPosition1x16(0);
				GXPosition1x16(x2);
				GXPosition1x16(y1);

				GXPosition1x16(w2);
				GXPosition1x16(h2);
				GXPosition1x16(0);
				GXPosition1x16(x2);
				GXPosition1x16(y2);

				GXPosition1x16(w1);
				GXPosition1x16(h2);
				GXPosition1x16(0);
				GXPosition1x16(x1);
				GXPosition1x16(y2);

				i++;
				width2 += width;
			}
		}
		GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
		GXSetColorUpdate(1);
		GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
		GXSetAlphaUpdate(1);
		GXCopyDisp(DemoCurrentBuffer, GX_TRUE);
		VISetNextFrameBuffer(DemoCurrentBuffer);
		VISetBlack(0);
		VIFlush();
		do { ; } while (count == VIGetRetraceCount());
		if (DemoCurrentBuffer == DemoFrameBuffer1)
			DemoCurrentBuffer = DemoFrameBuffer2;
		else
			DemoCurrentBuffer = DemoFrameBuffer1;
	}
}
