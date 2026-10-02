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
#include <musyx/musyx.h>
#include <string.h>
#include "sequence/seq_title.h"

extern SoundWork sound;
extern PaperSoundBGM psbgm[2];
extern GlobalWork* gp;

// This unit's .rodata blob, in the original's symbol order. The functions reach it through
// `base = (u8*)vec3_802c11a0` (see L_titleInit/titleMain); defining the objects here is what emits
// the bytes (the literals used before are gone once the functions use base+offset).
// The `extern Vec vec3_802c11a0[]` declaration in include/sequence/seq_title.h has no `const` and
// no element count, so the six original vec3_802c11a0..dc symbols are emitted as one 6-element
// array (the five tail symbols are not referenced by any function).
__declspec(section ".rodata") Vec vec3_802c11a0[6] = {
	{ 0.0f, 0.0f, 0.0f },
	{ 1.0f, 1.0f, 1.0f },
	{ 0.0f, 0.0f, 0.0f },
	{ 1.0f, 1.0f, 1.0f },
	{ 0.0f, 0.0f, 0.0f },
	{ 1.0f, 1.0f, 1.0f },
};
const char str_802C11E8[12] = "%s/demo.tpl";
const char str_802C11F4[8] = "c_peach";
const char str_802C11FC[8] = "c_nazo\000";
const char str_802C1204[24] = "MOBJ_GoldenTreasureBox\000";
const char str_802C121C[20] = "MOBJ_BigTreasureBox";
const char str_802C1230[8] = "c_roten";
const char str_802C1238[12] = "c_kuribo_n\000";
const char str_802C1244[12] = "c_tyusan\000\000\000";
const f64 double_to_int_802c1250 = 4503601774854144.0;
const f64 double_to_int_mask_802c1258 = 4503599627370496.0;
const char str_802C1260[24] = "OFF_d_maku_left_right\000\000";
const char str_802C1278[20] = "OFF_d_maku_center\000\000";
const char str_802C128C[8] = "dmo_00\000";
const char str_802C1294[16] = "%s/mariost.tpl\000";
const char str_802C12A4[12] = "BGM_DIGEST1";
const char str_802C12B0[12] = "BGM_TITLE1\000";

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

// Match shape: the original reaches its .rodata strings as offsets from one base register
// (r31 = vec3_802c11a0): `addi r4, r31, 0xf4/0x104/0x110`. This unit's .rodata strings live at
// those offsets; loading them through the same base reproduces the target's address form, while
// literal strings compile to a separate lis/addi pair per string.
void L_titleInit(SequenceWork* work) {
	u8* base = (u8*)vec3_802c11a0; // base of this unit's .rodata pool
	memset(wp2, 0, sizeof(TitleWork));
	wp2->texture = fileAllocf(4, (const char*)(base + 0xF4), getMarioStDvdRoot());
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
			psndBGMOn(0x200, (char*)(base + 0x104));
			if (psbgm[0].streamId != 0xFFFFFFFF) {
				sound.streams[psbgm[0].streamId].flags |= 0x8000;
			}
		}
		else {
			psndBGMOn(0x200, (char*)(base + 0x110));
			if (psbgm[0].streamId != 0xFFFFFFFF) {
				sound.streams[psbgm[0].streamId].flags |= 0x8000;
			}
		}
	}
	camGetPtr(CAMERA_2D)->flags &= ~0x100;
	camGetPtr(CAMERA_2D)->flags |= 0x400;
}

void titleMain(SequenceWork* work) {
	u8* base = (u8*)vec3_802c11a0; // base of this unit's .rodata pool

	switch (wp2->field_0x8) {
		case 0:
			if ((u32)psndBGMStartCheck(0) == 0) {
				break;
			}
			if (!animGroupBaseAsync((const char*)(base + 0xC0), 2, 0)) {
				break;
			}
			if (!animGroupBaseAsync((const char*)(base + 0xD8), 2, 0)) {
				break;
			}
			if (seqGetPrevSeq() == SEQ_LOAD) {
				fadeEntry(0x40, 0, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
				fadeEntry(0x1F, 0, (GXColor){ 0x00, 0x00, 0x00, 0xFF });
				wp2->field_0x10 = 120.0f;
				wp2->field_0x18 = -170.0f;
				*(s32*)(wp2->field_0x20 + 0x4) = 0;
			}
			else if (seqGetPrevSeq() == SEQ_GAME_OVER) {
				fadeEntry(0x15, 0, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
				fadeEntry(0x1D, 0, (GXColor){ 0x00, 0x00, 0x00, 0xFF });
				*(s32*)(wp2->field_0x20 + 0x4) = 0x4E;
			}
			else {
				fadeEntry(0x15, 0, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
				fadeEntry(0x22, 0, (GXColor){ 0x00, 0x00, 0x00, 0xFF });
				fadeSetNarrowFast();
				*(s32*)(wp2->field_0x20 + 0x4) = 0x17;
			}
			wp2->field_0x8 += 1;
			break;
		case 1:
			*(s32*)(wp2->field_0x20 + 0x4) -= 1;
			if (*(s32*)(wp2->field_0x20 + 0x4) == 0) {
				if (psbgm[0].streamId != 0xFFFFFFFF) {
					sound.streams[psbgm[0].streamId].flags &= ~0x8000;
				}
			}
			if (!fadeIsFinish()) {
				break;
			}
			if (seqGetPrevSeq() != SEQ_LOAD) {
				fadeEntry(0x14, 0x1F4, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
				wp2->field_0x8 += 1;
			}
			else {
				wp2->field_0x10 = 120.0f;
				wp2->field_0x18 = -170.0f;
				*(s32*)(wp2->field_0x20 + 0xC) = 0;
				wp2->field_0x8 = 3;
			}
			break;
		case 2:
			wp2->field_0xC = 0.0f;
			wp2->field_0x10 = intplGetValue(10, *(s32*)(wp2->field_0x20), 60, 300.0f, 120.0f);
			*(s32*)(wp2->field_0x20 + 0x4) -= 1;
			if (*(s32*)(wp2->field_0x20 + 0x4) == 0) {
				if (psbgm[0].streamId != 0xFFFFFFFF) {
					sound.streams[psbgm[0].streamId].flags &= ~0x8000;
				}
				if (seqGetPrevSeq() == SEQ_GAME_OVER) {
					psndSFXOn((const char*)0xAB8);
				}
			}
			wp2->field_0x14 = 0.0f;
			wp2->field_0x18 = intplGetValue(11, *(s32*)(wp2->field_0x20), 60, -300.0f, -170.0f);
			*(s32*)(wp2->field_0x20) += 1;
			if (*(s32*)(wp2->field_0x20) > 60) {
				fileAsyncf(4, 0, (const char*)(base + 0x48), getMarioStDvdRoot());
				animGroupBaseAsync((const char*)(base + 0x54), 0, 0);
				animGroupBaseAsync((const char*)(base + 0x5C), 0, 0);
				animGroupBaseAsync((const char*)(base + 0x64), 0, 0);
				animGroupBaseAsync((const char*)(base + 0x7C), 0, 0);
				animGroupBaseAsync((const char*)(base + 0x90), 0, 0);
				animGroupBaseAsync((const char*)(base + 0x98), 0, 0);
				animGroupBaseAsync((const char*)(base + 0xA4), 0, 0);
				*(s32*)(wp2->field_0x20 + 0xC) = 0;
				wp2->field_0x8 += 1;
			}
			break;
		case 3:
			wp2->field_0x1C += 0.1f;
			if (wp2->field_0x1C > 1.0f) {
				wp2->field_0x1C = 1.0f;
			}
			if (keyGetButtonTrg(0) & 0x1100) {
				psndSFXOn((const char*)8);
				wp2->field_0x8 = 0xA;
				break;
			}
			if (++*(s32*)(wp2->field_0x20 + 0x8) > 0x474) {
				wp2->field_0x8 = 0x14;
			}
			break;
		case 0xA:
			fadeEntry(0x3F, 0, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
			wp2->field_0x8 += 1;
			// fall through
		case 0xB:
			if (fadeIsFinish()) {
				fadeEntry(0x1A, 0, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
				wp2->field_0x8 += 1;
			}
			break;
		case 0xC:
			if (fadeIsFinish()) {
				fadeReset(4);
				seqSetSeq(SEQ_LOAD, NULL, NULL);
			}
			break;
		case 0x14:
			fadeEntry(0x21, 0, (GXColor){ 0x00, 0x00, 0x00, 0xFF });
			fadeEntry(0xA, 0x3E8, (GXColor){ 0x00, 0x00, 0x00, 0xFF });
			psndBGMOff_f_d(0x200, 0x3E8, 0);
			*(s32*)(wp2->field_0x20) = 0;
			wp2->field_0x8 += 1;
			break;
		case 0x15:
			wp2->field_0x10 = intplGetValue(11, *(s32*)(wp2->field_0x20), 60, 120.0f, 400.0f);
			wp2->field_0x18 = intplGetValue(11, *(s32*)(wp2->field_0x20), 60, -170.0f, -400.0f);
			*(s32*)(wp2->field_0x20) += 1;
			if (*(s32*)(wp2->field_0x20) > 60) {
				*(s32*)(wp2->field_0x20) = 60;
			}
			wp2->field_0x1C -= 0.1f;
			if (wp2->field_0x1C < 0.0f) {
				wp2->field_0x1C = 0.0f;
			}
			if (fadeIsFinish()) {
				fadeReset(1);
				if (pouchGetHP() <= 0) {
					pouchSetHP(1);
				}
				gp->areaFadeInType = 10;
				gp->areaFadeInDuration = 0;
				seqSetSeq(SEQ_MAP_CHANGE, (const char*)(base + 0xEC), NULL);
			}
			break;
	}

	if (wp2->field_0x8 != 0xC && work->state < 0xE) {
		dispEntry(4, 0.0f, 0, disp1, NULL);
		dispEntry(7, 0.0f, 0, disp2, NULL);
		switch (wp2->debug) {
			case 0:
				if (keyGetButtonTrg(0) & ~0x400) {
					wp2->debug = 0;
					break;
				}
				if (keyGetButtonTrg(0) & 0x400) {
					wp2->debug++;
				}
				break;
			case 1:
				if (keyGetButtonTrg(0) & ~0x200) {
					wp2->debug = 0;
					break;
				}
				if (keyGetButtonTrg(0) & 0x200) {
					wp2->debug++;
				}
				break;
			case 2:
				if (keyGetButtonTrg(0) & ~0x20) {
					wp2->debug = 0;
					break;
				}
				if (keyGetButtonTrg(0) & 0x20) {
					wp2->debug++;
				}
				break;
			case 3:
				if (keyGetButtonTrg(0) & ~0x200) {
					wp2->debug = 0;
					break;
				}
				if (keyGetButtonTrg(0) & 0x200) {
					wp2->debug++;
				}
				break;
			case 4:
				if (keyGetButtonTrg(0) & ~0x800) {
					wp2->debug = 0;
					break;
				}
				if (keyGetButtonTrg(0) & 0x800) {
					wp2->debug++;
				}
				break;
			case 5:
				if (keyGetButtonTrg(0) & ~0x40) {
					wp2->debug = 0;
					break;
				}
				if (keyGetButtonTrg(0) & 0x40) {
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

// Match shape: the original copies the zero Vec into the by-value argument temp and then
// overwrites x/y there (so the intermediate temp sits directly below the outgoing Vec home).
// Taking the zero Vec by value reproduces that; a pointer/return pair allocates the temp in
// the wrong slot (97.14%). The parameter order (zero, y, x) is the one that puts the wp2
// x/y loads (field_0xC to f1, field_0x10 to f0) the way the original does.
static inline Vec disp2Vec(Vec v, f32 y, f32 x) {
	v.x = x;
	v.y = y;
	return v;
}

// The two Vec by-value argument temp blocks sit in the original slots (scale 0x38/0x14,
// intermediate 0x44/0x20, outgoing 0x50/0x2c) and the pulse multiply matches. Two diffs remain:
//  (1) the int-to-float magic double: the target loads it from the rodata base
//      (`lfd f2, 0xb0(r31)`, r31 = vec3_802c11a0); MWCC here emits its own address
//      (`lis r3, @440@ha; lfd f2, @440@l(r3)`) even now that the whole .rodata blob
//      (vec3_802c11a0 .. str_802C12B0, incl. double_to_int_802c1250) is defined in this file;
//      adding `#pragma pool_data on` compiled to the same bytes. The compiler's generated double
//      does not join the blob pool.
//  (2) the FontDrawString calls: the target evaluates the two floats before the string
//      pointer (`lfs f1; lfs f2; lwz r3`), but include/manager/fontmgr.h declares
//      FontDrawString(const char*, f32, f32), so MWCC evaluates the string pointer first. Only the
//      first of the two calls differs (the second already matches). The callee's ABI registers are
//      the same either way; only the call-site order differs, and the header is shared with other
//      files so it cannot be reordered here.
// NONMATCHING (97.32%): the two blockers above; the Vec arg temps and pulse multiply match.
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

	unk_80177008(5, disp2Vec(*(Vec*)(base + 0x18), wp2->field_0x10, wp2->field_0xC), *(Vec*)(base + 0x24),
	             (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });

	winTexSet(6, disp2Vec(*(Vec*)(base + 0x30), wp2->field_0x18, wp2->field_0x14), *(Vec*)(base + 0x3c),
	          (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });

	*(s32*)((u8*)wp2 + 0x2C) += 1; // TitleWork pulse counter, no field declared in the header
	pulse = (f32)sin((f64)(6.2831855f * (f32)*(s32*)((u8*)wp2 + 0x2C) / 120.0f));
	alpha = (s32)(wp2->field_0x1C * (459.0f * pulse));
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
