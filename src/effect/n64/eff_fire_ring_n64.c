#include "eff/n64/eff_fire_ring_n64.h"
#include "eff/eff_n64.h"
#include "eff/n64/eff_fire_dust_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"

#include "math_ext.h"

// .sdata2 TEV colour initialisers (global in the original, no header declares them).

// .data vertex array, local to this unit in the original.
static u32 stg__s_v[0x348 / 4] = {
	0x00630000, 0x00CD23D5, 0x1000FFFF, 0xFFFF0036,
	0x00000071, 0x25A90800, 0xFFFFFFFF, 0x00000000,
	0x007D2A1C, 0x0800FFFF, 0xFFFF0000, 0x000000E4,
	0x2BEF1000, 0xFFFFFFFF, 0x00B20000, 0x008E1D8E,
	0x1000FFFF, 0xFFFF0062, 0x0000004E, 0x1F620800,
	0xFFFFFFFF, 0x00360000, 0x007123D5, 0x0800FFFF,
	0xFFFF0063, 0x000000CD, 0x25A81000, 0xFFFFFFFF,
	0x00DE0000, 0x00331748, 0x1000FFFF, 0xFFFF007A,
	0x0000001C, 0x191B0800, 0xFFFFFFFF, 0x00620000,
	0x004E1D8E, 0x0800FFFF, 0xFFFF00B2, 0x0000008E,
	0x1F621000, 0xFFFFFFFF, 0x00DE0000, 0xFFCD1101,
	0x1000FFFF, 0xFFFF007A, 0x0000FFE4, 0x12D40800,
	0xFFFFFFFF, 0x007A0000, 0x001C1748, 0x0800FFFF,
	0xFFFF00DE, 0x00000033, 0x191B1000, 0xFFFFFFFF,
	0x00B20000, 0xFF720ABA, 0x1000FFFF, 0xFFFF0062,
	0x0000FFB2, 0x0C8D0800, 0xFFFFFFFF, 0x007A0000,
	0xFFE41101, 0x0800FFFF, 0xFFFF00DE, 0x0000FFCD,
	0x12D41000, 0xFFFFFFFF, 0x00630000, 0xFF330473,
	0x1000FFFF, 0xFFFF0036, 0x0000FF8F, 0x06470800,
	0xFFFFFFFF, 0x00620000, 0xFFB20ABA, 0x0800FFFF,
	0xFFFF00B2, 0x0000FF72, 0x0C8D1000, 0xFFFFFFFF,
	0x001B0000, 0xFF89023A, 0x0800FFFF, 0xFFFF0036,
	0x0000FF8F, 0x04730800, 0xFFFFFFFF, 0x00630000,
	0xFF330647, 0x1000FFFF, 0xFFFF0031, 0x0000FF28,
	0x023A1000, 0xFFFFFFFF, 0x00000000, 0x00E42A1C,
	0x1000FFFF, 0xFFFF0000, 0x0000007D, 0x2BEF0800,
	0xFFFFFFFF, 0xFFCA0000, 0x00713063, 0x0800FFFF,
	0xFFFFFF9D, 0x000000CD, 0x32361000, 0xFFFFFFFF,
	0x00000000, 0xFF1C560B, 0x1000FFFF, 0xFFFFFF9D,
	0x0000FF33, 0x4FC41000, 0xFFFFFFFF, 0xFF4E0000,
	0xFF72497E, 0x1000FFFF, 0xFFFFFF22, 0x0000FFCD,
	0x43371000, 0xFFFFFFFF, 0xFF220000, 0x00333CF0,
	0x1000FFFF, 0xFFFFFF4E, 0x0000008E, 0x36A91000,
	0xFFFFFFFF, 0xFF9D0000, 0x00CD3063, 0x1000FFFF,
	0xFFFF0000, 0x0000FF83, 0x57DE0800, 0xFFFFFFFF,
	0x001B0000, 0xFF895A18, 0x0800FFFF, 0xFFFF0031,
	0x0000FF28, 0x5A181000, 0xFFFFFFFF, 0xFFCA0000,
	0xFF8F5198, 0x0800FFFF, 0xFFFF0000, 0x0000FF83,
	0x560B0800, 0xFFFFFFFF, 0x00000000, 0xFF1C57DE,
	0x1000FFFF, 0xFFFFFF9E, 0x0000FFB2, 0x4B510800,
	0xFFFFFFFF, 0xFFCA0000, 0xFF8F4FC4, 0x0800FFFF,
	0xFFFFFF9D, 0x0000FF33, 0x51981000, 0xFFFFFFFF,
	0xFF860000, 0xFFE4450A, 0x0800FFFF, 0xFFFFFF9E,
	0x0000FFB2, 0x497E0800, 0xFFFFFFFF, 0xFF4E0000,
	0xFF724B51, 0x1000FFFF, 0xFFFFFF86, 0x0000001C,
	0x3EC40800, 0xFFFFFFFF, 0xFF860000, 0xFFE44337,
	0x0800FFFF, 0xFFFFFF22, 0x0000FFCD, 0x450A1000,
	0xFFFFFFFF, 0xFF9E0000, 0x004E387D, 0x0800FFFF,
	0xFFFFFF86, 0x0000001C, 0x3CF00800, 0xFFFFFFFF,
	0xFF220000, 0x00333EC3, 0x1000FFFF, 0xFFFFFFCA,
	0x00000071, 0x32360800, 0xFFFFFFFF, 0xFF9E0000,
	0x004E36A9, 0x0800FFFF, 0xFFFFFF4E, 0x0000008E,
	0x387D1000, 0xFFFFFFFF,
};

typedef struct effFireRingData {
	s32 type;        // 0x00
	f32 x;           // 0x04
	f32 y;           // 0x08
	f32 z;           // 0x0C
	u8 pad_10[0xC];  // 0x10
	f32 angle;       // 0x1C
	f32 angleSpeed;  // 0x20
	s32 alpha;       // 0x24
	s32 life;        // 0x28
	s32 counter;     // 0x2C
	f32 rotX;        // 0x30
	f32 rotY;        // 0x34
	f32 rotZ;        // 0x38
	f32 rotW;        // 0x3C
	f32 rotSpeedX;   // 0x40
	f32 rotSpeedY;   // 0x44
	f32 rotSpeedZ;   // 0x48
	f32 rotSpeedW;   // 0x4C
	u8 colorR;       // 0x50
	u8 colorG;       // 0x51
	u8 colorB;       // 0x52
	u8 pad_53;       // 0x53
} effFireRingData;   // 0x54

EffectEntry* effFireRingN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry;
	effFireRingData* data;

	entry = effEntry();
	entry->type = "FireRingN64";
	entry->count = 32;
	data = __memAlloc(HEAP_EFFECT, sizeof(effFireRingData) * 32);
	entry->userdata = data;
	entry->callback = effFireRingMain;

	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->alpha = 0xff;
	data->life = life;
	data->counter = 0;
	data->rotX = 0.0f;
	data->rotY = 0.0f;
	data->rotZ = 0.0f;
	data->rotW = 0.0f;
	data->rotSpeedX = -1.1f;
	data->rotSpeedY = 0.0f;
	data->rotSpeedZ = 0.4f;
	data->rotSpeedW = 1.0f;
	data->angle = 0.0f;
	data->angleSpeed = 0.1f * scale;
	data->colorR = 0xff;
	data->colorG = 0xff;
	data->colorB = 0x40;

	return entry;
}

void effFireRingMain(EffectEntry* effect) {
	effFireRingData* data = effect->userdata;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second
	   local before dispCalcZ; a single local folds the copy away (same idiom as
	   effFlameMain / effGlassMain). */
	Vec position;
#endif
	Vec disp = (Vec){ 0.0f, 0.0f, 0.0f };
	s32 life;
	s32 counter;
	f32 max;
	s32 r;
	f32 angle;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
#ifdef MUST_MATCH
	position = disp;
#endif

	data->life--;
	data->counter++;

	life = data->life;
	if (life < 0) {
		effDelete(effect);
		return;
	}

	counter = data->counter;
	if (counter & 1) {
		r = effTblRandN64(0x168, counter);
		angle = 6.2831855f * (f32)r / 360.0f;
		sin(angle);
		cos(angle);
		effFireDustN64Entry(0, 10, 20, data->x, data->y, data->z, (f32)(counter * 8), 10.0f);
	}

	data->angle += data->angleSpeed;
	if (life < 10) {
		data->alpha = life * 0x19;
	}

	max = 128.0f;
	data->rotX += data->rotSpeedX;
	data->rotY += data->rotSpeedY;
	data->rotZ += data->rotSpeedZ;
	data->rotW += data->rotSpeedW;

	if (data->rotX > max) {
		data->rotX -= max;
	}
	if (data->rotY > max) {
		data->rotY -= max;
	}
	if (data->rotZ > max) {
		data->rotZ -= max;
	}
	if (data->rotW > max) {
		data->rotW -= max;
	}
	if (data->rotX < 0.0f) {
		data->rotX += 128.0f;
	}
	if (data->rotY < 0.0f) {
		data->rotY += 128.0f;
	}
	if (data->rotZ < 0.0f) {
		data->rotZ += 128.0f;
	}
	if (data->rotW < 0.0f) {
		data->rotW += 128.0f;
	}

#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effFireRingDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effFireRingDisp, effect);
#endif
}

static inline void effFireRingSetColor0(s32 alpha) {
	GXColor color = (GXColor){ 0xFF, 0xB3, 0x1E, 0x00 };
	color.a = alpha;
	GXSetTevColor(GX_TEVREG0, color);
}

static inline void effFireRingSetColor1(effFireRingData* data) {
	GXColor color = (GXColor){ 0x00, 0x00, 0x00, 0x80 };
	color.r = data->colorR;
	color.g = data->colorG;
	color.b = data->colorB;
	GXSetTevColor(GX_TEVREG1, color);
}

void effFireRingDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effFireRingData* data;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx trans;
	Mtx scale;

	cam = camGetPtr(camId);
	data = effect->userdata;

	PSMTXScale(scale, 0.015873017f, 0.015873017f, 0.0f);
	PSMTXTrans(trans, data->rotX, data->rotY, 0.0f);
	PSMTXConcat(scale, trans, trans);
	GXLoadTexMtxImm(trans, GX_TEXMTX0, GX_MTX2x4);

	PSMTXScale(scale, 0.032258064f, 0.032258064f, 0.0f);
	PSMTXTrans(trans, data->rotZ, data->rotW, 0.0f);
	PSMTXConcat(scale, trans, trans);
	GXLoadTexMtxImm(trans, GX_TEXMTX1, GX_MTX2x4);

	PSMTXTrans(trans, data->x, data->y, data->z);
	PSMTXScale(scale, data->angle, data->angle, data->angle);
	PSMTXConcat(trans, scale, trans);
	PSMTXConcat(cam->view, trans, trans);
	GXLoadPosMtxImm(trans, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);

	effGetTexObjN64(0x2d, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	effGetTexObjN64(0x2e, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP1);

	effFireRingSetColor0(data->alpha);
	effFireRingSetColor1(data);

	GXSetNumChans(0);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, 0x7d);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, 0x7d);
	GXSetNumTevStages(3);

	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A1, GX_CA_TEXA);

	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_C1);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);

	GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_TEXC, GX_CC_TEXC, GX_CC_CPREV);
	GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);

	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64((effVtxArray*)stg__s_v);
	GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0x5a);
	tri2(0, 1, 2, 0, 0, 2, 3, 0);
	tri2(4, 5, 6, 0, 4, 6, 7, 0);
	tri2(8, 9, 10, 0, 8, 10, 11, 0);
	tri2(12, 13, 14, 0, 12, 14, 15, 0);
	tri2(16, 17, 18, 0, 16, 18, 19, 0);
	tri2(20, 21, 22, 0, 20, 22, 23, 0);
	tri2(24, 25, 26, 0, 24, 26, 27, 0);
	tri2(28, 29, 30, 0, 28, 30, 31, 0);
	tri2(32, 39, 40, 0, 32, 40, 41, 0);
	tri2(33, 42, 43, 0, 33, 43, 44, 0);
	tri2(34, 45, 46, 0, 34, 46, 47, 0);
	tri2(35, 48, 49, 0, 35, 49, 50, 0);
	tri2(36, 51, 52, 0, 36, 52, 53, 0);
	tri2(37, 54, 55, 0, 37, 55, 56, 0);
	tri2(38, 57, 58, 0, 38, 58, 59, 0);
}
