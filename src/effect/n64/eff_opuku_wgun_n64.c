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

#ifdef MUST_MATCH
// Build each colour and make the GXSetTevColor call inside the inline: mwcc then places each colour local
// and its by-value copy in the original's stack slots (the eff_opuku_wprotect pattern). Returning the GXColor
// and calling outside packs all locals together instead.
static inline void opukuWgunColor1(u8 r, u8 g, u8 b, u8 a) {
	GXColor c = unk_804231e8;
	c.r = r; c.g = g; c.b = b; c.a = a;
	GXSetTevColor(GX_TEVREG0, c);
}
static inline void opukuWgunColor2(u8 r, u8 g, u8 b) {
	GXColor c = *(GXColor*)&dat_8041f840;
	c.r = r; c.g = g; c.b = b;
	GXSetTevColor(GX_TEVREG1, c);
}
#endif
// The frame (0x190), the saved-register set (f21-f31, r22-r31), both TEV-colour blocks, the first vertex
// loop, the transform loop, the GX state and the triangle loop all line up. The GQR2-5 setup retail runs
// before the s16->f32 reads and before the f32->s16 writes is reproduced with OSInitFastCast, and the
// paired-single psq_l/psq_st conversions with the SDK's __OSs16tof32 / __OSf32tos16 inlines (plain casts
// would lower to lha/fctiwz). The `zero` local keeps `data->z[i] + 0.0f` from folding away, which lands the
// 0.0 in f31 and the 2pi/360/20/scale constants in retail's f28/f29/f30/f24; the `angle`/`rad` split gives
// angle f22, rad/s f23 and t f21; `alpha` is an s32 so a_data*data->alpha/255 stores with a plain stb (no
// clrlwi); and the triangle loop's `if (j >= 0)` guard reproduces retail's always-taken test. The two TEV
// colours are built inside the inline (the eff_opuku_wprotect pattern) so each colour local and its by-value
// copy land in retail's stack slots (temp/arg 0x1c/0x20 and 0x14/0x18); returning the GXColor and calling
// outside packed them differently. What still differs (every instruction opcode/operand matches): mwcc's
// integer colouring of the two vertex loops -- retail keeps the walking vtx pointer in r23 and its base in
// r24, i in r26, texS in r28, alpha in r22, and the triangle loop's j in r24 / x in r23; we now get base r24
// and i r26 right (declaration order below), but the running pointer and texS are swapped (r28/r23) and the
// triangle loop's j/x are swapped (r23/r24) -- and retail emits a redundant `beq` before the i==0 `bne`
// (a branch-to-next-instruction) that we do not. The f32->s16 temporaries land 4 apart (OSFastCast.h's temp
// is an f32).
// NONMATCHING (99.56%): the running/texS and j/x register swaps plus retail's redundant `beq`;
// every instruction opcode/operand matches.
void effOpukuWgunDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	CameraEntry* cam = camGetPtr(camId);
	EffOpukuWgunData* data = effect->userdata;
	smartEntry* entry;
	GXTexObj texObj;
	Mtx scaleMtx;
	Vec v;
#define OW_VEC (&v)
	GXColor color;
	f32 scale;
	f32 t;
	f32 s;
	f32 angle;
	f32 rad;
	f32 zero = 0.0f;
	f32 c;
	s32 i;
	s32 type;
	OpukuWgunVtx* p;
	OpukuWgunVtx* vtx;
	s32 alpha;
	s32 x;
	s32 j;
	s32 m;

	type = data->type;
	scale = data->scale;

#ifdef MUST_MATCH
	opukuWgunColor1(data->field_0x34, data->field_0x38, data->field_0x3C, data->field_0x40);
	opukuWgunColor2(data->field_0x44, data->field_0x48, data->field_0x4C);
#else
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
#endif

	GXSetCullMode(GX_CULL_NONE);

	vtx = (OpukuWgunVtx*)smartAlloc(0x2A0, HEAP_EFFECT)->address;
	p = vtx;

	for (i = 0; i < 12; i++) {
		switch (i) {
		case 0:
			angle = -angleABf(data->y[i + 1], -data->x[i + 1], data->y[i], -data->x[i]);
			break;
		default:
			if (i != 11 && data->counter[i] == 0) {
				angle = -angleABf(data->y[i + 1], -data->x[i + 1], data->y[i], -data->x[i]);
			}
			break;
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
		rad = float_6p2832_8041f858 * angle / float_360_8041f85c;
		s = t * (f32)sin(rad);
		c = t * (f32)cos(rad);
		vtx[0].x = (s16)(float_20_8041f860 * (data->x[i] + s));
		vtx[0].y = (s16)(float_20_8041f860 * (data->y[i] + c));
		vtx[0].z = (s16)(float_20_8041f860 * (data->z[i] + zero));
		vtx[0].s = (i * 5) << 6;
		vtx[0].t = 0;
		vtx[0].r = (i & 1) * 255;
		vtx[0].g = ((i >> 1) & 1) * 255;
		vtx[0].b = ((i >> 2) & 1) * 255;
		vtx[0].a = alpha;
		vtx[1].x = (s16)(float_20_8041f860 * (data->x[i] - s));
		vtx[1].y = (s16)(float_20_8041f860 * (data->y[i] - c));
		vtx[1].z = (s16)(float_20_8041f860 * (data->z[i] + zero));
		vtx[1].s = (i * 5) << 6;
		vtx[1].t = 0x400;
		vtx[1].r = (i & 1) * 255;
		vtx[1].g = ((i >> 1) & 1) * 255;
		vtx[1].b = ((i >> 2) & 1) * 255;
		vtx[1].a = alpha;
		vtx += 2;
	}

	PSMTXScale(scaleMtx, float_0p05_8041f868, float_0p05_8041f868, float_0p05_8041f868);

	for (j = 0; j < i * 2; j++) {
		OSInitFastCast();
		OW_VEC->x = __OSs16tof32(&p[j].x);
		OW_VEC->y = __OSs16tof32(&p[j].y);
		OW_VEC->z = __OSs16tof32(&p[j].z);
		PSMTXMultVec(scaleMtx, OW_VEC, OW_VEC);
		OSInitFastCast();
		p[j].x = __OSf32tos16(OW_VEC->x);
		p[j].y = __OSf32tos16(OW_VEC->y);
		p[j].z = __OSf32tos16(OW_VEC->z);
	}

	DCFlushRange(p, 0x2A0);
	GXInvalidateVtxCache();
	effSetVtxDescN64((effVtxArray*)p);
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

	for (m = 0, x = 0; m < i - 1; x += 2, m++) {
		if (m >= 0) {
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
			tri2(x, x + 2, x + 1, x, x + 1, x + 2, x + 3, x);
		}
	}
}
