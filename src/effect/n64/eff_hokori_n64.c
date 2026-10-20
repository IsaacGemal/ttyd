#include "eff/n64/eff_hokori_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <stdlib.h>
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

typedef struct HokoriParticle {
	s32 type;         // 0x00
	f32 x;            // 0x04
	f32 y;            // 0x08
	f32 z;            // 0x0C
	f32 velY;         // 0x10
	f32 accelY;       // 0x14
	s32 alpha;        // 0x18
	f32 scaleY;       // 0x1C
	f32 scaleXZ;      // 0x20
	f32 scaleYStep;   // 0x24
	f32 scaleXZStep;  // 0x28
	s32 life;         // 0x2C
	s32 color1R;      // 0x30
	s32 color1G;      // 0x34
	s32 color1B;      // 0x38
	s32 color2R;      // 0x3C
	s32 color2G;      // 0x40
	s32 color2B;      // 0x44
} HokoriParticle;

// .sdata2 / .rodata constants used only by effHokoriN64Entry, named after their
// original addresses. The string lives in this unit's own .rodata.

// .sbss2 colour bases the retail unit leaves uninitialised; the fields are
// overwritten one by one before each GXSetTevColor.

// Float pairs that precede the vertex arrays in the retail unit's .data; kept so
// the unit's data section is complete. The arrays themselves are reached through
// lbl_8039D5D0 (declared in the header) in effHokoriDisp under MUST_MATCH.
// The unit's own .data is reached only through lbl_8039D5D0 (the previous unit's last object), so keep it linked.
#pragma force_active on
__declspec(section ".data") u32 lbl_8039D5D0[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") static f32 negone_one_374[2] = { -1.0f, 1.0f };
__declspec(section ".data") f32 lbl_8039D5E0[2] = { 1.0f, -1.0f };
__declspec(section ".data") static f32 one_negone_373[2] = { 1.0f, -1.0f };
__declspec(section ".data") f32 lbl_8039D5F0[2] = { -1.0f, 1.0f };
__declspec(section ".data") static f32 negone_one_360[2] = { -1.0f, 1.0f };
__declspec(section ".data") f32 lbl_8039D600[2] = { 1.0f, -1.0f };
__declspec(section ".data") static f32 one_negone_359[2] = { 1.0f, -1.0f };

// .data vertex arrays.
u32 size16x32_tex32x64_vtx[14] = {
	0xFFF8FFE0, 0x00000000, 0x04000000, 0x00FF0008,
	0xFFE00000, 0x04000400, 0x000000FF, 0x00080000,
	0x00000400, 0x0BE00000, 0x00FFFFF8, 0x00000000,
	0x00000BE0, 0x000000FF,
};
static u32 size16x16_tex32x32_vtx[14] = {
	0xFFF8FFF0, 0x00000000, 0x00000000, 0x00FF0008,
	0xFFF00000, 0x04000000, 0x000000FF, 0x00080000,
	0x00000400, 0x04000000, 0x00FFFFF8, 0x00000000,
	0x00000400, 0x000000FF,
};
static u32 size8x8_tex32x32_vtx[14] = {
	0xFFFCFFFC, 0x00000000, 0x00000000, 0x00FF0004,
	0xFFFC0000, 0x04000000, 0x000000FF, 0x00040004,
	0x00000400, 0x04000000, 0x00FFFFFC, 0x00040000,
	0x00000400, 0x000000FF,
};
#pragma force_active reset

EffectEntry* effHokoriN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z) {
	EffectEntry* effect;
	HokoriParticle* data;
	HokoriParticle* p;
	s32 num;
	s32 i;

	effect = effEntry();
	num = (type <= 1) ? 5 : 4;
	effect->type = "HokoriN64";
	effect->count = num;
	data = __memAlloc(HEAP_EFFECT, num * 0x48);
	effect->userdata = data;
	effect->callback = effHokoriMain;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->life = count;
	data->alpha = 0xff;
	data->color1R = 0xa0;
	data->color1G = 0xa0;
	data->color1B = 0x8c;
	data->color2R = 0xa0;
	data->color2G = 0xa0;
	data->color2B = 0x8c;

	for (i = 1, p = data + 1; i < num; i++, p++) {
		p->x = (f32)(rand() % 200 - 100);
		p->y = (f32)(rand() % 50);
		p->z = 0.0f;
		p->velY = -1.0f;
		p->accelY = 0.0f;
		p->scaleY = 1.0f;
		p->scaleXZ = 1.0f;
		p->scaleYStep = (f32)(rand() % 10) / 200.f;
		switch (type) {
		case 0:
			p->scaleXZStep = 0.1f * p->scaleYStep;
			break;
		case 1:
			p->scaleXZStep = 0.2f * p->scaleYStep;
			break;
		default:
			p->scaleYStep = 0.4f;
			p->scaleXZStep = 0.4f;
			p->velY = 0.0f;
			p->x = 0.1f * (f32)(rand() % 40 - 20);
			p->y = 0.1f * (f32)(rand() % 40 - 20);
			break;
		}
	}
	return effect;
}

inline Vec makeVec(f32 x, f32 y, f32 z) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = x;
	temp.y = y;
	temp.z = z;
	return temp;
}

void effHokoriMain(EffectEntry* effect) {
	HokoriParticle* data;
	Vec position2;
	Vec position;
	s32 type;
	s32 i;

	data = effect->userdata;
	position = makeVec(data->x, data->y, data->z);
	position2 = position;
	data->life--;
	if (data->life < 0) {
		effDelete(effect);
	}
	else {
		type = data->type;
		if (data->life < 0x18) {
#ifdef MUST_MATCH
			f32 k = 0.9f;
			data->alpha = (s32)((f32)data->alpha * k);
#else
			data->alpha = (s32)((f32)data->alpha * 0.9f);
#endif
		}
		i = 1;
		data++;
		if (type <= 1) {
			for (; i < effect->count; i++, data++) {
				data->velY += data->accelY;
				data->y += data->velY;
				data->scaleYStep += 0.005f;
				data->scaleY += data->scaleYStep;
				data->scaleXZ += data->scaleXZStep;
			}
		}
		else {
			for (; i < effect->count; i++, data++) {
				data->velY += data->accelY;
				data->y += data->velY;
				data->scaleY += data->scaleYStep;
				data->scaleXZ += data->scaleXZStep;
			}
		}
		dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effHokoriDisp, effect);
	}
}

static inline void hokoriSetColor1(HokoriParticle* data) {
	GXColor color = (GXColor){ 0, 0, 0, 0 };
	color.r = data->color1R;
	color.g = data->color1G;
	color.b = data->color1B;
	color.a = data->alpha;
	GXSetTevColor(1, color);
}

static inline void hokoriSetColor2(HokoriParticle* data) {
	GXColor color = (GXColor){ 0, 0, 0, 0 };
	color.r = data->color2R;
	color.g = data->color2G;
	color.b = data->color2B;
	GXSetTevColor(2, color);
}

void effHokoriDisp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	HokoriParticle* data;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx trans;
	Mtx scale;
	Mtx rot;
	Mtx work;
	effVtxArray* vtx;
#ifdef MUST_MATCH
	u8* vtxBase = (u8*)lbl_8039D5D0;
#endif
	s32 i;
	s32 type;

	cam = camGetPtr(cameraId);
	data = effect->userdata;

	GXSetNumChans(GX_FALSE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	PSMTXScale(scale, 0.03125f, 0.010416667f, 0.0f);
	GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);

	hokoriSetColor1(data);
	hokoriSetColor2(data);

	switch (data->type) {
	case 0:
		GXSetNumTevStages(1);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
		effGetTexObjN64(0x15, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		break;
	default:
		GXSetNumTevStages(1);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C1, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
		effGetTexObjN64(0x16, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		break;
	}

	PSMTXTrans(trans, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(trans, rot, work);
	PSMTXConcat(cam->view, work, work);

	type = data->type;
	data++;
	GXSetCullMode(GX_CULL_NONE);

	for (i = 1; i < effect->count; i++, data++) {
		PSMTXTrans(trans, data->x, data->y, data->z);
		PSMTXScale(scale, data->scaleXZ, data->scaleY, data->scaleXZ);
		PSMTXConcat(trans, scale, trans);
		PSMTXConcat(work, trans, trans);
		GXLoadPosMtxImm(trans, 0);
		GXSetCurrentMtx(0);
		if (type <= 1) {
			if (i & 1) {
#ifdef MUST_MATCH
				vtx = (effVtxArray*)(vtxBase + 0x40);
#else
				vtx = (effVtxArray*)size16x32_tex32x64_vtx;
#endif
			} else {
#ifdef MUST_MATCH
				vtx = (effVtxArray*)(vtxBase + 0x78);
#else
				vtx = (effVtxArray*)size16x16_tex32x32_vtx;
#endif
			}
		} else {
#ifdef MUST_MATCH
			vtx = (effVtxArray*)(vtxBase + 0xb0);
#else
			vtx = (effVtxArray*)size8x8_tex32x32_vtx;
#endif
		}
		effSetVtxDescN64(vtx);
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
	}
}
