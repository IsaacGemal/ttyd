#include "eff/n64/eff_rubble_n64.h"
#include "eff/n64/eff_hokori_n64.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdlib.h>

const Vec vec3_802fb5a0 = { 0.0f, 0.0f, 0.0f };

typedef struct effRubbleData {
	s32 type;        // 0x00
	f32 x;           // 0x04
	f32 y;           // 0x08
	f32 z;           // 0x0C
	f32 field_0x10;  // 0x10
	f32 field_0x14;  // 0x14
	f32 field_0x18;  // 0x18
	f32 field_0x1C;  // 0x1C
	f32 field_0x20;  // 0x20
	f32 field_0x24;  // 0x24
	f32 field_0x28;  // 0x28
	f32 field_0x2C;  // 0x2C
	f32 field_0x30;  // 0x30
	f32 field_0x34;  // 0x34
	f32 field_0x38;  // 0x38
	f32 field_0x3C;  // 0x3C
	s32 field_0x40;  // 0x40
	s32 field_0x44;  // 0x44
	s32 field_0x48;  // 0x48
	s32 field_0x4C;  // 0x4C
} effRubbleData;

extern const f32 float_0p8_8041fc38;
extern const f32 float_0_8041fc3c;
extern const f32 float_20_8041fc40;
extern const f32 float_neg20_8041fc44;
extern const f32 float_2_8041fc48;
extern const f32 float_deg2rad_8041fc34;
extern const u32 dat_8041fc30;
extern u8 stg_hahen_v[];

static inline void rubbleSetColor(s32 a) {
	GXColor color = *(GXColor*)&dat_8041fc30;
	color.a = a;
	GXSetTevColor(GX_TEVREG0, color);
}
const f64 double_to_int_802fb5b0 = 4503601774854144.0;
const char str_802FB5B8[] = "RubbleN64";

EffectEntry* effRubbleN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 a5, f32 a6, f32 a7, f32 a8) {
	EffectEntry* effect;
	effRubbleData* data;

	effect = effEntry();
	effect->type = str_802FB5B8;
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effRubbleData));
	effect->userdata = data;
	effect->callback = effRubbleMain;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x10 = a5;
	data->field_0x18 = a6;
	data->field_0x20 = a7;
	data->field_0x48 = life;
	data->field_0x4C = 0;
	data->field_0x14 = float_0_8041fc3c;
	data->field_0x1C = float_0_8041fc3c;
	data->field_0x24 = float_0_8041fc3c;
	data->field_0x28 = (f32)(rand() % 360);
	data->field_0x2C = (f32)(rand() % 360);
	data->field_0x30 = float_0_8041fc3c;
	data->field_0x38 = float_20_8041fc40;
	data->field_0x34 = float_neg20_8041fc44;
	data->field_0x44 = 0xFF;
	data->field_0x3C = float_2_8041fc48 * a8;
	data->field_0x40 = 0;
	return effect;
}

void effRubbleMain(EffectEntry* effect) {
	effRubbleData* data;
	Vec position2;
	Vec position;
	s32 life;

	data = effect->userdata;
	position = (Vec){ 0.0f, 0.0f, 0.0f };
	position.x = data->x;
	position.y = data->y;
	position.z = data->z;
	position2 = position;
	data->field_0x48--;
	data->field_0x4C++;
	life = data->field_0x48;
	if (life < 0) {
		effDelete(effect);
		return;
	}
	if (data->y != data->field_0x10) {
		data->field_0x18 += data->field_0x20;
		data->y += data->field_0x18;
		data->field_0x24 += data->field_0x30;
		data->field_0x2C += data->field_0x38;
		data->field_0x28 += data->field_0x34;
		if (data->y <= data->field_0x10) {
			data->y = data->field_0x10;
			effHokoriN64Entry(2, 0x14, data->x, data->y, data->z);
			data->field_0x48 = 0x1e;
		}
	}
	if (life < 10) {
		data->field_0x44 = (s32)((f32)data->field_0x44 * float_0p8_8041fc38);
	}
	dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effRubbleDisp, effect);
}

void effRubbleDisp(CameraId cameraId, void* param) {
	EffectEntry* effect;
	effRubbleData* data;
	CameraEntry* cam;
	Mtx trans;
	Mtx scale;
	Mtx rot;
	s32 alpha;

	effect = param;
	cam = camGetPtr(cameraId);
	data = effect->userdata;
	alpha = data->field_0x44;

	PSMTXTrans(trans, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'x', float_deg2rad_8041fc34 * data->field_0x24);
	PSMTXConcat(trans, rot, trans);
	PSMTXRotRad(rot, 'y', float_deg2rad_8041fc34 * data->field_0x2C);
	PSMTXConcat(trans, rot, trans);
	PSMTXRotRad(rot, 'z', float_deg2rad_8041fc34 * data->field_0x28);
	PSMTXConcat(trans, rot, trans);
	PSMTXScale(scale, data->field_0x3C, data->field_0x3C, data->field_0x3C);
	PSMTXConcat(trans, scale, trans);
	PSMTXConcat(cam->view, trans, trans);
	GXLoadPosMtxImm(trans, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);

	rubbleSetColor(alpha);

	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(0);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_RASC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
	GXSetCullMode(GX_CULL_NONE);

	switch (data->type) {
	case 0:
	case 3:
		effSetVtxDescN64((effVtxArray*)((u8*)stg_hahen_v + 0x188));
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0x1e);
		tri2(0, 1, 2, 0, 3, 4, 5, 0);
		tri2(6, 7, 8, 0, 9, 10, 11, 0);
		tri2(12, 13, 14, 0, 15, 16, 17, 0);
		tri2(18, 19, 20, 0, 21, 22, 23, 0);
		tri2(24, 25, 26, 0, 27, 28, 29, 0);
		break;
	case 1:
	case 4:
		effSetVtxDescN64((effVtxArray*)((u8*)stg_hahen_v + 0x142));
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0x12);
		tri2(0, 1, 2, 0, 1, 3, 2, 0);
		tri2(1, 0, 4, 0, 1, 4, 3, 0);
		tri2(4, 0, 2, 0, 3, 4, 2, 0);
		break;
	case 2:
	case 5:
		effSetVtxDescN64((effVtxArray*)stg_hahen_v);
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0x1e);
		tri2(0, 1, 2, 0, 2, 3, 4, 0);
		tri2(5, 6, 7, 0, 5, 7, 8, 0);
		tri2(9, 10, 11, 0, 12, 13, 14, 0);
		tri2(12, 14, 15, 0, 16, 17, 18, 0);
		tri2(19, 20, 21, 0, 19, 21, 22, 0);
		break;
	}
}
