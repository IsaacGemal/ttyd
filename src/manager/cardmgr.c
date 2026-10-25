/* "cardmgr" - Memory Card Manager
* Status: matched
*
* Function: handles all communication with save data and writing to memory cards
*
* Last Update: 1/19/2023, first finished, can prolly use a cleanup pass
*/
#include "manager/cardmgr.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "memory.h"
#include "party.h"
#include <string.h>
#include <stddef.h>

extern GlobalWork* gp;
extern BadgeShopWork* bdsw;
extern PiantaParlorWork* yuwp;
extern InformantWork _jdt;

//.rodata
#include "assets/_mariost_icon_tex.inc"
#include "assets/_mariost_icon_tlut.inc"
#include "assets/_mariost_banner_tex.inc"
#include <dolphin/card.h>
#include <stdio.h>

//.sbss
static CardWork* wp;

//.sdata
const char* version = "009";

//local prototypes

static void memcard_format(void);
static void memcard_delete(void);
static void memcard_create(void);
static void memcard_write(void* addr, s32 length, s32 offset);




CardData* cardGetFilePtr(void) {
	return wp->data;
}

void cardBufReset(void) {
	s32 flags;
	s32 timeout;

#ifdef MUST_MATCH
	// Dead store: never read, but it changes the object.
	flags = wp->flags;
#endif
	if (wp->flags & 1) {
		CARDCancel(&wp->info);
		for (timeout = 1000000; timeout > 0; timeout--) {
			if (!(wp->flags & 1)) {
				break;
			}
		}
		wp->flags &= ~1;
	}

	make_header();
	make_data();

	wp->chan = -1;
	wp->fileNo = -1;
	wp->serialNo = 0;
	wp->operation = 0;
	wp->result = CARD_RESULT_NOCARD;
	wp->flags = 0;
}

void cardInit(void) {
	wp = __memAlloc(HEAP_DEFAULT, sizeof(CardWork));
	memset(wp, 0, sizeof(CardWork));
	wp->workArea = __memAlloc(HEAP_DEFAULT, CARD_WORKAREA_SIZE);
	memset(wp->workArea, 0, CARD_WORKAREA_SIZE);
	wp->data = __memAlloc(HEAP_DEFAULT, sizeof(CardData));
	memset(wp->data, 0, sizeof(CardData));
	wp->readBuf = __memAlloc(HEAP_DEFAULT, sizeof(CardSaveSlot));
	memset(wp->readBuf, 0, sizeof(CardSaveSlot));
	wp->writeBuf = __memAlloc(HEAP_DEFAULT, sizeof(CardSaveSlot));
	memset(wp->writeBuf, 0, sizeof(CardSaveSlot));

	make_header();
	make_data();

	wp->chan = -1;
	wp->fileNo = -1;
	wp->serialNo = 0;
	wp->operation = 0;
	wp->result = CARD_RESULT_NOCARD;
	wp->flags = 0;
	CARDInit();
}

void cardSlotWatchBegin(void) {
	wp->slotWatchChan = wp->chan;
	wp->slotWatchTimer = 10;
	wp->flags |= 0x8000;
}

void cardSlotWatchEnd(void) {
	wp->flags &= ~0x8000;
	wp->flags &= ~0xC000;
}

BOOL cardSlotChanged(void) {
    if (wp->flags & 0x8000 && wp->flags & 0x4000) {
        return TRUE;
    }
    return FALSE;
}

BOOL cardIsExec(void) {
    if (wp == NULL) {
        return FALSE;
    }
    if (wp->flags & 2) {
        return TRUE;
    }
    if (wp->flags & 1) {
        return TRUE;
    }
    return wp->slotWatchTimer != 0;
}

s32 cardGetCode(void) {
	return wp->result;
}

void cardMain(void) {
	s32 chan;

	if (wp->flags & 0x8000) {
		chan = CARDProbe(0) ? 0 : -1; //is something inserted?
		if (wp->slotWatchChan != chan) {
			wp->flags |= 0x4000;
		}
		wp->slotWatchChan = chan;
		if (wp->slotWatchTimer) {
			wp->slotWatchTimer--;
		}
	}

	switch (wp->operation) {
		case 1:
			write_main();
			break;

		case 2:
			write_header_main();
			break;

		case 3:
			read_main();
			break;

		case 4:
			read_all_main();
			break;

		case 5:
			create_main();
			break;

		case 6:
			if (wp->result != CARD_RESULT_READY) {
				wp->operation = 8;
				wp->flags |= 0x200;
				wp->flags &= ~2;
			}
			else if (!(wp->flags & 1)) {
				switch (wp->step) {
					case 0:
						memcard_probe();
						break;

					case 1:
						memcard_mount();
						break;

					case 2:
						memcard_format();
						break;

					case 3:
						memcard_unmount();
						break;

					case 4:
						wp->operation = 0;
						wp->flags |= 0x100;
						wp->flags &= ~2;
						break;
				}
				wp->step++;
			}
			else if (CARDGetResultCode(wp->chan) != CARD_RESULT_BUSY && wp->result != CARDGetResultCode(wp->chan)) {
				wp->result = CARDGetResultCode(wp->chan);
				wp->flags &= ~1;
			}
			break;

		case 7:
			if (wp->result != CARD_RESULT_READY) {
				wp->operation = 8;
				wp->flags |= 0x200;
				wp->flags &= ~2;
			}
			else if (!(wp->flags & 1)) {
				switch (wp->step) {
					case 0:
						memcard_probe();
						break;

					case 1:
						memcard_mount();
						break;

					case 2:
						memcard_check();
						break;

					case 3:
						memcard_delete();
						break;

					case 4:
						memcard_unmount();
						break;

					case 5:
						wp->operation = 0;
						wp->flags |= 0x100;
						wp->flags &= ~2;
						break;
				}
				wp->step++;
			}
			else if (CARDGetResultCode(wp->chan) != CARD_RESULT_BUSY && wp->result != CARDGetResultCode(wp->chan)) {
				wp->result = CARDGetResultCode(wp->chan);
				wp->flags &= ~1;
			}
			break;

		case 8:
		default:
			break;
	}
}

void cardCopy2Main(s32 slot) {
    void* backup2;
    u32 backup3;
    CardSaveSlot* save;
    void* backup1;
    s32 backup4;
    s32 backup5;
    s32 backup6;

    save = &wp->data->saves[slot];
    backup1 = gp->fbatData;
    backup2 = gp->mapAlloc;
    backup3 = (u32)(gp->flags & 8);
    backup4 = gp->soundSurround;
    backup5 = gp->rumbleEnabled;
    backup6 = gp->lastFileSlot;
    memcpy(gp, &save->data.global, sizeof(GlobalWork));
    gp->fbatData = backup1;
    memcpy(pouchGetPtr(), &save->data.pouch, sizeof(PouchData));
    memcpy(fbatGetPointer()->deadInfo, save->data.deadInfo, sizeof(NpcDeadInfo) * 64);
    memcpy(bdsw, &save->data.badgeShop, sizeof(BadgeShopWork));
    memcpy(yuwp, &save->data.parlor, sizeof(PiantaParlorWork));
    memcpy(&_jdt.unk14, &save->data.informant, sizeof(InformantUnknown));
    gp->resetType = 0;
    gp->resetButtonDown = 0;
    gp->dvdError = 0;
    gp->retraceTime = OSGetTime();
    strcpy(gp->currentAreaName, "123");
    gp->relocationBase = NULL;
    gp->soundSurround = backup4;
    gp->rumbleEnabled = backup5;
    gp->lastFileSlot = backup6;
    gp->mapAlloc = backup2;
    gp->unk8 = 0;
    gp->field_C = 0;
    if (backup3 != 0) {
        gp->flags |= 8;
    }
    else {
        gp->flags &= ~8;
    }
}

void cardErase(s32 slotNo) {
    CardSaveSlot* save;

    if (!(wp->flags & 2)) {
        wp->operation = 1;
        wp->currSlot = slotNo;
        wp->step = 0;
        wp->result = 0;
        wp->flags |= 2;
        wp->flags &= ~0x300;
        wp->flags |= 0x800;
        save = wp->writeBuf;
        memset(save, 0, sizeof(save->data));
        save->data.flags |= 1;
        check_sum(save);
    }
}

void cardCopy(s32 from, s32 to) {
    if (!(wp->flags & 2)) {
        wp->operation = 1;
        wp->currSlot = to;
        wp->step = 0;
        wp->result = 0;
        wp->flags |= 2;
        wp->flags &= ~0x300;
        wp->flags |= 0x800;
        memcpy(wp->writeBuf, &wp->data->saves[from], sizeof(CardSaveSlot));
    }
}

void cardWrite(s32 slot) {
    CardSaveSlot* save;
    MarioWork* mario;
    PartyEntry* party;
    s8 partyId;

    if (!(wp->flags & 2)) {
        wp->operation = 1;
        wp->currSlot = slot;
        wp->step = 0;
        wp->result = 0;
        wp->flags |= 2;
        wp->flags &= ~0x300;
        wp->flags &= ~0x800;
        save = &wp->data->saves[slot];
        mario = marioGetPtr();
        memset(save, 0, sizeof(CardSaveSlot));
        gp->savePlayerPos = mario->position;
        party = partyGetPtr(marioGetPartyId());
        if (party != NULL) {
            partyId = party->currentMemberId;
        } else {
            partyId = mario->prevPartyMemberId[0];
        }
        gp->savePartyId[0] = partyId;
        party = partyGetPtr(marioGetExtraPartyId());
        if (party != NULL) {
            partyId = party->currentMemberId;
        } else {
            partyId = mario->prevPartyMemberId[1];
        }
        gp->savePartyId[1] = partyId;
        gp->saveTime = OSGetTime();
        gp->saveCount++;
        gp->flags &= ~1;
        gp->flags &= ~2;
        gp->resetType = 0;
        gp->dvdError = 0;
        memcpy(&save->data.global, gp, sizeof(GlobalWork));
        memcpy(&save->data.pouch, pouchGetPtr(), sizeof(PouchData));
        memcpy(save->data.deadInfo, fbatGetPointer()->deadInfo, sizeof(save->data.deadInfo));
        memcpy(&save->data.badgeShop, bdsw, sizeof(BadgeShopWork));
        memcpy(&save->data.parlor, yuwp, sizeof(PiantaParlorWork));
        memcpy(&save->data.informant, &_jdt.unk14, sizeof(_jdt.unk14));
        save->data.flags &= ~1;
        save->data.flags &= ~2;
        check_sum(save);
    }
}

void write_main(void) {
    if (wp->result) {
        wp->operation = 8;
        wp->flags |= 0x200;
        wp->flags &= ~2;
    }
    else if (!(wp->flags & 1)) {
        switch (wp->step) {
            case 0:
                memcard_probe();
                break;
            case 1:
                memcard_mount();
                break;
            case 2:
                memcard_check();
                break;
            case 3:
                memcard_open();
                break;
            case 4:
                memcard_read(wp->readBuf, sizeof(CardMetadata), 0);
                break;
            case 5:
                if (!headerCheck()) {
                    make_header();
                    memcard_write(wp->data, sizeof(CardMetadata), 0);
                }
                break;
            case 6:
                memcard_read(wp->readBuf, sizeof(CardSaveSlot), (s32)(sizeof(CardMetadata) + (wp->currSlot * sizeof(CardSaveSlot))));
                break;
            case 7:
                wp->validSave1 = dataCheck();
                break;
            case 8:
                memcard_read(wp->readBuf, sizeof(CardSaveSlot), (s32)(sizeof(CardData) + (wp->currSlot * sizeof(CardSaveSlot))));
                break;
            case 9:
                wp->validSave2 = dataCheck();
                break;
            case 10:
                if (wp->validSave1 && !wp->validSave2) {
                    wp->firstCopy = 1;
                    wp->secondCopy = 0;
                } else {
                    wp->firstCopy = 0;
                    wp->secondCopy = 1;
                }
                if (wp->flags & 0x800) {
                    memcard_write(wp->writeBuf, sizeof(CardSaveSlot), (s32)(sizeof(CardMetadata) + (wp->currSlot * sizeof(CardSaveSlot)) + (wp->firstCopy * (sizeof(CardSaveSlot) * 4))));
                } else {
                    memcard_write(&wp->data->saves[wp->currSlot], sizeof(CardSaveSlot), (s32)(sizeof(CardMetadata) + (wp->currSlot * sizeof(CardSaveSlot)) + (wp->firstCopy * (sizeof(CardSaveSlot) * 4))));
                }
                break;
            case 11:
                if (wp->flags & 0x800) {
                    memcard_write(wp->writeBuf, sizeof(CardSaveSlot), (s32)(sizeof(CardMetadata) + (wp->currSlot * sizeof(CardSaveSlot)) + (wp->secondCopy * (sizeof(CardSaveSlot) * 4))));
                } else {
                    memcard_write(&wp->data->saves[wp->currSlot], sizeof(CardSaveSlot), (s32)(sizeof(CardMetadata) + (wp->currSlot * sizeof(CardSaveSlot)) + (wp->secondCopy * (sizeof(CardSaveSlot) * 4))));
                }
                break;
            case 12:
                memcard_get_serial();
                break;
            case 13:
                memcard_close();
                break;
            case 14:
                memcard_unmount();
                break;
            case 15:
                wp->operation = 0;
                wp->flags |= 0x100;
                wp->flags |= 0x2000;
                wp->flags &= ~2;
                if (wp->flags & 0x800) {
                    wp->flags &= ~0x800;
                    memcpy(&wp->data->saves[wp->currSlot], wp->writeBuf, sizeof(CardSaveSlot));
                }
                break;
        }
        wp->step++;
    }
    else if (CARDGetResultCode(wp->chan) != CARD_RESULT_BUSY && wp->result != CARDGetResultCode(wp->chan)) {
        wp->result = CARDGetResultCode(wp->chan);
        wp->flags &= ~1;
    }
}

void cardWriteHeader(void) {
    if (!(wp->flags & 2)) {
        wp->operation = 2;
        wp->currSlot = 0;
        wp->step = 0;
        wp->result = 0;
        wp->flags |= 2;
        wp->flags &= ~0x300;
        wp->flags &= ~0x800;
        make_header();
    }
}

void write_header_main(void) {
    if (wp->result) {
        wp->operation = 8;
        wp->flags |= 0x200;
        wp->flags &= ~2;
    }
    else if (!(wp->flags & 1)) {
        switch (wp->step) {
            case 0:
                memcard_probe();
                break;
            case 1:
                memcard_mount();
                break;
            case 2:
                memcard_check();
                break;
            case 3:
                memcard_open();
                break;
            case 4:
                memcard_write(wp->data, sizeof(CardMetadata), 0);
                break;
            case 5:
                memcard_get_serial();
                break;
            case 6:
                memcard_close();
                break;
            case 7:
                memcard_unmount();
                break;
            case 8:
                wp->operation = 0;
                wp->flags |= 0x100;
                wp->flags |= 0x2000;
                wp->flags &= ~2;
                break;
        }
        wp->step++;
    }
    else if (CARDGetResultCode(wp->chan) != CARD_RESULT_BUSY && wp->result != CARDGetResultCode(wp->chan)) {
        wp->result = CARDGetResultCode(wp->chan);
        wp->flags &= ~1;
    }
}

void read_main(void) {
    CardSaveSlot* save;
    s32 saveType; //invalid = -1, main = 0, backup = 1

    if (wp->result) {
        wp->operation = 8;
        wp->flags |= 0x200;
        wp->flags &= ~2;
    }
    else if (!(wp->flags & 1)) {
        switch (wp->step) {
            case 0:
                memcard_probe();
                break;
            case 1:
                memcard_mount();
                break;
            case 2:
                memcard_check();
                break;
            case 3:
                memcard_open();
                break;
            case 4:
                memcard_read(wp->readBuf, sizeof(CardMetadata), 0);
                break;
            case 5:
                if (!headerCheck()) {
                    wp->flags |= 0x400;
                }
                break;
            case 6:
                memcard_read(wp->readBuf, sizeof(CardSaveSlot), (s32)((wp->readSlot * sizeof(CardSaveSlot)) + sizeof(CardMetadata)));
                break;
            case 7:
                wp->validSave1 = dataCheck();
                save = wp->readBuf;
                wp->lastSaveTime1 = save->data.global.saveTime;
                break;
            case 8:
                memcard_read(wp->readBuf, sizeof(CardSaveSlot), (s32)((wp->readSlot * sizeof(CardSaveSlot)) + sizeof(CardData)));
                break;
            case 9:
                wp->validSave2 = dataCheck();
                save = wp->readBuf;
                wp->lastSaveTime2 = save->data.global.saveTime;
                break;
            case 10:
                if (wp->validSave1 != 0 && wp->validSave2 != 0) {
                    if (!(wp->lastSaveTime1 <= wp->lastSaveTime2)) {
                        saveType = 0;
                    }
                    else {
                        saveType = 1;
                    }
                } else if (wp->validSave1 == 0 && wp->validSave2 != 0) {
                    saveType = 1;
                } else if (wp->validSave1 != 0 && wp->validSave2 == 0) {
                    saveType = 0;
                } else {
                    saveType = -1;
                }
                if (saveType != -1) {
                    memcard_read(&wp->data->saves[wp->readSlot], sizeof(CardSaveSlot), (s32)(sizeof(CardMetadata) + (wp->readSlot * sizeof(CardSaveSlot)) + (saveType * (sizeof(CardSaveSlot) * 4))));
                } else {
                    wp->data->saves[wp->readSlot].data.flags |= 2;
                }
                break;
            case 11:
                memcard_get_serial();
                break;
            case 12:
                memcard_close();
                break;
            case 13:
                memcard_unmount();
                break;
            case 14:
                wp->operation = 0;
                wp->flags |= 0x100;
                wp->flags |= 0x2000;
                wp->flags &= ~2;
                break;
        }
        wp->step++;
    }
    else if (CARDGetResultCode(wp->chan) != CARD_RESULT_BUSY && wp->result != CARDGetResultCode(wp->chan)) {
        wp->result = CARDGetResultCode(wp->chan);
        wp->flags &= ~1;
    }
}

void cardReadAll(void) {
    if (!(wp->flags & 2)) {
        wp->operation = 4;
        wp->readSlot = 0;
        wp->step = 0;
        wp->result = 0;
        wp->flags |= 2;
        wp->flags &= ~0x700;
        wp->flags &= ~0x2000;
    }
}

void read_all_main(void) {
    CardSaveSlot* save;
    s32 saveType; //invalid = -1, main = 0, backup = 1

    if (wp->result) {
        wp->operation = 8;
        wp->flags |= 0x200;
        wp->flags &= ~2;
        return;
    }
    else if (!(wp->flags & 1)) {
        switch (wp->step) {
            case 0:
                memcard_probe();
                break;
            case 1:
                memcard_mount();
                break;
            case 2:
                memcard_check();
                break;
            case 3:
                memcard_open();
                break;
            case 4:
                memcard_read(wp->data, 0x2000, 0);
                break;
            case 5:
                if (!headerCheck2()) {
                    make_header();
                    wp->flags |= 0x400;
                }
                gp->soundSurround = wp->data->metadata.data.soundSurround;
                gp->rumbleEnabled = wp->data->metadata.data.rumbleEnabled;
                gp->lastFileSlot = wp->data->metadata.data.lastFileSlot;
                break;
            case 6:
                memcard_read(wp->readBuf, sizeof(CardSaveSlot), (wp->readSlot << 0xE) + 0x2000);
                break;
            case 7:
                wp->validSave1 = dataCheck();
                save = wp->readBuf;
                wp->lastSaveTime1 = save->data.global.saveTime;
                break;
            case 8:
                memcard_read(wp->readBuf, sizeof(CardSaveSlot), (wp->readSlot << 0xE) + 0x12000);
                break;
            case 9:
                wp->validSave2 = dataCheck();
                save = wp->readBuf;
                wp->lastSaveTime2 = save->data.global.saveTime;
                break;
            case 10:
                if (wp->validSave1 != 0 && wp->validSave2 != 0) {
                    if (!(wp->lastSaveTime1 <= wp->lastSaveTime2)) {
                        saveType = 0;
                    }
                    else {
                        saveType = 1;
                    }
                } else if (wp->validSave1 == 0 && wp->validSave2 != 0) {
                    saveType = 1;
                } else if (wp->validSave1 != 0 && wp->validSave2 == 0) {
                    saveType = 0;
                } else {
                    saveType = -1;
                }
                if (saveType != -1) {
                    memcard_read(&wp->data->saves[wp->readSlot], sizeof(CardSaveSlot), (s32)(sizeof(CardMetadata) + (wp->readSlot * sizeof(CardSaveSlot)) + (saveType * (sizeof(CardSaveSlot) * 4))));
                } else {
                    wp->data->saves[wp->readSlot].data.flags |= 2;
                }
                wp->readSlot++;
                if (wp->readSlot < 4) {
                    wp->step = 3;
                }
                break;
            case 11:
                memcard_get_serial();
                break;
            case 12:
                memcard_close();
                break;
            case 13:
                memcard_unmount();
                break;
            case 14:
                wp->operation = 0;
                wp->flags |= 0x100;
                wp->flags |= 0x2000;
                wp->flags &= ~2;
                break;
        }
        wp->step++;
    }
    else if (CARDGetResultCode(wp->chan) != CARD_RESULT_BUSY && wp->result != CARDGetResultCode(wp->chan)) {
        wp->result = CARDGetResultCode(wp->chan);
        wp->flags &= ~1;
    }
}

void cardCreate(void) {
    if (!(wp->flags & 2)) {
        wp->operation = 5;
        wp->step = 0;
        wp->result = 0;
        wp->flags |= 2;
        wp->flags &= ~0x700;
        wp->flags &= ~0x2000;
        wp->flags &= ~0x800;
    }
}

void create_main(void) {
    if (wp->result) {
        wp->operation = 8;
        wp->flags |= 0x200;
        wp->flags &= ~2;
    }
    else if (!(wp->flags & 1)) {
        switch (wp->step) {
            case 0:
                memcard_probe();
                break;
            case 1:
                memcard_mount();
                break;
            case 2:
                memcard_check();
                break;
            case 3:
                memcard_delete();
                break;
            case 4:
                memcard_create();
                break;
            case 5:
                make_header();
                make_data();
                memcard_write(wp->data, 0x12000, 0);
                break;
            case 6:
                memcard_write(wp->data->saves, 0x10000, 0x12000);
                break;
            case 7:
                memcard_get_stat();
                break;
            case 8:
                CARDSetCommentAddress(&wp->status,  0);
                CARDSetIconAddress(&wp->status, 0x40);
                CARDSetBannerFormat(&wp->status, CARD_STAT_BANNER_RGB5A3);
                CARDSetIconAnim(&wp->status, CARD_STAT_ANIM_LOOP);
                CARDSetIconFormat(&wp->status, 0, CARD_STAT_ICON_C8);
                CARDSetIconSpeed(&wp->status, 0, CARD_STAT_SPEED_MIDDLE);
                CARDSetIconFormat(&wp->status, 1, CARD_STAT_ICON_NONE);
                CARDSetIconSpeed(&wp->status, 1, CARD_STAT_SPEED_END);
                break;
            case 9:
                memcard_set_stat();
                break;
            case 10:
                memcard_get_serial();
                break;
            case 11:
                memcard_close();
                break;
            case 12:
                memcard_unmount();
                break;
            case 13:
                wp->operation = 0;
                wp->flags |= 0x100;
                wp->flags |= 0x2000;
                wp->flags &= ~2;
                break;
        }
        wp->step++;
    }
    else if (CARDGetResultCode(wp->chan) != CARD_RESULT_BUSY && wp->result != CARDGetResultCode(wp->chan)) {
        wp->result = CARDGetResultCode(wp->chan);
        wp->flags &= ~1;
    }
}

void cardFormat(void) {
    if (!(wp->flags & 2)) {
        wp->operation = 6;
        wp->step = 0;
        wp->result = 0;
        wp->flags |= 2;
        wp->flags &= ~0x700;
        wp->flags &= ~0x2000;
    }
}

void format_main(void) {
    if (wp->result != CARD_RESULT_READY) {
        wp->operation = 8;
        wp->flags |= 0x200;
        wp->flags &= ~2;
    }
    else if (!(wp->flags & 1)) {
        switch (wp->step) {
            case 0:
                memcard_probe();
                break;
            
            case 1:
                memcard_mount();
                break;
            
            case 2:
                memcard_format();
                break;
            
            case 3:
                memcard_unmount();
                break;
            
            case 4:
                wp->operation = 0;
                wp->flags |= 0x100;
                wp->flags &= ~2;
                break;
            
        }
        wp->step++;
    }
    else if (CARDGetResultCode(wp->chan) != CARD_RESULT_BUSY && wp->result != CARDGetResultCode(wp->chan)) {
        wp->result = CARDGetResultCode(wp->chan);
        wp->flags &= ~1;
    }
}

void delete_main(void) {
    if (wp->result != CARD_RESULT_READY) {
        wp->operation = 8;
        wp->flags |= 0x200;
        wp->flags &= ~2;
    }
    else if (!(wp->flags & 1)) {
        switch (wp->step) {
            case 0:
                memcard_probe();
                break;
            
            case 1:
                memcard_mount();
                break;
            
            case 2:
                memcard_check();
                break;
            
            case 3:
                memcard_delete();
                break;
            
            case 4:
                memcard_unmount();
                break;
            
            case 5:
                wp->operation = 0;
                wp->flags |= 0x100;
                wp->flags &= ~2;
                break;
            
        }
        wp->step++;
    }
    else if (CARDGetResultCode(wp->chan) != CARD_RESULT_BUSY && wp->result != CARDGetResultCode(wp->chan)) {
        wp->result = CARDGetResultCode(wp->chan);
        wp->flags &= ~1;
    }
}

void check_sum(CardSaveSlot* save) {
    s32 i;
    u8* temp;
    u32 checksum;
    
    strcpy(save->version, version);
    save->size = sizeof(save->data);
    save->checksum1 = (checksum = 0u);
    save->checksum2 = ~0u;
    
    temp = (u8*)&save->data;
    for (i = 0; i < (s32)sizeof(save->data); i++) {
        checksum += temp[i];
    }
    save->checksum1 = checksum;
    save->checksum2 = ~save->checksum1;
}

BOOL headerCheck(void) {
    s32 i;
    s32 checksum;
    u8* temp;
    CardMetadata* metadata;

    metadata = wp->readBuf;
    if (metadata->checksum2 != ~metadata->checksum1) {
        return FALSE;
    } else {
        checksum = 0;
        temp = (u8*)&metadata->data;
        for (i = 0; i < (s32)sizeof(metadata->data); i++) {
            checksum += temp[i];
        }
        if (metadata->checksum1 != checksum) {
            return FALSE;
        } else {
            return TRUE;
        }
    }
}

BOOL headerCheck2(void) {
    s32 i;
    s32 checksum;
    u8* temp;
    CardMetadata* metadata;

    metadata = &wp->data->metadata;
    if (metadata->checksum2 != ~metadata->checksum1) {
        return FALSE;
    } else {
        checksum = 0;
        temp = (u8*)&metadata->data;
        for (i = 0; i < (s32)sizeof(metadata->data); i++) {
            checksum += temp[i];
        }
        if (metadata->checksum1 != checksum) {
            return FALSE;
        } else {
            return TRUE;
        }
    }
}

BOOL dataCheck(void) {
    s32 i;
    u32 checksum;
    u8* temp;
    CardSaveSlot* save;

    save = wp->readBuf;
    if (save->size != sizeof(save->data)) {
        return FALSE;
    } else if (strcmp(save->version, version)) {
        return FALSE;
    } else if (save->checksum2 != ~save->checksum1) {
        return FALSE;
    } else {
        checksum = 0;
        temp = (u8*)&save->data;
        for (i = 0; i < (s32)sizeof(save->data); i++) {
            checksum += temp[i];
        }
        if (save->checksum1 != checksum) {
            return FALSE;
        } else {
            return TRUE;
        }
    }
}

void make_header(void) {
	CardMetadata* metadata;
	s32 i;
	u8* temp;
	u32 checksum;
	OSCalendarTime td;

	metadata = &cardGetFilePtr()->metadata;
	memset(metadata, 0, sizeof(metadata->data));
	strcpy(metadata->data.fileName, "ペーパーマリオＲＰＧ");
	OSTicksToCalendarTime(OSGetTime(), &td);
	sprintf(metadata->data.dateString, "%2d月%2d日のセーブデータです", td.mon + 1, td.mday);
	memcpy(metadata->data.bannerTex, _mariost_banner_tex, sizeof(metadata->data.bannerTex));
	memcpy(metadata->data.iconTex, _mariost_icon_tex, sizeof(metadata->data.iconTex));
	memcpy(metadata->data.iconTlut, _mariost_icon_tlut, sizeof(metadata->data.iconTlut));
	metadata->data.rumbleEnabled = (gp->rumbleEnabled != 0);
	metadata->data.soundSurround = (u16)gp->soundSurround;
	metadata->data.lastFileSlot = (u16)gp->lastFileSlot;
	metadata->checksum1 = (checksum = 0u);
	metadata->checksum2 = ~0u;

	temp = (u8*)&metadata->data;
	for (i = 0; i < (s32)sizeof(metadata->data); i++) {
		checksum += temp[i];
	}
	metadata->checksum1 = checksum;
	metadata->checksum2 = ~metadata->checksum1;
}

void make_data(void) {
    CardSaveSlot* save;
    s32 i;
    
    for (i = 0; i < 4; i++) {
        if (i != -1) {
            save = &cardGetFilePtr()->saves[i];
        }
        else {
            save = wp->writeBuf;
        }
        memset(save, 0, sizeof(save->data));
        save->data.flags |= 1;
        check_sum(save);
    }
}

void memcard_probe(void) {
    s32 memSize, sectorSize;
    s32 result;
    s32 i;
    s32 timeout;
    
    wp->flags |= 1;

#ifdef MUST_MATCH
    // The retail compiler materializes `timeout` in the loop header, before `i = 0`.
    // This dead test reproduces that schedule; the natural form is under #else.
    timeout = 1000000;
    i = 0;
    if (timeout <= 0) {
        result = CARD_RESULT_FATAL_ERROR;
    }
#else
    i = 0;
    timeout = 1000000;
#endif
    while ((result = CARDProbeEx(0, &memSize, &sectorSize)) == CARD_RESULT_BUSY) {
        if (++i > timeout) {
            result = CARD_RESULT_FATAL_ERROR;
            break;
        }
    }

    if (result == CARD_RESULT_READY && sectorSize != 0x2000) {
        result = CARD_RESULT_WRONGDEVICE;
    }
    if (result == CARD_RESULT_READY) {
        wp->result = CARD_RESULT_READY;
        wp->chan = 0;
    }
    else if (result == CARD_RESULT_WRONGDEVICE) {
        wp->result = CARD_RESULT_WRONGDEVICE;
        wp->chan = 0;
    }
    else {
        wp->result = result;
        wp->chan = -1;
    }
    wp->flags &= ~1;
}

void mountDetachCallback(s32 chan, s32 result) {
	; //stubbed in retail
}

void mountAttachCallback(s32 chan, s32 result) {
    wp->result = result;
    if (result == CARD_RESULT_BROKEN || result == CARD_RESULT_ENCODING) {
        wp->result = CARD_RESULT_READY;
    }
    wp->flags &= ~1;
}

void memcard_mount(void) {
    CARDSetFastMode(TRUE);
    wp->flags |= 1;
    CARDMountAsync(wp->chan, wp->workArea, mountDetachCallback, mountAttachCallback);
}

void memcard_unmount(void) {
    s32 result;
    s32 i;
    
    wp->flags |= 1;
    i = 0;
    while ((result = CARDUnmount(wp->chan)) == CARD_RESULT_BUSY) {
        if (++i > 1000000) {
            result = CARD_RESULT_FATAL_ERROR;
            break;
        }
    }
    wp->result = result;
    wp->flags &= ~1;
}

void checkCallback(s32 chan, s32 result) {
	wp->result = result;
	wp->flags &= ~1;
}

void memcard_check(void) {
    wp->flags |= 1;
    CARDCheckAsync(wp->chan, checkCallback);
}

void formatCallback(s32 chan, s32 result) {
	wp->result = result;
	wp->flags &= ~1;
}

static void memcard_format(void) {
    wp->flags |= 1;
    CARDFormatAsync(wp->chan, formatCallback);
}

static void memcard_delete(void) {
    s32 result;
    s32 i;
    
    wp->flags |= 1;
    
    i = 0;
    while ((result = CARDDelete(wp->chan, "mariost_save_file")) == CARD_RESULT_BUSY) {
        if (++i > 1000000) {
            result = CARD_RESULT_FATAL_ERROR;
            break;
        }
    }
    
    if (result == CARD_RESULT_NOFILE) {
        result = CARD_RESULT_READY;
    }
    wp->result = result;
    wp->flags &= ~1;
}

void createCallback(s32 chan, s32 result) {
	wp->result = result;
	wp->flags &= ~1;
}

static void memcard_create(void) {
    wp->flags |= 1;
    CARDCreateAsync(wp->chan, "mariost_save_file", sizeof(CardData) + (sizeof(CardSaveSlot) * 4), &wp->info, createCallback);
}

void writeCallback(s32 chan, s32 result) {
	wp->result = result;
	wp->flags &= ~1;
}

static void memcard_write(void* addr, s32 length, s32 offset) {
    wp->flags |= 1;
    CARDWriteAsync(&wp->info, addr, length, offset, writeCallback);
}

void readCallback(s32 chan, s32 result) {
	wp->result = result;
	wp->flags &= ~1;
}

void memcard_read(void* addr, s32 length, s32 offset) {
    wp->flags |= 1;
    CARDReadAsync(&wp->info, addr, length, offset, readCallback);
}

void statusCallback(s32 chan, s32 result) {
	wp->result = result;
	wp->flags &= ~1;
}

void memcard_set_stat(void) {
    wp->flags |= 1;
    CARDSetStatusAsync(wp->chan, wp->info.fileNo, &wp->status, statusCallback);
}

void memcard_get_stat(void) {
    s32 result;
    s32 i;
    
    wp->flags |= 1;
    
    i = 0;
    while ((result = CARDGetStatus(wp->chan, wp->info.fileNo, &wp->status)) == CARD_RESULT_BUSY) {
        if (++i > 1000000) {
            result = CARD_RESULT_FATAL_ERROR;
            break;
        }
    }
    wp->result = result;
    wp->flags &= ~1;
}

void memcard_get_serial(void) {
    s32 result;
    s32 i;

    wp->flags |= 1;

    i = 0;
    while ((result = CARDGetSerialNo(wp->chan, &wp->serialNo)) == CARD_RESULT_BUSY) {
        if (++i > 1000000) {
            result = CARD_RESULT_FATAL_ERROR;
            break;
        }
    }

    wp->result = result;
    wp->flags &= ~1;
}

void memcard_open(void) {
    CARDStat stat;
    u64 serialNo;
    s32 byteNotUsed;
    s32 filesNotUsed;
    s32 i;
    s32 fileNo;
    s32 result;

    serialNo = 0;
    wp->flags |= 1;
    if (wp->flags & 0x2000) {
        i = 0;
        while ((result = CARDGetSerialNo(wp->chan, &serialNo)) == -1) {
            if (++i > 1000000) {
                result = CARD_RESULT_FATAL_ERROR;
                break;
            }
        }
        if (result != 0) {
            wp->result = result;
            wp->flags &= ~1;
            return;
        }
        if (serialNo != wp->serialNo) {
            wp->result = CARD_RESULT_NOPERM;
            wp->flags &= ~1;
            return;
        }
    }
    for (fileNo = 0; fileNo < 127; fileNo++) {
        i = 0;
        while ((result = CARDGetStatus(wp->chan, fileNo, &stat)) == CARD_RESULT_BUSY) {
            if (++i > 1000000) {
                result = CARD_RESULT_FATAL_ERROR;
                break;
            }
        }
        if ((result != CARD_RESULT_NOFILE) && (result != CARD_RESULT_NOPERM)) {
            if (result == CARD_RESULT_READY) {
                if (!strcmp(stat.fileName, "mariost_save_file")) {
                    if ((stat.commentAddr == 0) && (stat.iconAddr == 0x40) && (stat.length == 0x22000)) {
                        wp->fileNo = fileNo;
                        break;
                    }
                    else {
                        wp->fileNo = fileNo;
                        wp->result = CARD_RESULT_NOFILE;
                        wp->flags &= ~1;
                        return;
                    }
                }
            }
            else {
                if ((result == CARD_RESULT_FATAL_ERROR) || (result == CARD_RESULT_NOCARD)) {
                    wp->result = result;
                    wp->flags &= ~1;
                    return;
                }
            }
        }
    }
    if (fileNo >= 127) {
        i = 0;
        while ((result = CARDFreeBlocks(wp->chan, &byteNotUsed, &filesNotUsed)) == CARD_RESULT_BUSY) {
            if (++i > 1000000) {
                result = CARD_RESULT_FATAL_ERROR;
                break;
            }
        }
        if (result != 0) {
            wp->result = result;
            wp->flags &= ~1;
            return;
        }
        if (byteNotUsed < 0x22000) {
            wp->result = CARD_RESULT_INSSPACE;
            wp->flags &= ~1;
            return;
        }
        if (filesNotUsed < 1) {
            wp->result = CARD_RESULT_NOENT;
            wp->flags &= ~1;
            return;
        }
        wp->result = -4;
        wp->flags &= ~1;
        return;
    }
    i = 0;
    while ((result = CARDOpen(wp->chan, "mariost_save_file", &wp->info)) == CARD_RESULT_BUSY) {
        if (++i > 1000000) {
            result = CARD_RESULT_FATAL_ERROR;
            break;
        }
    }
    wp->result = result;
    wp->flags &= ~1;
}

void memcard_close(void) {
    s32 result;
    s32 i;

    wp->flags |= 1;

    i = 0;
    while ((result = CARDClose(&wp->info)) == CARD_RESULT_BUSY) {
        if (++i > 1000000) {
            result = CARD_RESULT_FATAL_ERROR;
            break;
        }
    }

    wp->result = result;
    wp->flags &= ~1;
}
