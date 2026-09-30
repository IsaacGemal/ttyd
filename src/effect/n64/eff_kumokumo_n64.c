#include "eff/n64/eff_kumokumo_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "driver/camdrv.h"
#include "memory.h"
#include "system.h"
#include <dolphin/mtx.h>
#include <dolphin/gx.h>
#include "eff/eff_n64.h"
#include "math_ext.h"


extern const f32 float_deg2rad_8041f580;
extern const f32 float_90_8041f584;
extern const f32 float_neg1_8041f588;
extern const f32 float_0p4_8041f58c;
extern const f32 float_0p1_8041f590;
extern const f32 float_0_8041f594;
extern const f32 float_360_8041f598;
extern const f32 float_1_8041f5a0;
extern const f32 float_2_8041f5a4;
extern const f32 float_3_8041f5ac;
extern const f32 float_8_8041f5b0;
extern const f32 float_1p5_8041f5b4;
extern const f32 float_30_8041f5b8;
extern const f32 float_6p2832_8041f5bc;
extern const f32 float_0p03125_8041f5c0;
extern const f32 float_200_8041f5c4;
extern const f32 float_0p018182_8041f5c8;
extern const f32 float_0p04_8041f5cc;
extern const f32 float_5_8041f59c;
extern const f32 float_0p5_8041f5a8;
extern u8 lbl_803A2280[];
extern u32 unk_804231a0;
extern u32 unk_804231a4;
extern const u32 dat_8041f578;
extern const u32 dat_8041f57c;
extern u32 cloud_normal_tbl[8];
extern u32 cloud_color0_tbl[5];
extern void* cloud_dl_0_tbl[4];
extern const u8 cloud_dl_0_size_tbl[4];
extern u32 batten_normal_tbl[8];
extern u32 batten_color0_tbl[6];
extern void* batten_dl_0_tbl[4];
extern const u8 batten_dl_0_size_tbl[4];

const f64 double_to_int_802faaa0 = 4503601774854144.0;
const f64 double_to_int_mask_802faaa8 = 4503599627370496.0;
const f64 double_360_802faab0 = 360.0;

extern const Vec vec3_802faa90;

u8 rz_tbl[32] = {
	0x00, 0x09, 0x12, 0x1C, 0x25, 0x2F, 0x39, 0x43, 0x4D, 0x57, 0x61, 0x6B, 0x74, 0x7D, 0x86, 0x8F,
	0x97, 0x9F, 0xA6, 0xAD, 0xB4, 0xB8, 0xB9, 0xB9, 0xB6, 0xB4, 0xB1, 0xAE, 0xAE, 0xAF, 0xB4, 0x00,
};

const char str_802FAAB8[] = "KumokumoN64";


// The int->float magic doubles pool as anonymous labels (@392/@393/@447); the target names them
// double_to_int_802faaa0/double_to_int_mask_802faaa8/double_360_802faab0. The relocation names differ,
// but the instruction bytes are identical.
EffectEntry* effKumokumoN64Entry(s32 type, s32 param, f32 x, f32 y, f32 z, f32 scale, f32 a6, f32 a7) {
	EffectEntry* effect;
	effKumokumoData* data;
	s32 count;
	s32 i;

	effect = effEntry();
	count = 11;
	if (type == 1) {
		count = 1;
	}
	if (type == 2) {
		scale = float_0_8041f594;
	}
	effect->type = str_802FAAB8;
	effect->count = count;
	data = __memAlloc(HEAP_EFFECT, sizeof(effKumokumoData) * count);
	effect->userdata = data;
	effect->callback = effKumokumoMain;
	effect->flags |= 2;
	data->type = type;
	data->field_0x14 = 0;
	if (param <= 0) {
		data->lifetime = 1000;
	}
	else {
		data->lifetime = param;
	}
	data->color1A = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = a7;
	data->color2A = 0xff;
	data->rotation = scale;
	data->camera = 4;
	data->texX = float_0_8041f594;
	data->texY = float_0_8041f594;
	data->texX2 = float_0_8041f594;
	data->texY2 = float_0_8041f594;
	data->texXVel = float_0_8041f594;
	data->texYVel = float_0p018182_8041f5c8;
	data->texX2Vel = float_0_8041f594;
	data->texY2Vel = float_0p04_8041f5cc;
	if (type == 1 || type == 3) {
		data->texXVel *= float_neg1_8041f588;
		data->texYVel *= float_neg1_8041f588;
		data->texX2Vel *= float_neg1_8041f588;
		data->texY2Vel *= float_neg1_8041f588;
	}
	switch (type) {
		case 0:
			data->color1R = 0xff;
			data->color1G = 0xff;
			data->color1B = 0x96;
			data->color2R = 0xd7;
			data->color2G = 0xd2;
			data->color2B = 0xa;
			break;
		case 1:
			data->color1R = 0xe6;
			data->color1G = 0x61;
			data->color1B = 0x2c;
			data->color2R = 0x68;
			data->color2G = 0x2f;
			data->color2B = 0x95;
			break;
		case 2:
			data->color1R = 0xff;
			data->color1G = 0xff;
			data->color1B = 0xff;
			data->color2R = 0xff;
			data->color2G = 0xff;
			data->color2B = 0xff;
			break;
		case 3:
		default:
			data->color1R = 0;
			data->color1G = 0x99;
			data->color1B = 0xff;
			data->color2R = 0;
			data->color2G = 0;
			data->color2B = 0;
			break;
	}
	for (i = 1, data++; i < effect->count; i++, data++) {
		f32 angle, radius;

		switch (type) {
			case 0:
			case 1:
				data->x = float_0_8041f594;
				data->y = float_3_8041f5ac;
				data->z = float_0_8041f594;
				data->scale = float_0_8041f594;
				data->color1A = 0xcc;
				data->lifetime = i * 30;
				data->rotation = (f32)(-irand(20));
				data->texX = float_8_8041f5b0;
				break;
			case 2:
				data->x = (f32)(200 - irand(400));
				data->y = -(f32)(irand(50) + 200);
				data->z = float_0_8041f594;
				data->scale = float_1_8041f5a0;
				data->color1A = 0;
				data->lifetime = i * 15;
				data->rotation = float_0_8041f594;
				break;
			case 3:
				angle = (f32)irand(360);
				radius = float_200_8041f5c4 + (f32)irand(200);
				angle = (float_6p2832_8041f5bc * angle) / float_360_8041f598;
				data->x = radius * (f32)sin((f64)angle);
				data->y = radius * (f32)cos((f64)angle);
				data->z = float_0_8041f594;
				data->scale = float_2_8041f5a4;
				data->color1A = 0;
				data->lifetime = i * 15;
				data->rotation = float_0_8041f594;
				break;
			case 4:
			default:
				data->x = float_0_8041f594;
				data->y = float_3_8041f5ac;
				data->z = float_0_8041f594;
				data->scale = float_0_8041f594;
				data->color1A = 0xcc;
				data->lifetime = i * 30;
				data->rotation = (f32)(-irand(20));
				data->texX = float_8_8041f5b0;
				break;
		}
	}
	return effect;
}

// NONMATCHING (99.39%): the type-0/4 decay blocks keep 30.0/scale in the opposite f-registers from the
// original and the lifetime<16 temp lands in r0 instead of r3 (register allocation only); the compiler's
// int->float magic constants are anonymous where the original named them.
void effKumokumoMain(EffectEntry* effect) {
	effKumokumoData* data;
	Vec pos2;
	Vec pos;
	s32 i;
	s32 type;
	s32 camera;
	f32 angle;
	f32 radius;
	f32 f1;
	f32 f2;
	f32 f3;
	f32 f4;

	data = effect->userdata;
	pos = vec3_802faa90;
	pos.x = data->x;
	pos.y = data->y;
	pos.z = data->z;
	pos2 = pos;
	camera = data->camera;
	type = data->type;
	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 16;
	}
	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	data->field_0x14++;
	if (data->lifetime < 0) {
		effDelete(effect);
		return;
	}
	if (data->lifetime < 16) {
		data->color1A = data->lifetime << 4;
	}
	if (data->field_0x14 < 16) {
		data->color1A = (data->field_0x14 << 4) + 0xf;
	}
	data->texX += data->texXVel;
	data->texY += data->texYVel;
	data->texX2 += data->texX2Vel;
	data->texY2 += data->texY2Vel;
	if (type == 3) {
		data->rotation -= float_2_8041f5a4;
	}
	for (i = 1, data++; i < effect->count; i++, data++) {
		if (data->lifetime != 0) {
			data->lifetime--;
		}
		else {
			switch (type) {
				case 0:
					data->color1A -= 2;
					if (data->color1A < 0) {
						data->x = float_0_8041f594;
						data->y = float_3_8041f5ac;
						data->z = float_0_8041f594;
						data->scale = float_0_8041f594;
						data->color1A = 0xcc;
						data->lifetime = i * 30;
						data->rotation = (f32)(-irand(20));
						data->texX = float_8_8041f5b0;
					}
					else {
						f4 = float_30_8041f5b8;
						f3 = data->scale;
						data->scale = f3 + (float_1p5_8041f5b4 - f3) / f4;
						f3 = data->rotation;
						data->rotation = f3 + (float_30_8041f5b8 - f3) / f4;
						f1 = (float_6p2832_8041f5bc * data->rotation) / float_360_8041f598;
						f3 = (f32)cos((f64)f1);
						data->x += data->texX * f3;
						f1 = (float_6p2832_8041f5bc * data->rotation) / float_360_8041f598;
						f3 = (f32)sin((f64)f1);
						data->y += data->texX * (float_2_8041f5a4 * f3);
						f2 = data->texX;
						data->texX += (float_2_8041f5a4 - f2) / float_30_8041f5b8;
					}
					break;
				case 2:
					data->color1A += (0xff - data->color1A) / 4;
					data->scale += -data->scale * float_0p03125_8041f5c0;
					data->x += -data->x * float_0p03125_8041f5c0;
					data->y += -data->y * float_0p03125_8041f5c0;
					data->z += -data->z * float_0p03125_8041f5c0;
					if (data->scale < float_0p1_8041f590) {
						data->x = (f32)(200 - irand(400));
						data->y = -(f32)(irand(50) + 200);
						data->z = float_0_8041f594;
						data->scale = float_1_8041f5a0;
						data->color1A = 0;
						data->lifetime = i * 15;
						data->rotation = float_0_8041f594;
					}
					data->rotation = (f32)(u32)(rz_tbl[(u32)data->field_0x14 % 31] + ((u32)data->field_0x14 * 180) / 31);
					data->rotation = (f32)fmod(data->rotation, double_360_802faab0);
					break;
				case 3:
					data->color1A += (0xc0 - data->color1A) / 4;
					data->scale += -data->scale * float_0p03125_8041f5c0;
					data->x += -data->x * float_0p03125_8041f5c0;
					data->y += -data->y * float_0p03125_8041f5c0;
					data->z += -data->z * float_0p03125_8041f5c0;
					if (data->scale < float_0p1_8041f590) {
						angle = (f32)irand(360);
						radius = (f32)irand(200);
						angle = (float_6p2832_8041f5bc * angle) / float_360_8041f598;
						radius = float_200_8041f5c4 + radius;
						data->x = radius * (f32)sin((f64)angle);
						data->y = radius * (f32)cos((f64)angle);
						data->z = float_0_8041f594;
						data->scale = float_3_8041f5ac;
						data->color1A = 0;
						data->lifetime = i * 15;
						data->rotation = float_0_8041f594;
					}
					break;
				case 4:
					data->color1A -= 2;
					if (data->color1A < 0) {
						data->x = float_0_8041f594;
						data->y = float_3_8041f5ac;
						data->z = float_0_8041f594;
						data->scale = float_0_8041f594;
						data->color1A = 0xcc;
						data->lifetime = i * 30;
						data->rotation = (f32)(-irand(20));
						data->texX = float_8_8041f5b0;
					}
					else {
						f3 = data->scale;
						f4 = float_30_8041f5b8;
						data->scale = f3 + (float_1_8041f5a0 - f3) / f4;
						f3 = data->rotation;
						data->rotation = f3 + (float_30_8041f5b8 - f3) / f4;
						f1 = (float_6p2832_8041f5bc * data->rotation) / float_360_8041f598;
						f3 = (f32)cos((f64)f1);
						data->x += data->texX * f3;
						f1 = (float_6p2832_8041f5bc * data->rotation) / float_360_8041f598;
						f3 = (f32)sin((f64)f1);
						data->y += data->texX * (float_2_8041f5a4 * f3);
						f2 = data->texX;
						data->texX += (float_2_8041f5a4 - f2) / float_30_8041f5b8;
					}
					break;
				default:
					break;
			}
			data->field_0x14++;
		}
	}
	dispEntry(camera, dispCalcZ(&pos2), 2, effKumokumoDisp, effect);
}

// NONMATCHING (97.60%): the 0x180 stack frame comes out 0x1a0 and the local matrices sit 0x4 lower
// (scale/rot transposed); the per-entry type3/4 body is laid out inline where the original puts it
// out of line, and the quad vertex block reuses one float_0 load where the original reloads it.
void effKumokumoDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effKumokumoData* data = effect->userdata;
	CameraEntry* cam;
	u8* tbl;
	Mtx mtx;
	Mtx rot;
	Mtx scale;
	Mtx view;
	GXTexObj texObj;
	GXColor color;
	f32 halfW;
	f32 halfH;
	f32 f5;
	s32 alpha;
	s32 type;
	s32 i;
	s32 j;
	u32 k;

	tbl = lbl_803A2280;
	cam = camGetPtr(camId);
	alpha = data->color1A;
	type = data->type;

	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', -camGetPtr(camId)->viewYaw * float_deg2rad_8041f580);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXRotRad(rot, 'z', float_deg2rad_8041f580 * data->rotation);
	PSMTXConcat(mtx, rot, mtx);

	if (type != 3) {
		if (data->rotation < float_90_8041f584) {
			PSMTXScale(scale, data->scale, data->scale, data->scale);
		} else {
			PSMTXScale(scale, data->scale, float_neg1_8041f588 * data->scale, data->scale);
		}
		PSMTXConcat(mtx, scale, mtx);
		PSMTXScale(scale, float_0p4_8041f58c, float_0p4_8041f58c, float_0p4_8041f58c);
		PSMTXConcat(mtx, scale, mtx);
	} else {
		PSMTXScale(scale, data->scale, data->scale, data->scale);
		PSMTXConcat(mtx, scale, mtx);
		PSMTXScale(scale, float_0p1_8041f590, float_0p1_8041f590, float_0p1_8041f590);
		PSMTXConcat(mtx, scale, mtx);
	}

	PSMTXConcat(cam->view, mtx, view);
	GXLoadPosMtxImm(view, 0);
	GXSetCurrentMtx(0);

	color = *(GXColor*)&unk_804231a0;
	color.r = data->color1R;
	color.g = data->color1G;
	color.b = data->color1B;
	color.a = alpha;
	GXSetTevColor(1, color);

	color = *(GXColor*)&unk_804231a4;
	color.r = data->color2R;
	color.g = data->color2G;
	color.b = data->color2B;
	color.a = data->color2A;
	GXSetTevColor(2, color);

	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
	GXSetNumTevStages(3);
	GXSetTevOrder(0, 0, 0, 0xff);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 4, 2, 9, 0xf);
	GXSetTevAlphaIn(0, 7, 1, 4, 7);
	GXSetTevOrder(1, 1, 1, 0xff);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 0xf, 0, 9, 0xf);
	GXSetTevAlphaIn(1, 7, 0, 4, 7);
	GXSetTevOrder(2, 0xff, 0xff, 4);
	GXSetTevColorOp(2, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
	GXSetTevColorIn(2, 0xf, 0, 0xa, 0xf);
	GXSetTevAlphaIn(2, 7, 0, 5, 7);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	GXSetTexCoordGen2(1, 1, 5, 0x21, 0, 0x7d);
	effGetTexObj(0x84, &texObj);
	GXLoadTexObj(&texObj, 0);
	effGetTexObj(0x85, &texObj);
	GXLoadTexObj(&texObj, 1);
	PSMTXTrans(mtx, data->texX, data->texY, float_0_8041f594);
	GXLoadTexMtxImm(mtx, 0x1e, 1);
	PSMTXTrans(mtx, data->texX2, data->texY2, float_0_8041f594);
	GXLoadTexMtxImm(mtx, 0x21, 1);
	GXSetCullMode(0);

	switch (type) {
		case 0:
		case 1:
			GXClearVtxDesc();
			GXSetVtxDesc(9, 2);
			GXSetVtxAttrFmt(0, 9, 1, 3, 6);
			GXSetArray(9, tbl + 0x40, 6);
			GXSetVtxDesc(10, 2);
			GXSetVtxAttrFmt(0, 10, 0, 1, 6);
			GXSetArray(10, cloud_normal_tbl, 3);
			GXSetVtxDesc(11, 2);
			GXSetVtxAttrFmt(0, 11, 1, 5, 0);
			GXSetArray(11, cloud_color0_tbl, 4);
			GXSetVtxDesc(13, 2);
			GXSetVtxAttrFmt(0, 13, 1, 3, 0xe);
			GXSetArray(13, tbl + 0x120, 4);
			GXSetVtxDesc(14, 2);
			GXSetVtxAttrFmt(0, 14, 1, 3, 0xe);
			GXSetArray(14, tbl + 0x1c0, 4);
			for (k = 0; k < 4; k++) {
				GXCallDisplayList(cloud_dl_0_tbl[k], cloud_dl_0_size_tbl[k] << 5);
			}
			break;
		case 2:
			GXClearVtxDesc();
			GXSetVtxDesc(9, 2);
			GXSetVtxAttrFmt(0, 9, 1, 3, 6);
			GXSetArray(9, tbl + 0x3e0, 6);
			GXSetVtxDesc(10, 2);
			GXSetVtxAttrFmt(0, 10, 0, 1, 6);
			GXSetArray(10, batten_normal_tbl, 3);
			GXSetVtxDesc(11, 2);
			GXSetVtxAttrFmt(0, 11, 1, 5, 0);
			GXSetArray(11, batten_color0_tbl, 4);
			GXSetVtxDesc(13, 2);
			GXSetVtxAttrFmt(0, 13, 1, 3, 0xe);
			GXSetArray(13, tbl + 0x4c0, 4);
			GXSetVtxDesc(14, 2);
			GXSetVtxAttrFmt(0, 14, 1, 3, 0xe);
			GXSetArray(14, tbl + 0x560, 4);
			for (k = 0; k < 4; k++) {
				GXCallDisplayList(batten_dl_0_tbl[k], batten_dl_0_size_tbl[k] << 5);
			}
			break;
		case 3:
			for (j = 0; j < 5; j++) {
				PSMTXRotRad(rot, 'z', float_deg2rad_8041f580 * (float_360_8041f598 * (f32)j) / float_5_8041f59c);
				PSMTXConcat(view, rot, rot);
				GXLoadPosMtxImm(rot, 0);
				GXSetCurrentMtx(0);
				GXClearVtxDesc();
				GXSetVtxDesc(9, 2);
				GXSetVtxAttrFmt(0, 9, 1, 3, 6);
				GXSetArray(9, tbl + 0x40, 6);
				GXSetVtxDesc(10, 2);
				GXSetVtxAttrFmt(0, 10, 0, 1, 6);
				GXSetArray(10, cloud_normal_tbl, 3);
				GXSetVtxDesc(11, 2);
				GXSetVtxAttrFmt(0, 11, 1, 5, 0);
				GXSetArray(11, cloud_color0_tbl, 4);
				GXSetVtxDesc(13, 2);
				GXSetVtxAttrFmt(0, 13, 1, 3, 0xe);
				GXSetArray(13, tbl + 0x120, 4);
				GXSetVtxDesc(14, 2);
				GXSetVtxAttrFmt(0, 14, 1, 3, 0xe);
				GXSetArray(14, tbl + 0x1c0, 4);
				for (k = 0; k < 4; k++) {
					GXCallDisplayList(cloud_dl_0_tbl[k], cloud_dl_0_size_tbl[k] << 5);
				}
			}
			break;
		default:
			GXClearVtxDesc();
			GXSetVtxDesc(9, 2);
			GXSetVtxAttrFmt(0, 9, 1, 3, 6);
			GXSetArray(9, tbl + 0x40, 6);
			GXSetVtxDesc(10, 2);
			GXSetVtxAttrFmt(0, 10, 0, 1, 6);
			GXSetArray(10, cloud_normal_tbl, 3);
			GXSetVtxDesc(11, 2);
			GXSetVtxAttrFmt(0, 11, 1, 5, 0);
			GXSetArray(11, cloud_color0_tbl, 4);
			GXSetVtxDesc(13, 2);
			GXSetVtxAttrFmt(0, 13, 1, 3, 0xe);
			GXSetArray(13, tbl + 0x120, 4);
			GXSetVtxDesc(14, 2);
			GXSetVtxAttrFmt(0, 14, 1, 3, 0xe);
			GXSetArray(14, tbl + 0x1c0, 4);
			for (k = 0; k < 4; k++) {
				GXCallDisplayList(cloud_dl_0_tbl[k], cloud_dl_0_size_tbl[k] << 5);
			}
			break;
	}

	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 4);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xf, 0xf, 0xf, 0xa);
	GXSetTevAlphaIn(0, 7, 4, 5, 7);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x3c, 0, 0x7d);

	switch (type) {
		case 0:
		case 1:
			effGetTexObj(0x34, &texObj);
			GXLoadTexObj(&texObj, 0);
			break;
		case 2:
			effGetTexObj(0xb, &texObj);
			GXLoadTexObj(&texObj, 0);
			break;
		default:
			effGetTexObjN64(7, &texObj);
			GXLoadTexObj(&texObj, 0);
			break;
	}
	GXSetCullMode(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xd, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xd, 1, 4, 0);

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		if (data->lifetime != 0) {
			continue;
		}
		if (type == 3 || type == 4) {
			GXColor c;
			c = *(GXColor*)&dat_8041f57c;
			c.a = (data->color1A * alpha) / 255;
			GXSetChanMatColor(4, c);
		} else {
			GXColor c;
			c = *(GXColor*)&dat_8041f578;
			c.a = (data->color1A * alpha) / 255;
			GXSetChanMatColor(4, c);
		}
		PSMTXTrans(mtx, data->x, data->y, data->z);
		PSMTXRotRad(rot, 'z', float_deg2rad_8041f580 * data->rotation);
		PSMTXScale(scale, data->scale, data->scale, data->scale);
		PSMTXConcat(view, mtx, mtx);
		PSMTXConcat(mtx, rot, mtx);
		PSMTXConcat(mtx, scale, mtx);
		GXLoadPosMtxImm(mtx, 0);
		GXSetCurrentMtx(0);
		GXBegin(0x80, 0, 4);
		halfW = (f32)GXGetTexObjWidth(&texObj);
		halfH = (f32)GXGetTexObjHeight(&texObj);
		if (type == 3 || type == 4) {
			f5 = float_2_8041f5a4;
		} else {
			f5 = float_1_8041f5a0;
		}
		GXPosition3f32(-halfW * float_0p5_8041f5a8, halfH * float_0p5_8041f5a8, float_0_8041f594);
		GXTexCoord2f32(float_0_8041f594, float_0_8041f594);
		GXPosition3f32(halfW * float_0p5_8041f5a8, halfH * float_0p5_8041f5a8, float_0_8041f594);
		GXTexCoord2f32(f5, float_0_8041f594);
		GXPosition3f32(halfW * float_0p5_8041f5a8, -halfH * float_0p5_8041f5a8, float_0_8041f594);
		GXTexCoord2f32(f5, f5);
		GXPosition3f32(-halfW * float_0p5_8041f5a8, -halfH * float_0p5_8041f5a8, float_0_8041f594);
		GXTexCoord2f32(float_0_8041f594, f5);
		GXEnd();
	}
}
