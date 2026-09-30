#include "eff/eff_number.h"
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdio.h>
#include <string.h>

// Register allocation is the only difference; the instruction stream and switch dispatch match.
// Folding `end` into the index (`buf[len - 1 - j]`) puts value/color in the target's r14/r17, but
// type lands in r18 (not r19), cam in r25 (not r18), len stays r26 instead of reusing type's r19 and
// the eight colour bytes take r18-r25 instead of r20-r27. The named-`end` form scores 98.58% (cam r14,
// value r17, color r18, len r26). Local declaration order, GXColor-returning inlines, -O4,s/
// optimize_for_size, opt_propagation/opt_common_subs/opt_lifetimes and register/volatile hints all
// score lower.
// NONMATCHING (98.71%): len does not reuse type's r19; value/color/cam are r14/r17/r25, not r14/r17/r18.
void effNumberGX(Mtx* mtx, s32 type, s32 value, s32 color, s32 unk) {
	CameraEntry* cam;
	GXColor colors[2];
	GXTexObj tex;
	Mtx tmp;
	Mtx scale;
	f32 neg24;
	f32 pos24;
	char buf[0x20];
	s32 half;
	s32 len;
	s32 i;
	s32 j;

	cam = camGetCurPtr();

	switch (type) {
	case 0:
		colors[1] = (GXColor){ 0xDE, 0x0B, 0x16, color };
		colors[0] = (GXColor){ 0xFF, 0xF0, 0xF1, color };
		break;
	case 1:
		colors[1] = (GXColor){ 0x68, 0xC0, 0x03, color };
		colors[0] = (GXColor){ 0xEB, 0xFE, 0x6F, color };
		break;
	case 2:
		colors[1] = (GXColor){ 0xFF, 0x56, 0x27, color };
		colors[0] = (GXColor){ 0xFF, 0xC5, 0x18, color };
		break;
	case 3:
	default:
		colors[1] = (GXColor){ 0xFF, 0xFF, 0xFF, color };
		colors[0] = (GXColor){ 0xFF, 0xFF, 0xFF, color };
		break;
	}

	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 30, 0, 125);
	GXSetCullMode(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xB, 1);
	GXSetVtxDesc(0xD, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
	GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);

	sprintf(buf, "%d", value);
	len = strlen(buf);
	half = len / 2;
	neg24 = -24.0f;
	pos24 = 24.0f;

	for (i = 0; i < unk + 1; i++) {
		if (i == 0) {
			effGetTexObj(1, &tex);
			GXLoadTexObj(&tex, 0);
			GXSetNumTevStages(1);
			GXSetTevOrder(0, 0, 0, 4);
			GXSetTevColorOp(0, 0, 0, 0, 1, 0);
			GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
			GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0xF);
			GXSetTevAlphaIn(0, 7, 5, 4, 7);
			GXSetTevSwapMode(0, 0, 0);
		} else {
			effGetTexObj(0x88, &tex);
			GXLoadTexObj(&tex, 0);
			GXSetNumTevStages(3);
			GXSetTevOrder(0, 0, 0, 0xFF);
			GXSetTevColorOp(0, 0xE, 0, 0, 1, 0);
			GXSetTevAlphaOp(0, 0xE, 0, 0, 1, 0);
			GXSetTevColorIn(0, 8, 0xF, 8, 0xF);
			GXSetTevAlphaIn(0, 4, 7, 4, 7);
			GXSetTevSwapModeTable(1, 0, 1, 2, 0);
			GXSetTevSwapMode(0, 1, 1);
			GXSetTevOrder(1, 0xFF, 0xFF, 4);
			GXSetTevColorOp(1, 0, 0, 0, 1, 0);
			GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
			GXSetTevColorIn(1, 0xF, 0, 0xA, 0xF);
			GXSetTevAlphaIn(1, 7, 7, 7, 0);
			GXSetTevSwapMode(1, 0, 0);
			GXSetTevOrder(2, 0, 0, 4);
			GXSetTevColorOp(2, 0, 0, 0, 1, 0);
			GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
			GXSetTevColorIn(2, 2, 0, 1, 0xF);
			GXSetTevAlphaIn(2, 7, 4, 5, 7);
			GXSetTevColor(1, (GXColor){ 0x3C, 0x3C, 0x3C, 0xFF });
			GXSetTevSwapMode(2, 0, 0);
		}

		for (j = 0; j < len; j++) {
			PSMTXTrans(tmp, 32.0f * (half - j) - i * 2, (f32)(i * 2), 0.0f);
			PSMTXConcat(*mtx, tmp, tmp);
			PSMTXConcat(cam->view, tmp, tmp);
			GXLoadPosMtxImm(tmp, 0);
			GXSetCurrentMtx(0);

			if ((s8)buf[len - 1 - j] == '-') {
				PSMTXScale(scale, 0.09090909f, 1.0f, 1.0f);
				PSMTXTrans(tmp, 10.0f, 0.0f, 0.0f);
				PSMTXConcat(scale, tmp, tmp);
				GXLoadTexMtxImm(tmp, 30, GX_MTX2x4);
			} else {
				PSMTXScale(scale, 0.09090909f, 1.0f, 1.0f);
				PSMTXTrans(tmp, (f32)((s8)buf[len - 1 - j] - '0'), 0.0f, 0.0f);
				PSMTXConcat(scale, tmp, tmp);
				GXLoadTexMtxImm(tmp, 30, GX_MTX2x4);
			}

			GXBegin(0x80, 0, 4);
			GXPosition3f32(neg24, pos24, 0.0f);
			GXColor4u8(colors[1].r, colors[1].g, colors[1].b, colors[1].a);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(pos24, pos24, 0.0f);
			GXColor4u8(colors[1].r, colors[1].g, colors[1].b, colors[1].a);
			GXTexCoord2f32(1.0f, 0.0f);
			GXPosition3f32(pos24, neg24, 0.0f);
			GXColor4u8(colors[0].r, colors[0].g, colors[0].b, colors[0].a);
			GXTexCoord2f32(1.0f, 1.0f);
			GXPosition3f32(neg24, neg24, 0.0f);
			GXColor4u8(colors[0].r, colors[0].g, colors[0].b, colors[0].a);
			GXTexCoord2f32(0.0f, 1.0f);
		}

	}

	GXSetTevSwapModeTable(1, 0, 1, 2, 3);
	GXSetTevSwapMode(0, 0, 0);
}
