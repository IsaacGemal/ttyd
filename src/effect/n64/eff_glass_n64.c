#include "eff/n64/eff_glass_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <stdlib.h>
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sbss2 zero GXColor scratch objects (declared in reverse: they are placed in reverse of declaration order)
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_8042312c;
__declspec(section ".sdata") GXColor unk_80423128;
__declspec(section ".sdata") GXColor unk_80423124;
__declspec(section ".sdata") GXColor unk_80423120;

// .data vertex arrays, local to this file.
static u32 size16x16_tex32x32_vtx[0x38 / 4] = {
	0xFFF8FFF8, 0x00000000, 0x00000000, 0x00FF0008, 0xFFF80000, 0x04000000, 0x000000FF,
	0x00080008, 0x00000400, 0x04000000, 0x00FFFFF8, 0x00080000, 0x00000400, 0x000000FF,
};
static u32 size48x48_tex32x32_vtx[0x48 / 4] = {
	0xFFE00000, 0x00000000, 0x0000FFFF, 0xFF000000, 0xFFE00000, 0x04000000, 0xFFFFFF00,
	0x00200000, 0x00000400, 0x0400FFFF, 0xFF000000, 0x00200000, 0x00000400, 0xFFFFFF00,
	0x00000000, 0x00000000, 0x0000FFFF, 0xFFC80000,
};

EffectEntry* effGlassN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry;
	effGlassData* data;

	entry = effEntry();
	entry->type = "GlassN64";
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x5C);
	entry->userdata = data;
	entry->callback = effGlassMain;
	entry->flags |= 2;
	data->type = type;
	data->counter = 0;
	if (life <= 0) {
		data->life = 1000;
	} else {
		data->life = life;
	}
	data->alpha = 0xff;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->scale = scale;

	switch (type) {
	case 0:
	case 1:
		data->field_0x18 = 0xff;
		data->field_0x1C = 0xff;
		data->field_0x20 = 0xff;
		data->field_0x28 = 0xff;
		data->field_0x2C = 0xff;
		data->field_0x30 = 0xff;
		data->field_0x34 = 0xff;
		break;
	case 2:
	case 3:
		data->field_0x18 = 0;
		data->field_0x1C = 0xff;
		data->field_0x20 = 0;
		data->field_0x28 = 0;
		data->field_0x2C = 0xff;
		data->field_0x30 = 0;
		data->field_0x34 = 0xff;
		break;
	}

	data->angle = 0.0f;
	data->angleVel = (0.1f * (f32)(rand() % 5) + 0.1f) * (0.25f * (f32)(rand() % 2 * 2 - 1));
	data->rot = (f32)(rand() % 360);
	data->velX = (f32)(rand() % 11 - 5);
	data->velY = (f32)(rand() % 11 - 5);
	data->velZ = (f32)(rand() % 11 - 5);
	data->rotVel = (f32)(rand() % 41 - 20);
	data->gravity = -0.1f;
	return entry;
}

void effGlassMain(EffectEntry* effect) {
	effGlassData* data = effect->userdata;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second
	   local before dispCalcZ; a single local folds the copy away (same idiom as
	   effFlameMain). */
	Vec position;
#endif
	Vec disp = { 0 };

	disp.x = data->position.x;
	disp.y = data->position.y;
	disp.z = data->position.z;
#ifdef MUST_MATCH
	position = disp;
#endif

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->life = 16;
	}

	if (data->life < 1000) {
		data->life--;
	}
	data->counter++;

	if (data->life < 0) {
		effDelete(effect);
		return;
	}

	if (data->life < 16) {
		data->alpha = data->life * 16;
	}

	data->angle += data->angleVel;
	if (data->angle < 0.0f) {
		data->angle += 1.0f;
	}
	if (data->angle > 1.0f) {
		data->angle = data->angle - 1.0f;
	}

	data->velY += data->gravity;
	data->position.x += data->velX;
	data->position.y += data->velY;
	data->position.z += data->velZ;
	data->rot += data->rotVel;

#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effGlassDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effGlassDisp, effect);
#endif
}

/* effGlassDisp matches with the colour writes and the frame conversion in inlines (see the MUST_MATCH
   shapes below). The natural form is under each #else. */
#ifdef MUST_MATCH
/* MUST_MATCH: setting each colour inside a void inline (rather than assigning a local then passing it)
   reproduces the original's per-call by-value colour slots, local 0x18/copy 0x1c, 0x10/0x14, 0x08/0x0c
   (same shape as effHitDisp). */
static inline void effGlassSetColor1(effGlassData* data, s32 a) {
	GXColor color = unk_80423120;
	color.r = data->field_0x18;
	color.g = data->field_0x1C;
	color.b = data->field_0x20;
	color.a = a;
	GXSetTevColor(1, color);
}
static inline void effGlassSetColor2(effGlassData* data, s32 a) {
	GXColor color = unk_80423124;
	color.r = data->field_0x28;
	color.g = data->field_0x2C;
	color.b = data->field_0x30;
	color.a = a;
	GXSetTevColor(2, color);
}
static inline void effGlassSetColor3(effGlassData* data, s32 a) {
	GXColor color = unk_80423128;
	color.r = data->field_0x18;
	color.g = data->field_0x1C;
	color.b = data->field_0x20;
	color.a = a;
	GXSetTevColor(1, color);
}
/* MUST_MATCH: calling the cast through an inline makes mwcc allocate the frame conversion's scratch slot
   first (0x100) instead of last, matching the original. */
static inline s32 effGlassFrame(f32 x) {
	return (s32)x;
}
#endif
void effGlassDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	s32 type;
	effGlassData* data;
	CameraEntry* cam;
	GXTexObj texObj;
	GXColor color1;
	GXColor color2;
	GXColor color3;
	Mtx trans;
	Mtx scale;
	Mtx rot;
	Mtx viewMtx;
	s32 frame;
	s32 typeOffset;
	s32 frameOffset;
	s32 dataAlpha;
	s32 tileAlpha;

	cam = camGetPtr(camId);
	data = effect->userdata;
	dataAlpha = data->alpha;
	type = data->type;

	PSMTXTrans(trans, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXScale(scale, data->scale, data->scale, data->scale);
	PSMTXConcat(trans, rot, trans);
	PSMTXConcat(trans, scale, trans);
	PSMTXConcat(cam->view, trans, viewMtx);
	PSMTXRotRad(rot, 'z', 0.017453292f * data->rot);
	PSMTXConcat(viewMtx, rot, trans);
	GXLoadPosMtxImm(trans, 0);
	GXSetCurrentMtx(0);

	GXSetNumChans(0);
	GXSetNumTevStages(3);
	GXSetTevOrder(0, 0, 0, 0xff);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xf, 0xf, 0xf, 8);
	GXSetTevAlphaIn(0, 7, 7, 7, 4);
	GXSetTevOrder(1, 1, 1, 0xff);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 0, 8, 5, 0xf);
	GXSetTevAlphaIn(1, 0, 4, 2, 7);
	GXSetTevOrder(2, 0xff, 0xff, 0xff);
	GXSetTevColorOp(2, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
	GXSetTevColorIn(2, 4, 2, 0, 0xf);
	GXSetTevAlphaIn(2, 7, 1, 0, 7);
	effGetTexObjN64(0x59, &texObj);
	GXLoadTexObj(&texObj, 0);
	GXLoadTexObj(&texObj, 1);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	GXSetTexCoordGen2(1, 1, 4, 0x21, 0, 0x7d);

	type = type % 2;
#ifdef MUST_MATCH
	frame = effGlassFrame(4.0f * data->angle);
#else
	frame = (s32)(4.0f * data->angle);
#endif
	tileAlpha = (s32)(256.0f * (4.0f * data->angle - (f32)((s32)data->angle * 4)));

	typeOffset = type * 0x20;
	frameOffset = frame * 0x20;
	PSMTXScale(scale, 0.015625f, 0.0078125f, 0.0f);
	PSMTXTrans(trans, (f32)typeOffset, (f32)frameOffset, 0.0f);
	PSMTXConcat(scale, trans, scale);
	GXLoadTexMtxImm(scale, 0x1e, 1);
	PSMTXScale(scale, 0.015625f, 0.0078125f, 0.0f);
	PSMTXTrans(trans, (f32)typeOffset, (f32)(frameOffset + 0x20), 0.0f);
	PSMTXConcat(scale, trans, scale);
	GXLoadTexMtxImm(scale, 0x21, 1);

#ifdef MUST_MATCH
	effGlassSetColor1(data, dataAlpha);
	effGlassSetColor2(data, tileAlpha);
#else
	color1 = unk_80423120;
	color1.r = data->field_0x18;
	color1.g = data->field_0x1C;
	color1.b = data->field_0x20;
	color1.a = dataAlpha;
	GXSetTevColor(1, color1);
	color2 = unk_80423124;
	color2.r = data->field_0x28;
	color2.g = data->field_0x2C;
	color2.b = data->field_0x30;
	color2.a = tileAlpha;
	GXSetTevColor(2, color2);
#endif
	GXSetCullMode(0);
	effSetVtxDescN64((effVtxArray*)size16x16_tex32x32_vtx);
	GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
	tri2(0, 1, 2, 0, 0, 2, 3, 0);

	if (type == 1 && (s32)(4.0f * data->angle) == 3) {
#ifdef MUST_MATCH
		effGlassSetColor3(data, dataAlpha * tileAlpha / 255);
#else
		color3 = unk_80423128;
		color3.r = data->field_0x18;
		color3.g = data->field_0x1C;
		color3.b = data->field_0x20;
		color3.a = dataAlpha * tileAlpha / 255;
		GXSetTevColor(1, color3);
#endif
		GXSetNumChans(1);
		GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
		GXSetNumTevStages(1);
		GXSetTevOrder(0, 0xff, 0xff, 4);
		GXSetTevColorOp(0, 0, 0, 0, 1, 0);
		GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
		GXSetTevColorIn(0, 0xf, 0xf, 0xf, 2);
		GXSetTevAlphaIn(0, 7, 1, 5, 7);
		effSetVtxDescN64((effVtxArray*)size48x48_tex32x32_vtx);
		GXLoadPosMtxImm(viewMtx, 0);
		GXSetCurrentMtx(0);
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0xc);
		tri2(0, 1, 4, 0, 1, 2, 4, 0);
		tri2(0, 4, 3, 0, 4, 2, 3, 0);
	}
}
