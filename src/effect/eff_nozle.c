#include "eff/eff_nozle.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_kemutest.h"
#include "memory.h"

#include <dolphin/mtx.h>
#include <dolphin/gx.h>
#include "evt/evt_cmd.h"

// First object of this unit's .data (the float pair that used to end the previous unit's .data).

typedef struct effNozleEntryData {
	f32 field_0x00; // 0x00
	f32 offsetX;    // 0x04, petal offset, decays by 1/300 per frame
	f32 offsetY;    // 0x08
	f32 offsetZ;    // 0x0C, 20 * index
	f32 scaleX;     // 0x10, +1 or -1 by index parity
	f32 scaleY;     // 0x14, eases towards 1.3, then 0.8 once shrinking
	f32 scrollX0;   // 0x18, texture 0 translation
	f32 scrollY0;   // 0x1C, +0.002 per frame
	f32 scrollX1;   // 0x20, texture 1 translation
	f32 scrollY1;   // 0x24, +0.004 per frame
	f32 field_0x28; // 0x28
	f32 field_0x2C; // 0x2C
	f32 field_0x30; // 0x30
	s32 field_0x34; // 0x34, set to 0xFF, never read (the head's alpha slot; Disp uses the head alpha)
	s32 shrinking;  // 0x38, set to 1 once scaleY passes 1.2
	f32 field_0x3C; // 0x3C
	f32 field_0x40; // 0x40
} effNozleEntryData;

typedef struct effNozleData {
	s32 type;                    // 0x00
	Vec position;                // 0x04
	f32 scaleX;                  // 0x10
	f32 scaleY;                  // 0x14
	f32 scrollX0;                // 0x18, texture 0 translation (x)
	f32 scrollY0;                // 0x1C, texture 0 translation (-y), -1/24 per frame
	f32 scrollX1;                // 0x20, texture 1 translation (x)
	f32 scrollY1;                // 0x24, texture 1 translation (-y), -1/12 per frame
	f32 scale;                   // 0x28
	s32 lifetime;                // 0x2C
	s32 counter;                 // 0x30
	s32 alpha;                   // 0x34
	f32 field_0x38;              // 0x38
	f32 rotX;                    // 0x3C, degrees
	f32 rotY;                    // 0x40, degrees
	effNozleEntryData entries[]; // 0x44
} effNozleData;

// The original reaches this file's tables from its .data section anchor, lbl_803AE260.
u32 lbl_803AE260[2];
static u32 negone_one_374[2];
u32 lbl_803AE270[2];
static u32 one_negone_373[2];
u32 lbl_803AE280[2];
static u32 negone_one_360[2];
u32 lbl_803AE290[2];
static u32 one_negone_359[2];
u32 nozle_1_vertex_tbl[112];
u32 nozle_1_texcoord0_tbl[56];
u32 nozle_1_texcoord1_tbl[56];
u32 nozle_1_dl_0_0[16];
u32 nozle_1_dl_0_1[16];
u32 nozle_1_dl_0_2[16];
u32 nozle_1_dl_0_3[16];
u32 nozle_1_dl_0_4[16];
u32 nozle_1_dl_1_0[16];
u32 nozle_1_dl_1_1[16];
u32 nozle_1_dl_1_2[16];
u32 nozle_1_dl_1_3[16];
u32 nozle_1_dl_1_4[16];
u32 nozle_3_vertex_tbl[160];
u32 nozle_3_texcoord0_tbl[96];
u32 nozle_3_texcoord1_tbl[96];
u32 nozle_3_color0_tbl[32];
u32 nozle_3_dl_0_0[48];
u32 nozle_3_dl_0_1[48];
u32 nozle_3_dl_0_2[40];
u32 nozle_3_dl_0_3[24];
u32 nozle_3_dl_0_4[16];
u32 nozle_3_dl_0_5[16];
u32 nozle_3_dl_0_6[16];
u32 nozle_3_dl_0_7[16];
u32 nozle_3_dl_0_8[16];
u32 nozle_3_dl_0_9[16];
u32 nozle_3_dl_0_10[16];
u32 nozle_3_dl_0_11[16];
u32 nozle_3_dl_0_12[16];
u32 nozle_3_dl_0_13[16];
u32 nozle_3_dl_0_14[16];
u32 nozle_3_dl_0_15[16];
u32 nozle_3_dl_0_16[16];
u32 nozle_3_dl_0_tbl[17];
u32 nozle_3_dl_1_0[48];
u32 nozle_3_dl_1_1[48];
u32 nozle_3_dl_1_2[40];
u32 nozle_3_dl_1_3[24];
u32 nozle_3_dl_1_4[16];
u32 nozle_3_dl_1_5[16];
u32 nozle_3_dl_1_6[16];
u32 nozle_3_dl_1_7[16];
u32 nozle_3_dl_1_8[16];
u32 nozle_3_dl_1_9[16];
u32 nozle_3_dl_1_10[16];
u32 nozle_3_dl_1_11[16];
u32 nozle_3_dl_1_12[16];
u32 nozle_3_dl_1_13[16];
u32 nozle_3_dl_1_14[16];
u32 nozle_3_dl_1_15[16];
u32 nozle_3_dl_1_16[16];
u32 nozle_3_dl_1_tbl[17];
u32 nozle_4_vertex_tbl[48];
u32 nozle_4_texcoord0_tbl[32];
u32 nozle_4_texcoord1_tbl[32];
u32 nozle_4_dl_0_0[16];
u32 nozle_4_dl_0_1[16];
u32 nozle_4_dl_0_2[16];
u32 nozle_4_dl_0_3[16];
u32 nozle_4_dl_0_4[16];
u32 nozle_4_dl_0_5[16];
u32 nozle_4_dl_0_6[16];
u32 nozle_4_dl_0_7[16];
u32 nozle_1_normal_tbl[8];
u32 nozle_1_color0_tbl[8];
u32 nozle_1_dl_0_5[8];
u32 nozle_1_dl_0_6[8];
u32 nozle_1_dl_0_7[8];
u32 nozle_1_dl_0_8[8];
u32 nozle_1_dl_0_9[8];
u8 nozle_1_dl_0_size_tbl[12];
u32 nozle_1_dl_1_5[8];
u32 nozle_1_dl_1_6[8];
u32 nozle_1_dl_1_7[8];
u32 nozle_1_dl_1_8[8];
u32 nozle_1_dl_1_9[8];
u8 nozle_1_dl_1_size_tbl[12];
u32 nozle_3_normal_tbl[1];
u8 nozle_3_dl_0_size_tbl[20];
u8 nozle_3_dl_1_size_tbl[20];
u32 nozle_4_normal_tbl[8];
u32 nozle_4_color0_tbl[3];
u8 nozle_4_dl_0_size_tbl[8];


static u8 scale_dt[44];

extern s32 negone_one[];
extern s32 one_negone[];
u32 nozle_1_dl_0_tbl[10];
u32 nozle_1_dl_1_tbl[10];
u32 nozle_4_dl_0_tbl[8];

// clang-format off

// The original has each display-list loop in its own inline helper; with the loops inlined at
// their call sites MWCC colours the function's pointers differently and rotates every loop
// pointer.  The helpers leave the whole function matching
// except `data`, which lands in r28 where the original keeps it in r27.
static inline void nozleDrawDL1_0(void) {
	u32 j;
	for (j = 0; j < 10; j++) {
		GXCallDisplayList((void*)nozle_1_dl_0_tbl[j], nozle_1_dl_0_size_tbl[j] << 5);
	}
}

static inline void nozleDrawDL1_1(void) {
	u32 j;
	for (j = 0; j < 10; j++) {
		GXCallDisplayList((void*)nozle_1_dl_1_tbl[j], nozle_1_dl_1_size_tbl[j] << 5);
	}
}

static inline void nozleDrawDL3_1(void) {
	u32 j;
	for (j = 0; j < 0x11; j++) {
		GXCallDisplayList((void*)nozle_3_dl_1_tbl[j], nozle_3_dl_1_size_tbl[j] << 5);
	}
}

static inline void nozleDrawDL3_0(void) {
	u32 j;
	for (j = 0; j < 0x11; j++) {
		GXCallDisplayList((void*)nozle_3_dl_0_tbl[j], nozle_3_dl_0_size_tbl[j] << 5);
	}
}

static inline void nozleDrawDL4_0(void) {
	u32 j;
	for (j = 0; j < 8; j++) {
		GXCallDisplayList((void*)nozle_4_dl_0_tbl[j], nozle_4_dl_0_size_tbl[j] << 5);
	}
}

EffectEntry* effNozleEntry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 rotX, f32 rotY) {
	EffectEntry* effect;
	effNozleData* data;
	effNozleEntryData* entry;
	s32 i;
	s32 dir;
	s32 count;

	if (type == 1) {
		return effKemuTestEntry(3, x, y, z, 1.0f);
	}

	effect = effEntry();
	count = 1;
	switch (type) {
	case 0:
	case 1:
	case 2:
		count = 1;
		break;
	case 3:
		count = 4;
		break;
	}
	effect->type = "Nozle";
	effect->count = count;
	data = __memAlloc(HEAP_EFFECT, count * sizeof(effNozleEntryData));
	effect->userdata = data;
	effect->callback = effNozleMain;
	effect->flags |= 2;

	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->scaleX = (type == 3) ? 1.0f : 0.0f;
	data->scaleY = (type == 3) ? 1.0f : 0.0f;
	data->scale = 1.0f;
	data->scrollY0 = 0.0f;
	data->scrollX0 = 0.0f;
	data->scrollY1 = 0.0f;
	data->scrollX1 = 0.0f;
	if (duration <= 0) {
		data->lifetime = 1000;
	} else {
		data->lifetime = duration;
	}
	data->counter = 0;
	data->alpha = 0xFF;
	data->rotX = rotX;
	data->rotY = rotY;

	entry = data->entries;
	for (i = 1; i < effect->count; i++) {
		dir = (i & 1) ? -1 : 1;
		entry->offsetX = (f32)(dir * 400);
		entry->offsetY = 0.0f;
		entry->offsetZ = 20.f * (f32)i;
		entry->scaleX = (f32)((i & 1) ? 1 : -1);
		entry->scaleY = 1.0f;
		entry->scrollY0 = 0.0f;
		entry->scrollX0 = 0.0f;
		entry->scrollY1 = 0.0f;
		entry->scrollX1 = 0.0f;
		entry->field_0x34 = 0xFF;
		entry->shrinking = 0;
		entry++;
	}
	return effect;
}


void effNozleMain(EffectEntry* effect) {
	effNozleData* data = effect->userdata;
	effNozleEntryData* entry;
	s32 type;
	s32 i;
	Vec pos;
	Vec out;

	out = (Vec){ data->position.x, data->position.y, data->position.z };
	pos = out;

	type = data->type;
	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 0x20;
	}

	switch (type) {
	case 0:
	case 2:
		if ((u32)data->counter < 0x15) {
			data->scaleX = (f32)scale_dt[data->counter * 2] / 100.f;
			data->scaleY = (f32)scale_dt[data->counter * 2 + 1] / 100.f;
		}
		if ((u32)data->lifetime < 0x15) {
			data->scaleX = (f32)scale_dt[data->lifetime * 2] / 100.f;
			data->scaleY = (f32)scale_dt[data->lifetime * 2 + 1] / 100.f;
		}
		data->scrollY0 += -0.041666668f;
		data->scrollY1 += -0.083333336f;
		break;
	case 3:
	{
		// Retail reloads userdata, steps it once (entry++) past the 0x44-byte head into the entries,
		// and reloads it again at the join below.
		entry = effect->userdata;
		entry++;
		for (i = 1; i < effect->count; i++, entry++) {
			entry->offsetX += -entry->offsetX / 300.f;
			if (entry->shrinking == 0) {
				entry->scaleY += (1.3f - entry->scaleY) / 500.f;
				if (entry->scaleY > 1.2f) {
					entry->shrinking = 1;
				}
			} else {
				entry->scaleY += (0.8f - entry->scaleY) / 300.f;
			}
			entry->scrollY0 += 0.002f;
			entry->scrollX1 += 0.002f;
			entry->scrollY1 += 0.004f;
		}
		break;
	}
	}

	data = effect->userdata;
	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	if (data->lifetime < 0x10) {
		data->alpha = data->lifetime << 4;
	}
	data->counter++;
	if (data->lifetime < 0) {
		effDelete(effect);
	} else if (type == 3) {
		dispEntry(CAMERA_3D, dispCalcZ(&pos), 8, effNozleDisp, effect);
	} else {
		dispEntry(CAMERA_3D, dispCalcZ(&pos), 2, effNozleDisp, effect);
	}
}


void effNozleDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	// `entry` is declared before `data` on purpose: MWCC allocates `data` to r27 and reuses r27
	// for `entry` in the type-3 loop, and swapping the declarations is what makes it do that.
	effNozleEntryData* entry;
	effNozleData* data;
	s32 i;
	CameraEntry* camera;
	s32 alpha;
	GXTexObj tex;
	Mtx transMtx;
	Mtx rotMtx;
	Mtx scaleMtx;
	Mtx mtx;

	data = effect->userdata;
	camera = camGetPtr(camId);
	alpha = data->alpha;
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);

	PSMTXTrans(transMtx, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(rotMtx, 'y', 0.017453292f * data->rotY);
	PSMTXConcat(transMtx, rotMtx, transMtx);
	PSMTXRotRad(rotMtx, 'x', 0.017453292f * data->rotX);
	PSMTXConcat(transMtx, rotMtx, transMtx);
	PSMTXScale(scaleMtx, data->scale * data->scaleX, data->scale * data->scaleY, data->scale);
	PSMTXConcat(transMtx, scaleMtx, transMtx);
	PSMTXConcat(camera->view, transMtx, mtx);
	GXLoadPosMtxImm(mtx, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);

	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);

	PSMTXTrans(transMtx, data->scrollX0, -data->scrollY0, 0.0f);
	GXLoadTexMtxImm(transMtx, GX_TEXMTX0, GX_MTX2x4);
	PSMTXTrans(transMtx, data->scrollX1, -data->scrollY1, 0.0f);
	GXLoadTexMtxImm(transMtx, GX_TEXMTX1, GX_MTX2x4);

	effGetTexObj(0x3C, &tex);
	GXLoadTexObj(&tex, GX_TEXMAP0);
	switch (data->type) {
	case 0:
	case 3:
		effGetTexObj(0x3D, &tex);
		GXLoadTexObj(&tex, GX_TEXMAP1);
		break;
	case 2:
		effGetTexObj(0x3E, &tex);
		GXLoadTexObj(&tex, GX_TEXMAP1);
		break;
	}

	GXSetNumTevStages(3);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP1, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXA, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_TEXA, GX_CA_ZERO);
	GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);
	GXSetCullMode(GX_CULL_NONE);

	switch (data->type) {
	case 0:
		GXClearVtxDesc();
		GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 7);
		GXSetArray(GX_VA_POS, nozle_1_vertex_tbl, 6);
		GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
		GXSetArray(GX_VA_NRM, nozle_1_normal_tbl, 3);
		GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
		GXSetArray(GX_VA_CLR0, nozle_1_color0_tbl, 4);
		GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0xE);
		GXSetArray(GX_VA_TEX0, nozle_1_texcoord0_tbl, 4);
		GXSetVtxDesc(GX_VA_TEX1, GX_INDEX8);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_S16, 0xE);
		GXSetArray(GX_VA_TEX1, nozle_1_texcoord1_tbl, 4);
		nozleDrawDL1_0();
		GXClearVtxDesc();
		GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 7);
		GXSetArray(GX_VA_POS, nozle_1_vertex_tbl, 6);
		GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
		GXSetArray(GX_VA_NRM, nozle_1_normal_tbl, 3);
		GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
		GXSetArray(GX_VA_CLR0, nozle_1_color0_tbl, 4);
		GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0xE);
		GXSetArray(GX_VA_TEX0, nozle_1_texcoord0_tbl, 4);
		GXSetVtxDesc(GX_VA_TEX1, GX_INDEX8);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_S16, 0xE);
		GXSetArray(GX_VA_TEX1, nozle_1_texcoord1_tbl, 4);
		nozleDrawDL1_1();
		break;
	case 2:
		GXClearVtxDesc();
		GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 7);
		GXSetArray(GX_VA_POS, nozle_3_vertex_tbl, 6);
		GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
		GXSetArray(GX_VA_NRM, nozle_3_normal_tbl, 3);
		GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
		GXSetArray(GX_VA_CLR0, nozle_3_color0_tbl, 4);
		GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0xE);
		GXSetArray(GX_VA_TEX0, nozle_3_texcoord0_tbl, 4);
		GXSetVtxDesc(GX_VA_TEX1, GX_INDEX16);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_S16, 0xE);
		GXSetArray(GX_VA_TEX1, nozle_3_texcoord1_tbl, 4);
		nozleDrawDL3_1();
		GXClearVtxDesc();
		GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 7);
		GXSetArray(GX_VA_POS, nozle_3_vertex_tbl, 6);
		GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
		GXSetArray(GX_VA_NRM, nozle_3_normal_tbl, 3);
		GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
		GXSetArray(GX_VA_CLR0, nozle_3_color0_tbl, 4);
		GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0xE);
		GXSetArray(GX_VA_TEX0, nozle_3_texcoord0_tbl, 4);
		GXSetVtxDesc(GX_VA_TEX1, GX_INDEX16);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_S16, 0xE);
		GXSetArray(GX_VA_TEX1, nozle_3_texcoord1_tbl, 4);
		nozleDrawDL3_0();
		break;
	case 3:
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
		GXSetTevColor(GX_TEVREG0, (GXColor){ 0, 0, 0, alpha });
		entry = effect->userdata;
		entry++;
		for (i = 1; i < effect->count; i++, entry++) {
			PSMTXTrans(transMtx, entry->scrollX0, -entry->scrollY0, 0.0f);
			GXLoadTexMtxImm(transMtx, GX_TEXMTX0, GX_MTX2x4);
			PSMTXTrans(transMtx, entry->scrollX1, -entry->scrollY1, 0.0f);
			GXLoadTexMtxImm(transMtx, GX_TEXMTX1, GX_MTX2x4);
			PSMTXTrans(transMtx, entry->offsetX, entry->offsetY, entry->offsetZ);
			PSMTXScale(scaleMtx, entry->scaleX, entry->scaleY, 1.0f);
			PSMTXConcat(transMtx, scaleMtx, scaleMtx);
			PSMTXConcat(mtx, scaleMtx, scaleMtx);
			GXLoadPosMtxImm(scaleMtx, GX_PNMTX0);

			GXClearVtxDesc();
			GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 6);
			GXSetArray(GX_VA_POS, nozle_4_vertex_tbl, 6);
			GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
			GXSetArray(GX_VA_NRM, nozle_4_normal_tbl, 3);
			GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
			GXSetArray(GX_VA_CLR0, nozle_4_color0_tbl, 4);
			GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0xD);
			GXSetArray(GX_VA_TEX0, nozle_4_texcoord0_tbl, 4);
			GXSetVtxDesc(GX_VA_TEX1, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_S16, 0xD);
			GXSetArray(GX_VA_TEX1, nozle_4_texcoord1_tbl, 4);
			nozleDrawDL4_0();
		}
		break;
	}
}

// The tables are reached through one base pointer (the data anchor), the linker would drop the others.
#pragma force_active on
// TODO: type
__declspec(section ".data") u32 lbl_803AE260[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
static __declspec(section ".data") u32 negone_one_374[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_803AE270[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
static __declspec(section ".data") u32 one_negone_373[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_803AE280[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
static __declspec(section ".data") u32 negone_one_360[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_803AE290[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
static __declspec(section ".data") u32 one_negone_359[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
u32 nozle_1_vertex_tbl[112] = {
    0xFCE40000, 0x0000031C, 0x00000000, 0xF9330854,
    0x000006CD, 0x08540000, 0xF62E10AA, 0x000009D2,
    0x10AA0000, 0xF30C18FF, 0x00000CF4, 0x18FF0000,
    0xF0CD2154, 0x00000F33, 0x21540000, 0xEEDD29AA,
    0x00001123, 0x29AA0000, 0xEBE631FF, 0x0000141A,
    0x31FF0000, 0x00000000, 0x00000000, 0x08540000,
    0x000010AA, 0x00000000, 0x18FF0000, 0x00002154,
    0x00000000, 0x29AA0000, 0x0000397F, 0x0000FE72,
    0x00000000, 0xFC9A0854, 0x0000FB17, 0x10AA0000,
    0xF92818FF, 0x0000F809, 0x21540000, 0xF76F29AA,
    0x0000F5F3, 0x377F0000, 0x018E0000, 0x00000366,
    0x08540000, 0x04E910AA, 0x000006D8, 0x18FF0000,
    0x07F72154, 0x00000891, 0x29AA0000, 0x0A0D377F,
    0x0000FB01, 0x0000FD80, 0x04FF0000, 0xFD80F6A6,
    0x0854FD80, 0x095A0854, 0xFD80F2E6, 0x10AAFD80,
    0x0D1A10AA, 0xFD80EFC3, 0x18FFFD80, 0x103D18FF,
    0xFD80ED85, 0x2154FD80, 0x127B2154, 0xFD80EB94,
    0x29AAFD80, 0x146C29AA, 0xFD80E89E, 0x3BA7FD80,
    0x17623BA7, 0xFD800000, 0x0000FD80, 0x00000854,
    0xFD800000, 0x10AAFD80, 0x000018FF, 0xFD800000,
    0x2154FD80, 0x000029AA, 0xFD800000, 0x4327FD80,
    0xFD810000, 0xFD80FBA8, 0x0854FD80, 0xF9C810AA,
    0xFD80F7D9, 0x18FFFD80, 0xF6B92154, 0xFD80F61F,
    0x29AAFD80, 0xF4A44127, 0xFD80027F, 0x0000FD80,
    0x04580854, 0xFD800638, 0x10AAFD80, 0x082718FF,
    0xFD800947, 0x2154FD80, 0x09E129AA, 0xFD800B5C,
    0x4127FD80, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_1_texcoord0_tbl[56] = {
    0x00003555, 0x10003555, 0x10004000, 0x00004000,
    0x00002AAA, 0x10002AAA, 0x00002000, 0x10002000,
    0x00001555, 0x10001555, 0x00000AAA, 0x10000AAA,
    0x00000000, 0x10000000, 0x20003555, 0x30003555,
    0x30004000, 0x20004000, 0x30002AAA, 0x20002AAA,
    0x30002000, 0x20002000, 0x30001555, 0x20001555,
    0x30000AAA, 0x20000AAA, 0x30000000, 0x20000000,
    0x40003555, 0x40004000, 0x40002AAA, 0x40002000,
    0x40001555, 0x40000AAA, 0x40000000, 0x0000F556,
    0x1000F556, 0x0000EAAB, 0x1000EAAB, 0x0000E000,
    0x1000E000, 0x3000F556, 0x2000F556, 0x3000EAAB,
    0x2000EAAB, 0x3000E000, 0x2000E000, 0x4000F556,
    0x4000EAAB, 0x4000E000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_1_texcoord1_tbl[56] = {
    0x00003555, 0x10003555, 0x10004000, 0x00004000,
    0x00002AAA, 0x10002AAA, 0x00002000, 0x10002000,
    0x00001555, 0x10001555, 0x00000AAA, 0x10000AAA,
    0x00000000, 0x10000000, 0x20003555, 0x30003555,
    0x30004000, 0x20004000, 0x30002AAA, 0x20002AAA,
    0x30002000, 0x20002000, 0x30001555, 0x20001555,
    0x30000AAA, 0x20000AAA, 0x30000000, 0x20000000,
    0x40003555, 0x40004000, 0x40002AAA, 0x40002000,
    0x40001555, 0x40000AAA, 0x40000000, 0x0000F556,
    0x1000F556, 0x0000EAAB, 0x1000EAAB, 0x0000E000,
    0x1000E000, 0x3000F556, 0x2000F556, 0x3000EAAB,
    0x2000EAAB, 0x3000E000, 0x2000E000, 0x4000F556,
    0x4000EAAB, 0x4000E000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_1_dl_0_0[16] = {
    0x98000A0D, 0x00042222, 0x0B000221, 0x21220004,
    0x1A1A2100, 0x03181814, 0x00041B1B, 0x13000519,
    0x191B0004, 0x0D0D1A00, 0x030B0B0A, 0x00020A0A,
    0x08000008, 0x08000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_1_dl_0_1[16] = {
    0x98000A0B, 0x00022121, 0x09000020, 0x20210003,
    0x18182000, 0x00161613, 0x00051919, 0x12000017,
    0x171A0003, 0x0B0B1900, 0x00090908, 0x00000808,
    0x06000006, 0x06000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_1_dl_0_2[16] = {
    0x98000A09, 0x00002020, 0x0700001F, 0x1F200000,
    0x16161F00, 0x00141412, 0x00001717, 0x11000015,
    0x15190000, 0x09091800, 0x00070706, 0x00000606,
    0x04000004, 0x04000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_1_dl_0_3[16] = {
    0x98000A07, 0x00001F1F, 0x0500001E, 0x1E1F0000,
    0x14141E00, 0x00121211, 0x00001515, 0x10000013,
    0x13180000, 0x07071700, 0x00050504, 0x00000404,
    0x02000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_1_dl_0_4[16] = {
    0x98000A05, 0x00001E1E, 0x0300001C, 0x1C1E0000,
    0x12121D00, 0x000F0F10, 0x00001313, 0x0F00000E,
    0x0E170000, 0x05051600, 0x00010102, 0x00000000,
    0x00000103, 0x03000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_1_dl_1_0[16] = {
    0x98000A30, 0x00043131, 0x2E000230, 0x30450004,
    0x2D2D4400, 0x032B2B37, 0x00042E2E, 0x3600052C,
    0x2C3E0004, 0x28283D00, 0x0326262D, 0x00022525,
    0x2B000023, 0x23000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_1_dl_1_1[16] = {
    0x98000A2E, 0x00023030, 0x2C00002F, 0x2F440003,
    0x2B2B4300, 0x00292936, 0x00052C2C, 0x3500002A,
    0x2A3D0003, 0x26263C00, 0x0024242B, 0x00002323,
    0x2900000C, 0x0C000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_1_dl_1_2[16] = {
    0x98000A2C, 0x00002F2F, 0x2A000022, 0x22430000,
    0x29294200, 0x001A1A35, 0x00002A2A, 0x3400001B,
    0x1B3C0000, 0x24243B00, 0x000D0D29, 0x00000C0C,
    0x2700000A, 0x0A000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_1_dl_1_3[16] = {
    0x98000A2A, 0x00002222, 0x28000021, 0x21420000,
    0x1A1A4100, 0x00181834, 0x00001B1B, 0x33000019,
    0x193B0000, 0x0D0D3A00, 0x000B0B27, 0x00000A0A,
    0x25000008, 0x08000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_1_dl_1_4[16] = {
    0x98000A28, 0x00002121, 0x26000020, 0x20410000,
    0x18184000, 0x00161633, 0x00001919, 0x32000017,
    0x173A0000, 0x0B0B3900, 0x00090925, 0x00000808,
    0x23000106, 0x06000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_vertex_tbl[160] = {
    0xFBA00000, 0x0000FE11, 0x00000000, 0x00000000,
    0x000001EF, 0x00000000, 0x04600000, 0x0000F420,
    0x08550000, 0xFA100855, 0x00000000, 0x08550000,
    0x05F00855, 0x00000BE0, 0x08550000, 0xEFA110AA,
    0x0000F810, 0x10AA0000, 0x000010AA, 0x000007F0,
    0x10AA0000, 0x105F10AA, 0x0000ED20, 0x19000000,
    0xF7111900, 0x00000000, 0x19000000, 0x08EF1900,
    0x000012DF, 0x19000000, 0xEBA12155, 0x0000F610,
    0x21550000, 0x00002155, 0x000009F0, 0x21550000,
    0x145F2155, 0x0000EAA1, 0x29AA0000, 0xF59029AA,
    0x00000000, 0x29AA0000, 0x0A7029AA, 0x0000155F,
    0x29AA0000, 0x00003200, 0x00000AF0, 0x32000000,
    0x15E03200, 0x0000EA20, 0x32000000, 0xEB3335DC,
    0x0000EE4E, 0x3B580000, 0xF3253F9C, 0x0000F93E,
    0x42E30000, 0x00004500, 0x000006C2, 0x42E30000,
    0x0CDB3F9C, 0x000011B2, 0x3B580000, 0x14CD35DC,
    0x0000F510, 0x32000000, 0xF59A33EE, 0x0000F727,
    0x37AC0000, 0xF9933A8E, 0x0000FC9F, 0x3CF10000,
    0x00003E40, 0x00000361, 0x3CF10000, 0x066D3A8E,
    0x000008D9, 0x37AC0000, 0x0A6633EE, 0x0000FB00,
    0x0000FD80, 0xFDCA0000, 0xFD800000, 0x0000FD80,
    0x02360000, 0xFD800500, 0x0000FD80, 0xF26E08C0,
    0xFD80F937, 0x08C0FD80, 0x000008C0, 0xFD8006C9,
    0x08C0FD80, 0x0D9208C0, 0xFD80ED4A, 0x1180FD80,
    0xF6EE1180, 0xFD800000, 0x1180FD80, 0x09121180,
    0xFD8012B6, 0x1180FD80, 0xEA6E1A40, 0xFD80F5CA,
    0x1A40FD80, 0x00001A40, 0xFD800A36, 0x1A40FD80,
    0x15921A40, 0xFD80E8B7, 0x2300FD80, 0xF4A52300,
    0xFD800000, 0x2300FD80, 0x0B5B2300, 0xFD801749,
    0x2300FD80, 0xE7932BC0, 0xFD80F413, 0x2BC0FD80,
    0x00002BC0, 0xFD800BED, 0x2BC0FD80, 0x186D2BC0,
    0xFD800000, 0x3480FD80, 0x0C803480, 0xFD801900,
    0x3480FD80, 0xE7003480, 0xFD80E83A, 0x388EFD80,
    0xEBC73E50, 0xFD80F14F, 0x42CBFD80, 0xF847463B,
    0xFD800000, 0x4873FD80, 0x07B9463B, 0xFD800EB1,
    0x42CBFD80, 0x14393E50, 0xFD8017C6, 0x388EFD80,
    0xF3803480, 0xFD80F41D, 0x3687FD80, 0xF5E43A75,
    0xFD80F8A8, 0x3D7BFD80, 0xFC243FFD, 0xFD800000,
    0x415CFD80, 0x03DC3FFD, 0xFD800758, 0x3D7BFD80,
    0x0A1C3A75, 0xFD800BE3, 0x3687FD80, 0x00000000,
};

// TODO: type
u32 nozle_3_texcoord0_tbl[96] = {
    0x00003555, 0x10003555, 0x10004000, 0x00004000,
    0x20003555, 0x20004000, 0x30003555, 0x30004000,
    0x40003555, 0x40004000, 0x00002AAA, 0x10002AAA,
    0x20002AAA, 0x30002AAA, 0x40002AAA, 0x00002000,
    0x10002000, 0x20002000, 0x30002000, 0x40002000,
    0x00001555, 0x10001555, 0x20001555, 0x30001555,
    0x40001555, 0x00000AAA, 0x10000AAA, 0x20000AAA,
    0x30000AAA, 0x40000AAA, 0x00000000, 0x10000000,
    0x20000000, 0x30000000, 0x40000000, 0x1000FCCD,
    0x0000FCCD, 0x1000F99A, 0x0000F99A, 0x1000F667,
    0x0000F667, 0x1000F334, 0x0000F334, 0x1000F000,
    0x0000F000, 0x3000F334, 0x3000F000, 0x4000F000,
    0x4000F334, 0x3000F667, 0x4000F667, 0x3000F99A,
    0x4000F99A, 0x3000FCCD, 0x4000FCCD, 0x0000F556,
    0x1000F556, 0x2000F556, 0x3000F556, 0x4000F556,
    0x0000EAAB, 0x1000EAAB, 0x2000EAAB, 0x3000EAAB,
    0x4000EAAB, 0x0000E000, 0x1000E000, 0x2000E000,
    0x3000E000, 0x4000E000, 0x1000DCCD, 0x0000DCCD,
    0x1000D99A, 0x0000D99A, 0x1000D667, 0x0000D667,
    0x1000D334, 0x0000D334, 0x1000D000, 0x0000D000,
    0x3000D334, 0x3000D000, 0x4000D000, 0x4000D334,
    0x3000D667, 0x4000D667, 0x3000D99A, 0x4000D99A,
    0x3000DCCD, 0x4000DCCD, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_texcoord1_tbl[96] = {
    0x00003555, 0x10003555, 0x10004000, 0x00004000,
    0x20003555, 0x20004000, 0x30003555, 0x30004000,
    0x40003555, 0x40004000, 0x00002AAA, 0x10002AAA,
    0x20002AAA, 0x30002AAA, 0x40002AAA, 0x00002000,
    0x10002000, 0x20002000, 0x30002000, 0x40002000,
    0x00001555, 0x10001555, 0x20001555, 0x30001555,
    0x40001555, 0x00000AAA, 0x10000AAA, 0x20000AAA,
    0x30000AAA, 0x40000AAA, 0x00000000, 0x10000000,
    0x20000000, 0x30000000, 0x40000000, 0x1000FCCD,
    0x0000FCCD, 0x1000F99A, 0x0000F99A, 0x1000F667,
    0x0000F667, 0x1000F334, 0x0000F334, 0x1000F000,
    0x0000F000, 0x3000F334, 0x3000F000, 0x4000F000,
    0x4000F334, 0x3000F667, 0x4000F667, 0x3000F99A,
    0x4000F99A, 0x3000FCCD, 0x4000FCCD, 0x0000F556,
    0x1000F556, 0x2000F556, 0x3000F556, 0x4000F556,
    0x0000EAAB, 0x1000EAAB, 0x2000EAAB, 0x3000EAAB,
    0x4000EAAB, 0x0000E000, 0x1000E000, 0x2000E000,
    0x3000E000, 0x4000E000, 0x1000DCCD, 0x0000DCCD,
    0x1000D99A, 0x0000D99A, 0x1000D667, 0x0000D667,
    0x1000D334, 0x0000D334, 0x1000D000, 0x0000D000,
    0x3000D334, 0x3000D000, 0x4000D000, 0x4000D334,
    0x3000D667, 0x4000D667, 0x3000D99A, 0x4000D99A,
    0x3000DCCD, 0x4000DCCD, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_color0_tbl[32] = {
    0xFFCB17CC, 0xFFF0B633, 0xFFB117CC, 0xFF7C21CC,
    0xE35115CC, 0xD64C14CC, 0xAA3D1066, 0xB4401166,
    0xBC431266, 0xB440114C, 0xAA3D104C, 0xB4401133,
    0xAA3D1033, 0xB4401119, 0xAA3D1019, 0xB4401100,
    0xAA3D1000, 0xB79210CC, 0xBAB08533, 0xB27B10CC,
    0xB25717CC, 0xAA3D10CC, 0x7E2D0CCC, 0x5F220966,
    0x5F22094C, 0x5F220933, 0x5F220919, 0x5F220900,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_0_0[48] = {
    0x98001100, 0x04000000, 0x01000900, 0x09000300,
    0x00000100, 0x07000700, 0x09000000, 0x00000800,
    0x08000800, 0x00000000, 0x06000600, 0x0E000000,
    0x02000E00, 0x0E000D00, 0x00000200, 0x0D000D00,
    0x13000000, 0x03001300, 0x13001200, 0x00000300,
    0x12001200, 0x18000000, 0x04001800, 0x18001700,
    0x00000400, 0x17001700, 0x1D000000, 0x05001D00,
    0x1D001C00, 0x00000500, 0x1C001C00, 0x20000000,
    0x06002200, 0x22001F00, 0x00000700, 0x21002100,
    0x34000000, 0x07003500, 0x35001E00, 0x00000800,
    0x20002000, 0x33000000, 0x09003300, 0x33000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_0_1[48] = {
    0x98001100, 0x02000000, 0x01000500, 0x05000100,
    0x00000100, 0x02000200, 0x07000000, 0x00000400,
    0x04000600, 0x00000000, 0x01000100, 0x0C000000,
    0x02000C00, 0x0C000B00, 0x00000200, 0x0B000B00,
    0x11000000, 0x03001100, 0x11001000, 0x00000300,
    0x10001000, 0x16000000, 0x04001600, 0x16001500,
    0x00000400, 0x15001500, 0x1B000000, 0x05001B00,
    0x1B001A00, 0x00000500, 0x1A001A00, 0x1E000000,
    0x08002000, 0x20002B00, 0x00000700, 0x1F001F00,
    0x2C000000, 0x07002300, 0x23002200, 0x00000600,
    0x24002400, 0x23000000, 0x0A002600, 0x26000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_0_2[40] = {
    0x98000E00, 0x03000000, 0x01000700, 0x07000200,
    0x00000100, 0x05000500, 0x08000000, 0x00000600,
    0x06000700, 0x00000000, 0x04000400, 0x0D000000,
    0x02000D00, 0x0D000C00, 0x00000200, 0x0C000C00,
    0x12000000, 0x03001200, 0x12001100, 0x00000300,
    0x11001100, 0x17000000, 0x04001700, 0x17001600,
    0x00000400, 0x16001600, 0x1C000000, 0x05001C00,
    0x1C001B00, 0x00000500, 0x1B001B00, 0x1F000000,
    0x07002100, 0x21001E00, 0x00000800, 0x20002000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_0_3[24] = {
    0x98000900, 0x26000000, 0x10002F00, 0x2F002700,
    0x00000E00, 0x30003000, 0x30000000, 0x0F002E00,
    0x2E003100, 0x00000D00, 0x2D002D00, 0x1E000000,
    0x08002000, 0x20003200, 0x00000B00, 0x31003100,
    0x33000000, 0x09003300, 0x33002900, 0x00000A00,
    0x34003400, 0x2A000000, 0x06003600, 0x36000000,
};

// TODO: type
u32 nozle_3_dl_0_4[16] = {
    0x98000600, 0x30000000, 0x0F002B00, 0x2B001E00,
    0x00000800, 0x20002000, 0x2F000000, 0x0D002900,
    0x29002E00, 0x00000B00, 0x27002700, 0x25000000,
    0x0E002A00, 0x2A002400, 0x00000C00, 0x28002800,
};

// TODO: type
u32 nozle_3_dl_0_5[16] = {
    0x98000500, 0x29000000, 0x0A003400, 0x34003200,
    0x00000B00, 0x31003100, 0x28000000, 0x0C003200,
    0x32003100, 0x00000D00, 0x2D002D00, 0x27000000,
    0x0E003000, 0x30000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_0_6[16] = {
    0x98000500, 0x2E000000, 0x0B002700, 0x27001E00,
    0x00000800, 0x20002000, 0x2D000000, 0x09002500,
    0x25002C00, 0x00000700, 0x23002300, 0x23000000,
    0x0A002600, 0x26000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_0_7[16] = {
    0x98000400, 0x23000000, 0x0A002600, 0x26002400,
    0x00000C00, 0x28002800, 0x2D000000, 0x09002500,
    0x25002E00, 0x00000B00, 0x27002700, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_0_8[16] = {
    0x98000400, 0x25000000, 0x0E002A00, 0x2A002600,
    0x00001000, 0x2C002C00, 0x2F000000, 0x0D002900,
    0x29003000, 0x00000F00, 0x2B002B00, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_0_9[16] = {
    0x98000400, 0x0B000000, 0x02000B00, 0x0B000600,
    0x00000000, 0x01000100, 0x05000000, 0x00000000,
    0x00000000, 0x00000100, 0x03000300, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_0_10[16] = {
    0x98000400, 0x10000000, 0x03001000, 0x10000B00,
    0x00000200, 0x0B000B00, 0x0A000000, 0x02000A00,
    0x0A000500, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_0_11[16] = {
    0x98000400, 0x15000000, 0x04001500, 0x15001000,
    0x00000300, 0x10001000, 0x0F000000, 0x03000F00,
    0x0F000A00, 0x00000200, 0x0A000A00, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_0_12[16] = {
    0x98000400, 0x1A000000, 0x05001A00, 0x1A001500,
    0x00000400, 0x15001500, 0x14000000, 0x04001400,
    0x14000F00, 0x00000300, 0x0F000F00, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_0_13[16] = {
    0x98000400, 0x2B000000, 0x07001F00, 0x1F001A00,
    0x00000500, 0x1A001A00, 0x19000000, 0x05001900,
    0x19001400, 0x00000400, 0x14001400, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_0_14[16] = {
    0x98000400, 0x22000000, 0x06002400, 0x24002B00,
    0x00000700, 0x1F001F00, 0x21000000, 0x06001E00,
    0x1E001900, 0x00000500, 0x19001900, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_0_15[16] = {
    0x98000400, 0x20000000, 0x06002200, 0x22003400,
    0x00000700, 0x35003500, 0x2A000000, 0x06003600,
    0x36003300, 0x00000900, 0x33003300, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_0_16[16] = {
    0x98000300, 0x06000000, 0x00000100, 0x01000100,
    0x00000100, 0x02000200, 0x00000000, 0x01000300,
    0x03000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_0_tbl[17] = {
    (u32)&nozle_3_dl_0_0, (u32)&nozle_3_dl_0_1, (u32)&nozle_3_dl_0_2, (u32)&nozle_3_dl_0_3,
    (u32)&nozle_3_dl_0_4, (u32)&nozle_3_dl_0_5, (u32)&nozle_3_dl_0_6, (u32)&nozle_3_dl_0_7,
    (u32)&nozle_3_dl_0_8, (u32)&nozle_3_dl_0_9, (u32)&nozle_3_dl_0_10, (u32)&nozle_3_dl_0_11,
    (u32)&nozle_3_dl_0_12, (u32)&nozle_3_dl_0_13, (u32)&nozle_3_dl_0_14, (u32)&nozle_3_dl_0_15,
    (u32)&nozle_3_dl_0_16,
};
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".data") u32 gap_05_803AF4A4_data[7] = { 0 };
#pragma force_active reset
#pragma explicit_zero_data reset

// TODO: type
u32 nozle_3_dl_1_0[48] = {
    0x98001100, 0x39000000, 0x12001300, 0x13003800,
    0x00001200, 0x12001200, 0x3E000000, 0x11001800,
    0x18003D00, 0x00001100, 0x17001700, 0x43000000,
    0x13001D00, 0x1D004200, 0x00001300, 0x1C001C00,
    0x48000000, 0x14002200, 0x22004700, 0x00001400,
    0x21002100, 0x4D000000, 0x15003B00, 0x3B004C00,
    0x00001500, 0x3A003A00, 0x52000000, 0x16004000,
    0x40005100, 0x00001600, 0x3F003F00, 0x55000000,
    0x17004500, 0x45005400, 0x00001700, 0x44004400,
    0x69000000, 0x17005800, 0x58005300, 0x00001700,
    0x43004300, 0x68000000, 0x18005600, 0x56000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_1_1[48] = {
    0x98001100, 0x37000000, 0x12001100, 0x11003600,
    0x00001200, 0x10001000, 0x3C000000, 0x11001600,
    0x16003B00, 0x00001100, 0x15001500, 0x41000000,
    0x13001B00, 0x1B004000, 0x00001300, 0x1A001A00,
    0x46000000, 0x14002000, 0x20004500, 0x00001400,
    0x1F001F00, 0x4B000000, 0x15003900, 0x39004A00,
    0x00001500, 0x38003800, 0x50000000, 0x16003E00,
    0x3E004F00, 0x00001600, 0x3D003D00, 0x53000000,
    0x17004300, 0x43006000, 0x00001700, 0x42004200,
    0x61000000, 0x17004600, 0x46005700, 0x00001700,
    0x47004700, 0x58000000, 0x18004900, 0x49000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_1_2[40] = {
    0x98000E00, 0x38000000, 0x12001200, 0x12003700,
    0x00001200, 0x11001100, 0x3D000000, 0x11001700,
    0x17003C00, 0x00001100, 0x16001600, 0x42000000,
    0x13001C00, 0x1C004100, 0x00001300, 0x1B001B00,
    0x47000000, 0x14002100, 0x21004600, 0x00001400,
    0x20002000, 0x4C000000, 0x15003A00, 0x3A004B00,
    0x00001500, 0x39003900, 0x51000000, 0x16003F00,
    0x3F005000, 0x00001600, 0x3E003E00, 0x54000000,
    0x17004400, 0x44005300, 0x00001700, 0x43004300,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_1_3[24] = {
    0x98000900, 0x5B000000, 0x1B005200, 0x52005C00,
    0x00001A00, 0x53005300, 0x65000000, 0x1B005100,
    0x51006600, 0x00001A00, 0x50005000, 0x53000000,
    0x17004300, 0x43006700, 0x00001900, 0x54005400,
    0x68000000, 0x18005600, 0x56005E00, 0x00001800,
    0x57005700, 0x5F000000, 0x17005900, 0x59000000,
};

// TODO: type
u32 nozle_3_dl_1_4[16] = {
    0x98000600, 0x65000000, 0x1B004E00, 0x4E005300,
    0x00001700, 0x43004300, 0x64000000, 0x1A004C00,
    0x4C006300, 0x00001900, 0x4A004A00, 0x5A000000,
    0x1A004D00, 0x4D005900, 0x00001900, 0x4B004B00,
};

// TODO: type
u32 nozle_3_dl_1_5[16] = {
    0x98000500, 0x5E000000, 0x18005700, 0x57006700,
    0x00001900, 0x54005400, 0x5D000000, 0x19005500,
    0x55006600, 0x00001A00, 0x50005000, 0x5C000000,
    0x1A005300, 0x53000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_1_6[16] = {
    0x98000500, 0x63000000, 0x19004A00, 0x4A005300,
    0x00001700, 0x43004300, 0x62000000, 0x18004800,
    0x48006100, 0x00001700, 0x46004600, 0x58000000,
    0x18004900, 0x49000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_1_7[16] = {
    0x98000400, 0x58000000, 0x18004900, 0x49005900,
    0x00001900, 0x4B004B00, 0x62000000, 0x18004800,
    0x48006300, 0x00001900, 0x4A004A00, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_1_8[16] = {
    0x98000400, 0x5A000000, 0x1A004D00, 0x4D005B00,
    0x00001B00, 0x4F004F00, 0x64000000, 0x1A004C00,
    0x4C006500, 0x00001B00, 0x4E004E00, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_1_9[16] = {
    0x98000400, 0x40000000, 0x13001A00, 0x1A003B00,
    0x00001100, 0x15001500, 0x3A000000, 0x11001400,
    0x14003500, 0x00001200, 0x0F000F00, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_1_10[16] = {
    0x98000400, 0x45000000, 0x14001F00, 0x1F004000,
    0x00001300, 0x1A001A00, 0x3F000000, 0x13001900,
    0x19003A00, 0x00001100, 0x14001400, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_1_11[16] = {
    0x98000400, 0x4A000000, 0x15003800, 0x38004500,
    0x00001400, 0x1F001F00, 0x44000000, 0x14001E00,
    0x1E003F00, 0x00001300, 0x19001900, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_1_12[16] = {
    0x98000400, 0x4F000000, 0x16003D00, 0x3D004A00,
    0x00001500, 0x38003800, 0x49000000, 0x15003700,
    0x37004400, 0x00001400, 0x1E001E00, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_1_13[16] = {
    0x98000400, 0x60000000, 0x17004200, 0x42004F00,
    0x00001600, 0x3D003D00, 0x4E000000, 0x16003C00,
    0x3C004900, 0x00001500, 0x37003700, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_1_14[16] = {
    0x98000400, 0x57000000, 0x17004700, 0x47006000,
    0x00001700, 0x42004200, 0x56000000, 0x17004100,
    0x41004E00, 0x00001600, 0x3C003C00, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_1_15[16] = {
    0x98000400, 0x55000000, 0x17004500, 0x45006900,
    0x00001700, 0x58005800, 0x5F000000, 0x17005900,
    0x59006800, 0x00001800, 0x56005600, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_1_16[16] = {
    0x98000300, 0x3B000000, 0x11001500, 0x15003600,
    0x00001200, 0x10001000, 0x35000000, 0x12000F00,
    0x0F000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_3_dl_1_tbl[17] = {
    (u32)&nozle_3_dl_1_0, (u32)&nozle_3_dl_1_1, (u32)&nozle_3_dl_1_2, (u32)&nozle_3_dl_1_3,
    (u32)&nozle_3_dl_1_4, (u32)&nozle_3_dl_1_5, (u32)&nozle_3_dl_1_6, (u32)&nozle_3_dl_1_7,
    (u32)&nozle_3_dl_1_8, (u32)&nozle_3_dl_1_9, (u32)&nozle_3_dl_1_10, (u32)&nozle_3_dl_1_11,
    (u32)&nozle_3_dl_1_12, (u32)&nozle_3_dl_1_13, (u32)&nozle_3_dl_1_14, (u32)&nozle_3_dl_1_15,
    (u32)&nozle_3_dl_1_16,
};
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".data") u32 gap_05_803AFAC4_data[7] = { 0 };
#pragma force_active reset
#pragma explicit_zero_data reset

// TODO: type
u32 nozle_4_vertex_tbl[48] = {
    0xA6000000, 0x0000BC80, 0x00000000, 0xD3000000,
    0x0000E980, 0x00000000, 0x00000000, 0x00001680,
    0x00000000, 0x2D000000, 0x00004380, 0x00000000,
    0x5A000000, 0x0000A600, 0x05000000, 0xBC800570,
    0x0000D300, 0x05000000, 0xE980058A, 0x00000000,
    0x06810000, 0x16800681, 0x00002D00, 0x06480000,
    0x43800500, 0x00005A00, 0x05000000, 0xA6000A00,
    0x0000BC80, 0x0AFC0000, 0xD3000A00, 0x0000E980,
    0x0A8A0000, 0x00000BF0, 0x00001680, 0x0BF00000,
    0x2D000B48, 0x00004380, 0x0A000000, 0x5A000A00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_4_texcoord0_tbl[32] = {
    0x1000A800, 0x1000C000, 0x0000C000, 0x0000A800,
    0x1000D800, 0x0000D800, 0x1000F000, 0x0000F000,
    0x10000800, 0x00000800, 0x10002000, 0x00002000,
    0x10003800, 0x00003800, 0x10005000, 0x00005000,
    0x10006800, 0x00006800, 0x2000A800, 0x2000C000,
    0x2000D800, 0x2000F000, 0x20000800, 0x20002000,
    0x20003800, 0x20005000, 0x20006800, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_4_texcoord1_tbl[32] = {
    0x1000D000, 0x1000E000, 0x0000E000, 0x0000D000,
    0x1000F000, 0x0000F000, 0x10000000, 0x00000000,
    0x10001000, 0x00001000, 0x10002000, 0x00002000,
    0x10003000, 0x00003000, 0x10004000, 0x00004000,
    0x10005000, 0x00005000, 0x2000D000, 0x2000E000,
    0x2000F000, 0x20000000, 0x20001000, 0x20002000,
    0x20003000, 0x20004000, 0x20005000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_4_dl_0_0[16] = {
    0x98000612, 0x00001212, 0x13000013, 0x13090000,
    0x00000A00, 0x00010100, 0x00000303, 0x01000002,
    0x02000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_4_dl_0_1[16] = {
    0x98000613, 0x00001313, 0x14000014, 0x140A0000,
    0x01010B00, 0x00040401, 0x00000202, 0x02000005,
    0x05000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_4_dl_0_2[16] = {
    0x98000614, 0x00001414, 0x15000015, 0x150B0000,
    0x04040C00, 0x00060602, 0x00000505, 0x03000007,
    0x07000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_4_dl_0_3[16] = {
    0x98000615, 0x00001515, 0x16000016, 0x160C0000,
    0x06060D00, 0x00080803, 0x00000707, 0x04000009,
    0x09000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_4_dl_0_4[16] = {
    0x98000616, 0x00001616, 0x17000017, 0x170D0000,
    0x08080E00, 0x000A0A04, 0x00000909, 0x0500000B,
    0x0B000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_4_dl_0_5[16] = {
    0x98000617, 0x00001717, 0x18000018, 0x180E0000,
    0x0A0A0F00, 0x000C0C05, 0x00000B0B, 0x0600000D,
    0x0D000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_4_dl_0_6[16] = {
    0x98000618, 0x00001818, 0x19000119, 0x190F0000,
    0x0C0C1000, 0x010E0E06, 0x00000D0D, 0x0700010F,
    0x0F000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_4_dl_0_7[16] = {
    0x98000619, 0x00011919, 0x1A00021A, 0x1A100001,
    0x0E0E1100, 0x02101007, 0x00010F0F, 0x08000211,
    0x11000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_1_normal_tbl[8] = {
    0x00004000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_1_color0_tbl[8] = {
    0xFFFFFFCC, 0xFFFFFF33, 0xFFFFFF4C, 0xFFFFFF66,
    0xFFFFFF00, 0xFFFFFF7F, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_1_dl_0_5[8] = {
    0x98000401, 0x00011D1D, 0x1C000110, 0x10030000,
    0x1C1C1D00, 0x000F0F00, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_1_dl_0_6[8] = {
    0x9800041C, 0x00011010, 0x0E000111, 0x111D0000,
    0x0F0F0F00, 0x000E0E00, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_1_dl_0_7[8] = {
    0x9800040E, 0x00011111, 0x15000102, 0x020F0000,
    0x0E0E1600, 0x00010100, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_1_dl_0_8[8] = {
    0x9800030C, 0x00040C0C, 0x1B00040D, 0x0D0A0002,
    0x0A0A0000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_1_dl_0_9[8] = {
    0x98000316, 0x00000101, 0x15000102, 0x02000001,
    0x03030000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_1_dl_0_tbl[10] = {
    (u32)&nozle_1_dl_0_0, (u32)&nozle_1_dl_0_1, (u32)&nozle_1_dl_0_2, (u32)&nozle_1_dl_0_3,
    (u32)&nozle_1_dl_0_4, (u32)&nozle_1_dl_0_5, (u32)&nozle_1_dl_0_6, (u32)&nozle_1_dl_0_7,
    (u32)&nozle_1_dl_0_8, (u32)&nozle_1_dl_0_9,
};

// TODO: type
u8 nozle_1_dl_0_size_tbl[12] = {
    0x02, 0x02, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x00, 0x00,
};
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_80415ED4_sdata[3] = { 0 };
#pragma force_active reset
#pragma explicit_zero_data reset

// TODO: type
u32 nozle_1_dl_1_5[8] = {
    0x98000424, 0x00011F1F, 0x3F000114, 0x14260000,
    0x20204000, 0x00161600, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_1_dl_1_6[8] = {
    0x9800043F, 0x00011414, 0x31000115, 0x15400000,
    0x16163200, 0x00171700, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_1_dl_1_7[8] = {
    0x98000431, 0x00011515, 0x38000107, 0x07320000,
    0x17173900, 0x00090900, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_1_dl_1_8[8] = {
    0x9800032F, 0x00042727, 0x3E000428, 0x282D0002,
    0x25250000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_1_dl_1_9[8] = {
    0x98000339, 0x00000909, 0x38000107, 0x07230001,
    0x06060000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_1_dl_1_tbl[10] = {
    (u32)&nozle_1_dl_1_0, (u32)&nozle_1_dl_1_1, (u32)&nozle_1_dl_1_2, (u32)&nozle_1_dl_1_3,
    (u32)&nozle_1_dl_1_4, (u32)&nozle_1_dl_1_5, (u32)&nozle_1_dl_1_6, (u32)&nozle_1_dl_1_7,
    (u32)&nozle_1_dl_1_8, (u32)&nozle_1_dl_1_9,
};

// TODO: type
u8 nozle_1_dl_1_size_tbl[12] = {
    0x02, 0x02, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x00, 0x00,
};
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_80415FB4_sdata[3] = { 0 };
#pragma force_active reset
#pragma explicit_zero_data reset

// TODO: type
u32 nozle_3_normal_tbl[1] = {
    0x00004000,
};

// TODO: type
u8 nozle_3_dl_0_size_tbl[20] = {
    0x06, 0x06, 0x05, 0x03, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x00, 0x00, 0x00,
};

// TODO: type
u8 nozle_3_dl_1_size_tbl[20] = {
    0x06, 0x06, 0x05, 0x03, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x00, 0x00, 0x00,
};
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_80415FEC_sdata[5] = { 0 };
#pragma force_active reset
#pragma explicit_zero_data reset

// TODO: type
u32 nozle_4_normal_tbl[8] = {
    0x00004000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 nozle_4_color0_tbl[3] = {
    0xFFFFFF7F, 0xFFFFFF4C, 0xFFFFFF00,
};

// TODO: type
u32 nozle_4_dl_0_tbl[8] = {
    (u32)&nozle_4_dl_0_0, (u32)&nozle_4_dl_0_1, (u32)&nozle_4_dl_0_2, (u32)&nozle_4_dl_0_3,
    (u32)&nozle_4_dl_0_4, (u32)&nozle_4_dl_0_5, (u32)&nozle_4_dl_0_6, (u32)&nozle_4_dl_0_7,
};

// TODO: type
u8 nozle_4_dl_0_size_tbl[8] = {
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
};

// 44 scale entries (22 pairs) indexed by counter/lifetime.
static u8 scale_dt[44] = {
	0x00, 0x00, 0x0B, 0x0D, 0x16, 0x1B, 0x22, 0x29, 0x2E, 0x37, 0x39,
	0x45, 0x44, 0x52, 0x4F, 0x5E, 0x57, 0x69, 0x5E, 0x71, 0x64, 0x78,
	0x66, 0x7B, 0x67, 0x7C, 0x65, 0x7B, 0x62, 0x78, 0x5F, 0x74, 0x5A,
	0x6F, 0x56, 0x6B, 0x53, 0x67, 0x51, 0x65, 0x50, 0x64, 0x00, 0x00,
};
// clang-format on

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).

#pragma force_active reset

// .sbss2 4-byte gap after the unit's colour template.
#pragma section ".sdata" ".sbss2"
#pragma force_active on
__declspec(section ".sdata") u32 gap_10_80423304_sbss2;
#pragma force_active reset
