#include "eff/eff_mizutama.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include <dolphin/mtx.h>
#include <dolphin/types.h>

typedef struct MizutamaData {
	s32 type;       // 0x00
	f32 x;          // 0x04
	f32 y;          // 0x08
	f32 z;          // 0x0C
	f32 vx;         // 0x10
	f32 vy;         // 0x14
	f32 scale;      // 0x18
	s32 field_0x1C; // 0x1C
	s32 field_0x20; // 0x20
	s32 timer;      // 0x24
} MizutamaData;

extern const f32 float_0p97_80421288;
extern const f32 float_0p3_8042128c;
extern const f32 float_0p99_80421290;
extern const f32 float_0p01_80421294;
extern const f32 float_8_80421270;
extern const f32 float_360_80421274;
extern const f32 float_0p5_80421278;
extern const f32 float_4_8042127c;
#ifdef MUST_MATCH
/* MUST_MATCH: the original reloads these two .sdata2 constants before every FIFO store;
   as plain `const` MWCC keeps them live in f0/f3 and the vertex block differs. */
extern volatile const f32 float_0_80421280;
extern volatile const f32 float_1_80421284;
#else
extern const f32 float_0_80421280;
extern const f32 float_1_80421284;
#endif

void effMizutamaMain(EffectEntry* effect) {
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch and copies it to a second local before
	   dispCalcZ; a single local folds the copy away. */
	Vec position;
#endif
	MizutamaData* data = effect->userdata;
	Vec disp = { 0 };
	s32 done = 0;
	s32 i;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
#ifdef MUST_MATCH
	position = disp;
#endif

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		if (data->timer != 0) {
			data->timer--;
		} else {
			data->x += data->vx;
			data->y += data->vy;
			data->vx *= float_0p97_80421288;
			data->vy *= float_0p97_80421288;
			data->y += float_0p3_8042128c;
			data->scale *= float_0p99_80421290;
			if (data->scale < float_0p01_80421294) {
				done++;
			}
		}
	}

	if (done >= effect->count - 1) {
		effDelete(effect);
	} else {
#ifdef MUST_MATCH
		dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effMizutamaDisp, effect);
#else
		dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effMizutamaDisp, effect);
#endif
	}
}

// match: MWCC reloads the named .sdata2 float constants instead of keeping them live
void effMizutamaDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	MizutamaData* data = effect->userdata;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx mtxTrans;
	Mtx mtxScale;
	Mtx mtxBase;
	f32 y;
	f32 sz;
	s32 i;

	cam = camGetPtr(camId);
	y = data->y;

	PSMTXTrans(mtxTrans, data->x, y, data->z);
	PSMTXScale(mtxScale, data->scale, data->scale, data->scale);
	PSMTXConcat(mtxTrans, mtxScale, mtxBase);

	GXSetNumChans(0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
	effGetTexObj(0x3F, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	sz = -float_8_80421270;
	i = 1;
	data++;
	for (; i < effect->count; i++, data++) {
		if (data->timer != 0) {
			continue;
		}

		PSMTXTrans(mtxTrans, data->x, data->y, data->z);
		PSMTXScale(mtxScale, data->scale, data->scale, data->scale);
		PSMTXConcat(mtxTrans, mtxScale, mtxTrans);
		PSMTXConcat(mtxBase, mtxTrans, mtxTrans);

		if (y > float_360_80421274) {
			continue;
		}

		PSMTXConcat(cam->view, mtxTrans, mtxTrans);
		GXLoadPosMtxImm(mtxTrans, 0);
		GXSetCurrentMtx(0);
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(sz * float_0p5_80421278, float_4_8042127c, float_0_80421280);
		GXTexCoord2f32(float_0_80421280, float_0_80421280);
		GXPosition3f32(float_4_8042127c, float_4_8042127c, float_0_80421280);
		GXTexCoord2f32(float_1_80421284, float_0_80421280);
		GXPosition3f32(float_4_8042127c, sz * float_0p5_80421278, float_0_80421280);
		GXTexCoord2f32(float_1_80421284, float_1_80421284);
		GXPosition3f32(sz * float_0p5_80421278, sz * float_0p5_80421278, float_0_80421280);
		GXTexCoord2f32(float_0_80421280, float_1_80421284);
	}
}

