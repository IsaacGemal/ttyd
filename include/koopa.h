#pragma once

#include <dolphin/types.h>
s32 kpaCamFollow(void);
void kpaChgPose(const char* marioPose, const char* kpa2Pose);
f32 kpaSetJumpPara(void);
s32 kpaMutekiCheck(void);
void kpaSetLevel(s32 level);
s32 kpaGetLevel(void);
void kpaAttackStart(void);
void kpa2DMain(void);
void kpa3DMain(void);
void kpaFireMain(void);
s32 kpaFireAttackCheck(void);
s32 kpaGetBodyStatus(void);
f32 kpaGetBaseScale(s32 index);
s32 kpaGetStageViewType(void);
void kpaFireAttack(void);
void kpa_jumpNpc(void);
s32 kpaGetStageType(void);
void* getKoopaFireEfp(void);
