#include "eff/n64/eff_coin_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

typedef struct effCoinData {
	s32 field_0x0;  // 0x0
	s32 anim;       // 0x4
	Vec position;   // 0x8
	s32 counter;    // 0x14
	s32 frame;      // 0x18
	f32 field_0x1C; // 0x1C
	s32 timer;      // 0x20
} effCoinData;

// Per-coin spawn template (5 words, stride 0x14).
typedef struct effCoinN64Spawn {
	s32 timer; // 0x0
	s32 x;     // 0x4
	s32 y;     // 0x8
	s32 z;     // 0xC
	s32 scale; // 0x10
} effCoinN64Spawn;

static effCoinN64Spawn _data[4] = {
	{ 2, 0, 0, 0, 100 },
	{ 4, 4, -2, 0, 100 },
	{ 6, -2, 2, 0, 70 },
	{ 8, 2, 4, 0, 40 },
};

// .data: one effVtxArray (0x38 bytes) for the textured quad.
static u32 size16x16_tex32x32_vtx[0x38 / 4] = {
	0xFFF8FFF8, 0x00000000, 0x00000000, 0x00FF0008,
	0xFFF80000, 0x04000000, 0x000000FF, 0x00080008,
	0x00000400, 0x04000000, 0x00FFFFF8, 0x00080000,
	0x00000400, 0x000000FF,
};

EffectEntry* effCoinN64Entry(s32 a, s32 b, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effCoinData* data;
	effCoinData* p;
	effCoinN64Spawn* src;
	s32 i;

	effect = effEntry();
	effect->type = "CoinN64";
	effect->count = 5;
	data = __memAlloc(HEAP_EFFECT, 0xb4);
	effect->userdata = data;
	effect->callback = effCoinMain;
	data->field_0x0 = a;
	data->frame = 0;
	data->counter = 14;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	p = data + 1;
	for (i = 1; i < 5; i++, p++) {
		src = &_data[i - 1];
		p->position.x = (f32)src->x;
		p->position.y = (f32)src->y;
		p->position.z = (f32)src->z;
		p->anim = -1;
		p->timer = src->timer;
		p->field_0x1C = scale * (f32)src->scale * 0.01f;
	}
	return effect;
}

void effCoinMain(EffectEntry* effect) {
	effCoinData* data = effect->userdata;
	Vec position;
	Vec disp = { 0.0f, 0.0f, 0.0f };
	s32 i;

	disp.x = data->position.x;
	disp.y = data->position.y;
	disp.z = data->position.z;
	position = disp;

	data->counter--;
	data->frame++;
	if (data->counter < 0) {
		effDelete(effect);
		return;
	}

	for (i = 1, data++; i < effect->count; i++, data++) {
		if (data->timer > 0) {
			data->timer--;
			if (data->timer > 0) {
				continue;
			}
		}
		data->anim++;
		if (data->anim >= 10) {
			data->timer = -1;
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effCoinDisp, effect);
}

void effCoinDisp(CameraId camId, void* param) {
	EffectEntry* effect;
	effCoinData* data;
	s32 i;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx mtx0;
	Mtx scaleMtx;
	Mtx mtx1;
	Mtx mtx2;
	f32 scale;

	effect = param;
	cam = camGetPtr(camId);
	data = effect->userdata;
	PSMTXTrans(mtx0, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(mtx1, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(mtx0, mtx1, mtx2);
	GXSetNumChans(0);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_TEXC, GX_CC_ZERO, GX_CC_C0, GX_CC_C0);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	effGetTexObjN64(0x84, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64(size16x16_tex32x32_vtx);
	GXSetTevColor(GX_TEVREG0, (GXColor){ 0xFF, 0xFF, 0x0F, 0xFF });

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		if (data->timer != 0) {
			continue;
		}
		scale = data->field_0x1C;
		PSMTXTrans(mtx0, data->position.x, data->position.y, data->position.z);
		if (1.0f != scale) {
			PSMTXScale(scaleMtx, scale, scale, 1.0f);
			PSMTXConcat(mtx0, scaleMtx, mtx0);
		}
		PSMTXConcat(mtx2, mtx0, mtx0);
		PSMTXConcat(cam->view, mtx0, mtx0);
		GXLoadPosMtxImm(mtx0, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		PSMTXScale(scaleMtx, 0.03125f, 0.00625f, 0.0f);
		PSMTXTrans(mtx0, 0.0f, 32.0f * (f32)(4 - (data->anim >> 1)), 0.0f);
		PSMTXConcat(scaleMtx, mtx0, scaleMtx);
		GXLoadTexMtxImm(scaleMtx, GX_TEXMTX0, GX_MTX2x4);
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
	}
}
