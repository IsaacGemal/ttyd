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
	f32 field_0x10[6];  // 0x10
	s32 lifetime;       // 0x28
	s32 timer;          // 0x2C
	s32 field_0x30;     // 0x30
	s32 field_0x34;     // 0x34
	s32 field_0x38;     // 0x38
	u32 field_0x3C;     // 0x3C
	f32 field_0x40;     // 0x40
	f32 field_0x44;     // 0x44
	f32 field_0x48;     // 0x48
	f32 field_0x4C;     // 0x4C
	f32 field_0x50;     // 0x50
	f32 field_0x54;     // 0x54
	f32 field_0x58;     // 0x58
	f32 field_0x5C;     // 0x5C
	s32 field_0x60;     // 0x60
	s32 field_0x64;     // 0x64
	s32 field_0x68;     // 0x68
} effPokopiPipoData;

// .sdata2 (pool constants referenced by name so the symbol matches the original)
extern const f32 float_0p71429_8041f9e8;
extern const f32 float_deg2rad_8041f9ec;
extern const f32 float_0p03125_8041f9f0;
extern const f32 float_0_8041f9f4;
extern const f32 float_0p01_8041f9f8;
extern const f32 float_40_8041f9fc;
extern const f32 float_neg0p2_8041fa00;
extern const f32 float_neg1_8041fa04;
extern const f32 float_neg4_8041fa08;
extern const f32 float_14p286_8041fa0c;
extern const f32 float_neg20_8041fa10;
extern const f32 float_neg0p1_8041fa14;
extern const f32 float_1_8041fa18;

// .sdata
u8 w_scale_data[8] = { 0x6E, 0x96, 0x82, 0x6E, 0x64, 0x5F, 0x64, 0x00 };
u8 h_scale_data[8] = { 0x50, 0x3C, 0x50, 0x64, 0x78, 0x6E, 0x64, 0x00 };

// .sbss2 zero GXColor scratch object the original builds its TEV color from.
GXColor unk_80423218;

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
	data->field_0x68 = count;
	data->field_0x3C = 0xff;
	data->field_0x10[0] = x;
	data->field_0x10[1] = y;
	data->field_0x10[2] = z;
	data->field_0x10[3] = x2;
	data->field_0x10[4] = y2;
	data->field_0x10[5] = z2;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x44 = (x2 - x) / (f32)count;
	data->field_0x48 = (y2 - y) / (f32)count - float_neg0p1_8041fa14 * (f32)count;
	data->field_0x4C = (z2 - z) / (f32)count;
	if (y2 - y < float_0_8041f9f4) {
		data->field_0x64 = 1;
	} else {
		data->field_0x64 = 0;
	}
	data->field_0x40 = scale;
	data->field_0x30 = 0x46;
	data->field_0x34 = 0xb4;
	data->field_0x38 = 0x78;
	data->field_0x50 = (f32)(rand() % 360);
	data->field_0x54 = (f32)(rand() % 10 + 5);
	data->field_0x60 = -1;
	data->field_0x58 = float_1_8041fa18;
	data->field_0x5C = float_1_8041fa18;
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
	frame = data->field_0x60;
	timer = data->timer;
	if (frame >= 0 && frame < 7) {
		data->field_0x58 = float_0p01_8041f9f8 * (f32)w_scale_data[frame];
		data->field_0x5C = float_0p01_8041f9f8 * (f32)h_scale_data[frame];
		data->field_0x50 -= float_40_8041f9fc;
		data->field_0x60++;
	} else {
		data->field_0x48 += float_neg0p2_8041fa00;
		data->x += data->field_0x44;
		data->y += data->field_0x48;
		data->z += data->field_0x4C;
		data->field_0x50 += data->field_0x54;
	}
	if (timer - 1 == data->field_0x68) {
		data->field_0x48 = data->field_0x48 * float_neg1_8041fa04;
		data->field_0x54 = float_neg4_8041fa08;
		data->field_0x60 = 0;
	} else if (data->field_0x48 < float_0_8041f9f4 && data->y < float_14p286_8041fa0c) {
#ifdef MUST_MATCH
		/* MUST_MATCH: the original computes f - f rather than loading the 0.0f pool entry. */
		data->field_0x48 = data->field_0x48 - data->field_0x48;
#else
		data->field_0x48 = 0.0f;
#endif
		data->y = float_14p286_8041fa0c;
		data->field_0x54 = float_neg20_8041fa10;
	}
#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effPokopiPipoDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effPokopiPipoDisp, effect);
#endif
}

// The original builds the TEV colour inside an inlined helper; keeping that helper (and
// passing the alpha byte) is what reproduces the stack slot the by-value GXColor copy lands in.
static inline void effPokopiPipoSetColor(effPokopiPipoData* data, s32 a) {
	GXColor color = unk_80423218;
	color.r = data->field_0x30;
	color.g = data->field_0x34;
	color.b = data->field_0x38;
	color.a = a;
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
	s32 a;
	s32 type;
	s32 texId;
	f32 scaleXZ;
	f32 angle;

	camera = camGetPtr(camId);
	data = effect->userdata;
	a = data->field_0x3C;
	type = data->type;
	scaleXZ = float_0p71429_8041f9e8 * data->field_0x40;
	PSMTXTrans(mtx, data->x, data->y, data->z);
	camera3d = camGetPtr(CAMERA_3D);
#ifdef MUST_MATCH
	angle = -camera3d->viewYaw;
	angle = float_deg2rad_8041f9ec * angle;
#else
	angle = -camera3d->viewYaw * float_deg2rad_8041f9ec;
#endif
	PSMTXRotRad(rot, 'y', angle);
	PSMTXScale(scale, scaleXZ * data->field_0x58, scaleXZ * data->field_0x5C, scaleXZ);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXConcat(mtx, scale, mtx);
	PSMTXRotRad(rot, 'z', float_deg2rad_8041f9ec * data->field_0x50);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXConcat(camera->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, 0);
	GXSetCurrentMtx(0);

	effPokopiPipoSetColor(data, a);

	GXSetNumChans(0);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, 0x7d);
	PSMTXScale(scale, float_0p03125_8041f9f0, float_0p03125_8041f9f0, float_0_8041f9f4);
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
