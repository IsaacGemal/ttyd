#include "eff/n64/eff_thruhammer_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "math_ext.h"

// .sbss2: the default .sbss2 is only used for compiler-generated pool templates, so pair it with a small-data section.
#pragma section ".sdata" ".sbss2"
// .sdata zeroed GXColor scratch the original builds its TEV colors from.
__declspec(section ".sdata") u32 unk_804232a4;
__declspec(section ".sdata") u32 unk_804232a0;
__declspec(section ".sdata") u32 unk_8042329c;
__declspec(section ".sdata") u32 unk_80423298;



static void main_dl(EffectEntry* effect, Mtx matrix);

// Per-particle work data; same layout as EffThruHammerData in the header.
typedef struct ThruHammerWork {
	s16 type;        //0x00
	Vec position;    //0x04
	f32 dirX;        //0x10
	f32 dirZ;        //0x14
	f32 field_0x18;  //0x18
	f32 scaleX;      //0x1C
	f32 scaleY;      //0x20
	f32 scaleZ;      //0x24
	s32 life;        //0x28
	s32 age;         //0x2C
	f32 rotY;        //0x30
	f32 rotZ;        //0x34
	s32 alpha;       //0x38
	f32 texScrollA;  //0x3C
	f32 texSpeedA;   //0x40
	f32 texScrollB;  //0x44
	f32 texSpeedB;   //0x48
	s32 delay;       //0x4C
	u8 color[6];     //0x50
} ThruHammerWork;    //0x58



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
	ThruHammerWork* data;
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
	effect->type = "ThruHammerN64";
	effect->count = count + 1;
	data = __memAlloc(HEAP_EFFECT, (count + 1) * 0x58);
	effect->userdata = data;
	effect->callback = effThruHammerMain;

	data->life = 60;
	data->age = 0;
	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->alpha = 0xFF;
	data->color[0] = 0;
	data->color[1] = 0xFF;
	data->color[2] = 0x7A;
	data->color[3] = 0xF0;
	data->color[4] = 0xFF;
	data->color[5] = 0xFA;

	for (i = 1, data++; i < count + 1; i++, data++) {
		data->delay = -1 - (i - 1) * 4;
		data->rotZ = angle;
		data->rotY = (f32)(i * 30);
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

// The single-use temp forces sin's result to stay in f1 through the negate
// (retail: frsp f1,f1 / fneg f1,f1 / fmuls f1,f3,f1); the inline expression
// makes MWCC move it to f3 and load 0.5f into f1 instead.
void effThruHammerMain(EffectEntry* effect) {
	ThruHammerWork* data = effect->userdata;
	Vec disp;
	s32 i;

	disp = initializeVec(&data->position);

	data->age++;
	data->life--;
	if (data->life < 0) {
		effDelete(effect);
		return;
	}
	if (data->age >= 8) {
		data->alpha *= 0.8f;
	}
	for (i = 1, data++; i < effect->count; i++, data++) {
		data->delay++;
		if (data->delay < 0) {
			continue;
		}
		if (data->delay == 0) {
			data->position.x = 0.0f;
			data->position.y = 0.0f;
			data->position.z = 0.0f;
#ifdef MUST_MATCH
			{
				f32 s = (f32)sin(6.2831855f * data->rotZ / 360.0f);
				data->dirX = -s * 0.5f;
			}
#else
			data->dirX = -(f32)sin(6.2831855f * data->rotZ / 360.0f) * 0.5f;
#endif
			data->dirZ = 0.5f * (f32)cos(6.2831855f * data->rotZ / 360.0f);
			data->field_0x18 = 0.0f;
			data->texScrollA = 32.0f;
			data->texSpeedA = 32.0f;
			data->texSpeedB = 32.0f;
			data->scaleY = 10.0f;
			data->scaleX = 0.5f;
			data->scaleZ = 0.5f;
			data->texScrollB = 32.0f;
		}
		data->texScrollA += data->texSpeedA;
		data->texScrollB += data->texSpeedB;
		data->texSpeedA *= 0.9f;
		data->scaleX += 0.2f;
		data->scaleZ += 0.2f;
		data->scaleY += (0.1f - data->scaleY) * 0.2f;
		if (data->texScrollA > 128.0f) {
			data->texScrollA = 128.0f;
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

// The four GXColors are built and passed inside the void inlines above, which makes
// MWCC emit retail's per-call by-value slots (inline local just below the copy it passes).
void effThruHammerDisp(CameraId camId, void* param) {
	CameraEntry* cam = camGetPtr(camId);
	EffectEntry* effect = param;
	ThruHammerWork* data = effect->userdata;
	GXTexObj texObj;
	Mtx trans;
	Mtx rot;
	Mtx work;
	s32 alpha;
	u8 cr = data->color[0];
	u8 cg = data->color[1];
	u8 cb = data->color[2];
	u8 cr2 = data->color[3];
	u8 cg2 = data->color[4];
	u8 cb2 = data->color[5];

	GXSetNumChans(0);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_C1, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
	effGetTexObjN64(0x23, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	effGetTexObjN64(0x24, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP1);
	effSetVtxDescN64((effVtxArray*)stg_thruhammer_v);

	PSMTXTrans(trans, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(trans, rot, trans);
	PSMTXConcat(cam->view, trans, work);

	alpha = data->alpha;
#ifdef MUST_MATCH
	thruColor4(1, &unk_80423298, cr, cg, cb, alpha);
#else
	{
		GXColor color1 = *(GXColor*)&unk_80423298;
		color1.r = cr;
		color1.g = cg;
		color1.b = cb;
		color1.a = alpha;
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

	GXSetCullMode(GX_CULL_BACK);
	main_dl(effect, work);

#ifdef MUST_MATCH
	thruColor4(1, &unk_804232a0, cr, cg, cb, alpha);
#else
	{
		GXColor color3 = *(GXColor*)&unk_804232a0;
		color3.r = cr;
		color3.g = cg;
		color3.b = cb;
		color3.a = alpha;
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

	GXSetCullMode(GX_CULL_FRONT);
	main_dl(effect, work);
}

static void main_dl(EffectEntry* effect, Mtx matrix) {
	ThruHammerWork* data = effect->userdata;
	Mtx trans;
	Mtx work;
	Mtx rot;
	s32 i;

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		if (data->delay < 0) {
			continue;
		}

		PSMTXTrans(trans, data->position.x, data->position.y, data->position.z);
		PSMTXRotRad(rot, 'z', 0.017453292f * data->rotZ);
		PSMTXConcat(trans, rot, trans);
		PSMTXScale(work, 0.3f * data->scaleX, 0.3f * data->scaleY, 0.3f * data->scaleZ);
		PSMTXConcat(trans, work, trans);
		PSMTXRotRad(rot, 'y', 0.017453292f * data->rotY);
		PSMTXConcat(trans, rot, trans);
		PSMTXConcat(matrix, trans, trans);
		GXLoadPosMtxImm(trans, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);

		PSMTXScale(work, 0.015873017f, 0.032258064f, 0.0f);
		PSMTXTrans(trans, 0.0f, data->texScrollA / 4.0f, 0.0f);
		PSMTXConcat(work, trans, work);
		GXLoadTexMtxImm(work, GX_TEXMTX0, GX_MTX2x4);

		PSMTXScale(work, 0.12903225f, 0.12903225f, 0.0f);
		PSMTXTrans(trans, 0.0f, data->texScrollB / 4.0f, 0.0f);
		PSMTXConcat(work, trans, work);
		GXLoadTexMtxImm(work, GX_TEXMTX1, GX_MTX2x4);

		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0x36);
		tri2(0, 1, 2, 0, 2, 3, 4, 0);
		tri2(2, 4, 5, 0, 1, 3, 2, 0);
		tri2(4, 6, 5, 0, 5, 6, 7, 0);
		tri2(5, 7, 8, 0, 0, 9, 1, 0);
		tri2(0xa, 9, 0, 0, 0xb, 0xa, 0, 0);
		tri2(0xb, 0xc, 0xa, 0, 0xd, 0xc, 0xb, 0);
		tri2(8, 0xe, 0xf, 0, 7, 0xe, 8, 0);
		tri2(8, 0xf, 0x10, 0, 0xf, 0x11, 0x10, 0);
		tri2(0x10, 0x11, 0x12, 0, 0x10, 0x12, 0x13, 0);
	}
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
