#include "matching.h"
#include "eff/eff_mist2.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "system.h"
#include "math_ext.h"

extern const f32 float_deg2rad_80421ca8;
extern const f32 float_16_80421cac;
extern const f32 float_255_80421cb0;
extern const f32 float_0p5_80421cb4;

// .sbss2: the default .sbss2 is only used for compiler-generated pool
// templates, so pair it with a small-data section and place it by hand.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_80423340;

#ifdef MUST_MATCH
// Build the GXColor in a helper so the by-value argument lands in stack 0xc, as retail does.
static inline GXColor effMist2Color(u8 r, u8 g, u8 b, u8 a) {
	GXColor c = *(GXColor*)&unk_80423340;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	return c;
}
#endif

EffectEntry* effMist2Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 range, f32 yOffset) {
	EffectEntry* entry;
	EffMist2Data* data;
	EffMist2Data* elem;
	s32 num;
	s32 count;
	s32 i;
	f32 fAngle;
	f32 fRange;

	entry = effEntry();
	num = (type == 0) ? 30 : 10;
	count = num + 1;
	entry->type = "Mist2";
	entry->count = count;
	data = __memAlloc(HEAP_EFFECT, count * sizeof(EffMist2Data));
	entry->userdata = data;
	entry->callback = effMist2Main;
	entry->flags |= 2;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	if (lifetime <= 0) {
		data->unk34 = 1000;
	} else {
		data->unk34 = lifetime;
	}
	data->unk30 = 0;
	data->unk2C = float_255_80421cb0;
	data->unk18 = 1.0f;
	data->unk38 = 0;
	data->unk39 = 0;
	data->unk3A = 0;

	elem = &data[1];
	for (i = 1; i < entry->count; i++, elem++) {
		elem->unk1C = range;
		elem->unk20 = yOffset;
		fRange = (f32)irand((s32)elem->unk1C);
		fAngle = 6.2831855f * (f32)irand(360) / 360.f;
		elem->x = fRange * (f32)sin(fAngle);
		elem->y = 0.0f;
		elem->z = fRange * (f32)cos(fAngle);
		elem->unk18 = 0.0f;
		elem->unk14 = float_0p5_80421cb4 + (f32)irand(100) / 100.f;
		elem->unk2C = float_255_80421cb0;
		elem->unk34 = irand(60) + 60;
		elem->unk30 = 0;
		elem->unk28 = (f32)(5 - irand(10));
		elem->unk24 = 0.0f;
		elem->unk3C = irand(30);
		elem->unk40 = 0.2f;
		elem->unk44 = 0.0f;
		if (type == 1) {
			f32 fA = 6.2831855f * (f32)irand(360) / 360.f;
			elem->x = elem->unk1C * (f32)sin(fA);
			elem->y = elem->unk20;
			elem->z = elem->unk1C * (f32)cos(fA);
		}
	}
	return entry;
}

void effMist2Main(EffectEntry* effect) {
	EffMist2Data* data = effect->userdata;
	Vec position;
	Vec tmp = (Vec){ 0.0f, 0.0f, 0.0f };
	s32 i;
	s32 type;
	f32 fAngle;
	f32 fRange;

	tmp.x = data->x;
	tmp.y = data->y;
	tmp.z = data->z;
	position = tmp;
	type = data->type;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->unk34 = 0x20;
	}
	if (data->unk34 < 1000) {
		data->unk34--;
	}
	if (data->unk34 < 0) {
		effDelete(effect);
		return;
	}
	if (data->unk34 < 0x20) {
		data->unk2C = (f32)(data->unk34 << 3);
	}

	data->unk30++;
	data++;
	for (i = 1; i < effect->count; i++, data++) {
		if (data->unk3C != 0) {
			data->unk3C--;
			continue;
		}
		data->unk34--;
		if (data->unk34 < 0x20) {
			data->unk2C = (f32)(data->unk34 << 3);
		}
		if (data->unk2C <= 0.0f) {
			fRange = (f32)irand((s32)data->unk1C);
			fAngle = 6.2831855f * (f32)irand(360) / 360.f;
			data->x = fRange * (f32)sin(fAngle);
			data->y = 0.0f;
			data->z = fRange * (f32)cos(fAngle);
			data->unk18 = 0.0f;
			data->unk14 = float_0p5_80421cb4 + (f32)irand(100) / 100.f;
			data->unk2C = float_255_80421cb0;
			data->unk34 = irand(60) + 60;
			data->unk30 = 0;
			data->unk28 = (f32)(5 - irand(10));
			data->unk24 = 0.0f;
			data->unk3C = irand(30);
			data->unk40 = 0.2f;
			data->unk44 = 0.0f;
		} else {
			data->unk24 += data->unk28;
			data->unk18 += (data->unk14 - data->unk18) / 10.f;
			if (type == 1) {
				data->y += data->unk44;
				data->unk44 -= data->unk40;
				if (data->y < 0.0f) {
					data->y = 0.0f;
				}
			}
		}
	}
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effMist2Disp, effect);
}

// The GXColor build is in the MUST_MATCH inline above so the by-value argument lands in stack
// 0xc, matching retail.
void effMist2Disp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	EffMist2Data* work = effect->userdata;
	CameraEntry* cam;
	GXTexObj texObj;
	GXColor color;
	Mtx mtxTrans;
	Mtx mtxRot;
	Mtx mtxScale;
	Mtx mtxView;
	s32 i;
	s32 alpha;
	u8 cr;
	u8 cg;
	u8 cb;
	f32 f28;
	f32 f27;

	cam = camGetPtr(cameraId);
	alpha = (s32)work->unk2C;
	cr = work->unk38;
	cg = work->unk39;
	cb = work->unk3A;
	effGetTexObj(0x5E, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C, GX_FALSE, 0x7D);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
	PSMTXTrans(mtxTrans, work->x, work->y, work->z);
	PSMTXRotRad(mtxRot, 'y', float_deg2rad_80421ca8 * -camGetPtr(cameraId)->viewYaw);
	PSMTXScale(mtxScale, work->unk18, work->unk18, work->unk18);
	PSMTXConcat(mtxTrans, mtxRot, mtxView);
	PSMTXConcat(mtxView, mtxScale, mtxView);
	PSMTXConcat(cam->view, mtxView, mtxView);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	f28 = -float_16_80421cac * float_0p5_80421cb4;
	work = &work[1];
	for (i = 1; i < effect->count; i++, work++) {
		PSMTXTrans(mtxTrans, work->x, work->y, work->z);
		PSMTXScale(mtxScale, work->unk18, work->unk18, work->unk18);
		PSMTXRotRad(mtxRot, 'z', float_deg2rad_80421ca8 * work->unk24);
		PSMTXConcat(mtxTrans, mtxScale, mtxTrans);
		PSMTXConcat(mtxTrans, mtxRot, mtxTrans);
		PSMTXConcat(mtxView, mtxTrans, mtxTrans);
		GXLoadPosMtxImm(mtxTrans, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
#ifdef MUST_MATCH
		color = effMist2Color(cr, cg, cb, (u8)(s32)(work->unk2C * (f32)alpha / float_255_80421cb0));
#else
		color = unk_80423340;
		color.r = cr;
		color.g = cg;
		color.b = cb;
		color.a = (u8)(s32)(work->unk2C * (f32)alpha / float_255_80421cb0);
#endif
		f27 = 8.0f;
		GXSetChanMatColor(GX_COLOR0A0, color);
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(f28, f27, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(f27, f27, 0.0f);
		GXTexCoord2f32(1.0f, 0.0f);
		GXPosition3f32(f27, f28, 0.0f);
		GXTexCoord2f32(1.0f, 1.0f);
		GXPosition3f32(f28, f28, 0.0f);
		GXTexCoord2f32(0.0f, 1.0f);
	}
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_deg2rad_80421ca8 = 0.017453292f;
const f32 float_16_80421cac = 16.0f;
const f32 float_255_80421cb0 = 255.0f;
const f32 float_0p5_80421cb4 = 0.5f;
