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
extern const Vec vec3_802fe628;
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
extern const GXColor dat_804210d8;
extern const GXColor dat_804210dc;

// .rodata (0x802FE628..0x802FE670)
const char str_80421104[] = "Z_2";
const char str_80421108[] = "Z_1";
const char str_802FE658[0xC] = "EFF_sango";
const char str_802FE664[0xC] = "MObjBroken";

static void rendermodeFunc(s32 wXluStage) {
	AnimationWork* wp = animGetPtr();

	wp->currDispMode = wXluStage;
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
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

// NONMATCHING (87.36%): see effMObjBrokenGetAxis note; the two cube passes and GX state match.
void effMObjBrokenDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	MobjBrokenWork* data;
	CameraEntry* camera;
	GXTexObj tex;
	Mtx mtx;
	Mtx scale;
	Mtx rot;
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
	}
	else {
		effGetTexObj(0x3B, &tex);
	}
	GXLoadTexObj(&tex, 0);
	GXSetChanMatColor(4, dat_804210d8);
	data = effect->userdata;
	data++;
	for (i = 1; i < effect->count; i++, data++) {
		Vec axis = effMObjBrokenGetAxis(&vec3_802fe634, data->field_0x1C);
		PSMTXTrans(mtx, data->posX + float_5_804210fc, data->posY - float_5_804210fc, data->posZ);
		PSMTXScale(scale, data->scale, data->scale, data->scale);
		PSMTXConcat(mtx, scale, mtx);
		PSMTXRotAxisRad(rot, &axis, float_deg2rad_804210e8 * -data->field_0x18);
		PSMTXConcat(mtx, rot, mtx);
		PSMTXScale(scale, 1.0f, 1.0f, 0.0f);
		PSMTXConcat(mtx, scale, mtx);
		PSMTXConcat(view, mtx, mtx);
		GXLoadPosMtxImm(mtx, 0);
		GXSetCurrentMtx(0);
		GXDrawCube();
	}
	GXSetChanMatColor(4, dat_804210dc);
	data = effect->userdata;
	data++;
	for (i = 1; i < effect->count; i++, data++) {
		Vec axis = effMObjBrokenGetAxis(&vec3_802fe640, data->field_0x1C);
		PSMTXTrans(mtx, data->posX, data->posY, data->posZ);
		PSMTXScale(scale, data->scale, data->scale, data->scale);
		PSMTXConcat(mtx, scale, mtx);
		PSMTXRotAxisRad(rot, &axis, float_deg2rad_804210e8 * -data->field_0x18);
		PSMTXConcat(mtx, rot, mtx);
		PSMTXScale(scale, 1.0f, 1.0f, 0.0f);
		PSMTXConcat(mtx, scale, mtx);
		PSMTXConcat(view, mtx, mtx);
		GXLoadPosMtxImm(mtx, 0);
		GXSetCurrentMtx(0);
		GXDrawCube();
	}
}

void effMObjBrokenDisp2(CameraId camId, void* param) {
	EffectEntry* effect = param;
	MobjBrokenWork* data;
	Mtx mtx;
	Mtx scale;
	Mtx rot;
	Mtx view;
	s32 i;
	s32 j;

	data = effect->userdata;
	camGetPtr(camId);
	PSMTXTrans(mtx, data->posX, data->posY, data->posZ);
	PSMTXRotRad(rot, 'y', float_deg2rad_804210e8 * -camGetPtr(camId)->viewYaw);
	PSMTXScale(scale, data->scale, data->scale, data->scale);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXConcat(mtx, scale, view);
	for (j = 0; j < 2; j++) {
		data = effect->userdata;
		data++;
		for (i = 1; i < effect->count; i++, data++) {
			if (data->poseId == -1) {
				continue;
			}
			if (j == 0) {
				animPoseSetMaterialFlagOn(data->poseId, 0x40);
				animPoseSetMaterialEvtColor(data->poseId, dat_804210e0);
			}
			else {
				animPoseSetMaterialFlagOn(data->poseId, 0x40);
				animPoseSetMaterialEvtColor(data->poseId, dat_804210e4);
			}
			PSMTXTrans(mtx, data->posX + (1 - j) * 4, data->posY - (1 - j) * 4, data->posZ);
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
	f32 d;

	disp = vec3_802fe628;
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
		if (!animGroupBaseAsync(str_802FE658, inBattle, NULL)) {
			return;
		}
		break;
	}
	{
	f32 kpi, k360, k2pi, k1, k16605, k4967, kneg100;

	k2pi = float_6p2832_804210f4;
	k360 = float_360_804210f8;
	k16605 = float_0p16605_80421110;
	k1 = float_1_80421100;
	kpi = float_3p1416_80421114;
	k4967 = float_0p4967_80421124;
	kneg100 = float_neg100_80421138;

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		switch (type) {
		case 7:
			if (data->poseId == -1) {
				data->poseId = animPoseEntry(str_802FE658, inBattle);
				animPoseSetAnim(data->poseId, (i & 1) ? str_80421104 : str_80421108, TRUE);
			}
			break;
		}
		{
			f32 s = MATCH_F32(float_0p00761_8042110c);
			f32 angle = k2pi * data->field_0x1C / k360;

			if (angle <= kpi) {
				if (angle <= float_1p5708_80421118) {
					s = s * (angle * angle) - k16605;
					s = s * (angle * angle) + k1;
					s = s * angle;
				}
				else {
					angle = float_1p5708_80421118 - (angle - float_1p5708_80421118);
					s = s * (angle * angle) - k16605;
					s = s * (angle * angle) + k1;
					s = s * angle;
				}
			}
			else {
				if (angle < float_4p7124_8042111c) {
					angle = angle - kpi;
					s = s * (angle * angle) - k16605;
					s = s * (angle * angle) + k1;
					s = s * angle;
					s = -s;
				}
				else {
					angle = float_1p5708_80421118 - (angle - float_4p7124_8042111c);
					s = s * (angle * angle) - k16605;
					s = s * (angle * angle) + k1;
					s = s * angle;
					s = -s;
				}
			}
			data->posX = data->field_0x20 * s + data->posX;
		}
		{
			f32 angle = k2pi * data->field_0x1C / k360;
			f32 c = MATCH_F32(float_0p03705_80421120);

			if (angle <= kpi) {
				if (angle <= float_1p5708_80421118) {
					c = c * (angle * angle) - k4967;
					c = c * (angle * angle) + k1;
				}
				else {
					angle = float_1p5708_80421118 - (angle - float_1p5708_80421118);
					c = c * (angle * angle) - k4967;
					c = c * (angle * angle) + k1;
					c = -c;
				}
			}
			else {
				if (angle < float_4p7124_8042111c) {
					angle = angle - kpi;
					c = c * (angle * angle) - k4967;
					c = c * (angle * angle) + k1;
					c = -c;
				}
				else {
					angle = float_1p5708_80421118 - (angle - float_4p7124_8042111c);
					c = c * (angle * angle) - k4967;
					c = c * (angle * angle) + k1;
				}
			}
			data->posZ = data->field_0x20 * -c + data->posZ;
		}
		data->posY = data->posY + data->field_0x24;
		data->field_0x24 = data->field_0x24 - data->field_0x28;
		if (type < 6) {
			d = float_20_80421128 - data->field_0x18;
			data->field_0x18 = d * float_0p125_8042112c + data->field_0x18;
		}
		else {
			d = float_720_80421130 - data->field_0x18;
			data->field_0x18 = d * float_0p03125_80421134 + data->field_0x18;
		}
		if (data->posY < kneg100) {
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

EffectEntry* effMObjBrokenEntry(s32 type, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	MobjBrokenWork* data;
	MobjBrokenWork* work;
	s32 i;
	s32 idx;
	s32 r27;
	s32 r26;
	s32 r25;
	f32 f29;
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
	entry->type = str_802FE664;
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
			f29 = float_6p25_8042113c;
			r27 = (idx - 1) % 4 / 2;
			r26 = (idx - 1) / 4;
			r25 = (idx - 1) % 4 % 2;
			break;
		case 3:
		case 4:
		case 5:
			f29 = float_12p5_80421140;
			r27 = (idx - 1) % 4 / 2;
			r26 = (idx - 1) / 4;
			r25 = (idx - 1) % 4 % 2;
			break;
		case 6:
			f29 = float_12p5_80421140;
			r27 = (idx - 1) % 2;
			r26 = (idx - 1) / 2;
			r25 = 0;
			break;
		case 7:
			f29 = float_12p5_80421140;
			r27 = (idx - 1) % 2;
			r26 = (idx - 1) / 2;
			r25 = 0;
			break;
		default:
			break;
		}
		work->posX = (f32)r27 * f29 - f29 * float_0p5_80421144;
		work->posY = (f32)r26 * f29 + f29 * float_0p5_80421144;
		work->posZ = (f32)r25 * f29 - f29 * float_0p5_80421144;
		work->scale = f29;
		work->field_0x14 = 0xFF;
		work->field_0x18 = float_0_804210ec;
		work->poseId = -1;
		{
			f32 r = (f32)(irand(0xA) + 0xA);

			work->field_0x20 = r * float_0p0625_80421148;
		}
		work->field_0x1C = angleABf(0.0f, 0.0f, work->posX, work->posZ);
		work->field_0x24 = (f32)(irand(3) + 3);
		work->field_0x28 = (f32)(irand(5) + 2) / float_5_804210fc;
	}
	if (type != 7) {
		// Retail reads work[-1].posY here; work is one past the last element.
		f32 f0 = (work - 1)->posY;
		f32 scale = (type < 3) ? float_0p5_80421144 : float_1_80421100;

		effKemuTestEntry(2, x, y + f0, z, scale);
	}
	else {
		// Retail reads work[0].posY, one past the type-7 allocation (count is 9).
		f32 f0 = work->posY;

		effKemuTestEntry(0x10, x, y + f0, z, float_1_80421100);
	}
	return entry;
}
