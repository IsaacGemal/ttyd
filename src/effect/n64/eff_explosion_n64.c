#include "eff/n64/eff_explosion_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"

extern GXColor unk_804230e8;
extern GXColor unk_804230ec;
extern GXColor unk_804230f0;
extern const u32 dat_8041ed60;
extern const u32 dat_8041ed64;
extern u32 size32x32_tex32x32_vtx[];

const Vec vec3_802fa510 = { 0.0f, 0.0f, 0.0f };

// The .sbss2 work colours and the two .sdata2 packed TEV colours are defined by the split map.
EffectEntry* effExplosionN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effExplosionN64Data* data;

	effect = effEntry();
	effect->type = "ExplosionN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effExplosionN64Data));
	effect->userdata = data;
	effect->callback = effExplosionMain;
	data->life = duration;
	data->counter = 0;
	data->duration = duration;
	data->ratio = 0.0f;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->velocity = 0.0f;
	if (type == 0) {
		data->accel = 0.0f;
	}
	else {
		data->accel = 40.0f;
	}
	return effect;
}
void effExplosionMain(EffectEntry* effect) {
	effExplosionN64Data* data;
	Vec position2;
	Vec position;

	data = effect->userdata;
	position = (Vec){ 0.0f, 0.0f, 0.0f };
	position.x = data->x;
	position.y = data->y;
	position.z = data->z;
	position2 = position;

	data->ratio = 8.0f * data->counter / data->duration;
	data->counter++;
	data->life--;
	if (data->life < 0) {
		effDelete(effect);
		return;
	}

	data->velocity += data->accel;
	if (data->type == 0) {
		data->scale += 0.1f;
	}

	dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effExplosionDisp, effect);
}

static inline void explosionSetColor1(f32 s) {
	GXColor c = unk_804230e8;
	c.r = 255.0f * s;
	c.g = 18.0f * s;
	c.b = 59.0f * s;
	GXSetTevColor(GX_TEVREG0, c);
}

static inline void explosionSetColor2(f32 s, s32 a) {
	GXColor c = unk_804230ec;
	c.r = 255.0f * s;
	c.g = 53.0f * s;
	c.b = 24.0f * s;
	c.a = a;
	GXSetTevColor(GX_TEVREG1, c);
}

static inline void explosionSetColor3(f32 s) {
	GXColor c = unk_804230f0;
	c.r = 211.0f * s;
	c.g = 255.0f * s;
	c.b = 216.0f * s;
	GXSetTevKColor(GX_KCOLOR0, c);
}

static inline void explosionSetColorAlt0(void) {
	GXColor c = *(GXColor*)&dat_8041ed60;
	GXSetTevColor(GX_TEVREG0, c);
}

static inline void explosionSetColorAlt(s32 a) {
	GXColor c = *(GXColor*)&dat_8041ed64;
	c.a = a;
	GXSetTevColor(GX_TEVREG1, c);
}

// NONMATCHING (98.91%): the type-0 TEV colour temp/argument stack slots and the type!=0 branch's
// first GXSetTevColor argument are 4 bytes off (the else's first colour gets 0x28 instead of 0x10),
// and the texture-matrix offset uses r29 instead of r28.
void effExplosionDisp(CameraId camId, void* param) {
	EffectEntry* effect;
	effExplosionN64Data* data;
	CameraEntry* camera;
	GXTexObj texobj;
	Mtx m0;
	Mtx m2;
	Mtx m1;
	Mtx viewMtx;
	f32 colorScale;
	s32 type;
	s32 lastFrame;
	s32 frame;
	s32 frac;

	effect = param;
	camera = camGetPtr(camId);
	data = effect->userdata;
	type = data->type;
	frame = (s32)data->ratio;
	frac = (s32)(256.0f * (data->ratio - (f32)frame));
	lastFrame = (frame == 7);

	PSMTXTrans(m0, data->x, data->y, data->z);
	PSMTXRotRad(m1, 'y', -camGetPtr(CAMERA_3D)->viewYaw * 0.017453292f);
	PSMTXScale(m2, data->scale, data->scale, data->scale);
	PSMTXConcat(m0, m1, viewMtx);
	PSMTXConcat(viewMtx, m2, viewMtx);
	PSMTXConcat(camera->view, viewMtx, viewMtx);
	GXLoadPosMtxImm(viewMtx, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
	effGetTexObjN64(0xE, &texobj);
	GXLoadTexObj(&texobj, GX_TEXMAP0);
	GXLoadTexObj(&texobj, GX_TEXMAP1);

	if (type == 0) {
		if (data->ratio > 5.0f) {
			colorScale = 1.0f - (data->ratio - 5.0f);
			if (colorScale < 0.0f) {
				colorScale = 0.0f;
			}
		}
		else {
			colorScale = 1.0f;
		}

		explosionSetColor1(colorScale);
		explosionSetColor2(colorScale, frac);
		explosionSetColor3(colorScale);

		GXSetNumTevStages(4);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
		GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG2);
		GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_TEXC, GX_CC_A1, GX_CC_ZERO);
		GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_APREV, GX_CA_TEXA, GX_CA_A1, GX_CA_ZERO);
		GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_C0, GX_CC_C2, GX_CC_C1);
		GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
		GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE3, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_KONST, GX_CC_C2, GX_CC_CPREV);
		GXSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
		GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K0);
	}
	else {
		GXSetTevColor(GX_TEVREG0, *(GXColor*)&dat_8041ed60);

		explosionSetColorAlt(frac);

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
		GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_CPREV, GX_CC_ONE, GX_CC_C0, GX_CC_ZERO);
		GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
	}

	GXSetNumChans(0);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);

	PSMTXScale(m2, 0.00390625f, 0.03125f, 0.0f);
	PSMTXTrans(m0, (f32)(frame * 32), 0.0f, 0.0f);
	PSMTXConcat(m2, m0, m2);
	GXLoadTexMtxImm(m2, GX_TEXMTX0, GX_MTX2x4);

	if (lastFrame) {
		PSMTXScale(m2, 0.0f, 0.0f, 0.0f);
		GXLoadTexMtxImm(m2, GX_TEXMTX1, GX_MTX2x4);
	}
	else {
		PSMTXScale(m2, 0.00390625f, 0.03125f, 0.0f);
		PSMTXTrans(m0, (f32)(frame * 32 + 32), 0.0f, 0.0f);
		PSMTXConcat(m2, m0, m2);
		GXLoadTexMtxImm(m2, GX_TEXMTX1, GX_MTX2x4);
	}

	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64((effVtxArray*)size32x32_tex32x32_vtx);
	GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
	tri2(0, 1, 2, 0, 0, 2, 3, 0);
}

