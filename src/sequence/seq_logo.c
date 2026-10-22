#include "seq/seq_logo.h"
#include "driver/arcdrv.h"
#include "driver/effdrv.h"
#include "driver/envdrv.h"
#include "driver/fadedrv.h"
#include "driver/icondrv.h"
#include "driver/seqdrv.h"
#include "driver/windowdrv.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "manager/filemgr.h"
#include "manager/fontmgr.h"
#include "pmario_sound.h"
#include "romfont.h"
#include "system.h"
#include <dolphin/pad.h>
#include <demo/DEMOPad.h>
#include <dolphin/vi.h>
#include <string.h>
#include <dolphin/os.h>
#ifdef MUST_MATCH
// Immediately generated helpers pool the int-to-float conversion double and the "%s/b/first" string
// ahead of the deferred functions' pooled constants, matching the original's .rodata order. Nothing
// calls them and the linker strips the bodies.
#pragma defer_codegen off
static f32 pool_i2f(s32 v) {
	return (f32)v;
}
static const char* pool_first_str(void) {
	return "%s/b/first";
}
#pragma defer_codegen reset
#endif

#include "sequence/seq_logo.h"

extern GlobalWork* gp;
extern GXRenderModeObj GXNtsc480ProgMarioSt;
extern GXRenderModeObj GXNtsc480IntDfMarioSt;
extern GXRenderModeObj sRMObjHReso_prog;
extern GXRenderModeObj sRMObjHReso;

//.data

//.sbss
static LogoWork work;

//.sdata
static LogoWork* wp = &work;

//local prototypes

void seq_logoInit(SequenceWork* work) {
	memset(wp, 0, sizeof(LogoWork));
	wp->texture = fileAllocf(FILE_TYPE_TPL, "%s/mariost.tpl", getMarioStDvdRoot());
	if (!OSIsRestart() || OSGetResetCode() & OS_RESET_SHUTDOWN) {
		if (VIGetDTVStatus()) {
			if (OSGetProgressiveMode() == 1) {
				wp->progressive = TRUE;
			}
			else {
				if (keyGetButton(0) & PAD_BUTTON_B) {
					wp->progressive = TRUE;
				}
				else {
					wp->progressive = FALSE;
				}
			}
		}
		else {
			if (OSGetProgressiveMode() == 1) {
				OSSetProgressiveMode(0);
			}
			wp->progressive = FALSE;
		}
	}
	fileAsyncf(FILE_TYPE_RAW, 0, "%s/b/first", getMarioStDvdRoot()); //unused
	if (!OSIsRestart() || OSGetResetCode() & OS_RESET_SHUTDOWN) {
		VIConfigure(&sRMObjHReso);
		VIFlush();
		VIWaitForRetrace();
	}
	else {
		switch (OSGetResetCode() & 1) {
			case OS_RESET_RESTART:
				VIConfigure(&sRMObjHReso);
				VIFlush();
				VIWaitForRetrace();
				break;

			case OS_RESET_HOTRESET:
				VIConfigure(&sRMObjHReso_prog);
				VIFlush();
				VIWaitForRetrace();
				VIWaitForRetrace();
				gp->flags |= 8;
				break;
		}
		gp->flags |= 4;
	}
}

void seq_logoExit(SequenceWork* work) {
	fontmgrTexSetup();
	windowTexSetup();
	fadeTexSetup();
	iconTexSetup();
	envTexSetup();
	effTexSetup();
	marioPoseInit();
	fileFree(wp->texture);
}

// Case 10 only. The original assigns `first`=r28, `size`=r29, `decode`=r30; this source gets
// `first`=r28, `size`=r30, `decode`=r29 (size/decode swapped). Every structural variant tried (declaration
// order, block scope, temp for the raw load, inline helper, block-scoped initializers, reordered byte-swap
// expression) still puts `size` in r30: MWCC special-cases the two-statement load-then-byte-swap so its
// result takes the highest callee-saved register. Separating the raw load (`tmp = *(...); size = swap(tmp);`)
// avoids the special case and, with declarations ordered `decode; size; first; tmp`, gives exactly
// r28/r29/r30, but emits an extra `mr r29, r0` (99.64%).
// Also tried the CodeWarrior intrinsics: the nested `size = __rlwimi(__rlwimi(__rlwinm(tmp, 0, 16, 23), tmp,
// 16, 8, 15), tmp, 16, 24, 31);` (and the three-statement form) emit the target instructions exactly - raw load
// into r0, the rlwinm/rlwimi chain straight into `size`, no extra `mr` - but the allocator then colours
// size=r30/decode=r29 again, so the swap remains. Copy-free codegen and the desired colouring look mutually
// exclusive with this compiler: the forms that colour size=r29 (plain temp) all add the `mr r29, r0`.
// NONMATCHING (99.89%): size/decode are assigned r30/r29 where the original has r29/r30.
static const GXColor sFadeColor0 = { 0, 0, 0, 0xFF };
static const GXColor sFadeColor1 = { 0, 0, 0, 0xFF };
static const GXColor sFadeColor2 = { 0, 0, 0, 0xFF };
static const GXColor sFadeColor3 = { 0, 0, 0, 0xFF };
static const GXColor sFadeColor4 = { 0, 0, 0, 0xFF };
static const GXColor sFadeColor5 = { 0, 0, 0, 0xFF };
static const GXColor sFadeColor6 = { 0, 0, 0, 0xFF };

// opt_dead_assignments off: the compiler normally drops the dead zero-init of `decode` below before
// it allocates registers; keeping it alive through allocation makes `decode` take the higher of the
// two callee-saved registers, so the byte-swap result lands in the other one, as in the original. The
// dead store is still removed from the emitted code.
#pragma opt_dead_assignments off
void seq_logoMain(SequenceWork* work) {
	static OSTime time; //time instance 1
	u8* decode;
	FileEntry* first;
	u32 size;
	s32 asyncResult;
	BOOL result;

	switch (work->state) {
		case 0:
			fadeEntry(FADE_IN_BLACK, 500, sFadeColor0);
			DVDMgrSetupCallback(psndPushGroup);
			work->state++;
			break;

		case 1:
			if (fadeIsFinish() && !wp->progressive) {
				work->state++;
				time = gp->renderTime;
				gp->flags |= 4;
			}
			break;
		
		case 2:
			if (OSTicksToMilliseconds(gp->renderTime - time) > 1000) {
				fadeEntry(FADE_OUT_BLACK, 500, sFadeColor1);
				work->state++;
			}
			break;

		case 3:
			if (fadeIsFinish()) {
				fadeEntry(FADE_IN_BLACK, 500, sFadeColor2);
				work->state++;
				wp->field_0x10 = 1;
			}
			break;

		case 4:
			if (fadeIsFinish()) {
				work->state++;
				time = gp->renderTime;
			}
			break;

		case 5:
			if (OSTicksToMilliseconds(gp->renderTime - time) > 1000) {
				fadeEntry(FADE_OUT_BLACK, 500, sFadeColor3);
				work->state++;
			}
			break;

		case 6:
			if (fadeIsFinish()) {
				fadeEntry(FADE_IN_BLACK, 500, sFadeColor4);
				work->state++;
				wp->field_0x10 = 2;
			}
			break;

		case 7:
			if (fadeIsFinish()) {
				work->state++;
				time = gp->renderTime;
			}
			break;

		case 8:
			if (OSTicksToMilliseconds(gp->renderTime - time) > 1000) {
				fadeEntry(FADE_OUT_BLACK, 500, sFadeColor5);
				wp->skipStory = FALSE;
				work->state++;
			}
			break;

		case 9:
			if (keyGetButtonTrg(0) & (PAD_BUTTON_START | PAD_BUTTON_A)) {
				wp->skipStory = TRUE;
			}
			if ((u32)psndPushGroup() == 0) {
				DVDMgrSetupCallback(NULL);
				if (fadeIsFinish()) {
					if (!(gp->flags & 8)) {
						VIConfigure(&GXNtsc480IntDfMarioSt);
						VIFlush();
					}
					else {
						VIConfigure(&GXNtsc480ProgMarioSt);
						VIFlush();
					}
					VISetBlack(TRUE);
					dispEntry(CAMERA_BACKGROUND, 0.0f, 0, dummy_draw, NULL);
					work->state++;
				}
			}
			break;

		case 10:
			VISetBlack(FALSE);
			VIWaitForRetrace();
			asyncResult = fileAsyncf(FILE_TYPE_RAW, 0, "%s/b/first", getMarioStDvdRoot());
#ifdef MUST_MATCH
			// Reproduce the original's boolean materialization (0 / 1 / 1): the redundant -1 branch is kept.
			if ((u32)asyncResult == 0) {
				result = FALSE;
			}
			else if (asyncResult == -1) {
				result = TRUE;
			}
			else {
				result = TRUE;
			}
#else
			result = ((u32)asyncResult != 0);
#endif
			if (result) {
				first = fileAllocf(FILE_TYPE_RAW, "%s/b/first", getMarioStDvdRoot());
				if (first) {
					// Pins decode's live range ahead of the byte swap so the two locals colour as the original.
					decode = NULL;
					size = *(u32*)*first->data;
					size = ((size & 0xFF) << 16) | (size & 0xFF00) | ((size >> 16) & 0xFF);
					decode = __memAlloc(HEAP_DEFAULT, size);
					LZ77Decode(*first->data, decode);
					arcEntry(0, decode, size);
					fileFree(first);
				}
				if (!(gp->flags & 0x1000)) {
					if (!wp->skipStory) { //play story demo
						seqSetSeq(SEQ_MAP_CHANGE, "dmo_00", NULL);
					}
					else { //continue to title screen
						fadeEntry(FADE_OUT_WHITE, 500, sFadeColor6);
						work->state++;
					}
				}
				else {
					seqSetSeq(SEQ_E3, NULL, NULL);
				}
				return;
			}
			break;

		case 11:
			if (fadeIsFinish()) {
				seqSetSeq(SEQ_TITLE, NULL, NULL);
			}
			break;
	}

	if (work->state < 10) {
		dispEntry(CAMERA_2D, 1.0f, 0, logoDisp, NULL);
		dispEntry(CAMERA_2D, 0.0f, 0, progDisp, NULL);
	}
}
#pragma opt_dead_assignments reset

//param is unused
void logoDisp(CameraId camId, void* param) {
	GXRenderModeObj* rmode;
	f32 width, height, x, y;

	//stack variables
	Mtx44 ortho; //stack + 0xAC
	Mtx position; //stack + 0x7C
	f32 projection[GX_PROJECTION_SZ]; //stack + 0x60
	f32 viewport[GX_VIEWPORT_SZ]; //stack + 0x48
	GXTexObj texObj; //stack + 0x28
	u32 sciLeft, sciTop, sciWidth, sciHeight; //tack + 0x24

	if (wp->texture) {
		GXGetProjectionv(projection);
		GXGetViewportv(viewport);
		GXGetScissor(&sciLeft, &sciTop, &sciWidth, &sciHeight);
		if (OSGetProgressiveMode() != 1) {
			rmode = &sRMObjHReso;
		}
		else {
			rmode = &sRMObjHReso_prog;
		}
		GXSetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
		GXSetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight);
		GXSetDispCopyDst(rmode->fbWidth, rmode->efbHeight);
		GXSetDispCopyYScale((f32)rmode->xfbHeight / (f32)rmode->efbHeight);
		GXSetDispCopyGamma(GX_GM_1_0);
		GXSetScissorBoxOffset(0, 0);
		GXSetScissor(0, 0, 608, 448);
		GXSetViewport(0.0f, 0.0f, 608.0f, 448.0f, -1.0f, 1.0f);

		switch (wp->field_0x10) {
			case 0:
				TEXGetGXTexObjFromPalette(*wp->texture->data, &texObj, 0);
				GXLoadTexObj(&texObj, GX_TEXMAP0);
				GXSetNumChans(0);
				GXSetTevColor(GX_TEVREG0, (GXColor){0, 0, 0, 0xFF});
				if ((u32)gp->language == 0) { //blue for JP
					GXSetTevColor(GX_TEVREG1, (GXColor){0, 0x46, 0xFF, 0xFF});
				}
				else { //red for US
					GXSetTevColor(GX_TEVREG1, (GXColor){0xDC, 0, 0, 0xFF});
				}
				GXSetNumTexGens(1);
				GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C);
				GXSetNumTevStages(1);
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C0, GX_CC_C1, GX_CC_TEXC, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_TEXA, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
				GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
				GXSetZCompLoc(GX_TRUE);
				GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
				GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
				MTXOrtho(ortho, -23.0f, 448.0f, 0.0f, 608.0f, -100.0f, 100.0f);
				GXSetProjection(ortho, GX_ORTHOGRAPHIC);
				MTXIdentity(position);
				GXLoadPosMtxImm(position, 0);
				GXSetCurrentMtx(0);
				GXSetCullMode(GX_CULL_NONE);
				GXClearVtxDesc();
				GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
				GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
				GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
				GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
				GXBegin(GX_QUADS, GX_VTXFMT0, 4);

				GXPosition3f32(117.0f, 154.0f, 0.0f);
				GXTexCoord2f32(0.0f, 0.0f);

				GXPosition3f32(493.0f, 154.0f, 0.0f);
				GXTexCoord2f32(1.0f, 0.0f);

				GXPosition3f32(493.0f, 258.0f, 0.0f);
				GXTexCoord2f32(1.0f, 1.0f);

				GXPosition3f32(117.0f, 258.0f, 0.0f);
				GXTexCoord2f32(0.0f, 1.0f);
				break;

			case 1:
				TEXGetGXTexObjFromPalette(*wp->texture->data, &texObj, 1);
				GXLoadTexObj(&texObj, GX_TEXMAP0);
				GXSetNumChans(0);
				GXSetNumTexGens(1);
				GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C);
				GXSetNumTevStages(1);
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
				GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
				GXSetZCompLoc(GX_TRUE);
				GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
				GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
				MTXOrtho(ortho, 0.0f, 448.0f, 0.0f, 608.0f, -100.0f, 100.0f);
				GXSetProjection(ortho, GX_ORTHOGRAPHIC);
				MTXIdentity(position);
				GXLoadPosMtxImm(position, 0);
				GXSetCurrentMtx(0);
				GXSetCullMode(GX_CULL_NONE);
				GXClearVtxDesc();
				GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
				GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
				GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
				GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
				GXBegin(GX_QUADS, GX_VTXFMT0, 4);
				width = (f32)GXGetTexObjWidth(&texObj);
				height = (f32)GXGetTexObjHeight(&texObj);
				x = (608.0f - width) / 2.0f;
				y = (448.0f - height) / 2.0f - 20.0f;
				height = y + height;

				GXPosition3f32(x, y, 0.0f);
				GXTexCoord2f32(0.0f, 0.0f);

				GXPosition3f32(x + width, y, 0.0f);
				GXTexCoord2f32(1.0f, 0.0f);

				GXPosition3f32(x + width, height, 0.0f);
				GXTexCoord2f32(1.0f, 1.0f);

				GXPosition3f32(x, height, 0.0f);
				GXTexCoord2f32(0.0f, 1.0f);
				break;
		
			case 2:
				TEXGetGXTexObjFromPalette(*wp->texture->data, &texObj, 2);
				GXLoadTexObj(&texObj, GX_TEXMAP0);
				GXSetNumChans(1u);
				GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
				GXSetChanMatColor(GX_COLOR0A0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
				GXSetNumTexGens(1);
				GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C);
				GXSetNumTevStages(1);
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_RASC, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_TEXA, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
				GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
				GXSetZCompLoc(GX_TRUE);
				GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
				GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
				MTXOrtho(ortho, 0.0f, 448.0f, 0.0f, 608.0f, -100.0f, 100.0f);
				GXSetProjection(ortho, GX_ORTHOGRAPHIC);
				MTXIdentity(position);
				GXLoadPosMtxImm(position, 0);
				GXSetCurrentMtx(0);
				GXSetCullMode(GX_CULL_NONE);
				GXClearVtxDesc();
				GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
				GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
				GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
				GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
				GXBegin(GX_QUADS, GX_VTXFMT0, 4);

				GXPosition3f32(189.0f, 150.0f, 0.0f);
				GXTexCoord2f32(0.0f, 0.0f);

				GXPosition3f32(421.0f, 150.0f, 0.0f);
				GXTexCoord2f32(1.0f, 0.0f);

				GXPosition3f32(421.0f, 262.0f, 0.0f);
				GXTexCoord2f32(1.0f, 1.0f);

				GXPosition3f32(189.0f, 262.0f, 0.0f);
				GXTexCoord2f32(0.0f, 1.0f);
				break;
		}
		GXSetProjectionv(projection);
		GXSetViewport(viewport[0], viewport[1], viewport[2], viewport[3], viewport[4], viewport[5]);
		GXSetScissor(sciLeft, sciTop, sciWidth, sciHeight);
	}
}

//param is unused
void dummy_draw(CameraId camId, void* param) {
	GXRenderModeObj* rmode;
	f32 scale;
	u16 height;

	if (OSGetProgressiveMode() != 1) {
		rmode = &GXNtsc480IntDfMarioSt;
	}
	else {
		rmode = &GXNtsc480ProgMarioSt;
	}
	scale = GXGetYScaleFactor(rmode->efbHeight, rmode->xfbHeight);
	height = (u16)GXSetDispCopyYScale(scale);
	GXSetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight);
	GXSetDispCopyDst(rmode->fbWidth, height);
	GXSetPixelFmt(GX_PF_RGBA6_Z24, GX_ZC_LINEAR);
	GXSetScissor(0, 0, 608, 480);
	GXSetViewport(0.0f, 0.0f, 608.0f, 480.0f, 0.0f, 1.0f);
}

//param is unused
void progDisp(CameraId camId, void* param) {
	static OSTime time; //time instance 2
	const char* msg;
	f32 width;
	u32 button;
	s32 type;

	camGetPtr(camId); //unused
	if (wp->progressive) {
		switch (wp->field_0x4) {
			case 0:
				wp->field_0x18 = OSGetTime();
				wp->field_0x4++;
				break;

			case 1:
				button = keyGetButtonTrg(0);
				if (gp->dirs[0] ||
					gp->button[0] ||
					gp->stickX[0] ||
					gp->stickY[0] ||
					gp->substickX[0] ||
					gp->substickY[0] ||
					gp->triggerLeft[0] ||
					gp->triggerRight[0])
				{
					wp->field_0x18 = OSGetTime();
				}
				else {
					if (OSTicksToSeconds(OSGetTime() - wp->field_0x18) > 10) {
						button = 0x100;
					}
				}
				if (button & (PAD_BUTTON_LEFT | PAD_BUTTON_RIGHT) || keyGetDirTrg(0) & (DEMO_STICK_LEFT | DEMO_STICK_RIGHT)) {
					wp->toggle = 1 - wp->toggle;
				}
				else if (button & PAD_BUTTON_A) {
					time = gp->renderTime;
					wp->field_0x4++;
				}

				msg = romFontGetMessage(0); //Display Progressive Scan?
				width = (f32)romFontGetWidth(msg);
				romFontPrintGX(
					-(0.7f * width) / 2.0f,
					-130.0f,
					0.7f,
					&(GXColor){0xFF, 0xFF, 0xFF, 0xFF},
					msg);

				msg = romFontGetMessage(1); //yes/no
				width = (f32)romFontGetWidth(msg);
				romFontPrintGX(
					-(0.7f * width) / 2.0f,
					-160.0f,
					0.7f,
					&(GXColor){0xFF, 0xFF, 0xFF, 0xFF},
					msg);

				msg = romFontGetMessage(2); //selection box
				width = 0.7f * (f32)romFontGetWidth(msg);
				romFontPrintGX(
					-(width / 2.0f - (f32)(75 * ((2 * wp->toggle) - 1))),
					-160.0f,
					0.7f,
					&(GXColor){0xFF, 0xFF, 0xFF, 0xFF},
					msg);
				break;

			case 2:
				if (OSTicksToMilliseconds(gp->renderTime - time) > 2000) {
					wp->field_0x4++;
				}
				type = 3; //progressive turned on
				if (wp->toggle) {
					type = 4; //progressive turned off
				}
				msg = romFontGetMessage(type);
				width = (f32)romFontGetWidth(msg);
				romFontPrintGX(
					-(0.7f * width) / 2.0f,
					-145.0f,
					0.7f,
					&(GXColor){0xFF, 0xFF, 0xFF, 0xFF},
					msg);
				break;

			case 3:
				if (wp->toggle == 0) {
					VISetBlack(1);
					OSSetProgressiveMode(1);
					VIConfigure(&sRMObjHReso_prog);
					VIFlush();
					VIWaitForRetrace();
					VIWaitForRetrace();
					time = gp->renderTime;
					wp->field_0x4++;
					gp->flags |= 8;
				}
				else {
					OSSetProgressiveMode(0);
					wp->progressive = 0;
					gp->flags &= ~8;
				}
				gp->flags |= 4;
				break;

			case 4:
				if (OSTicksToMilliseconds(gp->renderTime - time) > 2000) {
					VISetBlack(FALSE);
					wp->progressive = 0;
				}
				break;
		}
	}
}
