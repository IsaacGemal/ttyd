#include "window/win_party.h"
#include "window/win_root.h"
#include "driver/animdrv.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "mario/mario_pouch.h"
#include "pmario_sound.h"

extern WindowPartyDt winPartyDt[];

// Extended fields of WindowWork used by this unit. win/win_main.h only declares
// the first 0x40 bytes, so the rest are reached through raw offsets below.
#define WW_U16(w, off) (*(u16*)((u8*)(w) + (off)))
#define WW_U32(w, off) (*(u32*)((u8*)(w) + (off)))
#define WW_S32(w, off) (*(s32*)((u8*)(w) + (off)))
#define WW_F32(w, off) (*(f32*)((u8*)(w) + (off)))

void winPartyMain2(WindowWork* work) {
    f32 factor = 0.25f;
    f32 diff = WW_F32(work, 0x1E8) - WW_F32(work, 0x1E4);

    WW_F32(work, 0x1E4) += diff * factor;
}

s32 winPartyMain(WindowWork* work) {
    s32 k;

    if (WW_S32(work, 0x1E0) == 0) {
        return -1;
    }

    switch (WW_S32(work, 0x200)) {
    case 0: {
        u32 button = WW_U32(work, 0x4);
        u32 dir;

        if (button & 0x100) {
            WW_S32(work, 0x19C) = 1;
            return -2;
        }
        if (button & 0x400) {
            WW_S32(work, 0x200) = 1;
            WW_S32(work, 0x208) = 0;
            psndSFXOn((const char*)0x20012);
            goto apply;
        }
        if (button & 0x200) {
            psndSFXOn((const char*)0x20013);
            WW_S32(work, 0x19C) = 0;
            return -1;
        }
        if (button & 0x1000) {
            WW_S32(work, 0x19C) = 0;
            return -2;
        }

        dir = WW_U32(work, 0x10);
        if (dir & 0x4000) {
            s32 prev = WW_S32(work, 0x1DC);

            WW_S32(work, 0x1DC)--;
            while (WW_S32(work, 0x1DC) < 0) {
                WW_S32(work, 0x1DC) += WW_S32(work, 0x1E0);
            }
            WW_F32(work, 0x1E8) = 360.0f / (f32)WW_S32(work, 0x1E0) * (f32)WW_S32(work, 0x1DC);
            while (WW_F32(work, 0x1E4) < WW_F32(work, 0x1E8)) {
                WW_F32(work, 0x1E4) += 360.0f;
            }
            if (prev != WW_S32(work, 0x1DC)) {
                psndSFXOn((const char*)0x209D6);
            }
            for (k = 0; k < 8; k++) {
                if (((s32*)work)[WW_S32(work, 0x1DC) + 0x6F] == winPartyDt[k].field_0x0) {
                    WW_S32(work, 0x1D8) = k;
                    break;
                }
            }
            goto apply;
        }
        if (dir & 0x8000) {
            s32 prev = WW_S32(work, 0x1DC);

            WW_S32(work, 0x1DC)++;
            while (WW_S32(work, 0x1DC) > WW_S32(work, 0x1E0) - 1) {
                WW_S32(work, 0x1DC) -= WW_S32(work, 0x1E0);
            }
            WW_F32(work, 0x1E8) = 360.0f / (f32)WW_S32(work, 0x1E0) * (f32)WW_S32(work, 0x1DC);
            while (WW_F32(work, 0x1E4) > WW_F32(work, 0x1E8)) {
                WW_F32(work, 0x1E4) -= 360.0f;
            }
            if (prev != WW_S32(work, 0x1DC)) {
                psndSFXOn((const char*)0x209D6);
            }
            for (k = 0; k < 8; k++) {
                if (((s32*)work)[WW_S32(work, 0x1DC) + 0x6F] == winPartyDt[k].field_0x0) {
                    WW_S32(work, 0x1D8) = k;
                    break;
                }
            }
            goto apply;
        }
    apply:
        WW_F32(work, 0x158) = -200.0f;
        WW_F32(work, 0x15C) = 20.0f;
        WW_S32(work, 0x204) = pouchGetPartyAttackLv(winPartyDt[WW_S32(work, 0x1D8)].field_0x0) + 2;
        winMsgEntry(work, 0, winPartyDt[WW_S32(work, 0x1D8)].field_0xC, 0);
        break;
    }
    case 1: {
        u32 dir = WW_U32(work, 0x10);

        if (dir & 0x1000) {
            WW_S32(work, 0x208)--;
            if (WW_S32(work, 0x208) < 0) {
                WW_S32(work, 0x208) = WW_S32(work, 0x204) - 1;
            }
            psndSFXOn((const char*)0x20005);
        } else if (dir & 0x2000) {
            WW_S32(work, 0x208)++;
            if (WW_S32(work, 0x208) >= WW_S32(work, 0x204)) {
                WW_S32(work, 0x208) = 0;
            }
            psndSFXOn((const char*)0x20005);
        } else {
            u32 button = WW_U32(work, 0x4);

            if (button & 0x200) {
                WW_S32(work, 0x200) = 0;
                psndSFXOn((const char*)0x20013);
            } else if (button & 0x1000) {
                WW_S32(work, 0x19C) = 0;
                return -2;
            }
        }

        WW_F32(work, 0x158) = -10.0f;
        WW_F32(work, 0x15C) = -26.0f * (f32)WW_S32(work, 0x208) + 5.0f;
        winMsgEntry(work, 0, *(const char**)((u8*)((void**)winPartyDt[WW_S32(work, 0x1D8)].field_0x20)[WW_S32(work, 0x208)] + 0xC), 0);
        break;
    }
    default:
        break;
    }

    return 0;
}

void winPartyExit(WindowWork* work) {
    u8* p = (u8*)work;
    s32 i;

    for (i = 0; i < WW_S32(work, 0x1E0); i++) {
        if (*(s32*)(p + 0x1A0) != -1) {
            animPoseRelease(*(s32*)(p + 0x1A0));
        }
        p += 4;
    }
}

void winPartyInit2(WindowWork* work) {
    f32 time = 360.0f / (f32)WW_S32(work, 0x1E0) * (f32)WW_S32(work, 0x1DC);

    WW_F32(work, 0x1E8) = time;
    WW_F32(work, 0x1E4) = time;
    WW_S32(work, 0x200) = 0;
    WW_F32(work, 0x158) = -200.0f;
    WW_F32(work, 0x15C) = 20.0f;
    winMsgEntry(work, 0, winPartyDt[WW_S32(work, 0x1D8)].field_0xC, 0);
}

// NONMATCHING (99.97%): only the .rodata-base offsets differ (the addi/lfd immediates for
// the yoshi strings and the u32->double constant); winPartyDt/winPartyDisp are not decompiled,
// so the strings that precede them are missing. Every instruction opcode/register matches.
void winPartyInit(WindowWork* work) {
    PouchData* pouch = pouchGetPtr();
    s32 i;
    s32 j;
    s32 k;
    s32 pose;
    const char* model;
    const char* anim;

    WW_S32(work, 0x1E0) = 0;

    for (i = 0; i < 7; i++) {
        for (j = 0; j < 8; j++) {
            if (j != winPartyDt[i].field_0x0)
                continue;
            if (!(pouch->partyData[j].flags & 1))
                continue;
            if (pouch->partyData[j].flags & 2)
                continue;

            if (WW_U16(work, 0x0) & 0x2000) {
                model = winPartyDt[i].field_0x18;
                switch (pouchGetPartyColor(PARTNER_YOSHI)) {
                case 0: anim = "R_5"; break;
                case 1: anim = "R_6"; break;
                case 2: anim = "R_7"; break;
                case 3: anim = "R_8"; break;
                case 4: anim = "R_9"; break;
                case 5: anim = "R_10"; break;
                case 6:
                default: anim = "R_11"; break;
                }
            } else if (j == 4) {
                switch (pouchGetPartyColor(PARTNER_YOSHI)) {
                case 0: model = "EFF_m_yoshi"; break;
                case 1: model = "EFF_m_yoshi2"; break;
                case 2: model = "EFF_m_yoshi3"; break;
                case 3: model = "EFF_m_yoshi4"; break;
                case 4: model = "EFF_m_yoshi5"; break;
                case 5: model = "EFF_m_yoshi6"; break;
                case 6:
                default: model = "EFF_m_yoshi7"; break;
                }
                anim = "W_1";
            } else {
                model = winPartyDt[i].field_0x10;
                anim = winPartyDt[i].field_0x14;
            }

            pose = animPoseEntry(model, 0);
            ((s32*)work)[WW_S32(work, 0x1E0) + 0x68] = pose;
            animPoseSetAnim(((s32*)work)[WW_S32(work, 0x1E0) + 0x68], anim, 1);
            animPoseSetMaterialFlagOn(((s32*)work)[WW_S32(work, 0x1E0) + 0x68], 0x1800);
            ((s32*)work)[WW_S32(work, 0x1E0) + 0x6F] = j;
            WW_S32(work, 0x1E0)++;
        }
    }

    i = 0;
    WW_S32(work, 0x1DC) = i;
    for (; i < WW_S32(work, 0x1E0); i++) {
        s32 party;

        party = !marioBgmodeChk() ? marioGetParty() : marioGetPtr()->prevPartyMemberId[0];
        if (party == ((s32*)work)[i + 0x6F]) {
            WW_S32(work, 0x1DC) = i;
            break;
        }
    }

    for (k = 0; k < 8; k++) {
        if (((s32*)work)[WW_S32(work, 0x1DC) + 0x6F] == winPartyDt[k].field_0x0) {
            WW_S32(work, 0x1D8) = k;
            break;
        }
    }

    WW_S32(work, 0x1FC) = WW_S32(work, 0x1DC);
    {
        f32 time = 360.0f / (f32)WW_S32(work, 0x1E0) * (f32)WW_S32(work, 0x1DC);

        WW_F32(work, 0x1E8) = time;
        WW_F32(work, 0x1E4) = time;
    }
    WW_S32(work, 0x200) = 0;
    WW_S32(work, 0x204) = pouchGetPartyAttackLv(winPartyDt[WW_S32(work, 0x1D8)].field_0x0) + 2;

    switch (pouchGetPartyColor(PARTNER_YOSHI)) {
    case 0: winPartyDt[3].field_0x4 = 0x158; break;
    case 1: winPartyDt[3].field_0x4 = 0x159; break;
    case 2: winPartyDt[3].field_0x4 = 0x15A; break;
    case 3: winPartyDt[3].field_0x4 = 0x15B; break;
    case 4: winPartyDt[3].field_0x4 = 0x15C; break;
    case 5: winPartyDt[3].field_0x4 = 0x15D; break;
    case 6:
    default: winPartyDt[3].field_0x4 = 0x15E; break;
    }
}
