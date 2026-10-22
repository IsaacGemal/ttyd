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

// .sbss2 zero-init scratch words, declared in reverse address order because MWCC emits them backwards.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_80423064;
__declspec(section ".sdata") u32 unk_80423060;
__declspec(section ".sdata") u32 unk_8042305c;
__declspec(section ".sdata") u32 unk_80423058;
#pragma section ".sdata" ".sbss"

#ifdef MUST_MATCH
// Match-only: the colours built from the .sbss2 templates above go through inlines that return a GXColor. The
// inline's local and its result take two stack slots below the call arguments' copies, where the original has them;
// a named local built and passed directly sits above the copies, with the argument copy below it. The original
// passes compound literals (`(GXColor){a, a, a, a}`), whose templates are the four .sbss2 words; that form needs
// the words to be the compiler's own anonymous templates instead of the named objects above.
static inline GXColor effFireDisp2Color(GXColor tevColor, u8 a) {
	tevColor.r = a;
	tevColor.g = a;
	tevColor.b = a;
	tevColor.a = a;
	return tevColor;
}

static inline GXColor effFireDisp2KColor(GXColor tevKColor, u8 a) {
	tevKColor.a = a;
	return tevKColor;
}

static inline GXColor effFireDisp3KColor(u8 a) {
	GXColor c = *(GXColor*)&unk_80423060;
	c.a = a;
	return c;
}

static inline GXColor effFireSmokeColor(effFireSmokeData* data) {
	GXColor c = unk_80423064;
	c.r = data->color.r;
	c.g = data->color.g;
	c.b = data->color.b;
	c.a = data->color.a;
	return c;
}
#endif

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
extern const u32 dat_8041e0b8;


ScaleEntry scale1[13] = {
	{ 0x00, 1.0f }, { 0x14, 1.1f }, { 0x1E, 1.0f }, { 0x2D, 1.05f },
	{ 0x3C, 0.95f }, { 0x46, 1.05f }, { 0x50, 1.025f }, { 0x64, 1.1f },
	{ 0x78, 1.0f }, { 0x82, 1.025f }, { 0xA0, 0.95f }, { 0xB4, 1.025f },
	{ 0xC8, 1.0f },
};
ScaleEntry scale2[4] = {
	{ 0x00, 1.0f }, { 0x64, 1.2f }, { 0xA0, 0.95f }, { 0xC8, 1.0f },
};

static u32 col_tbl[24] = {
	0xAC2E00FF, 0xE02200FF, 0xFD5F00FF, 0xFD5F04FF,
	0xFF6900FF, 0xFF8A00FF, 0xFFB700FF, 0xFFD700FF,
	0x0AE0FFFF, 0x10C3FFFF, 0x0961FFFF, 0x0961FFFF,
	0xD1F9FFFF, 0xD1F9FFFF, 0xA3E5FFFF, 0x3ACAFFFF,
	0x5BF63DFF, 0x3BDE1CFF, 0x3BDE1CFF, 0x1DB900FF,
	0xDEF618FF, 0xA0F606FF, 0xA0F606FF, 0x92E400FF,
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

#ifdef MUST_MATCH
// Match-only: these two immediately generated helpers pool the u32->f32 conversion double and the
// "FireSmoke" string before the deferred functions, in the original's .rodata order (mask double, string,
// signed double). Nothing calls them; the linker strips the unused bodies.
#pragma defer_codegen off
static f32 effFire_pool_u2f(u32 v) {
	return (f32)v;
}

static const char* effFire_pool_str(void) {
	return "FireSmoke";
}
#pragma defer_codegen reset
#endif

EffectEntry* effFireEntry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry;
	effFireData* data;
	Vec position;

	entry = effEntry();
	entry->type = "Fire";
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
	position = vec3_802f8c10;
	position.x = x;
	position.y = y;
	position.z = z;
	data->position = position;
	data->scale = scale;
	data->lifetime = lifetime;
	data->counter = 0;
	data->scaleX = 1.0f;
	data->scaleY = 1.0f;
	data->sparkTimer = 0;
	data->sparkInterval = rand() % 30 + 30;
	data->colorIndex1 = 0;
	data->colorIndex2 = 0;
	data->field_0x4C = 0.0f;
	data->field_0x50 = 0.0f;
	data->field_0x54 = 0.0f;
	data->field_0x58 = 0.0f;
	data->field_0x5C = 0.0f;
	data->field_0x60 = 0.0f;

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
	data->curScale = 0.0f;
	data->field_0x80 = 0;
	data->field_0x84 = 0;
	data->useMapBlend = 0;
	return entry;
}

void effFireMain(EffectEntry* effect) {
	effFireData* data;
	s32 type;
	s32 tableCount;
	const ScaleEntry* scaleTable;
	f32 interp;
	// Non-const scale factors preserve the original FP register allocation.
	f32 stretch = 1.4f;
	f32 widen = 1.2f;
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
		data->frameBlend = 0x80;
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
			scaleTable = scale1;
			tableCount = 13;
			break;
		case 4:
			scaleTable = scale2;
			tableCount = 4;
			break;
		}
		for (;;) {
			if (data->scaleCounter < scaleTable[data->scaleIndex].threshold) {
				break;
			}
			data->scaleIndex++;
			if (data->scaleIndex >= tableCount) {
				data->scaleIndex = 0;
				data->scaleCounter = 0;
			}
		}
		interp = (f32)(scaleTable[data->scaleIndex].threshold - data->scaleCounter) /
			(f32)(scaleTable[data->scaleIndex].threshold - scaleTable[data->scaleIndex - 1].threshold);
		data->curScale = (1.0f - interp) * scaleTable[data->scaleIndex].scale +
			interp * scaleTable[data->scaleIndex - 1].scale;
		data->curScale = data->curScale * stretch;
		data->curScale = data->curScale * widen;
		data->scaleCounter++;
		break;
	case 5:
	case 6:
	case 7:
		data->sparkTimer++;
		if (data->sparkTimer > data->sparkInterval) {
			effFireSparkN64Entry(type, 2, 200, data->position.x, data->position.y + 10.f * data->scale,
				data->position.z, 30.f * data->scaleX, 30.f,
				data->scale, 1.0f, 1.0f);
			data->sparkTimer = 0;
			data->sparkInterval = rand() % 30 + 30;
		}
		break;
	default:
		data->sparkTimer++;
		if (data->sparkTimer > data->sparkInterval) {
			// Per-type vertical spawn offsets packed into one word.
			u32 packedYOffsets = dat_8041e0b8;
			// px/py/pz/sc are declared uninitialised and assigned in the order pz, sc, px, py, which colours
			// pz=f29 and px=f31; declaring them with initialisers in this order gives pz=f31 and px=f29.
			f32 px;
			f32 py;
			f32 pz;
			f32 sc;
			Vec smokePos;
			effFireSmokeData* smoke;
			pz = data->position.z - 5.0f;
			sc = data->scale;
			px = data->position.x;
			py = data->position.y + (f32)((u8*)&packedYOffsets)[type];

			smokeEntry = effEntry();
			smokeEntry->type = "FireSmoke";
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
			smoke->velocity.x = 0.0f;
			smoke->velocity.y = 1.0f;
			smoke->velocity.z = 0.0f;
			smoke->angleVel = -8.0f;
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
	s32 frameCount;
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
		frameCount = 0xC;
		texWidth = 0x20;
		texHeight = 0x20;
		break;
	case 1:
		effGetTexObj(0x46, &tex);
		GXLoadTexObj(&tex, GX_TEXMAP0);
		frameCount = 4;
		texWidth = 0x20;
		texHeight = 0x28;
		break;
	case 2:
		effGetTexObj(0x47, &tex);
		GXLoadTexObj(&tex, GX_TEXMAP0);
		frameCount = 0xA;
		texWidth = 0x18;
		texHeight = 0x28;
		break;
	default:
		break;
	}
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);

	frame = (data->counter >> 1) % frameCount;
	PSMTXScale(scaleMtx, 1.0f / (f32)frameCount, 1.0f, 1.0f);
	PSMTXTrans(transMtx, (f32)frame, 0.0f, 0.0f);
	PSMTXConcat(scaleMtx, transMtx, mtx);
	GXLoadTexMtxImm(mtx, GX_TEXMTX0, GX_MTX2x4);

	PSMTXTrans(transMtx, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(rotMtx, 'y', -camGetPtr(camId)->viewYaw * 0.017453292f);
	scale = 20.f * data->scale;
	scale *= 0.03125f;
	PSMTXScale(scaleMtx, scale * data->scaleX, scale * data->scaleY, scale);
	PSMTXConcat(transMtx, rotMtx, mtx);
	PSMTXConcat(mtx, scaleMtx, mtx);
	PSMTXConcat(camera->view, mtx, mtx);
	GXSetCurrentMtx(GX_PNMTX0);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	PSMTXTrans(offsetMtx, 0.01f, -0.01f, 0.0f);
	PSMTXConcat(mtx, offsetMtx, offsetMtx);
	GXLoadPosMtxImm(offsetMtx, GX_PNMTX0);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_COMP_A8_GT, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_KONST);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_TEXA, GX_CA_A0, GX_CA_KONST, GX_CA_ZERO);
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
	GXLoadPosMtxImm(mtx, GX_PNMTX0);
	GXSetNumTevStages(4);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ONE, GX_CC_KONST, GX_CC_RASC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
	GXSetTevKColor(GX_KCOLOR0, *(GXColor*)&dat_8041e0c4);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ONE, GX_CC_KONST, GX_CC_RASC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GXSetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K1);
	GXSetTevKColor(GX_KCOLOR1, *(GXColor*)&dat_8041e0c8);
	GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_COMP_A8_GT, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
	GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_TEXA, GX_CA_A2, GX_CA_KONST, GX_CA_ZERO);
	GXSetTevKAlphaSel(GX_TEVSTAGE2, GX_TEV_KASEL_1);
	GXSetTevColor(GX_TEVREG2, *(GXColor*)&dat_8041e0cc);
	GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE3, GX_CC_C1, GX_CC_CPREV, GX_CC_APREV, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_TEXA, GX_CA_ZERO, GX_CA_APREV, GX_CA_APREV);
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

void effFireDisp2(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effFireData* data = effect->userdata;
	CameraEntry* camera;
	GXTexObj tex;
	Mtx transMtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	Mtx mtx;
	GXColor tevColor;
	GXColor tevKColor;
	s32 type;
	f32 negHalf;
	// Non-const dimensions retain runtime negation and the cached quad coordinates.
	f32 width = 24.0f;
	f32 half = 0.5f;
	f32 halfWidth = 12.0f;
	f32 quadSize = 32.0f;
	f32 height = 40.0f;
	f32 halfSize = 16.0f;

	camera = camGetPtr(camId);
	type = data->type;
	effGetTexObj(0x48, &tex);
	GXLoadTexObj(&tex, GX_TEXMAP0);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);

	PSMTXScale(scaleMtx, 1.0f / (f32)data->frameMax, 1.0f, 1.0f);
	PSMTXTrans(transMtx, (f32)data->colorIndex1, 0.0f, 0.0f);
	PSMTXConcat(scaleMtx, transMtx, transMtx);
	GXLoadTexMtxImm(transMtx, GX_TEXMTX0, GX_MTX2x4);

	PSMTXScale(scaleMtx, 1.0f / (f32)data->frameMax, 1.0f, 1.0f);
	PSMTXTrans(transMtx, (f32)data->colorIndex2, 0.0f, 0.0f);
	PSMTXConcat(scaleMtx, transMtx, transMtx);
	GXLoadTexMtxImm(transMtx, GX_TEXMTX1, GX_MTX2x4);

	PSMTXTrans(transMtx, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(rotMtx, 'y', -camGetPtr(camId)->viewYaw * 0.017453292f);
	PSMTXScale(scaleMtx, data->scale * data->scaleX, data->scale * data->scaleY, data->scale);
	PSMTXConcat(transMtx, rotMtx, mtx);
	PSMTXConcat(mtx, scaleMtx, mtx);
	PSMTXConcat(camera->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
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
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C1, GX_CC_C0, GX_CC_RASC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
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
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_APREV, GX_CA_TEXA, GX_CA_A2, GX_CA_ZERO);
	tevColor = *(GXColor*)&unk_80423058;
#ifdef MUST_MATCH
	GXSetTevColor(GX_TEVREG2, effFireDisp2Color(tevColor, data->frameBlend));
#else
	tevColor.r = data->frameBlend;
	tevColor.g = data->frameBlend;
	tevColor.b = data->frameBlend;
	tevColor.a = data->frameBlend;
	GXSetTevColor(GX_TEVREG2, tevColor);
#endif
	GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
	GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_KONST, GX_CA_ZERO);
	GXSetTevKAlphaSel(GX_TEVSTAGE2, GX_TEV_KASEL_K0_A);
	tevKColor = *(GXColor*)&unk_8042305c;
#ifdef MUST_MATCH
	GXSetTevKColor(GX_KCOLOR0, effFireDisp2KColor(tevKColor, data->alpha));
#else
	tevKColor.a = data->alpha;
	GXSetTevKColor(GX_KCOLOR0, tevKColor);
#endif

	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	negHalf = -width * half;
	GXPosition3f32(negHalf, height, 0.0f);
	GXColor4u8(0xFF, 0xFF, 0xFF, 0xFF);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3f32(halfWidth, height, 0.0f);
	GXColor4u8(0xFF, 0xFF, 0xFF, 0xFF);
	GXTexCoord2f32(1.0f, 0.0f);
	GXPosition3f32(halfWidth, 0.0f, 0.0f);
	GXColor4u8(0, 0, 0, 0xFF);
	GXTexCoord2f32(1.0f, 1.0f);
	GXPosition3f32(negHalf, 0.0f, 0.0f);
	GXColor4u8(0, 0, 0, 0xFF);
	GXTexCoord2f32(0.0f, 1.0f);

	switch (type) {
	case 3:
	case 8:
		PSMTXTrans(transMtx, 0.0f, 20.f, 0.0f);
		break;
	case 4:
		PSMTXTrans(transMtx, 0.0f, 25.0f, 0.0f);
		break;
	}
	PSMTXScale(scaleMtx, data->curScale, data->curScale, data->curScale);
	PSMTXConcat(transMtx, scaleMtx, transMtx);
	if (type == 4) {
		PSMTXRotRad(rotMtx, 'z', 0.7853982f);
		PSMTXConcat(transMtx, rotMtx, transMtx);
	}
	PSMTXConcat(mtx, transMtx, transMtx);
	GXLoadPosMtxImm(transMtx, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
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
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
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
		f32 maxU;
		f32 maxV;
		f32 negHalfSize;
		if (type == 3 || type == 8) {
			maxU = 2.0f;
			maxV = 1.0f;
		} else {
			maxU = 2.0f;
			maxV = maxU;
		}
		negHalfSize = -quadSize * half;
		GXPosition3f32(negHalfSize, halfSize, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(halfSize, halfSize, 0.0f);
		GXTexCoord2f32(maxU, 0.0f);
		GXPosition3f32(halfSize, negHalfSize, 0.0f);
		GXTexCoord2f32(maxU, maxV);
		GXPosition3f32(negHalfSize, negHalfSize, 0.0f);
		GXTexCoord2f32(0.0f, maxV);
	}
}

// effFireDisp3 source-shape notes (all needed for the byte match):
//  - `data` is declared after `type` and assigned just before `camGetPtr`, with `type = data->type` after
//    the call; that gives the target's data r26 / type r27.
//  - `third` and `halfWidth` are f32 locals so MWCC caches them in f29/f30, and the inner-loop
//    GXPosition3f32/GXTexCoord2f32 y and texcoord values are inline expressions rather than hoisted locals.
//  - the four zero-vector copies below are MUST_MATCH volatile reads: without them MWCC hoists all twelve
//    loads out of the outer loop, and without the explicit temp the frame loses 0x30.
//  - vec3_802f8c10 is a non-const object forced into .rodata so effFireEntry copies it with integer
//    lwz/stw as the target does (a const object is folded into lfsu).

void effFireDisp3(CameraId camId, void* param) {
	EffectEntry* effect = param;
	CameraEntry* camera;
	MapWork* work;
	GXTexObj tex;
	Mtx transMtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	Mtx mtx;
	Mtx modelMtx;
	GXColor blend;
	GXColor tevColor;
	Vec v;
	Vec screen;
	const GXColor* colorEntry;
	s32 type;
	effFireData* data;
	s32 i;
	s32 j;
	f32 rowHeight;
	f32 leftX;
	// Non-const dimensions retain runtime negation and the original loop FP registers.
	f32 width = 32.0f;
	f32 half = 0.5f;
	f32 third = 0.33333334f;
	f32 halfWidth = 16.0f;

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
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);

	PSMTXScale(scaleMtx, 1.0f / (f32)data->frameMax, 1.0f, 1.0f);
	PSMTXTrans(transMtx, (f32)data->colorIndex1, 0.0f, 0.0f);
	PSMTXConcat(scaleMtx, transMtx, transMtx);
	GXLoadTexMtxImm(transMtx, GX_TEXMTX0, GX_MTX2x4);

	PSMTXScale(scaleMtx, 1.0f / (f32)data->frameMax, 1.0f, 1.0f);
	PSMTXTrans(transMtx, (f32)data->colorIndex2, 0.0f, 0.0f);
	PSMTXConcat(scaleMtx, transMtx, transMtx);
	GXLoadTexMtxImm(transMtx, GX_TEXMTX1, GX_MTX2x4);

	PSMTXTrans(transMtx, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(rotMtx, 'y', -camGetPtr(camId)->viewYaw * 0.017453292f);
	PSMTXScale(scaleMtx, data->scale * data->scaleX, data->scale * data->scaleY, data->scale);
	PSMTXConcat(transMtx, rotMtx, mtx);
	PSMTXConcat(mtx, scaleMtx, mtx);
	GXSetNumTevStages(3);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_DIVIDE_2, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_APREV, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
	GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
	GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);
	if (data->useMapBlend) {
		work = mapGetWork();
		if (work->entries[0].flags & 2) {
			mapGetBlend(&blend);
			GXSetNumTevStages(4);
			GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
			GXSetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevColorIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_CPREV, GX_CC_C1, GX_CC_ZERO);
			GXSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_APREV, GX_CA_A1, GX_CA_ZERO);
			GXSetTevColor(GX_TEVREG1, blend);
		}
	}
#ifdef MUST_MATCH
	tevColor = effFireDisp3KColor(data->alpha);
#else
	tevColor = *(GXColor*)&unk_80423060;
	tevColor.a = data->alpha;
#endif
	GXSetTevColor(GX_TEVREG0, tevColor);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	// Accumulate the screen-space bounds of both quads.
	data->screenLeft = 0x2710;
	data->screenTop = -0x2710;
	data->screenRight = -0x2710;
	data->screenBottom = 0x2710;

	leftX = -width * half;
	rowHeight = 13.333333f;
	colorEntry = (const GXColor*)&col_tbl[(type - 5) * 8];

	for (i = 0; i < 2; i++) {
		switch (i) {
		case 0:
			PSMTXIdentity(scaleMtx);
			break;
		case 1:
			PSMTXTrans(transMtx, 0.0f, 5.0f, 1.0f);
			PSMTXScale(scaleMtx, 0.6f, 0.6f, 0.6f);
			PSMTXConcat(transMtx, scaleMtx, scaleMtx);
			break;
		}
		PSMTXConcat(mtx, scaleMtx, modelMtx);
		PSMTXConcat(camera->view, modelMtx, scaleMtx);
		GXLoadPosMtxImm(scaleMtx, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		GXBegin(GX_QUADS, GX_VTXFMT0, 0xC);
		for (j = 0; j < 3; j++) {
			const GXColor* color0 = colorEntry + j;
			const GXColor* color1 = colorEntry + (j + 1);
			GXPosition3f32(leftX, (rowHeight * (f32)(j + 1)), 0.0f);
			GXColor4u8(color1->r, color1->g, color1->b, color1->a);
			GXTexCoord2f32(0.0f, third * (f32)(2 - j));
			GXPosition3f32(halfWidth, (rowHeight * (f32)(j + 1)), 0.0f);
			GXColor4u8(color1->r, color1->g, color1->b, color1->a);
			GXTexCoord2f32(1.0f, third * (f32)(2 - j));
			GXPosition3f32(halfWidth, (rowHeight * (f32)j), 0.0f);
			GXColor4u8(color0->r, color0->g, color0->b, color0->a);
			GXTexCoord2f32(1.0f, third * (f32)(3 - j));
			GXPosition3f32(leftX, (rowHeight * (f32)j), 0.0f);
			GXColor4u8(color0->r, color0->g, color0->b, color0->a);
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
			tmp.x = leftX;
			tmp.y = 40.f;
			v = tmp;
		}
		PSMTXMultVec(modelMtx, &v, &v);
		getScreenPoint(&v, &screen);
		if (screen.x < (f32)data->screenLeft) data->screenLeft = (s32)screen.x;
		if (screen.x > (f32)data->screenRight) data->screenRight = (s32)screen.x;
		if (screen.y > (f32)data->screenTop) data->screenTop = (s32)screen.y;
		if (screen.y < (f32)data->screenBottom) data->screenBottom = (s32)screen.y;

		{
			Vec tmp;
#ifdef MUST_MATCH
			// Match-only: the volatile read keeps the zero-vector copy inside the loop (with its
			// temp+copy shape) instead of hoisting all twelve loads out of the outer loop.
			tmp = *(volatile Vec*)&vec3_802f8c28;
#else
			tmp = vec3_802f8c28;
#endif
			tmp.x = halfWidth;
			tmp.y = 40.f;
			v = tmp;
		}
		PSMTXMultVec(modelMtx, &v, &v);
		getScreenPoint(&v, &screen);
		if (screen.x < (f32)data->screenLeft) data->screenLeft = (s32)screen.x;
		if (screen.x > (f32)data->screenRight) data->screenRight = (s32)screen.x;
		if (screen.y > (f32)data->screenTop) data->screenTop = (s32)screen.y;
		if (screen.y < (f32)data->screenBottom) data->screenBottom = (s32)screen.y;

		{
			Vec tmp;
#ifdef MUST_MATCH
			// Match-only: the volatile read keeps the zero-vector copy inside the loop (with its
			// temp+copy shape) instead of hoisting all twelve loads out of the outer loop.
			tmp = *(volatile Vec*)&vec3_802f8c34;
#else
			tmp = vec3_802f8c34;
#endif
			tmp.x = halfWidth;
			v = tmp;
		}
		PSMTXMultVec(modelMtx, &v, &v);
		getScreenPoint(&v, &screen);
		if (screen.x < (f32)data->screenLeft) data->screenLeft = (s32)screen.x;
		if (screen.x > (f32)data->screenRight) data->screenRight = (s32)screen.x;
		if (screen.y > (f32)data->screenTop) data->screenTop = (s32)screen.y;
		if (screen.y < (f32)data->screenBottom) data->screenBottom = (s32)screen.y;

		{
			Vec tmp;
#ifdef MUST_MATCH
			// Match-only: the volatile read keeps the zero-vector copy inside the loop (with its
			// temp+copy shape) instead of hoisting all twelve loads out of the outer loop.
			tmp = *(volatile Vec*)&vec3_802f8c40;
#else
			tmp = vec3_802f8c40;
#endif
			tmp.x = leftX;
			v = tmp;
		}
		PSMTXMultVec(modelMtx, &v, &v);
		getScreenPoint(&v, &screen);
		if (screen.x < (f32)data->screenLeft) data->screenLeft = (s32)screen.x;
		if (screen.x > (f32)data->screenRight) data->screenRight = (s32)screen.x;
		if (screen.y > (f32)data->screenTop) data->screenTop = (s32)screen.y;
		if (screen.y < (f32)data->screenBottom) data->screenBottom = (s32)screen.y;

		colorEntry += 4;
	}

	data->screenLeft += 0x130;
	data->screenRight += 0x130;
	data->screenTop = 0xF0 - data->screenTop;
	data->screenBottom = 0xF0 - data->screenBottom;
	if (data->screenLeft < 0) data->screenLeft = 0;
	if (data->screenRight > 0x260) data->screenRight = 0x260;
	if (data->screenTop < 0) data->screenTop = 0;
	if (data->screenBottom > 0x1E0) data->screenBottom = 0x1E0;
	data->field_0x64 = 1;
}

#ifdef MUST_MATCH
// Match-only: deferred emission pools the smoke angular velocity after SmokeMain and before Disp3.
// Main shares this anonymous literal; the linker strips the unused helper body.
static f32 effFire_pool_smoke_speed(void) {
	return -8.0f;
}
#endif

void effFireSmokeMain(EffectEntry* effect) {
	effFireSmokeData* data = effect->userdata;
	// The non-const fade factor keeps the conversion double and multiplier in their original registers.
	f32 fade = 0.99f;

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
	data->velocity.x *= 0.95f;
	data->velocity.y *= 0.95f;
	data->velocity.z *= 0.95f;
	data->position.x += data->velocity.x;
	data->position.y += data->velocity.y;
	data->position.z += data->velocity.z;
	data->angle += data->angleVel;
	data->color.r = (u8)(data->color.r * fade);
	data->color.g = (u8)(data->color.g * fade);
	data->color.b = (u8)(data->color.b * fade);
	dispEntry(CAMERA_3D, dispCalcZ(&data->position), 2, effFireSmokeDisp, effect);
}

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
	// Non-const factors retain the runtime negation and cached half-size coordinate.
	f32 one = 1.0f;
	f32 half = 0.5f;

	camera = camGetPtr(camId);
	if ((u32)data->counter * 2 < 0x3C) {
		scaleX = scale_dt[data->counter * 2] / 128.0f;
		scaleY = scale_dt[data->counter * 2 + 1] / 128.0f;
	} else {
		scaleX = 1.5f;
		scaleY = 0.8f;
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
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);

	PSMTXTrans(posMtx, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(rotMtx, 'y', -camGetPtr(camId)->viewYaw * 0.017453292f);
	scale = 8.0f * data->scale;
	PSMTXScale(scaleMtx, scale * scaleX, scale * scaleY, scale);
	PSMTXConcat(posMtx, rotMtx, mtx);
	PSMTXConcat(mtx, scaleMtx, mtx);
	PSMTXRotRad(rotMtx, 'z', 0.017453292f * data->angle);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXConcat(camera->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	negHalf = -one * half;
	GXPosition3f32(negHalf, half, 0.0f);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3f32(half, half, 0.0f);
	GXTexCoord2f32(1.0f, 0.0f);
	GXPosition3f32(half, negHalf, 0.0f);
	GXTexCoord2f32(1.0f, 1.0f);
	GXPosition3f32(negHalf, negHalf, 0.0f);
	GXTexCoord2f32(0.0f, 1.0f);
}

// Named color and offset words precede the anonymous float/string pool. Define them after the code
// so MWCC preserves the objects instead of folding their contents into the functions.
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
