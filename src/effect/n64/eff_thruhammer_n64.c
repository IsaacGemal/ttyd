#include "eff/n64/eff_thruhammer_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <math.h>

extern const f32 float_deg2rad_8041ffe0;
extern u32 unk_80423298;
extern u32 unk_8042329c;
extern u32 unk_804232a0;
extern u32 unk_804232a4;

static void main_dl(EffectEntry* effect, Mtx matrix);

const char str_802FB858[] = "ThruHammerN64";

const f64 double_to_int_802fb850 = 4503601774854144.0;

static u32 stg_thruhammer_v[0x118 / 4] = {
    0xFFDEFFFE, 0x001410AB, 0x0400FFFF, 0xFFFFFFCC, 0x00320000, 0x0F000800, 0xFFFFFF00,
    0xFFDEFFFE, 0xFFEC0D55, 0x0400FFFF, 0xFFFFFFCC, 0x0032FFE2, 0x0D550800, 0xFFFFFF00,
    0xFFE60032, 0xFFD30BAB, 0x0800FFFF, 0xFF000000, 0xFFFEFFD8, 0x0A000400, 0xFFFFFFFF,
    0x00000032, 0xFFC40A00, 0x0800FFFF, 0xFF00001A, 0x0032FFD3, 0x08550800, 0xFFFFFF00,
    0x0022FFFE, 0xFFEC06AB, 0x0400FFFF, 0xFFFFFFCC, 0x0032001E, 0x10AB0800, 0xFFFFFF00,
    0xFFE60032, 0x002D1255, 0x0800FFFF, 0xFF000000, 0xFFFE0028, 0x14000400, 0xFFFFFFFF,
    0x00000032, 0x003C1400, 0x0800FFFF, 0xFF00001A, 0x0032002D, 0x15AB0800, 0xFFFFFF00,
    0x00340032, 0xFFE206AB, 0x0800FFFF, 0xFF000034, 0x00320000, 0x05000800, 0xFFFFFF00,
    0x0022FFFE, 0x00140355, 0x0400FFFF, 0xFFFF0034, 0x0032001E, 0x03550800, 0xFFFFFF00,
    0x001A0032, 0x002D01AB, 0x0800FFFF, 0xFF000000, 0xFFFE0028, 0x00000400, 0xFFFFFFFF,
};

EffectEntry* effThruHammerN64Entry(s32 type, f32 x, f32 y, f32 z, f32 angle) {
	EffectEntry* effect;
	EffThruHammerData* data;
	s32 count;
	s32 i;

	effect = effEntry();
	count = 1;
	switch (type) {
	case 0:
		count = 4;
		break;
	case 1:
	default:
		count = 1;
		break;
	}
	effect->type = str_802FB858;
	effect->count = count + 1;
	data = __memAlloc(HEAP_EFFECT, (count + 1) * 0x58);
	effect->userdata = data;
	effect->callback = effThruHammerMain;

	data->field_0x28 = 60;
	data->field_0x2C = 0;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x38 = 0xFF;
	data->field_0x50 = 0;
	data->field_0x51 = 0xFF;
	data->field_0x52 = 0x7A;
	data->field_0x53 = 0xF0;
	data->field_0x54 = 0xFF;
	data->field_0x55 = 0xFA;

	for (i = 1, data++; i < count + 1; i++, data++) {
		data->field_0x4C = -1 - (i - 1) * 4;
		data->field_0x34 = angle;
		data->field_0x30 = (f32)(i * 30);
	}

	return effect;
}

//local helper
static inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };

	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

// NONMATCHING (99.80%): only the sin block's register allocation differs (sin in f3
// instead of f1); the rest are anonymous .sdata2/.rodata pool labels.
void effThruHammerMain(EffectEntry* effect) {
	EffThruHammerData* data = effect->userdata;
	Vec disp;
	s32 i;

	disp = initializeVec((Vec*)&data->x);

	data->field_0x2C++;
	data->field_0x28--;
	if (data->field_0x28 < 0) {
		effDelete(effect);
		return;
	}
	if (data->field_0x2C >= 8) {
		data->field_0x38 *= 0.8f;
	}
	for (i = 1, data++; i < effect->count; i++, data++) {
		data->field_0x4C++;
		if (data->field_0x4C < 0) {
			continue;
		}
		if (data->field_0x4C == 0) {
			data->x = 0.0f;
			data->y = 0.0f;
			data->z = 0.0f;
			data->field_0x10 = -(f32)sin(6.2831855f * data->field_0x34 / 360.0f) * 0.5f;
			data->field_0x14 = 0.5f * (f32)cos(6.2831855f * data->field_0x34 / 360.0f);
			data->field_0x18 = 0.0f;
			data->field_0x3C = 32.0f;
			data->field_0x40 = 32.0f;
			data->field_0x48 = 32.0f;
			data->field_0x20 = 10.0f;
			data->field_0x1C = 0.5f;
			data->field_0x24 = 0.5f;
			data->field_0x44 = 32.0f;
		}
		data->field_0x3C += data->field_0x40;
		data->field_0x44 += data->field_0x48;
		data->field_0x40 *= 0.9f;
		data->field_0x1C += 0.2f;
		data->field_0x24 += 0.2f;
		data->field_0x20 += (0.1f - data->field_0x20) * 0.2f;
		if (data->field_0x3C > 128.0f) {
			data->field_0x3C = 128.0f;
		}
	}
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effThruHammerDisp, effect);
}

#ifdef MUST_MATCH
/* MUST_MATCH: setting each colour inside a void inline (rather than assigning a local and
   returning/passing it) reproduces the original's per-call by-value colour slots, with the
   inline's build local just below the copy the call passes (same shape as effGlassDisp). */
static inline void thruColor4(s32 reg, u32* base, u8 r, u8 g, u8 b, s32 a) {
	GXColor c = *(GXColor*)base;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	GXSetTevColor(reg, c);
}

static inline void thruColor3(s32 reg, u32* base, u8 r, u8 g, u8 b) {
	GXColor c = *(GXColor*)base;
	c.r = r;
	c.g = g;
	c.b = b;
	GXSetTevColor(reg, c);
}
#endif

// 100%: the four GXColors are built and passed inside the void inlines above, which makes
// MWCC emit retail's per-call by-value slots (inline local just below the copy it passes).
void effThruHammerDisp(CameraId camId, void* param) {
	CameraEntry* cam = camGetPtr(camId);
	EffectEntry* effect = param;
	EffThruHammerData* data = effect->userdata;
	GXTexObj texObj;
	Mtx trans;
	Mtx rot;
	Mtx work;
	s32 ca;
	u8 cr = data->field_0x50;
	u8 cg = data->field_0x51;
	u8 cb = data->field_0x52;
	u8 cr2 = data->field_0x53;
	u8 cg2 = data->field_0x54;
	u8 cb2 = data->field_0x55;

	GXSetNumChans(0);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	GXSetTexCoordGen2(1, 1, 4, 0x21, 0, 0x7d);
	GXSetTevOrder(0, 0, 0, 0xff);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xf, 0xf, 0xf, 0xf);
	GXSetTevAlphaIn(0, 7, 1, 4, 7);
	GXSetTevOrder(1, 1, 1, 0xff);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 4, 2, 8, 0xf);
	GXSetTevAlphaIn(1, 7, 7, 7, 0);
	effGetTexObjN64(0x23, &texObj);
	GXLoadTexObj(&texObj, 0);
	effGetTexObjN64(0x24, &texObj);
	GXLoadTexObj(&texObj, 1);
	effSetVtxDescN64((effVtxArray*)stg_thruhammer_v);

	PSMTXTrans(trans, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', float_deg2rad_8041ffe0 * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(trans, rot, trans);
	PSMTXConcat(cam->view, trans, work);

	ca = data->field_0x38;
#ifdef MUST_MATCH
	thruColor4(1, &unk_80423298, cr, cg, cb, ca);
#else
	{
		GXColor color1 = *(GXColor*)&unk_80423298;
		color1.r = cr;
		color1.g = cg;
		color1.b = cb;
		color1.a = ca;
		GXSetTevColor(1, color1);
	}
#endif

#ifdef MUST_MATCH
	thruColor3(2, &unk_8042329c, cr2, cg2, cb2);
#else
	{
		GXColor color2 = *(GXColor*)&unk_8042329c;
		color2.r = cr2;
		color2.g = cg2;
		color2.b = cb2;
		GXSetTevColor(2, color2);
	}
#endif

	GXSetCullMode(2);
	main_dl(effect, work);

#ifdef MUST_MATCH
	thruColor4(1, &unk_804232a0, cr, cg, cb, ca);
#else
	{
		GXColor color3 = *(GXColor*)&unk_804232a0;
		color3.r = cr;
		color3.g = cg;
		color3.b = cb;
		color3.a = ca;
		GXSetTevColor(1, color3);
	}
#endif

#ifdef MUST_MATCH
	thruColor3(2, &unk_804232a4, cr2, cg2, cb2);
#else
	{
		GXColor color4 = *(GXColor*)&unk_804232a4;
		color4.r = cr2;
		color4.g = cg2;
		color4.b = cb2;
		GXSetTevColor(2, color4);
	}
#endif

	GXSetCullMode(1);
	main_dl(effect, work);
}
