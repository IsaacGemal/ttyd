#pragma once

#include "evt/evt_badgeshop.h"
#include "evt/evt_johoya.h"
#include "evt/evt_yuugijou.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include <dolphin/card.h>
#include <dolphin/os.h>

typedef struct {
    struct {
        char fileName[0x20]; //0x0
        char dateString[0x20]; //0x20
        u8 bannerTex[0x1800]; //0x40
        u8 iconTex[0x400]; //0x1840
        u8 iconTlut[0x200]; //0x1C40
        u16 rumbleEnabled; //0x1E40, gp->rumbleEnabled (make_header stores it, read_all_main restores it)
        u16 soundSurround; //0x1E42, gp->soundSurround, the same way
        u16 lastFileSlot; //0x1E44, gp->lastFileSlot, the same way
    } data;
    u8 __align[0x1FF8 - 0x1E46]; //0x1E46
    u32 checksum1; //0x1FF8
    u32 checksum2; //0x1FFC
} CardMetadata;

typedef struct {
    struct {
        u16 flags; //0x0
        u8 pad_0x2[0x8 - 0x2]; //0x2
        GlobalWork global; //0x8
        PouchData pouch; //0x13E0
        NpcDeadInfo deadInfo[64]; //0x19B4
        BadgeShopWork badgeShop; //0x1EB4
        PiantaParlorWork parlor; //0x1FD8
        InformantUnknown informant; //0x2110
        u8 pad_0x213B[0x2140 - 0x213B]; //0x213B
    } data;
    u8 __align[0x3FF0 - 0x2140]; //0x2140
    char version[4]; //0x3FF0
    s32 size; //0x3FF4
    u32 checksum1; //0x3FF8
    u32 checksum2; //0x3FFC
} CardSaveSlot;

typedef struct {
    CardMetadata metadata; //0x0
    CardSaveSlot saves[4]; //0x2000 (+ 0x4000 * save_index)
} CardData;

typedef struct {
    vu16 flags; //0x0, 0x8000 = slot watch on, 0x4000 = slot A card inserted/removed while watching
    u8 pad_0x2[0x4 - 0x2]; //0x2
    void* workArea; //0x4
    s32 chan; //0x8
    s32 fileNo; //0xC
    u64 serialNo; //0x10
    u8 pad_0x18[0x1C - 0x18]; //0x18
    CARDFileInfo info; //0x1C
    CARDStat status; //0x30
    s32 result; //0x9C
    s32 currSlot; //0xA0
    s32 readSlot; //0xA4, the slot read_main reads; read_all_main steps it from 0 to 3
    CardData* data; //0xA8
    void* readBuf; //0xAC, memcard_read's buffer for the header check (CardMetadata) and the save checks (CardSaveSlot)
    CardSaveSlot* writeBuf; //0xB0, the save cardErase and cardCopy prepare; write_main writes it (flag 0x800), then copies it to data->saves
    BOOL validSave1; //0xB4, is main save valid?
    BOOL validSave2; //0xB8, is backup save valid?
    s32 firstCopy; //0xBC, the copy write_main writes first (0 main, 1 backup): the backup when only the main copy is valid
    s32 secondCopy; //0xC0, the copy it writes next
    u8 pad_0xC4[0xC8 - 0xC4]; //0xC4
    OSTime lastSaveTime1; //0xC8
    OSTime lastSaveTime2; //0xD0
    s32 step; //0xD8, the running operation's step (each *_main switches on it), advanced after each finished step
    u8 pad_0xDC[0xE0 - 0xDC]; //0xDC
    s32 operation; //0xE0, what cardMain runs: 1 write_main (cardWrite, cardErase, cardCopy), 2 write_header_main, 3 read_main,
                   //      4 read_all_main (cardReadAll), 5 create_main (cardCreate), 6 the format steps (cardFormat),
                   //      7 the delete steps, 8 nothing (set after an error); 0 when idle
    s32 slotWatchChan; //0xE4, last slot A state while watching: 0 = card in, -1 = none (same encoding as chan)
    u8 pad_0xE8[0xEC - 0xE8]; //0xE8
    s32 slotWatchTimer; //0xEC, frames cardIsExec stays busy after cardSlotWatchBegin
} CardWork;

CardData* cardGetFilePtr(void);
void cardBufReset(void);
void cardInit(void);
void cardSlotWatchBegin(void); // called by evt_memcard
void cardSlotWatchEnd(void); // called by evt_memcard
BOOL cardSlotChanged(void); // called by evt_memcard
BOOL cardIsExec(void);
s32 cardGetCode(void);
void cardMain(void);
void cardCopy2Main(s32 slot);
void cardErase(s32 slot);
void cardCopy(s32 from, s32 to);
void cardWrite(s32 slot);
void cardWriteHeader(void);
void cardReadAll(void);
void cardCreate(void);
void cardFormat(void);
void make_header(void);
void make_data(void);
void memcard_probe(void);
void memcard_mount(void);
void memcard_unmount(void);
void memcard_check(void);
void memcard_read(void* addr, s32 length, s32 offset);
void memcard_set_stat(void);
void memcard_get_stat(void);
void memcard_get_serial(void);
void memcard_open(void);
void memcard_close(void);
void mountDetachCallback(s32 chan, s32 result);
void mountAttachCallback(s32 chan, s32 result);
void checkCallback(s32 chan, s32 result);
void formatCallback(s32 chan, s32 result);
void createCallback(s32 chan, s32 result);
void writeCallback(s32 chan, s32 result);
void readCallback(s32 chan, s32 result);
void statusCallback(s32 chan, s32 result);
void write_main(void);
void write_header_main(void);
void read_main(void);
void read_all_main(void);
void create_main(void);
void format_main(void);
void delete_main(void);
BOOL headerCheck(void);
BOOL headerCheck2(void);
BOOL dataCheck(void);
void check_sum(CardSaveSlot* save);
