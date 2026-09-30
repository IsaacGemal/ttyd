#pragma once

typedef struct EventEntry EventEntry;

#include "driver/camdrv.h"

#include <dolphin/types.h>
s32 winGhostDiaryChk(void);
void winDisp(CameraId camId, void* param);
void winDispKoopa(CameraId camId, void* param);
void winLectureKeyMask(void);
s32 unk_80175e98(void);
s32 unk_80175eb0(EventEntry* event, BOOL isFirstCall);
s32 unk_80177cf8(void);
s32 itemUseFunc2(EventEntry* event, BOOL isFirstCall);
s32 party_color(EventEntry* event, BOOL isFirstCall);
s32 famicom_check(EventEntry* event, BOOL isFirstCall);
void winFontInit(void);
void winFontSet(Vec* pos, Vec* scale, GXColor* color, const char* format, ...);
void winFontSetPitch(Vec* pos, Vec* scale, GXColor* color, f32 pitch, const char* format, ...);
void winIconGrayInit(void);
void winFontSetLabel(Vec* pos, Vec* scale, GXColor* color, const char* tag);
void winFontSetEdge(Vec* pos, Vec* scale, GXColor* color, const char* format, ...);
void winFontSetR(Vec* pos, Vec* scale, GXColor* color, const char* format, ...);
