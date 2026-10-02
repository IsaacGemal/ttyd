#include "eff/eff_funemizu.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "driver/camdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"
#include <dolphin/mtx.h>
#include <dolphin/gx.h>
#include <math.h>
#include <string.h>

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u32 lbl_803B0200[2] = { 0xBF800000, 0x3F800000 };

typedef struct FunemizuMapEntry {
	const char* name;
	u32 value;
} FunemizuMapEntry;

const Vec vec3_802fe8a0 = {0.0f, 0.0f, 0.0f};
extern const f32 float_deg2rad_80421470;
extern const f32 float_3_80421474;
extern const f32 float_0_80421478;
extern const f32 float_0p5_8042147c;
extern const f32 float_1p5_80421480;
extern const f32 float_1_80421484;
extern const f32 float_neg15_80421488;
extern const f32 float_2_8042148c;
extern const f32 float_10_80421490;
extern const f32 float_0p05_80421494;
extern const f32 float_0p1_80421498;
extern const f32 float_6p2832_8042149c;
extern const f32 float_360_804214a0;
extern const f32 float_14_804214a4;
extern const f32 float_100_804214a8;
extern const s32 dat_80421468;
extern const u32 dat_8042146c;
extern const char str_802FE8B8[];

extern FunemizuMapEntry unk_803b0640[];
extern u8 funemizu_vertex_tbl[];
extern u8 funemizu_texcoord0_tbl[];
extern u8 funemizu_normal_tbl[];
extern u8 funemizu_color0_tbl[];
extern u8* funemizu_dl_0_tbl[];
extern u8 funemizu_dl_0_size_tbl[];
extern u8* funemizu_dl_1_tbl[];
extern u8 funemizu_dl_1_size_tbl[];
extern u8 gor_fune_vertex_tbl[];
extern u8 gor_fune_texcoord0_tbl[];
extern u8 gor_fune_normal_tbl[];
extern u8 gor_fune_color0_tbl[];
extern u8* gor_fune_dl_0_tbl[];
extern u8 gor_fune_dl_0_size_tbl[];
extern u8* gor_fune_dl_1_tbl[];
extern u8 gor_fune_dl_1_size_tbl[];
extern u8 funemizu_normal_tbl[32];
extern u8 funemizu_color0_tbl[32];
extern u8* funemizu_dl_0_tbl[3];
extern u8 funemizu_dl_0_size_tbl[4];
extern u8* funemizu_dl_1_tbl[4];
extern u8 funemizu_dl_1_size_tbl[4];
extern u8 gor_fune_normal_tbl[32];
extern u8 gor_fune_color0_tbl[8];
extern u8* gor_fune_dl_0_tbl[2];
extern u8 gor_fune_dl_0_size_tbl[4];
extern u8* gor_fune_dl_1_tbl[2];
extern u8 gor_fune_dl_1_size_tbl[4];

const f64 double_to_int_802fe8b0 = 4503601774854144.0;

u32 unk_80224298(void) {
	FunemizuMapEntry* p;

	for (p = unk_803b0640; p; p++) {
		if (p->name == NULL) {
			break;
		}
		if (strcmp(p->name, gp->currentMapName) == 0) {
			return p->value;
		}
	}
	for (p = unk_803b0640; p; p++) {
		if (p->name == NULL) {
			break;
		}
		if (strncmp(p->name, gp->currentMapName, 3) == 0) {
			return p->value;
		}
	}
	return dat_80421468;
}

// 100%: the int->float magic pools as an anonymous @418 literal instead of double_to_int_802fe8b0;
// objdiff does not score the symbol name (same limitation as winMarioMain).  The loop walks `data` with
// `data++` and computes the loop-invariant `d * 0.5f` into `dz` at the store, which makes MWCC colour
// d/e/0.0 as the target (f28/f26/f27) and the entry pointer in r30.
EffectEntry* effFunemizuEntry(s32 type, f32 a, f32 b, f32 c, f32 d, f32 e) {
	EffectEntry* entry;
	EffFunemizuData* data;
	s32 count;
	s32 i;
#ifdef MUST_MATCH
	// Match-only temp: computing the scale in the loop instead of before it reproduces the original's
	// FPR colouring.  The plain `d *= 0.5f` form is under #else.
	f32 dz;
#endif

	entry = effEntry();
	count = 1;
	if (type == 1) {
		count = (s32)d * 3 + 1;
	}
	if (count > 10) {
		count = 10;
	}
	entry->type = str_802FE8B8;
	entry->count = count;
	data = __memAlloc(HEAP_EFFECT, sizeof(EffFunemizuData) * entry->count);
	entry->userdata = data;
	entry->callback = effFunemizuMain;

	data->type = type;
#ifndef MUST_MATCH
	d *= float_0p5_8042147c;
#endif
	data->x = a;
	data->y = b;
	data->z = c;
	data->field_0x18 = float_0_80421478;
	data->field_0x1C = float_0_80421478;
	data->field_0x20 = float_0_80421478;
	data->field_0x24 = float_0_80421478;
	data->field_0x2C = float_0_80421478;
	data->field_0x28 = float_0_80421478;
	data->field_0x30 = 0;
	data->field_0x44 = float_1_80421484;
	data->field_0x48 = 4;

	for (i = 1, data = data + 1; i < entry->count; i++, data++) {
		data->x = float_0_80421478;
		data->y = float_0_80421478;
		data->z = float_0_80421478;
#ifdef MUST_MATCH
		dz = d * float_0p5_8042147c;
		data->field_0x10 = dz;
#else
		data->field_0x10 = d;
#endif
		data->field_0x14 = e;
		data->field_0x34 = data->field_0x10 + (f32)irand(2);
		data->field_0x38 = (f32)irand(10) / float_100_804214a8 + float_0p1_80421498;
		data->field_0x3C = irand(5);
		data->field_0x40 = 255;
	}
	return entry;
}

static inline f32 clamp01(f32 v) {
	if (v < float_0_80421478) {
		v = float_0_80421478;
	}
	if (v > float_1_80421484) {
		v = float_1_80421484;
	}
	return v;
}

// 100%: the clamp above needs opt_common_subs off so each constant test reloads the pool value.
#pragma opt_common_subs off // without it MWCC CSEs the clamp's 1.0/0.0 into f1/f0 and drops the pool reloads
void effFunemizuMain(EffectEntry* effect) {
	EffFunemizuData* data;
	EffFunemizuData* ptr;
	Vec pos2;
	Vec pos;
	s32 type;
	s32 camId;
	s32 i;
	s32 dead;
	f32 a;
	f32 b;

	data = effect->userdata;
	dead = 0;
	pos = vec3_802fe8a0;
	pos.x = data->x;
	pos.y = data->y;
	pos.z = data->z;
	pos2 = pos;

	type = data->type;
	camId = data->field_0x48;
	data->field_0x30++;
	if (data->field_0x30 > 1000) {
		data->field_0x30 -= 1000;
	}

	if (data->type == 0 || data->type == 2) {
		a = data->field_0x10;
		b = a - float_1_80421484;
		a = clamp01(a);
		if (b < float_0_80421478) {
			b = float_0_80421478;
		}
		if (b > float_1_80421484) {
			b = float_1_80421484;
		}
		data->field_0x18 += (a - data->field_0x18) / float_10_80421490;
		data->field_0x1C += (b - data->field_0x1C) / float_10_80421490;
		if (data->field_0x10 > float_1_80421484) {
			data->field_0x20 += (a - data->field_0x20) / float_10_80421490;
			data->field_0x24 += (b - data->field_0x24) / float_10_80421490;
		}
		else {
			f32 d;

			d = a - data->field_0x20;
			data->field_0x20 += d * float_0p5_8042147c;
			d = b - data->field_0x24;
			data->field_0x24 += d * float_0p5_8042147c;
		}
		data->field_0x28 += float_0p05_80421494 * data->field_0x18;
		data->field_0x2C += float_0p1_80421498 * data->field_0x1C;
		if (data->field_0x10 > float_0p1_80421498) {
			if (data->field_0x30 % 10 == 0) {
				f32 sn;
				f32 cs;
				f32 angle;

				angle = float_6p2832_8042149c * (data->field_0x14 - camGetPtr(CAMERA_3D)->viewYaw) / float_360_804214a0;
				sn = (f32)sin(angle);
				cs = (f32)cos(angle);
				effFunemizuEntry(1, data->x - float_14_804214a4 * cs, data->y,
				                 data->z - float_14_804214a4 * sn, data->field_0x10,
				                 data->field_0x14);
			}
		}
	}
	else {
		data->field_0x18 = float_1_80421484;
		for (i = 1, data = data + 1; i < effect->count; i++, data++) {
			f32 s;

			if (data->y < float_0_80421478) {
				dead++;
				continue;
			}
			if (data->field_0x3C != 0) {
				data->field_0x3C--;
				continue;
			}
			data->x += data->field_0x10 * (f32)cos(float_6p2832_8042149c * data->field_0x14 / float_360_804214a0);
			s = (f32)sin(float_6p2832_8042149c * data->field_0x14 / float_360_804214a0);
			data->z += data->field_0x10 * -s;
			data->y += data->field_0x34;
			data->field_0x34 -= data->field_0x38;
			data->field_0x40 -= 8;
			if (data->field_0x40 < 0) {
				data->field_0x40 = 0;
			}
		}
		if (dead >= effect->count - 1) {
			effDelete(effect);
			return;
		}
	}

	if (type == 0 || type == 2) {
		dispEntry(camId, dispCalcZ(&pos2), 2, effFunemizuDisp, effect);
	}
	else {
		dispEntry(camId, dispCalcZ(&pos2), 2, effFunemizuShibukiDisp, effect);
	}
}
#pragma opt_common_subs reset

// NONMATCHING (80.40%): 64 instructions differ, 11 missing, 49 extra, plus register differences.
void effFunemizuDisp(CameraId camId, void* param) {
	EffectEntry* effect;
	EffFunemizuData* data;
	CameraEntry* cam;
	Mtx mtxC0;
	Mtx mtx90;
	Mtx mtx60;
	Mtx mtx30;
	GXTexObj texObj;
	FunemizuMapEntry* p;
	u32 colorVal;
	GXColor color;
	s32 type;
	s32 j;
	s32 i;

	effect = param;
	data = effect->userdata;
	type = data->type;
	cam = camGetPtr(camId);

	for (p = unk_803b0640; p; p++) {
		if (p->name == NULL) {
			break;
		}
		if (strcmp(p->name, gp->currentMapName) == 0) {
			colorVal = p->value;
			goto found;
		}
	}
	for (p = unk_803b0640; p; p++) {
		if (p->name == NULL) {
			break;
		}
		if (strncmp(p->name, gp->currentMapName, 3) == 0) {
			colorVal = p->value;
			goto found;
		}
	}
	colorVal = (u32)dat_80421468;
found:

	color = *(GXColor*)&colorVal;
	if (type == 0) {
		funemizu_color0_tbl[0] = color.r;
		funemizu_color0_tbl[1] = color.g;
		funemizu_color0_tbl[2] = color.b;
	}
	else {
		gor_fune_color0_tbl[0] = color.r;
		gor_fune_color0_tbl[1] = color.g;
		gor_fune_color0_tbl[2] = color.b;
	}

	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);

	PSMTXTrans(mtxC0, data->x, data->y, data->z);
	PSMTXRotRad(mtx60, 'y',
	            float_deg2rad_80421470 * (data->field_0x14 - camGetPtr(camId)->viewYaw));
	PSMTXScale(mtx90, data->field_0x44, data->field_0x44, data->field_0x44);
	PSMTXConcat(mtxC0, mtx60, mtxC0);
	PSMTXConcat(mtxC0, mtx90, mtxC0);
	PSMTXTrans(mtx60, float_neg15_80421488, float_0_80421478, float_0_80421478);
	PSMTXConcat(mtxC0, mtx60, mtxC0);
	PSMTXConcat(cam->view, mtxC0, mtx30);

	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x1e, GX_FALSE, GX_PTIDENTITY);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	effGetTexObj(0x43, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);

	for (i = 0; i < 2; i++) {
		f32 scale;

		if (i == 0) {
			GXSetCullMode(GX_CULL_BACK);
		}
		else {
			GXSetCullMode(GX_CULL_FRONT);
		}
		scale = float_1_80421484 - float_2_8042148c * (f32)i;
		PSMTXTrans(mtxC0, float_0_80421478, float_0_80421478, scale);
		PSMTXConcat(mtx30, mtxC0, mtxC0);
		PSMTXScale(mtx90, float_1_80421484, float_1_80421484, scale);
		PSMTXConcat(mtxC0, mtx90, mtxC0);
		PSMTXScale(mtx90, data->field_0x18, data->field_0x20, data->field_0x18);
		PSMTXConcat(mtxC0, mtx90, mtx90);
		GXLoadPosMtxImm(mtx90, 0);
		GXSetCurrentMtx(0);
		PSMTXTrans(mtx90, data->field_0x28, float_0_80421478, float_0_80421478);
		GXLoadTexMtxImm(mtx90, 0x1e, GX_MTX2x4);

		if (type == 0) {
			GXClearVtxDesc();
			GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 9);
			GXSetArray(GX_VA_POS, funemizu_vertex_tbl, 6);
			GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
			GXSetArray(GX_VA_NRM, funemizu_normal_tbl, 3);
			GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
			GXSetArray(GX_VA_CLR0, funemizu_color0_tbl, 4);
			GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0xc);
			GXSetArray(GX_VA_TEX0, funemizu_texcoord0_tbl, 4);
			for (j = 0; j < 3; i++) {
				GXCallDisplayList(funemizu_dl_0_tbl[j], funemizu_dl_0_size_tbl[j] << 5);
			}
		}
		else {
			GXClearVtxDesc();
			GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 8);
			GXSetArray(GX_VA_POS, gor_fune_vertex_tbl, 6);
			GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
			GXSetArray(GX_VA_NRM, gor_fune_normal_tbl, 3);
			GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
			GXSetArray(GX_VA_CLR0, gor_fune_color0_tbl, 4);
			GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0xe);
			GXSetArray(GX_VA_TEX0, gor_fune_texcoord0_tbl, 4);
			for (j = 0; j < 2; i++) {
				GXCallDisplayList(gor_fune_dl_0_tbl[j], gor_fune_dl_0_size_tbl[j] << 5);
			}
		}

		PSMTXScale(mtx90, data->field_0x1C, data->field_0x24, data->field_0x1C);
		PSMTXConcat(mtxC0, mtx90, mtx90);
		GXLoadPosMtxImm(mtx90, 0);
		GXSetCurrentMtx(0);
		PSMTXTrans(mtx90, data->field_0x2C, float_0_80421478, float_0_80421478);
		GXLoadTexMtxImm(mtx90, 0x1e, GX_MTX2x4);

		if (type == 0) {
			GXClearVtxDesc();
			GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 9);
			GXSetArray(GX_VA_POS, funemizu_vertex_tbl, 6);
			GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
			GXSetArray(GX_VA_NRM, funemizu_normal_tbl, 3);
			GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
			GXSetArray(GX_VA_CLR0, funemizu_color0_tbl, 4);
			GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0xc);
			GXSetArray(GX_VA_TEX0, funemizu_texcoord0_tbl, 4);
			for (j = 0; j < 4; i++) {
				GXCallDisplayList(funemizu_dl_1_tbl[j], funemizu_dl_1_size_tbl[j] << 5);
			}
		}
		else {
			GXClearVtxDesc();
			GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 8);
			GXSetArray(GX_VA_POS, gor_fune_vertex_tbl, 6);
			GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
			GXSetArray(GX_VA_NRM, gor_fune_normal_tbl, 3);
			GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
			GXSetArray(GX_VA_CLR0, gor_fune_color0_tbl, 4);
			GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0xe);
			GXSetArray(GX_VA_TEX0, gor_fune_texcoord0_tbl, 4);
			for (j = 0; j < 2; i++) {
				GXCallDisplayList(gor_fune_dl_1_tbl[j], gor_fune_dl_1_size_tbl[j] << 5);
			}
		}
	}
}

// 100%: `data++` before the loop (rather than a separate `ptr`) and the GX colour helper below put the
// pointer init and the by-value GXColor in the original's slots.  The 0.0f/1.0f arguments are literals
// (not the named externs) so each GX write reloads them, as the original does.
#ifdef MUST_MATCH
// Match-only: building the colour through a helper lands the by-value GXColor in the original's stack
// slot; the in-place build under #else lands it one word lower.
static inline GXColor funemizuShibukiColor(u8 a) {
	GXColor c = *(GXColor*)&dat_8042146c;
	c.a = a;
	return c;
}
#endif

void effFunemizuShibukiDisp(CameraId camId, void* param) {
	EffectEntry* effect;
	EffFunemizuData* data;
	CameraEntry* cam;
	Mtx mtx90;
	Mtx mtx60;
	Mtx mtx30;
	GXColor color;
	GXTexObj texObj;
	s32 i;
	f32 sz;

	effect = param;
	data = effect->userdata;
	cam = camGetPtr(camId);

	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);

	PSMTXTrans(mtx90, data->x, data->y, data->z);
	PSMTXRotRad(mtx60, 'y',
	            float_deg2rad_80421470 * (data->field_0x14 - camGetPtr(camId)->viewYaw));
	PSMTXConcat(mtx90, mtx60, mtx90);
	PSMTXConcat(cam->view, mtx90, mtx30);

	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3c, GX_FALSE, GX_PTIDENTITY);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	effGetTexObj(0x40, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	sz = -float_3_80421474;
	i = 1;
	data++;
	for (; i < effect->count; i++, data++) {
		if (data->field_0x3C != 0) {
			continue;
		}
		if (data->y < float_0_80421478) {
			continue;
		}
#ifdef MUST_MATCH
		GXSetChanMatColor(GX_COLOR0A0, funemizuShibukiColor(data->field_0x40));
#else
		color = *(GXColor*)&dat_8042146c;
		color.a = data->field_0x40;
		GXSetChanMatColor(GX_COLOR0A0, color);
#endif
		PSMTXTrans(mtx90, data->x, data->y, data->z);
		PSMTXConcat(mtx30, mtx90, mtx90);
		GXLoadPosMtxImm(mtx90, 0);
		GXSetCurrentMtx(0);
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(sz * float_0p5_8042147c, float_1p5_80421480, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(float_1p5_80421480, float_1p5_80421480, 0.0f);
		GXTexCoord2f32(1.0f, 0.0f);
		GXPosition3f32(float_1p5_80421480, sz * float_0p5_8042147c, 0.0f);
		GXTexCoord2f32(1.0f, 1.0f);
		GXPosition3f32(sz * float_0p5_8042147c, sz * float_0p5_8042147c, 0.0f);
		GXTexCoord2f32(0.0f, 1.0f);
		GXEnd();
	}
}
