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
#include "evt/evt_cmd.h"

// The .data tables below (lbl_803A2290 etc.) are not referenced from any code, so the linker dead-strips
// them and the linked .data shrinks by more than a kilobyte, shifting every later section. force_active
// keeps them; the reference object's tables survive because they are not in per-symbol sections.
#pragma force_active on

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u8 lbl_803A2280[8] = { 0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00 };


extern u32 cloud_normal_tbl[8];
extern u32 cloud_color0_tbl[5];
extern void* cloud_dl_0_tbl[4];
extern u8 cloud_dl_0_size_tbl[4];
extern u32 batten_normal_tbl[8];
extern u32 batten_color0_tbl[6];
extern void* batten_dl_0_tbl[4];
extern u8 batten_dl_0_size_tbl[4];




extern u8 rz_tbl[32];
extern s32 negone_one[];
extern s32 one_negone[];

// clang-format off

// The four display-list loops must live in an inlined helper, not inline in effKumokumoDisp: with the
// loops inline MWCC colours their counters k=r22/r23 instead of the target's r26/r25 (the rest of the
// function is byte-identical).
static inline void effKumokumoCallDL(void** dl, const u8* size) {
	u32 k;

	for (k = 0; k < 4; k++) {
		GXCallDisplayList(dl[k], size[k] << 5);
	}
}

// The int->float magic doubles pool as anonymous labels (@392/@393/@447); the target names them
// double_to_int_802faaa0/double_to_int_mask_802faaa8/360.0. The relocation names differ,
// but the instruction bytes are identical.
EffectEntry* effKumokumoN64Entry(s32 type, s32 param, f32 x, f32 y, f32 z, f32 rotation, f32 a6, f32 scale) {
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
		rotation = 0.0f;
	}
	// The four trailing NULs make the .rodata string object 16 bytes. The target has the 12-byte string
	// ("KumokumoN64\0") followed by a 4-byte zero word; an explicit zero object would be emitted in the
	// alignment padding after the .rodata Vec instead, so the padding end of the string is widened here.
	effect->type = "KumokumoN64\0\0\0\0";
	effect->count = count;
	data = __memAlloc(HEAP_EFFECT, sizeof(effKumokumoData) * count);
	effect->userdata = data;
	effect->callback = effKumokumoMain;
	effect->flags |= 2;
	data->type = type;
	data->frame = 0;
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
	data->scale = scale;
	data->color2A = 0xff;
	data->rotation = rotation;
	data->camera = CAMERA_3D;
	data->texX = 0.0f;
	data->texY = 0.0f;
	data->texX2 = 0.0f;
	data->texY2 = 0.0f;
	data->texXVel = 0.0f;
	data->texYVel = 0.018181818f;
	data->texX2Vel = 0.0f;
	data->texY2Vel = 0.04f;
	if (type == 1 || type == 3) {
		data->texXVel *= -1.0f;
		data->texYVel *= -1.0f;
		data->texX2Vel *= -1.0f;
		data->texY2Vel *= -1.0f;
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
				data->x = 0.0f;
				data->y = 3.0f;
				data->z = 0.0f;
				data->scale = 0.0f;
				data->color1A = 0xcc;
				data->lifetime = i * 30;
				data->rotation = (f32)(-irand(20));
				data->texX = 8.0f;
				break;
			case 2:
				data->x = (f32)(200 - irand(400));
				data->y = -(f32)(irand(50) + 200);
				data->z = 0.0f;
				data->scale = 1.0f;
				data->color1A = 0;
				data->lifetime = i * 15;
				data->rotation = 0.0f;
				break;
			case 3:
				angle = (f32)irand(360);
				radius = 200.0f + (f32)irand(200);
				angle = (6.2831855f * angle) / 360.0f;
				data->x = radius * (f32)sin((f64)angle);
				data->y = radius * (f32)cos((f64)angle);
				data->z = 0.0f;
				data->scale = 2.0f;
				data->color1A = 0;
				data->lifetime = i * 15;
				data->rotation = 0.0f;
				break;
			case 4:
			default:
				data->x = 0.0f;
				data->y = 3.0f;
				data->z = 0.0f;
				data->scale = 0.0f;
				data->color1A = 0xcc;
				data->lifetime = i * 30;
				data->rotation = (f32)(-irand(20));
				data->texX = 8.0f;
				break;
		}
	}
	return effect;
}


// The type-3 respawn's angle must be built in the shared temp f1, not a dedicated local: reusing the
// variable makes MWCC coalesce the raw (f32)irand(360) and its normalized value into f31, as the target
// does. The type-0/4 decay writes scale/rotation without f3/f4 temporaries (with them MWCC colours
// 30.0/scale to the opposite f-registers), and frame is copied to a local before the lifetime<16
// test so the compiler hoists its reload ahead of that branch's bge.
void effKumokumoMain(EffectEntry* effect) {
	effKumokumoData* data;
	Vec pos2;
	Vec pos;
	s32 i;
	s32 type;
	s32 camera;
	f32 radius;
	f32 f1;
	f32 f2;
	f32 f3;
	f32 f4;
	f32 k = 0.03125f;
	s32 frame;

	data = effect->userdata;
	pos = (Vec){ 0.0f, 0.0f, 0.0f };
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
	data->frame++;
	if (data->lifetime < 0) {
		effDelete(effect);
		return;
	}
	frame = data->frame;
	if (data->lifetime < 16) {
		data->color1A = data->lifetime << 4;
	}
	if (frame < 16) {
		data->color1A = (frame << 4) + 0xf;
	}
	data->texX += data->texXVel;
	data->texY += data->texYVel;
	data->texX2 += data->texX2Vel;
	data->texY2 += data->texY2Vel;
	if (type == 3) {
		data->rotation -= 2.0f;
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
						data->x = 0.0f;
						data->y = 3.0f;
						data->z = 0.0f;
						data->scale = 0.0f;
						data->color1A = 0xcc;
						data->lifetime = i * 30;
						data->rotation = (f32)(-irand(20));
						data->texX = 8.0f;
					}
					else {
						// (1.0f + 0.5f) folds to 1.5f but makes MWCC put data->scale in f3 and 30.0f in f4 (the
						// literal 1.5f swaps them, which differs by nine instructions).
						data->scale += ((1.0f + 0.5f) - data->scale) / 30.0f;
						data->rotation += (30.0f - data->rotation) / 30.0f;
						f1 = (6.2831855f * data->rotation) / 360.0f;
						f3 = (f32)cos((f64)f1);
						data->x += data->texX * f3;
						f1 = (6.2831855f * data->rotation) / 360.0f;
						f3 = (f32)sin((f64)f1);
						data->y += data->texX * (2.0f * f3);
						f2 = data->texX;
						data->texX += (2.0f - f2) / 30.0f;
					}
					break;
				case 2:
					data->color1A += (0xff - data->color1A) / 4;
					data->scale += -data->scale * k;
					data->x += -data->x * k;
					data->y += -data->y * k;
					data->z += -data->z * k;
					if (data->scale < 0.1f) {
						data->x = (f32)(200 - irand(400));
						data->y = -(f32)(irand(50) + 200);
						data->z = 0.0f;
						data->scale = 1.0f;
						data->color1A = 0;
						data->lifetime = i * 15;
						data->rotation = 0.0f;
					}
					data->rotation = (f32)(rz_tbl[(u32)data->frame % 31] + ((u32)data->frame * 180) / 31);
					data->rotation = (f32)fmod(data->rotation, 360.0);
					break;
				case 3:
					data->color1A += (0xc0 - data->color1A) / 4;
					data->scale += -data->scale * k;
					data->x += -data->x * k;
					data->y += -data->y * k;
					data->z += -data->z * k;
					if (data->scale < 0.1f) {
						f1 = (f32)irand(360);
						radius = (f32)irand(200);
						f1 = (6.2831855f * f1) / 360.0f;
						radius = 200.0f + radius;
						data->x = radius * (f32)sin((f64)f1);
						data->y = radius * (f32)cos((f64)f1);
						data->z = 0.0f;
						data->scale = 3.0f;
						data->color1A = 0;
						data->lifetime = i * 15;
						data->rotation = 0.0f;
					}
					break;
				case 4:
					data->color1A -= 2;
					if (data->color1A < 0) {
						data->x = 0.0f;
						data->y = 3.0f;
						data->z = 0.0f;
						data->scale = 0.0f;
						data->color1A = 0xcc;
						data->lifetime = i * 30;
						data->rotation = (f32)(-irand(20));
						data->texX = 8.0f;
					}
					else {
						data->scale += (1.0f - data->scale) / 30.0f;
						data->rotation += (30.0f - data->rotation) / 30.0f;
						f1 = (6.2831855f * data->rotation) / 360.0f;
						f3 = (f32)cos((f64)f1);
						data->x += data->texX * f3;
						f1 = (6.2831855f * data->rotation) / 360.0f;
						f3 = (f32)sin((f64)f1);
						data->y += data->texX * (2.0f * f3);
						f2 = data->texX;
						data->texX += (2.0f - f2) / 30.0f;
					}
					break;
				default:
					break;
			}
			data->frame++;
		}
	}
	dispEntry(camera, dispCalcZ(&pos2), 2, effKumokumoDisp, effect);
}


void effKumokumoDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effKumokumoData* data = effect->userdata;
	CameraEntry* cam;
	u8* tbl;
	Mtx mtx;
	Mtx scale;
	Mtx rot;
	Mtx view;
	GXTexObj texObj;
	f32 halfW;
	f32 halfH;
	f32 half = 0.5f;
	f32 f5;
	s32 alpha;
	s32 type;
	s32 i;
	s32 j;

	tbl = lbl_803A2280;
	cam = camGetPtr(camId);
	alpha = data->color1A;
	type = data->type;

	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', -camGetPtr(camId)->viewYaw * 0.017453292f);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXRotRad(rot, 'z', 0.017453292f * data->rotation);
	PSMTXConcat(mtx, rot, mtx);

	if (type != 3) {
		if (data->rotation < 90.0f) {
			PSMTXScale(scale, data->scale, data->scale, data->scale);
		} else {
			PSMTXScale(scale, data->scale, -1.0f * data->scale, data->scale);
		}
		PSMTXConcat(mtx, scale, mtx);
		PSMTXScale(scale, 0.4f, 0.4f, 0.4f);
		PSMTXConcat(mtx, scale, mtx);
	} else {
		PSMTXScale(scale, data->scale, data->scale, data->scale);
		PSMTXConcat(mtx, scale, mtx);
		PSMTXScale(scale, 0.1f, 0.1f, 0.1f);
		PSMTXConcat(mtx, scale, mtx);
	}

	PSMTXConcat(cam->view, mtx, view);
	GXLoadPosMtxImm(view, 0);
	GXSetCurrentMtx(0);

	GXSetTevColor(1, (GXColor){ data->color1R, data->color1G, data->color1B, alpha });
	GXSetTevColor(2, (GXColor){ data->color2R, data->color2G, data->color2B, data->color2A });

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
	PSMTXTrans(mtx, data->texX, data->texY, 0.0f);
	GXLoadTexMtxImm(mtx, 0x1e, 1);
	PSMTXTrans(mtx, data->texX2, data->texY2, 0.0f);
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
			effKumokumoCallDL(cloud_dl_0_tbl, cloud_dl_0_size_tbl);
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
			effKumokumoCallDL(batten_dl_0_tbl, batten_dl_0_size_tbl);
			break;
		case 3:
			for (j = 0; j < 5; j++) {
				float angle = (360.0f * (f32)j) / 5.0f;

				PSMTXRotRad(rot, 'z', 0.017453292f * angle);
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
				effKumokumoCallDL(cloud_dl_0_tbl, cloud_dl_0_size_tbl);
			}
			break;
		case 4:
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
			effKumokumoCallDL(cloud_dl_0_tbl, cloud_dl_0_size_tbl);
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
		case 3:
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
		if (type != 3 && type != 4) {
			GXSetChanMatColor(4, (GXColor){ 0xDB, 0xDF, 0xE7, (data->color1A * alpha) / 255 });
		} else {
			GXSetChanMatColor(4, (GXColor){ 0xFF, 0xFF, 0xFF, (data->color1A * alpha) / 255 });
		}
		PSMTXTrans(mtx, data->x, data->y, data->z);
		PSMTXRotRad(rot, 'z', 0.017453292f * data->rotation);
		PSMTXScale(scale, data->scale, data->scale, data->scale);
		PSMTXConcat(view, mtx, mtx);
		PSMTXConcat(mtx, rot, mtx);
		PSMTXConcat(mtx, scale, mtx);
		GXLoadPosMtxImm(mtx, 0);
		GXSetCurrentMtx(0);
		GXBegin(0x80, 0, 4);
		halfW = (f32)GXGetTexObjWidth(&texObj);
		halfH = (f32)GXGetTexObjHeight(&texObj);
		f5 = 1.0f;
		if (type == 3 || type == 4) {
			f5 = 2.0f;
		}
		GXPosition3f32(-halfW * half, halfH * half, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(halfW * half, halfH * half, 0.0f);
		GXTexCoord2f32(f5, 0.0f);
		GXPosition3f32(halfW * half, -halfH * half, 0.0f);
		GXTexCoord2f32(f5, f5);
		GXPosition3f32(-halfW * half, -halfH * half, 0.0f);
		GXTexCoord2f32(0.0f, f5);
		GXEnd();
	}
}

// TODO: type
static __declspec(section ".data") u32 negone_one_374[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_803A2290[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
static __declspec(section ".data") u32 one_negone_373[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_803A22A0[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
static __declspec(section ".data") u32 negone_one_360[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_803A22B0[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
static __declspec(section ".data") u32 one_negone_359[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
u32 cloud_vertex_tbl[56] = {
    0x61BF081C, 0x00006A40, 0x19CD0000, 0x6C5F3000,
    0x0000670F, 0x406A0000, 0x60AF4515, 0x0000571F,
    0xFDF60000, 0x571F0D58, 0x0000571F, 0x20150000,
    0x571F32D2, 0x0000571F, 0x42350000, 0x4400F9C8,
    0x00004400, 0x05DA0000, 0x440013D5, 0x00004400,
    0x21D00000, 0x44002DE3, 0x00002FD0, 0xF8860000,
    0x2FD00123, 0x00002FD0, 0x0A7F0000, 0x2FD013DC,
    0x00002FD0, 0x1C7A0000, 0x1CB0F845, 0x00001CB0,
    0xFDAA0000, 0x1CB00300, 0x00001CB0, 0x08560000,
    0x1CB00DBB, 0x00000DCF, 0xF9E80000, 0x0DCFFD27,
    0x00000DD0, 0x00000000, 0x0DD002D9, 0x00000DD0,
    0x06180000, 0x0000FCEC, 0x00000000, 0xFEA10000,
    0x00000000, 0x00000000, 0x015F0000, 0x00000314,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 cloud_texcoord0_tbl[40] = {
    0x0000CCCD, 0x1000CCCD, 0x1000C001, 0x0000C001,
    0x2000CCCD, 0x2000C001, 0x3000CCCD, 0x3000C001,
    0x4000CCCD, 0x4000C001, 0x0000D667, 0x1000D667,
    0x2000D667, 0x3000D667, 0x4000D667, 0x0000E000,
    0x1000E000, 0x2000E000, 0x3000E000, 0x4000E000,
    0x0000E99A, 0x1000E99A, 0x2000E99A, 0x3000E99A,
    0x4000E99A, 0x0000F334, 0x1000F334, 0x2000F334,
    0x3000F334, 0x4000F334, 0x00000000, 0x10000000,
    0x20000000, 0x30000000, 0x40000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 cloud_texcoord1_tbl[40] = {
    0x0000CCCD, 0x1000CCCD, 0x1000C001, 0x0000C001,
    0x2000CCCD, 0x2000C001, 0x3000CCCD, 0x3000C001,
    0x4000CCCD, 0x4000C001, 0x0000D667, 0x1000D667,
    0x2000D667, 0x3000D667, 0x4000D667, 0x0000E000,
    0x1000E000, 0x2000E000, 0x3000E000, 0x4000E000,
    0x0000E99A, 0x1000E99A, 0x2000E99A, 0x3000E99A,
    0x4000E99A, 0x0000F334, 0x1000F334, 0x2000F334,
    0x3000F334, 0x4000F334, 0x00000000, 0x10000000,
    0x20000000, 0x30000000, 0x40000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 cloud_dl_0_0[24] = {
    0x98000E1E, 0x00001E1E, 0x1F00001F, 0x1F190001,
    0x19191A00, 0x031A1A14, 0x00011414, 0x15000315,
    0x150F0001, 0x0F0F1000, 0x0310100A, 0x00010A0A,
    0x0B00030B, 0x0B050000, 0x00000600, 0x01010100,
    0x00000303, 0x01000002, 0x02000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 cloud_dl_0_1[24] = {
    0x98000E1F, 0x00001F1F, 0x20000020, 0x201A0003,
    0x1A1A1B00, 0x041B1B15, 0x00031515, 0x16000416,
    0x16100003, 0x10101100, 0x0411110B, 0x00030B0B,
    0x0C00040C, 0x0C060001, 0x01010700, 0x02040401,
    0x00000202, 0x02000005, 0x05000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 cloud_dl_0_2[24] = {
    0x98000E20, 0x00002020, 0x21000021, 0x211B0004,
    0x1B1B1C00, 0x031C1C16, 0x00041616, 0x17000317,
    0x17110004, 0x11111200, 0x0312120C, 0x00040C0C,
    0x0D00030D, 0x0D070002, 0x04040800, 0x01060602,
    0x00000505, 0x03000007, 0x07000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 cloud_dl_0_3[24] = {
    0x98000E21, 0x00002121, 0x22000022, 0x221C0003,
    0x1C1C1D00, 0x011D1D17, 0x00031717, 0x18000118,
    0x18120003, 0x12121300, 0x0113130D, 0x00030D0D,
    0x0E00010E, 0x0E080001, 0x06060900, 0x00080803,
    0x00000707, 0x04000009, 0x09000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 batten_vertex_tbl[56] = {
    0xC890C3A1, 0x0000DEBD, 0xC1000000, 0x0000BE60,
    0x00002143, 0xC1000000, 0x3770C3A1, 0x0000D58E,
    0xCE200000, 0xE689CE20, 0x00000000, 0xCE200000,
    0x1977CE20, 0x00002A72, 0xCE200000, 0xDEC0D8A0,
    0x0000EC0D, 0xD8A00000, 0x0000D8A0, 0x000013F3,
    0xD8A00000, 0x2140D8A0, 0x0000E393, 0xE0800000,
    0xEEF2E080, 0x00000000, 0xE0800000, 0x110EE080,
    0x00001C6D, 0xE0800000, 0xE7C3E860, 0x0000F175,
    0xE8600000, 0x0000E860, 0x00000E8B, 0xE8600000,
    0x183DE860, 0x0000ED67, 0xF5800000, 0xF4D7F580,
    0x00000000, 0xF5800000, 0x0B29F580, 0x00001299,
    0xF5800000, 0xEF430000, 0x0000F5F5, 0x00000000,
    0x00000000, 0x00000A0B, 0x00000000, 0x10BD0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 batten_texcoord0_tbl[40] = {
    0x00002AAA, 0x10002AAA, 0x10003000, 0x00003000,
    0x20002AAA, 0x20003000, 0x30002AAA, 0x30003000,
    0x40002AAA, 0x40003000, 0x00002555, 0x10002555,
    0x20002555, 0x30002555, 0x40002555, 0x00002000,
    0x10002000, 0x20002000, 0x30002000, 0x40002000,
    0x00001AAA, 0x10001AAA, 0x20001AAA, 0x30001AAA,
    0x40001AAA, 0x00001555, 0x10001555, 0x20001555,
    0x30001555, 0x40001555, 0x00001000, 0x10001000,
    0x20001000, 0x30001000, 0x40001000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 batten_texcoord1_tbl[40] = {
    0x00002AAA, 0x10002AAA, 0x10003000, 0x00003000,
    0x20002AAA, 0x20003000, 0x30002AAA, 0x30003000,
    0x40002AAA, 0x40003000, 0x00002555, 0x10002555,
    0x20002555, 0x30002555, 0x40002555, 0x00002000,
    0x10002000, 0x20002000, 0x30002000, 0x40002000,
    0x00001AAA, 0x10001AAA, 0x20001AAA, 0x30001AAA,
    0x40001AAA, 0x00001555, 0x10001555, 0x20001555,
    0x30001555, 0x40001555, 0x00001000, 0x10001000,
    0x20001000, 0x30001000, 0x40001000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 batten_dl_0_0[24] = {
    0x98000E1E, 0x00021E1E, 0x1F00021F, 0x1F190000,
    0x19191A00, 0x041A1A14, 0x00001414, 0x15000415,
    0x150F0000, 0x0F0F1000, 0x0410100A, 0x00000A0A,
    0x0B00040B, 0x0B050000, 0x00000600, 0x01010100,
    0x00020303, 0x01000202, 0x02000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 batten_dl_0_1[24] = {
    0x98000E1F, 0x00021F1F, 0x20000220, 0x201A0004,
    0x1A1A1B00, 0x051B1B15, 0x00041515, 0x16000516,
    0x16100004, 0x10101100, 0x0511110B, 0x00040B0B,
    0x0C00050C, 0x0C060001, 0x01010700, 0x03040401,
    0x00020202, 0x02000205, 0x05000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 batten_dl_0_2[24] = {
    0x98000E20, 0x00022020, 0x21000221, 0x211B0005,
    0x1B1B1C00, 0x041C1C16, 0x00051616, 0x17000417,
    0x17110005, 0x11111200, 0x0412120C, 0x00050C0C,
    0x0D00040D, 0x0D070003, 0x04040800, 0x01060602,
    0x00020505, 0x03000207, 0x07000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 batten_dl_0_3[24] = {
    0x98000E21, 0x00022121, 0x22000222, 0x221C0004,
    0x1C1C1D00, 0x001D1D17, 0x00041717, 0x18000018,
    0x18120004, 0x12121300, 0x0013130D, 0x00040D0D,
    0x0E00000E, 0x0E080001, 0x06060900, 0x00080803,
    0x00020707, 0x04000209, 0x09000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 cloud_normal_tbl[8] = {
    0x00004000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 cloud_color0_tbl[5] = {
    0xFFFFFF00, 0xFFFFFF33, 0xFFFFFF66, 0xFFFFFF7F,
    0xFFFFFFCC,
};

// TODO: type
void* cloud_dl_0_tbl[4] = {
    cloud_dl_0_0, cloud_dl_0_1, cloud_dl_0_2, cloud_dl_0_3,
};

// TODO: type
u8 cloud_dl_0_size_tbl[4] = {
    0x03, 0x03, 0x03, 0x03,
};

// 24 zero bytes between the cloud and batten .sdata tables: an all-zero object needs explicit_zero_data
// to be emitted and force_active to survive the linker.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_80413AC8_sdata[6] = { 0 };
#pragma force_active reset
#pragma explicit_zero_data reset

// TODO: type
u32 batten_normal_tbl[8] = {
    0x00004000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 batten_color0_tbl[6] = {
    0xFFFFFF33, 0xFFFFFF66, 0xFFFFFF00, 0xFFFFFF99,
    0xFFFFFF7F, 0xFFFFFFCC,
};

// TODO: type
void* batten_dl_0_tbl[4] = {
    batten_dl_0_0, batten_dl_0_1, batten_dl_0_2, batten_dl_0_3,
};

// TODO: type
u8 batten_dl_0_size_tbl[4] = {
    0x03, 0x03, 0x03, 0x03,
};

u8 rz_tbl[32] = {
	0x00, 0x09, 0x12, 0x1C, 0x25, 0x2F, 0x39, 0x43, 0x4D, 0x57, 0x61, 0x6B, 0x74, 0x7D, 0x86, 0x8F,
	0x97, 0x9F, 0xA6, 0xAD, 0xB4, 0xB8, 0xB9, 0xB9, 0xB6, 0xB4, 0xB1, 0xAE, 0xAE, 0xAF, 0xB4, 0x00,
};

// 4-byte zero word that ends the unit's .sdata.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_80413B4C_sdata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset
// clang-format on

#pragma force_active reset
