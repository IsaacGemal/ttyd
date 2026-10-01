#pragma once

#include <dolphin/os.h>
typedef struct WindowEntry WindowEntry;
typedef struct EventEntry EventEntry;
#include "memory.h"

typedef struct MsgDataEntry {
	u32 field_0x0; //0x0, probably a GXColor?
	u16 glyph; //0x4
	u8 field_0x6[0xC - 0x6]; //0x6
	s32 field_0xC; //0xC
	u8 field_0x10[0x18 - 0x10]; //0x10
	/*s32 poseId; //0x0
	s16 field_0x4; //0x4
	s16 field_0x6; //0x6
	s16 field_0x8; //0x8
	u8 pad_0xA[2]; //0xA
	s32 paperPoseId; //0xC
	f32 scale; //0x10
	f32 rotY; //0x14*/
} MsgDataEntry;

// The smart buffer a message window owns (window->userdata).
typedef struct MessageData {
	const char* tag; //0x0
	u32 field_0x4; //0x4
	u8 pad_0x08[0x10 - 0x8]; //0x8
	s32 field_0x10; //0x10
	s32 field_0x14; //0x14
	OSTime field_0x18; //0x18
	s32 field_0x20; //0x20
	s32 field_0x24; //0x24
	s32 field_0x28; //0x28
	u8 pad_0x2C[0x30 - 0x2C]; //0x2C
	OSTime field_0x30; //0x30
	s32 entryId; //0x38
	MsgDataEntry entries[1280]; //0x3C
	u8 pad_0x783C[0x7840 - 0x783C]; //0x783C
	s32 field_0x7840; //0x7840
	u8 pad_0x7844[0x79FC - 0x7844]; //0x7844
	s32 field_0x79FC; //0x79FC
	EventEntry* event; //0x7A00
	char text[0x20]; //0x7A04
	s32 field_0x7A24; //0x7A24
	s32 field_0x7A28; //0x7A28
	u8 field_0x7A2C; //0x7A2C
	u8 pad_0x7A2D[0x7A30 - 0x7A2D]; //0x7A2D
	s32 field_0x7A30; //0x7A30
	s16 field_0x7A34; //0x7A34
	s16 field_0x7A36; //0x7A36
	s16 field_0x7A38; //0x7A38
	s16 field_0x7A3A; //0x7A3A
	s32 field_0x7A3C; //0x7A3C
	s8 field_0x7A40; //0x7A40
	s8 field_0x7A41; //0x7A41
	s8 field_0x7A42; //0x7A42
	s8 field_0x7A43; //0x7A43
	s16 field_0x7A44; //0x7A44
	s16 field_0x7A46; //0x7A46
	f32 field_0x7A48; //0x7A48
	WindowEntry* window; //0x7A4C
	s32 field_0x7A50; //0x7A50
	s32 field_0x7A54; //0x7A54
} MessageData;

typedef struct MessageEntry {
	u32 length; //0x0
	void* address; //0x4
	u32 count; //0x8
	smartEntry* lookup; //0xC
} MessageEntry;

typedef struct MessageWork {
	MessageEntry entries[2]; //0x0
	s32 poseId; //0x20
} MessageWork;

extern f32 float_400_8041a530;
void msgInit(void);
void msgLoad(const char* filename, s32 index);



const char* msgSearch(const char* tag);
BOOL _ismbblead(u32 a1);
int msg_compare(const void* a, const void* b);
void msgDispKeyWait_render(s32 wXluStage);
void msgWindow_ForceClose(const char* tag);
void msgWindow_Repeat(s32 id);
void msgWindow_Continue(s32 id);
s32 msgWindow_Entry(const char* msg, s32 a, s32 b);
void msgWindow_Add(const char* msg, s32 id);
#include "driver/camdrv.h"
MessageWork* L_msgGetWork(void);
void msgWindow_Delete(WindowEntry* window);
void msgAnalize(smartEntry* smart, const char* msg);
void msgWindow_Main(WindowEntry* window);
void msgWindow_Clear_Main(WindowEntry* window);
void selectWindow_Main(WindowEntry* window);
void selectWindow_Disp(CameraId camId, void* param);
void msgWindow_Disp(CameraId camId, void* param);
s32 msgMain(smartEntry* smart);
char* msgGetCommand(char* str, char* arg1, char* arg2);
s32 msgIconStr2ID(const char* str);
