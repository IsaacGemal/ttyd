#pragma once

#include "evt/evt_cmd.h"
#include <dolphin/types.h>

typedef struct {
    u16 readFlag; //0x0, bit 0 set by johoya_keti_oldspeak_setreadflag
    s16 labelId; //0x2
    char* initEndPos; //0x4
    s16 tagEndPos; //0x8
    s16 price; //0xA, 5 from johoya_data_make; the newjoho list window prints it, johoya_keti_newjoho_makelabel returns it
               //     as the coins tik_00's script checks and takes
    s16 storyId; //0xC
    s16 field_0xE; //0xE
} InformantInfo;

typedef struct {
    u8 ketiNewjohoReadFlag[0xD]; //0x0, johoya_keti_newjoho_setreadflag (bit per storyId)
    u8 perfectNewjohoReadFlag[0xD]; //0xD, johoya_perfect_newjoho_setreadflag
    u8 luigiNewjohoReadFlag[0xD]; //0x1A, johoya_luigi_newjoho_setreadflag
    u8 field_0x27[0x4]; //0x27, saved with the flags (cardmgr), no access
} InformantUnknown;

typedef struct {
    InformantInfo* info; //0x0
    s32 count; //0x4
    const char* prefix; //0x8
    char* infoEnd; //0xC
    s32 curIdx; //0x10, info index picked by the makelabel functions or winmgr's list; the setreadflag functions mark it
    InformantUnknown readFlags; //0x14, copied to and from the card's informant block (cardmgr)
    u8 pad_0x3F[0x40 - 0x3F]; //0x3F
} InformantWork;

USER_FUNC(johoya_data_alloc);
USER_FUNC(johoya_data_free);
void johoya_init(void);
s32 search_evt_no(const char* label);
s32 johoya_data_make(const char* prefix, s32 type);
s16 johoya_get(u8* data, s16 id);
s16 johoya_set(u8* data, s16 id, s16 value);
const char* search_evt_moji(s32 index);

USER_FUNC(johoya_keti_newjoho_check);
USER_FUNC(johoya_keti_newjoho_makelabel);
USER_FUNC(johoya_keti_newjoho_setreadflag);
USER_FUNC(johoya_keti_oldspeak_check);
USER_FUNC(johoya_keti_oldspeak_makelabel);
USER_FUNC(johoya_keti_oldspeak_setreadflag);
USER_FUNC(johoya_luigi_newjoho_setreadflag);
USER_FUNC(johoya_perfect_newjoho_setreadflag);
extern InformantWork _jdt;
