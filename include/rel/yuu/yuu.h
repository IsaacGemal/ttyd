#pragma once

#include "evt/evt_cmd.h"
#include "driver/camdrv.h"
void yuuminigame_init_1hikoki(EventEntry* event, BOOL isFirstCall);
void yuuminigame_main_1hikoki(EventEntry* event, BOOL isFirstCall);
void yuuminigame_disp_1hikoki(CameraId cameraId, void* param);
void yuuminigame_init_2fune(EventEntry* event, BOOL isFirstCall);
void yuuminigame_main_2fune(EventEntry* event, BOOL isFirstCall);
void yuuminigame_disp_2fune(CameraId cameraId, void* param);
void yuuminigame_init_3roll(EventEntry* event, BOOL isFirstCall);
void yuuminigame_main_3roll(EventEntry* event, BOOL isFirstCall);
void yuuminigame_disp_3roll(CameraId cameraId, void* param);
void yuuminigame_init_4slit(EventEntry* event, BOOL isFirstCall);
void yuuminigame_main_4slit(EventEntry* event, BOOL isFirstCall);
void yuuminigame_disp_4slit(CameraId cameraId, void* param);
extern void* yuunpc_yuuminigame4_ptr;
void yuunpc_wp_flag_off(s32 mask);
void ext_yuunpc_minigame4_init_sub(void);
void yuucounter_flag_on(s32 flag);
s32 yuucounter_get_data(void);
extern f32 yuusintbl[];
