#include "eff/n64/eff_fireworks_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "driver/camdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include "math_ext.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdlib.h>

// .data vertex pools, local to this file (pointed at by vtx_data).
u32 vtx[72] = {
	0x24005D36, 0xD84A4300, 0x4A52DC2D, 0x59002D3C, 0xBE2D15C0, 0x4A1CAB2D, 0x0A9E0F31, 0xAA0FA6D8,
	0x0FA9ED2D, 0x9D000FB8, 0xCB2DA913, 0x2DA6280F, 0xBE490FB8, 0x352DD34D, 0x2DE15E0F, 0xF7592DB8,
	0x352D0BDE, 0x5D000064, 0xCAD84AB8, 0xCB2DD3B3, 0x2DA6D80F, 0xBEB70F0B, 0xDE5DEBC0, 0x4A15C04A,
	0xF7A72D1C, 0xAB2D0A62, 0x0F50C60F, 0x61EB0F31, 0xAA0F0A9E, 0x0FE1A20F, 0xBEB70F31, 0x560F503A,
	0x0F61150F, 0xE3EB5D00, 0x00640B22, 0x5DE3155D, 0x15404AEB, 0x404ACA28, 0x4A1C552D, 0x43004A59,
	0x002D2400, 0x5D000064, 0x0B225D36, 0x284A1540, 0x4ACA284A, 0xB8352D00, 0x0064E3EB, 0x5DBD004A,
	0xCAD84AE3, 0x155D0000, 0x643CBE2D, 0x52DC2D59, 0x002D1CAB, 0x2D1CAB2D, 0xF7A72DD3, 0xB32DB8CB,
	0x2D3C422D, 0x1C552D52, 0x242D1C55, 0x2D59002D, 0xF7592DD3, 0x4D2DB835, 0x2D31560F, 0x0A620F52,
	0x242D61EB, 0x0F61150F, 0x3C422D1C, 0x552DA913, 0x2DBE490F, 0xA6280FA9, 0xED2DB8CB, 0x2D000000,
};
u32 vtx2[50] = {
	0xD0F100DC, 0xE616D300, 0x16D00F00, 0xDC1A16E3, 0x2800DC1A, 0x16F22B16, 0x0032000E, 0x2B1608E7,
	0x2B0ED516, 0x24E6161A, 0x002B0000, 0x3200CE00, 0x1DD8002D, 0x00161DD8, 0x0024E616, 0x0ED51630,
	0xF1002D00, 0x1600CE00, 0x0ED516F2, 0xD516E3D8, 0x00DCE616, 0x1D280024, 0x1A16300F, 0x000E2B16,
	0x2D001600, 0x00320819, 0x2BEB0F2B, 0x0E2B161A, 0x002B2D00, 0x16000032, 0x08192BEB, 0xF12B0000,
	0x32DCE616, 0x08E72B00, 0x0032EB0F, 0x2BEBF12B, 0xDC1A16F2, 0x2B16DC1A, 0x161D2800, 0x00320024,
	0x1A1630F1, 0x00300F00, 0x0E2B16F2, 0xD516D0F1, 0x00E3D800, 0x0ED516D3, 0x0016DCE6, 0x16E32800,
	0xD00F0000, 0x00000000,
};
u32 vtx3[42] = {
	0xF76900EF, 0x57001157, 0x00096900, 0x007A00E6, 0x45001A45, 0x00DD3400, 0x233400CA, 0x3100BAFF,
	0x00C8F100, 0x00C80038, 0xF10046FF, 0x00363100, 0xC4DE00EF, 0xBF0011BF, 0x003CDE00, 0x550D004A,
	0x2E00631B, 0x005D2B00, 0x0C21000C, 0x16001816, 0x00182100, 0x0C0A0018, 0x0A00182D, 0x00E82100,
	0xE81600E8, 0x2D007128, 0x00C1CB00, 0xBEB700B6, 0x2E00A32B, 0x0023B600, 0x34AD000C, 0x2D00F416,
	0x00F42100, 0xE80A00F4, 0x0A00F42D, 0x00CCAD00, 0xDDB600BA, 0xA400AB0D, 0x009D1B00, 0x8F28003F,
	0xCB0042B7, 0x0046A400,
};

// .sdata table of {vertex pool, vertex count}, indexed by effFireworksData::type.
u32 vtx_data[3][2] = {
	{ (u32)vtx, 0x5F },
	{ (u32)vtx2, 0x42 },
	{ (u32)vtx3, 0x38 },
};

// .sbss frame counter and the running alpha shared with effFireworksMain's fade.
static u32 sflag_449;
static s32 salpha_450;

// .sdata2 constants (named after their original addresses in the asm).
extern const f32 float_6p2832_8041ef28;
extern const f32 float_360_8041ef2c;
extern const f32 float_0p5_8041ef30;
extern const f32 float_480_8041ef34;
extern const f32 float_0_8041ef38;
extern const f32 float_608_8041ef3c;
extern const f32 float_1_8041ef40;
extern const f32 float_32768_8041ef44;
extern const f32 float_8_8041ef48;
extern const f32 float_0p95_8041ef4c;
extern const f32 float_0p15_8041ef50;
extern const f32 float_0p11_8041ef54;
extern const f32 float_80_8041ef58;
extern const f32 float_0p1_8041ef5c;
extern const f32 float_neg1000_8041ef60;
extern const f32 float_1000_8041ef64;

// .rodata
const Vec vec3_802fa600 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802fa60c = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802fa618 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802fa624 = { 0.0f, 0.0f, 1000.0f };
const Vec vec3_802fa630 = { 0.0f, 1.0f, 0.0f };
const f64 double_to_int_802fa640 = 4503601774854144.0;
const f64 double_to_int_mask_802fa648 = 4503599627370496.0;
const char str_802FA650[16] = "FireworksN64";

EffectEntry* effFireworksN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 a5, f32 a6, f32 a7, f32 a8) {
	EffectEntry* entry;
	effFireworksData* data;
	s32 i;

	entry = effEntry();
	entry->type = str_802FA650;
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effFireworksData));
	entry->userdata = data;
	entry->callback = effFireworksMain;
	data->type = type;
	data->counter = 0;
	if (life <= 0) {
		data->life = 1000;
	}
	else {
		data->life = life + 0x20;
	}
	data->field_0x4C = 0;
	data->color0[3] = 0xff;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x44 = 0.0f;
	data->field_0x10 = a5;
	data->field_0x14 = a6;
	data->field_0x18 = a7;
	data->field_0x48 = a8;
	data->color0[0] = 0xff;
	data->color0[1] = 0xff;
	data->color0[2] = 0x64;
	data->color1[0] = 0xff;
	data->color1[1] = 0x64;
	data->color1[2] = 0x96;
	data->color1[3] = 0xff;
	for (i = 0; i < 4; i++) {
		data->field_0x50[i] = data->x;
		data->field_0x60[i] = data->y - 1000.0f;
		data->field_0x70[i] = data->z;
		data->field_0x80[i] = 0.0f;
		data->field_0x90[i] = 0.0f;
		data->field_0xA0[i] = 0.0f;
	}
	return entry;
}

// NONMATCHING (97.38%): sin/angle FP registers are rotated one slot (frsp f5 vs f6,
// Bf in f1 vs f0, 80.0f in f0 vs f5) and the prologue loads the vec3_802fa600 words
// a few instructions earlier than the original. Everything else matches.
void effFireworksMain(EffectEntry* effect) {
	effFireworksData* data = effect->userdata;
	Vec position2;
	Vec position = vec3_802fa600;
	s32 counter;
	s32 i;
	f32 angle;
	f32 sinAngle;
	s32 b;

	position.x = data->x;
	position.y = data->y;
	position.z = data->z;
	position2 = position;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->life = 16;
	}
	if (data->life < 1000) {
		data->life--;
	}
	data->counter++;
	if (data->life < 0) {
		effDelete(effect);
		return;
	}
	counter = data->counter;
	if (data->life < 0x20) {
		data->color0[3] = data->life << 3;
	}

	if (data->field_0x4C == 1) {
		data->x += data->field_0x10;
		data->y += data->field_0x14;
		data->z += data->field_0x18;
		data->field_0x10 *= float_0p95_8041ef4c;
		data->field_0x14 *= float_0p95_8041ef4c;
		data->field_0x18 *= float_0p95_8041ef4c;
		data->field_0x14 -= float_0p15_8041ef50;
		data->field_0x44 += float_0p11_8041ef54 * (data->field_0x48 - data->field_0x44);
	}
	else {
		b = 32 - counter;
		data->field_0x50[counter & 3] = data->x - data->field_0x10 * (f32)b;
		angle = float_6p2832_8041ef28 * (f32)(counter * 90 >> 5) / float_360_8041ef2c;
		sinAngle = (f32)sin(angle);
		data->field_0x60[counter & 3] = data->y - data->field_0x14 * (f32)b - (float_80_8041ef58 - float_80_8041ef58 * sinAngle);
		data->field_0x70[counter & 3] = data->z - data->field_0x18 * (f32)b;
		data->field_0x80[counter & 3] = (f32)(rand() % 11 - 5) * float_0p1_8041ef5c;
		data->field_0x90[counter & 3] = (f32)(rand() % 11 - 5) * float_0p1_8041ef5c;
		data->field_0xA0[counter & 3] = (f32)(rand() % 11 - 5) * float_0p1_8041ef5c;
		for (i = 0; i < 4; i++) {
			data->field_0x50[i] += data->field_0x80[i];
			data->field_0x60[i] += data->field_0x90[i];
			data->field_0x70[i] += data->field_0xA0[i];
			data->field_0x90[i] -= float_0p15_8041ef50;
			if (counter >= 27) {
				data->field_0x60[i] = float_neg1000_8041ef60;
			}
		}
		if (counter >= 0x20) {
			data->field_0x4C = 1;
			data->counter = 1;
		}
	}

	dispEntry(4, dispCalcZ(&position2), 2, effFireworksDisp, effect);
}

// Builds the display-space position; the inline return-value temp is at 0x18 and
// is copied to the caller's Vec at 0x48, matching the original's stack layout.
static inline Vec effFireworksPos(f32 x, f32 y, f32 z) {
	Vec v = vec3_802fa60c;
	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}

#ifdef MUST_MATCH
// Returning the GXColor from an inline puts the build/copy slots at 0x10/0x14 and
// 0x8/0xc, which is what the original's two GXSetTevColor calls use.
static inline GXColor effFireworksColor0(effFireworksData* data, u8 a) {
	GXColor c = unk_80423108;
	c.r = data->color0[0];
	c.g = data->color0[1];
	c.b = data->color0[2];
	c.a = a;
	return c;
}

static inline GXColor effFireworksColor1(effFireworksData* data) {
	GXColor c = unk_8042310c;
	c.r = data->color0[0];
	c.g = data->color0[1];
	c.b = data->color0[2];
	return c;
}
#endif

// GXColor build/copy slots, the pooled int->float magic double, the mode==1 signed-byte
// rotation (`-s0*sin + s2*cos` as fmsubs not fmadds), and alpha/mode register allocation differ.
// NONMATCHING (93.7%): codegen-only differences; the body is otherwise equivalent.
void effFireworksDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effFireworksData* data;
	CameraEntry* cam;
	void* vp;
	s32 type;
	s32 mode;
	s32 alpha;
	s32 count;
	s32 i;
	f32 angle;
	f32 sinA;
	f32 cosA;
	f32 baseX;
	f32 baseY;
	f32 baseZ;
	f32 radius;
	f32 px;
	f32 py;
	f32 pz;
	f32 s;
	s8 s0;
	s8 s1;
	s8 s2;
	f32 cx;
	f32 cy;
	Vec position;
	Vec eye;
	Vec up;
	Vec target;
	Mtx44 ortho;
	Mtx lookAt;
	GXTexObj texObj;
	f32 proj[7];
	GXColor color;

	cam = camGetPtr(camId);
	data = effect->userdata;
	alpha = data->color0[3];
	type = data->type;
	angle = float_6p2832_8041ef28 * -camGetPtr(CAMERA_3D)->viewYaw / float_360_8041ef2c;
	sinA = (f32)sin(angle);
	cosA = (f32)cos(angle);
	mode = data->field_0x4C;
	if (sflag_449 != gp->retraceCount) {
		sflag_449 = gp->retraceCount;
		salpha_450 = 0;
	}
	if (salpha_450 < alpha) {
		salpha_450 = alpha;
	}

	GXSetNumChans(0);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C1, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
	effGetTexObjN64(8, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0);

#ifdef MUST_MATCH
	color = effFireworksColor0(data, alpha);
#else
	color = unk_80423108;
	color.r = data->color0[0];
	color.g = data->color0[1];
	color.b = data->color0[2];
	color.a = alpha;
#endif
	GXSetTevColor(GX_TEVREG0, color);
#ifdef MUST_MATCH
	color = effFireworksColor1(data);
#else
	color = unk_8042310c;
	color.r = data->color0[0];
	color.g = data->color0[1];
	color.b = data->color0[2];
#endif
	GXSetTevColor(GX_TEVREG1, color);

	if (mode == 1) {
		vp = (void*)vtx_data[type][0];
		count = vtx_data[type][1];
	}
	else {
		vp = NULL;
		count = 4;
	}

	baseX = data->x;
	baseY = data->y;
	baseZ = data->z;
	radius = data->field_0x44;
	for (i = 0; i < count; i++) {
		if (mode == 1) {
			if ((rand() % 17) < 6) {
				goto next;
			}
			s0 = ((s8*)vp)[0];
			s1 = ((s8*)vp)[1];
			s2 = ((s8*)vp)[2];
			px = baseX + radius * ((f32)s0 * cosA + (f32)s2 * sinA);
			py = baseY + radius * (f32)s1;
			pz = baseZ + radius * ((f32)s2 * cosA - (f32)s0 * sinA);
		}
		else {
			px = data->field_0x50[i];
			py = data->field_0x60[i];
			pz = data->field_0x70[i];
		}

		position = effFireworksPos(px, py, pz);
		PSMTXMultVec(cam->view, &position, &position);
		PSMTX44MultVec(cam->projection, &position, &position);

		s = (f32)gp->fbWidth;
		cx = position.x * s * float_0p5_8041ef30;
		s = (f32)gp->efbHeight;
		cy = position.y * s * float_0p5_8041ef30;

		eye = vec3_802fa624;
		up = vec3_802fa630;
		target = vec3_802fa618;
		GXGetProjectionv(proj);
		C_MTXOrtho(ortho, float_480_8041ef34, float_0_8041ef38, float_0_8041ef38, float_608_8041ef3c, float_1_8041ef40,
				   float_32768_8041ef44);
		GXSetProjection(ortho, GX_ORTHOGRAPHIC);
		C_MTXLookAt(lookAt, &eye, &up, &target);
		GXLoadPosMtxImm(lookAt, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(cx, cy, float_0_8041ef38);
		GXTexCoord2s16(0, 1);
		GXPosition3f32(cx + float_8_8041ef48, cy, float_0_8041ef38);
		GXTexCoord2s16(1, 1);
		GXPosition3f32(cx + float_8_8041ef48, cy - float_8_8041ef48, float_0_8041ef38);
		GXTexCoord2s16(1, 0);
		GXPosition3f32(cx, cy - float_8_8041ef48, float_0_8041ef38);
		GXTexCoord2s16(0, 0);
		GXSetProjectionv(proj);

	next:
		vp = (u8*)vp + 3;
	}
}

// .sdata2 constant definitions (placed last so the functions can't fold
// their values and must reference the original symbols).
const f32 float_6p2832_8041ef28 = 6.2831855f;
const f32 float_360_8041ef2c = 360.0f;
const f32 float_0p5_8041ef30 = 0.5f;
const f32 float_480_8041ef34 = 480.0f;
const f32 float_0_8041ef38 = 0.0f;
const f32 float_608_8041ef3c = 608.0f;
const f32 float_1_8041ef40 = 1.0f;
const f32 float_32768_8041ef44 = 32768.0f;
const f32 float_8_8041ef48 = 8.0f;
const f32 float_0p95_8041ef4c = 0.95f;
const f32 float_0p15_8041ef50 = 0.15f;
const f32 float_0p11_8041ef54 = 0.11f;
const f32 float_80_8041ef58 = 80.0f;
const f32 float_0p1_8041ef5c = 0.1f;
const f32 float_neg1000_8041ef60 = -1000.0f;
const f32 float_1000_8041ef64 = 1000.0f;
