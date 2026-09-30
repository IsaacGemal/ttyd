#include "eff/n64/eff_hit_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include <dolphin/mtx.h>

// Constants referenced by the original update code (defined in this unit's data).
extern const f32 float_5_8041f0e0;
extern const f32 float_4_8041f0e4;
extern const f32 float_0p03125_8041f0e8;
extern const f32 float_0p0078125_8041f0ec;
extern const f32 float_0_8041f0f0;
extern const f32 float_96_8041f0f4;
extern const f32 float_deg2rad_8041f0f8;
extern const f32 float_0p0039062_8041f100;
extern const f32 float_1_8041f104;

extern u8 cnt_table[4];
extern u8 anim_data[16];
extern u8 radius_data[16];

// Vertex descriptor table for the 32x32 texture; 5 entries of 0x38 bytes.
extern effVtxArray size32x32_tex32x32_vtx[];

typedef struct effHitData {
	s32 type;
	Vec position;
	u8 pad_0x10[0x30 - 0x10];
	s32 lifetime;
	s32 timer;
	f32 field_0x38;
	u8 color[6];
	u8 pad_0x42[0x44 - 0x42];
} effHitData;

typedef struct effHitParticle {
	s32 field_0x0;
	Vec position;
	u8 pad_0x10[0x1c - 0x10];
	f32 rotZ;
	f32 field_0x20;
	f32 rotY;
	u8 pad_0x28[0x44 - 0x28];
} effHitParticle;

static inline Vec effHitInitVec(Vec* input) {
	Vec temp = { 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

void effHitMain(EffectEntry* effect) {
	effHitData* data = effect->userdata;
	Vec disp;
	s32 idx;
	s32 i;
	s32 timer;
	s32 type;

	disp = effHitInitVec(&data->position);
	type = data->type;
	data->lifetime--;
	data->timer++;
	if (data->lifetime < 0) {
		effDelete(effect);
		return;
	}
	timer = data->timer;
	if (timer <= cnt_table[type]) {
		data->field_0x38 = (f32)anim_data[timer - 1] * float_0p0039062_8041f100;
	} else {
		data->field_0x38 = float_1_8041f104;
	}

	for (i = 1, data++; i < effect->count; i++, data++) {
		idx = (timer <= cnt_table[type]) ? timer - 1 : 12;
		data->position.x = (f32)radius_data[idx];
		data->position.y = float_0_8041f0f0;
		data->position.z = float_0_8041f0f0;
	}
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effHitDisp, effect);
}

static void main_dl(EffectEntry* effect, Mtx matrix) {
	// Locals are declared in reverse of the order they are used: this compiler
	// assigns stack slots and registers in reverse declaration order, and this
	// ordering reproduces the original's layout (m0/m1/m2 in the low slots).
	effHitData* data;
	effVtxArray* vtx;
	effHitParticle* part;
	CameraEntry* cam;
	Mtx temp;
	Mtx texMtx;
	Mtx m2;
	Mtx m1;
	Mtx m0;
	s32 frame;
	s32 flag;
	s32 i;
	f32 f;

	data = effect->userdata;
	f = float_5_8041f0e0 * data->field_0x38;
	frame = (s32)f;
	flag = (f >= float_4_8041f0e4) ? 1 : 0;

	PSMTXScale(texMtx, float_0p03125_8041f0e8, float_0p0078125_8041f0ec, float_0_8041f0f0);
	PSMTXTrans(temp, float_0_8041f0f0, float_96_8041f0f4, float_0_8041f0f0);
	PSMTXConcat(texMtx, temp, texMtx);
	GXLoadTexMtxImm(texMtx, GX_TEXMTX0, GX_MTX2x4);
	PSMTXScale(texMtx, float_0p03125_8041f0e8, float_0p0078125_8041f0ec, float_0_8041f0f0);
	GXLoadTexMtxImm(texMtx, GX_TEXMTX1, GX_MTX2x4);
	if (flag != 0) {
		PSMTXScale(texMtx, float_0_8041f0f0, float_0_8041f0f0, float_0_8041f0f0);
		GXLoadTexMtxImm(texMtx, GX_TEXMTX1, GX_MTX2x4);
	}

	vtx = size32x32_tex32x32_vtx + frame;
	part = (effHitParticle*)(data + 1);
	for (i = 1; i < effect->count; i++, part++) {
		PSMTXRotRad(m0, 'y', float_deg2rad_8041f0f8 * part->rotY);
		PSMTXRotRad(m1, 'z', float_deg2rad_8041f0f8 * part->rotZ);
		PSMTXTrans(m2, part->position.x, part->position.y, part->position.z);
		PSMTXConcat(m0, m1, m1);
		PSMTXConcat(m1, m2, m2);
		PSMTXConcat(matrix, m2, m2);
		cam = camGetPtr(CAMERA_3D);
		PSMTXConcat(cam->view, m2, m2);
		GXLoadPosMtxImm(m2, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		effSetVtxDescN64(vtx);
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
	}
}
