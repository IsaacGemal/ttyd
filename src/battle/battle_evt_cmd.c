#include <dolphin/types.h>
#pragma force_active on

extern u8 btlevtcmd_ACSuccessEffect[];

extern u32 jumptable_8034A578[7];
static u32 gap_05_8034A594_data[1];

__declspec(section ".data") u32 jumptable_8034A578[7] = {
    (u32)((u8*)btlevtcmd_ACSuccessEffect +0xDC), (u32)((u8*)btlevtcmd_ACSuccessEffect +0xF4), (u32)((u8*)btlevtcmd_ACSuccessEffect +0x10C), (u32)((u8*)btlevtcmd_ACSuccessEffect +0x124), (u32)((u8*)btlevtcmd_ACSuccessEffect +0x13C), (u32)((u8*)btlevtcmd_ACSuccessEffect +0x154), (u32)((u8*)btlevtcmd_ACSuccessEffect +0x164)
};

static __declspec(section ".data") u32 gap_05_8034A594_data[1] = {
    0x00000000
};
#pragma force_active reset
