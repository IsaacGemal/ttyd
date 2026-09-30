#include "eff/eff_ice.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_kemutest.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

typedef struct IceRSTEntry {
	s32 count;      // 0x00
	void* hermit;   // 0x04
	s32 unk08;      // 0x08
	void* pPlane;   // 0x0C
} IceRSTEntry;

typedef struct IceAnimRSTData {
	s32 count;            // 0x00
	IceRSTEntry* entries; // 0x04
} IceAnimRSTData;

typedef struct IceAnimData {
	u32 unk00;               // 0x00
	f32 unk04;               // 0x04
	IceAnimRSTData* rstData; // 0x08
} IceAnimData;

typedef struct IceMayaAnimEntry {
	Mtx mtx; // 0x00
	f32 unk; // 0x30
} IceMayaAnimEntry;

typedef struct IceMayaAnim {
	u16 flags;                  // 0x00
	u8 pad_02[2];               // 0x02
	f32 frame;                  // 0x04
	OSTime time;                // 0x08
	IceAnimData* animData;      // 0x10
	IceMayaAnimEntry entries[]; // 0x14
} IceMayaAnim;

typedef struct IceData {
	s32 type;      // 0x00
	f32 x;         // 0x04
	f32 y;         // 0x08
	f32 z;         // 0x0C
	f32 scale;     // 0x10
	f32 scaleX;    // 0x14
	f32 scaleY;    // 0x18
	f32 scaleZ;    // 0x1C
	s32 timer;     // 0x20
	s32 frame;     // 0x24
	s32 state;     // 0x28
	IceMayaAnim* anim; // 0x2C
} IceData;


extern const f32 float_1_8041e3f4;
extern const f32 float_deg2rad_8041e3e8;
extern const f32 float_0_8041e3ec;
extern const f32 float_neg30_8041e3f0;
extern const f32 float_30_8041e3f8;
extern const f32 float_8_8041e3fc;
extern const f32 float_191p25_8041e400;
extern const f32 float_127p5_8041e404;
extern const f32 float_1000_8041e408;
extern const f32 float_40_8041e40c;
extern const f32 float_0p99_8041e410;
extern const f32 float_1p005_8041e414;

extern const char str_8041E418[];
extern u32 ice_effAnimRSTDt[];
__declspec(section ".sdata") u32 ice_effAnimDt[] = { 0x00000000, 0x42EE0000, (u32)ice_effAnimRSTDt };


// effIceMain is local to this file in the original object (.fn effIceMain, local).
static void effIceMain(EffectEntry* effect);
// effIceDisp is local to this file in the original object (.fn effIceDisp, local).
static void effIceDisp(CameraId cameraId, void* param);

EffectEntry* effIceEntry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	IceData* data;

	effect = effEntry();
	effect->type = str_8041E418;
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x30);
	effect->userdata = data;
	effect->callback = effIceMain;
	effect->flags |= 2;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->scaleX = float_1_8041e3f4;
	data->scaleY = float_1_8041e3f4;
	data->scaleZ = float_1_8041e3f4;
	if (count > 0) {
		data->timer = count;
	} else {
		data->timer = 0x3E8;
	}
	data->frame = 0;
	data->state = 0;
	data->anim = effMayaAnimAlloc(ice_effAnimDt);
	effPlayMayaAnim(data->anim);
	return effect;
}

// effIceMain is local to this file in the original object (.fn effIceMain, local).
static void effIceMain(EffectEntry* effect) {
	Vec pos;
	Vec disp = { 0 };
	OSTime time;
	u64 msecPerFrame;
	IceData* data = effect->userdata;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
	pos = disp;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->frame = 90;
		data->state = 1;
		if (gp->inBattle != 0) {
			time = gp->renderTime;
		} else {
			time = gp->renderFieldTime;
		}
		msecPerFrame = (u64)(float_1000_8041e408 / (f32)gp->framerate);
		data->anim->time = OSTicksToMilliseconds(time) - msecPerFrame * data->frame;
	}

	switch (data->state) {
	case 0:
		if (data->timer < 1000) {
			data->timer--;
		}
		if (data->timer == 0) {
			data->state = 1;
		}
		if (data->frame < 90) {
			data->frame++;
			effCalcMayaAnim(data->anim);
		}
		break;
	case 1:
		data->frame++;
		effCalcMayaAnim(data->anim);
		break;
	case 2:
		data->frame++;
		if (data->frame % 30 == 0) {
			f32 argx = data->x + (f32)(20 - irand((s32)(float_40_8041e40c * data->scale)));

			effKemuTestEntry(1, argx,
				data->y - float_30_8041e3f8 * (float_1_8041e3f4 - data->scaleY), data->z, float_1_8041e3f4);
		}
		data->scaleY *= float_0p99_8041e410;
		data->scaleX *= float_1p005_8041e414;
		data->scaleZ *= float_0p99_8041e410;
		break;
	}

	switch (data->state) {
	case 0:
	case 1:
		if ((f32)data->frame > data->anim->animData->unk04) {
			effDeleteMayaAnim(data->anim);
			effDelete(effect);
			return;
		}
		break;
	case 2:
		if (data->frame > 200) {
			effDeleteMayaAnim(data->anim);
			effDelete(effect);
			return;
		}
		break;
	}

	dispEntry(CAMERA_3D, dispCalcZ(&pos), 2, effIceDisp, effect);
}

// effIceDisp is local to this file in the original object (.fn effIceDisp, local).
static void effIceDisp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	IceData* data = effect->userdata;
	GXTexObj texObj;
	Mtx mtxA;
	Mtx mtxB;
	Mtx mtxC;
	Mtx mtxD;
	Mtx mtxE;
	s32 i;

	PSMTXTrans(mtxA, data->x, data->y, data->z);
	PSMTXRotRad(mtxB, 'y', -camGetPtr(cameraId)->viewYaw * float_deg2rad_8041e3e8);
	PSMTXConcat(mtxA, mtxB, mtxC);
	PSMTXTrans(mtxA, float_0_8041e3ec, float_neg30_8041e3f0, float_0_8041e3ec);
	PSMTXConcat(mtxC, mtxA, mtxC);
	PSMTXScale(mtxD, data->scaleX, data->scaleY, float_1_8041e3f4);
	PSMTXConcat(mtxC, mtxD, mtxC);
	PSMTXTrans(mtxA, float_0_8041e3ec, float_30_8041e3f8, float_0_8041e3ec);
	PSMTXConcat(mtxC, mtxA, mtxC);
	PSMTXScale(mtxD, float_8_8041e3fc * data->scale, float_8_8041e3fc * data->scale, float_8_8041e3fc * data->scale);
	PSMTXConcat(mtxC, mtxD, mtxC);

	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	GXSetCurrentMtx(0);

	for (i = 1; i < data->anim->animData->rstData->count; i++) {
		s32 texId = 0x20;

		if (i < data->anim->animData->rstData->count - 1) {
			texId = 0x1F;
		}
		effGetTexObj(texId, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		if (i < data->anim->animData->rstData->count - 1) {
			effGetTexObj(0x1F, &texObj);
			GXLoadTexObj(&texObj, GX_TEXMAP0);
			GXSetChanMatColor(GX_COLOR0A0, (GXColor){ 0xFF, 0xFF, 0xFF, (u8)(s32)(float_191p25_8041e400 * data->scaleZ) });
		} else {
			effGetTexObj(0x20, &texObj);
			GXLoadTexObj(&texObj, GX_TEXMAP0);
			GXSetChanMatColor(GX_COLOR0A0, (GXColor){ 0xFF, 0xFF, 0xFF, (u8)(s32)(float_127p5_8041e404 * data->scaleZ) });
		}
		PSMTXConcat(mtxC, data->anim->entries[0].mtx, mtxE);
		PSMTXConcat(mtxE, data->anim->entries[i].mtx, mtxE);
		PSMTXConcat(camGetPtr(cameraId)->view, mtxE, mtxE);
		GXLoadPosMtxImm(mtxE, 0);
		effDrawMayaPoly(&data->anim->animData->rstData->entries[i]);
	}
}

__declspec(section ".sdata2") const char str_8041E418[] = "Ice";
