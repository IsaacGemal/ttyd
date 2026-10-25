#pragma once

#include <dolphin/mtx.h>

/* typedef union SoundData {
    const char* name;
    s32 value;
} SoundData; */

typedef struct SoundEnvList {
    u16 nameId;        // 0x0
    u16 sfxNo;         // 0x2, pssfxlist index the env plays (psndENVMain passes it to __psndSFXOn)
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
    u8 field_0xE;           // 0xE, 1: __psndSFXOn sets the slot's 0x200 flag (not muted by psnd.flag 0x80)
    u8 reverb;              // 0xF, reverb
    u8 prio;                // 0x10, PaperSoundEffect.prio
    u8 delay;               // 0x11, frames added to the slot's start delay
    u16 pitch;              // 0x12
} SoundEffectEntry;

// A fade record: psnd holds one per env slot (envFade[]) and one for the SFX fade-out (sfxFade), and each BGM slot
// one (fade). The volume fade scales by fadeTimer / fadeFrames; the silent fade (psndBGMChkSilent, silent_tbl) by
// silentTimer / silentFrames.
typedef struct PaperSoundEnvWork {
    u8 fadeState;     // 0x0, 1 fading in, 2 fading out then stopping, 4 fading out and holding
    u8 pad_0x1;       // 0x1
    s16 fadeFrames;   // 0x2
    s16 fadeTimer;    // 0x4, counts up to fadeFrames fading in, down to 0 fading out
    u8 silentState;   // 0x6, 8 going silent, 0x10 coming back
    u8 silentLevel;   // 0x7, percent of silentFrames that silentTimer goes down to (silent_tbl)
    s16 silentFrames; // 0x8
    s16 silentTimer;  // 0xA
} PaperSoundEnvWork;

typedef struct PaperSoundBGM {
    u32 bgmNo;               // 0x0, psbgmlist index, -1 when the slot is free
    u32 streamId;            // 0x4, the SoundSS or SoundSong channel it plays on
    u32 flags;               // 0x8
    u8 field_0xC;            // 0xC
    u8 group;                // 0xD, BGMListEntry.group
    u8 volume;               // 0xE
    u8 pan;                  // 0xF
    u8 scopeTimer;           // 0x10, psndBGMScope: 0xFF in scope (level 100), 0 out (follows psnd.bgmLevel), else frames left
    u8 pad_0x11[0x20 - 0x11]; // 0x11
    PaperSoundEnvWork fade;  // 0x20
    u8 pad_0x2C[4];          // 0x2C
    s64 startTime;           // 0x30
} PaperSoundBGM;

typedef struct PaperSoundEffect {
    u32 sfxNo;             // 0x0, pssfxlist index (low 13 bits) and __psndSFXOn's flag bits, -1 when the slot is free
    u16 serial;            // 0x4, psnd.sfxSerial when started; psndSFXChk compares it with the id's bits 8-23
    u16 flags;             // 0x6, 1 waiting to start (psndSFXMain), 2 positioned (calc3D), 0x40 fading out
    u8 vol;                // 0x8, the caller's volume
    u8 baseVol;            // 0x9, SoundEffectEntry.volume; played volume is vol * baseVol / 127
    u8 pan;                // 0xA
    u8 reverb;             // 0xB
    u16 prio;              // 0xC, SoundEffectEntry.prio; __psndSFXOn takes over a slot whose prio is higher
    u16 delay;             // 0xE, frames psndSFXMain waits before starting the effect
    u16 attenuation;       // 0x10, taken off every volume psndSFXMain sets
    u16 dist;              // 0x12, psndSFX_dist
    s16 pitch;             // 0x14, psndSFX_pit; SoundEfxSetPitch's pitch
    u16 field_0x16;        // 0x16
    u32 channel;           // 0x18, the SoundSS, SoundSong or SoundEfx channel it plays on, -1 for none
    Vec position;          // 0x1C
} PaperSoundEffect;

typedef struct PaperSoundEnv {
    u32 sfxNo;           // 0x0, SoundEnvList.sfxNo
    u16 envId;           // 0x4, SoundEnvList.nameId, 0 when the slot is free
    u8 volume;           // 0x6, SoundEnvList.volume; psndENVGetVol scales it
    u8 field_0x7;        // 0x7, SoundEnvList.field_0x5
    u8 mode;             // 0x8, SoundEnvList.mode; psndENVMain's switch
    u8 intervalRand;     // 0x9, SoundEnvList.intervalRand
    u8 interval;         // 0xA, SoundEnvList.interval
    u8 radius;           // 0xB, SoundEnvList.radius; mode 0 plays at radius * 10 + irand(radiusRand * 10)
    u8 radiusRand;       // 0xC, SoundEnvList.radiusRand
    u8 step;             // 0xD, psndENVMain's step within the mode
    u16 timer;           // 0xE, frames until the next play (interval * 60 + irand(intervalRand) * 60)
    u16 span;            // 0x10, SoundEnvList.span, passed to __psndSFXOn as its dist
    u8 pad_0x12[2];      // 0x12
    u32 sfxId;           // 0x14, the playing __psndSFXOn id, -1 when none
    Vec position;        // 0x18, SoundEnvList.posX/Y/Z; the play position
} PaperSoundEnv;

typedef struct PaperSoundWork {
    PaperSoundEnvWork envFade[2]; // 0x0, per env slot
    u16 envId[2];                 // 0x18, the env id playing in each slot, 0 when none
    u16 envLpf[2];                // 0x1C, each slot's LPF, slewed towards envLpfTarget
    u16 envLpfTarget[2];          // 0x20, each slot's target LPF (psndENV_LPF)
    PaperSoundEnvWork sfxFade;    // 0x24, the SFX fade-out (psndSFXMain)
    Vec position;                 // 0x30
    f32 direction;                // 0x3C
    char mapName[0x10];           // 0x40, psndMapChange: the reverb is reset when the map's name changes
    u16 sfxSerial;                // 0x50, counts __psndSFXOn's effects; each id carries it in bits 8-23
    u16 reverb;                   // 0x52, psndSetReverb; added to each effect's reverb
    u16 repeatDelay;              // 0x54, __psndSFXOn: grows by 8 when an effect is queued again in the same frame, delaying the repeat; psndSFXMain clears it
    u16 flag;                     // 0x56, psndSetFlag/psndClearFlag/psndGetFlag
    s8 bgmLevel;                  // 0x58, percent applied to every BGM volume (psndBGMMain), eases towards 100 or 65
    s8 sfxLevel;                  // 0x59, percent applied to positioned effects' volumes (psndSFXMain)
    s8 envLevel;                  // 0x5A, percent applied to env volumes (psndENVGetVol)
    u8 field_0x5B;                // 0x5B
    s8 lockTimer;                 // 0x5C, __psndSFXOn: while nonzero five effect ids are refused (set to 6, counted down by psndMain)
    u8 pad_0x5D[0x60 - 0x5D];     // 0x5D
} PaperSoundWork;

s32 searchPSSFXList(const char* name);
void psndInit(void);
void psndMapChange(void);
void psndMain(void);
void psndMainInt(void);
void psndExit(void);

BOOL psndPushGroup(void);

BOOL psndBGMOn(u32 flags, char* bgm_name);
void psndStopAllFadeOut(void);
BOOL psndENVOff(u32 id);
void psndSFXAllOff(void);

s32 psndSFXOn(const char* name);
s32 psndSFXOn_(const char* name);
s32 psndSFXOn_3D(const char* name, Vec* position);
void psndSFXOnVol(s32 a, u8 b);

BOOL psndBGMOff_f_d(s32 flags, u32 fadetime, u32 someswitch);
u32 psndBGMStartCheck(s32 id);

void psndSetFlag(s32 flag);
void psndClearFlag(s32 flag);

void psndSetPosDirListener(Vec* position, f32 direction);

u16 psndGetFlag(void);
void psndSFXOff(u32 id);
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
s32 psndENVOff_f_d(s32 id, u32 value, s32 flag);
s32 psndENVOn_f_d(s32 id, s32 name, u32 value, s32 flag);
s32 psndBGMOn_f_d(u32 a, s32 name, u32 c, s32 d, s32 e);
void psndSFX_pos(u32 id, Vec* position);
s32 __psndSFXOn(s32 name, u8 vol, u8 pan, s32 delay, Vec* position, u32 a6, s32 dist, s32 a8);
u32 calc3D(u32 flags, Vec* position, u8 volume, u16 span);



s32 psndENV_LPF(s32 id, u16 freq);
void psndSFX_dist(u32 id, u16 dist);
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
    union {
        u32 params; // 0xC, packed volume/pan/group/scope bytes, as the table spells them
        struct {
            u8 volume;  // 0xC, PaperSoundBGM.volume
            u8 pan;     // 0xD, PaperSoundBGM.pan
            u8 group;   // 0xE, nonzero: psndBGMOn_f_d keeps a playing BGM of the same group instead of restarting it
            u8 inScope; // 0xF, 1: the BGM starts in scope (PaperSoundBGM.scopeTimer 0xFF)
        };
    };
} BGMListEntry;
