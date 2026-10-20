#include "eff/n64/eff_pokopi_pipo_n64.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdlib.h>

typedef struct effPokopiPipoData {
	s32 type;           // 0x0
	f32 x;              // 0x4
	f32 y;              // 0x8
	f32 z;              // 0xC
	f32 field_0x10[6];  // 0x10 the entry's x,y,z then x2,y2,z2; written at init, never read
	s32 lifetime;       // 0x28
	s32 timer;          // 0x2C
	s32 tevReg0R;       // 0x30 TEVREG0 red
	s32 tevReg0G;       // 0x34 TEVREG0 green
	s32 tevReg0B;       // 0x38 TEVREG0 blue
	u32 tevReg0A;       // 0x3C TEVREG0 alpha (0xFF)
	f32 scale;          // 0x40
	f32 velX;           // 0x44
	f32 velY;           // 0x48
	f32 velZ;           // 0x4C
	f32 rotZ;           // 0x50 degrees, the disp's 'z' rotation
	f32 spinVel;        // 0x54 degrees per frame added to rotZ
	f32 scaleX;         // 0x58 w_scale_data / 100
	f32 scaleY;         // 0x5C h_scale_data / 100
	s32 scaleFrame;     // 0x60 index into the scale tables: -1 in the air, 0-6 while squashing, parked at 7
	s32 field_0x64;     // 0x64 1 when y2 < y, else 0; written at init, never read
	s32 flightFrames;   // 0x68 frames until the bounce (the count argument)
} effPokopiPipoData;

// .sdata2 (pool constants referenced by name so the symbol matches the original)

// .sdata
u8 w_scale_data[8] = { 0x6E, 0x96, 0x82, 0x6E, 0x64, 0x5F, 0x64, 0x00 };
u8 h_scale_data[8] = { 0x50, 0x3C, 0x50, 0x64, 0x78, 0x6E, 0x64, 0x00 };

// .sbss2 zero GXColor scratch object the original builds its TEV color from.

// .data 32x32 quad vertex descriptor; the original passes its address to effSetVtxDescN64.
static u32 size32x32_tex32x32_vtx[0xE] = {
	0xFFF0FFF0, 0x00000000, 0x00000000, 0x00FF0010, 0xFFF00000, 0x04000000, 0x000000FF,
	0x00100010, 0x00000400, 0x04000000, 0x00FFFFF0, 0x00100000, 0x00000400, 0x000000FF
};

EffectEntry* effPokopiPipoN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 x2, f32 y2, f32 z2, f32 scale) {
	EffectEntry* effect;
	effPokopiPipoData* data;

	effect = effEntry();
	effect->type = "PokopiPipoN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effPokopiPipoData));
	effect->userdata = data;
	effect->callback = effPokopiPipoMain;
	effect->flags |= 2;

	data->type = type;
	data->timer = 0;
	if (count <= 0) {
		data->lifetime = 1000;
	} else {
		data->lifetime = count + 60;
	}
	data->flightFrames = count;
	data->tevReg0A = 0xff;
	data->field_0x10[0] = x;
	data->field_0x10[1] = y;
	data->field_0x10[2] = z;
	data->field_0x10[3] = x2;
	data->field_0x10[4] = y2;
	data->field_0x10[5] = z2;
	data->x = x;
	data->y = y;
	data->z = z;
	data->velX = (x2 - x) / (f32)count;
	data->velY = (y2 - y) / (f32)count - -0.1f * (f32)count;
	data->velZ = (z2 - z) / (f32)count;
	if (y2 - y < 0.0f) {
		data->field_0x64 = 1;
	} else {
		data->field_0x64 = 0;
	}
	data->scale = scale;
	data->tevReg0R = 0x46;
	data->tevReg0G = 0xb4;
	data->tevReg0B = 0x78;
	data->rotZ = (f32)(rand() % 360);
	data->spinVel = (f32)(rand() % 10 + 5);
	data->scaleFrame = -1;
	data->scaleX = 1.0f;
	data->scaleY = 1.0f;
	return effect;
}

void effPokopiPipoMain(EffectEntry* effect) {
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch and copies it to a second local before
	   dispCalcZ; a single local folds the copy away. */
	Vec position;
#endif
	Vec disp = { 0.0f, 0.0f, 0.0f };
	effPokopiPipoData* data = effect->userdata;
	s32 frame;
	s32 timer;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
#ifdef MUST_MATCH
	position = disp;
#endif

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 16;
	}
	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	data->timer++;
	if (data->lifetime < 0) {
		effDelete(effect);
		return;
	}
	frame = data->scaleFrame;
	timer = data->timer;
	if (frame >= 0 && frame < 7) {
		data->scaleX = 0.01f * (f32)w_scale_data[frame];
		data->scaleY = 0.01f * (f32)h_scale_data[frame];
		data->rotZ -= 40.f;
		data->scaleFrame++;
	} else {
		data->velY += -0.2f;
		data->x += data->velX;
		data->y += data->velY;
		data->z += data->velZ;
		data->rotZ += data->spinVel;
	}
	if (timer - 1 == data->flightFrames) {
#ifdef MUST_MATCH
		f32 k = -1.0f;
		data->velY = data->velY * k;
#else
		data->velY = data->velY * -1.0f;
#endif
		data->spinVel = -4.0f;
		data->scaleFrame = 0;
	} else if (data->velY < 0.0f && data->y < 14.285715f) {
#ifdef MUST_MATCH
		/* MUST_MATCH: the original computes f - f rather than loading the 0.0f pool entry. */
		data->velY = data->velY - data->velY;
#else
		data->velY = 0.0f;
#endif
		data->y = 14.285715f;
		data->spinVel = -20.f;
	}
#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effPokopiPipoDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effPokopiPipoDisp, effect);
#endif
}

// The original builds the TEV colour inside an inlined helper; keeping that helper (and
// passing the alpha byte) is what reproduces the stack slot the by-value GXColor copy lands in.
static inline void effPokopiPipoSetColor(effPokopiPipoData* data, s32 alpha) {
	GXColor color = (GXColor){ 0, 0, 0, 0 };
	color.r = data->tevReg0R;
	color.g = data->tevReg0G;
	color.b = data->tevReg0B;
	color.a = alpha;
	GXSetTevColor(GX_TEVREG0, color);
}

void effPokopiPipoDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effPokopiPipoData* data;
	CameraEntry* camera;
	CameraEntry* camera3d;
	Mtx mtx;
	Mtx scale;
	Mtx rot;
	GXTexObj tex;
	s32 alpha;
	s32 type;
	s32 texId;
	f32 scaleXZ;
	f32 angle;

	camera = camGetPtr(camId);
	data = effect->userdata;
	alpha = data->tevReg0A;
	type = data->type;
	scaleXZ = 0.71428573f * data->scale;
	PSMTXTrans(mtx, data->x, data->y, data->z);
	camera3d = camGetPtr(CAMERA_3D);
#ifdef MUST_MATCH
	angle = -camera3d->viewYaw;
	angle = 0.017453292f * angle;
#else
	angle = -camera3d->viewYaw * 0.017453292f;
#endif
	PSMTXRotRad(rot, 'y', angle);
	PSMTXScale(scale, scaleXZ * data->scaleX, scaleXZ * data->scaleY, scaleXZ);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXConcat(mtx, scale, mtx);
	PSMTXRotRad(rot, 'z', 0.017453292f * data->rotZ);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXConcat(camera->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);

	effPokopiPipoSetColor(data, alpha);

	GXSetNumChans(0);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	PSMTXScale(scale, 0.03125f, 0.03125f, 0.0f);
	GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
	texId = 0x96;
	if (type == 0) {
		texId = 0x95;
	}
	effGetTexObjN64(texId, &tex);
	GXLoadTexObj(&tex, GX_TEXMAP0);
	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64((effVtxArray*)size32x32_tex32x32_vtx);
	GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
	tri2(0, 1, 2, 0, 0, 2, 3, 0);
}
