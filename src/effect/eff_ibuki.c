#include "eff/eff_ibuki.h"
#include "eff/n64/eff_vapor_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "system.h"

#include <dolphin/mtx.h>
#include <dolphin/gx.h>
#include <math.h>
#include <stdlib.h>

extern const Vec vec3_803006c0;
extern const GXColor dat_80421f00;
extern const GXColor dat_80421f04;
extern u8 lbl_803B4860[];
// The original reaches this file's tables from its .data section anchor, lbl_803B4860.
#ifdef MUST_MATCH
#define DATA_AT(off, sym) (base + (off))
#else
extern u8 ibuki_vertex_tbl[];
extern u8 ibuki_texcoord0_tbl[];
extern u8 ibuki_texcoord1_tbl[];
#define DATA_AT(off, sym) ((u8*)(sym))
#endif
extern const f32 float_0_80421f08;
extern const f32 float_0p5_80421f18;
extern const f32 float_8_80421f1c;
extern const f32 float_2_80421f20;
extern const f32 float_neg0p0125_80421f24;
extern const f32 float_0p0125_80421f28;
extern const f32 float_30_80421f2c;
extern const f32 float_1_80421f30;
extern const f32 float_10_80421f34;
extern const f32 float_1p01_80421f38;
extern const f32 float_300_80421f3c;
extern const f32 float_6p2832_80421f40;
extern const f32 float_360_80421f44;
extern const f32 float_deg2rad_80421f10;
extern const f32 float_16_80421f14;
extern const f32 float_0p02_80421f0c;

extern u8 ibuki_dl_0_0[];
extern u8 ibuki_dl_0_1[];
extern u8 ibuki_dl_0_2[];
extern u8 ibuki_dl_0_3[];

void* ibuki_dl_0_tbl[4] = { ibuki_dl_0_0, ibuki_dl_0_1, ibuki_dl_0_2, ibuki_dl_0_3 };
u8 ibuki_dl_0_size_tbl[4] = { 2, 2, 2, 2 };
u32 ibuki_normal_tbl[8] = { 0x00004000, 0, 0, 0, 0, 0, 0, 0 };
u32 ibuki_color0_tbl[4] = { 0xFFFFFF00, 0xFFFFFF66, 0xFFFFFF4C, 0xFFFFFF7F };

const f64 double_to_int_803006d0 = 4503601774854144.0;
const char str_80421F48[] = "Ibuki";

// NONMATCHING (95.27%): whole body present; callee-saved register allocation differs (fewer regs, 0x120
// frame vs 0x130), float constants land in a different FPR order, folded int->float magic is an @N.
void effIbukiDisp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	effIbukiData* data = effect->userdata;
	effIbukiData* elem;
	CameraEntry* camera;
	GXColor color;
	GXColor color2;
	GXTexObj tex;
	Mtx mtx;
	Mtx rot;
	Mtx scale;
	Mtx out;
	s32 i;
	s32 alpha;
	s32 alpha2;
	f32 tmp;
	u8* base;

	base = lbl_803B4860;
	camera = camGetPtr(cameraId);
	alpha = data->alpha;
	alpha2 = data->alpha2;
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);
	GXSetNumTevStages(4);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ONE);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXA, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_TEXA, GX_CA_ZERO);
	GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);
	GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_CPREV, GX_CC_C0, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);

	color = dat_80421f00;
	color.a = (alpha * alpha2) / 255;
	GXSetTevColor(GX_TEVREG0, color);

	PSMTXTrans(mtx, data->texU0, data->texV0, float_0_80421f08);
	GXLoadTexMtxImm(mtx, GX_TEXMTX0, GX_MTX2x4);
	PSMTXTrans(mtx, data->texU1, float_0p02_80421f0c, float_0_80421f08);
	GXLoadTexMtxImm(mtx, GX_TEXMTX1, GX_MTX2x4);
	GXSetCullMode(GX_CULL_NONE);
	PSMTXTrans(mtx, data->positionX, data->positionY, data->positionZ);
	camera = camGetPtr(cameraId);
	PSMTXRotRad(rot, 'y', float_deg2rad_80421f10 * -camera->viewYaw);
	if (data->type != 0) {
		PSMTXScale(scale, -data->scale, data->scale, data->scale);
	} else {
		PSMTXScale(scale, data->scale, data->scale, data->scale);
	}
	PSMTXConcat(mtx, rot, out);
	PSMTXConcat(out, scale, out);
	PSMTXConcat(camera->view, out, out);
	GXLoadPosMtxImm(out, 0);
	GXSetCurrentMtx(0);
	effGetTexObj(0x3c, &tex);
	GXLoadTexObj(&tex, GX_TEXMAP0);
	effGetTexObj(0x62, &tex);
	GXLoadTexObj(&tex, GX_TEXMAP1);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 6);
	GXSetArray(GX_VA_POS, DATA_AT(0x40, ibuki_vertex_tbl), 6);
	GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
	GXSetArray(GX_VA_NRM, ibuki_normal_tbl, 3);
	GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GXSetArray(GX_VA_CLR0, ibuki_color0_tbl, 4);
	GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0xe);
	GXSetArray(GX_VA_TEX0, DATA_AT(0xa0, ibuki_texcoord0_tbl), 4);
	GXSetVtxDesc(GX_VA_TEX1, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_S16, 0xe);
	GXSetArray(GX_VA_TEX1, DATA_AT(0xe0, ibuki_texcoord1_tbl), 4);
	for (i = 0; i < 4u; i++) {
		GXCallDisplayList(ibuki_dl_0_tbl[i], ibuki_dl_0_size_tbl[i] << 5);
	}

	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3c, GX_FALSE, GX_PTIDENTITY);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	effGetTexObj(0x63, &tex);
	GXLoadTexObj(&tex, GX_TEXMAP0);
	tmp = -float_16_80421f14;
	i = 1;
	elem = &data[1];
	for (; i < effect->count; i++) {
		alpha2 = (elem->alpha * alpha) / 255;
		if (elem->wait == 0) {
			PSMTXTrans(mtx, elem->positionX, elem->positionY, elem->positionZ);
			PSMTXRotRad(rot, 'x', float_deg2rad_80421f10 * elem->rotX);
			PSMTXConcat(mtx, rot, mtx);
			PSMTXRotRad(rot, 'y', float_deg2rad_80421f10 * elem->rotY);
			PSMTXConcat(mtx, rot, mtx);
			PSMTXScale(scale, elem->scale, elem->scale, elem->scale);
			PSMTXConcat(mtx, scale, mtx);
			PSMTXConcat(out, mtx, mtx);
			GXLoadPosMtxImm(mtx, 0);
			GXSetCurrentMtx(0);
			color2 = dat_80421f04;
			color2.a = alpha2;
			GXSetChanMatColor(GX_COLOR0A0, color2);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			GXPosition3f32(tmp * float_0p5_80421f18, float_8_80421f1c, float_0_80421f08);
			GXTexCoord2f32(float_0_80421f08, float_0_80421f08);
			GXPosition3f32(float_8_80421f1c, float_8_80421f1c, float_0_80421f08);
			GXTexCoord2f32(float_2_80421f20, float_0_80421f08);
			GXPosition3f32(float_8_80421f1c, tmp * float_0p5_80421f18, float_0_80421f08);
			GXTexCoord2f32(float_2_80421f20, float_2_80421f20);
			GXPosition3f32(tmp * float_0p5_80421f18, tmp * float_0p5_80421f18, float_0_80421f08);
			GXTexCoord2f32(float_0_80421f08, float_2_80421f20);
		}
		elem++;
	}
}

// NONMATCHING (98.89%): logic matches; data/elem sit in r31 instead of r30, the deg2rad multiply and the
// 1.0+rand/10 scale use a different FPR order, folded int->float magic is an @N.
void effIbukiMain(EffectEntry* effect) {
	s32 wait;
	effIbukiData* data;
	effIbukiData* elem;
	Vec dispPos;
	Vec vaporPos;
	Vec pos;
	Mtx mtx;
	CameraEntry* camera;
	s32 i;
	f32 angle;

	data = effect->userdata;
	pos = vec3_803006c0;
	pos.x = data->positionX;
	pos.y = data->positionY;
	pos.z = data->positionZ;
	dispPos = pos;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 0x10;
	}
	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	if (data->lifetime < 0) {
		effDelete(effect);
		return;
	}
	if (data->counter < 0x10) {
		data->alpha = data->counter * 0x10 + 0xf;
	}
	if (data->lifetime < 0x10) {
		data->alpha = data->lifetime * 0x10;
	}
	if (data->counter >= 0x3c && data->counter < 0x5c) {
		data->alpha2 = (data->counter - 0x3c) * 8 + 7;
	}
	data->counter++;
	data->texU0 += float_neg0p0125_80421f24;
	data->texV0 += float_0p0125_80421f28;
	data->texU1 += float_neg0p0125_80421f24;
	data->field_0x4C = float_0_80421f08;
	if (data->counter > 0x78 && data->counter % 16 == 0) {
		vaporPos.x = data->positionX + (f32)irand(300);
		vaporPos.y = float_30_80421f2c + (data->positionY + (f32)irand(100));
		vaporPos.z = data->positionZ;
		camera = camGetPtr(CAMERA_3D);
		PSMTXRotRad(mtx, 'y', float_deg2rad_80421f10 * -camera->viewYaw);
		PSMTXMultVec(mtx, &vaporPos, &vaporPos);
		angle = (f32)irand(10);
		effVaporN64Entry(2, 0x1e, vaporPos.x, vaporPos.y, vaporPos.z, float_1_80421f30 + angle / float_10_80421f34);
	}

	i = 1;
	wait = 10;
	elem = &data[1];
	for (; i < effect->count; i++) {
		if (elem->wait != 0) {
			elem->wait--;
		} else {
			elem->positionX += elem->velocityX;
			elem->positionY += elem->velocityY;
			elem->positionZ += elem->velocityZ;
			elem->rotX += elem->rotVelX;
			elem->rotY += elem->rotVelY;
			elem->velocityX *= float_1p01_80421f38;
			elem->velocityY *= float_1p01_80421f38;
			elem->velocityZ *= float_1p01_80421f38;
			elem->rotVelX *= float_1p01_80421f38;
			elem->rotVelY *= float_1p01_80421f38;
			elem->alpha += 5;
			if (elem->alpha > 0xff) {
				elem->alpha = 0xff;
			}
			if (elem->positionX > float_300_80421f3c) {
				angle = (f32)(30 - irand(40));
				elem->positionX = float_0_80421f08;
				elem->positionY = (f32)irand(30);
				elem->positionZ = float_0_80421f08;
				angle = angle * float_6p2832_80421f40 / float_360_80421f44;
				elem->velocityX = (f32)cos(angle);
				elem->velocityY = (f32)sin(angle);
				elem->velocityZ = float_0_80421f08;
				elem->scale = float_1_80421f30;
				elem->alpha = 0;
				elem->wait = wait;
				elem->rotX = float_0_80421f08;
				elem->rotY = float_0_80421f08;
				if (irand(1000) < 500) {
					elem->rotVelX = (f32)(irand(10) + 10);
					elem->rotVelY = float_0_80421f08;
				} else {
					elem->rotVelX = float_0_80421f08;
					elem->rotVelY = (f32)(irand(10) + 10);
				}
			}
		}
		wait += 10;
		elem++;
	}
	dispEntry(CAMERA_3D, dispCalcZ(&dispPos), 2, effIbukiDisp, effect);
}

// NONMATCHING (99.20%): only the callee-saved FPR numbers of the loop-invariant constants differ and the
// folded int->float magic is an anonymous @N instead of double_to_int_803006d0.
EffectEntry* effIbukiEntry(s32 type, s32 lifetime, f32 x, f32 y, f32 z) {
	s32 wait;
	EffectEntry* entry;
	effIbukiData* data;
	effIbukiData* elem;
	s32 i;
	f32 angle;

	entry = effEntry();
	entry->type = str_80421F48;
	entry->count = 0x21;
	data = __memAlloc(HEAP_EFFECT, sizeof(effIbukiData) * 0x21);
	entry->userdata = data;
	entry->callback = effIbukiMain;
	entry->flags |= 2;
	data->type = type;
	data->positionX = x;
	data->positionY = y;
	data->positionZ = z;
	if (lifetime <= 0) {
		data->lifetime = 1000;
	} else {
		data->lifetime = lifetime;
	}
	data->counter = 0;
	data->scale = float_1_80421f30;
	data->alpha = 0;
	data->alpha2 = 0;
	data->texU0 = float_0_80421f08;
	data->texV0 = float_0_80421f08;
	data->texU1 = float_0_80421f08;
	data->field_0x4C = float_0_80421f08;

	i = 1;
	wait = 10;
	elem = &data[i];
	for (; i < entry->count; i++) {
		angle = (f32)(30 - irand(40));
		elem->positionX = float_0_80421f08;
		elem->positionY = (f32)irand(30);
		elem->positionZ = float_0_80421f08;
		angle = angle * float_6p2832_80421f40 / float_360_80421f44;
		elem->velocityX = (f32)cos(angle);
		elem->velocityY = (f32)sin(angle);
		elem->velocityZ = float_0_80421f08;
		elem->scale = float_1_80421f30;
		elem->alpha = 0;
		elem->wait = wait;
		elem->rotX = float_0_80421f08;
		elem->rotY = float_0_80421f08;
		if (irand(1000) < 500) {
			elem->rotVelX = (f32)(irand(10) + 10);
			elem->rotVelY = float_0_80421f08;
		} else {
			elem->rotVelX = float_0_80421f08;
			elem->rotVelY = (f32)(irand(10) + 10);
		}
		wait += 10;
		elem++;
	}
	return entry;
}
