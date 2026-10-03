#include "eff/eff_ultra_hammer.h"
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

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u8 lbl_803B70C0[8] = { 0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00 };

// These error strings and the copy-clear colour live in unk_0x8025de70.c (same data blob).
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
extern const u32 dat_80422600;
extern const char str_80422604[4];
extern const f32 float_0_80422608;
extern const f32 float_608_8042260c;
extern const f32 float_480_80422610;
extern const f32 float_1_80422614;
extern const f32 float_0p5_80422618;

void N_setupErrorHandler(void) {
	OSSetErrorHandler(OS_ERROR_DSI, (OSErrorHandler)N_systemErrorHandler);
	OSSetErrorHandler(OS_ERROR_ISI, (OSErrorHandler)N_systemErrorHandler);
	OSSetErrorHandler(OS_ERROR_PROGRAM, (OSErrorHandler)N_systemErrorHandler);
}

// NONMATCHING (93.72%): the remaining diffs are register allocation plus two things the original did while
// the error strings and this function shared one translation unit. (1) The interrupt restore after
// OSFillFPUContext: the original is inline mfmsr/ori 0x8000/ori 2/mtmsr/isync (inline asm is not allowed here),
// written as PPCMtmsr(PPCMfmsr() | 0x8002), which calls the two SDK functions. (2) The font MTXScale
// conversion: the original is unsigned and loads its 2^52 mask at str_803019E0+0x190
// (double_to_int_mask_80301b70 in unk_0x8025de70.o's .rodata); under MUST_MATCH it converts as signed and
// shares this unit's .rodata double_to_int_803019d8, because an unsigned conversion would emit an extra
// 8-byte .rodata object here and shift the unit's data (matched_data drops 80 -> 32). The error strings are
// read as offsets from str_803019E0, the single .rodata base register the original compiler used (its `addi
// rN, r29, off`); the offsets are the original's. The original also keeps context/error/dsisr/dar in
// r26/r23/r25/r24, the string base in r29 and an indexed gpr loop (offset in r23), while this compiler picks
// different callee-saved registers and a running pointer, so it saves two fewer registers and the frame is
// 0x10 smaller.
// NONMATCHING (93.72%): register allocation, the inline-msr restore and the unsigned font-conversion mask.
void N_systemErrorHandler(OSError error, OSContext* context, u32 dsisr, u32 dar) {
	//registers
	OSFontHeader* fontHeader;
	s8* loop_newline;
	s8* loop_zero;
	s32 width2, height2;
	int i;
	u32 count, stack;
	const char* strings;
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
#ifdef MUST_MATCH
	u32 clearColor;
#endif

	strings = (const char*)str_803019E0;
	if (N_getDebugMode() >= 0) {
		psndExit();
		while (1) {} //softlock
	}

	switch (error) {
		case OS_ERROR_DSI:
			strcpy(print_buffer, strings + 0x0);
			break;
		case OS_ERROR_ISI:
			strcpy(print_buffer, strings + 0x18);
			break;
		case OS_ERROR_PROGRAM:
			strcpy(print_buffer, strings + 0x30);
			break;
	}
	sprintf(print_buffer2, strings + 0x4c, context);
	strcat(print_buffer, print_buffer2);
	for (i = 0; i < 16; i++) {
		sprintf(print_buffer2, strings + 0x68,
			i, context->gpr[i],
			i + 16, context->gpr[i + 16]
		);
		strcat(print_buffer, print_buffer2);
	}
	sprintf(print_buffer2, strings + 0x8c, context->lr, context->cr);
	strcat(print_buffer, print_buffer2);
	sprintf(print_buffer2, strings + 0xac, context->srr0, context->srr1);
	strcat(print_buffer, print_buffer2);
	sprintf(print_buffer2, strings + 0xcc, dsisr, dar);
	strcat(print_buffer, print_buffer2);
	strcat(print_buffer, str_80422604);
	sprintf(print_buffer2, strings + 0xec);
	strcat(print_buffer, print_buffer2);
	stack = context->gpr[1]; //get r1 (stack)
	i = 0;
	while ((stack != 0) && (stack != 0xFFFFFFFF) && (i++ < 16)) {
		sprintf(print_buffer2, strings + 0x114, stack, *(u32*)stack, *(u32*)(stack + 4));
		strcat(print_buffer, print_buffer2);
		stack = *(u32*)stack;
	}
	strcat(print_buffer, str_80422604);
	sprintf(print_buffer2, strings + 0x130, context->srr0, dar);
	strcat(print_buffer, print_buffer2);
	psndExit();
	OSFillFPUContext(context);
	PPCMtmsr(PPCMfmsr() | 0x8002);
	L_smartReInit();
	if (OSGetFontEncode() == OS_FONT_ENCODE_SJIS) {
		fontHeader = smartAlloc(OS_FONT_SIZE_SJIS, 0)->address;
	}
	else {
		fontHeader = smartAlloc(OS_FONT_SIZE_ANSI, 0)->address;
	}
	OSInitFont(fontHeader);
#ifdef MUST_MATCH
	// The original loads the clear colour from the shared constant pool and copies it through a
	// u32 local (giving the two stores before the call); the plain compound literal folds away.
	clearColor = dat_80422600;
	GXSetCopyClear(*(GXColor*)&clearColor, 0xFFFFFF);
#else
	GXSetCopyClear((GXColor){0, 0, 0, 0xFF}, 0xFFFFFF);
#endif
	GXCopyDisp((void*)DEMOGetCurrentBuffer(), 1);
	GXSetViewport(float_0_80422608, float_0_80422608, float_608_8042260c, float_480_80422610, float_0_80422608, float_1_80422614);
	GXSetScissor(0, 0, 608, 480);
	MTXOrtho(projection, float_0_80422608, float_608_8042260c, float_0_80422608, float_480_80422610, float_0_80422608, float_1_80422614);
	GXSetProjection(projection, GX_ORTHOGRAPHIC);
	MTXScale(matrix, float_0p5_80422618, float_0p5_80422618, float_1_80422614);
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
		loop_newline = (s8*)&print_buffer;
		width2 = 50;
		height2 = 50;
		i = 0;
		loop_zero = loop_newline;
		while (*loop_zero != '\0') {
			if (*loop_newline == '\n') {
				width2 = 50;
				height2 += 24;
				i++;
				loop_zero++;
				loop_newline++;
			}
			else {
				OSGetFontTexture((const char*)&print_buffer[i], &image, &x, &y, &width);
				GXInitTexObj(&texObj, image, fontHeader->sheetWidth, fontHeader->sheetHeight,
					(GXTexFmt)fontHeader->sheetFormat, GX_CLAMP, GX_CLAMP, GX_FALSE);
				GXLoadTexObj(&texObj, GX_TEXMAP0);
#ifdef MUST_MATCH
				MTXScale(matrix2, float_1_80422614 / (s32)fontHeader->sheetWidth, float_1_80422614 / (s32)fontHeader->sheetHeight, float_1_80422614);
#else
				MTXScale(matrix2, float_1_80422614 / fontHeader->sheetWidth, float_1_80422614 / fontHeader->sheetHeight, float_1_80422614);
#endif
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
				loop_zero++;
				loop_newline++;
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

#ifdef __MWERKS__
u8 wakka_dl_0_0[] ATTRIBUTE_ALIGN(32) = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	1, 0, 0, 2, // 0
	0, 0, 0, 3, // 1
	21, 0, 0, 1, // 2
	20, 0, 0, 0, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_1[] ATTRIBUTE_ALIGN(32) = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	2, 0, 0, 5, // 0
	1, 0, 0, 2, // 1
	22, 0, 0, 4, // 2
	21, 0, 0, 1, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_2[] ATTRIBUTE_ALIGN(32) = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	3, 0, 0, 7, // 0
	2, 0, 0, 5, // 1
	23, 0, 0, 6, // 2
	22, 0, 0, 4, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_3[] ATTRIBUTE_ALIGN(32) = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	39, 0, 0, 38, // 0
	20, 0, 0, 0, // 1
	19, 0, 0, 39, // 2
	0, 0, 0, 3, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_4[] ATTRIBUTE_ALIGN(32) = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	38, 0, 0, 36, // 0
	39, 0, 0, 38, // 1
	18, 0, 0, 37, // 2
	19, 0, 0, 39, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_5[] ATTRIBUTE_ALIGN(32) = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	37, 0, 0, 34, // 0
	38, 0, 0, 36, // 1
	17, 0, 0, 35, // 2
	18, 0, 0, 37, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_6[] ATTRIBUTE_ALIGN(32) = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	36, 0, 0, 32, // 0
	37, 0, 0, 34, // 1
	16, 0, 0, 33, // 2
	17, 0, 0, 35, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_7[] ATTRIBUTE_ALIGN(32) = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	35, 0, 0, 30, // 0
	36, 0, 0, 32, // 1
	15, 0, 0, 31, // 2
	16, 0, 0, 33, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_8[] ATTRIBUTE_ALIGN(32) = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	34, 0, 0, 28, // 0
	35, 0, 0, 30, // 1
	14, 0, 0, 29, // 2
	15, 0, 0, 31, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_9[] ATTRIBUTE_ALIGN(32) = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	33, 0, 0, 26, // 0
	34, 0, 0, 28, // 1
	13, 0, 0, 27, // 2
	14, 0, 0, 29, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_10[] ATTRIBUTE_ALIGN(32) = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	32, 0, 0, 24, // 0
	33, 0, 0, 26, // 1
	12, 0, 0, 25, // 2
	13, 0, 0, 27, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_11[] ATTRIBUTE_ALIGN(32) = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	31, 0, 0, 22, // 0
	32, 0, 0, 24, // 1
	11, 0, 0, 23, // 2
	12, 0, 0, 25, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_12[] ATTRIBUTE_ALIGN(32) = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	30, 0, 0, 20, // 0
	31, 0, 0, 22, // 1
	10, 0, 0, 21, // 2
	11, 0, 0, 23, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_13[] ATTRIBUTE_ALIGN(32) = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	29, 0, 0, 18, // 0
	30, 0, 0, 20, // 1
	9, 0, 0, 19, // 2
	10, 0, 0, 21, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_14[] ATTRIBUTE_ALIGN(32) = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	28, 0, 0, 16, // 0
	29, 0, 0, 18, // 1
	8, 0, 0, 17, // 2
	9, 0, 0, 19, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_15[] ATTRIBUTE_ALIGN(32) = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	27, 0, 0, 14, // 0
	28, 0, 0, 16, // 1
	7, 0, 0, 15, // 2
	8, 0, 0, 17, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_16[] ATTRIBUTE_ALIGN(32) = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	26, 0, 0, 12, // 0
	27, 0, 0, 14, // 1
	6, 0, 0, 13, // 2
	7, 0, 0, 15, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_17[] ATTRIBUTE_ALIGN(32) = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	25, 0, 0, 9, // 0
	26, 0, 0, 12, // 1
	5, 0, 0, 10, // 2
	6, 0, 0, 13, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_18[] ATTRIBUTE_ALIGN(32) = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	24, 0, 0, 8, // 0
	25, 0, 0, 9, // 1
	4, 0, 0, 11, // 2
	5, 0, 0, 10, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_19[] ATTRIBUTE_ALIGN(32) = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	41, 0, 1, 41, // 0
	3, 0, 0, 7, // 1
	40, 0, 1, 40, // 2
	23, 0, 0, 6, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_size_tbl[] = {
	1, // 0
	1, // 1
	1, // 2
	1, // 3
	1, // 4
	1, // 5
	1, // 6
	1, // 7
	1, // 8
	1, // 9
	1, // 10
	1, // 11
	1, // 12
	1, // 13
	1, // 14
	1, // 15
	1, // 16
	1, // 17
	1, // 18
	1, // 19
};

s16 wakka_vertex_tbl[] ATTRIBUTE_ALIGN(32) = {
	24347, 0, -8825, // 0
	20710, 0, -15961, // 1
	15047, 0, -21625, // 2
	7910, 0, -25261, // 3
	0, 0, -26514, // 4
	-7910, 0, -25261, // 5
	-15047, 0, -21625, // 6
	-20710, 0, -15961, // 7
	-24347, 0, -8825, // 8
	-25600, 0, -914, // 9
	-24347, 0, 6996, // 10
	-20710, 0, 14133, // 11
	-15047, 0, 19796, // 12
	-7910, 0, 23432, // 13
	0, 0, 24685, // 14
	7910, 0, 23432, // 15
	15047, 0, 19796, // 16
	20710, 0, 14133, // 17
	24347, 0, 6996, // 18
	25600, 0, -914, // 19
	12173, 0, -4869, // 20
	10355, 0, -8437, // 21
	7523, 0, -11269, // 22
	3955, 0, -13087, // 23
	0, 0, -13714, // 24
	-3955, 0, -13087, // 25
	-7523, 0, -11269, // 26
	-10355, 0, -8437, // 27
	-12173, 0, -4869, // 28
	-12800, 0, -914, // 29
	-12173, 0, 3041, // 30
	-10355, 0, 6609, // 31
	-7523, 0, 9441, // 32
	-3955, 0, 11259, // 33
	0, 0, 11885, // 34
	3955, 0, 11259, // 35
	7523, 0, 9441, // 36
	10355, 0, 6609, // 37
	12173, 0, 3041, // 38
	12800, 0, -914, // 39
	0, 0, -13714, // 40
	0, 0, -26514, // 41
};

s8 wakka_normal_tbl[] ATTRIBUTE_ALIGN(32) = {
	0, 64, 0, // 0
};

u8 wakka_color0_tbl[] ATTRIBUTE_ALIGN(32) = {
	0xFF, 0xFF, 0xFF, 0x7F, // 0
	0xFF, 0xFF, 0xFF, 0x00  // 1
};

s16 wakka_texcoord0_tbl[] ATTRIBUTE_ALIGN(32) = {
	24576, 0, // 0
	25600, 0, // 1
	25600, 8192, // 2
	24576, 8192, // 3
	26624, 0, // 4
	26624, 8192, // 5
	27648, 0, // 6
	27648, 8192, // 7
	8192, 0, // 8
	9216, 0, // 9
	9216, 8192, // 10
	8192, 8192, // 11
	10240, 0, // 12
	10240, 8192, // 13
	11264, 0, // 14
	11264, 8192, // 15
	12288, 0, // 16
	12288, 8192, // 17
	13312, 0, // 18
	13312, 8192, // 19
	14336, 0, // 20
	14336, 8192, // 21
	15360, 0, // 22
	15360, 8192, // 23
	16384, 0, // 24
	16384, 8192, // 25
	17408, 0, // 26
	17408, 8192, // 27
	18432, 0, // 28
	18432, 8192, // 29
	19456, 0, // 30
	19456, 8192, // 31
	20480, 0, // 32
	20480, 8192, // 33
	21504, 0, // 34
	21504, 8192, // 35
	22528, 0, // 36
	22528, 8192, // 37
	23552, 0, // 38
	23552, 8192, // 39
	28672, 0, // 40
	28672, 8192, // 41
};

void* wakka_dl_0_tbl[] = {
	wakka_dl_0_0, // 0
	wakka_dl_0_1, // 1
	wakka_dl_0_2, // 2
	wakka_dl_0_3, // 3
	wakka_dl_0_4, // 4
	wakka_dl_0_5, // 5
	wakka_dl_0_6, // 6
	wakka_dl_0_7, // 7
	wakka_dl_0_8, // 8
	wakka_dl_0_9, // 9
	wakka_dl_0_10, // 10
	wakka_dl_0_11, // 11
	wakka_dl_0_12, // 12
	wakka_dl_0_13, // 13
	wakka_dl_0_14, // 14
	wakka_dl_0_15, // 15
	wakka_dl_0_16, // 16
	wakka_dl_0_17, // 17
	wakka_dl_0_18, // 18
	wakka_dl_0_19, // 19
};
#else
u8 wakka_dl_0_0[];
u8 wakka_dl_0_1[];
u8 wakka_dl_0_2[];
u8 wakka_dl_0_3[];
u8 wakka_dl_0_4[];
u8 wakka_dl_0_5[];
u8 wakka_dl_0_6[];
u8 wakka_dl_0_7[];
u8 wakka_dl_0_8[];
u8 wakka_dl_0_9[];
u8 wakka_dl_0_10[];
u8 wakka_dl_0_11[];
u8 wakka_dl_0_12[];
u8 wakka_dl_0_13[];
u8 wakka_dl_0_14[];
u8 wakka_dl_0_15[];
u8 wakka_dl_0_16[];
u8 wakka_dl_0_17[];
u8 wakka_dl_0_18[];
u8 wakka_dl_0_19[];
void* wakka_dl_0_tbl[];
u8 wakka_dl_0_size_tbl[];
s16 wakka_vertex_tbl[];
s8 wakka_normal_tbl[];
u32 wakka_color0_tbl[];
s16 wakka_texcoord0_tbl[];
#endif

//-3.5 / 24 * i (up to 25) with some imprecision
f32 scroll_tbl[] = {
	0.0f, -0.14583334f, -0.29166669f, -0.43750003f, -0.58333337f,
	-0.72916669f, -0.87500006f, -1.0208334f, -1.1666667f, -1.3125f,
	-1.4583334f, -1.6041666f, -1.75f, -1.8958334f, -2.0416667f,
	-2.1875f, -2.3333333f, -2.4791667f, -2.625f, -2.7708333f,
	-2.9166667f, -3.0625f, -3.2083333f, -3.3541667f, -3.5f
};


EffectEntry* effUltraHammerEntry(BOOL arg0, s32 counter, f32 x, f32 y, f32 z, f32 scale) {
    EffectEntry* effect;
    EffUltraHammerData* data;
    s32 i;

    effect = effEntry();
    effect->type = "UltraHammer";
    effect->count = 4;
    data = __memAlloc(HEAP_EFFECT, sizeof(EffUltraHammerData) * 4);
    effect->userdata = data;
    effect->callback = effUltraHammerMain;
    data->unk0 = arg0;
    data->position.x = x;
    data->position.y = y;
    data->position.z = z;
    data->scale = scale;
    if (counter <= 0) {
        data->counter = 1000;
    }
    else {
        data->counter = counter;
    }
    data->alpha = 0xFF;
    data->scroll = 0;
    data++;
    for (i = 1; i < effect->count; i++, data++) {
        s32 updates[3] = {0, 30, 15};
        data->position.x = 0.0f;
        data->position.y = -4.0f * (f32)(i - 1);
        data->position.z = 0.0f;
        data->scale = 1.0f;
        data->update = updates[i - 1];
        data->scroll = 0;
        data->counter = 0;
    }
    return effect;
}

void effUltraHammerMain(EffectEntry* effect) {
    EffUltraHammerData* data;
    Vec position;
    int i;

    data = effect->userdata;
    position = initializeVec(&data->position);
    if (data->counter < 1000) {
        data->counter--;
    }
    if (data->counter < 16) {
        data->alpha = data->counter * 16;
    }
    if (data->counter < 0) {
        effDelete(effect);
        return;
    }
    data++;
    for (i = 1; i < effect->count; i++, data++) {
        if (data->update != 0) {
            data->update--;
        } else {
            if (data->scroll < 25) {
                data->offsetX = scroll_tbl[data->scroll];
                data->scale = 3.5f;
                data->scroll += 1;
            } else {
                data->update = 15;
                data->scroll = 0;
            }
        }
    }
    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effUltraHammerDisp, effect);
}

// unknown function, is used for many functions to "initialize"/copy some value into the local stack, with an extra vec
inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}


#ifdef MUST_MATCH
// lbl_803B70C0 is evt_memcard's last object; the original reaches the wakka tables that follow it
// as offsets from that symbol.
#define DATA_AT(off, sym) (base + (off))
#else
#define DATA_AT(off, sym) ((u8*)(sym))
#endif

// MUST_MATCH: splitting the 20-entry display-list loop into this inline function gives it the
// original's callee-saved colouring for the loop (dlptr/size_tbl/j in r27/r28/r29); inlined at
// the call site the compiler instead colours them r28/r29/r27. The natural form is the #else loop.
#ifdef MUST_MATCH
static inline void wakkaDrawDL(u8* base) {
    u32 j;
    for (j = 0; j < 20; j++) {
        GXCallDisplayList(((u8**)DATA_AT(0x1e8, wakka_dl_0_tbl))[j], wakka_dl_0_size_tbl[j] * 32);
    }
}
#else
static inline void wakkaDrawDL(void) {
    u32 j;
    for (j = 0; j < 20; j++) {
        GXCallDisplayList(wakka_dl_0_tbl[j], wakka_dl_0_size_tbl[j] * 32);
    }
}
#endif

void effUltraHammerDisp(enum CameraId camId, void *param) {
    EffectEntry *effect;
    CameraEntry *temp_r31;
    EffUltraHammerData *data;
    int i;
    s32 temp_r24;
#ifdef MUST_MATCH
    u8* base;
#endif
    Mtx spC0;
    Mtx sp90;
    Mtx sp60;
    Mtx sp30;
    GXTexObj sp10;
    
    effect = param;
#ifdef MUST_MATCH
    base = lbl_803B70C0;
#endif
    data = effect->userdata;
    temp_r31 = camGetPtr(camId);
    temp_r24 = data->unk0;
    
    PSMTXTrans(spC0, data->position.x, data->position.y, data->position.z);
    PSMTXScale(sp60, data->scale, data->scale, data->scale);
    PSMTXRotRad(sp90, 'y', MTXDegToRad(-camGetPtr(camId)->viewYaw));
    PSMTXConcat(spC0, sp90, spC0);
    PSMTXConcat(spC0, sp60, sp30);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, 0, GX_SRC_REG, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(2);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 30, 0, 125);
    effGetTexObj(145, &sp10);
    GXLoadTexObj(&sp10, GX_TEXMAP0);
    GXSetCullMode(GX_CULL_NONE);
    GXSetTevColor(GX_TEVREG0, (GXColor){0xFF, 0xFF, 0xFF, data->alpha});
    data++;
    for (i = 1; i < effect->count; i++, data++) {
        if (data->update == 0) {
            PSMTXTrans(spC0, data->position.x, data->position.y, data->position.z);
            PSMTXScale(sp60, 0.6f * data->scale, 0.6f * data->scale, 0.6f * data->scale);
            PSMTXConcat(spC0, sp60, spC0);
            if (temp_r24 != 0) {
                PSMTXScale(sp60, -1.0f, 1.0f, 1.0f);
                PSMTXConcat(spC0, sp60, spC0);
            }
            PSMTXConcat(sp30, spC0, spC0);
            PSMTXConcat(temp_r31->view, spC0, spC0);
            GXLoadPosMtxImm(spC0, 0);
            GXSetCurrentMtx(0);
            PSMTXTrans(spC0, data->offsetX, data->offsetY, 0.0f);
            GXLoadTexMtxImm(spC0, 30, GX_MTX2x4);
            GXClearVtxDesc();
            GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
            GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_TEX_ST, GX_RGBA4, 10);
            GXSetArray(GX_VA_POS, DATA_AT(0x40, wakka_vertex_tbl), 6);
            GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
            GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_TEX_S, GX_RGB8, 6);
            GXSetArray(GX_VA_NRM, &wakka_normal_tbl, 3);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
            GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_TEX_ST, GX_RGBA8, 0);
            GXSetArray(GX_VA_CLR0, &wakka_color0_tbl, 4);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
            GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_RGBA4, 13);
            GXSetArray(GX_VA_TEX0, DATA_AT(0x140, wakka_texcoord0_tbl), 4);
#ifdef MUST_MATCH
            wakkaDrawDL(base);
#else
            wakkaDrawDL();
#endif
        }
    }
}
