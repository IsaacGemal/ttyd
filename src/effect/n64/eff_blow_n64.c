#include "eff/n64/eff_blow_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sdata2 word constant of this unit (volatile: forces the per-element reload the original does).
const volatile u32 dat_8041e980 = 0xC8FFFF00;

// .data: retail 16x16 quad vertex array.
static u32 size16x16_tex32x32_vtx[14] = {
	0xFFF8FFF8, 0x00000000, 0x0000FFFF, 0xFFFF0008,
	0xFFF80000, 0x04000000, 0xFFFFFFFF, 0x00080008,
	0x00000400, 0x0400FFFF, 0xFFFFFFF8, 0x00080000,
	0x00000400, 0xFFFFFFFF,
};

static inline void blowSetColor(u8 a) {
    GXColor color = *(GXColor*)&dat_8041e980;
    color.a = a;
    GXSetTevColor(1, color);
}

// .rodata anchor: the zero-Vec literal in effBlowMain is emitted at .rodata+0xC, right after this symbol, which
// the original unit also has (it belongs to the not-yet-decompiled effBlowN64Entry). Without it the literal lands
// at .rodata+0 and objdiff maps it to the wrong target symbol. Reuse this definition in effBlowN64Entry.
const Vec vec3_802fa250 = { 0.0f, 0.0f, 0.0f };

typedef struct effBlowParticle {
	s32 field_0x00;  // 0x00
	s32 type;        // 0x04
	f32 x;           // 0x08
	f32 y;           // 0x0C
	f32 z;           // 0x10
	f32 vx;          // 0x14
	f32 vy;          // 0x18
	f32 vz;          // 0x1C
	f32 field_0x20;  // 0x20
	f32 field_0x24;  // 0x24
	f32 field_0x28;  // 0x28
	f32 field_0x2C;  // 0x2C
	f32 field_0x30;  // 0x30
	f32 field_0x34;  // 0x34
	f32 field_0x38;  // 0x38
	f32 field_0x3C;  // 0x3C
	f32 field_0x40;  // 0x40
	f32 scaleX;      // 0x44
	f32 scaleY;      // 0x48
	f32 rotZ;        // 0x4C
	f32 field_0x50;  // 0x50
	f32 field_0x54;  // 0x54
	s32 field_0x58;  // 0x58
	s32 life;        // 0x5C
	s32 field_0x60;  // 0x60
	s32 field_0x64;  // 0x64
} effBlowParticle;

extern const f32 float_0p2_8041e9ac;
extern const f32 float_0p225_8041e9b0;
extern const f32 float_0p45_8041e9b4;
extern const f32 float_0p8_8041e9b8;

void effBlowMain(EffectEntry* effect) {
	effBlowParticle* data;
	Vec position2;
	Vec position;
	s32 life;
	s32 i;
	s32 elemLife;
	s32 type;

	data = effect->userdata;
	position = (Vec){ 0.0f, 0.0f, 0.0f };
	position.x = data->x;
	position.y = data->y;
	position.z = data->z;
	position2 = position;

	data->life--;
	data->field_0x60++;
	life = data->life;
	type = data->type;
	if (life < 0 && type == 2) {
		effDelete(effect);
		return;
	}

	for (i = 1, data++; i < effect->count; i++, data++) {
		if (data->field_0x64 > 0) {
			data->field_0x64--;
			if (data->field_0x64 > 0) {
				continue;
			}
		}
		data->life--;
		elemLife = data->life;
		if (elemLife < 0) {
			if (type == 0) {
				data->x = 0.0f;
				data->y = 0.0f;
				data->z = 0.0f;
				data->vx = data->field_0x20;
				data->vy = data->field_0x24;
				data->vz = data->field_0x28;
				data->scaleX = 0.5f;
				data->scaleY = 4.0f;
				data->field_0x50 = 20.0f;
				data->rotZ = data->field_0x54;
				data->field_0x58 = 0xFF;
				data->life = 10;
				data->life--;
				elemLife = data->life;
			} else {
				data->x = 0.0f;
				data->y = 0.0f;
				data->z = 0.0f;
				data->vx = 0.0f;
				data->vy = 0.0f;
				data->vz = 0.0f;
				data->field_0x58 = 0;
				data->life = 0;
			}
		}
		if (elemLife == 5 || elemLife == 6 || elemLife == 4) {
			data->vx += data->field_0x2C;
			data->vy += data->field_0x30;
			data->vz += data->field_0x34;
		}
		if (elemLife < 8) {
			data->scaleX += 8.0f * (0.03f * (3.0f - data->scaleX));
			data->scaleY += 6.0f * (0.03f * (0.5f - data->scaleY));
		}
		data->vx += data->field_0x38;
		data->vy += data->field_0x3C;
		data->vz += data->field_0x40;
		if (data->vy < 0.0f) {
			data->field_0x50 *= float_0p2_8041e9ac;
			data->vx *= float_0p225_8041e9b0;
			data->vy *= float_0p45_8041e9b4;
			data->vz *= float_0p225_8041e9b0;
			data->field_0x58 = (s32)((f32)data->field_0x58 * float_0p8_8041e9b8);
		}
		data->x += data->vx;
		data->y += data->vy;
		data->z += data->vz;
		data->rotZ += data->field_0x50;
	}

	if (life < 0) {
		effBlowParticle* work = effect->userdata;
		if (work->type == 1) {
			work->type = 2;
		}
		work->life = 10;
		work->field_0x60 = 0;
	}
	dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effBlowDisp, effect);
}

void effBlowDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    effBlowData* data;
    CameraEntry* cam;
    GXTexObj texObj;
    Mtx trans;
    Mtx scale;
    Mtx rot;
    Mtx work;
    Mtx base;
    s32 i;
    f32 deg2rad;

    cam = camGetPtr(camId);
    data = effect->userdata;

    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xA, 2, 8, 0xF);
    GXSetTevAlphaIn(0, 7, 1, 4, 7);
    effGetTexObjN64(0x34, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    PSMTXScale(scale, 0.03125f, 0.03125f, 0.0f);
    GXLoadTexMtxImm(scale, 0x1E, 1);
    GXSetCullMode(0);
    effSetVtxDescN64((effVtxArray*)size16x16_tex32x32_vtx);

    PSMTXTrans(base, data->x, data->y, data->z);
    deg2rad = 0.017453292f;
    for (i = 1, data++; i < effect->count; i++, data++) {
        blowSetColor(data->field_0x58);

        PSMTXTrans(trans, data->x, data->y, data->z);
        PSMTXRotRad(rot, 'y', deg2rad * -camGetPtr(CAMERA_3D)->viewYaw);
        PSMTXConcat(trans, rot, work);
        PSMTXScale(scale, data->scaleX, data->scaleY, data->scaleX);
        PSMTXConcat(work, scale, work);
        PSMTXRotRad(rot, 'z', deg2rad * data->rotZ);
        PSMTXConcat(work, rot, work);
        PSMTXConcat(base, work, work);
        PSMTXConcat(cam->view, work, work);

        GXLoadPosMtxImm(work, 0);
        GXSetCurrentMtx(0);
        GXBegin(0x90, 0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}
