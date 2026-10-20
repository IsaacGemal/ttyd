#include "eff/n64/eff_balloon_heiho_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/mapdrv.h"
#include "memory.h"

#include <dolphin/mtx.h>
#include <dolphin/gx.h>

EffectEntry* effBalloonHeihoN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 accel, f32 scale) {
	EffectEntry* effect;
	BalloonHeihoData* data;

	effect = effEntry();
	effect->type = "BalloonHeihoN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(BalloonHeihoData));
	effect->userdata = data;
	effect->callback = effBalloonHeihoMain;
	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->velY = 0.0f;
	data->life = count;
	data->counter = 0;
	data->accelY = accel;
	data->scale = scale;
	return effect;
}

inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

void effBalloonHeihoMain(EffectEntry* effect) {
	BalloonHeihoData* data = effect->userdata;
	Vec position = initializeVec(&data->position);

	data->life--;
	data->counter++;
	if (data->life < 0) {
		effDelete(effect);
	} else {
		data->velY += data->accelY;
		if (data->velY > 2.0f) {
			data->velY = 2.0f;
		}
		data->position.y += data->velY;
		dispEntry(4, dispCalcZ(&position), 2, effBalloonHeihoDisp, effect);
	}
}

static u32 size32x64_tex32x64_vtx[28] = {
	0xFFF00040, 0x00000000, 0x08000000, 0x00FF0010, 0x00400000, 0x04000800, 0x000000FF,
	0xFFF00020, 0x00000000, 0x04000000, 0x00FF0010, 0x00200000, 0x04000400, 0x000000FF,
	0xFFF00010, 0x00000000, 0x02000000, 0x00FF0010, 0x00100000, 0x04000200, 0x000000FF,
	0xFFF00000, 0x00000000, 0x00000000, 0x00FF0010, 0x00000000, 0x04000000, 0x000000FF,
};

void effBalloonHeihoDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	BalloonHeihoData* data;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx mtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	s32 type;

	cam = camGetPtr(camId);
	data = effect->userdata;
	type = data->type;
	mapSetMaterialFog();
	PSMTXTrans(mtx, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(rotMtx, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXScale(scaleMtx, data->scale, data->scale, 1.0f);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXConcat(mtx, scaleMtx, mtx);
	PSMTXConcat(cam->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, 0);
	GXSetCurrentMtx(0);
	switch (type) {
	case 0:
		effGetTexObjN64(0x60, &texObj);
		break;
	case 1:
		effGetTexObjN64(0x61, &texObj);
		break;
	case 2:
		effGetTexObjN64(0x62, &texObj);
		break;
	}
	GXLoadTexObj(&texObj, 0);
	GXSetNumChans(0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	PSMTXScale(scaleMtx, 0.03125f, 0.015625f, 0.0f);
	GXLoadTexMtxImm(scaleMtx, 0x1e, 1);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 0xff);
	GXSetTevOp(0, 3);
	GXSetCullMode(0);
	effSetVtxDescN64((effVtxArray*)size32x64_tex32x64_vtx);
	GXBegin(0x90, 0, 0x18);
	tri2(0, 1, 2, 0, 1, 2, 3, 0);
	tri2(0, 1, 2, 0, 1, 2, 3, 0);
	tri2(2, 3, 4, 0, 3, 4, 5, 0);
	tri2(4, 5, 6, 0, 5, 6, 7, 0);
}
