#include "eff/eff_ibuki.h"
#include "eff/n64/eff_vapor_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "system.h"

#include <dolphin/mtx.h>
#include <dolphin/gx.h>
#include "math_ext.h"
#include "matching.h"
#include <stdlib.h>

// The unit's data, in the original's address order. Force the linker to keep every object.
#pragma force_active on

// .data
__declspec(section ".data") u8 lbl_803B4860[8] = {
	0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
};
static __declspec(section ".data") u8 negone_one_374[8] = {
	0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
};
__declspec(section ".data") u8 lbl_803B4870[8] = {
	0x3F, 0x80, 0x00, 0x00, 0xBF, 0x80, 0x00, 0x00,
};
static __declspec(section ".data") u8 one_negone_373[8] = {
	0x3F, 0x80, 0x00, 0x00, 0xBF, 0x80, 0x00, 0x00,
};
__declspec(section ".data") u8 lbl_803B4880[8] = {
	0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
};
static __declspec(section ".data") u8 negone_one_360[8] = {
	0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
};
__declspec(section ".data") u8 lbl_803B4890[8] = {
	0x3F, 0x80, 0x00, 0x00, 0xBF, 0x80, 0x00, 0x00,
};
static __declspec(section ".data") u8 one_negone_359[8] = {
	0x3F, 0x80, 0x00, 0x00, 0xBF, 0x80, 0x00, 0x00,
};
__declspec(section ".data") u32 ibuki_vertex_tbl[24] ATTRIBUTE_ALIGN(32) = {
	0x00000000, 0x00000F00, 0x00000000, 0x1DFF0000,
	0x000045FF, 0x00000000, 0x55000000, 0x00000000,
	0x05000000, 0x0F000500, 0x00001DFF, 0x05000000,
	0x45FF0500, 0x00005500, 0x05000000, 0x00000A00,
	0x00000F00, 0x0F000000, 0x1DFF1400, 0x000045FF,
	0x23000000, 0x55002800, 0x00000000, 0x00000000,
};
__declspec(section ".data") u32 ibuki_texcoord0_tbl[16] ATTRIBUTE_ALIGN(32) = {
	0x00002000, 0x0B4B2CCC, 0x0B4B4000, 0x00004000,
	0x16963333, 0x16964000, 0x34B43800, 0x34B44000,
	0x40003800, 0x40004000, 0x00000000, 0x0B4B0000,
	0x16960000, 0x34B40000, 0x40000000, 0x00000000,
};
__declspec(section ".data") u32 ibuki_texcoord1_tbl[16] ATTRIBUTE_ALIGN(32) = {
	0x00002000, 0x0B4B2CCC, 0x0B4B4000, 0x00004000,
	0x16963333, 0x16964000, 0x34B43800, 0x34B44000,
	0x40003800, 0x40004000, 0x00000000, 0x0B4B0000,
	0x16960000, 0x34B40000, 0x40000000, 0x00000000,
};
__declspec(section ".data") u32 ibuki_dl_0_0[16] ATTRIBUTE_ALIGN(32) = {
	0x9800060A, 0x00000A0A, 0x0B00020B, 0x0B050000,
	0x00000600, 0x01010100, 0x00000303, 0x01000002,
	0x02000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
__declspec(section ".data") u32 ibuki_dl_0_1[16] ATTRIBUTE_ALIGN(32) = {
	0x9800060B, 0x00020B0B, 0x0C00030C, 0x0C060001,
	0x01010700, 0x01040401, 0x00000202, 0x02000005,
	0x05000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
__declspec(section ".data") u32 ibuki_dl_0_2[16] ATTRIBUTE_ALIGN(32) = {
	0x9800060C, 0x00030C0C, 0x0D00010D, 0x0D070001,
	0x04040800, 0x01060602, 0x00000505, 0x03000007,
	0x07000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
__declspec(section ".data") u32 ibuki_dl_0_3[16] ATTRIBUTE_ALIGN(32) = {
	0x9800060D, 0x00010D0D, 0x0E00000E, 0x0E080001,
	0x06060900, 0x00080803, 0x00000707, 0x04000009,
	0x09000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

#pragma force_active reset

// .sdata
u32 ibuki_normal_tbl[8] = { 0x00004000, 0, 0, 0, 0, 0, 0, 0 };
u32 ibuki_color0_tbl[4] = { 0xFFFFFF00, 0xFFFFFF66, 0xFFFFFF4C, 0xFFFFFF7F };
void* ibuki_dl_0_tbl[4] = { ibuki_dl_0_0, ibuki_dl_0_1, ibuki_dl_0_2, ibuki_dl_0_3 };
u8 ibuki_dl_0_size_tbl[4] = { 2, 2, 2, 2 };

// .sdata2
const GXColor dat_80421f00 = { 0xFF, 0xFF, 0xFF, 0x00 };
const GXColor dat_80421f04 = { 0xFF, 0xFF, 0xFF, 0x00 };

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
	entry->type = "Ibuki";
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
	data->scale = 1.0f;
	data->alpha = 0;
	data->alpha2 = 0;
	data->texU0 = 0.0f;
	data->texV0 = 0.0f;
	data->texU1 = 0.0f;
	data->field_0x4C = 0.0f;

	i = 1;
	wait = 10;
	elem = &data[i];
	for (; i < entry->count; i++, elem++) {
		angle = (f32)(30 - irand(40));
		elem->positionX = 0.0f;
		elem->positionY = (f32)irand(30);
		elem->positionZ = 0.0f;
#ifdef MUST_MATCH
		// Match shape: the comma keeps the 6.2832*angle product in a scratch FPR (f0) instead of
		// coalescing it into angle, and sequences the division, as the target does.
		angle = (angle = 6.2831855f * angle, angle / 360.0f);
#else
		angle = angle * 6.2831855f / 360.0f;
#endif
		elem->velocityX = (f32)cos(angle);
		elem->velocityY = (f32)sin(angle);
		elem->velocityZ = 0.0f;
		elem->scale = 1.0f;
		elem->alpha = 0;
		elem->wait = wait;
		elem->rotX = 0.0f;
		elem->rotY = 0.0f;
		if (irand(1000) < 500) {
			elem->rotVelX = (f32)(irand(10) + 10);
			elem->rotVelY = 0.0f;
		} else {
			elem->rotVelX = 0.0f;
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
	pos = (Vec){ 0.0f, 0.0f, 0.0f };
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
	data->texU0 += -0.0125f;
	data->texV0 += 0.0125f;
	data->texU1 += -0.0125f;
	data->field_0x4C = 0.0f;
	if (data->counter > 0x78 && data->counter % 16 == 0) {
		vaporPos.x = data->positionX + (f32)irand(300);
		vaporPos.y = 30.0f + (data->positionY + (f32)irand(100));
		vaporPos.z = data->positionZ;
		camera = camGetPtr(CAMERA_3D);
		{
			f32 t = -camera->viewYaw;
			t = 0.017453292f * t;
			PSMTXRotRad(mtx, 'y', t);
		}
		PSMTXMultVec(mtx, &vaporPos, &vaporPos);
		effVaporN64Entry(2, 0x1e, vaporPos.x, vaporPos.y, vaporPos.z, 1.0f + (f32)irand(10) / 10.0f);
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
			data->velocityX *= 1.01f;
			data->velocityY *= 1.01f;
			data->velocityZ *= 1.01f;
			data->rotVelX *= 1.01f;
			data->rotVelY *= 1.01f;
			data->alpha += 5;
			if (data->alpha > 0xff) {
				data->alpha = 0xff;
			}
			if (data->positionX > 300.0f) {
				angle = (f32)(30 - irand(40));
				data->positionX = 0.0f;
				data->positionY = (f32)irand(30);
				data->positionZ = 0.0f;
				angle = angle * 6.2831855f / 360.0f;
				data->velocityX = (f32)cos(angle);
				data->velocityY = (f32)sin(angle);
				data->velocityZ = 0.0f;
				data->scale = 1.0f;
				data->alpha = 0;
				data->wait = wait;
				data->rotX = 0.0f;
				data->rotY = 0.0f;
				if (irand(1000) < 500) {
					data->rotVelX = (f32)(irand(10) + 10);
					data->rotVelY = 0.0f;
				} else {
					data->rotVelX = 0.0f;
					data->rotVelY = (f32)(irand(10) + 10);
				}
			}
		}
		wait += 10;
	}
	dispEntry(CAMERA_3D, dispCalcZ(&dispPos), 2, effIbukiDisp, effect);
}

// The first colour is built in a helper that takes the base colour as a parameter and is called
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

// Matches. Beyond the straightforward port, three source shapes were needed: (1) the final loop reuses
// `data` as the element pointer (data = &data[1]; ...; i++, data++) like effIbukiMain, which keeps `effect`
// in r29 and `cameraId` in r28 like retail (a separate `elem` pointer gives the swap); (2) the display-list
// loop is an explicit pointer walk (`j`, `dlSize`, `dlTbl` advanced in the for-latch) with setup ordered
// j = 0 then size then table, reproducing retail's `li` order; (3) the first GXColor helper takes the base
// colour as a parameter and is called directly in GXSetTevColor, while the second reads its base global
// inside the helper and is stored in `color2` -- this is what lands the two colours' inline locals and
// by-value argument copies in retail's 0x10/0x14 and 0x8/0xc stack slots. `opt_lifetimes off` is required
// (without it the effect/cameraId live ranges cascade into 15 extra diffs). The GX vertex block reloads
// float_0/float_2 per vertex (the literal 0.0f/2.0f keep MWCC from CSEing them).
#pragma opt_lifetimes off // required: without it the effect/cameraId live-range cascade costs 15 extra diffs
void effIbukiDisp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	effIbukiData* data = effect->userdata;
	s32 j;
	GXColor color2;
	GXTexObj tex;
	Mtx mtx;
	u8* dlSize;
	void** dlTbl;
	Mtx rot;
	s32 alpha;
	s32 i;
	Mtx scale;
	f32 tmp;
	f32 sixteen;
	f32 eight;
	s32 alpha2;
	Mtx out;
	CameraEntry* camera;

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

	GXSetTevColor(GX_TEVREG0, effIbukiColor0(dat_80421f00, (alpha * alpha2) / 255));

	PSMTXTrans(mtx, data->texU0, data->texV0, 0.0f);
	GXLoadTexMtxImm(mtx, GX_TEXMTX0, GX_MTX2x4);
	PSMTXTrans(mtx, data->texU1, 0.02f, 0.0f);
	GXLoadTexMtxImm(mtx, GX_TEXMTX1, GX_MTX2x4);
	GXSetCullMode(GX_CULL_NONE);
	PSMTXTrans(mtx, data->positionX, data->positionY, data->positionZ);
	PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(cameraId)->viewYaw);
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
	GXSetArray(GX_VA_POS, ibuki_vertex_tbl, 6);
	GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
	GXSetArray(GX_VA_NRM, ibuki_normal_tbl, 3);
	GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GXSetArray(GX_VA_CLR0, ibuki_color0_tbl, 4);
	GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0xe);
	GXSetArray(GX_VA_TEX0, ibuki_texcoord0_tbl, 4);
	GXSetVtxDesc(GX_VA_TEX1, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_S16, 0xe);
	GXSetArray(GX_VA_TEX1, ibuki_texcoord1_tbl, 4);
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
	// `-16.0f` as one expression gets constant-folded (and then `tmp * 0.5f` with it),
	// dropping the 16/0.5 pool pair retail keeps; an opaque local keeps both alive.
	sixteen = 16.0f;
	tmp = -sixteen;
	i = 1;
	data = &data[1];
	for (; i < effect->count; i++, data++) {
		alpha2 = (data->alpha * alpha) / 255;
		if (data->wait == 0) {
			PSMTXTrans(mtx, data->positionX, data->positionY, data->positionZ);
			PSMTXRotRad(rot, 'x', 0.017453292f * data->rotX);
			PSMTXConcat(mtx, rot, mtx);
			PSMTXRotRad(rot, 'y', 0.017453292f * data->rotY);
			PSMTXConcat(mtx, rot, mtx);
			PSMTXScale(scale, data->scale, data->scale, data->scale);
			PSMTXConcat(mtx, scale, mtx);
			PSMTXConcat(out, mtx, mtx);
			GXLoadPosMtxImm(mtx, 0);
			GXSetCurrentMtx(0);
			color2 = effIbukiColor1(alpha2);
			GXSetChanMatColor(GX_COLOR0A0, color2);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			// Local constant: retail keeps 8.0f in f1 across the strip.
			eight = 8.0f;
			GXPosition3f32(tmp / 2.0f, eight, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(eight, eight, 0.0f);
			GXTexCoord2f32(2.0f, 0.0f);
			GXPosition3f32(eight, tmp / 2.0f, 0.0f);
			GXTexCoord2f32(2.0f, 2.0f);
			GXPosition3f32(tmp / 2.0f, tmp / 2.0f, 0.0f);
			GXTexCoord2f32(0.0f, 2.0f);
		}
	}
}
#pragma opt_lifetimes reset
