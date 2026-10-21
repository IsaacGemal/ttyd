#pragma once

#include <dolphin/types.h>
#include "mario/mario_pouch.h"

typedef enum BattleWeaponSpecialFlags { SPECIAL_UNGUARDABLE = 0x20000 } BattleWeaponSpecialFlags;

typedef struct BattleWeapon {
    const char* name;                 // 0x0
    u16 icon;                         // 0x4
    ItemType item;                    // 0x8
    const char* description;          // 0xC
    u8 baseAccuracy;                  // 0x10
    u8 baseFpCost;                    // 0x11
    u8 baseSpCost;                    // 0x12
    u8 superguardType;                // 0x13, BattleCheckDamage switches on 1/2/3
    f32 field_0x14;                   // 0x14
    s8 mStylishCommandMultiplier;     // 0x18
    s8 field_0x19;                    // 0x19
    u8 mBingoSlotChance;              // 0x1A
    s8 field_0x1B;                    // 0x1B
    u32 (*hpDamageFn)(struct BattleUnit* unit1, struct BattleWeapon* weapon, struct BattleUnit* unit2,
                      struct BattleWorkUnitPart* part); // 0x1C, weaponGetPower*; BattleCalculateDamage's base damage
    u32 hpDamageWork[8];              // 0x20
    u32 (*fpDamageFn)(struct BattleUnit* unit1, struct BattleWeapon* weapon, struct BattleUnit* unit2,
                      struct BattleWorkUnitPart* part); // 0x40, weaponGetFPPower*; BattleCalculateFpDamage's base damage
    u32 fpDamageWork[8];              // 0x44
    u32 targetClassFlags;             // 0x64
    u32 targetPropertyFlags;          // 0x68
    u8 element;                       // 0x6C, 1, 3 and 4 bypass the special-property check in BattleCheckDamage
    u8 field_0x6D;                    // 0x6D, passed to BattleDamageDirect
    u8 actionLevel;                   // 0x6E, btlevtcmd_GetWeaponActionLv
    s8 field_0x6F;                    // 0x6F
    const char* acHelpMsg;            // 0x70
    BattleWeaponSpecialFlags special; // 0x74
    u32 counterResistanceFlags;       // 0x78
    u32 targetWeightingFlags;         // 0x7C
    // 0x80-0xAD: status chances (%, unsigned) and turn counts/strengths (signed), read by
    // _getSickStatusRate/_getSickStatusParam
    u8 sleepChance;                   // 0x80
    s8 sleepTime;                     // 0x81
    u8 stopChance;                    // 0x82
    s8 stopTime;                      // 0x83
    u8 dizzyChance;                   // 0x84
    s8 dizzyTime;                     // 0x85
    u8 poisonChance;                  // 0x86
    s8 poisonTime;                    // 0x87
    s8 poisonStrength;                // 0x88
    u8 confuseChance;                 // 0x89
    s8 confuseTime;                   // 0x8A
    u8 electricChance;                // 0x8B
    s8 electricTime;                  // 0x8C
    u8 dodgyChance;                   // 0x8D
    s8 dodgyTime;                     // 0x8E
    u8 burnChance;                    // 0x8F
    s8 burnTime;                      // 0x90
    u8 freezeChance;                  // 0x91
    s8 freezeTime;                    // 0x92
    u8 sizeChangeChance;              // 0x93
    s8 sizeChangeTime;                // 0x94
    s8 sizeChangeStrength;            // 0x95, > 0 huge, < 0 tiny
    u8 atkChangeChance;               // 0x96
    s8 atkChangeTime;                 // 0x97
    s8 atkChangeStrength;             // 0x98, > 0 up, < 0 down
    u8 defChangeChance;               // 0x99
    s8 defChangeTime;                 // 0x9A
    s8 defChangeStrength;             // 0x9B, > 0 up, < 0 down
    u8 allergicChance;                // 0x9C
    s8 allergicTime;                  // 0x9D
    u8 ohkoChance;                    // 0x9E
    s8 chargeStrength;                // 0x9F
    u8 fastChance;                    // 0xA0
    s8 fastTime;                      // 0xA1
    u8 slowChance;                    // 0xA2
    s8 slowTime;                      // 0xA3
    u8 frightChance;                  // 0xA4
    u8 galeForceChance;               // 0xA5
    s8 paybackTime;                   // 0xA6
    s8 holdFastTime;                  // 0xA7
    u8 invisibleChance;               // 0xA8
    s8 invisibleTime;                 // 0xA9
    s8 hpRegenTime;                   // 0xAA
    s8 hpRegenStrength;               // 0xAB
    s8 fpRegenTime;                   // 0xAC
    s8 fpRegenStrength;               // 0xAD
    void* mAttackEvtScript;           // 0xB0
    u8 field_B4[0xC0 - 0xB4];         // 0xB4
} BattleWeapon;
