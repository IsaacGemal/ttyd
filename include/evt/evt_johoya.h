#pragma once

#include "evt/evt_cmd.h"
#include <dolphin/types.h>

typedef struct {
    s16 unk0; //0x0
    s16 labelId; //0x2
    s32 initEndPos; //0x4
    s16 tagEndPos; //0x8
    s16 unkA; //0xA
    s16 storyId; //0xC
    s16 unkE; //0xE
} InformantInfo;

typedef struct {
    u8 unk0[0xD]; //0x0
    u8 unkD[0xD]; //0xD
    u8 unk1A[0xD]; //0x1A
    u8 unk27[0x4]; //0x27
} InformantUnknown;

typedef struct {
    InformantInfo* info; //0x0
    s32 count; //0x4
    const char* prefix; //0x8
    InformantInfo *infoEnd; //0xC
    u8 unk10[0x14 - 0x10]; //0x10
    InformantUnknown unk14; //0x14
    u8 unk3F[0x40 - 0x3F]; //0x3F
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
