#include "eff/n64/eff_kemuri1_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"

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
extern GXColor dat_8041f230;
extern GXColor dat_8041f234;
extern GXColor unk_80423168;

typedef struct effKemuri1Data {
	u16 type; //0x0
	u8 align_2[2]; //0x2, TODO: verify align
	Vec position; //0x4
	u8 field_0x10[0x38 - 0x10]; //0x10
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

// NONMATCHING (17.27%): 2 instructions differ, 300 missing, 2 extra, plus register differences.
EffectEntry* effKemuri1N64Entry(s32 type, f32 x, f32 y, f32 z, f32 a5) {
	EffectEntry* effect;
	s32 effCount;
	effKemuri1Data* data;

	effect = effEntry();
	switch (type) {
		case 2:
			effCount = 1;
			break;
		case 3:
			effCount = 4;
			break;
		case 4: //TODO: -strict off, case 4 ... 7:?
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
		case 4: //TODO: -strict off, case 4 ... 7:?
		case 5:
		case 6:
		case 7:
			data->field_0x38 = 3;
			data->field_0x4C = 65;
			break;
		default:
			break;
	}
}

// NONMATCHING (0.52%): empty placeholder, not decompiled yet.
void effKemuri1Main(EffectEntry* effect) {

}

// Draws every live entry as a camera-facing textured quad.
// NONMATCHING (91.71%): the frame is 0x180 instead of 0x160 (two more saved FPRs, f27/f28), the
// shared `fneg` is CSE'd differently, registers are permuted, and the int->float magic double is a
// local @263@ instead of double_to_int_802fa898.
void effKemuri1Disp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effKemuri1Data* data = effect->userdata;
	CameraEntry* cam;
	GXTexObj texobj;
	Mtx mtxTmp;
	Mtx mtxScale;
	Mtx mtxRot;
	Mtx mtxBase;
	s32 field0;
	s32 field1;
	f32 scrollX;
	f32 scrollY;
	f32 scale;
	f32 size;
	s32 type;
	s32 i;
	s32 j;

	cam = camGetPtr(camId);
	type = data->type;
	field0 = data->field_0x40;
	field1 = data->field_0x48;

	scale = float_1_8041f238;
	if (type < 8) {
		if (type >= 5) {
			scale = float_0p5_8041f23c;
		}
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

	scrollX = (f32)field0 * float_0p0625_8041f244;
	scrollY = (f32)field1 * float_0p0625_8041f244;

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
				GXSetChanMatColor(4, dat_8041f234);
				PSMTXTrans(mtxTmp, data->position.x + float_2_8041f24c,
					data->position.y - float_2_8041f24c, data->position.z);
				PSMTXConcat(mtxBase, mtxTmp, mtxTmp);
				GXLoadPosMtxImm(mtxTmp, 0);
			}
			else {
				GXColor color = unk_80423168;
				color.r = data->field_0x44;
				color.g = data->field_0x45;
				color.b = data->field_0x46;
				color.a = 0xFF;
				GXSetChanMatColor(4, color);
				PSMTXTrans(mtxTmp, data->position.x, data->position.y, data->position.z);
				PSMTXConcat(mtxBase, mtxTmp, mtxTmp);
				GXLoadPosMtxImm(mtxTmp, 0);
			}

			GXBegin(0x80, 0, 4);

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

			GXPosition3f32(-size * float_0p5_8041f23c, size * float_0p5_8041f23c, float_0_8041f248);
			GXTexCoord2f32(float_0_8041f248, float_0_8041f248);
			GXPosition3f32(size * float_0p5_8041f23c, size * float_0p5_8041f23c, float_0_8041f248);
			GXTexCoord2f32(float_1_8041f238, float_0_8041f248);
			GXPosition3f32(size * float_0p5_8041f23c, -size * float_0p5_8041f23c, float_0_8041f248);
			GXTexCoord2f32(float_1_8041f238, float_1_8041f238);
			GXPosition3f32(-size * float_0p5_8041f23c, -size * float_0p5_8041f23c, float_0_8041f248);
			GXTexCoord2f32(float_0_8041f248, float_1_8041f238);
		}
	}
}

