#pragma once

#include "evt/evt_cmd.h"

typedef struct {
    s16 field_0x0; //0x0
    s16 labelId; //0x2
    s16 storyId; //0x4
} KeijibanInfo;

typedef struct {
    KeijibanInfo* info;  //0x0
    s32 count;           //0x4
    const char* prefix;  //0x8
    char* infoEnd;       //0xC
    s32 index;           //0x10
    s32 maxLabel;        //0x14
} KeijibanWork;

s32 keijiban_data_make(const char* prefix, s32 type);
USER_FUNC(keijiban_joho_makelabel);
USER_FUNC(keijiban_joho_check);
USER_FUNC(keijiban_data_free);
USER_FUNC(keijiban_data_alloc);
