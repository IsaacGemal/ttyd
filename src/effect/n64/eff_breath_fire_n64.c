#include "eff/n64/eff_breath_fire_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "eff/eff_n64.h"

// .sbss2: the default .sbss2 is only used for compiler-generated pool templates, so pair it with a small-data section.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_804230b4;
__declspec(section ".sdata") GXColor unk_804230b0;


#include <dolphin/gx.h>
#include <dolphin/mtx.h>

u32 stg_fire_breath_v[35] = {
    0x0064FF9C, 0x00000C00, 0x00000000, 0x00FF0064,
    0x00640000, 0x0C000400, 0x000000FF, 0x00000000,
    0x00000A00, 0x0200FFFF, 0xFFFFFF9C, 0x00640000,
    0x08000400, 0x000000FF, 0xFF9CFF9C, 0x00000800,
    0x00000000, 0x00FF0064, 0xFF9C0005, 0x04000000,
    0x000000FF, 0x00640064, 0x00050400, 0x04000000,
    0x00FF0000, 0x00000005, 0x02000200, 0xFFFFFFFF,
    0xFF9C0064, 0x00050000, 0x04000000, 0x00FFFF9C,
    0xFF9C0005, 0x00000000, 0x000000FF,
};

extern const f32 float_256_8041ea80;
extern const f32 float_0p0078125_8041ea84;
extern const f32 float_0p03125_8041ea88;
extern const f32 float_0_8041ea8c;
extern const f32 float_deg2rad_8041ea90;
extern const f32 float_4_8041ea94;
extern const f32 float_10_8041ea98;

// .rodata zero vector, int->float and mode-1 doubles are compiler-generated pool constants here.

// .sbss2 zero GXColor locals the original copies before filling its fields.

// .data vertex array of this unit; the original hands a 0x46-byte slice to effSetVtxDescN64.

EffectEntry* effBreathFireN64Entry(s32 kind, f32 x, f32 y, s32 children, f32 z, s32 seed, f32 endX, s32 duration, f32 endY, f32 endZ) {
	EffectEntry* entry;
	effBreathFireData* data;

	entry = effEntry();
	entry->type = "BreathFireN64";
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effBreathFireData));
	entry->userdata = data;
	entry->callback = effBreathFireMain;
	data->kind = kind;
	data->childrenLeft = children;
	data->seed = seed;
	data->posX = x;
	data->posY = y;
	data->posZ = z;
	data->startX = x;
	data->startY = y;
	data->startZ = z;
	data->endX = endX;
	data->endY = endY;
	data->endZ = endZ;
	if (kind == 0) {
		data->scale = 0.05f;
	}
	else {
		data->scale = 0.040000003f;
	}
	data->baseScale = data->scale;
	data->duration = duration;
	data->life = duration;
	data->age = 0;
	data->alpha = 0xFF;
	data->texPos = float_0_8041ea8c;
	data->scaleRate = 0.1f;
	data->endScale = data->baseScale;
	if (kind == 0) {
		data->bendX = 0.2f * (endX - x) * (f32)(s32)(effTblRandN64(10, seed) - 5) * 0.2f;
		data->bendY = 0.2f * (endY - y) * (f32)(s32)(effTblRandN64(10, seed + 1) - 5) * 0.2f;
		data->bendZ = 0.2f * (endZ - z) * (f32)(s32)(effTblRandN64(10, seed + 2) - 5) * 0.2f;
	}
	else {
		data->bendX = 0.2f * (endX - x) * (f32)(s32)(effTblRandN64(10, seed + 3) - 5);
		data->bendY = 0.2f * (endY - y) * (f32)(s32)(effTblRandN64(10, seed + 4) - 5);
		data->bendZ = 0.2f * (endZ - z) * (f32)(s32)(effTblRandN64(10, seed + 5) - 5);
	}
	data->rise = float_0_8041ea8c;
	data->color0R = 0xFF;
	data->color0G = 0xAA;
	data->color0B = 0x2A;
	data->color1R = 0xF3;
	data->color1G = 0x30;
	data->color1B = 0;

	return entry;
}

inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

// Matching notes: the zero vector, the int->float magic and the two mode-1 doubles are all compiler-
// generated anonymous pool constants here, which puts .rodata exactly at vec+0, magic+0x10, 0.008+0x18,
// 0.3+0x20 (the given named definitions emitted first and shifted every base+offset load).
#pragma opt_common_subs off
void effBreathFireMain(EffectEntry* effect) {
	s32 life;
	effBreathFireData* data = effect->userdata;
	s32 total;
	s32 timer;
	Vec position = initializeVec((Vec*)&data->posX);
	EffectEntry* newEffect;

	data->texPos = float_4_8041ea94 * (f32)data->age / float_10_8041ea98;
	data->life--;
	data->age++;
	life = data->life;
	if (life < 0) {
		effDelete(effect);
		return;
	}
	timer = data->age;
	total = data->duration;
	if (life > 5 && data->kind == 0) {
		data->scale = data->scale + 0.05f * (2.5f - data->scale);
	}
	data->posX = data->startX + (f32)timer * (data->bendX + (data->endX - data->startX)) / (f32)total;
	data->posY = data->startY + (f32)timer * (data->bendY + (data->endY - data->startY)) / (f32)total;
	data->posZ = data->startZ + (f32)timer * (data->bendZ + (data->endZ - data->startZ)) / (f32)total;
	if (data->kind == 1) {
		data->rise = data->rise + 0.01f * (f32)timer;
		data->posY = data->posY + data->rise;
	}
	if (timer == data->seed + 1 && data->childrenLeft > 0) {
		newEffect = effBreathFireN64Entry(data->kind, data->startX, data->startY, data->childrenLeft - 1, data->startZ, data->seed, data->endX, total, data->endY, data->endZ);
		((effBreathFireData*)newEffect->userdata)->color0R = data->color0R;
		((effBreathFireData*)newEffect->userdata)->color0G = data->color0G;
		((effBreathFireData*)newEffect->userdata)->color0B = data->color0B;
		((effBreathFireData*)newEffect->userdata)->color1R = data->color1R;
		((effBreathFireData*)newEffect->userdata)->color1G = data->color1G;
		((effBreathFireData*)newEffect->userdata)->color1B = data->color1B;
		((effBreathFireData*)newEffect->userdata)->baseScale = ((effBreathFireData*)newEffect->userdata)->scale = data->baseScale;
		((effBreathFireData*)newEffect->userdata)->endScale = data->endScale;
		((effBreathFireData*)newEffect->userdata)->scaleRate = data->scaleRate;
	}
	if (life < 10 && data->kind == 0) {
		data->alpha = life * 25;
	}
	if (data->kind == 1) {
		data->alpha = life * 224 / total;
		data->scale = 0.008f * (0.3 - data->scale) + data->scale;
	}
	if (data->kind == 2) {
		data->alpha = life * 224 / total;
		data->scale = data->scaleRate * (data->endScale - data->scale) + data->scale;
	}
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effBreathFireDisp, effect);
}

static inline void breathSetColor0(effBreathFireData* data) {
	GXColor color = unk_804230b0;
	color.r = data->color0R;
	color.g = data->color0G;
	color.b = data->color0B;
	color.a = data->alpha;
	GXSetTevColor(GX_TEVREG0, color);
}

static inline void breathSetColor1(effBreathFireData* data, f32 s, s32 frame) {
	GXColor color = unk_804230b4;
	color.r = data->color1R;
	color.g = data->color1G;
	color.b = data->color1B;
	color.a = (u8)(float_256_8041ea80 * (s - (f32)frame));
	GXSetTevColor(GX_TEVREG1, color);
}

void effBreathFireDisp(CameraId camId, void* param) {
	EffectEntry* effect;
	effBreathFireData* data;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx trans;
	Mtx scale;
	Mtx rot;
	s32 frame;
	s32 frame2;
	f32 s;

	effect = param;
	cam = camGetPtr(camId);
	data = effect->userdata;
	s = data->texPos;
	frame = (s32)s;

	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTevStages(2);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C1, GX_CC_C0, GX_CC_RASC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_APREV, GX_CA_TEXA, GX_CA_A1, GX_CA_ZERO);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);

	breathSetColor0(data);
	breathSetColor1(data, s, frame);

	effGetTexObjN64(0x77, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	effGetTexObjN64(0x77, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP1);

	frame2 = frame << 5;
	PSMTXScale(scale, float_0p0078125_8041ea84, float_0p03125_8041ea88, float_0_8041ea8c);
	PSMTXTrans(trans, (f32)frame2, float_0_8041ea8c, float_0_8041ea8c);
	PSMTXConcat(scale, trans, scale);
	GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
	PSMTXScale(scale, float_0p0078125_8041ea84, float_0p03125_8041ea88, float_0_8041ea8c);
	PSMTXTrans(trans, (f32)(frame2 + 0x20), float_0_8041ea8c, float_0_8041ea8c);
	PSMTXConcat(scale, trans, scale);
	GXLoadTexMtxImm(scale, GX_TEXMTX1, GX_MTX2x4);

	PSMTXTrans(trans, data->posX, data->posY, data->posZ);
	PSMTXRotRad(rot, 'y', float_deg2rad_8041ea90 * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXScale(scale, data->scale, data->scale, float_0_8041ea8c);
	PSMTXConcat(trans, rot, trans);
	PSMTXConcat(trans, scale, trans);
	PSMTXConcat(cam->view, trans, trans);
	GXLoadPosMtxImm(trans, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
	GXSetCullMode(GX_CULL_NONE);

	effSetVtxDescN64((effVtxArray*)((u8*)stg_fire_breath_v + 0x46));
	GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0xC);
	tri2(0, 1, 2, 0, 1, 3, 2, 0);
	tri2(4, 2, 3, 0, 4, 0, 2, 0);
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_256_8041ea80 = 256.0f;
const f32 float_0p0078125_8041ea84 = 0.0078125f;
const f32 float_0p03125_8041ea88 = 0.03125f;
const f32 float_0_8041ea8c = 0.0f;
const f32 float_deg2rad_8041ea90 = 0.017453292f;
const f32 float_4_8041ea94 = 4.0f;
const f32 float_10_8041ea98 = 10.0f;
