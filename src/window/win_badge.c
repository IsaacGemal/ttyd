#include "window/win_badge.h"
#include "window/win_root.h"
#include "window/win_mario.h"
#include "window/win_main.h"
#include "win/win_main.h"
#include "mario/mario_pouch.h"
#include "driver/animdrv.h"
#include "driver/msgdrv.h"
#include "manager/fontmgr.h"
#include "pmario_sound.h"
#include "data/item_data.h"
#include <stdio.h>

// Extended WindowWork fields used by this unit. win_main.h only declares the
// first 0x40 bytes, so the rest are reached through raw offsets.
#define WW_S32(w, off) (*(s32*)((u8*)(w) + (off)))
#define WW_F32(w, off) (*(f32*)((u8*)(w) + (off)))

// .rodata (unit data, defined here so MWCC pools it and uses the vec3 base)
const Vec vec3_802f4a38 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4a44 = {1.0f, 1.0f, 1.0f};
const Vec vec3_802f4a50 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4a5c = {1.0f, 1.0f, 1.0f};
const Vec vec3_802f4a68 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4a74 = {1.0f, 1.0f, 1.0f};
const Vec vec3_802f4a80 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4a8c = {0.8f, 0.8f, 0.8f};
const Vec vec3_802f4a98 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4aa4 = {0.8f, 0.8f, 0.8f};
const Vec vec3_802f4ab0 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4abc = {0.5f, 0.5f, 0.5f};
const Vec vec3_802f4ac8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4ad4 = {0.8f, 0.8f, 0.8f};
const Vec vec3_802f4ae0 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4aec = {1.0f, 1.0f, 1.0f};
const Vec vec3_802f4af8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4b04 = {1.0f, 1.0f, 1.0f};
const Vec vec3_802f4b10 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4b1c = {44.0f, 1.0f, 1.0f};
const Vec vec3_802f4b28 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4b34 = {0.54f, 0.54f, 0.54f};
const Vec vec3_802f4b40 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4b4c = {0.9f, 0.9f, 0.9f};
const Vec vec3_802f4b58 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4b64 = {0.9f, 0.9f, 0.9f};
const Vec vec3_802f4b70 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4b7c = {1.0f, 1.0f, 1.0f};
const Vec vec3_802f4b88 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4b94 = {-1.0f, -1.0f, -1.0f};
const Vec vec3_802f4ba0 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4bac = {0.6f, 0.6f, 0.6f};
const Vec vec3_802f4bb8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4bc4 = {1.0f, 1.0f, 1.0f};
const Vec vec3_802f4bd0 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4bdc = {0.6f, 0.6f, 0.6f};
const Vec vec3_802f4be8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4bf4 = {0.7f, 0.7f, 0.7f};
const Vec vec3_802f4c00 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4c0c = {0.6f, 0.6f, 0.6f};
const Vec vec3_802f4c18 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4c24 = {0.5f, 0.5f, 0.5f};
const char str_802F4C30[24] = "msg_menu_sort_narabikae";
const char str_802F4C48[24] = "msg_menu_badge_hituyou\000";
const f64 double_to_int_802f4c60 = 4503601774854144.0;
const f64 double_to_int_mask_802f4c68 = 4503599627370496.0;
const char str_802F4C70[24] = "msg_menu_badge_nokori\000\000";
const char str_802F4C88[20] = "msg_menu_badge_all\000";
const char str_802F4C9C[24] = "msg_menu_badge_equip\000\000\000";
const char str_802F4CB4[8] = "d_mario";
const char str_802F4CBC[12] = "a_mario_l\000\000";
const char str_802F4CC8[12] = "a_mario_w\000\000";
const char str_802F4CD4[12] = "a_mario_wl\000";
const char str_802F4CE0[8] = "a_mario";


// .sdata2
const u32 dat_8041d348 = 0xFFFFFFFF;
const u32 dat_8041d34c = 0x000000FF;
const u32 dat_8041d350 = 0x00000080;
const u32 dat_8041d354 = 0x808080FF;
const u32 dat_8041d358 = 0xFFFFFFFF;
const u32 dat_8041d35c = 0xFFFFFFFF;
const u32 dat_8041d360 = 0x7F7F7FFF;
const u32 dat_8041d364 = 0x000000FF;
const u32 dat_8041d368 = 0xFFFFFFFF;
const u32 dat_8041d36c = 0x000000FF;
const f32 float_110_8041d370 = 110.0f;
const f32 float_82_8041d374 = 82.0f;
const f32 float_142_8041d378 = 142.0f;
const f32 float_160_8041d37c = 160.0f;
const f32 float_240_8041d380 = 240.0f;
const f32 float_neg240_8041d384 = -240.0f;
const f32 float_75_8041d388 = 75.0f;
const f32 float_55_8041d38c = 55.0f;
const f32 float_12_8041d390 = 12.0f;
const char str_8041D394[3] = "%s";
const f32 float_250_8041d398 = 250.0f;
const f32 float_20_8041d39c = 20.0f;
const f32 float_18_8041d3a0 = 18.0f;
const f32 float_113_8041d3a4 = 113.0f;
const f32 float_90_8041d3a8 = 90.0f;
const f32 float_15_8041d3ac = 15.0f;
const f32 float_0p7_8041d3b0 = 0.7f;
const f32 float_220_8041d3b4 = 220.0f;
const f32 float_92_8041d3b8 = 92.0f;
const f32 float_10_8041d3bc = 10.0f;
const f32 float_30_8041d3c0 = 30.0f;
const f32 float_100_8041d3c4 = 100.0f;
const f32 float_120_8041d3c8 = 120.0f;
const f32 float_135_8041d3cc = 135.0f;
const f32 float_365_8041d3d0 = 365.0f;
const f32 float_210_8041d3d4 = 210.0f;
const f32 float_105_8041d3d8 = 105.0f;
const f32 float_190_8041d3dc = 190.0f;
const f32 float_4_8041d3e0 = 4.0f;
const f32 float_260_8041d3e4 = 260.0f;
const f32 float_40_8041d3e8 = 40.0f;
const f32 float_150_8041d3ec = 150.0f;
const char str_8041D3F0[5] = "\202a\202o";
const f32 float_175_8041d3f8 = 175.0f;
const f32 float_180_8041d3fc = 180.0f;
const f32 float_108_8041d400 = 108.0f;
const f32 float_145_8041d404 = 145.0f;
const f32 float_16_8041d408 = 16.0f;
const f32 float_102_8041d40c = 102.0f;
const f32 float_28_8041d410 = 28.0f;
const f32 float_242_8041d414 = 242.0f;
const f32 float_184_8041d418 = 184.0f;
const f32 float_0p25_8041d41c = 0.25f;
const f32 float_neg310_8041d420 = -310.0f;
const f32 float_neg270_8041d424 = -270.0f;
const f32 float_neg110_8041d428 = -110.0f;
const char str_8041D42C[4] = "S_2";
const char str_8041D430[5] = "W_15";
const char str_8041D438[4] = "S_3";
const char str_8041D43C[5] = "W_17";
const char str_8041D444[4] = "S_4";
const char str_8041D448[5] = "W_18";
const char str_8041D450[4] = "S_1";
const char str_8041D454[4] = "W_1";
const char str_8041D458[6] = "M_S_1";
const char str_8041D460[6] = "M_W_1";

// 136 B
void winBadgeMain2(WindowWork* work) {
    WW_F32(work, 0x3FC) = float_184_8041d418 * (f32)WW_S32(work, 0x3EC);
    WW_F32(work, 0x3F4) += (WW_F32(work, 0x3FC) - WW_F32(work, 0x3F4)) / 4.0f;
    WW_F32(work, 0x400) = float_184_8041d418 * (f32)WW_S32(work, 0x3F0);
    WW_F32(work, 0x3F8) += (WW_F32(work, 0x400) - WW_F32(work, 0x3F8)) / 4.0f;
}

extern ItemData itemDataTable[];

// 1752 B
// 100%. Two shapes are needed to reproduce the original's register allocation/scheduling:
//   - the condition is written inline (`itemDataTable[pouchHaveBadge(idx)].bp_cost`) so the `lwz`/`cmpw`
//     land availableBP in r0 and bp_cost in r3 instead of r4/r0.
//   - the sort key's 0x3E4 pointer is held in a named `selPtr`, which puts `sel` in r28 (not r27).
// The `void* arg0` copy is a scheduling aid only (see MUST_MATCH below): it materialises `work` into r3
// before the `if`, so the `mr r3, r31` for winSortEntry is not sunk into the `lfs` delay slot.
s32 winBadgeMain(WindowWork* work) {
    PouchData* pouch = pouchGetPtr();
    s32 sel;
    s32 count;

    switch (WW_S32(work, 0x3DC)) {
    case 0:
        if (work->buttonTrg & 0x200) {
            psndSFXOn((const char*)0x20013);
            return -1;
        }
        if (work->buttonTrg & 0x1000) {
            return -2;
        }
        if (work->dirRep & 0x3000) {
            WW_S32(work, 0x3E0) = 1 - WW_S32(work, 0x3E0);
            psndSFXOn((const char*)0x20005);
            if (WW_S32(work, 0x3E0) != 0) {
                winMakeEquipList(work);
            }
        } else if (work->buttonTrg & 0x100) {
            if (WW_S32(work, 0x3E0) == 0) {
                if (pouchGetHaveBadgeCnt() == 0) {
                    goto end;
                }
            }
            if (WW_S32(work, 0x3E0) == 1) {
                if (WW_S32(work, 0xD64) == 0) {
                    goto end;
                }
            }
            psndSFXOn((const char*)0x20012);
            WW_S32(work, 0x3DC) = 10;
            goto end;
        } else if (work->buttonTrg & 0x10) {
#ifdef MUST_MATCH
            void* arg0 = work; // match-only: forces `mr r3, r31` before the idx branch (schedule)
#endif
            s32 idx = 3;
            if (WW_S32(work, 0x3E0) == 0) {
                idx = 2;
            }
#ifdef MUST_MATCH
            winSortEntry((WindowWork*)arg0, idx, float_neg310_8041d420, float_150_8041d3ec);
#else
            winSortEntry(work, idx, float_neg310_8041d420, float_150_8041d3ec);
#endif
            WW_S32(work, 0x124) = WW_S32(work, 0x3DC);
            psndSFXOn((const char*)0x20012);
            WW_S32(work, 0x3DC) = 1000;
        }
        WW_F32(work, 0x158) = float_neg270_8041d424;
        WW_F32(work, 0x15C) = (f32)(0x7D - WW_S32(work, 0x3E0) * 0x28);
        winMsgEntry(work, 0, WW_S32(work, 0x3E0) == 0 ? str_802F4C88 : str_802F4C9C, 0);
        goto end;

    case 10:
        if (WW_S32(work, 0x3E0) == 0) {
            count = pouchGetHaveBadgeCnt();
        } else {
            count = WW_S32(work, 0xD64);
        }
        sel = WW_S32(work, 0x3E0);
        {
            s32 rows = (count + 7) / 8;

            if (work->buttonTrg & 0x100) {
                if (sel == 0) {
                    s32 idx = ((s32*)((u8*)work + 0x3E4))[sel];
                    if (pouchEquipCheckBadgeIndex(idx)) {
                        psndSFXOn((const char*)0x20039);
                        pouchUnEquipBadgeIndex(idx);
                    } else {
                        if (pouch->availableBP >= itemDataTable[pouchHaveBadge(idx)].bp_cost) {
                            psndSFXOn((const char*)0x20038);
                            pouchEquipBadgeIndex(idx);
                        } else {
                            psndSFXOn((const char*)0x20014);
                        }
                    }
                    pouchReviseMarioParam();
                    pouchRevisePartyParam();
                    mario_change(work, pouchHaveBadge(idx));
                } else {
                    s32* selPtr = (s32*)((u8*)work + 0x3E4) + sel;
                    s32 entryIdx = selPtr[0];
                    s32 idx = ((s32*)((u8*)work))[entryIdx * 3 + 0x102];
                    if (((s32*)((u8*)work))[entryIdx * 3 + 0x101] != 0) {
                        psndSFXOn((const char*)0x20039);
                        pouchUnEquipBadgeIndex(idx);
                        ((s32*)((u8*)work + 0x404))[selPtr[0] * 3] = 0;
                    } else {
                        if (pouch->availableBP >= itemDataTable[pouchHaveBadge(idx)].bp_cost) {
                            psndSFXOn((const char*)0x20038);
                            pouchEquipBadgeIndex(idx);
                            ((s32*)((u8*)work + 0x404))[selPtr[0] * 3] = 1;
                        }
                    }
                    pouchReviseMarioParam();
                    pouchRevisePartyParam();
                    mario_change(work, pouchHaveBadge(idx));
                }
            } else if (work->buttonTrg & 0x200) {
                psndSFXOn((const char*)0x20013);
                WW_S32(work, 0x3DC) = 0;
            } else if (work->buttonTrg & 0x1000) {
                return -2;
            } else if (work->buttonRep & 0x40) {
                ((s32*)((u8*)work + 0x3EC))[sel] = ((s32*)((u8*)work + 0x3EC))[sel] - 1;
                if (((s32*)((u8*)work + 0x3EC))[sel] < 0) {
                    ((s32*)((u8*)work + 0x3EC))[sel] = 0;
                }
                ((s32*)((u8*)work + 0x3E4))[sel] = ((s32*)((u8*)work + 0x3EC))[sel] * 8;
                psndSFXOn((const char*)0x20035);
            } else if (work->buttonRep & 0x20) {
                ((s32*)((u8*)work + 0x3EC))[sel] = ((s32*)((u8*)work + 0x3EC))[sel] + 1;
                if (((s32*)((u8*)work + 0x3EC))[sel] >= rows) {
                    ((s32*)((u8*)work + 0x3EC))[sel] = rows - 1;
                }
                ((s32*)((u8*)work + 0x3E4))[sel] = ((s32*)((u8*)work + 0x3EC))[sel] * 8;
                psndSFXOn((const char*)0x20035);
            } else if (work->dirRep & 0x1000) {
                ((s32*)((u8*)work + 0x3E4))[sel] = ((s32*)((u8*)work + 0x3E4))[sel] - 1;
                psndSFXOn((const char*)0x20035);
            } else if (work->dirRep & 0x2000) {
                ((s32*)((u8*)work + 0x3E4))[sel] = ((s32*)((u8*)work + 0x3E4))[sel] + 1;
                psndSFXOn((const char*)0x20035);
            } else if (work->buttonTrg & 0x10) {
#ifdef MUST_MATCH
                void* arg0 = work; // match-only: forces `mr r3, r31` before the idx branch (schedule)
#endif
                s32 idx = 3;
                if (sel == 0) {
                    idx = 2;
                }
#ifdef MUST_MATCH
                winSortEntry((WindowWork*)arg0, idx, float_neg310_8041d420, float_150_8041d3ec);
#else
                winSortEntry(work, idx, float_neg310_8041d420, float_150_8041d3ec);
#endif
                WW_S32(work, 0x124) = WW_S32(work, 0x3DC);
                psndSFXOn((const char*)0x20012);
                WW_S32(work, 0x3DC) = 1000;
            }

            if (((s32*)((u8*)work + 0x3E4))[sel] < 0) {
                ((s32*)((u8*)work + 0x3E4))[sel] = count - 1;
            }
            if (((s32*)((u8*)work + 0x3E4))[sel] >= count) {
                ((s32*)((u8*)work + 0x3E4))[sel] = 0;
            }
            ((s32*)((u8*)work + 0x3EC))[sel] = ((s32*)((u8*)work + 0x3E4))[sel] / 8;
            WW_F32(work, 0x158) = float_neg110_8041d428;
            WW_F32(work, 0x15C) = (f32)(0x6E - (((s32*)((u8*)work + 0x3E4))[sel] % 8) * 0x17);
            {
                s32 msgId;
                if (WW_S32(work, 0x3E0) == 0) {
                    msgId = pouchHaveBadge(((s32*)((u8*)work + 0x3E4))[sel]);
                } else {
                    msgId = ((s32*)((u8*)work + 0x40C))[((s32*)((u8*)work + 0x3E4))[sel] * 3];
                }
                winMsgEntry(work, msgId, itemDataTable[msgId].menu_description, 0);
            }
        }
        goto end;

    case 1000:
        if (!winSortWait(work)) {
            WW_S32(work, 0x3DC) = WW_S32(work, 0x124);
        }
        if (work->buttonTrg & 0x1000) {
            return -2;
        }
        break;
    }

end:
    return 0;
}

// 516 B
void mario_change(WindowWork* work, s32 badgeId) {
    const char* anim;
    const char* s1;
    const char* s2;
    s32 badge147;
    s32 badge148;

    badge147 = pouchEquipCheckBadge(0x147);
    badge148 = pouchEquipCheckBadge(0x148);

    if (work->flags & 0x2000) {
        if (badge147 && !badge148) {
            s1 = str_8041D42C;
            s2 = str_8041D430;
        } else if (!badge147 && badge148) {
            s1 = str_8041D438;
            s2 = str_8041D43C;
        } else if (badge147 && badge148) {
            s1 = str_8041D444;
            s2 = str_8041D448;
        } else {
            s1 = str_8041D450;
            s2 = str_8041D454;
        }
        if (WW_S32(work, 0x188) != -1) {
            animPoseRelease(WW_S32(work, 0x188));
            WW_S32(work, 0x188) = animPoseEntry(str_802F4CB4, 0);
            animPoseSetAnim(WW_S32(work, 0x188), s1, 1);
            animPoseSetMaterialFlagOn(WW_S32(work, 0x188), 0x1800);
        }
        if (WW_S32(work, 0xE1C) != -1) {
            animPoseRelease(WW_S32(work, 0xE1C));
            WW_S32(work, 0xE1C) = animPoseEntry(str_802F4CB4, 0);
            animPoseSetAnim(WW_S32(work, 0xE1C), s2, 1);
            animPoseSetMaterialFlagOn(WW_S32(work, 0xE1C), 0x1800);
        }
    } else {
        if (badge147 && !badge148) {
            anim = str_802F4CBC;
        } else if (!badge147 && badge148) {
            anim = str_802F4CC8;
        } else if (badge147 && badge148) {
            anim = str_802F4CD4;
        } else {
            anim = str_802F4CE0;
        }
        if (WW_S32(work, 0x188) != -1) {
            animPoseRelease(WW_S32(work, 0x188));
            WW_S32(work, 0x188) = animPoseEntry(anim, 0);
            animPoseSetAnim(WW_S32(work, 0x188), str_8041D458, 1);
            animPoseSetMaterialFlagOn(WW_S32(work, 0x188), 0x1800);
        }
        if (WW_S32(work, 0xE1C) != -1) {
            animPoseRelease(WW_S32(work, 0xE1C));
            WW_S32(work, 0xE1C) = animPoseEntry(anim, 0);
            animPoseSetAnim(WW_S32(work, 0xE1C), str_8041D460, 1);
            animPoseSetMaterialFlagOn(WW_S32(work, 0xE1C), 0x1800);
        }
    }
}

// 4 B
void winBadgeExit(void) {
}

// 464 B
void winBadgeInit2(WindowWork* work) {
    WW_F32(work, 0x158) = float_neg270_8041d424;
    WW_F32(work, 0x15C) = (f32)(0x7D - WW_S32(work, 0x3E0) * 0x28);
    WW_F32(work, 0x3F4) = WW_F32(work, 0x3FC) = float_184_8041d418 * (f32)WW_S32(work, 0x3EC);
    WW_F32(work, 0x3F8) = WW_F32(work, 0x400) = float_184_8041d418 * (f32)WW_S32(work, 0x3F0);
    winMsgEntry(work, 0, WW_S32(work, 0x3E0) == 0 ? str_802F4C88 : str_802F4C9C, 0);
    winMakeEquipList(work);
}

// 312 B
void winBadgeInit(WindowWork* work) {
    WW_S32(work, 0x3DC) = 0;
    WW_S32(work, 0x3E0) = 0;
    WW_S32(work, 0x3E4) = 0;
    WW_S32(work, 0x3EC) = 0;
    WW_S32(work, 0x3E8) = 0;
    WW_S32(work, 0x3F0) = 0;
    winMakeEquipList(work);
}

// 288 B
void winMakeEquipList(WindowWork* work) {
    s32 count = 0;
    PouchData* pouch = pouchGetPtr();
    u8* entry = (u8*)work;
    s32 i;

    for (i = 0; i < MAX_BADGES; i++) {
        if (pouch->equippedBadges[i] != 0) {
            *(s32*)(entry + 0x404) = 1;
            count++;
            *(s32*)(entry + 0x408) = i;
            *(s32*)(entry + 0x40C) = pouch->equippedBadges[i];
            entry += 0xC;
        }
    }
    WW_S32(work, 0xD64) = count;
}

// 2496 B
// Every instruction is accounted for; the logic, constants, call arguments and control flow match (no switch,
// no jump table). The remaining diffs are compiler allocation only: frame 0x340 vs retail 0x330, callee-saved
// GPRs start at r14 vs retail r16, work lives in r15 where retail keeps it in r28 (i/ofs/entry likewise land in
// r20/r19/r18 instead of r29/r30/r31), the FP values are shifted down one slot (the inner-loop double is f29 vs
// retail f24), and the .rodata base pools as `...rodata.0` vs retail `vec3_802f4a38` (same address, different
// symbol). The inner-loop `winTexSet` constants are hoisted into registers where retail reloads them from .rodata
// and instead keeps the by-value argument addresses in r19/r20/r21. The compiler's int->float double also pools
// as an anonymous @NNN@ instead of double_to_int_802f4c60 (project-wide limitation). The by-value Vec arguments
// of winTexSet/winIconSet need the MUST_MATCH copies below, the same shape item_disp in win_item.c uses.
// NONMATCHING (94.13%): register/stack allocation only, see above.
#pragma optimize_for_size on // nudge the allocator toward retail's register assignment (see report)
void badge_disp(WindowWork* work, f32 x, f32 y) {
    GXColor iconColor = *(GXColor*)&dat_8041d358;
    GXColor color;
    GXColor black;
    GXColor white;
    f32 f21;
    f32 f29;
    f32 f28;
    f32 f27;
    f32 f26;
    f32 f25;
    f32 f24;
    s32 count;
    s32 i;
    s32 ofs;
    u8* entry;
    s32 gray;

    if (WW_S32(work, 0x3E0) == 0) {
        count = pouchGetHaveBadgeCnt();
    } else {
        count = WW_S32(work, 0xD64);
    }

    GXSetScissor(0, 0x77, 0x260, 0xB8);
    f25 = float_110_8041d370 + y;
    f29 = float_82_8041d374 + x;
    f28 = float_142_8041d378 + x;
    f27 = float_160_8041d37c + x;

    for (i = 0, ofs = 0, entry = (u8*)work; i < count; i++, ofs += 0x17, entry += 0xC) {
        s32 badge;
        s32 equipped;

        if (WW_S32(work, 0x3E0) == 0) {
            badge = pouchHaveBadge(i);
        } else {
            badge = *(s32*)(entry + 0x40C);
        }
        f24 = WW_F32(work, WW_S32(work, 0x3E0) * 4 + 0x3F4);
        if (WW_S32(work, 0x3E0) == 0) {
            equipped = pouchEquipCheckBadgeIndex(i);
        } else {
            equipped = *(s32*)(entry + 0x404);
        }
        f21 = f25 - (f32)ofs;
        f26 = f24 + f21;
        if (f26 > float_240_8041d380) {
            continue;
        }
        if (f26 < float_neg240_8041d384) {
            break;
        }

        if (equipped) {
            winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
            {
                Vec translate = vec3_802f4b10;
                translate.x = f29;
                translate.y = f26;
                winTexSet(0x1E, translate, vec3_802f4b1c, *(GXColor*)&dat_8041d35c);
            }
        }

        if (equipped == 0 && itemDataTable[badge].bp_cost > pouchGetPtr()->availableBP) {
            gray = 1;
            color = *(GXColor*)&dat_8041d360;
        } else {
            gray = 0;
            color = *(GXColor*)&dat_8041d364;
        }
        if (!gray) {
            winIconInit();
        } else {
            winIconGrayInit();
        }
        {
            GXColor ccopy = iconColor;
            Vec translate = vec3_802f4b28;
            Vec scale = vec3_802f4b34;
#ifdef MUST_MATCH
            Vec tcopy;
#endif
            translate.x = x - float_75_8041d388;
            translate.y = f26;
#ifdef MUST_MATCH
            tcopy = translate;
            winIconSet(*(u16*)&itemDataTable[badge].icon_id, &tcopy, &scale, &ccopy);
#else
            winIconSet(*(u16*)&itemDataTable[badge].icon_id, &translate, &scale, &iconColor);
#endif
        }
        winFontInit();
        {
            GXColor ccopy = color;
            Vec translate = vec3_802f4b40;
            Vec scale = vec3_802f4b4c;
            translate.x = x - float_55_8041d38c;
            translate.y = f24 + (float_12_8041d390 + f21);
            f21 = translate.y;
#ifdef MUST_MATCH
            {
                Vec tcopy = translate;
                winFontSet(&tcopy, &scale, &ccopy, itemDataTable[badge].id);
            }
#else
            winFontSet(&translate, &scale, &color, itemDataTable[badge].id);
#endif
        }
        {
            char buf[0x40];
            sprintf(buf, str_8041D394, winZenkakuStr(itemDataTable[badge].bp_cost));
            {
                GXColor ccopy = color;
                Vec translate = vec3_802f4b58;
                Vec scale = vec3_802f4b64;
                translate.x = f28 - (f32)((u16)FontGetMessageWidth(buf) / 2);
                translate.y = f21;
#ifdef MUST_MATCH
                {
                    Vec tcopy = translate;
                    winFontSet(&tcopy, &scale, &ccopy, buf);
                }
#else
                winFontSet(&translate, &scale, &color, buf);
#endif
            }
        }
        winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
        {
            s32 j;
            s32 jofs;
            for (j = 0, jofs = 0; j < itemDataTable[badge].bp_cost; j++, jofs += 0xD) {
                s32 tex = 0x1D;
                Vec translate = vec3_802f4b70;
                translate.x = f27 + (f32)jofs;
                translate.y = f26;
                if (equipped) {
                    tex = 0x1C;
                }
                winTexSet(tex, translate, vec3_802f4b7c, iconColor);
            }
        }
    }

    GXSetScissor(0, 0, 0x260, 0x1E0);
    white = *(GXColor*)&dat_8041d368;
    black = *(GXColor*)&dat_8041d36c;
    if (WW_S32(work, WW_S32(work, 0x3E0) * 4 + 0x3EC) > 0) {
        winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
        f21 = float_250_8041d398 + x;
        {
            Vec translate = vec3_802f4b88;
            Vec scale = vec3_802f4b94;
            translate.x = f21;
            translate.y = float_20_8041d39c + (float_18_8041d3a0 + f25);
            winTexSet(0x17, translate, scale, white);
        }
        winIconInit();
        {
            GXColor ccopy = white;
            Vec translate = vec3_802f4ba0;
            Vec scale = vec3_802f4bac;
#ifdef MUST_MATCH
            Vec tcopy;
#endif
            translate.x = f21;
            translate.y = float_20_8041d39c + (float_113_8041d3a4 + y);
#ifdef MUST_MATCH
            tcopy = translate;
            winIconSet(0x7E, &tcopy, &scale, &ccopy);
#else
            winIconSet(0x7E, &translate, &scale, &white);
#endif
        }
    }
    if (count > (WW_S32(work, WW_S32(work, 0x3E0) * 4 + 0x3EC) << 3) + 8) {
        winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
        f21 = float_250_8041d398 + x;
        {
            Vec translate = vec3_802f4bb8;
            Vec scale = vec3_802f4bc4;
            translate.x = f21;
            translate.y = float_20_8041d39c + ((y - float_90_8041d3a8) - float_15_8041d3ac);
            winTexSet(0x17, translate, scale, white);
        }
        winIconInit();
        {
            GXColor ccopy = white;
            Vec translate = vec3_802f4bd0;
            Vec scale = vec3_802f4bdc;
#ifdef MUST_MATCH
            Vec tcopy;
#endif
            translate.x = f21;
            translate.y = float_20_8041d39c + (y - float_90_8041d3a8);
#ifdef MUST_MATCH
            tcopy = translate;
            winIconSet(0x80, &tcopy, &scale, &ccopy);
#else
            winIconSet(0x80, &translate, &scale, &white);
#endif
        }
    }
    {
        const char* msg = msgSearch(str_802F4C30);
        f32 w = float_0p7_8041d3b0 * (f32)FontGetMessageWidth(msg);

        winFontInit();
        f21 = x + (float_220_8041d3b4 - w);
        {
            GXColor ccopy = black;
            Vec translate = vec3_802f4be8;
            Vec scale = vec3_802f4bf4;
            translate.x = f21;
            translate.y = float_20_8041d39c + ((y - float_92_8041d3b8) - float_10_8041d3bc);
#ifdef MUST_MATCH
            {
                Vec tcopy = translate;
                winFontSet(&tcopy, &scale, &ccopy, msgSearch(str_802F4C30));
            }
#else
            winFontSet(&translate, &scale, &black, msgSearch(str_802F4C30));
#endif
        }
        winIconInit();
        {
            GXColor ccopy = white;
            Vec translate = vec3_802f4c00;
            Vec scale = vec3_802f4c0c;
#ifdef MUST_MATCH
            Vec tcopy;
#endif
            translate.x = f21 - float_30_8041d3c0;
            translate.y = float_20_8041d39c + ((y - float_100_8041d3c4) - float_10_8041d3bc);
#ifdef MUST_MATCH
            tcopy = translate;
            winIconSet(0x217, &tcopy, &scale, &ccopy);
#else
            winIconSet(0x217, &translate, &scale, &white);
#endif
        }
    }
    {
        winFontInit();
        {
            GXColor ccopy = black;
            Vec translate = vec3_802f4c18;
            Vec scale = vec3_802f4c24;

            translate.x = float_120_8041d3c8 + x;
            translate.y = float_135_8041d3cc + y;
#ifdef MUST_MATCH
            {
                Vec tcopy = translate;
                winFontSet(&tcopy, &scale, &ccopy, msgSearch(str_802F4C48));
            }
#else
            winFontSet(&translate, &scale, &black, msgSearch(str_802F4C48));
#endif
        }
    }
}
#pragma optimize_for_size reset
