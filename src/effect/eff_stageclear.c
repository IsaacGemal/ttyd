#include "eff/eff_stageclear.h"
#include "driver/dispdrv.h"
#include "eff/eff_confetti.h"
#include "eff/eff_minigame.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "mario/mariost.h"
#include "math_ext.h"
#include "memory.h"
#include "pmario_sound.h"
#include "system.h"
#include <stdlib.h>
#include "matching.h"
#include <dolphin/mtx.h>
#include <dolphin/gx.h>

extern const f32 float_0p9_80420f18;
extern const f32 float_1p7_80420f1c;
extern const f32 float_0p6_80420f20;
extern const f32 float_48_80420f24;
extern const f32 float_0_80420f28;
extern const f32 float_1_80420f2c;
extern const f32 float_25_80420f30;
extern const f32 float_640_80420f34;
extern const f32 float_0p5_80420f38;
extern const f32 float_24_80420f3c;
extern const f32 float_100_80420f40;
extern const f32 float_0p2_80420f44;
extern const u32 dat_80420f10;
extern const u32 dat_80420f14;

// .rodata
const Vec vec3_802fe3c0 = { 0.0f, 0.0f, 0.0f };
#ifdef MUST_MATCH
// The non-const forms stop MWCC hoisting the loop-invariant cc/d8/e4 field loads out of the inner loop
// (with const it preloads them into r23-r30 and the whole function's allocation shifts); the section
// declspec keeps them in .rodata so the unit's section layout is unchanged.
__declspec(section ".rodata") Vec vec3_802fe3cc = { 0.0f, 0.0f, 0.0f };
__declspec(section ".rodata") Vec vec3_802fe3d8 = { 0.0f, 1.0f, 0.0f };
__declspec(section ".rodata") Vec vec3_802fe3e4 = { 0.0f, 0.0f, 0.0f };
#else
const Vec vec3_802fe3cc = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802fe3d8 = { 0.0f, 1.0f, 0.0f };
const Vec vec3_802fe3e4 = { 0.0f, 0.0f, 0.0f };
#endif
const f64 double_0p21817_802fe3f0 = 0.21816615760326385;

#ifdef MUST_MATCH
// Immediately generated helpers pool the int->float conversion doubles ahead of the deferred functions'
// "StageClear" string; nothing calls them and the linker strips the bodies.
#pragma defer_codegen off // Pool the conversion doubles before the deferred functions' string literals.
static f32 pool_i2f(s32 v) {
	f32 a = (f32)v;
	f32 b = (f32)(u32)v;

	return a + b;
}
#pragma defer_codegen reset
#endif

// .data
static u32 tex_us[13] = {
	0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA1, 0xA2, 0xA0, 0x9E, 0xA3, 0xA0, 0xA4, 0xA5,
};
u32 tex_shadow_us[13] = {
	0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAA, 0xA8, 0xAD, 0xAA, 0xAE, 0xAF,
};

// .sdata
static u32 tex_jp[8] = { 0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA1, 0xA2, 0xA3 };
u32 tex_shadow_jp[8] = { 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB };
static u32* texid_tbl[4] = { tex_jp, tex_shadow_jp, tex_us, tex_shadow_us };
static u32 niji_tbl[2] = { 0xAC, 0xB0 };

// .sdata: shake_dt/shake_dt2 come before scale_dt, matching the original's address order
s32 shake_dt[5] = { 2, 6, 2, 0, -1 };
s32 shake_dt2[5] = { 0xC, 0x24, 0xC, 0, -1 };
static u8 scale_dt[12] = {
	0x00, 0x00, 0x16, 0x2E, 0x47, 0x5D, 0x6E, 0x78, 0x77, 0x6E, 0x66, 0x00,
};
// The .sdata trailing 12 zero bytes (target gap_07_80415C34_sdata): explicit_zero_data keeps the
// all-zero object in .sdata and force_active keeps the linker from dropping it.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_80415C34_sdata[3] = { 0 };
#pragma force_active reset
#pragma explicit_zero_data reset

// .data
static s32 evt_shake[] = {
	EVT_CMD(2, OPCODE_IF_EQUAL), 0xFE363C81, 0x00000000,
	EVT_CMD(5, OPCODE_USER_FUNC), (s32)evt_cam_shake, 0x00000005, FLOAT(0.009765625f), FLOAT(0.009765625f), 0x00000032,
	EVT_CMD(0, OPCODE_ELSE),
	EVT_CMD(5, OPCODE_USER_FUNC), (s32)evt_cam_shake, 0x00000005, FLOAT(0.01953125f), FLOAT(0.01953125f), 0x00000064,
	EVT_CMD(0, OPCODE_END_IF),
	EVT_CMD(0, OPCODE_END_EVENT),
	OPCODE_END_SCRIPT
};

// The .data trailing 12 zero bytes (target gap_05_803ADD14_data), same recipe as above.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".data") u32 gap_05_803ADD14_data[3] = { 0 };
#pragma force_active reset
#pragma explicit_zero_data reset

EffectEntry* effStageClearEntry(s32 type, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	effStageClearWork* data;
	s32 count = 9;
	s32 i;

	if (type == 1 && (u32)gp->language != 0) {
		return effMiniGameEntry(1, x, y, z);
	}
	entry = effEntry();
	if ((u32)gp->language != 0) {
		count = 0xE;
	}
	if (type % 10 == 1) {
		count = 7;
	}
	entry->type = "StageClear";
	entry->count = count;
	data = __memAlloc(HEAP_EFFECT, entry->count * sizeof(effStageClearWork));
	entry->userdata = data;
	entry->callback = effStageClearMain;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = float_1_80420f2c;
	data->timer = 0;
	data->field_0x1C = 0;
	data->count = irand(5) + 1;
	data->done = 0;
	for (i = 1; i < entry->count; i++) {
		effStageClearWork* work = &data[i];

		work->x = (f32)((i - 1) * 0x26 - (entry->count - 1) * 0x26 / 2 + 0x14);
		work->y = float_0_80420f28;
		work->z = float_0_80420f28;
		work->scale = float_0_80420f28;
		work->timer = (s32)((f32)i * (float_0p2_80420f44 * gp->framerate));
		work->field_0x1C = 0;
		work->evt = NULL;
	}
	return entry;
}

static inline Vec initializeStageClearVec(Vec* input) {
	Vec temp = vec3_802fe3c0;
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

void effStageClearMain(EffectEntry* effect) {
	effStageClearWork* data = effect->userdata;
	Vec disp = initializeStageClearVec((Vec*)&data->x);
	s32 i;
	s32 type = data->type;
	s32 retired = 0;

	data->field_0x1C++;
	data++;
	for (i = 1; i < effect->count; i++, data++) {
		if (data->timer != 0) {
			data->timer--;
			if (data->timer == 0) {
				psndSFXOn((const char*)(type == 0 ? 0x29B : 0x82E));
			}
		} else {
			if ((u32)data->field_0x1C < 0xB) {
				data->scale = (f32)scale_dt[data->field_0x1C] / float_100_80420f40;
			}
			data->field_0x1C++;
			if (data->scale > float_0p9_80420f18) {
				if (data->evt == NULL) {
					s32* dt;

					data->evt = evtEntry(evt_shake, 0, 0);
					if (i == effect->count - 1) {
						dt = shake_dt2;
					} else {
						dt = shake_dt;
					}
					data->evt->lwData[0] = (s32)dt;
					data->evt->lwData[1] = (i == effect->count - 1) ? 1 : 0;
				}
				retired++;
			}
		}
	}
	if (retired >= effect->count - 1) {
		data = effect->userdata;
		data->done = 1;
	}
	if (type % 10 == 1 && retired >= effect->count - 1) {
		if (gp->retraceCount % 0x3C == 0) {
			data = effect->userdata;
			if (data->count > 0) {
				f32 x;
				f32 y;

				y = disp.y + (f32)(100 - irand(200));
				x = disp.x + (f32)(200 - irand(400));
				effConfettiEntry(rand() % 3 + 0x18, 0x3C, x, y, disp.z);
				psndSFXOn((const char*)0x82F);
				data->count--;
			}
		}
	}
	if (type < 10) {
		dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effStageClearDisp, effect);
	} else {
		dispEntry(CAMERA_3D_EFFECTA, dispCalcZ(&disp), 8, effStageClearDisp, effect);
	}
}

static inline Vec getStageClearCamPos(void) {
	Vec temp = vec3_802fe3cc;
#ifdef MUST_MATCH
	// Read the named double so the load comes off the base register instead of a pooled copy of it.
	temp.z = float_1_80420f2c / (f32)tan(*(volatile const f64*)&double_0p21817_802fe3f0);
#else
	temp.z = float_1_80420f2c / (f32)tan(double_0p21817_802fe3f0);
#endif
	return temp;
}

#pragma opt_common_subs off // without it MWCC CSEs the repeated float_0 pool loads in the GX vertex block
// Register colouring only, no structural or pool difference left. The long-lived
// FP values land in f31 (int->float magic), f30 (24), f29 (-24) and f28 (1) where the target has f30
// (magic), f31 (1), f29 (-24) and f28 (24); the target reuses f28 from the free `scale` for 24, and the
// two uncached float_1 texcoords follow from it. The outer loop counter/work pointers also land in
// r23/r21 where the target has r21/r23. Computing the inner `work` pointer in place (load userdata
// into work, then work++) instead of through &(...)[1] fixed the last scratch-register diffs; no
// declaration or statement order, loop shape, or pragma (opt_lifetimes, opt_dead_assignments,
// opt_propagation, scheduling, global_optimizer, optimization_level 0-4) moved the rest. The int->float
// magic and 0.2181 double pool at the target's 0x38/0x30 slots via the helper above and the named
// double. Also rejected (fresh scores, none above 99.40%): a K&R definition, defining the named floats
// so they fold, ~8000 random combinations of the texcoord MATCH mask, the constant hoist points, the
// declaration/assignment orders and every other work-pointer form, and compiling the unit with
// GC/1.3.2-2.7 (all six give this exact colouring; 3.0a3+ reorders everything). The target's four
// texcoords all reload float_1 while ours keep it in a register for the last two, so the target's 1.0
// is live only to the fdiv.
// NONMATCHING (99.40%): FP/GPR colouring only; the permuter is needed.
void effStageClearDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effStageClearWork* data = effect->userdata;
	CameraEntry* cam;
	CameraEntry* cam2;
	s32 field1C;
	s32 shadow;
	s32 i, j;
	s32 type;
	f32 scale;
	f32 f27;
	f32 f26;
	f32 f28;
	f32 f29;
	f32 f30;
	f32 f31;
	GXTexObj tex;
	f32 proj[7];
	f32 viewport[6];
	Mtx trans;
	Mtx scaleMtx;
	Mtx mtx;
	Mtx persp;
	Mtx lookAt;

	cam = camGetPtr(camId);
	scale = float_1p7_80420f1c * data->scale;
	scale = float_0p9_80420f18 * scale;
	field1C = data->field_0x1C;
	type = data->type;
	if ((u32)gp->language != 0) {
		scale = scale * float_0p6_80420f20;
	}
	GXGetProjectionv(proj);
	GXGetViewportv(viewport);
	cam2 = camGetPtr(CAMERA_2D);
	GXSetProjection(cam2->projection, cam2->projType);
	GXSetBlendMode(1, 4, 5, 0);
	GXSetZCompLoc(1);
	GXSetAlphaCompare(7, 0, 0, 7, 0);
	GXSetZMode(0, 3, 0);
	PSMTXTrans(trans, data->x, data->y, data->z);
	PSMTXScale(scaleMtx, scale, scale, scale);
	PSMTXConcat(trans, scaleMtx, mtx);
	GXSetNumChans(0);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
	GXSetTexCoordGen2(1, 1, 0, 0x1E, 0, 0x7D);
	GXSetCullMode(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xD, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
	shadow = type % 10;
#ifdef MUST_MATCH
	// match-only: assigning the loop-invariant i*4/-i*4 offsets and the 24/-24/1 constants inside the
	// inner loop makes MWCC hoist them together with the int->float magic, reproducing the target's FP
	// load order; the natural form is the #else arm (same values, just outside the loop)
	for (i = 1; i >= 0; i--) {
		effStageClearWork* work = (effStageClearWork*)effect->userdata;
		work++;

		for (j = 1; j < effect->count; j++, work++) {
			f27 = (f32)(i * 4);
			f26 = (f32)(-i * 4);
			f29 = -float_48_80420f24 * float_0p5_80420f38;
			f31 = float_1_80420f2c;
			f28 = float_24_80420f3c;
#else
	f29 = -float_48_80420f24 * float_0p5_80420f38;
	f31 = float_1_80420f2c;
	f28 = float_24_80420f3c;
	for (i = 1; i >= 0; i--) {
		effStageClearWork* work = &data[1];

		for (j = 1; j < effect->count; j++, work++) {
			f27 = (f32)(i * 4);
			f26 = (f32)(-i * 4);
#endif
			if (i != 0) {
				if (shadow == 0) {
					u32* tbl = texid_tbl[(u32)gp->language * 2 + 1];

					effGetTexObj(tbl[j - 1], &tex);
					GXLoadTexObj(&tex, 0);
				} else {
					effGetTexObj(j + 0x4F, &tex);
					GXLoadTexObj(&tex, 0);
				}
				GXSetNumTevStages(1);
				GXSetTevOrder(0, 0, 0, 0xFF);
				GXSetTevColorOp(0, 0, 0, 0, 1, 0);
				GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
				GXSetTevColorIn(0, 0xF, 8, 2, 0xF);
				GXSetTevAlphaIn(0, 7, 4, 1, 7);
				{
					GXColor color = *(GXColor*)&dat_80420f10;
					GXSetTevColor(1, color);
				}
			} else {
				if (shadow == 0) {
					u32* tbl = texid_tbl[(u32)gp->language * 2];

					effGetTexObj(tbl[j - 1], &tex);
					GXLoadTexObj(&tex, 0);
					effGetTexObj(niji_tbl[(u32)gp->language != 0], &tex);
					GXLoadTexObj(&tex, 1);
					GXSetNumTevStages(3);
					GXSetTevOrder(0, 0, 0, 0xFF);
					GXSetTevColorOp(0, 0xE, 0, 0, 1, 0);
					GXSetTevAlphaOp(0, 0xE, 0, 0, 1, 0);
					GXSetTevColorIn(0, 8, 0xF, 8, 0xF);
					GXSetTevAlphaIn(0, 4, 7, 4, 7);
					GXSetTevSwapModeTable(1, 0, 1, 2, 0);
					GXSetTevSwapMode(0, 1, 1);
					GXSetTevOrder(1, 1, 1, 0xFF);
					GXSetTevColorOp(1, 0, 0, 0, 1, 0);
					GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
					GXSetTevColorIn(1, 0xF, 0, 8, 0xF);
					GXSetTevAlphaIn(1, 7, 7, 7, 0);
					GXSetTevOrder(2, 0, 0, 0xFF);
					GXSetTevColorOp(2, 0, 0, 0, 1, 0);
					GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
					GXSetTevColorIn(2, 2, 0, 1, 0xF);
					GXSetTevAlphaIn(2, 7, 7, 7, 4);
					{
						GXColor color = *(GXColor*)&dat_80420f14;
						GXSetTevColor(1, color);
					}
				} else {
					effGetTexObj(j + 0x49, &tex);
					GXLoadTexObj(&tex, 0);
					GXSetNumTevStages(1);
					GXSetTevOrder(0, 0, 0, 0xFF);
					GXSetTevOp(0, 3);
				}
			}
			PSMTXTrans(trans, work->x + f27, work->y + f26, 0.0f);
			PSMTXScale(scaleMtx, work->scale, work->scale, work->scale);
			PSMTXConcat(mtx, trans, trans);
			PSMTXConcat(trans, scaleMtx, trans);
			PSMTXConcat(cam2->view, trans, scaleMtx);
			GXLoadPosMtxImm(scaleMtx, 0);
			GXSetCurrentMtx(0);
			{
				Vec camPos = getStageClearCamPos();
				Vec camUp = vec3_802fe3d8;
				Vec target = vec3_802fe3e4;

				C_MTXLightPerspective(persp, float_25_80420f30, float_640_80420f34,
					float_0p5_80420f38, float_0p5_80420f38,
					float_0p5_80420f38, float_0p5_80420f38);
				C_MTXLookAt(lookAt, &camPos, &camUp, &target);
				PSMTXConcat(persp, lookAt, persp);
				PSMTXConcat(persp, trans, trans);
				PSMTXTrans(scaleMtx, (f32)-field1C, 0.0f, 0.0f);
				PSMTXConcat(trans, scaleMtx, trans);
				GXLoadTexMtxImm(trans, 0x1E, 1);
				GXBegin(0x80, 0, 4);
				GXPosition3f32(f29, f28, 0.0f);
				GXTexCoord2f32(0.0f, 0.0f);
				GXPosition3f32(f28, f28, 0.0f);
				GXTexCoord2f32(MATCH_F32(float_1_80420f2c), 0.0f);
				GXPosition3f32(f28, f29, 0.0f);
				GXTexCoord2f32(MATCH_F32(float_1_80420f2c), float_1_80420f2c);
				GXPosition3f32(f29, f29, 0.0f);
				GXTexCoord2f32(0.0f, float_1_80420f2c);
			}
		}
	}
	GXSetTevSwapModeTable(1, 0, 1, 2, 3);
	GXSetProjectionv(proj);
	GXSetViewport(viewport[0], viewport[1], viewport[2], viewport[3], viewport[4], viewport[5]);
}

#pragma opt_common_subs reset

BOOL effStageClearEndChk(EffectEntry* effect) {
	effStageClearWork* data = effect->userdata;
	return data->done != 0;
}

// .sdata2: the two GXColor words and the float constants, defined after the code so MWCC keeps them as
// real objects rather than folding them into the function's literal pool.
const u32 dat_80420f10 = 0x00000080;
const u32 dat_80420f14 = 0x363636ff;
const f32 float_0p9_80420f18 = 0.9f;
const f32 float_1p7_80420f1c = 1.7f;
const f32 float_0p6_80420f20 = 0.6f;
const f32 float_48_80420f24 = 48.0f;
const f32 float_0_80420f28 = 0.0f;
const f32 float_1_80420f2c = 1.0f;
const f32 float_25_80420f30 = 25.0f;
const f32 float_640_80420f34 = 640.0f;
const f32 float_0p5_80420f38 = 0.5f;
const f32 float_24_80420f3c = 24.0f;
const f32 float_100_80420f40 = 100.0f;
const f32 float_0p2_80420f44 = 0.2f;
