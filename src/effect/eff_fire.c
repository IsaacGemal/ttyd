#include "eff/eff_fire.h"
#include "eff/n64/eff_fire_spark_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "driver/mapdrv.h"
#include "matching.h"

#include <dolphin/mtx.h>
#include <dolphin/gx.h>
#include <stdlib.h>

extern GXColor unk_80423064;
#ifdef MUST_MATCH
// Match-only: non-const object forced into .rodata so effFireEntry copies it with integer lwz/stw
// (a const object is folded into lfsu), as the target does.
__declspec(section ".rodata") Vec vec3_802f8c10 = { 0.0f, 0.0f, 0.0f };
#else
const Vec vec3_802f8c10 = { 0.0f, 0.0f, 0.0f };
#endif
const Vec vec3_802f8c1c = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f8c28 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f8c34 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f8c40 = { 0.0f, 0.0f, 0.0f };
#ifdef MUST_MATCH
// Match-only: the target copies vec3_802f8c4c into smokePos with integer lwz/stw from a lis/addi address.
// A plain `const Vec` lets MWCC fold that copy into lfsu; a non-const object forced into .rodata gives the
// integer copy and still places the symbol in .rodata, the section the original uses.
__declspec(section ".rodata") Vec vec3_802f8c4c = { 0.0f, 0.0f, 0.0f };
#else
const Vec vec3_802f8c4c = { 0.0f, 0.0f, 0.0f };
#endif
extern const f32 float_0p0078125_8041e0ec;
extern const f32 float_1p5_8041e0f0;
extern const f32 float_0p8_8041e0f4;
extern const f32 float_deg2rad_8041e0f8;
extern const f32 float_8_8041e0fc;
extern const f32 float_1_8041e100;
extern const f32 float_0p5_8041e104;
extern const f32 float_0_8041e108;
extern const f32 float_0p95_8041e10c;
extern const f32 float_0p99_8041e110;
extern const f32 float_neg8_8041e114;
extern const f32 float_1p4_8041e158;
extern const f32 float_1p2_8041e15c;
extern const f32 float_10_8041e160;
extern const f32 float_30_8041e164;
extern const f32 float_20_8041e13c;
extern const f32 float_0p03125_8041e14c;
extern const f32 float_0p01_8041e150;
extern const f32 float_neg0p01_8041e154;
extern const u32 dat_8041e0bc;
extern const u32 dat_8041e0c0;
extern const u32 dat_8041e0c4;
extern const u32 dat_8041e0c8;
extern const u32 dat_8041e0cc;
extern const u32 dat_8041e0d0;
extern const u32 dat_8041e0d4;
extern const u32 dat_8041e0d8;
extern const u32 dat_8041e0dc;
extern const u32 dat_8041e0e0;
extern const u32 dat_8041e0e4;
extern const u32 dat_8041e0e8;
extern const f32 float_32_8041e118;
extern const f32 float_16_8041e12c;
extern const f32 float_40_8041e130;
extern const f32 float_24_8041e134;
extern const f32 float_12_8041e138;
extern const f32 float_25_8041e140;
extern const f32 float_0p7854_8041e144;
extern const f32 float_2_8041e148;
extern const u32 unk_80423058;
extern const u32 unk_8042305c;
extern const u32 unk_80423060;
extern const f32 float_5_8041e11c;
extern const f32 float_0p6_8041e120;
extern const f32 float_13p333_8041e124;
extern const f32 float_0p33333_8041e128;
extern const u32 dat_8041e0b8;
const char str_8041E168[]; // tentative definition, completed after the code so it emits last in .sdata2
const char str_802F8C60[] = "FireSmoke";


ScaleEntry scale1[13] = {
	{ 0x00, 1.0f }, { 0x14, 1.1f }, { 0x1E, 1.0f }, { 0x2D, 1.05f },
	{ 0x3C, 0.95f }, { 0x46, 1.05f }, { 0x50, 1.025f }, { 0x64, 1.1f },
	{ 0x78, 1.0f }, { 0x82, 1.025f }, { 0xA0, 0.95f }, { 0xB4, 1.025f },
	{ 0xC8, 1.0f },
};
ScaleEntry scale2[4] = {
	{ 0x00, 1.0f }, { 0x64, 1.2f }, { 0xA0, 0.95f }, { 0xC8, 1.0f },
};

static u8 scale_dt[] = {
	0x80, 0x80, 0x81, 0x7F, 0x82, 0x7F, 0x83, 0x7E, 0x84, 0x7E, 0x84, 0x7E, 0x85, 0x7D, 0x86, 0x7D,
	0x87, 0x7C, 0x88, 0x7C, 0x89, 0x7C, 0x8A, 0x7B, 0x8A, 0x7B, 0x8B, 0x7B, 0x8C, 0x7B, 0x8D, 0x7A,
	0x8D, 0x7A, 0x8E, 0x7A, 0x8F, 0x79, 0x8F, 0x79, 0x90, 0x79, 0x91, 0x79, 0x92, 0x78, 0x92, 0x78,
	0x93, 0x78, 0x94, 0x77, 0x95, 0x77, 0x95, 0x77, 0x96, 0x76, 0x97, 0x76, 0x98, 0x76, 0x98, 0x76,
	0x99, 0x75, 0x9A, 0x75, 0x9B, 0x74, 0x9C, 0x74, 0x9D, 0x74, 0x9E, 0x73, 0x9F, 0x73, 0xA0, 0x73,
	0xA1, 0x72, 0xA2, 0x72, 0xA3, 0x71, 0xA5, 0x71, 0xA6, 0x70, 0xA7, 0x70, 0xA9, 0x6F, 0xAA, 0x6F,
	0xAB, 0x6E, 0xAD, 0x6D, 0xAE, 0x6D, 0xB0, 0x6C, 0xB2, 0x6B, 0xB4, 0x6B, 0xB5, 0x6A, 0xB7, 0x69,
	0xB9, 0x68, 0xBB, 0x68, 0xBD, 0x67, 0xC0, 0x66,
};

static u32 col_tbl[24] = {
	0xAC2E00FF, 0xE02200FF, 0xFD5F00FF, 0xFD5F04FF,
	0xFF6900FF, 0xFF8A00FF, 0xFFB700FF, 0xFFD700FF,
	0x0AE0FFFF, 0x10C3FFFF, 0x0961FFFF, 0x0961FFFF,
	0xD1F9FFFF, 0xD1F9FFFF, 0xA3E5FFFF, 0x3ACAFFFF,
	0x5BF63DFF, 0x3BDE1CFF, 0x3BDE1CFF, 0x1DB900FF,
	0xDEF618FF, 0xA0F606FF, 0xA0F606FF, 0x92E400FF,
};

EffectEntry* effFireEntry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry;
	effFireData* data;
	Vec position;

	entry = effEntry();
	entry->type = str_8041E168;
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x8C);
	entry->userdata = data;
	entry->callback = effFireMain;
	entry->flags |= 2;
	if (lifetime <= 0) {
		lifetime = 1000;
	}
	data->type = type;
	data->alpha = 0xFF;
#ifdef MUST_MATCH
	// Match-only: an anonymous zero literal instead of the named vec3_802f8c10 makes MWCC schedule the
	// alpha store before the position copy, as the target does (same shape as effFireworksMakeVec).
	position = vec3_802f8c10;
#else
	position = vec3_802f8c10;
#endif
	position.x = x;
	position.y = y;
	position.z = z;
	data->position = position;
	data->scale = scale;
	data->lifetime = lifetime;
	data->counter = 0;
	data->field_0x78 = float_1_8041e100;
	data->field_0x7C = float_1_8041e100;
	data->sparkTimer = 0;
	data->sparkInterval = rand() % 30 + 30;
	data->colorIndex1 = 0;
	data->colorIndex2 = 0;
	data->field_0x4C = float_0_8041e108;
	data->field_0x50 = float_0_8041e108;
	data->field_0x54 = float_0_8041e108;
	data->field_0x58 = float_0_8041e108;
	data->field_0x5C = float_0_8041e108;
	data->field_0x60 = float_0_8041e108;

	switch (type) {
	case 0:
		data->frameMax = 0x0C;
		data->counterMax = data->frameMax * 2;
		break;
	case 1:
		data->frameMax = 0x04;
		data->counterMax = data->frameMax * 2;
		break;
	case 2:
		data->frameMax = 0x0A;
		data->counterMax = data->frameMax * 2;
		break;
	case 3:
	case 4:
	case 8:
		data->frameMax = 0x0C;
		data->counterMax = data->frameMax * 2;
		data->alpha = 0xCC;
		break;
	case 5:
	case 6:
	case 7:
		data->frameMax = 0x0C;
		data->counterMax = data->frameMax * 2;
		data->alpha = 0xFF;
		break;
	}

	data->counter = rand() % data->counterMax;
	data->scaleIndex = 0;
	data->scaleCounter = 0;
	data->curScale = float_0_8041e108;
	data->field_0x80 = 0;
	data->field_0x84 = 0;
	data->field_0x88 = 0;
	return entry;
}

#pragma opt_common_subs off // without it MWCC CSEs data->position across the calls in the default branch into different registers
void effFireMain(EffectEntry* effect) {
	effFireData* data;
	s32 type;
	s32 count;
	const ScaleEntry* table;
	f32 factor;
	EffectEntry* smokeEntry;
	data = effect->userdata;
	type = data->type;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 0x10;
	}
	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	if (data->lifetime < 16) {
		data->alpha = data->lifetime * 16;
	}
	if (data->lifetime < 0) {
		effDelete(effect);
		return;
	}
	switch (type) {
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
		data->colorIndex1 = (s32)((f32)data->frameMax * (f32)data->counter / (f32)data->counterMax);
		data->colorIndex2 = (s32)((f32)data->frameMax * (f32)(data->counter + 1) / (f32)data->counterMax);
		if (data->colorIndex1 >= data->frameMax) {
			data->colorIndex1 = 0;
		}
		if (data->colorIndex2 >= data->frameMax) {
			data->colorIndex2 = 0;
		}
		data->field_0x38 = 0x80;
		break;
	}
	data->counter++;
	if (data->counter > data->counterMax) {
		data->counter = 0;
	}

	switch (type) {
	case 3:
	case 4:
	case 8:
		switch (type) {
		case 3:
		case 8:
			table = scale1;
			count = 13;
			break;
		case 4:
			table = scale2;
			count = 4;
			break;
		}
		for (;;) {
			if (data->scaleCounter < table[data->scaleIndex].threshold) {
				break;
			}
			data->scaleIndex++;
			if (data->scaleIndex >= count) {
				data->scaleIndex = 0;
				data->scaleCounter = 0;
			}
		}
		factor = (f32)(table[data->scaleIndex].threshold - data->scaleCounter) /
			(f32)(table[data->scaleIndex].threshold - table[data->scaleIndex - 1].threshold);
		data->curScale = (1.0f - factor) * table[data->scaleIndex].scale +
			factor * table[data->scaleIndex - 1].scale;
		data->curScale = data->curScale * float_1p4_8041e158;
		data->curScale = data->curScale * float_1p2_8041e15c;
		data->scaleCounter++;
		break;
	case 5:
	case 6:
	case 7:
		data->sparkTimer++;
		if (data->sparkTimer > data->sparkInterval) {
			effFireSparkN64Entry(type, 2, 200, data->position.x, data->position.y + float_10_8041e160 * data->scale,
				data->position.z, float_30_8041e164 * data->field_0x78, float_30_8041e164,
				data->scale, float_1_8041e100, float_1_8041e100);
			data->sparkTimer = 0;
			data->sparkInterval = rand() % 30 + 30;
		}
		break;
	default:
		data->sparkTimer++;
		if (data->sparkTimer > data->sparkInterval) {
			u32 offsets = dat_8041e0b8;
#ifdef MUST_MATCH
			// Match-only: declaring px/py/pz/sc uninitialised and assigning them in the target's computation
			// order (pz, sc, px, py) makes MWCC colour pz=f29 and px=f31 as the original. Declaring them with
			// initialisers in this order colours pz=f31 and px=f29.
			f32 px;
			f32 py;
			f32 pz;
			f32 sc;
#else
			f32 pz = data->position.z - 5.0f;
			f32 py = data->position.y + (f32)((u8*)&offsets)[type];
			f32 px = data->position.x;
			f32 sc = data->scale;
#endif
			Vec smokePos;
			effFireSmokeData* smoke;
#ifdef MUST_MATCH
			pz = data->position.z - 5.0f;
			sc = data->scale;
			px = data->position.x;
			py = data->position.y + (f32)((u8*)&offsets)[type];
#endif

			smokeEntry = effEntry();
			smokeEntry->type = str_802F8C60;
			smokeEntry->count = 1;
			smoke = __memAlloc(HEAP_EFFECT, 0x40);
			smokeEntry->userdata = smoke;
			smokeEntry->callback = effFireSmokeMain;
			smokeEntry->flags |= 2;
			smoke->field_0x0 = 0;
			smoke->color.a = 0x80;
			smokePos = vec3_802f8c4c;
			smokePos.x = px;
			smokePos.y = py;
			smokePos.z = pz;
			smoke->position = smokePos;
			smoke->scale = sc;
			smoke->lifetime = 0x3C;
			smoke->counter = 0;
			smoke->velocity.x = float_0_8041e108;
			smoke->velocity.y = float_1_8041e100;
			smoke->velocity.z = float_0_8041e108;
			smoke->angleVel = float_neg8_8041e114;
			smoke->color.r = 0xCF;
			smoke->color.g = 0x67;
			smoke->color.b = 0x38;
			data->sparkTimer = 0;
			data->sparkInterval = rand() % 30 + 30;
		}
		break;
	}

	switch (type) {
	case 3:
	case 4:
	case 8:
		dispEntry(CAMERA_3D, dispCalcZ(&data->position), 2, effFireDisp2, effect);
		break;
	case 5:
	case 6:
	case 7:
		dispEntry(CAMERA_3D, dispCalcZ(&data->position), 2, effFireDisp3, effect);
		break;
	default:
		dispEntry(CAMERA_3D, dispCalcZ(&data->position), 8, effFireDisp, effect);
		break;
	}
}
#pragma opt_common_subs reset

void effFireDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effFireData* data = effect->userdata;
	CameraEntry* camera;
	GXTexObj tex;
	Mtx transMtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	Mtx mtx;
	Mtx offsetMtx;
	s32 type;
	s32 count;
	s32 texWidth;
	s32 texHeight;
	s32 frame;
	f32 scale;

	camera = camGetPtr(camId);
	type = data->type;
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
	switch (type) {
	case 0:
		effGetTexObj(0x22, &tex);
		GXLoadTexObj(&tex, GX_TEXMAP0);
		count = 0xC;
		texWidth = 0x20;
		texHeight = 0x20;
		break;
	case 1:
		effGetTexObj(0x46, &tex);
		GXLoadTexObj(&tex, GX_TEXMAP0);
		count = 4;
		texWidth = 0x20;
		texHeight = 0x28;
		break;
	case 2:
		effGetTexObj(0x47, &tex);
		GXLoadTexObj(&tex, GX_TEXMAP0);
		count = 0xA;
		texWidth = 0x18;
		texHeight = 0x28;
		break;
	default:
		break;
	}
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x1E, GX_FALSE, 0x7D);

	frame = (data->counter >> 1) % count;
	PSMTXScale(scaleMtx, 1.0f / (f32)count, 1.0f, 1.0f);
	PSMTXTrans(transMtx, (f32)frame, 0.0f, 0.0f);
	PSMTXConcat(scaleMtx, transMtx, mtx);
	GXLoadTexMtxImm(mtx, 0x1E, GX_MTX2x4);

	PSMTXTrans(transMtx, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(rotMtx, 'y', -camGetPtr(camId)->viewYaw * float_deg2rad_8041e0f8);
	scale = float_20_8041e13c * data->scale;
	scale = scale * float_0p03125_8041e14c;
	PSMTXScale(scaleMtx, scale * data->field_0x78, scale * data->field_0x7C, scale);
	PSMTXConcat(transMtx, rotMtx, mtx);
	PSMTXConcat(mtx, scaleMtx, mtx);
	PSMTXConcat(camera->view, mtx, mtx);
	GXSetCurrentMtx(0);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	PSMTXTrans(offsetMtx, float_0p01_8041e150, float_neg0p01_8041e154, float_0_8041e108);
	PSMTXConcat(mtx, offsetMtx, offsetMtx);
	GXLoadPosMtxImm(offsetMtx, 0);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, (GXTevOp)0xE, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_KONST);
	GXSetTevAlphaIn(GX_TEVSTAGE0, (GXTevAlphaArg)4, (GXTevAlphaArg)1, (GXTevAlphaArg)6, (GXTevAlphaArg)7);
	GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
	GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K0_A);
	GXSetTevKColor(GX_KCOLOR0, *(GXColor*)&dat_8041e0bc);
	GXSetTevColor(GX_TEVREG0, *(GXColor*)&dat_8041e0c0);

	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	{
		f32 w = (f32)texWidth;
		f32 h = (f32)texHeight;
		f32 half = 0.5f;
		GXPosition3f32(-w * half, h, 0.0f);
		GXColor4u8(0xFF, 0xFF, 0xFF, 0xFF);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(w * half, h, 0.0f);
		GXColor4u8(0xFF, 0xFF, 0xFF, 0xFF);
		GXTexCoord2f32(1.0f, 0.0f);
		GXPosition3f32(w * half, 0.0f, 0.0f);
		GXColor4u8(0, 0, 0, 0xFF);
		GXTexCoord2f32(1.0f, 1.0f);
		GXPosition3f32(-w * half, 0.0f, 0.0f);
		GXColor4u8(0, 0, 0, 0xFF);
		GXTexCoord2f32(0.0f, 1.0f);
	}
	GXLoadPosMtxImm(mtx, 0);
	GXSetNumTevStages(4);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
	GXSetTevColorIn(GX_TEVSTAGE0, 0xC, 0xE, 0xA, 0xF);
	GXSetTevAlphaIn(GX_TEVSTAGE0, 7, 7, 7, 7);
	GXSetTevKColorSel(GX_TEVSTAGE0, 0xC);
	GXSetTevKColor(GX_KCOLOR0, *(GXColor*)&dat_8041e0c4);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1);
	GXSetTevColorIn(GX_TEVSTAGE1, 0xC, 0xE, 0xA, 0xF);
	GXSetTevAlphaIn(GX_TEVSTAGE1, 7, 7, 7, 7);
	GXSetTevKColorSel(GX_TEVSTAGE1, 0xD);
	GXSetTevKColor(GX_KCOLOR1, *(GXColor*)&dat_8041e0c8);
	GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE2, (GXTevOp)0xE, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE2, 0xF, 0xF, 0xF, 2);
	GXSetTevAlphaIn(GX_TEVSTAGE2, 4, 3, 6, 7);
	GXSetTevKAlphaSel(GX_TEVSTAGE2, 0);
	GXSetTevColor((GXTevRegID)3, *(GXColor*)&dat_8041e0cc);
	GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE3, 4, 0, 1, 0xF);
	GXSetTevAlphaIn(GX_TEVSTAGE3, 4, 7, 0, 0);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	{
		f32 w = (f32)texWidth;
		f32 h = (f32)texHeight;
		f32 half = 0.5f;
		GXPosition3f32(-w * half, h, 0.0f);
		GXColor4u8(0xFF, 0xFF, 0xFF, 0xFF);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(w * half, h, 0.0f);
		GXColor4u8(0xFF, 0xFF, 0xFF, 0xFF);
		GXTexCoord2f32(1.0f, 0.0f);
		GXPosition3f32(w * half, 0.0f, 0.0f);
		GXColor4u8(0, 0, 0, 0xFF);
		GXTexCoord2f32(1.0f, 1.0f);
		GXPosition3f32(-w * half, 0.0f, 0.0f);
		GXColor4u8(0, 0, 0, 0xFF);
		GXTexCoord2f32(0.0f, 1.0f);
	}
}

#pragma opt_common_subs off // keeps the two GX quads' constant loads out of CSE, which gives the original's FP colouring
#ifdef MUST_MATCH
static inline GXColor effFireDisp2Color(GXColor c, u8 a) {
	c.r = a;
	c.g = a;
	c.b = a;
	c.a = a;
	return c;
}

static inline GXColor effFireDisp2KColor(GXColor k, u8 a) {
	k.a = a;
	return k;
}
#endif

void effFireDisp2(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effFireData* data = effect->userdata;
	CameraEntry* camera;
	GXTexObj tex;
	Mtx transMtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	Mtx mtx;
	GXColor c;
	GXColor k;
	s32 type;
	f32 negHalf;

	camera = camGetPtr(camId);
	type = data->type;
	effGetTexObj(0x48, &tex);
	GXLoadTexObj(&tex, GX_TEXMAP0);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x1E, GX_FALSE, 0x7D);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, 0x21, GX_FALSE, 0x7D);

	PSMTXScale(scaleMtx, 1.0f / (f32)data->frameMax, 1.0f, 1.0f);
	PSMTXTrans(transMtx, (f32)data->colorIndex1, 0.0f, 0.0f);
	PSMTXConcat(scaleMtx, transMtx, transMtx);
	GXLoadTexMtxImm(transMtx, 0x1E, GX_MTX2x4);

	PSMTXScale(scaleMtx, 1.0f / (f32)data->frameMax, 1.0f, 1.0f);
	PSMTXTrans(transMtx, (f32)data->colorIndex2, 0.0f, 0.0f);
	PSMTXConcat(scaleMtx, transMtx, transMtx);
	GXLoadTexMtxImm(transMtx, 0x21, GX_MTX2x4);

	PSMTXTrans(transMtx, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(rotMtx, 'y', -camGetPtr(camId)->viewYaw * float_deg2rad_8041e0f8);
	PSMTXScale(scaleMtx, data->scale * data->field_0x78, data->scale * data->field_0x7C, data->scale);
	PSMTXConcat(transMtx, rotMtx, mtx);
	PSMTXConcat(mtx, scaleMtx, mtx);
	PSMTXConcat(camera->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, 0);
	GXSetCurrentMtx(0);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GXSetNumTevStages(3);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, (GXTevColorArg)4, (GXTevColorArg)2, (GXTevColorArg)0xA, (GXTevColorArg)0xF);
	GXSetTevAlphaIn(GX_TEVSTAGE0, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)4);
	switch (type) {
	case 3:
	case 4:
		GXSetTevColor(GX_TEVREG0, *(GXColor*)&dat_8041e0d0);
		GXSetTevColor(GX_TEVREG1, *(GXColor*)&dat_8041e0d4);
		break;
	case 8:
		GXSetTevColor(GX_TEVREG0, *(GXColor*)&dat_8041e0d8);
		GXSetTevColor(GX_TEVREG1, *(GXColor*)&dat_8041e0dc);
		break;
	}
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, (GXTevColorArg)0xF, (GXTevColorArg)0xF, (GXTevColorArg)0xF, (GXTevColorArg)0);
	GXSetTevAlphaIn(GX_TEVSTAGE1, (GXTevAlphaArg)0, (GXTevAlphaArg)4, (GXTevAlphaArg)3, (GXTevAlphaArg)7);
#ifdef MUST_MATCH
	c = *(GXColor*)&unk_80423058;
	GXSetTevColor((GXTevRegID)3, effFireDisp2Color(c, data->field_0x38));
#else
	c = *(GXColor*)&unk_80423058;
	c.r = data->field_0x38;
	c.g = data->field_0x38;
	c.b = data->field_0x38;
	c.a = data->field_0x38;
	GXSetTevColor((GXTevRegID)3, c);
#endif
	GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE2, (GXTevColorArg)0xF, (GXTevColorArg)0xF, (GXTevColorArg)0xF, (GXTevColorArg)0);
	GXSetTevAlphaIn(GX_TEVSTAGE2, (GXTevAlphaArg)7, (GXTevAlphaArg)0, (GXTevAlphaArg)6, (GXTevAlphaArg)7);
	GXSetTevKAlphaSel(GX_TEVSTAGE2, (GXTevKAlphaSel)0x1C);
#ifdef MUST_MATCH
	k = *(GXColor*)&unk_8042305c;
	GXSetTevKColor(GX_KCOLOR0, effFireDisp2KColor(k, data->alpha));
#else
	k = *(GXColor*)&unk_8042305c;
	k.a = data->alpha;
	GXSetTevKColor(GX_KCOLOR0, k);
#endif

	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	negHalf = -float_24_8041e134 * float_0p5_8041e104;
	GXPosition3f32(negHalf, float_40_8041e130, 0.0f);
	GXColor4u8(0xFF, 0xFF, 0xFF, 0xFF);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3f32(float_12_8041e138, float_40_8041e130, 0.0f);
	GXColor4u8(0xFF, 0xFF, 0xFF, 0xFF);
	GXTexCoord2f32(1.0f, 0.0f);
	GXPosition3f32(float_12_8041e138, 0.0f, 0.0f);
	GXColor4u8(0, 0, 0, 0xFF);
	GXTexCoord2f32(1.0f, 1.0f);
	GXPosition3f32(negHalf, 0.0f, 0.0f);
	GXColor4u8(0, 0, 0, 0xFF);
	GXTexCoord2f32(0.0f, 1.0f);

	switch (type) {
	case 3:
	case 8:
		PSMTXTrans(transMtx, 0.0f, float_20_8041e13c, 0.0f);
		break;
	case 4:
		PSMTXTrans(transMtx, 0.0f, float_25_8041e140, 0.0f);
		break;
	}
	PSMTXScale(scaleMtx, data->curScale, data->curScale, data->curScale);
	PSMTXConcat(transMtx, scaleMtx, transMtx);
	if (type == 4) {
		PSMTXRotRad(rotMtx, 'z', float_0p7854_8041e144);
		PSMTXConcat(transMtx, rotMtx, transMtx);
	}
	PSMTXConcat(mtx, transMtx, transMtx);
	GXLoadPosMtxImm(transMtx, 0);
	GXSetCurrentMtx(0);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	switch (type) {
	case 3:
		GXSetChanMatColor(GX_COLOR0A0, *(GXColor*)&dat_8041e0e0);
		break;
	case 4:
		GXSetChanMatColor(GX_COLOR0A0, *(GXColor*)&dat_8041e0e4);
		break;
	case 8:
		GXSetChanMatColor(GX_COLOR0A0, *(GXColor*)&dat_8041e0e8);
		break;
	}
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C, GX_FALSE, 0x7D);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	GXSetTevColorIn(GX_TEVSTAGE0, (GXTevColorArg)0xF, (GXTevColorArg)0xF, (GXTevColorArg)0xF, (GXTevColorArg)0xA);
	switch (type) {
	case 3:
	case 8:
		effGetTexObj(0x5F, &tex);
		GXLoadTexObj(&tex, GX_TEXMAP0);
		break;
	case 4:
		effGetTexObj(0x60, &tex);
		GXLoadTexObj(&tex, GX_TEXMAP0);
		break;
	}
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	{
		f32 f3;
		f32 f4;
		f32 f2;
		if (type == 3 || type == 8) {
			f3 = float_2_8041e148;
			f4 = 1.0f;
		} else {
			f3 = float_2_8041e148;
			f4 = f3;
		}
		f2 = -float_32_8041e118 * float_0p5_8041e104;
		GXPosition3f32(f2, float_16_8041e12c, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(float_16_8041e12c, float_16_8041e12c, 0.0f);
		GXTexCoord2f32(f3, 0.0f);
		GXPosition3f32(float_16_8041e12c, f2, 0.0f);
		GXTexCoord2f32(f3, f4);
		GXPosition3f32(f2, f2, 0.0f);
		GXTexCoord2f32(0.0f, f4);
	}
}
#pragma opt_common_subs reset

// effFireDisp3 source-shape notes (all needed for the byte match):
//  - `data` is declared after `type` and assigned just before `camGetPtr`, with `type = data->type` after
//    the call; that gives the target's data r26 / type r27.
//  - `third` (0.33333f) and `sixteen` (16.0f) are f32 locals so MWCC caches them in f29/f30, and the
//    inner-loop GXPosition3f32/GXTexCoord2f32 y and texcoord values are inline expressions rather than
//    hoisted locals; c0/c1 stay `const u8*`.
//  - the four zero-vector copies below are MUST_MATCH: without the volatile read MWCC hoists all twelve
//    loads out of the outer loop, and without the explicit temp the frame loses 0x30.
//  - the TevReg0 colour is built by the MUST_MATCH inline effFireDisp3KColor so GXSetTevColor gets the
//    target's stack argument slot.
//  - vec3_802f8c10 is a non-const object forced into .rodata so effFireEntry copies it with integer
//    lwz/stw as the target does (a const object is folded into lfsu).
#ifdef MUST_MATCH
// Match-only: building the TevReg0 colour in an inline returning GXColor gives the target's
// GXSetTevColor stack argument slot; a plain assignment lands in a different one.
static inline GXColor effFireDisp3KColor(u8 a) {
	GXColor c = *(GXColor*)&unk_80423060;
	c.a = a;
	return c;
}
#endif
void effFireDisp3(CameraId camId, void* param) {
	EffectEntry* effect = param;
	CameraEntry* camera;
	MapWork* work;
	GXTexObj tex;
	Mtx transMtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	Mtx mtx;
	Mtx mtx80;
	GXColor blend;
	GXColor k;
	Vec v;
	Vec screen;
	u8* colEntry;
	s32 type;
	effFireData* data;
	s32 i;
	s32 j;
	f32 f27;
	f32 f26;
	f32 third = 0.33333334f;
	f32 sixteen = 16.0f;

	data = effect->userdata;
	camera = camGetPtr(camId);
	type = data->type;

	effGetTexObj(0x49, &tex);
	GXLoadTexObj(&tex, GX_TEXMAP0);
	effGetTexObj(0x49, &tex);
	GXLoadTexObj(&tex, GX_TEXMAP1);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x1E, GX_FALSE, 0x7D);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, 0x21, GX_FALSE, 0x7D);

	PSMTXScale(scaleMtx, float_1_8041e100 / (f32)data->frameMax, float_1_8041e100, float_1_8041e100);
	PSMTXTrans(transMtx, (f32)data->colorIndex1, float_0_8041e108, float_0_8041e108);
	PSMTXConcat(scaleMtx, transMtx, transMtx);
	GXLoadTexMtxImm(transMtx, 0x1E, GX_MTX2x4);

	PSMTXScale(scaleMtx, float_1_8041e100 / (f32)data->frameMax, float_1_8041e100, float_1_8041e100);
	PSMTXTrans(transMtx, (f32)data->colorIndex2, float_0_8041e108, float_0_8041e108);
	PSMTXConcat(scaleMtx, transMtx, transMtx);
	GXLoadTexMtxImm(transMtx, 0x21, GX_MTX2x4);

	PSMTXTrans(transMtx, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(rotMtx, 'y', -camGetPtr(camId)->viewYaw * float_deg2rad_8041e0f8);
	PSMTXScale(scaleMtx, data->scale * data->field_0x78, data->scale * data->field_0x7C, data->scale);
	PSMTXConcat(transMtx, rotMtx, mtx);
	PSMTXConcat(mtx, scaleMtx, mtx);
	GXSetNumTevStages(3);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, (GXTevColorArg)0xF, (GXTevColorArg)0xF, (GXTevColorArg)0xF, (GXTevColorArg)0xF);
	GXSetTevAlphaIn(GX_TEVSTAGE0, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)4);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, (GXTevScale)3, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, (GXTevColorArg)0xF, (GXTevColorArg)0xF, (GXTevColorArg)0xF, (GXTevColorArg)0xA);
	GXSetTevAlphaIn(GX_TEVSTAGE1, (GXTevAlphaArg)0, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)4);
	GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE2, (GXTevColorArg)0xF, (GXTevColorArg)0xF, (GXTevColorArg)0xF, (GXTevColorArg)0);
	GXSetTevAlphaIn(GX_TEVSTAGE2, (GXTevAlphaArg)7, (GXTevAlphaArg)0, (GXTevAlphaArg)1, (GXTevAlphaArg)7);
	if (data->field_0x88) {
		work = mapGetWork();
		if (work->entries[0].flags & 2) {
			mapGetBlend(&blend);
			GXSetNumTevStages(4);
			GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
			GXSetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevColorIn(GX_TEVSTAGE3, (GXTevColorArg)0xF, (GXTevColorArg)0, (GXTevColorArg)4, (GXTevColorArg)0xF);
			GXSetTevAlphaIn(GX_TEVSTAGE3, (GXTevAlphaArg)7, (GXTevAlphaArg)0, (GXTevAlphaArg)2, (GXTevAlphaArg)7);
			GXSetTevColor(GX_TEVREG1, blend);
		}
	}
#ifdef MUST_MATCH
	k = effFireDisp3KColor(data->alpha);
#else
	k = *(GXColor*)&unk_80423060;
	k.a = data->alpha;
#endif
	GXSetTevColor(GX_TEVREG0, k);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	data->field_0x68 = 0x2710;
	data->field_0x70 = -0x2710;
	data->field_0x6C = -0x2710;
	data->field_0x74 = 0x2710;

	f26 = -float_32_8041e118 * float_0p5_8041e104;
	f27 = float_13p333_8041e124;
	colEntry = (u8*)col_tbl + (type - 5) * 0x20;

	for (i = 0; i < 2; i++) {
		switch (i) {
		case 0:
			PSMTXIdentity(scaleMtx);
			break;
		case 1:
			PSMTXTrans(transMtx, float_0_8041e108, float_5_8041e11c, float_1_8041e100);
			PSMTXScale(scaleMtx, float_0p6_8041e120, float_0p6_8041e120, float_0p6_8041e120);
			PSMTXConcat(transMtx, scaleMtx, scaleMtx);
			break;
		}
		PSMTXConcat(mtx, scaleMtx, mtx80);
		PSMTXConcat(camera->view, mtx80, scaleMtx);
		GXLoadPosMtxImm(scaleMtx, 0);
		GXSetCurrentMtx(0);
		GXBegin(GX_QUADS, GX_VTXFMT0, 0xC);
		for (j = 0; j < 3; j++) {
			const u8* c0 = colEntry + j * 4;
			const u8* c1 = colEntry + (j + 1) * 4;
			GXPosition3f32(f26, (f27 * (f32)(j + 1)), 0.0f);
			GXColor4u8(c1[0], c1[1], c1[2], c1[3]);
			GXTexCoord2f32(0.0f, third * (f32)(2 - j));
			GXPosition3f32(sixteen, (f27 * (f32)(j + 1)), 0.0f);
			GXColor4u8(c1[0], c1[1], c1[2], c1[3]);
			GXTexCoord2f32(1.0f, third * (f32)(2 - j));
			GXPosition3f32(sixteen, (f27 * (f32)j), 0.0f);
			GXColor4u8(c0[0], c0[1], c0[2], c0[3]);
			GXTexCoord2f32(1.0f, third * (f32)(3 - j));
			GXPosition3f32(f26, (f27 * (f32)j), 0.0f);
			GXColor4u8(c0[0], c0[1], c0[2], c0[3]);
			GXTexCoord2f32(0.0f, third * (f32)(3 - j));
		}

		{
			Vec tmp;
#ifdef MUST_MATCH
			// Match-only: the volatile read keeps the zero-vector copy inside the loop (with its
			// temp+copy shape) instead of hoisting all twelve loads out of the outer loop.
			tmp = *(volatile Vec*)&vec3_802f8c1c;
#else
			tmp = vec3_802f8c1c;
#endif
			tmp.x = f26;
			tmp.y = float_40_8041e130;
			v = tmp;
		}
		PSMTXMultVec(mtx80, &v, &v);
		getScreenPoint(&v, &screen);
		if (screen.x < (f32)data->field_0x68) data->field_0x68 = (s32)screen.x;
		if (screen.x > (f32)data->field_0x6C) data->field_0x6C = (s32)screen.x;
		if (screen.y > (f32)data->field_0x70) data->field_0x70 = (s32)screen.y;
		if (screen.y < (f32)data->field_0x74) data->field_0x74 = (s32)screen.y;

		{
			Vec tmp;
#ifdef MUST_MATCH
			// Match-only: the volatile read keeps the zero-vector copy inside the loop (with its
			// temp+copy shape) instead of hoisting all twelve loads out of the outer loop.
			tmp = *(volatile Vec*)&vec3_802f8c28;
#else
			tmp = vec3_802f8c28;
#endif
			tmp.x = sixteen;
			tmp.y = float_40_8041e130;
			v = tmp;
		}
		PSMTXMultVec(mtx80, &v, &v);
		getScreenPoint(&v, &screen);
		if (screen.x < (f32)data->field_0x68) data->field_0x68 = (s32)screen.x;
		if (screen.x > (f32)data->field_0x6C) data->field_0x6C = (s32)screen.x;
		if (screen.y > (f32)data->field_0x70) data->field_0x70 = (s32)screen.y;
		if (screen.y < (f32)data->field_0x74) data->field_0x74 = (s32)screen.y;

		{
			Vec tmp;
#ifdef MUST_MATCH
			// Match-only: the volatile read keeps the zero-vector copy inside the loop (with its
			// temp+copy shape) instead of hoisting all twelve loads out of the outer loop.
			tmp = *(volatile Vec*)&vec3_802f8c34;
#else
			tmp = vec3_802f8c34;
#endif
			tmp.x = sixteen;
			v = tmp;
		}
		PSMTXMultVec(mtx80, &v, &v);
		getScreenPoint(&v, &screen);
		if (screen.x < (f32)data->field_0x68) data->field_0x68 = (s32)screen.x;
		if (screen.x > (f32)data->field_0x6C) data->field_0x6C = (s32)screen.x;
		if (screen.y > (f32)data->field_0x70) data->field_0x70 = (s32)screen.y;
		if (screen.y < (f32)data->field_0x74) data->field_0x74 = (s32)screen.y;

		{
			Vec tmp;
#ifdef MUST_MATCH
			// Match-only: the volatile read keeps the zero-vector copy inside the loop (with its
			// temp+copy shape) instead of hoisting all twelve loads out of the outer loop.
			tmp = *(volatile Vec*)&vec3_802f8c40;
#else
			tmp = vec3_802f8c40;
#endif
			tmp.x = f26;
			v = tmp;
		}
		PSMTXMultVec(mtx80, &v, &v);
		getScreenPoint(&v, &screen);
		if (screen.x < (f32)data->field_0x68) data->field_0x68 = (s32)screen.x;
		if (screen.x > (f32)data->field_0x6C) data->field_0x6C = (s32)screen.x;
		if (screen.y > (f32)data->field_0x70) data->field_0x70 = (s32)screen.y;
		if (screen.y < (f32)data->field_0x74) data->field_0x74 = (s32)screen.y;

		colEntry += 0x10;
	}

	data->field_0x68 += 0x130;
	data->field_0x6C += 0x130;
	data->field_0x70 = 0xF0 - data->field_0x70;
	data->field_0x74 = 0xF0 - data->field_0x74;
	if (data->field_0x68 < 0) data->field_0x68 = 0;
	if (data->field_0x6C > 0x260) data->field_0x6C = 0x260;
	if (data->field_0x70 < 0) data->field_0x70 = 0;
	if (data->field_0x74 > 0x1E0) data->field_0x74 = 0x1E0;
	data->field_0x64 = 1;
}

void effFireSmokeMain(EffectEntry* effect) {
	effFireSmokeData* data = effect->userdata;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 0x10;
	}
	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	if (data->lifetime < 16) {
		data->color.a = data->lifetime * 8;
	}
	if (data->lifetime < 0) {
		effDelete(effect);
		return;
	}
	data->counter++;
	data->velocity.x *= float_0p95_8041e10c;
	data->velocity.y *= float_0p95_8041e10c;
	data->velocity.z *= float_0p95_8041e10c;
	data->position.x += data->velocity.x;
	data->position.y += data->velocity.y;
	data->position.z += data->velocity.z;
	data->angle += data->angleVel;
	data->color.r = (u8)(data->color.r * float_0p99_8041e110);
	data->color.g = (u8)(data->color.g * float_0p99_8041e110);
	data->color.b = (u8)(data->color.b * float_0p99_8041e110);
	dispEntry(CAMERA_3D, dispCalcZ(&data->position), 2, effFireSmokeDisp, effect);
}

#ifdef MUST_MATCH
static inline GXColor effFireSmokeColor(effFireSmokeData* data) {
	GXColor c = unk_80423064;
	c.r = data->color.r;
	c.g = data->color.g;
	c.b = data->color.b;
	c.a = data->color.a;
	return c;
}
#endif

// MATCH: writing the scale as / 128.0f (the same value as * 0.0078125f) makes MWCC strength-reduce
// it to the fmuls with the original's FP register colouring (mask f3, quotient f2, constant f1);
// the natural multiply leaves the mask in f2, the result in f1 and the constant in f3.
void effFireSmokeDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effFireSmokeData* data = effect->userdata;
	CameraEntry* camera;
	GXColor color;
	GXTexObj tex;
	Mtx posMtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	Mtx mtx;
	f32 scaleX;
	f32 scaleY;
	f32 scale;
	f32 negHalf;

	camera = camGetPtr(camId);
	if ((u32)data->counter * 2 < 0x3C) {
#ifdef MUST_MATCH
		scaleX = scale_dt[data->counter * 2] / 128.0f;
		scaleY = scale_dt[data->counter * 2 + 1] / 128.0f;
#else
		scaleX = scale_dt[data->counter * 2] * float_0p0078125_8041e0ec;
		scaleY = scale_dt[data->counter * 2 + 1] * float_0p0078125_8041e0ec;
#endif
	} else {
		scaleX = float_1p5_8041e0f0;
		scaleY = float_0p8_8041e0f4;
	}

	effGetTexObj(0x21, &tex);
	GXLoadTexObj(&tex, GX_TEXMAP0);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
#ifdef MUST_MATCH
	color = effFireSmokeColor(data);
#else
	color = unk_80423064;
	color.r = data->color.r;
	color.g = data->color.g;
	color.b = data->color.b;
	color.a = data->color.a;
#endif
	GXSetChanMatColor(GX_COLOR0A0, color);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, 0x7D);

	PSMTXTrans(posMtx, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(rotMtx, 'y', -camGetPtr(camId)->viewYaw * float_deg2rad_8041e0f8);
	scale = float_8_8041e0fc * data->scale;
	PSMTXScale(scaleMtx, scale * scaleX, scale * scaleY, scale);
	PSMTXConcat(posMtx, rotMtx, mtx);
	PSMTXConcat(mtx, scaleMtx, mtx);
	PSMTXRotRad(rotMtx, 'z', float_deg2rad_8041e0f8 * data->angle);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXConcat(camera->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, 0);
	GXSetCurrentMtx(0);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	negHalf = -float_1_8041e100 * float_0p5_8041e104;
	GXPosition3f32(negHalf, float_0p5_8041e104, 0.0f);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3f32(float_0p5_8041e104, float_0p5_8041e104, 0.0f);
	GXTexCoord2f32(1.0f, 0.0f);
	GXPosition3f32(float_0p5_8041e104, negHalf, 0.0f);
	GXTexCoord2f32(1.0f, 1.0f);
	GXPosition3f32(negHalf, negHalf, 0.0f);
	GXTexCoord2f32(0.0f, 1.0f);
}

// Original .sdata2 data pool, defined after the code (so MWCC keeps them as real objects instead of
// folding their values into the functions). Values taken word-for-word from the original object; they are
// the named constants every function in the unit loads.
const u32 dat_8041e0b8 = 0x050A0A0A;
const u32 dat_8041e0bc = 0xFF0000FF;
const u32 dat_8041e0c0 = 0x00000064;
const u32 dat_8041e0c4 = 0xF89928FF;
const u32 dat_8041e0c8 = 0xF19E8FFF;
const u32 dat_8041e0cc = 0x00000064;
const u32 dat_8041e0d0 = 0xFFCD15FF;
const u32 dat_8041e0d4 = 0xFF5100FF;
const u32 dat_8041e0d8 = 0x593359FF;
const u32 dat_8041e0dc = 0x000000FF;
const u32 dat_8041e0e0 = 0xFFEA5D4C;
const u32 dat_8041e0e4 = 0xFFEA5D26;
const u32 dat_8041e0e8 = 0x0000004C;
const f32 float_0p0078125_8041e0ec = 0.0078125f;
const f32 float_1p5_8041e0f0 = 1.5f;
const f32 float_0p8_8041e0f4 = 0.8f;
const f32 float_deg2rad_8041e0f8 = 0.017453292f;
const f32 float_8_8041e0fc = 8.0f;
const f32 float_1_8041e100 = 1.0f;
const f32 float_0p5_8041e104 = 0.5f;
const f32 float_0_8041e108 = 0.0f;
const f32 float_0p95_8041e10c = 0.95f;
const f32 float_0p99_8041e110 = 0.99f;
const f32 float_neg8_8041e114 = -8.0f;
const f32 float_32_8041e118 = 32.0f;
const f32 float_5_8041e11c = 5.0f;
const f32 float_0p6_8041e120 = 0.6f;
const f32 float_13p333_8041e124 = 13.333333f;
const f32 float_0p33333_8041e128 = 0.33333334f;
const f32 float_16_8041e12c = 16.0f;
const f32 float_40_8041e130 = 40.0f;
const f32 float_24_8041e134 = 24.0f;
const f32 float_12_8041e138 = 12.0f;
const f32 float_20_8041e13c = 20.0f;
const f32 float_25_8041e140 = 25.0f;
const f32 float_0p7854_8041e144 = 0.7853982f;
const f32 float_2_8041e148 = 2.0f;
const f32 float_0p03125_8041e14c = 0.03125f;
const f32 float_0p01_8041e150 = 0.01f;
const f32 float_neg0p01_8041e154 = -0.01f;
const f32 float_1p4_8041e158 = 1.4f;
const f32 float_1p2_8041e15c = 1.2f;
const f32 float_10_8041e160 = 10.0f;
const f32 float_30_8041e164 = 30.0f;
const char str_8041E168[] = "Fire";
