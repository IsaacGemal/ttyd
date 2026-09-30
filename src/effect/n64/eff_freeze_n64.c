#include "eff/n64/eff_freeze_n64.h"
#include "eff/n64/eff_snow_dust_n64.h"
#include "eff/eff_n64.h"
#include "driver/effdrv.h"
#include "driver/camdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <dolphin/gx.h>

// .sdata2 constants shared with the rest of the unit.
extern const f32 float_10_8041f010;
extern const f32 float_32_8041f030;
extern const f32 float_30_8041f034;

// .rodata
const char str_802FA6C0[] = "FreezeN64";

EffectEntry* effFreezeN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry;
	effFreezeData* data;

	entry = effEntry();
	entry->type = str_802FA6C0;
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effFreezeData));
	entry->userdata = data;
	entry->callback = effFreezeMain;
	entry->flags |= 2;

	data->type = type;
	data->counter = 0;
	if (life <= 0) {
		data->life = 1000;
	}
	else {
		data->life = life;
	}
	data->color0[3] = 0xff;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->color0[0] = 0xff;
	data->color0[1] = 0xff;
	data->color0[2] = 0xff;
	data->color1[0] = 0xf0;
	data->color1[1] = 0xff;
	data->color1[2] = 0xff;
	data->color1[3] = 0xff;
	data->field_0x25 = 0;
	data->field_0x24 = 0;
	data->snowDust = effSnowDustN64Entry(1, x, float_10_8041f010 + y, z, 4, 0, float_32_8041f030, float_30_8041f034, scale);
	return entry;
}

//.sbss2
GXColor unk_80423118;
GXColor unk_8042311c;

//.data
static u32 size32x64_tex32x64_vtx[56] = {
	0xFFF00000, 0x00030000, 0x00000000, 0x00FF0010,
	0x00000003, 0x04000000, 0x000000FF, 0x00100040,
	0x00030400, 0x08000000, 0x00FFFFF0, 0x00400003,
	0x00000800, 0x000000FF, 0xFFF00000, 0x00030000,
	0x08000000, 0x00FF0010, 0x00000003, 0x04000800,
	0x000000FF, 0x00100040, 0x00030400, 0x10000000,
	0x00FFFFF0, 0x00400003, 0x00001000, 0x000000FF,
	0xFFF00000, 0x00030000, 0x10000000, 0x00FF0010,
	0x00000003, 0x04001000, 0x000000FF, 0x00100040,
	0x00030400, 0x18000000, 0x00FFFFF0, 0x00400003,
	0x00001800, 0x000000FF, 0xFFF00000, 0x00030000,
	0x18000000, 0x00FF0010, 0x00000003, 0x04001800,
	0x000000FF, 0x00100040, 0x00030400, 0x20000000,
	0x00FFFFF0, 0x00400003, 0x00002000, 0x000000FF,
};

static inline void effFreezeSetColor1(u8* color, u8 alpha) {
	GXColor c = unk_80423118;
	c.r = color[0];
	c.g = color[1];
	c.b = color[2];
	c.a = alpha;
	GXSetTevColor(1, c);
}

static inline void effFreezeSetColor2(u8* color) {
	GXColor c = unk_8042311c;
	c.r = color[0];
	c.g = color[1];
	c.b = color[2];
	c.a = color[3];
	GXSetTevColor(2, c);
}

void effFreezeDisp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	effFreezeData* data;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx mtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	u8 alpha;

	cam = camGetPtr(cameraId);
	data = effect->userdata;
	alpha = data->color0[3];
	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXRotRad(rotMtx, 'y', -camGetPtr(CAMERA_3D)->viewYaw * 0.017453292f);
	PSMTXScale(scaleMtx, data->scale, data->scale, data->scale);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXConcat(mtx, scaleMtx, mtx);
	PSMTXConcat(cam->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, 0);
	GXSetCurrentMtx(0);
	effFreezeSetColor1(data->color0, alpha);
	effFreezeSetColor2(data->color1);
	GXSetNumChans(0);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 0xff);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 4, 2, 8, 0xf);
	GXSetTevAlphaIn(0, 7, 1, 4, 7);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	PSMTXScale(scaleMtx, 0.03125f, 0.00390625f, 0.0f);
	GXLoadTexMtxImm(scaleMtx, 0x1e, 1);
	effGetTexObjN64(0x8a, &texObj);
	GXLoadTexObj(&texObj, 0);
	GXSetCullMode(0);
	effSetVtxDescN64(&((effVtxArray*)size32x64_tex32x64_vtx)[(s8)data->field_0x24]);
	GXBegin(0x90, 0, 6);
	tri2(0, 1, 2, 0, 0, 2, 3, 0);
}
