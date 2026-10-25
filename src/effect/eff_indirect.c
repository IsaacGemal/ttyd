#include "eff/eff_indirect.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

typedef struct IndirectData {
	s32 type;              // 0x00
	f32 x;                 // 0x04
	f32 y;                 // 0x08
	f32 z;                 // 0x0C
	f32 rotX;              // 0x10
	f32 rotY;              // 0x14
	f32 rotZ;              // 0x18
	f32 scale;             // 0x1C
	u8 alpha;              // 0x20
	u8 pad_0x21[7];        // 0x21
	s32 timer;             // 0x28
	s32 field_0x2C;        // 0x2C
	f32 field_0x30;        // 0x30
	smartEntry* texBuffer; // 0x34
} IndirectData;

EffectEntry* effIndirectEntry(s32 type, s32 timer, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	IndirectData* data;

	entry = effEntry();
	entry->type = "Indirect";
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(IndirectData));
	entry->userdata = data;
	entry->callback = effIndirectMain;
	entry->flags |= 2;

	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = 0.0f;
	data->alpha = 0xFF;
	data->texBuffer = smartAlloc(GXGetTexBufferSize(0x40, 0x40, 3, 0, 0), 0);
	data->rotX = 0.0f;
	data->rotY = 0.0f;
	data->rotZ = 0.0f;
	data->timer = timer;
	data->field_0x2C = 0;
	data->field_0x30 = -0.05f;

	return entry;
}

void effIndirectMain(EffectEntry* effect) {
	IndirectData* data;
	Vec pos2;
	Vec pos;

	data = effect->userdata;
	pos = (Vec){ 0.0f, 0.0f, 0.0f };
	pos.x = data->x;
	pos.y = data->y;
	pos.z = data->z;
	pos2 = pos;

	switch (data->type) {
	case 0:
	case 1:
		if (effect->flags & 4) {
			effect->flags &= ~4;
			data->timer = 0x10;
		}
		if (data->timer < 1000) {
			data->timer--;
		}
		if (data->timer < 0) {
			smartFree(data->texBuffer);
			effDelete(effect);
			return;
		}
		if (data->timer < 0x20) {
			data->alpha = data->timer << 3;
		}
		data->rotZ += -5.0f;
		data->scale += 1.0f;
		break;
	}

	dispEntry(CAMERA_3D_EFFECTB, dispCalcZ(&pos2), 2, effIndirectDisp, effect);
}

void effIndirectDisp(CameraId camId, void* param) {
	f32 sixteen;
	EffectEntry* effect = param;
	IndirectData* data = effect->userdata;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx lightMtx;
	Mtx rotMtx;
	Mtx scaleMtx;
	Mtx trans;
	GXColor color;
	s32 type;
	u8 alpha;
	void* image;
	// half must be declared before u/v so MWCC assigns it f2 (target: half f2, u f3, v f4).
	f32 half;
	f32 u;
	f32 v;

	cam = camGetPtr(CAMERA_2D);
	type = data->type;
	alpha = data->alpha;
	GXSetBlendMode(1, 4, 5, 0);
	GXSetZCompLoc(1);
	GXSetAlphaCompare(7, 0, 0, 7, 0);
	GXSetZMode(0, 3, 0);
	switch (type) {
	case 0:
		effGetTexObj(0x86, &texObj);
		GXLoadTexObj(&texObj, 0);
		break;
	case 1:
		effGetTexObj(0x45, &texObj);
		GXLoadTexObj(&texObj, 0);
		break;
	}
	GXSetTexCopySrc(0, 0, 0x260, 0x1E0);
	GXSetTexCopyDst(0x130, 0xF0, 4, 1);
	image = smartAlloc(GXGetTexBufferSize(0x130, 0xF0, 4, 0, 0), 3)->address;
	GXCopyTex(image, 0);
	GXPixModeSync();
	GXInitTexObj(&texObj, image, 0x130, 0xF0, 4, 0, 0, 0);
	GXLoadTexObj(&texObj, 1);
	PSMTXTrans(lightMtx, data->x, data->y, data->z);
	PSMTXScale(scaleMtx, data->scale, data->scale, data->scale);
	PSMTXRotRad(rotMtx, 'y', 0.017453292f * -cam->viewYaw);
	PSMTXConcat(lightMtx, scaleMtx, scaleMtx);
	PSMTXConcat(scaleMtx, rotMtx, trans);
	PSMTXRotRad(rotMtx, 'z', 0.017453292f * data->rotZ);
	PSMTXConcat(trans, rotMtx, trans);
	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
	GXSetChanMatColor(4, (GXColor){ 0xFF, 0xFF, 0xFF, alpha });
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
	GXSetTexCoordGen2(1, 0, 0, 0x1E, 0, 0x7D);
	C_MTXLightPerspective(lightMtx, 25.0f, 1.2666667f, 0.5f, -0.5f,
	                      0.505f, 0.505f);
	PSMTXConcat(lightMtx, cam->view, lightMtx);
	PSMTXConcat(lightMtx, trans, lightMtx);
	GXLoadTexMtxImm(lightMtx, 0x1E, 0);
	GXSetNumTevStages(2);
	GXSetTevOrder(0, 0, 0, 4);
	GXSetTevOp(0, 0);
	GXSetTevOrder(1, 1, 1, 0xFF);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 0xF, 0xF, 0xF, 8);
	GXSetTevAlphaIn(1, 7, 7, 7, 0);
	GXSetCullMode(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xD, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
	PSMTXConcat(cam->view, trans, trans);
	GXLoadPosMtxImm(trans, 0);
	GXSetCurrentMtx(0);
	GXBegin(0x80, 0, 4);
	u = 1.0f;
	v = u;
	switch (type) {
	case 0:
		u = 2.0f;
		break;
	case 1:
		u = 2.0f;
		v = u;
		break;
	}
#ifdef MUST_MATCH
	// The two locals keep MWCC from folding -32 * 0.5 into one -16.0f constant;
	// the target loads 32 and 0.5 and does the fneg and fmuls at run time.
	{
		f32 w = 32.0f;
		f32 h = 0.5f;
		half = -w * h;
	}
#else
	half = -32.0f * 0.5f;
#endif
	sixteen = 16.0f;
	GXPosition3f32(half, sixteen, 0.0f);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3f32(sixteen, sixteen, 0.0f);
	GXTexCoord2f32(u, 0.0f);
	GXPosition3f32(sixteen, half, 0.0f);
	GXTexCoord2f32(u, v);
	GXPosition3f32(half, half, 0.0f);
	GXTexCoord2f32(0.0f, v);
}
