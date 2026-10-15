#pragma once

#include <dolphin/mtx.h>

/* typedef union SoundData {
    const char* name;
    s32 value;
} SoundData; */

typedef struct SoundEnvList {
    u16 field_0x0;  // 0x0
    u16 field_0x2;  // 0x2
    u8 field_0x4;   // 0x4
    u8 field_0x5;   // 0x5
    u8 field_0x6;   // 0x6
    u8 field_0x7;   // 0x7
    u8 field_0x8;   // 0x8
    u32 field_0xC;  // 0xC
    u32 field_0x10; // 0x10
    u32 field_0x14; // 0x14
    u8 field_0x18;  // 0x18
    u8 field_0x19;  // 0x19
    u8 field_0x1A;  // 0x1A
    // pad 1 byte
} SoundEnvList;

typedef enum SoundEffectFlags {
    SFX_FLAG_2 = (1 << 29),
    SFX_FLAG_4 = (1 << 30),
    SFX_FLAG_8 = (1 << 31)
} SoundEffectFlags;

// most are translated in __psndSFXOn, TODO
typedef struct SoundEffectEntry {
    const char* name;      // 0x0, sfx name
    u32 field_0x4;         // 0x4, lower 2 bytes are SND_FXID
    const char* field_0x8; // 0x8, .stm name
    u8 field_0xC;          // 0xC
    u8 field_0xD;          // 0xD
    u8 field_0xE;          // 0xE
    u8 field_0xF;          // 0xF, reverb
    u8 field_0x10;         // 0x10
    u8 field_0x11;         // 0x11
    u16 field_0x12;        // 0x12
} SoundEffectEntry;

typedef struct PaperSoundBGM {
    s32 unk0;              // 0x0
    s32 streamId;          // 0x4, TODO verify re: seq_title
    u8 unk8[0x20 - 0x8];   // 0x8
    s8 unk20;              // 0x20
    u8 unk21[0x38 - 0x21]; // 0x21
} PaperSoundBGM;

typedef struct PaperSoundEffect {
    s32 unk0;              // 0x0
    u8 unk4[0x6 - 0x4];    // 0x4
    s16 unk6;              // 0x6
    u8 unk8[0x18 - 0x8];   // 0x8
    s32 unk18;             // 0x0
    u8 unk1C[0x28 - 0x1C]; // 0x1C
} PaperSoundEffect;

// Named-field view of PaperSoundEffect used by psndSFXMain. Same layout as PaperSoundEffect; the effect
// record's byte fields are not individually declared in the base struct yet.
typedef struct PaperSoundEffectDetail {
    s32 unk0;              // 0x0
    u16 unk4;              // 0x4
    u16 unk6;              // 0x6
    u8 vol;                // 0x8
    u8 pan;                // 0x9
    u8 unkA;               // 0xA
    u8 padB[3];            // 0xB
    u16 unkE;              // 0xE
    u16 unk10;             // 0x10
    u16 unk12;             // 0x12
    s16 unk14;             // 0x14
    u8 pad16[2];           // 0x16
    s32 unk18;             // 0x18
    u8 pad1C[0x28 - 0x1C]; // 0x1C
} PaperSoundEffectDetail;

typedef struct PaperSoundEnv {
    u8 unk0[0x4 - 0x0];  // 0x0
    s16 unk4;            // 0x4
    u8 unk6[0x24 - 0x6]; // 0x6
} PaperSoundEnv;

typedef struct PaperSoundWork {
    s8 unk0;               // 0x0
    u8 unk1[0xC - 0x1];    // 0x1
    s8 unkC;               // 0xC
    u8 unkD[0x18 - 0xD];   // 0xD
    s16 unk18;             // 0x18
    s16 unk1A;             // 0x1A
    u8 unk1C[0x30 - 0x1C]; // 0x1C
    Vec position;          // 0x30
    f32 direction;         // 0x3C
    char unk40[0x10];      // 0x40
    s16 unk50;             // 0x50
    u8 unk52[0x54 - 0x52]; // 0x52
    s16 unk54;             // 0x54
    u16 unk56;             // 0x56, verified u16
    s8 unk58;              // 0x58
    s8 unk59;              // 0x59
    s8 unk5A;              // 0x5A
    u8 unk5B[0x5C - 0x5B]; // 0x5B
    s8 unk5C;              // 0x5C
    u8 unk5D[0x60 - 0x5D]; // 0x5D
} PaperSoundWork;

s32 searchPSSFXList(const char* name);
void psndInit(void);
void psndMapChange(void);
void psndMain(void);
void psndMainInt(void);
void psndExit(void);

BOOL psndPushGroup(void);

BOOL psndBGMOn(u32 r3, char* bgm_name);
void psndStopAllFadeOut(void);
BOOL psndENVOff(u32 r3);
void psndSFXAllOff(void);

s32 psndSFXOn(const char* name);
s32 psndSFXOn_(const char* name);
s32 psndSFXOn_3D(const char* name, Vec* position);
void psndSFXOnVol(s32 a, u8 b);

BOOL psndBGMOff_f_d(s32 flags, s32 fadetime, BOOL someswitch);
BOOL psndBGMStartCheck(s32 id);

void psndSetFlag(s32 flag);
void psndClearFlag(s32 flag);

void psndSetPosDirListener(Vec* position, f32 direction);

u16 psndGetFlag(void);
void psndSFXOff(s32 id);
BOOL psndBGMOff(s32 flags);
void psndSetReverb(s32 mode);
s32 psndSFXChk(u32 id);
void psndBGMScope(s32 id, s32 scope);
void psndBGM_rate(s32 id, u16 rate);
BOOL psndENVOn(s32 a, s32 b);
void psndBGMMain(void);
void psndENVMain(void);
void psndSFXMain(void);
void L_psndBGM_stop(void);
s32 psndENVOff_f_d(s32 id, s32 value, s32 flag);
s32 psndENVOn_f_d(s32 id, s32 a, s32 b, s32 c);
s32 psndBGMOn_f_d(s32 a, s32 name, s32 c, s32 d, s32 e);
void psndSFX_pos(s32 id, Vec* position);
s32 __psndSFXOn(s32, u8, u8, s32, Vec*, s32, s32, s32);
u32 calc3D(u32 flags, Vec* position, u8 volume, u16 span);



s32 psndENV_LPF(s32 id, u16 freq);
void psndSFX_dist(s32 id, u16 dist);
BOOL unk_800d7270(s32 a, char* b, s32 c, s32 d);
extern PaperSoundBGM psbgm[2];
extern SoundEnvList psenvlist[];
void psndENV_stop(u32 id);
s32 psndBGMPlayTime(s32 id);
BOOL psndBGMChk(s32 id);
BOOL psndBGMChkSilent(s32 id);
// 16-byte BGM table entry: two name/flag pairs (main BGM name and stream path).
typedef struct BGMListEntry {
    const char* field_0x0; // 0x0
    u32 field_0x4;         // 0x4
    const char* field_0x8; // 0x8
    u32 field_0xC;         // 0xC
} BGMListEntry;
