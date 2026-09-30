#include "eff/eff_fall.h"
#include "driver/animdrv.h"
#include "memory.h"
#include "pmario_sound.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <math.h>

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
extern u8 lbl_803ADD20[];
extern u8 shibuki_normal_tbl[];
extern u8 shibuki_color0_tbl[];
extern u8 shibuki_dl_1_0[];
extern u8 shibuki_dl_1_1[];
extern u8 shibuki_dl_1_2[];
extern u8 shibuki_dl_1_3[];
const f64 double_to_int_mask_802fe478 = 4503599627370496.0;
const f64 double_to_int_802fe480 = 4503601774854144.0;
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

// Full GX state, quad display list and both shibuki splash passes transcribed. Diffs are register
// allocation (original keeps cameraId/effect/work/base/camView in r27-r31 and uses `stmw r27`; ours
// needs one fewer GPR and a 0x10-smaller frame) plus a few commutative float operand orders.
// NONMATCHING (90.07%): 16 instructions differ, 19 missing, 18 extra, plus register differences.
void effFallDispWater(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	FallWork* work = effect->userdata;
	CameraEntry* cam;
	CameraEntry* camView;
	GXTexObj texObj;
	GXColor color;
	Mtx mtxPos, mtxScale, mtxRot, mtxView;
	u8* base;
	u16 height;
	f32 f31;
	f32 f30;
	f32 w;

	base = lbl_803ADD20;
	camView = camGetPtr(cameraId);
	PSMTXTrans(mtxPos, work->x, work->y, work->z);
	cam = camGetPtr(cameraId);
	PSMTXRotRad(mtxRot, 'y', float_deg2rad_80420f70 * -cam->viewYaw);
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

	f31 = float_64_80420f74 * float_0p5_80420f7c;
	f30 = float_704_80420f78 * float_0p5_80420f7c;
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
	w = f31 * float_0p5_80420f7c;
	GXPosition3f32(-w, f30, float_0_80420f84);
	GXColor4u8(0xff, 0xff, 0xff, 0xff);
	GXTexCoord2f32(float_0_80420f84, float_0_80420f84);
	GXPosition3f32(w, f30, float_0_80420f84);
	GXColor4u8(0xff, 0xff, 0xff, 0xff);
	GXTexCoord2f32(float_2_80420f88, float_0_80420f84);
	GXPosition3f32(w, float_0_80420f84, float_0_80420f84);
	GXColor4u8(0xff, 0xff, 0xff, 0x00);
	GXTexCoord2f32(float_2_80420f88, float_1_80420f80);
	GXPosition3f32(-w, float_0_80420f84, float_0_80420f84);
	GXColor4u8(0xff, 0xff, 0xff, 0x00);
	GXTexCoord2f32(float_0_80420f84, float_1_80420f80);

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
		PSMTXScale(mtxScale, float_0p7_80420f9c * (float_0p8_80420f94 * work[1].unk2C), float_0p5_80420f7c * (float_0p8_80420f94 * work[1].unk2C), float_0p8_80420f94 * work[1].unk2C);
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

// Full element-0 init, particle loop, and the type 0-8 switch bodies are reconstructed and match in
// substance. Diffs are register allocation (c lands in f21 not f31, shifting every callee-saved GPR/FPR
// and growing the frame by 0x10), the compiler's local jump table symbol, and the branch shape of the
// `type != 0 && type >= 0 && type < 9` test.
// NONMATCHING (86.33%): see above.
EffectEntry* effFallEntry(s32 type, f32 x, f32 y, s32 a, f32 z, s32 b, f32 c, f32 d, f32 e) {
	EffectEntry* entry;
	FallWork* data;
	FallWork* elem;
	Vec pos;
	s32 cInt;
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
	entry->flags |= 2;

	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->unk10 = c;
	data->unk14 = d;
	data->unk18 = e;
	data->unk1C = a;
	data->unk20 = b;
	data->unk24 = 0;
	data->unk28 = 0;
	data->unk5C = -1;
	data->unk60 = -1;

	cInt = (s32)c;
	for (i = 1; i < entry->count; i++) {
		elem = &data[i];
		angle = (f32)irand(360) * float_3p1416_8042103c / float_180_80420fc0;
		scale = (f32)irand(cInt);
		elem->x = scale * (f32)sin(angle);
		elem->y = float_300_80421060 + (f32)irand(entry->count);
		elem->z = scale * (f32)cos(angle);
		elem->unk24 = 0;
		elem->unk28 = 0;
		elem->unk3C = 0xFF;
		elem->unk48 = float_0_80420f84;
		elem->unk18 = (f32)((irand(100) <= 50) ? 180 : 0);
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
			elem->unk64 = psndSFXOn_3D(str_802FE584, &pos);
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
