#pragma once

#include <dolphin/types.h>

typedef void (*SequenceCallback)(struct SequenceWork* work);

typedef enum SequenceType {
	SEQ_UNDEFINED = -1,
	SEQ_LOGO = 0,
	SEQ_TITLE = 1,
	SEQ_GAME = 2,
	SEQ_MAP_CHANGE = 3,
	SEQ_BATTLE = 4,
	SEQ_GAME_OVER = 5,
	SEQ_LOAD = 6,
	SEQ_E3 = 7
} SequenceType;

typedef struct SequenceWork {
	SequenceType sequence; //0x0
	s32 state; //0x4, 0-7
	const char* mapName; //0x8
	const char* beroName; //0xC
	s32 frameCount; //0x10, seq_battleMain: counted up while the audience textures load and for 30 frames before leaving
	u8 pad_0x14[0x20 - 0x14]; //0x14
} SequenceWork;

void seqInit_MARIOSTORY(void);
void seqMain(void);
void seqSetSeq(SequenceType seq, const char* map, const char* bero);
SequenceType seqGetSeq(void);
SequenceType seqGetPrevSeq(void);
SequenceType seqGetNextSeq(void);
BOOL seqCheckSeq(void);
