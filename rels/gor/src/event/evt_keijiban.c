#include "evt_keijiban.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "driver/msgdrv.h"
#include "evt/evt_johoya.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include <stdio.h>
#include <string.h>

#pragma pool_data off

#pragma force_active on

// clang-format off
extend_data(28,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(29,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

KeijibanWork _kjdt = { 0 };

USER_FUNC(keijiban_data_alloc) {
    KeijibanWork* wp = &_kjdt;

    wp->prefix = (const char*)evtGetValue(event, *event->args);
    wp->count = keijiban_data_make(wp->prefix, 0);
    wp->info = _mapAlloc(wp->count * sizeof(KeijibanInfo) + 64);
    memset(wp->info, 0, wp->count * sizeof(KeijibanInfo) + 64);
    wp->infoEnd = (char*)&wp->info[wp->count];
    keijiban_data_make(wp->prefix, 1);
    return 2;
}

USER_FUNC(keijiban_data_free) {
    KeijibanWork* wp = &_kjdt;

    if (wp->info == NULL) {
        return 2;
    }
    _mapFree(wp->info);
    wp->info = NULL;
    wp->count = 0;
    wp->prefix = NULL;
    wp->infoEnd = NULL;
    wp->index = 0;
    return 2;
}

USER_FUNC(keijiban_joho_check) {
    KeijibanWork* wp = &_kjdt;
    s32* args = event->args;

    evtSetValue(event, args[0], wp->count);
    return 2;
}

USER_FUNC(keijiban_joho_makelabel) {
    s32* args = event->args;
    s32 value = evtGetValue(event, args[0]);
    KeijibanWork* wp = &_kjdt;

    if (value >= 0 && value < wp->count) {
        sprintf(wp->infoEnd, "%s_%04d", wp->prefix, wp->info[value].storyId);
        evtSetValue(event, args[1], (s32)wp->infoEnd);
        wp->index = value;
        return 2;
    }

    wp->infoEnd[0] = '\0';
    evtSetValue(event, args[1], (s32)wp->infoEnd);
    return 2;
}

s32 keijiban_data_make(const char* prefix, s32 type) {
    KeijibanWork* wp = &_kjdt;
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
        if (wp->maxLabel == -1) {
            return 0;
        }
    }
    else {
        wp->maxLabel = -1;
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
                }
                else {
                    mbbpos += 2;
                    tagdata += 2;
                }
            }
            else {
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
                        }
                        else {
                            mbbpos++;
                            tagdata++;
                        }
                }
            }
        }

        if (!strcmp(tag, "enddata")) {
            break;
        }
        if (strcmp(tag, "")) {
            labelId = search_evt_no(tag);
            if (labelId != -1) {
                if (type & 1) {
                    if (labelId != wp->maxLabel) {
                        continue;
                    }
                    wp->info[count].labelId = (s16)labelId;
                    wp->info[count].storyId = (s16)storyId;
                }
                else {
                    if (labelId > maxLabel) {
                        continue;
                    }
                    if (labelId < wp->maxLabel) {
                        continue;
                    }
                    if (labelId > wp->maxLabel) {
                        wp->maxLabel = labelId;
                        count = 0;
                    }
                }
                count++;
            }
        }
    }
    return count;
}
// clang-format on
