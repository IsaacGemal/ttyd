#include "eff/eff_mobj_broken.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_kemutest.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "math_ext.h"
#include "matching.h"

// .sdata2 / .rodata constants (defined by the original unit's data).
extern const Vec vec3_802fe634;
extern const Vec vec3_802fe640;
extern const f32 float_deg2rad_804210e8;
extern const f32 float_0_804210ec;
extern const f32 float_0p8_804210f0;
extern const f32 float_6p2832_804210f4;
extern const f32 float_360_804210f8;
extern const f32 float_5_804210fc;
extern const f32 float_1_80421100;
extern const f32 float_0p00761_8042110c;
extern const f32 float_0p16605_80421110;
extern const f32 float_3p1416_80421114;
extern const f32 float_1p5708_80421118;
extern const f32 float_4p7124_8042111c;
extern const f32 float_0p03705_80421120;
extern const f32 float_0p4967_80421124;
extern const f32 float_20_80421128;
extern const f32 float_0p125_8042112c;
extern const f32 float_720_80421130;
extern const f32 float_0p03125_80421134;
extern const f32 float_neg100_80421138;
extern const f32 float_6p25_8042113c;
extern const f32 float_12p5_80421140;
extern const f32 float_0p5_80421144;
extern const f32 float_0p0625_80421148;
extern const GXColor dat_804210e0;
extern const GXColor dat_804210e4;
extern const GXColor dat_804210dc;

// .rodata (0x802FE628..0x802FE670)
#ifdef MUST_MATCH
// Reads of the unit's own .sdata2 constants through the object, so the loads point at it instead of a new pool word.
#define ZERO0 (*(volatile const f32*)&float_0_804210ec)
#define ONE1 (*(volatile const f32*)&float_1_80421100)
#else
#define ZERO0 float_0_804210ec
#define ONE1 float_1_80421100
#endif
extern const char str_Z_2[];
extern const char str_80421108[];

EffectEntry* effMObjBrokenEntry(s32 type, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	MobjBrokenWork* data;
	MobjBrokenWork* work;
	s32 i;
	s32 idx;
	s32 gridX;
	s32 gridY;
	s32 gridZ;
	f32 spacing;
	s32 count;

	entry = effEntry();
	count = 1;
	switch (type) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		count = 0x11;
		break;
	case 6:
		count = 0x11;
		break;
	case 7:
		count = 9;
		break;
	}
	entry->type = "MObjBroken";
	entry->count = count;
	data = __memAlloc(HEAP_EFFECT, entry->count * sizeof(MobjBrokenWork));
	entry->userdata = data;
	entry->callback = effMObjBrokenMain;
	entry->flags |= 2;
	data->type = type;
	data->posX = x;
	data->posY = y;
	data->posZ = z;
	data->scale = float_1_80421100;
	data->poseId = -1;
	work = data + 1;
	for (i = 1; i < entry->count; i++, work++) {
		idx = i;
		if (idx > 9) {
			idx -= 8;
		}
		switch (type) {
		case 0:
		case 1:
		case 2:
			spacing = float_6p25_8042113c;
			gridX = (idx - 1) % 4 / 2;
			gridY = (idx - 1) / 4;
			gridZ = (idx - 1) % 4 % 2;
			break;
		case 3:
		case 4:
		case 5:
			spacing = float_12p5_80421140;
			gridX = (idx - 1) % 4 / 2;
			gridY = (idx - 1) / 4;
			gridZ = (idx - 1) % 4 % 2;
			break;
		case 6:
			spacing = float_12p5_80421140;
			gridX = (idx - 1) % 2;
			gridY = (idx - 1) / 2;
			gridZ = 0;
			break;
		case 7:
			spacing = float_12p5_80421140;
			gridX = (idx - 1) % 2;
			gridY = (idx - 1) / 2;
			gridZ = 0;
			break;
		default:
			break;
		}
		work->posX = (f32)gridX * spacing - spacing * float_0p5_80421144;
		work->posY = (f32)gridY * spacing + spacing * float_0p5_80421144;
		work->posZ = (f32)gridZ * spacing - spacing * float_0p5_80421144;
		work->scale = spacing;
		work->field_0x14 = 0xFF;
		work->field_0x18 = float_0_804210ec;
		work->poseId = -1;
		{
			f32 r = (f32)(irand(0xA) + 0xA);

			work->field_0x20 = r * float_0p0625_80421148;
		}
		{
			f32 zero = ZERO0;
			work->field_0x1C = angleABf(zero, zero, work->posX, work->posZ);
		}
		work->field_0x24 = (f32)(irand(3) + 3);
		work->field_0x28 = (f32)(irand(5) + 2) / float_5_804210fc;
	}
	if (type != 7) {
		// Retail reads work[-1].posY here; work is one past the last element.
		f32 offsetY = (work - 1)->posY;
		f32 scale = (type < 3) ? float_0p5_80421144 : float_1_80421100;

		effKemuTestEntry(2, x, y + offsetY, z, scale);
	}
	else {
		// Retail reads work[0].posY, one past the type-7 allocation (count is 9).
		f32 offsetY = work->posY;

		effKemuTestEntry(0x10, x, y + offsetY, z, float_1_80421100);
	}
	return entry;
}

// Inlines to the same struct-return copy retail shows (scratch vec copied to the local before
// PSMTXRotAxisRad), but the caller still allocates two more callee-saved GPRs and one FPR than
// retail (frame 0x190 vs 0x170), so the matrix stack offsets and register numbering differ.
static inline Vec effMObjBrokenGetAxis(const Vec* base, f32 a) {
	Vec axis = *base;
	f32 angle = float_6p2832_804210f4 * a / float_360_804210f8;

	axis.x = (f32)cos(angle);
	axis.z = (f32)sin(angle);
	return axis;
}

// opt_lifetimes off is required for the polynomial accumulators: the default lifetime analysis
// splits the seed load from the first fmsubs result instead of accumulating in place; with it
// on the function scores 98.33%. Note that with lifetimes off the early-release loop's pointer
// only lands in r27 (not type's r30) when `work` is declared in the later group of locals, next
// to `type`; declared with the other pointers, MWCC reuses type's r30 for it. Declaration order
// drives MWCC's web ordering here.
#pragma opt_lifetimes off
void effMObjBrokenMain(EffectEntry* effect) {
	MobjBrokenWork* data = effect->userdata;
	MobjBrokenWork* work2;
	s32 j;
	Vec position;
	Vec disp;
	s32 i;
	s32 type;
	MobjBrokenWork* work;
	s32 inBattle;
	s32 done = 0;
	f32 remain;

	disp = (Vec){ 0.0f, 0.0f, 0.0f };
	disp.x = data->posX;
	disp.y = data->posY;
	disp.z = data->posZ;
	position = disp;
	type = data->type;
	inBattle = gp->inBattle != 0;
	if (effect->flags & 4) {
		work = effect->userdata;
		for (done = 0; done < effect->count; done++, work++) {
			if (work->poseId != -1) {
				animPoseRelease(work->poseId);
			}
		}
		effDelete(effect);
		return;
	}
	switch (type) {
	case 7:
		if (!animGroupBaseAsync("EFF_sango", inBattle, NULL)) {
			return;
		}
		break;
	}
	{
	f32 kPi, kDeg360, kTwoPi, kOne, k16605, k4967, kMinY;

	kTwoPi = float_6p2832_804210f4;
	kDeg360 = float_360_804210f8;
	k16605 = float_0p16605_80421110;
	kOne = float_1_80421100;
	kPi = float_3p1416_80421114;
	k4967 = float_0p4967_80421124;
	kMinY = float_neg100_80421138;

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		switch (type) {
		case 7:
			if (data->poseId == -1) {
				data->poseId = animPoseEntry("EFF_sango", inBattle);
				animPoseSetAnim(data->poseId, (i & 1) ? str_Z_2 : str_80421108, TRUE);
			}
			break;
		}
		{
			f32 sinA = MATCH_F32(float_0p00761_8042110c);
			f32 angle = kTwoPi * data->field_0x1C / kDeg360;

			if (angle <= kPi) {
				if (angle <= float_1p5708_80421118) {
					sinA = sinA * (angle * angle) - k16605;
					sinA = sinA * (angle * angle) + kOne;
					sinA = sinA * angle;
				}
				else {
					angle = float_1p5708_80421118 - (angle - float_1p5708_80421118);
					sinA = sinA * (angle * angle) - k16605;
					sinA = sinA * (angle * angle) + kOne;
					sinA = sinA * angle;
				}
			}
			else {
				if (angle < float_4p7124_8042111c) {
					angle = angle - kPi;
					sinA = sinA * (angle * angle) - k16605;
					sinA = sinA * (angle * angle) + kOne;
					sinA = sinA * angle;
					sinA = -sinA;
				}
				else {
					angle = float_1p5708_80421118 - (angle - float_4p7124_8042111c);
					sinA = sinA * (angle * angle) - k16605;
					sinA = sinA * (angle * angle) + kOne;
					sinA = sinA * angle;
					sinA = -sinA;
				}
			}
			data->posX = data->field_0x20 * sinA + data->posX;
		}
		{
			f32 angle = kTwoPi * data->field_0x1C / kDeg360;
			f32 cosA = MATCH_F32(float_0p03705_80421120);

			if (angle <= kPi) {
				if (angle <= float_1p5708_80421118) {
					cosA = cosA * (angle * angle) - k4967;
					cosA = cosA * (angle * angle) + kOne;
				}
				else {
					angle = float_1p5708_80421118 - (angle - float_1p5708_80421118);
					cosA = cosA * (angle * angle) - k4967;
					cosA = cosA * (angle * angle) + kOne;
					cosA = -cosA;
				}
			}
			else {
				if (angle < float_4p7124_8042111c) {
					angle = angle - kPi;
					cosA = cosA * (angle * angle) - k4967;
					cosA = cosA * (angle * angle) + kOne;
					cosA = -cosA;
				}
				else {
					angle = float_1p5708_80421118 - (angle - float_4p7124_8042111c);
					cosA = cosA * (angle * angle) - k4967;
					cosA = cosA * (angle * angle) + kOne;
				}
			}
			data->posZ = data->field_0x20 * -cosA + data->posZ;
		}
		data->posY = data->posY + data->field_0x24;
		data->field_0x24 = data->field_0x24 - data->field_0x28;
		if (type < 6) {
			remain = float_20_80421128 - data->field_0x18;
			data->field_0x18 = remain * float_0p125_8042112c + data->field_0x18;
		}
		else {
			remain = float_720_80421130 - data->field_0x18;
			data->field_0x18 = remain * float_0p03125_80421134 + data->field_0x18;
		}
		if (data->posY < kMinY) {
			done++;
		}
	}
	}
	if (done >= effect->count - 1) {
		work2 = effect->userdata;
		for (j = 0; j < effect->count; j++, work2++) {
			if (work2->poseId != -1) {
				animPoseRelease(work2->poseId);
			}
		}
		effDelete(effect);
	}
	else if (type != 7) {
		dispEntry(CAMERA_3D, dispCalcZ(&position), 8, effMObjBrokenDisp, effect);
	}
	else {
		dispEntry(CAMERA_3D, dispCalcZ(&position), 8, effMObjBrokenDisp2, effect);
	}
}
#pragma opt_lifetimes reset


// The original keeps both cube loops' axis scratch vectors in one function-wide block
// (`axis1`/`axis2` and their `tmpAxis` copies) instead of per-loop locals; that is what gives the
// two passes their distinct stack slots. Reproducing the loop-invariant loads also needs the
// MUST_MATCH quirk below.
void effMObjBrokenDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	MobjBrokenWork* data;
	CameraEntry* camera;
	Vec axis1;
	Vec axis2;
	Vec tmpAxis1;
	Vec tmpAxis2;
#ifdef MUST_MATCH
	// volatile keeps MWCC from hoisting the shared zero-Vec load out of the loop.
	volatile const Vec* zeroAxis;
#else
	const Vec* zeroAxis;
#endif
	GXTexObj tex;
	Mtx mtx;
	Mtx rot;
	Mtx scale;
	Mtx view;
	s32 type;
	s32 i;

	data = effect->userdata;
	camera = camGetPtr(camId);
	type = data->type;
	GXSetBlendMode(1, 4, 5, 0);
	GXSetZCompLoc(1);
	GXSetAlphaCompare(7, 0, 0, 7, 0);
	GXSetZMode(0, 3, 0);
	PSMTXTrans(mtx, data->posX, data->posY, data->posZ);
	PSMTXScale(scale, data->scale, data->scale, data->scale);
	PSMTXConcat(mtx, scale, mtx);
	PSMTXConcat(camera->view, mtx, view);
	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 4);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevOp(0, 0);
	GXSetCullMode(2);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xD, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
	if (type < 6) {
		effGetTexObj(0x38 + type % 3, &tex);
		GXLoadTexObj(&tex, 0);
	}
	else {
		effGetTexObj(0x3B, &tex);
		GXLoadTexObj(&tex, 0);
	}
	GXSetChanMatColor(4, (GXColor){ 0x00, 0x00, 0x00, 0x80 });
	zeroAxis = &vec3_802fe634;
	data = effect->userdata;
	data++;
	for (i = 1; i < effect->count; i++, data++) {
		f32 kDegToRad;
		f32 kOffset;
		f32 kDeg360;
		f32 kTwoPi;

		kTwoPi = float_6p2832_804210f4;
		kDeg360 = float_360_804210f8;
		kOffset = float_5_804210fc;
		kDegToRad = float_deg2rad_804210e8;
		tmpAxis1 = *zeroAxis;
		tmpAxis1.x = (f32)cos(kTwoPi * data->field_0x1C / kDeg360);
		tmpAxis1.z = (f32)sin(kTwoPi * data->field_0x1C / kDeg360);
		axis1 = tmpAxis1;
		PSMTXTrans(mtx, kOffset + data->posX, data->posY - kOffset, data->posZ);
		PSMTXScale(scale, data->scale, data->scale, data->scale);
		PSMTXConcat(mtx, scale, mtx);
		PSMTXRotAxisRad(rot, &axis1, kDegToRad * -data->field_0x18);
		PSMTXConcat(mtx, rot, mtx);
		{
			f32 one = ONE1;
			PSMTXScale(scale, one, one, float_0_804210ec);
		}
		PSMTXConcat(mtx, scale, mtx);
		PSMTXConcat(view, mtx, mtx);
		GXLoadPosMtxImm(mtx, 0);
		GXSetCurrentMtx(0);
		GXDrawCube();
	}
	GXSetChanMatColor(4, dat_804210dc);
	zeroAxis = &vec3_802fe640;
	data = effect->userdata;
	data++;
	for (i = 1; i < effect->count; i++, data++) {
		tmpAxis2 = *zeroAxis;
		tmpAxis2.x = (f32)cos(float_6p2832_804210f4 * data->field_0x1C / float_360_804210f8);
		tmpAxis2.z = (f32)sin(float_6p2832_804210f4 * data->field_0x1C / float_360_804210f8);
		axis2 = tmpAxis2;
		PSMTXTrans(mtx, data->posX, data->posY, data->posZ);
		PSMTXScale(scale, data->scale, data->scale, data->scale);
		PSMTXConcat(mtx, scale, mtx);
		PSMTXRotAxisRad(rot, &axis2, float_deg2rad_804210e8 * -data->field_0x18);
		PSMTXConcat(mtx, rot, mtx);
		{
			f32 one = ONE1;
			PSMTXScale(scale, one, one, float_0_804210ec);
		}
		PSMTXConcat(mtx, scale, mtx);
		PSMTXConcat(view, mtx, mtx);
		GXLoadPosMtxImm(mtx, 0);
		GXSetCurrentMtx(0);
		GXDrawCube();
	}
}

static void rendermodeFunc(s32 wXluStage) {
	AnimationWork* wp = animGetPtr();

	wp->currDispMode = wXluStage;
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
}
void effMObjBrokenDisp2(CameraId camId, void* param) {
	EffectEntry* effect = param;
	MobjBrokenWork* data;
	Mtx mtx;
	Mtx scale;
	Mtx rot;
	Mtx view;
	s32 i;
	s32 pass;

	data = effect->userdata;
	camGetPtr(camId);
	PSMTXTrans(mtx, data->posX, data->posY, data->posZ);
	PSMTXRotRad(rot, 'y', float_deg2rad_804210e8 * -camGetPtr(camId)->viewYaw);
	PSMTXScale(scale, data->scale, data->scale, data->scale);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXConcat(mtx, scale, view);
	for (pass = 0; pass < 2; pass++) {
		data = effect->userdata;
		data++;
		for (i = 1; i < effect->count; i++, data++) {
			if (data->poseId == -1) {
				continue;
			}
			if (pass == 0) {
				animPoseSetMaterialFlagOn(data->poseId, 0x40);
				animPoseSetMaterialEvtColor(data->poseId, dat_804210e0);
			}
			else {
				animPoseSetMaterialFlagOn(data->poseId, 0x40);
				animPoseSetMaterialEvtColor(data->poseId, dat_804210e4);
			}
			PSMTXTrans(mtx, data->posX + (1 - pass) * 4, data->posY - (1 - pass) * 4, data->posZ);
			PSMTXScale(scale, data->scale, data->scale, data->scale);
			PSMTXConcat(view, mtx, mtx);
			PSMTXConcat(mtx, scale, mtx);
			animPoseSetGXFunc(data->poseId, rendermodeFunc, 0);
			animPoseMain(data->poseId);
			animPoseDrawMtx(data->poseId, mtx, 1, float_0_804210ec, float_0p8_804210f0);
			animPoseDrawMtx(data->poseId, mtx, 2, float_0_804210ec, float_0p8_804210f0);
			animPoseDrawMtx(data->poseId, mtx, 3, float_0_804210ec, float_0p8_804210f0);
		}
	}
}

// Original data objects of this unit, in address order.
const GXColor dat_804210dc = { 0xFF, 0xFF, 0xFF, 0xFF };
const GXColor dat_804210e0 = { 0x00, 0x00, 0x00, 0x80 };
const GXColor dat_804210e4 = { 0xFF, 0xFF, 0xFF, 0xFE };
const f32 float_deg2rad_804210e8 = 0.017453292f;
const f32 float_0_804210ec = 0.0f;
const f32 float_0p8_804210f0 = 0.8f;
const f32 float_6p2832_804210f4 = 6.2831855f;
const f32 float_360_804210f8 = 360.0f;
const f32 float_5_804210fc = 5.0f;
const f32 float_1_80421100 = 1.0f;
const char str_Z_2[] = "Z_2";
const char str_80421108[] = "Z_1";
const f32 float_0p00761_8042110c = 0.00761f;
const f32 float_0p16605_80421110 = 0.16605f;
const f32 float_3p1416_80421114 = 3.1415927f;
const f32 float_1p5708_80421118 = 1.5707964f;
const f32 float_4p7124_8042111c = 4.712389f;
const f32 float_0p03705_80421120 = 0.03705f;
const f32 float_0p4967_80421124 = 0.4967f;
const f32 float_20_80421128 = 20.0f;
const f32 float_0p125_8042112c = 0.125f;
const f32 float_720_80421130 = 720.0f;
const f32 float_0p03125_80421134 = 0.03125f;
const f32 float_neg100_80421138 = -100.0f;
const f32 float_6p25_8042113c = 6.25f;
const f32 float_12p5_80421140 = 12.5f;
const f32 float_0p5_80421144 = 0.5f;
const f32 float_0p0625_80421148 = 0.0625f;
const Vec vec3_802fe634 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802fe640 = { 0.0f, 0.0f, 0.0f };
