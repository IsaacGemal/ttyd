#include "battle/battle_message.h"
#include "manager/evtmgr_cmd.h"
#include "battle/battle.h"

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
