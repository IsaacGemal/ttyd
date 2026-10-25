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
// .rodata: the Vec literals of main_weapon, main_star0 and main_star1 and main_object's
// initialiser open the section; disp_3D's vectors follow it, then the strings.
extern const f32 vec3_8030034c[3];
extern const f32 vec3_80300358[3];
extern const f32 vec3_80300364[3];
extern const f32 vec3_80300370[3];
extern const f32 vec3_8030037c[3];
extern const f32 vec3_80300388[3];
extern const f32 vec3_80300394[3];
extern const f32 vec3_803003a0[3];
extern const f32 vec3_803003ac[3];
extern const f32 vec3_803003b8[3];
extern const f32 vec3_803003c4[3];
extern const f32 vec3_803003d0[3];
extern const f32 vec3_803003dc[3];
extern const f32 vec3_803003e8[3];

// .sdata2: the M_I_Y animation name and the .data-referenced zero are defined after disp_3D,
// whose colour literals' templates open the section. The float constants are file-scope so
// the functions can name them, and MWCC folds each one into its literal pool (the static
// definitions vanish, leaving the pooled words in the original's order).
extern const char str_80421D3C[6];
extern const f32 float_0_80421d44;

typedef struct GenkiStar {
    s32 field_0;                // 0x0
    s32 field_4;                // 0x4
    s32 field_8;                // 0x8
    Vec field_C;                // 0xC
    Vec field_18;               // 0x18
    Vec field_24;               // 0x24
    Vec field_30;               // 0x30
    Vec field_3C;               // 0x3C
    Vec field_48;               // 0x48
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
        genki->field_40 = 0x384;
        genki->score[2] = 0;
        genki->score[1] = 0;
        genki->score[0] = 0;
        genki->field_24 = 0.0f;
        genki->field_20 = 0.0f;
        genki->field_1C = 0.0f;
        if (genki->field_C == 0) {
            genki->field_58 = 3;
        } else {
            genki->field_58 = 7;
        }
        if (genki->field_C == 0) {
            genki->field_5C = BattleAlloc(0x64);
        } else {
            genki->field_5C = BattleAlloc(0xC8);
        }
        genki->field_34 = effRecoveryEntry(2, 0, 0.0f, -1000.0f, 0.0f);
        genki->field_38 = effRecoveryEntry(2, 0, 0.0f, -1000.0f, 0.0f);
        genki->field_3C = effRecoveryEntry(3, 0, 0.0f, -1000.0f, 0.0f);
        for (i = 0; i < 7; i++) {
            genki->weapon[i].field_44 = effStarStoneEntry(i, 0.0f, -1000.0f, 0.0f, 1.0f);
            for (j = 0; j < 3; j++) {
                genki->weapon[i].field_48[j] = effStarStoneEntry(i + 7, 0.0f, -1000.0f, 0.0f, 1.0f);
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
    dispEntry(4, 0.0f, 1, disp_3D, NULL);
    dispEntry(4, 0.0f, 2, disp_3D_alpha, NULL);
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
    s32 stickX = keyGetStickX(0);
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
        if (genki->field_6C <= 0.0f) {
            genki->field_6C = 0.0f;
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
        weapon->pos[0] = (Vec){0.0f, -1000.0f, 0.0f};
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
        weapon->vel = (Vec){10.0f * (f32)cos(angle), 10.0f * (f32)sin(angle), 0.0f};
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
            effStarStoneData* data = weapon->field_44->userdata;

            data->x = weapon->pos[0].x;
            data->y = weapon->pos[0].y;
            data->z = weapon->pos[0].z;
            data->scale = scale;
            data->tevReg0A = 0xFF;
        }
        for (i = 0; i < 3; i++) {
            effStarStoneData* data = weapon->field_48[i]->userdata;

            data->x = weapon->pos[i + 1].x;
            data->y = weapon->pos[i + 1].y;
            data->z = weapon->pos[i + 1].z;
            data->scale = scale;
            data->tevReg0A = 0xFF - (i + 1) * 0x32;
        }
    }
}

// Matches. The int->float loads read the anonymous pool double (at .rodata+0x1B8, like
// retail's double_to_int_80300468) with an absolute `lis`, and the 30.0f literal as
// `@NNN` instead of retail's `float_30_80421d80`; objdiff does not score those symbol names. The
// `30.0f` literal, rather than a named constant (same value), is what makes MWCC emit
// the original's interleaved `li`/`lfs` argument setup. Keeping `fifteen` in a variable fixed the
// compare's f1/f2 assignment; the if/else that builds `limit` fixed the case-1 integer register
// allocation.
void main_object(s32 idx) {
    GenkiWork* genki = get_ptr();
    GenkiObject* obj = &genki->objects[idx];
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    s32 local[3] = {0, 4, 2};
    GenkiStar* star;
    f32 x, y, z;

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
            obj->field_C = star->field_C.x;
            obj->field_10 = star->field_C.y;
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

// 100%: the Vec literals make MWCC anchor the .rodata section base in r31, fold the
// anonymous int->float pool double into `lfd fX, 0x1b8(r31)`, and pick the original's
// callee-saved map (idx=r28, mario=r29, star=r30, vec=r31, genki=r27).
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
        BtlUnit_GetPos(mario, &star->field_C.x, &star->field_C.y, &star->field_C.z);
        star->field_C.y = star->field_C.y +
            (mario->mSizeMultiplier * (f32)mario->mHeight + 37.0f);
        star->field_18 = star->field_C;
        star->field_24 = star->field_C;
        star->field_24.y += 30.0f;
        star->field_3C = (Vec){0.0f, 0.0f, 0.0f};
        star->field_48 = (Vec){0.0f, 0.0f, 0.0f};
        star->field_54 = 0.0f;
        star->field_58 = 6.283184f * (f32)idx / 7.0f;
        star->field_5C = 0.1f;
        /* fallthrough */
    case 2:
        star->field_4++;
        if (star->field_4 <= 0x64) {
            star->field_C.y = intplGetValue(0, star->field_4, 0x64, star->field_18.y, star->field_24.y);
            BtlUnit_GetPos(mario, &x, &y, &z);
            star->field_54 = intplGetValue(0, star->field_4, 0x64, 0.0f,
                                           40.0f);
            star->field_58 += star->field_5C;
            star->field_C.x = x + star->field_54 * (f32)cos(star->field_58);
            star->field_C.z = z + star->field_54 * (f32)sin(star->field_58);
            f = intplGetValue(0, star->field_4, 0x64, 0.0f, 0.75f);
            star->field_3C.z = f;
            star->field_3C.y = f;
            star->field_3C.x = f;
        } else {
            star->field_C.y = star->field_24.y;
            BtlUnit_GetPos(mario, &x, &y, &z);
            star->field_58 += star->field_5C;
            star->field_C.x = x + star->field_54 * (f32)cos(star->field_58);
            star->field_C.z = z + star->field_54 * (f32)sin(star->field_58);
            star->field_3C.z = 0.75f;
            star->field_3C.y = 0.75f;
            star->field_3C.x = 0.75f;
        }
        star->field_48.y = intplGetValue(4, star->field_4, 0x78, 0.0f,
                                       2160.0f);
        if (star->field_4 >= 0x78) {
            star->field_0 = 3;
            star->field_4 = 0;
            star->field_18 = star->field_C;
            star->field_24 = (Vec){200.0f * (f32)rand() / 32767.0f, 180.0f, star->field_18.z};
            star->field_30 = (Vec){0.0f, 0.0f, 0.0f};
            star->field_30.y = (2.0f * (star->field_24.y - star->field_18.y) -
                              (60.0f * (60.0f *
                               (0.2f - 0.01f * (f32)idx)))) /
                             120.0f;
        }
        break;
    case 3:
        star->field_4++;
        star->field_C.x = intplGetValue(4, star->field_4, 0x3C, star->field_18.x, star->field_24.x);
        star->field_C.y += star->field_30.y;
        star->field_30.y += 0.2f - 0.01f * (f32)idx;
        star->field_48.y = 0.0f;
        star->field_3C.z = 0.75f;
        star->field_3C.y = 0.75f;
        star->field_3C.x = 0.75f;
        if (star->field_4 >= 0x3C) {
            star->field_0 = 4;
            star->field_4 = 0;
            if (irand(2) == 0) {
                star->field_30.x = -2.0f - 0.05f * (f32)idx;
            }
            star->field_30.x = 2.0f + 0.05f * (f32)idx;
            star->field_30.y = 0.0f;
        }
        break;
    case 4:
        star->field_C.x += star->field_30.x;
        if (star->field_C.x < 0.0f) {
            star->field_30.x = 3.0f + 0.05f * (f32)idx;
        }
        if (star->field_C.x > 200.0f) {
            star->field_30.x = -3.0f - 0.05f * (f32)idx;
        }
        star->field_30.y += 0.1f + 0.01f * (f32)idx;
        if (star->field_30.y >= 6.283184f) {
            star->field_30.y -= 6.283184f;
        }
        star->field_C.y = 10.0f * (f32)sin(star->field_30.y) + 180.0f;
        if (genki->field_40 <= 0) {
            star->field_0 = 5;
            star->field_4 = 0;
            star->field_C = (Vec){0.0f, -1000.0f, 0.0f};
        }
        break;
    case 5:
        break;
    }

    if (star->field_60 != NULL) {
        effStarStoneData* p = star->field_60->userdata;
        p->x = star->field_C.x;
        p->y = star->field_C.y;
        p->z = star->field_C.z;
        p->tipRotY = star->field_48.x;
        p->rotY = star->field_48.y;
        p->tipRotZ = star->field_48.z;
        p->scale = 1.5f * star->field_3C.x;
    }
}

// 100%: the Vec literals make MWCC anchor the .rodata section base in r30, so its
// anonymous int->float pool double (at .rodata+0x1B8) folds into `lfd fY, 0x1b8(r30)`
// exactly as in the original. Logic, schedule and every constant match the assembly.
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
        star->field_60 = effStarStoneEntry(4, 0.0f, -1000.0f, 0.0f, 1.0f);
        BtlUnit_GetPos(mario, &star->field_C.x, &star->field_C.y, &star->field_C.z);
        star->field_C.y = star->field_C.y +
            (mario->mSizeMultiplier * (f32)mario->mHeight + 37.0f);
        star->field_18 = star->field_C;
        star->field_24 = star->field_C;
        star->field_24.y += 50.0f;
        star->field_24.z += -1.0f;
        star->field_3C = (Vec){0.0f, 0.0f, 0.0f};
        star->field_48 = (Vec){0.0f, 0.0f, 0.0f};
        /* fallthrough */
    case 2:
        star->field_4++;
        if (star->field_4 <= 0x64) {
            star->field_C.x = intplGetValue(0, star->field_4, 0x64, star->field_18.x, star->field_24.x);
            star->field_C.y = intplGetValue(0, star->field_4, 0x64, star->field_18.y, star->field_24.y);
            star->field_C.z = intplGetValue(0, star->field_4, 0x64, star->field_18.z, star->field_24.z);
            f = intplGetValue(0, star->field_4, 0x64, 0.0f, 2.0f);
            star->field_3C.z = f;
            star->field_3C.y = f;
            star->field_3C.x = f;
        } else {
            star->field_C.x = star->field_24.x;
            star->field_C.y = star->field_24.y;
            star->field_C.z = star->field_24.z;
            star->field_3C.z = 2.0f;
            star->field_3C.y = 2.0f;
            star->field_3C.x = 2.0f;
        }
        star->field_48.y = intplGetValue(4, star->field_4, 0x78, 0.0f,
                                       2160.0f);
        if (star->field_4 >= 0x78) {
            star->field_0 = 3;
            star->field_4 = 0;
            star->field_18 = star->field_C;
            star->field_24 = (Vec){200.0f * (f32)rand() / 32767.0f, 180.0f, star->field_18.z};
            star->field_30 = (Vec){0.0f, 0.0f, 0.0f};
            star->field_30.y = (2.0f * (star->field_24.y - star->field_18.y) -
                              720.0f) / 120.0f;
        }
        break;
    case 3:
        star->field_4++;
        star->field_C.x = intplGetValue(4, star->field_4, 0x3C, star->field_18.x, star->field_24.x);
        star->field_C.y += star->field_30.y;
        star->field_30.y += 0.2f;
        star->field_48.y = 0.0f;
        star->field_3C.z = 2.0f;
        star->field_3C.y = 2.0f;
        star->field_3C.x = 2.0f;
        if (star->field_4 >= 0x3C) {
            star->field_0 = 4;
            star->field_4 = 0;
            if (irand(2) == 0) {
                star->field_30.x = -2.0f;
            }
            star->field_30.x = 2.0f;
            star->field_30.y = 0.0f;
        }
        break;
    case 4:
        star->field_C.x += star->field_30.x;
        if (star->field_C.x < 0.0f) {
            star->field_30.x = 3.0f;
        }
        if (star->field_C.x > 200.0f) {
            star->field_30.x = -3.0f;
        }
        star->field_30.y += 0.1f;
        if (star->field_30.y >= 6.283184f) {
            star->field_30.y -= 6.283184f;
        }
        star->field_C.y = 10.0f * (f32)sin(star->field_30.y) + 180.0f;
        if (genki->field_40 <= 0) {
            star->field_0 = 5;
            star->field_4 = 0;
            star->field_C = (Vec){0.0f, -1000.0f, 0.0f};
        }
        break;
    case 5:
        break;
    }

    if (star->field_60 != NULL) {
        effStarStoneData* p = star->field_60->userdata;
        p->x = star->field_C.x;
        p->y = star->field_C.y;
        p->z = star->field_C.z;
        p->tipRotY = star->field_48.x;
        p->rotY = star->field_48.y;
        p->tipRotZ = star->field_48.z;
        p->scale = 1.5f * star->field_3C.x;
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

// Reading each vector by name (`*(const Vec*)vec3_8030034c`) rather than through a base
// pointer makes MWCC anchor the whole .rodata section in r30 and copies the
// trans/scale/rot/tmp locals into the original's slots. The two final quads call btlDispTex4
// directly. The int->float loads read the compiler's anonymous pool double, which lands
// at .rodata+0x1B8 just like retail's double_to_int_80300468. Under MUST_MATCH the five icon colours are named
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
                iconC1 = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
                ((void (*)(Mtx, s32, BOOL, GXColor*))iconNumberDispGx3D)(mtx, (s32)genki->field_1C, 0, &iconC1);
#else
                iconNumberDispGx3D(mtx, (s32)genki->field_1C, 0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
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
                    iconC2 = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
                    ((void (*)(Mtx, s32, BOOL, GXColor*))iconNumberDispGx3D)(mtx, (s32)genki->field_20, 0, &iconC2);
#else
                    iconNumberDispGx3D(mtx, (s32)genki->field_20, 0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
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
                    iconC3 = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
                    ((void (*)(Mtx, s32, BOOL, GXColor*))iconNumberDispGx3D)(mtx, (s32)genki->field_24, 0, &iconC3);
#else
                    iconNumberDispGx3D(mtx, (s32)genki->field_24, 0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
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
                    iconC4 = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
                    ((void (*)(Mtx, s32, BOOL, GXColor*))iconNumberDispGx3D)(mtx, (s32)genki->field_24, 0, &iconC4);
#else
                    iconNumberDispGx3D(mtx, (s32)genki->field_24, 0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
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
            iconC5 = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
            ((void (*)(Mtx, u16, s32, GXColor*))iconDispGxCol)(mtx, flags, icon, &iconC5);
#else
            iconDispGxCol(mtx, flags, icon, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
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
            color = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
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

            color = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
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
const char str_80421D3C[] = "M_I_Y";
const f32 float_0_80421d44 = 0.0f;

static void disp_3D_alpha(CameraId camera, void* param) {
}

static GenkiWork* get_ptr(void) {
    return (GenkiWork*)&_battleWorkPointer->actionCommands.acWork[0];
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
    s32* p = _battleWorkPointer->cmdDisable;

    p[0] = 0;
    p[1] = 0;
    p[2] = 0;
    p[3] = 0;
    p[4] = 0;
    return 2;
}




EVT_BEGIN(genki_evt_common)
    CALL(btlevtcmd_GetUnitId, -4, LW(13))
    CALL(btlevtcmd_GetBodyId, LW(13), LW(14))
    CALL(btlevtcmd_CheckStatus, LW(13), 27, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(13), -1)
    END_IF
    CALL(btlevtcmd_reset_move_color_lv_all)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    BEGIN_SIBLING_EVENT
        CALL(main_genki, LW(13), LW(15))
    END_SIBLING_EVENT
    CALL_CHILD(sac_start_camera_evt)
    WAIT_MS(500)
    CALL(sac_enemy_slide_go)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_SAC_START1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL_CHILD(sac_mario_item_on_evt)
    WAIT_MS(500)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_SAC_START2"), EVTDAT_POINTER_MAX, 0, LW(11))
    IF_INT_EQUAL(LW(15), 0)
        CALL(star_appear)
    ELSE
        CALL(star_stone_appear)
    END_IF
    BEGIN_SIBLING_EVENT
        CALL_CHILD(sac_start_stardust)
    END_SIBLING_EVENT
    WAIT_FRAMES(100)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_SAC_HEART_SHOOT1"), EVTDAT_POINTER_MAX, 0, LW(10))
    CALL(evt_snd_sfxoff, LW(11))
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(40)
    CALL_CHILD(sac_mario_item_on_end_evt)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, -75, LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, PTR((void*)str_80421D3C))
    WAIT_FRAMES(40)
    CALL(evt_snd_sfxoff, LW(10))
    CALL(sac_ac_help_on)
    CALL(start_game)
    CALL(wait_game_end)
    CALL(sac_ac_help_off)
    CALL(get_score, LW(10), LW(11), LW(12))
    WAIT_FRAMES(70)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(status_on)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoomSpeedLv, 0, 2)
    SET_VALUE(LW(15), LW(10))
    ADD(LW(15), LW(11))
    ADD(LW(15), LW(12))
    IF_INT_GREATER_OR_EQUAL(LW(15), 1)
        CALL(sac_wao)
    END_IF
    IF_INT_NOT_EQUAL(LW(13), -1)
        CALL(evt_snd_sfxon, STRING("SFX_HP_RECOVER_SHINE2"), 0)
        SET_VALUE(LW(9), -2)
        CALL_CHILD(status_recover_evt)
        CALL(btlevtcmd_RecoverHp, -2, 1, LW(10))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(evt_eff, PTR((void*)&float_0_80421d44), STRING("stardust"), 2, LW(0), LW(1), LW(2), 50, 50, 50, 100, 0, 0, 0, 0)
        SET_VALUE(LW(9), LW(13))
        CALL_CHILD(status_recover_evt)
        CALL(btlevtcmd_StartWaitEvent, LW(13))
        CALL(btlevtcmd_RecoverHp, LW(13), LW(14), LW(11))
        CALL(btlevtcmd_GetPos, LW(13), LW(0), LW(1), LW(2))
        CALL(evt_eff, PTR((void*)&float_0_80421d44), STRING("stardust"), 2, LW(0), LW(1), LW(2), 50, 50, 50, 100, 0, 0, 0, 0)
        CALL(btlevtcmd_RecoverFp, -2, 1, LW(12))
    ELSE
        CALL(evt_snd_sfxon, STRING("SFX_HP_RECOVER_SHINE2"), 0)
        SET_VALUE(LW(9), -2)
        CALL_CHILD(status_recover_evt)
        CALL(btlevtcmd_RecoverHp, -2, 1, LW(10))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(evt_eff, PTR((void*)&float_0_80421d44), STRING("stardust"), 2, LW(0), LW(1), LW(2), 50, 50, 50, 100, 0, 0, 0, 0)
        CALL(btlevtcmd_RecoverFp, -2, 1, LW(12))
    END_IF
    WAIT_MS(1000)
    CALL(sac_enemy_slide_return)
    WAIT_MS(1000)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(1000)
    CALL(end_genki)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(status_recover_evt)
    CALL(btlevtcmd_CheckStatus, LW(9), 1, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_OnOffStatus, LW(9), 1, 0, 0, 0)
    END_IF
    CALL(btlevtcmd_CheckStatus, LW(9), 2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_OnOffStatus, LW(9), 2, 0, 0, 0)
    END_IF
    CALL(btlevtcmd_CheckStatus, LW(9), 3, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_OnOffStatus, LW(9), 3, 0, 0, 0)
    END_IF
    CALL(btlevtcmd_CheckStatus, LW(9), 4, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_OnOffStatus, LW(9), 4, 0, 0, 0)
    END_IF
    CALL(btlevtcmd_CheckStatus, LW(9), 5, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_OnOffStatus, LW(9), 5, 0, 0, 0)
    END_IF
    CALL(btlevtcmd_CheckStatus, LW(9), 8, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_OnOffStatus, LW(9), 8, 0, 0, 0)
    END_IF
    CALL(btlevtcmd_CheckStatus, LW(9), 9, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_OnOffStatus, LW(9), 9, 0, 0, 0)
    END_IF
    CALL(btlevtcmd_CheckStatus, LW(9), 13, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_OnOffStatus, LW(9), 13, 0, 0, 0)
    END_IF
    CALL(btlevtcmd_CheckStatus, LW(9), 11, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_OnOffStatus, LW(9), 11, 0, 0, 0)
    END_IF
    CALL(btlevtcmd_CheckStatus, LW(9), 15, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_OnOffStatus, LW(9), 15, 0, 0, 0)
    END_IF
    CALL(btlevtcmd_CheckStatus, LW(9), 20, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_OnOffStatus, LW(9), 20, 0, 0, 0)
    END_IF
    CALL(recover_cmd_disable)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_Genki0)
    SET_VALUE(LW(15), 0)
    CALL_CHILD(genki_evt_common)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_Genki1)
    SET_VALUE(LW(15), 1)
    CALL_CHILD(genki_evt_common)
    RETURN
EVT_END()

