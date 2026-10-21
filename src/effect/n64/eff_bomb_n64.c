#include "eff/n64/eff_bomb_n64.h"
#include "eff/eff_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "eff/n64/eff_confetti_n64.h"
#include "eff/n64/eff_kemuri8_n64.h"
#include "memory.h"
#include "system.h"

#include <dolphin/mtx.h>

// Data owned by the bomb effect. The comments give the offsets the original uses.
typedef struct effBombN64Data {
	s32 type;          // 0x00
	f32 x;             // 0x04
	f32 y;             // 0x08
	f32 z;             // 0x0C
	u8 pad_0x10[0x20]; // 0x10
	s32 life;          // 0x30: frames left before the effect deletes itself
	s32 counter;       // 0x34: frame counter
	u8 pad_0x38[0x4];  // 0x38
	s32 typeHigh;      // 0x3C: caller's type was >= 3 (selects the kemuri8 type + 3)
	s32 camId;         // 0x40
	s32 texType;       // 0x44: -1 when type 0 has no bomb texture
	u8 pad_0x48[0x1C]; // 0x48
	f32 scale;         // 0x64
	f32 scaleStep;     // 0x68
	u8 pad_0x6C[0x10]; // 0x6C
	s32 alpha;         // 0x7C: alpha of the bomb texture colour
	u8 pad_0x80[0x8];  // 0x80
	s32 field_0x88;    // 0x88
	u8 pad_0x8C[0x1C]; // 0x8C
	f32 animScale;     // 0xA8
	f32 animScaleStep; // 0xAC
	f32 animRotZ;      // 0xB0
	f32 animRotSpeed;  // 0xB4
	u8 pad_0xB8[0x8];  // 0xB8
	s32 animAlpha;     // 0xC0
	u8 pad_0xC4[0x8];  // 0xC4
} effBombN64Data;

// .sdata2 float constants, named after their original addresses.

// .sdata2 GXColor seeds the original builds its TEV colours from.
extern const GXColor dat_8041e9d0;
extern const GXColor dat_8041e9d4;

// .data vertex array, local to this file in the original.
static u32 size8x8_tex32x32_vtx[0x38 / 4] = {
	0xFFFCFFFC, 0x00000000, 0x0000FFFF, 0xFFFF0004,
	0xFFFC0000, 0x04000000, 0xFFFFFFFF, 0x00040004,
	0x00000400, 0x0400FFFF, 0xFFFFFFFC, 0x00040000,
	0x00000400, 0xFFFFFFFF,
};

// Local to this file in the original (`.fn effBombMain, local`).
static void effBombMain(EffectEntry* effect);
// Local to this file in the original (`.fn effBombDisp, local`).
static void effBombDisp(CameraId camId, void* param);

void effBombN64SetCamId(EffectEntry* entry, s32 camId) {
	effBombN64Data* data = entry->userdata;
	data->camId = camId;
}

EffectEntry* effBombN64Entry(s32 type, f32 x, f32 y, f32 z) {
	EffectEntry* effect;
	effBombN64Data* data;
	s32 typeHigh;

	effect = effEntry();
	typeHigh = 0;
	if (type >= 3) {
		typeHigh = 1;
		type -= 3;
	}
	effect->type = "BombN64";
	effect->count = 3;
	data = __memAlloc(HEAP_EFFECT, 0xCC);
	effect->userdata = data;
	effect->callback = effBombMain;
	data->x = x;
	data->y = y;
	data->z = z;
	data->counter = 0;
	data->life = 60;
	data->type = type;
	data->typeHigh = typeHigh;
	data->camId = 4;
	data->texType = (type == 0) ? -1 : type;
	data->alpha = 0;
	data->scale = (type == 1) ? 1.0f : 3.0f;
	data->scaleStep = (type == 1) ? 1.0f : 3.0f;
	data->field_0x88 = type;
	data->animAlpha = 0xFF;
	switch (type) {
	case 0:
		data->animScale = 9.9f;
		data->animScaleStep = 1.6f;
		break;
	case 1:
		data->animScale = 13.9f;
		data->animScaleStep = 2.4f;
		break;
	case 2:
		data->animScale = 17.8f;
		data->animScaleStep = 3.2f;
		break;
	}
	data->animRotZ = 0.0f;
	data->animRotSpeed = 30.f;
	return effect;
}

inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

// The two `timer >= 8` else-blocks only put their `*= k` products in f1 (as the original does) when the
// multiplier is a local assigned before use rather than a literal in the expression.
static void effBombMain(EffectEntry* effect) {
	effBombN64Data* data = effect->userdata;
	Vec position = initializeVec((Vec*)&data->x);
	s32 type;
	s32 timer;
	s32 camId = data->camId;

	data->life--;
	if (data->life < 0) {
		effDelete(effect);
		return;
	}

	data->counter++;
	timer = data->counter;
	type = data->type;

	if (timer == 7) {
		if (data->typeHigh == 0) {
			effKemuri8N64SetCamId(effKemuri8N64Entry(type, data->x, data->y, data->z), camId);
		} else {
			effKemuri8N64SetCamId(effKemuri8N64Entry(type + 3, data->x, data->y, data->z), camId);
		}
	} else if (timer == 1) {
		effConfettiN64SetCamId(effConfettiN64Entry(type + 4, 50, data->x, data->y, data->z, 1.0f), camId);
	}

	data->scale += data->scaleStep;
	if (timer < 8) {
		data->scaleStep += (type == 2) ? 1.0f : 0.4f;
		data->alpha = 0.6f * (255.0f - (f32)data->alpha);
	} else {
		f32 k6 = 0.6f;
		f32 k9 = 0.9f;
		data->scaleStep *= k6;
		data->alpha = (f32)data->alpha * k9;
	}

	if (timer >= 3) {
		if (timer == 3) {
			data->animScale = 1.0f;
		}
		data->animScale += data->animScaleStep;
	}

	if (timer >= 3) {
		if (timer < 6) {
			data->animScaleStep += 0.1f;
		} else {
			f32 half = 0.5f;
			f32 k9 = 0.9f;
			data->animScaleStep *= half;
			data->animAlpha = (f32)data->animAlpha * k9;
		}
	}

	data->animRotZ += data->animRotSpeed;
	dispEntry(camId, dispCalcZ(&position), 2, effBombDisp, effect);
}

static inline void bombSetColor1(u8 a) {
	GXColor c = dat_8041e9d0;
	c.a = a;
	GXSetTevColor(GX_TEVREG0, c);
}
static inline void bombSetColor2(u8 b, u8 a) {
	GXColor c = dat_8041e9d4;
	c.b = b;
	c.a = a;
	GXSetTevColor(GX_TEVREG0, c);
}

// The local declaration order is the shape MWCC needs to colour the callee-saved GPRs like the
// original (data r29, camera r30, timer r31). The natural order (timer first) makes mwcc put timer in
// r30 and camera in r31, ten instructions apart; with this order no #pragma is required.
static void effBombDisp(CameraId camId, void* param) {
	effBombN64Data* data;
	CameraEntry* camera;
	Mtx mtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	Mtx outMtx;
	GXTexObj texObj;
	GXColor color2;
	s32 alpha;
	s32 timer;
	s32 blue;
	s32 tmp;

	camera = camGetPtr(camId);
	data = ((EffectEntry*)param)->userdata;
	timer = data->counter;

	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXRotRad(rotMtx, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(mtx, rotMtx, outMtx);
	PSMTXConcat(camera->view, outMtx, outMtx);

	GXSetNumChans(0);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	PSMTXScale(mtx, 0.0625f, 0.0625f, 0.0f);
	GXLoadTexMtxImm(mtx, GX_TEXMTX0, GX_MTX2x4);
	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64((effVtxArray*)size8x8_tex32x32_vtx);

	if (data->texType != -1) {
		effGetTexObjN64(0x18, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		bombSetColor1(data->alpha);
		PSMTXScale(scaleMtx, data->scale, data->scale, 1.0f);
		PSMTXConcat(outMtx, scaleMtx, scaleMtx);
		GXLoadPosMtxImm(scaleMtx, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
	}

	if (timer == 1) {
		alpha = 0xC0;
	} else if (timer == 2) {
		alpha = 0xFF;
	} else {
		alpha = data->animAlpha;
	}
	if (timer > 2) {
		tmp = 0x7F;
		if (timer < 8) {
			tmp = timer * 16;
		}
		blue = tmp + 0x80;
	} else {
		blue = 0xFF;
	}
	bombSetColor2(blue, alpha);

	PSMTXScale(scaleMtx, data->animScale, data->animScale, data->animScale);
	PSMTXRotRad(rotMtx, 'z', 0.017453292f * data->animRotZ);
	PSMTXConcat(rotMtx, scaleMtx, scaleMtx);
	PSMTXConcat(outMtx, scaleMtx, scaleMtx);
	GXLoadPosMtxImm(scaleMtx, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);

	switch (timer % 3) {
	case 0:
		effGetTexObjN64(0x19, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		break;
	case 1:
		effGetTexObjN64(0x1A, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		break;
	case 2:
		effGetTexObjN64(0x1B, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		break;
	}

	GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
	tri2(0, 1, 2, 0, 0, 2, 3, 0);
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const GXColor dat_8041e9d0 = { 0xFF, 0xFF, 0xF0, 0x00 };
const GXColor dat_8041e9d4 = { 0xFF, 0xFF, 0x00, 0x00 };
