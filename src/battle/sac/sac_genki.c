#include <stdlib.h>
#include "battle/battle.h"
#include "battle/battle_audience.h"
#include "battle/battle_unit.h"
#include "battle/battle_ac.h"
#include "battle/battle_disp.h"
#include "battle/battle_disp_tex4.h"
#include "driver/effdrv.h"
#include "driver/icondrv.h"
#include "driver/dispdrv.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario_pouch.h"
#include "system.h"
#include <string.h>
// math_ext.h instead of <math.h>: the SDK's math.h inlines would place extra
// float constants ahead of .rodata and shift main_star1's int->float pool double
// off its 0x1B8 offset.
#include "math_ext.h"
#include <dolphin/mtx.h>
#include "eff/eff_recovery.h"
#include "eff/eff_starstone.h"
#include "statuswindow.h"
#include "battle/sac/sac_genki.h"
#include "battle/sac/sac_deka.h"
#include "eff/n64/eff_stamp_n64.h"
#include "eff/eff_stardust.h"
#include "eff/n64/eff_stardust_n64.h"
#include "eff/eff_hit.h"
#include "pmario_sound.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_camera.h"
#include "battle/sac/sac_common.h"
#include "evt/evt_eff.h"
#include "party/party_motion.h"

// Data objects defined later in this file or in other units, declared before their first use.
extern s32 sac_start_camera_evt[];
extern s32 sac_mario_item_on_evt[];
extern s32 sac_mario_item_on_end_evt[];
extern s32 sac_start_stardust[];
extern s32 status_recover_evt[];
static void disp_2D(CameraId camera, void* param);
static void disp_3D_alpha(CameraId camera, void* param);
static void main_star(void);
static void main_base(void);  // local to this file in the original
static void disp_3D(CameraId cameraId, void* param);  // local to this file in the original
// This unit's .rodata, in retail order: the vec3_803002b0...vec3_803003e8
// f32[3] table (vec3_803002bc covers 0xc..0x17), then dat_803002c8 and the
// strings. The layout only matters because MWCC folds its anonymous int->float
// pool double into the section base when a function names several of these
// objects, so the double must land at vec3_803002b0+0x1B8 (see main_star1).
__declspec(section ".rodata") f32 vec3_803002b0[3] = {0.0f, -1000.0f, 0.0f};
__declspec(section ".rodata") f32 vec3_803002bc[3] = {0.0f, 0.0f, 0.0f};
const s32 dat_803002c8[3] = {0, 4, 2};
// Match-only: retail defines these five as const. A non-const object stops MWCC's
// -O4 scheduler from hoisting the named loads above the star stores (the const form
// drops main_star0 to 88.17%); the section attribute keeps the bytes at their
// retail .rodata offsets.
__declspec(section ".rodata") f32 vec3_803002d4[3] = {0.0f, 0.0f, 0.0f};
__declspec(section ".rodata") f32 vec3_803002e0[3] = {0.0f, 0.0f, 0.0f};
__declspec(section ".rodata") f32 vec3_803002ec[3] = {0.0f, 180.0f, 0.0f};
__declspec(section ".rodata") f32 vec3_803002f8[3] = {0.0f, 0.0f, 0.0f};
__declspec(section ".rodata") f32 vec3_80300304[3] = {0.0f, -1000.0f, 0.0f};
// Match-only: retail defines these five as const. A non-const object defeats
// MWCC's load hoisting in main_star1 (a const object is provably stable, so the
// -O4 scheduler lifts the named loads above the star stores, dropping the
// function to 88.87%); the section attribute keeps the bytes at their retail
// .rodata offsets.
__declspec(section ".rodata") f32 vec3_80300310[3] = {0.0f, 0.0f, 0.0f};
__declspec(section ".rodata") f32 vec3_8030031c[3] = {0.0f, 0.0f, 0.0f};
__declspec(section ".rodata") f32 vec3_80300328[3] = {0.0f, 180.0f, 0.0f};
__declspec(section ".rodata") f32 vec3_80300334[3] = {0.0f, 0.0f, 0.0f};
__declspec(section ".rodata") f32 vec3_80300340[3] = {0.0f, -1000.0f, 0.0f};
const f32 vec3_8030034c[3] = {0.0f, 0.0f, 0.0f};
const f32 vec3_80300358[3] = {0.0f, 0.0f, 0.0f};
const f32 vec3_80300364[3] = {0.0f, 0.0f, 0.0f};
const f32 vec3_80300370[3] = {0.0f, 0.0f, 0.0f};
const f32 vec3_8030037c[3] = {0.0f, 0.0f, 0.0f};
const f32 vec3_80300388[3] = {0.0f, 0.0f, 0.0f};
const f32 vec3_80300394[3] = {0.0f, 0.0f, 0.0f};
const f32 vec3_803003a0[3] = {0.0f, 0.0f, 0.0f};
const f32 vec3_803003ac[3] = {0.0f, 0.0f, 0.0f};
const f32 vec3_803003b8[3] = {0.5f, 0.5f, 1.0f};
const f32 vec3_803003c4[3] = {0.0f, 0.0f, 0.0f};
const f32 vec3_803003d0[3] = {0.0f, 0.0f, 0.0f};
const f32 vec3_803003dc[3] = {0.5f, 0.5f, 1.0f};
const f32 vec3_803003e8[3] = {0.0f, 0.0f, 0.0f};

// .sdata2 objects, in the original's address order. Only the seven icon colours, the
// M_I_Y animation name and the .data-referenced zero are real objects; the float
// constants are file-scope so the functions can name them, and MWCC folds each one
// into its literal pool (the static definitions vanish, leaving the pooled words in
// the original's order).
const GXColor dat_80421d20 = {0xFF, 0xFF, 0xFF, 0xFF};
const GXColor dat_80421d24 = {0xFF, 0xFF, 0xFF, 0xFF};
const GXColor dat_80421d28 = {0xFF, 0xFF, 0xFF, 0xFF};
const GXColor dat_80421d2c = {0xFF, 0xFF, 0xFF, 0xFF};
const GXColor dat_80421d30 = {0xFF, 0xFF, 0xFF, 0xFF};
const GXColor dat_80421d34 = {0xFF, 0xFF, 0xFF, 0xFF};
const GXColor dat_80421d38 = {0xFF, 0xFF, 0xFF, 0xFF};
const char str_80421D3C[] = "M_I_Y";
const f32 float_0_80421d44 = 0.0f;
static const f32 float_0_80421d84 = 0.0f;
static const f32 float_neg1000_80421d88 = -1000.0f;
static const f32 float_7_80421dc8 = 7.0f;
static const f32 float_40_80421dcc = 40.0f;
static const f32 float_0p75_80421dd0 = 0.75f;
static const f32 float_0p01_80421dd4 = 0.01f;
static const f32 float_0p05_80421dd8 = 0.05f;

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

typedef struct GenkiEffect {
    s32 field_0;                // 0x0
    Vec pos;                    // 0x4
    f32 field_10;               // 0x10
} GenkiEffect;                  // 0x14

typedef struct GenkiWeapon {
    s32 field_0;                // 0x0
    s32 field_4;                // 0x4
    Vec pos[4];                 // 0x8, current position then 3 trail positions
    Vec vel;                    // 0x38
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
    Vec field_74;               // 0x74
    GenkiEffect field_80[7];    // 0x80
    GenkiWeapon weapon[7];      // 0x10C
    GenkiObject objects[10];    // 0x358
    GenkiStar stars[7];         // 0x510
} GenkiWork;

static GenkiWork* get_ptr(void);

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
        {
            f32 zero = float_0_80421d84;

            genki->field_40 = 0x384;
            genki->score[2] = 0;
            genki->score[1] = 0;
            genki->score[0] = 0;
            genki->field_24 = zero;
            genki->field_20 = zero;
            genki->field_1C = zero;
        }
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
        genki->field_34 = effRecoveryEntry(2, 0, 0.0f, -1000.0f, 0.0f);
        genki->field_38 = effRecoveryEntry(2, 0, 0.0f, -1000.0f, 0.0f);
        genki->field_3C = effRecoveryEntry(3, 0, 0.0f, -1000.0f, 0.0f);
        for (i = 0; i < 7; i++) {
            genki->weapon[i].field_44 = effStarStoneEntry(i, float_0_80421d84, float_neg1000_80421d88,
                                                          float_0_80421d84, 1.0f);
            for (j = 0; j < 3; j++) {
                genki->weapon[i].field_48[j] = effStarStoneEntry(i + 7, float_0_80421d84,
                                                                 float_neg1000_80421d88,
                                                                 float_0_80421d84, 1.0f);
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
    dispEntry(8, 901.0f, 1, disp_2D, NULL);
    dispEntry(4, float_0_80421d84, 1, disp_3D, NULL);
    dispEntry(4, float_0_80421d84, 2, disp_3D_alpha, NULL);
    return 0;
}

static void main_base(void) {
    GenkiWork* genki = get_ptr();
    s32 i;
    s32 a;
    s32 b;
    s32 t;
    s32 k;

    if (genki->field_1C > (f32)genki->score[0]) {
        genki->field_1C -= 1.0f;
        genki->field_28 = 0x3C;
    }
    if (genki->field_1C < (f32)genki->score[0]) {
        genki->field_1C += 1.0f;
        genki->field_28 = 0x3C;
    }
    if (genki->field_20 > (f32)genki->score[1]) {
        genki->field_20 -= 1.0f;
        genki->field_2C = 0x3C;
    }
    if (genki->field_20 < (f32)genki->score[1]) {
        genki->field_20 += 1.0f;
        genki->field_2C = 0x3C;
    }
    if (genki->field_24 > (f32)genki->score[2]) {
        genki->field_24 -= 1.0f;
        genki->field_30 = 0x3C;
    }
    if (genki->field_24 < (f32)genki->score[2]) {
        genki->field_24 += 1.0f;
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
            k = 0;
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
            k = 0;
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
            s32 ocount;
            s32 wcount = 0;
            for (i = 0; i < genki->field_58; i++) {
                if (genki->weapon[i].field_0 == 0) {
                    wcount++;
                } else {
                    break;
                }
            }
            ocount = 0;
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

void main_mario(void) {
    GenkiWork* genki = get_ptr();
    s32 stickX = (s8)keyGetStickX(0);
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    BattleUnitPart* parts = BtlUnit_GetPartsPtr(mario, BtlUnit_GetBodyPartsId(mario));

    if (genki->field_44 >= 1 && genki->field_44 <= 4 && genki->field_40 <= 0 && object_get_num() == 0) {
        genki->field_44 = 5;
        genki->field_64 = 4;
        BtlUnit_SetAnim(parts, "M_S_1");
    }
    switch (genki->field_44) {
    case 0:
        break;
    case 1:
        if (stickX < -0x28) {
            genki->field_44 = 2;
            genki->field_48 = 0;
            genki->field_54 = 10;
            BtlUnit_SetAnim(parts, "M_C_2A");
        }
        break;
    case 2:
        if (stickX > -0x14) {
            genki->field_44 = 1;
            weapon_entry();
        } else {
            genki->field_54--;
            if (genki->field_54 == 2) {
                BtlUnit_SetAnim(parts, "M_C_2B");
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
        BtlUnit_SetAnim(parts, "M_D_1");
        {
            f32 x, y, z;
            EffectEntry* eff;
            BtlUnit_GetPos(mario, &x, &y, &z);
            eff = effStampN64Entry(2, x, 20.0f + y, 10.0f + z);
            ((effStampData*)eff->userdata)->tevReg0R = 0;
            ((effStampData*)eff->userdata)->tevReg0G = 0xC8;
            ((effStampData*)eff->userdata)->tevReg0B = 0;
            ((effStampData*)eff->userdata)->tevReg1R = 0;
            ((effStampData*)eff->userdata)->tevReg1G = 0xA;
            ((effStampData*)eff->userdata)->tevReg1B = 0;
        }
    case 4:
        genki->field_48++;
        if (genki->field_48 == 0x1E) {
            BtlUnit_SetAnim(parts, "M_X_1");
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

// `auto_inline off` (not `dont_inline`) is what the original needs: MWCC must not auto-inline
// plain helpers such as get_ptr (every call is a real `bl`, and inlining get_ptr wrecks the
// callers), but `_drawGenkiTex`/`_drawGenkiTex2` are declared `inline` and DO inline at their
// two call sites in disp_3D, exactly as the original does.
#pragma auto_inline off
void main_target(void) {
    GenkiWork* genki = get_ptr();

    switch (genki->field_64) {
    case 0:
        break;
    case 1:
        genki->field_68 = 0;
        genki->field_6C = 30.0f;
        genki->field_70 = 1.25f;
        genki->field_64 = 2;
    case 2:
        genki->field_68++;
        if (genki->field_68 >= 0x168) {
            genki->field_68 = (s32)((f32)genki->field_68 - 360.0f);
        }
        genki->field_6C += genki->field_70;
        if (genki->field_6C >= 60.0f) {
            genki->field_6C = 60.0f;
            genki->field_70 = -1.25f;
            genki->field_64 = 3;
        }
        break;
    case 3:
        genki->field_68++;
        if (genki->field_68 >= 0x168) {
            genki->field_68 = (s32)((f32)genki->field_68 - 360.0f);
        }
        genki->field_6C += genki->field_70;
        if (genki->field_6C <= float_0_80421d84) {
            genki->field_6C = float_0_80421d84;
            genki->field_70 = 1.25f;
            genki->field_64 = 2;
        }
        break;
    case 4:
        break;
    }
}

// The function names its vec3 objects directly and the SFX sound name is a pooled string literal, so
// MWCC anchors the .rodata base in r29 and folds the int->float pool double into `lfd 0x1b8(r29)` and
// the string into `addi r4, r29, 0x1c0`, as retail does. The effect's position is one Vec copy.
void main_weapon(s32 idx) {
    GenkiWork* genki = get_ptr();
    GenkiWeapon* weapon = &genki->weapon[idx];
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    f32 angle = 3.141592f * (2.0f * genki->field_6C) / 360.0f;
    s32 audienceNum = BattleAudience_GetAudienceNum();
    s32 i;

    for (i = 2; i > 0; i--) {
        weapon->pos[i + 1] = weapon->pos[i];
    }
    weapon->pos[1] = weapon->pos[0];

    switch (weapon->field_0) {
    case 0:
        weapon->pos[0] = *(const Vec*)vec3_803002b0;
        break;
    case 1:
        weapon->field_4--;
        if (weapon->field_4 >= 0) {
            break;
        }
        weapon->field_0 = 2;
        BtlUnit_GetPos(mario, &weapon->pos[0].x, &weapon->pos[0].y, &weapon->pos[0].z);
        weapon->pos[0].x += 20.0f * mario->mSizeMultiplier;
        weapon->pos[0].y += 28.0f * mario->mSizeMultiplier;
        {
            Vec vel = *(const Vec*)vec3_803002bc;

            vel.x = 10.0f * (f32)cos(angle);
            vel.y = 10.0f * (f32)sin(angle);
            weapon->vel = vel;
        }
        for (i = 0; i < 7; i++) {
            GenkiEffect* eff = &genki->field_80[i];

            if (eff->field_0 <= 0) {
                eff->field_0 = 0x1E;
                eff->pos = genki->field_74;
                eff->field_10 = 2.0f * (f32)genki->field_68;
                break;
            }
        }
        BtlUnit_snd_se(mario, "SFX_BTL_SAC_HEART_SHOOT2", 0xF1194D80, 0);
        /* fallthrough */
    case 2:
        weapon->pos[0].x += weapon->vel.x;
        weapon->pos[0].y += weapon->vel.y;
        weapon->vel.y -= 0.15f;
        {
            f32 x, y, z;

            BtlUnit_GetPos(mario, &x, &y, &z);
            if (weapon->pos[0].x >= 250.0f || weapon->pos[0].y >= 180.0f ||
                weapon->pos[0].y <= y) {
                weapon->field_0 = 3;
                break;
            }
            {
                GenkiObject* obj;
                f32 best = 10000000.0f;
                s32 found = -1;
                f32 wx = weapon->pos[0].x;
                f32 wy = weapon->pos[0].y;

                for (i = 0; i < 10; i++) {
                    obj = &genki->objects[i];

                    if (obj->field_0 == 2) {
                        f32 dx = wx - obj->field_C;
                        f32 dy = wy - obj->field_10;
                        f32 d = dx * dx + dy * dy;

                        if (d < best) {
                            best = d;
                            found = i;
                        }
                    }
                }
                if (found == -1) {
                    break;
                }
                obj = &genki->objects[found];
                {
                    f32 threshold;

                    switch (obj->field_8) {
                    case 6:
                        threshold = 344.0f;
                        break;
                    case 0:
                    case 2:
                    case 4:
                        threshold = 576.0f;
                        break;
                    default:
                        threshold = 1296.0f;
                        break;
                    }
                    if (best <= threshold && obj->field_10 <= 200.0f) {
                        weapon->field_0 = 3;
                        obj->field_0 = 3;
                effStardustN64Entry(5, obj->field_C, obj->field_10, obj->field_14,
                                    10.0f);
                effHitEntry(0, 5, obj->field_C, obj->field_10, obj->field_14, 1.0f);
                switch (obj->field_8) {
                case 0:
                    psndSFXOn_3D((const char*)0xA64, &weapon->pos[0]);
                    genki->score[0] += 1;
                    if (genki->score[0] >= 99) {
                        genki->score[0] = 99;
                    }
                    break;
                case 1:
                    psndSFXOn_3D((const char*)0xA66, &weapon->pos[0]);
                    genki->score[0] += 3;
                    if (genki->score[0] >= 99) {
                        genki->score[0] = 99;
                    }
                    break;
                case 2:
                    psndSFXOn_3D((const char*)0xA64, &weapon->pos[0]);
                    genki->score[1] += 1;
                    if (genki->score[1] >= 99) {
                        genki->score[1] = 99;
                    }
                    break;
                case 3:
                    psndSFXOn_3D((const char*)0xA66, &weapon->pos[0]);
                    genki->score[1] += 3;
                    if (genki->score[1] >= 99) {
                        genki->score[1] = 99;
                    }
                    break;
                case 4:
                    psndSFXOn_3D((const char*)0xA65, &weapon->pos[0]);
                    genki->score[2] += 1;
                    if (genki->score[2] >= 99) {
                        genki->score[2] = 99;
                    }
                    break;
                case 5:
                    psndSFXOn_3D((const char*)0xA67, &weapon->pos[0]);
                    genki->score[2] += 3;
                    if (genki->score[2] >= 99) {
                        genki->score[2] = 99;
                    }
                    break;
                case 6:
                    psndSFXOn_3D((const char*)0xA68, &weapon->pos[0]);
                    genki->field_44 = 3;
                    break;
                }
                switch (obj->field_8) {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                    if (audienceNum >= 1 && audienceNum <= 0x31) {
                        BattleAudienceSoundCallKind(1);
                    }
                    if (audienceNum >= 0x32 && audienceNum <= 0x63) {
                        BattleAudienceSoundCallKind(1);
                        BattleAudienceSoundHandBeat();
                    }
                    if (audienceNum >= 0x64 && audienceNum <= 0x95) {
                        BattleAudienceSoundCallKind(2);
                    }
                    if (audienceNum >= 0x96) {
                        BattleAudienceSoundCallKind(2);
                        BattleAudienceSoundHandBeat();
                    }
                    break;
                case 6:
                    if (audienceNum >= 1 && audienceNum <= 0x31) {
                        BattleAudienceSoundBooingKind(1);
                    }
                    if (audienceNum >= 0x32 && audienceNum <= 0x63) {
                        BattleAudienceSoundBooingKind(1);
                    }
                    if (audienceNum >= 0x64 && audienceNum <= 0x95) {
                        BattleAudienceSoundBooingKind(2);
                    }
                    if (audienceNum >= 0x96) {
                        BattleAudienceSoundBooingKind(3);
                    }
                    break;
                }
                }
            }
        }
        }
        break;
    case 3:
        weapon->field_0 = 0;
        break;
    }

    {
        f32 scale = 1.125f;

        if (weapon->field_44 != NULL) {
            effStarStoneData* data = (effStarStoneData*)weapon->field_44->userdata;

            data->x = weapon->pos[0].x;
            data->y = weapon->pos[0].y;
            data->z = weapon->pos[0].z;
            data->scale = scale;
            data->tevReg0A = 0xFF;
        }
        for (i = 0; i < 3; i++) {
            effStarStoneData* data = (effStarStoneData*)weapon->field_48[i]->userdata;

            data->x = weapon->pos[i + 1].x;
            data->y = weapon->pos[i + 1].y;
            data->z = weapon->pos[i + 1].z;
            data->scale = scale;
            data->tevReg0A = 0xFF - (i + 1) * 0x32;
        }
    }
}

// Matches. The int->float loads read the anonymous pool double (now at vec3_803002b0+0x1B8,
// like retail's double_to_int_80300468) with an absolute `lis`, and the 30.0f literal as
// `@NNN` instead of retail's `float_30_80421d80`; objdiff does not score those symbol names. The
// `30.0f` literal, rather than a named constant (same value), is what makes MWCC emit
// the original's interleaved `li`/`lfs` argument setup. Keeping `fifteen` in a variable fixed the
// compare's f1/f2 assignment; the if/else that builds `limit` fixed the case-1 integer register
// allocation.
void main_object(s32 idx) {
    GenkiWork* genki = get_ptr();
    GenkiObject* obj = &genki->objects[idx];
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
            s32 limit;
            if (genki->field_C == 0) {
                limit = 0x19;
            } else {
                limit = 0x32;
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
            obj->field_10 -= 1.32f;
        } else {
            obj->field_10 -= 1.98f;
        }
        {
            BtlUnit_GetPos(mario, &x, &y, &z);
            {
                f32 fifteen = 15.0f;
                if (obj->field_10 <= fifteen + y) {
                    f32 thirty = 30.0f;
                    obj->field_0 = 3;
                    effStardustEntry(5, 0xA, 0x3C, obj->field_C, fifteen + y, obj->field_14,
                                     thirty, thirty);
                }
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
    }
}

static void main_star(void) {
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

// 100%: naming the vec3 objects directly (rather than reaching them through a
// vec3_803002b0 pointer) makes MWCC anchor the .rodata section base in r31, fold the
// anonymous int->float pool double into `lfd fX, 0x1b8(r31)`, and pick the original's
// callee-saved map (idx=r28, mario=r29, star=r30, vec=r31, genki=r27). The named
// objects must be non-const (see the data block at the top): a const object lets the -O4
// scheduler hoist its loads above the star stores and drops the function to 88%.
// Putting field_58 after field_54/field_3C/field_48 (its original source order) fixed
// the case-1 schedule. Logic, schedule and constants match the assembly.
void main_star0(s32 idx) {
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
        star->field_60 = effStarStoneEntry(idx, 0.0f, -1000.0f, 0.0f, 1.0f);
        BtlUnit_GetPos(mario, &star->field_C, &star->field_10, &star->field_14);
        star->field_10 = star->field_10 +
            (mario->mSizeMultiplier * (f32)mario->mHeight + 37.0f);
        *(Vec*)&star->field_18 = *(Vec*)&star->field_C;
        *(Vec*)&star->field_24 = *(Vec*)&star->field_C;
        star->field_28 += 30.0f;
        *(Vec*)&star->field_3C = *(const Vec*)vec3_803002d4;
        *(Vec*)&star->field_48 = *(const Vec*)vec3_803002e0;
        star->field_54 = 0.0f;
        star->field_58 = 6.283184f * (f32)idx / 7.0f;
        star->field_5C = 0.1f;
        /* fallthrough */
    case 2:
        star->field_4++;
        if (star->field_4 <= 0x64) {
            star->field_10 = intplGetValue(0, star->field_4, 0x64, star->field_1C, star->field_28);
            BtlUnit_GetPos(mario, &x, &y, &z);
            star->field_54 = intplGetValue(0, star->field_4, 0x64, 0.0f,
                                           40.0f);
            star->field_58 += star->field_5C;
            star->field_C = x + star->field_54 * (f32)cos(star->field_58);
            star->field_14 = z + star->field_54 * (f32)sin(star->field_58);
            f = intplGetValue(0, star->field_4, 0x64, 0.0f, 0.75f);
            star->field_44 = f;
            star->field_40 = f;
            star->field_3C = f;
        } else {
            star->field_10 = star->field_28;
            BtlUnit_GetPos(mario, &x, &y, &z);
            star->field_58 += star->field_5C;
            star->field_C = x + star->field_54 * (f32)cos(star->field_58);
            star->field_14 = z + star->field_54 * (f32)sin(star->field_58);
            star->field_44 = 0.75f;
            star->field_40 = 0.75f;
            star->field_3C = 0.75f;
        }
        star->field_4C = intplGetValue(4, star->field_4, 0x78, 0.0f,
                                       2160.0f);
        if (star->field_4 >= 0x78) {
            Vec v;

            star->field_0 = 3;
            star->field_4 = 0;
            *(Vec*)&star->field_18 = *(Vec*)&star->field_C;
            v = *(const Vec*)vec3_803002ec;
            v.x = 200.0f * (f32)rand() / 32767.0f;
            v.z = star->field_20;
            *(Vec*)&star->field_24 = v;
            *(Vec*)&star->field_30 = *(const Vec*)vec3_803002f8;
            star->field_34 = (2.0f * (star->field_28 - star->field_1C) -
                              (60.0f * (60.0f *
                               (0.2f - 0.01f * (f32)idx)))) /
                             120.0f;
        }
        break;
    case 3:
        star->field_4++;
        star->field_C = intplGetValue(4, star->field_4, 0x3C, star->field_18, star->field_24);
        star->field_10 += star->field_34;
        star->field_34 += 0.2f - 0.01f * (f32)idx;
        star->field_4C = 0.0f;
        star->field_44 = 0.75f;
        star->field_40 = 0.75f;
        star->field_3C = 0.75f;
        if (star->field_4 >= 0x3C) {
            star->field_0 = 4;
            star->field_4 = 0;
            if (irand(2) == 0) {
                star->field_30 = -2.0f - 0.05f * (f32)idx;
            }
            star->field_30 = 2.0f + 0.05f * (f32)idx;
            star->field_34 = 0.0f;
        }
        break;
    case 4:
        star->field_C += star->field_30;
        if (star->field_C < 0.0f) {
            star->field_30 = 3.0f + 0.05f * (f32)idx;
        }
        if (star->field_C > 200.0f) {
            star->field_30 = -3.0f - 0.05f * (f32)idx;
        }
        star->field_34 += 0.1f + 0.01f * (f32)idx;
        if (star->field_34 >= 6.283184f) {
            star->field_34 -= 6.283184f;
        }
        star->field_10 = 10.0f * (f32)sin(star->field_34) + 180.0f;
        if (genki->field_40 <= 0) {
            star->field_0 = 5;
            star->field_4 = 0;
            *(Vec*)&star->field_C = *(const Vec*)vec3_80300304;
        }
        break;
    case 5:
        break;
    }

    if (star->field_60 != NULL) {
        effStarStoneData* p = star->field_60->userdata;
        p->x = star->field_C;
        p->y = star->field_10;
        p->z = star->field_14;
        p->tipRotY = star->field_48;
        p->rotY = star->field_4C;
        p->tipRotZ = star->field_50;
        p->scale = 1.5f * star->field_3C;
    }
}

// 100%: naming the five vectors (rather than reaching them through a
// vec3_803002b0 pointer) makes MWCC anchor the .rodata section base in r30, so its
// anonymous int->float pool double folds into `lfd fY, 0x1b8(r30)` exactly as in
// the original (the double lives at vec3_803002b0+0x1B8 thanks to the data block
// at the top of this file). Logic, schedule and every constant match the assembly.
void main_star1(void) {
    GenkiWork* genki = get_ptr();
    GenkiStar* star = &genki->stars[0];
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    f32 f;

    switch (star->field_0) {
    case 0:
        break;
    case 1:
        star->field_0 = 2;
        star->field_4 = 0;
        star->field_60 = effStarStoneEntry(4, float_0_80421d84, float_neg1000_80421d88, float_0_80421d84, 1.0f);
        BtlUnit_GetPos(mario, &star->field_C, &star->field_10, &star->field_14);
        star->field_10 = star->field_10 +
            (mario->mSizeMultiplier * (f32)mario->mHeight + 37.0f);
        *(Vec*)&star->field_18 = *(Vec*)&star->field_C;
        *(Vec*)&star->field_24 = *(Vec*)&star->field_C;
        star->field_28 += 50.0f;
        star->field_2C += -1.0f;
        *(Vec*)&star->field_3C = *(const Vec*)&vec3_80300310[0];
        *(Vec*)&star->field_48 = *(const Vec*)&vec3_8030031c[0];
        /* fallthrough */
    case 2:
        star->field_4++;
        if (star->field_4 <= 0x64) {
            star->field_C = intplGetValue(0, star->field_4, 0x64, star->field_18, star->field_24);
            star->field_10 = intplGetValue(0, star->field_4, 0x64, star->field_1C, star->field_28);
            star->field_14 = intplGetValue(0, star->field_4, 0x64, star->field_20, star->field_2C);
            f = intplGetValue(0, star->field_4, 0x64, float_0_80421d84, 2.0f);
            star->field_44 = f;
            star->field_40 = f;
            star->field_3C = f;
        } else {
            star->field_C = star->field_24;
            star->field_10 = star->field_28;
            star->field_14 = star->field_2C;
            star->field_44 = 2.0f;
            star->field_40 = 2.0f;
            star->field_3C = 2.0f;
        }
        star->field_4C = intplGetValue(4, star->field_4, 0x78, float_0_80421d84,
                                       2160.0f);
        if (star->field_4 >= 0x78) {
            Vec v;

            star->field_0 = 3;
            star->field_4 = 0;
            *(Vec*)&star->field_18 = *(Vec*)&star->field_C;
            v = *(const Vec*)&vec3_80300328[0];
            v.x = 200.0f * (f32)rand() / 32767.0f;
            v.z = star->field_20;
            *(Vec*)&star->field_24 = v;
            *(Vec*)&star->field_30 = *(const Vec*)&vec3_80300334[0];
            star->field_34 = (2.0f * (star->field_28 - star->field_1C) -
                              720.0f) / 120.0f;
        }
        break;
    case 3:
        star->field_4++;
        star->field_C = intplGetValue(4, star->field_4, 0x3C, star->field_18, star->field_24);
        star->field_10 += star->field_34;
        star->field_34 += 0.2f;
        star->field_4C = float_0_80421d84;
        star->field_44 = 2.0f;
        star->field_40 = 2.0f;
        star->field_3C = 2.0f;
        if (star->field_4 >= 0x3C) {
            star->field_0 = 4;
            star->field_4 = 0;
            if (irand(2) == 0) {
                star->field_30 = -2.0f;
            }
            star->field_30 = 2.0f;
            star->field_34 = float_0_80421d84;
        }
        break;
    case 4:
        star->field_C += star->field_30;
        if (star->field_C < float_0_80421d84) {
            star->field_30 = 3.0f;
        }
        if (star->field_C > 200.0f) {
            star->field_30 = -3.0f;
        }
        star->field_34 += 0.1f;
        if (star->field_34 >= 6.283184f) {
            star->field_34 -= 6.283184f;
        }
        star->field_10 = 10.0f * (f32)sin(star->field_34) + 180.0f;
        if (genki->field_40 <= 0) {
            star->field_0 = 5;
            star->field_4 = 0;
            *(Vec*)&star->field_C = *(const Vec*)&vec3_80300340[0];
        }
        break;
    case 5:
        break;
    }

    if (star->field_60 != NULL) {
        effStarStoneData* p = star->field_60->userdata;
        p->x = star->field_C;
        p->y = star->field_10;
        p->z = star->field_14;
        p->tipRotY = star->field_48;
        p->rotY = star->field_4C;
        p->tipRotZ = star->field_50;
        p->scale = 1.5f * star->field_3C;
    }
}

// The target's `offset`/`woffset` are MWCC's strength-reduced induction variables for
// `&genki->stars[i]`/`&genki->weapon[i]`; no explicit offset locals.
USER_FUNC(end_genki) {
    GenkiWeapon* weapon;
    s32 i;
    GenkiWork* genki = get_ptr();
    GenkiStar* star;
    s32 j;

    for (i = 0; i < 7; i++) {
        star = &genki->stars[i];
        if (star->field_60 != NULL) {
            effDelete(((effStarStoneData*)star->field_60->userdata)->dustEffect);
            effDelete(star->field_60);
        }
    }
    for (i = 0; i < 7; i++) {
        weapon = &genki->weapon[i];
        if (weapon->field_44 != NULL) {
            effDelete(((effStarStoneData*)weapon->field_44->userdata)->dustEffect);
            effDelete(weapon->field_44);
            for (j = 0; j < 3; j++) {
                effDelete(((effStarStoneData*)weapon->field_48[j]->userdata)->dustEffect);
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

static void disp_2D(CameraId camera, void* param) {
}

// Reading each vector by name (`*(const Vec*)vec3_8030034c`) rather than through a
// `vec3_803002b0` pointer makes MWCC anchor the whole .rodata section in r30 and copies the
// trans/scale/rot/tmp locals into the original's slots. The two final quads call btlDispTex4
// directly. The int->float loads read the compiler's anonymous pool double, which now lands
// at vec3_803002b0+0x1B8 just like retail's double_to_int_80300468. Under MUST_MATCH the five icon colours are named
// locals passed by address, so MWCC allocates them 0x20..0x10 and the final colours 0xc/0x8.
static void disp_3D(CameraId cameraId, void* param) {
    GenkiObject* obj;
    GenkiWork* genki = get_ptr();
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    BattleUnit* party = BattleGetPartyPtr(_battleWorkPointer);
    f32 angle = 3.141592f * genki->field_6C / 180.0f;
    Mtx mtx;
    Vec pos;
    Vec pos2;
    f32 x, y, z;
    f32 px, py, pz;
    u16 flags;
    s32 i;
#ifdef MUST_MATCH
    // Match-only: the original allocates the five icon colours 0x20..0x10 in declaration
    // order, then the two final colours at 0xc/0x8. That needs each colour to be a named
    // local whose address is passed, instead of the anonymous by-value temp MWCC makes for
    // the natural call. The #else forms below use the natural by-value calls.
    GXColor iconC1;
    GXColor iconC2;
    GXColor iconC3;
    GXColor iconC4;
    GXColor iconC5;
#endif

    if (genki->field_4 != 0) {
        BtlUnit_GetHomePos(mario, &x, &y, &z);
        y += 60.0f;
        {
            RecoveryData* ud = genki->field_34->userdata;
            ud->x = x;
            ud->y = y - 10.0f;
            ud->z = z + 5.0f;
        }
        {
            s32 frame;
            if (genki->field_1C >= 10.0f) {
                Vec v = *(const Vec*)vec3_8030034c;
                v.x = x + 6.0f;
                v.y = y;
                v.z = z + 8.0f;
                pos = v;
            } else {
                Vec v = *(const Vec*)vec3_80300358;
                v.x = x;
                v.y = y;
                v.z = z + 8.0f;
                pos = v;
            }
            PSMTXTrans(mtx, pos.x, pos.y, pos.z);
            frame = genki->field_28 % 12;
            if (frame >= 0 && frame <= 5) {
#ifdef MUST_MATCH
                iconC1 = *(GXColor*)&dat_80421d20;
                ((void (*)(Mtx, s32, BOOL, GXColor*))iconNumberDispGx3D)(mtx, (s32)genki->field_1C, 0, &iconC1);
#else
                iconNumberDispGx3D(mtx, (s32)genki->field_1C, 0, *(GXColor*)&dat_80421d20);
#endif

            }
        }
        if (genki->field_50 != -1) {
            BtlUnit_GetHomePos(party, &px, &py, &pz);
            py += 60.0f;
            {
                RecoveryData* ud = genki->field_38->userdata;
                ud->x = px;
                ud->y = py - 10.0f;
                ud->z = pz + 5.0f;
            }
            {
                s32 frame;
                if (genki->field_20 >= 10.0f) {
                    Vec v = *(const Vec*)vec3_80300364;
                    v.x = px + 6.0f;
                    v.y = py;
                    v.z = pz + 8.0f;
                    pos2 = v;
                } else {
                    Vec v = *(const Vec*)vec3_80300370;
                    v.x = px;
                    v.y = py;
                    v.z = pz + 8.0f;
                    pos2 = v;
                }
                PSMTXTrans(mtx, pos2.x, pos2.y, pos2.z);
                frame = genki->field_2C % 12;
                if (frame >= 0 && frame <= 5) {
#ifdef MUST_MATCH
                    iconC2 = *(GXColor*)&dat_80421d24;
                    ((void (*)(Mtx, s32, BOOL, GXColor*))iconNumberDispGx3D)(mtx, (s32)genki->field_20, 0, &iconC2);
#else
                    iconNumberDispGx3D(mtx, (s32)genki->field_20, 0, *(GXColor*)&dat_80421d24);
#endif

                }
            }
            BtlUnit_GetHomePos(mario, &x, &y, &z);
            BtlUnit_GetHomePos(party, &px, &py, &pz);
            x = 0.5f * (x + px);
            y += 90.0f;
            {
                RecoveryData* ud = genki->field_3C->userdata;
                ud->x = x;
                ud->y = y - 10.0f;
                ud->z = z + 5.0f;
            }
            {
                s32 frame;
                if (genki->field_24 >= 10.0f) {
                    Vec v = *(const Vec*)vec3_8030037c;
                    v.x = x + 6.0f;
                    v.y = y;
                    v.z = z + 8.0f;
                    pos = v;
                } else {
                    Vec v = *(const Vec*)vec3_80300388;
                    v.x = x;
                    v.y = y;
                    v.z = z + 8.0f;
                    pos = v;
                }
                PSMTXTrans(mtx, pos.x, pos.y, pos.z);
                frame = genki->field_30 % 12;
                if (frame >= 0 && frame <= 5) {
#ifdef MUST_MATCH
                    iconC3 = *(GXColor*)&dat_80421d28;
                    ((void (*)(Mtx, s32, BOOL, GXColor*))iconNumberDispGx3D)(mtx, (s32)genki->field_24, 0, &iconC3);
#else
                    iconNumberDispGx3D(mtx, (s32)genki->field_24, 0, *(GXColor*)&dat_80421d28);
#endif

                }
            }
        } else {
            BtlUnit_GetHomePos(mario, &x, &y, &z);
            y += 90.0f;
            {
                RecoveryData* ud = genki->field_3C->userdata;
                ud->x = x;
                ud->y = y - 10.0f;
                ud->z = z + 5.0f;
            }
            {
                s32 frame;
                if (genki->field_24 >= 10.0f) {
                    Vec v = *(const Vec*)vec3_80300394;
                    v.x = x + 6.0f;
                    v.y = y;
                    v.z = z + 8.0f;
                    pos = v;
                } else {
                    Vec v = *(const Vec*)vec3_803003a0;
                    v.x = x;
                    v.y = y;
                    v.z = z + 8.0f;
                    pos = v;
                }
                PSMTXTrans(mtx, pos.x, pos.y, pos.z);
                frame = genki->field_30 % 12;
                if (frame >= 0 && frame <= 5) {
#ifdef MUST_MATCH
                    iconC4 = *(GXColor*)&dat_80421d2c;
                    ((void (*)(Mtx, s32, BOOL, GXColor*))iconNumberDispGx3D)(mtx, (s32)genki->field_24, 0, &iconC4);
#else
                    iconNumberDispGx3D(mtx, (s32)genki->field_24, 0, *(GXColor*)&dat_80421d2c);
#endif

                }
            }
        }
    }

    for (i = 0; i < 10; i++) {
        obj = &genki->objects[i];

        if (obj->field_0 != 2) {
            continue;
        }
        flags = 0;
        {
            f32 scale = 1.25f;
            s32 icon;
            s32 frame;

            switch (obj->field_8) {
            case 0:
            case 1:
                icon = 0x1A3;
                if (obj->field_8 == 1) {
                    scale = 2.0f;
                    frame = obj->field_4 % 12;
                    if (frame >= 0 && frame <= 5) {
                        flags |= 0x20;
                    }
                }
                break;
            case 2:
            case 3:
                switch (party->currentKind - 0xE0) {
                case 0:
                    icon = 0x1A4;
                    break;
                case 1:
                    icon = 0x1A5;
                    break;
                case 3:
                    icon = 0x1AE;
                    break;
                case 2:
                    switch (pouchGetPartyColor(4)) {
                    case 1:
                        icon = 0x1A8;
                        break;
                    case 2:
                        icon = 0x1A9;
                        break;
                    case 3:
                        icon = 0x1AA;
                        break;
                    case 4:
                        icon = 0x1AB;
                        break;
                    case 5:
                        icon = 0x1AC;
                        break;
                    case 6:
                        icon = 0x1AD;
                        break;
                    case 0:
                        icon = 0x1A7;
                        break;
                    }
                    break;
                case 4:
                    icon = 0x1AF;
                    break;
                case 5:
                    icon = 0x1A6;
                    break;
                case 6:
                    icon = 0x1B0;
                    break;
                default:
                    icon = 0;
                    break;
                }
                if (obj->field_8 == 3) {
                    scale = 2.0f;
                    frame = obj->field_4 % 12;
                    if (frame >= 0 && frame <= 5) {
                        flags |= 0x20;
                    }
                }
                break;
            case 4:
            case 5:
                icon = 0x1B1;
                if (obj->field_8 == 5) {
                    scale = 2.0f;
                    frame = obj->field_4 % 12;
                    if (frame >= 0 && frame <= 5) {
                        flags |= 0x20;
                    }
                }
                break;
            case 6:
                icon = 0x143;
                break;
            }
            PSMTXScale(mtx, scale, scale, 1.0f);
            PSMTXTransApply(mtx, mtx, obj->field_C,
                            obj->field_10 - 20.0f * scale, obj->field_14);
#ifdef MUST_MATCH
            // The original passes `flags` without a sign extension; with the header's s16
            // parameter MWCC emits `extsh`, so call it through a u16 prototype. The named
            // `iconC5` address (not a by-value temp) gives it the original's 0x10 slot.
            iconC5 = *(GXColor*)&dat_80421d30;
            ((void (*)(Mtx, u16, s32, GXColor*))iconDispGxCol)(mtx, flags, icon, &iconC5);
#else
            iconDispGxCol(mtx, flags, icon, *(GXColor*)&dat_80421d30);
#endif
        }
    }

    if (genki->field_64 >= 2 && genki->field_64 <= 3) {
        Mtx mtx2;

        PSMTXScale(mtx2, 0.5f, 0.5f, 0.5f);
        BtlUnit_GetPos(mario, &x, &y, &z);
        x += 85.0f * (f32)cos(angle);
        y += 85.0f * (f32)sin(angle);
        {
            Vec trans;
            Vec pos;
            Vec scale;
            Vec rot;
            Vec tmp;
            GXColor color;

            x += 10.0f;
            y += 30.0f;
            z += 10.0f;
            color = *(GXColor*)&dat_80421d34;
            tmp = *(const Vec*)vec3_803003c4;
            tmp.z = 6.0f * (f32)genki->field_68;
            rot = tmp;
            scale = *(const Vec*)vec3_803003b8;
            pos = *(const Vec*)vec3_803003ac;
            pos.x = x;
            pos.y = y;
            pos.z = z;
            trans = pos;
            btlDispTex4(0x41, &trans, &scale, &rot, &color);
        }
        {
            Vec trans;
            Vec pos;
            Vec scale;
            Vec rot;
            Vec tmp;
            GXColor color;

            color = *(GXColor*)&dat_80421d38;
            tmp = *(const Vec*)vec3_803003e8;
            tmp.z = 6.0f * -(f32)genki->field_68;
            rot = tmp;
            scale = *(const Vec*)vec3_803003dc;
            pos = *(const Vec*)vec3_803003d0;
            pos.x = x;
            pos.y = y;
            pos.z = z;
            trans = pos;
            btlDispTex4(0x42, &trans, &scale, &rot, &color);
        }
    }
}

static inline void _drawGenkiTex(s32 id, GXColor color, Vec trans, Vec rot, Vec scale) {
    btlDispTex4(id, &trans, &scale, &rot, &color);
}
static inline void _drawGenkiTex2(s32 id, Vec scale, Vec rot, Vec trans, GXColor color) {
    btlDispTex4(id, &trans, &scale, &rot, &color);
}

static inline Vec _makeRot2(const f32* vec, f32 z) {
    Vec v = *(const Vec*)&vec[0x138 / 4];
    v.z = z;
    return v;
}
static inline Vec _makeTrans2(const f32* vec, f32 x, f32 y, f32 z) {
    Vec v = *(const Vec*)&vec[0x120 / 4];
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}
static inline Vec _makeRot1(const f32* vec, f32 z) {
    Vec v = *(const Vec*)&vec[0x114 / 4];
    v.z = z;
    return v;
}
static inline Vec _makeTrans1(const f32* vec, f32 x, f32 y, f32 z) {
    Vec v = *(const Vec*)&vec[0xFC / 4];
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

static void disp_3D_alpha(CameraId camera, void* param) {
}

static GenkiWork* get_ptr(void) {
    return (GenkiWork*)((u8*)_battleWorkPointer + 0x1F4C);
}

// The trailing block scope puts mario/parts in r31 and genki in r29, matching the original;
// at function scope MWCC cycles those two callee-saved registers.
void weapon_entry(void) {
    GenkiWork* genki = get_ptr();
    s32 offsets[10];
    s32 found = 0;
    s32 i;
    s32 idx;
    s32 count;

    {
        BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
        BattleUnitPart* parts = BtlUnit_GetPartsPtr(mario, BtlUnit_GetBodyPartsId(mario));

        count = genki->field_58;
        for (i = 0; i < count; i++) {
            if (genki->weapon[i].field_0 == 0) {
                offsets[found++] = i;
            }
        }
        if (found >= 1) {
            idx = offsets[irand(found)];
            {
                GenkiWeapon* wep = &genki->weapon[idx];

                wep->field_0 = 1;
                wep->field_4 = 7;
            }
            BtlUnit_SetAnim(parts, "M_C_2C");
        }
    }
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
#pragma auto_inline reset

USER_FUNC(start_game) {
    GenkiWork* genki = get_ptr();

    genki->field_4 = 1;
    return 2;
}

USER_FUNC(wait_game_end) {
    GenkiWork* genki = get_ptr();

    return (genki->field_4 == 3) ? 2 : 0;
}

USER_FUNC(get_score) {
    GenkiWork* genki = get_ptr();
    s32* args = event->args;

    evtSetValue(event, args[0], genki->score[0]);
    evtSetValue(event, args[1], genki->score[1]);
    evtSetValue(event, args[2], genki->score[2]);
    return 2;
}

USER_FUNC(status_on) {
    statusWinOpen();
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

USER_FUNC(star_stone_appear) {
    GenkiWork* genki = get_ptr();

    genki->stars[0].field_0 = 1;
    genki->stars[0].field_8 = 1;
    return 2;
}

USER_FUNC(recover_cmd_disable) {
    s32* p = _battleWorkPointer->field_19038;

    p[0] = 0;
    p[1] = 0;
    p[2] = 0;
    p[3] = 0;
    p[4] = 0;
    return 2;
}




// TODO: type
s32 genki_evt_common[345] = {
    0x0003005B, (s32)(void*)btlevtcmd_GetUnitId, 0xFFFFFFFC, 0xFE363C8D,
    0x0003005B, (s32)(void*)btlevtcmd_GetBodyId, 0xFE363C8D, 0xFE363C8E,
    0x0004005B, (s32)(void*)btlevtcmd_CheckStatus, 0xFE363C8D, 0x0000001B,
    0xFE363C80, 0x00020019, 0xFE363C80, 0x00000000,
    0x00020032, 0xFE363C8D, 0xFFFFFFFF, 0x00000021,
    0x0001005B, (s32)(void*)btlevtcmd_reset_move_color_lv_all, 0x0002005B, (s32)(void*)btlevtcmd_CommandPayWeaponCost,
    0xFFFFFFFE, 0x0000006E, 0x0003005B, (s32)(void*)main_genki,
    0xFE363C8D, 0xFE363C8F, 0x00000070, 0x0001005E,
    (s32)(void*)sac_start_camera_evt, 0x0001000A, 0x000001F4, 0x0001005B,
    (s32)(void*)sac_enemy_slide_go, 0x0006005B, (s32)(void*)btlevtcmd_snd_se, 0xFFFFFFFE,
    (s32)(void*)"SFX_BTL_SAC_START1", 0xF1194D80, 0x00000000, 0xF1194D80,
    0x0001005E, (s32)(void*)sac_mario_item_on_evt, 0x0001000A, 0x000001F4,
    0x0006005B, (s32)(void*)btlevtcmd_snd_se, 0xFFFFFFFE, (s32)(void*)"SFX_BTL_SAC_START2",
    0xF1194D80, 0x00000000, 0xFE363C8B, 0x00020018,
    0xFE363C8F, 0x00000000, 0x0001005B, (s32)(void*)star_appear,
    0x00000020, 0x0001005B, (s32)(void*)star_stone_appear, 0x00000021,
    0x0000006E, 0x0001005E, (s32)(void*)sac_start_stardust, 0x00000070,
    0x00010009, 0x00000064, 0x0006005B, (s32)(void*)btlevtcmd_snd_se,
    0xFFFFFFFE, (s32)(void*)"SFX_BTL_SAC_HEART_SHOOT1", 0xF1194D80, 0x00000000,
    0xFE363C8A, 0x0002005B, (s32)(void*)evt_snd_sfxoff, 0xFE363C8B,
    0x0003005B, (s32)(void*)evt_btl_camera_set_mode, 0x00000000, 0x00000000,
    0x00010009, 0x00000028, 0x0001005E, (s32)(void*)sac_mario_item_on_end_evt,
    0x0004005B, (s32)(void*)btlevtcmd_AnimeChangePoseType, 0xFFFFFFFE, 0x00000001,
    0x0000002A, 0x0003005B, (s32)(void*)btlevtcmd_SetMoveSpeed, 0xFFFFFFFE,
    0xF24A8680, 0x0005005B, (s32)(void*)btlevtcmd_GetPos, 0xFFFFFFFE,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0008005B,
    (s32)(void*)btlevtcmd_MovePosition, 0xFFFFFFFE, 0xFFFFFFB5, 0xFE363C81,
    0xFE363C82, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0x0004005B, (s32)(void*)btlevtcmd_AnimeChangePose, 0xFFFFFFFE, 0x00000001,
    (s32)(void*)str_80421D3C, 0x00010009, 0x00000028, 0x0002005B,
    (s32)(void*)evt_snd_sfxoff, 0xFE363C8A, 0x0001005B, (s32)(void*)sac_ac_help_on,
    0x0001005B, (s32)(void*)start_game, 0x0001005B, (s32)(void*)wait_game_end,
    0x0001005B, (s32)(void*)sac_ac_help_off, 0x0004005B, (s32)(void*)get_score,
    0xFE363C8A, 0xFE363C8B, 0xFE363C8C, 0x00010009,
    0x00000046, 0x0004005B, (s32)(void*)btlevtcmd_AnimeChangePoseType, 0xFFFFFFFE,
    0x00000001, 0x0000002A, 0x0005005B, (s32)(void*)btlevtcmd_GetHomePos,
    0xFFFFFFFE, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0003005B, (s32)(void*)btlevtcmd_SetMoveSpeed, 0xFFFFFFFE, 0xF24A8680,
    0x0008005B, (s32)(void*)btlevtcmd_MovePosition, 0xFFFFFFFE, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000000, 0xFFFFFFFF,
    0x00000000, 0x0004005B, (s32)(void*)btlevtcmd_AnimeChangePoseType, 0xFFFFFFFE,
    0x00000001, 0x00000028, 0x0001005B, (s32)(void*)status_on,
    0x0003005B, (s32)(void*)evt_btl_camera_set_mode, 0x00000000, 0x00000007,
    0x0004005B, (s32)(void*)evt_btl_camera_set_homing_unit, 0x00000000, 0xFFFFFFFE,
    0xFFFFFFFF, 0x0003005B, (s32)(void*)evt_btl_camera_set_zoom, 0x00000000,
    0x000000C8, 0x0003005B, (s32)(void*)evt_btl_camera_set_moveSpeedLv, 0x00000000,
    0x00000002, 0x0003005B, (s32)(void*)evt_btl_camera_set_zoomSpeedLv, 0x00000000,
    0x00000002, 0x00020032, 0xFE363C8F, 0xFE363C8A,
    0x00020035, 0xFE363C8F, 0xFE363C8B, 0x00020035,
    0xFE363C8F, 0xFE363C8C, 0x0002001D, 0xFE363C8F,
    0x00000001, 0x0001005B, (s32)(void*)sac_wao, 0x00000021,
    0x00020019, 0xFE363C8D, 0xFFFFFFFF, 0x0003005B,
    (s32)(void*)evt_snd_sfxon, (s32)(void*)"SFX_HP_RECOVER_SHINE2", 0x00000000, 0x00020032,
    0xFE363C89, 0xFFFFFFFE, 0x0001005E, (s32)(void*)status_recover_evt,
    0x0004005B, (s32)(void*)btlevtcmd_RecoverHp, 0xFFFFFFFE, 0x00000001,
    0xFE363C8A, 0x0005005B, (s32)(void*)btlevtcmd_GetPos, 0xFFFFFFFE,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x000F005B,
    (s32)(void*)evt_eff, (s32)(void*)&float_0_80421d44, (s32)(void*)"stardust", 0x00000002,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000032,
    0x00000032, 0x00000032, 0x00000064, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00020032,
    0xFE363C89, 0xFE363C8D, 0x0001005E, (s32)(void*)status_recover_evt,
    0x0002005B, (s32)(void*)btlevtcmd_StartWaitEvent, 0xFE363C8D, 0x0004005B,
    (s32)(void*)btlevtcmd_RecoverHp, 0xFE363C8D, 0xFE363C8E, 0xFE363C8B,
    0x0005005B, (s32)(void*)btlevtcmd_GetPos, 0xFE363C8D, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x000F005B, (s32)(void*)evt_eff,
    (s32)(void*)&float_0_80421d44, (s32)(void*)"stardust", 0x00000002, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000032, 0x00000032,
    0x00000032, 0x00000064, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0004005B, (s32)(void*)btlevtcmd_RecoverFp,
    0xFFFFFFFE, 0x00000001, 0xFE363C8C, 0x00000020,
    0x0003005B, (s32)(void*)evt_snd_sfxon, (s32)(void*)"SFX_HP_RECOVER_SHINE2", 0x00000000,
    0x00020032, 0xFE363C89, 0xFFFFFFFE, 0x0001005E,
    (s32)(void*)status_recover_evt, 0x0004005B, (s32)(void*)btlevtcmd_RecoverHp, 0xFFFFFFFE,
    0x00000001, 0xFE363C8A, 0x0005005B, (s32)(void*)btlevtcmd_GetPos,
    0xFFFFFFFE, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x000F005B, (s32)(void*)evt_eff, (s32)(void*)&float_0_80421d44, (s32)(void*)"stardust",
    0x00000002, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000032, 0x00000032, 0x00000032, 0x00000064,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0004005B, (s32)(void*)btlevtcmd_RecoverFp, 0xFFFFFFFE, 0x00000001,
    0xFE363C8C, 0x00000021, 0x0001000A, 0x000003E8,
    0x0001005B, (s32)(void*)sac_enemy_slide_return, 0x0001000A, 0x000003E8,
    0x0003005B, (s32)(void*)evt_btl_camera_set_mode, 0x00000000, 0x00000000,
    0x0001000A, 0x000003E8, 0x0001005B, (s32)(void*)end_genki,
    0x0002005B, (s32)(void*)btlevtcmd_StartWaitEvent, 0xFFFFFFFE, 0x00000002,
    0x00000001,
};

// TODO: type
s32 status_recover_evt[180] = {
    0x0004005B, (s32)(void*)btlevtcmd_CheckStatus, 0xFE363C89, 0x00000001,
    0xFE363C80, 0x00020019, 0xFE363C80, 0x00000000,
    0x0006005B, (s32)(void*)btlevtcmd_OnOffStatus, 0xFE363C89, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, 0x00000021,
    0x0004005B, (s32)(void*)btlevtcmd_CheckStatus, 0xFE363C89, 0x00000002,
    0xFE363C80, 0x00020019, 0xFE363C80, 0x00000000,
    0x0006005B, (s32)(void*)btlevtcmd_OnOffStatus, 0xFE363C89, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x00000021,
    0x0004005B, (s32)(void*)btlevtcmd_CheckStatus, 0xFE363C89, 0x00000003,
    0xFE363C80, 0x00020019, 0xFE363C80, 0x00000000,
    0x0006005B, (s32)(void*)btlevtcmd_OnOffStatus, 0xFE363C89, 0x00000003,
    0x00000000, 0x00000000, 0x00000000, 0x00000021,
    0x0004005B, (s32)(void*)btlevtcmd_CheckStatus, 0xFE363C89, 0x00000004,
    0xFE363C80, 0x00020019, 0xFE363C80, 0x00000000,
    0x0006005B, (s32)(void*)btlevtcmd_OnOffStatus, 0xFE363C89, 0x00000004,
    0x00000000, 0x00000000, 0x00000000, 0x00000021,
    0x0004005B, (s32)(void*)btlevtcmd_CheckStatus, 0xFE363C89, 0x00000005,
    0xFE363C80, 0x00020019, 0xFE363C80, 0x00000000,
    0x0006005B, (s32)(void*)btlevtcmd_OnOffStatus, 0xFE363C89, 0x00000005,
    0x00000000, 0x00000000, 0x00000000, 0x00000021,
    0x0004005B, (s32)(void*)btlevtcmd_CheckStatus, 0xFE363C89, 0x00000008,
    0xFE363C80, 0x00020019, 0xFE363C80, 0x00000000,
    0x0006005B, (s32)(void*)btlevtcmd_OnOffStatus, 0xFE363C89, 0x00000008,
    0x00000000, 0x00000000, 0x00000000, 0x00000021,
    0x0004005B, (s32)(void*)btlevtcmd_CheckStatus, 0xFE363C89, 0x00000009,
    0xFE363C80, 0x00020019, 0xFE363C80, 0x00000000,
    0x0006005B, (s32)(void*)btlevtcmd_OnOffStatus, 0xFE363C89, 0x00000009,
    0x00000000, 0x00000000, 0x00000000, 0x00000021,
    0x0004005B, (s32)(void*)btlevtcmd_CheckStatus, 0xFE363C89, 0x0000000D,
    0xFE363C80, 0x00020019, 0xFE363C80, 0x00000000,
    0x0006005B, (s32)(void*)btlevtcmd_OnOffStatus, 0xFE363C89, 0x0000000D,
    0x00000000, 0x00000000, 0x00000000, 0x00000021,
    0x0004005B, (s32)(void*)btlevtcmd_CheckStatus, 0xFE363C89, 0x0000000B,
    0xFE363C80, 0x00020019, 0xFE363C80, 0x00000000,
    0x0006005B, (s32)(void*)btlevtcmd_OnOffStatus, 0xFE363C89, 0x0000000B,
    0x00000000, 0x00000000, 0x00000000, 0x00000021,
    0x0004005B, (s32)(void*)btlevtcmd_CheckStatus, 0xFE363C89, 0x0000000F,
    0xFE363C80, 0x00020019, 0xFE363C80, 0x00000000,
    0x0006005B, (s32)(void*)btlevtcmd_OnOffStatus, 0xFE363C89, 0x0000000F,
    0x00000000, 0x00000000, 0x00000000, 0x00000021,
    0x0004005B, (s32)(void*)btlevtcmd_CheckStatus, 0xFE363C89, 0x00000014,
    0xFE363C80, 0x00020019, 0xFE363C80, 0x00000000,
    0x0006005B, (s32)(void*)btlevtcmd_OnOffStatus, 0xFE363C89, 0x00000014,
    0x00000000, 0x00000000, 0x00000000, 0x00000021,
    0x0001005B, (s32)(void*)recover_cmd_disable, 0x00000002, 0x00000001,
};

// TODO: type
s32 marioAttackEvent_Genki0[7] = {
    0x00020032, 0xFE363C8F, 0x00000000, 0x0001005E,
    (s32)(void*)genki_evt_common, 0x00000002, 0x00000001,
};

// TODO: type
s32 marioAttackEvent_Genki1[7] = {
    0x00020032, 0xFE363C8F, 0x00000001, 0x0001005E,
    (s32)(void*)genki_evt_common, 0x00000002, 0x00000001,
};

