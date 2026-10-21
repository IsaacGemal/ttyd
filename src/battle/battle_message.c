#include "battle/battle_message.h"
#include "manager/evtmgr_cmd.h"
#include "battle/battle.h"
#include "driver/dispdrv.h"
#include "driver/msgdrv.h"
#include "driver/windowdrv.h"
#include "manager/fontmgr.h"
#include "data/item_data.h"
#include <stdio.h>

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
    dispEntry(CAMERA_2D, 900.0f, 1, _disp, NULL);
    event->userdata[0]--;
    return event->userdata[0] <= 0;
}

void _disp(CameraId cameraId, void* param) {
    BattleWork* wp = _battleWorkPointer;
    u8* base = (u8*)wp + 0x20000;
    s16 x = *(s16*)(base - 0x7390);
    s16 y = *(s16*)(base - 0x738e);
    f32 fx = (f32)x;
    f32 fy = (f32)(y + 0x78);
    GXColor color;
    u16 lines;
    u16 width = FontGetMessageWidthLine((const char*)(base - 0x737c), &lines);

    lines++;
    color = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
    fx -= (f32)(width / 2);
    // The original passes the GXColor by value, and the header's by-value prototype copies it to a stack temp
    // as usual. Calling through a by-pointer prototype is ABI-identical (the struct is passed by reference) but
    // steers MWCC's register allocation: the 0x4330 conversion magic lands in r6 and the colour in r7, where
    // the by-value form swaps them. Same match-only trick as battle_stage_object.c's local prototype, spelled
    // as a cast so it needs no local declaration.
#ifdef MUST_MATCH
    ((void (*)(s32, GXColor*, f32, f32, f32, f32, f32))windowDispGX_Waku_col)(0, &color, fx - 10.0f,
                                                                             -29.0f + (3.0f + fy),
                                                                             (f32)(width + 0x14),
                                                                             (f32)(lines * 0x1d + 3), 10.0f);
#else
    windowDispGX_Waku_col(fx - 10.0f, -29.0f + (3.0f + fy), (f32)(width + 0x14), (f32)(lines * 0x1d + 3), 10.0f, 0,
                          color);
#endif
    FontDrawStart();
    FontDrawMessage((s32)fx, (s32)fy - 0x1d, (const char*)(base - 0x737c));
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
