#include "eff/eff_itemget.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "mario/mariost.h"
#include "math_ext.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

static const Vec vec3_802fdda8 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802fddb4 = { 0.0f, 0.0f, 1000.0f };
static const Vec vec3_802fddc0 = { 0.0f, 1.0f, 0.0f };
static const Vec vec3_802fddcc = { 0.0f, 0.0f, 0.0f };

static const f64 double_to_int_mask_802fddd8 = 4503599627370496.0;
static const f64 double_to_int_802fdde8 = 4503601774854144.0;

static const char str_802FDDE0[] = "ItemGet";

static u8 scale_data[12] = {
	0x00, 0x12, 0x26, 0x3B, 0x4E, 0x60, 0x6E, 0x78, 0x79, 0x72, 0x69, 0x64
};
GXColor white = { 0xE1, 0xE1, 0xE1, 0xFF };
static GXColor color_tbl[10] = {
	{ 0xC8, 0xB2, 0x25, 0xFF },
	{ 0x26, 0xA3, 0x52, 0xFF },
	{ 0x5C, 0xCA, 0x10, 0xFF },
	{ 0xCB, 0x4C, 0x4D, 0xFF },
	{ 0x7E, 0xC2, 0x1E, 0xFF },
	{ 0x30, 0x9C, 0xBF, 0xFF },
	{ 0xEB, 0x7D, 0x06, 0xFF },
	{ 0xE8, 0x5B, 0xD0, 0xFF },
	{ 0x51, 0x75, 0x9D, 0xFF },
	{ 0xC8, 0xB2, 0x25, 0xFF },
};
u8 size_tbl[12] = {
	0x32, 0x32, 0x32, 0x28, 0x0C, 0x0C, 0x0C, 0x0A, 0x0A, 0x0F, 0x00, 0x00
};
static const GXColor dat_80420b98 = { 0xFF, 0xFF, 0xFF, 0x00 };
static const GXColor dat_80420b9c = { 0xFF, 0xFF, 0xFF, 0x00 };
static const GXColor dat_80420ba0 = { 0xFF, 0xFF, 0xFF, 0x00 };
GXColor unk_804232f0;
GXColor unk_804232f4;
GXColor unk_804232f8;

EffectEntry* effItemGetEntry(s32 id, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	effItemGetData* data;
	effItemGetData* ptr;
	s32 count;
	s32 i;

	entry = effEntry();
	count = 1;
	if (id == 0 || id == 3 || id == 9) {
		count = 8;
	} else if (id == 10) {
		count = 6;
	}
	entry->type = str_802FDDE0;
	entry->count = count;
	data = __memAlloc(HEAP_EFFECT, entry->count * sizeof(effItemGetData));
	entry->userdata = data;
	entry->callback = effItemGetMain;
	entry->flags |= 2;
	data->field_0x0 = id;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->field_0x10 = 0.0f;
	data->field_0x14 = (f32)size_tbl[data->field_0x0] / 10.0f * 0.75f;
	data->field_0x1C = 0;
	data->field_0x20 = 0;
	data->field_0x38 = 4;
	data->field_0x34 = 0xFF;
	data->field_0x18 = 1000;
	ptr = data + 1;
	for (i = 1; i < entry->count; i++, ptr++) {
		ptr->position.x = 0.0f;
		ptr->position.y = 0.0f;
		ptr->position.z = 0.0f;
		if (id == 10) {
			f32 c;
			ptr->field_0x30 = 1.0471976f + 6.2831855f * (i - 1) / 6.0f;
			ptr->field_0x24 = 5.0f * (f32)sin(ptr->field_0x30);
			c = (f32)cos(ptr->field_0x30);
			ptr->field_0x28 = -c * 5.0f;
			ptr->field_0x2C = 0.0f;
		} else {
			ptr->field_0x30 = 6.2831855f * (i - 1) / (entry->count - 1);
			ptr->field_0x24 = (f32)sin(ptr->field_0x30);
			ptr->field_0x28 = -(f32)cos(ptr->field_0x30);
			ptr->field_0x2C = 0.0f;
		}
		ptr->field_0x34 = 0;
		ptr->field_0x1C = 0;
	}
	return entry;
}

// Local copy of system.c's inline helper so it inlines here (as in eff_rankup.c).
inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

void effItemGetMain(EffectEntry* effect) {
	effItemGetData* data = effect->userdata;
	Vec disp;
	s32 zeroCount = 0;
	s32 id;
	s32 camera;
	s32 i;

	disp = initializeVec(&data->position);

	camera = data->field_0x38;
	id = data->field_0x0;
	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->field_0x18 = 16;
	}
	if (data->field_0x18 < 1000) {
		data->field_0x18--;
	}
	if (data->field_0x18 < 16) {
		data->field_0x34 = data->field_0x18 << 4;
	}
	if (data->field_0x18 < 0) {
		effDelete(effect);
		return;
	}
	data->field_0x1C++;
	if ((u32)data->field_0x1C < 12) {
		data->field_0x10 = (f32)scale_data[data->field_0x1C] / 100.0f;
	}
	if (data->field_0x1C > 1000) {
		data->field_0x1C = 1000;
	}
	switch (data->field_0x20) {
	case 0:
		break;
	case 1:
		if (data->field_0x10 < 100.0f) {
			data->field_0x10 *= 1.1f;
		}
		break;
	case 2:
		data->field_0x10 *= 0.9f;
		break;
	}
	data++;
	for (i = 1; i < effect->count; i++, data++) {
		data->field_0x1C++;
		if (data->field_0x1C < 16) {
			data->field_0x34 += 16;
			if (data->field_0x34 > 0xFF) {
				data->field_0x34 = 0xFF;
			}
		} else {
			data->field_0x34 -= 8;
			if (data->field_0x34 < 0) {
				data->field_0x34 = 0;
			}
		}
		if (data->field_0x1C > 1000) {
			data->field_0x1C = 1000;
		}
		if (data->field_0x34 == 0) {
			zeroCount++;
		} else {
			data->position.x += data->field_0x24;
			data->position.y += data->field_0x28;
			data->position.z += data->field_0x2C;
			if (id == 10) {
				data->field_0x24 *= 0.9f;
				data->field_0x28 *= 0.9f;
				data->field_0x2C *= 0.9f;
			}
		}
	}
	if (id == 10) {
		if (zeroCount >= effect->count - 1) {
			effDelete(effect);
			return;
		}
		dispEntry(camera, dispCalcZ(&disp), 2, effItemUseDisp, effect);
	} else {
		dispEntry(camera, dispCalcZ(&disp), 2, effItemGetDisp, effect);
	}
}

#ifdef MUST_MATCH
// builds the colour in the same stack temp the original GXSetChanMatColor call used
static inline GXColor itemColor0(u8 a, GXColor c) {
	c.a = a;
	return c;
}
#endif

void effItemGetDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effItemGetData* data = effect->userdata;
	CameraEntry* cam;
	Mtx mtxTrans;
	Mtx mtxScale;
	Mtx mtxRot;
	Mtx mtxTex;
	Mtx mtxLight;
	Mtx mtxLook;
	Mtx mtxBase;
	GXTexObj texObj;
	Vec vecA;
	Vec vecB;
	Vec vecC;
	s32 alphaB;
	s32 alphaA;
	s32 field;
	s32 i;
	s32 j;
	f32 f31;
	f32 f30;
	f32 f30b;
	f32 f29;
	f32 scale;

	cam = camGetPtr(camId);
	vecA = vec3_802fddb4;
	vecB = vec3_802fddc0;
	vecC = vec3_802fddcc;
	scale = data->field_0x10 * data->field_0x14;
	field = data->field_0x34;

	GXSetBlendMode(1, 4, 5, 0);
	GXSetZCompLoc(1);
	GXSetAlphaCompare(7, 0, 0, 7, 0);
	GXSetZMode(1, 3, 0);
	PSMTXTrans(mtxTrans, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(mtxRot, 'y', -camGetPtr(camId)->viewYaw * 0.017453292f);
	PSMTXScale(mtxScale, scale, scale, scale);
	PSMTXConcat(mtxTrans, mtxRot, mtxTrans);
	PSMTXConcat(mtxTrans, mtxScale, mtxBase);
	f31 = (f32)cos(6.2831855f * camGetPtr(camId)->viewYaw / 360.0f);
	PSMTXTrans(mtxTrans, 2.0f * (f32)sin(6.2831855f * camGetPtr(camId)->viewYaw / 360.0f), 0.0f, 2.0f * -f31);
	PSMTXConcat(mtxTrans, mtxBase, mtxBase);

	GXSetTevColor(1, white);
	GXSetTevColor(2, color_tbl[data->field_0x0]);
	C_MTXLightOrtho(mtxLight, 0.0f, 6.0f, 0.0f, 6.0f, 0.5f, 0.5f, 0.5f, 0.5f);
	PSMTXMultVec(mtxRot, &vecA, &vecA);
	C_MTXLookAt(mtxLook, &vecA, &vecB, &vecC);
	PSMTXConcat(mtxLight, mtxLook, mtxLight);
	PSMTXConcat(mtxLight, mtxBase, mtxTex);

	GXSetNumTexGens(2);
	GXSetTexCoordGen2(0, 1, 0, 0x1E, 0, 0x7D);
	GXSetTexCoordGen2(1, 1, 4, 0x3C, 0, 0x7D);
	GXLoadTexMtxImm(mtxTex, 0x1E, 1);
	GXSetNumTevStages(2);
	GXSetTevOrder(0, 0, 0, 0xFF);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 2, 4, 8, 0xF);
	GXSetTevAlphaIn(0, 7, 7, 7, 7);
	GXSetTevOrder(1, 1, 1, 4);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 0xF, 0, 0xA, 0xF);
	GXSetTevAlphaIn(1, 7, 4, 5, 7);
	effGetTexObj(0x2C, &texObj);
	GXLoadTexObj(&texObj, 0);
	effGetTexObj(0x2A, &texObj);
	GXLoadTexObj(&texObj, 1);
	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
	GXSetCullMode(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xD, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);

	alphaA = field * 128 / 255;
	alphaB = field * 255 / 255;
	f31 = 32.0f;
	{
		f32 f = 64.0f;
		f32 half = 0.5f;
		f30 = -f * half;
	}
	for (i = 0; i < 2; i++) {
		if (i == 0) {
			PSMTXConcat(cam->view, mtxBase, mtxTrans);
			PSMTXTrans(mtxRot, 2.0f, -2.0f, 0.0f);
			PSMTXConcat(mtxTrans, mtxRot, mtxTrans);
#ifdef MUST_MATCH
			GXSetChanMatColor(4, itemColor0(alphaA, unk_804232f0));
#else
			{
				GXColor c = unk_804232f0;
				c.a = alphaA;
				GXSetChanMatColor(4, c);
			}
#endif
		} else {
			PSMTXConcat(cam->view, mtxBase, mtxTrans);
#ifdef MUST_MATCH
			GXSetChanMatColor(4, itemColor0(alphaB, dat_80420b98));
#else
			{
				GXColor c = dat_80420b98;
				c.a = alphaB;
				GXSetChanMatColor(4, c);
			}
#endif
		}
		GXLoadPosMtxImm(mtxTrans, 0);
		GXSetCurrentMtx(0);
		GXBegin(0x80, 0, 4);
		GXPosition3f32(f30, f31, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(f31, f31, 0.0f);
		GXTexCoord2f32(2.0f, 0.0f);
		GXPosition3f32(f31, f30, 0.0f);
		GXTexCoord2f32(2.0f, 2.0f);
		GXPosition3f32(f30, f30, 0.0f);
		GXTexCoord2f32(0.0f, 2.0f);
	}

	GXSetTevColor(1, color_tbl[data->field_0x0]);
	GXSetTevColor(2, white);
	effGetTexObj(0x2B, &texObj);
	GXLoadTexObj(&texObj, 1);

	{
		f32 f42 = 42.666668f;
		f30b = -f42;
	}
	f31 = 5.3333335f;
#ifdef MUST_MATCH
	// The half-size is computed inside the loop; MWCC hoists the negate back to the preheader but
	// colours its scratch in f2 (as the original) instead of fusing it into the product destination's
	// register.  `f30b` gives the direct `-42.666668f` its own variable (reused as machine f30), which
	// puts the first-loop cos result in f31 as the original.  The values are idempotent, so recomputing
	// them per iteration is harmless.
#else
	{
		f32 f10 = 10.666667f;
		f32 half = 0.5f;
		f29 = -f10 * half;
	}
#endif
	i = 1;
	data++;
	for (; i < effect->count; i++, data++) {
#ifdef MUST_MATCH
		{
			f32 f = 10.666667f;
			f29 = -f;
		}
		f29 = f29 / 2.0f;
#endif
		for (j = 0; j < 2; j++) {
			if (j == 0) {
				alphaA = data->field_0x34 * 128 / 255;
				if (data->field_0x34 != 0xFF) {
					alphaA -= 20;
					if (alphaA < 0) {
						alphaA = 0;
					}
				}
				PSMTXTrans(mtxTrans, data->position.x, data->position.y, data->position.z);
				PSMTXConcat(mtxBase, mtxTrans, mtxTex);
				PSMTXTrans(mtxTrans, 2.0f, -2.0f, 0.0f);
				PSMTXConcat(mtxTex, mtxTrans, mtxTex);
				PSMTXRotRad(mtxRot, 'z', data->field_0x30);
				PSMTXConcat(mtxTex, mtxRot, mtxTex);
#ifdef MUST_MATCH
				GXSetChanMatColor(4, itemColor0(alphaA, unk_804232f4));
#else
				{
					GXColor c = unk_804232f4;
					c.a = alphaA;
					GXSetChanMatColor(4, c);
				}
#endif
			} else {
				PSMTXTrans(mtxTrans, data->position.x, data->position.y, data->position.z);
				PSMTXConcat(mtxBase, mtxTrans, mtxTex);
				PSMTXRotRad(mtxRot, 'z', data->field_0x30);
				PSMTXConcat(mtxTex, mtxRot, mtxTex);
#ifdef MUST_MATCH
				GXSetChanMatColor(4, itemColor0(data->field_0x34 * 255 / 255, dat_80420b9c));
#else
				{
					GXColor c = dat_80420b9c;
					c.a = data->field_0x34 * 255 / 255;
					GXSetChanMatColor(4, c);
				}
#endif
			}
			PSMTXConcat(cam->view, mtxTex, mtxTrans);
			GXLoadPosMtxImm(mtxTrans, 0);
			GXSetCurrentMtx(0);
			PSMTXConcat(mtxLight, mtxTex, mtxTrans);
			GXLoadTexMtxImm(mtxTrans, 0x1E, 0);
			GXBegin(0x80, 0, 4);
			GXPosition3f32(f29, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(f31, 0.0f, 0.0f);
			GXTexCoord2f32(2.0f, 0.0f);
			GXPosition3f32(f31, f30b, 0.0f);
			GXTexCoord2f32(2.0f, 1.0f);
			GXPosition3f32(f29, f30b, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
		}
	}
}

#ifdef MUST_MATCH
// builds the colour in the same stack temp the original GXSetChanMatColor call used
static inline GXColor itemColor1(u8 a, GXColor c) {
	c.a = a;
	return c;
}

#endif

void effItemUseDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effItemGetData* data = effect->userdata;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx mtx1;
	Mtx mtx3;
	Mtx mtx2;
	Mtx mtx4;
	s32 i;
	s32 j;
	f32 f30;
	f32 f31;
	f32 f29;

	cam = camGetPtr(camId);
	GXSetBlendMode(1, 4, 5, 0);
	GXSetZCompLoc(1);
	GXSetAlphaCompare(7, 0, 0, 7, 0);
	GXSetZMode(1, 3, 0);
	PSMTXTrans(mtx1, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(mtx2, 'y', -camGetPtr(CAMERA_3D)->viewYaw * 0.017453292f);
	PSMTXScale(mtx3, 0.4f, 0.4f, 0.4f);
	PSMTXConcat(mtx1, mtx3, mtx4);
	PSMTXConcat(mtx4, mtx2, mtx4);
	PSMTXConcat(cam->view, mtx4, mtx4);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 4);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevOp(0, 0);
	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
	GXSetCullMode(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xD, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
	effGetTexObj(0x2B, &texObj);
	GXLoadTexObj(&texObj, 0);

#ifdef MUST_MATCH
	// The half-size is computed inside the loop after the j==0 branch's 2.0f call; MWCC hoists it to
	// the preheader, colours the product's negate scratch in f2 (as the original) and pools 2.0 before
	// 0.5.  The values are idempotent, so recomputing them per iteration is harmless.
#else
	{
		f32 f = 21.333334f;
		f31 = -f;
	}
	f31 = f31 / 2.0f;
	{
		f32 f = 42.666668f;
		f30 = -f;
	}
	f29 = 10.666667f;
#endif
	i = 1;
	data++;
	for (; i < effect->count; i++, data++) {
		for (j = 0; j < 2; j++) {
			if (j == 0) {
				PSMTXTrans(mtx1, data->position.x + 2.0f, data->position.y - 2.0f, data->position.z);
				PSMTXRotRad(mtx2, 'z', data->field_0x30);
				PSMTXConcat(mtx1, mtx2, mtx1);
				PSMTXConcat(mtx4, mtx1, mtx1);
#ifdef MUST_MATCH
				GXSetChanMatColor(4, itemColor1((data->field_0x34 * 64) / 255, unk_804232f8));
#else
				{
					GXColor color = unk_804232f8;
					color.a = (data->field_0x34 * 64) / 255;
					GXSetChanMatColor(4, color);
				}
#endif
			} else {
				PSMTXTrans(mtx1, data->position.x, data->position.y, data->position.z);
				PSMTXRotRad(mtx2, 'z', data->field_0x30);
				PSMTXConcat(mtx1, mtx2, mtx1);
				PSMTXConcat(mtx4, mtx1, mtx1);
#ifdef MUST_MATCH
				GXSetChanMatColor(4, itemColor1((data->field_0x34 * 128) / 255, dat_80420ba0));
#else
				{
					GXColor color2 = dat_80420ba0;
					color2.a = (data->field_0x34 * 128) / 255;
					GXSetChanMatColor(4, color2);
				}
#endif
			}
#ifdef MUST_MATCH
			{
				f32 f = 21.333334f;
				f31 = -f;
			}
			f31 = f31 / 2.0f;
			{
				f32 f = 42.666668f;
				f30 = -f;
			}
			f29 = 10.666667f;
#endif
			GXLoadPosMtxImm(mtx1, 0);
			GXSetCurrentMtx(0);
			GXBegin(0x80, 0, 4);
			GXPosition3f32(f31, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(f29, 0.0f, 0.0f);
			GXTexCoord2f32(2.0f, 0.0f);
			GXPosition3f32(f29, f30, 0.0f);
			GXTexCoord2f32(2.0f, 1.0f);
			GXPosition3f32(f31, f30, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
		}
	}
}
