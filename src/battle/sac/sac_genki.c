#include <stdlib.h>
#include "battle/battle.h"
#include "battle/battle_audience.h"
#include "battle/battle_unit.h"
#include "battle/battle_ac.h"
#include "driver/effdrv.h"
#include "driver/icondrv.h"
#include "driver/dispdrv.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario_pouch.h"
#include "system.h"
#include <string.h>
#include <math.h>
#include <dolphin/mtx.h>
#include "eff/eff_recovery.h"
#include "eff/eff_starstone.h"
#include "statuswindow.h"
#include "battle/sac/sac_genki.h"
#include "battle/sac/sac_deka.h"
#include "eff/n64/eff_stamp_n64.h"
#include "eff/eff_stardust.h"

extern char str_8030045C[];
extern f32 float_30_80421d80;
extern f32 float_1p25_80421d6c;
extern f64 double_to_int_80300468;
extern f32 float_360_80421de8;
extern f32 float_60_80421d50;
extern f32 float_neg1p25_80421e0c;
extern f32 float_0_80421d84;
const char str_8030048C[] = "M_C_2A";
const char str_80300494[] = "M_C_2B";
const char str_80421D3C[] = "M_I_Y";
const char str_80421E10[] = "M_S_1";
const char str_80421E18[] = "M_D_1";
const char str_80421E20[] = "M_X_1";
extern f32 float_20_80421d78;
extern f32 float_10_80421d54;
const s32 dat_803002c8[3] = {0, 4, 2};
extern f32 float_1p32_80421ddc;
extern f32 float_1p98_80421de0;
extern f32 float_15_80421de4;
extern f32 float_neg1000_80421d88;
extern f32 float_1_80421d74;
extern f32 float_901_80421e28;
extern const f32 vec3_803002b0[];
extern f32 float_37_80421d8c;
extern f32 float_50_80421d90;
extern f32 float_neg1_80421d94;
extern f32 float_2160_80421d98;
extern f32 float_200_80421d9c;
extern f32 float_32767_80421da0;
extern f32 float_720_80421da4;
extern f32 float_120_80421da8;
extern f32 float_0p2_80421dac;
extern f32 float_neg2_80421db0;
extern f32 float_3_80421db4;
extern f32 float_neg3_80421db8;
extern f32 float_0p1_80421dbc;
extern f32 float_6p2832_80421dc0;
extern f32 float_1p5_80421dc4;
extern f32 float_180_80421d4c;
extern f32 float_2_80421d70;
extern f32 float_7_80421dc8;
extern f32 float_40_80421dcc;
extern f32 float_0p75_80421dd0;
extern f32 float_0p01_80421dd4;
extern f32 float_0p05_80421dd8;
extern f32 float_3p1416_80421d48;
extern f32 float_28_80421dec;
extern f32 float_0p15_80421df0;
extern f32 float_250_80421df4;
extern f32 float_1E07_80421df8;
extern f32 float_1p125_80421e08;
extern char str_80300470[];


typedef struct GenkiStar {
    s32 field_0;                // 0x0
    s32 field_4;                // 0x4
    s32 field_8;                // 0x8
    f32 field_C;                // 0xC
    f32 field_10;               // 0x10
    f32 field_14;               // 0x14
    f32 field_18;               // 0x18
    f32 field_1C;               // 0x1C
    f32 field_20;               // 0x20
    f32 field_24;               // 0x24
    f32 field_28;               // 0x28
    f32 field_2C;               // 0x2C
    f32 field_30;               // 0x30
    f32 field_34;               // 0x34
    f32 field_38;               // 0x38
    f32 field_3C;               // 0x3C
    f32 field_40;               // 0x40
    f32 field_44;               // 0x44
    f32 field_48;               // 0x48
    f32 field_4C;               // 0x4C
    f32 field_50;               // 0x50
    f32 field_54;               // 0x54
    f32 field_58;               // 0x58
    f32 field_5C;               // 0x5C
    EffectEntry* field_60;      // 0x60
} GenkiStar;                    // 0x64

typedef struct GenkiObject {
    s32 field_0;                // 0x0
    s32 field_4;                // 0x4
    s32 field_8;                // 0x8
    f32 field_C;                // 0xC
    f32 field_10;               // 0x10
    f32 field_14;               // 0x14
    u8 pad_18[0x2C - 0x18];     // 0x18
} GenkiObject;                  // 0x2C

typedef struct GenkiWeapon {
    s32 field_0;                // 0x0
    s32 field_4;                // 0x4
    f32 field_8;                // 0x8
    f32 field_C;                // 0xC
    f32 field_10;               // 0x10
    f32 field_14;               // 0x14
    f32 field_18;               // 0x18
    f32 field_1C;               // 0x1C
    f32 field_20;               // 0x20
    f32 field_24;               // 0x24
    f32 field_28;               // 0x28
    f32 field_2C;               // 0x2C
    f32 field_30;               // 0x30
    f32 field_34;               // 0x34
    f32 field_38;               // 0x38
    f32 field_3C;               // 0x3C
    f32 field_40;               // 0x40
    EffectEntry* field_44;      // 0x44
    EffectEntry* field_48[3];   // 0x48
} GenkiWeapon;                  // 0x54

typedef struct GenkiWork {
    u8 pad_0[0x4];              // 0x0
    s32 field_4;                // 0x4
    u8 pad_8[0xC - 0x8];        // 0x8
    s32 field_C;                // 0xC
    s32 score[3];               // 0x10
    f32 field_1C;               // 0x1C
    f32 field_20;               // 0x20
    f32 field_24;               // 0x24
    s32 field_28;               // 0x28
    s32 field_2C;               // 0x2C
    s32 field_30;               // 0x30
    EffectEntry* field_34;      // 0x34
    EffectEntry* field_38;      // 0x38
    EffectEntry* field_3C;      // 0x3C
    s32 field_40;               // 0x40
    s32 field_44;               // 0x44
    s32 field_48;               // 0x48
    s32 field_4C;               // 0x4C
    s32 field_50;               // 0x50
    s32 field_54;               // 0x54
    s32 field_58;               // 0x58
    s32* field_5C;              // 0x5C
    s32 field_60;               // 0x60
    s32 field_64;               // 0x64
    s32 field_68;               // 0x68
    f32 field_6C;               // 0x6C
    f32 field_70;               // 0x70
    u8 pad_74[0x10C - 0x74];    // 0x74
    GenkiWeapon weapon[7];      // 0x10C
    GenkiObject objects[10];    // 0x358
    GenkiStar stars[7];         // 0x510
} GenkiWork;

// The whole file was compiled with -O4,s (optimize for size); without this every constant-trip
// loop is unrolled and nothing matches. configure.py should carry extra_cflags=["-O4,s"] for it.
#pragma optimize_for_size on
// The original file never inlines its own helpers: every call is a real `bl`.
#pragma dont_inline on

static GenkiWork* get_ptr(void);

USER_FUNC(recover_cmd_disable) {
    u32* p = (u32*)((u8*)_battleWorkPointer + 0x19038);

    p[0] = 0;
    p[1] = 0;
    p[2] = 0;
    p[3] = 0;
    p[4] = 0;
    return 2;
}

USER_FUNC(star_stone_appear) {
    GenkiWork* genki = get_ptr();

    genki->stars[0].field_0 = 1;
    genki->stars[0].field_8 = 1;
    return 2;
}

USER_FUNC(star_appear) {
    GenkiWork* genki = get_ptr();
    s32 i;

    for (i = 0; i < 7; i++) {
        GenkiStar* star = &genki->stars[i];

        star->field_0 = 1;
        star->field_8 = 0;
    }
    return 2;
}

USER_FUNC(status_on) {
    statusWinOpen();
    return 2;
}

USER_FUNC(get_score) {
    GenkiWork* genki = get_ptr();
    s32* args = event->args;

    evtSetValue(event, args[0], genki->score[0]);
    evtSetValue(event, args[1], genki->score[1]);
    evtSetValue(event, args[2], genki->score[2]);
    return 2;
}

USER_FUNC(wait_game_end) {
    GenkiWork* genki = get_ptr();

    return (genki->field_4 == 3) ? 2 : 0;
}

USER_FUNC(start_game) {
    GenkiWork* genki = get_ptr();

    genki->field_4 = 1;
    return 2;
}

s32 object_get_num(void) {
    GenkiWork* genki = get_ptr();
    s32 i = 0;
    s32 count = 0;

    for (; i < 10; i++) {
        if (genki->objects[i].field_0 == 2) {
            count++;
        }
    }
    return count;
}

void object_entry(void) {
    GenkiWork* genki = get_ptr();
    s32 i;

    for (i = 0; i < 10; i++) {
        GenkiObject* obj = &genki->objects[i];

        if (obj->field_0 == 0) {
            obj->field_0 = 1;
            break;
        }
    }
}

// NONMATCHING (96.48%): 1 missing, plus register differences.
void weapon_entry(void) {
    BattleUnit* mario;
    BattleUnitPart* parts;
    GenkiWork* genki = get_ptr();
    s32 offsets[10];
    s32 found = 0;
    s32 i;
    s32 idx;
    s32 count;

    mario = BattleGetMarioPtr(_battleWorkPointer);
    parts = BtlUnit_GetPartsPtr(mario, BtlUnit_GetBodyPartsId(mario));
    count = genki->field_58;
    for (i = 0; i < count; i++) {
        if (genki->weapon[i].field_0 == 0) {
            offsets[found++] = i;
        }
    }
    if (found >= 1) {
        idx = offsets[irand(found)];
        genki->weapon[idx].field_0 = 1;
        genki->weapon[idx].field_4 = 7;
        BtlUnit_SetAnim(parts, str_8030045C);
    }
}

#pragma dont_inline on
static GenkiWork* get_ptr(void) {
    return (GenkiWork*)((u8*)_battleWorkPointer + 0x1F4C);
}
#pragma dont_inline reset

void disp_3D_alpha(CameraId camera, void* param) {
}

void disp_2D(CameraId camera, void* param) {
}

// NONMATCHING (96.46%): same instructions; only register numbers or operands differ.
USER_FUNC(end_genki) {
    GenkiWork* genki = get_ptr();
    s32 i;
    s32 j;
    s32 offset;
    s32 woffset;
    GenkiStar* star;
    GenkiWeapon* weapon;

    offset = 0;
    for (i = 0; i < 7; i++, offset += 0x64) {
        star = (GenkiStar*)((u8*)genki + offset + 0x510);
        if (star->field_60 != NULL) {
            effDelete(*(EffectEntry**)((u8*)star->field_60->userdata + 0x34));
            effDelete(star->field_60);
        }
    }
    woffset = 0;
    for (i = 0; i < 7; i++, woffset += 0x54) {
        weapon = (GenkiWeapon*)((u8*)genki + woffset + 0x10C);
        if (weapon->field_44 != NULL) {
            effDelete(*(EffectEntry**)((u8*)weapon->field_44->userdata + 0x34));
            effDelete(weapon->field_44);
            for (j = 0; j < 3; j++) {
                effDelete(*(EffectEntry**)((u8*)weapon->field_48[j]->userdata + 0x34));
                effDelete(weapon->field_48[j]);
            }
        }
    }
    effDelete(genki->field_34);
    effDelete(genki->field_38);
    effDelete(genki->field_3C);
    BattleFree(genki->field_5C);
    return 2;
}

void main_star(void) {
    GenkiWork* genki = get_ptr();
    s32 i;

    if (genki->field_C == 0) {
        for (i = 0; i < 7; i++) {
            main_star0(i);
        }
    } else {
        main_star1();
    }
}

// NONMATCHING (92.01%): 1 instruction differs, 3 missing, 6 extra, plus register differences.
void main_target(void) {
    GenkiWork* genki = get_ptr();

    switch (genki->field_64) {
    case 0:
        break;
    case 1:
        genki->field_68 = 0;
        genki->field_6C = float_30_80421d80;
        genki->field_70 = float_1p25_80421d6c;
        genki->field_64 = 2;
    case 2:
        genki->field_68++;
        if (genki->field_68 >= 0x168) {
            genki->field_68 = (s32)((f32)genki->field_68 - float_360_80421de8);
        }
        genki->field_6C += genki->field_70;
        if (genki->field_6C >= float_60_80421d50) {
            genki->field_6C = float_60_80421d50;
            genki->field_70 = float_neg1p25_80421e0c;
            genki->field_64 = 3;
        }
        break;
    case 3:
        genki->field_68++;
        if (genki->field_68 >= 0x168) {
            genki->field_68 = (s32)((f32)genki->field_68 - float_360_80421de8);
        }
        genki->field_6C += genki->field_70;
        if (genki->field_6C <= float_0_80421d84) {
            genki->field_6C = float_0_80421d84;
            genki->field_70 = float_1p25_80421d6c;
            genki->field_64 = 2;
        }
        break;
    case 4:
        break;
    }
}

void main_mario(void) {
    GenkiWork* genki = get_ptr();
    s32 stickX = (s8)keyGetStickX(0);
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    BattleUnitPart* parts = BtlUnit_GetPartsPtr(mario, BtlUnit_GetBodyPartsId(mario));

    if (genki->field_44 >= 1 && genki->field_44 <= 4 && genki->field_40 <= 0 && object_get_num() == 0) {
        genki->field_44 = 5;
        genki->field_64 = 4;
        BtlUnit_SetAnim(parts, str_80421E10);
    }
    switch (genki->field_44) {
    case 0:
        break;
    case 1:
        if (stickX < -0x28) {
            genki->field_44 = 2;
            genki->field_48 = 0;
            genki->field_54 = 10;
            BtlUnit_SetAnim(parts, str_8030048C);
        }
        break;
    case 2:
        if (stickX > -0x14) {
            genki->field_44 = 1;
            weapon_entry();
        } else {
            genki->field_54--;
            if (genki->field_54 == 2) {
                BtlUnit_SetAnim(parts, str_80300494);
            }
            if (genki->field_54 <= 0 && genki->field_C == 1) {
                genki->field_54 = 10;
                weapon_entry();
            }
        }
        break;
    case 3:
        genki->field_44 = 4;
        genki->field_48 = 0;
        BtlUnit_SetAnim(parts, str_80421E18);
        {
            f32 x, y, z;
            EffectEntry* eff;
            BtlUnit_GetPos(mario, &x, &y, &z);
            eff = effStampN64Entry(2, x, float_20_80421d78 + y, float_10_80421d54 + z);
            ((u8*)eff->userdata)[0x38] = 0;
            ((u8*)eff->userdata)[0x39] = 0xC8;
            ((u8*)eff->userdata)[0x3A] = 0;
            ((u8*)eff->userdata)[0x3B] = 0;
            ((u8*)eff->userdata)[0x3C] = 0xA;
            ((u8*)eff->userdata)[0x3D] = 0;
        }
    case 4:
        genki->field_48++;
        if (genki->field_48 == 0x1E) {
            BtlUnit_SetAnim(parts, str_80421E20);
        }
        if (genki->field_48 == 0xB4) {
            BtlUnit_SetAnim(parts, str_80421D3C);
            genki->field_44 = 1;
            genki->field_48 = 0;
        }
        break;
    case 5:
        break;
    }
}

// NONMATCHING (95.32%): 2 instructions differ, 3 missing, 10 extra, plus register differences.
void main_object(s32 idx) {
    GenkiWork* genki = get_ptr();
    GenkiObject* obj = (GenkiObject*)((u8*)genki + 0x358 + idx * 0x2C);
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    s32 local[3];
    GenkiStar* star;
    f32 x, y, z;

    local[0] = dat_803002c8[0];
    local[1] = dat_803002c8[1];
    local[2] = dat_803002c8[2];

    switch (obj->field_0) {
    case 0:
        break;
    case 1:
        obj->field_0 = 2;
        obj->field_4 = 0;
        {
            s32 limit = 0x32;
            if (genki->field_C == 0) {
                limit = 0x19;
            }
            if (genki->field_60 < limit) {
                obj->field_8 = genki->field_5C[genki->field_60];
            } else {
                s32 r = irand(genki->field_C == 0 ? 7 : 0xE);
                if (r == 0) {
                    obj->field_8 = 6;
                } else {
                    if (genki->field_50 != -1) {
                        obj->field_8 = local[irand(3)];
                    } else {
                        obj->field_8 = local[irand(2)];
                    }
                    if (irand(5) == 0 && genki->field_C == 1) {
                        switch (obj->field_8) {
                        case 0:
                            obj->field_8 = 1;
                            break;
                        case 2:
                            obj->field_8 = 3;
                            break;
                        case 4:
                            obj->field_8 = 5;
                            break;
                        }
                    }
                }
            }
        }
        if (genki->field_50 == -1 && obj->field_8 == 2) {
            obj->field_8 = 0;
        }
        if (genki->field_50 == -1 && obj->field_8 == 3) {
            obj->field_8 = 1;
        }
        genki->field_60++;
        if (genki->field_C == 0) {
            star = &genki->stars[irand(7)];
        } else {
            star = &genki->stars[0];
        }
        {
            BtlUnit_GetPos(mario, &x, &y, &z);
            obj->field_C = star->field_C;
            obj->field_10 = star->field_10;
            obj->field_14 = z + (f32)idx;
        }
    case 2:
        if (genki->field_C == 0) {
            obj->field_10 -= float_1p32_80421ddc;
        } else {
            obj->field_10 -= float_1p98_80421de0;
        }
        {
            BtlUnit_GetPos(mario, &x, &y, &z);
            if (obj->field_10 <= float_15_80421de4 + y) {
                obj->field_0 = 3;
                effStardustEntry(5, 0xA, 0x3C, obj->field_C, float_15_80421de4 + y, obj->field_14,
                                 float_30_80421d80, float_30_80421d80);
            }
        }
        obj->field_4++;
        if (obj->field_4 > 0x78) {
            obj->field_4 = 0;
        }
        break;
    case 3:
        obj->field_0 = 0;
        break;
    case 4:
        break;
    }
}

// NONMATCHING (90.67%): 2 instructions differ, 5 missing, 14 extra, plus register differences.
s32 main_genki(EventEntry* event, BOOL isFirstCall) {
    GenkiWork* genki = get_ptr();
    s32 i;
    s32 j;
    s32* args;

    if (isFirstCall) {
        args = event->args;
        memset(genki, 0, 0x7CC);
        genki->field_50 = evtGetValue(event, args[0]);
        genki->field_C = evtGetValue(event, args[1]);
        genki->field_40 = 0x384;
        genki->score[2] = 0;
        genki->score[1] = 0;
        genki->score[0] = 0;
        genki->field_24 = float_0_80421d84;
        genki->field_20 = float_0_80421d84;
        genki->field_1C = float_0_80421d84;
        if (genki->field_C == 0) {
            genki->field_58 = 3;
        } else {
            genki->field_58 = 7;
        }
        if (genki->field_C == 0) {
            genki->field_5C = (s32*)BattleAlloc(0x64);
        } else {
            genki->field_5C = (s32*)BattleAlloc(0xC8);
        }
        genki->field_34 = effRecoveryEntry(2, 0, float_0_80421d84, float_neg1000_80421d88, float_0_80421d84);
        genki->field_38 = effRecoveryEntry(2, 0, float_0_80421d84, float_neg1000_80421d88, float_0_80421d84);
        genki->field_3C = effRecoveryEntry(3, 0, float_0_80421d84, float_neg1000_80421d88, float_0_80421d84);
        for (i = 0; i < 7; i++) {
            genki->weapon[i].field_44 = effStarStoneEntry(i, float_0_80421d84, float_neg1000_80421d88,
                                                          float_0_80421d84, float_1_80421d74);
            for (j = 0; j < 3; j++) {
                genki->weapon[i].field_48[j] = effStarStoneEntry(i + 7, float_0_80421d84,
                                                                 float_neg1000_80421d88,
                                                                 float_0_80421d84, float_1_80421d74);
            }
        }
    }
    main_base();
    main_mario();
    main_target();
    for (i = 0; i < genki->field_58; i++) {
        main_weapon(i);
    }
    for (i = 0; i < 10; i++) {
        main_object(i);
    }
    main_star();
    dispEntry(8, float_901_80421e28, 1, disp_2D, NULL);
    dispEntry(4, float_0_80421d84, 1, disp_3D, NULL);
    dispEntry(4, float_0_80421d84, 2, disp_3D_alpha, NULL);
    return 0;
}

// NONMATCHING (98.74%): 2 missing, 4 extra, plus register differences.
void main_base(void) {
    GenkiWork* genki = get_ptr();
    s32 i;
    s32 a;
    s32 b;
    s32 t;

    if (genki->field_1C > (f32)genki->score[0]) {
        genki->field_1C -= float_1_80421d74;
        genki->field_28 = 0x3C;
    }
    if (genki->field_1C < (f32)genki->score[0]) {
        genki->field_1C += float_1_80421d74;
        genki->field_28 = 0x3C;
    }
    if (genki->field_20 > (f32)genki->score[1]) {
        genki->field_20 -= float_1_80421d74;
        genki->field_2C = 0x3C;
    }
    if (genki->field_20 < (f32)genki->score[1]) {
        genki->field_20 += float_1_80421d74;
        genki->field_2C = 0x3C;
    }
    if (genki->field_24 > (f32)genki->score[2]) {
        genki->field_24 -= float_1_80421d74;
        genki->field_30 = 0x3C;
    }
    if (genki->field_24 < (f32)genki->score[2]) {
        genki->field_24 += float_1_80421d74;
        genki->field_30 = 0x3C;
    }
    if (genki->field_28 > 0) {
        genki->field_28--;
    }
    if (genki->field_2C > 0) {
        genki->field_2C--;
    }
    if (genki->field_30 > 0) {
        genki->field_30--;
    }

    switch (genki->field_4) {
    case 0:
        break;
    case 1:
        genki->field_4 = 2;
        genki->field_44 = 1;
        genki->field_64 = 1;
        genki->field_60 = 0;
        if (genki->field_C == 0) {
            s32 k = 0;
            for (i = 0; i < 7; i++) {
                genki->field_5C[k++] = 0;
            }
            for (i = 0; i < 7; i++) {
                genki->field_5C[k++] = 2;
            }
            for (i = 0; i < 8; i++) {
                genki->field_5C[k++] = 4;
            }
            for (i = 0; i < 3; i++) {
                genki->field_5C[k++] = 6;
            }
            for (i = 0; i < 200; i++) {
                a = irand(25);
                b = irand(25);
                t = genki->field_5C[a];
                genki->field_5C[a] = genki->field_5C[b];
                genki->field_5C[b] = t;
            }
        } else {
            s32 k = 0;
            for (i = 0; i < 12; i++) {
                genki->field_5C[k++] = 0;
            }
            for (i = 0; i < 4; i++) {
                genki->field_5C[k++] = 1;
            }
            for (i = 0; i < 12; i++) {
                genki->field_5C[k++] = 2;
            }
            for (i = 0; i < 4; i++) {
                genki->field_5C[k++] = 3;
            }
            for (i = 0; i < 12; i++) {
                genki->field_5C[k++] = 4;
            }
            for (i = 0; i < 4; i++) {
                genki->field_5C[k++] = 5;
            }
            for (i = 0; i < 2; i++) {
                genki->field_5C[k++] = 6;
            }
            for (i = 0; i < 200; i++) {
                a = irand(50);
                b = irand(50);
                t = genki->field_5C[a];
                genki->field_5C[a] = genki->field_5C[b];
                genki->field_5C[b] = t;
            }
        }
        /* fallthrough */
    case 2:
        genki->field_4C--;
        if (genki->field_4C <= 0) {
            if (genki->field_C == 0) {
                genki->field_4C = 0x25;
            } else {
                genki->field_4C = 0x12;
            }
            if (genki->field_40 > 0) {
                object_entry();
            }
        }
        if (genki->field_40 <= 0) {
            s32 ocount = 0;
            s32 wcount = 0;
            for (i = 0; i < genki->field_58; i++) {
                if (genki->weapon[i].field_0 == 0) {
                    wcount++;
                } else {
                    break;
                }
            }
            for (i = 0; i < 10; i++) {
                if (genki->objects[i].field_0 == 0) {
                    ocount++;
                } else {
                    break;
                }
            }
            if (wcount == genki->field_58 && ocount == 10) {
                genki->field_4 = 3;
            }
        } else {
            genki->field_40--;
        }
        break;
    case 3:
        break;
    }
}

// NONMATCHING (61.75%): 55 instructions differ, 60 missing, 124 extra, plus register differences.
void main_star0(s32 idx) {
    const f32* vec = vec3_803002b0;
    GenkiWork* genki = get_ptr();
    GenkiStar* star = &genki->stars[idx];
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    f32 x, y, z;
    f32 f;

    switch (star->field_0) {
    case 0:
        break;
    case 1:
        star->field_0 = 2;
        star->field_4 = 0;
        star->field_60 = effStarStoneEntry(idx, float_0_80421d84, float_neg1000_80421d88,
                                           float_0_80421d84, float_1_80421d74);
        BtlUnit_GetPos(mario, &star->field_C, &star->field_10, &star->field_14);
        star->field_10 = star->field_10 +
            (mario->mSizeMultiplier * (f32)mario->mHeight + float_37_80421d8c);
        star->field_18 = star->field_C;
        star->field_1C = star->field_10;
        star->field_20 = star->field_14;
        star->field_24 = star->field_C;
        star->field_28 = star->field_10;
        star->field_2C = star->field_14;
        star->field_28 += float_30_80421d80;
        star->field_58 = float_6p2832_80421dc0 * (f32)idx / float_7_80421dc8;
        star->field_3C = vec[0x24 / 4];
        star->field_40 = vec[0x28 / 4];
        star->field_44 = vec[0x2C / 4];
        star->field_48 = vec[0x30 / 4];
        star->field_4C = vec[0x34 / 4];
        star->field_50 = vec[0x38 / 4];
        star->field_54 = float_0_80421d84;
        star->field_5C = float_0p1_80421dbc;
        /* fallthrough */
    case 2:
        star->field_4++;
        if (star->field_4 <= 0x64) {
            star->field_10 = intplGetValue(0, star->field_4, 0x64, star->field_1C, star->field_28);
            BtlUnit_GetPos(mario, &x, &y, &z);
            star->field_54 = intplGetValue(0, star->field_4, 0x64, float_0_80421d84,
                                           float_40_80421dcc);
            star->field_58 += star->field_5C;
            star->field_C = x + star->field_54 * (f32)cos(star->field_58);
            star->field_14 = z + star->field_54 * (f32)sin(star->field_58);
            f = intplGetValue(0, star->field_4, 0x64, float_0_80421d84, float_0p75_80421dd0);
            star->field_44 = f;
            star->field_40 = f;
            star->field_3C = f;
        } else {
            star->field_10 = star->field_28;
            BtlUnit_GetPos(mario, &x, &y, &z);
            star->field_58 += star->field_5C;
            star->field_C = x + star->field_54 * (f32)cos(star->field_58);
            star->field_14 = z + star->field_54 * (f32)sin(star->field_58);
            star->field_44 = float_0p75_80421dd0;
            star->field_40 = float_0p75_80421dd0;
            star->field_3C = float_0p75_80421dd0;
        }
        star->field_4C = intplGetValue(4, star->field_4, 0x78, float_0_80421d84,
                                       float_2160_80421d98);
        if (star->field_4 >= 0x78) {
            star->field_0 = 3;
            star->field_4 = 0;
            star->field_18 = star->field_C;
            star->field_1C = star->field_10;
            star->field_20 = star->field_14;
            star->field_24 = float_200_80421d9c * (f32)rand() / float_32767_80421da0;
            star->field_28 = vec[0x40 / 4];
            star->field_2C = star->field_20;
            star->field_30 = vec[0x48 / 4];
            star->field_34 = vec[0x4C / 4];
            star->field_38 = vec[0x50 / 4];
            star->field_34 = (float_2_80421d70 * (star->field_28 - star->field_1C) -
                              (float_60_80421d50 * float_60_80421d50 *
                               (float_0p2_80421dac - float_0p01_80421dd4 * (f32)idx))) /
                             float_120_80421da8;
        }
        break;
    case 3:
        star->field_4++;
        star->field_C = intplGetValue(4, star->field_4, 0x3C, star->field_18, star->field_24);
        star->field_10 += star->field_34;
        star->field_34 += float_0p2_80421dac - float_0p01_80421dd4 * (f32)idx;
        star->field_4C = float_0_80421d84;
        star->field_44 = float_0p75_80421dd0;
        star->field_40 = float_0p75_80421dd0;
        star->field_3C = float_0p75_80421dd0;
        if (star->field_4 >= 0x3C) {
            star->field_0 = 4;
            star->field_4 = 0;
            if (irand(2) == 0) {
                star->field_30 = float_neg2_80421db0 - float_0p05_80421dd8 * (f32)idx;
            }
            star->field_30 = float_2_80421d70 + float_0p05_80421dd8 * (f32)idx;
            star->field_34 = float_0_80421d84;
        }
        break;
    case 4:
        star->field_C += star->field_30;
        if (star->field_C < float_0_80421d84) {
            star->field_30 = float_3_80421db4 + float_0p05_80421dd8 * (f32)idx;
        }
        if (star->field_C > float_200_80421d9c) {
            star->field_30 = float_neg3_80421db8 - float_0p05_80421dd8 * (f32)idx;
        }
        star->field_34 += float_0p1_80421dbc + float_0p01_80421dd4 * (f32)idx;
        if (star->field_34 >= float_6p2832_80421dc0) {
            star->field_34 -= float_6p2832_80421dc0;
        }
        star->field_10 = float_10_80421d54 * (f32)sin(star->field_34) + float_180_80421d4c;
        if (genki->field_40 <= 0) {
            star->field_0 = 5;
            star->field_4 = 0;
            star->field_C = vec[0x54 / 4];
            star->field_10 = vec[0x58 / 4];
            star->field_14 = vec[0x5C / 4];
        }
        break;
    case 5:
        break;
    }

    if (star->field_60 != NULL) {
        f32* p = (f32*)star->field_60->userdata;
        p[0x8 / 4] = star->field_C;
        p[0xC / 4] = star->field_10;
        p[0x10 / 4] = star->field_14;
        p[0x18 / 4] = star->field_48;
        p[0x1C / 4] = star->field_4C;
        p[0x20 / 4] = star->field_50;
        p[0x14 / 4] = float_1p5_80421dc4 * star->field_3C;
    }
}

