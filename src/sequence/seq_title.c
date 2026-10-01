#include "seq/seq_title.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "mario/mariost.h"
#include "pmario_sound.h"
#include "sound.h"
#include "system.h"
#include "win/win_main.h"
#include "manager/fontmgr.h"
#include <musyx/musyx.h>
#include <string.h>
#include "sequence/seq_title.h"

extern SoundWork sound;
extern PaperSoundBGM psbgm[2];
extern GlobalWork* gp;


TitleWork work2;
TitleWork* wp2 = &work2;

//local prototypes
static void pressStartGX(s32 alpha, f32 x, f32 y); // local to this file in the original

const char* DbgBtlSel_GetMsgDataPtr(void) {
	return NULL;
}

void seq_titleInit(SequenceWork* work) {
	L_titleInit(work);
	gp->flags &= ~2;
	gp->flags &= ~0x40;
	gp->flags &= ~0x20;
	gp->flags &= ~0x1000;
}

void seq_titleExit(SequenceWork* work) {
	fileFree(wp2->texture);
	if (seqGetNextSeq() != SEQ_LOAD) {
		psndStopAllFadeOut();
	}
}

void seq_titleMain(SequenceWork* work) {
	titleMain(work);
}

s32 getDebugMode(void) {
	return wp2->debug;
}

s32 N_getDebugMode(void) {
	return wp2->debug;
}

// NONMATCHING (91.55%): the original reaches its .rodata strings as offsets from one base register (vec3_802c11a0);
// ours loads each string address on its own.
void L_titleInit(SequenceWork* work) {
	memset(wp2, 0, sizeof(TitleWork));
	wp2->texture = fileAllocf(4, "%s/mariost.tpl", getMarioStDvdRoot());
	wp2->field_0xC = 0.0f;
	wp2->field_0x10 = 1000.0f;
	wp2->field_0x14 = 0.0f;
	wp2->field_0x18 = -1000.0f;
	wp2->field_0x1C = 0.0f;
	wp2->debug = 0;
	if (seqGetPrevSeq() == SEQ_LOAD) {
		psndSFXAllOff();
	}
	else if (seqGetPrevSeq() != SEQ_LOGO) {
		if (seqGetPrevSeq() == SEQ_MAP_CHANGE) {
			psndBGMOff_f_d(512, 3000, 0);
			psndBGMOff_f_d(513, 3000, 0);
			psndSFXAllOff();
		}
		else if (seqGetPrevSeq() == SEQ_GAME_OVER) {
			psndStopAllFadeOut();
		}
	}
	if (seqGetPrevSeq() != SEQ_LOAD) {
		if (seqGetPrevSeq() == SEQ_GAME_OVER) {
			if (!OSGetSoundMode()) {
				SoundSetOutputMode(SND_OUTPUTMODE_MONO);
			}
			else {
				SoundSetOutputMode(SND_OUTPUTMODE_STEREO);
			}
			psndBGMOn(0x200, "BGM_DIGEST1");
			if (psbgm[0].streamId != 0xFFFFFFFF) {
				sound.streams[psbgm[0].streamId].flags |= 0x8000;
			}
		}
		else {
			psndBGMOn(0x200, "BGM_TITLE1");
			if (psbgm[0].streamId != 0xFFFFFFFF) {
				sound.streams[psbgm[0].streamId].flags |= 0x8000;
			}
		}
	}
	camGetPtr(CAMERA_2D)->flags &= ~0x100;
	camGetPtr(CAMERA_2D)->flags |= 0x400;
}

// NONMATCHING (4.50%): 11 instructions differ, 509 missing, plus register differences.
// dont_inline keeps titleMain a real `bl` from seq_titleMain; retail does not inline it.
#pragma dont_inline on
void titleMain(SequenceWork* work) {
	switch (wp2->field_0x8) {
		case 0:
			if (psndBGMStartCheck(0)
				&& animGroupBaseAsync("OFF_d_maku_left_right", 2, 0)
				&& animGroupBaseAsync("OFF_d_maku_center", 2, 0))
			{

			}
			break;
	}
}

#pragma dont_inline reset

void disp1(CameraId cameraId, void* param) {
	CameraEntry* camera = camGetPtr(CAMERA_2D);
	f32 projBackup[GX_PROJECTION_SZ];
	f32 viewportBackup[GX_VIEWPORT_SZ];
	CameraEntry camBackup;

	camBackup = *camGetPtr(cameraId);
	*camGetPtr(cameraId) = *camera;

	GXGetProjectionv(projBackup);
	GXGetViewportv(viewportBackup);
	GXSetProjection(camera->projection, camera->projType);
	GXSetViewport(camera->viewport[0], camera->viewport[1], camera->viewport[2], camera->viewport[3],
	              camera->viewport[4], camera->viewport[5]);

	winTexInit(*wp2->texture->data);
	winTexSet(7, (Vec){ 0.0f, 0.0f, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });

	*camGetPtr(cameraId) = camBackup;
	GXSetProjectionv(projBackup);
	GXSetViewport(viewportBackup[0], viewportBackup[1], viewportBackup[2], viewportBackup[3], viewportBackup[4],
	              viewportBackup[5]);
}

// Match shape from docs/AGENT_BRIEF.md ("a Vec temp copied from a named zero vector"):
// building the translate in an inline that starts from the rodata zero vector keeps the
// argument temp in the same place the original's does; the plain local form scores ~78%.
static inline Vec disp2Vec(f32 x, f32 y, const Vec* zero) {
	Vec v = *zero;
	v.x = x;
	v.y = y;
	return v;
}

// NONMATCHING (97.14%): identical instructions, different stack slots. Target packs each call
// as scale/temp/outgoing (0x38/0x44/0x50, 0x14/0x20/0x2c); we get the inline temps first (0x20,
// 0x14) then the scales (0x44, 0x2c). One fmuls dst too; .sdata2 needs titleMain's constants.
void disp2(CameraId cameraId, void* param) {
	u8* base = (u8*)vec3_802c11a0; // base of this unit's .rodata pool
	CameraEntry* camera = camGetPtr(CAMERA_2D);
	f32 projBackup[GX_PROJECTION_SZ];
	f32 viewportBackup[GX_VIEWPORT_SZ];
	CameraEntry camBackup;
	f32 pulse;
	s32 alpha;

	camBackup = *camGetPtr(cameraId);
	*camGetPtr(cameraId) = *camera;

	GXGetProjectionv(projBackup);
	GXGetViewportv(viewportBackup);
	GXSetProjection(camera->projection, camera->projType);
	GXSetViewport(camera->viewport[0], camera->viewport[1], camera->viewport[2], camera->viewport[3],
	              camera->viewport[4], camera->viewport[5]);

	winTexInit(*wp2->texture->data);

	unk_80177008(5, disp2Vec(wp2->field_0xC, wp2->field_0x10, (Vec*)(base + 0x18)), *(Vec*)(base + 0x24),
	             (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });

	winTexSet(6, disp2Vec(wp2->field_0x14, wp2->field_0x18, (Vec*)(base + 0x30)), *(Vec*)(base + 0x3c),
	          (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });

	*(s32*)((u8*)wp2 + 0x2C) += 1; // TitleWork pulse counter, no field declared in the header
	pulse = (f32)sin((f64)(6.2831855f * (f32)*(s32*)((u8*)wp2 + 0x2C) / 120.0f));
	alpha = (s32)(pulse * 459.0f * wp2->field_0x1C);
	if (alpha < 0) {
		alpha = 0;
	}
	if (alpha > 255) {
		alpha = 0xFF;
	}
	pressStartGX(alpha, 0.0f, -110.0f);

	if (wp2->debug < 0 && (keyGetButton(0) & 0x10) != 0) {
		FontDrawStart();
		FontDrawEdge();
		FontDrawColor(&(GXColor){ 0xFA, 0xFA, 0xFA, 0xFF });
		FontDrawScale(0.8f);
		FontDrawString(dat_ptr_8040f260, 60.0f, 220.0f);
		FontDrawString(dat_ptr_8040f264, 200.0f, 220.0f);
	}

	*camGetPtr(cameraId) = camBackup;
	GXSetProjectionv(projBackup);
	GXSetViewport(viewportBackup[0], viewportBackup[1], viewportBackup[2], viewportBackup[3], viewportBackup[4],
	              viewportBackup[5]);
}

static void pressStartGX(s32 alpha, f32 x, f32 y) {
	CameraEntry* camera = camGetCurPtr();
	GXTexObj startText, gradient;
	Mtx v17, v16;

	TEXGetGXTexObjFromPalette(*wp2->texture->data, &startText, 8);
	GXLoadTexObj(&startText, GX_TEXMAP0);
	TEXGetGXTexObjFromPalette(*wp2->texture->data, &gradient, 9);
	GXLoadTexObj(&gradient, GX_TEXMAP1);
	GXSetNumTevStages(2);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);
	GXSetTevColor(GX_TEVREG0, (GXColor){ 0x00, 0x00, 0x00, alpha });
	GXSetNumChans(0);
	GXSetNumTexGens(2);
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 60);
	GXSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, 30);
	MTXScale(v16, 1.0f, 1.0f, 1.0f);
	MTXTrans(v17, (f32)*(s32*)((u8*)wp2 + 0x2C) / -200.0f, 0.0f, 0.0f); // pulse counter, undeclared field
	MTXConcat(v16, v17, v17);
	GXLoadTexMtxImm(v17, 30, GX_MTX2x4);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetZMode(GX_DISABLE, GX_LEQUAL, GX_DISABLE);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	MTXTrans(v17, x, y, 0.0f);
	MTXScale(v16, (f32)GXGetTexObjWidth(&startText), (f32)GXGetTexObjHeight(&startText), 1.0f);
	MTXConcat(v17, v16, v17);
	MTXConcat(camera->view, v17, v17);
	GXLoadPosMtxImm(v17, 0);
	GXSetCurrentMtx(0);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3f32(-0.5f, -0.5f, 0.0f);
	GXTexCoord2f32(0.0f, 1.0f);
	GXPosition3f32(0.5f, -0.5f, 0.0f);
	GXTexCoord2f32(1.0f, 1.0f);
	GXPosition3f32(0.5f, 0.5f, 0.0f);
	GXTexCoord2f32(1.0f, 0.0f);
	GXPosition3f32(-0.5f, 0.5f, 0.0f);
	GXTexCoord2f32(0.0f, 0.0f);
	GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
}
