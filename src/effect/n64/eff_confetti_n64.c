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
extern const f32 float_0p05_8041ec20;
extern const f32 float_10_8041ec24;
extern const f32 float_0p98_8041ec28;
extern const f32 float_neg0p5_8041ec2c;
extern const f32 float_3_8041ec30;
extern const f32 float_4_8041ec34;
extern const f32 float_0p5_8041ec38;
extern const f32 float_6p2832_8041ec3c;
extern const f32 float_360_8041ec40;
extern const f32 float_1_8041ec44;
extern const f32 float_neg0p02_8041ec48;
extern const f32 float_0p02_8041ec4c;

// .rodata zero vector this unit's particles are laid out from.
const Vec vec3_802fa438 = { 0.0f, 0.0f, 0.0f };

// .rodata int->float conversion constant the original references by name.  Kept const: the
// compiler folds it to an anonymous @NNN pool object, but objdiff's function score ignores the
// reloc-name difference (only `check.py FUNC`'s instruction listing shows it).
const f64 double_to_int_802fa448 = 4503601774854144.0;

// .rodata effect name the original stores into EffectEntry::type.
const char str_802FA450[12] = "ConfettiN64";

// .sbss2 scratch GXColor the original builds its TEV colour from.
extern GXColor unk_804230c8;

// .sdata tables and .data vertex arrays belonging to this unit.
extern u8 color_table[36];
extern u8 anm_seq[8];
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
		radius = float_3_8041ec30;
		break;
	case 1:
	case 5:
		radius = float_4_8041ec34;
		break;
	case 2:
	case 3:
	case 6:
	default:
		radius = float_5_8041ec14;
		break;
	}
	effect->type = str_802FA450;
	num = count + 1;
	effect->count = num;
	data = __memAlloc(HEAP_EFFECT, num * sizeof(effConfettiN64Data));
	effect->userdata = data;
	effect->callback = effConfettiMain;
	part = data + 1;
	data->x = x;
	data->y = y;
	data->z = z;
	*(s32*)((u8*)data + 0x24) = 0;
	*(s32*)((u8*)data + 0x20) = lifetime;
	data->type = type;
	*(s32*)((u8*)data + 0x2c) = count;
	*(s32*)((u8*)data + 0x28) = 0xff;
	data->camId = 4;

	angle0 = rand() % 360;
	half = float_0p5_8041ec38 * radius;
	radius2 = half + half;

	for (i = 0, timer = 0; i < count; timer += 3, i++, part++) {
		randCount = (f32)(rand() % 101);
		randRadius = float_0p01_8041ec1c * randCount * radius;
		angle = float_6p2832_8041ec3c *
			((f32)angle0 + float_360_8041ec40 * (f32)i / (f32)count) / float_360_8041ec40;
		part->type = type;
		sinA = (f32)sin(angle);
		cosA = (f32)cos(angle);
		sinB = (f32)sin(angle);
		cosB = (f32)cos(angle);
		if (type10 == 3) {
			part->x = float_0p1_8041ec04 * (rand() % 701) - float_35_8041ec08;
			part->y = float_0_8041ec00;
			part->z = float_0p1_8041ec04 * (rand() % 301) - float_15_8041ec0c;
			*(f32*)((u8*)part + 0x10) = float_0p03_8041ec10 *
				(float_0p1_8041ec04 * (rand() % 101) + part->x - float_5_8041ec14);
			*(f32*)((u8*)part + 0x14) = float_neg1p7_8041ec18 -
				float_0p01_8041ec1c * (float_0p1_8041ec04 * (rand() % 801));
			*(f32*)((u8*)part + 0x18) = float_0p05_8041ec20 *
				(float_0p1_8041ec04 * (rand() % 201) - float_10_8041ec24);
			*(s32*)((u8*)part + 0x20) = 0x78;
			*(s32*)((u8*)part + 0x30) = timer;
		} else {
			part->x = cosB * (randRadius * sinA);
			part->y = cosB * (randRadius * cosA);
			part->z = randRadius * sinB;
			*(f32*)((u8*)part + 0x10) = cosB * (radius2 * sinA);
			*(f32*)((u8*)part + 0x14) = cosB * (radius2 * cosA) + float_1_8041ec44;
			*(f32*)((u8*)part + 0x18) = radius2 * sinB;
			if (type10 >= 4) {
				*(f32*)((u8*)part + 0x1c) = float_neg0p02_8041ec48 -
					float_0p02_8041ec4c * (f32)((i + 1) % 3);
			} else {
				*(f32*)((u8*)part + 0x1c) = float_0_8041ec00;
			}
			*(s32*)((u8*)part + 0x30) = i & 3;
		}
	}
	return effect;
}

#pragma opt_common_subs reset

// MUST_MATCH: retail builds the position Vec in a scratch slot and copies it into a second local
// before dispCalcZ.  The inline starts from a literal zero vector: a named zero vector's load is
// scheduled before the prologue's stmw, which the original does not do.  vec3_802fa438 is still
// defined above so the unit's .rodata keeps the named object.
#ifdef MUST_MATCH
static inline Vec makeConfettiVec(f32 x, f32 y, f32 z) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = x;
	temp.y = y;
	temp.z = z;
	return temp;
}
#endif

void effConfettiMain(EffectEntry* effect) {
	s32 t;
	effConfettiN64Data* data = effect->userdata;
	Vec position;
	s32 i;
	s32 type;
	s32 frame;
	s32 count;
	s32 camId;

#ifdef MUST_MATCH
	Vec position2 = makeConfettiVec(data->x, data->y, data->z);
#else
	Vec position2 = vec3_802fa438;
	position2.x = data->x;
	position2.y = data->y;
	position2.z = data->z;
#endif
	position = position2;
	camId = data->camId;
	*(s32*)((u8*)data + 0x20) -= 1;
	if (*(s32*)((u8*)data + 0x20) < 0) {
		effDelete(effect);
		return;
	}
	*(s32*)((u8*)data + 0x24) += 1;
	if (*(s32*)((u8*)data + 0x24) > 0x4F1A0) {
		*(s32*)((u8*)data + 0x24) = 10;
	}
	frame = *(s32*)((u8*)data + 0x24);
	type = data->type;
	count = *(s32*)((u8*)data + 0x2C);
	if (*(s32*)((u8*)data + 0x20) < 10) {
		*(s32*)((u8*)data + 0x28) = *(s32*)((u8*)data + 0x20) * 25;
	}
	t = type % 10;
	data++;
	for (i = 0; i < count; i++, data++) {
		switch (t) {
		case 3:
			if (*(s32*)((u8*)data + 0x30) > 0) {
				*(s32*)((u8*)data + 0x30) -= 1;
				if (*(s32*)((u8*)data + 0x30) > 0) {
					continue;
				}
			}
			*(s32*)((u8*)data + 0x20) -= 1;
			if (*(s32*)((u8*)data + 0x20) < 0) {
				data->x = float_0p1_8041ec04 * (rand() % 701) - float_35_8041ec08;
				data->y = float_0_8041ec00;
				data->z = float_0p1_8041ec04 * (rand() % 301) - float_15_8041ec0c;
				*(f32*)((u8*)data + 0x10) = float_0p03_8041ec10 *
					(float_0p1_8041ec04 * (rand() % 101) + data->x - float_5_8041ec14);
				*(f32*)((u8*)data + 0x14) = float_neg1p7_8041ec18 -
					float_0p01_8041ec1c * (float_0p1_8041ec04 * (rand() % 801));
				*(f32*)((u8*)data + 0x18) = float_0p05_8041ec20 *
					(float_0p1_8041ec04 * (rand() % 201) - float_10_8041ec24);
				*(s32*)((u8*)data + 0x20) = 0x78;
				*(s32*)((u8*)data + 0x20) -= 1;
			}
			break;
		default:
			if (*(s32*)((u8*)data + 0x30) > 0) {
				*(s32*)((u8*)data + 0x30) -= 1;
				if (*(s32*)((u8*)data + 0x30) > 0) {
					continue;
				}
			}
			if (t >= 4) {
				*(f32*)((u8*)data + 0x10) *= float_0p98_8041ec28;
				*(f32*)((u8*)data + 0x14) *= float_0p98_8041ec28;
				*(f32*)((u8*)data + 0x18) *= float_0p98_8041ec28;
				if (frame > 5) {
					if (*(f32*)((u8*)data + 0x14) > float_neg0p5_8041ec2c) {
						*(f32*)((u8*)data + 0x14) += *(f32*)((u8*)data + 0x1C);
					}
				}
			}
			break;
		}
		data->x += *(f32*)((u8*)data + 0x10);
		data->y += *(f32*)((u8*)data + 0x14);
		data->z += *(f32*)((u8*)data + 0x18);
	}
	if (type < 10) {
		dispEntry(camId, dispCalcZ(&position), 2, effConfettiDisp, effect);
	} else if (type < 20) {
		dispEntry(CAMERA_3D_EFFECTB, dispCalcZ(&position), 2, effConfettiDisp, effect);
	} else {
		dispEntry(CAMERA_2D, dispCalcZ(&position), 2, effConfettiDisp, effect);
	}
}

// MUST_MATCH: retail builds each by-value GXColor in a scratch slot and word-copies it to the
// outgoing slot.  Building the caller's colour local directly lands it at 0x8 and the second call's
// temp at 0xc (a downward 0x10->0xc copy); putting each GXSetTevColor inside an inlined helper makes
// mwcc lay its locals out in the original's order (colour at 0xc, second temp at 0x8, outgoing 0x10).
// The plain field assignments under #else are the natural form.  Same quirk as eff_thunder_n64.
#ifdef MUST_MATCH
static inline void makeConfettiColor(u8 r, u8 g, u8 b, u8 a) {
	GXColor c = unk_804230c8;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	GXSetTevColor(GX_TEVREG0, c);
}

static inline void makeConfettiColor1(void) {
	GXColor c = *(GXColor*)&float_0p011489_8041ebf0;
	GXSetTevColor(GX_TEVREG1, c);
}
#endif

// The instructions, stack slots, GXColor temporaries and the frame/loop register (r26) now all
// match.  Reading the frame into `seq` (then `seq = (seq >> 1) % 6`) rather than into `i` is what
// lets the allocator keep `i` in r26.  The only remaining difference is the colouring of the
// three loop-carried values: the target uses anm_seq r29, divisor r30, seq r31; this build uses
// seq r29, anm_seq r30, divisor r31 (a 3-cycle).  The divisor matters because MWCC CSEs the
// 0x2AAAAAAB magic of `(seq >> 1) % 6` with that of `i % 12` in the loop, so it is live across the
// whole loop rather than just the preheader.  The two modulos schedule identically, so it is a pure
// allocator tie-break.  Declaration-order, load-order, expression-form, pointer/helper, pragma
// (opt_*/global_optimizer/scheduling/optimization_level), -g and statement-order searches did not move it.  The
// implicit int->float constant is also emitted as an anonymous @NNN pool object instead of
// double_to_int_802fa448 (a duplicate of the file's named .rodata copy).
// NONMATCHING (99.86%): only the seq/anm_seq/divisor 3-cycle above differs.
void effConfettiDisp(CameraId camId, void* param) {
	EffectEntry* effect;
	s32 type;
	effConfettiN64Data* data;
	s32 i;
	s32 seq;
	s32 alpha;
	s32 count;
	s32 idx2;
	u8* clr;
	CameraEntry* cam;
	GXTexObj texObj;
	GXColor color;
	Mtx mtx0;
	Mtx scaleMtx;
	Mtx mtx1;
	Mtx mtx2;

	effect = param;
	cam = camGetPtr(camId);
	data = effect->userdata;
	seq = *(s32*)((u8*)data + 0x24);
	type = data->type;
	count = *(s32*)((u8*)data + 0x2C);
	alpha = *(s32*)((u8*)data + 0x28);
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
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, 0x7D);
	seq = (seq >> 1) % 6;
	type %= 10;
	i = 0;
	for (; i < count; i++, data++) {
		clr = &color_table[(i % 12) * 3];
		if (*(s32*)((u8*)data + 0x30) > 0) {
			continue;
		}
		PSMTXTrans(mtx0, data->x, data->y, data->z);
		PSMTXConcat(mtx2, mtx0, mtx0);
		GXLoadPosMtxImm(mtx0, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
#ifdef MUST_MATCH
		makeConfettiColor(clr[0], clr[1], clr[2], alpha);
#else
		color = unk_804230c8;
		color.r = clr[0];
		color.g = clr[1];
		color.b = clr[2];
		color.a = alpha;
		GXSetTevColor(GX_TEVREG0, color);
#endif
#ifdef MUST_MATCH
		makeConfettiColor1();
#else
		GXSetTevColor(GX_TEVREG1, *(GXColor*)&float_0p011489_8041ebf0);
#endif
		idx2 = ((i + anm_seq[seq]) & 0xF) << 6;
		PSMTXScale(scaleMtx, float_0p0625_8041ebf8, float_0p0039062_8041ebfc, float_0_8041ec00);
		PSMTXTrans(mtx0, float_0_8041ec00, (f32)idx2, float_0_8041ec00);
		PSMTXConcat(scaleMtx, mtx0, scaleMtx);
		GXLoadTexMtxImm(scaleMtx, GX_TEXMTX0, GX_MTX2x4);
		switch (type) {
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
