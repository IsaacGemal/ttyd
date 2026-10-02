#include "eff/n64/eff_hit_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <stdlib.h>
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
extern const f32 float_256_8041f0fc;

extern u8 cnt_table[4];
extern u8 anim_data[16];
extern u8 radius_data[16];
extern u8 fire_data[16];
// Declared as an incomplete array so MWCC addresses this .rodata Vec with lis/addi instead of sda21
// (the original copies it to a stack Vec each loop iteration).
extern Vec vec3_802fa784[];

// Shared scratch GXColors (zero-initialised in the original .sbss2; only their bytes are overwritten).
GXColor unk_80423138;
GXColor unk_8042313c;

// Vertex descriptor table for the 32x32 texture; 5 entries of 0x38 bytes.
extern effVtxArray size32x32_tex32x32_vtx[];

static void main_dl(EffectEntry* effect, Mtx matrix);

typedef struct effHitData {
	s32 type;
	Vec position;
	u8 pad_0x10[0x28 - 0x10];
	f32 field_0x28;
	s32 field_0x2c;
	s32 lifetime;
	s32 timer;
	f32 field_0x38;
	u8 color[6];
	u8 pad_0x42[0x44 - 0x42];
} effHitData;

typedef struct effHitParticle {
	s32 field_0x0;
	Vec position;
	f32 field_0x10;
	f32 field_0x14;
	u8 pad_0x18[0x1c - 0x18];
	f32 rotZ;
	f32 field_0x20;
	f32 rotY;
	u8 pad_0x28[0x2c - 0x28];
	s32 field_0x2C;
	u8 pad_0x30[0x44 - 0x30];
} effHitParticle;

static inline Vec effHitInitVec(Vec* input) {
	Vec temp = { 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

// Every instruction is the same opcode with the same operands except that the 0.0f constant and the
// pooled int->float double have swapped FPRs (ours zero=f30/double=f31, the original double=f30/zero=f31),
// so every zero store and conversion reads the other register, and the two zero stores before the rand()
// call are scheduled before it instead of after. The GPRs (effect/count/part/i/angle/count-1/magic) match.
// NONMATCHING (93.24%): the FPR swap above and the two pre-rand zero-store positions.
EffectEntry* effHitN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale) {
	effHitData* data;
	s32 r;
	EffectEntry* effect;
	effHitParticle* part;
	s32 i;
	s32 count;
	s32 angle;

	effect = effEntry();
	count = (duration * 5 + 20) / 3 + 1;
	effect->type = "HitN64";
	effect->count = count;
	data = __memAlloc(HEAP_EFFECT, count * sizeof(effHitData));
	effect->userdata = data;
	effect->callback = effHitMain;

	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->lifetime = cnt_table[type];
	data->timer = 0;
	data->field_0x38 = float_0_8041f0f0;

	switch (type) {
	case 0:
		data->color[0] = 0xff;
		data->color[1] = 0xff;
		data->color[2] = 0xff;
		data->color[3] = 0xff;
		data->color[4] = 0xff;
		data->color[5] = 0xff;
		break;
	case 1:
		data->color[0] = 0xff;
		data->color[1] = 0xff;
		data->color[2] = 0x00;
		data->color[3] = 0xff;
		data->color[4] = 0x00;
		data->color[5] = 0x00;
		break;
	case 2:
		data->color[0] = 0xff;
		data->color[1] = 0xff;
		data->color[2] = 0x00;
		data->color[3] = 0xc8;
		data->color[4] = 0xff;
		data->color[5] = 0xe6;
		break;
	case 3:
	default:
		data->color[0] = 0x00;
		data->color[1] = 0xff;
		data->color[2] = 0xff;
		data->color[3] = 0x00;
		data->color[4] = 0x00;
		data->color[5] = 0xe6;
		break;
	}

	data->field_0x28 = scale;

	part = (effHitParticle*)(data + 1);
	for (i = 1; i < count; i++, part++) {
		part->position.z = float_0_8041f0f0;
		part->field_0x20 = float_0_8041f0f0;
		r = rand();
		part->position.x = float_0_8041f0f0;
		part->position.y = float_0_8041f0f0;
		part->field_0x10 = float_0_8041f0f0;
		part->field_0x14 = float_0_8041f0f0;
		part->field_0x10 = float_0_8041f0f0;
		angle = (i * 360) / (count - 1);
		part->rotY = (f32)(r % 360);
		part->field_0x0 = 0;
		part->rotZ = (f32)angle;
		part->field_0x2C = 0xff;
	}
	return effect;
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

// These two inlines are inlined in the original; keeping them as separate scopes reproduces the
// per-call by-value GXColor stack slots (local 0x10/copy 0x14 and local 0x8/copy 0xC).
static inline void effHitSetColor1(s32 r, s32 g, s32 b, s32 a) {
	GXColor color = unk_80423138;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	GXSetTevColor(GX_TEVREG0, color);
}

static inline void effHitSetColor2(s32 r, s32 g, s32 b, s32 a) {
	GXColor color = unk_8042313c;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	GXSetTevColor(GX_TEVREG1, color);
}

void effHitDisp(CameraId camId, void* param) {
	effHitData* data;
	CameraEntry* cam;
	Mtx mtxA;
	Mtx mtxC;
	Mtx mtxB;
	Mtx mtx;
	Mtx mtxLoop;
	GXTexObj texObj;
	Vec axis;
	f32 frames;
	f32 scale;
	s32 frame;
	s32 timer;
	s32 i;
	s32 type;
	s32 c1r;
	s32 c1g;
	s32 c1b;
	s32 c2r;
	s32 c2g;
	s32 c2b;
	s32 alpha;
	s32 c1a;
	EffectEntry* effect = param;
	s32 fire;

	cam = camGetPtr(camId);
	data = effect->userdata;
	timer = data->timer;
	type = data->type;
	frames = float_5_8041f0e0 * data->field_0x38;
	frame = (s32)frames;
	if (frame > 4) {
		return;
	}

	scale = data->field_0x28;
	PSMTXTrans(mtxA, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(mtxB, 'y', float_deg2rad_8041f0f8 * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXScale(mtxC, scale, scale, scale);
	PSMTXConcat(mtxA, mtxB, mtxA);
	PSMTXConcat(mtxA, mtxC, mtx);

	alpha = (s32)((frames - (f32)frame) * float_256_8041f0fc);
	c1a = data->field_0x2c;

	switch (type) {
	case 1:
		fire = (timer - 1 < 13) ? fire_data[timer - 1] : 0;
		c1r = data->color[0];
		c1g = data->color[1] * fire >> 8;
		c1b = data->color[2] * fire >> 8;
		c2r = data->color[3] * fire >> 8;
		c2g = data->color[4] * fire >> 8;
		c2b = data->color[5] * fire >> 8;
		break;
	case 2:
		if (timer & 1) {
			c1r = data->color[0];
			c1g = data->color[1];
			c1b = data->color[2];
			c2r = data->color[3];
			c2g = data->color[4];
			c2b = data->color[5];
		}
		else {
			c1r = 0;
			c1g = 0;
			c1b = 0;
			c2r = data->color[0];
			c2g = data->color[1];
			c2b = data->color[2];
		}
		break;
	case 0:
	default:
		c1r = data->color[0];
		c1g = data->color[1];
		c1b = data->color[2];
		c2r = data->color[3];
		c2g = data->color[4];
		c2b = data->color[5];
		break;
	}

	GXSetNumChans(0);
	GXSetNumTevStages(2);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_C1, GX_CC_C0, GX_CC_CPREV, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_APREV, GX_CA_TEXA, GX_CA_A1, GX_CA_ZERO);
	effHitSetColor1(c1r, c1g, c1b, c1a);
	effHitSetColor2(c2r, c2g, c2b, alpha);
	effGetTexObjN64(0x4a, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXLoadTexObj(&texObj, GX_TEXMAP1);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);
	GXSetCullMode(GX_CULL_NONE);

	for (i = 0; i < 3; i++) {
		axis = vec3_802fa784[0];
		PSMTXRotAxisRad(mtxLoop, &axis, float_deg2rad_8041f0f8 * (f32)(i * 120));
		PSMTXConcat(mtx, mtxLoop, mtxLoop);
		main_dl(effect, mtxLoop);
	}
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
