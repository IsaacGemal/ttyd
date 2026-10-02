#pragma once

#include <dolphin/mtx.h>

#include <dolphin/types.h>
void kpaAddScorePos(s32 score, Vec* pos);
void kpaAddScore(s32 score);
void kpaAddCoin(s32 coins);
#include "driver/camdrv.h"
#include "manager/evtmgr.h"
void kpaAddCoinPos(s32 coins, Vec* pos);
s32 kpa_chg_pose(EventEntry* event, BOOL isFirstCall);
s32 kpa_score_disp_init(EventEntry* event, BOOL isFirstCall);
s32 kpa_score_disp_main(EventEntry* event, BOOL isFirstCall);
void kpaScoreDisp(CameraId cameraId, void* param);
extern s32 kpa_powerup_evt[7];
