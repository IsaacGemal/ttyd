#include "eff/n64/eff_timestamp_n64.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os/OSCache.h>
#include <math.h>

typedef struct effTimestampN64Data {
	s32 type;            // 0x0
	f32 x;               // 0x4
	f32 y;               // 0x8
	f32 z;               // 0xC
	s32 field_0x10;      // 0x10
	s32 field_0x14;      // 0x14
	s32 field_0x18;      // 0x18
	s32 field_0x1c;      // 0x1C
	s32 field_0x20;      // 0x20
	s32 field_0x24;      // 0x24
	f32 scale;           // 0x28
	s32 field_0x2c;      // 0x2C
	f32 field_0x30;      // 0x30
	f32 posX[10];        // 0x34
	f32 posY[10];        // 0x5C
	f32 posZ[10];        // 0x84
	f32 rotX[10];        // 0xAC
	f32 rotY[10];        // 0xD4
	f32 rotZ[10];        // 0xFC
	s32 field_0x124[10]; // 0x124
	s32 field_0x14c[10]; // 0x14C
} effTimestampN64Data;

// .rodata / .sdata2 constants (float constants referenced through the linker)
const f64 double_to_int_802fb8c8 = 4503601774854144.0;
const f64 double_to_int_mask_802fb8d0 = 4503599627370496.0;

// .sdata local tables used only by effTimestampDisp.
static u8 scale_data[4] = { 0x78, 0x50, 0x28, 0x3C };
static u8 colr[20] = {
	0xFF, 0xFF, 0xFF, 0xC6, 0x8A, 0x82, 0x82, 0x82, 0x82, 0x82,
	0xAF, 0xD7, 0xFA, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
};
static u8 colg[20] = {
	0x82, 0x82, 0x82, 0x82, 0x82, 0xA5, 0xD5, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xE1, 0xC6, 0x9F, 0x82, 0x00, 0x00, 0x00,
};
static u8 colb[20] = {
	0x82, 0xC4, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xCA, 0x90,
	0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x00, 0x00, 0x00,
};


// .rodata
const char str_802FB8D8[16] = "TimestampN64";

// .data: ten vertices, { x, y, timer } each
s32 pos_data[10][3] = {
	{ -50, -70, 5 },
	{ 80, -90, 20 },
	{ 20, -50, 45 },
	{ -80, 30, 10 },
	{ 30, 0, 35 },
	{ 120, -30, 50 },
	{ 10, 50, 30 },
	{ 100, 30, 15 },
	{ 40, 100, 40 },
	{ 140, 90, 25 },
};

EffectEntry* effTimestampN64Entry(s32 type, s32 time, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effTimestampN64Data* data;
	s32 i;

	effect = effEntry();
	effect->type = str_802FB8D8;
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effTimestampN64Data));
	effect->userdata = data;
	effect->callback = effTimestampMain;
	effect->flags |= 2;
	data->type = type;
	data->field_0x14 = 0;
	if (time <= 0) {
		data->field_0x10 = 1000;
	} else {
		data->field_0x10 = time;
	}
	data->field_0x24 = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->field_0x18 = 70;
	data->field_0x1c = 180;
	data->field_0x20 = 120;
	data->field_0x2c = 0;
	data->field_0x30 = float_1_804200d4;

	for (i = 0; i < 10; i++) {
		data->posX[i] = (f32)pos_data[i][0];
		data->posY[i] = (f32)pos_data[i][1];
		data->posZ[i] = float_0_804200b8;
		data->rotX[i] = float_0_804200b8;
		data->rotY[i] = float_0_804200b8;
		data->rotZ[i] = float_0_804200b8;
		data->field_0x124[i] = 0;
		data->field_0x14c[i] = pos_data[i][2];
	}
	return effect;
}

void effTimestampMain(EffectEntry* effect) {
	effTimestampN64Data* data = effect->userdata;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch and copies it to a second local before
	   dispCalcZ; a single local folds the copy away. */
	Vec position;
#endif
	Vec disp = { 0.0f, 0.0f, 0.0f };
	s32 life;
	s32 counter;
	s32 i;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
#ifdef MUST_MATCH
	position = disp;
#endif

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->field_0x10 = 64;
	}
	if (data->field_0x10 < 1000) {
		data->field_0x10--;
	}
	data->field_0x14++;
	life = data->field_0x10;
	if (life < 0) {
		effDelete(effect);
		return;
	}
	counter = data->field_0x14;
	if (life < 64) {
		data->field_0x30 -= 0.05f;
		if (data->field_0x30 < 0.0f) {
			data->field_0x30 = 0.0f;
		}
	}
	if (life < 32) {
		data->field_0x2c += 0x80;
		if (data->field_0x2c > 255) {
			data->field_0x2c = 255;
		}
		data->field_0x30 = 0.0f;
	}
	if (life < 16) {
		data->field_0x24 = life << 4;
	}
	if (counter < 16) {
		data->field_0x24 = (counter << 4) + 0xf;
	}
	for (i = 0; i < 10; i++) {
		if (data->field_0x14c[i] != 0) {
			data->field_0x14c[i]--;
			if (data->field_0x14c[i] != 0) {
				continue;
			}
		}
		data->field_0x124[i] += 8;
		if (data->field_0x124[i] > 255) {
			data->field_0x124[i] = 255;
		}
	}

#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effTimestampDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effTimestampDisp, effect);
#endif
}

// The two by-value GXColor builds live in helpers so the compiler gives their temporaries the
// same stack slots as the original (same shape as effFireDustSetColor/effFireDustSetVtxColor).
static inline void effTimestampSetColor(effTimestampN64Data* data) {
	GXColor color = unk_804232b0;
	color.a = data->field_0x2c;
	GXSetTevColor(GX_TEVREG1, color);
}

static inline void effTimestampSetVtxColor(effTimestampN64Data* data, s32 counter, s32 i) {
	GXColor color = unk_804232b4;
	color.r = colr[i];
	color.g = colg[i];
	color.b = colb[i];
	color.a = (counter * data->field_0x124[i]) / 255;
	GXSetTevColor(GX_TEVREG0, color);
}

#pragma opt_common_subs off // with CSE on MWCC folds the constants differently and the integer colouring drifts further
#pragma opt_lifetimes off // the default lifetime analysis picks a different, worse-matching register assignment
// The target's field_0x30 scale and the transform scale must be separate locals: splitting them fixed
// most of the colouring. The first-loop `data`/`i` pair is still swapped (ours data=r28 i=r26, target
// data=r26 i=r28), the first-loop FP constants are rotated (ours 360=f27 500=f28 200=f29 6.2832=f30
// double=f31, target 6.2832=f27 double=f28 200=f29 500=f30 360=f31), and the second loop swaps the
// colr/data pointers and the j/texT pair. Separate loop counters were tried: no change or worse.
// NONMATCHING (98.94%): instruction shapes and frame match; only the register colours differ (above).
void effTimestampDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	CameraEntry* cam = camGetPtr(camId);
	s32 i;
	s32 counter;
	s32 timer;
	s32 angleA;
	effTimestampN64Data* data;
	s32 offset;
	s32 angleB;
	effTimestampVtx* vtx;
	effTimestampVtx* base;
	s32 texT;
	Mtx trans;
	Mtx scale;
	Mtx rot;
	Mtx work;
	GXTexObj texObj;
	smartEntry* entry;
	s32 j;
	f32 scaleF;
	f32 fieldScale;
	s32 valA;
	s32 valB;


	data = effect->userdata;
	timer = data->field_0x14;
	counter = data->field_0x24;
	PSMTXTrans(trans, data->x, data->y, data->z);
	scaleF = data->scale;
	PSMTXScale(scale, scaleF, scaleF, scaleF);
	PSMTXConcat(trans, scale, work);
	PSMTXRotRad(rot, 'y', float_deg2rad_804200b0 * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(work, rot, work);
	PSMTXConcat(cam->view, work, work);
	GXSetNumChans(0);
	GXSetNumTevStages(2);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C0, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_ONE, GX_CC_A1, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);
	effGetTexObjN64(0x5A, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	effGetTexObjN64(0x5B, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP1);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	scaleF = float_0p015625_804200b4;
	PSMTXScale(scale, scaleF, scaleF, float_0_804200b8);
	GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
	effTimestampSetColor(data);
	entry = smartAlloc(0x1C0, HEAP_EFFECT);
	angleA = timer * 10;
	base = entry->address;
	offset = 0;
	vtx = base;
	angleB = angleA;
	texT = 0;
	for (i = 0; i < 16; i++, vtx += 2) {
		fieldScale = data->field_0x30;
		valA = (s32)(fieldScale * (float_500_804200c4 * (f32)sin(float_6p2832_804200bc * (f32)angleA / float_360_804200c0)));
		valB = (s32)(fieldScale * (float_200_804200c8 * (f32)sin(float_6p2832_804200bc * (f32)angleB / float_360_804200c0)));
		vtx[0].x = valA - 3200;
		vtx[0].y = valB + offset - 3000;
		vtx[0].z = 0;
		vtx[0].s = 0;
		vtx[0].t = texT;
		vtx[1].x = valA + 3200;
		vtx[1].y = valB + offset - 3000;
		vtx[1].z = 0;
		vtx[1].s = 0x800;
		vtx[1].t = texT;
		angleA += 60;
		angleB += 6;
		offset += 400;
		texT += 0x80;
	}
	DCFlushRange(base, 0x1C0);
	GXInvalidateVtxCache();
	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64((effVtxArray*)base);
	for (i = 0; i < 10; i++) {
		PSMTXTrans(trans, data->posX[i], data->posY[i], data->posZ[i]);
		scaleF = float_0p01_804200cc * (f32)scale_data[i % 4];
		PSMTXScale(scale, scaleF, scaleF, scaleF);
		PSMTXConcat(trans, scale, trans);
		PSMTXRotRad(rot, 'x', float_deg2rad_804200b0 * data->rotX[i]);
		PSMTXConcat(trans, rot, trans);
		PSMTXRotRad(rot, 'y', float_deg2rad_804200b0 * data->rotY[i]);
		PSMTXConcat(trans, rot, trans);
		PSMTXRotRad(rot, 'z', float_deg2rad_804200b0 * data->rotZ[i]);
		PSMTXConcat(trans, rot, trans);
		scaleF = float_0p01_804200cc;
		PSMTXScale(scale, scaleF, scaleF, scaleF);
		PSMTXConcat(trans, scale, trans);
		PSMTXConcat(work, trans, trans);
		GXLoadPosMtxImm(trans, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		effTimestampSetVtxColor(data, counter, i);
		texT = 0;
		for (j = 0; j < 15; j++, texT += 2) {
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
			tri2(texT, texT + 2, texT + 1, texT, texT + 1, texT + 2, texT + 3, texT);
		}
	}
}
#pragma opt_lifetimes reset
