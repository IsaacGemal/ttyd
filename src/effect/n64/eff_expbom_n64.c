#include "eff/n64/eff_expbom_n64.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include "math_ext.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdlib.h>

typedef struct effExpBomData {
	s16 frame; //0x0
	s16 life; //0x2
	s16 duration; //0x4
	s16 dragTimer; //0x6
	s32 dispTimer; //0x8
	Vec position; //0xC
	f32 offsetX; //0x18
	f32 offsetY; //0x1C
	f32 velX; //0x20
	f32 velY; //0x24
	u8 pad_0x28[0x2C - 0x28]; //0x28
} effExpBomData;

//.sdata
f32 bom_dir[10] = {
	10.0f, 40.0f, 80.0f, 170.0f, 140.0f,
	100.0f, 25.0f, 155.0f, 60.0f, 120.0f
};

f32 bom_spd[] = {
	2.2f, 2.7f, 3.0f, 2.2f, 2.7f,
	3.0f, 1.9f, 1.9f, 1.5f, 1.5f
};

f32 bom_scale[10] = {
	1.4f, 1.3f, 1.2f, 1.3f, 1.4f,
	1.3f, 1.6f, 1.6f, 1.6f, 1.6f
};

f32 bom_ry[10] = {
	0.0f, 234.0f, 468.0f, 702.0f, 936.0f,
	1260.0f, 1404.0f, 1638.0f, 1902.0f, 1976.0f
};

//local prototypes

EffectEntry* effExpBomN64Entry(f32 x, f32 y, f32 z) {
	f32 *direction, *speed;
	effExpBomData* data;
	s32 random;
	// Declaration order pins the FPR/GPR allocation: sinAngle/angle/velMag/cosAngle -> f23/f22/f21/f2, and
	// effect declared last gets r31. The i=0-before-pointers and the increment order below are
	// likewise required to reproduce the original's scheduling.
	f32 sinAngle;
	f32 angle;
	f32 velMag;
	f32 cosAngle;
	int i;
	EffectEntry* effect = effEntry();

	effect->type = "ExpBomN64";
	effect->count = 10;
	data = __memAlloc(HEAP_EFFECT, sizeof(effExpBomData) * 10);
	effect->userdata = data;
	effect->callback = effExpBomMain;
	i = 0;
	direction = bom_dir;
	speed = bom_spd;
	for (; i < effect->count; direction++, speed++, i++, data++) {
		data->frame = 0;
		random = rand() % 6 + 16;
		data->duration = random;
		data->life = random;
		data->dragTimer = 4;
		data->dispTimer = 0;
		data->position.x = x;
		data->position.y = y;
		data->position.z = z;
		data->offsetX = 0.0f;
		data->offsetY = 0.0f;

		velMag = 2.0f * *speed;
		angle = ((6.2831855f * (90.0f + *direction)) / 360.0f);
		sinAngle = (f32)sin(angle);
		cosAngle = (f32)cos(angle);
		data->velX = velMag * sinAngle;
		data->velY = -velMag * cosAngle;
	}
	return effect;
}

static inline Vec effExpBomMakeVec(f32 x, f32 y, f32 z) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = x;
	temp.y = y;
	temp.z = z;
	return temp;
}

void effExpBomMain(EffectEntry* effect) {
	effExpBomData* data;
	int i;
	BOOL disp;
	Vec position2;
	Vec position;

	disp = FALSE;
	data = effect->userdata;
	position = effExpBomMakeVec(data->position.x, data->position.y, data->position.z);
	position2 = position;

	for (i = 0; i < effect->count; i++, data++) {
		if (data->life >= 0) {
			data->dispTimer--;
			if (data->dispTimer < 0) {
				disp = TRUE;
				data->offsetX += data->velX;
				data->offsetY += data->velY;
				data->frame++;
				data->life--;
				if (data->life >= 0) {
					data->dragTimer--;
					if (data->dragTimer < 0) {
						data->velX /= 1.25f;
						data->velY /= 1.25f;
					}
				}
			}
		}
	}
	if (!disp) {
		effDelete(effect);
	}
	else {
		dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effExpBomDisp, effect);
	}
}

void effExpBomDisp(CameraId camId, void* param) {
	EffectEntry* effect;
	effExpBomData* data;
	CameraEntry* camera;
	GXTexObj texobj;
	Mtx m0;
	Mtx m1;
	Mtx m2;
	Mtx viewMtx;
	s32 count;
	s32 i;
	s32 texIndex;

	effect = param;
	camera = camGetPtr(camId);
	data = effect->userdata;

	PSMTXTrans(m0, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(m1, 'y', -camGetPtr(camId)->viewYaw * 0.017453292f);
	PSMTXConcat(m0, m1, viewMtx);
	PSMTXConcat(camera->view, viewMtx, viewMtx);

	for (i = 0; i < effect->count; i++, data++) {
		if (data->life >= 0 && data->dispTimer < 0) {
			s32 alpha;
			f32 frame;
			f32 lastFrame;
			f32 t;

			count = data->life;
			if (count > 16) {
				count = 16;
			}
			PSMTXTrans(m0, data->offsetX, data->offsetY, 0.0f);
			PSMTXScale(m2, bom_scale[i], bom_scale[i], bom_scale[i]);
			PSMTXRotRad(m1, 'z', bom_ry[i] * 0.017453292f);
			PSMTXConcat(m0, m2, m0);
			PSMTXConcat(m0, m1, m1);
			PSMTXConcat(viewMtx, m1, m1);
			GXLoadPosMtxImm(m1, GX_PNMTX0);

			frame = (f32)data->frame;
			t = frame - 1.0f;
			lastFrame = (f32)data->duration;
			if (t >= lastFrame) {
				alpha = 0xFF;
				texIndex = 6;
			}
			else {
				f32 scaledT = 7.0f * t;
				f32 angleDeg = 90.0f * (scaledT / lastFrame) / 7.0f;
				f32 sine = (f32)sin(6.2831855f * angleDeg / 360.0f);
				f32 alphaF;

				sine = 7.0f * sine;
				alphaF = 255.0f * sine;
				alpha = (s32)alphaF % 256;
				texIndex = (s32)sine;
			}

			GXSetTevColor(GX_TEVREG0, (GXColor){ 0, 0, 0, alpha });

			GXSetTevColor(GX_TEVREG1, (GXColor){ 0, 0, 0, (count * 210) >> 4 });

			GXSetNumChans(0);
			GXSetNumTexGens(2);
			GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
			GXSetTexCoordGen2(1, 1, 4, 0x3C, 0, 0x7D);
			GXSetNumTevStages(3);
			GXSetTevOrder(0, 0, 0, 0xFF);
			GXSetTevColorOp(0, 0, 0, 0, 1, 0);
			GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
			GXSetTevColorIn(0, 0xF, 0xF, 0xF, 8);
			GXSetTevAlphaIn(0, 7, 7, 7, 4);
			GXSetTevOrder(1, 1, 1, 0xFF);
			GXSetTevColorOp(1, 0, 0, 0, 1, 0);
			GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
			GXSetTevColorIn(1, 0, 8, 3, 0xF);
			GXSetTevAlphaIn(1, 0, 4, 1, 7);
			GXSetTevOrder(2, 0xFF, 0xFF, 0xFF);
			GXSetTevColorOp(2, 0, 0, 0, 1, 0);
			GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
			GXSetTevColorIn(2, 0xF, 0xF, 0xF, 0);
			GXSetTevAlphaIn(2, 7, 0, 2, 7);
			GXSetCullMode(0);
			GXClearVtxDesc();
			GXSetVtxDesc(9, 1);
			GXSetVtxDesc(13, 1);
			GXSetVtxAttrFmt(0, 9, 1, 4, 0);
			GXSetVtxAttrFmt(0, 13, 1, 4, 0);

			switch (texIndex) {
			case 0:
				effGetTexObjN64(0, &texobj);
				GXLoadTexObj(&texobj, 0);
				effGetTexObjN64(1, &texobj);
				GXLoadTexObj(&texobj, 1);
				break;
			case 1:
				effGetTexObjN64(1, &texobj);
				GXLoadTexObj(&texobj, 0);
				effGetTexObjN64(2, &texobj);
				GXLoadTexObj(&texobj, 1);
				break;
			case 2:
				effGetTexObjN64(2, &texobj);
				GXLoadTexObj(&texobj, 0);
				effGetTexObjN64(3, &texobj);
				GXLoadTexObj(&texobj, 1);
				break;
			case 3:
				effGetTexObjN64(3, &texobj);
				GXLoadTexObj(&texobj, 0);
				effGetTexObjN64(4, &texobj);
				GXLoadTexObj(&texobj, 1);
				break;
			case 4:
				effGetTexObjN64(4, &texobj);
				GXLoadTexObj(&texobj, 0);
				effGetTexObjN64(5, &texobj);
				GXLoadTexObj(&texobj, 1);
				break;
			case 5:
				effGetTexObjN64(5, &texobj);
				GXLoadTexObj(&texobj, 0);
				effGetTexObjN64(6, &texobj);
				GXLoadTexObj(&texobj, 1);
				break;
			case 6:
				effGetTexObjN64(6, &texobj);
				GXLoadTexObj(&texobj, 0);
				effGetTexObjN64(6, &texobj);
				GXLoadTexObj(&texobj, 1);
				break;
			}

			GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			GXPosition3f32(-16.0f, 16.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(16.0f, 16.0f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);
			GXPosition3f32(16.0f, -16.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);
			GXPosition3f32(-16.0f, -16.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
		}
	}
}
