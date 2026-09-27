#pragma once

typedef struct _GXRenderModeObj GXRenderModeObj;

typedef struct DVDFileInfo DVDFileInfo;

#include <dolphin/types.h>
void __THPSimpleDVDCallback(s32 result, DVDFileInfo* fileInfo);
BOOL VideoDecode(u8* videoFrame);
void THPAudioMixCallback(void);
void THPGXRestore(void);
void THPGXYuv2RgbSetup(GXRenderModeObj* rmode);
void THPGXYuv2RgbDraw(u8* y_data, u8* u_data, u8* v_data, s16 x, s16 y, s16 textureWidth, s16 textureHeight,
                      s16 polygonWidth, s16 polygonHeight);
