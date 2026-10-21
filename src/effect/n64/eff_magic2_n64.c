#include "eff/n64/eff_magic2_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <math_ext.h>
#include <stdlib.h>

// .sbss2: the default .sbss2 is only used for compiler-generated pool templates, so pair it with a small-data section.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_804231b4;
__declspec(section ".sdata") GXColor unk_804231b0;


//.data
u32 stg_magic2_2_line_v[91] = {
	0xFFA90032, 0x00001555, 0x0800FFFF, 0xFF00FFA9, 0x0032F830, 0x15550000,
	0xFFFFFFFF, 0xFFCE0057, 0xF83012AB, 0x0000FFFF, 0xFFFFFFCE, 0x00570000,
	0x12AB0800, 0xFFFFFF00, 0xFF9C0000, 0x00001800, 0x0800FFFF, 0xFF00FF9C,
	0x0000F830, 0x18000000, 0xFFFFFFFF, 0x00000064, 0xF8301000, 0x0000FFFF,
	0xFFFF0000, 0x00640000, 0x10000800, 0xFFFFFF00, 0xFFA9FFCE, 0x00001AAB,
	0x0800FFFF, 0xFF00FFA9, 0xFFCEF830, 0x1AAB0000, 0xFFFFFFFF, 0xFFCEFFA9,
	0x00001D55, 0x0800FFFF, 0xFF00FFCE, 0xFFA9F830, 0x1D550000, 0xFFFFFFFF,
	0x0000FF9C, 0x00002000, 0x0800FFFF, 0xFF000000, 0xFF9CF830, 0x20000000,
	0xFFFFFFFF, 0x00320057, 0xF8300D55, 0x0000FFFF, 0xFFFF0032, 0x00570000,
	0x0D550800, 0xFFFFFF00, 0x0057FFCE, 0x00000555, 0x0800FFFF, 0xFF000057,
	0xFFCEF830, 0x05550000, 0xFFFFFFFF, 0x0032FFA9, 0xF83002AB, 0x0000FFFF,
	0xFFFF0032, 0xFFA90000, 0x02AB0800, 0xFFFFFF00, 0x0000FF9C, 0xF8300000,
	0x0000FFFF, 0xFFFF0000, 0xFF9C0000, 0x00000800, 0xFFFFFF00, 0x00640000,
	0x00000800, 0x0800FFFF, 0xFF000064, 0x0000F830, 0x08000000, 0xFFFFFFFF,
	0x00570032, 0x00000AAB, 0x0800FFFF, 0xFF000057, 0x0032F830, 0x0AAB0000,
	0xFFFFFFFF,
};
u32 stg_magic2_1_v[91] = {
	0xFFA90032, 0x00001555, 0x0200FFFF, 0xFFFFFFA9, 0x0032FFCE, 0x15550000,
	0xFFFFFFFF, 0xFFCE0057, 0xFFCE12AB, 0x0000FFFF, 0xFFFFFFCE, 0x00570000,
	0x12AB0200, 0xFFFFFFFF, 0xFF9C0000, 0x00001800, 0x0200FFFF, 0xFFFFFF9C,
	0x0000FFCE, 0x18000000, 0xFFFFFFFF, 0x00000064, 0xFFCE1000, 0x0000FFFF,
	0xFFFF0000, 0x00640000, 0x10000200, 0xFFFFFFFF, 0xFFA9FFCE, 0x00001AAB,
	0x0200FFFF, 0xFFFFFFA9, 0xFFCEFFCE, 0x1AAB0000, 0xFFFFFFFF, 0xFFCEFFA9,
	0x00001D55, 0x0200FFFF, 0xFFFFFFCE, 0xFFA9FFCE, 0x1D550000, 0xFFFFFFFF,
	0x0000FF9C, 0x00002000, 0x0200FFFF, 0xFFFF0000, 0xFF9CFFCE, 0x20000000,
	0xFFFFFFFF, 0x00320057, 0xFFCE0D55, 0x0000FFFF, 0xFFFF0032, 0x00570000,
	0x0D550200, 0xFFFFFFFF, 0x0057FFCE, 0x00000555, 0x0200FFFF, 0xFFFF0057,
	0xFFCEFFCE, 0x05550000, 0xFFFFFFFF, 0x0032FFA9, 0xFFCE02AB, 0x0000FFFF,
	0xFFFF0032, 0xFFA90000, 0x02AB0200, 0xFFFFFFFF, 0x0000FF9C, 0xFFCE0000,
	0x0000FFFF, 0xFFFF0000, 0xFF9C0000, 0x00000200, 0xFFFFFFFF, 0x00640000,
	0x00000800, 0x0200FFFF, 0xFFFF0064, 0x0000FFCE, 0x08000000, 0xFFFFFFFF,
	0x00570032, 0x00000AAB, 0x0200FFFF, 0xFFFF0057, 0x0032FFCE, 0x0AAB0000,
	0xFFFFFFFF,
};

//.sdata
static GXColor color_rotation_data[6] = {
    { 0xFF, 0xC5, 0x9C, 0xFF },
    { 0x9C, 0xFF, 0xFF, 0x73 },
    { 0xD6, 0xFF, 0x9C, 0xFF },
    { 0xFF, 0xC5, 0x7B, 0xFF },
    { 0xB4, 0xFF, 0xDE, 0x73 },
    { 0xFF, 0xFF, 0x94, 0x9C },
};

//.sbss2

// Local copy of system.c's inline helper so it inlines here (as in eff_rankup.c).
inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

inline Vec initializeVec2(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

EffectEntry* effMagic2N64Entry(s32 type, s32 a1, f32 x, f32 y, f32 z, f32 a2) {
	EffectEntry* effect;
	effMagic2Data* data;

	effect = effEntry();
	effect->type = "Magic2N64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effMagic2Data));
	effect->userdata = data;
	effect->callback = effMagic2Main;
	data->type = type;
	data->life = a1;
	data->timer = 0;
	data->alpha = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->rotSpeed = 0;
	data->rot = 0;
	data->rot2Speed = 0;
	data->rot2 = 0;
	data->uSpeed = 0;
	data->uOffset = 0;
	data->vSpeed = 0;
	data->vOffset = 0;
	data->vx = x;
	data->vy = y;
	data->vz = z;
	data->scale = 10.0f;
	data->scale3 = (f32)(rand() % 360);
	data->scale3Speed = 4.0f;
	data->scale2 = 0.0f;
	data->scale2Speed = 0.0f;
	if (type == 1) {
		data->vSpeed = 0x60;
		data->scaleMul = a2;
	} else {
		data->rotSpeed = -2;
		data->rot2Speed = -4;
		data->scaleMul = 0.2f * a2;
	}
	return effect;
}

void effMagic2Main(EffectEntry* effect) {
	effMagic2Data* data = effect->userdata;
	Vec pos;
	Vec v;
	s32 life;
	s32 i;
	f32 s;

	pos = initializeVec((Vec*)&data->x);
	data->life--;
	data->timer++;
	life = data->life;
	if (life < 0) {
		effDelete(effect);
		return;
	}
	if (data->timer <= 10) {
		data->alpha = data->timer * 0xFF / 10;
	}
	if (life < 6) {
		data->alpha = life * 0xFF / 6;
	}
	if (life < 10 && data->type == 0) {
		data->scale *= 0.7f;
	}
	if (data->type == 1) {
		s = (f32)sin(6.2831855f * (life * 10) / 360.0f);
		data->scale += 0.4f * (data->scaleMul * (0.1f * s) + data->scaleMul - data->scale);
	} else {
		s = (f32)sin(6.2831855f * (life * 10) / 360.0f);
		data->scale += 0.3f * (data->scaleMul * (0.1f * s) + data->scaleMul - data->scale);
	}
	v = initializeVec2((Vec*)&data->vx);
	PSMTXMultVec(camGetPtr(CAMERA_3D)->view, &v, &v);
	PSMTX44MultVec(camGetPtr(CAMERA_3D)->projection, &v, &v);
	data->x = 304.0f * v.x;
	data->y = 240.0f * v.y;
	data->z = 0.0f;
	for (i = 0; i < effect->count; i++, data++) {
		data->scale3 += data->scale3Speed;
		data->scale2 += data->scale2Speed;
		data->uOffset += data->uSpeed;
		data->vOffset += data->vSpeed;
		data->rot += data->rotSpeed;
		data->rot2 += data->rot2Speed;
		if (data->rot < 0) {
			data->rot += 0x100;
		}
		if (data->rot2 < 0) {
			data->rot2 += 0x100;
		}
	}
	dispEntry(CAMERA_3D, dispCalcZ(&pos), 2, effMagic2Disp, effect);
}

static inline GXColor magic2Color2(s32 alpha) {
	GXColor c = unk_804231b4;
	c.a = alpha / 2;
	return c;
}

static inline GXColor magic2Color1(GXColor c, u8* ptr, s32 alpha) {
	c.r = ptr[0];
	c.b = ptr[2];
	c.a = alpha;
	return c;
}

// The declaration order below is tuned so MWCC assigns the original's callee-saved registers
// (data=r29, i=r28, idx=r27, alpha=r26, type=r25, rot=r24, vOffset=r23, rot2=r22,
// effect=r21, uOffset=r20). The four uOffset/rot/vOffset/rot2 loads sit after the
// GXSetTevColor calls, as in the original.
void effMagic2Disp(CameraId camId, void* param) {
	effMagic2Data* data;
	s32 i;
	s32 idx;
	s32 alpha;
	s32 type;
	s32 rot;
	s32 vOffset;
	s32 rot2;
	EffectEntry* effect;
	s32 uOffset;
	Mtx mtxD;
	Mtx mtxE;
	Mtx44 mtxC;
	Mtx44 mtxA;
	Mtx44 mtxB;
	f32 proj[7];
	f32 viewport[6];

	effect = param;
	camGetPtr(camId);
	data = effect->userdata;
	type = data->type;
	GXGetProjectionv(proj);
	GXGetViewportv(viewport);
	PSMTX44Trans(mtxA, data->x, data->y, data->z);
	PSMTX44Scale(mtxB, data->scale, data->scale, 1.0f);
	PSMTX44Concat(mtxA, mtxB, mtxA);
	C_MTXPerspective(mtxC, (f32)(type == 1 ? 130 : 30), 1.2666667f, 1.0f, 32768.0f);
	PSMTX44Concat(mtxC, mtxA, mtxC);
	GXSetProjection(mtxC, 0);
	switch (type) {
	case 1:
		PSMTXTrans(mtxD, 0.0f, 0.0f, -70.0f);
		break;
	case 0:
		PSMTXTrans(mtxD, 0.0f, 0.0f, -80.0f);
		break;
	}
	GXLoadPosMtxImm(mtxD, 0);
	GXSetCurrentMtx(0);
	alpha = data->alpha;
	idx = data->timer * 3;
	for (i = 0; i < effect->count; i++, data++) {
		u8* ptr;
		GXColor color2;

		idx += 30;
		idx %= 24;
		ptr = (u8*)color_rotation_data + idx;
		GXSetTevColor(1, magic2Color1(unk_804231b0, ptr, alpha));
		color2 = magic2Color2(alpha);
		GXSetTevColor(2, color2);
		uOffset = data->uOffset;
		rot = data->rot;
		vOffset = data->vOffset;
		rot2 = data->rot2;
		if (type == 0) {
			PSMTXScale(mtxE, 0.015625f, 0.015625f, 1.0f);
			PSMTXTrans(mtxD, (f32)(uOffset / 4), (f32)(rot / 4), 0.0f);
			PSMTXConcat(mtxE, mtxD, mtxD);
			GXLoadTexMtxImm(mtxD, 0x1E, 1);
			PSMTXScale(mtxE, 0.015625f, 0.015625f, 1.0f);
			PSMTXTrans(mtxD, (f32)(vOffset / 4), (f32)(rot2 / 4), 0.0f);
			PSMTXConcat(mtxE, mtxD, mtxD);
			GXLoadTexMtxImm(mtxD, 0x21, 1);
		} else {
			PSMTXScale(mtxE, 0.015625f, 0.0625f, 0.0f);
			PSMTXTrans(mtxD, (f32)(uOffset / 4), (f32)(rot / 4), 0.0f);
			PSMTXConcat(mtxE, mtxD, mtxD);
			GXLoadTexMtxImm(mtxD, 0x1E, 1);
			PSMTXScale(mtxE, 0.015625f, 0.0625f, 1.0f);
			PSMTXTrans(mtxD, (f32)(vOffset / 4), (f32)(rot2 / 4), 0.0f);
			PSMTXConcat(mtxE, mtxD, mtxD);
			GXLoadTexMtxImm(mtxD, 0x21, 1);
		}
		switch (type) {
		case 0:
			magic2_2_line_dl();
			break;
		case 1:
			magic2_4_dl();
			break;
		}
	}
	GXSetProjectionv(proj);
	GXSetScissorBoxOffset(0, 0);
}

void magic2_2_line_dl(void) {
	GXTexObj texObj;

	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
	GXSetTexCoordGen2(1, 1, 4, 0x21, 0, 0x7D);
	effGetTexObjN64(0x11, &texObj);
	GXLoadTexObj(&texObj, 0);
	GXLoadTexObj(&texObj, 1);
	GXSetNumTevStages(3);
	GXSetTevOrder(0, 0, 0, 0xFF);
	GXSetTevColorOp(0, 1, 0, 0, 1, 3);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xF, 4, 8, 0xC);
	GXSetTevAlphaIn(0, 7, 2, 4, 7);
	GXSetTevOrder(1, 1, 1, 4);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 0xF, 2, 8, 2);
	GXSetTevAlphaIn(1, 7, 0, 5, 7);
	GXSetTevOrder(2, 1, 1, 4);
	GXSetTevColorOp(2, 1, 0, 0, 1, 0);
	GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
	GXSetTevColorIn(2, 0xF, 8, 6, 0);
	GXSetTevAlphaIn(2, 7, 4, 5, 0);
	GXSetCullMode(0);
	effSetVtxDescN64((effVtxArray*)stg_magic2_2_line_v);
	GXBegin(0x90, 0, 0x48);
	tri2(0, 1, 2, 0, 0, 2, 3, 0);
	tri2(4, 5, 1, 0, 4, 1, 0, 0);
	tri2(3, 2, 6, 0, 3, 6, 7, 0);
	tri2(8, 9, 5, 0, 8, 5, 4, 0);
	tri2(10, 11, 9, 0, 10, 9, 8, 0);
	tri2(12, 13, 11, 0, 12, 11, 10, 0);
	tri2(7, 6, 14, 0, 7, 14, 15, 0);
	tri2(16, 17, 18, 0, 16, 18, 19, 0);
	tri2(19, 18, 20, 0, 19, 20, 21, 0);
	tri2(22, 23, 17, 0, 22, 17, 16, 0);
	tri2(24, 25, 23, 0, 24, 23, 22, 0);
	tri2(15, 14, 25, 0, 15, 25, 24, 0);
}

void magic2_4_dl(void) {
	GXTexObj texObj;

	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
	GXSetTexCoordGen2(1, 1, 4, 0x21, 0, 0x7D);
	effGetTexObjN64(0x12, &texObj);
	GXLoadTexObj(&texObj, 0);
	effGetTexObjN64(0x13, &texObj);
	GXLoadTexObj(&texObj, 1);
	GXSetNumTevStages(3);
	GXSetTevOrder(0, 0, 0, 0xFF);
	GXSetTevColorOp(0, 1, 0, 0, 1, 3);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xF, 4, 8, 0xC);
	GXSetTevAlphaIn(0, 7, 2, 4, 7);
	GXSetTevOrder(1, 1, 1, 0xFF);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 0xF, 2, 8, 2);
	GXSetTevAlphaIn(1, 7, 0, 1, 7);
	GXSetTevOrder(2, 1, 1, 0xFF);
	GXSetTevColorOp(2, 1, 0, 0, 1, 0);
	GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
	GXSetTevColorIn(2, 0xF, 8, 6, 0);
	GXSetTevAlphaIn(2, 7, 4, 1, 0);
	GXSetCullMode(0);
	effSetVtxDescN64((effVtxArray*)stg_magic2_1_v);
	GXBegin(0x90, 0, 0x48);
	tri2(0, 1, 2, 0, 0, 2, 3, 0);
	tri2(4, 5, 1, 0, 4, 1, 0, 0);
	tri2(3, 2, 6, 0, 3, 6, 7, 0);
	tri2(8, 9, 5, 0, 8, 5, 4, 0);
	tri2(10, 11, 9, 0, 10, 9, 8, 0);
	tri2(12, 13, 11, 0, 12, 11, 10, 0);
	tri2(7, 6, 14, 0, 7, 14, 15, 0);
	tri2(16, 17, 18, 0, 16, 18, 19, 0);
	tri2(19, 18, 20, 0, 19, 20, 21, 0);
	tri2(22, 23, 17, 0, 22, 17, 16, 0);
	tri2(24, 25, 23, 0, 24, 23, 22, 0);
	tri2(15, 14, 25, 0, 15, 25, 24, 0);
}
