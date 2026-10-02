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
// splash pointer is re-read from effect->userdata (see REPORT.md).
// NONMATCHING (99.05%): 18 instructions differ, all register-allocation/scheduling: quad `fneg`/`fmuls`
// operand order, GXColor temp slots, and the u16->float mask address. See REPORT.md for details.
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
	base = lbl_803ADD20;
	camView = camGetPtr(cameraId);
	PSMTXTrans(mtxPos, work->x, work->y, work->z);
	cam = camGetPtr(cameraId);
	PSMTXRotRad(mtxRot, 'y', -cam->viewYaw * float_deg2rad_80420f70);
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
	GXBegin(0x80, 0, 4);
	GXPosition3f32(0.5f * -f31, f30, 0.0f);
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
		color = dat_80420f68;
		color.a = work[1].unk3C;
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
// NONMATCHING (95.74%): the type-check branch order (`blt`/`beq` swapped, plus two dead `b`s the original
// emits), the sound-name address picks up an extra `mr` before r29, and the vec3_802fe418 `lwz`s in case 3
// schedule earlier than the original's. Everything else is byte-identical.
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
	if (type >= 0 && type != 0 && type < 9) {
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
// NONMATCHING (85.36%): the remaining diffs are register allocation (the
// original keeps data/effect in r26/r19 and its constant addresses are pooled through vec3_802fe418 in
// r31, so nearly every load/store differs in its base register) plus a few scheduling/pooling sites: the
// int-to-float double constants are loaded through the compiler's own pool instead of base+0x60/0x68, the
// frame is 0x110 vs 0x120, and `pos` sits at 0x8 instead of the original's 0x14.
void effFallMain(EffectEntry* effect) {
	FallWork* data = effect->userdata;
	FallWork* elem;
	Vec pos;
	s32 i;
	s32 done;
	s32 flag = 0;
	s32 type;
	s32 unk20;
	s32 inBattle;
	f32 y;
	f32 f30;
	f32 floor;
	f32 unk10;
	f32 f29;
	f32 f26;

	pos = vec3_802fe424;
	pos.x = data->x;
	pos.y = data->y;
	pos.z = data->z;

	inBattle = gp->inBattle != 0;
	y = data->y;
	type = data->type;
	unk10 = data->unk10;
	f30 = float_6p2832_80420fa4 * data->unk18;
	unk20 = data->unk20;
	floor = y + data->unk14;
	f29 = float_30_80420fe0 + y;

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

	{
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
						psndSFXOn_3D(str_802FE488, &pos);
					} else if (i == 1) {
						psndSFXOn_3D(str_802FE4A0, &pos);
					}
				}
				break;
			case 1:
				if (type == 0 && i > 1 && flag == 0 && gp->retraceCount % 6 == 0) {
					psndSFXOn_3D(str_802FE4A0, &pos);
					flag = 1;
				}
				elem->y += elem->unk34;
				elem->unk34 -= elem->unk30;
				if (distABf(elem->x, elem->z, float_0_80420f84, float_0_80420f84) < unk10 * 0.5f) {
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
				if (++elem->unk24 > 30) {
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
			if (gp->retraceCount & 4) {
				elem->unk28++;
			}
			if (elem->unk28 >= 3) {
				elem->unk28 = 0;
			}
			elem->unk48 = float_35_80420fe8 * (f32)sin(0.5f * (f32)gp->retraceCount + (f32)i);
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
					psndSFXOn_3D(str_802FE4B8, &pos);
				}
				break;
			case 1:
				elem->unk44 = (float_60_80420ff0 - (f32)elem->unk24) *
				                  (f32)sin(float_62p832_80420fec * (f32)elem->unk24 / float_60_80420ff0) /
				                  float_60_80420ff0;
				if (++elem->unk24 > 60) {
					elem->unk38++;
				}
				break;
			case 2:
				if (++elem->unk24 > 30) {
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
				smartEntry* se = smartAlloc(0x280, 3);
				f32* v;
				s32 j;
				*(smartEntry**)((u8*)elem + 0x40) = se;
				v = se->address;
				for (j = 0; j < 0x20; j++) {
					v[j * 5 + 0] = (j & 1) ? float_16_80420ff4 : float_neg16_80420ff8;
					v[j * 5 + 1] = float_128_80420ffc - float_8p5333_80421000 * (f32)(j / 2);
					v[j * 5 + 2] = float_0_80420f84;
					v[j * 5 + 3] = (j & 1) ? float_2_80420f88 : float_0_80420f84;
					v[j * 5 + 4] = float_0p066667_80421004 * (f32)(j / 2);
				}
				for (j = 0; j < 0x20; j += 4) {
					v[(j + 0) * 5 + 0] += elem->unk44 * ((v[(j + 0) * 5 + 1] * v[(j + 0) * 5 + 1]) / float_600_80421008);
					v[(j + 1) * 5 + 0] += elem->unk44 * ((v[(j + 1) * 5 + 1] * v[(j + 1) * 5 + 1]) / float_600_80421008);
					v[(j + 2) * 5 + 0] += elem->unk44 * ((v[(j + 2) * 5 + 1] * v[(j + 2) * 5 + 1]) / float_600_80421008);
					v[(j + 3) * 5 + 0] += elem->unk44 * ((v[(j + 3) * 5 + 1] * v[(j + 3) * 5 + 1]) / float_600_80421008);
				}
				DCFlushRange(se->address, 0x280);
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
				if (++elem->unk24 > unk20) {
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
				if (distABf(elem->x, elem->z, float_0_80420f84, float_0_80420f84) < unk10 * 0.5f) {
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
				if (++elem->unk24 > 30) {
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
			if (type == 5) {
				if (elem->unk38 > 1) {
					elem->unk48 += (float_430_80421020 - elem->unk48) / float_10_80420fb0;
				}
			} else if (type == 4) {
				if (elem->unk38 > 1) {
					elem->unk48 += (float_360_80420fa8 - elem->unk48) / float_10_80420fb0;
				}
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
					psndSFXOn_3D(str_802FE4D4, &pos);
				}
				elem->x += elem->unk50;
				elem->y += elem->unk54;
				elem->z += elem->unk58;
				if (++elem->unk24 > 30) {
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
				if (++elem->unk24 > 0xb4) {
					elem->unk24 = 0;
					elem->unk38++;
				}
				if (elem->unk24 == 0x64) {
					psndSFXOn_3D(str_802FE4D4, &pos);
				}
				break;
			case 2:
				elem->unk48 = intplGetValue(0xb, elem->unk24, 0xc8, float_neg27_80421024, float_80_80421028);
				if (elem->unk48 < float_0_80420f84) {
					elem->unk4C = elem->unk48;
					elem->unk48 = float_0_80420f84;
				}
				if (++elem->unk24 > 0xc8) {
					animPoseRelease(elem->unk5C);
					elem->unk5C = -1;
					done++;
				}
				if (elem->unk24 == 0x46) {
					psndSFXOn_3D(str_802FE4D4, &pos);
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
				if (!animGroupBaseAsync(str_802FE56C, inBattle, NULL)) {
					break;
				}
				psndSFXOn_3D(str_802FE4F0, &pos);
				if (elem->unk5C == -1) {
					elem->unk5C = animPoseEntry(str_802FE56C, inBattle);
					animPoseSetAnim(elem->unk5C, str_80421018, 1);
				}
				if (elem->unk60 == -1) {
					elem->unk60 = animPoseEntry(str_802FE56C, inBattle);
					animPoseSetAnim(elem->unk60, str_8042102C, 1);
				}
				/* fallthrough */
			case 2:
			case 4:
			case 6: {
				Vec tbl[4] = {
					{ 140.0f, 40.0f, -10.0f },
					{ 0.0f, 40.0f, -40.0f },
					{ -140.0f, 40.0f, -10.0f },
					{ -400.0f, 40.0f, -40.0f },
				};
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
				f32 t, s, c;
				const f32 p00761 = float_0p00761_80421034, p16605 = float_0p16605_80421038;
				const f32 p03705 = float_0p03705_80421048, p4967 = float_0p4967_8042104c;
				const f32 one = float_1_80420f80;
				elem->y += elem->unk34;
				elem->unk34 -= elem->unk30;
				t = float_6p2832_80420fa4 * elem->unk18 / float_360_80420fa8;
				if (t <= float_3p1416_8042103c) {
					if (t <= float_1p5708_80421040) {
						s = ((p00761 * (t * t) - p16605) * (t * t) +
						     one) *
						    t;
					} else {
						t = float_1p5708_80421040 - (t - float_1p5708_80421040);
						s = ((p00761 * (t * t) - p16605) * (t * t) +
						     one) *
						    t;
					}
				} else {
					if (t < float_4p7124_80421044) {
						t = t - float_3p1416_8042103c;
						s = -(((p00761 * (t * t) - p16605) * (t * t) +
						       float_1_80420f80) *
						      t);
					} else {
						t = float_1p5708_80421040 - (t - float_4p7124_80421044);
						s = -(((p00761 * (t * t) - p16605) * (t * t) +
						       float_1_80420f80) *
						      t);
					}
				}
				elem->x += float_5_80421030 * s;
				t = float_6p2832_80420fa4 * elem->unk18 / float_360_80420fa8;
				if (t <= float_3p1416_8042103c) {
					if (t <= float_1p5708_80421040) {
						c = (p03705 * (t * t) - p4967) * (t * t) +
						    float_1_80420f80;
					} else {
						t = float_1p5708_80421040 - (t - float_1p5708_80421040);
						c = -((p03705 * (t * t) - p4967) * (t * t) +
						      float_1_80420f80);
					}
				} else {
					if (t < float_4p7124_80421044) {
						t = t - float_3p1416_8042103c;
						c = -((p03705 * (t * t) - p4967) * (t * t) +
						      float_1_80420f80);
					} else {
						t = float_1p5708_80421040 - (t - float_4p7124_80421044);
						c = (p03705 * (t * t) - p4967) * (t * t) +
						    float_1_80420f80;
					}
				}
				elem->z += float_5_80421030 * -c;
				elem->unk24--;
				if (elem->unk24 < 0) {
					elem->x = elem->unk50;
					elem->y = elem->unk54;
					elem->z = elem->unk58;
					elem->unk38++;
					if (elem->unk38 == 2) {
						elem->unk64 = psndSFXOn_3D(str_802FE50C, &pos);
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
					psndSFXOn_3D(str_802FE52C, &pos);
				}
				break;
			case 3:
				if (animPoseGetLoopTimes(elem->unk5C) >= float_1_80420f80) {
					if (++elem->unk24 > elem->unk20) {
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
