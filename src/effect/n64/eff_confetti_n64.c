#include "eff/n64/eff_confetti_n64.h"
#include "driver/effdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"

#include <stdlib.h>
#include "math_ext.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>


// .sdata2 float constants (named after their original addresses).
extern const f32 float_0p011489_8041ebf0;
extern const f32 float_deg2rad_8041ebf4;
extern const f32 float_0p0625_8041ebf8;
extern const f32 float_0p0039062_8041ebfc;
extern const f32 float_0_8041ec00;
extern const f32 float_0p1_8041ec04;
extern const f32 float_35_8041ec08;
extern const f32 float_15_8041ec0c;
extern const f32 float_0p03_8041ec10;
extern const f32 float_5_8041ec14;
extern const f32 float_neg1p7_8041ec18;
extern const f32 float_0p01_8041ec1c;

// .rodata effect name the original stores into EffectEntry::type.

// .sbss2 scratch GXColor the original builds its TEV colour from, followed by the section's
// trailing 4-byte gap.  Objects land in reverse declaration order, so the gap is declared first.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") u8 gap_10_804230CC_sbss2[4];
__declspec(section ".sdata") GXColor unk_804230c8;

// .sdata tables and .data vertex arrays belonging to this unit.
u8 color_table[36] = {
	0xE8, 0xA0, 0xA8, 0xA8, 0x50, 0x58, 0xA0, 0xA8,
	0xE8, 0x48, 0x48, 0xE8, 0xA0, 0xE8, 0xA0, 0x60,
	0xB0, 0x78, 0xE0, 0xE0, 0x58, 0xB0, 0xA0, 0x38,
	0xE8, 0xA0, 0xE8, 0xB0, 0x40, 0xA0, 0xA0, 0xD8,
	0xD8, 0x58, 0xA8, 0xA8,
};
u8 anm_seq[8] = { 0x00, 0x01, 0x02, 0x03, 0x02, 0x01, 0x00, 0x00 };
// 4-byte zero word that ends the unit's .sdata: an all-zero object needs explicit_zero_data to be
// emitted and force_active to survive the linker.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") const u32 gap_07_80413874_sdata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset
// .data vertex arrays (retail defines both here, before the Entry jump table).
u32 size6x6_tex16x16_vtx[14] = {
	0xFFFDFFFD, 0x00000000, 0x0000FFFF, 0xFFFF0003,
	0xFFFD0000, 0x02000000, 0xFFFFFFFF, 0x00030003,
	0x00000200, 0x0200FFFF, 0xFFFFFFFD, 0x00030000,
	0x00000200, 0xFFFFFFFF,
};
u32 size6x6_tex16x16_2_vtx[56] = {
	0xFFFDFFFD, 0x00000000, 0x0000FFFF, 0xFFFF0003,
	0xFFFD0000, 0x02000000, 0xFFFFFFFF, 0x00030003,
	0x00000200, 0x0200FFFF, 0xFFFFFFFD, 0x00030000,
	0x00000200, 0xFFFFFFFF, 0xFFDE001A, 0x001E0000,
	0x0800FFFF, 0xFFFFFFE4, 0x001A001E, 0x02000800,
	0xFFFFFFFF, 0xFFE40020, 0x001E0200, 0x0A00FFFF,
	0xFFFFFFDE, 0x0020001E, 0x00000A00, 0xFFFFFFFF,
	0x0010002E, 0x00280000, 0x1000FFFF, 0xFFFF0016,
	0x002E0028, 0x02001000, 0xFFFFFFFF, 0x00160034,
	0x00280200, 0x1200FFFF, 0xFFFF0010, 0x00340028,
	0x00001200, 0xFFFFFFFF, 0xFFF20006, 0xFFCE0000,
	0x1800FFFF, 0xFFFFFFF8, 0x0006FFCE, 0x02001800,
	0xFFFFFFFF, 0xFFF8000C, 0xFFCE0200, 0x1A00FFFF,
	0xFFFFFFF2, 0x000CFFCE, 0x00001A00, 0xFFFFFFFF,
};

void effConfettiN64SetCamId(EffectEntry* entry, s32 camId) {
	((effConfettiN64Data*)entry->userdata)->camId = camId;
}

// Declaring randCount as f32 (so its int->float conversion is the first use of the magic double)
// and using an explicit `half` temp for radius2 makes the FPR locals land on the target's
// f6/f21-f27, and the loop-invariant constants on f28=double, f29=0.01, f30=6p2832, f31=360.
// `opt_common_subs off` is required for that colouring: with CSE on MWCC puts float_360 in f28
// and double/6p2832 in f30/f31.  All GPRs, the position/velocity math and the type-3 reinit match
// either way.
#pragma opt_common_subs off // required for the f28-f31 preheader constant colouring above
EffectEntry* effConfettiN64Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 scale) {
	s32 timer;
	s32 type10;
	EffectEntry* effect;
	effConfettiN64Data* data;
	effConfettiN64Data* part;
	s32 i;
	s32 count;
	s32 num;
	s32 angle0;
	f32 randCount;
	f32 sinA;
	f32 cosA;
	f32 angle;
	f32 sinB;
	f32 radius2;
	f32 radius;
	f32 randRadius;
	f32 cosB;
	f32 half;

	effect = effEntry();
	type10 = type % 10;
	switch (type10) {
	case 0:
		count = 12;
		break;
	case 1:
		count = 18;
		break;
	case 2:
		count = 24;
		break;
	case 4:
		count = 12;
		break;
	case 5:
		count = 18;
		break;
	case 6:
		count = 24;
		break;
	default:
		count = 48;
		break;
	}
	switch (type10) {
	case 0:
	case 4:
		radius = 3.0f;
		break;
	case 1:
	case 5:
		radius = 4.0f;
		break;
	case 2:
	case 3:
	case 6:
	default:
		radius = float_5_8041ec14;
		break;
	}
	effect->type = "ConfettiN64";
	num = count + 1;
	effect->count = num;
	data = __memAlloc(HEAP_EFFECT, num * sizeof(effConfettiN64Data));
	effect->userdata = data;
	effect->callback = effConfettiMain;
	part = data + 1;
	data->x = x;
	data->y = y;
	data->z = z;
	data->frame = 0;
	data->lifetime = lifetime;
	data->type = type;
	data->count = count;
	data->alpha = 0xff;
	data->camId = CAMERA_3D;

	angle0 = rand() % 360;
	half = 0.5f * radius;
	radius2 = half + half;

	for (i = 0, timer = 0; i < count; timer += 3, i++, part++) {
		randCount = (f32)(rand() % 101);
		randRadius = float_0p01_8041ec1c * randCount * radius;
		angle = 6.2831855f *
			((f32)angle0 + 360.0f * (f32)i / (f32)count) / 360.0f;
		part->type = type;
		sinA = (f32)sin(angle);
		cosA = (f32)cos(angle);
		sinB = (f32)sin(angle);
		cosB = (f32)cos(angle);
		if (type10 == 3) {
			part->x = float_0p1_8041ec04 * (rand() % 701) - float_35_8041ec08;
			part->y = float_0_8041ec00;
			part->z = float_0p1_8041ec04 * (rand() % 301) - float_15_8041ec0c;
			part->velX = float_0p03_8041ec10 *
				(float_0p1_8041ec04 * (rand() % 101) + part->x - float_5_8041ec14);
			part->velY = float_neg1p7_8041ec18 -
				float_0p01_8041ec1c * (float_0p1_8041ec04 * (rand() % 801));
			part->velZ = 0.05f *
				(float_0p1_8041ec04 * (rand() % 201) - 10.0f);
			part->lifetime = 0x78;
			part->delay = timer;
		} else {
			part->x = cosB * (randRadius * sinA);
			part->y = cosB * (randRadius * cosA);
			part->z = randRadius * sinB;
			part->velX = cosB * (radius2 * sinA);
			part->velY = cosB * (radius2 * cosA) + 1.0f;
			part->velZ = radius2 * sinB;
			if (type10 >= 4) {
				part->accelY = -0.02f -
					0.02f * (f32)((i + 1) % 3);
			} else {
				part->accelY = float_0_8041ec00;
			}
			part->delay = i & 3;
		}
	}
	return effect;
}

#pragma opt_common_subs reset

// Retail builds the position Vec in a scratch slot and copies it into a second local
// before dispCalcZ.  The inline starts from a literal zero vector: a named zero vector's load is
// scheduled before the prologue's stmw, which the original does not do.
static inline Vec makeConfettiVec(f32 x, f32 y, f32 z) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = x;
	temp.y = y;
	temp.z = z;
	return temp;
}

void effConfettiMain(EffectEntry* effect) {
	s32 t;
	effConfettiN64Data* data = effect->userdata;
	Vec position;
	s32 i;
	s32 type;
	s32 frame;
	s32 count;
	s32 camId;

	Vec position2 = makeConfettiVec(data->x, data->y, data->z);
	position = position2;
	camId = data->camId;
	data->lifetime -= 1;
	if (data->lifetime < 0) {
		effDelete(effect);
		return;
	}
	data->frame += 1;
	if (data->frame > 0x4F1A0) {
		data->frame = 10;
	}
	frame = data->frame;
	type = data->type;
	count = data->count;
	if (data->lifetime < 10) {
		data->alpha = data->lifetime * 25;
	}
	t = type % 10;
	data++;
	for (i = 0; i < count; i++, data++) {
		switch (t) {
		case 3:
			if (data->delay > 0) {
				data->delay -= 1;
				if (data->delay > 0) {
					continue;
				}
			}
			data->lifetime -= 1;
			if (data->lifetime < 0) {
				data->x = float_0p1_8041ec04 * (rand() % 701) - float_35_8041ec08;
				data->y = float_0_8041ec00;
				data->z = float_0p1_8041ec04 * (rand() % 301) - float_15_8041ec0c;
				data->velX = float_0p03_8041ec10 *
					(float_0p1_8041ec04 * (rand() % 101) + data->x - float_5_8041ec14);
				data->velY = float_neg1p7_8041ec18 -
					float_0p01_8041ec1c * (float_0p1_8041ec04 * (rand() % 801));
				data->velZ = 0.05f *
					(float_0p1_8041ec04 * (rand() % 201) - 10.0f);
				data->lifetime = 0x78;
				data->lifetime -= 1;
			}
			break;
		default:
			if (data->delay > 0) {
				data->delay -= 1;
				if (data->delay > 0) {
					continue;
				}
			}
			if (t >= 4) {
				data->velX *= 0.98f;
				data->velY *= 0.98f;
				data->velZ *= 0.98f;
				if (frame > 5) {
					if (data->velY > -0.5f) {
						data->velY += data->accelY;
					}
				}
			}
			break;
		}
		data->x += data->velX;
		data->y += data->velY;
		data->z += data->velZ;
	}
	if (type < 10) {
		dispEntry(camId, dispCalcZ(&position), 2, effConfettiDisp, effect);
	} else if (type < 20) {
		dispEntry(CAMERA_3D_EFFECTB, dispCalcZ(&position), 2, effConfettiDisp, effect);
	} else {
		dispEntry(CAMERA_2D, dispCalcZ(&position), 2, effConfettiDisp, effect);
	}
}

// Retail builds each by-value GXColor in a scratch slot and word-copies it to the
// outgoing slot.  Building the caller's colour local directly lands it at 0x8 and the second call's
// temp at 0xc (a downward 0x10->0xc copy); putting each GXSetTevColor inside an inlined helper makes
// mwcc lay its locals out in the original's order (colour at 0xc, second temp at 0x8, outgoing 0x10).
// Same quirk as eff_thunder_n64.
static inline void makeConfettiColor(u8 r, u8 g, u8 b, u8 a) {
	GXColor c = unk_804230c8;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	GXSetTevColor(GX_TEVREG0, c);
}

#ifdef MUST_MATCH
static inline void makeConfettiColor1(void) {
	GXColor c = *(GXColor*)&float_0p011489_8041ebf0;
	GXSetTevColor(GX_TEVREG1, c);
}
#endif

// Reading the frame into `seq` rather than into `i` leaves `i` free for the loop counter, and
// computing the animation index into `seq2` keeps the modulo out of the counter's live range.
// `type % 10` is folded into the switch below; both loop-invariant expressions are hoisted into
// the preheader by the compiler.
void effConfettiDisp(CameraId camId, void* param) {
	EffectEntry* effect;
	s32 type;
	effConfettiN64Data* data;
	s32 i;
	s32 seq;
	s32 seq2;
	s32 alpha;
	s32 count;
	s32 idx2;
	u8* clr;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx mtx0;
	Mtx scaleMtx;
	Mtx mtx1;
	Mtx mtx2;

	effect = param;
	cam = camGetPtr(camId);
	data = effect->userdata;
	seq = data->frame;
	type = data->type;
	count = data->count;
	alpha = data->alpha;
	PSMTXTrans(mtx0, data->x, data->y, data->z);
	PSMTXRotRad(mtx1, 'y', float_deg2rad_8041ebf4 * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(mtx0, mtx1, mtx2);
	PSMTXConcat(cam->view, mtx2, mtx2);
	data++;
	effGetTexObjN64(0x4B, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetCullMode(GX_CULL_NONE);
	GXSetNumChans(0);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_C1);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	seq2 = (seq >> 1) % 6;
	i = 0;
	for (; i < count; i++, data++) {
		clr = &color_table[(i % 12) * 3];
		if (data->delay > 0) {
			continue;
		}
		PSMTXTrans(mtx0, data->x, data->y, data->z);
		PSMTXConcat(mtx2, mtx0, mtx0);
		GXLoadPosMtxImm(mtx0, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		makeConfettiColor(clr[0], clr[1], clr[2], alpha);
#ifdef MUST_MATCH
		makeConfettiColor1();
#else
		GXSetTevColor(GX_TEVREG1, *(GXColor*)&float_0p011489_8041ebf0);
#endif
		idx2 = ((i + anm_seq[seq2]) & 0xF) << 6;
		PSMTXScale(scaleMtx, float_0p0625_8041ebf8, float_0p0039062_8041ebfc, float_0_8041ec00);
		PSMTXTrans(mtx0, float_0_8041ec00, (f32)idx2, float_0_8041ec00);
		PSMTXConcat(scaleMtx, mtx0, scaleMtx);
		GXLoadTexMtxImm(scaleMtx, GX_TEXMTX0, GX_MTX2x4);
		switch (type % 10) {
		case 3:
			effSetVtxDescN64((effVtxArray*)size6x6_tex16x16_2_vtx);
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0x18);
			tri2(0, 1, 2, 0, 0, 2, 3, 0);
			tri2(4, 5, 6, 4, 4, 6, 7, 4);
			tri2(8, 9, 10, 8, 8, 10, 11, 8);
			tri2(12, 13, 14, 12, 12, 14, 15, 12);
			break;
		default:
			effSetVtxDescN64((effVtxArray*)size6x6_tex16x16_vtx);
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
			tri2(0, 1, 2, 0, 0, 2, 3, 0);
			break;
		}
	}
}

// Named .sdata2 constants, defined after the code so the compiler reads them by name.
const f32 float_0p011489_8041ebf0 = 0.0114889145f;
const f32 float_deg2rad_8041ebf4 = 0.017453292f;
const f32 float_0p0625_8041ebf8 = 0.0625f;
const f32 float_0p0039062_8041ebfc = 0.00390625f;
#pragma explicit_zero_data on
const f32 float_0_8041ec00 = 0.0f;
#pragma explicit_zero_data reset
const f32 float_0p1_8041ec04 = 0.1f;
const f32 float_35_8041ec08 = 35.0f;
const f32 float_15_8041ec0c = 15.0f;
const f32 float_0p03_8041ec10 = 0.03f;
const f32 float_5_8041ec14 = 5.0f;
const f32 float_neg1p7_8041ec18 = -1.7f;
const f32 float_0p01_8041ec1c = 0.01f;
