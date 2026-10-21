#include "evt/evt_cmd.h"
#include "evt/evt_johoya.h"
#include "evt/evt_uranaisi.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "driver/msgdrv.h"
#include "mario/mario_pouch.h"
#include "memory.h"
#include <string.h>
#include <stdio.h>


// Local prototypes not yet in a shared header.

// Local structs not yet in a shared header.
typedef struct UranaiInfo {
    s16 unk0;    // 0x0, never read or written by this file
    s16 labelId; // 0x2
    s16 storyId; // 0x4
} UranaiInfo;

typedef struct UranaiWork {
    UranaiInfo* info;      // 0x0
    s32 count;             // 0x4
    const char* prefix;    // 0x8
    char* label;           // 0xC, "%s_%04d" buffer at the end of the info block
    s32 index;             // 0x10, entry picked by the last makelabel
    s32 mode;              // 0x14, 0=next, 1=starpiece, 2=supercoin
    u8 starpieceFlags[0xD]; // 0x18, per-storyId read flags
    u8 supercoinFlags[0xD]; // 0x25, per-storyId read flags
    u8 pad_0x32[2];        // 0x32
    s32 lastLabelId;       // 0x34, evt label id of the newest story batch
} UranaiWork;

typedef struct UranaiTableEntry {
    const char* name; // 0x0
    s32 value;        // 0x4
} UranaiTableEntry;

UranaiWork _udt = {0};
const char zero_80420460[4] = "";
static const u8 gap_09_80420464_sdata2[4] = {0};

UranaiTableEntry uranai_table_starpiece[] = {
	{"GSWF_STARPIECE_00", (s32)0xF840713Fu},
	{"GSWF_STARPIECE_01", (s32)0xF8407140u},
	{"GSWF_STARPIECE_02", (s32)0xF8407141u},
	{"GSWF_STARPIECE_03", (s32)0xF8407142u},
	{"GSWF_STARPIECE_04", (s32)0xF8407143u},
	{"GSWF_STARPIECE_05", (s32)0xF8407144u},
	{"GSWF_STARPIECE_06", (s32)0xF8407145u},
	{"GSWF_STARPIECE_07", (s32)0xF8407146u},
	{"GSWF_STARPIECE_08", (s32)0xF8407147u},
	{"GSWF_STARPIECE_09", (s32)0xF8407148u},
	{"GSWF_STARPIECE_10", (s32)0xF8407149u},
	{"GSWF_STARPIECE_11", (s32)0xF840714Au},
	{"GSWF_STARPIECE_12", (s32)0xF840714Bu},
	{"GSWF_STARPIECE_13", (s32)0xF840714Cu},
	{"GSWF_STARPIECE_14", (s32)0xF840714Du},
	{"GSWF_STARPIECE_15", (s32)0xF840714Eu},
	{"GSWF_STARPIECE_16", (s32)0xF840714Fu},
	{"GSWF_STARPIECE_17", (s32)0xF8407150u},
	{"GSWF_STARPIECE_18", (s32)0xF8407151u},
	{"GSWF_STARPIECE_19", (s32)0xF8407152u},
	{"GSWF_STARPIECE_20", (s32)0xF8407153u},
	{"GSWF_STARPIECE_21", (s32)0xF8407154u},
	{"GSWF_STARPIECE_22", (s32)0xF8407155u},
	{"GSWF_STARPIECE_23", (s32)0xF8407156u},
	{"GSWF_STARPIECE_24", (s32)0xF8407157u},
	{"GSWF_STARPIECE_25", (s32)0xF8407158u},
	{"GSWF_STARPIECE_26", (s32)0xF8407159u},
	{"GSWF_STARPIECE_27", (s32)0xF840715Au},
	{"GSWF_STARPIECE_28", (s32)0xF840715Bu},
	{"GSWF_STARPIECE_29", (s32)0xF840715Cu},
	{"GSWF_STARPIECE_30", (s32)0xF840715Du},
	{"GSWF_STARPIECE_31", (s32)0xF840715Eu},
	{"GSWF_STARPIECE_32", (s32)0xF840715Fu},
	{"GSWF_STARPIECE_33", (s32)0xF8407160u},
	{"GSWF_STARPIECE_34", (s32)0xF8407161u},
	{"GSWF_STARPIECE_35", (s32)0xF8407162u},
	{"GSWF_STARPIECE_36", (s32)0xF8407163u},
	{"GSWF_STARPIECE_37", (s32)0xF8407164u},
	{"GSWF_STARPIECE_38", (s32)0xF8407165u},
	{"GSWF_STARPIECE_39", (s32)0xF8407166u},
	{"GSWF_STARPIECE_40", (s32)0xF8407167u},
	{"GSWF_STARPIECE_41", (s32)0xF8407168u},
	{"GSWF_STARPIECE_42", (s32)0xF8407169u},
	{"GSWF_STARPIECE_43", (s32)0xF840716Au},
	{"GSWF_STARPIECE_44", (s32)0xF840716Bu},
	{"GSWF_STARPIECE_45", (s32)0xF840716Cu},
	{"GSWF_STARPIECE_46", (s32)0xF840716Du},
	{"GSWF_STARPIECE_47", (s32)0xF840716Eu},
	{"GSWF_STARPIECE_48", (s32)0xF840716Fu},
	{"GSWF_STARPIECE_49", (s32)0xF8407170u},
	{"GSWF_STARPIECE_50", (s32)0xF8407171u},
	{"GSWF_STARPIECE_51", (s32)0xF8407172u},
	{"GSWF_STARPIECE_52", (s32)0xF8407173u},
	{"GSWF_STARPIECE_53", (s32)0xF8407174u},
	{"GSWF_STARPIECE_54", (s32)0xF8407175u},
	{"GSWF_STARPIECE_55", (s32)0xF8407176u},
	{"GSWF_STARPIECE_56", (s32)0xF8407177u},
	{"GSWF_STARPIECE_57", (s32)0xF8407178u},
	{"GSWF_STARPIECE_58", (s32)0xF8407179u},
	{"GSWF_STARPIECE_59", (s32)0xF840717Au},
	{"GSWF_STARPIECE_60", (s32)0xF840717Bu},
	{"GSWF_STARPIECE_61", (s32)0xF840717Cu},
	{"GSWF_STARPIECE_62", (s32)0xF840717Du},
	{"GSWF_STARPIECE_63", (s32)0xF840717Eu},
	{"GSWF_STARPIECE_64", (s32)0xF840717Fu},
	{"GSWF_STARPIECE_65", (s32)0xF8407180u},
	{"GSWF_STARPIECE_66", (s32)0xF8407181u},
	{"GSWF_STARPIECE_67", (s32)0xF8407182u},
	{"GSWF_STARPIECE_68", (s32)0xF8407183u},
	{"GSWF_STARPIECE_69", (s32)0xF8407184u},
	{"GSWF_STARPIECE_70", (s32)0xF8407185u},
	{"GSWF_STARPIECE_71", (s32)0xF8407186u},
	{"GSWF_STARPIECE_72", (s32)0xF8407187u},
	{"GSWF_STARPIECE_73", (s32)0xF8407188u},
	{"GSWF_STARPIECE_74", (s32)0xF8407189u},
	{"GSWF_STARPIECE_75", (s32)0xF840718Au},
	{"GSWF_STARPIECE_76", (s32)0xF840718Bu},
	{"GSWF_STARPIECE_77", (s32)0xF840718Cu},
	{"GSWF_STARPIECE_78", (s32)0xF840718Du},
	{"GSWF_STARPIECE_79", (s32)0xF840718Eu},
	{"GSWF_STARPIECE_80", (s32)0xF840718Fu},
	{"GSWF_STARPIECE_81", (s32)0xF8407190u},
	{"GSWF_STARPIECE_82", (s32)0xF8407191u},
	{"GSWF_STARPIECE_83", (s32)0xF8407192u},
	{"GSWF_STARPIECE_84", (s32)0xF8407193u},
	{"GSWF_STARPIECE_85", (s32)0xF8407194u},
	{"GSWF_STARPIECE_86", (s32)0xF8407195u},
	{"GSWF_STARPIECE_87", (s32)0xF8407196u},
	{"GSWF_STARPIECE_88", (s32)0xF8407197u},
	{"GSWF_STARPIECE_89", (s32)0xF8407198u},
	{"GSWF_STARPIECE_90", (s32)0xF8407199u},
	{"GSWF_STARPIECE_91", (s32)0xF840719Au},
	{"GSWF_STARPIECE_92", (s32)0xF840719Bu},
	{"GSWF_STARPIECE_93", (s32)0xF840719Cu},
	{"GSWF_STARPIECE_94", (s32)0xF840719Du},
	{"GSWF_STARPIECE_95", (s32)0xF840719Eu},
	{"GSWF_STARPIECE_96", (s32)0xF840719Fu},
	{"GSWF_STARPIECE_97", (s32)0xF84071A0u},
	{"GSWF_STARPIECE_98", (s32)0xF84071A1u},
	{"GSWF_STARPIECE_99", (s32)0xF84071A2u},
	{NULL, 0},
};

UranaiTableEntry uranai_table_supercoin[] = {
	{"GSWF_SHINE_00", (s32)0xF8407115u},
	{"GSWF_SHINE_01", (s32)0xF8407116u},
	{"GSWF_SHINE_02", (s32)0xF8407117u},
	{"GSWF_SHINE_03", (s32)0xF8407118u},
	{"GSWF_SHINE_04", (s32)0xF8407119u},
	{"GSWF_SHINE_05", (s32)0xF840711Au},
	{"GSWF_SHINE_06", (s32)0xF840711Bu},
	{"GSWF_SHINE_07", (s32)0xF840711Cu},
	{"GSWF_SHINE_08", (s32)0xF840711Du},
	{"GSWF_SHINE_09", (s32)0xF840711Eu},
	{"GSWF_SHINE_10", (s32)0xF840711Fu},
	{"GSWF_SHINE_11", (s32)0xF8407120u},
	{"GSWF_SHINE_12", (s32)0xF8407121u},
	{"GSWF_SHINE_13", (s32)0xF8407122u},
	{"GSWF_SHINE_14", (s32)0xF8407123u},
	{"GSWF_SHINE_15", (s32)0xF8407124u},
	{"GSWF_SHINE_16", (s32)0xF8407125u},
	{"GSWF_SHINE_17", (s32)0xF8407126u},
	{"GSWF_SHINE_18", (s32)0xF8407127u},
	{"GSWF_SHINE_19", (s32)0xF8407128u},
	{"GSWF_SHINE_20", (s32)0xF8407129u},
	{"GSWF_SHINE_21", (s32)0xF840712Au},
	{"GSWF_SHINE_22", (s32)0xF840712Bu},
	{"GSWF_SHINE_23", (s32)0xF840712Cu},
	{"GSWF_SHINE_24", (s32)0xF840712Du},
	{"GSWF_SHINE_25", (s32)0xF840712Eu},
	{"GSWF_SHINE_26", (s32)0xF840712Fu},
	{"GSWF_SHINE_27", (s32)0xF8407130u},
	{"GSWF_SHINE_28", (s32)0xF8407131u},
	{"GSWF_SHINE_29", (s32)0xF8407132u},
	{"GSWF_SHINE_30", (s32)0xF8407133u},
	{"GSWF_SHINE_31", (s32)0xF8407134u},
	{"GSWF_SHINE_32", (s32)0xF8407135u},
	{"GSWF_SHINE_33", (s32)0xF8407136u},
	{"GSWF_SHINE_34", (s32)0xF8407137u},
	{"GSWF_SHINE_35", (s32)0xF8407138u},
	{"GSWF_SHINE_36", (s32)0xF8407139u},
	{"GSWF_SHINE_37", (s32)0xF840713Au},
	{"GSWF_SHINE_38", (s32)0xF840713Bu},
	{"GSWF_SHINE_39", (s32)0xF840713Cu},
	{"GSWF_SHINE_40", (s32)0xF840713Du},
	{"GSWF_SHINE_41", (s32)0xF840713Eu},
	{NULL, 0},
};

s32 uranaisi_ryokin_tbl[3] = {5, 5, 10};

USER_FUNC(uranaisi_data_alloc) {
    UranaiWork* wp = &_udt;
    s32* args = event->args;
    s32 count;
    const char* prefix;

    _udt.mode = evtGetValue(event, args[0]);
    _udt.prefix = (const char*)evtGetValue(event, args[1]);
    switch (_udt.mode) {
        case 0:
            count = uranaisi_data_make_next(_udt.prefix, 0);
            break;
        case 1:
            count = uranaisi_data_make_starpiece(_udt.prefix, 0);
            break;
        case 2:
            count = uranaisi_data_make_supercoin(_udt.prefix, 0);
            break;
        default:
            count = 0;
            break;
    }
    wp->count = count;
    wp->info = _mapAlloc(wp->count * sizeof(UranaiInfo) + 0x40);
    memset(wp->info, 0, wp->count * sizeof(UranaiInfo) + 0x40);
    wp->label = (char*)&wp->info[wp->count];
    prefix = wp->prefix;
    switch (wp->mode) {
        case 0:
            uranaisi_data_make_next(prefix, 1);
            break;
        case 1:
            uranaisi_data_make_starpiece(prefix, 1);
            break;
        case 2:
            uranaisi_data_make_supercoin(prefix, 1);
            break;
    }
    return 2;
}

USER_FUNC(uranaisi_data_free) {
    UranaiWork* wp = &_udt;

    if (wp->info == NULL) {
        return 2;
    }
    _mapFree(wp->info);
    wp->info = NULL;
    wp->count = 0;
    wp->prefix = NULL;
    wp->label = NULL;
    wp->index = 0;
    return 2;
}

USER_FUNC(uranaisi_table_clear) {
    UranaiWork* wp = &_udt;

    memset(wp->starpieceFlags, 0, 0xD);
    memset(wp->supercoinFlags, 0, 6);
    return 2;
}

USER_FUNC(uranaisi_next_check) {
    UranaiWork* wp = &_udt;

    evtSetValue(event, event->args[0], wp->count);
    return 2;
}

USER_FUNC(uranaisi_next_makelabel) {
    UranaiWork* wp = &_udt;
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);

    sprintf(wp->label, "%s_%04d", wp->prefix, wp->info[idx].storyId);
    evtSetValue(event, args[1], (s32)wp->label);
    return 2;
}

USER_FUNC(uranaisi_starpiece_check) {
    UranaiWork* wp = &_udt;
    s32* args = event->args;
    s32 i;
    s32 count;

    count = 0;
    i = 0;
    while (i < wp->count) {
        if (johoya_get(wp->starpieceFlags, wp->info[i].storyId) == 0) {
            count++;
        }
        i++;
    }
    evtSetValue(event, args[0], count);
    return 2;
}

USER_FUNC(uranaisi_starpiece_makelabel) {
    s32* args = event->args;
    s32 arg = evtGetValue(event, args[0]);
    UranaiWork* wp = &_udt;
    s32 i;
    s32 unread;

    unread = 0;
    i = 0;
    while (i < wp->count) {
        if (johoya_get(wp->starpieceFlags, wp->info[i].storyId) == 0) {
            if (arg == unread) {
                sprintf(wp->label, "%s_%04d", wp->prefix, wp->info[i].storyId);
                evtSetValue(event, args[1], (s32)wp->label);
                wp->index = i;
                return 2;
            }
            unread++;
        }
        i++;
    }
    wp->label[0] = 0;
    evtSetValue(event, args[1], (s32)wp->label);
    return 2;
}

USER_FUNC(uranaisi_starpiece_setreadflag) {
    UranaiWork* wp = &_udt;

    johoya_set(wp->starpieceFlags, wp->info[wp->index].storyId, 1);
    return 2;
}

USER_FUNC(uranaisi_supercoin_check) {
    UranaiWork* wp = &_udt;
    s32* args = event->args;
    s32 i;
    s32 count;

    count = 0;
    i = 0;
    while (i < wp->count) {
        if (johoya_get(wp->supercoinFlags, wp->info[i].storyId) == 0) {
            count++;
        }
        i++;
    }
    evtSetValue(event, args[0], count);
    return 2;
}

USER_FUNC(uranaisi_supercoin_makelabel) {
    s32* args = event->args;
    s32 arg = evtGetValue(event, args[0]);
    UranaiWork* wp = &_udt;
    s32 i;
    s32 unread;

    unread = 0;
    i = 0;
    while (i < wp->count) {
        if (johoya_get(wp->supercoinFlags, wp->info[i].storyId) == 0) {
            if (arg == unread) {
                sprintf(wp->label, "%s_%04d", wp->prefix, wp->info[i].storyId);
                evtSetValue(event, args[1], (s32)wp->label);
                wp->index = i;
                return 2;
            }
            unread++;
        }
        i++;
    }
    wp->label[0] = 0;
    evtSetValue(event, args[1], (s32)wp->label);
    return 2;
}

USER_FUNC(uranaisi_supercoin_setreadflag) {
    UranaiWork* wp = &_udt;

    johoya_set(wp->supercoinFlags, wp->info[wp->index].storyId, 1);
    return 2;
}

s32 uranaisi_data_make_next(const char* prefix, s32 type) {
    UranaiWork* wp = &_udt;
    char label[64];
    char tag[64];
    // Declaration order below is the original's: Metrowerks gives the
    // later-declared locals the lower callee-saved registers.
    s32 storyId;
    s32 mbbpos;
    s32 tagNum;
    BOOL tagdone;
    const s8* tagdata;
    s32 count;
    s32 maxLabel;
    const s8* msg;
    s32 labelId;

    maxLabel = evtGetValue(0, -170000000);
    if (type & 1) {
        if (wp->lastLabelId == -1) {
            return 0;
        }
    } else {
        wp->lastLabelId = -1;
    }

    count = 0;
    for (storyId = 0;; storyId++) {
        sprintf(label, "%s_%04d", prefix, storyId);
        msg = (const s8*)msgSearch(label);
        if (msg[0] != '<') {
            break;
        }
        tagdata = msg + 2;
        if (msg[1] != '!') {
            break;
        }
        tagNum = 0;
        mbbpos = 0;
        tagdone = FALSE;
        while (!tagdone) {
            if (_ismbblead(*tagdata)) {
                if (tagNum == 0) {
                    tag[mbbpos++] = tagdata[0];
                    tag[mbbpos++] = tagdata[1];
                    tagdata += 2;
                } else {
                    mbbpos += 2;
                    tagdata += 2;
                }
            } else {
                switch (tagdata[0]) {
                    case '>':
                        tagdone = TRUE;
                        // fallthrough
                    case ',':
                        tagdata++;
                        if (tagNum == 0) {
                            tag[mbbpos] = '\0';
                        }
                        mbbpos = 0;
                        tagNum++;
                        break;
                    default:
                        if (tagNum == 0) {
                            tag[mbbpos++] = *tagdata++;
                        } else {
                            mbbpos++;
                            tagdata++;
                        }
                        break;
                }
            }
        }

        if (!strcmp(tag, "enddata")) {
            break;
        }
        if (strcmp(tag, zero_80420460)) {
            labelId = search_evt_no(tag);
            if (labelId != -1) {
                if (type & 1) {
                    if (labelId != wp->lastLabelId) {
                        continue;
                    }
                    wp->info[count].labelId = (s16)labelId;
                    wp->info[count].storyId = (s16)storyId;
                } else {
                    if (labelId > maxLabel) {
                        continue;
                    }
                    if (labelId < wp->lastLabelId) {
                        continue;
                    }
                    if (labelId > wp->lastLabelId) {
                        wp->lastLabelId = labelId;
                        count = 0;
                    }
                }
                count++;
            }
        }
    }
    return count;
}

// The GSWF flag the table maps the name to, or -1 when the name is not in it.
static inline s32 uranai_table_search(UranaiTableEntry* table, const char* name) {
    s32 i;
    for (i = 0; table[i].name != NULL; i++) {
        if (!strcmp(name, table[i].name)) {
            return table[i].value;
        }
    }
    return -1;
}

// opt_lifetimes off: one variable per local for the whole function, so `tmp` (the end-of-tag
// flag, then the byte saved under the terminator) keeps the original's register.
#pragma opt_lifetimes off
s32 uranaisi_data_make_starpiece(const char* prefix, s32 type) {
    s32 mbbpos;
    char label[64];
    char tag[64];
    s32 tagNum;
    UranaiWork* wp = &_udt;
    const s8* msg;
    BOOL typeFlag;
    s32 storyId;
    char* initEndPos;
    s32 labelId;
    s16 tagEndPos;
    s32 gswf;
    const s8* tagdata;
    s32 maxLabel;
    s32 count;
    s32 tmp;

    maxLabel = evtGetValue(NULL, -170000000);
    typeFlag = type & 1;
    count = 0;
    storyId = 0;
    while (TRUE) {
        sprintf(label, "%s_%04d", prefix, storyId);
        msg = (const s8*)msgSearch(label);
        if (msg[0] != '<') {
            break;
        }
        tagdata = msg + 2;
        if (msg[1] != '!') {
            break;
        }

        tagNum = 0;
        mbbpos = 0;
        tmp = FALSE;
        while (!tmp) {
            if (_ismbblead(*tagdata)) {
                if (tagNum == 0) {
                    tag[mbbpos++] = tagdata[0];
                    tag[mbbpos++] = tagdata[1];
                    tagdata += 2;
                } else {
                    mbbpos += 2;
                    tagdata += 2;
                }
            } else {
                switch (tagdata[0]) {
                    case '>':
                        tmp = TRUE;
                        // fallthrough
                    case ',':
                        tagdata++;
                        if (tagNum == 0) {
                            tag[mbbpos] = '\0';
                            initEndPos = (char*)tagdata;
                        } else {
                            tagEndPos = (s16)mbbpos;
                        }
                        mbbpos = 0;
                        tagNum++;
                        break;
                    default:
                        if (tagNum == 0) {
                            tag[mbbpos++] = *tagdata++;
                        } else {
                            mbbpos++;
                            tagdata++;
                        }
                        break;
                }
            }
        }

        if (!strcmp(tag, "enddata")) {
            break;
        }
        if (strcmp(tag, zero_80420460)) {
            labelId = search_evt_no(tag);
            if (labelId != -1 && labelId <= maxLabel) {
                tmp = initEndPos[tagEndPos];
                initEndPos[tagEndPos] = '\0';
                gswf = uranai_table_search(uranai_table_starpiece, initEndPos);
                initEndPos[tagEndPos] = tmp;
                if (gswf != -1 && evtGetValue(NULL, gswf) == 0) {
                    if (typeFlag) {
                        wp->info[count].labelId = (s16)labelId;
                        wp->info[count].storyId = (s16)storyId;
                    }
                    count++;
                }
            }
        }
        storyId++;
    }
    return count;
}

// opt_lifetimes off: see uranaisi_data_make_starpiece.
s32 uranaisi_data_make_supercoin(const char* prefix, s32 type) {
    s32 mbbpos;
    char label[64];
    char tag[64];
    s32 tagNum;
    UranaiWork* wp = &_udt;
    const s8* msg;
    BOOL typeFlag;
    s32 storyId;
    char* initEndPos;
    s32 labelId;
    s16 tagEndPos;
    s32 gswf;
    const s8* tagdata;
    s32 maxLabel;
    s32 count;
    s32 tmp;

    maxLabel = evtGetValue(NULL, -170000000);
    typeFlag = type & 1;
    count = 0;
    storyId = 0;
    while (TRUE) {
        sprintf(label, "%s_%04d", prefix, storyId);
        msg = (const s8*)msgSearch(label);
        if (msg[0] != '<') {
            break;
        }
        tagdata = msg + 2;
        if (msg[1] != '!') {
            break;
        }

        tagNum = 0;
        mbbpos = 0;
        tmp = FALSE;
        while (!tmp) {
            if (_ismbblead(*tagdata)) {
                if (tagNum == 0) {
                    tag[mbbpos++] = tagdata[0];
                    tag[mbbpos++] = tagdata[1];
                    tagdata += 2;
                } else {
                    mbbpos += 2;
                    tagdata += 2;
                }
            } else {
                switch (tagdata[0]) {
                    case '>':
                        tmp = TRUE;
                        // fallthrough
                    case ',':
                        tagdata++;
                        if (tagNum == 0) {
                            tag[mbbpos] = '\0';
                            initEndPos = (char*)tagdata;
                        } else {
                            tagEndPos = (s16)mbbpos;
                        }
                        mbbpos = 0;
                        tagNum++;
                        break;
                    default:
                        if (tagNum == 0) {
                            tag[mbbpos++] = *tagdata++;
                        } else {
                            mbbpos++;
                            tagdata++;
                        }
                        break;
                }
            }
        }

        if (!strcmp(tag, "enddata")) {
            break;
        }
        if (strcmp(tag, zero_80420460)) {
            labelId = search_evt_no(tag);
            if (labelId != -1 && labelId <= maxLabel) {
                tmp = initEndPos[tagEndPos];
                initEndPos[tagEndPos] = '\0';
                gswf = uranai_table_search(uranai_table_supercoin, initEndPos);
                initEndPos[tagEndPos] = tmp;
                if (gswf != -1 && evtGetValue(NULL, gswf) == 0) {
                    if (typeFlag) {
                        wp->info[count].labelId = (s16)labelId;
                        wp->info[count].storyId = (s16)storyId;
                    }
                    count++;
                }
            }
        }
        storyId++;
    }
    return count;
}

#pragma opt_lifetimes reset

s32 uranaisi_ryokin_rtn(s32 idx) {
    if (idx != 0) {
        return uranaisi_ryokin_tbl[idx];
    }
    return pouchGetPtr()->level * 2;
}

USER_FUNC(uranaisi_ryokin) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    s32 value;

    if (idx != 0) {
        value = uranaisi_ryokin_tbl[idx];
    } else {
        value = pouchGetPtr()->level * 2;
    }
    evtSetValue(event, args[1], value);
    return 2;
}

