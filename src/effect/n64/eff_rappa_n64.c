#include "eff/n64/eff_rappa_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "math_ext.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdlib.h>

#ifndef MUST_MATCH
// .rodata: zero vector the natural position helper starts from (MUST_MATCH builds it as an anonymous literal).
const Vec vec3_802fb4a0 = { 0.0f, 0.0f, 0.0f };
#endif

// .sbss2: the default .sbss2 is only used for compiler-generated pool templates, so pair it with a small-data section.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor gap_10_80423234_sbss2;
__declspec(section ".sdata") GXColor unk_80423230;


// .data: one effVtxArray (0x38 bytes) for the textured quad.
static u32 size16x16_tex32x32_vtx[0x38 / 4] = {
	0xFFF8FFF8, 0x00000000, 0x00000000, 0x00FF0008,
	0xFFF80000, 0x04000000, 0x000000FF, 0x00080008,
	0x00000400, 0x04000000, 0x00FFFFF8, 0x00080000,
	0x00000400, 0x000000FF,
};

// .sdata: byte-indexed color rotation table (36 bytes).
static u8 color_rotation_data[40] = {
	0xFE, 0xAC, 0xAC, 0xFE, 0xAC, 0xD5, 0xFE, 0xB4, 0x9A, 0xD5, 0xB4, 0xFE,
	0xB4, 0xB4, 0xFE, 0xB4, 0xDD, 0xFE, 0xB4, 0xFE, 0xFE, 0xB4, 0xFE, 0xD5,
	0xB4, 0xFE, 0xB4, 0xD5, 0xFE, 0xB4, 0xFE, 0xFE, 0xB4, 0xFE, 0xD5, 0xAC,
};

// .sbss2: shared scratch GXColor, only its bytes are overwritten (see eff_small_star_n64.c).

// Local copy of system.c's inline helper so it inlines here (as in eff_sstar.c).
inline Vec initializeVec(Vec* input) {
#ifdef MUST_MATCH
	// Match-only trick: the anonymous zero-vector literal makes MWCC schedule the userdata load before
	// the vec words, as in the original; the named vector hoists the vec loads into the prologue slots.
	// The original loads vec3_802fb4a0, so this adds a duplicate zero vector to .rodata.
	Vec temp = { 0.0f, 0.0f, 0.0f };
#else
	Vec temp = vec3_802fb4a0;
#endif
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

EffectEntry* effRappaN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 yaw, f32 pitch) {
	EffectEntry* effect;
	effRappaData* data;
	effRappaData* p;
	s32 i;
	f32 theta;
	f32 speed;
	f32 sinTheta;
	f32 phi;
	// These temporaries pin MWCC's FPR assignment and commutative-operand order.
	f32 tmp;
	f32 randValue;
	f32 cosPhi;

	effect = effEntry();
	if (count == 0) {
		return NULL;
	}
	effect->type = "RappaN64";
	effect->count = count + 1;
	data = __memAlloc(HEAP_EFFECT, (count + 1) * sizeof(effRappaData));
	effect->userdata = data;
	effect->callback = effRappaMain;
	tmp = float_6p2832_8041faf8 * yaw;
	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->life = 0x1E;
	data->counter = 0;
	data->alpha = 0xFF;

	p = data + 1;
	for (i = 1; i < count + 1; i++, p++) {
		randValue = (f32)(rand() % 10);
		speed = randValue * float_0p25_8041fb00 + float_5_8041fafc;
		theta = tmp / float_360_8041fb04;
		p->type = type;
		p->position.x = x;
		p->position.y = y;
		p->position.z = z;
		sinTheta = (f32)sin(theta);
		phi = float_6p2832_8041faf8 * (pitch + (f32)((i & 2) * (i / 2) * 4)) / float_360_8041fb04;
		cosPhi = (f32)cos(phi);
		p->velX = cosPhi * sinTheta;
		p->velY = (f32)sin(phi);
		p->velZ = (f32)cos(phi) * (f32)cos(theta);
		p->speed = speed;
		p->scale = speed / float_10_8041faf4;
	}
	return effect;
}

void effRappaMain(EffectEntry* effect) {
	EffectEntry* e;
	effRappaData* data;
	Vec position;
	s32 i;
	s32 life;

	e = effect;
	data = e->userdata;
	position = initializeVec(&data->position);

	data->life--;
	data->counter++;
	life = data->life;
	if (life < 0) {
		effDelete(e);
		return;
	}
	if (life < 10) {
		data->alpha = (s32)((f32)data->alpha * float_0p8_8041faec);
	}

	data++;
	for (i = 1; i < e->count; i++, data++) {
		data->position.x += data->speed * data->velX;
		data->position.y += data->speed * data->velY;
		data->position.z += data->speed * data->velZ;
		data->speed *= float_0p9_8041faf0;
		if (life < 10) {
			data->scale += data->speed / float_10_8041faf4;
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effRappaDisp, e);
}

// Building the TEV colour in an inlined helper allocates it at r1+0x8 and word-copies it to the
// outgoing arg slot at r1+0xc (the original's layout); building it directly in effRappaDisp puts
// it at r1+0xc (the known GXColor temp/pass swap, see eff_onpu_n64.c).
static inline void rappaSetColor(s32 idx1, s32 idx2, s32 idx3, s32 alpha) {
	GXColor color = unk_80423230;

	color.r = color_rotation_data[idx1 % 36];
	color.g = color_rotation_data[idx2 % 36];
	color.b = color_rotation_data[idx3 % 36];
	color.a = alpha;
	GXSetTevColor(GX_TEVREG0, color);
}

void effRappaDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	s32 idx1;
	s32 idx2;
	s32 idx3;
	effRappaData* data;
	s32 base;
	s32 i;
	s32 alpha;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx mtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	f32 scale;

	cam = camGetPtr(camId);
	data = effect->userdata;
	alpha = data->alpha;
	base = (data->counter - 1) * 3;
	base %= 36;

	GXSetNumChans(0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	PSMTXScale(scaleMtx, float_0p0625_8041fad8, float_0p03125_8041fadc, float_0_8041fae0);
	GXLoadTexMtxImm(scaleMtx, GX_TEXMTX0, GX_MTX2x4);
	effGetTexObjN64(0x1D, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);
	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64((effVtxArray*)size16x16_tex32x32_vtx);

	idx1 = base + 3;
	idx2 = base + 4;
	idx3 = base + 5;
	data++;
	for (i = 1; i < effect->count; i++, data++) {
		rappaSetColor(idx1, idx2, idx3, alpha);

		PSMTXTrans(mtx, data->position.x, data->position.y, data->position.z);
		PSMTXRotRad(rotMtx, 'y', float_deg2rad_8041fae4 * -camGetPtr(CAMERA_3D)->viewYaw);
		scale = data->scale;
		PSMTXScale(scaleMtx, scale, scale, float_1_8041fae8);
		PSMTXConcat(mtx, rotMtx, mtx);
		PSMTXConcat(mtx, scaleMtx, mtx);
		PSMTXConcat(cam->view, mtx, mtx);
		GXLoadPosMtxImm(mtx, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);

		idx1 += 3;
		idx2 += 3;
		idx3 += 3;
	}
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_0p0625_8041fad8 = 0.0625f;
const f32 float_0p03125_8041fadc = 0.03125f;
const f32 float_0_8041fae0 = 0.0f;
const f32 float_deg2rad_8041fae4 = 0.017453292f;
const f32 float_1_8041fae8 = 1.0f;
const f32 float_0p8_8041faec = 0.8f;
const f32 float_0p9_8041faf0 = 0.9f;
const f32 float_10_8041faf4 = 10.0f;
const f32 float_6p2832_8041faf8 = 6.2831855f;
const f32 float_5_8041fafc = 5.0f;
const f32 float_0p25_8041fb00 = 0.25f;
const f32 float_360_8041fb04 = 360.0f;
