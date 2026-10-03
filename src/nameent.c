#include "nameent.h"
#include "driver/dispdrv.h"
#include "driver/fadedrv.h"
#include "system.h"
#include <string.h>
#include "math_ext.h"

//.data
const char* katakana_tbl[] = {
	"\x83\x41", "\x83\x43", "\x83\x45", "\x83\x47", "\x83\x49", "\x83\x8F",
	"\x81\x40", "\x83\x92", "\x81\x40", "\x83\x93", "\x82\x4F", "\x82\x50",
	"\x82\x51", "\x82\x52", "\x83\x4A", "\x83\x4C", "\x83\x4E", "\x83\x50",
	"\x83\x52", "\x83\x4B", "\x83\x4D", "\x83\x4F", "\x83\x51", "\x83\x53",
	"\x82\x53", "\x82\x54", "\x82\x55", "\x82\x56", "\x83\x54", "\x83\x56",
	"\x83\x58", "\x83\x5A", "\x83\x5C", "\x83\x55", "\x83\x57", "\x83\x59",
	"\x83\x5B", "\x83\x5D", "\x82\x57", "\x82\x58", "\x81\x49", "\x81\x48",
	"\x83\x5E", "\x83\x60", "\x83\x63", "\x83\x65", "\x83\x67", "\x83\x5F",
	"\x83\x61", "\x83\x64", "\x83\x66", "\x83\x68", "\x81\x40", "\x94\xB5",
	"\x81\x99", "\x81\xF4", "\x83\x69", "\x83\x6A", "\x83\x6B", "\x83\x6C",
	"\x83\x6D", "\x83\x6F", "\x83\x72", "\x83\x75", "\x83\x78", "\x83\x7B",
	NULL,       NULL,       NULL,       NULL,       "\x83\x6E", "\x83\x71",
	"\x83\x74", "\x83\x77", "\x83\x7A", "\x83\x70", "\x83\x73", "\x83\x76",
	"\x83\x79", "\x83\x7C", NULL,       NULL,       NULL,       NULL,
	"\x83\x7D", "\x83\x7E", "\x83\x80", "\x83\x81", "\x83\x82", "\x83\x40",
	"\x83\x42", "\x83\x44", "\x83\x46", "\x83\x48", NULL,       NULL,
	NULL,       NULL,       "\x83\x84", "\x81\x40", "\x83\x86", "\x81\x40",
	"\x83\x88", "\x83\x83", "\x83\x85", "\x83\x87", "\x83\x62", "\x83\x94",
	NULL,       NULL,       NULL,       NULL,       "\x83\x89", "\x83\x8A",
	"\x83\x8B", "\x83\x8C", "\x83\x8D", "\x81\x7C", "\x81\x60", "\x81\x45",
	"\x81\x44", "\x81\x40", NULL,       NULL,       NULL,       NULL
};
const char* hiragana_tbl[] = {
	"\x82\xA0", "\x82\xA2", "\x82\xA4", "\x82\xA6", "\x82\xA8", "\x82\xED",
	"\x81\x40", "\x82\xF0", "\x81\x40", "\x82\xF1", "\x82\x4F", "\x82\x50",
	"\x82\x51", "\x82\x52", "\x82\xA9", "\x82\xAB", "\x82\xAD", "\x82\xAF",
	"\x82\xB1", "\x82\xAA", "\x82\xAC", "\x82\xAE", "\x82\xB0", "\x82\xB2",
	"\x82\x53", "\x82\x54", "\x82\x55", "\x82\x56", "\x82\xB3", "\x82\xB5",
	"\x82\xB7", "\x82\xB9", "\x82\xBB", "\x82\xB4", "\x82\xB6", "\x82\xB8",
	"\x82\xBA", "\x82\xBC", "\x82\x57", "\x82\x58", "\x81\x49", "\x81\x48",
	"\x82\xBD", "\x82\xBF", "\x82\xC2", "\x82\xC4", "\x82\xC6", "\x82\xBE",
	"\x82\xC0", "\x82\xC3", "\x82\xC5", "\x82\xC7", "\x81\x40", "\x94\xB5",
	"\x81\x99", "\x81\xF4", "\x82\xC8", "\x82\xC9", "\x82\xCA", "\x82\xCB",
	"\x82\xCC", "\x82\xCE", "\x82\xD1", "\x82\xD4", "\x82\xD7", "\x82\xDA",
	NULL,       NULL,       NULL,       NULL,       "\x82\xCD", "\x82\xD0",
	"\x82\xD3", "\x82\xD6", "\x82\xD9", "\x82\xCF", "\x82\xD2", "\x82\xD5",
	"\x82\xD8", "\x82\xDB", NULL,       NULL,       NULL,       NULL,
	"\x82\xDC", "\x82\xDD", "\x82\xDE", "\x82\xDF", "\x82\xE0", "\x82\x9F",
	"\x82\xA1", "\x82\xA3", "\x82\xA5", "\x82\xA7", NULL,       NULL,
	NULL,       NULL,       "\x82\xE2", "\x81\x40", "\x82\xE4", "\x81\x40",
	"\x82\xE6", "\x82\xE1", "\x82\xE3", "\x82\xE5", "\x82\xC1", "\x89\x4C",
	NULL,       NULL,       NULL,       NULL,       "\x82\xE7", "\x82\xE8",
	"\x82\xE9", "\x82\xEA", "\x82\xEB", "\x81\x7C", "\x81\x60", "\x81\x45",
	"\x81\x44", "\x81\x40", NULL,       NULL,       NULL,       NULL
};

//.bss
static NameEntryWork work;

//.sdata
static const char* msg_tbl[] = {
	"msg_nameent_1",
	"msg_nameent_2",
	"msg_nameent_3",
	"msg_nameent_3",
	"msg_nameent_4"
};
static NameEntryWork* wp = &work;

void nameMaskGX(f32 alpha) {
	GXColor color;
	GXTexObj texObj;
	Mtx mtxMain;
	Mtx mtxScale;
	Mtx mtxNeg;
	f32 scale;

	scale = 1000.0f * alpha * alpha * alpha * alpha * alpha;
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_OR);
	GXSetZMode(GX_DISABLE, GX_ALWAYS, GX_DISABLE);
	GXSetNumChans(0);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
	color = (GXColor){ 0, 0, 0, 0x80 };
	GXSetTevColor(GX_TEVREG0, color);
	TEXGetGXTexObjFromPalette(fadeGetTpl(), &texObj, 1);
	GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetNumTexGens(1);
	PSMTXTrans(mtxMain, 0.5f, 0.5f, 0.0f);
	PSMTXScale(mtxScale, scale, scale, 0.0f);
	PSMTXTrans(mtxNeg, -0.5f, -0.5f, 0.0f);
	PSMTXConcat(mtxMain, mtxScale, mtxMain);
	PSMTXConcat(mtxMain, mtxNeg, mtxMain);
	GXLoadTexMtxImm(mtxMain, GX_TEXMTX0, GX_MTX2x4);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, 0);
	GXSetCurrentMtx(0);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3s16(-304, 240, 0);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3s16(304, 240, 0);
	GXTexCoord2f32(1.0f, 0.0f);
	GXPosition3s16(304, -240, 0);
	GXTexCoord2f32(1.0f, 1.0f);
	GXPosition3s16(-304, -240, 0);
	GXTexCoord2f32(0.0f, 1.0f);
}

// The scale is the original's (f32)__fabs(<double literal>) / GXGetTexObj*; the separate f64
// temporaries per quad keep the pool in instruction order. Depends on the file using math_ext.h
// instead of <math.h>: math.h's extern inline sqrtf/sqrt put _half/_three at the start of
// .rodata and stop the compiler anchoring the pool to str_802FD9A8.
//
// NONMATCHING (99.97%): only the .rodata pool offsets of the four doubled constants differ
// (480/32/544/int-mask sit at 0x60/0x68/0x70/0x78 from str_802FD9A8, not 0x148/0x150/0x158/0x138);
// they resolve once the rest of the unit's constants are in the object.
void nameBG(void) {
	GXTexObj texObj1;
	GXTexObj texObj2;
	GXTexObj texObj3;
	Mtx mtx1;
	Mtx mtx2;
	Mtx mtx3;
	f32 xscale;
	f32 yscale;
	f64 yt;
	f64 xt;
	f64 yt2;
	f64 xt2;
	f64 yt3;
	f64 xt3;

	GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
	GXSetZCompLoc(GX_FALSE);
	GXSetAlphaCompare(GX_GEQUAL, 0x80, GX_AOP_OR, GX_NEVER, 0);
	GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
	GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
	GXSetNumChans(0);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, 0);
	GXSetCurrentMtx(0);

	TEXGetGXTexObjFromPalette((TEXPalette*)wp->field_0x60->data[0], &texObj1, 0x22);
	GXInitTexObjLOD(&texObj1, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
	GXLoadTexObj(&texObj1, GX_TEXMAP0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	yt = __fabs(480.0);
	xt = __fabs(32.0);
	yscale = (f32)yt / GXGetTexObjHeight(&texObj1);
	xscale = (f32)xt / GXGetTexObjWidth(&texObj1);
	PSMTXScale(mtx1, xscale, yscale, 1.0f);
	GXLoadTexMtxImm(mtx1, GX_TEXMTX0, GX_MTX2x4);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3f32(-304.0f, 240.0f, 0.0f);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3f32(-304.0f, -240.0f, 0.0f);
	GXTexCoord2f32(0.0f, 1.0f);
	GXPosition3f32(-272.0f, -240.0f, 0.0f);
	GXTexCoord2f32(1.0f, 1.0f);
	GXPosition3f32(-272.0f, 240.0f, 0.0f);
	GXTexCoord2f32(1.0f, 0.0f);

	TEXGetGXTexObjFromPalette((TEXPalette*)wp->field_0x60->data[0], &texObj2, 0x21);
	GXInitTexObjLOD(&texObj2, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
	GXLoadTexObj(&texObj2, GX_TEXMAP0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	yt2 = __fabs(480.0);
	xt2 = __fabs(544.0);
	yscale = (f32)yt2 / GXGetTexObjHeight(&texObj2);
	xscale = (f32)xt2 / GXGetTexObjWidth(&texObj2);
	PSMTXScale(mtx2, xscale, yscale, 1.0f);
	GXLoadTexMtxImm(mtx2, GX_TEXMTX0, GX_MTX2x4);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3f32(-272.0f, 240.0f, 0.0f);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3f32(-272.0f, -240.0f, 0.0f);
	GXTexCoord2f32(0.0f, 1.0f);
	GXPosition3f32(272.0f, -240.0f, 0.0f);
	GXTexCoord2f32(1.0f, 1.0f);
	GXPosition3f32(272.0f, 240.0f, 0.0f);
	GXTexCoord2f32(1.0f, 0.0f);

	TEXGetGXTexObjFromPalette((TEXPalette*)wp->field_0x60->data[0], &texObj3, 0x23);
	GXInitTexObjLOD(&texObj3, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
	GXLoadTexObj(&texObj3, GX_TEXMAP0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	yt3 = __fabs(480.0);
	xt3 = __fabs(32.0);
	yscale = (f32)yt3 / GXGetTexObjHeight(&texObj3);
	xscale = (f32)xt3 / GXGetTexObjWidth(&texObj3);
	PSMTXScale(mtx3, xscale, yscale, 1.0f);
	GXLoadTexMtxImm(mtx3, GX_TEXMTX0, GX_MTX2x4);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3f32(272.0f, 240.0f, 0.0f);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3f32(272.0f, -240.0f, 0.0f);
	GXTexCoord2f32(0.0f, 1.0f);
	GXPosition3f32(304.0f, -240.0f, 0.0f);
	GXTexCoord2f32(1.0f, 1.0f);
	GXPosition3f32(304.0f, 240.0f, 0.0f);
	GXTexCoord2f32(1.0f, 0.0f);
}

#define NAME_KIRINUKI_SCALE(id_, W, H) \
	{ \
		GXTexObj texObj; \
		Mtx mtx; \
		f32 xs; \
		f64 hh; \
		f64 ww; \
		TEXGetGXTexObjFromPalette((TEXPalette*)wp->field_0x60->data[0], &texObj, (id_)); \
		GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1); \
		GXLoadTexObj(&texObj, GX_TEXMAP0); \
		GXSetNumTexGens(1); \
		GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY); \
		hh = __fabs(H); \
		ww = __fabs(W); \
		yscale = (f32)hh / (f32)GXGetTexObjHeight(&texObj); \
		xs = (f32)ww / (f32)GXGetTexObjWidth(&texObj); \
		PSMTXScale(mtx, xs, yscale, 1.0f); \
		GXLoadTexMtxImm(mtx, GX_TEXMTX0, GX_MTX2x4); \
	}


static inline void nameKirinukiQuad(f32 x, f32 y, f32 w, f32 h) {
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3f32(x, y, 0.0f);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3f32(x, y - h, 0.0f);
	GXTexCoord2f32(0.0f, 1.0f);
	GXPosition3f32(x + w, y - h, 0.0f);
	GXTexCoord2f32(1.0f, 1.0f);
	GXPosition3f32(x + w, y, 0.0f);
	GXTexCoord2f32(1.0f, 0.0f);
}

// NONMATCHING (99.75%): frame 0x3e0 and the whole instruction stream match; only the float callee-saved register numbers of the eight per-quad size/temp values differ (46 register-name diffs) — MWCC's float allocator parks the short-lived `__fabs(8.0f)` temp in f24 where the target keeps a live size value there.
void nameKirinukiGX(f32 x, f32 y, f32 z, f32 w) {
	f32 yscale;

	GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
	GXSetZCompLoc(GX_FALSE);
	GXSetAlphaCompare(GX_GEQUAL, 0x80, GX_AOP_OR, GX_NEVER, 0);
	GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
	GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
	GXSetNumChans(0);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, 0);
	GXSetCurrentMtx(0);

	NAME_KIRINUKI_SCALE(0x2f, 8.0f, 8.0f);
	nameKirinukiQuad(x, y, 8.0f, 8.0f);
	NAME_KIRINUKI_SCALE(0x2d, z - 8.0f - 8.0f, 8.0f);
	nameKirinukiQuad(x + 8.0f, y, z - 8.0f - 8.0f, 8.0f);
	NAME_KIRINUKI_SCALE(0x31, 8.0f, 8.0f);
	nameKirinukiQuad(x + z - 8.0f, y, 8.0f, 8.0f);
	NAME_KIRINUKI_SCALE(0x2e, 8.0f, w - 8.0f - 8.0f);
	nameKirinukiQuad(x, y - 8.0f, 8.0f, w - 8.0f - 8.0f);
	NAME_KIRINUKI_SCALE(0x2c, z - 8.0f, w - 8.0f - 8.0f);
	nameKirinukiQuad(x + 8.0f, y - 8.0f, z - 8.0f, w - 8.0f - 8.0f);
	NAME_KIRINUKI_SCALE(0x30, 8.0f, 8.0f);
	nameKirinukiQuad(x, y - (w - 8.0f), 8.0f, 8.0f);
	NAME_KIRINUKI_SCALE(0x2c, z - 8.0f - 8.0f, 8.0f);
	nameKirinukiQuad(x + 8.0f, y - (w - 8.0f), z - 8.0f - 8.0f, 8.0f);
	NAME_KIRINUKI_SCALE(0x32, 8.0f, 8.0f);
	nameKirinukiQuad(x + z - 8.0f, y - (w - 8.0f), 8.0f, 8.0f);
}

void nameEntMain(void) {
	BOOL loaded;

	switch (wp->field_0x4) {
	case 0:
		break;
	case 1:
		if (fileAsyncf(4, NULL, "%s/w/jp/name.tpl", getMarioStDvdRoot()) != 0U) {
			wp->field_0x60 = fileAllocf(4, "%s/w/jp/name.tpl", getMarioStDvdRoot());
			loaded = TRUE;
		} else {
			loaded = FALSE;
		}
		if (loaded) {
			wp->field_0x4++;
		}
	case 2:
		nameMain();
		break;
	case 3:
		if (wp->field_0x60 != NULL) {
			fileFree(wp->field_0x60);
		}
		wp->field_0x60 = NULL;
		wp->field_0x4 = 0;
		break;
	}
	if (wp->field_0x4 > 1) {
		dispEntry(4, 0.0f, 7, nameEntDisp, NULL);
	}
}

char* nameEntGetName(void) {
	s32 off;
	s32 i;

	if (wp->field_0x8 != 0x65) {
		return NULL;
	}
	for (i = 7, off = 0xe; i >= 1; i--, off -= 2) {
		if (strncmp(&wp->field_0x3C[off], "\x81\x40", 2) != 0) {
			break;
		}
		wp->field_0x3C[off] = 0;
		wp->field_0x3C[off + 1] = 0;
	}
	return wp->field_0x3C;
}

BOOL nameEntIsCancel(void) {
	return (wp->field_0x0 >> 1) & 1;
}

BOOL nameEntWait(void) {
	wp->field_0x0 |= 1;
	return wp->field_0x8 != 0x65;
}

void nameEntOff(void) {
	wp->field_0x4 = 3;
}

// The integer expression is written in the original's operand order under MUST_MATCH; the natural form
// `x*24 + x/5*30 - 190` reassociates and emits a different register/add order. The original selects the
// type-2 table entry with a two-case switch, which is why it branches `beq case2; b default`.
void nameEntOn(s32 type) {
	s32 i;

	wp->field_0x0 = 0;
	wp->field_0xC = type;
	wp->field_0x4 = 1;
	wp->field_0x8 = 0;
	wp->field_0x50 = hiragana_tbl;
	wp->field_0x18 = 0;
	wp->field_0x10 = 0;
	wp->field_0x14 = 0;

#ifdef MUST_MATCH
	wp->x1 = wp->x2 = (f32)(wp->field_0x10 * 24 - 190 + wp->field_0x10 / 5 * 30);
#else
	wp->x1 = wp->x2 = (f32)(wp->field_0x10 * 24 + wp->field_0x10 / 5 * 30 - 190);
#endif
	wp->y1 = wp->y2 = (f32)(112 - wp->field_0x14 * 28);
	wp->field_0x2C = wp->field_0x34 = -106.0f;
	wp->field_0x30 = wp->field_0x38 = 135.0f;

	strcpy(wp->field_0x3C, "");
	for (i = 0; i < 8; i++) {
		strcat(wp->field_0x3C, "\x81\x40");
	}
	wp->field_0x54 = 0.0f;

	switch (type) {
	case 2:
		hiragana_tbl[9] = "\x81\x40";
		katakana_tbl[9] = "\x81\x40";
		break;
	default:
		hiragana_tbl[9] = "\x82\xF1";
		katakana_tbl[9] = "\x83\x93";
		break;
	}
}

BOOL nameEntPrepare(void) {
	return fileAsyncf(4, NULL, "%s/w/jp/name.tpl", getMarioStDvdRoot()) != 0;
}

void nameEntReInit(void) {
	wp->field_0x4 = 3;
}

void nameEntInit(void) {
	memset(wp, 0, sizeof(NameEntryWork));
}
static inline void nameWinQuad(f32 x, f32 y, f32 w, f32 h) {
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3f32(x, y, 0.0f);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3f32(x, y - h, 0.0f);
	GXTexCoord2f32(0.0f, 1.0f);
	GXPosition3f32(x + w, y - h, 0.0f);
	GXTexCoord2f32(1.0f, 1.0f);
	GXPosition3f32(x + w, y, 0.0f);
	GXTexCoord2f32(1.0f, 0.0f);
}

// The second coord's scale. Separate f64 temps (hh2/ww2) reproduce the target's
// callee-saved FP assignment; a shared hh/ww pair reassigns the register the first
// coord just freed, so f23/f24 come out swapped.
#ifdef MUST_MATCH
#define NAME_WIN_SCALE2(H, W) \
		hh2 = __fabs(H); \
		ww2 = __fabs(W); \
		yscale = (f32)hh2 / (f32)GXGetTexObjHeight(&texObj); \
		xs = (f32)ww2 / (f32)GXGetTexObjWidth(&texObj);
#else
#define NAME_WIN_SCALE2(H, W) \
		hh = __fabs(H); \
		ww = __fabs(W); \
		yscale = (f32)hh / (f32)GXGetTexObjHeight(&texObj); \
		xs = (f32)ww / (f32)GXGetTexObjWidth(&texObj);
#endif

#define NAME_WIN_QUAD(tex, id0, id1, id2, vx, vy, W, H) \
	{ \
		GXTexObj texObj; \
		Mtx mtx; \
		f32 xs; \
		f64 hh; \
		f64 ww; \
		f64 hh2; \
		f64 ww2; \
		GXSetNumTexGens(2); \
		TEXGetGXTexObjFromPalette((TEXPalette*)(tex), &texObj, (id0)); \
		GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1); \
		GXLoadTexObj(&texObj, GX_TEXMAP0); \
		GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY); \
		hh = __fabs(H); \
		ww = __fabs(W); \
		yscale = (f32)hh / (f32)GXGetTexObjHeight(&texObj); \
		xs = (f32)ww / (f32)GXGetTexObjWidth(&texObj); \
		PSMTXScale(mtx, xs, yscale, 1.0f); \
		GXLoadTexMtxImm(mtx, GX_TEXMTX0, GX_MTX2x4); \
		TEXGetGXTexObjFromPalette((TEXPalette*)(tex), &texObj, (id1)); \
		GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1); \
		GXLoadTexObj(&texObj, GX_TEXMAP1); \
		TEXGetGXTexObjFromPalette((TEXPalette*)(tex), &texObj, (id2)); \
		GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1); \
		GXLoadTexObj(&texObj, GX_TEXMAP2); \
		GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY); \
		NAME_WIN_SCALE2(H, W) \
		PSMTXScale(mtx, xs, yscale, 1.0f); \
		GXLoadTexMtxImm(mtx, GX_TEXMTX1, GX_MTX2x4); \
		nameWinQuad(vx, vy, W, H); \
	}

void nameWinGX(void* tex, s32 arg1, f32 x, f32 y, f32 z, f32 w) {
	GXColor color;
	f32 yscale; // shared across the four quads like nameKirinukiGX's

	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_OR);
	GXSetZMode(GX_DISABLE, GX_ALWAYS, GX_DISABLE);
	GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
	GXSetNumChans(0);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTevStages(3);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXA, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
	color = (GXColor){ 0, 0, 0, 0x80 };
	GXSetTevColor(GX_TEVREG0, color);
	GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
	GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD1, GX_TEXMAP2, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_CPREV, GX_CC_TEXC, GX_CC_TEXA, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_APREV, GX_CA_KONST, GX_CA_TEXA, GX_CA_ZERO);
	GXSetTevKAlphaSel(GX_TEVSTAGE2, GX_TEV_KASEL_1);
	GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, 0);
	GXSetCurrentMtx(0);

	NAME_WIN_QUAD(tex, arg1 + 4, arg1 + 5, arg1, x, y, z - 16.0f, w - 16.0f);
	NAME_WIN_QUAD(tex, arg1 + 4, arg1 + 7, arg1 + 2, x + z - 16.0f, y, 16.0f, w - 16.0f);
#ifdef MUST_MATCH
	// bottom row: the top edge is written as bottom+16 so it is recomputed from the
	// y-w the compiler already has; the natural `y - (w - 16.0f)` gets CSE'd with the
	// upper rows and throws off the callee-saved FP assignment.
	NAME_WIN_QUAD(tex, arg1 + 4, arg1 + 6, arg1 + 1, x, y - w + 16.0f, z - 16.0f, 16.0f);
	NAME_WIN_QUAD(tex, arg1 + 4, arg1 + 8, arg1 + 3, x + z - 16.0f, y - w + 16.0f, 16.0f, 16.0f);
#else
	NAME_WIN_QUAD(tex, arg1 + 4, arg1 + 6, arg1 + 1, x, y - (w - 16.0f), z - 16.0f, 16.0f);
	NAME_WIN_QUAD(tex, arg1 + 4, arg1 + 8, arg1 + 3, x + z - 16.0f, y - (w - 16.0f), 16.0f, 16.0f);
#endif
}
