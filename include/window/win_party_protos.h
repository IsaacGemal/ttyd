#pragma once

/* The original win_party callers pass these windows' Vec and GXColor
   aggregates by value, while the shared win/win_main.h and driver/animdrv.h
   prototypes take pointers.  This unit hides those while including and pulls
   the by-value forms from here, so every other unit keeps its prototypes. */
void winFontSet(Vec pos, Vec scale, GXColor color, const char* format, ...);
void winFontSetLabel(Vec* pos, Vec* scale, GXColor color, const char* tag);
void winFontSetEdge(Vec pos, Vec scale, GXColor color, const char* format, ...);
void winFontSetR(Vec pos, Vec scale, GXColor color, const char* format, ...);
void winIconSet(s32 iconId, Vec* translate, Vec* scale, GXColor color);
void animPoseVivianMain(s32 poseId, Vec pos);
