#include "eff/n64/eff_akari_sflush_n64.h"
#include "eff/eff_n64.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "math_ext.h"
#include "driver/dispdrv.h"
#include "matching.h"
#include "memory.h"

// .sdata2 constants owned by this unit (named after their original addresses).
extern const f32 float_deg2rad_8041e904;
extern const f32 float_6p2832_8041e908;
extern const f32 float_360_8041e90c;
extern const f32 float_0p1_8041e910;
extern const f32 float_1_8041e914;
extern const f32 float_30_8041e918;
extern const f32 float_0_8041e91c;
extern GXColor dat_8041e900;
// .sbss2 scratch GXColor the original builds its TEV colour from.
extern GXColor unk_804230a0;
// .rodata integer-to-float conversion constant.
extern const f64 double_to_int_802fa1f0;

// .data vertex array, local to this unit (0x54 bytes).
static u32 size16x16_tex32x32_vtx[0x54 / 4] = {
    0x00100000, 0x00000000, 0x00000000, 0x00FF0003, 0x00040000, 0x00000000, 0x000000FF,
    0x00000008, 0x00000000, 0x00000000, 0x00FF0000, 0xFFF80000, 0x00000000, 0x000000FF,
    0xFFFDFFFC, 0x00000000, 0x0000C8C8, 0xC8FFFFF0, 0x00000000, 0x00000000, 0xFFFFFFFF,
};

EffectEntry* effAkariSflushN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	EffAkariSflushData* data;

	effect = effEntry();
	effect->type = "AkariSflushN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(EffAkariSflushData));
	effect->userdata = data;
	effect->callback = effAkariSflushMain;
	effect->flags |= 2;
	data->type = type;
	data->field_0x14 = 0;
	if (count <= 0) {
		data->field_0x10 = 1000;
	} else {
		data->field_0x10 = count;
	}
	data->field_0x24 = 0xFF;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->field_0x18 = 0xFF;
	data->field_0x1C = 0xE6;
	data->field_0x20 = 0x32;
	data->field_0x30 = 0;
	data->field_0x2C = 1.0f;
	return effect;
}

void effAkariSflushMain(EffectEntry* effect) {
	EffAkariSflushData* data = effect->userdata;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second
	   local before dispCalcZ; a single local folds the copy away (same idiom as
	   effGlassMain/effFlameMain). */
	Vec position;
#endif
	Vec disp = { 0 };
	s32 life;
	s32 counter;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
#ifdef MUST_MATCH
	position = disp;
#endif

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->field_0x10 = 16;
	}

	if (data->field_0x10 < 1000) {
		data->field_0x10--;
	}
	data->field_0x14++;

	life = data->field_0x10;
	if (life < 0) {
		effDelete(effect);
		return;
	}

	counter = data->field_0x14;

	if (life < 8) {
		data->field_0x2C += float_0p3_8041e920 * (8 - life);
	}

	if (counter < 16) {
		data->field_0x24 = counter * 16 + 15;
	}
	if (life < 8) {
		data->field_0x24 = life * 32 + 31;
	}

#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effAkariSflushDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effAkariSflushDisp, effect);
#endif
}

// The 1.0f/30.0f/0.0f arguments stay as literals: the original rematerialises them from the pool each inner
// iteration (pool entries float_1_8041e914 / float_30_8041e918 / float_0_8041e91c); referencing the named
// externs instead makes mwcc hoist them into callee-saved FPRs and shifts every other constant's register.
// The only remaining differences are (1) the first GXSetTevColor colour is built at r1+0x10 and copied to the
// outgoing slot at r1+0xc (original: local r1+0xc, arg r1+0x10; the mwcc quirk also in eff_akari_pose_n64.c /
// eff_confetti_n64.c), and (2) the four 1.0f/30.0f/0.0f pool loads get local labels @212-@214 instead of the
// named .sdata2 symbols (same class as eff_confetti_n64.c's @194). Every other instruction, GPR and FPR matches.
// NONMATCHING (99.97%): the GXColor stack-slot direction and the @212-@214 pool labels above.
void effAkariSflushDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	EffAkariSflushData* data;
	CameraEntry* cam;
	GXColor color;
	Mtx mtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	Mtx viewMtx;
	Mtx outerMtx;
	Mtx workMtx;
	s32 field14;
	s32 i;
	s32 alpha;
	s32 j;
	f32 scaleVal;

	cam = camGetPtr(camId);
	data = effect->userdata;
	field14 = data->field_0x14;
	alpha = data->field_0x24;
	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXRotRad(rotMtx, 'y', -camGetPtr(CAMERA_3D)->viewYaw * float_deg2rad_8041e904);
	PSMTXScale(scaleMtx, data->scale, data->scale, data->scale);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXConcat(mtx, scaleMtx, mtx);
	PSMTXConcat(cam->view, mtx, viewMtx);

	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
	GXSetNumTevStages(2);
	GXSetTevOrder(0, 0xff, 0xff, 4);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xf, 0xf, 0xf, 0xa);
	GXSetTevAlphaIn(0, 7, 1, 5, 7);
	GXSetTevOrder(1, 0xff, 0xff, 0xff);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 2, 4, 0, 0xf);
	GXSetTevAlphaIn(1, 7, 7, 7, 0);
	GXSetNumTexGens(0);

	color = unk_804230a0;
	color.r = data->field_0x18;
	color.g = data->field_0x1C;
	color.b = data->field_0x20;
	color.a = alpha;
	GXSetTevColor(1, color);

	GXSetTevColor(2, dat_8041e900);

	GXSetCullMode(0);
	effSetVtxDescN64((effVtxArray*)size16x16_tex32x32_vtx);

	for (i = 0; i < data->field_0x30 + 1; i++) {
		scaleVal = float_0p1_8041e910 * (f32)cos(float_6p2832_8041e908 *
		                                        (f32)(field14 * 8 + i * 180 / (data->field_0x30 + 1)) /
		                                        float_360_8041e90c) +
		           (f32)(i + 1);
		scaleVal *= data->field_0x2C;
		PSMTXRotRad(rotMtx, 'z', float_deg2rad_8041e904 * (f32)(i * 36));
		PSMTXScale(scaleMtx, scaleVal, scaleVal, scaleVal);
		PSMTXConcat(rotMtx, scaleMtx, outerMtx);

		for (j = 0; j < 5; j++) {
			PSMTXRotRad(rotMtx, 'z', float_deg2rad_8041e904 * (f32)(90 + j * 72));
			PSMTXScale(scaleMtx, 1.0f, 1.0f, 1.0f);
			PSMTXConcat(rotMtx, scaleMtx, rotMtx);
			PSMTXTrans(mtx, 30.0f, 0.0f, 0.0f);
			PSMTXConcat(rotMtx, mtx, mtx);
			PSMTXRotRad(rotMtx, 'x', 0.0f);
			PSMTXConcat(mtx, rotMtx, workMtx);
			PSMTXConcat(viewMtx, outerMtx, mtx);
			PSMTXConcat(mtx, workMtx, mtx);
			GXLoadPosMtxImm(mtx, 0);
			GXSetCurrentMtx(0);
			GXBegin(0x90, 0, 0xc);
			tri2(0, 1, 2, 0, 1, 2, 3, 0);
			tri2(2, 3, 4, 0, 3, 4, 5, 0);
		}
	}
}
