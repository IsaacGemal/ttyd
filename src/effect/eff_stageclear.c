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
extern const f64 double_0p21817_802fe3f0;

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
static EVT_BEGIN(evt_shake)
    IF_INT_EQUAL(LW(1), 0)
        CALL(evt_cam_shake, 5, FLOAT(0.01f), FLOAT(0.01f), 50)
    ELSE
        CALL(evt_cam_shake, 5, FLOAT(0.02f), FLOAT(0.02f), 100)
    END_IF
    RETURN
EVT_END()

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
	data->scale = 1.0f;
	data->timer = 0;
	data->field_0x1C = 0;
	data->count = irand(5) + 1;
	data->done = 0;
	for (i = 1; i < entry->count; i++) {
		effStageClearWork* work = &data[i];

		work->x = (f32)((i - 1) * 0x26 - (entry->count - 1) * 0x26 / 2 + 0x14);
		work->y = 0.0f;
		work->z = 0.0f;
		work->scale = 0.0f;
		work->timer = (s32)((f32)i * (0.2f * gp->framerate));
		work->field_0x1C = 0;
		work->evt = NULL;
	}
	return entry;
}

static inline Vec initializeStageClearVec(Vec* input) {
	Vec temp = (Vec){ input->x, input->y, input->z };
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
				data->scale = (f32)scale_dt[data->field_0x1C] / 100.f;
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
					data->evt->lwData[1] = i == effect->count - 1;
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
#ifdef MUST_MATCH
	// Read the named double so the load comes off the base register instead of a pooled copy of it.
	Vec temp = (Vec){ 0.0f, 0.0f, 1.0f / (f32)tan(*(volatile const f64*)&double_0p21817_802fe3f0) };
#else
	Vec temp = (Vec){ 0.0f, 0.0f, 1.0f / (f32)tan(double_0p21817_802fe3f0) };
#endif
	return temp;
}

// The constants after the first four .sdata2 floats are literals the compiler pools in first-use order;
// the 48/2 half-extent is computed at runtime from the named 48 and a `half` local, as the original does.
// The declaration order of shadow/work/j/i/type gives the original's GPR colouring.
void effStageClearDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effStageClearWork* data = effect->userdata;
	CameraEntry* cam;
	CameraEntry* cam2;
	s32 field1C;
	s32 shadow;
	effStageClearWork* work;
	s32 j;
	s32 i;
	s32 type;
	f32 scale;
	f32 f27;
	f32 f26;
	f32 lo;
	f32 hi;
	f32 half;
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
	for (i = 1; i >= 0; i--) {
		work = effect->userdata;
		work++;

		for (j = 1; j < effect->count; j++, work++) {
			f27 = (f32)(i * 4);
			f26 = (f32)(-i * 4);
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
				GXSetTevColor(1, (GXColor){ 0x00, 0x00, 0x00, 0x80 });
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
					GXSetTevColor(1, (GXColor){ 0x36, 0x36, 0x36, 0xFF });
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
				Vec camUp = { 0.0f, 1.0f, 0.0f };
				Vec target = { 0.0f, 0.0f, 0.0f };

				C_MTXLightPerspective(persp, 25.0f, 640.f,
					0.5f, 0.5f,
					0.5f, 0.5f);
				C_MTXLookAt(lookAt, &camPos, &camUp, &target);
				PSMTXConcat(persp, lookAt, persp);
				PSMTXConcat(persp, trans, trans);
				PSMTXTrans(scaleMtx, (f32)-field1C, 0.0f, 0.0f);
				PSMTXConcat(trans, scaleMtx, trans);
				GXLoadTexMtxImm(trans, 0x1E, 1);
				half = 0.5f;
				lo = -float_48_80420f24 * half;
				hi = 24.0f;
				GXBegin(0x80, 0, 4);
				GXPosition3f32(lo, hi, 0.0f);
				GXTexCoord2f32(0.0f, 0.0f);
				GXPosition3f32(hi, hi, 0.0f);
				GXTexCoord2f32(1.0f, 0.0f);
				GXPosition3f32(hi, lo, 0.0f);
				GXTexCoord2f32(1.0f, 1.0f);
				GXPosition3f32(lo, lo, 0.0f);
				GXTexCoord2f32(0.0f, 1.0f);
			}
		}
	}
	GXSetTevSwapModeTable(1, 0, 1, 2, 3);
	GXSetProjectionv(proj);
	GXSetViewport(viewport[0], viewport[1], viewport[2], viewport[3], viewport[4], viewport[5]);
}


BOOL effStageClearEndChk(EffectEntry* effect) {
	effStageClearWork* data = effect->userdata;
	return data->done != 0;
}

// .rodata: the double follows the Vec templates.
const f64 double_0p21817_802fe3f0 = 0.21816615760326385;

// .sdata2: the float constants, defined after the code so MWCC keeps them as real objects rather than
// folding them into the function's literal pool.
const f32 float_0p9_80420f18 = 0.9f;
const f32 float_1p7_80420f1c = 1.7f;
const f32 float_0p6_80420f20 = 0.6f;
const f32 float_48_80420f24 = 48.0f;
