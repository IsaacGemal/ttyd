#pragma once

#include "evt/evt_cmd.h"
#include "driver/dispdrv.h"

typedef struct ExtTableEntry {
	const char* name;
	const char* type;
	f32 value;
} ExtTableEntry;

typedef struct SightWork {
	void* file;     // 0x0
	s32 field_4[6]; // 0x4
	s32 state;      // 0x1C
	s32 field_20;   // 0x20
	s32 field_24;   // 0x24
} SightWork;

typedef struct PoseEntry {
	char name[8]; // 0x0
	f32 field_8;  // 0x8
	s32 field_C;  // 0xC
	s32 field_10; // 0x10
} PoseEntry;      // 0x14

typedef struct PoseState {
	u8 field_0;   // 0x0
	u8 field_1;   // 0x1
	u8 pad_2[2];  // 0x2
	Vec field_4;  // 0x4
	f32 field_10; // 0x10
	f32 field_14; // 0x14
	s16 field_18; // 0x18
	u8 pad_1A[2]; // 0x1A
	f32 field_1C; // 0x1C
	s32 field_20; // 0x20
	s32 field_24; // 0x24
	u64 time;     // 0x28
} PoseState;      // 0x30

typedef struct PoseData {
	u16 flags;            // 0x0
	u8 pad_2[6];          // 0x2
	PoseEntry entries[8]; // 0x8
	s32 count;            // 0xA8
	u8 pad_AC[4];         // 0xAC
	PoseState states[1];  // 0xB0
} PoseData;

typedef struct PoseRoot {
	PoseData* data; // 0x0
} PoseRoot;

extern u32 lp[1];

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
