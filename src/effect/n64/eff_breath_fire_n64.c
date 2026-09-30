#include "eff/n64/eff_breath_fire_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "eff/eff_n64.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

typedef struct effBreathFireData {
	s32 field_0x00; //0x00
	s32 field_0x04; //0x04
	s32 field_0x08; //0x08
	f32 field_0x0C; //0x0C
	f32 field_0x10; //0x10
	f32 field_0x14; //0x14
	f32 field_0x18; //0x18
	f32 field_0x1C; //0x1C
	f32 field_0x20; //0x20
	f32 field_0x24; //0x24
	f32 field_0x28; //0x28
	f32 field_0x2C; //0x2C
	f32 field_0x30; //0x30
	f32 field_0x34; //0x34
	f32 field_0x38; //0x38
	f32 field_0x3C; //0x3C
	u32 field_0x40; //0x40
	s32 field_0x44; //0x44
	s32 field_0x48; //0x48
	s32 field_0x4C; //0x4C
	f32 field_0x50; //0x50
	f32 field_0x54; //0x54
	f32 field_0x58; //0x58
	f32 field_0x5C; //0x5C
	f32 field_0x60; //0x60
	u32 field_0x64; //0x64
	u32 field_0x68; //0x68
	u32 field_0x6C; //0x6C
	u32 field_0x70; //0x70
	u32 field_0x74; //0x74
	u32 field_0x78; //0x78
} effBreathFireData;

extern const f32 float_256_8041ea80;
extern const f32 float_0p0078125_8041ea84;
extern const f32 float_0p03125_8041ea88;
extern const f32 float_0_8041ea8c;
extern const f32 float_deg2rad_8041ea90;
extern const f32 float_4_8041ea94;
extern const f32 float_10_8041ea98;
extern const f32 float_0p05_8041ea9c;
extern const f32 float_2p5_8041eaa0;
extern const f32 float_0p01_8041eaa4;
extern const f32 float_0p04_8041eaa8;
extern const f32 float_0p1_8041eaac;
extern const f32 float_0p2_8041eab0;

// .rodata zero vector, int->float and mode-1 doubles are compiler-generated pool constants here.

// .sbss2 zero GXColor locals the original copies before filling its fields.
extern GXColor unk_804230b0;
extern GXColor unk_804230b4;

// .data vertex array of this unit; the original hands a 0x46-byte slice to effSetVtxDescN64.
extern u8 stg_fire_breath_v[];

#pragma opt_common_subs off // without it MWCC CSEs the two 0.2f pool loads across the rand call into a callee-saved FPR
EffectEntry* effBreathFireN64Entry(s32 a, f32 b, f32 c, s32 d, f32 e, s32 f, f32 g, s32 h, f32 i, f32 j) {
	EffectEntry* entry;
	effBreathFireData* data;

	entry = effEntry();
	entry->type = "BreathFireN64";
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effBreathFireData));
	entry->userdata = data;
	entry->callback = effBreathFireMain;
	data->field_0x00 = a;
	data->field_0x04 = d;
	data->field_0x08 = f;
	data->field_0x0C = b;
	data->field_0x10 = c;
	data->field_0x14 = e;
	data->field_0x18 = b;
	data->field_0x1C = c;
	data->field_0x20 = e;
	data->field_0x24 = g;
	data->field_0x28 = i;
	data->field_0x2C = j;
	if (a == 0) {
		data->field_0x38 = float_0p05_8041ea9c;
	}
	else {
		data->field_0x38 = float_0p04_8041eaa8;
	}
	data->field_0x30 = data->field_0x38;
	data->field_0x44 = h;
	data->field_0x48 = h;
	data->field_0x4C = 0;
	data->field_0x40 = 0xFF;
	data->field_0x5C = float_0_8041ea8c;
	data->field_0x3C = float_0p1_8041eaac;
	data->field_0x34 = data->field_0x30;
	if (a == 0) {
		data->field_0x50 = float_0p2_8041eab0 * (g - b) * (f32)(s32)(effTblRandN64(10, f) - 5) * float_0p2_8041eab0;
		data->field_0x54 = float_0p2_8041eab0 * (i - c) * (f32)(s32)(effTblRandN64(10, f + 1) - 5) * float_0p2_8041eab0;
		data->field_0x58 = float_0p2_8041eab0 * (j - e) * (f32)(s32)(effTblRandN64(10, f + 2) - 5) * float_0p2_8041eab0;
	}
	else {
		data->field_0x50 = float_0p2_8041eab0 * (g - b) * (f32)(s32)(effTblRandN64(10, f + 3) - 5);
		data->field_0x54 = float_0p2_8041eab0 * (i - c) * (f32)(s32)(effTblRandN64(10, f + 4) - 5);
		data->field_0x58 = float_0p2_8041eab0 * (j - e) * (f32)(s32)(effTblRandN64(10, f + 5) - 5);
	}
	data->field_0x60 = float_0_8041ea8c;
	data->field_0x64 = 0xFF;
	data->field_0x68 = 0xAA;
	data->field_0x6C = 0x2A;
	data->field_0x70 = 0xF3;
	data->field_0x74 = 0x30;
	data->field_0x78 = 0;

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
// 0.3+0x20 (the given named definitions emitted first and shifted every base+offset load). Under MUST_MATCH
// the speed term is `float_4 * (f32)... / float_10` (rather than `(f32)... * float_4`), which makes mwcc
// hoist the float_4 load into f3 first, matching the original's schedule.
void effBreathFireMain(EffectEntry* effect) {
	s32 life;
	effBreathFireData* data = effect->userdata;
	s32 total;
	s32 timer;
	Vec position = initializeVec((Vec*)&data->field_0x0C);
	EffectEntry* newEffect;

#ifdef MUST_MATCH
	data->field_0x5C = float_4_8041ea94 * (f32)data->field_0x4C / float_10_8041ea98;
#else
	data->field_0x5C = (f32)data->field_0x4C * float_4_8041ea94 / float_10_8041ea98;
#endif
	data->field_0x48--;
	data->field_0x4C++;
	life = data->field_0x48;
	if (life < 0) {
		effDelete(effect);
		return;
	}
	timer = data->field_0x4C;
	total = data->field_0x44;
	if (life > 5 && data->field_0x00 == 0) {
		data->field_0x38 = data->field_0x38 + float_0p05_8041ea9c * (float_2p5_8041eaa0 - data->field_0x38);
	}
	data->field_0x0C = data->field_0x18 + (f32)timer * (data->field_0x50 + (data->field_0x24 - data->field_0x18)) / (f32)total;
	data->field_0x10 = data->field_0x1C + (f32)timer * (data->field_0x54 + (data->field_0x28 - data->field_0x1C)) / (f32)total;
	data->field_0x14 = data->field_0x20 + (f32)timer * (data->field_0x58 + (data->field_0x2C - data->field_0x20)) / (f32)total;
	if (data->field_0x00 == 1) {
		data->field_0x60 = data->field_0x60 + float_0p01_8041eaa4 * (f32)timer;
		data->field_0x10 = data->field_0x10 + data->field_0x60;
	}
	if (timer == data->field_0x08 + 1 && data->field_0x04 > 0) {
		newEffect = effBreathFireN64Entry(data->field_0x00, data->field_0x18, data->field_0x1C, data->field_0x04 - 1, data->field_0x20, data->field_0x08, data->field_0x24, total, data->field_0x28, data->field_0x2C);
		((effBreathFireData*)newEffect->userdata)->field_0x64 = data->field_0x64;
		((effBreathFireData*)newEffect->userdata)->field_0x68 = data->field_0x68;
		((effBreathFireData*)newEffect->userdata)->field_0x6C = data->field_0x6C;
		((effBreathFireData*)newEffect->userdata)->field_0x70 = data->field_0x70;
		((effBreathFireData*)newEffect->userdata)->field_0x74 = data->field_0x74;
		((effBreathFireData*)newEffect->userdata)->field_0x78 = data->field_0x78;
		((effBreathFireData*)newEffect->userdata)->field_0x30 = ((effBreathFireData*)newEffect->userdata)->field_0x38 = data->field_0x30;
		((effBreathFireData*)newEffect->userdata)->field_0x34 = data->field_0x34;
		((effBreathFireData*)newEffect->userdata)->field_0x3C = data->field_0x3C;
	}
	if (life < 10 && data->field_0x00 == 0) {
		data->field_0x40 = life * 25;
	}
	if (data->field_0x00 == 1) {
		data->field_0x40 = life * 224 / total;
		data->field_0x38 = 0.008f * (0.3 - data->field_0x38) + data->field_0x38;
	}
	if (data->field_0x00 == 2) {
		data->field_0x40 = life * 224 / total;
		data->field_0x38 = data->field_0x3C * (data->field_0x34 - data->field_0x38) + data->field_0x38;
	}
	dispEntry(4, dispCalcZ(&position), 2, effBreathFireDisp, effect);
}

#ifdef MUST_MATCH
// Match-only: building each colour in its own inline (the shape eff_rubble_n64 uses) makes mwcc
// place the by-value GXSetTevColor GXColor temporaries in the original's stack slots. The plain
// form under #else reverses the first colour's home and outgoing slots (the known mwcc quirk).
static inline void breathSetColor0(effBreathFireData* data) {
	GXColor color = unk_804230b0;
	color.r = data->field_0x64;
	color.g = data->field_0x68;
	color.b = data->field_0x6C;
	color.a = data->field_0x40;
	GXSetTevColor(GX_TEVREG0, color);
}

static inline void breathSetColor1(effBreathFireData* data, f32 s, s32 frame) {
	GXColor color = unk_804230b4;
	color.r = data->field_0x70;
	color.g = data->field_0x74;
	color.b = data->field_0x78;
	color.a = (u8)(float_256_8041ea80 * (s - (f32)frame));
	GXSetTevColor(GX_TEVREG1, color);
}
#endif

void effBreathFireDisp(CameraId camId, void* param) {
	EffectEntry* effect;
	effBreathFireData* data;
	CameraEntry* cam;
	GXTexObj texObj;
	GXColor color;
	Mtx trans;
	Mtx scale;
	Mtx rot;
	s32 frame;
	s32 frame2;
	f32 s;

	effect = param;
	cam = camGetPtr(camId);
	data = effect->userdata;
	s = data->field_0x5C;
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
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, 0x7D);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, 0x7D);

#ifdef MUST_MATCH
	breathSetColor0(data);
	breathSetColor1(data, s, frame);
#else
	color = unk_804230b0;
	color.r = data->field_0x64;
	color.g = data->field_0x68;
	color.b = data->field_0x6C;
	color.a = data->field_0x40;
	GXSetTevColor(GX_TEVREG0, color);

	color = unk_804230b4;
	color.r = data->field_0x70;
	color.g = data->field_0x74;
	color.b = data->field_0x78;
	color.a = (u8)(float_256_8041ea80 * (s - (f32)frame));
	GXSetTevColor(GX_TEVREG1, color);
#endif

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

	PSMTXTrans(trans, data->field_0x0C, data->field_0x10, data->field_0x14);
	PSMTXRotRad(rot, 'y', float_deg2rad_8041ea90 * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXScale(scale, data->field_0x38, data->field_0x38, float_0_8041ea8c);
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
