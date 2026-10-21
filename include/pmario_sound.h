#pragma once

#include <dolphin/mtx.h>

/* typedef union SoundData {
    const char* name;
    s32 value;
} SoundData; */

typedef struct SoundEnvList {
    u16 nameId;        // 0x0
    u16 flags;         // 0x2
    u8 volume;         // 0x4
    u8 field_0x5;      // 0x5
    u8 mode;           // 0x6
    u8 intervalRand;   // 0x7
    u8 interval;       // 0x8
    u32 posX;          // 0xC
    u32 posY;          // 0x10
    u32 posZ;          // 0x14
    u8 radius;         // 0x18
    u8 radiusRand;     // 0x19
    u8 span;           // 0x1A
    // pad 1 byte
} SoundEnvList;

typedef enum SoundEffectFlags {
    SFX_FLAG_2 = (1 << 29),
    SFX_FLAG_4 = (1 << 30),
    SFX_FLAG_8 = (1 << 31)
} SoundEffectFlags;

// most are translated in __psndSFXOn, TODO
typedef struct SoundEffectEntry {
    const char* name;       // 0x0, sfx name
    u32 flags;              // 0x4, lower 2 bytes are SND_FXID
    const char* streamName; // 0x8, .stm name
    u8 volume;              // 0xC
    u8 pan;                 // 0xD
    u8 field_0xE;           // 0xE
    u8 reverb;              // 0xF, reverb
    u8 field_0x10;          // 0x10
    u8 field_0x11;          // 0x11
    u16 pitch;              // 0x12
} SoundEffectEntry;

// A fade record: psnd holds one per env slot (unk0[]) and one for the SFX fade-out (field_0x24), and each BGM slot
// one (unk20). unk0 is the volume fade's state (1 in, 2 out then stop, 4 out and hold) over unk2 frames, counted by unk4;
// unk6 is the silent fade's state (8 down to unk7 percent, 0x10 back up) over unk8 frames, counted by unkA.
typedef struct PaperSoundEnvWork {
    u8 unk0;  // 0x0
    u8 unk1;  // 0x1
    s16 unk2; // 0x2
    s16 unk4; // 0x4
    u8 unk6;  // 0x6
    u8 unk7;  // 0x7
    s16 unk8; // 0x8
    s16 unkA; // 0xA
} PaperSoundEnvWork;

typedef struct PaperSoundBGM {
    s32 unk0;                // 0x0
    s32 streamId;            // 0x4, TODO verify re: seq_title
    u32 flags;               // 0x8
    u8 unkC;                 // 0xC
    u8 reverb;               // 0xD
    u8 volume;               // 0xE
    u8 pan;                  // 0xF
    u8 scopeTimer;           // 0x10, psndBGMScope: 0xFF in scope (level 100), 0 out (follows psnd.unk58), else frames left
    u8 unk11[0x20 - 0x11];   // 0x11
    PaperSoundEnvWork unk20; // 0x20
    u8 pad_0x2C[4];          // 0x2C
    s64 startTime;           // 0x30
} PaperSoundBGM;

typedef struct PaperSoundEffect {
    s32 unk0;              // 0x0
    u16 unk4;              // 0x4
    u16 unk6;              // 0x6
    u8 vol;                // 0x8, the caller's volume
    u8 baseVol;            // 0x9, SoundEffectEntry.volume; played volume is vol * baseVol / 127
    u8 pan;                // 0xA
    u8 reverb;             // 0xB
    u16 unkC;              // 0xC, SoundEffectEntry.field_0x10; compared when a slot is evicted
    u16 unkE;              // 0xE
    u16 unk10;             // 0x10
    u16 dist;              // 0x12, psndSFX_dist
    s16 pitch;             // 0x14, psndSFX_pit; SoundEfxSetPitch's pitch
    u16 unk16;             // 0x16
    s32 unk18;             // 0x18
    Vec position;          // 0x1C
} PaperSoundEffect;

typedef struct PaperSoundEnv {
    u32 unk0;            // 0x0, SoundEnvList.flags
    u16 unk4;            // 0x4, SoundEnvList.nameId, 0 when the slot is free
    u8 field_0x6;        // 0x6, SoundEnvList.volume
    u8 field_0x7;        // 0x7, SoundEnvList.field_0x5
    u8 field_0x8;        // 0x8, SoundEnvList.mode
    u8 field_0x9;        // 0x9, SoundEnvList.intervalRand
    u8 field_0xA;        // 0xA, SoundEnvList.interval
    u8 field_0xB;        // 0xB, SoundEnvList.radius
    u8 field_0xC;        // 0xC, SoundEnvList.radiusRand
    u8 field_0xD;        // 0xD, psndENVMain's step
    u16 field_0xE;       // 0xE, frames until the next play (interval * 60 + irand(intervalRand) * 60)
    u16 field_0x10;      // 0x10, SoundEnvList.span, passed to __psndSFXOn as its flags
    u8 pad_0x12[2];      // 0x12
    s32 field_0x14;      // 0x14, the playing __psndSFXOn id, -1 when none
    Vec field_0x18;      // 0x18, SoundEnvList.posX/Y/Z; the play position
} PaperSoundEnv;

typedef struct PaperSoundWork {
    PaperSoundEnvWork unk0[2];    // 0x0, per env slot
    u16 unk18[2];                 // 0x18, the env id playing in each slot, 0 when none
    u16 unk1C[2];                 // 0x1C, each slot's LPF, slewed towards unk20
    u16 unk20[2];                 // 0x20, each slot's target LPF (psndENV_LPF)
    PaperSoundEnvWork field_0x24; // 0x24, the SFX fade-out (psndSFXMain)
    Vec position;                 // 0x30
    f32 direction;                // 0x3C
    char unk40[0x10];             // 0x40
    u16 unk50;                    // 0x50
    u16 reverb;                   // 0x52, psndSetReverb; added to each effect's reverb
    u16 unk54;                    // 0x54
    u16 flag;                     // 0x56, psndSetFlag/psndClearFlag/psndGetFlag
    s8 unk58;                     // 0x58
    s8 unk59;                     // 0x59
    s8 unk5A;                     // 0x5A
    u8 unk5B;                     // 0x5B
    s8 unk5C;                     // 0x5C
    u8 unk5D[0x60 - 0x5D];        // 0x5D
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
u32 psndBGMStartCheck(s32 id);

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
BOOL psndBGMOn_f(s32 a, char* b, s32 c, s32 d);
extern PaperSoundBGM psbgm[2];
extern SoundEnvList psenvlist[];
void psndENV_stop(u32 id);
s32 psndBGMPlayTime(s32 id);
BOOL psndBGMChk(s32 id);
u32 psndBGMChkSilent(s32 id);
// 16-byte BGM table entry: two name/flag pairs (main BGM name and stream path).
typedef struct BGMListEntry {
    const char* name;       // 0x0
    u32 flags;              // 0x4
    const char* streamName; // 0x8
    u32 params;             // 0xC, packed volume/pan/reverb bytes
} BGMListEntry;
