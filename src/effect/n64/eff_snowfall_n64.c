#include "eff/n64/eff_snowfall_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/camdrv.h"
#include "driver/mapdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <dolphin/os.h>
#include <stdlib.h>
#include "evt/evt_cmd.h"

// .rodata
extern const char str_SnowfallN64[];
extern const f64 double_to_int_802fb680;

// .data vertex array (32 vertices, 14 bytes each)
extern u32 eff_snowfall_v[];

// .sbss2 4-byte gap after the unit's colour template.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") u32 gap_10_8042326C_sbss2;

// .sdata2
extern const f32 float_0p03125_8041fd14;
extern const f32 float_0_8041fd18;
extern const f32 float_2_8041fd1c;
extern const f32 float_deg2rad_8041fd20;
extern const f32 float_166_8041fd24;
extern const f32 float_1000_8041fd28;
extern const f32 float_0p05_8041fd2c;
extern const f32 float_10_8041fd30;
extern const f32 float_neg1p2_8041fd34;
extern const f32 float_0p01_8041fd38;

typedef struct effSnowfallData {
	s32 active;      // 0x00
	s32 type;        // 0x04
	Vec position;    // 0x08
	Vec velocity;    // 0x14
	s32 life;        // 0x20
	s32 age;         // 0x24
	s32 alpha;       // 0x28
	s32 count;       // 0x2C
	s32 timer;       // 0x30
} effSnowfallData;


// clang-format off

void cam_clip_chk(void* p, s32 a, s32 b, s32 c) {
	effSnowfallData* data = p;
	CameraEntry* cam = camGetPtr(CAMERA_3D);
	f32 cx = cam->cameraAt.x;
	f32 cy = (cam->cameraAt.y + cam->cameraPos.y) / 2.0f;
	f32 cz = (cam->cameraAt.z + cam->viewDistance) / 2.0f;
	f32 px = data->position.x;
	f32 py = data->position.y;
	f32 pz = data->position.z;
	s32 i;

	i = (s32)(px - cx);
	if (i > a) {
		i %= a;
		data->position.x = cx - a + i;
	} else if (i < -a) {
		i = -i;
		i %= a;
		data->position.x = cx + a - i;
	}

	i = (s32)(py - cy);
	if (i > b) {
		i %= b;
		data->position.y = cy - b + i;
	} else if (i < -b) {
		if (c < 100) {
			data->velocity.y = float_0_8041fd18;
			data->alpha = 0;
		} else {
			i = -i;
			i %= b;
			data->position.y = cy + b - i;
		}
	}

	i = (s32)(pz - cz);
	if (i > a) {
		i %= a;
		data->position.z = cz - a + i;
	} else if (i < -a) {
		i = -i;
		i %= a;
		data->position.z = cz + a - i;
	}
}


EffectEntry* effSnowfallN64Entry(s32 type, s32 count) {
	EffectEntry* effect;
	effSnowfallData* data;
	effSnowfallData* particle;
	s32 i;
	s32 num;

	effect = effEntry();
	num = count + 1;
	effect->type = str_SnowfallN64;
	effect->count = num;
	data = __memAlloc(HEAP_EFFECT, num * sizeof(effSnowfallData));
	effect->userdata = data;
	effect->callback = effSnowfallMain;
	data->age = 0;
	data->life = 100;
	data->type = type;
	data->count = count;
	data->alpha = 0xFF;
	data->active = 1;
	particle = data + 1;
	for (i = 0; i < count; i++, particle++) {
		CameraEntry* cam = camGetPtr(CAMERA_3D);

		particle->position.x = cam->cameraPos.x + (f32)(rand() % 2000) - float_1000_8041fd28;
		particle->position.y = cam->cameraPos.y + (f32)(rand() % 2000) - float_1000_8041fd28;
		particle->position.z = cam->cameraPos.z + (f32)(rand() % 2000) - float_1000_8041fd28;
		particle->velocity.x = ((f32)(rand() % 20) - float_10_8041fd30) * float_0p05_8041fd2c;
		particle->velocity.y = float_neg1p2_8041fd34 - (f32)(rand() % 80) * float_0p01_8041fd38;
		particle->velocity.z = ((f32)(rand() % 20) - float_10_8041fd30) * float_0p05_8041fd2c;
		particle->alpha = 0xFF;
		particle->velocity.x *= 0.5f;
		particle->velocity.y *= 0.5f;
		particle->velocity.z *= 0.5f;
		particle->timer = 0;
	}
	return effect;
}

// `data++` rather than a byte-cast add is what makes mwcc colour data r31 and the effect parameter r30 as the
// original does.
void effSnowfallMain(EffectEntry* effect) {
	effSnowfallData* data = effect->userdata;
	Vec position2;
	Vec position;
	s32 i;
	s32 life;
	s32 count;
	s32 limit;

	position = (Vec){ data->position.x, data->position.y, data->position.z };
	position2 = position;
	camGetPtr(CAMERA_3D);
	if (data->life < 100) {
		data->life--;
	}
	if (data->life < 0) {
		effDelete(effect);
		return;
	}
	data->age++;
	life = data->life;
	count = data->count;
	if (life < 10) {
		data->alpha = life * 25;
	}
	limit = (s32)float_166_8041fd24;
	data++;
	for (i = 0; i < count; i++, data++) {
		if (data->timer > 0) {
			data->timer--;
			if (data->timer > 0) {
				continue;
			}
		}
		data->position.x += data->velocity.x;
		data->position.y += data->velocity.y;
		data->position.z += data->velocity.z;
		cam_clip_chk(data, 0x226, limit, life);
	}
	dispEntry(CAMERA_3D, dispCalcZ(&position2), 10, effSnowfallDisp, effect);
}


void effSnowfallDisp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	effSnowfallData* data;
	CameraEntry* cam;
	Mtx transMtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	GXTexObj texObj;
	GXTexObj texObj2;
	s32 count;
	u8* dst;
	s32 i;
	s32 alpha;
	u8* src;
	s32 j;
	s32 k;
	GXColor blend;

	cam = camGetPtr(cameraId);
	data = effect->userdata;
	if (data->active == 0) {
		return;
	}
	count = data->count;
	alpha = data->alpha;
	GXSetNumChans(0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	PSMTXScale(scaleMtx, float_0p03125_8041fd14, float_0p03125_8041fd14, float_0_8041fd18);
	GXLoadTexMtxImm(scaleMtx, GX_TEXMTX0, GX_MTX2x4);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ONE);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);
	effGetTexObjN64(0x63, &texObj);
	effGetTexObjN64(0x64, &texObj2);
	dst = OSPhysicalToCached(GXGetTexObjData(&texObj));
	src = OSPhysicalToCached(GXGetTexObjData(&texObj2));
	dst += GXGetTexBufferSize(0x40, 0x40, GX_TF_I4, GX_TRUE, 2);
	memcpy(dst, src, GXGetTexBufferSize(0x10, 0x10, GX_TF_I4, GX_FALSE, 0));
	DCFlushRange(dst, GXGetTexBufferSize(0x40, 0x40, GX_TF_I4, GX_TRUE, 3));
	GXInitTexObjLOD(&texObj, GX_LIN_MIP_LIN, GX_LINEAR, float_0_8041fd18, float_2_8041fd1c, float_0_8041fd18, GX_TRUE, GX_TRUE, GX_ANISO_1);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	if (mapGetWork()->entries[0].flags & 2) {
		mapGetBlend(&blend);
		GXSetTevColor(GX_TEVREG0, (GXColor){ blend.r * 0xFF, blend.g * 0xFF, blend.b * 0xFF, alpha * blend.a });
	} else {
		GXSetTevColor(GX_TEVREG0, (GXColor){ 0xFF, 0xFF, 0xFF, alpha });
	}
	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64(eff_snowfall_v);
	PSMTXRotRad(rotMtx, 'y', float_deg2rad_8041fd20 * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(cam->view, rotMtx, rotMtx);
	data++;
	for (i = 0; i < count; i++, data++) {
		if (data->timer > 0) {
			continue;
		}
		if (data->alpha == 0) {
			continue;
		}
		PSMTXTrans(transMtx, data->position.x, data->position.y, data->position.z);
		PSMTXConcat(rotMtx, transMtx, transMtx);
		GXLoadPosMtxImm(transMtx, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		GXBegin(GX_QUADS, GX_VTXFMT0, 0x20);
		for (j = 0; j < 4; j++) {
			for (k = 0; k < 8; k++) {
				GXPosition1x16(j * 8 + k);
				GXColor1x16(j * 8 + k);
				GXTexCoord1x16(j * 8 + k);
			}
		}
	}
}

// TODO: type
u32 eff_snowfall_v[112] = {
    0xFFB20003, 0xFFDA0000, 0x0800FFFF, 0xFFFFFFBC,
    0x0003FFDA, 0x08000800, 0xFFFFFFFF, 0xFFBC000C,
    0xFFDA0800, 0x0000FFFF, 0xFFFFFFB2, 0x000CFFDA,
    0x00000000, 0xFFFFFFFF, 0xFFDFFFDF, 0xFFC20000,
    0x0800FFFF, 0xFFFFFFE9, 0xFFDFFFC2, 0x08000800,
    0xFFFFFFFF, 0xFFE9FFE8, 0xFFC20800, 0x0000FFFF,
    0xFFFFFFDF, 0xFFE8FFC2, 0x00000000, 0xFFFFFFFF,
    0xFFE20027, 0xFFF20000, 0x0800FFFF, 0xFFFFFFEC,
    0x0027FFF2, 0x08000800, 0xFFFFFFFF, 0xFFEC0030,
    0xFFF20800, 0x0000FFFF, 0xFFFFFFE2, 0x0030FFF2,
    0x00000000, 0xFFFFFFFF, 0xFFF3FFBE, 0xFFDA0000,
    0x0800FFFF, 0xFFFFFFFD, 0xFFBEFFDA, 0x08000800,
    0xFFFFFFFF, 0xFFFDFFC7, 0xFFDA0800, 0x0000FFFF,
    0xFFFFFFF3, 0xFFC7FFDA, 0x00000000, 0xFFFFFFFF,
    0xFFFBFFFC, 0x00000000, 0x0800FFFF, 0xFFFF0005,
    0xFFFC0000, 0x08000800, 0xFFFFFFFF, 0x00050005,
    0x00000800, 0x0000FFFF, 0xFFFFFFFB, 0x00050000,
    0x00000000, 0xFFFFFFFF, 0x004CFFF3, 0xFFD20000,
    0x0800FFFF, 0xFFFF0056, 0xFFF3FFD2, 0x08000800,
    0xFFFFFFFF, 0x0056FFFC, 0xFFD20800, 0x0000FFFF,
    0xFFFF004C, 0xFFFCFFD2, 0x00000000, 0xFFFFFFFF,
    0x002D0025, 0xFFF30000, 0x0800FFFF, 0xFFFF0037,
    0x0025FFF3, 0x08000800, 0xFFFFFFFF, 0x0037002E,
    0xFFF30800, 0x0000FFFF, 0xFFFF002D, 0x002EFFF3,
    0x00000000, 0xFFFFFFFF, 0x001BFFCB, 0xFFC70000,
    0x0800FFFF, 0xFFFF0025, 0xFFCBFFC7, 0x08000800,
    0xFFFFFFFF, 0x0025FFD4, 0xFFC70800, 0x0000FFFF,
    0xFFFF001B, 0xFFD4FFC7, 0x00000000, 0xFFFFFFFF,
};
// clang-format on

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_0p03125_8041fd14 = 0.03125f;
const f32 float_0_8041fd18 = 0.0f;
const f32 float_2_8041fd1c = 2.0f;
const f32 float_deg2rad_8041fd20 = 0.017453292f;
const f32 float_166_8041fd24 = 166.0f;
const f32 float_1000_8041fd28 = 1000.0f;
const f32 float_0p05_8041fd2c = 0.05f;
const f32 float_10_8041fd30 = 10.0f;
const f32 float_neg1p2_8041fd34 = -1.2f;
const f32 float_0p01_8041fd38 = 0.01f;

const char str_SnowfallN64[] = "SnowfallN64";
