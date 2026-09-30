#include "battle/battle_message.h"
#include "manager/evtmgr_cmd.h"
#include "battle/battle.h"
#include "driver/dispdrv.h"
#include "driver/msgdrv.h"
#include "data/item_data.h"
#include <stdio.h>

f32 float_900_8041e850 = 900.0f;

// The original saves r25-r31 with stmw/lmw (size-oriented prologue); battle_message.c wants -O4,s.
//s32 kind, s32 x, s32 y, const char* tag, s32 duration
USER_FUNC(btlevtcmd_AnnounceMessage) {
    BattleWork* wp = _battleWorkPointer;
    u8* base = (u8*)wp + 0x20000;
    s32* args = event->args;
    s32 kind;
    s32 x;
    s32 y;
    const char* tag;
    s32 duration;

    kind = evtGetValue(event, args[0]);
    x = evtGetValue(event, args[1]);
    y = evtGetValue(event, args[2]);
    tag = (const char*)evtGetValue(event, args[3]);
    duration = evtGetValue(event, args[4]);

    if (tag == NULL) {
        return EVT_RETURN_DONE;
    }
    if (isFirstCall) {
        event->userdata[0] = duration;
        *(s16*)(base - 0x7390) = x;
        *(s16*)(base - 0x738e) = y;
        {
            const char* msg = msgSearch(tag);
            switch (kind) {
            case 1:
                sprintf((char*)(base - 0x737c), msg, itemDataTable[*(s32*)(base - 0x738c)].id);
                break;
            default:
                sprintf((char*)(base - 0x737c), msg);
                break;
            }
        }
    }
    dispEntry(CAMERA_2D, float_900_8041e850, 1, _disp, NULL);
    event->userdata[0]--;
    return event->userdata[0] <= 0;
}

//s32 index, s32 value
USER_FUNC(btlevtcmd_AnnounceSetParam) {
    BattleWork* wp = _battleWorkPointer;
    s32* base = (s32*)((u8*)wp + 0x18C70);
    s32* args = event->args;
    s32 index;
    s32 value;

    index = evtGetValue(event, args[0]);
    value = evtGetValue(event, args[1]);
    base[index + 1] = value;
    return EVT_RETURN_DONE;
}
