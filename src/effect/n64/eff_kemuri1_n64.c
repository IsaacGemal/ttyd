#include "eff/n64/eff_kemuri1_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "math_ext.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sbss2 zero GXColor scratch the material colours are built from. The pragma pairs the default .sbss2
// with a small-data section so the object lands where the original's does.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_80423168;

// .sdata2 colour constants this unit defines.
const GXColor dat_8041f230 = { 0x80, 0x80, 0x80, 0x80 };
const GXColor dat_8041f234 = { 0x00, 0x00, 0x00, 0x60 };

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

// The case-4-7 loop and its preheader are instruction-for-instruction identical to the original only with the
// two match shapes below: the sin/cos results for field_0x30/0x34 run through loop-local temps, and the 0.94
// constant is written straight into both fields instead of a function-scope `k94` temp. Without them the
// allocator colours the ten-value loop-invariant clique (nine constants + the a5 parameter) with a5 in f24 and
// the constants rotated (99.69%); with them a5 lands in f31 and neg3..10 in f22..f30 as the original. Scoping
// `angle` to the loop (shadow of the function-scope `angle`) keeps the angle temp in f20. objdiff names the
// pooled "Kemuri1N64" string and int->float double anonymous @NNN@ instead of str_802FA8A0/
// double_to_int_802fa898; those are unscored.
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
#ifdef MUST_MATCH
	// Holds the sin/cos argument so MWCC does not fold the constant into a .rodata double.
	f32 arg;
#endif

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
				entry->field_0x10 = -3.0f;
				entry->field_0x14 = 0.29999998f;
				entry->field_0x18 = 0.0f;
				entry->field_0x1C = 0.0f;
				entry->field_0x20 = 1.75f;
				entry->field_0x24 = -0.16f;
				entry->field_0x28 = 0.0f;
				entry->field_0x2C = 0.0f;
				entry->position.x = 0.0f;
				entry->position.y = 0.0f;
				entry->position.z = 4.0f;
#ifdef MUST_MATCH
				// Split so 90/180 pool before 6.28, as the original's .sdata2 order requires.
				angle = reviseAngle(90.0f + 180.0f * (f32)(i - 1));
				angle = 6.2831855f * angle / 360.0f;
#else
				angle = 6.2831855f * reviseAngle(180.0f * (f32)(i - 1) + 90.0f) / 360.0f;
#endif
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
#ifdef MUST_MATCH
					arg = 2.3561945f;
					sinAngle = (f32)sin(arg);
#else
					sinAngle = (f32)sin(2.3561945f);
#endif
					entry->position.x = 25.0f * -sinAngle;
					entry->position.y = 0.0f;
#ifdef MUST_MATCH
					cosAngle = (f32)cos(arg);
#else
					cosAngle = (f32)cos(2.3561945f);
#endif
					entry->position.z = 25.0f * -cosAngle;
					entry->field_0x20 = 0.95f;
					entry->field_0x24 = -0.04f;
					k94 = 0.94f;
				entry->field_0x28 = k94;
					break;
				case 1:
#ifdef MUST_MATCH
					arg = -2.3561945f;
					sinAngle = (f32)sin(arg);
#else
					sinAngle = (f32)sin(-2.3561945f);
#endif
					entry->position.x = 25.0f * -sinAngle;
					entry->position.y = 4.0f;
#ifdef MUST_MATCH
					cosAngle = (f32)cos(arg);
#else
					cosAngle = (f32)cos(-2.3561945f);
#endif
					entry->position.z = 25.0f * -cosAngle;
					entry->field_0x20 = 0.84999996f;
					entry->field_0x24 = -0.04f;
					k94 = 0.94f;
				entry->field_0x28 = k94;
					break;
				case 2:
#ifdef MUST_MATCH
					arg = 0.34906587f;
					sinAngle = (f32)sin(arg);
#else
					sinAngle = (f32)sin(0.34906587f);
#endif
					entry->position.x = 25.0f * -sinAngle;
					entry->position.y = 10.0f;
#ifdef MUST_MATCH
					cosAngle = (f32)cos(arg);
#else
					cosAngle = (f32)cos(0.34906587f);
#endif
					entry->position.z = 25.0f * -cosAngle;
					entry->field_0x20 = 0.75f;
					entry->field_0x24 = -0.04f;
					k94 = 0.94f;
				entry->field_0x28 = k94;
					break;
				}
				entry->field_0x30 = 0.0f;
				entry->field_0x34 = 0.0f;
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
#ifdef MUST_MATCH
				// The temp and direct-constant shapes below are what colour this loop's a5 to f31 and its
				// nine constants to f22..f30; the #else form is the natural code and scores 99.69%.
				f32 s30;
				f32 c34;
#endif
				entry->field_0x10 = -3.0f;
				entry->field_0x14 = 0.93f;
				entry->field_0x20 = 0.0f;
				entry->field_0x24 = 0.099999994f;
#ifdef MUST_MATCH
				entry->field_0x28 = 0.94f;
				entry->field_0x2C = 0.94f;
#else
				k94 = 0.94f;
				entry->field_0x28 = k94;
				entry->field_0x2C = k94;
#endif
				angle = 6.2831855f * (a5 + (f32)((i - 1) * 360 / (effCount - 1))) / 360.0f;
				sinAngle = (f32)sin(angle);
				entry->position.x = 10.0f * -sinAngle;
				entry->position.y = 0.0f;
				cosAngle = (f32)cos(angle);
				entry->position.z = 10.0f * -cosAngle;
#ifdef MUST_MATCH
				s30 = sin(angle);
				entry->field_0x30 = s30;
				c34 = cos(angle);
				entry->field_0x34 = c34;
#else
				entry->field_0x30 = sin(angle);
				entry->field_0x34 = cos(angle);
#endif
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
	b = (s32)(16.0f * (f32)data->field_0x3C / (f32)data->field_0x4C);
	a = (s32)(16.0f * (f32)(data->field_0x3C + 1) / (f32)data->field_0x4C);
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

	scale = 1.0f;
	switch (type) {
	case 5:
	case 6:
	case 7:
		scale = 0.5f;
		break;
	default:
		break;
	}

	PSMTXTrans(mtxTmp, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(mtxRot, 'y', -camGetPtr(camId)->viewYaw * 0.017453292f);
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

	scrollX = 0.0625f * (f32)field0;
	scrollY = 0.0625f * (f32)field1;

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		if (data->field_0x40 > 15) {
			continue;
		}

		PSMTXTrans(mtxTmp, scrollX, 0.0f, 0.0f);
		PSMTXScale(mtxScale, 0.0625f, 1.0f, 1.0f);
		PSMTXConcat(mtxTmp, mtxScale, mtxTmp);
		GXLoadTexMtxImm(mtxTmp, 0x1E, 1);

		PSMTXTrans(mtxTmp, scrollY, 0.0f, 0.0f);
		PSMTXScale(mtxScale, 0.0625f, 1.0f, 1.0f);
		PSMTXConcat(mtxTmp, mtxScale, mtxTmp);
		GXLoadTexMtxImm(mtxTmp, 0x21, 1);

		for (j = 0; j < 2; j++) {
			if (j == 0) {
#ifdef MUST_MATCH
				kemuri1SetChanMatColorA();
#else
				GXSetChanMatColor(4, dat_8041f234);
#endif
				PSMTXTrans(mtxTmp, 2.0f + data->position.x,
					data->position.y - 2.0f, data->position.z);
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
				size = 16.0f;
				size2 = size;
				break;
			case 1:
				size = 24.0f;
				size2 = size;
				break;
			default:
				size = 32.0f;
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
				size = 16.0f;
				break;
			case 1:
				size = 24.0f;
				break;
			default:
				size = 32.0f;
				break;
			}

			GXPosition3f32(-size * half, size * half, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(size * half, size * half, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);
			GXPosition3f32(size * half, -size * half, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);
			GXPosition3f32(-size * half, -size * half, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
#endif
		}
	}
}

// The unit's float constants are the compiler's own pool entries; the only named .sdata2 objects are the
// two colours above. The local `arg` in effKemuri1N64Entry keeps sin/cos from folding their arguments into
// rodata doubles, and splitting the reviseAngle expression pools 90/180 before 6.28 as the original does.

// 4-byte zero word that ends the unit's .rodata: an all-zero object needs explicit_zero_data to be emitted
// and force_active to survive the linker.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".rodata") const u32 gap_04_802FA8AC_rodata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset

