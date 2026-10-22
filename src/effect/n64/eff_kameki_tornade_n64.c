#include "eff/n64/eff_kameki_tornade_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <stdlib.h>
#include "eff/eff_n64.h"
#include "matching.h"
#include "math_ext.h"
#include <dolphin/os/OSCache.h>
#include <dolphin/os/OSFastCast.h>
#include <string.h>

EffectEntry* effKamekiTornadeN64Entry(s32 a, s32 b, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effKamekiTornadeData* data;
	s32 i;

	effect = effEntry();
#ifdef MUST_MATCH
	/* MUST_MATCH: retail's 20-byte string object is followed by a 4-byte section-tail gap; the
	   padded 24-byte literal leaves the same trailing zero words. */
	effect->type = "KamekiTornadeN64\0\0\0\0\0\0\0";
#else
	effect->type = "KamekiTornadeN64";
#endif
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effKamekiTornadeData));
	effect->userdata = data;
	effect->callback = effKamekiTornadeMain;
	effect->flags |= 2;
	data->type = a;
	data->timer = 0;
	if (b <= 0) {
		data->life = 1000;
	} else {
		data->life = b;
	}
	data->alpha = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->colorR = 0xFF;
	data->colorG = 0xFF;
	data->colorB = 0xFF;
	data->color2R = 0xFF;
	data->color2G = 0xFF;
	data->color2B = 0xEB;
	for (i = 0; i < 8; i++) {
		data->nodeX[i] = x;
		data->nodeY[i] = y;
		data->nodeZ[i] = z;
		data->nodeScale[i] = 0.2f * i + 1.0f;
		data->nodeAngle[i] = rand() % 360;
		data->nodeSpin[i] = 0.1f * (rand() % 100) + 2.0f;
		data->nodeRadius[i] = 0.1f * (rand() % 30);
		data->nodeTilt[i] = 0.0f;
	}
	data->spinAngle = 0.0f;
	data->baseRotZ = 0.0f;
	data->texScroll0 = 0.0f;
	data->texScroll1 = 0.0f;
	data->alphaScale = 1.0f;
	return effect;
}

void effKamekiTornadeMain(EffectEntry* effect) {
	effKamekiTornadeData* data;
	Vec position;
	Vec disp = { 0 };
	s32 i;
	s32 timer;
	f32 scale;
	f32 f5;

	data = effect->userdata;
	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
	position = disp;
	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->life = 0x10;
	}
	if (data->life < 1000) {
		data->life--;
	}
	data->timer++;
	if (data->life < 0) {
		effDelete(effect);
		return;
	}
	timer = data->timer;
	scale = data->scale;
	if (data->life < 0x10) {
		data->alpha = data->life << 4;
	}
	if (timer < 0x10) {
		data->alpha = (timer << 4) + 0xF;
	}
	data->nodeX[0] = data->x;
	data->nodeY[0] = data->y;
	data->nodeZ[0] = data->z;
	f5 = 5.0f * scale;
	for (i = 7; i != 0; i--) {
		data->nodeTilt[i] = 4.0f * (data->nodeX[i - 1] - data->nodeX[i]);
		data->nodeX[i] = data->nodeX[i - 1];
		data->nodeY[i] = data->nodeY[i - 1] + f5;
		data->nodeZ[i] = data->nodeZ[i - 1];
		if (data->nodeTilt[i] > 90.f) {
			data->nodeTilt[i] = 90.f;
		}
		else if (data->nodeTilt[i] < -90.f) {
			data->nodeTilt[i] = -90.f;
		}
	}
	data->spinAngle += 10.f;
	data->texScroll0 += 12.959999f;
	data->texScroll1 += 17.28f;
	if (data->texScroll0 > 64.0f) {
		data->texScroll0 -= 64.0f;
	}
	if (data->texScroll1 > 64.0f) {
		data->texScroll1 -= 64.0f;
	}
	for (i = 0; i < 8; i++) {
		data->nodeAngle[i] += data->nodeSpin[i];
	}
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effKamekiTornadeDisp, effect);
}
// One 14-byte vertex: copy the source, then rotate its s16 position through the matrix with the
// SDK fast-cast inlines. Retail inlines this once per vertex, each with its own Vec temp and its
// own GQR2-5 setup (same shape as eff_flame_n64/eff_sstar_n64).
static inline void kamekiTornadeSetVtx(KamekiTornadeVtx* dst, KamekiTornadeVtx* src, Mtx mtx) {
	Vec v;
	memcpy(dst, src, sizeof(KamekiTornadeVtx));
	OSInitFastCast();
	OSs16tof32(&dst->pos[0], &v.x);
	OSs16tof32(&dst->pos[1], &v.y);
	OSs16tof32(&dst->pos[2], &v.z);
	PSMTXMultVec(mtx, &v, &v);
	OSInitFastCast();
	OSf32tos16(&v.x, &dst->pos[0]);
	OSf32tos16(&v.y, &dst->pos[1]);
	OSf32tos16(&v.z, &dst->pos[2]);
}
// The float constants are literals the compiler pools in first-use order. Declaring f19/angle/scale in
// this order, and sharing one set of theta/s/rad/c locals between the two ring halves, gives the
// original's FPR colouring.
void effKamekiTornadeDisp(CameraId camId, void* param) {
	EffectEntry* effect;
	CameraEntry* cam;
	effKamekiTornadeData* data;
	s32 life;
	KamekiTornadeVtx* tbl;
	KamekiTornadeVtx* vtx;
	GXTexObj texObj;
	Mtx work;
	Mtx scaleMtx;
	Mtx rotMtx;
	Mtx baseMtx;
	Vec axis;
	f32 theta;
	f32 s;
	f32 rad;
	f32 c;
	f32 f19;
	f32 angle;
	f32 scale;
	s32 i;
	s32 j;
	s32 field24;

	effect = param;
	cam = camGetPtr(camId);
	data = effect->userdata;
	field24 = data->alpha >> 1;
	life = data->life;
	scale = data->scale;

	GXSetNumChans(0);
	GXSetNumTevStages(3);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_TEXC, GX_CC_CPREV, GX_CC_CPREV, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_TEXA, GX_CA_ZERO);
	GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_C0, GX_CC_C1, GX_CC_CPREV, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);
	effGetTexObjN64(0x6C, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXLoadTexObj(&texObj, GX_TEXMAP1);

	GXSetTevColor(GX_TEVREG0, (GXColor){ data->colorR, data->colorG, data->colorB,
	                                     (u8)(s32)((f32)field24 * data->alphaScale) });

	GXSetTevColor(GX_TEVREG1, (GXColor){ data->color2R, data->color2G, data->color2B, 0x20 });

	GXSetCullMode(GX_CULL_NONE);

	tbl = smartAlloc(0x1C0, 3)->address;

	{
		KamekiTornadeVtx* q;
		s32 s;

		for (i = 0, s = 0; i < 16; i++, s += 0x80) {
			KamekiTornadeVtx* p = &tbl[i];

			angle = 6.2831855f * (f32)(i * 0x168 / 0xF) / 360.f;

			p->pos[0] = (s16)((f32)cos(angle) * 100.f);
			p->pos[1] = 0;
			p->pos[2] = (s16)((f32)sin(angle) * 100.f);
			p->tex[0] = s;
			p->tex[1] = 0;
			{
				q = &tbl[i + 16];

				q->pos[0] = (s16)((f32)cos(angle) * 100.f);
				q->pos[1] = 0;
				q->pos[2] = (s16)((f32)sin(angle) * 100.f);
				q->tex[0] = (i * 2) << 8;
				q->tex[1] = 0x400;
			}
		}
	}

	PSMTXRotRad(rotMtx, 'z', 0.017453292f * data->baseRotZ);
	PSMTXScale(baseMtx, 0.1f, 0.1f, 0.1f);
	PSMTXConcat(rotMtx, baseMtx, baseMtx);

	f19 = 0.02f * (f32)(0x7F - field24);

	for (i = 0; i < 7; i++) {
		vtx = smartAlloc(0x1C0, 3)->address;
		effSetVtxDescN64(vtx);

		/* first half: the 0..15 ring */
		PSMTXTrans(work, 10.f * data->nodeX[i], 10.f * data->nodeY[i],
		           10.f * data->nodeZ[i]);
		PSMTXRotRad(rotMtx, 'z', 0.017453292f * data->nodeTilt[i]);
		PSMTXConcat(work, rotMtx, work);
		axis = (Vec){ -0.03f, 1.0f, 0.1f };
		PSMTXRotAxisRad(rotMtx, &axis, 0.017453292f * (data->spinAngle + (f32)(i * i)));
		PSMTXConcat(work, rotMtx, work);
		{
			f32 zScale = data->nodeScale[i] * scale;

			if (life < 0x10) {
				zScale += f19;
			}
			PSMTXScale(scaleMtx, zScale, scale, zScale);
		}
		PSMTXConcat(work, scaleMtx, work);
		rad = 10.f * data->nodeRadius[i];
		theta = 6.2831855f * data->nodeAngle[i] / 360.f;
		s = (f32)sin(theta);
		c = (f32)cos(theta);

		PSMTXTrans(scaleMtx, rad * s, 0.0f, rad * c);
		PSMTXConcat(work, scaleMtx, work);
		PSMTXConcat(baseMtx, work, work);
		for (j = 0; j < 16; j++) {
			kamekiTornadeSetVtx(&vtx[j], &tbl[j], work);
		}

		/* second half: the 16..31 ring */
		PSMTXTrans(work, 10.f * data->nodeX[i + 1],
		           10.f * data->nodeY[i + 1],
		           10.f * data->nodeZ[i + 1]);
		PSMTXRotRad(rotMtx, 'z', 0.017453292f * data->nodeTilt[i + 1]);
		PSMTXConcat(work, rotMtx, work);
		axis = (Vec){ 0.03f, 1.0f, 0.0f };
		PSMTXRotAxisRad(rotMtx, &axis, 0.017453292f * (data->spinAngle + (f32)(i * i)));
		PSMTXConcat(work, rotMtx, work);
		{
			f32 zScale = data->nodeScale[i + 1] * scale;

			PSMTXScale(scaleMtx, zScale, scale, zScale);
		}
		PSMTXConcat(work, scaleMtx, work);
		rad = 10.f * data->nodeRadius[i];
		s = (f32)sin(6.2831855f * data->nodeAngle[i] / 360.f);
		c = (f32)cos(6.2831855f * data->nodeAngle[i] / 360.f);

		PSMTXTrans(scaleMtx, rad * s, 0.0f, rad * c);
		PSMTXConcat(work, scaleMtx, work);
		PSMTXConcat(baseMtx, work, work);
		for (j = 16; j < 32; j++) {
			kamekiTornadeSetVtx(&vtx[j], &tbl[j], work);
		}

		GXInvalidateVtxCache();
		DCFlushRange(vtx, 0x1C0);

		PSMTXScale(scaleMtx, 0.015625f, 0.03125f, 0.0f);
		{
			f32 z = 0.0f;

			PSMTXTrans(work, data->texScroll0 + (f32)(i * 4), z, z);
		}
		PSMTXConcat(scaleMtx, work, scaleMtx);
		GXLoadTexMtxImm(scaleMtx, GX_TEXMTX0, GX_MTX2x4);

		PSMTXScale(scaleMtx, 0.015625f, 0.03125f, 0.0f);
		{
			f32 z = 0.0f;

			PSMTXTrans(work, data->texScroll1 + (f32)(i * 2), z, z);
		}
		PSMTXConcat(scaleMtx, work, scaleMtx);
		GXLoadTexMtxImm(scaleMtx, GX_TEXMTX1, GX_MTX2x4);

		GXLoadPosMtxImm(cam->view, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);

		for (j = 0; j < 0xF; j++) {
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
			tri2(j + 0x10, j + 1, j, 0, j + 1, j + 0x10, j + 0x11, 1);
		}
	}
}

// .sbss2 4-byte section-tail gap, after effKamekiTornadeDisp's colour template.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") u8 gap_10_80423164_sbss2[4];
#pragma section ".sdata" ".sbss"
