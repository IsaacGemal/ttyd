#include "eff/eff_teresa.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_kemutest.h"
#include "memory.h"
#include "pmario_sound.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sbss2: the default .sbss2 is only used for compiler-generated pool templates, so pair it with a small-data section.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_8042332c;
__declspec(section ".sdata") GXColor unk_80423328;

const GXColor dat_80421548 = { 0xFF, 0xFF, 0xFF, 0 };
const f32 float_deg2rad_8042154c = 0.01745329252f;
const f32 float_32_80421550 = 32.0f;
const f32 float_0p5_80421554 = 0.5f;

EffectEntry* effTeresaEntry(s32 type, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	EffTeresaData* data;
	Vec pos;

	entry = effEntry();
	entry->type = "Teresa";
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(EffTeresaData));
	entry->userdata = data;
	entry->callback = effTeresaMain;

	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = (type == 0) ? 30.f : 0.0f;
	data->field_0x30 = 0;
	data->field_0x2C = 0;
	data->alpha = (f32)((type == 0) ? 0 : 255);
	data->state = 0;

	if (type == 0) {
		pos = (Vec){0.0f, 0.0f, 0.0f};
		pos.x = data->x;
		pos.y = data->y;
		pos.z = data->z;
		psndSFXOn_3D((const char*)0x5D5, &pos);
	}
	return entry;
}

void effTeresaMain(EffectEntry* effect) {
	EffTeresaData* data;
	Vec position;
	Vec disp = { 0 };

	data = effect->userdata;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
	position = disp;

	if (data->type == 0) {
		// state 0: shrink while spinning; once tiny, pop with a smoke puff and expand back out while fading
		if (data->state == 0) {
			if (data->scale > 0.1f) {
				data->scale *= 0.8f;
			} else {
				Vec pos;

				data->state = 1;
				effKemuTestEntry(8, data->x, data->y, data->z, 1.0f);
				pos = (Vec){0.0f, 0.0f, 0.0f};
				pos.x = data->x;
				pos.y = data->y;
				pos.z = data->z;
				psndSFXOn_3D((const char*)0x5D6, &pos);
			}
			data->angle += 10.f;
			data->alpha = 178.5f;
		} else {
			data->angle = 0.0f;
			data->scale += (20.f - data->scale) / 30.f;
			data->alpha *= 0.8f;
			if (data->alpha < 3.0f) {
				effDelete(effect);
				return;
			}
		}
	} else {
		data->scale += (20.f - data->scale) / 30.f;
		data->alpha *= 0.98f;
		if (data->alpha < 3.0f) {
			effDelete(effect);
			return;
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effTeresaDisp, effect);
}

// The original loads these .sdata2 constants at runtime; a plain read lets MWCC
// fold them into immediates. The volatile read is a MUST_MATCH device only.
#ifdef MUST_MATCH
#define READ_RODATA(x) (*(volatile const f32*)&x)
#else
#define READ_RODATA(x) (x)
#endif

#ifdef MUST_MATCH
// The original builds the alpha-patched colour inside a helper, which places its
// local below the by-value copy (effThunderDisp/effIrekaeru do the same).
static inline void teresaColor0(u8 alpha) {
	GXColor color = dat_80421548;

	color.a = alpha;
	GXSetChanMatColor(4, color);
}

static inline void teresaColor1(u8 alpha) {
	GXColor color = unk_80423328;

	color.a = alpha;
	GXSetChanMatColor(4, color);
}
#endif

void effTeresaDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	EffTeresaData* data = effect->userdata;
	CameraEntry* cam;
	GXTexObj texObj;
	GXColor color;
	Mtx mtx98;
	Mtx mtx68;
	Mtx mtx38;
	s32 type;
	f32 negSize;
	f32 posSize;

	cam = camGetPtr(camId);
	type = data->type;
	effGetTexObj(0x45, &texObj);
	GXLoadTexObj(&texObj, 0);
	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
	if (type == 0) {
#ifdef MUST_MATCH
		teresaColor0(data->alpha);
#else
		color = dat_80421548;
		color.a = data->alpha;
		GXSetChanMatColor(4, color);
#endif
	} else {
#ifdef MUST_MATCH
		teresaColor1(data->alpha);
#else
		color = unk_80423328;
		color.a = data->alpha;
		GXSetChanMatColor(4, color);
#endif
	}
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 4);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0xA);
	GXSetTevAlphaIn(0, 7, 5, 4, 7);
	PSMTXTrans(mtx98, data->x, data->y, data->z);
	PSMTXRotRad(mtx68, 'y', READ_RODATA(float_deg2rad_8042154c) * -camGetPtr(camId)->viewYaw);
	PSMTXConcat(mtx98, mtx68, mtx98);
	PSMTXScale(mtx38, data->scale, data->scale, data->scale);
	PSMTXConcat(mtx98, mtx38, mtx98);
	PSMTXRotRad(mtx68, 'z', READ_RODATA(float_deg2rad_8042154c) * data->angle);
	PSMTXConcat(mtx98, mtx68, mtx98);
	PSMTXConcat(cam->view, mtx98, mtx98);
	GXLoadPosMtxImm(mtx98, 0);
	GXSetCurrentMtx(0);
	GXSetCullMode(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xD, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
	GXBegin(0x80, 0, 4);
	negSize = -READ_RODATA(float_32_80421550) * READ_RODATA(float_0p5_80421554);
	posSize = 16.0f;
	GXPosition3f32(negSize, posSize, 0.0f);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3f32(posSize, posSize, 0.0f);
	GXTexCoord2f32(2.0f, 0.0f);
	GXPosition3f32(posSize, negSize, 0.0f);
	GXTexCoord2f32(2.0f, 2.0f);
	GXPosition3f32(negSize, negSize, 0.0f);
	GXTexCoord2f32(0.0f, 2.0f);
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
