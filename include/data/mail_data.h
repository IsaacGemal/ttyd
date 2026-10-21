#pragma once

#include <dolphin/types.h>
void* mailGetPtr(s32 id);

// A mail_data entry as the pause menu reads it (win_root, win_item); mailGetPtr returns it untyped.
// mail_data.c defines the entries with a u32 at 0x0, whose high half is this flags word.
typedef struct WinMailEntry {
    u16 flags;           // 0x0: 1/2/4 pick the envelope colour, 0x1000 has a picture
    u8 field_0x2[0x6];   // 0x2
    const char* name;    // 0x8
    const char* body;    // 0xC
    s32 picture;         // 0x10
} WinMailEntry;
