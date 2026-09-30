#include "eff/n64/eff_peach_dust_n64.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include <stdlib.h>
#include "math_ext.h"
#include <dolphin/mtx.h>

inline Vec makeVec(f32 x, f32 y, f32 z) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = x;
	temp.y = y;
	temp.z = z;
	return temp;
}

// .rodata
const f64 double_to_int_mask_802fb350 = 4503599627370496.0;

// .rodata effect type name.
const char str_802FB358[16] = "PeachDustN64";

// .rodata int->float conversion bias.
const f64 double_to_int_802fb368 = 4503601774854144.0;

// .sdata2
extern const f32 float_0p01_8041f968;
extern const f32 float_0p4_8041f964;
extern const f32 float_0p1_8041f96c;
extern const f32 float_0p0056818_8041f94c;
extern const f32 float_0p045455_8041f950;
extern const f32 float_0_8041f954;
extern const f32 float_deg2rad_8041f958;
extern const f32 float_1p4_8041f95c;
extern const f32 float_0p7_8041f960;
extern const f32 float_6p2832_8041f970;
extern const f32 float_360_8041f974;
extern const f32 float_0p04_8041f978;
extern const f32 float_0p001_8041f97c;

// .sdata2 word constant the original copies before filling the RGB fields.
const GXColor dat_8041f948 = { 0, 0, 0, 0xFF };

// .data per-type 16x16 particle placement grid; '.' marks an empty cell. Indexed as
// data[type*0x100 + (0xF - row)*0x10 + col].
s8 data[0x300] =
	".......+..............@.#...........$%&.*=..........-...;>.........,'....)!.......~~......{....."
	"..~~......]@@^....~/........(_......#:<.....+.......[}|.....+.......1.......23......45.....677.."
	"....8......7.......90......7.......a22b...c...........deee............f++++..........ff....+...."
	"....ff......+........+......+.......+.......ff......+........+...f++.........+..ff...........+f."
	".+............+.ff............+..+............+..f+.f+++f.....+....+f..+......+.......+.......+."
	"......+.......+........+++++++f.....gh............+g.ggg..........g...hh..........g....g........"
	"..hh..hh.hh.......g..hg+ghgg......hghg.g...g.......hggg.....+.....g+.h......g....g..........g..."
	".g..........g.....g.........g.....g.........hgg...g...........g...g.........hg.....gggggggggh...";

// .data vertex strip; 0x38-byte slices are passed to effSetVtxDescN64.
static u32 size8x8_tex22x22_vtx[0x70] = {
	0xFFF8FFF8, 0x00000000, 0x00000000, 0x00FF0008, 0xFFF80000, 0x02C00000,
	0x000000FF, 0x00080008, 0x000002C0, 0x02C00000, 0x00FFFFF8, 0x00080000,
	0x000002C0, 0x000000FF, 0xFFF8FFF8, 0x000002C0, 0x00000000, 0x00FF0008,
	0xFFF80000, 0x05800000, 0x000000FF, 0x00080008, 0x00000580, 0x02C00000,
	0x00FFFFF8, 0x00080000, 0x02C002C0, 0x000000FF, 0xFFF8FFF8, 0x00000580,
	0x00000000, 0x00FF0008, 0xFFF80000, 0x08400000, 0x000000FF, 0x00080008,
	0x00000840, 0x02C00000, 0x00FFFFF8, 0x00080000, 0x058002C0, 0x000000FF,
	0xFFF8FFF8, 0x00000840, 0x00000000, 0x00FF0008, 0xFFF80000, 0x0B000000,
	0x000000FF, 0x00080008, 0x00000B00, 0x02C00000, 0x00FFFFF8, 0x00080000,
	0x084002C0, 0x000000FF, 0xFFF8FFF8, 0x00000B00, 0x00000000, 0x00FF0008,
	0xFFF80000, 0x0DC00000, 0x000000FF, 0x00080008, 0x00000DC0, 0x02C00000,
	0x00FFFFF8, 0x00080000, 0x0B0002C0, 0x000000FF, 0xFFF8FFF8, 0x00000DC0,
	0x00000000, 0x00FF0008, 0xFFF80000, 0x10800000, 0x000000FF, 0x00080008,
	0x00001080, 0x02C00000, 0x00FFFFF8, 0x00080000, 0x0DC002C0, 0x000000FF,
	0xFFF8FFF8, 0x00001080, 0x00000000, 0x00FF0008, 0xFFF80000, 0x13400000,
	0x000000FF, 0x00080008, 0x00001340, 0x02C00000, 0x00FFFFF8, 0x00080000,
	0x108002C0, 0x000000FF, 0xFFF8FFF8, 0x00001340, 0x00000000, 0x00FF0008,
	0xFFF80000, 0x16000000, 0x000000FF, 0x00080008, 0x00001600, 0x02C00000,
	0x00FFFFF8, 0x00080000, 0x134002C0, 0x000000FF,
};

// .sdata
static u8 scale_data[28] = {
	0x1E, 0x3C, 0x5A, 0x64, 0x68, 0x6A, 0x6C, 0x6E,
	0x70, 0x71, 0x71, 0x6E, 0x6B, 0x67, 0x64, 0x68,
	0x6A, 0x6C, 0x6E, 0x70, 0x71, 0x71, 0x6E, 0x6B,
	0x67, 0x00, 0x00, 0x00,
};

// .sdata color table.
static u8 color_rotation_data[36] = {
	0xFE, 0xAC, 0xAC, 0xFE, 0xAC, 0xD5, 0xFE, 0xB4, 0x9A, 0xD5, 0xB4, 0xFE,
	0xB4, 0xB4, 0xFE, 0xB4, 0xDD, 0xFE, 0xB4, 0xFE, 0xFE, 0xB4, 0xFE, 0xD5,
	0xB4, 0xFE, 0xB4, 0xD5, 0xFE, 0xB4, 0xFE, 0xFE, 0xB4, 0xFE, 0xD5, 0xAC,
};

EffectEntry* effPeachDustN64Entry(s32 type, f32 x, f32 y, f32 z, f32 a4, f32 a5) {
	s8* table;
	EffectEntry* effect;
	PeachDustParticle* work;
	PeachDustParticle* particle;
	s32 count;
	s32 j;
	s32 i;
	f32 angle;
	f32 rad;
	f32 radius;

	effect = effEntry();
	effect->type = str_802FB358;
	effect->count = 0x65;
	work = __memAlloc(HEAP_EFFECT, 0x115C);
	effect->userdata = work;
	effect->callback = effPeachDustMain;
	work->x = x;
	work->y = y;
	work->z = z;
	work->counter = 0;
	work->field_0x00 = type;
	work->life = 0x50;
	angle = float_6p2832_8041f970 * a5;
	table = &data[type * 0x100];
	count = 1;
	particle = work + 1;
	for (i = 0; i < 0x10; i++) {
		for (j = 0; j < 0x10; j++) {
			if (table[(0xF - i) * 0x10 + j] == '.') {
				continue;
			}
			radius = (f32)((j - 8) * 2) + float_0p1_8041f96c * (f32)(rand() % 20 - 10);
			rad = angle / float_360_8041f974;
			particle->x = radius * (f32)cos(rad);
			particle->y = (f32)(i * 2) + float_0p1_8041f96c * (f32)(rand() % 20 - 10);
			particle->z = radius * (f32)sin(rad);
			particle->vx = float_0p04_8041f978 * particle->x;
			particle->vy = float_0p04_8041f978 * particle->y;
			particle->gravity = float_0p001_8041f97c * (f32)(rand() % 10);
			particle->life = rand() % 7 + 0x1E;
			particle->counter = 0;
			particle++;
			count++;
			if (count >= 0x65) {
				break;
			}
		}
		if (count >= 0x65) {
			break;
		}
	}
	for (; count < 0x65; count++, particle++) {
		particle->scale = float_0_8041f954;
		particle->gravity = float_0_8041f954;
		particle->vy = float_0_8041f954;
		particle->vx = float_0_8041f954;
		particle->z = float_0_8041f954;
		particle->y = float_0_8041f954;
		particle->x = float_0_8041f954;
		particle->life = -1;
		particle->counter = 0;
	}
	return effect;
}

void effPeachDustMain(EffectEntry* effect) {
	PeachDustParticle* data;
	Vec position2;
	Vec position;
	s32 colorIndex;
	s32 i;
	s32 active;

	data = effect->userdata;
	position = makeVec(data->x, data->y, data->z);
	position2 = position;
	data->life--;
	data->counter++;
	colorIndex = data->counter;
	active = 0;
	data++;
	for (i = 0; i < effect->count - 1; i++, data++) {
		data->field_0x28 = (colorIndex + i + i) & 7;
		if (data->life < 0) {
			active++;
		}
		data->life--;
		if (data->life > 30 || data->life < 0) {
			data->field_0x28 = -1;
		}
		else {
			data->counter++;
			colorIndex = data->counter;
			if (colorIndex - 1 < 25) {
				data->scale = float_0p01_8041f968 * (f32)scale_data[colorIndex - 1] * float_0p4_8041f964;
			}
			else {
				f32 k = float_0p1_8041f96c;
				data->scale = k * (k - data->scale) + data->scale;
			}
			data->x += data->vx;
			data->y += data->vy;
			data->vy += data->gravity;
			data->z += data->vx;
		}
	}
	if (active >= effect->count - 1) {
		effDelete(effect);
	}
	else {
		dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effPeachDustDisp, effect);
	}
}

#ifdef MUST_MATCH
// Match-only: building the colour in an inline fixes the by-value GXColor stack slot, and
// routing the u8*k multiply through an inline local makes mwcc emit the byte as fmuls's
// first operand. The plain forms are under #else below.
static inline s32 scaleColor(const u8* t, s32 i, f32 k) {
	u8 c = t[i];
	return (s32)(c * k);
}

static inline GXColor peachDustColor(u8 r, u8 g, u8 b) {
	GXColor c = dat_8041f948;
	c.r = r;
	c.g = g;
	c.b = b;
	return c;
}
#endif

// Local declaration order is chosen to reproduce mwcc's GPR assignment (camera i colorType
// colorIndex vtx); moving the lines changes which register each loop variable gets.
void effPeachDustDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	PeachDustParticle* data;
	Mtx mtx;
	Mtx texMtx;
	Mtx rot;
	Mtx view;
	GXTexObj texObj;
	GXColor color;
	s32 r;
	s32 g;
	s32 b;
	CameraEntry* camera;
	s32 i;
	s32 colorType;
	s32 colorIndex;
	effVtxArray* vtx;
	f32 scale;

	camera = camGetPtr(camId);
	data = effect->userdata;
	GXSetNumChans(0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
	PSMTXScale(texMtx, float_0p0056818_8041f94c, float_0p045455_8041f950, float_0_8041f954);
	GXLoadTexMtxImm(texMtx, 0x1E, 1);
	effGetTexObjN64(0x20, &texObj);
	GXLoadTexObj(&texObj, 0);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 0xFF);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 2, 0xC, 8, 0xF);
	GXSetTevAlphaIn(0, 7, 1, 4, 7);
	GXSetCullMode(0);
	colorIndex = (data->counter - 1) * 3;
	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', float_deg2rad_8041f958 * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(mtx, rot, view);
	PSMTXConcat(camera->view, view, view);
	data++;
	for (i = 0; i < effect->count - 1; i++, data++) {
		colorType = data->field_0x28;
		if (colorType < 0) {
			continue;
		}
		PSMTXTrans(mtx, data->x, data->y, data->z);
		scale = data->scale;
		mtx[0][0] = scale;
		mtx[1][1] = scale;
		mtx[2][2] = scale;
		PSMTXConcat(view, mtx, mtx);
		GXLoadPosMtxImm(mtx, 0);
		GXSetCurrentMtx(0);
		if (colorIndex >= 0x24) {
			colorIndex = 0;
		}
#ifdef MUST_MATCH
		r = scaleColor(color_rotation_data, colorIndex++, float_1p4_8041f95c);
		g = scaleColor(color_rotation_data, colorIndex++, 0.7f);
		b = scaleColor(color_rotation_data, colorIndex++, 0.7f);
#else
		r = (s32)(color_rotation_data[colorIndex++] * float_1p4_8041f95c);
		g = (s32)(color_rotation_data[colorIndex++] * float_0p7_8041f960);
		b = (s32)(color_rotation_data[colorIndex++] * float_0p7_8041f960);
#endif
		if (r > 0xFF) {
			r = 0xFF;
		}
		if (g > 0xFF) {
			g = 0xFF;
		}
		if (b > 0xFF) {
			b = 0xFF;
		}
#ifdef MUST_MATCH
		color = peachDustColor(r, g, b);
#else
		color = dat_8041f948;
		color.r = r;
		color.g = g;
		color.b = b;
#endif
		GXSetTevColor(1, color);
		switch (colorType & 7) {
		case 0:
			vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0xA8);
			break;
		case 1:
			vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0xE0);
			break;
		case 2:
			vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0x118);
			break;
		case 3:
			vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0x150);
			break;
		case 4:
			vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0x188);
			break;
		case 5:
			vtx = (effVtxArray*)size8x8_tex22x22_vtx;
			break;
		case 6:
			vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0x38);
			break;
		case 7:
			vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0x70);
			break;
		}
		effSetVtxDescN64(vtx);
		GXBegin(0x90, 0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
	}
}
