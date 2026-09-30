#include "eff/n64/eff_bomhei_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "matching.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

const f32 float_9_8041ea4c = 9.0f;

const Vec vec3_802fa2b0 = { 0.0f, 0.0f, 0.0f };

// NB: when the f32 constants below are defined here the compiler folds every use to an anonymous
// .sdata2 copy; declaring them extern keeps the original's named @sda21 references (the int->float
// double is defined, its anonymous copy is an ignored reloc-label diff).
const f64 double_to_int_802fa2c0 = 4503601774854144.0;
extern const f32 float_256_8041ea2c;
extern const f32 float_0p03125_8041ea30;
extern const f32 float_0p0034722_8041ea34;
extern const f32 float_0_8041ea38;
extern const f32 float_32_8041ea3c;
extern const f32 float_deg2rad_8041ea40;
extern const f32 float_1_8041ea44;
extern const f32 float_4_8041ea48;

const GXColor dat_8041ea20 = { 0xFF, 0xFF, 0xFF, 0x7F };
const GXColor dat_8041ea24 = { 0xFF, 0xFF, 0x80, 0x7F };
const GXColor dat_8041ea28 = { 0xFF, 0xFF, 0x8B, 0x00 };

// .data: two quads of a 32x32 tile with the shared 32x32 atlas texture.
static u32 size32x32_tex32x32_vtx[0xE] = {
	0xFFF0FFF0, 0x00000000, 0x00000000, 0x00FF0010,
	0xFFF00000, 0x04000000, 0x000000FF, 0x00100010,
	0x00000400, 0x04000000, 0x00FFFFF0, 0x00100000,
	0x00000400, 0x000000FF,
};

static inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

EffectEntry* effBomheiN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 a5) {
	EffectEntry* effect;
	effBomheiData* data;

	effect = effEntry();
	effect->type = "BomheiN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effBomheiData));
	effect->userdata = data;
	effect->callback = effBomheiMain;
	data->field_0x14 = count;
	data->field_0x18 = 0;
	data->field_0x1C = count;
	data->field_0x20 = 0.0f;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x10 = a5;
	data->field_0x24 = 0.0f;
	if (type == 0) {
		data->field_0x28 = 0.0f;
	}
	else {
		data->field_0x28 = 40.0f;
	}
	return effect;
}

void effBomheiMain(EffectEntry* effect) {
	EffectEntry* e;
	effBomheiData* data;
	Vec position;

	e = effect;
	data = e->userdata;
	position = initializeVec((Vec*)&data->x);
	data->field_0x20 = float_9_8041ea4c * (f32)data->field_0x18 / (f32)data->field_0x1C;
	data->field_0x18++;
	data->field_0x14--;
	if (data->field_0x14 < 0) {
		effDelete(e);
		return;
	}

	data->field_0x24 += data->field_0x28;
	dispEntry(4, dispCalcZ(&position), 2, effBomheiDisp, e);
}

#ifdef MUST_MATCH
// Build the stage-2 colour in an inline so the original's colour stack slot (0x8) is used.
static inline GXColor bomheiColor(u8 alpha) {
	GXColor c = *(GXColor*)&dat_8041ea28;
	c.a = alpha;
	return c;
}
#endif

// 100%. The stage-2 colour is built by bomheiColor so its stack slot (0x8) matches the
// original; the only remaining objdiff lines are the ignored double_to_int_802fa2c0 reloc labels.
void effBomheiDisp(CameraId camId, void* param) {
	EffectEntry* effect;
	effBomheiData* data;
	CameraEntry* cam;
	GXColor color;
	GXTexObj texObj;
	Mtx trans;
	Mtx scale;
	Mtx rot;
	s32 whole;
	s32 frac;
	s32 next;

	effect = param;
	cam = camGetPtr(camId);
	data = effect->userdata;
	whole = (s32)data->field_0x20;
	frac = (s32)(float_256_8041ea2c * (data->field_0x20 - (f32)whole));

	GXSetNumChans(0);
	GXSetNumTevStages(3);
	GXSetTevOrder(0, 0, 0, 0xFF);
	GXSetTevColorOp(0, 0, 0, 0, 1, 3);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 3);
	GXSetTevColorIn(0, 0xF, 0xF, 0xF, 8);
	GXSetTevAlphaIn(0, 7, 7, 7, 4);
	GXSetTevOrder(1, 1, 1, 0xFF);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 6, 8, 5, 0xF);
	GXSetTevAlphaIn(1, 3, 4, 2, 7);
	GXSetTevOrder(2, 1, 1, 0xFF);
	GXSetTevColorOp(2, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
	GXSetTevColorIn(2, 2, 0xC, 0, 0xF);
	GXSetTevAlphaIn(2, 7, 7, 7, 0);
	effGetTexObjN64(0x35, &texObj);
	GXLoadTexObj(&texObj, 0);
	GXLoadTexObj(&texObj, 1);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
	GXSetTexCoordGen2(1, 1, 4, 0x21, 0, 0x7D);
	PSMTXScale(scale, float_0p03125_8041ea30, float_0p0034722_8041ea34, float_0_8041ea38);
	PSMTXTrans(trans, float_0_8041ea38, float_32_8041ea3c * (f32)whole, float_0_8041ea38);
	PSMTXConcat(scale, trans, trans);
	GXLoadTexMtxImm(trans, 0x1E, 1);
#ifdef MUST_MATCH
	// Scheduling/register aids: `x`/`s` hoist the x and scale loads before the frame clamp and `mp`
	// pins the matrix address so `next` lands in r4; the #else form is the natural expression.
	{
		f32 x = float_0_8041ea38;
		f32 s = float_32_8041ea3c;
		f32 (*mp)[4] = trans;
		next = 8;
		if (whole + 1 < 8) {
			next = whole + 1;
		}
		PSMTXTrans(mp, x, s * (f32)next, MATCH_F32(float_0_8041ea38));
	}
#else
	next = 8;
	if (whole + 1 < 8) {
		next = whole + 1;
	}
	PSMTXTrans(trans, float_0_8041ea38, float_32_8041ea3c * (f32)next, float_0_8041ea38);
#endif
	PSMTXConcat(scale, trans, trans);
	GXLoadTexMtxImm(trans, 0x21, 1);
	GXSetCullMode(0);
	effSetVtxDescN64((effVtxArray*)size32x32_tex32x32_vtx);

	PSMTXTrans(trans, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', float_deg2rad_8041ea40 * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXScale(scale, data->field_0x10, data->field_0x10, float_1_8041ea44);
	PSMTXConcat(trans, rot, trans);
	PSMTXConcat(trans, scale, trans);
	PSMTXRotRad(rot, 'z', float_deg2rad_8041ea40 * data->field_0x24);
	PSMTXConcat(trans, rot, trans);
	PSMTXConcat(cam->view, trans, trans);
	GXLoadPosMtxImm(trans, 0);
	GXSetCurrentMtx(0);

	if (data->field_0x20 > float_4_8041ea48) {
		GXSetTevColor(1, dat_8041ea20);
	}
	else {
		GXSetTevColor(1, dat_8041ea24);
	}
#ifdef MUST_MATCH
	color = bomheiColor(frac);
#else
	color = dat_8041ea28;
	color.a = frac;
#endif
	GXSetTevColor(2, color);
	GXBegin(0x90, 0, 6);
	tri2(0, 1, 2, 0, 0, 2, 3, 0);
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_256_8041ea2c = 256.0f;
const f32 float_0p03125_8041ea30 = 0.03125f;
const f32 float_0p0034722_8041ea34 = 0.0034722222f;
const f32 float_0_8041ea38 = 0.0f;
const f32 float_32_8041ea3c = 32.0f;
const f32 float_deg2rad_8041ea40 = 0.017453292f;
const f32 float_1_8041ea44 = 1.0f;
const f32 float_4_8041ea48 = 4.0f;
