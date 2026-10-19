#pragma once

#include <dolphin/os.h>
typedef struct WindowEntry WindowEntry;
typedef struct EventEntry EventEntry;
#include "memory.h"


typedef struct MsgDataEntry {
	u32 arg; //0x0: glyph flags (0x1 pop-in, 0x2 shake, 0x4 wave, 0x8 talk, 0x10 SE, 0x100 << n for select item n);
	         // for commands the payload (animPose id, icon id, NpcEntry*, a sscanf value)
	u16 glyph; //0x4
	s16 x; //0x6
	s16 y; //0x8
	s16 iconOffsetY; //0xA: the icon tag's fourth value, added to the icon's y (0xFFF8)
	s32 startTime; //0xC: ms at which the entry is reached (compared with textTime); an index into paperPoseTbl for 0xFFF3
	f32 scale; //0x10
	f32 field_0x14; //0x14: pop-in scale that decays to 1 (flag 0x1), or a paper pose's Y rotation
} MsgDataEntry;

// The smart buffer a message window owns (window->userdata).
typedef struct MessageData {
	const char* tag; //0x0
	u32 flags; //0x4: 0x2 calls the callback with 0 once, 0x4 a key press advances, 0x8 the talk/stay anim of
	           // battleUnitId is driven, 0x40 the scroll history queue is drawn, 0x80 the text is held,
	           // 0x200 nothing is added to the history
	u16 choiceIndex; //0x8: choices made so far; a 0xFFFE entry shows when its x equals this
	u16 choiceState; //0xA: 1 while msgMain waits for a choice
	u16 choiceMade; //0xC: set to 1 when a choice is confirmed
	u8 pad_0x0E[0x10 - 0xE]; //0xE
	s32 entryPos; //0x10: index of the next entry msgMain reads
	u8 pad_0x14[0x18 - 0x14]; //0x14
	OSTime effectStart; //0x18: render time the wave and rotate effects count from
	s32 textTime; //0x20: ms of text shown so far, compared with each entry's start time
	s32 scrollTarget; //0x24: scroll offset the text moves toward
	s32 scrollY; //0x28: current scroll offset, added to each glyph's y
	u8 pad_0x2C[0x30 - 0x2C]; //0x2C
	OSTime lastFrameTime; //0x30: render time of the last msgMain
	s32 entryId; //0x38: number of entries msgAnalize has written
	MsgDataEntry entries[1280]; //0x3C
	s32 historyCursor; //0x783C: read by msgDisp's queue mode; this file never writes it
	s32 historyCount; //0x7840
	s32 history[0x6E]; //0x7844: scroll offsets shown so far, at most 110
	void (*callback)(s32, smartEntry*); //0x79FC: called with a step code (0 to 5) as the text advances
	EventEntry* event; //0x7A00
	char text[0x20]; //0x7A04
	s32 battleUnitId; //0x7A24: battle unit that the talk/stay flag (0x8) drives
	s32 field_0x7A28; //0x7A28: set by 0xFFFA entries, never read
	u8 sfxVolume; //0x7A2C: volume of the text SE, set by 0xFFF9 entries (0x7F at open)
	u8 pad_0x7A2D[0x7A30 - 0x7A2D]; //0x7A2D
	s32 timeCursor; //0x7A30: start time carried from one msgAnalize tag to the next
	s16 penX; //0x7A34
	s16 penY; //0x7A36
	s16 savedX; //0x7A38: msgAnalize's var[1] (x), saved across a tag
	s16 savedY; //0x7A3A: msgAnalize's var[0] (y), saved across a tag
	s32 charDelay; //0x7A3C: ms added to timeCursor per character (0x19 at open, 0 after some tags)
	s8 selectCount; //0x7A40: items in a select window; its width is count * 0x24 + 0x28
	s8 selectInit; //0x7A41: initial cursor
	s8 field_0x7A42; //0x7A42: second value of the select tag, never read
	s8 selectCursor; //0x7A43: cursor; evt_msg reads it as the chosen item
	s16 selectWidth; //0x7A44
	s16 selectY; //0x7A46
	f32 selectTopY; //0x7A48
	WindowEntry* window; //0x7A4C
	s32 talkAnimTimer; //0x7A50: frames until the talk anim toggles; set to 15 by a 0x8 glyph
	s32 talkAnimOn; //0x7A54
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

extern const f32 float_400_8041a530;
extern u32 dat_8041a500;
extern u32 dat_8041a504;
extern u32 dat_8041a508;
extern u32 dat_8041a50c;
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
BOOL msgDisp(smartEntry* smart, u8 alpha, f32 x, f32 y);
