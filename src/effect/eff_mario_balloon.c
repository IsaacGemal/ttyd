#include "eff/eff_mario_balloon.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/npcdrv.h"
#include "mario/mario.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

extern const char str_802F3484[];

EffectEntry* effMarioBalloonEntry(s32 type, NpcEntry* npc) {
	EffectEntry* entry;
	EffMarioBalloonData* data;

	entry = effEntry();
	entry->type = str_802F3484;
	entry->count = 1;
	data = __memAlloc(3, entry->count << 5);
	entry->userdata = data;
	entry->callback = effMarioBalloonMain;
	data->type = type;
	data->angle = 0.0f;
	data->npc = npc;
	data->cameraId = 4;
	switch (type) {
		case 0:
			data->scale = 0.4f;
			break;
		case 1:
			data->scale = 0.8f;
			break;
	}
	return entry;
}

void effMarioBalloonMain(EffectEntry* effect) {
	EffMarioBalloonData* data = effect->userdata;
	MarioWork* mario;
	NpcEntry* npc;

	npc = data->npc;
	if (npc == NULL) {
		mario = marioGetPtr();
		data->x = mario->position.x;
		data->y = mario->position.y + mario->wPlayerCollisionBox.y + 10.0f;
		data->z = mario->position.z;
	} else {
		data->x = npc->position.x;
		data->y = npc->position.y + npc->height + 10.0f;
		data->z = npc->position.z;
	}

	data->angle += 4.0f;
	{
		Vec pos = { 0.0f, 0.0f, 0.0f };

		pos.x = data->x;
		pos.y = data->y;
		pos.z = data->z;
		dispEntry(data->cameraId, dispCalcZ(&pos), 8, effMarioBalloonDisp, effect);
	}
}

void effMarioBalloonDisp(CameraId camId, void* param) {
	EffMarioBalloonData* data = ((EffectEntry*)param)->userdata;
	Mtx mtx;
	Mtx rot;
	Mtx scale;
	Mtx mtx2;
	Mtx nrm;
	GXTexObj tex;
	f32 s;

	GXSetNumChans(0);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxDesc(GX_VA_NRM, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', 0.017453292f * (data->angle - camGetPtr(camId)->viewYaw));
	s = 0.7f * data->scale;
	PSMTXScale(scale, s, s, s);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXConcat(mtx, scale, mtx);
	PSMTXConcat(camGetPtr(camId)->view, mtx, mtx);
	GXSetCurrentMtx(0);

	switch (data->type) {
		case 0:
			effGetTexObj(0xF, &tex);
			GXLoadTexObj(&tex, GX_TEXMAP0);
			GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
			GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
			PSMTXTransApply(mtx, mtx2, 1.0f, -1.0f, 0.0f);
			GXLoadPosMtxImm(mtx2, 0);
			GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
			GXSetZCompLoc(GX_TRUE);
			GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
			GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
			GXSetCullMode(GX_CULL_NONE);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			GXPosition3f32(-16.0f, 64.0f, 0.0f);
			GXNormal3f32(0.0f, 0.0f, 1.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(16.0f, 64.0f, 0.0f);
			GXNormal3f32(0.0f, 0.0f, 1.0f);
			GXTexCoord2f32(2.0f, 0.0f);
			GXPosition3f32(16.0f, 0.0f, 0.0f);
			GXNormal3f32(0.0f, 0.0f, 1.0f);
			GXTexCoord2f32(2.0f, 1.0f);
			GXPosition3f32(-16.0f, 0.0f, 0.0f);
			GXNormal3f32(0.0f, 0.0f, 1.0f);
			GXTexCoord2f32(0.0f, 1.0f);

			effGetTexObj(0xE, &tex);
			GXLoadTexObj(&tex, GX_TEXMAP0);
			{
				GXColor c = { 0xFF, 0xFF, 0xFF, 0xFF };
				GXSetTevColor(GX_TEVREG0, c);
			}
			GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
			GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
			GXLoadPosMtxImm(mtx, 0);
			PSMTXInvXpose(mtx, nrm);
			GXLoadNrmMtxImm(nrm, 0);
			GXSetCurrentMtx(0);
			GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
			GXSetZCompLoc(GX_FALSE);
			GXSetAlphaCompare(GX_GEQUAL, 0x80, GX_AOP_OR, GX_NEVER, 0);
			GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
			GXSetCullMode(GX_CULL_BACK);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			GXPosition3f32(-16.0f, 64.0f, 0.0f);
			GXNormal3f32(0.0f, 0.0f, 1.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(16.0f, 64.0f, 0.0f);
			GXNormal3f32(0.0f, 0.0f, 1.0f);
			GXTexCoord2f32(2.0f, 0.0f);
			GXPosition3f32(16.0f, 0.0f, 0.0f);
			GXNormal3f32(0.0f, 0.0f, 1.0f);
			GXTexCoord2f32(2.0f, 1.0f);
			GXPosition3f32(-16.0f, 0.0f, 0.0f);
			GXNormal3f32(0.0f, 0.0f, 1.0f);
			GXTexCoord2f32(0.0f, 1.0f);

			{
				GXColor c = { 0xA0, 0xA0, 0xA0, 0xFF };
				GXSetTevColor(GX_TEVREG0, c);
			}
			GXSetCullMode(GX_CULL_FRONT);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			GXPosition3f32(-16.0f, 64.0f, 0.0f);
			GXNormal3f32(0.0f, 0.0f, 1.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(16.0f, 64.0f, 0.0f);
			GXNormal3f32(0.0f, 0.0f, 1.0f);
			GXTexCoord2f32(2.0f, 0.0f);
			GXPosition3f32(16.0f, 0.0f, 0.0f);
			GXNormal3f32(0.0f, 0.0f, 1.0f);
			GXTexCoord2f32(2.0f, 1.0f);
			GXPosition3f32(-16.0f, 0.0f, 0.0f);
			GXNormal3f32(0.0f, 0.0f, 1.0f);
			GXTexCoord2f32(0.0f, 1.0f);
			break;

		case 1:
			effGetTexObj(0x11, &tex);
			GXLoadTexObj(&tex, GX_TEXMAP0);
			GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
			GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
			PSMTXTransApply(mtx, mtx2, 1.0f, -1.0f, 0.0f);
			GXLoadPosMtxImm(mtx2, 0);
			GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
			GXSetZCompLoc(GX_TRUE);
			GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
			GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
			GXSetCullMode(GX_CULL_NONE);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			GXPosition3f32(-20.0f, 32.0f, 0.0f);
			GXNormal3f32(0.0f, 0.0f, 1.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(20.0f, 32.0f, 0.0f);
			GXNormal3f32(0.0f, 0.0f, 1.0f);
			GXTexCoord2f32(1.0f, 0.0f);
			GXPosition3f32(20.0f, 0.0f, 0.0f);
			GXNormal3f32(0.0f, 0.0f, 1.0f);
			GXTexCoord2f32(1.0f, 1.0f);
			GXPosition3f32(-20.0f, 0.0f, 0.0f);
			GXNormal3f32(0.0f, 0.0f, 1.0f);
			GXTexCoord2f32(0.0f, 1.0f);

			effGetTexObj(0x10, &tex);
			GXLoadTexObj(&tex, GX_TEXMAP0);
			{
				GXColor c = { 0xFF, 0xFF, 0xFF, 0xFF };
				GXSetTevColor(GX_TEVREG0, c);
			}
			GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
			GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
			GXLoadPosMtxImm(mtx, 0);
			PSMTXInvXpose(mtx, nrm);
			GXLoadNrmMtxImm(nrm, 0);
			GXSetCurrentMtx(0);
			GXSetCullMode(GX_CULL_BACK);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			GXPosition3f32(-20.0f, 32.0f, 0.0f);
			GXNormal3f32(0.0f, 0.0f, 1.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(20.0f, 32.0f, 0.0f);
			GXNormal3f32(0.0f, 0.0f, 1.0f);
			GXTexCoord2f32(1.0f, 0.0f);
			GXPosition3f32(20.0f, 0.0f, 0.0f);
			GXNormal3f32(0.0f, 0.0f, 1.0f);
			GXTexCoord2f32(1.0f, 1.0f);
			GXPosition3f32(-20.0f, 0.0f, 0.0f);
			GXNormal3f32(0.0f, 0.0f, 1.0f);
			GXTexCoord2f32(0.0f, 1.0f);

			{
				GXColor c = { 0xA0, 0xA0, 0xA0, 0xFF };
				GXSetTevColor(GX_TEVREG0, c);
			}
			GXSetCullMode(GX_CULL_FRONT);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			GXPosition3f32(-20.0f, 32.0f, 0.0f);
			GXNormal3f32(0.0f, 0.0f, 1.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(20.0f, 32.0f, 0.0f);
			GXNormal3f32(0.0f, 0.0f, 1.0f);
			GXTexCoord2f32(1.0f, 0.0f);
			GXPosition3f32(20.0f, 0.0f, 0.0f);
			GXNormal3f32(0.0f, 0.0f, 1.0f);
			GXTexCoord2f32(1.0f, 1.0f);
			GXPosition3f32(-20.0f, 0.0f, 0.0f);
			GXNormal3f32(0.0f, 0.0f, 1.0f);
			GXTexCoord2f32(0.0f, 1.0f);
			break;
	}
}

// .rodata
const char str_802F3484[] = "MarioBalloon";
