#include "eff/eff_fall.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include "pmario_sound.h"
#include "system.h"
#include "eff/n64/eff_kemuri1_n64.h"
#include "eff/n64/eff_water_damage_n64.h"
#include "manager/evtmgr.h"
#include <dolphin/os/OSCache.h>
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <math.h>

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u8 lbl_803ADD20[8] = { 0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00 };

extern const f32 float_deg2rad_80420f70;
extern const f32 float_6p2832_80420fa4;
extern const f32 float_360_80420fa8;
extern const f32 float_2p05_80420fac;
extern const f32 float_2p1_80420fb4;
extern const f32 float_10_80420fb0;
extern const f32 float_2_80420f88;
extern const f32 float_0_80420f84;
extern const GXColor dat_80420f5c;
extern const GXColor dat_80420f60;
extern const Vec vec3_802fe46c;
extern const Vec vec3_802fe418;
extern const f32 float_3p1416_8042103c;
extern const f32 float_180_80420fc0;
extern const f32 float_300_80421060;
extern const f32 float_0p2_80421014;
extern const f32 float_0p5_80420f7c;
extern const f32 float_1p2_80421064;
extern const f32 float_1p6_80421068;
extern const f32 float_0p1_8042106c;
extern const f32 float_200_80420fc8;
extern const f32 float_0p6_80421070;
extern const f32 float_1p3_80421074;
extern const f32 float_400_80421078;
extern const f32 float_0p25_80420fa0;
extern const f32 float_30_80420fe0;
extern const f32 float_1_80420f80;
extern const f32 float_64_80420f74;
extern const f32 float_704_80420f78;
extern const f32 float_0p56_80420f8c;
extern const f32 float_0p4_80420f90;
extern const f32 float_0p8_80420f94;
extern const f32 float_0p125_80420f98;
extern const f32 float_0p7_80420f9c;
extern const GXColor dat_80420f64;
extern const GXColor dat_80420f68;
extern const GXColor dat_80420f6c;
extern const GXColor dat_80420f48;
extern const GXColor dat_80420f4c;
extern const GXColor dat_80420f50;
extern const GXColor dat_80420f54;
extern const GXColor dat_80420f58;
extern const f32 float_42p667_80420fb8;
extern const f32 float_0p33333_80420fbc;
extern const f32 float_neg200_80420fc4;
extern const f32 float_neg82p655_80420fcc;
extern const f32 float_88p8_80420fd0;
extern const f32 float_1p03_80420fd4;
extern const f32 float_82p655_80420fd8;
extern const f32 float_neg88p8_80420fdc;
extern const Vec vec3_802fe460;
// effFallMain constants and effect-local rodata (addresses from the target symbol table).
extern const Vec vec3_802fe424;
extern const Vec dat_802fe430[];
extern const f32 float_neg0p3_80420fe4;
extern const f32 float_35_80420fe8;
extern const f32 float_62p832_80420fec;
extern const f32 float_60_80420ff0;
extern const f32 float_16_80420ff4;
extern const f32 float_neg16_80420ff8;
extern const f32 float_128_80420ffc;
extern const f32 float_8p5333_80421000;
extern const f32 float_0p066667_80421004;
extern const f32 float_600_80421008;
extern const f32 float_0p9_8042100c;
extern const f32 float_0p01_80421010;
extern const f32 float_neg0p5_8042101c;
extern const f32 float_430_80421020;
extern const f32 float_neg27_80421024;
extern const f32 float_80_80421028;
extern const f32 float_5_80421030;
extern const f32 float_0p00761_80421034;
extern const f32 float_0p16605_80421038;
extern const f32 float_1p5708_80421040;
extern const f32 float_4p7124_80421044;
extern const f32 float_0p03705_80421048;
extern const f32 float_0p4967_8042104c;
extern const f32 float_40_80421050;
__declspec(section ".sdata2") extern const char str_80421018[];
__declspec(section ".sdata2") extern const char str_8042102C[];
__declspec(section ".sdata2") extern const char str_80421054[];
extern const char str_802FE488[];
extern const char str_802FE4A0[];
extern const char str_802FE4B8[];
extern const char str_802FE4D4[];
extern const char str_802FE4F0[];
extern const char str_802FE50C[];
extern const char str_802FE52C[];
extern const char str_802FE548[];
extern const char str_802FE554[];
extern const char str_802FE560[];
extern const char str_802FE56C[];
extern const char str_802FE578[];
__declspec(section ".sdata") extern s32 evt_shake[];
__declspec(section ".sdata") extern u8 shibuki_normal_tbl[];
__declspec(section ".sdata") extern u8 shibuki_color0_tbl[];
__declspec(section ".sdata") extern u8 shibuki_dl_1_0[];
__declspec(section ".sdata") extern u8 shibuki_dl_1_1[];
__declspec(section ".sdata") extern u8 shibuki_dl_1_2[];
__declspec(section ".sdata") extern u8 shibuki_dl_1_3[];
const Vec vec3_802fe418 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802fe424 = { 0.0f, 0.0f, 0.0f };
const Vec dat_802fe430[4] = {
	{ 140.0f, 40.0f, -10.0f },
	{ 0.0f, 40.0f, -40.0f },
	{ -140.0f, 40.0f, -10.0f },
	{ -400.0f, 40.0f, -40.0f },
};
const Vec vec3_802fe460 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802fe46c = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_mask_802fe478 = 4503599627370496.0;
const f64 double_to_int_802fe480 = 4503601774854144.0;
const char str_802FE488[] = "SFX_BTL_STAGE_FALL_BUG1";
const char str_802FE4A0[] = "SFX_BTL_STAGE_FALL_BUG2";
const char str_802FE4B8[] = "SFX_BTL_STAGE_DAMAGE_FORK1";
const char str_802FE4D4[] = "SFX_BTL_STAGE_FALL_KOOPA1";
const char str_802FE4F0[] = "SFX_BTL_STAGE_FALL_METEOR1";
const char str_802FE50C[] = "SFX_BTL_STAGE_DAMAGE_METEOR2";
const char str_802FE52C[] = "SFX_BTL_STAGE_DAMAGE_LIGHT1";
const char str_802FE548[] = "EFF_Baketu";
const char str_802FE554[] = "EFF_Tarai";
const char str_802FE560[] = "EFF_koopa";
const char str_802FE56C[] = "EFF_inseki";
const char str_802FE578[] = "EFF_light";
const char str_80421058[] = "Fall";
const char str_802FE584[] = "SFX_BTL_STAGE_FALL_WATER1";

s32 effFallCheckHit(EffectEntry* effect) {
	FallWork* work = effect->userdata;

	switch (work->type) {
	case 0:
	case 1:
	case 4:
	case 5:
		if (work[1].unk38 >= 2) {
			return 1;
		}
		break;
	case 2:
	case 3:
		if (work[1].unk38 >= 1) {
			return 1;
		}
		break;
	case 6:
		if (work[1].unk38 < 2) {
			if (work[1].unk38 != 1) {
				break;
			}
			if (work[1].unk24 <= 90) {
				break;
			}
		}
		return 1;
	case 7:
		if (work[1].unk38 >= 4) {
			return 1;
		}
		break;
	case 8:
		if (work[1].unk38 >= 3) {
			return 1;
		}
		break;
	}
	return 0;
}

// Full GX state, quad display list and both shibuki splash passes. The quad uses float literals and the
// splash pointer is re-read from effect->userdata. `rad`/`nf31` and the effFallCol68 inline are
// match-only spellings for the register-allocation clusters.
#ifdef MUST_MATCH
static inline GXColor effFallCol68(u8 a) { GXColor c = dat_80420f68; c.a = a; return c; }
#endif

// NONMATCHING (99.07%): 14 instructions differ, all register-allocation/scheduling: the u16->float mask
// address and its surrounding load order, the quad `fmuls` operand order, and the GXColor temp/argument
// stack slots.
void effFallDispWater(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	FallWork* work = effect->userdata;
	CameraEntry* cam;
	CameraEntry* camView;
	GXColor color;
	GXTexObj texObj;
	Mtx mtxPos, mtxScale, mtxRot, mtxView;
	u8* base;
	u16 height;
	f32 f31;
	f32 f30;
#ifdef MUST_MATCH
	f32 rad;
	f32 nf31;
#endif
	base = lbl_803ADD20;
	camView = camGetPtr(cameraId);
	PSMTXTrans(mtxPos, work->x, work->y, work->z);
	cam = camGetPtr(cameraId);
#ifdef MUST_MATCH
	rad = -cam->viewYaw;
	rad = float_deg2rad_80420f70 * rad;
	PSMTXRotRad(mtxRot, 'y', rad);
#else
	PSMTXRotRad(mtxRot, 'y', -cam->viewYaw * float_deg2rad_80420f70);
#endif
	PSMTXConcat(mtxPos, mtxRot, mtxView);
	PSMTXConcat(camView->view, mtxView, mtxView);

	GXSetNumTevStages(2);
	GXSetTevOrder(0, 0, 0, 4);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevOp(0, 0);
	GXSetTevOrder(1, 0xff, 0xff, 0xff);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 0xf, 2, 0, 0xf);
	GXSetTevAlphaIn(1, 7, 1, 0, 7);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
	GXSetCullMode(2);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xb, 1);
	GXSetVtxDesc(0xd, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xb, 1, 5, 0);
	GXSetVtxAttrFmt(0, 0xd, 1, 4, 0);
	effGetTexObj(0x35, &texObj);
	GXLoadTexObj(&texObj, 0);

	f31 = float_64_80420f74;
	f30 = float_704_80420f78;
	f31 = f31 * float_0p5_80420f7c;
	f30 = f30 * float_0p5_80420f7c;
	PSMTXTrans(mtxPos, work[1].unk50, work[1].unk54, work[1].unk58);
	height = GXGetTexObjHeight(&texObj);
	PSMTXScale(mtxScale, float_1_80420f80, f30 / (f32)height, float_1_80420f80);
	PSMTXConcat(mtxPos, mtxScale, mtxPos);
	GXLoadTexMtxImm(mtxPos, 0x1e, 1);
	PSMTXTrans(mtxPos, work[1].x, work[1].y, work[1].z);
	PSMTXScale(mtxScale, work[1].unk2C, float_1_80420f80, float_1_80420f80);
	PSMTXConcat(mtxPos, mtxScale, mtxPos);
	PSMTXConcat(mtxView, mtxPos, mtxPos);
	GXLoadPosMtxImm(mtxPos, 0);
	GXSetTevColor(1, dat_80420f64);
#ifdef MUST_MATCH
	nf31 = -f31;
#endif
	GXBegin(0x80, 0, 4);
#ifdef MUST_MATCH
	GXPosition3f32(nf31 * 0.5f, f30, 0.0f);
#else
	GXPosition3f32(0.5f * -f31, f30, 0.0f);
#endif
	GXColor4u8(0xff, 0xff, 0xff, 0xff);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3f32(f31 * 0.5f, f30, 0.0f);
	GXColor4u8(0xff, 0xff, 0xff, 0xff);
	GXTexCoord2f32(2.0f, 0.0f);
	GXPosition3f32(f31 * 0.5f, 0.0f, 0.0f);
	GXColor4u8(0xff, 0xff, 0xff, 0x00);
	GXTexCoord2f32(2.0f, 1.0f);
	GXPosition3f32(0.5f * -f31, 0.0f, 0.0f);
	GXColor4u8(0xff, 0xff, 0xff, 0x00);
	GXTexCoord2f32(0.0f, 1.0f);

	work = effect->userdata;
	if (work[1].unk38 > 0) {
		PSMTXTrans(mtxPos, work[1].x, work[1].y, work[1].z);
		PSMTXScale(mtxScale, float_0p56_80420f8c, float_0p4_80420f90, float_0p8_80420f94);
		PSMTXConcat(mtxView, mtxPos, mtxPos);
		PSMTXConcat(mtxPos, mtxScale, mtxPos);
		GXLoadPosMtxImm(mtxPos, 0);
		PSMTXTrans(mtxPos, work[1].unk54 * float_0p125_80420f98, float_0_80420f84, float_0_80420f84);
		GXLoadTexMtxImm(mtxPos, 0x1e, 1);
#ifdef MUST_MATCH
		color = effFallCol68(work[1].unk3C);
#else
		color = dat_80420f68;
		color.a = work[1].unk3C;
#endif
		GXSetTevColor(1, color);
		effGetTexObj(0x37, &texObj);
		GXLoadTexObj(&texObj, 0);
		GXClearVtxDesc();
		GXSetVtxDesc(9, 2);
		GXSetVtxAttrFmt(0, 9, 1, 3, 8);
		GXSetArray(9, base + 0x40, 6);
		GXSetVtxDesc(0xa, 2);
		GXSetVtxAttrFmt(0, 0xa, 0, 1, 6);
		GXSetArray(0xa, shibuki_normal_tbl, 3);
		GXSetVtxDesc(0xb, 2);
		GXSetVtxAttrFmt(0, 0xb, 1, 5, 0);
		GXSetArray(0xb, shibuki_color0_tbl, 4);
		GXSetVtxDesc(0xd, 2);
		GXSetVtxAttrFmt(0, 0xd, 1, 3, 0xe);
		GXSetArray(0xd, base + 0x140, 4);
		GXCallDisplayList(shibuki_dl_1_0, 0x20);
		GXCallDisplayList(shibuki_dl_1_1, 0x20);
		GXCallDisplayList(shibuki_dl_1_2, 0x20);
		GXCallDisplayList(shibuki_dl_1_3, 0x20);
		PSMTXTrans(mtxPos, work[1].x, work[1].y, work[1].z);
		PSMTXScale(mtxScale, float_0p7_80420f9c * (work[1].unk2C * float_0p8_80420f94), float_0p5_80420f7c * (work[1].unk2C * float_0p8_80420f94), work[1].unk2C * float_0p8_80420f94);
		PSMTXConcat(mtxView, mtxPos, mtxPos);
		PSMTXConcat(mtxPos, mtxScale, mtxPos);
		GXLoadPosMtxImm(mtxPos, 0);
		PSMTXTrans(mtxPos, float_0_80420f84, -work[1].unk54 * float_0p25_80420fa0, float_0_80420f84);
		GXLoadTexMtxImm(mtxPos, 0x1e, 1);
		GXSetTevColor(1, dat_80420f6c);
		effGetTexObj(0x36, &texObj);
		GXLoadTexObj(&texObj, 0);
		GXClearVtxDesc();
		GXSetVtxDesc(9, 2);
		GXSetVtxAttrFmt(0, 9, 1, 3, 8);
		GXSetArray(9, base + 0x40, 6);
		GXSetVtxDesc(0xa, 2);
		GXSetVtxAttrFmt(0, 0xa, 0, 1, 6);
		GXSetArray(0xa, shibuki_normal_tbl, 3);
		GXSetVtxDesc(0xb, 2);
		GXSetVtxAttrFmt(0, 0xb, 1, 5, 0);
		GXSetArray(0xb, shibuki_color0_tbl, 4);
		GXSetVtxDesc(0xd, 2);
		GXSetVtxAttrFmt(0, 0xd, 1, 3, 0xe);
		GXSetArray(0xd, base + 0x140, 4);
		GXCallDisplayList(base + 0x1e0, 0x40);
		GXCallDisplayList(base + 0x220, 0x40);
		GXCallDisplayList(base + 0x260, 0x40);
		GXCallDisplayList(base + 0x2a0, 0x40);
	}
}

void effFallDispInseki(CameraId cameraId, void* param) {
#ifdef MUST_MATCH
	f32 rad;
#endif
	EffectEntry* effect = param;
	FallWork* work = effect->userdata;
	CameraEntry* cam;
	Mtx mtxA, mtxD, mtxB, mtxC;
	Vec axis;
	GXColor color;
	s32 i;

	camGetPtr(cameraId);
	PSMTXTrans(mtxA, work->x, work->y, work->z);
	cam = camGetPtr(cameraId);
#ifdef MUST_MATCH
	rad = -cam->viewYaw;
	rad = float_deg2rad_80420f70 * rad;
	PSMTXRotRad(mtxB, 'y', rad);
#else
	PSMTXRotRad(mtxB, 'y', -cam->viewYaw * float_deg2rad_80420f70);
#endif
	PSMTXConcat(mtxA, mtxB, mtxC);

	if (work[1].unk5C != -1 && work[1].unk60 != -1) {
		Vec tmpAxis;
		tmpAxis = vec3_802fe46c;
		tmpAxis.x = (f32)cos(float_6p2832_80420fa4 * work[1].unk18 / float_360_80420fa8);
		tmpAxis.z = (f32)sin(float_6p2832_80420fa4 * work[1].unk18 / float_360_80420fa8);
		axis = tmpAxis;

		for (i = 0; i < 2; i++) {
			if (i == 0) {
				PSMTXTrans(mtxA, work[1].x, work[1].y, work[1].z);
				PSMTXRotAxisRad(mtxB, &axis, float_deg2rad_80420f70 * -work[1].unk48);
				PSMTXScale(mtxD, float_2p05_80420fac * work[1].unk2C, float_2p05_80420fac * work[1].unk2C,
				           float_2p05_80420fac * work[1].unk2C);
				PSMTXConcat(mtxC, mtxA, mtxA);
				PSMTXConcat(mtxA, mtxB, mtxA);
				PSMTXConcat(mtxA, mtxD, mtxA);
				animPoseSetMaterialFlagOn(work[1].unk5C, 0x40);
				color = dat_80420f5c;
				animPoseSetMaterialEvtColor(work[1].unk5C, color);
			} else {
				PSMTXTrans(mtxA, work[1].x, work[1].y, work[1].z);
				PSMTXRotAxisRad(mtxB, &axis, float_deg2rad_80420f70 * -work[1].unk48);
				PSMTXScale(mtxD, float_2_80420f88 * work[1].unk2C, float_2_80420f88 * work[1].unk2C,
				           float_2_80420f88 * work[1].unk2C);
				PSMTXConcat(mtxC, mtxA, mtxA);
				PSMTXConcat(mtxA, mtxB, mtxA);
				PSMTXConcat(mtxA, mtxD, mtxA);
				animPoseSetMaterialFlagOn(work[1].unk5C, 0x40);
				color = dat_80420f60;
				animPoseSetMaterialEvtColor(work[1].unk5C, color);
			}
			animPoseDrawMtx(work[1].unk5C, mtxA, 1, float_0_80420f84, float_10_80420fb0);
			animPoseDrawMtx(work[1].unk5C, mtxA, 2, float_0_80420f84, float_10_80420fb0);
			animPoseDrawMtx(work[1].unk5C, mtxA, 3, float_0_80420f84, float_10_80420fb0);
		}

		PSMTXTrans(mtxA, work[1].x, work[1].y, work[1].z);
		PSMTXScale(mtxD, float_2p1_80420fb4 * work[1].unk2C, float_2p1_80420fb4 * work[1].unk2C,
		           float_2p1_80420fb4 * work[1].unk2C);
		PSMTXConcat(mtxC, mtxA, mtxA);
		PSMTXConcat(mtxA, mtxD, mtxA);
		animPoseDrawMtx(work[1].unk60, mtxA, 1, float_0_80420f84, float_10_80420fb0);
		animPoseDrawMtx(work[1].unk60, mtxA, 2, float_0_80420f84, float_10_80420fb0);
		animPoseDrawMtx(work[1].unk60, mtxA, 3, float_0_80420f84, float_10_80420fb0);
	}
}

// Full element-0 init, particle loop, and the type 0-8 switch bodies are reconstructed. The ternary is the
// `irand(100) > 50 ? 0 : 180` form (writing `<= 50 ? 180 : 0` compiles to a different select idiom), the vec
// copy stores the zero vector before the x/y/z overwrite, and the sound-name address is hoisted into a
// callee-saved register: the declaration order (cInt, sfx, entry, elem) is what puts entry in r28 and cInt in
// r30 as the original does. `sfx` exists only to force that hoist.
// NONMATCHING (95.77%): the original emits two dead `b`s after the type-check `&&` chain that this compiler
// will not produce from any source shape tried (if/nested-if/goto/do-while); the sound-name address picks up
// an extra `mr` before r29; and the vec3_802fe418 `lwz`s in case 3 schedule earlier than the original's.
EffectEntry* effFallEntry(s32 type, f32 x, f32 y, s32 a, f32 z, s32 b, f32 c, f32 d, f32 e) {
	s32 cInt;
#ifdef MUST_MATCH
	// Hoists the sound-name address to a callee-saved register like the original (rule 7):
	const char* sfx;
#endif
	EffectEntry* entry;
	FallWork* elem;
	FallWork* data;
	Vec pos;
	s32 count;
	s32 i;
	f32 angle;
	f32 scale;

	entry = effEntry();
	count = a + 1;
	if (type != 0 && type >= 0 && type < 9) {
		a = 1;
		count = 2;
	}
	entry->type = str_80421058;
	entry->count = count;
	data = __memAlloc(HEAP_EFFECT, sizeof(FallWork) * entry->count);
	entry->userdata = data;
	entry->callback = effFallMain;
#ifdef MUST_MATCH
	sfx = str_802FE584;
#endif
	entry->flags |= 2;

	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->unk10 = c;
	data->unk14 = d;
	data->unk18 = e;
	data->unk1C = a;
	data->unk24 = 0;
	data->unk28 = 0;
	data->unk20 = b;
	data->unk5C = -1;
	data->unk60 = -1;

	cInt = (s32)c;
	elem = &data[1];
	for (i = 1; i < entry->count; i++, elem++) {
		angle = (f32)irand(360) * float_3p1416_8042103c / float_180_80420fc0;
		scale = (f32)irand(cInt);
		elem->x = scale * (f32)sin(angle);
		elem->y = float_300_80421060 + (f32)irand(entry->count);
		elem->z = scale * (f32)cos(angle);
		elem->unk24 = 0;
		elem->unk28 = 0;
		elem->unk3C = 0xFF;
		elem->unk48 = float_0_80420f84;
		elem->unk18 = (f32)((irand(100) > 50) ? 0 : 180);
		elem->unk38 = 0;
		elem->unk5C = -1;
		elem->unk60 = -1;
		switch (type) {
		case 0:
		case 1:
			elem->unk34 = float_0_80420f84;
			elem->unk30 = float_0p2_80421014;
			if (i == 1) {
				elem->x = float_0_80420f84;
				elem->z = float_0_80420f84;
				elem->unk24 = 0;
			} else {
				elem->unk24 = irand(entry->count) + 0x3c;
			}
			elem->unk2C = (type == 0) ? float_0p5_80420f7c : float_1p2_80421064;
			break;
		case 2:
			elem->unk34 = float_0_80420f84;
			elem->unk30 = float_1p6_80421068;
			elem->x = float_0_80420f84;
			elem->z = float_0_80420f84;
			elem->unk24 = 0;
			elem->unk44 = float_0_80420f84;
			elem->unk2C = float_0p5_80420f7c;
			break;
		case 3:
			elem->unk34 = float_0_80420f84;
			elem->unk30 = float_0p1_8042106c;
			elem->x = float_0_80420f84;
			elem->y = float_200_80420fc8;
			elem->z = float_0_80420f84;
			elem->unk50 = float_0_80420f84;
			elem->unk54 = float_0_80420f84;
			elem->unk58 = float_0_80420f84;
			elem->unk2C = float_1_80420f80;
			pos = vec3_802fe418;
			pos.x = x;
			pos.y = y;
			pos.z = z;
			elem->unk64 = psndSFXOn_3D(
#ifdef MUST_MATCH
			    sfx
#else
			    str_802FE584
#endif
			    , &pos);
			break;
		case 4:
		case 5:
			elem->unk34 = float_0_80420f84;
			elem->unk30 = float_0p6_80421070;
			elem->x = float_0_80420f84;
			elem->z = float_0_80420f84;
			elem->unk24 = 0;
			elem->unk2C = float_1_80420f80;
			break;
		case 6:
			elem->x = float_0_80420f84;
			elem->y = float_200_80420fc8;
			elem->z = float_0_80420f84;
			elem->unk50 = -elem->x / float_30_80420fe0;
			elem->unk54 = -elem->y / float_30_80420fe0;
			elem->unk58 = -elem->z / float_30_80420fe0;
			elem->unk2C = float_1p3_80421074;
			elem->unk48 = float_30_80420fe0;
			elem->unk4C = float_0_80420f84;
			break;
		case 7:
			elem->x = float_400_80421078;
			elem->y = float_200_80420fc8;
			elem->z = float_0_80420f84;
			elem->unk2C = float_0p5_80420f7c;
			break;
		case 8:
			elem->unk34 = float_0_80420f84;
			elem->unk30 = float_0p25_80420fa0;
			elem->x = float_0_80420f84;
			elem->y = float_400_80421078;
			elem->z = float_0_80420f84;
			elem->unk2C = float_1_80420f80;
			break;
		}
	}
	return entry;
}

// Full reconstruction of the element-0 init, the per-element state machines for types 0-8, the type 2
// procedural vertex build, the type 7 hand-rolled sin/cos approximation, and the display registration.
// The switch bodies, call order, type 7 sin/cos polynomial and type 2 vertex build are byte-identical.
#ifdef MUST_MATCH
#define FB(off, sym) ((const char*)&vec3_802fe418 + (off))
#define FV(off, sym) (*(const Vec*)((const u8*)&vec3_802fe418 + (off)))
#else
#define FB(off, sym) (sym)
#define FV(off, sym) (sym)
#endif

// Builds the effect's position vector through a temporary so the zero-vector copy and the x/y/z overwrite
// schedule (and spill) as the original does; plain assignment under #else.
static inline Vec effFallMakePos(f32 x, f32 y, f32 z) {
#ifdef MUST_MATCH
	Vec temp = *(const Vec*)((const u8*)&vec3_802fe418 + 0xC);
#else
	Vec temp = vec3_802fe424;
#endif
	temp.x = x;
	temp.y = y;
	temp.z = z;
	return temp;
}

// Register allocation and the int-to-float pool remain. Every call, constant and switch body is
// byte-identical; the diffs are:
//  - ours saves r14 where the target's lowest callee-saved is r15: effect lands in r18 (target r19) and
//    data in r6 (target r26); the loop-invariant strings 0x154/0x160 are hoisted into callee-saved
//    registers where the target rematerialises `addi rX, r31, imm` at each use.
//  - the compiler's `(f32)` magic doubles are addressed from its anonymous pool (@664/@992); the target
//    loads them as the named objects at r31+0x60/0x68 (a .rodata pool layout difference).
// The sfx70/sfx88 locals force the two BUG sound-name addresses into registers the way the target does
// (using the named constants instead made MWCC hoist `unk10 * 0.5f` as a common subexpression).
// NONMATCHING (94.67%): see the note above.
void effFallMain(EffectEntry* effect) {
	FallWork* data = effect->userdata;
	FallWork* elem;
	Vec pos;
	s32 inBattle;
	s32 type;
	s32 unk20;
	s32 i;
	s32 flag = 0;
	s32 done;
	f32 y;
	f32 f30;
	f32 f29;
	f32 unk10;
	f32 floor;
	f32 f26;
	f32 unk18;

	pos = effFallMakePos(data->x, data->y, data->z);

	y = data->y;
	unk20 = data->unk20;
	type = data->type;
	unk18 = data->unk18;
	unk10 = data->unk10;
	floor = y + data->unk14;
	inBattle = gp->inBattle != 0;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		for (i = 0, elem = data; i < effect->count; i++, elem++) {
			if (elem->unk5C != -1) {
				animPoseRelease(elem->unk5C);
			}
			if (elem->unk60 != -1) {
				animPoseRelease(elem->unk60);
			}
			psndSFXOff(elem->unk64);
		}
		effDelete(effect);
		return;
	}

	f30 = float_6p2832_80420fa4 * unk18;
	f29 = float_30_80420fe0 + y;

	{
	const char* sfx70 = FB(0x70, str_802FE488);
	const char* sfx88 = FB(0x88, str_802FE4A0);
	done = 0;
	for (i = 1, elem = &data[1]; i < effect->count; i++, elem++) {
		switch (type) {
		case 0:
		case 1:
			switch (elem->unk38) {
			case 0:
				if (elem->unk24 != 0) {
					elem->unk24--;
				}
				if (elem->unk24 == 0) {
					elem->unk38++;
					if (type == 1) {
						psndSFXOn_3D(sfx70, &pos);
					} else if (i == 1) {
						psndSFXOn_3D(sfx88, &pos);
					}
				}
				break;
			case 1:
				if (type == 0 && i > 1 && flag == 0 && gp->retraceCount % 6 == 0) {
					psndSFXOn_3D(sfx88, &pos);
					flag = 1;
				}
				elem->y += elem->unk34;
				elem->unk34 -= elem->unk30;
				if (distABf(elem->x, elem->z, float_0_80420f84, float_0_80420f84) < unk10 * float_0p5_80420f7c) {
					if (elem->y < floor) {
						elem->y = floor;
						elem->unk34 *= float_neg0p3_80420fe4;
						elem->unk38++;
					}
				} else {
					if (elem->y < y) {
						elem->y = y;
						elem->unk34 *= float_neg0p3_80420fe4;
						elem->unk38++;
					}
				}
				break;
			case 2:
				elem->y += elem->unk34;
				elem->unk34 -= elem->unk30;
				f26 = float_6p2832_80420fa4 * angleABf(float_0_80420f84, float_0_80420f84, elem->x, elem->z) / float_360_80420fa8;
				if (i == 1) {
					f26 = f30 / float_360_80420fa8;
				}
				elem->x += float_0p5_80420f7c * (f32)sin(f26);
				elem->z += float_0p5_80420f7c * -(f32)cos(f26);
				if (elem->y < y) {
					elem->y = y;
					elem->unk24 = 0;
					elem->unk38++;
				}
				break;
			case 3:
				elem->unk24++;
				if (elem->unk24 > 30) {
					elem->unk38++;
				}
				break;
			case 4:
				elem->unk3C -= 10;
				if (elem->unk3C < 0) {
					elem->unk3C = 0;
					elem->unk38++;
				}
				break;
			case 5:
				done++;
				break;
			}
			if ((gp->retraceCount >> 2) & 1) {
				elem->unk28++;
			}
			if (elem->unk28 >= 3) {
				elem->unk28 = 0;
			}
			elem->unk48 = float_35_80420fe8 * (f32)sin(float_0p5_80420f7c * (f32)gp->retraceCount + (f32)i);
			break;
		case 2:
			switch (elem->unk38) {
			case 0:
				elem->y += elem->unk34;
				elem->unk34 -= elem->unk30;
				if (elem->y < y) {
					elem->y = y;
					elem->unk24 = 0;
					elem->unk38++;
					psndSFXOn_3D(FB(0xA0, str_802FE4B8), &pos);
				}
				break;
			case 1:
				elem->unk44 = (float_60_80420ff0 - (f32)elem->unk24) *
				                  (f32)sin(float_62p832_80420fec * (f32)elem->unk24 / float_60_80420ff0) /
				                  float_60_80420ff0;
				elem->unk24++;
				if (elem->unk24 > 60) {
					elem->unk38++;
				}
				break;
			case 2:
				elem->unk24++;
				if (elem->unk24 > 30) {
					elem->unk38++;
				}
				break;
			case 3:
				elem->unk3C -= 10;
				if (elem->unk3C < 0) {
					elem->unk3C = 0;
					elem->unk38++;
				}
				break;
			case 4:
				done++;
				break;
			}
			{
				f32* v;
				s32 j;
				*(smartEntry**)((u8*)elem + 0x40) = smartAlloc(0x280, 3);
				v = (*(smartEntry**)((u8*)elem + 0x40))->address;
				for (j = 0; j < 0x20; j++) {
					v[j * 5 + 0] = (j & 1) ? float_16_80420ff4 : float_neg16_80420ff8;
					v[j * 5 + 1] = float_128_80420ffc - float_8p5333_80421000 * (f32)(j / 2);
					v[j * 5 + 2] = float_0_80420f84;
					v[j * 5 + 3] = (j & 1) ? float_2_80420f88 : float_0_80420f84;
					v[j * 5 + 4] = float_0p066667_80421004 * (f32)(j / 2);
				}
				v = (*(smartEntry**)((u8*)elem + 0x40))->address;
				for (j = 0; j < 0x20; j += 4) {
					v[(j + 0) * 5 + 0] += elem->unk44 * ((v[(j + 0) * 5 + 1] * v[(j + 0) * 5 + 1]) / float_600_80421008);
					v[(j + 1) * 5 + 0] += elem->unk44 * ((v[(j + 1) * 5 + 1] * v[(j + 1) * 5 + 1]) / float_600_80421008);
					v[(j + 2) * 5 + 0] += elem->unk44 * ((v[(j + 2) * 5 + 1] * v[(j + 2) * 5 + 1]) / float_600_80421008);
					v[(j + 3) * 5 + 0] += elem->unk44 * ((v[(j + 3) * 5 + 1] * v[(j + 3) * 5 + 1]) / float_600_80421008);
				}
				DCFlushRange((*(smartEntry**)((u8*)elem + 0x40))->address, 0x280);
			}
			break;
		case 3:
			switch (elem->unk38) {
			case 0:
				elem->y += elem->unk34;
				elem->unk34 -= elem->unk30;
				if (elem->y < y) {
					elem->y = y;
					elem->unk24 = 0;
					elem->unk38++;
				}
				break;
			case 1:
				if (elem->unk24 % 10 == 0) {
					effWaterDamageN64Entry(4, 20, pos.x, pos.y, pos.z, float_1_80420f80);
				}
				elem->unk24++;
				if (elem->unk24 > unk20) {
					elem->unk38++;
				}
				break;
			case 2:
				elem->unk3C = (s32)((f32)elem->unk3C * float_0p9_8042100c);
				elem->unk2C *= float_0p9_8042100c;
				if (elem->unk2C < float_0p01_80421010) {
					psndSFXOff(elem->unk64);
					effDelete(effect);
					return;
				}
				break;
			}
			elem->unk54 -= float_0p2_80421014;
			break;
		case 4:
		case 5:
			switch (elem->unk38) {
			case 0:
				if (type == 5) {
					if (!animGroupBaseAsync(str_802FE548, inBattle, NULL)) {
						break;
					}
					if (elem->unk5C == -1) {
						elem->unk5C = animPoseEntry(str_802FE548, inBattle);
						animPoseSetAnim(elem->unk5C, str_80421018, 1);
					}
				} else {
					if (!animGroupBaseAsync(str_802FE554, inBattle, NULL)) {
						break;
					}
					if (elem->unk5C == -1) {
						elem->unk5C = animPoseEntry(str_802FE554, inBattle);
						animPoseSetAnim(elem->unk5C, str_80421018, 1);
					}
				}
				if (elem->unk24 != 0) {
					elem->unk24--;
				}
				if (elem->unk24 == 0) {
					elem->unk38++;
				}
				break;
			case 1:
				elem->y += elem->unk34;
				elem->unk34 -= elem->unk30;
				if (distABf(elem->x, elem->z, float_0_80420f84, float_0_80420f84) < unk10 * float_0p5_80420f7c) {
					if (elem->y < floor) {
						elem->y = floor;
						elem->unk34 *= float_neg0p3_80420fe4;
						elem->unk38++;
					}
				} else {
					if (elem->y < y) {
						elem->y = y;
						elem->unk34 *= float_neg0p3_80420fe4;
						elem->unk38++;
					}
				}
				break;
			case 2:
				elem->y += elem->unk34;
				elem->unk34 -= elem->unk30;
				f26 = float_6p2832_80420fa4 * angleABf(float_0_80420f84, float_0_80420f84, elem->x, elem->z) / float_360_80420fa8;
				if (i == 1) {
					f26 = f30 / float_360_80420fa8;
				}
				elem->x += float_2_80420f88 * (f32)sin(f26);
				elem->z += float_2_80420f88 * -(f32)cos(f26);
				if (elem->y < y) {
					elem->y = y;
					elem->unk34 = float_neg0p5_8042101c * elem->unk34;
					elem->y = y;
					elem->unk24 = 0;
					if (elem->unk34 < float_1_80420f80) {
						elem->unk38++;
					}
				}
				break;
			case 3:
				elem->unk24++;
				if (elem->unk24 > 30) {
					elem->unk38++;
				}
				break;
			case 4:
				elem->unk3C -= 10;
				if (elem->unk3C < 0) {
					elem->unk3C = 0;
					elem->unk38++;
					animPoseRelease(elem->unk5C);
					elem->unk5C = -1;
				}
				break;
			case 5:
				done++;
				break;
			}
			switch (type) {
			case 4:
				if (elem->unk38 > 1) {
					elem->unk48 += (float_360_80420fa8 - elem->unk48) / float_10_80420fb0;
				}
				break;
			case 5:
				if (elem->unk38 > 1) {
					elem->unk48 += (float_430_80421020 - elem->unk48) / float_10_80420fb0;
				}
				break;
			}
			if (elem->unk5C != -1) {
				animPoseMain(elem->unk5C);
			}
			break;
		case 6:
			switch (elem->unk38) {
			case 0:
				if (!animGroupBaseAsync(str_802FE560, inBattle, NULL)) {
					break;
				}
				if (elem->unk5C == -1) {
					elem->unk5C = animPoseEntry(str_802FE560, inBattle);
					animPoseSetAnim(elem->unk5C, str_80421018, 1);
					psndSFXOn_3D(FB(0xBC, str_802FE4D4), &pos);
				}
				elem->x += elem->unk50;
				elem->y += elem->unk54;
				elem->z += elem->unk58;
				elem->unk24++;
				if (elem->unk24 > 30) {
					elem->unk24 = 0;
					elem->unk38++;
					effKemuri1N64Entry(4, pos.x + elem->x, pos.y + elem->y, pos.z + elem->z, float_0_80420f84);
					evtEntry(evt_shake, 0, 0);
				}
				break;
			case 1:
				elem->unk48 = intplGetValue(0xb, elem->unk24, 0xb4, float_30_80420fe0, float_neg27_80421024);
				if (elem->unk48 < float_0_80420f84) {
					elem->unk4C = elem->unk48;
					elem->unk48 = float_0_80420f84;
				}
				elem->unk24++;
				if (elem->unk24 > 0xb4) {
					elem->unk24 = 0;
					elem->unk38++;
				}
				if (elem->unk24 == 0x64) {
					psndSFXOn_3D(FB(0xBC, str_802FE4D4), &pos);
				}
				break;
			case 2:
				elem->unk48 = intplGetValue(0xb, elem->unk24, 0xc8, float_neg27_80421024, float_80_80421028);
				if (elem->unk48 < float_0_80420f84) {
					elem->unk4C = elem->unk48;
					elem->unk48 = float_0_80420f84;
				}
				elem->unk24++;
				if (elem->unk24 > 0xc8) {
					animPoseRelease(elem->unk5C);
					elem->unk5C = -1;
					done++;
				}
				if (elem->unk24 == 0x46) {
					psndSFXOn_3D(FB(0xBC, str_802FE4D4), &pos);
				}
				break;
			}
			if (elem->unk5C != -1) {
				animPoseMain(elem->unk5C);
			}
			break;
		case 7:
			switch (elem->unk38) {
			case 0:
				if (!animGroupBaseAsync(FB(0x154, str_802FE56C), inBattle, NULL)) {
					break;
				}
				psndSFXOn_3D(FB(0xD8, str_802FE4F0), &pos);
				if (elem->unk5C == -1) {
					elem->unk5C = animPoseEntry(FB(0x154, str_802FE56C), inBattle);
					animPoseSetAnim(elem->unk5C, str_80421018, 1);
				}
				if (elem->unk60 == -1) {
					elem->unk60 = animPoseEntry(FB(0x154, str_802FE56C), inBattle);
					animPoseSetAnim(elem->unk60, str_8042102C, 1);
				}
				/* fallthrough */
			case 2:
			case 4:
			case 6: {
				Vec tbl[4];
#ifdef MUST_MATCH
				tbl[0] = *(const Vec*)((const u8*)&vec3_802fe418 + 0x18);
				tbl[1] = *(const Vec*)((const u8*)&vec3_802fe418 + 0x24);
				tbl[2] = *(const Vec*)((const u8*)&vec3_802fe418 + 0x30);
				tbl[3] = *(const Vec*)((const u8*)&vec3_802fe418 + 0x3C);
#else
				tbl[0] = dat_802fe430[0];
				tbl[1] = dat_802fe430[1];
				tbl[2] = dat_802fe430[2];
				tbl[3] = dat_802fe430[3];
#endif
				elem->unk50 = tbl[elem->unk38 / 2].x;
				elem->unk54 = tbl[elem->unk38 / 2].y;
				elem->unk58 = tbl[elem->unk38 / 2].z;
				elem->unk30 = float_0p4_80420f90;
				elem->unk24 = (s32)(distABf(elem->unk50, elem->unk58, elem->x, elem->z) / float_5_80421030);
				elem->unk34 = (elem->unk54 - elem->y) / (f32)elem->unk24 +
				              0.5f * (elem->unk30 * (f32)elem->unk24);
				elem->unk18 = angleABf(elem->x, elem->z, elem->unk50, elem->unk58);
				elem->unk48 = float_0_80420f84;
				elem->unk38++;
				break;
			}
			case 1:
			case 3:
			case 5:
			case 7: {
				f32 t, s, c, t2;
				f32 p00761 = float_0p00761_80421034, p16605 = float_0p16605_80421038;
				f32 p03705 = float_0p03705_80421048, p4967 = float_0p4967_8042104c;
				f32 one = float_1_80420f80;
				elem->y += elem->unk34;
				elem->unk34 -= elem->unk30;
				t = float_6p2832_80420fa4 * elem->unk18 / float_360_80420fa8;
				if (t <= float_3p1416_8042103c) {
					t2 = t * t;
					if (t <= float_1p5708_80421040) {
						s = ((p00761 * t2 - p16605) * t2 +
						     one) *
						    t;
					} else {
						t = float_1p5708_80421040 - (t - float_1p5708_80421040);
						t2 = t * t;
						s = ((p00761 * t2 - p16605) * t2 +
						     one) *
						    t;
					}
				} else {
					if (t < float_4p7124_80421044) {
						t = t - float_3p1416_8042103c;
						t2 = t * t;
						s = -(((p00761 * t2 - p16605) * t2 +
						       one) *
						      t);
					} else {
						t = float_1p5708_80421040 - (t - float_4p7124_80421044);
						t2 = t * t;
						s = -(((p00761 * t2 - p16605) * t2 +
						       one) *
						      t);
					}
				}
				elem->x += float_5_80421030 * s;
				t = elem->unk18 * float_6p2832_80420fa4 / float_360_80420fa8;
				if (t <= float_3p1416_8042103c) {
					if (t <= float_1p5708_80421040) {
						t2 = t * t;
						c = (p03705 * t2 - p4967) * t2 +
						    one;
					} else {
						t = float_1p5708_80421040 - (t - float_1p5708_80421040);
						t2 = t * t;
						c = -((p03705 * t2 - p4967) * t2 +
						      one);
					}
				} else {
					if (t < float_4p7124_80421044) {
						t = t - float_3p1416_8042103c;
						t2 = t * t;
						c = -((p03705 * t2 - p4967) * t2 +
						      one);
					} else {
						t = float_1p5708_80421040 - (t - float_4p7124_80421044);
						t2 = t * t;
						c = (p03705 * t2 - p4967) * t2 +
						    one;
					}
				}
				elem->z += float_5_80421030 * -c;
				if (--elem->unk24 < 0) {
					elem->x = elem->unk50;
					elem->y = elem->unk54;
					elem->z = elem->unk58;
					elem->unk38++;
					if (elem->unk38 == 2) {
						elem->unk64 = psndSFXOn_3D(FB(0xF4, str_802FE50C), &pos);
					}
					if (elem->unk38 != 8) {
						effKemuri1N64Entry(4, pos.x + elem->x, pos.y + elem->y - float_40_80421050,
						                   pos.z + elem->z, float_0_80420f84);
						evtEntry(evt_shake, 0, 0);
					}
				}
				elem->unk48 += (float_360_80420fa8 - elem->unk48) / float_30_80420fe0;
				break;
			}
			case 8:
				psndSFXOff(elem->unk64);
				animPoseRelease(elem->unk5C);
				animPoseRelease(elem->unk60);
				elem->unk5C = -1;
				elem->unk60 = -1;
				elem->unk38++;
				break;
			case 9:
				done++;
				break;
			}
			if (elem->unk5C != -1) {
				animPoseMain(elem->unk5C);
			}
			if (elem->unk60 != -1) {
				animPoseMain(elem->unk60);
			}
			break;
		case 8:
			switch (elem->unk38) {
			case 0:
				if (!animGroupBaseAsync(str_802FE578, inBattle, NULL)) {
					break;
				}
				elem->unk5C = animPoseEntry(str_802FE578, inBattle);
				animPoseSetAnim(elem->unk5C, str_80421018, 1);
				elem->unk38++;
				break;
			case 1:
			case 2:
				elem->y += elem->unk34;
				elem->unk34 -= elem->unk30;
				if (elem->y < y) {
					elem->y = y;
					elem->unk24 = 0;
					elem->unk38++;
				}
				if (elem->unk38 == 1 && elem->y < f29) {
					effKemuri1N64Entry(4, pos.x + elem->x, pos.y + y, pos.z + elem->z, float_0_80420f84);
					evtEntry(evt_shake, 0, 0);
					animPoseSetAnim(elem->unk5C, str_80421054, 1);
					elem->unk38++;
					psndSFXOn_3D(FB(0x114, str_802FE52C), &pos);
				}
				break;
			case 3:
				if (animPoseGetLoopTimes(elem->unk5C) >= float_1_80420f80) {
					elem->unk24++;
					if (elem->unk24 > elem->unk20) {
						elem->unk38++;
					}
				}
				break;
			case 4:
				elem->unk3C -= 10;
				if (elem->unk3C < 0) {
					elem->unk3C = 0;
					elem->unk38++;
				}
				break;
			case 5:
				animPoseRelease(elem->unk5C);
				elem->unk5C = -1;
				elem->unk38++;
				break;
			case 6:
				done++;
				break;
			}
			if (elem->unk5C != -1) {
				animPoseMain(elem->unk5C);
			}
			break;
		}
	}

	if (done >= effect->count - 1) {
		effDelete(effect);
		return;
	}

	switch (type) {
	case 3:
		dispEntry(CAMERA_3D, dispCalcZ(&pos), 2, effFallDispWater, effect);
		break;
	case 4:
	case 5:
		dispEntry(CAMERA_3D, dispCalcZ(&pos), 0, effFallDisp, effect);
		break;
	case 7:
		dispEntry(CAMERA_3D, dispCalcZ(&pos), 2, effFallDispInseki, effect);
		break;
	default:
		dispEntry(CAMERA_3D, dispCalcZ(&pos), 2, effFallDisp, effect);
		break;
	}
	}
}

// Builds the type-8 light colour and the case 0/1 / case 2 material colours. Returning the colour by
// value (rather than assigning a base and then `color.a`) is what puts the colour temp and its
// outgoing argument copy in the target's stack slots; `effFallColA`'s (alpha, base) parameter order is
// what lands the base word / alpha byte in the target's r5 / r0. MUST_MATCH builds them, otherwise the
// natural assignment at each site is used.
#ifdef MUST_MATCH
static inline GXColor effFallCol8(u8 a) { GXColor c = dat_80420f58; c.a = a; return c; }
static inline GXColor effFallColA(u8 a, GXColor c) { c.a = a; return c; }
#endif

// GX display for the default/koopa/light types: a per-type textured quad, the procedural type-2 strip,
// the type 4/5 animation halo, the type 6 koopa body and the type 8 light pose.
// The viewYaw product, the case-0/1 half-angle products and the two GXChanMat colours go through
// explicit locals under MUST_MATCH: `rad`, `w` and `neg29`/`pos29` put the compiler's temporaries in the
// target's FPRs, and the colour helpers put them in its GPRs/stack slots. The natural expressions sit
// under each #else.
void effFallDisp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	FallWork* elem = effect->userdata;
	CameraEntry* cam;
	CameraEntry* camView;
	GXTexObj texObj;
	Mtx mtxPos, mtxScale, mtxRot, mtxView;
	s32 i;
	s32 type;
	f32 f31;
	f32 f30;
	f32 f29;
	f32 f28;
#ifdef MUST_MATCH
	f32 rad;
#endif

	camView = camGetPtr(cameraId);
	type = elem->type;
	f30 = elem->unk18;
	PSMTXTrans(mtxPos, elem->x, elem->y, elem->z);
	cam = camGetPtr(cameraId);
#ifdef MUST_MATCH
	rad = -cam->viewYaw;
	rad = float_deg2rad_80420f70 * rad;
	PSMTXRotRad(mtxRot, 'y', rad);
#else
	PSMTXRotRad(mtxRot, 'y', -cam->viewYaw * float_deg2rad_80420f70);
#endif
	PSMTXConcat(mtxPos, mtxRot, mtxView);

	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 4);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevOp(0, 0);
	GXSetCullMode(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xd, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xd, 1, 4, 0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);

	switch (type) {
	case 0:
	case 1:
		effGetTexObj(0x32, &texObj);
		GXLoadTexObj(&texObj, 0);
		break;
	case 2:
		effGetTexObj(0x33, &texObj);
		GXLoadTexObj(&texObj, 0);
		break;
	}

	f31 = float_6p2832_80420fa4 * f30;
	for (i = 1, elem = &elem[1]; i < effect->count; i++, elem++) {
		switch (type) {
		case 0:
		case 1: {
#ifndef MUST_MATCH
			GXColor color;
#endif
#ifdef MUST_MATCH
			f32 w;
			f32 neg29;
			f32 pos29;
#endif
			f29 = float_64_80420f74;
			f28 = float_42p667_80420fb8;
			f29 = f29 * float_0p5_80420f7c;
			f28 = f28 * float_0p5_80420f7c;
			PSMTXTrans(mtxPos, elem->x, elem->y, elem->z);
			PSMTXRotRad(mtxRot, 'y', float_deg2rad_80420f70 * elem->unk18);
			PSMTXConcat(mtxPos, mtxRot, mtxPos);
#ifdef MUST_MATCH
			w = f28 * float_0p5_80420f7c;
			PSMTXTrans(mtxRot, float_0_80420f84, w * elem->unk2C, float_0_80420f84);
#else
			PSMTXTrans(mtxRot, float_0_80420f84, f28 * 0.5f * elem->unk2C, float_0_80420f84);
#endif
			PSMTXConcat(mtxPos, mtxRot, mtxPos);
			PSMTXRotRad(mtxRot, 'z', float_deg2rad_80420f70 * elem->unk48);
			PSMTXConcat(mtxPos, mtxRot, mtxPos);
#ifdef MUST_MATCH
			w = -f28 * float_0p5_80420f7c;
			PSMTXTrans(mtxRot, float_0_80420f84, w * elem->unk2C, float_0_80420f84);
#else
			PSMTXTrans(mtxRot, float_0_80420f84, -f28 * 0.5f * elem->unk2C, float_0_80420f84);
#endif
			PSMTXConcat(mtxPos, mtxRot, mtxPos);
			PSMTXScale(mtxScale, elem->unk2C, elem->unk2C, elem->unk2C);
			PSMTXConcat(mtxPos, mtxScale, mtxPos);
			PSMTXConcat(mtxView, mtxPos, mtxPos);
			PSMTXConcat(camView->view, mtxPos, mtxPos);
			GXLoadPosMtxImm(mtxPos, 0);
			GXSetCurrentMtx(0);
#ifdef MUST_MATCH
			GXSetChanMatColor(4, effFallColA(elem->unk3C, dat_80420f48));
#else
			color = dat_80420f48;
			color.a = elem->unk3C;
			GXSetChanMatColor(4, color);
#endif
			PSMTXScale(mtxScale, float_1_80420f80, float_0p33333_80420fbc, float_1_80420f80);
			PSMTXTrans(mtxPos, float_0_80420f84, (f32)elem->unk28, float_0_80420f84);
			PSMTXConcat(mtxScale, mtxPos, mtxPos);
			GXLoadTexMtxImm(mtxPos, 0x1e, 1);
			GXBegin(0x80, 0, 4);
#ifdef MUST_MATCH
			neg29 = -f29;
			neg29 = neg29 * float_0p5_80420f7c;
			pos29 = f29 * float_0p5_80420f7c;
			GXPosition3f32(neg29, f28, 0.0f);
#else
			GXPosition3f32(-f29 * 0.5f, f28, 0.0f);
#endif
			GXTexCoord2f32(0.0f, 0.0f);
#ifdef MUST_MATCH
			GXPosition3f32(pos29, f28, 0.0f);
#else
			GXPosition3f32(f29 * 0.5f, f28, 0.0f);
#endif
			GXTexCoord2f32(1.0f, 0.0f);
#ifdef MUST_MATCH
			GXPosition3f32(pos29, 0.0f, 0.0f);
#else
			GXPosition3f32(f29 * 0.5f, 0.0f, 0.0f);
#endif
			GXTexCoord2f32(1.0f, 1.0f);
#ifdef MUST_MATCH
			GXPosition3f32(neg29, 0.0f, 0.0f);
#else
			GXPosition3f32(-f29 * 0.5f, 0.0f, 0.0f);
#endif
			GXTexCoord2f32(0.0f, 1.0f);
			break;
		}
		case 2: {
#ifndef MUST_MATCH
			GXColor color;
#endif
			s32 j;
			f32* v;
			PSMTXTrans(mtxPos, elem->x, elem->y, elem->z);
			PSMTXScale(mtxScale, elem->unk2C, elem->unk2C, elem->unk2C);
			PSMTXConcat(mtxPos, mtxScale, mtxPos);
			PSMTXConcat(mtxView, mtxPos, mtxPos);
			PSMTXConcat(camView->view, mtxPos, mtxPos);
			GXLoadPosMtxImm(mtxPos, 0);
			GXSetCurrentMtx(0);
#ifdef MUST_MATCH
			GXSetChanMatColor(4, effFallColA(elem->unk3C, dat_80420f4c));
#else
			color = dat_80420f4c;
			color.a = elem->unk3C;
			GXSetChanMatColor(4, color);
#endif
			PSMTXIdentity(mtxPos);
			GXLoadTexMtxImm(mtxPos, 0x1e, 1);
			for (j = 0; j < 0x1e; j += 2) {
				v = (f32*)(*(smartEntry**)((u8*)elem + 0x40))->address + j * 5;
				GXBegin(0x80, 0, 4);
				GXPosition3f32(v[0], v[1], v[2]);
				GXTexCoord2f32(v[3], v[4]);
				GXPosition3f32(v[10], v[11], v[12]);
				GXTexCoord2f32(v[13], v[14]);
				GXPosition3f32(v[15], v[16], v[17]);
				GXTexCoord2f32(v[18], v[19]);
				GXPosition3f32(v[5], v[6], v[7]);
				GXTexCoord2f32(v[8], v[9]);
			}
			break;
		}
		case 3:
		case 7:
			break;
		case 4:
		case 5: {
			Vec axis;
			Vec tmpAxis;
			f28 = f31 / float_360_80420fa8;
			tmpAxis = vec3_802fe460;
			tmpAxis.x = (f32)cos(f28);
			tmpAxis.z = (f32)sin(f28);
			axis = tmpAxis;
			PSMTXTrans(mtxPos, elem->x, elem->y, elem->z);
			PSMTXScale(mtxScale, elem->unk2C, elem->unk2C, elem->unk2C);
			PSMTXConcat(mtxPos, mtxScale, mtxPos);
			PSMTXTrans(mtxScale, 0.0f, (f32)(type == 4 ? 8 : 16), 0.0f);
			PSMTXConcat(mtxPos, mtxScale, mtxPos);
			PSMTXRotAxisRad(mtxRot, &axis, float_deg2rad_80420f70 * -elem->unk48);
			PSMTXConcat(mtxPos, mtxRot, mtxPos);
			PSMTXTrans(mtxScale, 0.0f, (f32)-(type == 4 ? 8 : 16), 0.0f);
			PSMTXConcat(mtxPos, mtxScale, mtxPos);
			PSMTXConcat(mtxView, mtxPos, mtxPos);
			if (elem->unk5C != -1) {
				animPoseDrawMtx(elem->unk5C, mtxPos, 1, float_0_80420f84, float_10_80420fb0);
				animPoseDrawMtx(elem->unk5C, mtxPos, 2, float_0_80420f84, float_10_80420fb0);
				animPoseDrawMtx(elem->unk5C, mtxPos, 3, float_0_80420f84, float_10_80420fb0);
			}
			break;
		}
		case 6: {
			GXColor color;
			if (elem->unk5C != -1) {
				PSMTXTrans(mtxPos, elem->x, elem->y, elem->z);
				if (f30 < float_180_80420fc0) {
					PSMTXScale(mtxScale, elem->unk2C, elem->unk2C, elem->unk2C);
				} else {
					PSMTXScale(mtxScale, -elem->unk2C, elem->unk2C, elem->unk2C);
				}
				PSMTXConcat(mtxPos, mtxScale, mtxPos);
				if (elem->unk48 != float_0_80420f84) {
					PSMTXRotRad(mtxRot, 'z', float_deg2rad_80420f70 * -elem->unk48);
					PSMTXConcat(mtxPos, mtxRot, mtxPos);
				} else {
					PSMTXTrans(mtxScale, float_neg200_80420fc4, float_0_80420f84, float_0_80420f84);
					PSMTXConcat(mtxPos, mtxScale, mtxPos);
					PSMTXRotRad(mtxRot, 'z', float_deg2rad_80420f70 * -elem->unk4C);
					PSMTXConcat(mtxPos, mtxRot, mtxPos);
					PSMTXTrans(mtxScale, float_200_80420fc8, float_0_80420f84, float_0_80420f84);
					PSMTXConcat(mtxPos, mtxScale, mtxPos);
				}
				PSMTXConcat(mtxView, mtxPos, mtxPos);
				PSMTXTrans(mtxScale, float_neg82p655_80420fcc, float_88p8_80420fd0, float_0_80420f84);
				PSMTXConcat(mtxPos, mtxScale, mtxScale);
				PSMTXScale(mtxRot, float_1p03_80420fd4, float_1p03_80420fd4, float_1p03_80420fd4);
				PSMTXConcat(mtxScale, mtxRot, mtxRot);
				PSMTXTrans(mtxScale, float_82p655_80420fd8, float_neg88p8_80420fdc, float_0_80420f84);
				PSMTXConcat(mtxRot, mtxScale, mtxScale);
				animPoseSetMaterialFlagOn(elem->unk5C, 0x40);
				color = dat_80420f50;
				animPoseSetMaterialEvtColor(elem->unk5C, color);
				animPoseDrawMtx(elem->unk5C, mtxScale, 1, float_0_80420f84, float_10_80420fb0);
				animPoseDrawMtx(elem->unk5C, mtxScale, 2, float_0_80420f84, float_10_80420fb0);
				animPoseDrawMtx(elem->unk5C, mtxScale, 3, float_0_80420f84, float_10_80420fb0);
				animPoseSetMaterialFlagOn(elem->unk5C, 0x40);
				color = dat_80420f54;
				animPoseSetMaterialEvtColor(elem->unk5C, color);
				animPoseDrawMtx(elem->unk5C, mtxPos, 1, float_0_80420f84, float_10_80420fb0);
				animPoseDrawMtx(elem->unk5C, mtxPos, 2, float_0_80420f84, float_10_80420fb0);
				animPoseDrawMtx(elem->unk5C, mtxPos, 3, float_0_80420f84, float_10_80420fb0);
			}
			break;
		}
		case 8: {
			GXColor color;
			if (elem->unk5C != -1) {
				PSMTXTrans(mtxPos, elem->x, elem->y, elem->z);
				PSMTXScale(mtxScale, elem->unk2C, elem->unk2C, elem->unk2C);
				PSMTXConcat(mtxPos, mtxScale, mtxPos);
				PSMTXConcat(mtxView, mtxPos, mtxPos);
				animPoseSetMaterialFlagOn(elem->unk5C, 0x40);
#ifdef MUST_MATCH
				color = effFallCol8(elem->unk3C);
#else
				color = dat_80420f58;
				color.a = elem->unk3C;
#endif
				animPoseSetMaterialEvtColor(elem->unk5C, color);
				animPoseDrawMtx(elem->unk5C, mtxPos, 1, float_0_80420f84, float_10_80420fb0);
				animPoseDrawMtx(elem->unk5C, mtxPos, 2, float_0_80420f84, float_10_80420fb0);
				animPoseDrawMtx(elem->unk5C, mtxPos, 3, float_0_80420f84, float_10_80420fb0);
			}
			break;
		}
		}
	}
}
