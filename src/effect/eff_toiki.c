#include "eff/eff_toiki.h"
#include "driver/dispdrv.h"
#include "eff/eff_mist.h"
#include "memory.h"
#include "math_ext.h"

// .sbss2: the default .sbss2 is only used for compiler-generated pool templates, so pair it with a small-data section.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_80423338;


// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u32 lbl_803B2FA0[2] = { 0xBF800000, 0x3F800000 };

const Vec vec3_80300230 = { 0.0f, 0.0f, 0.0f };


extern u32 queen_toiki_normal_tbl[8];
extern u32 queen_toiki_color0_tbl[8];
extern u8 queen_toiki_dl_0_size_tbl[16];

static GXColor color_tbl[3] = {
	{ 0x03, 0x02, 0x21, 0xFF },
	{ 0xD8, 0x0F, 0x51, 0xFF },
	{ 0x0B, 0x1B, 0x00, 0xFF },
};

EffectEntry* effToikiEntry(s32 type, s32 life, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	ToikiData* data;
	f32 angle;

	entry = effEntry();
	entry->type = "Toiki";
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x34);
	entry->userdata = data;
	entry->callback = effToikiMain;
	entry->flags |= 2;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = 0.5f;
	data->rotation = 0.0f;
	data->life = life;
	data->field_0x2C = 0;
	data->field_0x28 = 255.0f;
	angle = 6.2831855f * (200.0f + data->rotation) / 360.0f;
	data->velX = 2.5f * (f32)cos(angle);
	angle = 6.2831855f * (200.0f + data->rotation) / 360.0f;
	data->velY = 2.5f * (f32)sin(angle);
	data->velZ = 0.0f;
	data->field_0x24 = 0.0f;
	return entry;
}

void effToikiMain(EffectEntry* effect) {
	ToikiData* data = effect->userdata;
	Vec pos;
	Vec mistPos;
	Vec sp;
	Mtx mtx;
	s32 type;
	f32 angle;
	f32 size;

	sp = vec3_80300230;
	sp.x = data->x;
	sp.y = data->y;
	sp.z = data->z;
	pos = sp;
	type = data->type;
	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->life = 0x10;
	}
	if (data->life < 1000) {
		data->life--;
	}
	if (data->life < 0) {
		effDelete(effect);
		return;
	}
	data->x += data->velX;
	data->y += data->velY;
	data->z += data->velZ;
	data->velX *= 0.95f;
	data->velY *= 0.95f;
	data->velZ *= 0.95f;
	if (data->life < 0x10) {
		data->field_0x28 = (f32)(data->life * 16);
	}
	if (data->field_0x24 > 1.0f && data->field_0x24 < 4.0f && data->life % 20 == 0) {
		size = 320.0f * data->scale;
		angle = 6.2831855f * (200.0f + data->rotation) / 360.0f;
		mistPos.x = data->x + size * (f32)cos(angle);
		mistPos.y = data->y + size * (f32)sin(angle);
		mistPos.z = data->z + 1.0f;
		PSMTXRotRad(mtx, 'y', -camGetPtr(CAMERA_3D)->viewYaw * 0.017453292f);
		PSMTXMultVec(mtx, &mistPos, &mistPos);
		effMistEntry(type + 1, 0x3c, mistPos.x, mistPos.y, mistPos.z, 0.0f);
	}
	data->field_0x2C++;
	data->field_0x24 += 0.015f;
	if (data->field_0x24 > 4.0f) {
		data->field_0x24 = 4.0f;
	}
	dispEntry(CAMERA_3D, dispCalcZ(&pos), 2, effToikiDisp, effect);
}

#ifdef MUST_MATCH
// MUST_MATCH: building the colour inside an inline that also issues GXSetTevColor makes MWCC allocate the
// temp and the by-value argument copy together in retail's stack slots (0x8/0xc). The parameter order is
// deliberate: it also lines up the field evaluation so r/g/b/a land in the original's registers.
static inline void toikiSetColor(u8 a, u8 b, u8 g, u8 r) {
	GXColor c = *(GXColor*)&unk_80423338;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	GXSetTevColor(1, c);
}
#endif

#ifdef MUST_MATCH
// MUST_MATCH: with the loop inlined at its call site MWCC colours dl/i/sz r28/r29/r30; splitting it into
// this inline function gives the original's r29/r30/r31. The size table and the increment order are also
// part of the colouring: `sz` must be initialised in the for-init and incremented before `dl`.
static inline void toikiDrawDL(u8* base) {
	u8** dl = (u8**)(base + 0x360);
	u8* sz;
	u32 j;

	for (j = 0, sz = queen_toiki_dl_0_size_tbl; j < 0xe; j++, sz++, dl++) {
		GXCallDisplayList(*dl, *sz << 5);
	}
}
#endif

void effToikiDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	ToikiData* data = effect->userdata;
	CameraEntry* cam;
	u8* tbl;
	Mtx mtx;
	Mtx rot;
	Mtx scale;
	GXTexObj texObj;
#ifndef MUST_MATCH
	GXColor color;
#endif
	s32 type;

#ifdef MUST_MATCH
	// MUST_MATCH: the (u32) round-trip stops propagation from rematerialising lbl_803B2FA0 into each use;
	// with the plain assignment MWCC computes the address once per use instead of keeping it in r30.
	tbl = (u8*)(u32)lbl_803B2FA0;
#else
	tbl = (u8*)lbl_803B2FA0;
#endif
	cam = camGetPtr(camId);
	type = data->type;

	effGetTexObj(0x5c, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
#ifdef MUST_MATCH
	toikiSetColor(data->field_0x28, color_tbl[type].b, color_tbl[type].g, color_tbl[type].r);
#else
	color = *(GXColor*)&unk_80423338;
	color.r = color_tbl[type].r;
	color.g = color_tbl[type].g;
	color.b = color_tbl[type].b;
	color.a = data->field_0x28;
	GXSetTevColor(1, color);
#endif
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	PSMTXTrans(mtx, data->field_0x24, 0.0f, 0.0f);
	GXLoadTexMtxImm(mtx, 0x1e, 1);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 4);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xf, 2, 0xa, 0xf);
	GXSetTevAlphaIn(0, 7, 4, 5, 7);
	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', -camGetPtr(camId)->viewYaw * 0.017453292f);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXRotRad(rot, 'z', 0.017453292f * data->rotation);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXScale(scale, data->scale, data->scale, data->scale);
	PSMTXConcat(mtx, scale, mtx);
	PSMTXConcat(cam->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, 0);
	GXSetCurrentMtx(0);
	GXSetCullMode(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 2);
	GXSetVtxAttrFmt(0, 9, 1, 3, 6);
	GXSetArray(9, tbl + 0x40, 6);
	GXSetVtxDesc(10, 2);
	GXSetVtxAttrFmt(0, 10, 0, 1, 6);
	GXSetArray(10, queen_toiki_normal_tbl, 3);
	GXSetVtxDesc(11, 2);
	GXSetVtxAttrFmt(0, 11, 1, 5, 0);
	GXSetArray(11, queen_toiki_color0_tbl, 4);
	GXSetVtxDesc(13, 2);
	GXSetVtxAttrFmt(0, 13, 1, 3, 0xe);
	GXSetArray(13, tbl + 0x1c0, 4);
#ifdef MUST_MATCH
	toikiDrawDL(tbl);
#else
	{
		u8** dl = (u8**)(tbl + 0x360);
		u8* sz = queen_toiki_dl_0_size_tbl;
		u32 i;

		for (i = 0; i < 0xe; i++, dl++, sz++) {
			GXCallDisplayList(*dl, *sz << 5);
		}
	}
#endif
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
