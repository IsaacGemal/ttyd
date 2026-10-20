#include "sound.h"
#include <dolphin/os.h>
#include "manager/dvdmgr.h"
#include "memory.h"
#include "musyx/musyx.h"
#include "system.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

// TODO: use SoundInit to fill in SoundStream/Entry


//.bss
SND_AUX_REVERBHI revH;
SND_AUX_REVERBSTD revS;
SND_AUX_CHORUS cho;
SND_AUX_DELAY dly;
SoundWork sound;
static char str[0x100];

//.sbss
u32 aramMemArray[8];

s16 dtbl[16] = {1, 7, 24, 59, 83, 111, 236, 555, 742, 885, 1052, 2221, 3285, 4432, 6962, 9300};

// local prototypes



void* sndMalloc(size_t size) {
    return __memAlloc(HEAP_DEFAULT, size);
}

void sndFree(void* ptr) {
    __memFree(HEAP_DEFAULT, ptr);
}

void SoundInit(void) { // 1:1, assuming sndMalloc inlines
    SND_HOOKS hooks = (SND_HOOKS){ sndMalloc, sndFree };
    void* data;
    SoundStream* streams;
    SoundSong* songs;
    SoundDVD* dvds;
    SoundEffect* effects;
    int i;
    SoundStream* stream;
    SoundSong* song;
    SoundDVD* dvd;
    void* ptr;
    u32 size;
    SoundTemp* slot;

    ARInit(aramMemArray, 8);
    ARQInit();
    AIInit(0);
    ARAlloc(0xBFC000); // 12MB-16KB (reserved)
    sndSetHooks(&hooks);
    sndInit(0x40, 0xC, 0x2C, 1, 1, 0xBFC000);
    revH.tempDisableFX = 0;
    revH.time = 3.0f;
    revH.preDelay = 0.1f;
    revH.damping = 0.3f;
    revH.coloration = 0.0f;
    revH.crosstalk = 0.0f;
    revH.mix = 0.8f;
    sndAuxCallbackPrepareReverbHI(&revH);
    revS.tempDisableFX = 0;
    revS.time = 2.0f;
    revS.preDelay = 0.1f;
    revS.damping = 0.5f;
    revS.coloration = 0.5f;
    revS.mix = 0.5f;
    sndAuxCallbackPrepareReverbSTD(&revS);
    cho.baseDelay = 10;
    cho.variation = 0;
    cho.period = 500;
    sndAuxCallbackPrepareChorus(&cho);
    dly.delay[0] = 60;
    dly.delay[1] = 90;
    dly.delay[2] = 70;
    dly.feedback[0] = 20;
    dly.feedback[1] = 20;
    dly.feedback[2] = 20;
    dly.output[0] = 50;
    dly.output[1] = 50;
    dly.output[2] = 50;
    sndAuxCallbackPrepareDelay(&dly);
    sndSetAuxProcessingCallbacks(0, sndAuxCallbackReverbHI, &revH, 0xFF, 0, sndAuxCallbackChorus, &cho, 0xFF, 0);
    sndVolume(0x7F, 0, 0xFF);
    if (sound.outputMode != SND_OUTPUTMODE_STEREO) {
        sound.outputMode = SND_OUTPUTMODE_STEREO;
        sndOutputMode(SND_OUTPUTMODE_STEREO);
    }
    sound.callback = AIRegisterDMACallback(SoundAIDMACallback);
    sound.unk330 = 0;
    sound.unk15C0 = 0;
    sound.fadeIn = 10;
    sound.fadeOut = 500;
    sound.dataId = 0;
    sound.slibId = 0;
    sound.unk214 = 0;
    sound.unk224 = 0;
    sound.unkE8 = 0;

    data = sndMalloc(0x20000);
    sound.filedata = data;
    if (data == 0) {
        while (1)
            ;
    } // infinite loop

    sound.groups = 0;

    // force register
    song = (sound.songs = sndMalloc(sizeof(SoundSong) * 4));
    songs = song;
    if (songs == 0) {
        while (1)
            ;
    } // infinite loop

    for (i = 0; i < 4; i++, songs++) {
        songs->seqId = -1;
        songs->group = NULL;
    }

    effects = sndMalloc(sizeof(SoundEffect) * 40);
    sound.effects = effects;
    if (effects == 0) {
        while (1)
            ;
    } // infinite loop

    for (i = 0; i < 40; i++, effects++) {
        effects->flags = 0;
        effects->voiceId = -1;
        effects->unk2 = 0;
        effects->vol = 0x7F;
        effects->pan = 0x40;
    }

    // force register
    dvd = (sound.dvds = sndMalloc((sizeof(SoundDVD)) * 2));
    dvds = dvd;
    if (dvds == 0) {
        while (1)
            ;
    } // infinite loop

    for (i = 0; i < 2; i++, dvds++) {
        dvds->flags = 0;
    }

    streams = sndMalloc(sizeof(SoundStream) * 3);
    sound.streams = streams;
    if (streams == 0) {
        while (1)
            ;
    } // infinite loop

    stream = streams;
    for (i = 0; i < 3; i++, stream++) {
        stream->flags = 0;

        ptr = sndMalloc(0x8000);
        if (ptr == 0) {
            while (1)
                ;
        } // infinite loop
        stream->entries[0].loadBuffer = (u32)ptr;

        ptr = sndMalloc(0x8000);
        if (ptr == 0) {
            while (1)
                ;
        } // infinite loop
        stream->entries[0].unk30 = ptr;

        size = sndStreamAllocLength(0xE00, 0);
        ptr = sndMalloc(size);
        if (ptr == 0) {
            while (1)
                ;
        } // infinite loop
        stream->entries[0].unk28 = ptr;

        ptr = sndMalloc(0x8000);
        if (ptr == 0) {
            while (1)
                ;
        } // infinite loop
        stream->entries[1].loadBuffer = (u32)ptr;

        ptr = sndMalloc(0x8000);
        if (ptr == 0) {
            while (1)
                ;
        } // infinite loop
        stream->entries[1].unk30 = ptr;

        size = sndStreamAllocLength(0xE00, 0);
        ptr = sndMalloc(size);
        if (ptr == 0) {
            while (1)
                ;
        } // infinite loop
        stream->entries[1].unk28 = ptr;
    }

    slot = sound.unk108;
    slot->callback = sndAuxCallbackReverbHI;
    slot->data = &revH;
    slot->unk8 = 0xFF;
    slot->unkC = 0;

    slot++;
    slot->callback = sndAuxCallbackChorus;
    slot->data = &cho;
    slot->unk8 = 0xFF;
    slot->unkC = 0;
}

void SoundMain(void) {
    int i;
    SoundSong* song;

    for (i = 0, song = sound.songs; i < 4; i++, song++) {
        if (song->group && song->seqId != -1 && sndSeqGetValid(song->seqId) != TRUE) {
            song->group->flags |= 2;
            sndSeqStop(song->seqId);
            song->seqId = -1;
        }
    }
    SoundEfxMain();
    SoundDVDMain();
    SoundSSMain();
}

void SoundMainInt(void) {
    SoundSSMainInt();
}

void SoundSetFadeTime(u16 in, u16 out) { // 1:1
    sound.fadeIn = in;
    sound.fadeOut = out;
}

void* loadDVD_callback(u32 offset, u32 bytes) { // 1:1
    DVDFileInfo info;

    if (!DVDOpen(sound.filepath, &info)) {
        return NULL;
    }
    if (DVDReadPrio(&info, sound.filedata, (s32)bytes, (s32)offset, 2) <= 0) {
        return NULL;
    }
    DVDClose(&info);
    return sound.filedata;
}

BOOL SoundLoadDVD2(const char* path) { // 1:1
    DVDEntry* entry;
    u32 length;
    void* pointer;

    if (sound.dataId >= 8) {
        return 0;
    }

    sprintf(str, "%s/%s%s", getMarioStDvdRoot(), path, ".pool");
    entry = DVDMgrOpen(str, 2, 0);
    length = entry == NULL; // fixes regalloc
    if (entry == NULL) {
        pointer = NULL;
    } else {
        length = DVDMgrGetLength(entry);
        if (length == 0) {
            pointer = NULL;
        } else {
            length = OSRoundUp32B(length);
            pointer = __memAlloc(HEAP_DEFAULT, length);
            if (pointer == NULL) {
                pointer = NULL;
            } else if (DVDMgrRead(entry, pointer, length, 0) <= 0) {
                __memFree(HEAP_DEFAULT, pointer);
                pointer = NULL;
            } else {
                DVDMgrClose(entry);
            }
        }
    }
    if (pointer == NULL) {
        return FALSE;
    }
    sound.poolData[sound.dataId] = pointer;

    sprintf(str, "%s/%s%s", getMarioStDvdRoot(), path, ".proj");
    entry = DVDMgrOpen(str, 2, 0);
    if (entry == NULL) {
        pointer = NULL;
    } else {
        length = DVDMgrGetLength(entry);
        if (length == 0) {
            pointer = NULL;
        } else {
            length = OSRoundUp32B(length);
            pointer = __memAlloc(HEAP_DEFAULT, length);
            if (pointer == NULL) {
                pointer = NULL;
            } else if (DVDMgrRead(entry, pointer, length, 0) <= 0) {
                __memFree(HEAP_DEFAULT, pointer);
                pointer = NULL;
            } else {
                DVDMgrClose(entry);
            }
        }
    }
    if (pointer == NULL) {
        return FALSE;
    }
    sound.projData[sound.dataId] = pointer;

    sprintf(str, "%s/%s%s", getMarioStDvdRoot(), path, ".sdir");
    entry = DVDMgrOpen(str, 2, 0);
    if (entry == NULL) {
        pointer = NULL;
    } else {
        length = DVDMgrGetLength(entry);
        if (length == 0) {
            pointer = NULL;
        } else {
            length = OSRoundUp32B(length);
            pointer = __memAlloc(HEAP_DEFAULT, length);
            if (pointer == NULL) {
                pointer = NULL;
            } else if (DVDMgrRead(entry, pointer, length, 0) <= 0) {
                __memFree(HEAP_DEFAULT, pointer);
                pointer = NULL;
            } else {
                DVDMgrClose(entry);
            }
        }
    }
    if (pointer == NULL) {
        return FALSE;
    }
    sound.sdirData[sound.dataId] = pointer;

    sprintf(str, "%s/%s%s", getMarioStDvdRoot(), path, ".samp");
    strcpy(sound.filepath, str);
    sndSetSampleDataUploadCallback(loadDVD_callback, 0x20000);
    return TRUE;
}

BOOL SoundLoadDVD2PushGroup(u8* groupIds) { // 1:1
    SND_GROUPID id;
    s32 groupIndex;

    if (sound.dataId >= 8) {
        return 0;
    }
    sound.groupCount[sound.dataId] = 0;
    if (groupIds) {
        while (*groupIds != 0xFF) {
            id = *groupIds;
            sndPushGroup(sound.projData[sound.dataId], id, 0, sound.sdirData[sound.dataId],
                         sound.poolData[sound.dataId]);
            sound.groupCount[sound.dataId]++;
            groupIds++;
        };
    } else {
        groupIndex = sound.unkE8;
        while (groupIndex < 0xFF) {
            if (sndPushGroup(sound.projData[sound.dataId], (SND_GROUPID)groupIndex, 0, sound.sdirData[sound.dataId],
                             sound.poolData[sound.dataId]) != TRUE) {
                break;
            }
            sound.groupCount[sound.dataId]++;
            sound.unkE8++;
            groupIndex++;
        }
    }
    return TRUE;
}

void SoundLoadDVD2Free(void) {
    if (sound.filedata) {
        __memFree(HEAP_DEFAULT, sound.filedata);
        sound.filedata = NULL;
    }
}

BOOL SoundSLibLoadDVD(const char* path) { // 1:1
    // Regalloc: give each file read its own pointer/length local and declare the pointers before
    // entry; with a shared pointer/length, or with entry declared first, MWCC swaps r27/r28 here.
    void* slibData;
    void* stblData;
    void* etblData;
    DVDEntry* entry;
    SoundGroup* head;
    SoundGroup* iter;
    SoundGroup* group;
    u32* chunkPtr;
    u32 chunkSize;
    u16 id;
    u32 slibLength, stblLength, etblLength;
    void* arrfile;

    if (sound.slibId >= 8) {
        return FALSE;
    }

    sprintf(str, "%s/%s%s", getMarioStDvdRoot(), path, ".slib");
    entry = DVDMgrOpen(str, 2, 0);
    if (entry == NULL) {
        slibData = NULL;
    } else {
        slibLength = DVDMgrGetLength(entry);
        if (slibLength == 0) {
            slibData = NULL;
        } else {
            slibLength = OSRoundUp32B(slibLength);
            slibData = __memAlloc(HEAP_DEFAULT, slibLength);
            if (slibData == NULL) {
                slibData = NULL;
            } else if (DVDMgrRead(entry, slibData, slibLength, 0) <= 0) {
                __memFree(HEAP_DEFAULT, slibData);
                slibData = NULL;
            } else {
                DVDMgrClose(entry);
            }
        }
    }
    chunkPtr = slibData;
    if (slibData == NULL) {
        return FALSE;
    }
    sound.slibData[sound.slibId] = slibData;

    while (1) {
        chunkSize = *chunkPtr;
        if (*chunkPtr == 0xFFFFFFFF) {
            break;
        }
        group = __memAlloc(HEAP_DEFAULT, 0x10);
        if (group == NULL) {
            while (1)
                ;
        }
        head = sound.groups;
        id = 0;
        if (head == NULL) {
            sound.groups = group;
        } else {
            for (iter = sound.groups;; iter = iter->next) {
                id++;
                if (!iter->next) {
                    break;
                }
            }
            iter->next = group;
        }
        arrfile = chunkPtr + 8;
        group->flags = 1;
        group->unk2 = id;
        group->groupId = 0;
        group->songId = 0;
        group->arrfile = arrfile;
        group->next = 0;
        // Advances after the stores: MWCC then keeps the address in r0 and the field constants in r4.
        chunkPtr = (u32*)((u32)chunkPtr + (u32)chunkSize);
    }

    sprintf(str, "%s/%s%s", getMarioStDvdRoot(), path, ".stbl");
    entry = DVDMgrOpen(str, 2, 0);
    if (entry == NULL) {
        stblData = NULL;
    } else {
        stblLength = DVDMgrGetLength(entry);
        if (stblLength == 0) {
            stblData = NULL;
        } else {
            stblLength = OSRoundUp32B(stblLength);
            stblData = __memAlloc(HEAP_DEFAULT, stblLength);
            if (stblData == NULL) {
                stblData = NULL;
            } else if (DVDMgrRead(entry, stblData, stblLength, 0) <= 0) {
                __memFree(HEAP_DEFAULT, stblData);
                stblData = NULL;
            } else {
                DVDMgrClose(entry);
            }
        }
    }
    if (stblData == NULL) {
        return FALSE;
    }
    sound.stblData[sound.slibId] = stblData;

    sprintf(str, "%s/%s%s", getMarioStDvdRoot(), path, ".etbl");
    entry = DVDMgrOpen(str, 2, 0);
    if (entry == NULL) {
        etblData = NULL;
    } else {
        etblLength = DVDMgrGetLength(entry);
        if (etblLength == 0) {
            etblData = NULL;
        } else {
            etblLength = OSRoundUp32B(etblLength);
            etblData = __memAlloc(HEAP_DEFAULT, etblLength);
            if (etblData == NULL) {
                etblData = NULL;
            } else if (DVDMgrRead(entry, etblData, etblLength, 0) <= 0) {
                __memFree(HEAP_DEFAULT, etblData);
                etblData = NULL;
            } else {
                DVDMgrClose(entry);
            }
        }
    }
    if (etblData == NULL) {
        return FALSE;
    }
    sound.etblData[sound.slibId] = etblData;
    sound.slibId++;
    return TRUE;
}

static inline void _ssDropSeq(void) {
    SoundSong* song;
    SoundEffect* effect;
    SoundDVD* dvd;
    SoundStream* stream;
    u32 i;

    for (i = 0; i < 4; i++) {
        song = &sound.songs[i];
        if (sndSeqGetValid(song->seqId)) {
            sndSeqStop(song->seqId);
            song->seqId = (SND_SEQID)-1;
        }
    }
    for (i = 0; i < 40; i++) {
        effect = &sound.effects[i];
        if (effect->flags) {
            sndFXKeyOff(effect->voiceId);
            if (effect->flags & 0x10) {
                sndRemoveEmitter(&effect->emitter);
            }
            effect->flags = 0;
        }
    }
    for (i = 0; i < 2; i++) {
        dvd = &sound.dvds[i];
        if (dvd->flags & 3) {
            AISetStreamPlayState(0);
            dvd->flags = 0;
            sound.unkFC = -1;
            DVDCancelStream(&dvd->info.cb);
            DVDClose(&dvd->info);
        }
    }
    for (i = 0; i < 3; i++) {
        if (sound.streams[i].flags) {
            stream = &sound.streams[i];
            if (!sound.unk20C) {
                sound.interrupts = OSDisableInterrupts();
            }
            sound.unk20C++;
            if (stream->flags & 0x400) {
                sound.unk226[stream->unk2] = 0;
            }
            stream->flags = 0;
            DVDMgrClose(stream->entries[0].dvd);
            DVDMgrClose(stream->entries[1].dvd);
            sndStreamFree(stream->entries[0].streamId);
            if (stream->count == 2) {
                sndStreamFree(stream->entries[1].streamId);
            }
            if (sound.unk20C) {
                if (!--sound.unk20C) {
                    OSRestoreInterrupts(sound.interrupts);
                }
            }
            sound.unk104 = -1;
        }
    }
}

#ifdef MUST_MATCH
// MUST_MATCH: the original homes the second half's effects and dvd pointers in r26/r27. Taking the
// element address through a pointer-returning inline reproduces that; the plain `&sound.effects[i]`
// swaps the pointer and the strength-reduced offset registers (99.71%).
static inline SoundEffect* sdd_eff(u32 i) { return &sound.effects[i]; }
static inline SoundDVD* sdd_dvd(u32 i) { return &sound.dvds[i]; }
#else
#define sdd_eff(i) (&sound.effects[i])
#define sdd_dvd(i) (&sound.dvds[i])
#endif

BOOL SoundDropData(void) {
    SoundSong* song;
    SoundEffect* effect;
    SoundDVD* dvd;
    SoundStream* stream;
    SoundGroup *group, *nextGroup;
    u32 i;

    group = sound.groups;
    _ssDropSeq();
    sndSilence();

    for (i = 0; i < 4; i++) {
        song = &sound.songs[i];
        if (sndSeqGetValid(song->seqId)) {
            sndSeqStop(song->seqId);
            song->seqId = (SND_SEQID)-1;
        }
    }
    for (i = 0; i < 40; i++) {
        effect = sdd_eff(i);
        if (effect->flags) {
            sndFXKeyOff(effect->voiceId);
            if (effect->flags & 0x10) {
                sndRemoveEmitter(&effect->emitter);
            }
            effect->flags = 0;
        }
    }
    for (i = 0; i < 2; i++) {
        dvd = sdd_dvd(i);
        if (dvd->flags & 3) {
            AISetStreamPlayState(0);
            dvd->flags = 0;
            sound.unkFC = -1;
            DVDCancelStream(&dvd->info.cb);
            DVDClose(&dvd->info);
        }
    }
    for (i = 0; i < 3; i++) {
        if (sound.streams[i].flags) {
            stream = &sound.streams[i];
            if (!sound.unk20C) {
                sound.interrupts = OSDisableInterrupts();
            }
            sound.unk20C++;
            if (stream->flags & 0x400) {
                sound.unk226[stream->unk2] = 0;
            }
            stream->flags = 0;
            DVDMgrClose(stream->entries[0].dvd);
            DVDMgrClose(stream->entries[1].dvd);
            sndStreamFree(stream->entries[0].streamId);
            if (stream->count == 2) {
                sndStreamFree(stream->entries[1].streamId);
            }
            if (sound.unk20C) {
                if (!--sound.unk20C) {
                    OSRestoreInterrupts(sound.interrupts);
                }
            }
            sound.unk104 = -1;
        }
    }
    for (i = 0; i < sound.dataId; i++) {
        while (sound.groupCount[i]) {
            sndPopGroup();
            sound.groupCount[i]--;
        }
    }

    for (i = 0; i < sound.dataId; i++) {
        __memFree(HEAP_DEFAULT, sound.poolData[i]);
    }

    for (i = 0; i < sound.dataId; i++) {
        __memFree(HEAP_DEFAULT, sound.projData[i]);
    }

    for (i = 0; i < sound.dataId; i++) {
        __memFree(HEAP_DEFAULT, sound.sdirData[i]);
    }

    for (i = 0; i < sound.slibId; i++) {
        __memFree(HEAP_DEFAULT, sound.slibData[i]);
        __memFree(HEAP_DEFAULT, sound.stblData[i]);
        __memFree(HEAP_DEFAULT, sound.etblData[i]);
    }

    sound.dataId = 0;
    sound.slibId = 0;
    sound.unkE8 = 0;

    while (1) {
        if (!group) {
            break;
        }
        nextGroup = group->next;
        if (!(group->flags & 1)) {
            __memFree(HEAP_DEFAULT, group->arrfile);
        }
        __memFree(HEAP_DEFAULT, group);
        group = nextGroup;
    }
    sound.groups = 0;

    return TRUE;
}

void SoundSetOutputMode(SND_OUTPUTMODE mode) {
    if (sound.outputMode != mode) {
        sound.outputMode = mode;
        sndOutputMode(mode);
    }
}

void SoundOpenCover(void) {
    if (!sound.unk214) {
        sound.unk214 = 1;
    }
}

void SoundCloseCover(void) {
    SoundStream* stream;
    BOOL close;
    int i;

    if (sndIsInstalled() && sound.unk214 == 1) {
        close = FALSE;
        for (i = 0; i < 3; i++) {
            stream = &sound.streams[i];
            if (stream->flags & 8 && !(stream->flags & 0x400) && !(stream->flags & 2)) {
                if (stream->flags & 0x1000) {
                    close = TRUE;
                }
                if (stream->count == 2 && stream->flags & 0x2000) {
                    close = TRUE;
                }
            }
        }
        if (close) {
            sound.unk214 = 2;
        } else {
            sound.unk214 = 0;
        }
    }
}

inline SoundGroup* SoundSongGetGroup(u8 id) {
    u16 searchId = id;
    SoundGroup* group;

    for (group = sound.groups;; group = group->next) {
        if (group == 0) {
            return 0;
        }
        if (group->unk2 == searchId) {
            break;
        }
    }
    return group;
}

void SoundSongPlayCh(s32 chan, u32 songData) {
    SoundSong* song;
    SoundGroup* group;

    song = &sound.songs[chan];
    if (sndSeqGetValid(song->seqId) == 1) {
        sndSeqStop(song->seqId);
    }
    song->vol = 127;
    group = SoundSongGetGroup((u8)songData);
    if (group) {
        song->group = group;
        group->groupId = (SND_GROUPID)((songData >> 16) & 0xFF);
        group->songId = (SND_SONGID)((songData >> 8) & 0xFF);
        song->mask = 0xFFFFFFFF;
        song->seqId = sndSeqPlayEx(group->groupId, group->songId, group->arrfile, 0, 0);
        sndSeqVolume((u8)song->vol, 0, song->seqId, 0);
        sndSeqContinue(song->seqId);
        sndSeqMute(song->seqId, song->mask, 0);
    }
}

void SoundSongContinueCh(s32 chan) {
    SoundSong* song = &sound.songs[chan];

    if (sndSeqGetValid(song->seqId)) {
        sndSeqContinue(song->seqId);
        sndSeqVolume((u8)song->vol, 0, song->seqId, SND_SEQVOL_CONTINUE);
    }
}

void SoundSongStopCh(s32 chan) {
    SoundSong* song = &sound.songs[chan];

    if (sndSeqGetValid(song->seqId)) {
        sndSeqStop(song->seqId);
        song->seqId = (SND_SEQID)-1;
    }
}

void SoundSongFadeoutCh(s32 chan) {
    SoundSong* song = &sound.songs[chan];

    if (sndSeqGetValid(song->seqId)) {
        sndSeqVolume(0, sound.fadeOut, song->seqId, SND_SEQVOL_STOP);
        song->seqId = (SND_SEQID)-1;
    }
}

void SoundSongFadeinCh(s32 chan) {
    SoundSong* song = &sound.songs[chan];

    if (sndSeqGetValid(song->seqId)) {
        sndSeqVolume((u8)song->vol, sound.fadeIn, song->seqId, SND_SEQVOL_CONTINUE);
    }
}

void SoundSongSetVolCh(s32 chan, u8 vol) {
    SoundSong* song = &sound.songs[chan];

    if (sndSeqGetValid(song->seqId)) {
        if (vol > 127) {
            vol = 127;
        }
        song->vol = vol;
        sndSeqVolume((u8)song->vol, 0, song->seqId, SND_SEQVOL_CONTINUE);
    }
}

u8 SoundSongGetVolCh(s32 chan) {
    SoundSong* song = &sound.songs[chan];

    if (sndSeqGetValid(song->seqId) == 0) {
        return 0;
    }
    return (u8)song->vol;
}

BOOL SoundSongCheck(s32 chan) {
    return sndSeqGetValid(sound.songs[chan].seqId) != 0;
}

inline void SoundEfxMuteCh(s32 chan) {
    SoundEffect* sfx = &sound.effects[chan];
    if (sfx->flags) {
        sndFXKeyOff(sfx->voiceId);
        if (sfx->flags & 0x10) {
            sndRemoveEmitter(&sfx->emitter);
        }
        sfx->flags = 0;
    }
}

inline s32 SoundEfxStopAll(u8 a1) {
    SoundEffect* sfx = sound.effects;
    s32 chan = 0;

    for (chan = 0; chan < 40; chan++, sfx++) {
        if (sfx->unk2 <= a1) {
            SoundEfxMuteCh(chan);
            break;
        }
    }
    return chan;
}

inline u32 SoundEfxCheckPlay(s32 chan) {
    SoundEffect* sfx = &sound.effects[chan];
    SND_VOICEID vid;
    if (!sfx->flags) {
        return FALSE;
    }
    if (!(sfx->flags & 0x10)) {
        if (sfx->flags & 0x10) {
            vid = sndFXCheck(sndEmitterVoiceID(&sfx->emitter));
        } else {
            vid = sndFXCheck(sfx->voiceId);
        }
        if (sndFXCheck(vid) == -1) {
            return FALSE;
        }
    } else if (!sndCheckEmitter(&sfx->emitter)) {
        return FALSE;
    }
    return TRUE;
}

// 100%: the free-slot scan for the "stop all" fallback must be written inline (reusing chan/sfx) rather than
// calling inline SoundEfxStopAll; that is what lets MWCC coalesce them onto r29/r30.
s32 SoundEfxPlayEx(SND_FXID fid, u8 a2, u8 vol, u8 pan) {
    SoundEffect* sfx;
    s32 chan;

    sfx = sound.effects;
    for (chan = 0; chan < 40; chan++, sfx++) {
        if (!sfx->flags) {
            break;
        }
    }
    if (chan >= 40u) {
        sfx = sound.effects;
        for (chan = 0; chan < 40; chan++, sfx++) {
            if (sfx->unk2 <= a2) {
                SoundEfxMuteCh(chan);
                break;
            }
        }
        if (chan >= 40u) {
            return -1;
        }
    }
    sfx->flags = 0;
    if (!sound.unk20C) {
        sound.interrupts = OSDisableInterrupts();
    }
    ++sound.unk20C;
    sfx->voiceId = sndFXStart(fid, 127, pan);
    sndFXVolume(sfx->voiceId, vol);
    if (sound.unk20C && !--sound.unk20C) {
        OSRestoreInterrupts(sound.interrupts);
    }
    if (sfx->voiceId == -1) {
        return -1;
    }
    sfx->flags = 1;
    sfx->unk2 = a2;
    sfx->vol = vol;
    sfx->pan = pan;
    sfx->span = 0;
    sfx->fid = fid;
    if (sound.unk214) {
        sndFXVolume(sfx->voiceId, 0);
    }
    if (!SoundEfxCheckPlay(chan)) {
        chan = -1;
    }
    return chan;
}

void SoundEfxStop(s32 chan) {
    SoundEffect* sfx;

    sfx = &sound.effects[chan];
    if (sfx->flags) {
        sndFXKeyOff(sfx->voiceId);
        if (sfx->flags & 0x10) {
            sndRemoveEmitter(&sfx->emitter);
        }
        sfx->flags = 0;
    }
}

void SoundEfxSetPitch(s32 chan, u16 pitch) {
    SoundEffect* sfx;
    SND_VOICEID vid;

    sfx = &sound.effects[chan];
    if (sfx->flags & 0x10) {
        vid = sndFXCheck(sndEmitterVoiceID(&sfx->emitter));
    } else {
        vid = sndFXCheck(sfx->voiceId);
    }
    if (vid != -1) {
        sndFXPitchBend(vid, (u16)(pitch + 0x2000));
    }
}

void SoundEfxSetVolume(s32 chan, u8 vol) {
    SoundEffect* sfx;
    SND_VOICEID vid;

    sfx = &sound.effects[chan];
    if (sfx->flags & 0x10) {
        vid = sndFXCheck(sndEmitterVoiceID(&sfx->emitter));
    } else {
        vid = sndFXCheck(sfx->voiceId);
    }
    if (vid != -1) {
        sfx->vol = vol;
        sndFXVolume(vid, vol);
        if (sound.unk214) {
            sndFXVolume(vid, 0);
        }
    }
}

u8 SoundEfxGetVolume(s32 chan) {
    SoundEffect* effect;
    u32 idx = chan;
    effect = &sound.effects[idx];
    return (u8)effect->vol;
}

void SoundEfxSetAux1(s32 chan, u8 reverb) {
    SoundEffect* sfx;
    SND_VOICEID vid;

    sfx = &sound.effects[chan];
    if (sfx->flags & 0x10) {
        vid = sndFXCheck(sndEmitterVoiceID(&sfx->emitter));
    } else {
        vid = sndFXCheck(sfx->voiceId);
    }
    if (vid != -1) {
        sndFXReverb(vid, reverb);
    }
}

void SoundEfxSetPan(s32 chan, u8 pan) {
    SoundEffect* sfx;
    SND_VOICEID vid;
    u32 idx = chan;

    ((SoundEffect*)((u8*)sound.effects + idx * sizeof(SoundEffect)))->pan = pan;
    sfx = &sound.effects[idx];
    if (sfx->flags & 0x10) {
        vid = sndFXCheck(sndEmitterVoiceID(&sfx->emitter));
    } else {
        vid = sndFXCheck(sfx->voiceId);
    }
    if (vid != -1) {
        sndFXPanning(vid, pan);
    }
}

void SoundEfxSetSrndPan(s32 chan, u8 span) {
    SoundEffect* sfx;
    SND_VOICEID vid;
    u32 doppler;
    u32 idx = chan;

    ((SoundEffect*)((u8*)sound.effects + idx * sizeof(SoundEffect)))->span = span;
    sfx = &sound.effects[idx];
    if (sfx->flags & 0x10) {
        vid = sndFXCheck(sndEmitterVoiceID(&sfx->emitter));
    } else {
        vid = sndFXCheck(sfx->voiceId);
    }
    if (vid != -1) {
        sndFXSurroundPanning(vid, span);
        if (span == 0) {
            doppler = 0x2000;
        } else if (span < 64) {
            doppler = 0x2000 - 3 * (64 - span);
        } else {
            doppler = 0x2000 - 3 * (span - 64);
        }
        sndFXDoppler(vid, (u16)doppler);
    }
}

void SoundEfxSetLPF(s32 chan, s16 freq) {
    SoundEffect* sfx;
    u32 voiceId;

    sfx = &sound.effects[chan];
    if (sfx->flags & 0x10) {
        voiceId = sndFXCheck(sndEmitterVoiceID(&sfx->emitter));
    } else {
        voiceId = sndFXCheck(sfx->voiceId);
    }
    if (voiceId != -1) {
        if (freq == 0) {
            sndFXSetFilter(voiceId, SND_FILTER_NONE, 0);
        } else if (freq >= 16000) {
            sndFXSetFilter(voiceId, SND_FILTER_NONE, 0);
        } else {
            sndFXSetFilter(voiceId, SND_FILTER_LOWPASS, freq);
        }
    }
}

BOOL SoundEfxCheck(s32 chan) {
    SoundEffect* sfx;
    SND_VOICEID vid;

    sfx = &sound.effects[chan];
    if (!sfx->flags) {
        return FALSE;
    }
    if (!(sfx->flags & 0x10)) {
        if (sfx->flags & 0x10) {
            vid = sndFXCheck(sndEmitterVoiceID(&sfx->emitter));
        } else {
            vid = sndFXCheck(sfx->voiceId);
        }
        if (sndFXCheck(vid) == -1) {
            return FALSE;
        }
    } else if (!sndCheckEmitter(&sfx->emitter)) {
        return FALSE;
    }
    return TRUE;
}

inline void SoundEfxCheckCh(s32 chan) {
    SoundEffect* effect;
    SND_VOICEID vid;

    effect = &sound.effects[chan];
    if (effect->flags & 0x10) {
        vid = sndFXCheck(sndEmitterVoiceID(&effect->emitter));
    } else {
        vid = sndFXCheck(effect->voiceId);
    }
    if (sndFXCheck(vid) == -1) {
        SoundEfxMuteCh(chan);
    }
}

// `optimize_for_size on`: nudges the allocator into the original's loop-2 register assignment
// (channel index r29, effect pointer r30, i*0x88 byte offset r28); without it MWCC picks r28/r29/r30.
#pragma optimize_for_size on
void SoundEfxMain(void) {
    SoundEffect* effect;
    f32 x, y, z;
    f32 angle, headingZ, headingX;
    f32 one;
    int i;

    one = 1.0f;
    headingX = 0.0f;
    headingZ = 0.0f;
    angle = ((TWO_PI * sound.unk1F4) / 360.0f);
    sound.dir.x = sound.pos.x - sound.prevPos.x;
    sound.dir.y = sound.pos.y - sound.prevPos.y;
    sound.dir.z = sound.pos.z - sound.prevPos.z;
    sound.prevPos.x = sound.pos.x;
    sound.prevPos.y = sound.pos.y;
    sound.prevPos.z = sound.pos.z;
    headingX = ((one * (f32)sin(angle)) + headingX);
    headingZ = (headingZ - (one * (f32)cos(angle)));
    sound.heading.x = headingX;
    sound.heading.y = 0.0f;
    sound.heading.z = headingZ;
    sound.up.x = 0.0f;
    sound.up.y = 1.0f;
    sound.up.z = 0.0f;
    sndUpdateListener(&sound.listener, &sound.pos, &sound.dir, &sound.heading, &sound.up, (u8)sound.listenerVol, NULL);

    effect = sound.effects;
    for (i = 0; i < 40; i++, effect++) {
        if (effect->flags) {
            if (effect->flags & 0x10) {
                effect->dir.x = effect->pos.x - effect->prevPos.x;
                effect->dir.y = effect->pos.y - effect->prevPos.y;
                effect->dir.z = effect->pos.z - effect->prevPos.z;
                effect->prevPos.x = effect->pos.x;
                effect->prevPos.y = effect->pos.y;
                effect->prevPos.z = effect->pos.z;
                sndUpdateEmitter(&effect->emitter, &effect->pos, &effect->dir, (u8)effect->vol, NULL);
            }
        }
    }

    effect = sound.effects;
    for (i = 0; i < 40; i++, effect++) {
        if (effect->flags) {
            if (!(effect->flags & 0x10)) {
                SoundEfxCheckCh(i);
            } else {
                if (!sndCheckEmitter(&effect->emitter)) {
                    SoundEfxMuteCh(i);
                }
                if (effect->unkA) {
                    effect->unkA--;
                } else {
                    SoundEfxCheckCh(i);
                }
            }
        }
    }
}
#pragma optimize_for_size reset

inline void SoundDVDStopCh(s32 chan) {
    SoundDVD* dvd = &sound.dvds[chan];
    if (dvd->flags & 3) {
        AISetStreamPlayState(0);
        dvd->flags = 0;
        sound.unkFC = -1;
        DVDCancelStream(&dvd->info.cb);
        DVDClose(&dvd->info);
    }
}

inline void SoundDVDSetPanCh(s32 chan, u8 pan) { // I think this is the correct func name
    SoundDVD* dvd;
    f32 left, right;

    dvd = &sound.dvds[chan];
    dvd->unk2 = pan;
    left = dvd->unk8;
    right = left;
    if (pan < 64) {
        right = (((f32)pan * dvd->unk8) / 63.0f);
    }
    if (!((u32)(pan - 64) >> 31)) {
        left = (((f32)(127 - pan) * dvd->unk8) / 63.0f);
    }
    AISetStreamVolLeft((u8)left);
    AISetStreamVolRight((u8)right);
}

#ifdef MUST_MATCH
// MUST_MATCH: the original passes the channel to the inlined SoundDVD*Ch helpers as an unsigned
// value. Returning it through this u32 inline is a no-op in the code but makes MWCC home the
// helpers' strength-reduced `chan` induction in r27 and the loop counter in r29; passing the plain
// s32 `i` swaps those two (99.71%). Declaring `i` before `dvd` similarly homes the dvd pointer in r28.
static inline u32 SoundDVDChan(s32 chan) {
    return (u32)chan;
}
#else
#define SoundDVDChan(chan) (chan)
#endif

void SoundDVDMain(void) {
    int i;
    SoundDVD* dvd;

    for (i = 0; i < 2; i++) {
        dvd = &sound.dvds[i];
        if (!(dvd->flags & 2)) {
            if (dvd->flags & 0x40) {
                if (dvd->unk8 == 0.0f) {
                    dvd->samples = AIGetStreamSampleCount();
                    SoundDVDStopCh(SoundDVDChan(i));
                    dvd->flags |= 0x2;
                    dvd->flags &= ~0x40;
                    continue;
                }
                dvd->unk8 -= dvd->unkC;
                if (dvd->unk8 <= 0.0f) {
                    dvd->unk8 = 0.0f;
                }
                SoundDVDSetPanCh(SoundDVDChan(i), (u8)dvd->unk2);
            }
            if (dvd->flags & 0x20) {
                if (dvd->unk8 == 0.0f) {
                    dvd->flags &= ~0x20;
                    SoundDVDStopCh(SoundDVDChan(i));
                    continue;
                }
                dvd->unk8 -= dvd->unkC;
                if (dvd->unk8 <= 0.0f) {
                    dvd->unk8 = 0.0f;
                }
                SoundDVDSetPanCh(SoundDVDChan(i), (u8)dvd->unk2);
            }
            if (dvd->flags & 0x10) {
                dvd->unk8 += dvd->unkC;
                if (dvd->unk8 >= dvd->unk4) {
                    dvd->unk8 = dvd->unk4;
                    dvd->unkC = 0.0f;
                    dvd->flags &= ~0x10;
                }
                SoundDVDSetPanCh(SoundDVDChan(i), (u8)dvd->unk2);
            }
        }
    }
}

// Keep _ssDVDReadAsync_activeChk out of line: the original calls it rather than inlining.
#pragma dont_inline on
void _ssDVDReadAsync_activeChk(s32 result, DVDFileInfo* fileInfo) {
    SoundStream* stream;
    SoundStreamEntry* entry;
    u32* arr;
    s32 i;
    s32 j;

    if (!sound.unk20C) {
        sound.interrupts = OSDisableInterrupts();
    }
    sound.unk20C++;

    for (i = 0; i < 3; i++) {
        stream = &sound.streams[i];
        if (!stream->flags) {
            continue;
        }
        entry = NULL;
        j = 0;
        if (&stream->entries[0].dvd->info == fileInfo) {
            entry = &stream->entries[0];
            j = 0;
        }
        if (&stream->entries[1].dvd->info == fileInfo) {
            entry = &stream->entries[1];
            j = 1;
        }
        if (!entry) {
            continue;
        }
        if (result < 0) {
            goto unlock;
        }
        entry->bytesLoaded += entry->requestLength;
        // entry->unk38[4]: the pair of u32 buffer pointers toggled by the u16 index at entry->unk4C.
        arr = (u32*)&entry->unk38[4];
        arr[1 - *(u16*)entry->unk4C] = entry->requestLength;
        *(u16*)&entry->unk4C[8] = 1;
        if (entry->bytesLoaded >= *(u32*)&entry->unk4C[0x14]) {
            entry->bytesLoaded = *(u32*)&entry->unk4C[0x18];
            if (stream->flags & 0x200) {
                if (j == 0) {
                    stream->flags &= ~0x1000;
                } else {
                    stream->flags &= ~0x2000;
                }
            } else {
                stream->flags |= 0x4000;
            }
        } else if (j == 0) {
            stream->flags &= ~0x1000;
        } else {
            stream->flags &= ~0x2000;
        }
        if (stream->count == 2) {
            if (sound.unk214 == 2 && *(s16*)&stream->entries[0].unk4C[8] == 1 && *(s16*)&stream->entries[1].unk4C[8] == 1) {
                sound.unk214 = 0;
            }
        } else if (sound.unk214 == 2 && *(s16*)&stream->entries[0].unk4C[8] == 1) {
            sound.unk214 = 0;
        }
        break;
    }

    for (i = 0; i < 3; i++) {
        stream = &sound.streams[i];
        if (!stream->flags) {
            continue;
        }
        if (stream->flags & 8) {
            continue;
        }
        if (&stream->entries[0].dvd->info == fileInfo) {
            *(u16*)&stream->entries[0].unk4C[0xA] = 1;
        }
        if (&stream->entries[1].dvd->info == fileInfo) {
            *(u16*)&stream->entries[1].unk4C[0xA] = 1;
        }
        if (stream->count == 2) {
            if (*(s16*)&stream->entries[0].unk4C[0xA] == 0 || *(s16*)&stream->entries[1].unk4C[0xA] == 0) {
                continue;
            }
        } else if (*(s16*)&stream->entries[0].unk4C[0xA] == 0) {
            continue;
        }
        *(u16*)stream->entries[0].unk4C = 0;
        *(u16*)stream->entries[1].unk4C = 0;
        *(u16*)&stream->entries[0].unk4C[8] = 0;
        *(u16*)&stream->entries[1].unk4C[8] = 0;
        sndStreamActivate(stream->entries[0].streamId);
        if (stream->count == 2) {
            sndStreamActivate(stream->entries[1].streamId);
        }
        SoundSSSetPanCh(i, (u8)stream->pan);
        stream->flags |= 8;
        break;
    }
unlock:
    if (sound.unk20C && !--sound.unk20C) {
        OSRestoreInterrupts(sound.interrupts);
    }
}
#pragma dont_inline reset

void cache_flush(u32 pointerToARQRequest) {
    ARQRequest* request = (ARQRequest*)pointerToARQRequest;
    SoundStreamEntry* entry = (SoundStreamEntry*)request->owner;
    DVDEntry* dvd = entry->dvd;

    _ssDVDReadAsync_activeChk(0, &dvd->info);
}

static inline void _ssPlayChRead(SoundStream* stream, SoundStreamEntry* entry, u32 offset) {
    u32 len;
    u32 aligned;
    u32 n;
    SoundStreamEntry* e = entry;

    len = 0x8000;
    if (e->bytesLoaded + 0x8000 > *(u32*)&e->unk4C[0x14]) {
        len = *(u32*)&e->unk4C[0x14] - e->bytesLoaded;
    } else {
        for (n = 2; n < 6; n++) {
            if (e->bytesLoaded + n * 0x8000 <= *(u32*)&e->unk4C[0x14]) {
                continue;
            }
            len -= (0x8000 - (*(u32*)&e->unk4C[0x14] - (e->bytesLoaded + (n - 1) * 0x8000))) / n;
            len += 0x20 - (len & 0x1F);
            break;
        }
    }
    aligned = (len + 0x1F) & ~0x1F;
    if (!(stream->flags & 0x400)) {
        DVDMgrReadAsync(e->dvd, (void*)((&e->loadBuffer)[1 - *(u16*)e->unk4C]), aligned,
                        stream->unk4 + e->bytesLoaded + offset, _ssDVDReadAsync_activeChk);
    } else {
        u32 dest = ((u32*)sound.unk21C)[stream->unk2] + (offset + (stream->unk4 + (e->bytesLoaded)));
        DCInvalidateRange((void*)((&e->loadBuffer)[1 - *(u16*)e->unk4C]), 0x8000);
        ARQPostRequest(&e->request, (u32)e, 1, 0,
                       dest, (u32)((&e->loadBuffer)[1 - *(u16*)e->unk4C]), aligned, cache_flush);
    }
    e->requestLength = len;
}

void _ssDVDReadAsync_cache_aram(u32 pointerToARQRequest) {
    SoundStream* stream = &sound.streams[((ARQRequest*)pointerToARQRequest)->owner & 0xFF];
    SoundStreamEntry* entry;
    u32 length;
    u32 len;

    length = DVDMgrGetLength(stream->entries[0].dvd);
    if (*(u32*)stream->entries[0].unk38 >= length) {
        stream->entries[0].bytesLoaded = 0;
        *(u32*)stream->entries[0].unk38 = 0;
        stream->entries[1].bytesLoaded = 0;
        *(u32*)stream->entries[1].unk38 = 0;
        *(u16*)stream->entries[0].unk4C = 1;

        _ssPlayChRead(stream, &stream->entries[0], 0);

        if (stream->count == 2) {
            *(u16*)stream->entries[1].unk4C = 1;
            entry = &stream->entries[1];
            _ssPlayChRead(stream, entry, *(u32*)&entry->unk4C[0x14] + (0x20 - (*(u32*)&entry->unk4C[0x14] & 0x1F)));
        }
    } else {
        length = DVDMgrGetLength(stream->entries[0].dvd);
        if (stream->entries[0].bytesLoaded + 0x8000 >= length) {
            len = length - stream->entries[0].bytesLoaded;
        } else {
            len = 0x8000;
        }
        DVDMgrReadAsync(stream->entries[0].dvd, (void*)stream->entries[0].loadBuffer,
                        (len + 0x1F) & ~0x1F, stream->entries[0].bytesLoaded,
                        _ssDVDReadAsync_cache_next);
        stream->entries[0].requestLength = len;
    }
}

static u32 owner_no;

void _ssDVDReadAsync_cache_next(s32 result, DVDFileInfo* fileInfo) {
    SoundStream* stream;
    SoundStreamEntry* entry;
    u32 aramAddr;
    u32 owner;
    s32 i;

    for (i = 0; i < 3; i++) {
        stream = &sound.streams[i];
        if (stream->flags == 0) {
            continue;
        }
        if (&stream->entries[0].dvd->info != fileInfo) {
            continue;
        }
        if (result < 0) {
            break;
        }
        entry = &stream->entries[0];
        entry->bytesLoaded += entry->requestLength;
        aramAddr = ((u32*)entry->unk38)[0] + ((u32*)sound.unk21C)[stream->unk2];
        DCFlushRange((void*)entry->loadBuffer, 0x8000);
        owner = owner_no++;
        ARQPostRequest(&entry->request, (owner << 16) | i, 0, 0, entry->loadBuffer, aramAddr, 0x8000,
                       _ssDVDReadAsync_cache_aram);
        ((u32*)entry->unk38)[0] += entry->requestLength;
        break;
    }
}

u8* ssDecodeDPCM(SoundStream* stream, SoundStreamEntry* entry, s16* out, u32 count, u8* in) {
    u8* e = (u8*)entry;
    s8 delta;
    u8 byte;
    s32 index;

    while (count != 0) {
        byte = *in;
        if (*(u16*)(e + 0x5c) != 0) {
            if (*(u16*)(e + 0x5a) & 1) {
                delta = byte & 0xF;
            } else {
                delta = (byte >> 4) & 0xF;
            }
            index = *(u16*)(e + 0x58);
            delta <<= 4;
            delta >>= 4;
            *(u16*)(e + 0x5a) += 1;
            if (*(u16*)(e + 0x5a) >= *(u16*)(e + 0x5c)) {
                in++;
                *(u16*)(e + 0x5c) = 0;
                *(u32*)(e + 0x44) += 1;
            } else if ((*(u16*)(e + 0x5a) & 1) == 0) {
                in++;
                *(u32*)(e + 0x44) += 1;
            }
        } else {
            in++;
            *(u32*)(e + 0x44) += 1;
            index = byte & 0xF;
            if (index == 8) {
                *(u16*)(e + 0x5c) = ((byte >> 4) & 0xF) + 4;
                *(u16*)(e + 0x5a) = 0;
                continue;
            }
            delta = index;
            index = (byte >> 4) & 0xF;
            *(u16*)(e + 0x58) = index;
            delta <<= 4;
            delta >>= 4;
        }

        {
            s32 val = *(s16*)(e + 0x52) + delta * (s32)dtbl[index];
            if (val > 0x7fff) {
                val = 0x7fff;
            }
            if (val < -0x8000) {
                val = -0x8000;
            }
            *(s16*)(e + 0x52) = val;
            *out++ = val;
        }
        count--;
        *(u32*)&stream->unk1C[0x24 - 0x1C] += 1;

        if (*(u32*)(e + 0x44) >= *(u32*)(e + 0x3c + (*(u16*)(e + 0x4c) << 2))) {
            if (*(s16*)(e + 0x54) == 0 && !(stream->flags & 0x4000)) {
                *(u32*)(e + 0x38) -= *(u32*)(e + 0x44);
                *(u32*)(e + 0x44) = 0;
                in = (u8*)*(u32*)(e + 0x2c + (*(u16*)(e + 0x4c) << 2));
                continue;
            } else {
                *(s16*)(e + 0x54) = 0;
                *(u32*)(e + 0x44) = 0;
                *(u16*)(e + 0x4c) = 1 - *(u16*)(e + 0x4c);
                in = (u8*)*(u32*)(e + 0x2c + (*(u16*)(e + 0x4c) << 2));
            }
        }
        *(u32*)(e + 0x38) += 2;
        if (*(u32*)(e + 0x38) >= *(u32*)&stream->unk1C[0x1C - 0x1C]) {
            *(u32*)(e + 0x38) = *(u32*)&stream->unk1C[0x20 - 0x1C];
            *(s16*)(e + 0x52) = *(s16*)(e + 0x50);
            if (!(stream->flags & 0x200)) {
                stream->flags |= 4;
                break;
            }
        }
    }
    return in;
}

// SoundStream has no write-position field; it lives at +0x24 and is written raw. SoundStreamEntry at
// +0x2c is loadBuffer[?] and +0x38..0x60 is unk38/unk4C; all accessed raw here.
#pragma opt_propagation off // needed: with propagation on MWCC folds the /7 magic and homes n in r0
static inline u8* ssCopyPCM8(SoundStream* stream, u8* ep, u8* out, u32 count, u8* in) {
    while (count != 0) {
        *out++ = *in++;
        count--;
        *(u32*)(ep + 0x44) += 1;
        if (*(u32*)(ep + 0x44) >= *(u32*)(ep + 0x3c + ((*(u16*)(ep + 0x4c)) << 2))) {
            if (*(s16*)(ep + 0x54) == 0 && !(stream->flags & 0x4000)) {
                *(u32*)(ep + 0x38) -= *(u32*)(ep + 0x44);
                *(u32*)(ep + 0x44) = 0;
                in = (u8*)*(u32*)(ep + 0x2c + ((*(u16*)(ep + 0x4c)) << 2));
                continue;
            } else {
                *(s16*)(ep + 0x54) = 0;
                *(u32*)(ep + 0x44) = 0;
                *(u16*)(ep + 0x4c) = 1 - *(u16*)(ep + 0x4c);
                in = (u8*)*(u32*)(ep + 0x2c + ((*(u16*)(ep + 0x4c)) << 2));
            }
        }
        *(u32*)(ep + 0x38) += 1;
        if (*(u32*)(ep + 0x38) >= *(u32*)(stream->unk1C + 0)) {
            *(u32*)(ep + 0x38) = *(u32*)(stream->unk1C + 4);
            if (!(stream->flags & 0x200)) {
                stream->flags |= 4;
                break;
            }
        }
    }
    return in;
}

static inline u8* ssCopyPCM16(SoundStream* stream, u8* ep, s16* out, u32 count, u8* in) {
    while (count != 0) {
        *out++ = *(s16*)in;
        in += 2;
        count--;
        *(u32*)(stream->unk1C + 8) += 1;
        *(u32*)(ep + 0x44) += 2;
        if (*(u32*)(ep + 0x44) >= *(u32*)(ep + 0x3c + ((*(u16*)(ep + 0x4c)) << 2))) {
            if (*(s16*)(ep + 0x54) == 0 && !(stream->flags & 0x4000)) {
                *(u32*)(ep + 0x38) -= *(u32*)(ep + 0x44);
                *(u32*)(ep + 0x44) = 0;
                in = (u8*)*(u32*)(ep + 0x2c + ((*(u16*)(ep + 0x4c)) << 2));
                continue;
            } else {
                *(s16*)(ep + 0x54) = 0;
                *(u32*)(ep + 0x44) = 0;
                *(u16*)(ep + 0x4c) = 1 - *(u16*)(ep + 0x4c);
                in = (u8*)*(u32*)(ep + 0x2c + ((*(u16*)(ep + 0x4c)) << 2));
            }
        }
        *(u32*)(ep + 0x38) += 2;
        if (*(u32*)(ep + 0x38) >= *(u32*)(stream->unk1C + 0)) {
            *(u32*)(ep + 0x38) = *(u32*)(stream->unk1C + 4);
            if (!(stream->flags & 0x200)) {
                stream->flags |= 4;
                break;
            }
        }
    }
    return in;
}

u32 _sscallback(void* buffer1, u32 len1, void* buffer2, u32 len2, u32 user) {
    SoundStream* stream;
    s16* out1;
    u8* ep;
    u8* in;
    u32 size2;
    u32 i;
    s16* out2;

    stream = &sound.streams[user & 0xFF];
    size2 = len2;
    out1 = buffer1;
    out2 = buffer2;

    *(u32*)(stream->unk1C + 8) = 0;

    if ((stream->flags & 0x8006) || sound.unk214) {
        if (!(stream->flags & 0x100)) {
            for (i = 0; i != len1; i++) {
                *out1++ = 0;
                *(u32*)(stream->unk1C + 8) += 1;
            }
            for (i = 0; i != size2; i++) {
                *out2++ = 0;
                *(u32*)(stream->unk1C + 8) += 1;
            }
            return *(u32*)(stream->unk1C + 8);
        }
        {
            s32 n;
            s32 j;

            n = len1 / 7 * 4 & ~7;
            n /= 2;
            for (j = 0; j != n; j++) {
                *out1++ = 0;
            }
            n = size2 / 7 * 4 & ~7;
            n /= 2;
            for (j = 0; j != n; j++) {
                *out2++ = 0;
            }
        }
        return len1 + size2;
    }

    ep = (u8*)((user & 0x100) ? &stream->entries[1] : &stream->entries[0]);

    if (stream->flags & 0x80) {
        in = (u8*)((u32*)(ep + 0x2c))[*(u16*)(ep + 0x4c)];
        in += *(u32*)(ep + 0x44);
        in = ssDecodeDPCM(stream, (SoundStreamEntry*)ep, out1, len1, in);
        ssDecodeDPCM(stream, (SoundStreamEntry*)ep, out2, size2, in);
    } else if (stream->flags & 0x100) {
        in = (u8*)((u32*)(ep + 0x2c))[*(u16*)(ep + 0x4c)];
        in += *(u32*)(ep + 0x44);
        in = ssCopyPCM8(stream, ep, (u8*)out1, (len1 + 13) / 7 * 4 & ~7, in);
        ssCopyPCM8(stream, ep, (u8*)out2, (size2 + 13) / 7 * 4 & ~7, in);
        *(u32*)(stream->unk1C + 8) = len1 + size2;
    } else {
        in = (u8*)((u32*)(ep + 0x2c))[*(u16*)(ep + 0x4c)];
        in += *(u32*)(ep + 0x44) & ~1;
        in = ssCopyPCM16(stream, ep, out1, len1, in);
        ssCopyPCM16(stream, ep, out2, size2, in);
    }
    return *(u32*)(stream->unk1C + 8);
}
#pragma opt_propagation reset

void SoundSSPlayCh(s32 chan, u32 name) {
    SoundSSPlayChEx(chan, name, 0);
}

// Returns void: the original never writes r3, and an s32 definition (or header prototype!) shifts the tail to r4.
#ifdef MUST_MATCH
// Forces MWCC to materialise str's address into path's home register (r27) before the copy to r3;
// writing `path = str;` directly coalesces the address into r3 and copies the other way.
static inline char* SoundSSPlayChEx_getPath(void) {
    char* path = str;
    return path;
}
#endif
void SoundSSPlayChEx(s32 chan, u32 name, u32 useAram) {
    SoundStream* stream = &sound.streams[chan];
    u8* e = (u8*)stream;
    char* path;
    int i;

    if (stream->flags) {
        SoundSSStopCh(chan);
    }

    if ((s32)useAram != 0) {
        for (i = 0; i < sound.unk224; i++) {
            if (sound.unk226[i] == 0) {
                sound.unk226[i] = 1;
                stream->unk2 = i;
                break;
            }
        }
        if (i >= sound.unk224) {
            return;
        }
    }

    sound.unk104 = chan;
    *(u16*)(e + 0x74) = 0;
    *(u32*)(e + 0x6c) = 0;
    *(u32*)(e + 0x5c) = 0;
    *(u32*)(e + 0x60) = 0;
    *(u16*)(e + 0x7c) = 0;
    *(u16*)(e + 0x7e) = 0;
    *(u16*)(e + 0x80) = 0;
    *(u16*)(e + 0x82) = 0;
    *(u16*)(e + 0x84) = 0;
    *(u16*)(e + 0xfc) = 0;
    *(u32*)(e + 0xf4) = 0;
    *(u32*)(e + 0xe4) = 0;
    *(u32*)(e + 0xe8) = 0;
    *(u16*)(e + 0x104) = 0;
    *(u16*)(e + 0x106) = 0;
    *(u16*)(e + 0x108) = 0;
    *(u16*)(e + 0x10a) = 0;
    *(u16*)(e + 0x10c) = 0;
    stream->pan = 0x40;
    stream->span = 0;
    stream->unk10 = 0.0f;
    stream->vol = 127.0f;
    stream->flags = 1;
    if ((s32)useAram != 0) {
        stream->flags |= 0x400;
    }

    sprintf(str, "%s/%s%s", getMarioStDvdRoot(), name, ".stm");
#ifdef MUST_MATCH
    path = SoundSSPlayChEx_getPath();
#else
    path = str;
#endif
    if ((stream->entries[0].dvd = DVDMgrOpen(path, 0, 0)) == 0) {
        stream->flags = 0;
        return;
    }
    if ((stream->entries[1].dvd = DVDMgrOpen(path, 0, 0)) == 0) {
        stream->flags = 0;
        return;
    }
    DVDMgrReadAsync(stream->entries[0].dvd, stream->entries[0].unk28, 0x100, 0, SoundSSPlayChEx_main);
}

void SoundSSPlayChEx_main(s32 result, DVDFileInfo* fileInfo) {
    SoundStream* stream;
    s32 chan;
    u8 type;
    u32 size;
    StreamHeader* hdr;
    u8 pan;
    u8* p;
    u8* dst;
    u8* src;
    u8* dst2;
    u8* src2;
    u32 j;
    SoundStreamEntry* entry1;

    if (result < 0) {
        return;
    }

    for (chan = 0; chan < 3; chan++) {
        stream = &sound.streams[chan];
        if (stream->flags && (DVDFileInfo*)&stream->entries[0].dvd->info == fileInfo) {
            break;
        }
    }

    hdr = (StreamHeader*)stream->entries[0].unk28;
    type = hdr->field_0x0;
    stream->freq = hdr->freq;
    stream->count = hdr->count;
    *(u32*)&stream->unk1C[0] = hdr->field_0x8;
    *(u32*)&stream->unk1C[4] = hdr->field_0xC;
    *(u32*)&stream->entries[0].unk4C[0x14] = hdr->field_0x10;
    *(u32*)&stream->entries[1].unk4C[0x14] = hdr->field_0x14;
    *(u32*)&stream->entries[0].unk4C[0x18] = hdr->field_0x18;
    *(u32*)&stream->entries[1].unk4C[0x18] = hdr->field_0x1C;
    *(u16*)&stream->entries[0].unk4C[2] = hdr->field_0x20;
    *(u16*)&stream->entries[1].unk4C[2] = hdr->field_0x22;
    *(u16*)&stream->entries[0].unk4C[4] = hdr->field_0x24;
    *(u16*)&stream->entries[1].unk4C[4] = hdr->field_0x26;

    if (*(u32*)&stream->unk1C[4] != 0xFFFFFFFF) {
        stream->flags |= 0x200;
    } else {
        *(u32*)&stream->unk1C[4] = 0;
    }

    switch (type) {
    case 0:
        *(u32*)&stream->entries[0].unk4C[0x14] = *(u32*)&stream->unk1C[0];
        *(u32*)&stream->entries[1].unk4C[0x14] = *(u32*)&stream->unk1C[0];
        *(u32*)&stream->entries[0].unk4C[0x18] = *(u32*)&stream->unk1C[4];
        *(u32*)&stream->entries[1].unk4C[0x18] = *(u32*)&stream->unk1C[4];
        stream->unk4 = 0x40;
        break;
    case 1:
        stream->flags |= 0x80;
        *(s16*)&stream->entries[0].unk4C[6] = *(s16*)&stream->entries[0].unk4C[2];
        *(s16*)&stream->entries[1].unk4C[6] = *(s16*)&stream->entries[1].unk4C[2];
        stream->unk4 = 0x40;
        break;
    case 2:
        src = (u8*)hdr + 0x5C;
        dst = (u8*)&stream->entries[0].info;
        stream->flags |= 0x100;
        for (j = 0x20; j != 0; j--) {
            *dst++ = *src++;
        }
        if (stream->count == 2) {
            src2 = (u8*)hdr + 0xBC;
            dst2 = (u8*)&stream->entries[1].info;
            for (j = 0x20; j != 0; j--) {
                *dst2++ = *src2++;
            }
        }
        stream->unk4 = 0x100;
        break;
    }

    *(u16*)&stream->entries[0].unk4C[0] = 0;
    *(u32*)&stream->entries[0].unk38[0xC] = 0;
    stream->entries[0].bytesLoaded = 0;
    *(u32*)&stream->entries[0].unk38[0] = 0;
    *(u16*)&stream->entries[0].unk4C[8] = 0;
    *(u16*)&stream->entries[0].unk4C[0xA] = 0;
    *(u16*)&stream->entries[0].unk4C[0xC] = 0;
    *(u16*)&stream->entries[0].unk4C[0xE] = 0;
    *(u16*)&stream->entries[0].unk4C[0x10] = 0;
    *(u16*)&stream->entries[1].unk4C[0] = 0;
    *(u32*)&stream->entries[1].unk38[0xC] = 0;
    stream->entries[1].bytesLoaded = 0;
    *(u32*)&stream->entries[1].unk38[0] = 0;
    *(u16*)&stream->entries[1].unk4C[8] = 0;
    *(u16*)&stream->entries[1].unk4C[0xA] = 0;
    *(u16*)&stream->entries[1].unk4C[0xC] = 0;
    *(u16*)&stream->entries[1].unk4C[0xE] = 0;
    *(u16*)&stream->entries[1].unk4C[0x10] = 0;

    if (!(stream->flags & 0x400)) {
        *(u16*)&stream->entries[0].unk4C[0] = 1;
        _ssPlayChRead(stream, &stream->entries[0], 0);
        stream->flags |= 0x1000;
        if (stream->count == 2) {
            *(u16*)&stream->entries[1].unk4C[0] = 1;
            entry1 = &stream->entries[1];
            _ssPlayChRead(
                stream, entry1,
                *(u32*)&entry1->unk4C[0x14] + (0x20 - (*(u32*)&entry1->unk4C[0x14] & 0x1F)));
            stream->flags |= 0x2000;
        }
    }

    size = sndStreamAllocLength(0xE00, 0);
    p = stream->entries[0].unk28;
    for (j = size; j != 0; j--) {
        *p++ = 0;
    }
    if (stream->count == 2) {
        p = stream->entries[1].unk28;
        for (j = size; j != 0; j--) {
            *p++ = 0;
        }
    }

    pan = 0;
    if (stream->count == 1) {
        pan = 0x40;
    }
    if (!(stream->flags & 0x100)) {
        stream->entries[0].streamId =
            sndStreamAllocEx(0xFF, stream->entries[0].unk28, 0xE00, stream->freq, 0, pan, 0, 0, 0, 0,
                             0x10000, _sscallback, chan, NULL);
        if (stream->count == 2) {
            stream->entries[1].streamId = sndStreamAllocEx(
                0xFF, stream->entries[1].unk28, 0xE00, stream->freq, 0, 0x7F, 0, 0, 0, 0, 0x10000,
                _sscallback, chan | 0x100, NULL);
        }
        stream->flags |= 8;
    } else {
        stream->entries[0].streamId = sndStreamAllocEx(
            0xFF, stream->entries[0].unk28, 0xE00, stream->freq, 0, pan, 0, 0, 0, 0, 0x10001,
            _sscallback, chan, &stream->entries[0].info);
        if (stream->count == 2) {
            stream->entries[1].streamId = sndStreamAllocEx(
                0xFF, stream->entries[1].unk28, 0xE00, stream->freq, 0, 0x7F, 0, 0, 0, 0, 0x10001,
                _sscallback, chan | 0x100, &stream->entries[1].info);
        }
    }

    stream->unk14 = stream->vol;
    stream->flags |= 0x10;
    SoundSSSetPanCh(chan, (u8)stream->pan);

    if (stream->flags & 0x400) {
        u32 len;

        len = DVDMgrGetLength(stream->entries[0].dvd);
        if (stream->entries[0].bytesLoaded + 0x8000 >= len) {
            len = len - stream->entries[0].bytesLoaded;
        } else {
            len = 0x8000;
        }
        DVDMgrReadAsync(stream->entries[0].dvd, (void*)stream->entries[0].loadBuffer,
                        (len + 0x1F) & ~0x1F, stream->entries[0].bytesLoaded,
                        _ssDVDReadAsync_cache_next);
        stream->entries[0].requestLength = len;
    }
}

// Stop channel `chan`, leaving `stream` pointing at it. A macro so SoundSSMain can expand it (and
// SOUND_SS_SET_PAN_CH) with one pointer variable reused at every site, as the original's registers show.
#define SOUND_SS_STOP_CH(stream, chan)                        \
    do {                                                      \
        (stream) = &sound.streams[chan];                      \
        if ((stream)->flags) {                                \
            if (!sound.unk20C) {                              \
                sound.interrupts = OSDisableInterrupts();     \
            }                                                 \
            sound.unk20C++;                                   \
            if ((stream)->flags & 0x400) {                    \
                sound.unk226[(stream)->unk2] = 0;             \
            }                                                 \
            (stream)->flags = 0;                              \
            DVDMgrClose((stream)->entries[0].dvd);            \
            DVDMgrClose((stream)->entries[1].dvd);            \
            sndStreamFree((stream)->entries[0].streamId);     \
            if ((stream)->count == 2) {                       \
                sndStreamFree((stream)->entries[1].streamId); \
            }                                                 \
            if (sound.unk20C && !--sound.unk20C) {            \
                OSRestoreInterrupts(sound.interrupts);        \
            }                                                 \
            sound.unk104 = -1;                                \
        }                                                     \
    } while (0)

void SoundSSStopCh(s32 chan) {
    SoundStream* stream;

    SOUND_SS_STOP_CH(stream, chan);
}

void SoundSSFadeoutCh(s32 chan) {
    SoundStream* stream = &sound.streams[chan];
    f32 fade;

    if (stream->flags) {
        fade = (f32)sound.fadeOut / 16.666666f;
        if (fade < 1.0f) {
            fade = 1.0f;
        }
        stream->unk14 = stream->unk10 / fade;
        stream->flags |= 0x20;
    }
}

void SoundSSContinueCh(s32 chan) {
    SoundStream* stream = &sound.streams[chan];

    if (stream->flags) {
        stream->flags &= ~2;
        stream->unk14 = stream->vol;
        stream->flags |= 0x10;
    }
}

// Set channel `chan`'s pan (a plain u8 variable), leaving `stream` pointing at it (see SOUND_SS_STOP_CH).
#define SOUND_SS_SET_PAN_CH(stream, chan, pan)                                                                \
    do {                                                                                                      \
        (stream) = &sound.streams[chan];                                                                      \
        if ((stream)->flags) {                                                                                \
            (stream)->pan = pan;                                                                              \
            if ((stream)->flags & 8) {                                                                        \
                if ((stream)->count == 1) {                                                                   \
                    sndStreamMixParameter((stream)->entries[0].streamId, (stream)->unk10, pan,                \
                                          (u8)(stream)->span, 0);                                             \
                } else {                                                                                      \
                    f32 left = (stream)->unk10;                                                               \
                    f32 right = left;                                                                         \
                                                                                                              \
                    if (pan < 64) {                                                                           \
                        right = ((f32)pan * left) / 63.0f;                                                    \
                    }                                                                                         \
                    if (!((u32)(pan - 64) >> 31)) {                                                           \
                        left = ((f32)(127 - pan) * left) / 63.0f;                                             \
                    }                                                                                         \
                    sndStreamMixParameter((stream)->entries[0].streamId, left, 0, (u8)(stream)->span, 0);     \
                    sndStreamMixParameter((stream)->entries[1].streamId, right, 0x7F, (u8)(stream)->span, 0); \
                }                                                                                             \
            }                                                                                                 \
        }                                                                                                     \
    } while (0)

void SoundSSSetPanCh(s32 chan, u8 pan) {
    SoundStream* stream;

    SOUND_SS_SET_PAN_CH(stream, chan, pan);
}

// The index helper is an inline so the stream pointer's live range matches the original's
// register allocation (without it MWCC colors the index/pointer as r6/r7 instead of r7/r6).
static inline SoundStream* SoundSSGetStream(s32 chan) {
    return &sound.streams[chan];
}

void SoundSSSetVolCh(s32 chan, u8 vol) {
    SoundStream* stream = SoundSSGetStream(chan);
    f32 value;

    if (!stream->flags) {
        return;
    }
    value = (f32)vol;
    stream->unk10 = value;
    stream->vol = value;
    if (!(stream->flags & 8)) {
        return;
    }
    SoundSSSetPanCh(chan, (u8)stream->pan);
}

s32 SoundSSGetVolCh(s32 chan) {
    SoundStream* stream;

    stream = &sound.streams[chan];
    if (stream->flags) {
        return (s32)stream->vol;
    } else {
        return 0;
    }
}

void SoundSSSetSrndPanCh(s32 chan, u8 span) {
    SoundStream* stream = &sound.streams[chan];

    if (!stream->flags) {
        return;
    }
    stream->span = span;
    if (!(stream->flags & 8)) {
        return;
    }
    SoundSSSetPanCh(chan, (u8)stream->pan);
}

void SoundSSSetPlayFreqCh(s32 chan, u16 freq) {
    SoundStream* stream = &sound.streams[chan];

    if (!stream->flags) {
        return;
    }
    stream->freq = freq;
    if (!(stream->flags & 8)) {
        return;
    }
    if (!sound.unk20C) {
        sound.interrupts = OSDisableInterrupts();
    }
    sound.unk20C++;
    sndStreamFrq(stream->entries[0].streamId, stream->freq);
    if (stream->count == 2) {
        sndStreamFrq(stream->entries[1].streamId, stream->freq);
    }
    if (sound.unk20C && !--sound.unk20C) {
        OSRestoreInterrupts(sound.interrupts);
    }
}

s32 SoundSSCheck(s32 chan) {
    return sound.streams[chan].flags;
}

void SoundSSMuteOn(s32 chan) {
    SoundStream* stream;
    u32 idx = chan;
    stream = &sound.streams[idx];
    if (stream->flags) {
        stream->flags |= 2;
    }
}

void SoundSSMuteOff(s32 chan) {
    SoundStream* stream;
    u32 idx = chan;
    stream = &sound.streams[idx];
    if (stream->flags) {
        stream->flags &= ~2;
    }
}

// SoundDVDChan: as in SoundDVDMain, the channel reaches the helpers as an unsigned value.
void SoundSSMain(void) {
    SoundStream* ch;
    int i;
    u8 pan;

    for (i = 0; i < 3; i++) {
        SoundStream* stream = &sound.streams[i];
        if (!(stream->flags & 8)) {
            continue;
        }
        if (stream->flags & 2) {
            continue;
        }
        if (stream->flags & 4) {
            stream->flags &= ~4;
            SOUND_SS_STOP_CH(ch, SoundDVDChan(i));
            continue;
        }
        if (stream->flags & 0x40) {
            if (stream->unk10 == 0.0f) {
                stream->flags |= 2;
                stream->flags &= ~0x40;
                continue;
            }
            stream->unk10 -= stream->unk14;
            if (stream->unk10 <= 0.0f) {
                stream->unk10 = 0.0f;
            }
            pan = (u8)stream->pan;
            SOUND_SS_SET_PAN_CH(ch, SoundDVDChan(i), pan);
        }
        if (stream->flags & 0x20) {
            if (stream->unk10 == 0.0f) {
                stream->flags &= ~0x20;
                SOUND_SS_STOP_CH(ch, SoundDVDChan(i));
                continue;
            }
            stream->unk10 -= stream->unk14;
            if (stream->unk10 <= 0.0f) {
                stream->unk10 = 0.0f;
            }
            pan = (u8)stream->pan;
            SOUND_SS_SET_PAN_CH(ch, SoundDVDChan(i), pan);
        }
        if (stream->flags & 0x10) {
            stream->unk10 += stream->unk14;
            if (stream->unk10 >= stream->vol) {
                stream->unk10 = stream->vol;
                stream->unk14 = 0.0f;
                stream->flags &= ~0x10;
            }
            pan = (u8)stream->pan;
            SOUND_SS_SET_PAN_CH(ch, SoundDVDChan(i), pan);
        }
    }
}

// Variant of _ssPlayChRead used only by SoundSSMainInt; the two-buffer select and ARQ
// base are the entry's loadBuffer pair.
static inline void _ssDVDReadAsync_cache_entry_ss(SoundStream* stream, SoundStreamEntry* entry, u32 offset) {
    u32 len;
    u32 aligned;
    u32 n;
    SoundStreamEntry* e = entry;

    len = 0x8000;
    if (e->bytesLoaded + 0x8000 > *(u32*)&e->unk4C[0x14]) {
        len = *(u32*)&e->unk4C[0x14] - e->bytesLoaded;
    } else {
        for (n = 2; n < 6; n++) {
            if (e->bytesLoaded + n * 0x8000 <= *(u32*)&e->unk4C[0x14]) {
                continue;
            }
            len -= (0x8000 - (*(u32*)&e->unk4C[0x14] - (e->bytesLoaded + (n - 1) * 0x8000))) / n;
            len += 0x20 - (len & 0x1F);
            break;
        }
    }
    aligned = (len + 0x1F) & ~0x1F;
    if (!(stream->flags & 0x400)) {
        DVDMgrReadAsync(e->dvd, (void*)((&e->loadBuffer)[1 - *(u16*)e->unk4C]), aligned,
                        stream->unk4 + e->bytesLoaded + offset, _ssDVDReadAsync_activeChk);
    } else {
        u32 dest = stream->unk4 + e->bytesLoaded + offset + *(u32*)(sound.unk21C + stream->unk2 * 4);
        DCInvalidateRange((void*)((&e->loadBuffer)[1 - *(u16*)e->unk4C]), 0x8000);
        ARQPostRequest(&e->request, (u32)e, 1, 0,
                       dest, (&e->loadBuffer)[1 - *(u16*)e->unk4C], aligned, cache_flush);
    }
    e->requestLength = len;
}

// dont_inline keeps SoundSSMainInt out of SoundMainInt (the original keeps a
// stack frame and real call sequence).
#pragma dont_inline on
void SoundSSMainInt(void) {
    int i;
    SoundStream* stream;
    SoundStreamEntry* entry;

    for (i = 0; i < 3; i++) {
        stream = &sound.streams[i];
        if (!(stream->flags & 8)) {
            continue;
        }
        if (stream->flags & 2) {
            continue;
        }
        if (!(stream->flags & 1)) {
            continue;
        }

        if (*(u32*)&stream->entries[0].unk38[0xC] && *(s16*)&stream->entries[0].unk4C[8] == 0 &&
            !(stream->flags & 0x1000)) {
            _ssDVDReadAsync_cache_entry_ss(stream, &stream->entries[0], 0);
            stream->flags |= 0x1000;
            if (stream->flags & 0x400) {
                *(u16*)&stream->entries[0].unk4C[8] = 1;
            }
        }

        if (stream->count == 2 && *(u32*)&stream->entries[1].unk38[0xC] &&
            *(s16*)&stream->entries[1].unk4C[8] == 0 && !(stream->flags & 0x2000)) {
            entry = &stream->entries[1];
            _ssDVDReadAsync_cache_entry_ss(stream, entry,
                                           *(u32*)&entry->unk4C[0x14] + (0x20 - (*(u32*)&entry->unk4C[0x14] & 0x1F)));
            stream->flags |= 0x2000;
            if (stream->flags & 0x400) {
                *(u16*)&stream->entries[1].unk4C[8] = 1;
            }
        }
    }
}
#pragma dont_inline reset

// The two loops share `length` as their counter; the original keeps the first
// loop's counter in r3 and reuses r6 for the second, and `src`/`dest` both start at &sound.unk33C.
void SoundAIDMACallback(void) {
    s32 length;
    u8* address;
    u16* dest;
    s16* src;
    u32 sel;

    sound.callback();
    sound.dmaAddress = (u8*)(AIGetDMAStartAddr() | 0x80000000);
    length = AIGetDMALength();
    address = sound.dmaAddress;
    sound.dmaLength = length;

    src = (s16*)&sound.unk33C[0];
    dest = (u16*)src;
    for (; length != 0; length -= 2) {
        *dest = (address[0] << 8) | address[1];
        dest++;
        address += 2;
    }

    sel = sound.unk330;
    dest = (u16*)(sound.unk5C0 + (sel << 11));
    dest += sound.unk15C0;
    for (length = sound.dmaLength >> 1; length != 0; length -= 2) {
        *dest = *src;
        src += 2;
        dest++;
        if ((u32)++sound.unk15C0 >= 0x400) {
            sound.unk15C0 = 0;
            sound.unk330 = 1 - sound.unk330;
            dest = (u16*)(sound.unk5C0 + (sound.unk330 << 11));
        }
    }
}
