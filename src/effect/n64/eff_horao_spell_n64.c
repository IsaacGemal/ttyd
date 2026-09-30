#include "eff/n64/eff_horao_spell_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os/OSCache.h>
#include <math.h>
#include <string.h>

extern GXColor unk_80423150;
extern GXColor unk_80423154;
extern GXColor unk_80423158;
extern effHoraoSpellVtx horao_spell_vtx[];

// .rodata
const Vec vec3_802fa7f8 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_802fa808 = 4503601774854144.0;
const char str_802FA810[] = "HoraoSpellN64";

// .sdata
static u8 colr[20] = {
	0x78, 0x82, 0x8C, 0x96, 0xA0, 0xAA, 0xB4, 0xBE,
	0xC8, 0xC8, 0xBE, 0xB4, 0xAA, 0xA0, 0x96, 0x8C, 0x82, 0, 0, 0,
};
static u8 colg[20] = {
	0xAA, 0xA0, 0x96, 0x8C, 0x82, 0x78, 0x82, 0x8C,
	0x96, 0xA0, 0xAA, 0xB4, 0xBE, 0xC8, 0xC8, 0xBE, 0xB4, 0, 0, 0,
};
static u8 colb[20] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0,
};

// .sdata2 constants (named after their original addresses).
const f32 float_deg2rad_8041f15c = 0.017453292f;
const f32 float_0p0078125_8041f160 = 0.0078125f;
const f32 float_0p03125_8041f164 = 0.03125f;
const f32 float_0_8041f168 = 0.0f;
const f32 float_6p2832_8041f16c = 6.2831855f;
const f32 float_78p75_8041f170 = 78.75f;
const f32 float_0p34907_8041f174 = 0.34906584f;
const f32 float_6_8041f178 = 6.0f;
const f32 float_360_8041f17c = 360.0f;
const f32 float_neg30_8041f180 = -30.0f;
const f32 float_8_8041f184 = 8.0f;
const f32 float_30_8041f188 = 30.0f;
const f32 float_0p10472_8041f18c = 0.10471976f;
const f32 float_1_8041f190 = 1.0f;
const f32 float_0p01_8041f194 = 0.01f;
const u32 dat_8041f158 = 0xFFFFFF00;

// .data: 32 vertices (16 pairs), values read byte-for-byte from the original `.4byte` block.
effHoraoSpellVtx horao_spell_vtx[32] = {
	{ { 0, -8, 0 }, { 0, 0 }, { 0, 0, 0, 0 } },
	{ { 0, 8, 0 }, { 0, 1024 }, { 0, 0, 0, 0 } },
	{ { 0, -8, 0 }, { 512, 0 }, { 0, 0, 0, 50 } },
	{ { 0, 8, 0 }, { 512, 1024 }, { 0, 0, 0, 50 } },
	{ { 0, -8, 0 }, { 1024, 0 }, { 0, 0, 0, 100 } },
	{ { 0, 8, 0 }, { 1024, 1024 }, { 0, 0, 0, 100 } },
	{ { 0, -8, 0 }, { 1536, 0 }, { 0, 0, 0, 150 } },
	{ { 0, 8, 0 }, { 1536, 1024 }, { 0, 0, 0, 150 } },
	{ { 0, -8, 0 }, { 2048, 0 }, { 0, 0, 0, 200 } },
	{ { 0, 8, 0 }, { 2048, 1024 }, { 0, 0, 0, 200 } },
	{ { 0, -8, 0 }, { 2560, 0 }, { 0, 0, 0, 250 } },
	{ { 0, 8, 0 }, { 2560, 1024 }, { 0, 0, 0, 250 } },
	{ { 0, -8, 0 }, { 3072, 0 }, { 0, 0, 0, 255 } },
	{ { 0, 8, 0 }, { 3072, 1024 }, { 0, 0, 0, 255 } },
	{ { 0, -8, 0 }, { 3584, 0 }, { 0, 0, 0, 255 } },
	{ { 0, 8, 0 }, { 3584, 1024 }, { 0, 0, 0, 255 } },
	{ { 0, -8, 0 }, { 4096, 0 }, { 0, 0, 0, 255 } },
	{ { 0, 8, 0 }, { 4096, 1024 }, { 0, 0, 0, 255 } },
	{ { 0, -8, 0 }, { 4608, 0 }, { 0, 0, 0, 255 } },
	{ { 0, 8, 0 }, { 4608, 1024 }, { 0, 0, 0, 255 } },
	{ { 0, -8, 0 }, { 5120, 0 }, { 0, 0, 0, 255 } },
	{ { 0, 8, 0 }, { 5120, 1024 }, { 0, 0, 0, 255 } },
	{ { 0, -8, 0 }, { 5632, 0 }, { 0, 0, 0, 255 } },
	{ { 0, 8, 0 }, { 5632, 1024 }, { 0, 0, 0, 255 } },
	{ { 0, -8, 0 }, { 6144, 0 }, { 0, 0, 0, 255 } },
	{ { 0, 8, 0 }, { 6144, 1024 }, { 0, 0, 0, 255 } },
	{ { 0, -8, 0 }, { 6656, 0 }, { 0, 0, 0, 200 } },
	{ { 0, 8, 0 }, { 6656, 1024 }, { 0, 0, 0, 200 } },
	{ { 0, -8, 0 }, { 7168, 0 }, { 0, 0, 0, 100 } },
	{ { 0, 8, 0 }, { 7168, 1024 }, { 0, 0, 0, 100 } },
	{ { 0, -8, 0 }, { 7680, 0 }, { 0, 0, 0, 0 } },
	{ { 0, 8, 0 }, { 7680, 1024 }, { 0, 0, 0, 0 } },
};

// .sbss2 scratch GXColors and the .data vertex table are declared in the unit header.
EffectEntry* effHoraoSpellN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effHoraoSpellData* data;

	effect = effEntry();
	effect->type = str_802FA810;
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x3C);
	effect->userdata = data;
	effect->callback = effHoraoSpellMain;
	effect->flags |= 2;
	data->type = type;
	data->field_0x14 = 0;
	if (life <= 0) {
		data->field_0x10 = 1000;
	} else {
		data->field_0x10 = life;
	}
	data->field_0x24 = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x38 = scale;
	data->field_0x18 = 70;
	data->field_0x1C = 180;
	data->field_0x20 = 120;
	data->field_0x28 = 20;
	data->field_0x2C = 230;
	data->field_0x30 = 50;
	data->field_0x34 = 255;
	return effect;
}
void effHoraoSpellMain(EffectEntry* effect) {
	effHoraoSpellData* data;
	s32 timer;
	s32 counter;
	Vec pos2;
	Vec pos;

	data = effect->userdata;
	pos = (Vec){ 0.0f, 0.0f, 0.0f };
	pos.x = data->x;
	pos.y = data->y;
	pos.z = data->z;
	pos2 = pos;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->field_0x10 = 16;
	}

	timer = data->field_0x10;
	if (timer < 1000) {
		data->field_0x10 = timer - 1;
	}
	data->field_0x14++;
	timer = data->field_0x10;

	if (timer < 0) {
		effDelete(effect);
		return;
	}

	counter = data->field_0x14;
	if (timer < 16) {
		data->field_0x24 = timer * 16;
	}
	if (counter < 16) {
		data->field_0x24 = counter * 16 + 15;
	}
	dispEntry(4, dispCalcZ(&pos2), 2, effHoraoSpellDisp, effect);
}

// Inlines a per-vertex table copy + transform; retail lowers the packed s16<->f32 conversion through
// GQR5 (psq_l/psq_st), which plain C cannot produce, so this lowers to lha/fctiwz instead.
static inline void effHoraoSpellSetVtx(effHoraoSpellVtx* vtx, effHoraoSpellVtx* src, Mtx mtx) {
	Vec v;

	memcpy(vtx, src, sizeof(effHoraoSpellVtx));
	v.x = vtx->pos[0];
	v.y = vtx->pos[1];
	v.z = vtx->pos[2];
	PSMTXMultVec(mtx, &v, &v);
	vtx->pos[0] = v.x;
	vtx->pos[1] = v.y;
	vtx->pos[2] = v.z;
}

// NONMATCHING (73.03%): retail converts the vertex s16 positions through psq_l/psq_st (GQR5); plain C
// lowers to lha/int->float and fctiwz, so the vertex loop and everything downstream of it differ.
void effHoraoSpellDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	CameraEntry* cam = camGetPtr(camId);
	effHoraoSpellData* data = effect->userdata;
	effHoraoSpellVtx* vtx;
	smartEntry* entry;
	Mtx transMtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	Mtx resultMtx;
	Mtx baseMtx;
	GXTexObj texObj;
	GXColor color;
	s32 counter;
	s32 i;
	s32 j;
	s32 angle;
	s32 colorIdx;
	f32 scale;

	counter = data->field_0x14;
	PSMTXTrans(transMtx, data->x, data->y, data->z);
	PSMTXRotRad(rotMtx, 'y', float_deg2rad_8041f15c * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(transMtx, rotMtx, transMtx);
	scale = data->field_0x38;
	PSMTXScale(scaleMtx, scale, scale, scale);
	PSMTXConcat(transMtx, scaleMtx, transMtx);
	PSMTXConcat(cam->view, transMtx, resultMtx);

	color = unk_80423150;
	color.r = data->field_0x18;
	color.g = data->field_0x1C;
	color.b = data->field_0x20;
	color.a = data->field_0x24;
	GXSetTevColor(GX_TEVREG0, color);
	color = unk_80423154;
	color.r = data->field_0x28;
	color.g = data->field_0x2C;
	color.b = data->field_0x30;
	color.a = data->field_0x34;
	GXSetTevColor(GX_TEVREG1, color);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTevStages(2);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_RASA);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_C0, GX_CC_C1, GX_CC_CPREV, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);
	effGetTexObjN64(0x73, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	PSMTXScale(scaleMtx, float_0p0078125_8041f160, float_0p03125_8041f164, float_0_8041f168);
	GXLoadTexMtxImm(scaleMtx, GX_TEXMTX0, GX_MTX2x4);
	GXSetCullMode(GX_CULL_NONE);

	angle = -0x11A;
	for (i = 0; i < 2; i++, angle += 0x24) {
		colorIdx = (counter + 4 * i) % 17;
		PSMTXRotRad(rotMtx, 'z', float_deg2rad_8041f15c * (f32)angle);
		PSMTXConcat(resultMtx, rotMtx, transMtx);
		GXLoadPosMtxImm(transMtx, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		color = unk_80423158;
		color.r = colr[colorIdx];
		color.g = colg[colorIdx];
		color.b = colb[colorIdx];
		color.a = data->field_0x24;
		GXSetTevColor(GX_TEVREG0, color);
		color = *(GXColor*)&dat_8041f158;
		GXSetTevColor(GX_TEVREG1, color);
		PSMTXScale(scaleMtx, float_0p0078125_8041f160, float_0p03125_8041f164, float_0_8041f168);
		PSMTXTrans(transMtx, -2.0f * (f32)counter, float_0_8041f168, float_0_8041f168);
		PSMTXConcat(scaleMtx, transMtx, scaleMtx);
		GXLoadTexMtxImm(scaleMtx, GX_TEXMTX0, GX_MTX2x4);
		PSMTXRotRad(rotMtx, 'x', float_0p34907_8041f174);
		PSMTXTrans(transMtx, float_6_8041f178, float_0_8041f168, float_0_8041f168);
		PSMTXConcat(rotMtx, transMtx, transMtx);
		PSMTXRotRad(rotMtx, 'y', float_deg2rad_8041f15c * (float_neg30_8041f180 * (f32)sin(float_6p2832_8041f16c * (float_78p75_8041f170 + (f32)(counter * 8)) / float_360_8041f17c)));
		PSMTXConcat(transMtx, rotMtx, baseMtx);
		entry = smartAlloc(0x1C0, 3);
		vtx = (effHoraoSpellVtx*)entry->address;
		for (j = 0; j < 16; j++) {
			PSMTXTrans(transMtx, float_8_8041f184, float_0_8041f168, float_0_8041f168);
			PSMTXRotRad(rotMtx, 'y', float_deg2rad_8041f15c * (float_30_8041f188 * (f32)sin(float_6p2832_8041f16c * (f32)(counter * 8 + (j * 720) / 15) / float_360_8041f17c)));
			PSMTXConcat(transMtx, rotMtx, transMtx);
			PSMTXRotRad(rotMtx, 'z', float_0p10472_8041f18c);
			PSMTXConcat(transMtx, rotMtx, transMtx);
			scale = float_0p01_8041f194 * (f32)j + float_1_8041f190;
			PSMTXScale(scaleMtx, scale, scale, scale);
			PSMTXConcat(transMtx, scaleMtx, transMtx);
			PSMTXConcat(baseMtx, transMtx, baseMtx);
			effHoraoSpellSetVtx(&vtx[2 * j], &horao_spell_vtx[2 * j], baseMtx);
			effHoraoSpellSetVtx(&vtx[2 * j + 1], &horao_spell_vtx[2 * j + 1], baseMtx);
		}
		DCFlushRange(vtx, 0x1C0);
		GXInvalidateVtxCache();
		effSetVtxDescN64((effVtxArray*)vtx);
		{
			s32 k = 0;
			for (j = 0; j < 15; j++, k += 2) {
				GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
				tri2(k, k + 2, k + 1, k, k + 1, k + 2, k + 3, k);
			}
		}
	}
}
