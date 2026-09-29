#include <dolphin/types.h>
#pragma force_active on


extern const u8 dat_80302548[8];
extern u32 __lconv[14];

const __declspec(section ".rodata") u8 dat_80302548[8] = {
    0x2E, 0x00, 0x00, 0x43, 0x00, 0x00, 0x00, 0x00
};

u32 __lconv[14] = {
    (u32)(void*)dat_80302548, (u32)((u8*)dat_80302548 +2), (u32)((u8*)dat_80302548 +2), (u32)((u8*)dat_80302548 +2), (u32)((u8*)dat_80302548 +2), (u32)((u8*)dat_80302548 +2), (u32)((u8*)dat_80302548 +2), (u32)((u8*)dat_80302548 +2),
    (u32)((u8*)dat_80302548 +2), 0x7F7F7F7F, 0x7F7F7F00, (u32)((u8*)dat_80302548 +2), 0x7F7F7F7F, 0x7F7F7F00
};
#pragma force_active reset
