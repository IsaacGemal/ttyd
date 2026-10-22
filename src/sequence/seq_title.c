// sound.h -> musyx.h includes the SDK <math.h>, whose `extern inline` sqrtf/sqrt emit four 0.5/3.0
// localstatics into .rodata that the original unit does not have (shifting every string by 0x20).
// Suppress it and take the out-of-line MSL math ABI from math_ext.h instead.
#define _MATH_H_
#include "math_ext.h"
#include "seq/seq_title.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/fadedrv.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "pmario_sound.h"
#include "sound.h"
#include "system.h"
#include "win/win_main.h"
#include "manager/fontmgr.h"
void FontDrawColor(GXColor* color);
#include <string.h>

extern SoundWork sound;
extern PaperSoundBGM psbgm[2];
extern GlobalWork* gp;


TitleWork work2;
TitleWork* wp2 = &work2;

// The first string block of the .rodata pool, defined after disp2 so disp1/disp2's Vec templates come first.
extern const char str_demoTplPath[];
extern const char str_c_peach[];
extern const char str_c_nazo[];
extern const char str_MOBJ_GoldenTreasureBox[];
extern const char str_MOBJ_BigTreasureBox[];
extern const char str_c_roten[];
extern const char str_c_kuribo_n[];
extern const char str_c_tyusan[];

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

// The original's L_titleInit and titleMain reach the later pooled strings by fixed offsets from the
// .rodata base (disp1's first Vec template); plain literals are pooled the same way once the named
// .rodata objects are in place, so the natural (non-MATCH) form is the literal one.
void L_titleInit(SequenceWork* work) {
	memset(wp2, 0, sizeof(TitleWork));
	wp2->texture = fileAllocf(FILE_TYPE_TPL, "%s/mariost.tpl", getMarioStDvdRoot());
	wp2->tex5PosX = 0.0f;
	wp2->tex5PosY = 1000.0f;
	wp2->tex6PosX = 0.0f;
	wp2->tex6PosY = -1000.0f;
	wp2->fade = 0.0f;
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
			psndBGMOn(0x200, "BGM_TITLE1\0\0\0\0\0");
			if (psbgm[0].streamId != 0xFFFFFFFF) {
				sound.streams[psbgm[0].streamId].flags |= 0x8000;
			}
		}
	}
	camGetPtr(CAMERA_2D)->flags &= ~0x100;
	camGetPtr(CAMERA_2D)->flags |= 0x400;
}

void titleMain(SequenceWork* work) {
// The eight animation-name strings are named objects at the base of the pool (str_demoTplPath through str_c_tyusan);
// the three later ones are pooled literals from this function. Using the named objects here keeps
// the compiler from pooling a second copy of the first eight, which would shift the whole pool.
// The natural form is the plain literals.
#ifdef MUST_MATCH
#define TITLE_MAKU_LR "OFF_d_maku_left_right"
#define TITLE_MAKU_C "OFF_d_maku_center"
#define TITLE_DEMO_TPL str_demoTplPath
#define TITLE_PEACH str_c_peach
#define TITLE_NAZO str_c_nazo
#define TITLE_GOLD str_MOBJ_GoldenTreasureBox
#define TITLE_BIG str_MOBJ_BigTreasureBox
#define TITLE_ROTEN str_c_roten
#define TITLE_KURIBO str_c_kuribo_n
#define TITLE_TYUSAN str_c_tyusan
#define TITLE_DMO00 "dmo_00"
#else
#define TITLE_MAKU_LR "OFF_d_maku_left_right"
#define TITLE_MAKU_C "OFF_d_maku_center"
#define TITLE_DEMO_TPL "%s/demo.tpl"
#define TITLE_PEACH "c_peach"
#define TITLE_NAZO "c_nazo"
#define TITLE_GOLD "MOBJ_GoldenTreasureBox"
#define TITLE_BIG "MOBJ_BigTreasureBox"
#define TITLE_ROTEN "c_roten"
#define TITLE_KURIBO "c_kuribo_n"
#define TITLE_TYUSAN "c_tyusan"
#define TITLE_DMO00 "dmo_00"
#endif
	switch (wp2->step) {
		case 0: {
			u32 bgmCheck = psndBGMStartCheck(0);
			if (bgmCheck && animGroupBaseAsync(TITLE_MAKU_LR, 2, 0)
				&& animGroupBaseAsync(TITLE_MAKU_C, 2, 0))
			{
				if (seqGetPrevSeq() == SEQ_LOAD) {
					fadeEntry(0x40, 0, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
					fadeEntry(0x1f, 0, (GXColor){ 0x00, 0x00, 0x00, 0xFF });
					wp2->tex5PosY = 120.0f;
					wp2->tex6PosY = -170.0f;
					wp2->waitTimer = 0;
				}
				else if (seqGetPrevSeq() == SEQ_GAME_OVER) {
					fadeEntry(0x15, 0, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
					fadeEntry(0x1d, 0, (GXColor){ 0x00, 0x00, 0x00, 0xFF });
					wp2->waitTimer = 0x4e;
				}
				else {
					fadeEntry(0x15, 0, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
					fadeEntry(0x22, 0, (GXColor){ 0x00, 0x00, 0x00, 0xFF });
					fadeSetNarrowFast();
					wp2->waitTimer = 0x17;
				}
				wp2->step++;
			}
			break;
		}
		case 1:
			wp2->waitTimer = wp2->waitTimer - 1;
			if (wp2->waitTimer == 0
				&& psbgm[0].streamId != 0xFFFFFFFF)
			{
				sound.streams[psbgm[0].streamId].flags &= ~0x8000;
			}
			if (fadeIsFinish()) {
				if (seqGetPrevSeq() != SEQ_LOAD) {
					fadeEntry(0x14, 0x1f4, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
					wp2->step++;
				}
				else {
					wp2->tex5PosY = 120.0f;
					wp2->tex6PosY = -170.0f;
					wp2->pulseCounter = 0;
					wp2->step = 3;
				}
			}
			break;
		case 2:
			wp2->tex5PosX = 0.0f;
			wp2->tex5PosY = intplGetValue(0xa, wp2->phaseFrame, 0x3c, 300.0f, 120.0f);
			wp2->waitTimer = wp2->waitTimer - 1;
			if (wp2->waitTimer == 0) {
				if (psbgm[0].streamId != 0xFFFFFFFF) {
					sound.streams[psbgm[0].streamId].flags &= ~0x8000;
				}
				if (seqGetPrevSeq() == SEQ_GAME_OVER) {
					psndSFXOn((const char*)0xab8);
				}
			}
			wp2->tex6PosX = 0.0f;
			wp2->tex6PosY = intplGetValue(0xb, wp2->phaseFrame, 0x3c, -300.0f, -170.0f);
			wp2->phaseFrame = wp2->phaseFrame + 1;
			if (wp2->phaseFrame > 0x3c) {
				fileAsyncf(FILE_TYPE_TPL, 0, TITLE_DEMO_TPL, getMarioStDvdRoot());
				animGroupBaseAsync(TITLE_PEACH, 0, 0);
				animGroupBaseAsync(TITLE_NAZO, 0, 0);
				animGroupBaseAsync(TITLE_GOLD, 0, 0);
				animGroupBaseAsync(TITLE_BIG, 0, 0);
				animGroupBaseAsync(TITLE_ROTEN, 0, 0);
				animGroupBaseAsync(TITLE_KURIBO, 0, 0);
				animGroupBaseAsync(TITLE_TYUSAN, 0, 0);
				wp2->pulseCounter = 0;
				wp2->step++;
			}
			break;
		case 3:
			wp2->fade += 0.1f;
			if (wp2->fade > 1.0f) {
				wp2->fade = 1.0f;
			}
			if (keyGetButtonTrg(0) & 0x1100) {
				psndSFXOn((const char*)8);
				wp2->step = 0xa;
				break;
			}
			if (++wp2->idleTimer > 0x474) {
				wp2->step = 0x14;
			}
			break;
		case 10:
			fadeEntry(0x3f, 0, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
			wp2->step++;
			// fallthrough
		case 11:
			if (fadeIsFinish()) {
				fadeEntry(0x1a, 0, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
				wp2->step++;
			}
			break;
		case 12:
			if (fadeIsFinish()) {
				fadeReset(4);
				seqSetSeq(SEQ_LOAD, NULL, NULL);
			}
			break;
		case 20:
			fadeEntry(0x21, 0, (GXColor){ 0x00, 0x00, 0x00, 0xFF });
			fadeEntry(0xa, 0x3e8, (GXColor){ 0x00, 0x00, 0x00, 0xFF });
			psndBGMOff_f_d(0x200, 0x3e8, 0);
			wp2->phaseFrame = 0;
			wp2->step++;
			break;
		case 21:
			wp2->tex5PosY = intplGetValue(0xb, wp2->phaseFrame, 0x3c, 120.0f, 400.0f);
			wp2->tex6PosY = intplGetValue(0xb, wp2->phaseFrame, 0x3c, -170.0f, -400.0f);
			wp2->phaseFrame = wp2->phaseFrame + 1;
			if (wp2->phaseFrame > 0x3c) {
				wp2->phaseFrame = 0x3c;
			}
			wp2->fade -= 0.1f;
			if (wp2->fade < 0.0f) {
				wp2->fade = 0.0f;
			}
			if (fadeIsFinish()) {
				fadeReset(1);
				if (pouchGetHP() <= 0) {
					pouchSetHP(1);
				}
				gp->areaFadeInType = 0xa;
				gp->areaFadeInDuration = 0;
				seqSetSeq(SEQ_MAP_CHANGE, TITLE_DMO00, NULL);
			}
			break;
	}

	if (wp2->step != 12 && work->state < 14) {
		dispEntry(CAMERA_3D, 0.0f, 0, disp1, NULL);
		dispEntry(CAMERA_3D_EFFECTB, 0.0f, 0, disp2, NULL);

		switch (wp2->debug) {
			case 0:
				if (keyGetButtonTrg(0) & ~0x400) {
					wp2->debug = 0;
				}
				else if (keyGetButtonTrg(0) & 0x400) {
					wp2->debug++;
				}
				break;
			case 1:
				if (keyGetButtonTrg(0) & ~0x200) {
					wp2->debug = 0;
				}
				else if (keyGetButtonTrg(0) & 0x200) {
					wp2->debug++;
				}
				break;
			case 2:
				if (keyGetButtonTrg(0) & ~0x20) {
					wp2->debug = 0;
				}
				else if (keyGetButtonTrg(0) & 0x20) {
					wp2->debug++;
				}
				break;
			case 3:
				if (keyGetButtonTrg(0) & ~0x200) {
					wp2->debug = 0;
				}
				else if (keyGetButtonTrg(0) & 0x200) {
					wp2->debug++;
				}
				break;
			case 4:
				if (keyGetButtonTrg(0) & ~0x800) {
					wp2->debug = 0;
				}
				else if (keyGetButtonTrg(0) & 0x800) {
					wp2->debug++;
				}
				break;
			case 5:
				if (keyGetButtonTrg(0) & ~0x40) {
					wp2->debug = 0;
				}
				else if (keyGetButtonTrg(0) & 0x40) {
					wp2->debug = -1;
				}
				break;
		}
	}
}

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

// The two Vec by-value argument temp blocks sit in the original slots (scale 0x38/0x14,
// intermediate 0x44/0x20, outgoing 0x50/0x2c), the pulse multiply matches, and the compiler's
// int-to-float magic double is pooled at 0xb0 and read through the rodata base (`lfd f2, 0xb0(r31)`)
// because the unit defines its .rodata.
void disp2(CameraId cameraId, void* param) {
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

	unk_80177008(5, (Vec){ wp2->tex5PosX, wp2->tex5PosY, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f },
	             (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });

	winTexSet(6, (Vec){ wp2->tex6PosX, wp2->tex6PosY, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f },
	          (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });

	wp2->pulseCounter += 1;
	pulse = (f32)sin((f64)(6.2831855f * (f32)wp2->pulseCounter / 120.0f));
	alpha = (s32)(wp2->fade * (459.0f * pulse));
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
		FontDrawString(60.0f, 220.0f, buildDateStr);
		FontDrawString(200.0f, 220.0f, buildTimeStr);
	}

	*camGetPtr(cameraId) = camBackup;
	GXSetProjectionv(projBackup);
	GXSetViewport(viewportBackup[0], viewportBackup[1], viewportBackup[2], viewportBackup[3], viewportBackup[4],
	              viewportBackup[5]);
}

// The first string block of the .rodata pool (reached by fixed offsets from the base).
const char str_demoTplPath[] = "%s/demo.tpl";
const char str_c_peach[] = "c_peach";
const char str_c_nazo[] = "c_nazo";
const char str_MOBJ_GoldenTreasureBox[] = "MOBJ_GoldenTreasureBox";
const char str_MOBJ_BigTreasureBox[] = "MOBJ_BigTreasureBox";
const char str_c_roten[] = "c_roten";
const char str_c_kuribo_n[] = "c_kuribo_n";
const char str_c_tyusan[] = "c_tyusan";


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
	MTXTrans(v17, (f32)wp2->pulseCounter / -200.0f, 0.0f, 0.0f);
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

// The 4-byte zero that ends the unit's .sdata (after wp2).
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_8040F2E4_sdata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset
