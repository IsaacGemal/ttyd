#include "evt/evt_cmd.h"
#include "evt/evt_johoya.h"
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
    s16 unk0;    // 0x0
    s16 labelId; // 0x2
    s16 unk4;    // 0x4
} UranaiInfo;

typedef struct UranaiWork {
    UranaiInfo* info;   // 0x0
    s32 count;          // 0x4
    const char* prefix; // 0x8
    char* label;        // 0xC
    s32 index;          // 0x10
    s32 type;           // 0x14
    u8 starpiece[0xD];  // 0x18
    u8 supercoin[0xD];  // 0x25
    u8 pad_0x32[2];     // 0x32
    s32 next;           // 0x34
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

s32 uranaisi_ryokin_rtn(s32 idx) {
    if (idx != 0) {
        return uranaisi_ryokin_tbl[idx];
    }
    return pouchGetPtr()->level * 2;
}

// NONMATCHING (98.21%): the structure matches (stmw r17-r31, value load on the
// match path, gswf default at the loop exit). What differs is only the callee-saved
// register assignment: the target keeps prefix in r24, &_udt in r19, maxLabel in r20,
// the "enddata"/format literals in r22/r21 and count in r26, while this build puts
// count in r31 and storyId in r20, shifting the long-lived set up by two registers
// (tagNum r19 vs r23, saved r19 vs r25, labelId r25 vs r23). A randomised search over
// the 15 declaration lines did not find an order that reproduces the target mapping.
s32 uranaisi_data_make_supercoin(const char* prefix, s32 type) {
    UranaiWork* wp = &_udt;
    char label[64];
    char tag[64];
    BOOL tagdone;
    char saved;
    s32 tagNum;
    const s8* msg;
    s32 count;
    s32 mbbpos;
    BOOL typeFlag;
    s32 i;
    s32 labelId;
    s32 maxLabel;
    s32 storyId;
    s32 gswf;
    char* tagdata;
    char* initEndPos;
    s16 tagEndPos;

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
        tagdata = (char*)msg + 2;
        if (msg[1] != '!') {
            break;
        }

        tagNum = 0;
        mbbpos = 0;
        tagdone = FALSE;
        while (!tagdone) {
            if (_ismbblead((s8)*tagdata)) {
                if (tagNum == 0) {
                    tag[mbbpos++] = tagdata[0];
                    tag[mbbpos++] = tagdata[1];
                    tagdata += 2;
                } else {
                    mbbpos += 2;
                    tagdata += 2;
                }
            } else {
                switch ((s8)tagdata[0]) {
                    case '>':
                        tagdone = TRUE;
                        // fallthrough
                    case ',':
                        tagdata++;
                        if (tagNum == 0) {
                            tag[mbbpos] = '\0';
                            initEndPos = tagdata;
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
                saved = initEndPos[tagEndPos];
#ifdef MUST_MATCH
                // The target materialises the not-found default (-1) on the loop-exit
                // path, keeping gswf in a volatile register; a plain `break` keeps it
                // live across the strcmp calls and costs an extra callee-saved register.
                i = 0;
                initEndPos[tagEndPos] = '\0';
                while (uranai_table_supercoin[i].name != NULL) {
                    if (!strcmp(initEndPos, uranai_table_supercoin[i].name)) {
                        gswf = uranai_table_supercoin[i].value;
                        goto gswf_done;
                    }
                    i++;
                }
                gswf = -1;
            gswf_done:
#else
                initEndPos[tagEndPos] = '\0';
                i = 0;
                gswf = -1;
                while (uranai_table_supercoin[i].name != NULL) {
                    if (!strcmp(initEndPos, uranai_table_supercoin[i].name)) {
                        gswf = uranai_table_supercoin[i].value;
                        break;
                    }
                    i++;
                }
#endif
                initEndPos[tagEndPos] = saved;
                if (gswf != -1 && evtGetValue(NULL, gswf) == 0) {
                    if (typeFlag) {
                        wp->info[count].labelId = (s16)labelId;
                        wp->info[count].unk4 = (s16)storyId;
                    }
                    count++;
                }
            }
        }
        storyId++;
    }
    return count;
}

// NONMATCHING (98.21%): mirror of make_supercoin, same register-allocation difference
// (count in r31 instead of r26, and the &_udt/maxLabel/literal block shifted up two
// registers); the code shape itself matches.
s32 uranaisi_data_make_starpiece(const char* prefix, s32 type) {
    UranaiWork* wp = &_udt;
    char label[64];
    char tag[64];
    BOOL tagdone;
    char saved;
    s32 tagNum;
    const s8* msg;
    s32 count;
    s32 mbbpos;
    BOOL typeFlag;
    s32 i;
    s32 labelId;
    s32 maxLabel;
    s32 storyId;
    s32 gswf;
    char* tagdata;
    char* initEndPos;
    s16 tagEndPos;

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
        tagdata = (char*)msg + 2;
        if (msg[1] != '!') {
            break;
        }

        tagNum = 0;
        mbbpos = 0;
        tagdone = FALSE;
        while (!tagdone) {
            if (_ismbblead((s8)*tagdata)) {
                if (tagNum == 0) {
                    tag[mbbpos++] = tagdata[0];
                    tag[mbbpos++] = tagdata[1];
                    tagdata += 2;
                } else {
                    mbbpos += 2;
                    tagdata += 2;
                }
            } else {
                switch ((s8)tagdata[0]) {
                    case '>':
                        tagdone = TRUE;
                        // fallthrough
                    case ',':
                        tagdata++;
                        if (tagNum == 0) {
                            tag[mbbpos] = '\0';
                            initEndPos = tagdata;
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
                saved = initEndPos[tagEndPos];
#ifdef MUST_MATCH
                // See uranaisi_data_make_supercoin: the default must sit on the
                // loop-exit path so gswf stays in a volatile register.
                i = 0;
                initEndPos[tagEndPos] = '\0';
                while (uranai_table_starpiece[i].name != NULL) {
                    if (!strcmp(initEndPos, uranai_table_starpiece[i].name)) {
                        gswf = uranai_table_starpiece[i].value;
                        goto gswf_done;
                    }
                    i++;
                }
                gswf = -1;
            gswf_done:
#else
                initEndPos[tagEndPos] = '\0';
                i = 0;
                gswf = -1;
                while (uranai_table_starpiece[i].name != NULL) {
                    if (!strcmp(initEndPos, uranai_table_starpiece[i].name)) {
                        gswf = uranai_table_starpiece[i].value;
                        break;
                    }
                    i++;
                }
#endif
                initEndPos[tagEndPos] = saved;
                if (gswf != -1 && evtGetValue(NULL, gswf) == 0) {
                    if (typeFlag) {
                        wp->info[count].labelId = (s16)labelId;
                        wp->info[count].unk4 = (s16)storyId;
                    }
                    count++;
                }
            }
        }
        storyId++;
    }
    return count;
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
        if (wp->next == -1) {
            return 0;
        }
    } else {
        wp->next = -1;
    }

    count = 0;
    storyId = 0;
    while (1) {
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
                    if (labelId != wp->next) {
                        goto next;
                    }
                    wp->info[count].labelId = (s16)labelId;
                    wp->info[count].unk4 = (s16)storyId;
                } else {
                    if (labelId > maxLabel) {
                        goto next;
                    }
                    if (labelId < wp->next) {
                        goto next;
                    }
                    if (labelId > wp->next) {
                        wp->next = labelId;
                        count = 0;
                    }
                }
                count++;
            }
        }
    next:
        storyId++;
    }
    return count;
}

USER_FUNC(uranaisi_supercoin_setreadflag) {
    UranaiWork* wp = &_udt;

    johoya_set(wp->supercoin, wp->info[wp->index].unk4, 1);
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
        if (johoya_get(wp->supercoin, wp->info[i].unk4) == 0) {
            if (arg == unread) {
                sprintf(wp->label, "%s_%04d", wp->prefix, wp->info[i].unk4);
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

USER_FUNC(uranaisi_supercoin_check) {
    UranaiWork* wp = &_udt;
    s32* args = event->args;
    s32 i;
    s32 count;

    count = 0;
    i = 0;
    while (i < wp->count) {
        if (johoya_get(wp->supercoin, wp->info[i].unk4) == 0) {
            count++;
        }
        i++;
    }
    evtSetValue(event, args[0], count);
    return 2;
}

USER_FUNC(uranaisi_starpiece_setreadflag) {
    UranaiWork* wp = &_udt;

    johoya_set(wp->starpiece, wp->info[wp->index].unk4, 1);
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
        if (johoya_get(wp->starpiece, wp->info[i].unk4) == 0) {
            if (arg == unread) {
                sprintf(wp->label, "%s_%04d", wp->prefix, wp->info[i].unk4);
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

USER_FUNC(uranaisi_starpiece_check) {
    UranaiWork* wp = &_udt;
    s32* args = event->args;
    s32 i;
    s32 count;

    count = 0;
    i = 0;
    while (i < wp->count) {
        if (johoya_get(wp->starpiece, wp->info[i].unk4) == 0) {
            count++;
        }
        i++;
    }
    evtSetValue(event, args[0], count);
    return 2;
}

USER_FUNC(uranaisi_next_makelabel) {
    UranaiWork* wp = &_udt;
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);

    sprintf(wp->label, "%s_%04d", wp->prefix, wp->info[idx].unk4);
    evtSetValue(event, args[1], (s32)wp->label);
    return 2;
}

USER_FUNC(uranaisi_next_check) {
    UranaiWork* wp = &_udt;

    evtSetValue(event, event->args[0], wp->count);
    return 2;
}

USER_FUNC(uranaisi_table_clear) {
    UranaiWork* wp = &_udt;

    memset(wp->starpiece, 0, 0xD);
    memset(wp->supercoin, 0, 6);
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

USER_FUNC(uranaisi_data_alloc) {
    UranaiWork* wp = &_udt;
    s32* args = event->args;
    s32 count;
    const char* prefix;

    _udt.type = evtGetValue(event, args[0]);
    _udt.prefix = (const char*)evtGetValue(event, args[1]);
    switch (_udt.type) {
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
    wp->info = _mapAlloc(wp->count * 6 + 0x40);
    memset(wp->info, 0, wp->count * 6 + 0x40);
    wp->label = (char*)((u8*)wp->info + wp->count * 6);
    prefix = wp->prefix;
    switch (wp->type) {
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
