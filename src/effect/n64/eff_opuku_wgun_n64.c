#include "eff/n64/eff_opuku_wgun_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os/OSCache.h>
#include "math_ext.h"
#include <dolphin/os/OSFastCast.h>

// .rodata
const Vec vec3_802fb2c0 = { 0.0f, 0.0f, 0.0f };
// The compiler's int->float magic double (0x4330000080000000) and the "OpukuWgunN64"
// string literal are pooled by the compiler, in .text order; defining them here
// duplicates the pool (eff_opuku_wprotect_n64's recipe).

// .sdata2 leading word the TEV colour is copied from (real object, emitted before the pool).
const u32 dat_8041f840 = 0xC8;

// .sdata alpha table (non-const: MWCC places it in .sdata, not .sdata2).
static u8 a_data[12] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC8, 0x80, 0x20 };

// 4 zero bytes that end the unit's .sdata.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_80413BFC_sdata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset

// .sbss2 scratch colour (same pattern as eff_opuku_wprotect_n64.c's unk_804231f0).
// Declared gap first: MWCC lays uninitialised globals out in reverse declaration order.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor gap_10_804231EC_sbss2;
__declspec(section ".sdata") GXColor unk_804231e8;

// Local copy of system.c's inline helper so it inlines here (as in eff_kemuri11.c).
inline Vec initializeVec(Vec* input) {
	Vec temp = vec3_802fb2c0;
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}
// Build the sine in its own inline so mwcc pools 180 before 120: a call's argument
// constants pool after the other constants of the statement that holds the call, so the
// inline expression in effOpukuWgunMain pools 120 first and the .sdata2 order differs.
#ifdef MUST_MATCH
static inline f32 opukuWgunSin(f32 t) {
	return (f32)sin(6.2831855f * (180.0f * t) / 360.0f);
}
#endif
EffectEntry* effOpukuWgunN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 endX, f32 endY, f32 endZ, f32 scale) {
	EffectEntry* effect;
	EffOpukuWgunData* data;
	s32 i;

	effect = effEntry();
	effect->type = "OpukuWgunN64";
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
#ifdef MUST_MATCH
			data->y[i] = data->start.y + t * (data->end.y - data->start.y) + 120.0f * opukuWgunSin(t);
#else
			data->y[i] = data->start.y + t * (data->end.y - data->start.y)
			           + 120.0f * (f32)sin(6.2831855f * (180.0f * t) / 360.0f);
#endif
			data->z[i] = data->start.z + t * (data->end.z - data->start.z);
			data->alpha[i] = (u8)(255.0f - 255.0f * t);
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
// outside packed them differently. The f32->s16 temporaries land 4 apart (OSFastCast.h's temp is an f32).
// The dispatch's redundant `beq` is restored by the MUST_MATCH goto pair below. The triangle loop reuses
// `j` as its counter with `x = j * 2` in the body: MWCC strength-reduces that multiply to an induction
// variable initialised as `mr r23, r24` (not `li r23,0`), which is what puts the preheader in retail's
// order `li r24,0; subi r22; mr r23,r24`.
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
#ifdef MUST_MATCH
		// The original lowers the dispatch as two conditional branches from one compare (`beq` to
		// the case-0 body sitting immediately after, `bne` to the default), which a switch or an
		// if/else does not reproduce (they emit a bare `b` for the default). The two gotos keep
		// both comparisons, matching retail; the natural switch is under #else.
		if (i == 0) {
			goto opuku_then;
		}
		if (i != 0) {
			goto opuku_else;
		}
	opuku_then:
		angle = -angleABf(data->y[i + 1], -data->x[i + 1], data->y[i], -data->x[i]);
		goto opuku_done;
	opuku_else:
		if (i != 11 && data->counter[i] == 0) {
			angle = -angleABf(data->y[i + 1], -data->x[i + 1], data->y[i], -data->x[i]);
		}
	opuku_done:
		;
#else
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
#endif
		if (data->type == 0) {
			t = 0.5f * (f32)data->counter2[i] + 2.0f;
			if (t > 6.0f) {
				t = 6.0f;
			}
			alpha = a_data[i];
		} else {
			t = 0.5f * (f32)data->counter2[i] + 1.0f;
			if (t > 30.0f) {
				t = 30.0f;
			}
			alpha = a_data[i] * data->alpha[i] / 255;
		}
		t *= scale;
		rad = 6.2831855f * angle / 360.0f;
		s = t * (f32)sin(rad);
		c = t * (f32)cos(rad);
		vtx[0].x = (s16)(20.0f * (data->x[i] + s));
		vtx[0].y = (s16)(20.0f * (data->y[i] + c));
		vtx[0].z = (s16)(20.0f * (data->z[i] + zero));
		vtx[0].s = (i * 5) << 6;
		vtx[0].t = 0;
		vtx[0].r = (i & 1) * 255;
		vtx[0].g = ((i >> 1) & 1) * 255;
		vtx[0].b = ((i >> 2) & 1) * 255;
		vtx[0].a = alpha;
		vtx[1].x = (s16)(20.0f * (data->x[i] - s));
		vtx[1].y = (s16)(20.0f * (data->y[i] - c));
		vtx[1].z = (s16)(20.0f * (data->z[i] + zero));
		vtx[1].s = (i * 5) << 6;
		vtx[1].t = 0x400;
		vtx[1].r = (i & 1) * 255;
		vtx[1].g = ((i >> 1) & 1) * 255;
		vtx[1].b = ((i >> 2) & 1) * 255;
		vtx[1].a = alpha;
		vtx += 2;
	}

	PSMTXScale(scaleMtx, 0.05f, 0.05f, 0.05f);

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
		PSMTXScale(scaleMtx, 0.015625f, 0.03125f, 0.0f);
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
		PSMTXScale(scaleMtx, 0.03125f, 0.03125f, 0.0f);
		GXLoadTexMtxImm(scaleMtx, GX_TEXMTX0, GX_MTX2x4);
		effGetTexObjN64(0x8E, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
	}

	for (j = 0; j < i - 1; j++) {
		x = j * 2;
		if (j >= 0) {
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
			tri2(x, x + 2, x + 1, x, x + 1, x + 2, x + 3, x);
		}
	}
}
