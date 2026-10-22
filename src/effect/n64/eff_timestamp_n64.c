#include "eff/n64/eff_timestamp_n64.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os/OSCache.h>
#define _MATH_H_ // keep the SDK math.h (sqrt inlines and their pooled doubles) out of this unit
#include "math_ext.h"

typedef struct effTimestampN64Data {
	s32 type;           // 0x0
	f32 x;              // 0x4
	f32 y;              // 0x8
	f32 z;              // 0xC
	s32 life;           // 0x10
	s32 timer;          // 0x14
	s32 colorR;         // 0x18, set to 70/180/120 and not read in this unit
	s32 colorG;         // 0x1C
	s32 colorB;         // 0x20
	s32 alpha;          // 0x24
	f32 scale;          // 0x28
	s32 whiteFade;      // 0x2C, GX_TEVREG1 alpha: TEV stage 1 lerps the colour toward white by it
	f32 waveScale;      // 0x30
	f32 posX[10];       // 0x34
	f32 posY[10];       // 0x5C
	f32 posZ[10];       // 0x84
	f32 rotX[10];       // 0xAC
	f32 rotY[10];       // 0xD4
	f32 rotZ[10];       // 0xFC
	s32 pieceAlpha[10]; // 0x124, per-piece GX_TEVREG0 alpha, scaled by alpha
	s32 delay[10];      // 0x14C, frames before the piece starts fading in
} effTimestampN64Data;

// .rodata / .sdata2 constants (float constants referenced through the linker)

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
	effect->type = "TimestampN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effTimestampN64Data));
	effect->userdata = data;
	effect->callback = effTimestampMain;
	effect->flags |= 2;
	data->type = type;
	data->timer = 0;
	if (time <= 0) {
		data->life = 1000;
	} else {
		data->life = time;
	}
	data->alpha = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->colorR = 70;
	data->colorG = 180;
	data->colorB = 120;
	data->whiteFade = 0;
	data->waveScale = 1.0f;

	for (i = 0; i < 10; i++) {
		data->posX[i] = (f32)pos_data[i][0];
		data->posY[i] = (f32)pos_data[i][1];
		data->posZ[i] = 0.0f;
		data->rotX[i] = 0.0f;
		data->rotY[i] = 0.0f;
		data->rotZ[i] = 0.0f;
		data->pieceAlpha[i] = 0;
		data->delay[i] = pos_data[i][2];
	}
	return effect;
}

static inline Vec makeVec(f32 x, f32 y, f32 z) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = x;
	temp.y = y;
	temp.z = z;
	return temp;
}

void effTimestampMain(EffectEntry* effect) {
	effTimestampN64Data* data = effect->userdata;
	Vec disp = makeVec(data->x, data->y, data->z);
	s32 life;
	s32 counter;
	s32 i;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->life = 64;
	}
	if (data->life < 1000) {
		data->life--;
	}
	data->timer++;
	life = data->life;
	if (life < 0) {
		effDelete(effect);
		return;
	}
	counter = data->timer;
	if (life < 64) {
		data->waveScale -= 0.05f;
		if (data->waveScale < 0.0f) {
			data->waveScale = 0.0f;
		}
	}
	if (life < 32) {
		data->whiteFade += 0x80;
		if (data->whiteFade > 255) {
			data->whiteFade = 255;
		}
		data->waveScale = 0.0f;
	}
	if (life < 16) {
		data->alpha = life << 4;
	}
	if (counter < 16) {
		data->alpha = (counter << 4) + 0xf;
	}
	for (i = 0; i < 10; i++) {
		if (data->delay[i] != 0) {
			data->delay[i]--;
			if (data->delay[i] != 0) {
				continue;
			}
		}
		data->pieceAlpha[i] += 8;
		if (data->pieceAlpha[i] > 255) {
			data->pieceAlpha[i] = 255;
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effTimestampDisp, effect);
}

// The vertex loop is written per index (`&base[i * 2]`, `i * 400`, `timer * 10 + i * k`), so the
// frontend strength-reduces it into the original's preheader copies; the texture row `i * 2 * 0x40U`
// is unsigned, so the backend reduces it instead and its zero start becomes a copy of the `i * 400`
// one. Each sin result goes into the named `s` temp, which gives the loop's constants f27-f31.
void effTimestampDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	s32 j;
	smartEntry* entry;
	s32 counter;
	effTimestampN64Data* data;
	s32 i;
	s32 k;
	s32 timer;
	s32 valA;
	effTimestampVtx* base;
	CameraEntry* cam = camGetPtr(camId);
	Mtx trans;
	Mtx scale;
	Mtx rot;
	Mtx work;
	GXTexObj texObj;
	f32 scaleF;
	f32 fieldScale;
	s32 valB;

	data = effect->userdata;
	timer = data->timer;
	counter = data->alpha;
	PSMTXTrans(trans, data->x, data->y, data->z);
	scaleF = data->scale;
	PSMTXScale(scale, scaleF, scaleF, scaleF);
	PSMTXConcat(trans, scale, work);
	PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
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
	scaleF = 0.015625f;
	PSMTXScale(scale, scaleF, scaleF, 0.0f);
	GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
	GXSetTevColor(GX_TEVREG1, (GXColor){ 0, 0, 0, data->whiteFade });
	entry = smartAlloc(0x1C0, HEAP_EFFECT);
	base = entry->address;
	for (i = 0; i < 16; i++) {
		effTimestampVtx* vp = &base[i * 2];
		f32 s;
		fieldScale = data->waveScale;
		s = (f32)sin(6.2831855f * (f32)(timer * 10 + i * 60) / 360.f);
		valA = (s32)(fieldScale * (500.f * s));
		s = (f32)sin(6.2831855f * (f32)(timer * 10 + i * 6) / 360.f);
		valB = (s32)(fieldScale * (200.f * s));
		vp[0].x = valA - 3200;
		vp[0].y = valB + i * 400 - 3000;
		vp[0].z = 0;
		vp[0].s = 0;
		vp[0].t = i * 2 * 0x40U;
		vp[1].x = valA + 3200;
		vp[1].y = valB + i * 400 - 3000;
		vp[1].z = 0;
		vp[1].s = 0x800;
		vp[1].t = i * 2 * 0x40U;
	}
	DCFlushRange(base, 0x1C0);
	GXInvalidateVtxCache();
	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64(base);
	for (i = 0; i < 10; i++) {
		PSMTXTrans(trans, data->posX[i], data->posY[i], data->posZ[i]);
		scaleF = 0.01f * (f32)scale_data[i % 4];
		PSMTXScale(scale, scaleF, scaleF, scaleF);
		PSMTXConcat(trans, scale, trans);
		PSMTXRotRad(rot, 'x', 0.017453292f * data->rotX[i]);
		PSMTXConcat(trans, rot, trans);
		PSMTXRotRad(rot, 'y', 0.017453292f * data->rotY[i]);
		PSMTXConcat(trans, rot, trans);
		PSMTXRotRad(rot, 'z', 0.017453292f * data->rotZ[i]);
		PSMTXConcat(trans, rot, trans);
		scaleF = 0.01f;
		PSMTXScale(scale, scaleF, scaleF, scaleF);
		PSMTXConcat(trans, scale, trans);
		PSMTXConcat(work, trans, trans);
		GXLoadPosMtxImm(trans, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		GXSetTevColor(GX_TEVREG0, (GXColor){ colr[i], colg[i], colb[i], (counter * data->pieceAlpha[i]) / 255 });
		for (k = 0, j = 0; k < 15; k++, j += 2) {
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
			tri2(j, j + 2, j + 1, j, j + 1, j + 2, j + 3, j);
		}
	}
}
