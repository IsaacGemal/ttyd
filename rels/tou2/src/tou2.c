#include "tou2.h"
#include "dolphin/types.h"
#include "cmath.h"
#include "driver/swdrv.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_cmd.h"
#include "memory.h"
#include "forcedata.h"
#include "battle/battle_monosiri.h"
#include "data/mapdata.h"
#include "data/npc_data.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "driver/msgdrv.h"
#include "driver/npcdrv.h"
#include "driver/windowdrv.h"
#include "eff/eff_starstone.h"
#include "manager/fontmgr.h"
#include "mario/mario_pouch.h"
#include "math_trig.h"
#include "pmario_sound.h"
#include "system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pool_data off
#pragma force_active on

typedef struct RankEntry {
    u16 flags;
    // pad 2 bytes
    s32 id;
    u8 points[2]; // TODO: see if there's a way to re-type this to s16 and keep rankingInit matching
    // pad 2 bytes
} RankEntry;

typedef enum RankFlags {
    RANK_BELOW = 1 << 0, // This fighter is below us in the rankings
    RANK_HIDDEN = 1 << 1, // This fighter is hidden from us
    RANK_PLAYER = 1 << 2, // This is the player
} RankFlags;

typedef struct RankData {
    s32 count;
    RankEntry* entries;
} RankData;

typedef struct FighterData {
    u16 flags;
    // pad 2 bytes
    const char* name;
    const char* title;
    s32 setupId;
    const char* taunt;
    const char* victory;
    s16 field_18; // priority, order, sortId?
    // pad 2 bytes
} FighterData;

// This includes a bunch of "story markers" for the various important fights
typedef enum FighterFlags {
    FIGHT_MAJOR = 1 << 0,
    FIGHT_MINOR = 1 << 1,
    FIGHT_CHAMPION = 1 << 2,
    FIGHT_PLAYER = 1 << 3,
    FIGHT_BANDITS = 1 << 8,
    FIGHT_KOOPAS = 1 << 9,
    FIGHT_GOOMBAS = 1 << 10,
    FIGHT_CLEFTS = 1 << 11,
    FIGHT_KOOPATROL = 1 << 12,
} FighterFlags;

typedef enum FighterId {
    FIGHTER_RAWK_HAWK = 0,
    FIGHTER_KOOPINATOR = 1,
    FIGHTER_CHOMPS = 2,
    FIGHTER_BROS = 3,
    FIGHTER_CRAW = 4,
    FIGHTER_MAGIKOOPA = 5,
    FIGHTER_FUZZY = 6,
    FIGHTER_SHADY_KOOPAS = 7,
    FIGHTER_BRISTLES = 8,
    FIGHTER_SPIKE_TOPS = 9,
    FIGHTER_IRON_CLEFTS = 10,
    FIGHTER_BANDITS = 11,
    FIGHTER_BOB_OMB = 12,
    FIGHTER_BALD_CLEFTS = 13,
    FIGHTER_BOGGLY = 14,
    FIGHTER_LAKITU = 15,
    FIGHTER_DULL_BONES = 16,
    FIGHTER_POKEYS = 17,
    FIGHTER_KP_KOOPAS = 18,
    FIGHTER_GOOMBAS = 19,
    FIGHTER_PLAYER = 20,
    FIGHTER_SWOOPS = 21,
    FIGHTER_SPINIA = 22,
} FighterId;

// local prototypes
static void rankingControll(void);
static void rankingInit(void);
static void rankingExit(void);
static void printRanking(void);

// clang-format off
extend_data(16,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(17,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// Note: static .bss objects are emitted in reverse declaration order by this compiler,
// so declare them rank_work, disp_work, champion_belt_onoff to match the original's layout.
static RankData rank_work;
static DispWork disp_work;
static s32 champion_belt_onoff;

FighterData fighterDt[23] = {
    { FIGHT_CHAMPION, "stg3_tou_sensyu_00", "stg3_tou_sensyu2_00", 22, "stg3_tou_400_02", "stg3_tou_400_03", 250 },                // Rawk Hawk
    { FIGHT_KOOPATROL | FIGHT_MAJOR, "stg3_tou_sensyu_01", "stg3_tou_sensyu2_01", 19, "stg3_tou_118_00", "stg3_tou_118_01", 200 }, // The Koopinator
    { FIGHT_MAJOR, "stg3_tou_sensyu_02", "stg3_tou_sensyu2_02", 18, "stg3_tou_118_02", "stg3_tou_118_03", 180 },                   // Chomp Country
    { FIGHT_MAJOR, "stg3_tou_sensyu_03", "stg3_tou_sensyu2_03", 17, "stg3_tou_118_04", "stg3_tou_118_05", 170 },                   // Hamma, Bamma, and Flare
    { FIGHT_MAJOR, "stg3_tou_sensyu_04", "stg3_tou_sensyu2_04", 16, "stg3_tou_118_06", "stg3_tou_118_07", 160 },                   // Craw-Daddy
    { FIGHT_MAJOR, "stg3_tou_sensyu_05", "stg3_tou_sensyu2_05", 15, "stg3_tou_118_08", "stg3_tou_118_09", 150 },                   // The Magikoopa Masters
    { FIGHT_MAJOR, "stg3_tou_sensyu_06", "stg3_tou_sensyu2_06", 14, "stg3_tou_118_10", "stg3_tou_118_11", 140 },                   // The Fuzz
    { FIGHT_MAJOR, "stg3_tou_sensyu_07", "stg3_tou_sensyu2_07", 13, "stg3_tou_118_12", "stg3_tou_118_13", 130 },                   // The Shellshockers
    { FIGHT_MAJOR, "stg3_tou_sensyu_08", "stg3_tou_sensyu2_08", 12, "stg3_tou_118_14", "stg3_tou_118_15", 120 },                   // The Poker Faces
    { FIGHT_MAJOR, "stg3_tou_sensyu_09", "stg3_tou_sensyu2_09", 11, "stg3_tou_118_16", "stg3_tou_118_17", 110 },                   // The Tiny Spinies
    { FIGHT_CLEFTS | FIGHT_MAJOR, "stg3_tou_sensyu_10", "stg3_tou_sensyu2_10", 10, "stg3_tou_118_18", "stg3_tou_118_19", 100 },    // The Armored Harriers
    { FIGHT_BANDITS | FIGHT_MINOR, "stg3_tou_sensyu_11", "stg3_tou_sensyu2_11", 9, "stg3_tou_118_20", "stg3_tou_118_21", 90 },     // The Hand-It-Overs
    { FIGHT_MINOR, "stg3_tou_sensyu_12", "stg3_tou_sensyu2_12", 8, "stg3_tou_118_22", "stg3_tou_118_23", 80 },                     // The Bob-omb Squad
    { FIGHT_MINOR, "stg3_tou_sensyu_13", "stg3_tou_sensyu2_13", 7, "stg3_tou_118_24", "stg3_tou_118_25", 70 },                     // The Punk Rocks
    { FIGHT_MINOR, "stg3_tou_sensyu_14", "stg3_tou_sensyu2_14", 6, "stg3_tou_118_26", "stg3_tou_118_27", 60 },                     // The Mind-Bogglers
    { FIGHT_MINOR, "stg3_tou_sensyu_15", "stg3_tou_sensyu2_15", 5, "stg3_tou_118_28", "stg3_tou_118_29", 50 },                     // Spike Storm
    { FIGHT_MINOR, "stg3_tou_sensyu_16", "stg3_tou_sensyu2_16", 4, "stg3_tou_118_30", "stg3_tou_118_31", 40 },                     // The Dead Bones
    { FIGHT_MINOR, "stg3_tou_sensyu_17", "stg3_tou_sensyu2_17", 3, "stg3_tou_118_32", "stg3_tou_118_33", 30 },                     // The Pokey Triplets
    { FIGHT_KOOPAS | FIGHT_MINOR, "stg3_tou_sensyu_18", "stg3_tou_sensyu2_18", 2, "stg3_tou_118_34", "stg3_tou_118_35", 20 },      // The KP Koopas
    { FIGHT_GOOMBAS | FIGHT_MINOR, "stg3_tou_sensyu_19", "stg3_tou_sensyu2_19", 1, "stg3_tou_118_36", "stg3_tou_118_36_01", 10 },  // The Goomba Bros.
    { FIGHT_PLAYER, "stg3_tou_sensyu_20", "stg3_tou_sensyu2_20", 1, NULL, NULL, 0 },                                               // The Great Gonzales
    { FIGHT_MINOR, "stg3_tou_sensyu_21", "stg3_tou_sensyu2_21", 20, "stg3_tou_118_36_02", "stg3_tou_118_36_03", 5 },               // Wings of Night
    { FIGHT_MINOR, "stg3_tou_sensyu_22", "stg3_tou_sensyu2_22", 21, "stg3_tou_118_36_04", "stg3_tou_118_36_05", 0 },               // The Destructors
};

FighterData fighterDt_re[21] = {
    { FIGHT_CHAMPION, "stg3_tou_re_315", "stg3_tou_re_338", 22, "stg3_tou_re_289", "stg3_tou_re_290", 250 },                // Rawk Hawk
    { FIGHT_KOOPATROL | FIGHT_MAJOR, "stg3_tou_re_316", "stg3_tou_re_339", 19, "stg3_tou_re_179", "stg3_tou_re_180", 200 }, // The Koopinator
    { FIGHT_MAJOR, "stg3_tou_re_317", "stg3_tou_re_340", 18, "stg3_tou_re_181", "stg3_tou_re_182", 180 },                   // Chomp Country
    { FIGHT_MAJOR, "stg3_tou_re_318", "stg3_tou_re_341", 17, "stg3_tou_re_183", "stg3_tou_re_184", 170 },                   // Hamma, Bamma, and Flare
    { FIGHT_MAJOR, "stg3_tou_re_319", "stg3_tou_re_342", 16, "stg3_tou_re_185", "stg3_tou_re_186", 160 },                   // Craw-Daddy
    { FIGHT_MAJOR, "stg3_tou_re_320", "stg3_tou_re_343", 15, "stg3_tou_re_187", "stg3_tou_re_188", 150 },                   // The Magikoopa Masters
    { FIGHT_MAJOR, "stg3_tou_re_321", "stg3_tou_re_344", 14, "stg3_tou_re_189", "stg3_tou_re_190", 140 },                   // The Fuzz
    { FIGHT_MAJOR, "stg3_tou_re_322", "stg3_tou_re_345", 13, "stg3_tou_re_191", "stg3_tou_re_192", 130 },                   // The Shellshockers
    { FIGHT_MAJOR, "stg3_tou_re_323", "stg3_tou_re_346", 12, "stg3_tou_re_193", "stg3_tou_re_194", 120 },                   // The Poker Faces
    { FIGHT_MAJOR, "stg3_tou_re_324", "stg3_tou_re_347", 11, "stg3_tou_re_195", "stg3_tou_re_196", 110 },                   // The Tiny Spinies
    { FIGHT_CLEFTS | FIGHT_MAJOR, "stg3_tou_re_325", "stg3_tou_re_348", 10, "stg3_tou_re_197", "stg3_tou_re_198", 100 },    // The Armored Harriers
    { FIGHT_BANDITS | FIGHT_MINOR, "stg3_tou_re_326", "stg3_tou_re_349", 9, "stg3_tou_re_199", "stg3_tou_re_200", 90 },     // The Hand-It-Overs
    { FIGHT_MINOR, "stg3_tou_re_327", "stg3_tou_re_350", 8, "stg3_tou_re_201", "stg3_tou_re_202", 80 },                     // The Bob-omb Squad
    { FIGHT_MINOR, "stg3_tou_re_328", "stg3_tou_re_351", 7, "stg3_tou_re_203", "stg3_tou_re_204", 70 },                     // The Punk Rocks
    { FIGHT_MINOR, "stg3_tou_re_329", "stg3_tou_re_352", 6, "stg3_tou_re_205", "stg3_tou_re_206", 60 },                     // The Mind-Bogglers
    { FIGHT_MINOR, "stg3_tou_re_330", "stg3_tou_re_353", 5, "stg3_tou_re_207", "stg3_tou_re_208", 50 },                     // Spike Storm
    { FIGHT_MINOR, "stg3_tou_re_331", "stg3_tou_re_354", 4, "stg3_tou_re_209", "stg3_tou_re_210", 40 },                     // The Dead Bones
    { FIGHT_MINOR, "stg3_tou_re_332", "stg3_tou_re_355", 3, "stg3_tou_re_211", "stg3_tou_re_212", 30 },                     // The Pokey Triplets
    { FIGHT_KOOPAS | FIGHT_MINOR, "stg3_tou_re_333", "stg3_tou_re_356", 2, "stg3_tou_re_213", "stg3_tou_re_214", 20 },      // The KP Koopas
    { FIGHT_GOOMBAS | FIGHT_MINOR, "stg3_tou_re_334", "stg3_tou_re_357", 1, "stg3_tou_re_215", "stg3_tou_re_216", 10 },     // The Goomba Bros.
    { FIGHT_PLAYER, "stg3_tou_re_335", "stg3_tou_re_358", 1, NULL, NULL, 0 },                                               // The Great Gonzales
};

RuleEntry ruleDt_2bu[10] = {
    { "stg3_tou_107_01", "stg3_tou_cp_000", 0x12, 3, 3 },
    { "stg3_tou_107_02", "stg3_tou_cp_001", 0x01, 1, 1 },
    { "stg3_tou_107_03", "stg3_tou_cp_002", 0x03, 1, 1 },
    { "stg3_tou_107_04", "stg3_tou_cp_003", 0x30, 1, 1 },
    { "stg3_tou_107_05", "stg3_tou_cp_004", 0x05, 1, 1 },
    { "stg3_tou_107_06", "stg3_tou_cp_005", 0x1A, 1, 1 },
    { "stg3_tou_107_07", "stg3_tou_cp_006", 0x20, 1, 1 },
    { "stg3_tou_107_08", "stg3_tou_cp_007", 0x3B, 5, 5 },
    { "stg3_tou_107_12", "stg3_tou_cp_008", 0x06, 1, 1 },
    { "stg3_tou_107_16", "stg3_tou_cp_009", 0x2B, 1, 1 },
};

RuleEntry ruleDt_1bu[14] = {
    { "stg3_tou_107_01", "stg3_tou_cp_010", 0x12, 5, 5 },
    { "stg3_tou_107_02", "stg3_tou_cp_011", 0x01, 1, 1 },
    { "stg3_tou_107_03", "stg3_tou_cp_012", 0x03, 1, 1 },
    { "stg3_tou_107_04", "stg3_tou_cp_013", 0x30, 1, 1 },
    { "stg3_tou_107_05", "stg3_tou_cp_014", 0x05, 1, 1 },
    { "stg3_tou_107_06", "stg3_tou_cp_015", 0x1A, 1, 1 },
    { "stg3_tou_107_07", "stg3_tou_cp_016", 0x20, 1, 1 },
    { "stg3_tou_107_08", "stg3_tou_cp_017", 0x3B, 5, 5 },
    { "stg3_tou_107_12", "stg3_tou_cp_019", 0x06, 1, 1 },
    { "stg3_tou_107_13", "stg3_tou_cp_020", 0x0B, 0x14, 0x14 },
    { "stg3_tou_107_16", "stg3_tou_cp_021", 0x2B, 3, 3 },
    { "stg3_tou_107_17", "stg3_tou_cp_022", 0x41, 3, 3 },
    { "stg3_tou_107_19", "stg3_tou_cp_023", 0x40, 0xFF, 0xFF },
    { "stg3_tou_107_20", "stg3_tou_cp_024", 0x3F, 0xFF, 0xFF },
};

RuleEntry ruleDt_re[14] = {
    { "stg3_tou_re_362", "stg3_tou_cp_040", 0x12, 5, 5 },
    { "stg3_tou_re_363", "stg3_tou_cp_041", 0x01, 1, 1 },
    { "stg3_tou_re_364", "stg3_tou_cp_042", 0x03, 1, 1 },
    { "stg3_tou_re_365", "stg3_tou_cp_043", 0x30, 1, 1 },
    { "stg3_tou_re_366", "stg3_tou_cp_044", 0x05, 1, 1 },
    { "stg3_tou_re_367", "stg3_tou_cp_045", 0x1A, 1, 1 },
    { "stg3_tou_re_368", "stg3_tou_cp_046", 0x20, 1, 1 },
    { "stg3_tou_re_369", "stg3_tou_cp_047", 0x3B, 5, 5 },
    { "stg3_tou_re_371", "stg3_tou_cp_049", 0x06, 1, 1 },
    { "stg3_tou_re_372", "stg3_tou_cp_050", 0x0B, 0x14, 0x14 },
    { "stg3_tou_re_373", "stg3_tou_cp_051", 0x2B, 3, 3 },
    { "stg3_tou_re_374", "stg3_tou_cp_052", 0x41, 3, 3 },
    { "stg3_tou_re_375", "stg3_tou_cp_053", 0x40, 0xFF, 0xFF },
    { "stg3_tou_re_376", "stg3_tou_cp_054", 0x3F, 0xFF, 0xFF },
};

static RankData* rank_wp = &rank_work;
static DispWork* dwp = &disp_work;
static const char* suuji[] = { "０", "１", "２", "３", "４", "５", "６", "７", "８", "９" };
// clang-format on

// The area-name literal is padded with four zero bytes so the compiler's last
// .rodata object ends exactly where the original object's does.
void _prolog(void) {
    voidfunctionptr* ctor;

    for (ctor = _ctors; *ctor != NULL; ctor++) {
        (*ctor)();
    }
    relSetEvtAddr("tou_03", tou_03_init_evt);
    relSetBtlAddr("tou\0\0\0\0", btlsetup_tou_tbl, btl_tou_setup_no_tbl);
    rankingInit();
}

void _epilog(void) {
    voidfunctionptr* dtor;

    rankingExit();
    relSetEvtAddr("tou_03", NULL);
    relSetBtlAddr("tou\0\0\0\0", NULL, NULL);
    for (dtor = _dtors; *dtor != NULL; dtor++) {
        (*dtor)();
    }
}

void _unresolved(void) {
    (void)0; // bare blr; the null statement keeps lint_patch from flagging an empty stub
}

// dont_inline: rankingInit is called from _prolog and must stay a real call
#pragma dont_inline on
static void rankingInit(void) {
    s32 temp;
    s32 count;
    int i;
    RankEntry* entry;

    memset(rank_wp, 0, sizeof(RankData));
    // We need to handle the excluded entries if we're doing rematches
    temp = evtGetValue(NULL, GSW(0));
    count = 21;
    if (temp < 172) {
        count = 23;
    }
    rank_wp->count = count;
    rank_wp->entries = _mapAlloc(rank_wp->count * sizeof(RankEntry));
    memset(rank_wp->entries, 0, rank_wp->count * sizeof(RankEntry));

    for (i = 0, entry = rank_wp->entries; i < rank_wp->count; i++, entry++) {
        entry->flags = 0;
        entry->id = i;
        if (swGet(entry->id + 2465)) {
            entry->flags |= RANK_BELOW;
        }
        if (swGet(2529)) {
            int j;
            for (j = 0; j < 2; j++) {
                entry->points[j] = swByteGet(2 * entry->id + 519 + j);
            }
        } else {
            *(s16*)entry->points = fighterDt[entry->id].field_18;
        }
        if (entry->id == FIGHTER_PLAYER) {
            entry->flags |= RANK_PLAYER;
        }
    }

    swSet(2529);

    // Set up the initial run-through
    if (evtGetValue(NULL, GSW(0)) < 172) {
        rank_wp->entries[FIGHTER_SWOOPS].flags |= RANK_HIDDEN;
        rank_wp->entries[FIGHTER_SPINIA].flags |= RANK_HIDDEN;

        // If we're past this point in the story, hide King K
        if (evtGetValue(NULL, GSWF(2412))) {
            rank_wp->entries[FIGHTER_KP_KOOPAS].flags |= RANK_HIDDEN;
        }

        // If we're past this story, Sir Swoop has been introduced
        if (evtGetValue(NULL, GSWF(2413))) {
            rank_wp->entries[FIGHTER_SWOOPS].flags &= ~RANK_HIDDEN;

            if (!evtGetValue(NULL, GSWF(2530))) {
                rank_wp->entries[FIGHTER_SWOOPS].flags |= RANK_BELOW;
            }
            evtSetValue(NULL, GSWF(2530), TRUE);
        }

        // If we're past this point in the story, Bandy Andy got "retired" and The Destructors were added.
        if (evtGetValue(NULL, GSWF(2421))) {
            rank_wp->entries[FIGHTER_BANDITS].flags |= RANK_HIDDEN;
            rank_wp->entries[FIGHTER_SPINIA].flags &= ~RANK_HIDDEN;

            if (!evtGetValue(NULL, GSWF(2531))) {
                rank_wp->entries[FIGHTER_SPINIA].flags |= RANK_BELOW;
            }
            evtSetValue(NULL, GSWF(2531), TRUE);
        }
    }
    rankingControll();
}

static void rankingExit(void) {
    s32 i;
    s32 j;
    RankEntry* e = rank_wp->entries;

    for (i = 0; i < rank_wp->count; i++, e++) {
        if (e->flags & RANK_BELOW) {
            swSet(e->id + 0x9a1);
        } else {
            swClear(e->id + 0x9a1);
        }
        for (j = 0; j < 2; j++) {
            swByteSet(e->id * 2 + 0x207 + j, ((u8*)e)[8 + j]);
        }
    }
    _mapFree(rank_wp->entries);
}
#pragma dont_inline reset

static USER_FUNC(evt_rankingReInit) {
    rankingExit();
    rankingInit();
    return EVT_RETURN_DONE;
}

USER_FUNC(unk_tou2_00004b50) {
    s32 i;

    rankingExit();
    pouchRemoveItem(0x41);
    for (i = 0; i < 0x14; i++) {
        swClear(i + 0x9a1);
    }
    swClear(0x958);
    swClear(0x95f);
    swClear(0x960);
    swClear(0x94f);
    swClear(0x9e1);
    swClear(0x9e4);
    swClear(0x963);
    rankingInit();
    return EVT_RETURN_DONE;
}

// dont_inline: insert is called from rankingControll and must stay a real call
#pragma dont_inline on
static void insert(s32* arr, s32 n, s32 from, s32 to) {
    s32 i;

    if (from < to) {
        s32 tmp = arr[from];
        for (i = from; i < to; i++) {
            arr[i] = arr[i + 1];
        }
        arr[to] = tmp;
    } else {
        s32 tmp = arr[from];
        for (i = from; i > to; i--) {
            arr[i] = arr[i - 1];
        }
        arr[to] = tmp;
    }
}
#pragma dont_inline reset

static void rankingControll(void) {
    s32 above[32];
    s32 below[32];
    s32 nAbove;
    s32 nBelow;
    s32 i;
    s32 j;
    s32 tmp;
    s32 idx;
    s32 idx2;
    s32 firstIdx;
    s32 rank;
    RankEntry* entry;
    RankEntry* e;

    nAbove = 0;
    nBelow = 0;
    for (idx = 0, e = rank_wp->entries; idx < rank_wp->count; idx++, e++) {
        if (!(e->flags & RANK_PLAYER)) {
            if (e->flags & RANK_BELOW) {
                below[nBelow] = idx;
                nBelow++;
            } else {
                above[nAbove] = idx;
                nAbove++;
            }
        }
    }

    if (nAbove > 0) {
        for (i = 0; i < nAbove - 1; i++) {
            for (j = i + 1; j < nAbove; j++) {
                if (*(s16*)rank_wp->entries[above[i]].points < *(s16*)rank_wp->entries[above[j]].points) {
                    tmp = above[i];
                    above[i] = above[j];
                    above[j] = tmp;
                }
            }
        }

        for (i = 0; i < nAbove - 1; i++) {
            for (j = i + 1; j < nAbove; j++) {
                FighterData* fj = &fighterDt[rank_wp->entries[above[j]].id];
                FighterData* fi = &fighterDt[rank_wp->entries[above[i]].id];
                if ((fj->flags & 5) && (fi->flags & 2)) {
                    tmp = above[i];
                    above[i] = above[j];
                    above[j] = tmp;
                }
            }
        }

        if (nAbove > 0) {
            for (idx = 0; idx < nAbove; idx++) {
                if (fighterDt[rank_wp->entries[above[idx]].id].flags & 4) {
                    break;
                }
            }
            if (idx < nAbove) {
                insert(above, nAbove, idx, 0);
            }
        }

        if (nAbove > 1) {
            for (idx = 0; idx < nAbove; idx++) {
                if (fighterDt[rank_wp->entries[above[idx]].id].flags & 0x1000) {
                    break;
                }
            }
            if (idx < nAbove) {
                insert(above, nAbove, idx, 1);
            }
        }

        if (nAbove > 10) {
            for (idx = 0; idx < nAbove; idx++) {
                if (fighterDt[rank_wp->entries[above[idx]].id].flags & 0x800) {
                    break;
                }
            }
            if (idx < nAbove) {
                insert(above, nAbove, idx, 10);
            }
        }

        if (nAbove > 18) {
            for (idx = 0; idx < nAbove; idx++) {
                if (fighterDt[rank_wp->entries[above[idx]].id].flags & 0x200) {
                    break;
                }
            }
            if (idx < nAbove) {
                insert(above, nAbove, idx, 18);
            }
        }

        if (nAbove > 19) {
            for (idx = 0; idx < nAbove; idx++) {
                if (fighterDt[rank_wp->entries[above[idx]].id].flags & 0x400) {
                    break;
                }
            }
            if (idx < nAbove) {
                insert(above, nAbove, idx, 19);
            }
        }

        if (nAbove > 16) {
            for (idx = 0; idx < nAbove; idx++) {
                if (fighterDt[rank_wp->entries[above[idx]].id].flags & 0x100) {
                    break;
                }
            }
            if (idx < nAbove) {
                insert(above, nAbove, idx, 14);
            }
        }

        // Check if a match is booked (2532), and move our opponent (507) to above us for the fight.
        if (swGet(2532)) {
            for (idx = 0; idx < nAbove; idx++) {
                if (above[idx] == swByteGet(507)) {
                    break;
                }
            }
            if (idx < nAbove) {
                insert(above, nAbove, idx, nAbove - 1);
            }
        }
    }

    if (nBelow > 0) {
        for (i = 0; i < nBelow - 1; i++) {
            for (j = i + 1; j < nBelow; j++) {
                if (*(s16*)rank_wp->entries[below[i]].points < *(s16*)rank_wp->entries[below[j]].points) {
                    tmp = below[i];
                    below[i] = below[j];
                    below[j] = tmp;
                }
            }
        }

        for (i = 0; i < nBelow - 1; i++) {
            for (j = i + 1; j < nBelow; j++) {
                FighterData* fj = &fighterDt[rank_wp->entries[below[j]].id];
                FighterData* fi = &fighterDt[rank_wp->entries[below[i]].id];
                if ((fj->flags & 5) && (fi->flags & 2)) {
                    tmp = below[i];
                    below[i] = below[j];
                    below[j] = tmp;
                }
            }
        }

        if (nBelow > 0) {
            for (idx = 0; idx < nBelow; idx++) {
                if (fighterDt[rank_wp->entries[below[idx]].id].flags & 4) {
                    break;
                }
            }
            if (idx < nBelow) {
                insert(below, nBelow, idx, 0);
            }
        }

        if (nBelow > 0) {
            for (idx = 0; idx < nBelow; idx++) {
                if (fighterDt[rank_wp->entries[below[idx]].id].flags & 0x800) {
                    break;
                }
            }
            if (idx < nBelow) {
                // the original reuses the same counter for the second search and saves the first
                // hit first; this is what produces the `mr` before the second loop
                firstIdx = idx;
                for (idx = 0; idx < nBelow; idx++) {
                    if (fighterDt[rank_wp->entries[below[idx]].id].flags & 2) {
                        break;
                    }
                }
                if (idx < nBelow && idx > 0) {
                    insert(below, nBelow, firstIdx, idx - 1);
                }
            }
        }

        // Check if a match is booked (2532), and move our opponent (507) to above us for the fight.
        if (swGet(2532)) {
            for (idx = 0; idx < nBelow; idx++) {
                if (below[idx] == swByteGet(507)) {
                    break;
                }
            }
            if (idx < nBelow) {
                insert(below, nBelow, idx, 0);
            }
        }
    }

    entry = (RankEntry*)__memAlloc(0, sizeof(RankEntry) * rank_wp->count);
    for (i = 0; i < nAbove; i++) {
        entry[i] = rank_wp->entries[above[i]];
    }
#ifdef MUST_MATCH
    // MUST_MATCH: the original's flags store compiles to an indexed `sthx` at entry + nAbove*12;
    // routing it through a u16 view (flags is at offset 0) reproduces that, while id/points go
    // through a materialised RankEntry*.
    ((u16*)entry)[nAbove * 6] = RANK_PLAYER;
    {
        RankEntry* p = entry + nAbove;
        p->id = FIGHTER_PLAYER;
        *(s16*)p->points = 0;
    }
#else
    entry[nAbove].flags = RANK_PLAYER;
    entry[nAbove].id = FIGHTER_PLAYER;
    *(s16*)entry[nAbove].points = 0;
#endif
    for (i = 0; i < nBelow; i++) {
#ifdef MUST_MATCH
        // MUST_MATCH: MWCC reassociates the natural `nAbove + i + 1` to `nAbove + (i + 1)`; writing
        // the `+ 1` first makes it emit the original's `(nAbove + i) + 1` grouping instead.
        j = 1 + nAbove + i;
#else
        j = nAbove + i + 1;
#endif
        entry[j] = rank_wp->entries[below[i]];
    }
    memcpy(rank_wp->entries, entry, sizeof(RankEntry) * rank_wp->count);
    __memFree(0, entry);

    // Calculate our current rank and store it in 580.
    rank = 0;
    for (i = 0; i < rank_wp->count; i++) {
        if (!(rank_wp->entries[i].flags & RANK_HIDDEN)) {
            if (rank_wp->entries[i].flags & RANK_PLAYER) {
                break;
            }
            rank++;
        }
    }
    swByteSet(580, rank);
}

USER_FUNC(evt_tou_get_ranking_28_text_4048) {
    s32* args = event->args;
    s32 count = 0;
    s32 i;

    for (i = 0; i < rank_wp->count; i++) {
        u16 f = rank_wp->entries[i].flags;

        if (!(f & RANK_HIDDEN)) {
            if (f & RANK_PLAYER) {
                break;
            }
            count++;
        }
    }
    evtSetValue(event, args[0], count);
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_tou_get_next_match) {
    s32* args = event->args;
    s32 x = evtGetValue(event, 0xf840650b);
    s32 list[32];
    s32 i;
    s32 cnt = 0;
    s32 c;

    for (i = 0; i < rank_wp->count; i++) {
        if (!(rank_wp->entries[i].flags & RANK_HIDDEN)) {
            list[cnt++] = rank_wp->entries[i].id;
        }
    }
    c = 0;
    for (i = 0; i < rank_wp->count; i++) {
        u16 f = rank_wp->entries[i].flags;

        if (!(f & RANK_HIDDEN)) {
            if (f & RANK_PLAYER) {
                break;
            }
            c++;
        }
    }
    if (c > 0) {
        if (x != 0) {
            evtSetValue(event, args[0], list[c - 1]);
        } else if (c + 1 < cnt) {
            evtSetValue(event, args[0], list[c + 1]);
        } else {
            evtSetValue(event, args[0], list[c - 1]);
        }
    } else {
        evtSetValue(event, args[0], list[1]);
    }
    printRanking();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_tou_get_fighter_info_28_text_3E58) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    FighterData* dt;

    if (evtGetValue(NULL, GSW(0)) < 172) {
        dt = fighterDt;
    } else {
        dt = fighterDt_re;
    }
    evtSetValue(event, args[1], (s32)msgSearch(dt[idx].name));
    evtSetValue(event, args[2], fighterDt[idx].setupId);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_tou_get_fighter_name2_28_text_3DD4) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    FighterData* dt;

    if (evtGetValue(NULL, GSW(0)) < 172) {
        dt = fighterDt;
    } else {
        dt = fighterDt_re;
    }
    evtSetValue(event, args[1], (s32)msgSearch(dt[idx].title));
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_tou_get_fighter_rank) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    s32 rank = 0;
    s32 i;

    for (i = 0; i < rank_wp->count; i++) {
        if (!(rank_wp->entries[i].flags & RANK_HIDDEN)) {
            if (rank_wp->entries[i].id == id) {
                break;
            }
            rank++;
        }
    }
    evtSetValue(event, args[1], rank);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_tou_get_fighter_battle_msg_28_text_3CB4) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    FighterData* dt;

    if (evtGetValue(NULL, GSW(0)) < 172) {
        dt = fighterDt;
    } else {
        dt = fighterDt_re;
    }
    evtSetValue(event, args[1], (s32)dt[idx].taunt);
    evtSetValue(event, args[2], (s32)dt[idx].victory);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_tou_get_fighter_num) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);

    evtSetValue(event, args[1],
                btlsetup_tou_tbl[fighterDt[idx].setupId].offLoadouts->setup->numEnemies);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_tou_get_fighter_tribe) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    s32 variant = evtGetValue(event, args[1]);
    s32 setupId = fighterDt[idx].setupId;
    TattleEntry* tattle;
    NpcTribeInfo* tribe;
    const char* name;
    s32 found;

    tattle = battleGetUnitMonosiriPtr(
        btlsetup_tou_tbl[setupId].offLoadouts->setup->enemyData[variant].mUnitKindParams->mUnitType);
    found = 0;
    tribe = npcTribe;
    while (tribe->nameJp != NULL) {
        if (strcmp(tribe->modelName, tattle->modelName) == 0) {
            found = 1;
            break;
        }
        tribe++;
    }
    if (!found) {
        name = "クリボー";
        tribe = npcGetTribe(name);
    } else {
        name = tribe->nameJp;
    }
    if (setupId == 2 && variant == 1) {
        name = "パタファイター";
        tribe = npcGetTribe(name);
    }
    if (setupId == 10 && variant == 1) {
        name = "アイアンシンエモン2";
        tribe = npcGetTribe(name);
    }
    if (setupId == 13 && variant == 1) {
        name = "ウラパタ";
        tribe = npcGetTribe(name);
    }
    if (setupId == 15 && variant == 2) {
        name = "カメック緑空";
        tribe = npcGetTribe(name);
    }
    if (setupId == 11) {
        name = "ハイパートゲメット";
        tribe = npcGetTribe(name);
    }
    evtSetValue(event, args[2], (s32)tribe->modelName);
    evtSetValue(event, args[3], (s32)name);
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_tou_get_rule_info) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 n;
    RuleEntry* dt;
    s32 i;

    if (evtGetValue(NULL, GSW(0)) < 172) {
        s32 count = 0;

        for (i = 0; i < rank_wp->count; i++) {
            u16 f = rank_wp->entries[i].flags;

            if (!(f & RANK_HIDDEN)) {
                if (f & RANK_PLAYER) {
                    break;
                }
                count++;
            }
        }
        if (count > 10) {
            dt = ruleDt_2bu;
            n = 10;
        } else {
            dt = ruleDt_1bu;
            n = 14;
        }
    } else {
        dt = ruleDt_re;
        n = 14;
    }

    if (type == 0) {
        for (i = 0; i < n; i++) {
            if (dt[i].field_08 == 6) {
                break;
            }
        }
    } else if (type == 10) {
        for (;;) {
            i = rand() % n;
            if (dt[i].field_08 != 0x30 && dt[i].field_08 != 0x20 && dt[i].field_08 != 0x40) {
                break;
            }
        }
    } else {
        i = rand() % n;
    }

    evtSetValue(event, args[1], (s32)dt[i].field_00);
    evtSetValue(event, args[2], dt[i].field_08);
    evtSetValue(event, args[3], dt[i].field_0C);
    evtSetValue(event, args[4], dt[i].field_10);
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_tou_get_rule_msg2) {
    s32* args = event->args;
    s32 ruleId = evtGetValue(event, args[0]);
    s32 n;
    RuleEntry* dt;
    s32 i;

    if (evtGetValue(NULL, GSW(0)) < 172) {
        s32 count = 0;

        for (i = 0; i < rank_wp->count; i++) {
            u16 f = rank_wp->entries[i].flags;

            if (!(f & RANK_HIDDEN)) {
                if (f & RANK_PLAYER) {
                    break;
                }
                count++;
            }
        }
        if (count > 10) {
            dt = ruleDt_2bu;
            n = 10;
        } else {
            dt = ruleDt_1bu;
            n = 14;
        }
    } else {
        dt = ruleDt_re;
        n = 14;
    }

    for (i = 0; i < n; i++) {
        if (ruleId == dt[i].field_08) {
            break;
        }
    }
    if (i == n) {
        i = n - 1;
    }
    evtSetValue(event, args[1], (s32)msgSearch(dt[i].msg));
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_tou_set_ranking_win_lose_28_text_35C0) {
    s32* args = event->args;
    s32 a = evtGetValue(event, args[0]);
    s32 id = evtGetValue(event, args[1]);
    s32 i;
    s16 min;
    s16 max;

    if (a == 0) {
        for (i = 0; i < rank_wp->count; i++) {
            if (rank_wp->entries[i].id == id) {
                rank_wp->entries[i].flags |= RANK_BELOW;
                break;
            }
        }
        swSet(0x98a);
    } else {
        for (i = 0; i < rank_wp->count; i++) {
            if (rank_wp->entries[i].id == id) {
                rank_wp->entries[i].flags &= ~RANK_BELOW;
                break;
            }
        }
        swClear(0x98a);
    }
    min = 0x3e8;
    max = -0x3e8;
    for (i = 0; i < rank_wp->count; i++) {
        if (!(rank_wp->entries[i].flags & RANK_HIDDEN)) {
            s32 d = 0;

            d += rand() % 4;
            d += rand() % 4;

            *(s16*)rank_wp->entries[i].points += d - 3;
            if (*(s16*)rank_wp->entries[i].points < min) {
                min = *(s16*)rank_wp->entries[i].points;
            }
            if (*(s16*)rank_wp->entries[i].points > max) {
                max = *(s16*)rank_wp->entries[i].points;
            }
        }
    }
    swByteSet(0x1fb, id);
    swSet(0x9e4);
    rankingControll();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_tou_set_rule_result_28_text_355C) {
    s32 v = evtGetValue(event, event->args[0]);

    if (v & 4) {
        swSet(0x98b);
    } else if (v & 2) {
        swClear(0x98b);
    } else if (v & 1) {
        swClear(0x98b);
    }
    return EVT_RETURN_DONE;
}

// dont_inline: printRanking is called from evt_tou_get_next_match and must stay a real call
#pragma dont_inline on
static void printRanking(void) {
    s32 i;

    evtGetValue(NULL, GSW(0));
    for (i = 0; i < rank_wp->count; i++) {
    }
}
#pragma dont_inline reset

static void disp_proc(CameraId cameraId, void* param) {
    FighterData* dt;
    s32 i;
    s32 rank;
    s32 y;
    s32 off;
    f32 f31;
    f32 f30;
    f32 f28;
    f32 f27;
    f32 f26;
    Mtx scale;
    Mtx mtx;
    char buf[0x100];
    u16 scX;
    u16 scW;
    f32 c250;

    if (evtGetValue(NULL, GSW(0)) < 172) {
        dt = fighterDt;
    } else {
        dt = fighterDt_re;
    }
    {
        f32 t = 250.0f;
        f32 h = 0.5f;

        f31 = -t * h;
    }
    windowDispGX_System(0, 0xff, f31 - 30.0f, 165.0f, 310.0f, 330.0f);
    FontDrawStart();
    FontDrawScale(1.0f);
    i = FontGetMessageWidth("ランキング");
    {
        GXColor color = { 0xDC, 0xC8, 0xDC, 0xFF };

        FontDrawColor(&color);
    }
    // The angles are read from float locals so they pool with disp_proc's other
    // code constants; as block-scope compound-literal templates they pooled early.
    {
        f32 f135 = 135.0f;
        FontDrawMessage(-i / 2, (s32)f135, "ランキング");
    }
    c250 = 250.0f;
    scX = (u16)(304.0f + f31);
    scW = (u16)c250;
    {
        f32 f145 = 145.0f;
        f32 f220 = 220.0f;
        GXSetScissor(scX, (u16)f145, scW, (u16)f220);
    }
    f28 = 135.0f;
    f30 = 40.0f;
    rank = 0;
    y = 0;
    i = 0;
    off = 0;
    for (; i < rank_wp->count; i++, off += 0xc) {
        f32 f27 = 10.0f + f31;

        f26 = dwp->field_00 + ((f28 - (f32)y) - f30);
        if (!(((RankEntry*)((u8*)rank_wp->entries + off))->flags & RANK_HIDDEN)) {
            PSMTXScale(scale, 0.8f, 0.8f, 0.8f);
            if (rank != 0) {
                const char* p1 = (rank / 10) ? suuji[rank / 10] : "";

                sprintf(buf, "%s%s", p1, suuji[rank % 10]);
                FontDrawStart();
                {
                    GXColor color = { 0xDC, 0xC8, 0xDC, 0xFF };

                    FontDrawColor(&color);
                }
                PSMTXTrans(mtx, f27, f26, 0.0f);
                PSMTXConcat(mtx, scale, mtx);
                FontDrawMessageMtx(mtx, buf);
            }
            FontDrawStart();
            if (((RankEntry*)((u8*)rank_wp->entries + off))->flags & RANK_PLAYER) {
                GXColor color = { 0xFF, 0x00, 0x00, 0xFF };

                FontDrawColor(&color);
            } else {
                GXColor color = { 0xDC, 0xC8, 0xDC, 0xFF };

                FontDrawColor(&color);
            }
            PSMTXTrans(mtx, 40.0f + f27, f26, 0.0f);
            PSMTXConcat(mtx, scale, mtx);
            FontDrawMessageMtx(mtx, msgSearch(dt[((RankEntry*)((u8*)rank_wp->entries + off))->id].name));
            rank++;
            y += 0x14;
        }
    }
    {
        f32 f137 = 137.0f;
        f32 f228 = 228.0f;
        GXSetScissor(scX, (u16)f137, scW, (u16)f228);
    }
    {
        f32 yv = 95.0f + dwp->field_00;
        Vec pos = { 0.0f, 0.0f, 0.0f };

        pos.x = 15.0f + f31;
        pos.y = yv - 24.0f;
        iconDispGx(&pos, 0x10, 0x123, 0.8f);
    }
    GXSetScissor(0, 0, 0x260, 0x1e0);
    if (dwp->field_08 > 0) {
        Vec pos = { 0.0f, 125.0f, 0.0f };

        pos.x = f31 + c250;
        iconDispGx(&pos, 0x10, 0x205, -1.0f);
        {
            Vec pos = { 0.0f, 75.0f, 0.0f };

            pos.x = f31 + c250;
            iconDispGx(&pos, 0x10, 0x7e, 0.6f);
        }
    }
    if (dwp->field_08 < 1) {
        Vec pos = { 0.0f, -140.0f, 0.0f };

        pos.x = f31 + c250;
        iconDispGx(&pos, 0x10, 0x205, 1.0f);
        {
            Vec pos = { 0.0f, -125.0f, 0.0f };

            pos.x = f31 + c250;
            iconDispGx(&pos, 0x10, 0x80, 0.6f);
        }
    }
}

static USER_FUNC(evt_tou_print_ranking) {
    s32 i;
    s32 c;

    if (isFirstCall) {
        memset(dwp, 0, 0xc);
        c = 0;
        for (i = 0; i < rank_wp->count; i++) {
            u16 f = rank_wp->entries[i].flags;

            if (!(f & RANK_HIDDEN)) {
                if (f & RANK_PLAYER) {
                    break;
                }
                c++;
            }
        }
        dwp->field_08 = c / 0xb;
        dwp->field_00 = dwp->field_04 = (f32)(dwp->field_08 * 0xdc);
        printRanking();
        return EVT_RETURN_BLOCK;
    }
    if (keyGetButtonTrg(0) & 0x200) {
        psndSFXOn((const char*)0x13);
        return EVT_RETURN_YIELD;
    }
    if (dwp->field_08 > 0 && (keyGetButtonTrg(0) & 0x40)) {
        psndSFXOn((const char*)5);
        dwp->field_08--;
    }
    if (dwp->field_08 < 1 && (keyGetButtonTrg(0) & 0x20)) {
        dwp->field_08++;
        psndSFXOn((const char*)5);
    }
    dwp->field_04 = (f32)(dwp->field_08 * 0xdc);
#ifdef MUST_MATCH
    // MUST_MATCH: retail folds this interpolation step to `* 0.125f`; the `+=` over `/ 8.0f` form is
    // what makes MWCC keep 0.125 in f0 and the int-to-float temp in f2 (a literal `* 0.125f` swaps them).
    dwp->field_00 += (dwp->field_04 - dwp->field_00) / 8.0f;
#else
    dwp->field_00 = (dwp->field_04 - dwp->field_00) * 0.125f + dwp->field_00;
#endif
    dispEntry((CameraId)8, 300.0f, 0, disp_proc, NULL);
    return EVT_RETURN_BLOCK;
}

// dont_inline: atan2f wraps atan2 and must stay a real call
#pragma dont_inline on
f32 atan2f(f32 y, f32 x) {
    return atan2(y, x);
}
#pragma dont_inline reset

// dont_inline: camGetRy is called from tou_champion_belt and must stay a real call
#pragma dont_inline on
static f32 camGetRy(void) {
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    f32 ry =
        reviseAngle((f32)atan2f(-(cam->cameraAt.x - cam->cameraPos.x), -(cam->cameraAt.z - cam->cameraPos.z)) * 57.29579f);

    if (ry < 0.0f) {
        ry += 360.0f;
    }
    return ry;
}
#pragma dont_inline reset

// NONMATCHING (99.11%): `event`/`npc` callee-saved swap (original event=r31, npc=r30; ours reversed),
// which cascades into every `0x104/0x78/0xc` access; plus the first angle branch picks f1 for the
// `angle + viewYaw` sum where the original used f0. Both are allocator tie-breaks we couldn't steer.
USER_FUNC(tou_champion_belt_28_text_2A10) {
    AnimationPose* pose;
    NpcEntry* npc = npcNameToPtr((const char*)evtGetValue(event, event->args[0]));
    Vec* loc;
    s32 group;
    CameraEntry* cam;
    EffectEntry* handle;
    f32 angle;

    if (isFirstCall) {
        f32 zero = 0.0f;

        handle = effStarStoneEntry(9, zero, zero, zero, 0.6f);
        animPoseWorldPositionEvalOn(npc->poseId);
        event->userdata[0] = (s32)handle;
        champion_belt_onoff = 0;
    }
    pose = animPoseGetAnimPosePtr(npc->poseId);
    loc = ((PoseUser*)*(void**)((u8*)pose + 0x15c))->locators;
    group = animPoseGetGroupIdx(npc->poseId, "locator8");
    angle = reviseAngle(camGetRy() - pose->rotationY);
    handle = (EffectEntry*)event->userdata[0];
    ((BeltData*)handle->userdata)->pos.x = loc[group].x;
    ((BeltData*)handle->userdata)->pos.y = loc[group].y;
    ((BeltData*)handle->userdata)->pos.z = loc[group].z;
    if ((angle > 90.0f && angle <= 270.0f) || (angle < -90.0f && angle >= -270.0f)) {
        cam = camGetPtr(CAMERA_3D);
        ((BeltData*)handle->userdata)->field_1C = 8.0f - (angle + cam->viewYaw);
    } else {
        cam = camGetPtr(CAMERA_3D);
        ((BeltData*)handle->userdata)->field_1C = -(angle + cam->viewYaw) - 8.0f;
    }
    if (npc->position.y <= -1000.0f) {
        ((BeltData*)handle->userdata)->pos.x = 0.0f;
        ((BeltData*)handle->userdata)->pos.y = -1000.0f;
        ((BeltData*)handle->userdata)->pos.z = 0.0f;
    }
    if (champion_belt_onoff != 0) {
        ((BeltData*)handle->userdata)->pos.x = 0.0f;
        ((BeltData*)handle->userdata)->pos.y = -1000.0f;
        ((BeltData*)handle->userdata)->pos.z = 0.0f;
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(tou_champion_belt_onoff_28_text_29DC) {
    champion_belt_onoff = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}
