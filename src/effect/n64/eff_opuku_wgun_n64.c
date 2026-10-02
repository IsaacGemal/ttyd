#include "eff/n64/eff_opuku_wgun_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os/OSCache.h>
#include <math.h>
#include <dolphin/os/OSFastCast.h>

// .rodata
const Vec vec3_802fb2c0 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_802fb2d0 = 4503601774854144.0;
const char str_802FB2D8[] = "OpukuWgunN64";

// .sdata2 float constants (named after their original addresses).
const u32 dat_8041f840 = 0xC8;
const f32 float_2_8041f844 = 2.0f;
const f32 float_0p5_8041f848 = 0.5f;
const f32 float_6_8041f84c = 6.0f;
const f32 float_1_8041f850 = 1.0f;
const f32 float_30_8041f854 = 30.0f;
const f32 float_6p2832_8041f858 = 6.2831855f;
const f32 float_360_8041f85c = 360.0f;
const f32 float_20_8041f860 = 20.0f;
const f32 float_0_8041f864 = 0.0f;
const f32 float_0p05_8041f868 = 0.05f;
const f32 float_0p015625_8041f86c = 0.015625f;
const f32 float_0p03125_8041f870 = 0.03125f;
const f32 float_180_8041f874 = 180.0f;
const f32 float_120_8041f878 = 120.0f;
const f32 float_255_8041f87c = 255.0f;
const f32 float_10_8041f880 = 10.0f;

// .sdata alpha table (non-const: MWCC places it in .sdata, not .sdata2).
static u8 a_data[12] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC8, 0x80, 0x20 };

// .sbss2 scratch colour (same pattern as eff_opuku_wcurtain_n64.c's unk_804231e0).
GXColor unk_804231e8;

// Local copy of system.c's inline helper so it inlines here (as in eff_kemuri11.c).
inline Vec initializeVec(Vec* input) {
	Vec temp = vec3_802fb2c0;
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}
EffectEntry* effOpukuWgunN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 endX, f32 endY, f32 endZ, f32 scale) {
	EffectEntry* effect;
	EffOpukuWgunData* data;
	s32 i;

	effect = effEntry();
	effect->type = str_802FB2D8;
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(EffOpukuWgunData));
	effect->userdata = data;
	effect->callback = effOpukuWgunMain;
	effect->flags |= 2;

	data->type = type;
	data->frame = 0;
	if (duration <= 0) {
		data->timer = 1000;
	} else if (type == 0) {
		data->timer = duration * 2;
	} else {
		data->timer = duration * 4;
	}
	data->duration = duration;
	if (type == 0) {
		data->field_0x40 = 200;
	} else {
		data->field_0x40 = 255;
	}
	data->start.x = x;
	data->start.y = y;
	data->start.z = z;
	data->end.x = endX;
	data->end.y = endY;
	data->end.z = endZ;
	data->scale = scale;
	data->field_0x34 = 210;
	data->field_0x38 = 230;
	data->field_0x3C = 255;
	data->field_0x44 = 0;
	data->field_0x48 = 30;
	data->field_0x4C = 205;
	data->field_0x54 = 0;
	data->position.x = data->start.x;
	data->position.y = data->start.y;
	data->position.z = data->start.z;

	for (i = 0; i < 12; i++) {
		data->field_0x58[i] = 10.0f;
		data->field_0x88[i] = 2.0f;
		data->field_0xB8[i] = 0.0f;
		if (type == 0) {
			data->counter[i] = i + 1;
		} else {
			data->counter[i] = i * 5 + 1;
		}
		data->x[i] = data->position.x;
		data->y[i] = data->position.y;
		data->z[i] = data->position.z;
		data->counter2[i] = 0;
	}

	return effect;
}
void effOpukuWgunMain(EffectEntry* effect) {
	EffOpukuWgunData* data = effect->userdata;
	Vec disp = initializeVec(&data->position);
	s32 i;
	s32 type = data->type;
	s32 duration;
	f32 t;
	s32 value;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->timer = 0x10;
	}
	if (data->timer < 1000) {
		data->timer--;
	}
	data->frame++;
	if (data->timer < 0) {
		effDelete(effect);
		return;
	}
	duration = data->duration;
	data->field_0x54++;
	if (data->field_0x54 > 12) {
		data->field_0x54 = 12;
	}

	for (i = 0; i < 12; i++) {
		if (data->counter[i] != 0) {
			data->counter[i]--;
			if (data->counter[i] != 0) {
				continue;
			}
		}
		data->counter2[i]++;
		value = data->counter2[i];
		if (value >= duration) {
			value = duration;
		}
		t = (f32)value / (f32)duration;
		if (type == 0) {
			data->x[i] = data->start.x + t * (data->end.x - data->start.x);
			data->y[i] = data->start.y + t * (data->end.y - data->start.y);
			data->z[i] = data->start.z + t * (data->end.z - data->start.z);
		}
		else {
			data->x[i] = data->start.x + t * (data->end.x - data->start.x);
			data->y[i] = data->start.y + t * (data->end.y - data->start.y)
			           + float_120_8041f878 * (f32)sin(float_6p2832_8041f858 * (float_180_8041f874 * t) / float_360_8041f85c);
			data->z[i] = data->start.z + t * (data->end.z - data->start.z);
			data->alpha[i] = (u8)(float_255_8041f87c - float_255_8041f87c * t);
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effOpukuWgunDisp, effect);
}

// Control flow, constants, both TEV branches, the transform loop and the triangle loop all line up. The GQR2-5
// setup retail runs before the s16->f32 reads and before the f32->s16 writes is reproduced with OSInitFastCast
// (the SDK inline asm), so those blocks match. The remaining difference is the conversion itself: retail
// reads/writes the transformed vertex through paired-single psq_l/psq_st (GQR5, S16) while MWCC 2.6 lowers
// `f32 v = vtx->x` / `vtx->x = (s16)v` to the xoris/lfd/fsubs and fctiwz/sth integer path. That changes the
// transform loop, the saved FPR set (retail keeps f21-f31, we keep f25-f31) and the 0x190 vs 0x140 frame,
// which shifts every local slot and register assignment. No source form in this toolchain emits the psq path
// (see eff_balloon_n64.c / eff_flame_n64.c); game code may not use inline asm, so this cannot reach 100%.
// NONMATCHING (87.93%): transform loop uses lha/fctiwz instead of retail's psq_l/psq_st; the OSInitFastCast
// GQR blocks match.
void effOpukuWgunDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	CameraEntry* cam = camGetPtr(camId);
	EffOpukuWgunData* data = effect->userdata;
	OpukuWgunVtx* vtx;
	smartEntry* entry;
	GXTexObj texObj;
	Mtx scaleMtx;
	Vec v;
	GXColor color;
	f32 angle = 0.0f;
	f32 scale;
	f32 t;
	f32 s;
	f32 c;
	s32 type;
	u8 alpha;
	s32 i;
	s32 j;
	s32 texS;
	s32 x;

	type = data->type;
	scale = data->scale;

	color = unk_804231e8;
	color.r = data->field_0x34;
	color.g = data->field_0x38;
	color.b = data->field_0x3C;
	color.a = data->field_0x40;
	GXSetTevColor(GX_TEVREG0, color);

	color = *(GXColor*)&dat_8041f840;
	color.r = data->field_0x44;
	color.g = data->field_0x48;
	color.b = data->field_0x4C;
	GXSetTevColor(GX_TEVREG1, color);

	GXSetCullMode(GX_CULL_NONE);

	entry = smartAlloc(0x2A0, HEAP_EFFECT);
	vtx = (OpukuWgunVtx*)entry->address;
	texS = 0;

	for (i = 0; i < 12; i++) {
		if (i == 0) {
			angle = -angleABf(data->y[i + 1], -data->x[i + 1], data->y[i], -data->x[i]);
		} else {
			if (i != 11 && data->counter[i] == 0) {
				angle = -angleABf(data->y[i + 1], -data->x[i + 1], data->y[i], -data->x[i]);
			}
		}
		if (data->type == 0) {
			t = float_0p5_8041f848 * (f32)data->counter2[i] + float_2_8041f844;
			if (t > float_6_8041f84c) {
				t = float_6_8041f84c;
			}
			alpha = a_data[i];
		} else {
			t = float_0p5_8041f848 * (f32)data->counter2[i] + float_1_8041f850;
			if (t > float_30_8041f854) {
				t = float_30_8041f854;
			}
			alpha = a_data[i] * data->alpha[i] / 255;
		}
		t *= scale;
		angle = float_6p2832_8041f858 * angle / float_360_8041f85c;
		s = t * (f32)sin(angle);
		c = t * (f32)cos(angle);
		vtx[2 * i].x = (s16)(float_20_8041f860 * (data->x[i] + s));
		vtx[2 * i].y = (s16)(float_20_8041f860 * (data->y[i] + c));
		vtx[2 * i].z = (s16)(float_20_8041f860 * (data->z[i] + float_0_8041f864));
		vtx[2 * i].s = texS;
		vtx[2 * i].t = 0;
		vtx[2 * i].r = (i & 1) * 255;
		vtx[2 * i].g = ((i >> 1) & 1) * 255;
		vtx[2 * i].b = ((i >> 2) & 1) * 255;
		vtx[2 * i].a = alpha;
		vtx[2 * i + 1].x = (s16)(float_20_8041f860 * (data->x[i] - s));
		vtx[2 * i + 1].y = (s16)(float_20_8041f860 * (data->y[i] - c));
		vtx[2 * i + 1].z = (s16)(float_20_8041f860 * (data->z[i] + float_0_8041f864));
		vtx[2 * i + 1].s = texS;
		vtx[2 * i + 1].t = 0x400;
		vtx[2 * i + 1].r = (i & 1) * 255;
		vtx[2 * i + 1].g = ((i >> 1) & 1) * 255;
		vtx[2 * i + 1].b = ((i >> 2) & 1) * 255;
		vtx[2 * i + 1].a = alpha;
		texS += 0x140;
	}

	PSMTXScale(scaleMtx, float_0p05_8041f868, float_0p05_8041f868, float_0p05_8041f868);

	for (j = 0; j < i * 2; j++) {
		OSInitFastCast();
		v.x = vtx[j].x;
		v.y = vtx[j].y;
		v.z = vtx[j].z;
		PSMTXMultVec(scaleMtx, &v, &v);
		OSInitFastCast();
		vtx[j].x = (s16)v.x;
		vtx[j].y = (s16)v.y;
		vtx[j].z = (s16)v.z;
	}

	DCFlushRange(vtx, 0x2A0);
	GXInvalidateVtxCache();
	effSetVtxDescN64((effVtxArray*)vtx);
	GXLoadPosMtxImm(cam->view, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);

	if (type == 0) {
		GXSetNumChans(1);
		GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
		GXSetNumTevStages(2);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C1, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
		GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
		GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
		GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);
		GXSetNumTexGens(1);
		GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
		PSMTXScale(scaleMtx, float_0p015625_8041f86c, float_0p03125_8041f870, float_0_8041f864);
		GXLoadTexMtxImm(scaleMtx, GX_TEXMTX0, GX_MTX2x4);
		effGetTexObjN64(0x8D, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
	} else {
		GXSetNumChans(1);
		GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
		GXSetNumTevStages(2);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C0, GX_CC_TEXC, GX_CC_RASA, GX_CC_ZERO);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
		GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
		GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_C1, GX_CC_C0, GX_CC_CPREV, GX_CC_ZERO);
		GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
		GXSetNumTexGens(1);
		GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
		PSMTXScale(scaleMtx, float_0p03125_8041f870, float_0p03125_8041f870, float_0_8041f864);
		GXLoadTexMtxImm(scaleMtx, GX_TEXMTX0, GX_MTX2x4);
		effGetTexObjN64(0x8E, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
	}

	for (j = 0, x = 0; j < i - 1; j++, x += 2) {
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
		tri2(x, x + 2, x + 1, x, x + 1, x + 2, x + 3, x);
	}
}
