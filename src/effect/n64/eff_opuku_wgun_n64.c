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
static inline GXColor makeOpukuWgunColor1(u8 r, u8 g, u8 b, u8 a) {
	GXColor c = unk_804231e8;
	c.r = r; c.g = g; c.b = b; c.a = a;
	return c;
}
static inline GXColor makeOpukuWgunColor2p(GXColor c, u8 r, u8 g, u8 b) {
	c.r = r; c.g = g; c.b = b;
	return c;
}
// Building both tev colours through an inline (as eff_opuku_jetwrain does) makes mwcc keep each temporary
// in its own slot and copy it to a separate outgoing argument slot, producing retail's temp/arg pairs.
#endif
// The frame (0x190), the saved-register set (f21-f31, r22-r31), both TEV-colour blocks, the first vertex
// loop, the transform loop, the GX state and the triangle loop all line up. The GQR2-5 setup retail runs
// before the s16->f32 reads and before the f32->s16 writes is reproduced with OSInitFastCast, and the
// paired-single psq_l/psq_st conversions with the SDK's __OSs16tof32 / __OSf32tos16 inlines (plain casts
// would lower to lha/fctiwz). The `zero` local keeps `data->z[i] + 0.0f` from folding away, which lands the
// 0.0 in f31 and the 2pi/360/20/scale constants in retail's f28/f29/f30/f24; the `angle`/`rad` split gives
// angle f22, rad/s f23 and t f21; `alpha` is an s32 so a_data*data->alpha/255 stores with a plain stb (no
// clrlwi); and the triangle loop's `if (j >= 0)` guard reproduces retail's always-taken test. What still
// differs (every instruction opcode/operand matches): mwcc's integer colouring of the two vertex loops
// (retail keeps the walking vtx pointer in r23 and its base in r24, i in r26, texS in r28 and alpha in r22;
// we get base r26 / running r28, i r24, texS r23 and alpha r22 -- two independent register swaps), the
// first TEV colour's temporary slot (retail 0x1c/0x20, ours 0x10/0x1c) and the redundant `beq` retail emits
// before the i==0 `bne`. The f32->s16 temporaries now land 4 apart (OSFastCast.h's temp is an f32).
// NONMATCHING (98.28%): integer register colouring and the colour1 temp slot;
// every instruction opcode/operand matches.
void effOpukuWgunDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	CameraEntry* cam = camGetPtr(camId);
	EffOpukuWgunData* data = effect->userdata;
	OpukuWgunVtx* vtx;
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
	s32 type;
	s32 i;
	s32 j;
	s32 texS;
	s32 x;
	s32 alpha;

	type = data->type;
	scale = data->scale;

#ifdef MUST_MATCH
	GXSetTevColor(GX_TEVREG0, makeOpukuWgunColor1(data->field_0x34, data->field_0x38, data->field_0x3C, data->field_0x40));
	GXSetTevColor(GX_TEVREG1, makeOpukuWgunColor2p(*(GXColor*)&dat_8041f840, data->field_0x44, data->field_0x48, data->field_0x4C));
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
		rad = float_6p2832_8041f858 * angle / float_360_8041f85c;
		s = t * (f32)sin(rad);
		c = t * (f32)cos(rad);
		vtx[2 * i].x = (s16)(float_20_8041f860 * (data->x[i] + s));
		vtx[2 * i].y = (s16)(float_20_8041f860 * (data->y[i] + c));
		vtx[2 * i].z = (s16)(float_20_8041f860 * (data->z[i] + zero));
		vtx[2 * i].s = texS;
		vtx[2 * i].t = 0;
		vtx[2 * i].r = (i & 1) * 255;
		vtx[2 * i].g = ((i >> 1) & 1) * 255;
		vtx[2 * i].b = ((i >> 2) & 1) * 255;
		vtx[2 * i].a = alpha;
		vtx[2 * i + 1].x = (s16)(float_20_8041f860 * (data->x[i] - s));
		vtx[2 * i + 1].y = (s16)(float_20_8041f860 * (data->y[i] - c));
		vtx[2 * i + 1].z = (s16)(float_20_8041f860 * (data->z[i] + zero));
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
		OW_VEC->x = __OSs16tof32(&vtx[j].x);
		OW_VEC->y = __OSs16tof32(&vtx[j].y);
		OW_VEC->z = __OSs16tof32(&vtx[j].z);
		PSMTXMultVec(scaleMtx, OW_VEC, OW_VEC);
		OSInitFastCast();
		vtx[j].x = __OSf32tos16(OW_VEC->x);
		vtx[j].y = __OSf32tos16(OW_VEC->y);
		vtx[j].z = __OSf32tos16(OW_VEC->z);
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
		if (j >= 0) {
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
			tri2(x, x + 2, x + 1, x, x + 1, x + 2, x + 3, x);
		}
	}
}
