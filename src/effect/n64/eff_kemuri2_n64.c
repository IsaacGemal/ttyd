#include "eff/n64/eff_kemuri2_n64.h"
#include "eff/eff_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .rodata

// .sdata animation frame tables and the pointer table that selects between them.
u32 anim_data_0[4] = { 0x00010202, 0x03030404, 0x05050506, 0x0606FF00 };
u32 anim_data_1[5] = { 0x00010202, 0x03030404, 0x05050606, 0x06070707, 0xFF000000 };
static void* anim_data_table[2] = { anim_data_0, anim_data_1 };

// .sdata2 constants, named after their original addresses.

// Local to this file in the original (`.fn effKemuri2Main, local`).
static void effKemuri2Main(EffectEntry* effect);
static void main_dl(s32 type);
static void effKemuri2Disp(CameraId cameraId, void* param);

typedef struct effKemuri2Data {
	u32 active;                 // 0x0
	u16 type;                   // 0x4
	u8 pad_6[2];                // 0x6
	Vec position;               // 0x8
	u8 field_0x14[0x44 - 0x14]; // 0x14
	f32 driftScale;             // 0x44: scales the horizontal drift per frame
	f32 velocityY;              // 0x48: vertical velocity, updated by gravity
	f32 gravity;                // 0x4c
	f32 driftX;                 // 0x50
	f32 driftZ;                 // 0x54
	s32 animIndex;              // 0x58: selects an entry of anim_data_table
	s32 animCursor;             // 0x5c: next byte in the selected animation sequence
	s32 animFrame;              // 0x60: current frame, selects the texture tile
} effKemuri2Data;

EffectEntry* effKemuri2N64Entry(s32 type, f32 x, f32 y, f32 z, f32 driftX, f32 driftZ) {
	EffectEntry* entry;
	effKemuri2Data* data;

	entry = effEntry();
	entry->type = "Kemuri2N64";
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effKemuri2Data));
	entry->userdata = data;
	entry->callback = effKemuri2Main;

	data->animIndex = (type == 2);
	data->animCursor = 0;
	data->type = type;
	data->active = 1;
	data->driftX = driftX;
	data->driftZ = driftZ;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->driftScale = 0.33333334f;
	data->velocityY = 1.75f;
	data->gravity = -0.16f;
	return entry;
}

static void effKemuri2Main(EffectEntry* effect) {
	effKemuri2Data* data;
	Vec position2;
	Vec position;
	s32 i;

	data = effect->userdata;
	position = (Vec){ 0.0f, 0.0f, 0.0f };
	position.x = data->position.x;
	position.y = data->position.y;
	position.z = data->position.z;
	position2 = position;
	if ((data->animFrame = ((s8*)anim_data_table[data->animIndex])[data->animCursor++]) < 0) {
		effDelete(effect);
		return;
	}

	for (i = 0; i < effect->count; i++, data++) {
		data->position.x += data->driftScale * data->driftX;
		data->position.z += data->driftScale * data->driftZ;
		data->velocityY += data->gravity;
		data->position.y += data->velocityY;
	}

	dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effKemuri2Disp, effect);
}

static void main_dl(s32 type) {
	GXTexObj texObj;
	s16 sizeX;
	s16 sizeY;

	switch (type) {
	case 0:
		effGetTexObjN64(0xC, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		break;
	case 1:
		effGetTexObjN64(0xD, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		break;
	/* MUST_MATCH: the empty case 2 makes MWCC emit the duplicated branch to the
	   default body that retail has. */
	case 2:
	default:
		effGetTexObjN64(0xE, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		break;
	}

	GXSetNumChans(0);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);
	GXSetNumTevStages(3);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_APREV, GX_CA_TEXA, GX_CA_A1, GX_CA_ZERO);
	GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
	GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	switch (type) {
	case 0:
		sizeX = 16;
		sizeY = 16;
		break;
	/* MUST_MATCH: MWCC needs an explicit case 1 sharing the default body to emit the
	   cmp-1/cmp-0 decision tree retail has. */
	case 1:
	default:
		sizeX = 24;
		sizeY = 24;
		break;
	}

	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3s16(-sizeX / 2, sizeY / 2, 0);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3s16(sizeX / 2, sizeY / 2, 0);
	GXTexCoord2f32(1.0f, 0.0f);
	GXPosition3s16(sizeX / 2, -sizeY / 2, 0);
	GXTexCoord2f32(1.0f, 1.0f);
	GXPosition3s16(-sizeX / 2, -sizeY / 2, 0);
	GXTexCoord2f32(0.0f, 1.0f);
}

static void effKemuri2Disp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	effKemuri2Data* data;
	CameraEntry* camera;
	Mtx transMtx;
	Mtx rotMtx;
	Mtx scaleMtx;
	Mtx outMtx;
	f32 texWidth;
	f32 uScale;
	f32 frameSizeF;
	f32 vScale;
	f32 yTrans;
	s32 type;
	s32 flag;
	s32 i;
	s32 frame;
	s32 frameSize;
	s32 offset;

	camera = camGetPtr(cameraId);
	data = effect->userdata;
	flag = 0;
	frame = data->animFrame;
	type = data->type;
	if ((data->animIndex == 0 && frame == 6) ||
	    (data->animIndex == 1 && frame == 7)) {
		flag = 1;
	}
	GXSetTevColor(GX_TEVREG0, (GXColor){ 0xE6, 0xDE, 0xDE, 0x6E });
	GXSetTevColor(GX_TEVREG1, (GXColor){ 0x00, 0x00, 0x00, 0x7F });
	switch (type) {
	case 0:
		texWidth = 176.0f;
		frameSizeF = 24.0f;
		break;
	case 1:
		texWidth = 224.0f;
		frameSizeF = 32.0f;
		break;
	/* MUST_MATCH: the empty case 2 forces MWCC's switch decision tree to emit the
	   duplicated branch to the default body that retail has. */
	case 2:
	default:
		texWidth = 256.0f;
		frameSizeF = 32.0f;
		break;
	}
	frameSize = (type == 0) ? 24 : 32;
	offset = frame * frameSize;
	uScale = (f32)frameSize / texWidth;
	vScale = (f32)frameSize / frameSizeF;
	PSMTXScale(scaleMtx, uScale, vScale, 1.0f);
	yTrans = 0.0f / frameSizeF;
	PSMTXTrans(transMtx, (f32)offset / texWidth, yTrans, 0.0f);
	PSMTXConcat(transMtx, scaleMtx, transMtx);
	GXLoadTexMtxImm(transMtx, GX_TEXMTX0, GX_MTX2x4);
	if (flag != 0) {
		PSMTXScale(scaleMtx, uScale, 0.0f, 1.0f);
		PSMTXTrans(transMtx, (f32)(offset + frameSize) / texWidth, yTrans, 0.0f);
		PSMTXConcat(transMtx, scaleMtx, transMtx);
		GXLoadTexMtxImm(transMtx, GX_TEXMTX1, GX_MTX2x4);
	} else {
		PSMTXScale(scaleMtx, uScale, vScale, 1.0f);
		PSMTXTrans(transMtx, (f32)(offset + frameSize) / texWidth, yTrans, 0.0f);
		PSMTXConcat(transMtx, scaleMtx, transMtx);
		GXLoadTexMtxImm(transMtx, GX_TEXMTX1, GX_MTX2x4);
	}
	for (i = 0; i < effect->count; i++, data++) {
		PSMTXTrans(transMtx, data->position.x, data->position.y, data->position.z);
		PSMTXRotRad(rotMtx, 'y', 0.017453292f * -camGetPtr(cameraId)->viewYaw);
		PSMTXConcat(transMtx, rotMtx, outMtx);
		PSMTXConcat(camera->view, outMtx, outMtx);
		GXLoadPosMtxImm(outMtx, GX_PNMTX0);
		main_dl(type);
	}
}
