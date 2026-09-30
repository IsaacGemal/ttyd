#include "eff/n64/eff_kemuri8_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"

#include <dolphin/mtx.h>
#include "math_ext.h"

// .sdata2 constants (defined by this unit in the original), named after their asm addresses.
extern const GXColor dat_8041f420;
extern const f32 float_deg2rad_8041f424;
extern const f32 float_1_8041f428;
extern const f32 float_0p0625_8041f42c;
extern const f32 float_0_8041f430;
extern const f32 float_2_8041f434;
extern const f32 float_0p8_8041f438;
extern const f32 float_0p4_8041f43c;
extern const f32 float_32_8041f440;
extern const f32 float_24_8041f444;
extern const f32 float_0p5_8041f448;
extern const f32 float_16_8041f44c;
extern const f32 float_14_8041f450;
extern const f32 float_28_8041f454;
extern const f32 float_42_8041f458;
extern const f32 float_neg3_8041f45c;
extern const f32 float_0p2_8041f460;
extern const f32 float_1p75_8041f464;
extern const f32 float_neg0p16_8041f468;
extern const f32 float_6p2832_8041f46c;
extern const f32 float_360_8041f470;

// .sbss2 zero GXColor scratch objects the original builds its material colors from.
extern GXColor unk_80423170;
extern GXColor unk_80423174;

// .rodata constants of this unit.
extern const char str_802FA988[];
extern const f64 double_to_int_802fa980;

// One 0x5C-byte element of this effect's userdata array.
typedef struct effKemuri8Particle {
	u16 type;       // 0x0
	u8 pad_02[2];   // 0x2
	Vec position;   // 0x4
	f32 field_0x10; // 0x10
	f32 field_0x14; // 0x14
	f32 field_0x18; // 0x18
	f32 field_0x1C; // 0x1C
	f32 field_0x20; // 0x20
	f32 field_0x24; // 0x24
	f32 field_0x28; // 0x28
	f32 field_0x2C; // 0x2C
	f32 field_0x30; // 0x30
	f32 field_0x34; // 0x34
	s32 field_0x38; // 0x38
	s32 field_0x3C; // 0x3C
	s32 field_0x40; // 0x40
	u8 pad_44[4];   // 0x44
	s32 field_0x48; // 0x48
	s32 field_0x4C; // 0x4C
	s32 field_0x50; // 0x50
	s32 field_0x54; // 0x54
	s32 camId;      // 0x58
} effKemuri8Particle;

// Local copy of system.c's inline helper so it inlines here as in the original.
inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

void effKemuri8N64SetCamId(EffectEntry* entry, s32 camId) {
	((effKemuri8Data*)entry->userdata)->camId = camId;
}

// NONMATCHING (99.18%): instruction sequence and integer registers match; the ten callee-saved FP
// registers are rotated (original scale f31, constants f23-f30, params f26/f27/f22; ours scale f24,
// constants f25-f31/f23, params f28/f23/f22). The compiler-generated int->double magic always takes
// f31 here, where the original gives it f29 and puts the (loop-longest-lived) scale in f31.
EffectEntry* effKemuri8N64Entry(s32 type, f32 x, f32 y, f32 z) {
	EffectEntry* effect;
	effKemuri8Particle* data;
	effKemuri8Particle* p;
	s32 i;
	s32 count;
	s32 flag;
	f32 angle;
	f32 scale;

	effect = effEntry();
	flag = 0;
	if (type >= 3) {
		flag = 1;
		type -= 3;
	}
	switch (type) {
	case 0:
		scale = float_14_8041f450;
		count = 7;
		break;
	case 1:
		scale = float_28_8041f454;
		count = 9;
		break;
	default:
		count = 11;
		scale = float_42_8041f458;
		break;
	}
	effect->type = str_802FA988;
	effect->count = count;
	data = __memAlloc(HEAP_EFFECT, count * 0x5C);
	effect->userdata = data;
	effect->callback = effKemuri8Main;

	data->field_0x38 = 0;
	data->field_0x3C = 0;
	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->field_0x40 = 0;
	data->field_0x48 = 0;
	data->field_0x4C = 17;
	data->camId = 4;

	p = data + 1;
	for (i = 1; i < count; i++, p++) {
		p->field_0x10 = float_neg3_8041f45c;
		p->field_0x14 = float_0p2_8041f460;
		p->field_0x18 = float_0_8041f430;
		p->field_0x1C = float_0_8041f430;
		p->field_0x20 = float_1p75_8041f464;
		p->field_0x24 = float_neg0p16_8041f468;
		p->field_0x28 = float_0_8041f430;
		p->field_0x2C = float_0_8041f430;
		angle = float_6p2832_8041f46c * (f32)(((i - 1) * 360) / (count - 1)) / float_360_8041f470;
		p->position.x = scale * -(f32)sin(angle);
		p->position.y = scale * -(f32)cos(angle);
		p->position.z = float_0_8041f430;
		p->field_0x30 = (f32)sin(angle);
		p->field_0x34 = (f32)cos(angle);
		p->field_0x50 = flag;
		if (flag == 0) {
			p->pad_44[0] = 0xEE;
			p->pad_44[1] = 0xDE;
			p->pad_44[2] = 0xD7;
			p->pad_44[3] = 0xA0;
		} else {
			p->pad_44[0] = 0xFF;
			p->pad_44[1] = 0xFF;
			p->pad_44[2] = 0xFF;
			p->pad_44[3] = 0xA0;
			p->field_0x54 = 0;
		}
	}
	return effect;
}

void effKemuri8Main(EffectEntry* effect) {
	effKemuri8Particle* data = effect->userdata;
	Vec position;
	s32 camId;
	s32 i;
	// b before a: the original's register allocation (a in r4, b in r0) depends on this order.
	s32 b;
	s32 a;

	position = initializeVec(&data->position);
	camId = data->camId;
	b = (s32)(float_16_8041f44c * (f32)data->field_0x3C / (f32)data->field_0x4C);
	a = (s32)(float_16_8041f44c * (f32)(data->field_0x3C + 1) / (f32)data->field_0x4C);
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

	for (i = 1, data++; i < effect->count; i++, data++) {
		data->field_0x18 += data->field_0x1C;
		data->field_0x14 += data->field_0x18;
		data->field_0x10 += data->field_0x14;
		data->position.x += data->field_0x10 * data->field_0x30;
		data->position.y += data->field_0x10 * data->field_0x34;
	}

	dispEntry(camId, dispCalcZ(&position), 2, effKemuri8Disp, effect);
}

// NONMATCHING (98.51%): compiler stack-slot choice for the three by-value GXColor temporaries
// (ours 0x10/0x18/0x14 vs 0x18/0x10/0x8) and the vertex-block half-size CSE (original keeps
// size*0.5 and -size*0.5 in f2/f3/f1/f4; ours folds to f1/f2). See REPORT.md for what was tried.
void effKemuri8Disp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	effKemuri8Particle* data;
	CameraEntry* camera;
	GXTexObj texObj;
	Mtx transMtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	Mtx outMtx;
	f32 uOffset;
	f32 vOffset;
	s32 frameU;
	s32 frameV;
	f32 size;
	s32 type;
	s32 i;
	s32 j;

	data = effect->userdata;
	camera = camGetPtr(cameraId);
	type = data->type;
	frameU = data->field_0x40;
	frameV = data->field_0x48;

	PSMTXTrans(transMtx, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(rotMtx, 'y', float_deg2rad_8041f424 * -camGetPtr(cameraId)->viewYaw);
	PSMTXScale(scaleMtx, float_1_8041f428, float_1_8041f428, float_1_8041f428);
	PSMTXConcat(transMtx, rotMtx, transMtx);
	PSMTXConcat(transMtx, scaleMtx, transMtx);
	PSMTXConcat(camera->view, transMtx, outMtx);

	effGetTexObj(0x28, &texObj);
	GXLoadTexObj(&texObj, 0);
	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	GXSetTexCoordGen2(1, 1, 4, 0x21, 0, 0x7d);
	GXSetNumTevStages(3);
	GXSetTevOrder(0, 0, 0, 0xff);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xf, 0xf, 0xf, 8);
	GXSetTevAlphaIn(0, 7, 7, 7, 4);
	GXSetTevOrder(1, 1, 0, 0xff);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 0, 8, 2, 0xf);
	GXSetTevAlphaIn(1, 0, 4, 1, 7);
	GXSetTevColor(1, dat_8041f420);
	GXSetTevOrder(2, 0xff, 0xff, 4);
	GXSetTevColorOp(2, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
	GXSetTevColorIn(2, 0xf, 0, 0xa, 0xf);
	GXSetTevAlphaIn(2, 7, 0, 5, 7);
	GXSetCullMode(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xd, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xd, 1, 4, 0);

	uOffset = float_0p0625_8041f42c * (f32)frameU;
	vOffset = float_0p0625_8041f42c * (f32)frameV;

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		if (data->field_0x40 > 15) {
			continue;
		}
		PSMTXTrans(transMtx, uOffset, float_0_8041f430, float_0_8041f430);
		PSMTXScale(scaleMtx, float_0p0625_8041f42c, float_1_8041f428, float_1_8041f428);
		PSMTXConcat(transMtx, scaleMtx, transMtx);
		GXLoadTexMtxImm(transMtx, 0x1e, 1);
		PSMTXTrans(transMtx, vOffset, float_0_8041f430, float_0_8041f430);
		PSMTXScale(scaleMtx, float_0p0625_8041f42c, float_1_8041f428, float_1_8041f428);
		PSMTXConcat(transMtx, scaleMtx, transMtx);
		GXLoadTexMtxImm(transMtx, 0x21, 1);

		for (j = 0; j < 2; j++) {
			if (j == 0) {
				GXColor color = unk_80423170;
				color.a = (data->pad_44[3] * 0x60) / 255;
				GXSetChanMatColor(4, color);
				PSMTXTrans(transMtx, float_2_8041f434 + data->position.x, data->position.y - float_2_8041f434, data->position.z);
				PSMTXConcat(outMtx, transMtx, transMtx);
				GXLoadPosMtxImm(transMtx, 0);
			} else {
				if (data->field_0x50 != 0) {
					s32 red;
					s32 green;
					s32 blue;

					green = 0xff;
					red = 0xff;
					blue = 0xff;

					blue = 0xff - data->field_0x54 * 60;
					if (blue < 0) {
						green += (s32)(float_0p8_8041f438 * (f32)blue);
						blue = 0;
						if (green < 0) {
							red += (s32)(float_0p4_8041f43c * (f32)green);
							green = 0;
							if (red < 0) {
								red = 0;
							}
						}
					}
					data->pad_44[0] = red;
					data->pad_44[1] = green;
					data->pad_44[2] = blue;
					data->field_0x54++;
				}
				{
					GXColor color = unk_80423174;
					color.r = data->pad_44[0];
					color.g = data->pad_44[1];
					color.b = data->pad_44[2];
					color.a = data->pad_44[3];
					GXSetChanMatColor(4, color);
				}
				PSMTXTrans(transMtx, data->position.x, data->position.y, data->position.z);
				PSMTXConcat(outMtx, transMtx, transMtx);
				GXLoadPosMtxImm(transMtx, 0);
			}

			GXBegin(0x80, 0, 4);
			size = (type != 0) ? float_32_8041f440 : float_24_8041f444;
			GXPosition3f32(-size * 0.5f, size * 0.5f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(size * 0.5f, size * 0.5f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);
			GXPosition3f32(size * 0.5f, -size * 0.5f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);
			GXPosition3f32(-size * 0.5f, -size * 0.5f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
		}
	}
}
