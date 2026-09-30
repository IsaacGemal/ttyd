#include "seq/seq_title.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "mario/mariost.h"
#include "pmario_sound.h"
#include "sound.h"
#include "system.h"
#include "win/win_main.h"
#include <musyx/musyx.h>
#include <string.h>
#include "sequence/seq_title.h"

extern SoundWork sound;
extern PaperSoundBGM psbgm[2];
extern GlobalWork* gp;

TitleWork work2;
TitleWork* wp2 = &work2;

//local prototypes

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
