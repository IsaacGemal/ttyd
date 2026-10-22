#include "eff/n64/eff_hagetaka_flush_n64.h"
#include "eff/eff_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "driver/camdrv.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sdata2 constants (defined in the original unit's data section).

// .sdata byte-indexed color rotation table (24 bytes).
static u8 color_rotation_data[24] = {
	0xFF, 0xFF, 0x52, 0xFF, 0xFF, 0x52, 0xB5, 0x94,
	0x00, 0xB5, 0x94, 0x00, 0xFF, 0xFF, 0xD6, 0xFF,
	0xFF, 0xD6, 0xB5, 0x94, 0x00, 0xB5, 0x94, 0x00,
};

// .data: one effVtxArray (0x38 bytes) for the textured quad.
static u32 size16x16_tex64x64_vtx[0x38 / 4] = {
	0xFFF8FFF8, 0x00000000, 0x04000000, 0x00FF0008,
	0xFFF80000, 0x08000400, 0x000000FF, 0x00080008,
	0x00000800, 0x0C000000, 0x00FFFFF8, 0x00080000,
	0x00000C00, 0x000000FF,
};

EffectEntry* effHagetakaFlushN64Entry(s32 type, s32 unused, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effHagetakaFlushData* data;

	effect = effEntry();
	effect->type = "HagetakaFlushN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effHagetakaFlushData));
	effect->userdata = data;
	effect->callback = effHagetakaFlushMain;
	effect->flags |= 2;
	data->type = type;
	data->field_0x14 = 0;
	data->field_0x10 = 10;
	data->field_0x24 = 0xff;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x28 = scale;
	data->field_0x18 = 70;
	data->field_0x1C = 180;
	data->field_0x20 = 120;
	data->field_0x2C = 0.0f;
	return effect;
}

void effHagetakaFlushMain(EffectEntry* effect) {
	effHagetakaFlushData* data = effect->userdata;
	Vec position;
	Vec disp = { 0 };
	s32 frame;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
	position = disp;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->field_0x10 = 10;
	}

	if (data->field_0x10 < 1000) {
		data->field_0x10--;
	}
	data->field_0x14++;

	if (data->field_0x10 < 0) {
		effDelete(effect);
		return;
	}

	frame = data->field_0x14;
	if (data->field_0x10 < 4) {
		data->field_0x2C *= 0.5f;
	}
	else {
		data->field_0x2C = frame * 0.2f + 0.3f;
	}

	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effHagetakaFlushDisp, effect);
}

// The color index is computed here with `%=` rather than `% 24` at the call site:
// the in-place form makes the remainder reuse the dividend register (r7), matching
// the original's subf/add/lbzx.
void effHagetakaFlushDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effHagetakaFlushData* data;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx mtxTrans;
	Mtx mtxScale;
	Mtx mtxRot;
	f32 scale;
	s32 alpha;
	s32 colorIndex;

	cam = camGetPtr(camId);
	data = effect->userdata;

	alpha = data->field_0x24;
	scale = data->field_0x28 * data->field_0x2C;
	PSMTXTrans(mtxTrans, data->x, data->y, data->z);
	PSMTXRotRad(mtxRot, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXScale(mtxScale, scale, scale, scale);
	PSMTXConcat(mtxTrans, mtxRot, mtxTrans);
	PSMTXConcat(mtxTrans, mtxScale, mtxTrans);
	PSMTXConcat(cam->view, mtxTrans, mtxTrans);
	GXLoadPosMtxImm(mtxTrans, 0);
	GXSetCurrentMtx(0);
	GXSetNumChans(0);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 0xff);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xf, 0xf, 0xf, 2);
	GXSetTevAlphaIn(0, 7, 1, 4, 7);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	PSMTXScale(mtxScale, 0.03125f, 0.03125f, 0.0f);
	GXLoadTexMtxImm(mtxScale, 0x1e, 1);
	effGetTexObjN64(0x1c, &texObj);
	GXLoadTexObj(&texObj, 0);

	colorIndex = data->field_0x14 * 3;
	// `%=` (as in eff_wangflush_n64.c) lets the remainder reuse the dividend register r7.
	colorIndex %= 24;
	GXSetTevColor(1, (GXColor){ color_rotation_data[colorIndex], color_rotation_data[colorIndex + 1],
	                            color_rotation_data[colorIndex + 2], alpha });

	GXSetCullMode(0);
	effSetVtxDescN64((effVtxArray*)size16x16_tex64x64_vtx);
	GXBegin(0x90, 0, 6);
	tri2(0, 1, 2, 0, 0, 2, 3, 0);
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
