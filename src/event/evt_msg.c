#include "evt/evt_msg.h"
#include "evt/evt_cmd.h"
#include "evt/evt_npc.h"
#include "driver/msgdrv.h"
#include "driver/windowdrv.h"
#include "driver/camdrv.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "party.h"
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include <string.h>
#include <stdio.h>

// .rodata

// .bss
char partyLabel_578[0x40];
char partyLabel_582[0x40];
char partyLabel_590[0x40];

// .sdata
const char* partyMsgStr[11] = {
    "", "_kur", "_nok", "_bom", "_yos", "_win", "_viv", "_chu", "_jug", "_tel", "_lui"
};
const char* battlePartyMsgStr[7] = {
    "_kur", "_nok", "_yos", "_win", "_viv", "_bom", "_chu"
};
__declspec(export) u8 gap_07_804106A8_sdata[0x18] = {0};

void evt_msg_init(void) {
	//stubbed in retail
}

#define MSGWORK(w) ((MessageData*)(w)->userdata->address)

s32 _evt_msg_print(EventEntry* event, BOOL isFirstCall, s32 flags, const char* msg, s32 toge, const char* npcName) {
    WindowEntry* window;
    NpcEntry* npc = NULL;
    s32 type;

    if (isFirstCall) {
        const char* message;
        if (!(flags & 1)) {
            message = msgSearch(msg);
        }
        else {
            message = msg;
            msg = "DIRECT";
        }
        event->printWindowId = msgWindow_Entry(message, toge, (s16)event->msgPriority);
        event->msgPriority = 0;
        window = windowGetPointer(event->printWindowId);
        MSGWORK(window)->tag = msg;
        MSGWORK(window)->event = event;

        if (!(flags & 2)) {
            if (event->wNpcEventType == 6) {
                npc = event->ownerNPC;
            }
            if (npcName) {
                npc = evtNpcNameToPtr(event, npcName);
            }

            if (npc == (NpcEntry*)-1) {
                if ((event->printWindowFlags & 0x1EFFFFFF) == 0) {
                    PartyEntry* party = partyGetPtr(marioGetPartyId());

                    if (!party) {
                        window->anchor = marioGetPtr()->position;
                        window->anchor.y += marioGetPtr()->wPlayerCollisionBox.y;
                    }
                    else {
                        window->anchor = party->position;
                        window->anchor.y += party->height;
                    }
                }
                strcpy(MSGWORK(window)->text, npcName);
            }
            else if (npc == (NpcEntry*)-2) {
                if ((event->printWindowFlags & 0x1EFFFFFF) == 0) {
                    PartyEntry* party = partyGetPtr(marioGetExtraPartyId());

                    if (!party) {
                        window->anchor = marioGetPtr()->position;
                        window->anchor.y += marioGetPtr()->wPlayerCollisionBox.y;
                    }
                    else {
                        window->anchor = party->position;
                        window->anchor.y += party->height;
                    }
                }
                strcpy(MSGWORK(window)->text, npcName);
            }
            else if (npc == (NpcEntry*)-3) {
                if ((event->printWindowFlags & 0x1EFFFFFF) == 0) {
                    window->anchor = marioGetPtr()->position;
                    window->anchor.y += marioGetPtr()->wPlayerCollisionBox.y;
                }
                strcpy(MSGWORK(window)->text, npcName);
            }
            else if (npc) {
                if ((event->printWindowFlags & 0x1EFFFFFF) == 0) {
                    window->anchor = npc->position;
                    window->anchor.y += npc->height;
                }
                strcpy(MSGWORK(window)->text, npc->description);
            }
        }
        else {
            MSGWORK(window)->flags |= 8;
            MSGWORK(window)->battleUnitId = BattleTransID(event, (s32)npcName);
            BtlUnit_GetTalkTogePos(MSGWORK(window)->battleUnitId, &window->anchor.x,
                                   &window->anchor.y, &window->anchor.z);
            window->dispFlags = 3;
        }

        type = event->printWindowFlags & 0x1EFFFFFF;
        switch (type) {
        case 0:
            if (npc) {
                Vec sp14;
                Vec sp8;

                marioGetPtr();
                getScreenPoint(&window->anchor, &sp14);
                if (window->type == WINDOW_TYPE_SMALL) {
                    window->dispFlags = 7;
                }
                else if (__fabsf(sp14.x) >= 300.0f) {
                    window->dispFlags = 2;
                }
                else {
                    getScreenPoint(&marioGetPtr()->position, &sp8);
                    if (sp14.x < sp8.x) {
                        window->dispFlags = 3;
                    }
                    else {
                        window->dispFlags = 7;
                    }
                }
            }
            break;
        case 1:
            window->dispFlags = 0;
            break;
        case 2:
            window->dispFlags = 7;
            window->anchor = event->msgAnchor;
            break;
        case 3:
            window->dispFlags = 2;
            window->anchor = event->msgAnchor;
            break;
        }

        if (event->printWindowFlags & 0x80000000) {
            window->dispFlags = 3;
            if (event->printWindowFlags & 0x40000000) {
                window->dispFlags |= 4;
            }
            if (event->printWindowFlags & 0x20000000) {
                window->dispFlags |= 8;
            }
        }
        if (event->printWindowFlags & 0x01000000) {
            window->dispFlags |= 0x10;
        }
        event->printWindowFlags = 0;
    }
    if (windowCheckID(event->printWindowId)) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_msg_print) {
    s32* args = event->args;
    s32 a0 = evtGetValue(event, *args++);
    s32 a1 = evtGetValue(event, *args++);
    s32 a2 = evtGetValue(event, *args++);
    s32 a3 = evtGetValue(event, *args++);
    return _evt_msg_print(event, isFirstCall, a0, (const char*)a1, a2, (const char*)a3);
}

USER_FUNC(evt_msg_print_party) {
    s32* args = event->args;
    s32 member;
    s32 msg = evtGetValue(event, *args);
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (party) {
        member = party->currentMemberId;
    }
    else {
        member = 0;
    }
    strcpy(partyLabel_578, (const char*)msg);
    strcat(partyLabel_578, partyMsgStr[member]);
    return _evt_msg_print(event, isFirstCall, 0, partyLabel_578, 0, "party");
}

USER_FUNC(evt_msg_print_party_add) {
    s32* args = event->args;
    s32 member;
    s32 msg = evtGetValue(event, *args);
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (party) {
        member = party->currentMemberId;
    }
    else {
        member = 0;
    }
    if (isFirstCall) {
        strcpy(partyLabel_582, (const char*)msg);
        strcat(partyLabel_582, partyMsgStr[member]);
        msgWindow_Add(msgSearch(partyLabel_582), event->printWindowId);
    }
    if (windowCheckID(event->printWindowId)) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_msg_print_battle_party) {
    s32* args = event->args;
    s32 msg = evtGetValue(event, *args);
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, -4));

    if (!unit) {
        return EVT_RETURN_DONE;
    }
    strcpy(partyLabel_590, (const char*)msg);
    strcat(partyLabel_590, battlePartyMsgStr[unit->currentKind - UNIT_GOOMBELLA]);
    return _evt_msg_print(event, isFirstCall, 2, partyLabel_590, 0, (const char*)-4);
}

USER_FUNC(evt_msg_print_insert) {
    s32* args = event->args;
    s32 varargCount = (event->args[-2] >> 16) - 5;
    s32 a0 = evtGetValue(event, *args++);
    const char* a1 = (const char*)evtGetValue(event, *args++);
    s32 a2 = evtGetValue(event, *args++);
    s32 a3 = evtGetValue(event, *args++);
    s32 values[4];
    char buf[0x500];
    s32 i;

    for (i = 0; i < varargCount; i++) {
        values[i] = evtGetValue(event, *args++);
    }
    if (isFirstCall) {
        const char* fmt = (a0 & 1) ? a1 : msgSearch(a1);

        switch (varargCount) {
        case 0:
            break;
        case 1:
            sprintf(buf, fmt, values[0]);
            break;
        case 2:
            sprintf(buf, fmt, values[0], values[1]);
            break;
        case 3:
            sprintf(buf, fmt, values[0], values[1], values[2]);
            break;
        case 4:
            sprintf(buf, fmt, values[0], values[1], values[2], values[3]);
            break;
        }
    }
    return _evt_msg_print(event, isFirstCall, a0 | 1, buf, a2, (const char*)a3);
}

USER_FUNC(evt_msg_print_add) {
    s32* args = event->args;
    s32 flag = evtGetValue(event, *args++);
    const char* msg = (const char*)evtGetValue(event, *args);

    if (isFirstCall) {
        if (!(flag & 1)) {
            msgWindow_Add(msgSearch(msg), event->printWindowId);
        }
        else {
            msgWindow_Add(msg, event->printWindowId);
        }
    }
    if (windowCheckID(event->printWindowId)) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_msg_print_add_insert) {
    s32* args = event->args;
    s32 varargCount = (event->args[-2] >> 16) - 3;
    s32 a0 = evtGetValue(event, *args++);
    const char* a1 = (const char*)evtGetValue(event, *args++);
    s32 values[4];
    char buf[0x500];
    s32 i;

    for (i = 0; i < varargCount; i++) {
        values[i] = evtGetValue(event, *args++);
    }
    if (isFirstCall) {
        const char* fmt = (a0 & 1) ? a1 : msgSearch(a1);

        switch (varargCount) {
        case 0:
            break;
        case 1:
            sprintf(buf, fmt, values[0]);
            break;
        case 2:
            sprintf(buf, fmt, values[0], values[1]);
            break;
        case 3:
            sprintf(buf, fmt, values[0], values[1], values[2]);
            break;
        case 4:
            sprintf(buf, fmt, values[0], values[1], values[2], values[3]);
            break;
        }
        msgWindow_Add(buf, event->printWindowId);
    }
    if (windowCheckID(event->printWindowId)) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_msg_continue) {
    if (isFirstCall) {
        msgWindow_Continue(event->printWindowId);
    }
    if (windowCheckID(event->printWindowId)) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_msg_repeat) {
    if (isFirstCall) {
        msgWindow_Repeat(event->printWindowId);
    }
    if (windowCheckID(event->printWindowId)) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_msg_close) {
    msgWindow_ForceClose((const char*)evtGetValue(event, *event->args));
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_msg_select) {
    s32* args = event->args;
    s32 flag = evtGetValue(event, *args++);
    const char* msg = (const char*)evtGetValue(event, *args);
    const char* message;

    if (isFirstCall) {
        if (!(flag & 1)) {
            message = msgSearch(msg);
        }
        else {
            message = msg;
            msg = "DIRECT";
        }
        event->selectWindowId = msgWindow_Entry(message, 0, 0x14);
        MSGWORK(windowGetPointer(event->selectWindowId))->tag = msg;
    }
    if (windowCheckID(event->selectWindowId)) {
        return EVT_RETURN_BLOCK;
    }
    event->lwData[0] = MSGWORK(windowGetPointer(event->selectWindowId))->selectCursor;
    windowDeleteID(event->selectWindowId);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_msg_toge) {
    s32* args = event->args;

    s32 value = evtGetValue(event, *args++);

    event->printWindowFlags &= 0xE1000000;
    event->printWindowFlags |= value;
    event->msgAnchor.x = evtGetFloat(event, *args++);
    event->msgAnchor.y = evtGetFloat(event, *args++);
    event->msgAnchor.z = evtGetFloat(event, *args++);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_msg_togelr) {
    s32 value = evtGetValue(event, *event->args);

    event->printWindowFlags |= 0x80000000;
    switch (value) {
    case 0:
        break;
    case 1:
        event->printWindowFlags |= 0x40000000;
        break;
    case 2:
        event->printWindowFlags |= 0x20000000;
        break;
    case 3:
        event->printWindowFlags |= 0x60000000;
        break;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_msg_pri) {
    event->msgPriority = evtGetValue(event, *event->args);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_msg_npc) {
    const char* name = (const char*)evtGetValue(event, *event->args);
    WindowEntry* window = windowGetPointer(event->printWindowId);
    strcpy(MSGWORK(window)->text, name);
    return EVT_RETURN_DONE;
}
