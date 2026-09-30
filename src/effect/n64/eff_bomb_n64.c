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
	s32 field_0x30;    // 0x30
	s32 field_0x34;    // 0x34
	u8 pad_0x38[0x4];  // 0x38
	s32 field_0x3C;    // 0x3C
	s32 camId;         // 0x40
	s32 field_0x44;    // 0x44
	u8 pad_0x48[0x1C]; // 0x48
	f32 field_0x64;    // 0x64
	f32 field_0x68;    // 0x68
	u8 pad_0x6C[0x10]; // 0x6C
	s32 field_0x7C;    // 0x7C
	u8 pad_0x80[0x8];  // 0x80
	s32 field_0x88;    // 0x88
	u8 pad_0x8C[0x1C]; // 0x8C
	f32 field_0xA8;    // 0xA8
	f32 field_0xAC;    // 0xAC
	f32 field_0xB0;    // 0xB0
	f32 field_0xB4;    // 0xB4
	u8 pad_0xB8[0x8];  // 0xB8
	s32 field_0xC0;    // 0xC0
	u8 pad_0xC4[0x8];  // 0xC4
} effBombN64Data;

// .sdata2 float constants, named after their original addresses.
extern const f32 float_deg2rad_8041e9d8;
extern const f32 float_0p0625_8041e9dc;
extern const f32 float_0_8041e9e0;
extern const f32 float_1_8041e9e4;
extern const f32 float_0p4_8041e9e8;
extern const f32 float_0p6_8041e9ec;
extern const f32 float_255_8041e9f0;
extern const f32 float_0p9_8041e9f4;
extern const f32 float_0p1_8041e9f8;
extern const f32 float_0p5_8041e9fc;
extern const f32 float_3_8041ea00;
extern const f32 float_9p9_8041ea04;
extern const f32 float_1p6_8041ea08;
extern const f32 float_13p9_8041ea0c;
extern const f32 float_2p4_8041ea10;
extern const f32 float_17p8_8041ea14;
extern const f32 float_3p2_8041ea18;
extern const f32 float_30_8041ea1c;

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
	s32 flag;

	effect = effEntry();
	flag = 0;
	if (type >= 3) {
		flag = 1;
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
	data->field_0x34 = 0;
	data->field_0x30 = 60;
	data->type = type;
	data->field_0x3C = flag;
	data->camId = 4;
	data->field_0x44 = (type == 0) ? -1 : type;
	data->field_0x7C = 0;
	data->field_0x64 = (type == 1) ? float_1_8041e9e4 : float_3_8041ea00;
	data->field_0x68 = (type == 1) ? float_1_8041e9e4 : float_3_8041ea00;
	data->field_0x88 = type;
	data->field_0xC0 = 0xFF;
	switch (type) {
	case 0:
		data->field_0xA8 = float_9p9_8041ea04;
		data->field_0xAC = float_1p6_8041ea08;
		break;
	case 1:
		data->field_0xA8 = float_13p9_8041ea0c;
		data->field_0xAC = float_2p4_8041ea10;
		break;
	case 2:
		data->field_0xA8 = float_17p8_8041ea14;
		data->field_0xAC = float_3p2_8041ea18;
		break;
	}
	data->field_0xB0 = float_0_8041e9e0;
	data->field_0xB4 = float_30_8041ea1c;
	return effect;
}

inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

// NONMATCHING (99.49%): the two `timer >= 8` else-blocks compute the product in f0 instead of f1, rotating the
// int->float double and 0.9 temporaries (build f1/f2; target f2/f0). Tried `=`/reversed operands/temps and pragmas.
static void effBombMain(EffectEntry* effect) {
	effBombN64Data* data = effect->userdata;
	Vec position = initializeVec((Vec*)&data->x);
	s32 type;
	s32 timer;
	s32 camId = data->camId;

	data->field_0x30--;
	if (data->field_0x30 < 0) {
		effDelete(effect);
		return;
	}

	data->field_0x34++;
	timer = data->field_0x34;
	type = data->type;

	if (timer == 7) {
		if (data->field_0x3C == 0) {
			effKemuri8N64SetCamId(effKemuri8N64Entry(type, data->x, data->y, data->z), camId);
		} else {
			effKemuri8N64SetCamId(effKemuri8N64Entry(type + 3, data->x, data->y, data->z), camId);
		}
	} else if (timer == 1) {
		effConfettiN64SetCamId(effConfettiN64Entry(type + 4, 50, data->x, data->y, data->z, 1.0f), camId);
	}

	data->field_0x64 += data->field_0x68;
	if (timer < 8) {
		data->field_0x68 += (type == 2) ? 1.0f : 0.4f;
		data->field_0x7C = 0.6f * (255.0f - (f32)data->field_0x7C);
	} else {
		data->field_0x68 *= 0.6f;
		data->field_0x7C = (f32)data->field_0x7C * 0.9f;
	}

	if (timer >= 3) {
		if (timer == 3) {
			data->field_0xA8 = 1.0f;
		}
		data->field_0xA8 += data->field_0xAC;
	}

	if (timer >= 3) {
		if (timer < 6) {
			data->field_0xAC += 0.1f;
		} else {
			data->field_0xAC *= 0.5f;
			data->field_0xC0 = (f32)data->field_0xC0 * 0.9f;
		}
	}

	data->field_0xB0 += data->field_0xB4;
	dispEntry(camId, dispCalcZ(&position), 2, effBombDisp, effect);
}

#ifdef MUST_MATCH
// Returning each GXColor from an inline makes MWCC build it in a return temporary and copy it to the
// by-value argument slot. With both calls this reproduces the original's 0x10/0x14 (first call) and
// 0x8/0xc (second) layout, except for the two slots noted below.
static inline GXColor bombColor1(u8 a) {
	GXColor c = dat_8041e9d0;
	c.a = a;
	return c;
}

static inline GXColor bombColor2(u8 b, u8 a) {
	GXColor c = dat_8041e9d4;
	c.b = b;
	c.a = a;
	return c;
}
#endif

// Two MWCC allocation artifacts keep this below 100%. (1) The original keeps timer/camera in r31/r30;
// this build swaps them (camera r31, timer r30); both keep data in r29. (2) The colour temporaries
// interleave: this build writes color1's build word at 0x10/0x14 and color2's at 0x8/0x10, so color1's
// local (0xc here vs 0x10 original) and color2's by-value copy (0x10 here vs 0xc original) are swapped,
// while color1's copy (0x14) and color2's local (0x8) match. Same mwcc by-value-GXColor quirk as
// effStoneSmokeDisp/effConfettiDisp.
// NONMATCHING (99.77%): (1) camera/timer are r31/r30 here vs r30/r31 original;
// (2) color1's local and color2's copy stack slots are swapped (0xc/0x10 vs 0x10/0xc).
static void effBombDisp(CameraId camId, void* param) {
	s32 timer;
	effBombN64Data* data;
	CameraEntry* camera;
	Mtx mtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	Mtx outMtx;
	GXTexObj texObj;
	GXColor color2;
	s32 alpha;
	s32 blue;
	s32 tmp;

	camera = camGetPtr(camId);
	data = ((EffectEntry*)param)->userdata;
	timer = data->field_0x34;

	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXRotRad(rotMtx, 'y', float_deg2rad_8041e9d8 * -camGetPtr(CAMERA_3D)->viewYaw);
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
	PSMTXScale(mtx, float_0p0625_8041e9dc, float_0p0625_8041e9dc, float_0_8041e9e0);
	GXLoadTexMtxImm(mtx, GX_TEXMTX0, GX_MTX2x4);
	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64((effVtxArray*)size8x8_tex32x32_vtx);

	if (data->field_0x44 != -1) {
		effGetTexObjN64(0x18, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
#ifdef MUST_MATCH
		GXSetTevColor(GX_TEVREG0, bombColor1(data->field_0x7C));
#else
		{
			GXColor color1 = dat_8041e9d0;
			color1.a = data->field_0x7C;
			GXSetTevColor(GX_TEVREG0, color1);
		}
#endif
		PSMTXScale(scaleMtx, data->field_0x64, data->field_0x64, float_1_8041e9e4);
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
		alpha = data->field_0xC0;
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
#ifdef MUST_MATCH
	GXSetTevColor(GX_TEVREG0, bombColor2(blue, alpha));
#else
	color2 = dat_8041e9d4;
	color2.b = blue;
	color2.a = alpha;
	GXSetTevColor(GX_TEVREG0, color2);
#endif

	PSMTXScale(scaleMtx, data->field_0xA8, data->field_0xA8, data->field_0xA8);
	PSMTXRotRad(rotMtx, 'z', float_deg2rad_8041e9d8 * data->field_0xB0);
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
