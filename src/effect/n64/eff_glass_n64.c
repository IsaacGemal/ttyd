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
		data->color1R = 0xff;
		data->color1G = 0xff;
		data->color1B = 0xff;
		data->color2R = 0xff;
		data->color2G = 0xff;
		data->color2B = 0xff;
		data->field_0x34 = 0xff;
		break;
	case 2:
	case 3:
		data->color1R = 0;
		data->color1G = 0xff;
		data->color1B = 0;
		data->color2R = 0;
		data->color2G = 0xff;
		data->color2B = 0;
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
	Vec position;
	Vec disp = { 0 };

	disp.x = data->position.x;
	disp.y = data->position.y;
	disp.z = data->position.z;
	position = disp;

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

	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effGlassDisp, effect);
}

/* effGlassDisp matches with the colour writes and the frame conversion in inlines. */
/* Setting each colour inside a void inline (rather than assigning a local then passing it)
   reproduces the original's per-call by-value colour slots, local 0x18/copy 0x1c, 0x10/0x14, 0x08/0x0c
   (same shape as effHitDisp). */
static inline void effGlassSetColor1(effGlassData* data, s32 a) {
	GXColor color = unk_80423120;
	color.r = data->color1R;
	color.g = data->color1G;
	color.b = data->color1B;
	color.a = a;
	GXSetTevColor(GX_TEVREG0, color);
}
static inline void effGlassSetColor2(effGlassData* data, s32 a) {
	GXColor color = unk_80423124;
	color.r = data->color2R;
	color.g = data->color2G;
	color.b = data->color2B;
	color.a = a;
	GXSetTevColor(GX_TEVREG1, color);
}
static inline void effGlassSetColor3(effGlassData* data, s32 a) {
	GXColor color = unk_80423128;
	color.r = data->color1R;
	color.g = data->color1G;
	color.b = data->color1B;
	color.a = a;
	GXSetTevColor(GX_TEVREG0, color);
}
/* Calling the cast through an inline makes mwcc allocate the frame conversion's scratch slot
   first (0x100) instead of last, matching the original. */
static inline s32 effGlassFrame(f32 x) {
	return (s32)x;
}
void effGlassDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	s32 type;
	effGlassData* data;
	CameraEntry* cam;
	GXTexObj texObj;
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
	GXLoadPosMtxImm(trans, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);

	GXSetNumChans(0);
	GXSetNumTevStages(3);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_TEXC, GX_CC_A1, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_APREV, GX_CA_TEXA, GX_CA_A1, GX_CA_ZERO);
	GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_C1, GX_CC_C0, GX_CC_CPREV, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_A0, GX_CA_APREV, GX_CA_ZERO);
	effGetTexObjN64(0x59, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXLoadTexObj(&texObj, GX_TEXMAP1);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);

	type = type % 2;
	frame = effGlassFrame(4.0f * data->angle);
	tileAlpha = (s32)(256.0f * (4.0f * data->angle - (f32)((s32)data->angle * 4)));

	typeOffset = type * 0x20;
	frameOffset = frame * 0x20;
	PSMTXScale(scale, 0.015625f, 0.0078125f, 0.0f);
	PSMTXTrans(trans, (f32)typeOffset, (f32)frameOffset, 0.0f);
	PSMTXConcat(scale, trans, scale);
	GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
	PSMTXScale(scale, 0.015625f, 0.0078125f, 0.0f);
	PSMTXTrans(trans, (f32)typeOffset, (f32)(frameOffset + 0x20), 0.0f);
	PSMTXConcat(scale, trans, scale);
	GXLoadTexMtxImm(scale, GX_TEXMTX1, GX_MTX2x4);

	effGlassSetColor1(data, dataAlpha);
	effGlassSetColor2(data, tileAlpha);
	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64((effVtxArray*)size16x16_tex32x32_vtx);
	GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
	tri2(0, 1, 2, 0, 0, 2, 3, 0);

	if (type == 1 && (s32)(4.0f * data->angle) == 3) {
		effGlassSetColor3(data, dataAlpha * tileAlpha / 255);
		GXSetNumChans(1);
		GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
		GXSetNumTevStages(1);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_RASA, GX_CA_ZERO);
		effSetVtxDescN64((effVtxArray*)size48x48_tex32x32_vtx);
		GXLoadPosMtxImm(viewMtx, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0xc);
		tri2(0, 1, 4, 0, 1, 2, 4, 0);
		tri2(0, 4, 3, 0, 4, 2, 3, 0);
	}
}
