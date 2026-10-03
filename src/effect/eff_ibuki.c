#include "eff/eff_ibuki.h"
#include "eff/n64/eff_vapor_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "system.h"

#include <dolphin/mtx.h>
#include <dolphin/gx.h>
#include <math.h>
#include <stdlib.h>

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u8 lbl_803B4860[8] = { 0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00 };

extern const Vec vec3_803006c0;
extern const GXColor dat_80421f00;
extern const GXColor dat_80421f04;
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

// The int->float magic pools as an anonymous @N rather than double_to_int_803006d0 (does not affect the
// match score). The angle normalization needs the match shape guarded by MUST_MATCH below, which keeps
// the callee-saved FPRs on f27-f31 in the target's order.
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
	for (; i < entry->count; i++, elem++) {
		angle = (f32)(30 - irand(40));
		elem->positionX = float_0_80421f08;
		elem->positionY = (f32)irand(30);
		elem->positionZ = float_0_80421f08;
#ifdef MUST_MATCH
		// Match shape: the comma keeps the 6.2832*angle product in a scratch FPR (f0) instead of
		// coalescing it into angle, and sequences the division, as the target does.
		angle = (angle = float_6p2832_80421f40 * angle, angle / float_360_80421f44);
#else
		angle = angle * float_6p2832_80421f40 / float_360_80421f44;
#endif
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
	}
	return entry;
}

// The loop reuses `data` as the element pointer (the original colours both in r30) and the vapor scale is
// inlined so MWCC keeps the 1.0f in f0 and the 10.0f in f4. The folded int->float magic still pools as an
// anonymous @N rather than double_to_int_803006d0; that is a relocation-only difference (no score effect).
void effIbukiMain(EffectEntry* effect) {
	s32 wait;
	effIbukiData* data;
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
		{
			f32 t = -camera->viewYaw;
			t = float_deg2rad_80421f10 * t;
			PSMTXRotRad(mtx, 'y', t);
		}
		PSMTXMultVec(mtx, &vaporPos, &vaporPos);
		effVaporN64Entry(2, 0x1e, vaporPos.x, vaporPos.y, vaporPos.z, float_1_80421f30 + (f32)irand(10) / float_10_80421f34);
	}

	i = 1;
	wait = 10;
	data = &data[1];
	for (; i < effect->count; i++, data++) {
		if (data->wait != 0) {
			data->wait--;
		} else {
			data->positionX += data->velocityX;
			data->positionY += data->velocityY;
			data->positionZ += data->velocityZ;
			data->rotX += data->rotVelX;
			data->rotY += data->rotVelY;
			data->velocityX *= float_1p01_80421f38;
			data->velocityY *= float_1p01_80421f38;
			data->velocityZ *= float_1p01_80421f38;
			data->rotVelX *= float_1p01_80421f38;
			data->rotVelY *= float_1p01_80421f38;
			data->alpha += 5;
			if (data->alpha > 0xff) {
				data->alpha = 0xff;
			}
			if (data->positionX > float_300_80421f3c) {
				angle = (f32)(30 - irand(40));
				data->positionX = float_0_80421f08;
				data->positionY = (f32)irand(30);
				data->positionZ = float_0_80421f08;
				angle = (angle = float_6p2832_80421f40 * angle, angle / float_360_80421f44);
				data->velocityX = (f32)cos(angle);
				data->velocityY = (f32)sin(angle);
				data->velocityZ = float_0_80421f08;
				data->scale = float_1_80421f30;
				data->alpha = 0;
				data->wait = wait;
				data->rotX = float_0_80421f08;
				data->rotY = float_0_80421f08;
				if (irand(1000) < 500) {
					data->rotVelX = (f32)(irand(10) + 10);
					data->rotVelY = float_0_80421f08;
				} else {
					data->rotVelX = float_0_80421f08;
					data->rotVelY = (f32)(irand(10) + 10);
				}
			}
		}
		wait += 10;
	}
	dispEntry(CAMERA_3D, dispCalcZ(&dispPos), 2, effIbukiDisp, effect);
}

#ifdef MUST_MATCH
// MUST_MATCH: the first colour is built in a helper that takes the base colour as a parameter and is called
// directly in GXSetTevColor (the second reads its base global inside the helper and is stored in `color2`).
// This is what makes mwcc place the inline local and the outgoing argument copy in retail's stack slots
// (0x10/0x14 for the first colour vs 0x8/0xc for the second); reading the global inside the first helper
// instead leaves its local at 0xc and pushes the second colour's argument to 0x10.
static inline GXColor effIbukiColor0(GXColor c, u8 a) {
	c.a = a;
	return c;
}

static inline GXColor effIbukiColor1(u8 a) {
	GXColor c = dat_80421f04;
	c.a = a;
	return c;
}
#endif

// Matches. Beyond the straightforward port, three source shapes were needed: (1) the final loop reuses
// `data` as the element pointer (data = &data[1]; ...; i++, data++) like effIbukiMain, which keeps `effect`
// in r29 and `cameraId` in r28 like retail (a separate `elem` pointer gives the swap); (2) the display-list
// loop is an explicit pointer walk (`j`, `dlSize`, `dlTbl` advanced in the for-latch) with setup ordered
// j = 0 then size then table, reproducing retail's `li` order; (3) the first GXColor helper takes the base
// colour as a parameter and is called directly in GXSetTevColor, while the second reads its base global
// inside the helper and is stored in `color2` -- this is what lands the two colours' inline locals and
// by-value argument copies in retail's 0x10/0x14 and 0x8/0xc stack slots. `opt_lifetimes off` is required
// (without it the effect/cameraId live ranges cascade into 15 extra diffs). The GX vertex block reloads
// float_0/float_2 per vertex (the literal 0.0f/2.0f keep MWCC from CSEing them; the extern float_8/float_0p5
// keep the tmp*0.5 multiply inside the loop instead of hoisting it).
#pragma opt_lifetimes off // required: without it the effect/cameraId live-range cascade costs 15 extra diffs
void effIbukiDisp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	effIbukiData* data = effect->userdata;
	s32 j;
	GXColor color;
	GXColor color2;
	GXTexObj tex;
	Mtx mtx;
	u8* dlSize;
	void** dlTbl;
	Mtx rot;
	s32 alpha;
	u8* base;
	s32 i;
	Mtx scale;
	f32 tmp;
	s32 alpha2;
	Mtx out;
	CameraEntry* camera;

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

#ifdef MUST_MATCH
	GXSetTevColor(GX_TEVREG0, effIbukiColor0(dat_80421f00, (alpha * alpha2) / 255));
#else
	color = dat_80421f00;
	color.a = (alpha * alpha2) / 255;
	GXSetTevColor(GX_TEVREG0, color);
#endif

	PSMTXTrans(mtx, data->texU0, data->texV0, float_0_80421f08);
	GXLoadTexMtxImm(mtx, GX_TEXMTX0, GX_MTX2x4);
	PSMTXTrans(mtx, data->texU1, float_0p02_80421f0c, float_0_80421f08);
	GXLoadTexMtxImm(mtx, GX_TEXMTX1, GX_MTX2x4);
	GXSetCullMode(GX_CULL_NONE);
	PSMTXTrans(mtx, data->positionX, data->positionY, data->positionZ);
	PSMTXRotRad(rot, 'y', float_deg2rad_80421f10 * -camGetPtr(cameraId)->viewYaw);
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
	j = 0;
	dlSize = ibuki_dl_0_size_tbl;
	dlTbl = ibuki_dl_0_tbl;
	for (; j < 4u; j++, dlSize++, dlTbl++) {
		GXCallDisplayList(*dlTbl, *dlSize << 5);
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
	data = &data[1];
	for (; i < effect->count; i++, data++) {
		alpha2 = (data->alpha * alpha) / 255;
		if (data->wait == 0) {
			PSMTXTrans(mtx, data->positionX, data->positionY, data->positionZ);
			PSMTXRotRad(rot, 'x', float_deg2rad_80421f10 * data->rotX);
			PSMTXConcat(mtx, rot, mtx);
			PSMTXRotRad(rot, 'y', float_deg2rad_80421f10 * data->rotY);
			PSMTXConcat(mtx, rot, mtx);
			PSMTXScale(scale, data->scale, data->scale, data->scale);
			PSMTXConcat(mtx, scale, mtx);
			PSMTXConcat(out, mtx, mtx);
			GXLoadPosMtxImm(mtx, 0);
			GXSetCurrentMtx(0);
#ifdef MUST_MATCH
			color2 = effIbukiColor1(alpha2);
#else
			color2 = dat_80421f04;
			color2.a = alpha2;
#endif
			GXSetChanMatColor(GX_COLOR0A0, color2);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			GXPosition3f32(tmp * float_0p5_80421f18, float_8_80421f1c, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(float_8_80421f1c, float_8_80421f1c, 0.0f);
			GXTexCoord2f32(2.0f, 0.0f);
			GXPosition3f32(float_8_80421f1c, tmp * float_0p5_80421f18, 0.0f);
			GXTexCoord2f32(2.0f, 2.0f);
			GXPosition3f32(tmp * float_0p5_80421f18, tmp * float_0p5_80421f18, 0.0f);
			GXTexCoord2f32(0.0f, 2.0f);
		}
	}
}
#pragma opt_lifetimes reset
