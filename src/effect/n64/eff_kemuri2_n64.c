#include "eff/n64/eff_kemuri2_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .rodata
const Vec vec3_802fa8b0 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_802fa8c0 = 4503601774854144.0;
const char str_802FA8C8[12] = "Kemuri2N64";

// .sdata animation frame tables and the pointer table that selects between them.
u32 anim_data_0[4] = { 0x00010202, 0x03030404, 0x05050506, 0x0606FF00 };
u32 anim_data_1[5] = { 0x00010202, 0x03030404, 0x05050606, 0x06070707, 0xFF000000 };
static void* anim_data_table[2] = { anim_data_0, anim_data_1 };

// .sdata2 constants, named after their original addresses.
extern const GXColor dat_8041f2b0;
extern const GXColor dat_8041f2b4;
extern const f32 float_176_8041f2b8;
extern const f32 float_24_8041f2bc;
extern const f32 float_224_8041f2c0;
extern const f32 float_32_8041f2c4;
extern const f32 float_256_8041f2c8;
extern const f32 float_1_8041f2cc;
extern const f32 float_0_8041f2d0;
extern const f32 float_deg2rad_8041f2d4;
extern const f32 float_0p33333_8041f2d8;
extern const f32 float_1p75_8041f2dc;
extern const f32 float_neg0p16_8041f2e0;

// Local to this file in the original (`.fn effKemuri2Main, local`).
static void effKemuri2Main(EffectEntry* effect);
static void main_dl(s32 type);
static void effKemuri2Disp(CameraId cameraId, void* param);

typedef struct effKemuri2Data {
	u32 field_0x0;             // 0x0
	u16 type;                  // 0x4
	u8 pad_6[2];               // 0x6
	Vec position;              // 0x8
	u8 field_0x14[0x44 - 0x14]; // 0x14
	f32 field_0x44;            // 0x44
	f32 field_0x48;            // 0x48
	f32 field_0x4c;            // 0x4c
	f32 field_0x50;            // 0x50
	f32 field_0x54;            // 0x54
	s32 field_0x58;            // 0x58
	s32 field_0x5c;            // 0x5c
	s32 field_0x60;            // 0x60
} effKemuri2Data;

EffectEntry* effKemuri2N64Entry(s32 type, f32 x, f32 y, f32 z, f32 a5, f32 a6) {
	EffectEntry* entry;
	effKemuri2Data* data;

	entry = effEntry();
	entry->type = str_802FA8C8;
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effKemuri2Data));
	entry->userdata = data;
	entry->callback = effKemuri2Main;

	data->field_0x58 = (type == 2);
	data->field_0x5c = 0;
	data->type = type;
	data->field_0x0 = 1;
	data->field_0x50 = a5;
	data->field_0x54 = a6;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->field_0x44 = float_0p33333_8041f2d8;
	data->field_0x48 = float_1p75_8041f2dc;
	data->field_0x4c = float_neg0p16_8041f2e0;
	return entry;
}

static void effKemuri2Main(EffectEntry* effect) {
	effKemuri2Data* data;
	Vec position2;
	Vec position;
	s32 i;

	data = effect->userdata;
	position = (Vec){ 0.0f, 0.0f, 0.0f };
	position.x = data->position.x;
	position.y = data->position.y;
	position.z = data->position.z;
	position2 = position;
	if ((data->field_0x60 = ((s8*)anim_data_table[data->field_0x58])[data->field_0x5c++]) < 0) {
		effDelete(effect);
		return;
	}

	for (i = 0; i < effect->count; i++, data++) {
		data->position.x += data->field_0x44 * data->field_0x50;
		data->position.z += data->field_0x44 * data->field_0x54;
		data->field_0x48 += data->field_0x4c;
		data->position.y += data->field_0x48;
	}

	dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effKemuri2Disp, effect);
}

static void effKemuri2Disp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	effKemuri2Data* data;
	CameraEntry* camera;
	Mtx transMtx;
	Mtx rotMtx;
	Mtx scaleMtx;
	Mtx outMtx;
	f32 texWidth;
	f32 uScale;
	f32 frameSizeF;
	f32 vScale;
	f32 yTrans;
	s32 type;
	s32 flag;
	s32 i;
	s32 frame;
	s32 frameSize;
	s32 offset;

	camera = camGetPtr(cameraId);
	data = effect->userdata;
	flag = 0;
	frame = data->field_0x60;
	type = data->type;
	if ((data->field_0x58 == 0 && frame == 6) ||
	    (data->field_0x58 == 1 && frame == 7)) {
		flag = 1;
	}
	GXSetTevColor(GX_TEVREG0, dat_8041f2b0);
	GXSetTevColor(GX_TEVREG1, dat_8041f2b4);
	switch (type) {
	case 0:
		texWidth = float_176_8041f2b8;
		frameSizeF = float_24_8041f2bc;
		break;
	case 1:
		texWidth = float_224_8041f2c0;
		frameSizeF = float_32_8041f2c4;
		break;
	/* MUST_MATCH: the empty case 2 forces MWCC's switch decision tree to emit the
	   duplicated branch to the default body that retail has. */
	case 2:
	default:
		texWidth = float_256_8041f2c8;
		frameSizeF = float_32_8041f2c4;
		break;
	}
	frameSize = (type == 0) ? 24 : 32;
	offset = frame * frameSize;
	uScale = (f32)frameSize / texWidth;
	vScale = (f32)frameSize / frameSizeF;
	PSMTXScale(scaleMtx, uScale, vScale, float_1_8041f2cc);
	yTrans = float_0_8041f2d0 / frameSizeF;
	PSMTXTrans(transMtx, (f32)offset / texWidth, yTrans, float_0_8041f2d0);
	PSMTXConcat(transMtx, scaleMtx, transMtx);
	GXLoadTexMtxImm(transMtx, GX_TEXMTX0, GX_MTX2x4);
	if (flag != 0) {
		PSMTXScale(scaleMtx, uScale, float_0_8041f2d0, float_1_8041f2cc);
		PSMTXTrans(transMtx, (f32)(offset + frameSize) / texWidth, yTrans, float_0_8041f2d0);
		PSMTXConcat(transMtx, scaleMtx, transMtx);
		GXLoadTexMtxImm(transMtx, GX_TEXMTX1, GX_MTX2x4);
	} else {
		PSMTXScale(scaleMtx, uScale, vScale, float_1_8041f2cc);
		PSMTXTrans(transMtx, (f32)(offset + frameSize) / texWidth, yTrans, float_0_8041f2d0);
		PSMTXConcat(transMtx, scaleMtx, transMtx);
		GXLoadTexMtxImm(transMtx, GX_TEXMTX1, GX_MTX2x4);
	}
	for (i = 0; i < effect->count; i++, data++) {
		PSMTXTrans(transMtx, data->position.x, data->position.y, data->position.z);
		PSMTXRotRad(rotMtx, 'y', float_deg2rad_8041f2d4 * -camGetPtr(cameraId)->viewYaw);
		PSMTXConcat(transMtx, rotMtx, outMtx);
		PSMTXConcat(camera->view, outMtx, outMtx);
		GXLoadPosMtxImm(outMtx, GX_PNMTX0);
		main_dl(type);
	}
}
