#include <dolphin/types.h>
#pragma force_active on

extern u8 btlevtcmd_SetEventAttack[];
extern u8 btlevtcmd_SetEventDamage[];
extern u8 btlevtcmd_SetEventWait[];
extern u8 btlevtcmd_SetMaxMoveCount[];
extern u8 btlevtcmd_StartWaitEvent[];

extern const char str_802FD4C0[16];
extern const char str_802FD4D0[16];
extern const char str_802FD4E0[24];
extern const char str_802FD4F8[20];
extern const char str_802FD50C[28];
static u32 defence[2];
static u32 defence_attr[2];
static u32 regist[6];
static u32 data_table[2];
static u32 damage_event[2];
static u32 attack_event[2];
static u32 wait_event[2];
static u32 parts[19];
extern u32 unit_system[49];
static u32 init_event[21];
static u32 gap_05_803AB0EC_data[1];

const char str_802FD4C0[16] = "btl_un_system\000\000";

const char str_802FD4D0[16] = "SFX_BTL_DAMAGE1";

const char str_802FD4E0[24] = "SFX_BTL_DAMAGE_FIRE1\000\000\000";

const char str_802FD4F8[20] = "SFX_BTL_DAMAGE_ICE1";

const char str_802FD50C[28] = "SFX_BTL_DAMAGE_BIRIBIRI1\000\000\000";

static u32 defence[2] = {
    0x00000000, 0x00000000
};

static u32 defence_attr[2] = {
    0x02020202, 0x02000000
};

static u32 regist[6] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
};

static u32 data_table[2] = {
    0x00000000, 0x00000000
};

static u32 damage_event[2] = {
    0x00000002, 0x00000001
};

static u32 attack_event[2] = {
    0x00000002, 0x00000001
};

static u32 wait_event[2] = {
    0x00000002, 0x00000001
};

static u32 parts[19] = {
    0x00000001, (u32)(void*)str_802FD4C0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0014001E, 0x00FF0000, (u32)(void*)defence, (u32)(void*)defence_attr,
    0x03010009, 0x00000000, 0x00000000
};

u32 unit_system[49] = {
    0x000000DC, (u32)(void*)str_802FD4C0, 0x03E70000, 0x01010000, 0x00000000, 0x00090018, 0x00180000, 0x00140000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x41400000,
    0x00000000, 0x00000000, 0x41400000, 0x4179999A, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0000FF00, 0x64140000, 0x00000000, 0x00000000, 0x00000000, (u32)(void*)str_802FD4D0,
    (u32)(void*)str_802FD4E0, (u32)(void*)str_802FD4F8, (u32)(void*)str_802FD50C, 0x52003000, (u32)((u8*)regist +8), 0x01000000, (u32)(void*)parts, (u32)(void*)init_event,
    (u32)(void*)regist
};

static u32 init_event[21] = {
    0x0003005B, (u32)(void*)btlevtcmd_SetEventWait, 0xFFFFFFFE, (u32)(void*)wait_event, 0x0003005B, (u32)(void*)btlevtcmd_SetEventAttack, 0xFFFFFFFE, (u32)(void*)attack_event,
    0x0003005B, (u32)(void*)btlevtcmd_SetEventDamage, 0xFFFFFFFE, (u32)(void*)damage_event, 0x0003005B, (u32)(void*)btlevtcmd_SetMaxMoveCount, 0xFFFFFFFE, 0x00000000,
    0x0002005B, (u32)(void*)btlevtcmd_StartWaitEvent, 0xFFFFFFFE, 0x00000002, 0x00000001
};

static __declspec(section ".data") u32 gap_05_803AB0EC_data[1] = {
    0x00000000
};
#pragma force_active reset
