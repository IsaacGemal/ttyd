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

// .rodata
extern const char str_802FB674[];
extern const f64 double_to_int_802fb680;

// .data vertex array (32 vertices, 14 bytes each)
extern effVtxArray eff_snowfall_v[];

// .sbss2 scratch GXColor the blend path builds its TEV colour from
extern GXColor unk_80423268;

// .sdata2
extern const f32 float_0p03125_8041fd14;
extern const f32 float_0_8041fd18;
extern const f32 float_2_8041fd1c;
extern const f32 float_deg2rad_8041fd20;
extern const GXColor dat_8041fd10;
extern const f32 float_166_8041fd24;
extern const f32 float_1000_8041fd28;
extern const f32 float_0p05_8041fd2c;
extern const f32 float_10_8041fd30;
extern const f32 float_neg1p2_8041fd34;
extern const f32 float_0p01_8041fd38;
extern const f32 float_0p5_8041fd3c;

typedef struct effSnowfallData {
	s32 field_0x00;  // 0x00
	s32 type;        // 0x04
	Vec position;    // 0x08
	Vec velocity;    // 0x14
	s32 life;        // 0x20
	s32 field_0x24;  // 0x24
	s32 field_0x28;  // 0x28
	s32 count;       // 0x2C
	s32 timer;       // 0x30
} effSnowfallData;

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
			data->velocity.y = 0.0f;
			data->field_0x28 = 0;
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
	effect->type = str_802FB674;
	effect->count = num;
	data = __memAlloc(HEAP_EFFECT, num * sizeof(effSnowfallData));
	effect->userdata = data;
	effect->callback = effSnowfallMain;
	data->field_0x24 = 0;
	data->life = 100;
	data->type = type;
	data->count = count;
	data->field_0x28 = 0xFF;
	data->field_0x00 = 1;
	particle = data + 1;
	for (i = 0; i < count; i++, particle++) {
		CameraEntry* cam = camGetPtr(CAMERA_3D);

		particle->position.x = cam->cameraPos.x + (f32)(rand() % 2000) - float_1000_8041fd28;
		particle->position.y = cam->cameraPos.y + (f32)(rand() % 2000) - float_1000_8041fd28;
		particle->position.z = cam->cameraPos.z + (f32)(rand() % 2000) - float_1000_8041fd28;
		particle->velocity.x = ((f32)(rand() % 20) - float_10_8041fd30) * float_0p05_8041fd2c;
		particle->velocity.y = float_neg1p2_8041fd34 - (f32)(rand() % 80) * float_0p01_8041fd38;
		particle->velocity.z = ((f32)(rand() % 20) - float_10_8041fd30) * float_0p05_8041fd2c;
		particle->field_0x28 = 0xFF;
		particle->velocity.x *= float_0p5_8041fd3c;
		particle->velocity.y *= float_0p5_8041fd3c;
		particle->velocity.z *= float_0p5_8041fd3c;
		particle->timer = 0;
	}
	return effect;
}

// The position Vec is built by this makeVec inline (the plain `Vec position = vec3_802fb668;` plus three
// field stores schedules the loads worse).  `data++` rather than a byte-cast add is what makes mwcc colour
// data r31 and the effect parameter r30 as the original does; together they give a byte-exact match.
static inline Vec effSnowfallMakeVec(f32 x, f32 y, f32 z) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };

	temp.x = x;
	temp.y = y;
	temp.z = z;
	return temp;
}

void effSnowfallMain(EffectEntry* effect) {
	effSnowfallData* data = effect->userdata;
	Vec position2;
	Vec position;
	s32 i;
	s32 life;
	s32 count;
	s32 limit;

	position = effSnowfallMakeVec(data->position.x, data->position.y, data->position.z);
	position2 = position;
	camGetPtr(CAMERA_3D);
	if (data->life < 100) {
		data->life--;
	}
	if (data->life < 0) {
		effDelete(effect);
		return;
	}
	data->field_0x24++;
	life = data->life;
	count = data->count;
	if (life < 10) {
		data->field_0x28 = life * 25;
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

// MUST_MATCH: the original builds GXSetTevColor's by-value GXColor argument in a scratch slot and word-copies
// it up into the outgoing argument slot.  Building it in a plain local makes mwcc put the build in the higher of
// the two slots (the known GXColor quirk); when the build and the call both live in an inline, mwcc keeps the
// scratch below the outgoing copy, as the original does.  Natural form under #else.
#ifdef MUST_MATCH
static inline void effSnowfallSetBlendColor(GXColor* blend, s32 alpha) {
	GXColor c = unk_80423268;
	c.r = blend->r * 0xFF;
	c.g = blend->g * 0xFF;
	c.b = blend->b * 0xFF;
	c.a = alpha * blend->a;
	GXSetTevColor(1, c);
}
static inline void effSnowfallSetPlainColor(s32 alpha) {
	GXColor c = dat_8041fd10;
	c.a = alpha;
	GXSetTevColor(1, c);
}
#endif

// (1) The original builds the blend/flat GXColor in a scratch slot and copies it to the outgoing argument slot
//     (local below the copy); a plain local lands the other way round.  The effSnowfallSet*Color inlines fix it.
// (2) The rest of the function matches the original instruction for instruction.
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
	GXColor color;

	cam = camGetPtr(cameraId);
	data = effect->userdata;
	if (data->field_0x00 == 0) {
		return;
	}
	count = data->count;
	alpha = data->field_0x28;
	GXSetNumChans(0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	PSMTXScale(scaleMtx, float_0p03125_8041fd14, float_0p03125_8041fd14, float_0_8041fd18);
	GXLoadTexMtxImm(scaleMtx, 0x1e, 1);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 0xff);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xf, 0xf, 0xf, 0xc);
	GXSetTevAlphaIn(0, 7, 4, 1, 7);
	effGetTexObjN64(0x63, &texObj);
	effGetTexObjN64(0x64, &texObj2);
	dst = (u8*)OSPhysicalToCached(GXGetTexObjData(&texObj));
	src = (u8*)OSPhysicalToCached(GXGetTexObjData(&texObj2));
	dst += GXGetTexBufferSize(0x40, 0x40, 0, 1, 2);
	memcpy(dst, src, GXGetTexBufferSize(0x10, 0x10, 0, 0, 0));
	DCFlushRange(dst, GXGetTexBufferSize(0x40, 0x40, 0, 1, 3));
	GXInitTexObjLOD(&texObj, 5, 1, float_0_8041fd18, float_2_8041fd1c, float_0_8041fd18, 1, 1, 0);
	GXLoadTexObj(&texObj, 0);
	if (mapGetWork()->entries[0].flags & 2) {
		mapGetBlend(&blend);
#ifdef MUST_MATCH
		effSnowfallSetBlendColor(&blend, alpha);
#else
		color = unk_80423268;
		color.r = blend.r * 0xFF;
		color.g = blend.g * 0xFF;
		color.b = blend.b * 0xFF;
		color.a = alpha * blend.a;
		GXSetTevColor(1, color);
#endif
	} else {
#ifdef MUST_MATCH
		effSnowfallSetPlainColor(alpha);
#else
		color = dat_8041fd10;
		color.a = alpha;
		GXSetTevColor(1, color);
#endif
	}
	GXSetCullMode(0);
	effSetVtxDescN64(eff_snowfall_v);
	PSMTXRotRad(rotMtx, 'y', float_deg2rad_8041fd20 * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(cam->view, rotMtx, rotMtx);
	data++;
	for (i = 0; i < count; i++, data++) {
		if (data->timer > 0) {
			continue;
		}
		if (data->field_0x28 == 0) {
			continue;
		}
		PSMTXTrans(transMtx, data->position.x, data->position.y, data->position.z);
		PSMTXConcat(rotMtx, transMtx, transMtx);
		GXLoadPosMtxImm(transMtx, 0);
		GXSetCurrentMtx(0);
		GXBegin(0x80, 0, 0x20);
		for (j = 0; j < 4; j++) {
			for (k = 0; k < 8; k++) {
				GXPosition1x16(j * 8 + k);
				GXColor1x16(j * 8 + k);
				GXTexCoord1x16(j * 8 + k);
			}
		}
	}
}
