#include "eff/n64/eff_kemuri1_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "math_ext.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .rodata conversion constant used by the int -> float casts.
const f64 double_to_int_802fa898 = 4503601774854144.0;

// .sdata2 constants (named after their original addresses).
extern const f32 float_1_8041f238;
extern const f32 float_0p5_8041f23c;
extern const f32 float_deg2rad_8041f240;
extern const f32 float_0p0625_8041f244;
extern const f32 float_0_8041f248;
extern const f32 float_2_8041f24c;
extern const f32 float_16_8041f250;
extern const f32 float_24_8041f254;
extern const f32 float_32_8041f258;
extern const f32 float_neg3_8041f25c;
extern const f32 float_0p3_8041f260;
extern const f32 float_1p75_8041f264;
extern const f32 float_neg0p16_8041f268;
extern const f32 float_4_8041f26c;
extern const f32 float_90_8041f270;
extern const f32 float_180_8041f274;
extern const f32 float_6p2832_8041f278;
extern const f32 float_360_8041f27c;
extern const f32 float_2p3562_8041f280;
extern const f32 float_25_8041f284;
extern const f32 float_0p95_8041f288;
extern const f32 float_neg0p04_8041f28c;
extern const f32 float_0p94_8041f290;
extern const f32 float_neg2p3562_8041f294;
extern const f32 float_0p85_8041f298;
extern const f32 float_0p34907_8041f29c;
extern const f32 float_10_8041f2a0;
extern const f32 float_0p75_8041f2a4;
extern const f32 float_0p93_8041f2a8;
extern const f32 float_0p1_8041f2ac;
extern GXColor dat_8041f230;
extern GXColor dat_8041f234;
extern GXColor unk_80423168;

typedef struct effKemuri1Data {
	u16 type; //0x0
	u8 align_2[2]; //0x2, TODO: verify align
	Vec position; //0x4
	f32 field_0x10; //0x10
	f32 field_0x14; //0x14
	f32 field_0x18; //0x18
	f32 field_0x1C; //0x1C
	f32 field_0x20; //0x20
	f32 field_0x24; //0x24
	f32 field_0x28; //0x28
	f32 field_0x2C; //0x2C
	f32 field_0x30; //0x30
	f32 field_0x34; //0x34
	s32 field_0x38; //0x38
	s32 field_0x3C; //0x3C
	s32 field_0x40; //0x40
	u8 field_0x44; //0x44
	u8 field_0x45; //0x45
	u8 field_0x46; //0x46
	u8 align_47; //0x47
	s32 field_0x48; //0x48
	s32 field_0x4C; //0x4C
} effKemuri1Data;

//local prototypes

// The type 4-7 preheader and loop are instruction-for-instruction identical to the original except for the
// callee-saved FPR colours. The 5th float param `a5` is saved to f24 instead of f31; because a5 and the nine
// loop-invariant constants form one interference clique over f22-f31, the allocator then rotates every constant
// (neg3..10 land in f25,f26,f27,f28,f29,f30,f31,f22,f23 instead of f22..f30). Scoping `angle` to the case-4-7 loop
// (a one-line shadow of the function-scope `angle`) fixes the angle temp to the original's f20 and lifts the
// score from 99.62% to 99.69%. objdiff names the pooled "Kemuri1N64" string and int->float double anonymous
// @NNN@ instead of str_802FA8A0/double_to_int_802fa898; those names are unscored.
// NONMATCHING (99.69%): only the callee-saved FPR colouring of the case-4-7 clique differs (a5 at f24 instead
// of f31 drags the nine constants around); every instruction and the instruction order match.
EffectEntry* effKemuri1N64Entry(s32 type, f32 x, f32 y, f32 z, f32 a5) {
	EffectEntry* effect;
	effKemuri1Data* data;
	effKemuri1Data* entry;
	s32 effCount;
	s32 i;
	f32 angle;
	f32 sinAngle;
	f32 cosAngle;
	f32 k94;

	effect = effEntry();
	switch (type) {
		case 2:
			effCount = 1;
			break;
		case 3:
			effCount = 4;
			break;
		case 4:
		case 5:
		case 6:
		case 7:
			effCount = 9;
			break;
		default:
			effCount = 3;
			break;
	}

	effect->type = "Kemuri1N64";
	effect->count = effCount;
	data = __memAlloc(HEAP_EFFECT, sizeof(effKemuri1Data) * effCount);
	effect->userdata = data;
	effect->callback = effKemuri1Main;
	switch (type) {
		case 0:
		case 1:
			data->field_0x38 = 0;
			data->field_0x4C = 15;
			break;
		case 2:
			data->field_0x38 = 3;
			data->field_0x4C = 65;
			break;
		case 3:
			data->field_0x38 = 2;
			data->field_0x4C = 129;
			break;
		case 4:
		case 5:
		case 6:
		case 7:
			data->field_0x38 = 3;
			data->field_0x4C = 65;
			break;
		default:
			break;
	}

	data->field_0x3C = 0;
	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;

	entry = data + 1;
	switch (type) {
		case 0:
		case 1:
			for (i = 1; i < effCount; i++, entry++) {
				entry->field_0x10 = float_neg3_8041f25c;
				entry->field_0x14 = float_0p3_8041f260;
				entry->field_0x18 = float_0_8041f248;
				entry->field_0x1C = float_0_8041f248;
				entry->field_0x20 = float_1p75_8041f264;
				entry->field_0x24 = float_neg0p16_8041f268;
				entry->field_0x28 = float_0_8041f248;
				entry->field_0x2C = float_0_8041f248;
				entry->position.x = float_0_8041f248;
				entry->position.y = float_0_8041f248;
				entry->position.z = float_4_8041f26c;
				angle = float_6p2832_8041f278 * reviseAngle(float_180_8041f274 * (f32)(i - 1) + float_90_8041f270) / float_360_8041f27c;
				entry->field_0x30 = sin(angle);
				entry->field_0x34 = cos(angle);
				entry->field_0x44 = 0xFF;
				entry->field_0x45 = 0xFF;
				entry->field_0x46 = 0xFF;
			}
			break;
		case 3:
			for (i = 1; i < effCount; i++, entry++) {
				switch (i - 1) {
				case 0:
					sinAngle = (f32)sin(float_2p3562_8041f280);
					entry->position.x = float_25_8041f284 * -sinAngle;
					entry->position.y = float_0_8041f248;
					cosAngle = (f32)cos(float_2p3562_8041f280);
					entry->position.z = float_25_8041f284 * -cosAngle;
					entry->field_0x20 = float_0p95_8041f288;
					entry->field_0x24 = float_neg0p04_8041f28c;
					k94 = float_0p94_8041f290;
				entry->field_0x28 = k94;
					break;
				case 1:
					sinAngle = (f32)sin(float_neg2p3562_8041f294);
					entry->position.x = float_25_8041f284 * -sinAngle;
					entry->position.y = float_4_8041f26c;
					cosAngle = (f32)cos(float_neg2p3562_8041f294);
					entry->position.z = float_25_8041f284 * -cosAngle;
					entry->field_0x20 = float_0p85_8041f298;
					entry->field_0x24 = float_neg0p04_8041f28c;
					k94 = float_0p94_8041f290;
				entry->field_0x28 = k94;
					break;
				case 2:
					sinAngle = (f32)sin(float_0p34907_8041f29c);
					entry->position.x = float_25_8041f284 * -sinAngle;
					entry->position.y = float_10_8041f2a0;
					cosAngle = (f32)cos(float_0p34907_8041f29c);
					entry->position.z = float_25_8041f284 * -cosAngle;
					entry->field_0x20 = float_0p75_8041f2a4;
					entry->field_0x24 = float_neg0p04_8041f28c;
					k94 = float_0p94_8041f290;
				entry->field_0x28 = k94;
					break;
				}
				entry->field_0x30 = float_0_8041f248;
				entry->field_0x34 = float_0_8041f248;
				entry->field_0x44 = 0xFF;
				entry->field_0x45 = 0xFF;
				entry->field_0x46 = 0xFF;
			}
			break;
		case 4:
		case 5:
		case 6:
		case 7:
			for (i = 1; i < effCount; i++, entry++) {
				// Case-4-7 scope: shadowing the function-scope `angle` makes MWCC keep the angle in the
				// original's f20 (the function-scope one lands in f21 here).
				f32 angle;
				entry->field_0x10 = float_neg3_8041f25c;
				entry->field_0x14 = float_0p93_8041f2a8;
				entry->field_0x20 = float_0_8041f248;
				entry->field_0x24 = float_0p1_8041f2ac;
				k94 = float_0p94_8041f290;
				entry->field_0x28 = k94;
				entry->field_0x2C = k94;
				angle = float_6p2832_8041f278 * (a5 + (f32)((i - 1) * 360 / (effCount - 1))) / float_360_8041f27c;
				sinAngle = (f32)sin(angle);
				entry->position.x = float_10_8041f2a0 * -sinAngle;
				entry->position.y = float_0_8041f248;
				cosAngle = (f32)cos(angle);
				entry->position.z = float_10_8041f2a0 * -cosAngle;
				entry->field_0x30 = sin(angle);
				entry->field_0x34 = cos(angle);
				switch (type) {
				case 4:
				case 7:
					entry->field_0x44 = 0xFF;
					entry->field_0x45 = 0xFF;
					entry->field_0x46 = 0xFF;
					break;
				case 5:
					entry->field_0x44 = 0xDD;
					entry->field_0x45 = 0xB7;
					entry->field_0x46 = 0x05;
					break;
				case 6:
					entry->field_0x44 = 0x21;
					entry->field_0x45 = 0x8C;
					entry->field_0x46 = 0x8D;
					break;
				}
			}
			break;
	}

	return effect;
}

// Builds the stored position through a zero-Vec scratch, as the original does.
inline Vec kemuri1InitVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

// Advances the frame counter and the per-particle smoke physics, then queues the draw.
void effKemuri1Main(EffectEntry* effect) {
	effKemuri1Data* data = effect->userdata;
	Vec position;
	s32 type;
	s32 b;
	s32 a;
	s32 i;

	position = kemuri1InitVec(&data->position);

	type = data->type;
	b = (s32)(float_16_8041f250 * (f32)data->field_0x3C / (f32)data->field_0x4C);
	a = (s32)(float_16_8041f250 * (f32)(data->field_0x3C + 1) / (f32)data->field_0x4C);
	data->field_0x3C++;
	if (a > 15) {
		a = 15;
	}
	if (b > 15) {
		effDelete(effect);
		return;
	}
	data->field_0x40 = b;
	data->field_0x48 = a;

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		switch (type) {
		case 0:
		case 1:
			data->field_0x18 += data->field_0x1C;
			data->field_0x14 += data->field_0x18;
			data->field_0x10 += data->field_0x14;
			data->position.x += data->field_0x10 * data->field_0x30;
			data->position.z += data->field_0x10 * data->field_0x34;
			data->field_0x28 += data->field_0x2C;
			data->field_0x24 += data->field_0x28;
			data->field_0x20 += data->field_0x24;
			data->position.y += data->field_0x20;
			break;
		case 3:
			data->position.y += data->field_0x20;
			data->field_0x20 += data->field_0x24;
			data->field_0x24 *= data->field_0x28;
			break;
		case 4:
		case 5:
		case 6:
		case 7:
			data->position.x += data->field_0x10 * data->field_0x30;
			data->position.z += data->field_0x10 * data->field_0x34;
			data->field_0x10 *= data->field_0x14;
			data->position.y += data->field_0x20;
			data->field_0x20 += data->field_0x24;
			data->field_0x24 *= data->field_0x28;
			data->field_0x20 *= data->field_0x2C;
			break;
		}
	}

	if (effect->count - 1 <= 0) {
		effDelete(effect);
		return;
	}
	dispEntry(4, dispCalcZ(&position), 2, effKemuri1Disp, effect);
}

// Draws every live entry as a camera-facing textured quad. Matching uses the same tricks as the sibling
// effKemuri8Disp/effKemuri9Disp: a `half` local held in f29, the colour helpers above, and a two-variable
// size (size/size2) with 0.0f/1.0f literals in the vertex stream.
#ifdef MUST_MATCH
// Building each colour and making its GXSetChanMatColor call inside a void inline pairs the local with
// its outgoing copy (0x8/0xc and 0x10), so the TEV colour's by-value temp lands at 0x14 as the
// original's does (the plain `GXColor color` local would take 0x14 instead). Same shape as eff_kemuri8.
static inline void kemuri1SetChanMatColorA(void) {
	GXColor c = dat_8041f234;
	GXSetChanMatColor(4, c);
}

static inline void kemuri1SetChanMatColorB(u8 r, u8 g, u8 b) {
	GXColor c = unk_80423168;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = 0xFF;
	GXSetChanMatColor(4, c);
}
#endif

void effKemuri1Disp(CameraId camId, void* param) {
	f32 half = 0.5f;
	EffectEntry* effect = param;
	effKemuri1Data* data = effect->userdata;
	CameraEntry* cam;
	GXTexObj texobj;
	Mtx mtxTmp;
	Mtx mtxScale;
	Mtx mtxRot;
	Mtx mtxBase;
	s32 field1;
	s32 field0;
	f32 scrollX;
	f32 scrollY;
	f32 scale;
	f32 size;
	f32 size2;
	s32 type;
	s32 j;
	s32 i;

	cam = camGetPtr(camId);
	type = data->type;
	field0 = data->field_0x40;
	field1 = data->field_0x48;

	scale = float_1_8041f238;
	switch (type) {
	case 5:
	case 6:
	case 7:
		scale = float_0p5_8041f23c;
		break;
	default:
		break;
	}

	PSMTXTrans(mtxTmp, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(mtxRot, 'y', -camGetPtr(camId)->viewYaw * float_deg2rad_8041f240);
	PSMTXScale(mtxScale, scale, scale, scale);
	PSMTXConcat(mtxTmp, mtxRot, mtxTmp);
	PSMTXConcat(mtxTmp, mtxScale, mtxTmp);
	PSMTXConcat(cam->view, mtxTmp, mtxBase);

	effGetTexObj(0x28, &texobj);
	GXLoadTexObj(&texobj, 0);

	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
	GXSetTexCoordGen2(1, 1, 4, 0x21, 0, 0x7D);
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
	GXSetTevColor(1, dat_8041f230);
	GXSetTevOrder(2, 0xFF, 0xFF, 4);
	GXSetTevColorOp(2, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
	GXSetTevColorIn(2, 0xF, 0, 0xA, 0xF);
	GXSetTevAlphaIn(2, 7, 0, 5, 7);
	GXSetCullMode(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(13, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 13, 1, 4, 0);

	scrollX = float_0p0625_8041f244 * (f32)field0;
	scrollY = float_0p0625_8041f244 * (f32)field1;

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		if (data->field_0x40 > 15) {
			continue;
		}

		PSMTXTrans(mtxTmp, scrollX, float_0_8041f248, float_0_8041f248);
		PSMTXScale(mtxScale, float_0p0625_8041f244, float_1_8041f238, float_1_8041f238);
		PSMTXConcat(mtxTmp, mtxScale, mtxTmp);
		GXLoadTexMtxImm(mtxTmp, 0x1E, 1);

		PSMTXTrans(mtxTmp, scrollY, float_0_8041f248, float_0_8041f248);
		PSMTXScale(mtxScale, float_0p0625_8041f244, float_1_8041f238, float_1_8041f238);
		PSMTXConcat(mtxTmp, mtxScale, mtxTmp);
		GXLoadTexMtxImm(mtxTmp, 0x21, 1);

		for (j = 0; j < 2; j++) {
			if (j == 0) {
#ifdef MUST_MATCH
				kemuri1SetChanMatColorA();
#else
				GXSetChanMatColor(4, dat_8041f234);
#endif
				PSMTXTrans(mtxTmp, float_2_8041f24c + data->position.x,
					data->position.y - float_2_8041f24c, data->position.z);
				PSMTXConcat(mtxBase, mtxTmp, mtxTmp);
				GXLoadPosMtxImm(mtxTmp, 0);
			}
			else {
#ifdef MUST_MATCH
				kemuri1SetChanMatColorB(data->field_0x44, data->field_0x45, data->field_0x46);
#else
				GXColor color = unk_80423168;
				color.r = data->field_0x44;
				color.g = data->field_0x45;
				color.b = data->field_0x46;
				color.a = 0xFF;
				GXSetChanMatColor(4, color);
#endif
				PSMTXTrans(mtxTmp, data->position.x, data->position.y, data->position.z);
				PSMTXConcat(mtxBase, mtxTmp, mtxTmp);
				GXLoadPosMtxImm(mtxTmp, 0);
			}

			GXBegin(0x80, 0, 4);

#ifdef MUST_MATCH
			// Two variables so MWCC emits the original's duplicate `fmr` and four separate size*0.5 corners;
			// the 0.0f/1.0f literals make it reload them per store instead of hoisting them into extra FPRs.
			switch (type) {
			case 0:
				size = float_16_8041f250;
				size2 = size;
				break;
			case 1:
				size = float_24_8041f254;
				size2 = size;
				break;
			default:
				size = float_32_8041f258;
				size2 = size;
				break;
			}

			GXPosition3f32(-size2 * half, size * half, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(size2 * half, size * half, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);
			GXPosition3f32(size2 * half, -size * half, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);
			GXPosition3f32(-size2 * half, -size * half, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
#else
			switch (type) {
			case 0:
				size = float_16_8041f250;
				break;
			case 1:
				size = float_24_8041f254;
				break;
			default:
				size = float_32_8041f258;
				break;
			}

			GXPosition3f32(-size * half, size * half, float_0_8041f248);
			GXTexCoord2f32(float_0_8041f248, float_0_8041f248);
			GXPosition3f32(size * half, size * half, float_0_8041f248);
			GXTexCoord2f32(float_1_8041f238, float_0_8041f248);
			GXPosition3f32(size * half, -size * half, float_0_8041f248);
			GXTexCoord2f32(float_1_8041f238, float_1_8041f238);
			GXPosition3f32(-size * half, -size * half, float_0_8041f248);
			GXTexCoord2f32(float_0_8041f248, float_1_8041f238);
#endif
		}
	}
}

