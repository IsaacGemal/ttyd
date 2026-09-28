#include "eff/n64/eff_snow_dust_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "system.h"
#include <dolphin/mtx.h>
#include <dolphin/gx.h>
#include <math.h>
#include <stdlib.h>

typedef struct effSnowDustData {
	s32 type;       // 0x00
	f32 x;          // 0x04
	f32 y;          // 0x08
	f32 z;          // 0x0C
	f32 field_0x10; // 0x10
	f32 field_0x14; // 0x14
	f32 field_0x18; // 0x18
	f32 field_0x1C; // 0x1C
	f32 field_0x20; // 0x20
	f32 field_0x24; // 0x24
	f32 field_0x28; // 0x28
	f32 field_0x2C; // 0x2C
	s32 field_0x30; // 0x30
	s32 field_0x34; // 0x34
	s32 field_0x38; // 0x38
	s32 field_0x3C; // 0x3C
	s32 field_0x40; // 0x40
	s32 field_0x44; // 0x44
	s32 field_0x48; // 0x48
	s32 field_0x4C; // 0x4C
	s32 field_0x50; // 0x50
	s32 field_0x54; // 0x54
	s32 field_0x58; // 0x58
	f32 field_0x5C; // 0x5C
	f32 field_0x60; // 0x60
	f32 field_0x64; // 0x64
	s32 field_0x68; // 0x68
} effSnowDustData;

// NONMATCHING (98.82%): logic exact. The tail loop is `i < count + 1` to reproduce the original
// `addi`/`cmpwi`/`subi` trip count; only the MWCC switch decision tree (original pivots type>=4,
// ours ==1) and register/symbol operands differ.
EffectEntry* effSnowDustN64Entry(s32 type, f32 x, f32 y, f32 z, s32 count, s32 life, f32 a6, f32 a7, f32 scale) {
	EffectEntry* effect;
	effSnowDustData* data;
	effSnowDustData* ptr;
	f32 ratio;
	s32 i;

	effect = effEntry();
	effect->type = "SnowDustN64";
	effect->count = count + 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effSnowDustData) * (count + 1));
	effect->userdata = data;
	effect->callback = effSnowDustMain;
	effect->flags |= 2;
	data->type = type;
	data->field_0x38 = 0;
	if (life <= 0) {
		data->field_0x34 = 1000;
	}
	else {
		data->field_0x34 = life;
	}
	data->field_0x48 = 0xff;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x5C = scale;
	data->field_0x10 = a6;
	data->field_0x14 = a7;
	data->field_0x68 = 4;
	data->field_0x3C = 0xff;
	data->field_0x40 = 0xff;
	data->field_0x44 = 0xff;
	switch (type) {
		case 0:
			data->field_0x4C = 0xff;
			data->field_0x50 = 0xff;
			data->field_0x54 = 0xff;
			data->field_0x58 = 0xff;
			break;
		case 1:
			data->field_0x4C = 0;
			data->field_0x50 = 0x7f;
			data->field_0x54 = 0xff;
			data->field_0x58 = 0xff;
			break;
		case 2:
		case 3:
		case 4:
		case 5:
			data->field_0x4C = 0xff;
			data->field_0x50 = 0xff;
			data->field_0x54 = 0x80;
			data->field_0x58 = 0xff;
			break;
		case 6:
			data->field_0x3C = 0xcb;
			data->field_0x40 = 0x94;
			data->field_0x44 = 0xcd;
			data->field_0x4C = 0xcb;
			data->field_0x50 = 0x94;
			data->field_0x54 = 0xcd;
			break;
		default:
			data->field_0x3C = 128 + rand() % 127;
			data->field_0x40 = 128 + rand() % (0x7F - (data->field_0x3C - 0x80));
			data->field_0x44 = 128 + rand() % (0x7F - (data->field_0x3C - 0x80) - (data->field_0x40 - 0x80));
			data->field_0x4C = 0x7f;
			data->field_0x50 = 0x7f;
			data->field_0x54 = 0x7f;
			data->field_0x58 = 0x7f;
			break;
	}
	if (data->field_0x34 < 0x1e) {
		ratio = (f32)data->field_0x34 / (f32)count;
	}
	else {
		ratio = 30.0f / (f32)count;
	}
	ptr = &data[1];
	for (i = 1; i < count + 1; i++, ptr++) {
		ptr->field_0x30 = (s32)(-(f32)i * ratio) - 1;
	}
	return effect;
}

//.sdata
static u8 scale_data[32] = {
	0x02, 0x04, 0x08, 0x0D, 0x0F, 0x0D, 0x09, 0x06,
	0x04, 0x03, 0x02, 0x04, 0x08, 0x0D, 0x0F, 0x0D,
	0x09, 0x06, 0x04, 0x03, 0x02, 0x04, 0x08, 0x0D,
	0x0F, 0x0D, 0x09, 0x06, 0x04, 0x03, 0x00, 0x00
};

static u8 scale_data2[32] = {
	0x28, 0x5A, 0x51, 0x48, 0x41, 0x3B, 0x35, 0x2F,
	0x2B, 0x26, 0x22, 0x1F, 0x1C, 0x19, 0x16, 0x14,
	0x12, 0x10, 0x0F, 0x0D, 0x0C, 0x0A, 0x09, 0x08,
	0x07, 0x07, 0x06, 0x05, 0x05, 0x04, 0x00, 0x00
};

static u8 a_data[32] = {
	0xC8, 0xFA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFA, 0xC8
};

extern const Vec vec3_802fb968;

// NONMATCHING (97.83%): only register-number operands and symbol names differ; the two
// 'replace' diffs are the i=1 / ptr=data+1 init order. FP callee-saved register allocation
// (f26..f31) is permuted relative to the original.
void effSnowDustMain(EffectEntry* effect) {
	effSnowDustData* data;
	effSnowDustData* ptr;
	Vec position;
	Vec position2;
	s32 type;
	s32 camera;
	s32 i;
	s32 life;
	s32 r23, r24, r25, r30, r31;
	f32 f25;
	f32 f26, f27, f28, f29, f30, f31;

	data = effect->userdata;
	position = vec3_802fb968;
	position.x = data->x;
	position.y = data->y;
	position.z = data->z;
	position2 = position;
	camera = data->field_0x68;
	type = data->type;
	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->field_0x34 = 0x10;
	}
	if (data->field_0x34 < 1000) {
		data->field_0x34--;
	}
	data->field_0x38++;
	life = data->field_0x34;
	if (life < 0) {
		effDelete(effect);
		return;
	}
	if (life < 8) {
		data->field_0x48 = life << 5;
	}
	f29 = data->field_0x10;
	f25 = data->field_0x14;
	f28 = data->x;
	f27 = data->y;
	f26 = data->z;
	f31 = (f32)cos(6.2831855f * camGetPtr(CAMERA_3D)->viewYaw / 360.0f);
	f30 = (f32)sin(6.2831855f * camGetPtr(CAMERA_3D)->viewYaw / 360.0f);
	r25 = data->field_0x3C;
	r24 = data->field_0x40;
	r23 = data->field_0x44;
	r31 = (s32)f29;
	r30 = (s32)f25;
	f29 = 0.5f * f29;
	i = 1;
	ptr = data + 1;
	for (; i < effect->count; i++, ptr++) {
		ptr->field_0x30++;
		if (ptr->field_0x30 >= 0x1e) {
			if (life < 0x1e) {
				ptr->field_0x30 = 0x14;
			}
			else {
				ptr->field_0x30 = 0;
			}
		}
		if (ptr->field_0x30 >= 0) {
			if (ptr->field_0x30 == 0) {
				f25 = (f32)(rand() % r31) - f29;
				ptr->x = f25 * f31 + f28 - f30;
				ptr->y = f27 + (f32)(rand() % r30);
				ptr->z = f31 + (f25 * f30 + f26);
				ptr->field_0x3C = r25;
				ptr->field_0x40 = r24;
				ptr->field_0x44 = r23;
			}
			switch (type) {
				case 0:
				case 1:
					if (ptr->field_0x30 == 0) {
						ptr->field_0x18 = 0.0f;
						ptr->field_0x1C = 0.0f;
						ptr->field_0x24 = 0.0f;
						ptr->field_0x28 = 0.05f * (f32)(-(rand() % 10) - 2);
						ptr->field_0x60 = 0.0f;
						ptr->field_0x64 = 0.0f;
						ptr->field_0x5C = 0.05f * (f32)(rand() % 10) + 0.5f;
					}
					ptr->field_0x48 = a_data[ptr->field_0x30];
					ptr->x += ptr->field_0x18;
					ptr->y += ptr->field_0x1C;
					ptr->field_0x60 += 0.2f;
					ptr->field_0x64 += 0.6f;
					ptr->field_0x18 = 0.04f * (ptr->field_0x24 - ptr->field_0x18) + ptr->field_0x18;
					ptr->field_0x1C = 0.04f * (ptr->field_0x28 - ptr->field_0x1C) + ptr->field_0x1C;
					break;
				case 2:
				case 3:
					if (ptr->field_0x30 == 0) {
						if (type == 2) {
							ptr->field_0x18 = 0.05f * (f32)(rand() % 20 - 10);
							ptr->field_0x1C = 0.05f * (f32)(rand() % 20 - 10);
						}
						else {
							ptr->field_0x18 = 0.0f;
							ptr->field_0x1C = 0.0f;
						}
						ptr->field_0x24 = 0.0f;
						ptr->field_0x28 = 0.0f;
						ptr->field_0x60 = (f32)(rand() % 15);
						ptr->field_0x64 = (f32)(rand() % 15);
						ptr->field_0x48 = 0xff;
					}
					ptr->field_0x5C = 0.1f * (f32)scale_data[ptr->field_0x30];
					ptr->x += ptr->field_0x18;
					ptr->y += ptr->field_0x1C;
					ptr->field_0x60 += 0.2f;
					ptr->field_0x64 += 0.6f;
					ptr->field_0x18 = 0.04f * (ptr->field_0x24 - ptr->field_0x18) + ptr->field_0x18;
					ptr->field_0x1C = 0.04f * (ptr->field_0x28 - ptr->field_0x1C) + ptr->field_0x1C;
					break;
				case 6:
					if (ptr->field_0x30 == 0) {
						ptr->field_0x18 = 0.0f;
						ptr->field_0x1C = 0.0f;
						ptr->field_0x24 = 0.0f;
						ptr->field_0x28 = 0.05f * (f32)(-(rand() % 10) - 2);
						ptr->field_0x60 = 0.0f;
						ptr->field_0x64 = 0.0f;
						ptr->field_0x5C = 0.05f * (f32)(rand() % 10) + 0.5f;
						ptr->field_0x38 = irand(0x168);
					}
					ptr->field_0x48 = a_data[ptr->field_0x30];
					ptr->field_0x48 = 0xff;
					ptr->y += ptr->field_0x1C;
					ptr->field_0x60 += 0.2f;
					ptr->field_0x64 += 0.6f;
					ptr->field_0x1C = 0.04f * (ptr->field_0x28 - ptr->field_0x1C) + ptr->field_0x1C;
					f25 = distABf(position2.x, position2.z, ptr->x, ptr->z) * 0.97f;
					ptr->x = f25 * (f32)sin(6.2831855f * (f32)ptr->field_0x38 / 360.0f) + position2.x;
					ptr->z = f25 * (f32)cos(6.2831855f * (f32)ptr->field_0x38 / 360.0f) + position2.z;
					ptr->field_0x38 += 4;
					ptr->field_0x38 %= 360;
					break;
				default:
					if (ptr->field_0x30 == 0) {
						ptr->field_0x60 = 0.0f;
						ptr->field_0x64 = 0.0f;
						ptr->field_0x48 = 0xff;
						ptr->field_0x5C = 1.0f;
						ptr->field_0x3C = 128 + rand() % 127;
						ptr->field_0x40 = 128 + rand() % (0x7F - (ptr->field_0x3C - 0x80));
						ptr->field_0x44 = 128 + rand() % (0x7F - (ptr->field_0x3C - 0x80) - (ptr->field_0x40 - 0x80));
					}
					if (type == 4) {
						ptr->field_0x5C = 0.01f * (f32)scale_data2[ptr->field_0x30];
					}
					else {
						ptr->field_0x5C = 0.01f * (f32)scale_data[ptr->field_0x30];
					}
					break;
			}
		}
	}
	dispEntry(camera, dispCalcZ(&position2), 2, effSnowDustDisp, effect);
}

//.data
u32 size8x8_tex64x64_vtx[14] = {
	0xFFFCFFFC, 0x00000000, 0x00000000, 0x00FF0004,
	0xFFFC0000, 0x08000000, 0x000000FF, 0x00040004,
	0x00000800, 0x08000000, 0x00FFFFFC, 0x00040000,
	0x00000800, 0x000000FF,
};
u32 size4x4_tex64x64_vtx[14] = {
	0xFFFEFFFE, 0x00000000, 0x00000000, 0x00FF0002,
	0xFFFE0000, 0x08000000, 0x000000FF, 0x00020002,
	0x00000800, 0x08000000, 0x00FFFFFE, 0x00020000,
	0x00000800, 0x000000FF,
};
static u32 size12x12_tex64x64_vtx[14] = {
	0xFFFAFFFA, 0x00000000, 0x00000000, 0x00FF0006,
	0xFFFA0000, 0x08000000, 0x000000FF, 0x00060006,
	0x00000800, 0x08000000, 0x00FFFFFA, 0x00060000,
	0x00000800, 0x000000FF,
};
static u32 size16x16_tex64x64_vtx[14] = {
	0xFFF8FFF8, 0x00000000, 0x00000000, 0x00FF0008,
	0xFFF80000, 0x08000000, 0x000000FF, 0x00080008,
	0x00000800, 0x08000000, 0x00FFFFF8, 0x00080000,
	0x00000800, 0x000000FF,
};
static u32 size32x32_tex32x32_vtx[18] = {
	0xFFE00000, 0x00000000, 0x0000FFFF, 0xFF000000,
	0xFFE00000, 0x04000000, 0xFFFFFF00, 0x00200000,
	0x00000400, 0x0400FFFF, 0xFF000000, 0x00200000,
	0x00000400, 0xFFFFFF00, 0x00000000, 0x00000000,
	0x0000FFFF, 0xFFFF0000,
};

extern u8 lbl_803A8AA0[];

//.sbss2
GXColor unk_804232c8;
GXColor unk_804232cc;
GXColor unk_804232d0;

// NONMATCHING (97.22%): GX/PSMTX call sequence matches; remaining diffs are register
// allocation, stack-frame slot offsets and two compiler scheduling choices (color init order
// and a strength-reduced i*64 induction variable replacing the original clrlslwi).
void effSnowDustDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effSnowDustData* data;
	effSnowDustData* ptr;
	CameraEntry* cam;
	u8* base;
	GXColor color;
	GXColor kcolor;
	GXTexObj texObj;
	Mtx mtx;
	Mtx rotMtx;
	Mtx scaleMtx;
	Mtx texMtxA;
	Mtx texMtxB;
	s32 type;
	s32 i;
	f32 f31;
	f32 f;

	base = lbl_803A8AA0;
	cam = camGetPtr(camId);
	data = effect->userdata;
	color = unk_804232c8;
	color.r = data->field_0x4C;
	color.g = data->field_0x50;
	color.b = data->field_0x54;
	color.a = data->field_0x58;
	f31 = (data->field_0x48 / 255.0f) * data->field_0x5C;
	GXSetTevColor(2, color);
	type = data->type;
	switch (type) {
		case 0:
		case 6:
			GXSetNumChans(0);
			GXSetNumTevStages(2);
			GXSetTevOrder(0, 0, 0, 0xff);
			GXSetTevColorOp(0, 0, 0, 0, 1, 0);
			GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
			GXSetTevColorIn(0, 0xf, 0xf, 0xf, 0xf);
			GXSetTevAlphaIn(0, 7, 4, 1, 7);
			GXSetTevOrder(1, 1, 1, 0xff);
			GXSetTevColorOp(1, 0, 0, 0, 1, 0);
			GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
			GXSetTevColorIn(1, 4, 2, 8, 0xf);
			GXSetTevAlphaIn(1, 7, 7, 7, 0);
			effGetTexObjN64(8, &texObj);
			GXLoadTexObj(&texObj, 0);
			effGetTexObjN64(0x65, &texObj);
			GXLoadTexObj(&texObj, 1);
			GXSetNumTexGens(2);
			GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
			GXSetTexCoordGen2(1, 1, 4, 0x21, 0, 0x7d);
			PSMTXScale(texMtxA, 0.015625f, 0.015625f, 0.0f);
			PSMTXScale(texMtxB, 0.015625f, 0.015625f, 0.0f);
			break;
		case 1:
			GXSetNumChans(0);
			GXSetNumTevStages(2);
			GXSetTevOrder(0, 0, 0, 0xff);
			GXSetTevColorOp(0, 0, 0, 0, 1, 0);
			GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
			GXSetTevColorIn(0, 0xf, 0xf, 0xf, 0xf);
			GXSetTevAlphaIn(0, 7, 4, 1, 7);
			GXSetTevOrder(1, 1, 1, 0xff);
			GXSetTevColorOp(1, 0, 0, 0, 1, 0);
			GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
			GXSetTevColorIn(1, 4, 2, 8, 0xf);
			GXSetTevAlphaIn(1, 7, 7, 7, 0);
			GXSetTevOrder(2, 1, 1, 0xff);
			GXSetTevColorOp(2, 0, 0, 0, 1, 0);
			GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
			GXSetTevColorIn(2, 0, 0xc, 8, 0xf);
			GXSetTevAlphaIn(2, 7, 7, 7, 0);
			effGetTexObjN64(0x66, &texObj);
			GXLoadTexObj(&texObj, 0);
			effGetTexObjN64(0x65, &texObj);
			GXLoadTexObj(&texObj, 1);
			GXSetNumTexGens(2);
			GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
			GXSetTexCoordGen2(1, 1, 4, 0x21, 0, 0x7d);
			PSMTXScale(texMtxA, 0.03125f, 0.03125f, 0.0f);
			PSMTXScale(texMtxB, 0.03125f, 0.03125f, 0.0f);
			break;
		case 2:
		case 3:
			GXSetNumChans(0);
			GXSetNumTevStages(1);
			GXSetTevOrder(0, 0, 0, 0xff);
			GXSetTevColorOp(0, 0, 0, 0, 1, 0);
			GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
			GXSetTevColorIn(0, 4, 2, 8, 0xf);
			GXSetTevAlphaIn(0, 7, 4, 1, 7);
			effGetTexObjN64(0x67, &texObj);
			GXLoadTexObj(&texObj, 0);
			GXSetNumTexGens(1);
			GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
			PSMTXScale(texMtxA, 0.00390625f, 0.015625f, 0.0f);
			break;
		case 4:
		case 5:
			GXSetNumChans(0);
			GXSetNumTevStages(2);
			GXSetTevOrder(0, 0, 0, 0xff);
			GXSetTevColorOp(0, 0, 0, 0, 1, 0);
			GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
			GXSetTevColorIn(0, 0xf, 8, 5, 2);
			GXSetTevAlphaIn(0, 7, 4, 1, 7);
			GXSetTevOrder(1, 0xff, 0xff, 0xff);
			GXSetTevColorOp(1, 1, 0, 0, 1, 0);
			GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
			GXSetTevColorIn(1, 0xf, 4, 5, 0);
			GXSetTevAlphaIn(1, 7, 7, 7, 0);
			effGetTexObjN64(0x68, &texObj);
			GXLoadTexObj(&texObj, 0);
			GXSetNumTexGens(1);
			GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
			PSMTXScale(texMtxA, 0.015625f, 0.015625f, 0.0f);
			break;
		default:
			break;
	}
	GXSetCullMode(0);
	i = 1;
	ptr = data + 1;
	for (; i < effect->count; i++, ptr++) {
		if (ptr->field_0x30 < 0) {
			continue;
		}
		PSMTXTrans(mtx, ptr->x, ptr->y, ptr->z);
		PSMTXRotRad(rotMtx, 'y', -camGetPtr(4)->viewYaw * 0.017453292f);
		PSMTXConcat(mtx, rotMtx, mtx);
		f = ptr->field_0x5C * f31;
		PSMTXScale(scaleMtx, f, f, f);
		PSMTXConcat(mtx, scaleMtx, mtx);
		PSMTXConcat(cam->view, mtx, mtx);
		GXLoadPosMtxImm(mtx, 0);
		GXSetCurrentMtx(0);
		color = unk_804232cc;
		color.r = ptr->field_0x3C;
		color.g = ptr->field_0x40;
		color.b = ptr->field_0x44;
		color.a = (u8)(s32)(f31 * (f32)ptr->field_0x48);
		GXSetTevColor(1, color);
		kcolor = unk_804232d0;
		kcolor.r = 0x50 - ptr->field_0x30;
		kcolor.g = 0x50 - ptr->field_0x30;
		kcolor.b = 0x50 - ptr->field_0x30;
		kcolor.a = 0x50 - ptr->field_0x30;
		GXSetTevKColor(0, kcolor);
		GXLoadTexMtxImm(texMtxA, 0x1e, 1);
		PSMTXTrans(mtx, ptr->field_0x60, ptr->field_0x64, 0.0f);
		PSMTXConcat(texMtxB, mtx, texMtxB);
		GXLoadTexMtxImm(texMtxB, 0x21, 1);
		if (type == 2) {
			PSMTXTrans(mtx, (f32)(i * 64), 64.0f, 0.0f);
			PSMTXConcat(texMtxA, mtx, texMtxA);
			GXLoadTexMtxImm(texMtxA, 0x1e, 1);
		}
		switch (type) {
			case 0:
			case 6:
				effSetVtxDescN64((effVtxArray*)(base + 0x78));
				GXBegin(0x90, 0, 6);
				tri2(0, 1, 2, 0, 0, 2, 3, 0);
				break;
			case 1:
				effSetVtxDescN64((effVtxArray*)(base + 0x40));
				GXBegin(0x90, 0, 6);
				tri2(0, 1, 2, 0, 0, 2, 3, 0);
				break;
			case 2:
			case 3:
				effSetVtxDescN64((effVtxArray*)(base + 0xb0));
				GXBegin(0x90, 0, 6);
				tri2(0, 1, 2, 0, 0, 2, 3, 0);
				break;
			case 4:
			case 5:
				GXSetNumChans(0);
				GXSetNumTevStages(3);
				GXSetTevOrder(0, 0, 0, 0xff);
				GXSetTevColorOp(0, 0, 0, 0, 1, 0);
				GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
				GXSetTevColorIn(0, 0xf, 8, 5, 2);
				GXSetTevAlphaIn(0, 7, 4, 1, 7);
				GXSetTevOrder(1, 0xff, 0xff, 0xff);
				GXSetTevColorOp(1, 1, 0, 0, 1, 0);
				GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
				GXSetTevColorIn(1, 0xf, 4, 5, 0);
				GXSetTevAlphaIn(1, 7, 7, 7, 0);
				GXSetTevOrder(2, 0xff, 0xff, 0xff);
				GXSetTevColorOp(2, 1, 0, 0, 1, 0);
				GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
				GXSetTevColorIn(2, 0xe, 0xf, 0xf, 0);
				GXSetTevAlphaIn(2, 7, 7, 7, 0);
				GXSetTevKColorSel(2, 0xc);
				effSetVtxDescN64((effVtxArray*)(base + 0xe8));
				GXBegin(0x90, 0, 6);
				tri2(0, 1, 2, 0, 0, 2, 3, 0);
				GXSetNumChans(1);
				GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
				GXSetNumTevStages(1);
				GXSetTevOrder(0, 0xff, 0xff, 4);
				GXSetTevColorOp(0, 0, 0, 0, 1, 0);
				GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
				GXSetTevColorIn(0, 0xf, 0xf, 0xf, 0xc);
				GXSetTevAlphaIn(0, 7, 5, 6, 7);
				GXSetTevKAlphaSel(0, 0x1c);
				effSetVtxDescN64((effVtxArray*)(base + 0x120));
				GXBegin(0x90, 0, 0xc);
				tri2(0, 1, 4, 0, 1, 2, 4, 0);
				tri2(0, 4, 3, 0, 4, 2, 3, 0);
				break;
			default:
				break;
		}
	}
}
