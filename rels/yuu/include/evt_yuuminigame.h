#pragma once

#include "evt/evt_cmd.h"
#include "driver/camdrv.h"

typedef struct YuuminigameWork {
    EventEntry* event;                // 0x00
    EventEntry* childEvent;           // 0x04
    u16 flags;                        // 0x08
    u16 pad_0x0A;                     // 0x0A
    s32 type;                         // 0x0C
    void (*init)(EventEntry*, BOOL);  // 0x10
    void (*main)(EventEntry*, BOOL);  // 0x14
    void (*disp)(CameraId, void*);    // 0x18
    void* data;                       // 0x1C
} YuuminigameWork;

USER_FUNC(yuu_atai_init);
USER_FUNC(yuu_atai_pop);
USER_FUNC(yuu_atai_push);
USER_FUNC(eff_call_minigame);
USER_FUNC(yuuminigame_set_dataadr);
USER_FUNC(yuuminigame_get_dataadr);
USER_FUNC(evt_yuuminigame_free);
USER_FUNC(evt_yuuminigame_alloc);
USER_FUNC(evt_yuuminigame_main_func);
void evt_yuuminigame_disp_func(CameraId cameraId, void* param);
