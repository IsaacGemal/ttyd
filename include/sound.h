#pragma once

/* SLIB File Structure:
 * chunks of data, first word is size (including these bytes), 0x20 "header"
 * "arrfile" pointer is size+0x20, to end of chunk
 */

#include "dolphin/types.h"
#include "manager/dvdmgr.h"
#include <dolphin/ai.h>
#include <dolphin/ar.h>
// #include <dolphin/arq.h>
#include <dolphin/dvd.h>

#include <musyx/musyx.h>

#define TWO_PI 6.28318530717958647692528676655900576839433879875021f

typedef void (*ReverbCallback)(u8 reason, SND_AUX_INFO* info, void* user);

typedef struct StreamHeader {
    u8 field_0x0;                      // 0x0
    u16 freq;                          // 0x2
    u32 count;                         // 0x4
    u32 field_0x8;                     // 0x8
    u32 field_0xC;                     // 0xC
    u32 field_0x10;                    // 0x10
    u32 field_0x14;                    // 0x14
    u32 field_0x18;                    // 0x18
    u32 field_0x1C;                    // 0x1C
    u16 field_0x20;                    // 0x20
    u16 field_0x22;                    // 0x22
    u16 field_0x24;                    // 0x24
    u16 field_0x26;                    // 0x26
    u8 pad_0x28[0x5C - 0x28];          // 0x28
    SND_ADPCMSTREAM_INFO field_0x5C;   // 0x5C, copied to entries[0].info
    u8 pad_0x7C[0xBC - 0x7C];          // 0x7C
    SND_ADPCMSTREAM_INFO field_0xBC;   // 0xBC, copied to entries[1].info
} StreamHeader;

typedef struct SoundGroup {  // TODO: rename
    u16 flags;               // 0x0
    u16 unk2;                // 0x2
    SND_GROUPID groupId;     // 0x4
    SND_SONGID songId;       // 0x6
    void* arrfile;           // 0x8
    struct SoundGroup* next; // 0xC
} SoundGroup;

typedef struct SoundSong {
    SND_SEQID seqId;   // 0x0
    u32 vol;           // 0x4, casts to u8
    SoundGroup* group; // 0x8
    u32 mask;          // 0xC
} SoundSong;

typedef struct SoundEffect {
    u16 flags;             // 0x0
    u16 unk2;              // 0x2, count or active or smth
    u16 vol;               // 0x4
    u16 pan;               // 0x6
    u16 span;              // 0x8
    u16 unkA;              // 0xA
    SND_VOICEID voiceId;   // 0xC
    u16 fid;               // 0x10
    u8 unk12[0x14 - 0x12]; // 0x12
    SND_EMITTER emitter;   // 0x14
    SND_FVECTOR pos;       // 0x64
    SND_FVECTOR prevPos;   // 0x70
    SND_FVECTOR dir;       // 0x7C
} SoundEffect;

typedef struct SoundDVD {
    u16 flags;              // 0x0
    u16 unk2;               // 0x2
    f32 unk4;               // 0x4
    f32 unk8;               // 0x8
    f32 unkC;               // 0xC
    u8 unk10[0x114 - 0x10]; // 0x10
    u32 samples;            // 0x114
    DVDFileInfo info;       // 0x118
} SoundDVD;

typedef struct SoundStreamEntry {
    DVDEntry* dvd;             // 0x0
    ARQRequest request;        // 0x4
    SND_STREAMID streamId;     // 0x24
    void* unk28;               // 0x28, TODO: re-type? buffer
    u8* loadBuffer[2];         // 0x2C, the two 0x8000-byte read buffers, selected by field_0x4C
    u32 bytesLoaded;           // 0x34, number of bytes already loaded from the file
    u32 field_0x38;            // 0x38
    u32 field_0x3C[2];         // 0x3C, bytes read into each loadBuffer
    u32 field_0x44;            // 0x44, read offset in the current loadBuffer
    u32 requestLength;         // 0x48, number of bytes to load next ARQPostRequest
    u16 field_0x4C;            // 0x4C, current loadBuffer index (0/1)
    s16 field_0x4E;            // 0x4E
    s16 field_0x50;            // 0x50
    s16 field_0x52;            // 0x52
    s16 field_0x54;            // 0x54
    s16 field_0x56;            // 0x56
    u16 field_0x58;            // 0x58
    u16 field_0x5A;            // 0x5A
    u16 field_0x5C;            // 0x5C
    u8 pad_0x5E[2];            // 0x5E
    u32 field_0x60;            // 0x60
    u32 field_0x64;            // 0x64
    SND_ADPCMSTREAM_INFO info; // 0x68
} SoundStreamEntry;

typedef struct SoundStream {
    u16 flags;                   // 0x0
    u16 unk2;                    // 0x2
    u16 unk4;                    // 0x4
    u16 pan;                     // 0x6
    u16 span;                    // 0x8
    u8 unkA[0xC - 0xA];          // 0xA
    f32 vol;                     // 0xC
    f32 unk10;                   // 0x10, some volume setting
    f32 unk14;                   // 0x14, some other volume setting
    u16 freq;                    // 0x18
    u16 count;                   // 0x1A
    u32 field_0x1C;              // 0x1C
    u32 field_0x20;              // 0x20
    u32 field_0x24;              // 0x24
    SoundStreamEntry entries[2]; // 0x28
} SoundStream;

typedef struct SoundTemp {
    void* callback; // 0x0
    void* data;     // 0x4
    u8 unk8;        // 0x8
    u8 pad9[3];     // 0x9
    s32 unkC;       // 0xC
} SoundTemp;

typedef struct SoundWork {
    void* poolData[8];         // 0x0
    void* projData[8];         // 0x20
    void* sdirData[8];         // 0x40
    void* slibData[8];         // 0x60
    void* stblData[8];         // 0x80
    void* etblData[8];         // 0xA0
    u32 groupCount[8];         // 0xC0
    u32 dataId;                // 0xE0, not incremented, memory leak if SoundLoadDVD2 called multiple times
    u32 slibId;                // 0xE4
    s32 unkE8;                 // 0xE8
    SoundGroup* groups;        // 0xEC
    SoundSong* songs;          // 0xF0
    SoundEffect* effects;      // 0xF4
    SoundDVD* dvds;            // 0xF8
    s32 unkFC;                 // 0xFC
    SoundStream* streams;      // 0x100
    s32 unk104;                // 0x104
    SoundTemp unk108[2];       // 0x108
    SND_LISTENER listener;     // 0x128
    SND_FVECTOR pos;           // 0x1B8
    SND_FVECTOR prevPos;       // 0x1C4
    SND_FVECTOR dir;           // 0x1D0
    SND_FVECTOR heading;       // 0x1DC
    SND_FVECTOR up;            // 0x1E8
    f32 unk1F4;                // 0x1F4
    u8 unk1F8[0x204 - 0x1F8];  // 0x1F8
    u32 listenerVol;           // 0x204
    u16 fadeIn;                // 0x208
    u16 fadeOut;               // 0x20A
    u16 unk20C;                // 0x20C
    u8 unk20E[0x210 - 0x20E];  // 0x20E
    BOOL interrupts;           // 0x210
    u16 unk214;                // 0x214, some "mute" thing
    u8 pad216[2];              // 0x216
    SND_OUTPUTMODE outputMode; // 0x218
    u32 unk21C[2];             // 0x21C, ARAM address of each stream cache slot (unk226 marks it in use)
    u16 unk224;                // 0x224
    u16 unk226[2];             // 0x226
    char filepath[0x100];      // 0x22A
    u8 pad32A[2];              // 0x32A
    void* filedata;            // 0x32C
    u16 unk330;                // 0x330
    u8 pad332[2];              // 0x332
    u8* dmaAddress;            // 0x334
    u32 dmaLength;             // 0x338
    s16 unk33C[0x140];         // 0x33C
    AIDCallback callback;      // 0x5BC
    s16 unk5C0[2][0x400];      // 0x5C0
    s32 unk15C0;               // 0x15C0
} SoundWork;

void SoundInit(void);
void SoundMain(void);
void SoundMainInt(void);
void SoundSetFadeTime(u16 in, u16 out);
BOOL SoundLoadDVD2(const char* path);
BOOL SoundLoadDVD2PushGroup(u8* group_ids);
void SoundLoadDVD2Free(void);
BOOL SoundSLibLoadDVD(const char* path);
BOOL SoundDropData(void);
void SoundSetOutputMode(SND_OUTPUTMODE mode);
void SoundOpenCover(void);
void SoundCloseCover(void);

void SoundSongPlayCh(s32 id, u32 mask);
void SoundSongContinueCh(s32 chan);
void SoundSongStopCh(s32 chan);
void SoundSongFadeoutCh(s32 chan);
void SoundSongFadeinCh(s32 chan);
void SoundSongSetVolCh(s32 chan, u8 vol);
u8 SoundSongGetVolCh(s32 chan);
u32 SoundSongCheck(s32 chan);

s32 SoundEfxPlayEx(SND_FXID fid, u8 a2, u8 vol, u8 pan);
void SoundEfxStop(s32 chan);
void SoundEfxSetPitch(s32 chan, s16 pitch);
void SoundEfxSetVolume(s32 chan, u8 vol);
u8 SoundEfxGetVolume(s32 chan);
void SoundEfxSetAux1(s32 chan, u8 reverb);
void SoundEfxSetPan(s32 chan, u8 pan);
void SoundEfxSetSrndPan(s32 chan, u8 span);
void SoundEfxSetLPF(s32 chan, s16 freq);
u32 SoundEfxCheck(s32 chan);

void SoundSSMain(void);
u32 SoundSSCheck(s32);
void SoundSSSetPlayFreqCh(s32 chan, u16 freq);
void SoundSSStopCh(s32);
void SoundSSMuteOff(s32 chan);
void SoundSSFadeoutCh(s32 chan);
void SoundSSSetVolCh(s32 chan, u8 vol);
s32 SoundSSGetVolCh(s32 chan);
void* sndMalloc(size_t size);
void sndFree(void* ptr);
void* loadDVD_callback(u32 offset, u32 length);
void SoundEfxMain(void);
void SoundDVDMain(void);
void SoundSSMainInt(void);
void SoundAIDMACallback(void);
u8* ssDecodeDPCM(SoundStream* stream, SoundStreamEntry* entry, s16* out, u32 count, u8* in);
extern SoundWork sound;
void SoundSSPlayChEx(s32 chan, u32 name, u32 useAram);
void SoundSSPlayChEx_main(s32 result, DVDFileInfo* fileInfo);
void SoundSSSetPanCh(s32 chan, u8 pan);
void SoundSSPlayCh(s32 chan, u32 name);
void SoundSSMuteOn(s32 chan);
void SoundSSContinueCh(s32 chan);
void SoundSSSetSrndPanCh(s32 chan, u8 span);
void _ssDVDReadAsync_activeChk(s32 result, DVDFileInfo* fileInfo);
void _ssDVDReadAsync_cache_aram(u32 pointerToARQRequest);
void _ssDVDReadAsync_cache_next(s32 result, DVDFileInfo* fileInfo);
void cache_flush(u32 pointerToARQRequest);
