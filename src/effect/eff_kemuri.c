#include "eff/eff_kemuri.h"
#include "driver/dispdrv.h"
#include "driver/mapdrv.h"
#include "math_ext.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>

typedef struct effKemuriData {
	s32 field_0x0; //0x0
	f32 field_0x4; //0x4
	f32 field_0x8; //0x8
	f32 field_0xC; //0xC
	u8 field_0x10[0x40 - 0x10]; //0x10
	s32 field_0x40; //0x40
	f32 field_0x44; //0x44
	f32 field_0x48; //0x48
	f32 field_0x4C; //0x4C
	u8 field_0x50[0x58 - 0x50]; //0x50
	f32 field_0x58; //0x58
	f32 field_0x5C; //0x5C
	u8 field_0x60[0x74 - 0x60]; //0x60
	f32 field_0x74; //0x74
	u8 field_0x78[0x80 - 0x78]; //0x78
	s32 field_0x80; //0x80
	s32 field_0x84; //0x84
	s32 field_0x88; //0x88
} effKemuriData;

// Named sdata2 constants of the original unit (definitions live in the original object's data).
extern const f32 float_1_8041c6b0;
extern const f32 float_0_8041c6bc;
extern const f32 float_360_8041c70c;
extern const f32 float_1000_8041c710;
extern const f32 float_6p2832_8041c714;
extern const f32 float_5_8041c718;
extern const f32 float_0p7_8041c71c;
extern const f32 float_neg20_8041c720;
extern const char str_802F3120[];
extern const f32 float_12_8041c688;
extern const f32 float_8_8041c68c;
extern const f32 float_12p8_8041c690;
extern const f32 float_20_8041c694;
extern const f32 float_160_8041c698;
extern const f32 float_180_8041c69c;
extern const f32 float_340_8041c6a0;
extern const f32 float_deg2rad_8041c6a4;
extern const f32 float_0p17453_8041c6a8;
extern const f32 float_neg1_8041c6ac;
extern const f32 float_10_8041c6b4;
extern const f32 float_neg6_8041c6b8;
extern const f32 float_neg10_8041c6c0;
extern const f32 float_6_8041c6c4;
extern const f32 float_neg4_8041c6c8;
extern const f32 float_0p5_8041c6cc;
extern const f32 float_4_8041c6d0;
extern const f32 float_6p4_8041c6d4;
extern const GXColor unk_80423010;
extern const GXColor unk_80423014;
extern const GXColor dat_8041c680;
extern const GXColor dat_8041c684;

// .rodata conversion constant used by the int -> float casts.
const f64 double_to_int_802f3110 = 4503601774854144.0;

// local per the original: .fn effKemuri2Disp, local
static void effKemuri2Disp(CameraId camId, void* param);
// local per the original: .fn effKemuri2Main, local
static void effKemuri2Main(EffectEntry* effect);

EffectEntry* effKemuriEntry(s32 int2, f32 float3, f32 float4, f32 float5, f32 float6) {
	effKemuriData* data;
	s32 numEffects, i;
	
	EffectEntry* entry = effEntry();
	numEffects = 1;
	switch (int2) {
		case 0:
			numEffects = 3;
			break;
		case 1:
			numEffects = 2;
			break;
		case 2:
			numEffects = 2;
			break;
	}
	entry->type = "Kemuri";
	entry->count = numEffects;
	data = (effKemuriData*)__memAlloc(HEAP_EFFECT, sizeof(effKemuriData) * numEffects);
	entry->userdata = data;
	entry->callback = effKemuriMain;

	data->field_0x0 = int2;
	data->field_0x4 = float3;
	data->field_0x8 = float4;
	data->field_0xC = float5;

	data->field_0x40 = 0;

	data->field_0x44 = 1.0f;
	data->field_0x48 = 1.0f;
	data->field_0x74 = 1.0f;
	data++;
	for (i = 1; i < entry->count; i++, data++) {
		data->field_0x4 = 0.0f;
		data->field_0x8 = 0.0f;
		data->field_0xC = 0.0f;

		data->field_0x44 = 1.0f;
		data->field_0x48 = 1.0f;
		data->field_0x74 = 1.0f;

		data->field_0x58 = 0.0f;
		data->field_0x5C = 0.0f;

		data->field_0x40 = 0;
		data->field_0x80 = 0;
		data->field_0x84 = 0;

		switch (int2) {
			case 0:
				data->field_0x4C = i & 1 ? 3.0f : -3.0f;
				data->field_0x88 = 32;
				break;
			case 1:
				data->field_0x4C = 0.0f;
				data->field_0x88 = 32;
				break;
			case 2:
				data->field_0x4C = 0.0f;
				data->field_0x88 = 64;
				break;
		}
	}
	return entry;
}

// NONMATCHING (99.93%): only the viewYaw load/add differ -- retail coalesces sinVal, viewYaw and one
// into f26, ours gives viewYaw f28; the two-variable (angle/angle2) shape and declaration order below
// fix every other register (angle f24, cosVal/z f25, sinVal/one f26, p7 f27, neg20 f28, zero f31).
EffectEntry* effKemuri2Entry(s32 int2, f32 float3, f32 float4, f32 float5, f32 float6, f32 float7) {
	EffectEntry* entry = effEntry();
	effKemuri2Data* data;
	f32 neg20;
	f32 zero;
	f32 p7;
	f32 one;
	f32 sinVal;
	f32 z;
	f32 angle2;
	f32 angle;
	f32 cosVal;

	entry->type = str_802F3120;
	entry->count = 1;
	data = (effKemuri2Data*)__memAlloc(HEAP_EFFECT, sizeof(effKemuri2Data));
	entry->userdata = data;
	memset(data, 0, sizeof(effKemuri2Data));
	entry->callback = effKemuri2Main;

	sinVal = float_1000_8041c710 * float6;
	cosVal = float_1000_8041c710 * float7;
	angle = float_360_8041c70c * angleABf(float_0_8041c6bc, float_0_8041c6bc, sinVal, cosVal) / float_6p2832_8041c714;
	data->field_0x78 = reviseAngle(float_360_8041c70c * angleABf(float_0_8041c6bc, float_0_8041c6bc, sinVal, cosVal) / float_6p2832_8041c714 + camGetPtr(CAMERA_3D)->viewYaw);
	angle2 = float_6p2832_8041c714 * (f32)(s32)angle / float_360_8041c70c;
	z = float_5_8041c718 + float5;
	data->counter = 0;
	one = float_1_8041c6b0;
	p7 = float_0p7_8041c71c;
	neg20 = float_neg20_8041c720;
	zero = float_0_8041c6bc;
	data->field_0x30 = sin(angle2);
	data->field_0x34 = cos(angle2);
	data->x = float3;
	data->y = float4;
	data->z = z;
	data->field_0x10 = one;
	data->field_0x20 = p7;
	data->field_0x64 = neg20;
	data->field_0x68 = zero;
	data->alpha = 0xFF;
	data->field_0x70 = one;
	data->field_0x74 = one;
	return entry;
}

// The count tables are u8 and converted with the unsigned (2^52) mask; the two 1/128 scale factors
// are written as a division by 128.0f, which is what makes MWCC emit retail's f4=mask / f3=scale
// register order (a `* 0.0078125f` puts them the other way round).
void effKemuriMain(EffectEntry* effect) {
	effKemuriData* data = effect->userdata;
	s32 i = 1;
	s32 type;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch and copies it to a second local before
	   dispCalcZ; a single local folds the copy away. */
	Vec position;
#endif
	Vec disp = { 0.0f, 0.0f, 0.0f };

	disp.x = data->field_0x4;
	disp.y = data->field_0x8;
	disp.z = data->field_0xC;
#ifdef MUST_MATCH
	position = disp;
#endif

	type = data->field_0x0;
	data++;
	for (; i < effect->count; i++, data++) {
		data->field_0x80 = (s32)(float_16_8041c6f4 * (f32)data->field_0x40 / (f32)data->field_0x88);
		data->field_0x84 = (s32)(float_16_8041c6f4 * (f32)(data->field_0x40 + 1) / (f32)data->field_0x88);
		if (data->field_0x84 > 15) {
			data->field_0x84 = 15;
		}
		switch (type) {
		case 0:
			if ((u32)data->field_0x40 < 30U) {
#ifdef MUST_MATCH
				data->field_0x44 = (f32)touchdown_scale[data->field_0x40 * 2] / 128.0f;
				data->field_0x48 = (f32)touchdown_scale[data->field_0x40 * 2 + 1] / 128.0f;
#else
				data->field_0x44 = float_0p0078125_8041c6f8 * (f32)touchdown_scale[data->field_0x40 * 2];
				data->field_0x48 = float_0p0078125_8041c6f8 * (f32)touchdown_scale[data->field_0x40 * 2 + 1];
#endif
			} else {
				data->field_0x44 = float_1p5_8041c6fc;
				data->field_0x48 = float_0p8_8041c6f0;
			}
			data->field_0x4C *= float_0p85_8041c700;
			data->field_0x58 += data->field_0x4C;
			data->field_0x40++;
			break;
		case 1:
			data->field_0x40++;
			break;
		case 2:
			if ((u32)data->field_0x40 < 30U) {
#ifdef MUST_MATCH
				data->field_0x44 = (f32)heavy_scale[data->field_0x40 * 2] / 128.0f;
				data->field_0x48 = (f32)heavy_scale[data->field_0x40 * 2 + 1] / 128.0f;
#else
				data->field_0x44 = float_0p0078125_8041c6f8 * (f32)heavy_scale[data->field_0x40 * 2];
				data->field_0x48 = float_0p0078125_8041c6f8 * (f32)heavy_scale[data->field_0x40 * 2 + 1];
#endif
			} else {
				data->field_0x44 = float_2_8041c704;
				data->field_0x48 = float_1p2_8041c708;
			}
			data->field_0x40++;
			break;
		}
		if (data->field_0x40 >= data->field_0x88) {
			effDelete(effect);
			return;
		}
	}
#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effKemuriDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effKemuriDisp, effect);
#endif
}

// All instructions are present; the only differences are register allocation.
// NONMATCHING (98.63%): the original loads effect->userdata into r7 then copies it to the
// callee-saved r31 before the sin call (we coalesce the load straight into r31), and the alpha
// block's int->float conversion uses f2/f0/f1 where we use f1/f2/f0.
static void effKemuri2Main(EffectEntry* effect) {
	effKemuri2Data* data = effect->userdata;
	s32 i;
	f32 angle;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch and copies it to a second local before
	   dispCalcZ; a single local folds the copy away. */
	Vec position;
#endif
	Vec disp = { 0.0f, 0.0f, 0.0f };

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
#ifdef MUST_MATCH
	position = disp;
#endif

	angle = (f32)sin(3.1415927f * (f32)data->counter / 30.0f);
	data->counter++;
	if (data->counter > 30) {
		effDelete(effect);
		return;
	}

	for (i = 0; i < effect->count; i++, data++) {
		data->x += data->field_0x10 * data->field_0x30;
		data->z += data->field_0x10 * data->field_0x34;
		data->field_0x20 *= 0.9f;
		data->y += data->field_0x20;
		data->field_0x64 *= 0.95f;
		data->field_0x68 += data->field_0x64;
		data->field_0x70 = angle;
		if (data->counter > 15) {
			data->alpha = (s32)((f32)data->alpha * 0.8f);
		}
	}
#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effKemuri2Disp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effKemuri2Disp, effect);
#endif
}

// Draws a Kemuri puff as two camera-facing quads. `half`/`twenty` are locals (rather than the
// named float_0p5_8041c6cc/float_20_8041c694) because the compiler then hoists them into the
// FPRs retail uses (half f31, twenty f30); the named constants give a different hoist order.
void effKemuriDisp(CameraId camId, void* param) {
	f32 half = 0.5f;
	f32 twenty = 20.0f;
	EffectEntry* effect = param;
	effKemuriData* data = effect->userdata;
	s32 i;
	s32 j;
	s32 type = data->field_0x0;
	CameraEntry* camera;
	GXTexObj texobj;
	Mtx transMtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	Mtx outMtx;
	Mtx tmpMtx;

	camera = camGetPtr(camId);

	switch (type) {
	case 0:
		effGetTexObj(0x23, &texobj);
		GXLoadTexObj(&texobj, 0);
		break;
	case 1:
		effGetTexObj(0x24, &texobj);
		GXLoadTexObj(&texobj, 0);
		break;
	case 2:
		effGetTexObj(0x23, &texobj);
		GXLoadTexObj(&texobj, 0);
		break;
	}

	PSMTXTrans(transMtx, data->field_0x4, data->field_0x8, data->field_0xC);
	PSMTXRotRad(rotMtx, 'y', float_deg2rad_8041c6a4 * -camGetPtr(camId)->viewYaw);
	PSMTXScale(scaleMtx, data->field_0x74, data->field_0x74, float_1_8041c6b0);
	PSMTXConcat(transMtx, rotMtx, outMtx);
	PSMTXConcat(outMtx, scaleMtx, outMtx);
	PSMTXConcat(camera->view, outMtx, outMtx);

	GXSetCurrentMtx(0);
	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
	GXSetChanMatColor(4, dat_8041c670);
	GXSetNumTevStages(3);
	GXSetTevOrder(0, 0, 0, 0xFF);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xF, 0xF, 0xF, 8);
	GXSetTevAlphaIn(0, 7, 7, 7, 4);
	GXSetTevOrder(1, 1, 0, 0xFF);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 0, 8, 2, 0xF);
	GXSetTevAlphaIn(1, 0, 4, 1, 7);
	GXSetTevColor(1, dat_8041c674);
	GXSetTevOrder(2, 0xFF, 0xFF, 4);
	GXSetTevColorOp(2, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
	GXSetTevColorIn(2, 0xF, 0, 0xA, 0xF);
	GXSetTevAlphaIn(2, 7, 0, 5, 7);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
	GXSetTexCoordGen2(1, 1, 4, 0x21, 0, 0x7D);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xD, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		for (j = 0; j < 2; j++) {
			if (j == 0) {
				GXColor color = dat_8041c678;

				GXSetChanMatColor(4, color);
			} else {
				GXColor color = dat_8041c67c;

				GXSetChanMatColor(4, color);
			}

			{
				s32 t = 1 - j;

				PSMTXTrans(transMtx, data->field_0x58 + (f32)t, (f32)-t, float_0_8041c6bc);
			}
			PSMTXScale(scaleMtx, twenty * data->field_0x44, twenty * data->field_0x48, twenty);
			PSMTXConcat(outMtx, transMtx, tmpMtx);
			PSMTXConcat(tmpMtx, scaleMtx, tmpMtx);
			GXLoadPosMtxImm(tmpMtx, 0);

#ifdef MUST_MATCH
			PSMTXScale(scaleMtx, float_0p0625_8041c6d8, 1.0f, 1.0f);
#else
			PSMTXScale(scaleMtx, float_0p0625_8041c6d8, float_1_8041c6b0, float_1_8041c6b0);
#endif
			PSMTXTrans(transMtx, (f32)data->field_0x80, float_neg0p2_8041c6dc, float_0_8041c6bc);
			PSMTXConcat(scaleMtx, transMtx, transMtx);
			GXLoadTexMtxImm(transMtx, 0x1E, 1);

#ifdef MUST_MATCH
			PSMTXScale(scaleMtx, float_0p0625_8041c6d8, 1.0f, 1.0f);
#else
			PSMTXScale(scaleMtx, float_0p0625_8041c6d8, float_1_8041c6b0, float_1_8041c6b0);
#endif
			PSMTXTrans(transMtx, (f32)data->field_0x84, float_neg0p2_8041c6dc, float_0_8041c6bc);
			PSMTXConcat(scaleMtx, transMtx, transMtx);
			GXLoadTexMtxImm(transMtx, 0x21, 1);

			GXBegin(0x80, 0, 4);
			GXPosition3f32(-float_1_8041c6b0 * half, float_1_8041c6b0, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(half, float_1_8041c6b0, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);
			GXPosition3f32(half, 0.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);
			GXPosition3f32(-float_1_8041c6b0 * half, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
		}
	}
}

// Draws a Kemuri2 puff as two camera-facing textured quads. Setup order, constants and stack layout
// match; only the callee-saved FPR numbering (our -6.4/-8/-12 land in f26/f24/f25 instead of retail
// f24/f25/f26, shifting 20/double/deg2rad/180/6.4 up one), the four by-value GXColor argument copies
// (they sit below their variable instead of above) and the local @284/@285 pool symbols for 0.0/1.0
// (retail float_0_8041c6bc/float_1_8041c6b0) differ.
// NONMATCHING (99.67%): see the five lines above.
#ifdef MUST_MATCH
static inline GXColor kemuriTevColor(u8 alpha, GXColor c) {
	c.a = alpha;
	return c;
}
#endif

// NONMATCHING (99.73%): the +6.4 constant lands in f27, not retail's f31.
static void effKemuri2Disp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	effKemuri2Data* data = effect->userdata;
	CameraEntry* camera;
	GXTexObj texobj;
	Mtx transMtx;
	Mtx rotMtx;
	Mtx rotMtx2;
	Mtx scaleMtx;
	Mtx outMtx;
	Mtx tmpMtx;
	s32 i;

	camera = camGetPtr(cameraId);

	GXSetNumChans(0);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
	GXSetBlendMode(1, 4, 5, 0);
	GXSetZCompLoc(1);
	GXSetAlphaCompare(7, 0, 0, 7, 0);
	GXSetZMode(1, 3, 0);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 0xFF);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xF, 2, 8, 0xF);
	GXSetTevAlphaIn(0, 7, 1, 4, 7);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
	GXSetCullMode(0);
	mapSetMaterialFog();
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xD, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);

	for (i = 0; i < 2; i++) {
		const f32 negSizeX = -float_12_8041c688;
		const f32 negSizeY = -float_8_8041c68c;
		const f32 negSize2 = -float_12p8_8041c690 * float_0p5_8041c6cc;

		if ((data->field_0x78 > float_20_8041c694 && data->field_0x78 < float_160_8041c698) ||
		    (data->field_0x78 > float_180_8041c69c && data->field_0x78 < float_340_8041c6a0)) {
			PSMTXTrans(transMtx, data->x + (f32)(1 - i), data->y - (f32)(1 - i), data->z);
			PSMTXRotRad(rotMtx, 'y', float_deg2rad_8041c6a4 * -camGetPtr(cameraId)->viewYaw);
			PSMTXRotRad(rotMtx2, 'z', float_0p17453_8041c6a8);
			PSMTXScale(scaleMtx, data->field_0x70, data->field_0x70, data->field_0x70);
			PSMTXConcat(transMtx, rotMtx, outMtx);
			if (data->field_0x78 < float_180_8041c69c) {
				PSMTXScale(tmpMtx, float_neg1_8041c6ac, float_1_8041c6b0, float_neg1_8041c6ac);
				PSMTXConcat(outMtx, tmpMtx, outMtx);
			}
			PSMTXTrans(tmpMtx, float_10_8041c6b4, float_neg6_8041c6b8, 0.0f);
			PSMTXConcat(outMtx, tmpMtx, outMtx);
			PSMTXScale(tmpMtx, data->field_0x74, data->field_0x74, data->field_0x74);
			PSMTXConcat(outMtx, tmpMtx, outMtx);
			PSMTXTrans(tmpMtx, float_neg10_8041c6c0, float_6_8041c6c4, 0.0f);
			PSMTXConcat(outMtx, tmpMtx, outMtx);
			PSMTXConcat(outMtx, rotMtx2, outMtx);
			PSMTXConcat(outMtx, scaleMtx, outMtx);
			PSMTXConcat(camera->view, outMtx, outMtx);
			PSMTXTrans(transMtx, float_6_8041c6c4, float_neg4_8041c6c8, 0.0f);
			PSMTXConcat(outMtx, transMtx, outMtx);
			GXLoadPosMtxImm(outMtx, 0);
			GXSetCurrentMtx(0);
			effGetTexObj(0xD, &texobj);
			GXLoadTexObj(&texobj, 0);
			if (i == 0) {
#ifdef MUST_MATCH
				GXSetTevColor(GX_TEVREG0, kemuriTevColor(data->alpha, unk_80423010));
#else
				GXColor color = unk_80423010;

				color.a = data->alpha;
				GXSetTevColor(GX_TEVREG0, color);
#endif
			} else {
#ifdef MUST_MATCH
				GXSetTevColor(GX_TEVREG0, kemuriTevColor(data->alpha, dat_8041c680));
#else
				GXColor color = dat_8041c680;

				color.a = data->alpha;
				GXSetTevColor(GX_TEVREG0, color);
#endif
			}
			GXBegin(0x80, 0, 4);
			GXPosition3f32(negSizeX * float_0p5_8041c6cc, float_4_8041c6d0, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(float_6_8041c6c4, float_4_8041c6d0, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);
			GXPosition3f32(float_6_8041c6c4, negSizeY * float_0p5_8041c6cc, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);
			GXPosition3f32(negSizeX * float_0p5_8041c6cc, negSizeY * float_0p5_8041c6cc, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
		}

		PSMTXTrans(transMtx, data->x + (f32)(1 - i), data->y - (f32)(1 - i), data->z);
		PSMTXRotRad(rotMtx, 'y', float_deg2rad_8041c6a4 * -camGetPtr(cameraId)->viewYaw);
		PSMTXRotRad(rotMtx2, 'z', float_deg2rad_8041c6a4 * data->field_0x68);
		PSMTXScale(scaleMtx, data->field_0x70, data->field_0x70, data->field_0x70);
		PSMTXConcat(transMtx, rotMtx, outMtx);
		if (data->field_0x78 < float_180_8041c69c) {
			PSMTXScale(tmpMtx, float_neg1_8041c6ac, float_1_8041c6b0, float_neg1_8041c6ac);
			PSMTXConcat(outMtx, tmpMtx, outMtx);
		}
		PSMTXTrans(tmpMtx, float_10_8041c6b4, float_neg6_8041c6b8, 0.0f);
		PSMTXConcat(outMtx, tmpMtx, outMtx);
		PSMTXScale(tmpMtx, data->field_0x74, data->field_0x74, data->field_0x74);
		PSMTXConcat(outMtx, tmpMtx, outMtx);
		PSMTXTrans(tmpMtx, float_neg10_8041c6c0, float_6_8041c6c4, 0.0f);
		PSMTXConcat(outMtx, tmpMtx, outMtx);
		PSMTXConcat(outMtx, rotMtx2, outMtx);
		PSMTXConcat(outMtx, scaleMtx, outMtx);
		PSMTXConcat(camera->view, outMtx, outMtx);
		GXLoadPosMtxImm(outMtx, 0);
		GXSetCurrentMtx(0);
		if (i == 0) {
			effGetTexObj(0xC, &texobj);
			GXLoadTexObj(&texobj, 0);
#ifdef MUST_MATCH
			GXSetTevColor(GX_TEVREG0, kemuriTevColor(data->alpha, unk_80423014));
#else
			{
				GXColor color = unk_80423014;

				color.a = data->alpha;
				GXSetTevColor(GX_TEVREG0, color);
			}
#endif
		} else {
			effGetTexObj(0xB, &texobj);
			GXLoadTexObj(&texobj, 0);
#ifdef MUST_MATCH
			GXSetTevColor(GX_TEVREG0, kemuriTevColor(data->alpha, dat_8041c684));
#else
			{
				GXColor color = dat_8041c684;

				color.a = data->alpha;
				GXSetTevColor(GX_TEVREG0, color);
			}
#endif
		}
		GXBegin(0x80, 0, 4);
		GXPosition3f32(negSize2, float_6p4_8041c6d4, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(float_6p4_8041c6d4, float_6p4_8041c6d4, 0.0f);
		GXTexCoord2f32(1.0f, 0.0f);
		GXPosition3f32(float_6p4_8041c6d4, negSize2, 0.0f);
		GXTexCoord2f32(1.0f, 1.0f);
		GXPosition3f32(negSize2, negSize2, 0.0f);
		GXTexCoord2f32(0.0f, 1.0f);
	}
}
