#include "motion/mot_plane.h"
#include "mot/mot_plane.h"
#include "evt/evt_cmd.h"
#include "evt/evt_snd.h"
#include "mario/mario.h"
#include "memory.h"
#include <string.h>

EVT_BEGIN(sound_evt)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_AIRPLANE1"), LW(0), LW(1), LW(2), 0)
WAIT_FRAMES(12)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_AIRPLANE1"), LW(0), LW(1), LW(2), 0)
WAIT_FRAMES(12)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_AIRPLANE1"), LW(0), LW(1), LW(2), 0)
WAIT_FRAMES(12)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_AIRPLANE1"), LW(0), LW(1), LW(2), 0)
RETURN
EVT_END()

s32 marioPlaneGetFlyingDir(void) {
	MarioWork* mario = marioGetPtr();

	if (mario->currMotionId != MARIO_MOTION_PLANE) {
		return 0;
	}
	if (((MotionPlaneData*)mario->motStruct)->flags & 1) {
		return 1;
	}
	return -1;
}

s32 marioGetPlaneStatus(void) {
	MarioWork* mario = marioGetPtr();

	if (mario->currMotionId != MARIO_MOTION_PLANE) {
		return 0;
	}
#ifdef MUST_MATCH
	// The original compares currSubMotionId with cmpwi (signed); the header types it u32.
	if ((s32)mario->currSubMotionId >= 0xC && (s32)mario->currSubMotionId < 0x14) {
#else
	if (mario->currSubMotionId >= 0xC && mario->currSubMotionId < 0x14) {
#endif
		return 2;
	}
	return 1;
}

s32 mario_plane_cancel(void) {
	MarioWork* mario = marioGetPtr();

	if (mario->currMotionId != MARIO_MOTION_PLANE) {
		return 0;
	}
	if ((s32)mario->currSubMotionId >= 0xC && (s32)mario->currSubMotionId < 0x14) {
		mario->currSubMotionId = 0x32;
		*(f32*)mario->field_0x2AC = mario->position.y;
		return 1;
	}
	return 0;
}

// Paper animation names indexed by mario's current color (paper_plane@sda21 in the asm).
const char* paper_plane[4] = {
    "p_hikohki",
    "p_hikohki_g",
    "p_hikohki_y",
    "p_hikohki_v",
};

void marioForcePlaneAnime(void) {
    MarioWork* mario = marioGetPtr();

    marioPaperOff();
    marioChgPaper(NULL);
    marioPaperOn(paper_plane[marioGetColor()]);
    marioChgPaper("PM_P_1B");
    marioChgPose("M_Z_1");
    mario->dispFlags |= 0x8;
}

// NONMATCHING (3.48%): 6 instructions differ, 891 missing, plus register differences.
void mot_plane(void) {
	MarioWork* mario;
	MotionPlaneData* data;

	mario = marioGetPtr();
	if (mario->trigFlags & 1) {
		mario->trigFlags &= ~1;
		mario->motStruct = __memAlloc(HEAP_DEFAULT, sizeof(MotionPlaneData));
		memset(mario->motStruct, 0, sizeof(MotionPlaneData));
		data = marioGetPtr()->motStruct;
		data->field_0x28 = -1;
		mario->currSubMotionId = 0;
	}
	switch (mario->currSubMotionId) {
		case 0:
			//inline start?
			mario = marioGetPtr();

			//inline end?
			//marioPreJump();
			break;
	}
}

// NONMATCHING (2.15%): 4 instructions differ, 844 missing, plus register differences.
static void flyMain(void) {
	MarioWork* mario;

	mario = marioGetPtr();
	switch (mario->currSubMotionId) {
		case 0xC:
			if (mario->wCamVal1 >= mario->position.y) {
				mario->currSubMotionId = 14;
			}
			//inlined?
			mario = marioGetPtr();
	}
}
