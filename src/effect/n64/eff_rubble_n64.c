#include "eff/n64/eff_rubble_n64.h"
#include "eff/n64/eff_hokori_n64.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdlib.h>

u32 stg_hahen_v[204] = {
    0x00030003, 0xFFFD0000, 0x00003232, 0x32FFFFFD,
    0xFFFDFFFD, 0xFDBD0000, 0x323232FF, 0xFFFD0003,
    0x0003FEDE, 0xFE0A3232, 0x32FF0003, 0x00030003,
    0x006AFE74, 0x323232FF, 0x00030003, 0xFFFD0000,
    0x00003232, 0x32FF0003, 0xFFFDFFFD, 0xFC000000,
    0xE0E0E0FF, 0x0003FFFD, 0x0003FC00, 0xFE66E0E0,
    0xE0FFFFFD, 0xFFFD0003, 0xFD9AFE66, 0xE0E0E0FF,
    0xFFFDFFFD, 0xFFFDFD9A, 0x0000E0E0, 0xE0FF0003,
    0x0003FFFD, 0xFC9B0000, 0x323232FF, 0x0003FFFD,
    0xFFFDFB79, 0x01223232, 0x32FFFFFD, 0xFFFDFFFD,
    0xFA580000, 0x323232FF, 0x00030003, 0xFFFDFD9A,
    0xFA66A0A0, 0xA0FF0003, 0x00030003, 0xFD9AFC00,
    0xA0A0A0FF, 0x0003FFFD, 0x0003FC00, 0xFC00A0A0,
    0xA0FF0003, 0xFFFDFFFD, 0xFC00FA66, 0xA0A0A0FF,
    0xFFFDFFFD, 0xFFFDFC0A, 0xF9243232, 0x32FFFFFD,
    0xFFFD0003, 0xFD96F98E, 0x323232FF, 0xFFFD0003,
    0x0003FD2C, 0xFB1A3232, 0x32FFFFFD, 0x00030003,
    0x0000FA66, 0xC8C8C8FF, 0xFFFDFFFD, 0x00030000,
    0xFC00C8C8, 0xC8FF0003, 0xFFFD0003, 0xFE66FC00,
    0xC8C8C8FF, 0x00030003, 0x0003FE66, 0xFA66C8C8,
    0xC8FFFFFC, 0x0000FFFC, 0x022201DE, 0xFFFFFFFF,
    0xFFFC0000, 0x00040222, 0x0400FFFF, 0xFFFFFFFC,
    0x0001FFFF, 0x022202BC, 0xFFFFFFFF, 0x00040000,
    0x00010000, 0x0333FFFF, 0xFFFF0004, 0x0000FFFC,
    0x000001DE, 0xFFFFFFFF, 0x00020003, 0x0002FEEB,
    0xFEC33232, 0x32FFFFFD, 0x0000FFFB, 0x00000000,
    0x323232FF, 0xFFFB0000, 0x0003FE80, 0x00003232,
    0x32FF0002, 0x00030002, 0x00000000, 0x323232FF,
    0xFFFB0000, 0x0003FEB1, 0x00003232, 0x32FF0002,
    0x00000005, 0xFFCDFF48, 0x8E8E8EFF, 0x00020003,
    0x0002FD6B, 0xFE0B3232, 0x32FF0002, 0x00000005,
    0xFDF9FD8D, 0x8E8E8EFF, 0x00040000, 0xFFFFFE12,
    0xFEB28C8C, 0x8CFF0002, 0x00030002, 0xFCC30000,
    0x323232FF, 0x00040000, 0xFFFFFD6B, 0xFF598C8C,
    0x8CFF0000, 0x0000FFFA, 0xFE610000, 0x323232FF,
    0x00020003, 0x0002FCC3, 0x00003232, 0x32FF0000,
    0x0000FFFA, 0xFE610000, 0x323232FF, 0xFFFD0000,
    0xFFFBFE4E, 0x00923232, 0x32FF0000, 0x0000FFFA,
    0x00000000, 0x787878FF, 0x00040000, 0xFFFFFF09,
    0xFF59EAEA, 0xEAFFFFFF, 0xFFFDFFFF, 0xFFE9FEE6,
    0x8C8C8CFF, 0x00040000, 0xFFFF0000, 0x0000EAEA,
    0xEAFF0002, 0x00000005, 0xFEDA0000, 0xDCDCDCFF,
    0xFFFFFFFD, 0xFFFFFFB3, 0xFF108C8C, 0x8CFF0002,
    0x00000005, 0xFEBC0000, 0xDCDCDCFF, 0xFFFB0000,
    0x0003FF7F, 0xFEEA3E3E, 0x3EFFFFFF, 0xFFFDFFFF,
    0x00000000, 0x8C8C8CFF, 0xFFFB0000, 0x0003FF16,
    0xFED03E3E, 0x3EFFFFFD, 0x0000FFFB, 0x00000000,
    0x323232FF, 0xFFFFFFFD, 0xFFFFFEF3, 0x00008C8C,
    0x8CFFFFFD, 0x0000FFFB, 0xFFCCFF76, 0x323232FF,
    0x00000000, 0xFFFA0000, 0x00007878, 0x78FFFFFF,
    0xFFFDFFFF, 0xFEE60000, 0x8C8C8CFF, 0x00000000,
};


// Overlay for the raw vertex table at stg_hahen_v, split at the offsets the
// switch below hands to effSetVtxDescN64.
typedef struct rubbleVtxData {
	u8 type2_5Verts[0x142];            // 0x000, used by types 2 and 5
	u8 type1_4Verts[0x188 - 0x142];    // 0x142, used by types 1 and 4
	u8 type0_3Verts[0x330 - 0x188];    // 0x188, used by types 0 and 3
} rubbleVtxData;

typedef struct effRubbleData {
	s32 type;        // 0x00
	f32 x;           // 0x04
	f32 y;           // 0x08
	f32 z;           // 0x0C
	f32 groundY;     // 0x10, y at which the rubble lands
	f32 field_0x14;  // 0x14
	f32 vy;          // 0x18, velocity, advanced by gravity
	f32 field_0x1C;  // 0x1C
	f32 gravity;     // 0x20
	f32 rotX;        // 0x24
	f32 rotZ;        // 0x28
	f32 rotY;        // 0x2C
	f32 rotSpeedX;   // 0x30
	f32 rotSpeedZ;   // 0x34
	f32 rotSpeedY;   // 0x38
	f32 scale;       // 0x3C
	s32 field_0x40;  // 0x40
	s32 alpha;       // 0x44
	s32 life;        // 0x48
	s32 frame;       // 0x4C, counts up every frame
} effRubbleData;


static inline void rubbleSetColor(s32 alpha) {
	GXColor color = (GXColor){ 0xD7, 0xD7, 0x6E, 0x00 };
	color.a = alpha;
	GXSetTevColor(GX_TEVREG0, color);
}

EffectEntry* effRubbleN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 groundY, f32 vy, f32 gravity, f32 scale) {
	EffectEntry* effect;
	effRubbleData* data;

	effect = effEntry();
	effect->type = "RubbleN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effRubbleData));
	effect->userdata = data;
	effect->callback = effRubbleMain;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->groundY = groundY;
	data->vy = vy;
	data->gravity = gravity;
	data->life = life;
	data->frame = 0;
	data->field_0x14 = 0.0f;
	data->field_0x1C = 0.0f;
	data->rotX = 0.0f;
	data->rotZ = (f32)(rand() % 360);
	data->rotY = (f32)(rand() % 360);
	data->rotSpeedX = 0.0f;
	data->rotSpeedY = 20.f;
	data->rotSpeedZ = -20.f;
	data->alpha = 0xFF;
	data->scale = 2.0f * scale;
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
	data->life--;
	data->frame++;
	life = data->life;
	if (life < 0) {
		effDelete(effect);
		return;
	}
	if (data->y != data->groundY) {
		data->vy += data->gravity;
		data->y += data->vy;
		data->rotX += data->rotSpeedX;
		data->rotY += data->rotSpeedY;
		data->rotZ += data->rotSpeedZ;
		if (data->y <= data->groundY) {
			data->y = data->groundY;
			effHokoriN64Entry(2, 0x14, data->x, data->y, data->z);
			data->life = 0x1e;
		}
	}
	if (life < 10) {
		data->alpha *= 0.8f;
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
	alpha = data->alpha;

	PSMTXTrans(trans, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'x', 0.017453292f * data->rotX);
	PSMTXConcat(trans, rot, trans);
	PSMTXRotRad(rot, 'y', 0.017453292f * data->rotY);
	PSMTXConcat(trans, rot, trans);
	PSMTXRotRad(rot, 'z', 0.017453292f * data->rotZ);
	PSMTXConcat(trans, rot, trans);
	PSMTXScale(scale, data->scale, data->scale, data->scale);
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
		effSetVtxDescN64((effVtxArray*)((rubbleVtxData*)stg_hahen_v)->type0_3Verts);
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0x1e);
		tri2(0, 1, 2, 0, 3, 4, 5, 0);
		tri2(6, 7, 8, 0, 9, 10, 11, 0);
		tri2(12, 13, 14, 0, 15, 16, 17, 0);
		tri2(18, 19, 20, 0, 21, 22, 23, 0);
		tri2(24, 25, 26, 0, 27, 28, 29, 0);
		break;
	case 1:
	case 4:
		effSetVtxDescN64((effVtxArray*)((rubbleVtxData*)stg_hahen_v)->type1_4Verts);
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0x12);
		tri2(0, 1, 2, 0, 1, 3, 2, 0);
		tri2(1, 0, 4, 0, 1, 4, 3, 0);
		tri2(4, 0, 2, 0, 3, 4, 2, 0);
		break;
	case 2:
	case 5:
		effSetVtxDescN64((effVtxArray*)((rubbleVtxData*)stg_hahen_v)->type2_5Verts);
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0x1e);
		tri2(0, 1, 2, 0, 2, 3, 4, 0);
		tri2(5, 6, 7, 0, 5, 7, 8, 0);
		tri2(9, 10, 11, 0, 12, 13, 14, 0);
		tri2(12, 14, 15, 0, 16, 17, 18, 0);
		tri2(19, 20, 21, 0, 19, 21, 22, 0);
		break;
	}
}
