#include "eff/eff_mist2.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "system.h"
#include <math.h>

extern const f32 float_deg2rad_80421ca8;
extern const f32 float_16_80421cac;
extern const f32 float_255_80421cb0;
extern const f32 float_0p5_80421cb4;
extern const f32 float_8_80421cb8;
extern const f32 float_0_80421cbc;
extern const f32 float_1_80421cc0;
extern const f32 float_6p2832_80421cc4;
extern const f32 float_360_80421cc8;
extern const f32 float_100_80421ccc;
extern const f32 float_0p2_80421cd0;
extern const f32 float_10_80421cd4;
extern const Vec vec3_80300248;
const char str_80421CD8[] = "Mist2";

// NONMATCHING (94.95%): the original keeps effect/cameraId/work in r23/r30/r28 and the colour
// temp before the named GXColor (0x8 then 0xc); ours allocates those the other way round. All
// instructions and constants are otherwise identical.
void effMist2Disp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	EffMist2Data* work = effect->userdata;
	EffMist2Data* elem;
	CameraEntry* cam;
	GXTexObj texObj;
	GXColor color;
	Mtx mtxTrans;
	Mtx mtxRot;
	Mtx mtxScale;
	Mtx mtxView;
	s32 alpha;
	s32 i;
	u8 cr;
	u8 cg;
	u8 cb;
	f32 f28;
	f32 f27;

	cam = camGetPtr(cameraId);
	alpha = (s32)work->unk2C;
	cr = work->unk38;
	cg = work->unk39;
	cb = work->unk3A;
	effGetTexObj(0x5E, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C, GX_FALSE, 0x7D);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
	PSMTXTrans(mtxTrans, work->x, work->y, work->z);
	cam = camGetPtr(cameraId);
	PSMTXRotRad(mtxRot, 'y', float_deg2rad_80421ca8 * -cam->viewYaw);
	PSMTXScale(mtxScale, work->unk18, work->unk18, work->unk18);
	PSMTXConcat(mtxTrans, mtxRot, mtxView);
	PSMTXConcat(mtxView, mtxScale, mtxView);
	PSMTXConcat(cam->view, mtxView, mtxView);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	f28 = -float_16_80421cac * float_0p5_80421cb4;
	f27 = float_8_80421cb8;
	elem = &work[1];
	for (i = 1; i < effect->count; i++, elem++) {
		PSMTXTrans(mtxTrans, elem->x, elem->y, elem->z);
		PSMTXScale(mtxScale, elem->unk18, elem->unk18, elem->unk18);
		PSMTXRotRad(mtxRot, 'z', float_deg2rad_80421ca8 * elem->unk24);
		PSMTXConcat(mtxTrans, mtxScale, mtxTrans);
		PSMTXConcat(mtxTrans, mtxRot, mtxTrans);
		PSMTXConcat(mtxView, mtxTrans, mtxTrans);
		GXLoadPosMtxImm(mtxTrans, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		{
			GXColor tmp = { 0, 0, 0, 0 };
			tmp.r = cr;
			tmp.g = cg;
			tmp.b = cb;
			tmp.a = (u8)(s32)(elem->unk2C * (f32)alpha / float_255_80421cb0);
			color = tmp;
		}
		GXSetChanMatColor(GX_COLOR0A0, color);
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(f28, f27, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(f27, f27, 0.0f);
		GXTexCoord2f32(1.0f, 0.0f);
		GXPosition3f32(f27, f28, 0.0f);
		GXTexCoord2f32(1.0f, 1.0f);
		GXPosition3f32(f28, f28, 0.0f);
		GXTexCoord2f32(0.0f, 1.0f);
	}
}

void effMist2Main(EffectEntry* effect) {
	EffMist2Data* data = effect->userdata;
	Vec position;
	Vec tmp = vec3_80300248;
	s32 i;
	s32 type;
	f32 fAngle;
	f32 fRange;

	tmp.x = data->x;
	tmp.y = data->y;
	tmp.z = data->z;
	position = tmp;
	type = data->type;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->unk34 = 0x20;
	}
	if (data->unk34 < 1000) {
		data->unk34--;
	}
	if (data->unk34 < 0) {
		effDelete(effect);
		return;
	}
	if (data->unk34 < 0x20) {
		data->unk2C = (f32)(data->unk34 << 3);
	}

	data->unk30++;
	data++;
	for (i = 1; i < effect->count; i++, data++) {
		if (data->unk3C != 0) {
			data->unk3C--;
			continue;
		}
		data->unk34--;
		if (data->unk34 < 0x20) {
			data->unk2C = (f32)(data->unk34 << 3);
		}
		if (data->unk2C <= 0.0f) {
			fRange = (f32)irand((s32)data->unk1C);
			fAngle = float_6p2832_80421cc4 * (f32)irand(360) / float_360_80421cc8;
			data->x = fRange * (f32)sin(fAngle);
			data->y = 0.0f;
			data->z = fRange * (f32)cos(fAngle);
			data->unk18 = 0.0f;
			data->unk14 = float_0p5_80421cb4 + (f32)irand(100) / float_100_80421ccc;
			data->unk2C = float_255_80421cb0;
			data->unk34 = irand(60) + 60;
			data->unk30 = 0;
			data->unk28 = (f32)(5 - irand(10));
			data->unk24 = 0.0f;
			data->unk3C = irand(30);
			data->unk40 = float_0p2_80421cd0;
			data->unk44 = 0.0f;
		} else {
			data->unk24 += data->unk28;
			data->unk18 += (data->unk14 - data->unk18) / float_10_80421cd4;
			if (type == 1) {
				data->y += data->unk44;
				data->unk44 -= data->unk40;
				if (data->y < 0.0f) {
					data->y = 0.0f;
				}
			}
		}
	}
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effMist2Disp, effect);
}

EffectEntry* effMist2Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 range, f32 yOffset) {
	EffectEntry* entry;
	EffMist2Data* data;
	EffMist2Data* elem;
	s32 num;
	s32 count;
	s32 i;
	f32 fAngle;
	f32 fRange;

	entry = effEntry();
	num = (type == 0) ? 30 : 10;
	count = num + 1;
	entry->type = str_80421CD8;
	entry->count = count;
	data = __memAlloc(HEAP_EFFECT, count * sizeof(EffMist2Data));
	entry->userdata = data;
	entry->callback = effMist2Main;
	entry->flags |= 2;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	if (lifetime <= 0) {
		data->unk34 = 1000;
	} else {
		data->unk34 = lifetime;
	}
	data->unk30 = 0;
	data->unk2C = float_255_80421cb0;
	data->unk18 = float_1_80421cc0;
	data->unk38 = 0;
	data->unk39 = 0;
	data->unk3A = 0;

	elem = &data[1];
	for (i = 1; i < entry->count; i++, elem++) {
		elem->unk1C = range;
		elem->unk20 = yOffset;
		fRange = (f32)irand((s32)elem->unk1C);
		fAngle = float_6p2832_80421cc4 * (f32)irand(360) / float_360_80421cc8;
		elem->x = fRange * (f32)sin(fAngle);
		elem->y = float_0_80421cbc;
		elem->z = fRange * (f32)cos(fAngle);
		elem->unk18 = float_0_80421cbc;
		elem->unk14 = float_0p5_80421cb4 + (f32)irand(100) / float_100_80421ccc;
		elem->unk2C = float_255_80421cb0;
		elem->unk34 = irand(60) + 60;
		elem->unk30 = 0;
		elem->unk28 = (f32)(5 - irand(10));
		elem->unk24 = float_0_80421cbc;
		elem->unk3C = irand(30);
		elem->unk40 = float_0p2_80421cd0;
		elem->unk44 = float_0_80421cbc;
		if (type == 1) {
			f32 fA = float_6p2832_80421cc4 * (f32)irand(360) / float_360_80421cc8;
			elem->x = elem->unk1C * (f32)sin(fA);
			elem->y = elem->unk20;
			elem->z = elem->unk1C * (f32)cos(fA);
		}
	}
	return entry;
}
