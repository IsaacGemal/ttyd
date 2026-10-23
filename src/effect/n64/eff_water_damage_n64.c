#include "eff/n64/eff_water_damage_n64.h"
#include "mario/mario.h"
#include "memory.h"
#include "system.h"
#include <math.h>
#include <stdlib.h>
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "mario/mariost.h"
#include <dolphin/mtx.h>

typedef struct effWaterDamageData {
	s32 type;        //0x0
	Vec position;    //0x4
	Vec velocity;    //0x10
	s32 life;        //0x1C
	s32 timer;       //0x20
	s32 colorR;      //0x24
	s32 colorG;      //0x28
	s32 colorB;      //0x2C
	s32 alpha;       //0x30
	f32 scale;       //0x34
	s32 isBig;       //0x38
} effWaterDamageData; //0x3C

//data (matches the original unit's .data layout)
u32 negone_one_data[16] = {
	0xBF800000, 0x3F800000, 0xBF800000, 0x3F800000, 0x3F800000, 0xBF800000, 0x3F800000, 0xBF800000,
	0xBF800000, 0x3F800000, 0xBF800000, 0x3F800000, 0x3F800000, 0xBF800000, 0x3F800000, 0xBF800000
};
u32 size8x8_tex16x16_0_vtx[14] = {
	0xFFFCFFFC, 0x00000000, 0x00000000, 0x00FF0004, 0xFFFC0000, 0x02000000, 0x000000FF,
	0x00040004, 0x00000200, 0x02000000, 0x00FFFFFC, 0x00040000, 0x00000200, 0x000000FF
};
u32 size8x8_tex16x16_1_vtx[14] = {
	0xFFFBFFFD, 0x00000000, 0x00000000, 0x00FF0005, 0xFFFD0000, 0x02000000, 0x000000FF,
	0x00050003, 0x00000200, 0x02000000, 0x00FFFFFB, 0x00030000, 0x00000200, 0x000000FF
};
u32 size8x8_tex16x16_2_vtx[14] = {
	0xFFFDFFFB, 0x00000000, 0x00000000, 0x00FF0003, 0xFFFB0000, 0x02000000, 0x000000FF,
	0x00030005, 0x00000200, 0x02000000, 0x00FFFFFD, 0x00050000, 0x00000200, 0x000000FF
};
__declspec(section ".data") u32 gap_803a89e8[6] = { 0, 0, 0, 0, 0, 0 };
u32 unk_803a8a00[0x20] = {
	0x78A31427, 0x572B0B52, 0x5167133A, 0x7FDF7F2F, 0x653E32A7, 0xF5FFFF8B, 0x32670486, 0x8BFFD55E,
	0x60830B26, 0xF8E8FA55, 0x602011C5, 0x2E020278, 0x59A40486, 0xF7155555, 0x90200486, 0xBC555555,
	0xE77F1BFF, 0x6258577A, 0xAE9A0C4F, 0xF5B5D522, 0xF7BF241F, 0x78002AFF, 0xF7DF1BFF, 0x022AFFDD,
	0x8D7A0A92, 0xF5F5F5EC, 0xADFA098B, 0xF555FD25, 0xE77E135A, 0xA0035555, 0xADFA09CE, 0x25D5F5F5
};

EffectEntry* effWaterDamageN64Entry(s32 type, s32 time, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effWaterDamageData* data;
	s32 isBig;
	s32 count;
	s32 i;

	effect = effEntry();
	isBig = 0;
	if (type >= 10) {
		isBig = 1;
		type %= 10;
	}
	switch (type) {
	case 0:
	case 1:
		count = 0x15;
		break;
	case 2:
		count = 0x29;
		break;
	case 6:
		count = 9;
		break;
	default:
		count = 0xB;
		break;
	}
	effect->type = "WaterDamageN64";
	effect->count = count;
	data = __memAlloc(HEAP_EFFECT, count * sizeof(effWaterDamageData));
	effect->userdata = data;
	effect->callback = effWaterDamageMain;
	effect->flags |= 2;
	data->type = type;
	data->timer = 0;
	if (time <= 0) {
		data->life = 1000;
	}
	else {
		data->life = time;
	}
	data->alpha = 255;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->scale = scale;
	data->colorR = 70;
	data->colorG = 180;
	data->colorB = 255;
	data->isBig = isBig;

	for (i = 1, data++; i < effect->count; i++, data++) {
		data->position.x = 0.0f;
		data->position.y = 0.0f;
		data->position.z = 0.0f;
		data->scale = 2.0f;
		switch (type) {
		case 0:
			data->velocity.x = (f32)(rand() % 11 - 2);
			data->velocity.y = (f32)(rand() % 11 - 5);
			data->velocity.z = (f32)(rand() % 11 - 5);
			break;
		case 1:
			data->velocity.x = (f32)(-(rand() % 11 - 2));
			data->velocity.y = (f32)(rand() % 11 - 5);
			data->velocity.z = (f32)(rand() % 11 - 5);
			break;
		case 2:
			data->velocity.x = (f32)(rand() % 100 - 50) * 0.07f;
			data->velocity.y = (f32)(rand() % 100 + 10) * 0.2f;
			data->velocity.z = (f32)(rand() % 100 - 50) * 0.07f;
			break;
		case 3:
			data->velocity.x = (f32)(rand() % 10 - 5) * 0.5f;
			data->velocity.y = (f32)(rand() % 10 - 5) * 0.5f;
			data->velocity.z = (f32)(rand() % 10 - 5) * 0.5f;
			data->scale = 0.4f;
			data->life = i;
			break;
		case 4:
			data->velocity.x = (f32)(rand() % 7 - 3);
			data->velocity.y = (f32)(rand() % 3 + 1);
			data->velocity.z = (f32)(rand() % 7 - 3);
			break;
		case 5:
			data->velocity.x = (f32)(rand() % 10 - 5) * 0.3f;
			data->velocity.y = (f32)(rand() % 10 + 5) * 0.3f;
			data->velocity.z = (f32)(rand() % 10 - 5) * 0.3f;
			data->scale = 1.0f;
			data->life = i;
			break;
		case 6:
		{
			f32 base = 6.2831855f * (f32)i;

			data->position.x = 10.0f * (f32)sin(base / (f32)effect->count);
			data->position.z = 10.0f * (f32)cos(base / (f32)effect->count);
			data->velocity.x = data->position.x / 12.0f;
			data->velocity.y = (f32)(rand() % 5) * 0.3f;
			data->velocity.z = data->position.z / 12.0f;
			data->scale = 1.0f;
			data->life = i;
			break;
		}
		case 7:
		default:
		{
			MarioWork* mario = marioGetPtr();
			f32 randAngle = (f32)(irand(90) - 45);

			data->position.x = (f32)sin(6.2831855f * (randAngle + (-mario->directionView + camGetPtr(CAMERA_3D)->viewYaw)) / 360.0f);
			data->position.z = (f32)cos(6.2831855f * (randAngle + (-mario->directionView + camGetPtr(CAMERA_3D)->viewYaw)) / 360.0f);
			data->velocity.x = mario->baseSpeed * data->position.x / 2.0f;
			data->velocity.y = (f32)(rand() % 10 + 5) * 0.1f;
			data->velocity.z = mario->baseSpeed * data->position.z / 2.0f;
			data->scale = 1.0f;
			data->life = 3 * i;
			break;
		}
		}
		data->alpha = 255;
	}
	return effect;
}

void effWaterDamageMain(EffectEntry* effect) {
	effWaterDamageData* data = effect->userdata;
	Vec disp;
	s32 i;
	s32 type;
	s32 isBig;

	disp = initializeVec(&data->position);
	type = data->type;
	isBig = data->isBig;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->life = 0x10;
	}
	if (data->life < 0x3E8) {
		data->life--;
	}
	data->timer++;
	if (data->life < 0) {
		effDelete(effect);
		return;
	}
	if (data->life < 4) {
		data->alpha = data->life << 6;
	}

	for (i = 1, data++; i < effect->count; i++, data++) {
		data->position.x += data->velocity.x;
		data->position.y += data->velocity.y;
		data->position.z += data->velocity.z;
		data->alpha *= 0.98f;
		if (type == 2) {
			data->velocity.y += -0.8f;
		}
		data->velocity.y += -0.1f;
		data->velocity.x *= 0.98f;
		data->velocity.y *= 0.98f;
		data->velocity.z *= 0.98f;
		if (type == 3) {
			data->scale *= 1.2f;
		}
		else {
			data->scale *= 0.9f;
		}
	}
	if (isBig == 0) {
		dispEntry(CAMERA_3D, dispCalcZ(&disp), 8, effWaterDamageDisp, effect);
	}
	else {
		dispEntry(CAMERA_3D, dispCalcZ(&disp), 8, effWaterDamageDisp2, effect);
	}
}

//local helpers
static inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };

	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}


// Calling GXSetTevColor inside each inline places the colour build word and its by-value argument
// copy in the original's slots (color 0xc/0x10, color2 0x8).
static inline void effWaterDamageSetColor0(s32 alpha) {
	GXColor c = { 0xC8, 0xFF, 0xFF, 0x00 };
	c.a = alpha;
	GXSetTevColor(GX_TEVREG0, c);
}

static inline void effWaterDamageSetColor1(void) {
	GXColor c = { 0x00, 0x20, 0x20, 0x00 };
	GXSetTevColor(GX_TEVREG1, c);
}

// The loop's initial data++ is hoisted to just before GXInitTexObj, which matches the original's schedule.
void effWaterDamageDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	CameraEntry* cam = camGetPtr(camId);
	effWaterDamageData* data = effect->userdata;
	Mtx mtx;
	Mtx scale;
	Mtx rot;
	Mtx viewMtx;
	GXTexObj texObj;
	s32 i;
	s32 alpha;
	s32 x;
	s32 y;

	x = (s32)data->position.x;
	y = (s32)data->position.y;
	alpha = data->alpha;

	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);

	PSMTXTrans(mtx, (f32)x, (f32)y, data->position.z);
	PSMTXRotRad(rot, 'y', MTXDegToRad(-camGetPtr(CAMERA_3D)->viewYaw));
	PSMTXScale(scale, data->scale, data->scale, data->scale);
	PSMTXConcat(mtx, scale, mtx);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXConcat(cam->view, mtx, viewMtx);

	GXSetNumChans(0);
	GXSetNumTevStages(3);

	GXSetTevOrder(0, 0, 0, 255);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 15, 15, 15, 8);
	GXSetTevAlphaIn(0, 7, 7, 7, 4);

	GXSetTevOrder(1, 1, 1, 255);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 15, 2, 8, 4);
	GXSetTevAlphaIn(1, 7, 1, 0, 7);

	GXSetTevOrder(2, 0, 0, 255);
	GXSetTevColorOp(2, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
	GXSetTevColorIn(2, 0, 12, 8, 15);
	GXSetTevAlphaIn(2, 7, 7, 7, 0);

	GXSetNumTexGens(2);
	GXSetTexCoordGen2(0, 1, 4, 30, 0, 125);
	GXSetTexCoordGen2(1, 1, 4, 33, 0, 125);
	PSMTXScale(scale, 0.0625f, 0.0625f, 0.0f);
	GXLoadTexMtxImm(scale, 30, 1);
	effGetTexObjN64(0x89, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetCullMode(GX_CULL_NONE);
	data++;
	GXInitTexObj(&texObj, unk_803a8a00, 0x10, 0x10, 0xE, 0, 0, 0);
	GXInitTexObjLOD(&texObj, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);
	GXLoadTexObj(&texObj, GX_TEXMAP1);

	for (i = 1; i < effect->count; i++, data++) {
		s32 kind;

		PSMTXScale(scale, 0.0625f, 0.0625f, 0.0f);
		GXLoadTexMtxImm(scale, 33, 1);
		PSMTXTrans(mtx, data->position.x, data->position.y, data->position.z);
		PSMTXScale(scale, data->scale, data->scale, data->scale);
		PSMTXConcat(mtx, scale, mtx);
		PSMTXConcat(viewMtx, mtx, mtx);
		GXLoadPosMtxImm(mtx, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);

		effWaterDamageSetColor0(alpha * data->alpha / 255);
		effWaterDamageSetColor1();

		kind = (i + (gp->retraceCount >> 1)) & 3;
		switch (kind) {
		case 0:
		case 2:
			effSetVtxDescN64(size8x8_tex16x16_0_vtx);
			break;
		case 1:
			effSetVtxDescN64(size8x8_tex16x16_1_vtx);
			break;
		case 3:
			effSetVtxDescN64(size8x8_tex16x16_2_vtx);
			break;
		}
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
	}
}


// The original builds the material colour from a pooled GXColor and only overwrites .a; MWCC keeps the
// colour in the 0x8 stack slot and the outgoing copy at 0xc (the reverse of a plain assignment here).
static inline GXColor effWaterDamageColor(s32 alpha) {
	GXColor color = (GXColor){ 0xC8, 0xFF, 0xFF, 0x00 };
	color.a = alpha;
	return color;
}

// Matches: the original reuses cam's callee-saved register for the loop index, which needs the index
// declared before the value temporaries here.
void effWaterDamageDisp2(CameraId camId, void* param) {
	EffectEntry* effect = param;
	CameraEntry* cam = camGetPtr(camId);
	effWaterDamageData* data = effect->userdata;
	Mtx mtx;
	Mtx scale;
	Mtx rot;
	Mtx viewMtx;
	GXTexObj texObj;
	GXColor color;
	s32 i;
	s32 alpha;
	s32 x;
	s32 y;

	x = (s32)data->position.x;
	y = (s32)data->position.y;
	alpha = data->alpha;

	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);

	PSMTXTrans(mtx, (f32)x, (f32)y, data->position.z);
	PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXScale(scale, data->scale, data->scale, data->scale);
	PSMTXConcat(mtx, scale, mtx);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXConcat(cam->view, mtx, viewMtx);

	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);

	color = effWaterDamageColor(alpha * data->alpha / 255);
	GXSetChanMatColor(4, color);

	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 4);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 8, 15, 15, 10);
	GXSetTevAlphaIn(0, 7, 5, 4, 7);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 30, 0, 125);
	PSMTXScale(scale, 0.0625f, 0.0625f, 0.0f);
	GXLoadTexMtxImm(scale, 30, 1);
	effGetTexObjN64(0x89, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetCullMode(GX_CULL_NONE);
	GXSetCurrentMtx(GX_PNMTX0);

	for (i = 1, data++; i < effect->count; i++, data++) {
		s32 kind;

		PSMTXTrans(mtx, data->position.x, data->position.y, data->position.z);
		PSMTXScale(scale, data->scale, data->scale, data->scale);
		PSMTXConcat(mtx, scale, mtx);
		PSMTXConcat(viewMtx, mtx, mtx);
		GXLoadPosMtxImm(mtx, GX_PNMTX0);
		kind = (i + (gp->retraceCount >> 1)) & 3;
		switch (kind) {
		case 0:
		case 2:
			effSetVtxDescN64(size8x8_tex16x16_0_vtx);
			break;
		case 1:
			effSetVtxDescN64(size8x8_tex16x16_1_vtx);
			break;
		case 3:
			effSetVtxDescN64(size8x8_tex16x16_2_vtx);
			break;
		}
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
	}
}
