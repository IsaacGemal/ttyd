// MWCC rematerializes the shared 0.0f constant without this pragma, which drops one saved FPR
// (3 instead of 4) and shifts the whole prologue; keeping propagation off pins f31.
#pragma opt_propagation off

#include "eff/n64/eff_rothammer_fail_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "math_ext.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

u32 size24x24_tex32x32_vtx[14] = {
    0xFFF4FFF4, 0x00000000, 0x00000000, 0x00FF000C,
    0xFFF40000, 0xFC000000, 0x000000FF, 0x000C000C,
    0x0000FC00, 0x04000000, 0x00FFFFF4, 0x000C0000,
    0x00000400, 0x000000FF,
};

// .sdata2/.rodata/.data objects defined by the original unit's split map.

extern GXColor dat_8041fbc8;
GXColor unk_80423250;

typedef struct effRotHammerFailParticle {
	u8 field_0x00[4];    // 0x00
	f32 x;               // 0x04
	f32 y;               // 0x08
	f32 z;               // 0x0C
	f32 field_0x10;      // 0x10
	f32 field_0x14;      // 0x14
	f32 field_0x18;      // 0x18
	u8 field_0x1C[0x18]; // 0x1C
} effRotHammerFailParticle; // 0x34

typedef struct effRotHammerFailData {
	u16 type;                          // 0x00
	u8 pad_0x02[2];                    // 0x02
	f32 x;                             // 0x04
	f32 y;                             // 0x08
	f32 z;                             // 0x0C
	u8 field_0x10[0xC];                // 0x10
	f32 field_0x1C;                    // 0x1C
	f32 field_0x20;                    // 0x20
	s32 field_0x24;                    // 0x24
	s32 field_0x28;                    // 0x28
	s32 field_0x2C;                    // 0x2C
	f32 field_0x30;                    // 0x30
	effRotHammerFailParticle particle; // 0x34
} effRotHammerFailData;

// The original walks the effect's two 0x34-byte records as an array; this is the record view the
// Main callback iterates (the combined effRotHammerFailData above is the Entry allocation view).
typedef struct effRotHammerFailRecord {
	u16 type;        // 0x00
	u8 pad_0x02[2];  // 0x02
	f32 x;           // 0x04
	f32 y;           // 0x08
	f32 z;           // 0x0C
	f32 field_0x10;  // 0x10
	f32 field_0x14;  // 0x14
	f32 field_0x18;  // 0x18
	f32 field_0x1C;  // 0x1C
	f32 field_0x20;  // 0x20
	s32 field_0x24;  // 0x24
	s32 field_0x28;  // 0x28
	s32 field_0x2C;  // 0x2C
	f32 field_0x30;  // 0x30
} effRotHammerFailRecord; // 0x34

// The two 0x34-byte records are the header (index 0) and the particle (index 1). The original
// walks to the particle with the same `data++` the Main callback uses; the loop shape is what
// makes MWCC keep the header base in r3 and materialize the particle base in r30
// (addi r30, r3, 0x34) for the sin/cos stores.
EffectEntry* effRotHammerFailN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 a5) {
	EffectEntry* effect;
	effRotHammerFailRecord* data;
	f32 zero;
	s32 j;

	effect = effEntry();
	effect->type = "RotHammerFailN64";
	effect->count = 2;
	data = __memAlloc(HEAP_EFFECT, sizeof(effRotHammerFailData));
	effect->userdata = data;
	effect->callback = effRotHammerFailMain;
	zero = 0.0f;
	data->field_0x24 = life;
	data->field_0x28 = 0;
	data->field_0x2C = life;
	data->field_0x30 = zero;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x1C = 10.0f;
	data->field_0x20 = 30.0f;
	for (j = 1, data++; j < 2; j++, data++) {
		data->z = data->y = data->x = zero;
		data->field_0x10 = sin(zero);
		data->field_0x14 = zero;
		data->field_0x18 = cos(zero);
	}
	return effect;
}

void effRotHammerFailMain(EffectEntry* effect) {
	effRotHammerFailRecord* data = effect->userdata;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail copies a shared zero Vec into a scratch slot, overwrites it with
	   data->x/y/z, then copies it into a second local before dispCalcZ (same idiom as
	   effGlassMain). */
	Vec position;
#endif
	Vec disp = { 0 };
	s32 i;
	f32 scale;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
#ifdef MUST_MATCH
	position = disp;
#endif

	data->field_0x30 = 7.0f * data->field_0x28 / data->field_0x2C;
	data->field_0x28 += 1;
	data->field_0x24 -= 1;
	if (data->field_0x24 < 0) {
		effDelete(effect);
		return;
	}

	data->field_0x1C += 0.1f * (data->field_0x20 - data->field_0x1C);
	scale = data->field_0x1C;
	for (i = 1, data++; i < effect->count; i++, data++) {
		data->field_0x14 += 0.04f;
		data->field_0x14 *= 0.9f;
		data->x = scale * data->field_0x10;
		data->y += data->field_0x14;
		data->z = scale * data->field_0x18;
	}

#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effRotHammerFailDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effRotHammerFailDisp, effect);
#endif
}

static inline void hammerSetColor(u8 a) {
	GXColor color = unk_80423250;
	color.a = a;
	GXSetTevColor(GX_TEVREG1, color);
}

// Without this, MWCC CSEs the two branches' identical `offset + 32` and the flag==0 copy
// reuses a dead register (addi r27, r27, 0x20) instead of the target's fresh r3.
#pragma opt_common_subs off
void effRotHammerFailDisp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	effRotHammerFailRecord* data;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx trans;
	Mtx scale;
	Mtx rot;
	Mtx work;
	s32 flag;
	s32 i;
	s32 whole;
	s32 offset;
	s32 frac;

	cam = camGetPtr(cameraId);
	data = effect->userdata;

	whole = (s32)data->field_0x30;
	frac = (s32)(256.0f * (data->field_0x30 - (f32)whole));
	flag = (whole == 6);

	GXSetNumChans(0);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, 0x7d);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, 0x7d);
	effGetTexObjN64(0xd, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXLoadTexObj(&texObj, GX_TEXMAP1);
	GXSetNumTevStages(3);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_APREV, GX_CA_TEXA, GX_CA_A1, GX_CA_ZERO);
	GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
	GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);
	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64((effVtxArray*)size24x24_tex32x32_vtx);

	PSMTXTrans(trans, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(trans, rot, trans);
	PSMTXConcat(cam->view, trans, work);

	GXSetTevColor(GX_TEVREG0, dat_8041fbc8);
	hammerSetColor(frac);

	offset = whole * 32;
	PSMTXScale(scale, 0.004464286f, 0.03125f, 0.0f);
	PSMTXTrans(trans, (f32)offset, 0.0f, 0.0f);
	PSMTXConcat(scale, trans, trans);
	GXLoadTexMtxImm(trans, GX_TEXMTX0, GX_MTX2x4);

	if (flag != 0) {
		PSMTXScale(scale, 0.004464286f, 0.03125f, 0.0f);
		PSMTXTrans(trans, (f32)(offset + 32), 32.0f, 0.0f);
		PSMTXConcat(scale, trans, trans);
		GXLoadTexMtxImm(trans, GX_TEXMTX1, GX_MTX2x4);
	} else {
		PSMTXScale(scale, 0.004464286f, 0.03125f, 0.0f);
		PSMTXTrans(trans, (f32)(offset + 32), 0.0f, 0.0f);
		PSMTXConcat(scale, trans, trans);
		GXLoadTexMtxImm(trans, GX_TEXMTX1, GX_MTX2x4);
	}

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		PSMTXTrans(trans, data->x, data->y, data->z);
		PSMTXConcat(work, trans, trans);
		GXLoadPosMtxImm(trans, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
	}
}
#pragma opt_common_subs reset

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
