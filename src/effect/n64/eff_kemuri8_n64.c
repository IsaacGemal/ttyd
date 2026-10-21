#include "eff/n64/eff_kemuri8_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"

// .sbss2: the default .sbss2 is only used for compiler-generated pool templates, so pair it with a small-data section.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_80423174;
__declspec(section ".sdata") GXColor unk_80423170;


#include <dolphin/mtx.h>
#include "math_ext.h"

// .sbss2 zero GXColor scratch objects the original builds its material colors from.

// .rodata constants of this unit.

// One 0x5C-byte element of this effect's userdata array.
typedef struct effKemuri8Particle {
	u16 type;              // 0x0
	u8 pad_02[2];          // 0x2
	Vec position;          // 0x4
	f32 distance;          // 0x10
	f32 velocity;          // 0x14
	f32 acceleration;      // 0x18
	f32 accelerationStep;  // 0x1C
	f32 field_0x20;        // 0x20
	f32 field_0x24;        // 0x24
	f32 field_0x28;        // 0x28
	f32 field_0x2C;        // 0x2C
	f32 dirX;              // 0x30
	f32 dirY;              // 0x34
	s32 field_0x38;        // 0x38
	s32 age;               // 0x3C
	s32 frame;             // 0x40: sprite-sheet frame (of 16), GX_TEXMTX0 U offset
	GXColor color;         // 0x44
	s32 nextFrame;         // 0x48: the following frame, GX_TEXMTX1 U offset
	s32 lifetime;          // 0x4C
	s32 fade;              // 0x50
	s32 fadeTimer;         // 0x54
	s32 camId;             // 0x58
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

// The sin/cos results go through separate function-scope temps (sinAngle, cosAngle): this is the source shape the
// original used, and it is what makes MWCC pick the original's callee-saved FPR colouring (x f26,
// y f27, z/angle f22, scale f31, constants f23-f30); inlining the calls into the multiply rotates them.
EffectEntry* effKemuri8N64Entry(s32 type, f32 x, f32 y, f32 z) {
	EffectEntry* effect;
	effKemuri8Particle* data;
	effKemuri8Particle* p;
	s32 i;
	s32 count;
	s32 flag;
	f32 angle;
	f32 scale;
	f32 sinAngle;
	f32 cosAngle;

	effect = effEntry();
	flag = 0;
	if (type >= 3) {
		flag = 1;
		type -= 3;
	}
	switch (type) {
	case 0:
		scale = 14.0f;
		count = 7;
		break;
	case 1:
		scale = 28.0f;
		count = 9;
		break;
	default:
		count = 11;
		scale = 42.0f;
		break;
	}
	effect->type = "Kemuri8N64";
	effect->count = count;
	data = __memAlloc(HEAP_EFFECT, count * 0x5C);
	effect->userdata = data;
	effect->callback = effKemuri8Main;

	data->field_0x38 = 0;
	data->age = 0;
	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->frame = 0;
	data->nextFrame = 0;
	data->lifetime = 17;
	data->camId = 4;

	p = data + 1;
	for (i = 1; i < count; i++, p++) {
		p->distance = -3.0f;
		p->velocity = 0.19999999f;
		p->acceleration = 0.0f;
		p->accelerationStep = 0.0f;
		p->field_0x20 = 1.75f;
		p->field_0x24 = -0.16f;
		p->field_0x28 = 0.0f;
		p->field_0x2C = 0.0f;
		angle = 6.2831855f * (f32)(((i - 1) * 360) / (count - 1)) / 360.f;
		sinAngle = (f32)sin(angle);
		p->position.x = scale * -sinAngle;
		cosAngle = (f32)cos(angle);
		p->position.y = scale * -cosAngle;
		p->position.z = 0.0f;
		p->dirX = (f32)sin(angle);
		p->dirY = (f32)cos(angle);
		p->fade = flag;
		if (flag == 0) {
			p->color.r = 0xEE;
			p->color.g = 0xDE;
			p->color.b = 0xD7;
			p->color.a = 0xA0;
		} else {
			p->color.r = 0xFF;
			p->color.g = 0xFF;
			p->color.b = 0xFF;
			p->color.a = 0xA0;
			p->fadeTimer = 0;
		}
	}
	return effect;
}

void effKemuri8Main(EffectEntry* effect) {
	effKemuri8Particle* data = effect->userdata;
	Vec position;
	s32 camId;
	s32 i;
	// frame before nextFrame: the original's register allocation (nextFrame in r4, frame in r0)
	// depends on this order.
	s32 frame;
	s32 nextFrame;

	position = initializeVec(&data->position);
	camId = data->camId;
	frame = (s32)(16.0f * (f32)data->age / (f32)data->lifetime);
	nextFrame = (s32)(16.0f * (f32)(data->age + 1) / (f32)data->lifetime);
	data->age++;
	if (nextFrame > 15) {
		nextFrame = 15;
	}
	if (frame > 15) {
		effDelete(effect);
		return;
	}
	data->frame = frame;
	data->nextFrame = nextFrame;

	for (i = 1, data++; i < effect->count; i++, data++) {
		data->acceleration += data->accelerationStep;
		data->velocity += data->acceleration;
		data->distance += data->velocity;
		data->position.x += data->distance * data->dirX;
		data->position.y += data->distance * data->dirY;
	}

	dispEntry(camId, dispCalcZ(&position), 2, effKemuri8Disp, effect);
}

static inline void kemuri8SetChanMatColorA(u8 a) {
	GXColor c = unk_80423170;
	c.a = a;
	GXSetChanMatColor(4, c);
}

static inline void kemuri8SetChanMatColorB(u8 r, u8 g, u8 b, u8 a) {
	GXColor c = unk_80423174;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	GXSetChanMatColor(4, c);
}

void effKemuri8Disp(CameraId cameraId, void* param) {
	f32 half = 0.5f;
	EffectEntry* effect = param;
	effKemuri8Particle* data;
	CameraEntry* camera;
	GXTexObj texObj;
	Mtx transMtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	Mtx outMtx;
	f32 frameOffset;
	f32 nextFrameOffset;
	s32 frame;
	s32 nextFrame;
	f32 size;
	f32 size2;
	s32 type;
	s32 i;
	s32 j;

	data = effect->userdata;
	camera = camGetPtr(cameraId);
	type = data->type;
	frame = data->frame;
	nextFrame = data->nextFrame;

	PSMTXTrans(transMtx, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(rotMtx, 'y', 0.017453292f * -camGetPtr(cameraId)->viewYaw);
	PSMTXScale(scaleMtx, 1.0f, 1.0f, 1.0f);
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
	GXSetTevColor(1, (GXColor){ 0x80, 0x80, 0x80, 0x80 });
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

	frameOffset = 0.0625f * (f32)frame;
	nextFrameOffset = 0.0625f * (f32)nextFrame;

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		if (data->frame > 15) {
			continue;
		}
		PSMTXTrans(transMtx, frameOffset, 0.0f, 0.0f);
		PSMTXScale(scaleMtx, 0.0625f, 1.0f, 1.0f);
		PSMTXConcat(transMtx, scaleMtx, transMtx);
		GXLoadTexMtxImm(transMtx, 0x1e, 1);
		PSMTXTrans(transMtx, nextFrameOffset, 0.0f, 0.0f);
		PSMTXScale(scaleMtx, 0.0625f, 1.0f, 1.0f);
		PSMTXConcat(transMtx, scaleMtx, transMtx);
		GXLoadTexMtxImm(transMtx, 0x21, 1);

		for (j = 0; j < 2; j++) {
			if (j == 0) {
				kemuri8SetChanMatColorA((data->color.a * 0x60) / 255);
				PSMTXTrans(transMtx, 2.0f + data->position.x, data->position.y - 2.0f, data->position.z);
				PSMTXConcat(outMtx, transMtx, transMtx);
				GXLoadPosMtxImm(transMtx, 0);
			} else {
				if (data->fade != 0) {
					s32 red;
					s32 green;
					s32 blue;

					green = 0xff;
					red = 0xff;
					blue = 0xff;

					blue = 0xff - data->fadeTimer * 60;
					if (blue < 0) {
						green += (s32)(0.8f * (f32)blue);
						blue = 0;
						if (green < 0) {
							red += (s32)(0.4f * (f32)green);
							green = 0;
							if (red < 0) {
								red = 0;
							}
						}
					}
					data->color.r = red;
					data->color.g = green;
					data->color.b = blue;
					data->fadeTimer++;
				}
				kemuri8SetChanMatColorB(data->color.r, data->color.g,
					data->color.b, data->color.a);
				PSMTXTrans(transMtx, data->position.x, data->position.y, data->position.z);
				PSMTXConcat(outMtx, transMtx, transMtx);
				GXLoadPosMtxImm(transMtx, 0);
			}

			GXBegin(0x80, 0, 4);
			if (type != 0) {
				size2 = size = 32.0f;
			}
			else {
				size2 = size = 24.0f;
			}
			GXPosition3f32(-size2 * half, size * half, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(size2 * half, size * half, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);
			GXPosition3f32(size2 * half, -size * half, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);
			GXPosition3f32(-size2 * half, -size * half, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
		}
	}
}
