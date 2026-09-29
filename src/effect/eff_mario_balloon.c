#include "eff/eff_mario_balloon.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/npcdrv.h"
#include "mario/mario.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sdata2 constants (defined by the original object, resolved through symbols.txt).
#ifdef MUST_MATCH
// MUST_MATCH: retail reloads every .sdata2 constant right before each use instead of caching it
// in a register (the original repeats lfs/lwz for the same symbol); volatile reproduces that.
#define BALLOON_CONST const volatile
#else
#define BALLOON_CONST const
#endif
extern BALLOON_CONST GXColor dat_8041ca18;
extern BALLOON_CONST GXColor dat_8041ca1c;
extern BALLOON_CONST GXColor dat_8041ca20;
extern BALLOON_CONST GXColor dat_8041ca24;
extern BALLOON_CONST f32 float_deg2rad_8041ca28;
extern BALLOON_CONST f32 float_0p7_8041ca2c;
extern BALLOON_CONST f32 float_1_8041ca30;
extern BALLOON_CONST f32 float_neg1_8041ca34;
extern BALLOON_CONST f32 float_0_8041ca38;
extern BALLOON_CONST f32 float_neg16_8041ca3c;
extern BALLOON_CONST f32 float_64_8041ca40;
extern BALLOON_CONST f32 float_16_8041ca44;
extern BALLOON_CONST f32 float_2_8041ca48;
extern BALLOON_CONST f32 float_neg20_8041ca4c;
extern BALLOON_CONST f32 float_32_8041ca50;
extern BALLOON_CONST f32 float_20_8041ca54;
extern BALLOON_CONST f32 float_10_8041ca58;
extern BALLOON_CONST f32 float_4_8041ca5c;
extern BALLOON_CONST f32 float_0p4_8041ca60;
extern BALLOON_CONST f32 float_0p8_8041ca64;

// .rodata (defined at the bottom of this file).
extern BALLOON_CONST Vec vec3_802f3478;
extern const char str_802F3484[];

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
	PSMTXRotRad(rot, 'y', float_deg2rad_8041ca28 * (data->angle - camGetPtr(camId)->viewYaw));
	s = float_0p7_8041ca2c * data->scale;
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
			PSMTXTransApply(mtx, mtx2, float_1_8041ca30, float_neg1_8041ca34, float_0_8041ca38);
			GXLoadPosMtxImm(mtx2, 0);
			GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
			GXSetZCompLoc(GX_TRUE);
			GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
			GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
			GXSetCullMode(GX_CULL_NONE);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			GXPosition3f32(float_neg16_8041ca3c, float_64_8041ca40, float_0_8041ca38);
			GXNormal3f32(float_0_8041ca38, float_0_8041ca38, float_1_8041ca30);
			GXTexCoord2f32(float_0_8041ca38, float_0_8041ca38);
			GXPosition3f32(float_16_8041ca44, float_64_8041ca40, float_0_8041ca38);
			GXNormal3f32(float_0_8041ca38, float_0_8041ca38, float_1_8041ca30);
			GXTexCoord2f32(float_2_8041ca48, float_0_8041ca38);
			GXPosition3f32(float_16_8041ca44, float_0_8041ca38, float_0_8041ca38);
			GXNormal3f32(float_0_8041ca38, float_0_8041ca38, float_1_8041ca30);
			GXTexCoord2f32(float_2_8041ca48, float_1_8041ca30);
			GXPosition3f32(float_neg16_8041ca3c, float_0_8041ca38, float_0_8041ca38);
			GXNormal3f32(float_0_8041ca38, float_0_8041ca38, float_1_8041ca30);
			GXTexCoord2f32(float_0_8041ca38, float_1_8041ca30);

			effGetTexObj(0xE, &tex);
			GXLoadTexObj(&tex, GX_TEXMAP0);
			GXSetTevColor(GX_TEVREG0, dat_8041ca18);
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
			GXPosition3f32(float_neg16_8041ca3c, float_64_8041ca40, float_0_8041ca38);
			GXNormal3f32(float_0_8041ca38, float_0_8041ca38, float_1_8041ca30);
			GXTexCoord2f32(float_0_8041ca38, float_0_8041ca38);
			GXPosition3f32(float_16_8041ca44, float_64_8041ca40, float_0_8041ca38);
			GXNormal3f32(float_0_8041ca38, float_0_8041ca38, float_1_8041ca30);
			GXTexCoord2f32(float_2_8041ca48, float_0_8041ca38);
			GXPosition3f32(float_16_8041ca44, float_0_8041ca38, float_0_8041ca38);
			GXNormal3f32(float_0_8041ca38, float_0_8041ca38, float_1_8041ca30);
			GXTexCoord2f32(float_2_8041ca48, float_1_8041ca30);
			GXPosition3f32(float_neg16_8041ca3c, float_0_8041ca38, float_0_8041ca38);
			GXNormal3f32(float_0_8041ca38, float_0_8041ca38, float_1_8041ca30);
			GXTexCoord2f32(float_0_8041ca38, float_1_8041ca30);

			GXSetTevColor(GX_TEVREG0, dat_8041ca1c);
			GXSetCullMode(GX_CULL_FRONT);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			GXPosition3f32(float_neg16_8041ca3c, float_64_8041ca40, float_0_8041ca38);
			GXNormal3f32(float_0_8041ca38, float_0_8041ca38, float_1_8041ca30);
			GXTexCoord2f32(float_0_8041ca38, float_0_8041ca38);
			GXPosition3f32(float_16_8041ca44, float_64_8041ca40, float_0_8041ca38);
			GXNormal3f32(float_0_8041ca38, float_0_8041ca38, float_1_8041ca30);
			GXTexCoord2f32(float_2_8041ca48, float_0_8041ca38);
			GXPosition3f32(float_16_8041ca44, float_0_8041ca38, float_0_8041ca38);
			GXNormal3f32(float_0_8041ca38, float_0_8041ca38, float_1_8041ca30);
			GXTexCoord2f32(float_2_8041ca48, float_1_8041ca30);
			GXPosition3f32(float_neg16_8041ca3c, float_0_8041ca38, float_0_8041ca38);
			GXNormal3f32(float_0_8041ca38, float_0_8041ca38, float_1_8041ca30);
			GXTexCoord2f32(float_0_8041ca38, float_1_8041ca30);
			break;

		case 1:
			effGetTexObj(0x11, &tex);
			GXLoadTexObj(&tex, GX_TEXMAP0);
			GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
			GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
			PSMTXTransApply(mtx, mtx2, float_1_8041ca30, float_neg1_8041ca34, float_0_8041ca38);
			GXLoadPosMtxImm(mtx2, 0);
			GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
			GXSetZCompLoc(GX_TRUE);
			GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
			GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
			GXSetCullMode(GX_CULL_NONE);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			GXPosition3f32(float_neg20_8041ca4c, float_32_8041ca50, float_0_8041ca38);
			GXNormal3f32(float_0_8041ca38, float_0_8041ca38, float_1_8041ca30);
			GXTexCoord2f32(float_0_8041ca38, float_0_8041ca38);
			GXPosition3f32(float_20_8041ca54, float_32_8041ca50, float_0_8041ca38);
			GXNormal3f32(float_0_8041ca38, float_0_8041ca38, float_1_8041ca30);
			GXTexCoord2f32(float_1_8041ca30, float_0_8041ca38);
			GXPosition3f32(float_20_8041ca54, float_0_8041ca38, float_0_8041ca38);
			GXNormal3f32(float_0_8041ca38, float_0_8041ca38, float_1_8041ca30);
			GXTexCoord2f32(float_1_8041ca30, float_1_8041ca30);
			GXPosition3f32(float_neg20_8041ca4c, float_0_8041ca38, float_0_8041ca38);
			GXNormal3f32(float_0_8041ca38, float_0_8041ca38, float_1_8041ca30);
			GXTexCoord2f32(float_0_8041ca38, float_1_8041ca30);

			effGetTexObj(0x10, &tex);
			GXLoadTexObj(&tex, GX_TEXMAP0);
			GXSetTevColor(GX_TEVREG0, dat_8041ca20);
			GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
			GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
			GXLoadPosMtxImm(mtx, 0);
			PSMTXInvXpose(mtx, nrm);
			GXLoadNrmMtxImm(nrm, 0);
			GXSetCurrentMtx(0);
			GXSetCullMode(GX_CULL_BACK);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			GXPosition3f32(float_neg20_8041ca4c, float_32_8041ca50, float_0_8041ca38);
			GXNormal3f32(float_0_8041ca38, float_0_8041ca38, float_1_8041ca30);
			GXTexCoord2f32(float_0_8041ca38, float_0_8041ca38);
			GXPosition3f32(float_20_8041ca54, float_32_8041ca50, float_0_8041ca38);
			GXNormal3f32(float_0_8041ca38, float_0_8041ca38, float_1_8041ca30);
			GXTexCoord2f32(float_1_8041ca30, float_0_8041ca38);
			GXPosition3f32(float_20_8041ca54, float_0_8041ca38, float_0_8041ca38);
			GXNormal3f32(float_0_8041ca38, float_0_8041ca38, float_1_8041ca30);
			GXTexCoord2f32(float_1_8041ca30, float_1_8041ca30);
			GXPosition3f32(float_neg20_8041ca4c, float_0_8041ca38, float_0_8041ca38);
			GXNormal3f32(float_0_8041ca38, float_0_8041ca38, float_1_8041ca30);
			GXTexCoord2f32(float_0_8041ca38, float_1_8041ca30);

			GXSetTevColor(GX_TEVREG0, dat_8041ca24);
			GXSetCullMode(GX_CULL_FRONT);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			GXPosition3f32(float_neg20_8041ca4c, float_32_8041ca50, float_0_8041ca38);
			GXNormal3f32(float_0_8041ca38, float_0_8041ca38, float_1_8041ca30);
			GXTexCoord2f32(float_0_8041ca38, float_0_8041ca38);
			GXPosition3f32(float_20_8041ca54, float_32_8041ca50, float_0_8041ca38);
			GXNormal3f32(float_0_8041ca38, float_0_8041ca38, float_1_8041ca30);
			GXTexCoord2f32(float_1_8041ca30, float_0_8041ca38);
			GXPosition3f32(float_20_8041ca54, float_0_8041ca38, float_0_8041ca38);
			GXNormal3f32(float_0_8041ca38, float_0_8041ca38, float_1_8041ca30);
			GXTexCoord2f32(float_1_8041ca30, float_1_8041ca30);
			GXPosition3f32(float_neg20_8041ca4c, float_0_8041ca38, float_0_8041ca38);
			GXNormal3f32(float_0_8041ca38, float_0_8041ca38, float_1_8041ca30);
			GXTexCoord2f32(float_0_8041ca38, float_1_8041ca30);
			break;
	}
}

void effMarioBalloonMain(EffectEntry* effect) {
	EffMarioBalloonData* data = effect->userdata;
	MarioWork* mario;
	NpcEntry* npc;

	npc = data->npc;
	if (npc == NULL) {
		mario = marioGetPtr();
		data->x = mario->position.x;
		data->y = mario->position.y + mario->wPlayerCollisionBox.y + float_10_8041ca58;
		data->z = mario->position.z;
	} else {
		data->x = npc->position.x;
		data->y = npc->position.y + npc->height + float_10_8041ca58;
		data->z = npc->position.z;
	}

	data->angle += float_4_8041ca5c;
	{
		Vec pos = vec3_802f3478;

		pos.x = data->x;
		pos.y = data->y;
		pos.z = data->z;
		dispEntry(data->cameraId, dispCalcZ(&pos), 8, effMarioBalloonDisp, effect);
	}
}

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
	data->angle = float_0_8041ca38;
	data->npc = npc;
	data->cameraId = 4;
	switch (type) {
		case 0:
			data->scale = float_0p4_8041ca60;
			break;
		case 1:
			data->scale = float_0p8_8041ca64;
			break;
	}
	return entry;
}

// .rodata
BALLOON_CONST Vec vec3_802f3478 = { 0.0f, 0.0f, 0.0f };
const char str_802F3484[] = "MarioBalloon";
