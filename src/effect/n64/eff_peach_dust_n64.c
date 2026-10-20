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

// .rodata effect type name.

// .rodata int->float conversion bias.

// .sdata2

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

EffectEntry* effPeachDustN64Entry(s32 type, f32 x, f32 y, f32 z, f32 a4, f32 angleDeg) {
	s8* table;
	EffectEntry* effect;
	PeachDustParticle* work;
	PeachDustParticle* particle;
	s32 count;
	s32 j;
	s32 i;
	f32 angle;
	f32 radians;
	f32 radius;

	effect = effEntry();
	effect->type = "PeachDustN64";
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
	angle = 6.2831855f * angleDeg;
	table = &data[type * 0x100];
	count = 1;
	particle = work + 1;
	for (i = 0; i < 0x10; i++) {
		for (j = 0; j < 0x10; j++) {
			if (table[(0xF - i) * 0x10 + j] == '.') {
				continue;
			}
			radius = (f32)((j - 8) * 2) + 0.1f * (f32)(rand() % 20 - 10);
			radians = angle / 360.f;
			particle->x = radius * (f32)cos(radians);
			particle->y = (f32)(i * 2) + 0.1f * (f32)(rand() % 20 - 10);
			particle->z = radius * (f32)sin(radians);
			particle->vx = 0.04f * particle->x;
			particle->vy = 0.04f * particle->y;
			particle->gravity = 0.001f * (f32)(rand() % 10);
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
		particle->scale = 0.0f;
		particle->gravity = 0.0f;
		particle->vy = 0.0f;
		particle->vx = 0.0f;
		particle->z = 0.0f;
		particle->y = 0.0f;
		particle->x = 0.0f;
		particle->life = -1;
		particle->counter = 0;
	}
	return effect;
}

void effPeachDustMain(EffectEntry* effect) {
	PeachDustParticle* particles;
#ifdef MUST_MATCH
	f32* scalePtr;
#endif
	Vec position2;
	Vec position;
	s32 colorIndex;
	s32 i;
	s32 active;

	particles = effect->userdata;
	position = makeVec(particles->x, particles->y, particles->z);
	position2 = position;
	particles->life--;
	particles->counter++;
	colorIndex = particles->counter;
	active = 0;
	particles++;
	for (i = 0; i < effect->count - 1; i++, particles++) {
		particles->field_0x28 = (colorIndex + i + i) & 7;
		if (particles->life < 0) {
			active++;
		}
		particles->life--;
		if (particles->life > 30 || particles->life < 0) {
			particles->field_0x28 = -1;
		}
		else {
			particles->counter++;
			colorIndex = particles->counter;
#ifdef MUST_MATCH
			scalePtr = &particles->scale;
#endif
			if (colorIndex - 1 < 25) {
				particles->scale = 0.01f * (f32)scale_data[colorIndex - 1] * 0.4f;
			}
			else {
				f32 k = 0.1f;
#ifdef MUST_MATCH
				// Match-only: reading the old scale through a pointer taken earlier keeps the load order of the original.
				particles->scale = k * (k - particles->scale) + *scalePtr;
#else
				particles->scale = k * (k - particles->scale) + particles->scale;
#endif
			}
			particles->x += particles->vx;
			particles->y += particles->vy;
			particles->vy += particles->gravity;
			particles->z += particles->vx;
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
	PeachDustParticle* particles;
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
	particles = effect->userdata;
	GXSetNumChans(0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
	PSMTXScale(texMtx, 0.0056818184f, 0.045454547f, 0.0f);
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
	colorIndex = (particles->counter - 1) * 3;
	PSMTXTrans(mtx, particles->x, particles->y, particles->z);
	PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(mtx, rot, view);
	PSMTXConcat(camera->view, view, view);
	particles++;
	for (i = 0; i < effect->count - 1; i++, particles++) {
		colorType = particles->field_0x28;
		if (colorType < 0) {
			continue;
		}
		PSMTXTrans(mtx, particles->x, particles->y, particles->z);
		scale = particles->scale;
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
		r = scaleColor(color_rotation_data, colorIndex++, 1.4f);
		g = scaleColor(color_rotation_data, colorIndex++, 0.7f);
		b = scaleColor(color_rotation_data, colorIndex++, 0.7f);
#else
		r = (s32)(color_rotation_data[colorIndex++] * 1.4f);
		g = (s32)(color_rotation_data[colorIndex++] * 0.7f);
		b = (s32)(color_rotation_data[colorIndex++] * 0.7f);
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
			vtx = &((effVtxArray*)size8x8_tex22x22_vtx)[3];
			break;
		case 1:
			vtx = &((effVtxArray*)size8x8_tex22x22_vtx)[4];
			break;
		case 2:
			vtx = &((effVtxArray*)size8x8_tex22x22_vtx)[5];
			break;
		case 3:
			vtx = &((effVtxArray*)size8x8_tex22x22_vtx)[6];
			break;
		case 4:
			vtx = &((effVtxArray*)size8x8_tex22x22_vtx)[7];
			break;
		case 5:
			vtx = &((effVtxArray*)size8x8_tex22x22_vtx)[0];
			break;
		case 6:
			vtx = &((effVtxArray*)size8x8_tex22x22_vtx)[1];
			break;
		case 7:
			vtx = &((effVtxArray*)size8x8_tex22x22_vtx)[2];
			break;
		}
		effSetVtxDescN64(vtx);
		GXBegin(0x90, 0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
	}
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
