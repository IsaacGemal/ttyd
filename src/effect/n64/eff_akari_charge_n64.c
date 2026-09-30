#include "eff/n64/eff_akari_charge_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sdata2 float constants (named after their original addresses).
extern const f32 float_deg2rad_8041e8a8;
extern const f32 float_0_8041e8ac;
extern const f32 float_0p0625_8041e8b0;
extern const f32 float_1_8041e8b4;
extern const f32 float_0p0039062_8041e8b8;
extern const f32 float_0p03125_8041e8bc;

// .rodata double used for the integer-to-float conversion.
extern const f64 double_to_int_802fa188;

// .sbss2 scratch GXColors the original builds its TEV colours from.
extern GXColor unk_80423090;
extern GXColor unk_80423094;

// .data vertex array, local to this file.
extern u32 size2x8_tex8x16_vtx[];

typedef struct EffAkariChargeParticle {
	s32 field_0x0;  //0x0
	f32 field_0x4;  //0x4
	f32 field_0x8;  //0x8
	f32 field_0xC;  //0xC
	s32 life;       //0x10
	s32 counter;    //0x14
	u8 pad_0x18[0xC]; //0x18
	s32 field_0x24;   //0x24
	u8 pad_0x28[0xC]; //0x28
	f32 scale;      //0x34
	f32 field_0x38; //0x38
	f32 angle;      //0x3C
	s32 index;      //0x40
} EffAkariChargeParticle; //0x44

typedef struct EffAkariChargeWork {
	s32 type;       //0x0
	f32 x;          //0x4
	f32 y;          //0x8
	f32 z;          //0xC
	s32 field_0x10; //0x10
	s32 field_0x14; //0x14
	s32 field_0x18; //0x18
	s32 field_0x1C; //0x1C
	s32 field_0x20; //0x20
	s32 field_0x24; //0x24
	s32 field_0x28; //0x28
	s32 field_0x2C; //0x2C
	s32 field_0x30; //0x30
	f32 field_0x34; //0x34
	f32 field_0x38; //0x38
	f32 field_0x3C; //0x3C
	s32 field_0x40; //0x40
	EffAkariChargeParticle particles[21]; //0x44
} EffAkariChargeWork;

EffectEntry* effAkariChargeN64Entry(s32 type, s32 a2, f32 a3, f32 a4, f32 a5, f32 a6) {
	EffectEntry* entry;
	EffAkariChargeWork* work;
	EffAkariChargeParticle* p;
	s32 n;

	entry = effEntry();
	entry->type = "AkariChargeN64";
	entry->count = 0x16;
	work = __memAlloc(HEAP_EFFECT, 0x5D8);
	entry->userdata = work;
	entry->callback = effAkariChargeMain;
	entry->flags |= 2;

	work->type = type;
	work->field_0x14 = 0;
	if (a2 <= 0) {
		work->field_0x10 = 1000;
	} else {
		work->field_0x10 = a2;
	}
	work->field_0x24 = 0;
	work->x = a3;
	work->y = a4;
	work->z = a5;
	work->field_0x38 = a6;
	work->field_0x18 = 0xFF;
	work->field_0x1C = 0xE6;
	work->field_0x20 = 0;
	work->field_0x28 = 0xFF;
	work->field_0x2C = 0xFF;
	work->field_0x30 = 0xFF;

	n = 1;
	p = work->particles;
	while (n < 22) {
		p->life = 20;
		p->counter = 0;
		p->angle = (f32)((n - 1) * 360 / 21);
		p->field_0x4 = 0.0f;
		p->field_0x8 = 0.0f;
		p->scale = 1.0f;
		p->index = n & 0xF;
		n++;
		p++;
	}

	return entry;
}

// The two integer-to-float PSMTXTrans calls use 0.0f literals; naming float_0_8041e8ac there makes mwcc hoist
// it into a fourth fpr, which grows the frame and shifts every local. The prologue colour is built at 0x10 and
// copied to 0x14 in the original, the loop colour at 0x8 copied to 0xc; mwcc here puts the prologue copy at
// 0xc, the loop local at 0x14 and the loop copy at 0x8 (the same local/outgoing swap eff_akari_pose_n64.c
// documents), and schedules the three loop-setup insns around double_to_int/part differently.
// NONMATCHING (99.13%): only those GXColor stack slots and the loop-setup instruction order differ.
void effAkariChargeDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	EffAkariChargeParticle* part;
	EffAkariChargeWork* data;
	CameraEntry* cam;
	GXTexObj texObj;
	GXColor color2;
	GXColor color;
	Mtx mtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	Mtx viewMtx;
	s32 i;
	s32 field24;
	s32 type;
	s32 field14;
	s32 colR;
	s32 colG;
	s32 colB;
	f32 proj[7];
	f32 viewport[6];

	cam = camGetPtr(CAMERA_3D);
	data = effect->userdata;
	type = data->type;
	field14 = data->field_0x14;
	field24 = data->field_0x24;
	colR = data->field_0x18;
	colG = data->field_0x1C;
	colB = data->field_0x20;
	if (type == 1) {
		GXGetProjectionv(proj);
		GXGetViewportv(viewport);
		GXSetProjection(cam->projection, cam->projType);
		GXSetZMode(0, 7, 0);
	}

	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXRotRad(rotMtx, 'y', -camGetPtr(CAMERA_3D)->viewYaw * float_deg2rad_8041e8a8);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXScale(scaleMtx, data->field_0x38, data->field_0x38, data->field_0x38);
	PSMTXConcat(mtx, scaleMtx, viewMtx);
	PSMTXConcat(cam->view, viewMtx, viewMtx);

	color = unk_80423090;
	color.r = data->field_0x28;
	color.g = data->field_0x2C;
	color.b = data->field_0x30;
	GXSetTevColor(2, color);
	GXSetNumChans(0);
	GXSetNumTevStages(2);
	GXSetTevOrder(0, 0, 0, 0xff);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xf, 8, 8, 0xf);
	GXSetTevAlphaIn(0, 7, 1, 4, 7);
	GXSetTevOrder(1, 0xff, 0xff, 0xff);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 2, 4, 0, 0xf);
	GXSetTevAlphaIn(1, 7, 7, 7, 0);
	effGetTexObjN64(0x70, &texObj);
	GXLoadTexObj(&texObj, 0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	GXSetCullMode(0);
	effSetVtxDescN64((effVtxArray*)size2x8_tex8x16_vtx);

	i = 1;
	for (part = data->particles; i < effect->count; i++, part++) {
		color2 = unk_80423094;
		color2.r = colR;
		color2.g = colG;
		color2.b = colB;
		color2.a = (part->field_0x24 * field24) / 255;
		GXSetTevColor(1, color2);

		PSMTXTrans(mtx, part->field_0x4, part->field_0x8, float_0_8041e8ac);
		PSMTXRotRad(rotMtx, 'z', float_deg2rad_8041e8a8 * part->angle);
		PSMTXConcat(mtx, rotMtx, mtx);
		PSMTXScale(scaleMtx, part->scale * float_0p0625_8041e8b0, part->scale * float_0p0625_8041e8b0,
		           float_1_8041e8b4);
		PSMTXConcat(mtx, scaleMtx, mtx);
		PSMTXConcat(viewMtx, mtx, mtx);
		GXLoadPosMtxImm(mtx, 0);
		GXSetCurrentMtx(0);

		PSMTXScale(scaleMtx, float_0p0039062_8041e8b8, float_0p03125_8041e8bc, float_0_8041e8ac);
		PSMTXTrans(mtx, (f32)(part->index << 4), 0.0f, 0.0f);
		PSMTXConcat(scaleMtx, mtx, scaleMtx);
		GXLoadTexMtxImm(scaleMtx, 0x1e, 1);

		PSMTXScale(scaleMtx, float_0p0039062_8041e8b8, float_0p03125_8041e8bc, float_0_8041e8ac);
		PSMTXTrans(mtx, (f32)(field14 * 3), 0.0f, 0.0f);
		PSMTXConcat(scaleMtx, mtx, scaleMtx);
		GXLoadTexMtxImm(scaleMtx, 0x21, 1);

		GXBegin(0x90, 0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
	}

	if (type == 1) {
		GXSetProjectionv(proj);
		GXSetViewport(viewport[0], viewport[1], viewport[2], viewport[3], viewport[4], viewport[5]);
	}
}
