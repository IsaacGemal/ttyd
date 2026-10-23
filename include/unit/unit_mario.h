#pragma once

typedef struct BattleUnit BattleUnit;

typedef struct EventEntry EventEntry;

#include <dolphin/types.h>
s32 _mario_super_emblem_anim_set(EventEntry* event, s32 mode);
void faker_mario_makkuro_set(void);
struct BtlUnitPoseSound;
s32 mario_pinch_pose_sound_callback(BattleUnit* unit, struct BtlUnitPoseSound* entry, void* ps);
s32 _get_local_frame(EventEntry* event, BOOL isFirstCall);
