#include "eff/eff_funemizu.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "driver/camdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"
#include <dolphin/mtx.h>
#include <dolphin/gx.h>
#include "math_ext.h"
#include <string.h>

// Data objects defined later in this file or in other units, declared before their first use.
extern const Vec vec3_802fe8a0;
extern u32 funemizu_vertex_tbl[];
extern u32 funemizu_texcoord0_tbl[];
extern u32 gor_fune_vertex_tbl[];
extern u32 gor_fune_texcoord0_tbl[];

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") extern u32 lbl_803B0200[2];

// MUST_MATCH: the original reaches the unit's .data tables off one base register (&lbl_803B0200); the natural
// build names them.
#ifdef MUST_MATCH
#define FUNE_DATA(off, sym) ((void*)(base + (off)))
#else
#define FUNE_DATA(off, sym) ((void*)(sym))
#endif

typedef struct FunemizuMapEntry {
	const char* name;
	u32 value;
} FunemizuMapEntry;

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

extern FunemizuMapEntry unk_803b0640[];

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
	entry->type = "funemizu\0\0\0";
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

#ifdef MUST_MATCH
// Match-only: the original walks both display-list tables with pointer induction variables, the size table
// stepping 1 and the pointer table 4; the indexed form under #else is the natural equivalent.
#define FUNEMIZU_DL_LOOP(n, tbl, sztbl) \
	{ \
		u8* sz; \
		u8** dl; \
		j = 0; \
		sz = (sztbl); \
		dl = (u8**)(tbl); \
		for (; j < (n); j++, sz++, dl++) { \
			GXCallDisplayList(*dl, *sz << 5); \
		} \
	}
#define FUNEMIZU_DL_LOOP_H(n, tbl, sztbl) \
	{ \
		j = 0; \
		sz = (sztbl); \
		dl = (u8**)(tbl); \
		for (; j < (n); j++, sz++, dl++) { \
			GXCallDisplayList(*dl, *sz << 5); \
		} \
	}
#else
#define FUNEMIZU_DL_LOOP(n, tbl, sztbl) \
	for (j = 0; j < (n); j++) { \
		GXCallDisplayList((tbl)[j], (sztbl)[j] << 5); \
	}
#define FUNEMIZU_DL_LOOP_H(n, tbl, sztbl) FUNEMIZU_DL_LOOP(n, tbl, sztbl)
#endif

// The `.sdata` addressing below comes from the `__declspec(section ".sdata")` on the display-list/colour/
// normal table externs in include/eff/eff_funemizu.h; without it MWCC emits lis/addi for them.
// A second pointer variable coalesces back to r26, and keeping the first pointer live across the second
// walk (comma/&& in the condition, or shared/inlined helpers) makes mwcceppc 2.6 loop forever, so r24 is
// left to the permuter.
// NONMATCHING (99.94%): the map-lookup's two walks match except the second walk's pointer, which the
// original keeps in r24 while MWCC reuses r26 for both (the first walk is r26). Everything else - the
// .data base+offset addressing, the .sdata (r13) addressing and the display-list loop registers - matches.
#pragma opt_lifetimes off
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
	GXColor color;
	u32 colorVal;
	s32 type;
	u32 j;
	s32 i;
	u8* sz;
	u8** dl;
	f32 zero;
	f32 z;
	f32 oneR;
#ifdef MUST_MATCH
	u8* base = (u8*)lbl_803B0200; // first object of the unit's .data; the tables below are reached as base+offset
#endif

	effect = param;
	data = effect->userdata;
	cam = camGetPtr(camId);
	type = data->type;

	for (p = (FunemizuMapEntry*)FUNE_DATA(0x440, unk_803b0640); p; p++) {
		if (p->name == NULL) {
			break;
		}
		if (strcmp(p->name, gp->currentMapName) == 0) {
			colorVal = p->value;
			goto found;
		}
	}
	for (dl = (u8**)FUNE_DATA(0x440, unk_803b0640); dl; dl += 2) {
		if (((FunemizuMapEntry*)dl)->name == NULL) {
			break;
		}
		if (strncmp(((FunemizuMapEntry*)dl)->name, gp->currentMapName, 3) == 0) {
			colorVal = ((FunemizuMapEntry*)dl)->value;
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
	zero = float_0_80421478;
	PSMTXTrans(mtx60, float_neg15_80421488, zero, zero);
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
		oneR = float_0_80421478;
		PSMTXTrans(mtxC0, oneR, oneR, scale);
		PSMTXConcat(mtx30, mtxC0, mtxC0);
		oneR = *(volatile f32*)&float_1_80421484;
		PSMTXScale(mtx90, oneR, oneR, scale);
		PSMTXConcat(mtxC0, mtx90, mtxC0);
		PSMTXScale(mtx90, data->field_0x18, data->field_0x20, data->field_0x18);
		PSMTXConcat(mtxC0, mtx90, mtx90);
		GXLoadPosMtxImm(mtx90, 0);
		GXSetCurrentMtx(0);
		zero = float_0_80421478;
		PSMTXTrans(mtx90, data->field_0x28, zero, zero);
		GXLoadTexMtxImm(mtx90, 0x1e, GX_MTX2x4);

		if (type == 0) {
			GXClearVtxDesc();
			GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 9);
			GXSetArray(GX_VA_POS, FUNE_DATA(0x40, funemizu_vertex_tbl), 6);
			GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
			GXSetArray(GX_VA_NRM, funemizu_normal_tbl, 3);
			GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
			GXSetArray(GX_VA_CLR0, funemizu_color0_tbl, 4);
			GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0xc);
			GXSetArray(GX_VA_TEX0, FUNE_DATA(0x100, funemizu_texcoord0_tbl), 4);
			FUNEMIZU_DL_LOOP_H(3, funemizu_dl_0_tbl, funemizu_dl_0_size_tbl);
		}
		else {
			GXClearVtxDesc();
			GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 8);
			GXSetArray(GX_VA_POS, FUNE_DATA(0x200, gor_fune_vertex_tbl), 6);
			GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
			GXSetArray(GX_VA_NRM, gor_fune_normal_tbl, 3);
			GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
			GXSetArray(GX_VA_CLR0, gor_fune_color0_tbl, 4);
			GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0xe);
			GXSetArray(GX_VA_TEX0, FUNE_DATA(0x2e0, gor_fune_texcoord0_tbl), 4);
			FUNEMIZU_DL_LOOP(2, gor_fune_dl_0_tbl, gor_fune_dl_0_size_tbl);
		}

		PSMTXScale(mtx90, data->field_0x1C, data->field_0x24, data->field_0x1C);
		PSMTXConcat(mtxC0, mtx90, mtx90);
		GXLoadPosMtxImm(mtx90, 0);
		GXSetCurrentMtx(0);
		zero = float_0_80421478;
		PSMTXTrans(mtx90, data->field_0x2C, zero, zero);
		GXLoadTexMtxImm(mtx90, 0x1e, GX_MTX2x4);

		if (type == 0) {
			GXClearVtxDesc();
			GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 9);
			GXSetArray(GX_VA_POS, FUNE_DATA(0x40, funemizu_vertex_tbl), 6);
			GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
			GXSetArray(GX_VA_NRM, funemizu_normal_tbl, 3);
			GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
			GXSetArray(GX_VA_CLR0, funemizu_color0_tbl, 4);
			GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0xc);
			GXSetArray(GX_VA_TEX0, FUNE_DATA(0x100, funemizu_texcoord0_tbl), 4);
			FUNEMIZU_DL_LOOP(4, funemizu_dl_1_tbl, funemizu_dl_1_size_tbl);
		}
		else {
			GXClearVtxDesc();
			GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 8);
			GXSetArray(GX_VA_POS, FUNE_DATA(0x200, gor_fune_vertex_tbl), 6);
			GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
			GXSetArray(GX_VA_NRM, gor_fune_normal_tbl, 3);
			GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
			GXSetArray(GX_VA_CLR0, gor_fune_color0_tbl, 4);
			GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0xe);
			GXSetArray(GX_VA_TEX0, FUNE_DATA(0x2e0, gor_fune_texcoord0_tbl), 4);
			FUNEMIZU_DL_LOOP(2, gor_fune_dl_1_tbl, gor_fune_dl_1_size_tbl);
		}
	}
}

#pragma opt_lifetimes reset

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
		{
			f32 one5 = float_1p5_80421480;
			f32 half = sz * float_0p5_8042147c;

			GXWGFifo.f32 = half;
			GXWGFifo.f32 = one5;
			GXWGFifo.f32 = *(volatile f32*)&float_0_80421478;
			GXWGFifo.f32 = *(volatile f32*)&float_0_80421478;
			GXWGFifo.f32 = *(volatile f32*)&float_0_80421478;
			GXWGFifo.f32 = one5;
			GXWGFifo.f32 = one5;
			GXWGFifo.f32 = *(volatile f32*)&float_0_80421478;
			GXWGFifo.f32 = *(volatile f32*)&float_1_80421484;
			GXWGFifo.f32 = *(volatile f32*)&float_0_80421478;
			GXWGFifo.f32 = one5;
			GXWGFifo.f32 = half;
			GXWGFifo.f32 = *(volatile f32*)&float_0_80421478;
			GXWGFifo.f32 = *(volatile f32*)&float_1_80421484;
			GXWGFifo.f32 = *(volatile f32*)&float_1_80421484;
			GXWGFifo.f32 = half;
			GXWGFifo.f32 = half;
			GXWGFifo.f32 = *(volatile f32*)&float_0_80421478;
			GXWGFifo.f32 = *(volatile f32*)&float_0_80421478;
			GXWGFifo.f32 = *(volatile f32*)&float_1_80421484;
		}
		GXEnd();
	}
}

// ---------------------------------------------------------------------------
// Data ported from the original unit's .data/.sdata/.rodata/.sdata2.
// The map-lookup strings are .rodata objects referenced by unk_803b0640, so
// they are named here rather than left as string literals in that table.
// ---------------------------------------------------------------------------

// .rodata: the map-name strings, in the original's address order.
const char str_802FE7A0[8] = "dou_02";
const char str_802FE7A8[8] = "dou_03";
const char str_802FE7B0[8] = "dou_04";
const char str_802FE7B8[8] = "dou_05";
const char str_802FE7C0[8] = "dou_07";
const char str_802FE7C8[8] = "dou_08";
const char str_802FE7D0[8] = "dou_09";
const char str_802FE7D8[8] = "dou_10";
const char str_802FE7E0[8] = "dou_11";
const char str_802FE7E8[8] = "dou_12";
const char str_802FE7F0[8] = "gon_01";
const char str_802FE7F8[8] = "gor_00";
const char str_802FE800[8] = "gor_02";
const char str_802FE808[8] = "hei_01";
const char str_802FE810[8] = "las_04";
const char str_802FE818[8] = "las_09";
const char str_802FE820[8] = "mri_16";
const char str_802FE828[8] = "muj_00";
const char str_802FE830[8] = "muj_02";
const char str_802FE838[8] = "muj_04";
const char str_802FE840[8] = "muj_10";
const char str_802FE848[8] = "nok_00";
const char str_802FE850[8] = "nok_01";
const char str_802FE858[8] = "pik_00";
const char str_802FE860[8] = "pik_01";
const char str_802FE868[8] = "pik_04";
const char str_802FE870[8] = "tik_02";
const char str_802FE878[8] = "tik_15";
const char str_802FE880[8] = "tik_16";
const char str_802FE888[8] = "tik_17";
const char str_802FE890[8] = "tik_18";
const char str_802FE898[8] = "tik_20";
const Vec vec3_802fe8a0 = { 0.0f, 0.0f, 0.0f };

#pragma force_active on
#pragma explicit_zero_data on

// .data: the tables reached through lbl_803B0200, in the original's address order.
u32 lbl_803B0200[2] = {
	0xBF800000, 0x3F800000,
};
__declspec(section ".data") static u32 negone_one_374[2] = {
	0xBF800000, 0x3F800000,
};
__declspec(section ".data") u32 lbl_803B0210[2] = {
	0x3F800000, 0xBF800000,
};
__declspec(section ".data") static u32 one_negone_373[2] = {
	0x3F800000, 0xBF800000,
};
__declspec(section ".data") u32 lbl_803B0220[2] = {
	0xBF800000, 0x3F800000,
};
__declspec(section ".data") static u32 negone_one_360[2] = {
	0xBF800000, 0x3F800000,
};
__declspec(section ".data") u32 lbl_803B0230[2] = {
	0x3F800000, 0xBF800000,
};
__declspec(section ".data") static u32 one_negone_359[2] = {
	0x3F800000, 0xBF800000,
};
u32 funemizu_vertex_tbl[48] = {
	0x4E6FFD84, 0x000035C6, 0x00000000, 0x22C60000, 0x000015C6, 0x00000000, 0x00000000, 0x000035C6,
	0x06000000, 0x22C60A00, 0x000015C6, 0x0A000000, 0x02000000, 0x000046EA, 0x03830000, 0x35C60D00,
	0x000022C6, 0x15000000, 0x15C61570, 0x000008C6, 0x0BA60000, 0x38CEFE74, 0x04E12720, 0x000004E1,
	0x19800000, 0x04E1102E, 0x000004E1, 0x02000000, 0x04E12720, 0x03BD04E1, 0x1980063C, 0x04E1102E,
	0x063C04E1, 0x33690230, 0x04E12720, 0x081A04E1, 0x19800D17, 0x04E1102E, 0x0D5E04E1, 0x06DB0743,
	0x04E10000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
u32 funemizu_texcoord0_tbl[32] = {
	0x04001000, 0x04000800, 0x00000C00, 0x08001000, 0x08000800, 0x0C001000, 0x0C000800, 0x10001000,
	0x10000800, 0x04000100, 0x00000100, 0x08000100, 0x0C000100, 0x10000100, 0xB4001000, 0xB4000800,
	0xB0000C00, 0xB8001000, 0xB8000800, 0xBC001000, 0xBC000800, 0xC0000C00, 0xB4000100, 0xB0000100,
	0xB8000100, 0xBC000100, 0xC0000100, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
u32 funemizu_dl_0_0[16] = {
	0x98000A09, 0x00010A00, 0x0000020A, 0x00010905, 0x0001010B, 0x00010B06, 0x0001040C, 0x00010C07,
	0x0001060D, 0x00010D08, 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
u32 funemizu_dl_1_0[16] = {
	0x98000A16, 0x0001170E, 0x00001017, 0x00011613, 0x00010F18, 0x00011814, 0x00011219, 0x00011915,
	0x0001141A, 0x00011A12, 0x00001500, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
u32 gor_fune_vertex_tbl[56] = {
	0x5C350000, 0x00003FF0, 0x00000000, 0x2F310000, 0x00001FF8, 0x00000000, 0x10BF0000, 0x00000000,
	0x00000000, 0x5E86014D, 0x0000455A, 0x0CDB0000, 0x363E19A5, 0x00002853, 0x23400000, 0x18FE2462,
	0x00000000, 0x00000000, 0x5E4B0800, 0x0000470E, 0x12390000, 0x388320CD, 0x00002AE1, 0x2B4C0000,
	0x1BD42D53, 0x000008AF, 0x262E0000, 0x5C350000, 0x00003FF0, 0x00000000, 0x2F310000, 0x00001FF8,
	0x00000000, 0x10BF0000, 0x00000000, 0x00000000, 0x5E8600C8, 0x0000455A, 0x07B60000, 0x363E11C9,
	0x00002853, 0x19260000, 0x18FE19D4, 0x00000000, 0x00000000, 0x5E4B04CD, 0x0000470E, 0x0AEF0000,
	0x38831614, 0x00002AE1, 0x1DFA0000, 0x1BD41F32, 0x00000B96, 0x19160000, 0x00000000, 0x00000000,
};
u32 gor_fune_texcoord0_tbl[24] = {
	0xF3034000, 0xF3031884, 0xD5041884, 0xD5044000, 0x11004000, 0x11001884, 0x2EFF4000, 0x2EFF1884,
	0x4CFD4000, 0x4CFD1884, 0x6AFC4000, 0x6AFC1884, 0xF3030400, 0xD5040200, 0x11000400, 0x2EFF0400,
	0x4CFD0400, 0x6AFC0200, 0xF3033CCC, 0xD5043CCC, 0x11003CCC, 0x2EFF3CCC, 0x4CFD3CCC, 0x6AFC3CCC,
};
u32 gor_fune_dl_0_0[16] = {
	0x98000C0C, 0x00010D06, 0x0000020D, 0x00010C07, 0x0001010E, 0x00010E08, 0x0001050F, 0x00010F09,
	0x00010710, 0x0001100A, 0x00010911, 0x0001110B, 0x00000B00, 0x00000000, 0x00000000, 0x00000000,
};
u32 gor_fune_dl_0_1[16] = {
	0x98000C05, 0x00000A0B, 0x00000B04, 0x0000080A, 0x00010903, 0x00000609, 0x00010702, 0x00000408,
	0x00010501, 0x00000007, 0x00010100, 0x00000306, 0x00000200, 0x00000000, 0x00000000, 0x00000000,
};
u32 gor_fune_dl_1_0[16] = {
	0x98000C1E, 0x00010D18, 0x0000021F, 0x00010C19, 0x00010120, 0x00010E1A, 0x00010521, 0x00010F1B,
	0x00010722, 0x0001101C, 0x00010923, 0x0001111D, 0x00000B00, 0x00000000, 0x00000000, 0x00000000,
};
u32 gor_fune_dl_1_1[16] = {
	0x98000C17, 0x0000171D, 0x00000B16, 0x0000161C, 0x00010915, 0x0000151B, 0x00010714, 0x0000141A,
	0x00010513, 0x00001219, 0x00010112, 0x00001318, 0x00000200, 0x00000000, 0x00000000, 0x00000000,
};
FunemizuMapEntry unk_803b0640[32] = {
	{ str_802FE7A0, 0x0E2CA200 },
	{ str_802FE7A8, 0x0E2CA200 },
	{ str_802FE7B0, 0x0E2CA200 },
	{ str_802FE7B8, 0x0E2CA200 },
	{ str_802FE7C0, 0x0E2CA200 },
	{ str_802FE7C8, 0x0E2CA200 },
	{ str_802FE7D0, 0x0E2CA200 },
	{ str_802FE7D8, 0x0E2CA200 },
	{ str_802FE7E0, 0x0E2CA200 },
	{ str_802FE7E8, 0x0E2CA200 },
	{ str_802FE7F0, 0x00614900 },
	{ str_802FE7F8, 0x00306500 },
	{ str_802FE800, 0x00306500 },
	{ str_802FE808, 0x51C3CB00 },
	{ str_802FE810, 0x081C2C00 },
	{ str_802FE818, 0x45798E00 },
	{ str_802FE820, 0x00A6FF00 },
	{ str_802FE828, 0x0E2CA200 },
	{ str_802FE830, 0x20305D00 },
	{ str_802FE838, 0x0E2CA200 },
	{ str_802FE840, 0x0E2CA200 },
	{ str_802FE848, 0x69C3D700 },
	{ str_802FE850, 0x69C3D700 },
	{ str_802FE858, 0x82E3DF00 },
	{ str_802FE860, 0x6DCBFF00 },
	{ str_802FE868, 0x6DCBFF00 },
	{ str_802FE870, 0x042C5100 },
	{ str_802FE878, 0x304D5900 },
	{ str_802FE880, 0x304D5900 },
	{ str_802FE888, 0x304D5900 },
	{ str_802FE890, 0x304D5900 },
	{ str_802FE898, 0x304D5900 },
};
__declspec(section ".data") u32 unk_803b0640_end[1] = { 0 };

// .sdata: the display-list/colour/normal tables, in the original's address order.
u8 funemizu_normal_tbl[32] = {
	0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
__declspec(section ".sdata") u8 funemizu_color0_tbl[32] = {
	0, 0, 0, 255, 255, 255, 255, 255,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
};
__declspec(section ".sdata") u32 funemizu_dl_0_1[8] = {
	0x98000704, 0x00000708, 0x00000803, 0x00000507, 0x00010602, 0x00000306, 0x00010401, 0x00000000,
};
__declspec(section ".sdata") u32 funemizu_dl_0_2[8] = {
	0x98000400, 0x00000201, 0x00000005, 0x00010106, 0x00010400, 0x00000000, 0x00000000, 0x00000000,
};
__declspec(section ".sdata") u8* funemizu_dl_0_tbl[3] = {
	(u8*)funemizu_dl_0_0, (u8*)funemizu_dl_0_1, (u8*)funemizu_dl_0_2,
};
__declspec(section ".sdata") u8 funemizu_dl_0_size_tbl[4] = {
	2, 1, 1, 0,
};
__declspec(section ".sdata") u32 gap_07_804162F0_sdata[4] = { 0, 0, 0, 0 };
__declspec(section ".sdata") u32 funemizu_dl_1_1[8] = {
	0x98000412, 0x00001515, 0x00011411, 0x00001310, 0x00001100, 0x00000000, 0x00000000, 0x00000000,
};
__declspec(section ".sdata") u32 funemizu_dl_1_2[8] = {
	0x9800040E, 0x0000100F, 0x00000E13, 0x00010F14, 0x00011200, 0x00000000, 0x00000000, 0x00000000,
};
__declspec(section ".sdata") u32 funemizu_dl_1_3[8] = {
	0x98000415, 0x00011414, 0x00011210, 0x0000110F, 0x00000E00, 0x00000000, 0x00000000, 0x00000000,
};
__declspec(section ".sdata") u8* funemizu_dl_1_tbl[4] = {
	(u8*)funemizu_dl_1_0, (u8*)funemizu_dl_1_1, (u8*)funemizu_dl_1_2, (u8*)funemizu_dl_1_3,
};
__declspec(section ".sdata") u8 funemizu_dl_1_size_tbl[4] = {
	2, 1, 1, 1,
};
__declspec(section ".sdata") u32 gap_07_80416374_sdata[3] = { 0, 0, 0 };
u8 gor_fune_normal_tbl[32] = {
	0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
__declspec(section ".sdata") u8 gor_fune_color0_tbl[8] = {
	0, 0, 0, 255, 255, 255, 255, 255,
};
__declspec(section ".sdata") u8* gor_fune_dl_0_tbl[2] = {
	(u8*)gor_fune_dl_0_0, (u8*)gor_fune_dl_0_1,
};
__declspec(section ".sdata") u8 gor_fune_dl_0_size_tbl[4] = {
	2, 2, 0, 0,
};
__declspec(section ".sdata") u8* gor_fune_dl_1_tbl[2] = {
	(u8*)gor_fune_dl_1_0, (u8*)gor_fune_dl_1_1,
};
__declspec(section ".sdata") u8 gor_fune_dl_1_size_tbl[4] = {
	2, 2, 0, 0,
};

#pragma force_active reset
#pragma explicit_zero_data reset

// .sdata2: the named float/word constants, in the original's address order.
const s32 dat_80421468 = -1;
const u32 dat_8042146c = 0xFFFFFF00;
const f32 float_deg2rad_80421470 = 0.017453292f;
const f32 float_3_80421474 = 3.0f;
const f32 float_0_80421478 = 0.0f;
const f32 float_0p5_8042147c = 0.5f;
const f32 float_1p5_80421480 = 1.5f;
const f32 float_1_80421484 = 1.0f;
const f32 float_neg15_80421488 = -15.0f;
const f32 float_2_8042148c = 2.0f;
const f32 float_10_80421490 = 10.0f;
const f32 float_0p05_80421494 = 0.05f;
const f32 float_0p1_80421498 = 0.1f;
const f32 float_6p2832_8042149c = 6.2831855f;
const f32 float_360_804214a0 = 360.0f;
const f32 float_14_804214a4 = 14.0f;
const f32 float_100_804214a8 = 100.0f;


