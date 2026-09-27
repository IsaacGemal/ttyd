#pragma once

typedef struct BattleUnit BattleUnit;

typedef struct EventEntry EventEntry;

#include <dolphin/types.h>
s32 _get_mario_hammer_lv(EventEntry* event, BOOL isFirstCall);
s32 _mario_super_emblem_anim_set(EventEntry* event, s32 mode);
s32 _mario_makkuro_set(EventEntry* event, BOOL isFirstCall);
void faker_mario_makkuro_set(void);
s32 mario_pinch_pose_sound_callback(BattleUnit* unit);
s32 _get_local_frame(EventEntry* event, BOOL isFirstCall);
