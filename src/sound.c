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
    SoundStream* temp;
    SoundSong* tempsong;
    SoundDVD* tempdvd;
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
    tempsong = (sound.songs = sndMalloc(sizeof(SoundSong) * 4));
    songs = tempsong;
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
    tempdvd = (sound.dvds = sndMalloc((sizeof(SoundDVD)) * 2));
    dvds = tempdvd;
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

    temp = streams;
    for (i = 0; i < 3; i++, temp++) {
        temp->flags = 0;

        ptr = sndMalloc(0x8000);
        if (ptr == 0) {
            while (1)
                ;
        } // infinite loop
        temp->entries[0].loadBuffer = (u32)ptr;

        ptr = sndMalloc(0x8000);
        if (ptr == 0) {
            while (1)
                ;
        } // infinite loop
        temp->entries[0].unk30 = ptr;

        size = sndStreamAllocLength(0xE00, 0);
        ptr = sndMalloc(size);
        if (ptr == 0) {
            while (1)
                ;
        } // infinite loop
        temp->entries[0].unk28 = ptr;

        ptr = sndMalloc(0x8000);
        if (ptr == 0) {
            while (1)
                ;
        } // infinite loop
        temp->entries[1].loadBuffer = (u32)ptr;

        ptr = sndMalloc(0x8000);
        if (ptr == 0) {
            while (1)
                ;
        } // infinite loop
        temp->entries[1].unk30 = ptr;

        size = sndStreamAllocLength(0xE00, 0);
        ptr = sndMalloc(size);
        if (ptr == 0) {
            while (1)
                ;
        } // infinite loop
        temp->entries[1].unk28 = ptr;
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
    s32 temp;

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
        temp = sound.unkE8;
        while (temp < 0xFF) {
            if (sndPushGroup(sound.projData[sound.dataId], (SND_GROUPID)temp, 0, sound.sdirData[sound.dataId],
                             sound.poolData[sound.dataId]) != TRUE) {
                break;
            }
            sound.groupCount[sound.dataId]++;
            sound.unkE8++;
            temp++;
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

// NONMATCHING (98.96%): same instructions; only register numbers or operands differ.
BOOL SoundSLibLoadDVD(const char* path) { // regalloc, stack, otherwise 1:1
    DVDEntry* entry;
    SoundGroup* temp_r0;
    SoundGroup* iter;
    SoundGroup* group;
    SoundGroup* var_r4;
    u32* var_r27;
    u32 temp_r28;
    u16 id;
    u32 length;
    void* temp_r0_3;
    void* pointer;

    if (sound.slibId >= 8) {
        return FALSE;
    }

    sprintf(str, "%s/%s%s", getMarioStDvdRoot(), path, ".slib");
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
    var_r27 = pointer;
    if (pointer == NULL) {
        return FALSE;
    }
    sound.slibData[sound.slibId] = pointer;

    while (1) {
        temp_r28 = *var_r27;
        if (*var_r27 == 0xFFFFFFFF) {
            break;
        }
        group = __memAlloc(HEAP_DEFAULT, 0x10);
        if (group == NULL) {
            while (1)
                ;
        }
        temp_r0 = sound.groups;
        id = 0;
        if (temp_r0 == NULL) {
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
        temp_r0_3 = var_r27 + 8;
        group->flags = 1;
        var_r27 = (u32*)((u32)var_r27 + (u32)temp_r28);
        group->unk2 = id;
        group->groupId = 0;
        group->songId = 0;
        group->arrfile = temp_r0_3;
        group->next = 0;
    }

    sprintf(str, "%s/%s%s", getMarioStDvdRoot(), path, ".stbl");
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
    sound.stblData[sound.slibId] = pointer;

    sprintf(str, "%s/%s%s", getMarioStDvdRoot(), path, ".etbl");
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
    sound.etblData[sound.slibId] = pointer;
    sound.slibId++;
    return TRUE;
}

// NONMATCHING (97.77%): 2 instructions differ, plus register differences.
BOOL SoundDropData(void) {
    SoundSong* song;
    SoundEffect* effect;
    SoundDVD* dvd;
    SoundStream* stream;
    SoundGroup *group, *temp;
    u32 i, *j;

    group = sound.groups;
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
        stream = &sound.streams[i];
        if (stream->flags) {
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

    sndSilence();

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
        stream = &sound.streams[i];
        if (stream->flags) {
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
        for (j = sound.groupCount; *j;) {
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
        temp = group->next;
        if (!(group->flags & 1)) {
            __memFree(HEAP_DEFAULT, group->arrfile);
        }
        __memFree(HEAP_DEFAULT, group);
        group = temp;
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

inline SoundGroup* SoundSongGetGroup(u8 a1) {
    u16 temp = a1;
    SoundGroup* group;

    for (group = sound.groups;; group = group->next) {
        if (group == 0) {
            return 0;
        }
        if (group->unk2 == temp) {
            break;
        }
    }
    return group;
}

void SoundSongPlayCh(s32 chan, u32 a2) {
    SoundSong* song;
    SoundGroup* group;

    song = &sound.songs[chan];
    if (sndSeqGetValid(song->seqId) == 1) {
        sndSeqStop(song->seqId);
    }
    song->vol = 127;
    group = SoundSongGetGroup((u8)a2);
    if (group) {
        song->group = group;
        group->groupId = (SND_GROUPID)((a2 >> 16) & 0xFF);
        group->songId = (SND_SONGID)((a2 >> 8) & 0xFF);
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

// NONMATCHING (89.01%): 4 instructions differ, 13 missing, 5 extra, plus register differences.
void SoundEfxMain(void) {
    SoundEffect* effect;
    f32 x, y, z;
    f32 v6, v9, v10;
    int i;

    x = sound.prevPos.x;
    y = sound.prevPos.y;
    z = sound.prevPos.z;
    v6 = ((TWO_PI * sound.unk1F4) / 360.0f);
    sound.prevPos.x = sound.pos.x;
    sound.prevPos.y = sound.pos.y;
    sound.prevPos.z = sound.pos.z;
    sound.dir.x = sound.pos.x - x;
    sound.dir.y = sound.pos.y - y;
    sound.dir.z = sound.pos.z - z;
    v9 = ((1.0f * (f32)sin(v6)) + 0.0f);
    v10 = (f32)cos(v6);
    sound.heading.x = v9;
    sound.heading.y = 0.0f;
    sound.heading.z = -((1.0f * v10) - 0.0f);
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

// NONMATCHING (99.71%): same instructions; only register numbers or operands differ.
void SoundDVDMain(void) {
    SoundDVD* dvd;
    int i;

    for (i = 0; i < 2; i++) {
        dvd = &sound.dvds[i];
        if (!(dvd->flags & 2)) {
            if (dvd->flags & 0x40) {
                if (dvd->unk8 == 0.0f) {
                    dvd->samples = AIGetStreamSampleCount();
                    SoundDVDStopCh(i);
                    dvd->flags |= 0x2;
                    dvd->flags &= ~0x40;
                    continue;
                }
                dvd->unk8 -= dvd->unkC;
                if (dvd->unk8 <= 0.0f) {
                    dvd->unk8 = 0.0f;
                }
                SoundDVDSetPanCh(i, (u8)dvd->unk2);
            }
            if (dvd->flags & 0x20) {
                if (dvd->unk8 == 0.0f) {
                    dvd->flags &= ~0x20;
                    SoundDVDStopCh(i);
                    continue;
                }
                dvd->unk8 -= dvd->unkC;
                if (dvd->unk8 <= 0.0f) {
                    dvd->unk8 = 0.0f;
                }
                SoundDVDSetPanCh(i, (u8)dvd->unk2);
            }
            if (dvd->flags & 0x10) {
                dvd->unk8 += dvd->unkC;
                if (dvd->unk8 >= dvd->unk4) {
                    dvd->unk8 = dvd->unk4;
                    dvd->unkC = 0.0f;
                    dvd->flags &= ~0x10;
                }
                SoundDVDSetPanCh(i, (u8)dvd->unk2);
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

// NONMATCHING (0.44%): empty placeholder, not decompiled yet.
void _ssDVDReadAsync_cache_aram(u32 pointerToARQRequest) {
}

// NONMATCHING (1.49%): empty placeholder, not decompiled yet.
void _ssDVDReadAsync_cache_next(s32 result, DVDFileInfo* fileInfo) {
}

s16 dtbl[16] = {1, 7, 24, 59, 83, 111, 236, 555, 742, 885, 1052, 2221, 3285, 4432, 6962, 9300};

// NONMATCHING (99.92%): the else nibble is now in r9 like the original, but its sign-extension
// before the <<4 is fused into the shift destination (extsb r0,r9; slwi r0,r0,4) where the target
// extends in place (extsb r9,r9; slwi r0,r9,4); only those 2 operands differ (see REPORT.md).
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
            delta = (s8)(delta << 4);
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
            delta = (s8)index;
            if ((s8)delta == 8) {
                *(u16*)(e + 0x5c) = ((byte >> 4) & 0xF) + 4;
                *(u16*)(e + 0x5a) = 0;
                continue;
            }
            index = (byte >> 4) & 0xF;
            *(u16*)(e + 0x58) = index;
            delta = (s8)(delta << 4);
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

#ifdef MUST_MATCH
// MUST_MATCH: separate register-homed copies of buffer1 so MWCC picks the retail registers for the
// zero-fill and byte-copy loops. The #else keeps the natural code (same names, no hint).
#define SS_CB_REG register
#else
#define SS_CB_REG
#endif
// Register colouring only: MWCC homes buffer1 in r9 instead of retail r6, which then reshuffles the
// prologue (`li r0,0x0`/`mulli r8`/`lwz r9,...` vs `mulli r3`/`lwz r10,...`). The `register` copies of
// buffer1 below re-home the three loop pointers to the retail registers (98.73 vs 97.56 without). The
// R4a/R5 `src` setup and the DPCM `in` setup still colour temporaries differently (r10 vs r7).
// SoundStream has no write-position field; it lives at +0x24 and is written raw. SoundStreamEntry at
// +0x2c is loadBuffer[?] and +0x38..0x60 is unk38/unk4C; all accessed raw here.
// NONMATCHING (95.94%): every path and branch structure matches; only buffer1's home register
// (r9 vs retail r6) and the prologue/temporary colouring it cascades into differ.
u32 _sscallback(void* buffer1, u32 len1, void* buffer2, u32 len2, u32 user) {
    SoundStream* stream = &sound.streams[user & 0xFF];
    SS_CB_REG void* t1 = buffer1;
    SS_CB_REG void* t3 = buffer1;
    SS_CB_REG void* t5 = buffer1;
    u8* ep;
    u32 i;

    *(u32*)(stream->unk1C + 8) = 0;

    if ((stream->flags & 0x8006) || sound.unk214) {
        if (!(stream->flags & 0x100)) {
            for (i = 0; i != len1; i++) {
                *(s16*)t1 = 0;
                t1 = (s16*)t1 + 1;
                *(u32*)(stream->unk1C + 8) += 1;
            }
            for (i = 0; i != len2; i++) {
                *(s16*)buffer2 = 0;
                buffer2 = (s16*)buffer2 + 1;
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
                *(u16*)t3 = 0;
                t3 = (void*)((u16*)t3 + 1);
            }
            n = len2 / 7 * 4 & ~7;
            n /= 2;
            for (j = 0; j != n; j++) {
                *(u16*)buffer2 = 0;
                buffer2 = (void*)((u16*)buffer2 + 1);
            }
        }
        return len1 + len2;
    }

    ep = (u8*)((user & 0x100) ? &stream->entries[1] : &stream->entries[0]);

    if (stream->flags & 0x80) {
        u8* in;

        in = (u8*)(((u32*)(ep + 0x2c))[*(u16*)(ep + 0x4c)] + *(u32*)(ep + 0x44));
        in = ssDecodeDPCM(stream, (SoundStreamEntry*)ep, (s16*)buffer1, len1, in);
        ssDecodeDPCM(stream, (SoundStreamEntry*)ep, (s16*)buffer2, len2, in);
        return *(u32*)(stream->unk1C + 8);
    }

    if (stream->flags & 0x100) {
        u8* src;
        u8* dst;
        u32 count;

        src = (u8*)(((u32*)(ep + 0x2c))[*(u16*)(ep + 0x4c)] + *(u32*)(ep + 0x44));
        dst = (u8*)t5;
        count = (len1 + 13) / 7 * 4 & ~7;
        while (count != 0) {
            *dst++ = *src++;
            count--;
            *(u32*)(ep + 0x44) += 1;
            if (*(u32*)(ep + 0x44) >= *(u32*)(ep + 0x3c + ((*(u16*)(ep + 0x4c)) << 2))) {
                if (*(s16*)(ep + 0x54) == 0 && !(stream->flags & 0x4000)) {
                    *(u32*)(ep + 0x38) -= *(u32*)(ep + 0x44);
                    *(u32*)(ep + 0x44) = 0;
                    src = (u8*)*(u32*)(ep + 0x2c + ((*(u16*)(ep + 0x4c)) << 2));
                    continue;
                } else {
                    *(s16*)(ep + 0x54) = 0;
                    *(u32*)(ep + 0x44) = 0;
                    *(u16*)(ep + 0x4c) = 1 - *(u16*)(ep + 0x4c);
                    src = (u8*)*(u32*)(ep + 0x2c + ((*(u16*)(ep + 0x4c)) << 2));
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
        dst = (u8*)buffer2;
        count = (len2 + 13) / 7 * 4 & ~7;
        while (count != 0) {
            *dst++ = *src++;
            count--;
            *(u32*)(ep + 0x44) += 1;
            if (*(u32*)(ep + 0x44) >= *(u32*)(ep + 0x3c + ((*(u16*)(ep + 0x4c)) << 2))) {
                if (*(s16*)(ep + 0x54) == 0 && !(stream->flags & 0x4000)) {
                    *(u32*)(ep + 0x38) -= *(u32*)(ep + 0x44);
                    *(u32*)(ep + 0x44) = 0;
                    src = (u8*)*(u32*)(ep + 0x2c + ((*(u16*)(ep + 0x4c)) << 2));
                    continue;
                } else {
                    *(s16*)(ep + 0x54) = 0;
                    *(u32*)(ep + 0x44) = 0;
                    *(u16*)(ep + 0x4c) = 1 - *(u16*)(ep + 0x4c);
                    src = (u8*)*(u32*)(ep + 0x2c + ((*(u16*)(ep + 0x4c)) << 2));
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
        *(u32*)(stream->unk1C + 8) = len1 + len2;
        return len1 + len2;
    }

    {
        u8* src;
        s16* dst;
        u32 count;

        dst = (s16*)buffer1;
        count = len1;
        src = (u8*)(((u32*)(ep + 0x2c))[*(u16*)(ep + 0x4c)] + (*(u32*)(ep + 0x44) & ~1));
        while (count != 0) {
            *dst++ = *(s16*)src;
            src += 2;
            count--;
            *(u32*)(stream->unk1C + 8) += 1;
            *(u32*)(ep + 0x44) += 2;
            if (*(u32*)(ep + 0x44) >= *(u32*)(ep + 0x3c + ((*(u16*)(ep + 0x4c)) << 2))) {
                if (*(s16*)(ep + 0x54) == 0 && !(stream->flags & 0x4000)) {
                    *(u32*)(ep + 0x38) -= *(u32*)(ep + 0x44);
                    *(u32*)(ep + 0x44) = 0;
                    src = (u8*)*(u32*)(ep + 0x2c + ((*(u16*)(ep + 0x4c)) << 2));
                    continue;
                } else {
                    *(s16*)(ep + 0x54) = 0;
                    *(u32*)(ep + 0x44) = 0;
                    *(u16*)(ep + 0x4c) = 1 - *(u16*)(ep + 0x4c);
                    src = (u8*)*(u32*)(ep + 0x2c + ((*(u16*)(ep + 0x4c)) << 2));
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
        count = len2;
        dst = (s16*)buffer2;
        while (count != 0) {
            *dst++ = *(s16*)src;
            src += 2;
            count--;
            *(u32*)(stream->unk1C + 8) += 1;
            *(u32*)(ep + 0x44) += 2;
            if (*(u32*)(ep + 0x44) >= *(u32*)(ep + 0x3c + ((*(u16*)(ep + 0x4c)) << 2))) {
                if (*(s16*)(ep + 0x54) == 0 && !(stream->flags & 0x4000)) {
                    *(u32*)(ep + 0x38) -= *(u32*)(ep + 0x44);
                    *(u32*)(ep + 0x44) = 0;
                    src = (u8*)*(u32*)(ep + 0x2c + ((*(u16*)(ep + 0x4c)) << 2));
                    continue;
                } else {
                    *(s16*)(ep + 0x54) = 0;
                    *(u32*)(ep + 0x44) = 0;
                    *(u16*)(ep + 0x4c) = 1 - *(u16*)(ep + 0x4c);
                    src = (u8*)*(u32*)(ep + 0x2c + ((*(u16*)(ep + 0x4c)) << 2));
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
    }
    return *(u32*)(stream->unk1C + 8);
}
#undef SS_CB_REG

// Returns void: the original never writes r3, and an s32 definition (or header prototype!) shifts the tail to r4.
#ifdef MUST_MATCH
// Forces MWCC to materialise str's address into path's home register (r27) before the copy to r3;
// writing `path = str;` directly coalesces the address into r3 and copies the other way.
static inline char* SoundSSPlayChEx_getPath(void) {
    char* path = str;
    return path;
}
#endif
void SoundSSPlayChEx(s32 chan, u32 a2, u32 a3) {
    SoundStream* stream = &sound.streams[chan];
    u8* e = (u8*)stream;
    char* path;
    int i;

    if (stream->flags) {
        SoundSSStopCh(chan);
    }

    if ((s32)a3 != 0) {
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
    if ((s32)a3 != 0) {
        stream->flags |= 0x400;
    }

    sprintf(str, "%s/%s%s", getMarioStDvdRoot(), a2, ".stm");
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

// NONMATCHING (0.16%): 626 missing.
void SoundSSPlayChEx_main(s32 result, DVDFileInfo* fileInfo) {
    if (result >= 0) {}
}

s32 SoundSSCheck(s32 chan) {
    return sound.streams[chan].flags;
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

void SoundSSPlayCh(s32 chan, u32 a2) {
    SoundSSPlayChEx(chan, a2, 0);
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

void SoundSSContinueCh(s32 chan) {
    SoundStream* stream = &sound.streams[chan];

    if (stream->flags) {
        stream->flags &= ~2;
        stream->unk14 = stream->vol;
        stream->flags |= 0x10;
    }
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

s32 SoundSSGetVolCh(s32 chan) {
    SoundStream* stream;

    stream = &sound.streams[chan];
    if (stream->flags) {
        return (s32)stream->vol;
    } else {
        return 0;
    }
}

void SoundSSSetPanCh(s32 chan, u8 pan) {
    SoundStream* stream = &sound.streams[chan];

    if (!stream->flags) {
        return;
    }
    stream->pan = pan;
    if (!(stream->flags & 8)) {
        return;
    }
    if (stream->count == 1) {
        sndStreamMixParameter(stream->entries[0].streamId, stream->unk10, pan, (u8)stream->span, 0);
    } else {
        f32 left = stream->unk10;
        f32 right = left;

        if (pan < 64) {
            right = ((f32)pan * left) / 63.0f;
        }
        if (!((u32)(pan - 64) >> 31)) {
            left = ((f32)(127 - pan) * left) / 63.0f;
        }
        sndStreamMixParameter(stream->entries[0].streamId, left, 0, (u8)stream->span, 0);
        sndStreamMixParameter(stream->entries[1].streamId, right, 0x7F, (u8)stream->span, 0);
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

void SoundSSStopCh(s32 chan) {
    SoundStream* stream = &sound.streams[chan];

    if (!stream->flags) {
        return;
    }
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
    if (sound.unk20C && !--sound.unk20C) {
        OSRestoreInterrupts(sound.interrupts);
    }
    sound.unk104 = -1;
}

// The original allocates the inlined helpers' stream offset to r26 and the loop counter to r28; this build
// swaps them (r28/r26), and the outer offset/helper pointer land in r29/r30 instead of r30/r29.
// NONMATCHING (99.05%): instruction stream and all constants identical; only induction-variable registers differ.
void SoundSSMain(void) {
    SoundStream* stream;
    int i;

    for (i = 0; i < 3; i++) {
        stream = &sound.streams[i];
        if (!(stream->flags & 8)) {
            continue;
        }
        if (stream->flags & 2) {
            continue;
        }
        if (stream->flags & 4) {
            stream->flags &= ~4;
            SoundSSStopCh(i);
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
            SoundSSSetPanCh(i, (u8)stream->pan);
        }
        if (stream->flags & 0x20) {
            if (stream->unk10 == 0.0f) {
                stream->flags &= ~0x20;
                SoundSSStopCh(i);
                continue;
            }
            stream->unk10 -= stream->unk14;
            if (stream->unk10 <= 0.0f) {
                stream->unk10 = 0.0f;
            }
            SoundSSSetPanCh(i, (u8)stream->pan);
        }
        if (stream->flags & 0x10) {
            stream->unk10 += stream->unk14;
            if (stream->unk10 >= stream->vol) {
                stream->unk10 = stream->vol;
                stream->unk14 = 0.0f;
                stream->flags &= ~0x10;
            }
            SoundSSSetPanCh(i, (u8)stream->pan);
        }
    }
}

// dont_inline keeps SoundSSMainInt out of SoundMainInt (the original keeps a
// stack frame and real call sequence).
#pragma dont_inline on
// NONMATCHING (0.43%): 232 missing.
void SoundSSMainInt(void) {
    int i;
    SoundStream* stream;

    for (i = 0; i < 3; i++) {
        stream = &sound.streams[i];
        if (stream->flags & 8 && !(stream->flags & 2) && stream->flags & 1) {

        }
    }
}
#pragma dont_inline reset

// NONMATCHING (96.03%): second half matches byte-for-byte now that sound.unk330 is read unsigned (lhz) and
// the toggle uses clrlslwi; only the first loop's r6/r7/r8 assignment (i/dest/base) and one hoisted li r4 differ.
void SoundAIDMACallback(void) {
    s32 i;
    u32 length;
    u8* address;
    u16* dest;
    s16* src;
    u32 sel;

    sound.callback();
    sound.dmaAddress = (u8*)(AIGetDMAStartAddr() | 0x80000000);
    length = AIGetDMALength();
    address = sound.dmaAddress;
    sound.dmaLength = length;

    dest = (u16*)&sound.unk33C[0];
    for (; (s32)length != 0; length -= 2) {
        *dest = (address[0] << 8) | address[1];
        dest++;
        address += 2;
    }

    src = (s16*)&sound.unk33C[0];
    sel = sound.unk330;
    dest = (u16*)(sound.unk5C0 + (sel << 11));
    dest += sound.unk15C0;
    for (i = sound.dmaLength >> 1; i != 0; i -= 2) {
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
