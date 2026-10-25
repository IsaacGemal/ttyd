#pragma once

#include "evt/evt_cmd.h"
#include "driver/dispdrv.h"
#include "driver/extdrv.h"
#include "manager/filemgr.h"
#include "memory.h"

typedef struct SightWork {
	FileEntry* file; // 0x0
	s32 levels[5];  // 0x4, the last five |samples| SoundAIDMACallback wrote, shifted in every 4 frames; drawn as bars of level / 300 (at most 7)
	f32 rot;        // 0x18, drawBG's rotation for the four sight quarters
	s32 state;      // 0x1C
	s32 leftCount;  // 0x20, drawn by drawNum beside the left arrow; +1 every 4 frames while that arrow blinks
	s32 rightCount; // 0x24, the same for the right arrow
} SightWork;

typedef struct PoseEntry {
	char name[8]; // 0x0
	f32 duration;   // 0x8, frames per animation loop (pose_main)
	s32 firstTexId; // 0xC, texId of the loop's first frame
	s32 texCount;   // 0x10, frames in the loop
} PoseEntry;      // 0x14

typedef struct PoseState {
	u8 state;       // 0x0, ext_main's switch; 100 hops toward target
	u8 nextState;   // 0x1, the state set when the hop lands
	u8 pad_2[2];    // 0x2
	Vec target;     // 0x4, the hop's landing point
	f32 speed;      // 0x10, horizontal distance per hop frame
	f32 gravity;    // 0x14, taken off velY each hop frame
	s16 framesLeft; // 0x18, hop frames left; the pose lands on target when it drops below 0
	u8 pad_1A[2];   // 0x1A
	f32 velY;       // 0x1C, added to the pose's y each hop frame
	s32 entryIdx;   // 0x20, the PoseData.entries animation the pose plays (pose_change, pose_main)
	s32 field_0x24; // 0x24
	u64 time;     // 0x28
} PoseState;      // 0x30

typedef struct PoseData {
	u16 flags;            // 0x0
	u8 pad_2[2];          // 0x2
	s32 poseCount;        // 0x4, extGetPoseNum(): the length of states
	PoseEntry entries[8]; // 0x8
	s32 count;            // 0xA8
	u8 pad_AC[4];         // 0xAC
	PoseState states[1];  // 0xB0
} PoseData;

extern smartEntry* lp[1];

USER_FUNC(bom1000_jump);
USER_FUNC(bom1000_msg);
void ext_dispent_minnnanokoe(void);  // TODO: real signature
void ext_disp_minnnanokoe(CameraId cameraId, void* param);
void jump_minnnanokoe(s32 index, f32 f);
void ext_init_minnnanokoe(void);  // TODO: real signature
USER_FUNC(make_ext_table_minnnanokoe);
USER_FUNC(taihou_r);
void jump(s32 index, s32 a, f32 x, f32 y, f32 z, f32 p, f32 q);
USER_FUNC(make_ext_table);
USER_FUNC(bom_fall);
USER_FUNC(bom_flag);
USER_FUNC(sight_dir);
USER_FUNC(sight);
void disp_sight(CameraId cameraId, void* param);
void drawNum(s32 num, s32 count, f32 a, f32 b, f32 c);
void drawBG(s32 id, f32 x, f32 y, f32 scaleX, f32 scaleY, f32 rot);
EVT_DEF(evt_door_1);
EVT_DEF(evt_bom_01_karakara);
EVT_DEF(evt_bom1000);
EVT_DEF(gamen_shake);
EVT_DEF(gamen_shake2);
EVT_DEF(mario_tobideru);
EVT_DEF(mario_shot);
EVT_DEF(effect_delete);
EVT_DEF(evt_depth);
EVT_DEF(cam_outside);
EVT_DEF(cam_inside);
EVT_DEF(cam_inside2);
EVT_DEF(effect_1);
EVT_DEF(open_hatch);
EVT_DEF(init_nitoro);
EVT_DEF(talk_nitoro);
EVT_DEF(init_pentrit);
EVT_DEF(talk_pentrit);
EVT_DEF(init_surary);
EVT_DEF(talk_surary);
EVT_DEF(init_murabito_d);
EVT_DEF(talk_murabito_d);
EVT_DEF(bom1000_minnnanokoe);
EVT_DEF(bom_01_init_evt);
