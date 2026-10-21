#include "tou.h"
#include <stdlib.h>
#include <string.h>
#include "data/mapdata.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "manager/fontmgr.h"
void FontDrawColor(GXColor* color);
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "driver/windowdrv.h"
#include "driver/msgdrv.h"
#include "driver/swdrv.h"
#include "eff/eff_starstone.h"
#include "eff/eff_mahojin.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "mario/mario_pouch.h"
#include "math_trig.h"
#include "memory.h"
#include "party/party_motion.h"
#include "pmario_sound.h"
#include "system.h"
#include "driver/mapdrv.h"

// Retail loads constants separately, rather than through a shared pool base.
#pragma pool_data off

// Keep event tables and constants that scripts or other units reference.
#pragma force_active on

// MUST_MATCH: keep named scalar loads after their definitions instead of folding them to literals.
#ifdef MUST_MATCH
#define TOU_F32(name) (*(f32*)&(name))
#else
#define TOU_F32(name) (name)
#endif

typedef struct FighterEntry {
    u16 flags;         // 0x00 bits: 1 major, 2 minor, 4 champion, 8 player; 0x100-0x1000 pin story fighters to fixed ranks
    u16 pad_02;        // 0x02
    const char* name;  // 0x04
    const char* title; // 0x08
    s32 setupId;       // 0x0C battle setup table index
    const char* taunt; // 0x10 msg id shown when the battle starts
    const char* victory; // 0x14 msg id shown after the battle
    s16 defaultScore;  // 0x18 starting ranking score
    u16 pad_1A;        // 0x1A
} FighterEntry;

typedef struct RuleEntry {
    const char* name; // 0x00 rule title msg id
    const char* msg;  // 0x04 rule description msg id
    s32 kind;         // 0x08 rule kind: the _rule_msg_table index (NpcBattleInfo.battleRules[0])
    s32 param;        // 0x0C the value printed by the rule display (battleRules[1])
    s32 field_10;     // 0x10 battleRules[2]
} RuleEntry;

typedef struct RankEntry {
    u16 flags; // 0x00 bits: 1 beaten (below us), 2 hidden, 4 player line
    u16 pad;   // 0x02
    s32 id;    // 0x04
    s16 score; // 0x08
    u16 pad2;  // 0x0A
} RankEntry;

typedef struct RankWork {
    s32 count;          // 0x0
    RankEntry* entries; // 0x4
} RankWork;

typedef struct DispWork {
    f32 scroll;       // 0x0 current scroll offset, eases toward targetScroll
    f32 targetScroll; // 0x4 scroll offset for the current page
    s32 page;         // 0x8 current page index
} DispWork;

typedef struct PoseUser {
    Vec* locators; // 0x0
} PoseUser;

typedef struct GansWork {
    smartEntry* texData; // 0x0
    GXTexObj texObj;     // 0x4
    s32 enabled;         // 0x24 monitor on/off
    s32 alpha;           // 0x28 TEV color alpha, ramps toward 0 or 0x80
    s32 texReady;        // 0x2C set after the scene is copied to the texture
} GansWork;

extern const u32 dat_tou_00009c38;
extern const u32 dat_tou_00009904;
extern const u32 dat_tou_00009908;
extern const u32 dat_tou_0000990c;
extern const u32 dat_tou_00009910;
extern const Vec vec3_tou_00009914;
extern const Vec vec3_tou_00009920;
extern const Vec vec3_tou_0000992c;
extern const Vec vec3_tou_00009938;
extern const Vec vec3_tou_00009944;
extern const Vec vec3_tou_00009c14;
extern const Vec vec3_tou_00009c20;
extern const Vec vec3_tou_00009c2c;
extern const f32 float_0p99_tou_0000a860;
extern const f32 float_2_tou_0000a864;
extern const f32 float_0p5_tou_0000a868;
extern const f32 float_5_tou_0000a86c;
extern const f32 float_0p125_tou_0000a888;
extern const f32 float_300_tou_0000a88c;
extern const f32 float_250_tou_0000a890;
extern const f32 float_30_tou_0000a894;
extern const f32 float_165_tou_0000a898;
extern const f32 float_310_tou_0000a89c;
extern const f32 float_330_tou_0000a8a0;
extern const f32 float_135_tou_0000a8b0;
extern const f32 float_304_tou_0000a8b4;
extern const f32 float_145_tou_0000a8b8;
extern const f32 float_220_tou_0000a8bc;
extern const f32 float_10_tou_0000a8c0;
extern const f32 float_40_tou_0000a8c4;
extern const f32 float_0p8_tou_0000a8c8;
extern const f32 zero_tou_0000a8cc;
extern const f32 float_137_tou_0000a8d8;
extern const f32 float_228_tou_0000a8dc;
extern const f32 float_95_tou_0000a8e0;
extern const f32 float_15_tou_0000a8e4;
extern const f32 float_24_tou_0000a8e8;
extern const f32 float_neg1_tou_0000a8ec;

static const char str_00001F74[12];
static const char str_fmt_ss[8];

typedef void (*voidfunctionptr)(void);

extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];

extern s32 evt_tou_08_cake[];
extern s32 evt_tou_chibi_yoshi[];
static USER_FUNC(tou_champion_belt_onoff);
static f32 camGetRy(void);
static USER_FUNC(getYoshiColor);
static USER_FUNC(evt_tou_print_ranking);
static void disp_proc(CameraId cameraId, void* param);
static void printRanking(void);
static USER_FUNC(evt_tou_set_rule_result);
static USER_FUNC(evt_tou_set_ranking_win_lose);
static USER_FUNC(evt_tou_get_rule_info);
static USER_FUNC(evt_tou_get_fighter_battle_msg);
static USER_FUNC(evt_tou_get_fighter_rank);
static USER_FUNC(evt_tou_get_fighter_name2);
static USER_FUNC(evt_tou_get_next_match);
static void rankingControll(void);
static void insert(s32* arr, s32 unused, s32 from, s32 to);
static USER_FUNC(evt_rankingReInit);
static void rankingExit(void);
static void rankingInit(void);
static s32 evt_open[165];
s32 evt_doku_cake[234];
s32 evt_michel_1[39];
static u32 rank_work[2];
static u32 disp_work[3];
u32 gans_wp[1];

// clang-format off
extend_data(0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(2,
     f64, _three, 3,
     f64, _half, 0.5)

// TODO: type
static u32 fighterDt[161] = {
    0x00040000, (u32)"stg3_tou_sensyu_00", (u32)"stg3_tou_sensyu2_00", 0x00000016,
    (u32)"stg3_tou_400_02", (u32)"stg3_tou_400_03", 0x00FA0000, 0x10010000,
    (u32)"stg3_tou_sensyu_01", (u32)"stg3_tou_sensyu2_01", 0x00000013, (u32)"stg3_tou_118_00",
    (u32)"stg3_tou_118_01", 0x00C80000, 0x00010000, (u32)"stg3_tou_sensyu_02",
    (u32)"stg3_tou_sensyu2_02", 0x00000012, (u32)"stg3_tou_118_02", (u32)"stg3_tou_118_03",
    0x00B40000, 0x00010000, (u32)"stg3_tou_sensyu_03", (u32)"stg3_tou_sensyu2_03",
    0x00000011, (u32)"stg3_tou_118_04", (u32)"stg3_tou_118_05", 0x00AA0000,
    0x00010000, (u32)"stg3_tou_sensyu_04", (u32)"stg3_tou_sensyu2_04", 0x00000010,
    (u32)"stg3_tou_118_06", (u32)"stg3_tou_118_07", 0x00A00000, 0x00010000,
    (u32)"stg3_tou_sensyu_05", (u32)"stg3_tou_sensyu2_05", 0x0000000F, (u32)"stg3_tou_118_08",
    (u32)"stg3_tou_118_09", 0x00960000, 0x00010000, (u32)"stg3_tou_sensyu_06",
    (u32)"stg3_tou_sensyu2_06", 0x0000000E, (u32)"stg3_tou_118_10", (u32)"stg3_tou_118_11",
    0x008C0000, 0x00010000, (u32)"stg3_tou_sensyu_07", (u32)"stg3_tou_sensyu2_07",
    0x0000000D, (u32)"stg3_tou_118_12", (u32)"stg3_tou_118_13", 0x00820000,
    0x00010000, (u32)"stg3_tou_sensyu_08", (u32)"stg3_tou_sensyu2_08", 0x0000000C,
    (u32)"stg3_tou_118_14", (u32)"stg3_tou_118_15", 0x00780000, 0x00010000,
    (u32)"stg3_tou_sensyu_09", (u32)"stg3_tou_sensyu2_09", 0x0000000B, (u32)"stg3_tou_118_16",
    (u32)"stg3_tou_118_17", 0x006E0000, 0x08010000, (u32)"stg3_tou_sensyu_10",
    (u32)"stg3_tou_sensyu2_10", 0x0000000A, (u32)"stg3_tou_118_18", (u32)"stg3_tou_118_19",
    0x00640000, 0x01020000, (u32)"stg3_tou_sensyu_11", (u32)"stg3_tou_sensyu2_11",
    0x00000009, (u32)"stg3_tou_118_20", (u32)"stg3_tou_118_21", 0x005A0000,
    0x00020000, (u32)"stg3_tou_sensyu_12", (u32)"stg3_tou_sensyu2_12", 0x00000008,
    (u32)"stg3_tou_118_22", (u32)"stg3_tou_118_23", 0x00500000, 0x00020000,
    (u32)"stg3_tou_sensyu_13", (u32)"stg3_tou_sensyu2_13", 0x00000007, (u32)"stg3_tou_118_24",
    (u32)"stg3_tou_118_25", 0x00460000, 0x00020000, (u32)"stg3_tou_sensyu_14",
    (u32)"stg3_tou_sensyu2_14", 0x00000006, (u32)"stg3_tou_118_26", (u32)"stg3_tou_118_27",
    0x003C0000, 0x00020000, (u32)"stg3_tou_sensyu_15", (u32)"stg3_tou_sensyu2_15",
    0x00000005, (u32)"stg3_tou_118_28", (u32)"stg3_tou_118_29", 0x00320000,
    0x00020000, (u32)"stg3_tou_sensyu_16", (u32)"stg3_tou_sensyu2_16", 0x00000004,
    (u32)"stg3_tou_118_30", (u32)"stg3_tou_118_31", 0x00280000, 0x00020000,
    (u32)"stg3_tou_sensyu_17", (u32)"stg3_tou_sensyu2_17", 0x00000003, (u32)"stg3_tou_118_32",
    (u32)"stg3_tou_118_33", 0x001E0000, 0x02020000, (u32)"stg3_tou_sensyu_18",
    (u32)"stg3_tou_sensyu2_18", 0x00000002, (u32)"stg3_tou_118_34", (u32)"stg3_tou_118_35",
    0x00140000, 0x04020000, (u32)"stg3_tou_sensyu_19", (u32)"stg3_tou_sensyu2_19",
    0x00000001, (u32)"stg3_tou_118_36", (u32)"stg3_tou_118_36_01", 0x000A0000,
    0x00080000, (u32)"stg3_tou_sensyu_20", (u32)"stg3_tou_sensyu2_20", 0x00000001,
    0x00000000, 0x00000000, 0x00000000, 0x00020000,
    (u32)"stg3_tou_sensyu_21", (u32)"stg3_tou_sensyu2_21", 0x00000014, (u32)"stg3_tou_118_36_02",
    (u32)"stg3_tou_118_36_03", 0x00050000, 0x00020000, (u32)"stg3_tou_sensyu_22",
    (u32)"stg3_tou_sensyu2_22", 0x00000015, (u32)"stg3_tou_118_36_04", (u32)"stg3_tou_118_36_05",
    0x00000000,
};

// TODO: type
static u32 fighterDt_re[147] = {
    0x00040000, (u32)"stg3_tou_re_315", (u32)"stg3_tou_re_338", 0x00000016,
    (u32)"stg3_tou_re_289", (u32)"stg3_tou_re_290", 0x00FA0000, 0x10010000,
    (u32)"stg3_tou_re_316", (u32)"stg3_tou_re_339", 0x00000013, (u32)"stg3_tou_re_179",
    (u32)"stg3_tou_re_180", 0x00C80000, 0x00010000, (u32)"stg3_tou_re_317",
    (u32)"stg3_tou_re_340", 0x00000012, (u32)"stg3_tou_re_181", (u32)"stg3_tou_re_182",
    0x00B40000, 0x00010000, (u32)"stg3_tou_re_318", (u32)"stg3_tou_re_341",
    0x00000011, (u32)"stg3_tou_re_183", (u32)"stg3_tou_re_184", 0x00AA0000,
    0x00010000, (u32)"stg3_tou_re_319", (u32)"stg3_tou_re_342", 0x00000010,
    (u32)"stg3_tou_re_185", (u32)"stg3_tou_re_186", 0x00A00000, 0x00010000,
    (u32)"stg3_tou_re_320", (u32)"stg3_tou_re_343", 0x0000000F, (u32)"stg3_tou_re_187",
    (u32)"stg3_tou_re_188", 0x00960000, 0x00010000, (u32)"stg3_tou_re_321",
    (u32)"stg3_tou_re_344", 0x0000000E, (u32)"stg3_tou_re_189", (u32)"stg3_tou_re_190",
    0x008C0000, 0x00010000, (u32)"stg3_tou_re_322", (u32)"stg3_tou_re_345",
    0x0000000D, (u32)"stg3_tou_re_191", (u32)"stg3_tou_re_192", 0x00820000,
    0x00010000, (u32)"stg3_tou_re_323", (u32)"stg3_tou_re_346", 0x0000000C,
    (u32)"stg3_tou_re_193", (u32)"stg3_tou_re_194", 0x00780000, 0x00010000,
    (u32)"stg3_tou_re_324", (u32)"stg3_tou_re_347", 0x0000000B, (u32)"stg3_tou_re_195",
    (u32)"stg3_tou_re_196", 0x006E0000, 0x08010000, (u32)"stg3_tou_re_325",
    (u32)"stg3_tou_re_348", 0x0000000A, (u32)"stg3_tou_re_197", (u32)"stg3_tou_re_198",
    0x00640000, 0x01020000, (u32)"stg3_tou_re_326", (u32)"stg3_tou_re_349",
    0x00000009, (u32)"stg3_tou_re_199", (u32)"stg3_tou_re_200", 0x005A0000,
    0x00020000, (u32)"stg3_tou_re_327", (u32)"stg3_tou_re_350", 0x00000008,
    (u32)"stg3_tou_re_201", (u32)"stg3_tou_re_202", 0x00500000, 0x00020000,
    (u32)"stg3_tou_re_328", (u32)"stg3_tou_re_351", 0x00000007, (u32)"stg3_tou_re_203",
    (u32)"stg3_tou_re_204", 0x00460000, 0x00020000, (u32)"stg3_tou_re_329",
    (u32)"stg3_tou_re_352", 0x00000006, (u32)"stg3_tou_re_205", (u32)"stg3_tou_re_206",
    0x003C0000, 0x00020000, (u32)"stg3_tou_re_330", (u32)"stg3_tou_re_353",
    0x00000005, (u32)"stg3_tou_re_207", (u32)"stg3_tou_re_208", 0x00320000,
    0x00020000, (u32)"stg3_tou_re_331", (u32)"stg3_tou_re_354", 0x00000004,
    (u32)"stg3_tou_re_209", (u32)"stg3_tou_re_210", 0x00280000, 0x00020000,
    (u32)"stg3_tou_re_332", (u32)"stg3_tou_re_355", 0x00000003, (u32)"stg3_tou_re_211",
    (u32)"stg3_tou_re_212", 0x001E0000, 0x02020000, (u32)"stg3_tou_re_333",
    (u32)"stg3_tou_re_356", 0x00000002, (u32)"stg3_tou_re_213", (u32)"stg3_tou_re_214",
    0x00140000, 0x04020000, (u32)"stg3_tou_re_334", (u32)"stg3_tou_re_357",
    0x00000001, (u32)"stg3_tou_re_215", (u32)"stg3_tou_re_216", 0x000A0000,
    0x00080000, (u32)"stg3_tou_re_335", (u32)"stg3_tou_re_358", 0x00000001,
    0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
static u32 ruleDt_2bu[50] = {
    (u32)"stg3_tou_107_01", (u32)"stg3_tou_cp_000", 0x00000012, 0x00000003,
    0x00000003, (u32)"stg3_tou_107_02", (u32)"stg3_tou_cp_001", 0x00000001,
    0x00000001, 0x00000001, (u32)"stg3_tou_107_03", (u32)"stg3_tou_cp_002",
    0x00000003, 0x00000001, 0x00000001, (u32)"stg3_tou_107_04",
    (u32)"stg3_tou_cp_003", 0x00000030, 0x00000001, 0x00000001,
    (u32)"stg3_tou_107_05", (u32)"stg3_tou_cp_004", 0x00000005, 0x00000001,
    0x00000001, (u32)"stg3_tou_107_06", (u32)"stg3_tou_cp_005", 0x0000001A,
    0x00000001, 0x00000001, (u32)"stg3_tou_107_07", (u32)"stg3_tou_cp_006",
    0x00000020, 0x00000001, 0x00000001, (u32)"stg3_tou_107_08",
    (u32)"stg3_tou_cp_007", 0x0000003B, 0x00000005, 0x00000005,
    (u32)"stg3_tou_107_12", (u32)"stg3_tou_cp_008", 0x00000006, 0x00000001,
    0x00000001, (u32)"stg3_tou_107_16", (u32)"stg3_tou_cp_009", 0x0000002B,
    0x00000001, 0x00000001,
};

// TODO: type
static u32 ruleDt_1bu[70] = {
    (u32)"stg3_tou_107_01", (u32)"stg3_tou_cp_010", 0x00000012, 0x00000005,
    0x00000005, (u32)"stg3_tou_107_02", (u32)"stg3_tou_cp_011", 0x00000001,
    0x00000001, 0x00000001, (u32)"stg3_tou_107_03", (u32)"stg3_tou_cp_012",
    0x00000003, 0x00000001, 0x00000001, (u32)"stg3_tou_107_04",
    (u32)"stg3_tou_cp_013", 0x00000030, 0x00000001, 0x00000001,
    (u32)"stg3_tou_107_05", (u32)"stg3_tou_cp_014", 0x00000005, 0x00000001,
    0x00000001, (u32)"stg3_tou_107_06", (u32)"stg3_tou_cp_015", 0x0000001A,
    0x00000001, 0x00000001, (u32)"stg3_tou_107_07", (u32)"stg3_tou_cp_016",
    0x00000020, 0x00000001, 0x00000001, (u32)"stg3_tou_107_08",
    (u32)"stg3_tou_cp_017", 0x0000003B, 0x00000005, 0x00000005,
    (u32)"stg3_tou_107_12", (u32)"stg3_tou_cp_019", 0x00000006, 0x00000001,
    0x00000001, (u32)"stg3_tou_107_13", (u32)"stg3_tou_cp_020", 0x0000000B,
    0x00000014, 0x00000014, (u32)"stg3_tou_107_16", (u32)"stg3_tou_cp_021",
    0x0000002B, 0x00000003, 0x00000003, (u32)"stg3_tou_107_17",
    (u32)"stg3_tou_cp_022", 0x00000041, 0x00000003, 0x00000003,
    (u32)"stg3_tou_107_19", (u32)"stg3_tou_cp_023", 0x00000040, 0x000000FF,
    0x000000FF, (u32)"stg3_tou_107_20", (u32)"stg3_tou_cp_024", 0x0000003F,
    0x000000FF, 0x000000FF,
};

// TODO: type
static u32 ruleDt_re[70] = {
    (u32)"stg3_tou_re_362", (u32)"stg3_tou_cp_040", 0x00000012, 0x00000005,
    0x00000005, (u32)"stg3_tou_re_363", (u32)"stg3_tou_cp_041", 0x00000001,
    0x00000001, 0x00000001, (u32)"stg3_tou_re_364", (u32)"stg3_tou_cp_042",
    0x00000003, 0x00000001, 0x00000001, (u32)"stg3_tou_re_365",
    (u32)"stg3_tou_cp_043", 0x00000030, 0x00000001, 0x00000001,
    (u32)"stg3_tou_re_366", (u32)"stg3_tou_cp_044", 0x00000005, 0x00000001,
    0x00000001, (u32)"stg3_tou_re_367", (u32)"stg3_tou_cp_045", 0x0000001A,
    0x00000001, 0x00000001, (u32)"stg3_tou_re_368", (u32)"stg3_tou_cp_046",
    0x00000020, 0x00000001, 0x00000001, (u32)"stg3_tou_re_369",
    (u32)"stg3_tou_cp_047", 0x0000003B, 0x00000005, 0x00000005,
    (u32)"stg3_tou_re_371", (u32)"stg3_tou_cp_049", 0x00000006, 0x00000001,
    0x00000001, (u32)"stg3_tou_re_372", (u32)"stg3_tou_cp_050", 0x0000000B,
    0x00000014, 0x00000014, (u32)"stg3_tou_re_373", (u32)"stg3_tou_cp_051",
    0x0000002B, 0x00000003, 0x00000003, (u32)"stg3_tou_re_374",
    (u32)"stg3_tou_cp_052", 0x00000041, 0x00000003, 0x00000003,
    (u32)"stg3_tou_re_375", (u32)"stg3_tou_cp_053", 0x00000040, 0x000000FF,
    0x000000FF, (u32)"stg3_tou_re_376", (u32)"stg3_tou_cp_054", 0x0000003F,
    0x000000FF, 0x000000FF,
};

// TODO: type
static u32 rank_wp[1] = {
    (u32)&rank_work,
};

// TODO: type
static u32 dwp[1] = {
    (u32)&disp_work,
};

static char* suuji[10] = {
    "０", "１", "２", "３", "４", "５", "６", "７", "８", "９",
};

const u32 dat_tou_00009904 = 0xdcc8dcff;
const u32 dat_tou_00009908 = 0xdcc8dcff;
const u32 dat_tou_0000990c = 0xff0000ff;
const u32 dat_tou_00009910 = 0xdcc8dcff;
const Vec vec3_tou_00009914 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_tou_00009920 = { 0.0f, 125.0f, 0.0f };
const Vec vec3_tou_0000992c = { 0.0f, 75.0f, 0.0f };
const Vec vec3_tou_00009938 = { 0.0f, -140.0f, 0.0f };
const Vec vec3_tou_00009944 = { 0.0f, -125.0f, 0.0f };

char* lockerDt[46] = {
    "R_door_1", "a_door_1", "a_door_1_omote", "a_door_1_ura", "a_R_1_asi", "R_door_2", "a_door_2",
    "a_door_2_omote", "a_door_2_ura", "a_R_2_asi", "R_door_3", "a_door_3", "a_door_3_omote",
    "a_door_3_ura", "a_R_3_asi", "R_door_5", "a_door_5", "a_door_5_omote", "a_door_5_ura",
    "a_R_5_asi", "R_door_6", "a_door_6", "a_door_6_omote", "a_door_6_ura", "a_R_6_asi", "R_door_7",
    "a_door_7", "a_door_7_omote", "a_door_7_ura", "a_R_7_asi", "R_door_8", "a_door_8",
    "a_door_8_omote", "a_door_8_ura", "a_R_8_asi", "R_door_9", "a_door_9", "a_door_9_omote",
    "a_door_9_ura", "a_R_9_asi", "R_door_10", "a_door_10", "a_door_10_omote", "a_door_10_ura",
    "a_R_10_asi", NULL,
};

EVT_BEGIN(evt_close)
    SET_VALUE(LF(0), 0)
    IF_INT_EQUAL(LW(10), 0)
        CALL(evt_del_case_evt, 0, LW(12))
        SET_VALUE(LW(5), LW(0))
        SET_VALUE(LW(6), LW(1))
        SET_VALUE(LW(7), LW(2))
        SET_VALUE(LW(8), LW(3))
        SET_VALUE(LW(9), LW(4))
        CALL(evt_mapobj_get_position, LW(5), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_LOCKER_CLOSE1"), LW(10), LW(11), LW(12), 0)
        CALL(evt_sub_intpl_init, 12, 120, 0, 40)
        LOOP(41)
            CALL(evt_sub_intpl_get_value)
            IF_INT_EQUAL(LF(0), 1)
                MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
            END_IF
            CALL(evt_mapobj_rotate, 1, LW(5), 0, LW(0), 0)
            CALL(evt_hit_bind_update, LW(6))
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(LW(0), LW(5))
        SET_VALUE(LW(1), LW(6))
        SET_VALUE(LW(2), LW(7))
        SET_VALUE(LW(3), LW(8))
        SET_VALUE(LW(4), LW(9))
        CALL(evt_run_case_evt, 9, 1, LW(2), 0, PTR(evt_open), 0)
    ELSE
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_del_case_evt, 0, LW(11))
        SET_VALUE(LW(5), LW(0))
        SET_VALUE(LW(6), LW(1))
        SET_VALUE(LW(7), LW(2))
        SET_VALUE(LW(8), LW(3))
        SET_VALUE(LW(9), LW(4))
        CALL(evt_mapobj_get_position, LW(5), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_LOCKER_CLOSE1"), LW(10), LW(11), LW(12), 0)
        CALL(evt_sub_intpl_init, 12, 120, 0, 20)
        LOOP(21)
            CALL(evt_sub_intpl_get_value)
            IF_INT_EQUAL(LF(0), 1)
                MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
            END_IF
            CALL(evt_mapobj_rotate, 1, LW(5), 0, LW(0), 0)
            CALL(evt_hit_bind_update, LW(6))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_mario_key_onoff, 1)
        SET_VALUE(LW(0), LW(5))
        SET_VALUE(LW(1), LW(6))
        SET_VALUE(LW(2), LW(7))
        SET_VALUE(LW(3), LW(8))
        SET_VALUE(LW(4), LW(9))
        CALL(evt_run_case_evt, 10, 1, LW(3), 0, PTR(evt_open), 0)
    END_IF
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

static EVT_BEGIN(evt_open)
    IF_INT_LESS(GSW(0), 132)
        CALL(evt_sub_get_mapname, LW(5))
        IF_STR_EQUAL(LW(5), STRING("tou_10"))
            IF_INT_EQUAL(GSWF(2388), 0)
                CALL(evt_mario_key_onoff, 0)
                CALL(evt_msg_print, 0, STRING("stg3_tou_67"), 0, STRING("キノシコワ"))
                CALL(evt_mario_key_onoff, 1)
                RETURN
            END_IF
        END_IF
    END_IF
    SET_VALUE(LF(0), 0)
    SET_VALUE(LW(5), LW(0))
    SET_VALUE(LW(6), LW(1))
    SET_VALUE(LW(7), LW(2))
    SET_VALUE(LW(8), LW(3))
    SET_VALUE(LW(9), LW(4))
    CALL(evt_mapobj_get_position, LW(5), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_LOCKER_OPEN1"), LW(10), LW(11), LW(12), 0)
    CALL(evt_sub_intpl_init, 12, 0, 120, 40)
    LOOP(41)
        CALL(evt_sub_intpl_get_value)
        IF_INT_EQUAL(LF(0), 1)
            MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
        END_IF
        CALL(evt_mapobj_rotate, 1, LW(5), 0, LW(0), 0)
        CALL(evt_hit_bind_update, LW(6))
        WAIT_FRAMES(1)
    END_LOOP
    SET_VALUE(LW(0), LW(5))
    SET_VALUE(LW(1), LW(6))
    SET_VALUE(LW(2), LW(7))
    SET_VALUE(LW(3), LW(8))
    SET_VALUE(LW(4), LW(9))
    SET_VALUE(LW(10), 0)
    CALL(evt_run_case_evt, 10, 1, LW(2), 0, PTR(evt_close), LW(11))
    SET_VALUE(LW(0), LW(5))
    SET_VALUE(LW(1), LW(6))
    SET_VALUE(LW(2), LW(7))
    SET_VALUE(LW(3), LW(8))
    SET_VALUE(LW(4), LW(9))
    SET_VALUE(LW(10), 1)
    CALL(evt_run_case_evt, 0, 1, LW(4), 0, PTR(evt_close), LW(12))
    CALL(evt_set_case_wrk, LW(11), 11, LW(11))
    CALL(evt_set_case_wrk, LW(11), 12, LW(12))
    CALL(evt_set_case_wrk, LW(12), 11, LW(11))
    CALL(evt_set_case_wrk, LW(12), 12, LW(12))
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(evt_tou_locker_init)
    SET_INT_READ_ADDR(PTR(lockerDt))
    LOOP(0)
        READ_INT3(LW(0), LW(1), LW(2))
        READ_INT2(LW(3), LW(4))
        IF_INT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        SET_VALUE(LF(0), 0)
        CALL(evt_sub_get_mapname, LW(10))
        IF_STR_EQUAL(LW(10), STRING("tou_10"))
            IF_STR_EQUAL(LW(2), STRING("a_door_1_omote"))
                SET_VALUE(LF(0), 1)
            END_IF
            IF_STR_EQUAL(LW(2), STRING("a_door_7_omote"))
                SET_VALUE(LF(0), 1)
            END_IF
            IF_STR_EQUAL(LW(2), STRING("a_door_8_omote"))
                SET_VALUE(LF(0), 1)
            END_IF
            IF_STR_EQUAL(LW(2), STRING("a_door_9_omote"))
                SET_VALUE(LF(0), 1)
            END_IF
            IF_STR_EQUAL(LW(2), STRING("a_door_10_omote"))
                SET_VALUE(LF(0), 1)
            END_IF
        END_IF
        IF_STR_EQUAL(LW(10), STRING("tou_09"))
            IF_STR_EQUAL(LW(2), STRING("a_door_9_omote"))
                SET_VALUE(LF(0), 1)
            END_IF
        END_IF
        IF_STR_EQUAL(LW(10), STRING("tou_08"))
            IF_STR_EQUAL(LW(2), STRING("a_door_9_omote"))
                SET_VALUE(LF(0), 1)
            END_IF
            IF_STR_EQUAL(LW(2), STRING("a_door_10_omote"))
                SET_VALUE(LF(0), 1)
            END_IF
        END_IF
        IF_INT_EQUAL(LF(0), 0)
            CALL(evt_hit_bind_mapobj, LW(1), LW(0))
            CALL(evt_run_case_evt, 9, 1, LW(2), 0, PTR(evt_open), 0)
        END_IF
    END_LOOP
    RETURN
EVT_END()

const Vec vec3_tou_00009c14 = { 0.0f, 20.0f, 130.0f };
const Vec vec3_tou_00009c20 = { 0.0f, 1.0f, 0.0f };
const Vec vec3_tou_00009c2c = { 0.0f, 20.0f, 0.0f };
const u32 dat_tou_00009c38 = 0xFFFFFF00;

EVT_BEGIN(evt_tou_monitor_on)
    CALL(evt_hit_get_position, STRING("M_make"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_TOUROKU_COM2"), LW(0), LW(1), LW(2), 0)
    CALL(onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_tou_monitor_off)
    CALL(evt_hit_get_position, STRING("M_make"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_TOUROKU_COM2"), LW(0), LW(1), LW(2), 0)
    CALL(onoff, 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_tou_monitor_init)
    BEGIN_INLINE_EVENT
        CALL(make_monitor)
    END_INLINE_EVENT
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("tou_10"))
        CALL(evt_map_set_tevcallback, 1, PTR(monitor_tev))
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("gamen"), 16777216)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("gamen"), 256)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("tou_08"))
        CALL(evt_map_set_tevcallback, 1, PTR(monitor_tev))
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("gamenn"), 16777216)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("gamenn"), 256)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("tou_07"))
        CALL(evt_map_set_tevcallback, 1, PTR(monitor_tev))
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("gamenn"), 16777216)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("gamenn"), 256)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(talk_gardman_doku)
    CALL(evt_npc_get_position, STRING("ガードマン"), LW(0), LW(1), LW(2))
    IF_INT_GREATER(LW(2), 100)
        CALL(evt_msg_print, 0, STRING("stg3_tou_304"), 0, STRING("ガードマン"))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(dummy_party_msg_pkr)
    CALL(evt_msg_print, 0, STRING("stg3_tou_303_kur"), 0, STRING("me"))
    CALL_CHILD(talk_gardman_doku)
    RETURN
EVT_END()

EVT_BEGIN(dummy_party_msg_pnk)
    CALL(evt_msg_print, 0, STRING("stg3_tou_303_nok"), 0, STRING("me"))
    CALL_CHILD(talk_gardman_doku)
    RETURN
EVT_END()

EVT_BEGIN(dummy_party_msg_yos)
    CALL(evt_msg_print, 0, STRING("stg3_tou_303_yos"), 0, STRING("me"))
    CALL_CHILD(talk_gardman_doku)
    RETURN
EVT_END()

EVT_BEGIN(dummy_party_msg_win)
    CALL(evt_msg_print, 0, STRING("stg3_tou_303_win"), 0, STRING("me"))
    CALL_CHILD(talk_gardman_doku)
    RETURN
EVT_END()

EVT_BEGIN(party_npc_replace)
    CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_mario_get_party, LW(3))
    SWITCH(LW(3))
        CASE_EQUAL(1)
        CALL(evt_npc_entry, STRING("dummy_party"), STRING("c_pkr"))
        CALL(evt_npc_set_anim, STRING("dummy_party"), STRING("PKR_D_3"))
        CALL(evt_npc_change_interrupt, STRING("dummy_party"), 6, PTR(dummy_party_msg_pkr))
        CASE_EQUAL(2)
        CALL(evt_npc_entry, STRING("dummy_party"), STRING("c_pnk"))
        CALL(evt_npc_set_anim, STRING("dummy_party"), STRING("PNK_D_3"))
        CALL(evt_npc_change_interrupt, STRING("dummy_party"), 6, PTR(dummy_party_msg_pnk))
        CASE_EQUAL(4)
        CALL(getYoshiColor, LW(5))
        SWITCH(LW(5))
            CASE_EQUAL(0)
            SET_VALUE(LW(5), STRING("c_babyyoshi"))
            CASE_EQUAL(1)
            SET_VALUE(LW(5), STRING("c_babyyoshi2"))
            CASE_EQUAL(2)
            SET_VALUE(LW(5), STRING("c_babyyoshi3"))
            CASE_EQUAL(3)
            SET_VALUE(LW(5), STRING("c_babyyoshi4"))
            CASE_EQUAL(4)
            SET_VALUE(LW(5), STRING("c_babyyoshi5"))
            CASE_EQUAL(5)
            SET_VALUE(LW(5), STRING("c_babyyoshi6"))
            CASE_EQUAL(6)
            SET_VALUE(LW(5), STRING("c_babyyoshi7"))
        END_SWITCH
        CALL(evt_npc_entry, STRING("dummy_party"), LW(5))
        CALL(evt_npc_set_anim, STRING("dummy_party"), STRING("PYS_D_3"))
        CALL(evt_npc_change_interrupt, STRING("dummy_party"), 6, PTR(dummy_party_msg_yos))
        CASE_EQUAL(5)
        CALL(evt_npc_entry, STRING("dummy_party"), STRING("c_windy"))
        CALL(evt_npc_set_anim, STRING("dummy_party"), STRING("PWD_D_3"))
        CALL(evt_npc_change_interrupt, STRING("dummy_party"), 6, PTR(dummy_party_msg_win))
    END_SWITCH
    CALL(evt_npc_flag_onoff, 1, STRING("dummy_party"), 16777216)
    CALL(evt_npc_set_position, STRING("dummy_party"), LW(0), LW(1), LW(2))
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_tamago_scale)
    SET_VALUE(GW(10), FLOAT(1.2f))
    SET_VALUE(GW(11), FLOAT(0.9f))
    BEGIN_INLINE_EVENT
        DIVIDE(LW(3), 2)
        ADD(LW(3), 1)
        CALL(evt_sub_intpl_msec_init, 11, 120, 90, LW(3))
        LABEL(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
        SET_FLOAT_VALUE(GW(10), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(0)
        END_IF
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        DIVIDE(LW(3), 2)
        ADD(LW(3), 1)
        CALL(evt_sub_intpl_msec_init, 11, 90, 110, LW(3))
        LABEL(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
        SET_FLOAT_VALUE(GW(11), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(0)
        END_IF
    END_INLINE_EVENT
    CONVERT_FRAME_TO_MS(LW(3), LW(4))
    ADD(LW(4), 1)
    LOOP(LW(4))
        CALL(evt_npc_set_scale, STRING("たまご"), GW(10), GW(11), FLOAT(1.0f))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(evt_tamago_scale2)
    BEGIN_INLINE_EVENT
        DIVIDE(LW(3), 2)
        ADD(LW(3), 1)
        CALL(evt_sub_intpl_msec_init, 4, 90, 100, LW(3))
        LABEL(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
        SET_FLOAT_VALUE(GW(10), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(0)
        END_IF
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        DIVIDE(LW(3), 2)
        ADD(LW(3), 1)
        CALL(evt_sub_intpl_msec_init, 4, 110, 100, LW(3))
        LABEL(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
        SET_FLOAT_VALUE(GW(11), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(0)
        END_IF
    END_INLINE_EVENT
    CONVERT_FRAME_TO_MS(LW(3), LW(4))
    ADD(LW(4), 1)
    LOOP(LW(4))
        CALL(evt_npc_set_scale, STRING("たまご"), GW(10), GW(11), FLOAT(1.0f))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_tamago_byebye)
    CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_npc_check, STRING("たまご"), LW(10))
    IF_INT_EQUAL(LW(10), 0)
        CALL(evt_npc_entry, STRING("たまご"), STRING("c_babyyoshi"))
    END_IF
    CALL(evt_npc_set_tribe, STRING("たまご"), STRING("たまご"))
    CALL(evt_npc_set_anim, STRING("たまご"), STRING("PYS_E_2"))
    CALL(evt_npc_set_position, STRING("たまご"), LW(0), LW(1), LW(2))
    CALL(evt_party_set_pos, 1, 0, -1000, 0)
    CALL(evt_mario_kill_party, 1)
    CALL(evt_mario_set_dir_npc, STRING("たまご"))
    CALL(evt_party_set_dir_npc, 0, STRING("たまご"))
    BEGIN_INLINE_EVENT
        LOOP(3)
            CALL(evt_npc_get_position, STRING("たまご"), LW(0), LW(1), LW(2))
            CALL(evt_sub_random, 200, LW(3))
            ADD(LW(3), 200)
            CALL(evt_sub_random, 20, LW(4))
            ADD(LW(4), 20)
            SPAWN_EVENT(evt_tamago_scale)
            CALL(evt_npc_jump_position_nohit, STRING("たまご"), LW(0), LW(1), LW(2), LW(3), LW(4))
        END_LOOP
        IF_INT_GREATER(LW(0), 0)
            SET_VALUE(LW(3), 400)
            SPAWN_EVENT(evt_tamago_scale)
            CALL(evt_npc_jump_position_nohit, STRING("たまご"), 45, 20, 8, 400, 50)
            SET_VALUE(LW(3), 400)
            SPAWN_EVENT(evt_tamago_scale)
            CALL(evt_npc_jump_position_nohit, STRING("たまご"), 10, 20, 15, 400, 50)
            SET_VALUE(LW(3), 400)
            SPAWN_EVENT(evt_tamago_scale)
            CALL(evt_npc_jump_position_nohit, STRING("たまご"), -30, 20, 30, 400, 50)
        ELSE
            SET_VALUE(LW(3), 400)
            SPAWN_EVENT(evt_tamago_scale)
            CALL(evt_npc_jump_position_nohit, STRING("たまご"), -80, 20, 15, 400, 50)
            SET_VALUE(LW(3), 400)
            SPAWN_EVENT(evt_tamago_scale)
            CALL(evt_npc_jump_position_nohit, STRING("たまご"), -30, 20, 30, 400, 50)
        END_IF
        SET_VALUE(LW(3), 400)
        SPAWN_EVENT(evt_tamago_scale2)
    END_INLINE_EVENT
    RETURN
EVT_END()

EVT_BEGIN(talk_gardman)
    IF_INT_EQUAL(GSWF(2451), 0)
        IF_INT_EQUAL(GSWF(2450), 1)
            CALL(evt_msg_print, 0, STRING("stg3_tou_304_01"), 0, STRING("ガードマン"))
        END_IF
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 137)
        IF_INT_LESS(GSW(0), 138)
            CALL(chk, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL_CHILD(evt_tamago_byebye)
            END_IF
        END_IF
    END_IF
    SET_VALUE(LF(0), 0)
    SET_VALUE(LF(1), 0)
    SET_VALUE(LF(2), 0)
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("tou_10"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("tou_08"))
        SET_VALUE(LF(1), 1)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("tou_07"))
        SET_VALUE(LF(2), 1)
    END_IF
    CALL(evt_npc_flag_onoff, 1, STRING("ガードマン"), 32)
    WAIT_FRAMES(1)
    CALL(evt_npc_move_position, STRING("ガードマン"), 0, -130, 0, FLOAT(80.0f), 1)
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, 180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
        IF_INT_EQUAL(LF(0), 1)
            CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        END_IF
        IF_INT_EQUAL(LF(1), 1)
            CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        END_IF
        IF_INT_EQUAL(LF(2), 1)
            CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        END_IF
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("ガードマン"), 0, -200, 0, FLOAT(80.0f), 1)
        CALL(evt_npc_set_position, STRING("ガードマン"), 0, -1000, 0)
    END_INLINE_EVENT
    WAIT_MS(800)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, 0, -200, 60)
    END_INLINE_EVENT
    WAIT_MS(500)
    IF_INT_EQUAL(LF(1), 1)
        SET_VALUE(GSWF(2394), 1)
    END_IF
    IF_INT_LESS(GSW(0), 172)
        IF_INT_EQUAL(GSWF(2383), 1)
            CALL(evt_bero_mapchange, STRING("tou_04"), 0)
            RETURN
        END_IF
    END_IF
    CALL(evt_bero_mapchange, STRING("tou_03"), 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_gardman)
    SET_VALUE(LF(0), 0)
    SET_VALUE(LF(1), 0)
    SET_VALUE(LF(2), 0)
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("tou_10"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("tou_08"))
        SET_VALUE(LF(1), 1)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("tou_07"))
        SET_VALUE(LF(2), 1)
    END_IF
    CALL(unk_tou_0000063c)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_wait_landon)
    WAIT_FRAMES(1)
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_GREATER(LW(2), -60)
            RETURN
        END_IF
        IF_INT_LESS(LW(0), -35)
            RETURN
        END_IF
        IF_INT_GREATER(LW(0), 35)
            RETURN
        END_IF
        CALL(evt_mario_normalize)
        CALL(evt_mario_dispflag_onoff, 1, 2)
        CALL(evt_mario_set_pose, STRING("M_N_7"))
        CALL(evt_npc_flag_onoff, 1, STRING("ガードマン"), 32)
        CALL(evt_mario_jump_pos, 40, 0, -120, 400, 1, -1000)
        CALL(evt_mario_set_pos, 40, 0, -120)
        CALL(evt_npc_flag_onoff, 0, STRING("ガードマン"), 32)
        WAIT_MS(500)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        CALL(evt_mario_dispflag_onoff, 0, 2)
        CALL(evt_mario_set_dir_npc, STRING("ガードマン"))
    END_INLINE_EVENT
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, -180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
        IF_INT_EQUAL(LF(0), 1)
            CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        END_IF
        IF_INT_EQUAL(LF(1), 1)
            CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        END_IF
        IF_INT_EQUAL(LF(2), 1)
            CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        END_IF
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_npc_status_onoff, 0, STRING("ガードマン"), 2)
    CALL(evt_npc_flag_onoff, 1, STRING("ガードマン"), 32768)
    CALL(evt_set_dir_to_target, STRING("ガードマン"), STRING("mario"))
    CALL(evt_npc_set_position, STRING("ガードマン"), 0, 0, -170)
    CALL(evt_npc_move_position, STRING("ガードマン"), 0, -90, 0, FLOAT(80.0f), 1)
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_close_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, -180, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
        IF_INT_EQUAL(LF(0), 1)
            CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        END_IF
        IF_INT_EQUAL(LF(1), 1)
            CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        END_IF
        IF_INT_EQUAL(LF(2), 1)
            CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        END_IF
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_set_dir_to_target, STRING("ガードマン"), STRING("mario"))
    IF_INT_EQUAL(LF(0), 1)
        CALL(evt_msg_print, 0, STRING("stg3_tou_74"), 0, STRING("ガードマン"))
    END_IF
    IF_INT_EQUAL(LF(1), 1)
        CALL(evt_msg_print, 0, STRING("stg3_tou_327"), 0, STRING("ガードマン"))
    END_IF
    IF_INT_EQUAL(LF(2), 1)
        CALL(evt_msg_print, 0, STRING("stg3_tou_453"), 0, STRING("ガードマン"))
    END_IF
    WAIT_MS(300)
    CALL(evt_npc_change_interrupt, STRING("ガードマン"), 6, PTR(talk_gardman))
    CALL(evt_npc_status_onoff, 1, STRING("ガードマン"), 2)
    CALL(evt_npc_flag_onoff, 0, STRING("ガードマン"), 32768)
    SET_VALUE(LW(0), 0)
    CALL_CHILD(bero_case_switch_off)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(unk_evt_tou_00012b54)
    CALL_CHILD(evt_gardman)
    RETURN
EVT_END()

static inline void doInsert(s32* arr, s32 unused, s32 from, s32 to) {
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

EVT_BEGIN(evt_tou_match_make_default_sub)
    SET_VALUE(LF(0), 0)
    SET_VALUE(LF(1), 0)
    SET_VALUE(LF(2), 0)
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("tou_10"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("tou_08"))
        SET_VALUE(LF(1), 1)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("tou_07"))
        SET_VALUE(LF(2), 1)
    END_IF
    CALL(evt_party_stop, 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LF(2), 1)
        CALL(evt_cam3d_evt_set, 35, 91, 188, 35, 57, -1, 1000, 11)
        BEGIN_INLINE_EVENT
            CALL(evt_party_move_pos2, 0, 75, -96, FLOAT(80.0f))
            CALL(evt_party_set_dir, 0, 270, 100)
        END_INLINE_EVENT
        CALL(evt_mario_mov_pos2, 35, -96, FLOAT(80.0f))
        CALL(evt_mario_mov_pos2, 38, -99, FLOAT(80.0f))
    ELSE
        CALL(evt_cam3d_evt_set, -75, 91, 188, -75, 57, -1, 1000, 11)
        BEGIN_INLINE_EVENT
            CALL(evt_party_move_pos2, 0, -50, -96, FLOAT(80.0f))
            CALL(evt_party_set_dir, 0, 270, 100)
        END_INLINE_EVENT
        CALL(evt_mario_mov_pos2, -90, -96, FLOAT(80.0f))
        CALL(evt_mario_mov_pos2, -87, -99, FLOAT(80.0f))
    END_IF
    CALL_CHILD(evt_tou_monitor_on)
    WAIT_MS(800)
    CALL(evt_party_run, 0)
    IF_INT_EQUAL(LF(0), 1)
        CALL(evt_mapobj_get_position, STRING("gamen"), LW(0), LW(1), LW(2))
        CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
        CALL(evt_tou_get_next_match, LW(0))
        SET_VALUE(GSW(501), LW(0))
        CALL(evt_tou_get_fighter_info_27_text_1CB8, LW(0), LW(1), LW(2))
        CALL(evt_tou_get_fighter_rank, LW(0), LW(3))
        IF_INT_LESS(GSW(0), 172)
            IF_INT_EQUAL(LW(3), 10)
                IF_INT_EQUAL(GSWF(2397), 0)
                    CALL(evt_msg_print_insert, 0, STRING("stg3_tou_107_00_01"), 0, STRING("ガンス"), LW(3), LW(1))
                ELSE
                    CALL(evt_msg_print_insert, 0, STRING("stg3_tou_107_00"), 0, STRING("ガンス"), LW(3), LW(1))
                END_IF
            ELSE
                CALL(evt_msg_print_insert, 0, STRING("stg3_tou_107_00"), 0, STRING("ガンス"), LW(3), LW(1))
            END_IF
        ELSE
            CALL(evt_msg_print_insert, 0, STRING("stg3_tou_re_361"), 0, STRING("キノシコワ"), LW(3), LW(1))
        END_IF
    END_IF
    IF_INT_EQUAL(LF(1), 1)
        CALL(evt_mapobj_get_position, STRING("gamenn"), LW(0), LW(1), LW(2))
        CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
        CALL(evt_tou_get_next_match, LW(0))
        SET_VALUE(GSW(501), LW(0))
        CALL(evt_tou_get_fighter_info_27_text_1CB8, LW(0), LW(1), LW(2))
        CALL(evt_tou_get_fighter_rank, LW(0), LW(3))
        IF_INT_LESS(GSW(0), 172)
            IF_INT_EQUAL(LW(3), 0)
                CALL(evt_msg_print_insert, 0, STRING("stg3_tou_107_00_01"), 0, STRING("ガンス"), LW(1))
            ELSE
                CALL(evt_msg_print_insert, 0, STRING("stg3_tou_107_00"), 0, STRING("ガンス"), LW(3), LW(1))
            END_IF
        ELSE
            IF_INT_EQUAL(LW(3), 0)
                CALL(evt_msg_print_insert, 0, STRING("stg3_tou_re_361_01"), 0, STRING("キノシコワ"), LW(1))
            ELSE
                CALL(evt_msg_print_insert, 0, STRING("stg3_tou_re_361"), 0, STRING("キノシコワ"), LW(3), LW(1))
            END_IF
        END_IF
        IF_INT_EQUAL(GSW(501), 0)
            IF_INT_EQUAL(GSWF(2399), 0)
                SET_VALUE(GSWF(2383), 1)
            END_IF
        END_IF
    END_IF
    IF_INT_EQUAL(LF(2), 1)
        CALL(evt_mapobj_get_position, STRING("gamenn"), LW(0), LW(1), LW(2))
        CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
        CALL(evt_tou_get_next_match, LW(0))
        SET_VALUE(GSW(501), LW(0))
        CALL(evt_tou_get_fighter_info_27_text_1CB8, LW(0), LW(1), LW(2))
        CALL(evt_tou_get_fighter_rank, LW(0), LW(3))
        IF_INT_LESS(GSW(0), 172)
            CALL(evt_msg_print_insert, 0, STRING("stg3_tou_107_00"), 0, STRING("ガンス"), LW(3), LW(1))
        ELSE
            CALL(evt_msg_print_insert, 0, STRING("stg3_tou_re_361"), 0, STRING("キノシコワ"), LW(3), LW(1))
        END_IF
    END_IF
    CALL(evt_tou_get_rule_info, GSW(501), LW(10), LW(13), LW(14), LW(15))
    SET_VALUE(GSW(502), LW(13))
    SET_VALUE(GSW(503), LW(14))
    SET_VALUE(GSW(504), LW(15))
    CALL(evt_msg_print_add, 0, LW(10))
    CALL_CHILD(evt_tou_monitor_off)
    WAIT_MS(500)
    SET_VALUE(LW(0), 0)
    CALL_CHILD(bero_case_switch_off)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
    IF_INT_EQUAL(LW(0), 3)
        IF_INT_EQUAL(GSWF(2448), 0)
            SET_VALUE(GSWF(2448), 1)
            CALL_CHILD(evt_doku_cake)
        END_IF
    END_IF
    SPAWN_EVENT(evt_gardman)
    SET_VALUE(GSWF(2388), 1)
    IF_INT_LESS(GSW(0), 172)
        IF_INT_EQUAL(GSW(501), 18)
            IF_INT_EQUAL(GSWF(2405), 0)
                SET_VALUE(GSWF(2405), 1)
                CALL_CHILD(evt_michel_1)
            END_IF
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_tou_match_make_default)
    CALL(unk_tou_000005d0, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        RETURN
    END_IF
    CALL(evt_hit_get_position, STRING("M_make"), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_TOUROKU_COM1"), LW(10), LW(11), LW(12), 0)
    SET_VALUE(LF(0), 0)
    SET_VALUE(LF(1), 0)
    SET_VALUE(LF(2), 0)
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("tou_10"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("tou_08"))
        SET_VALUE(LF(1), 1)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("tou_07"))
        SET_VALUE(LF(2), 1)
    END_IF
    CALL(evt_mario_key_onoff, 0)
    IF_INT_EQUAL(LF(0), 1)
        SET_VALUE(LW(10), STRING("stg3_tou_103"))
        SET_VALUE(LW(11), STRING("stg3_tou_103_yn"))
        SET_VALUE(LW(12), STRING("stg3_tou_104_01"))
    END_IF
    IF_INT_EQUAL(LF(1), 1)
        SET_VALUE(LW(10), STRING("stg3_tou_105"))
        SET_VALUE(LW(11), STRING("stg3_tou_105_yn"))
        SET_VALUE(LW(12), STRING("stg3_tou_106_01"))
    END_IF
    IF_INT_EQUAL(LF(2), 1)
        SET_VALUE(LW(10), STRING("stg3_tou_476"))
        SET_VALUE(LW(11), STRING("stg3_tou_476_yn"))
        SET_VALUE(LW(12), STRING("stg3_tou_477_01"))
    END_IF
    CALL(evt_msg_print, 0, LW(10), 0, 0)
    CALL(evt_msg_select, 0, LW(11))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        SET_VALUE(LF(10), 0)
        SET_VALUE(LF(11), 0)
        SWITCH(GSW(0))
            CASE_BETWEEN(139, 142)
            SET_VALUE(LF(10), 1)
            IF_INT_EQUAL(LF(0), 1)
                CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
                IF_INT_LESS(LW(0), 11)
                    SET_VALUE(LF(10), 0)
                    SET_VALUE(LF(11), 1)
                END_IF
            END_IF
            CASE_EQUAL(143)
            IF_INT_EQUAL(LF(0), 1)
                CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
                IF_INT_LESS(LW(0), 11)
                    SET_VALUE(LF(11), 1)
                END_IF
            END_IF
            CASE_BETWEEN(144, 151)
            SET_VALUE(LF(10), 1)
            CASE_BETWEEN(153, 155)
            SET_VALUE(LF(10), 1)
            CASE_BETWEEN(158, 163)
            SET_VALUE(LF(10), 1)
            CASE_GREATER_OR_EQUAL(172)
            IF_INT_EQUAL(LF(0), 1)
                CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
                IF_INT_EQUAL(LW(0), 10)
                    SET_VALUE(LF(11), 1)
                END_IF
            END_IF
        END_SWITCH
        IF_INT_EQUAL(LF(10), 1)
            CALL(evt_party_stop, 0)
            CALL(evt_msg_print_add, 1, STRING("<scrl_auto><once_stop>"))
            CALL(evt_msg_print_add, 0, STRING("stg3_tou_106_02"))
            CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
            IF_INT_LESS(LW(0), LW(3))
                CALL(evt_mario_set_dir, 270, 300, 0)
            ELSE
                CALL(evt_mario_set_dir, 90, 300, 0)
            END_IF
            CALL(evt_msg_print_party, STRING("stg3_tou_106_03"))
            CALL(evt_party_run, 0)
            CALL(evt_mario_key_onoff, 1)
            RETURN
        END_IF
        IF_INT_EQUAL(LF(11), 1)
            CALL(evt_msg_print_add, 1, STRING("<scrl_auto><once_stop>"))
            CALL(evt_msg_print_add, 0, STRING("stg3_tou_104_01_1"))
            CALL(evt_mario_key_onoff, 1)
            RETURN
        END_IF
        IF_INT_EQUAL(GSWF(2388), 1)
            CALL(evt_tou_get_fighter_info_27_text_1CB8, GSW(501), LW(0), LW(1))
            CALL(evt_tou_get_rule_msg2_27_text_182C, GSW(502), LW(2))
            CALL(evt_msg_print_add, 1, STRING("<scrl_auto><once_stop>"))
            CALL(evt_msg_print_add_insert, 0, LW(12), LW(0), LW(2))
        ELSE
            CALL(evt_msg_continue)
            CALL_CHILD(evt_tou_match_make_default_sub)
        END_IF
        CASE_EQUAL(1)
        CALL(evt_msg_continue)
        CALL(evt_tou_print_ranking)
        CASE_DEFAULT
        CALL(evt_msg_continue)
    END_SWITCH
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_talk_doku_cake)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("stg3_tou_307_04"), 0, STRING("me"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_eat_doku_cake)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_party_get_pos, 0, LW(3), LW(4), LW(5))
    CALL(evt_sub_get_dist, LW(0), LW(2), LW(3), LW(5), LW(10))
    IF_INT_GREATER(LW(10), 100)
        CALL(evt_party_set_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_party_set_dir_pos, 0, 20, -20)
    END_IF
    WAIT_MS(200)
    CALL(evt_msg_print_party, STRING("stg3_tou_296_doku"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_297_doku"), 0, 0)
    CALL(evt_msg_select, 0, STRING("stg3_tou_297_yn_doku"))
    CALL(evt_msg_continue)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_party, STRING("stg3_tou_298_doku"))
        CALL(evt_mario_key_onoff, 1)
        CALL(evt_party_run, 0)
        RETURN
    END_IF
    CALL(evt_party_flg_onoff, 0, 16777216, 1)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    BEGIN_INLINE_EVENT
        SUBTRACT(LW(2), 30)
        CALL(evt_mario_mov_pos2, LW(0), LW(2), FLOAT(80.0f))
        CALL(evt_mario_adjust_dir)
    END_INLINE_EVENT
    CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(80.0f))
    CALL(evt_msg_print_party, STRING("stg3_tou_299_doku"))
    CALL(evt_item_delete, STRING("doku_cake"))
    CALL(evt_party_set_breed_pose, 0, 7)
    BEGIN_INLINE_EVENT_GET_ID(LW(0))
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_EAT_CAKE1"), LW(0), LW(1), LW(2), 0)
            WAIT_MS(500)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(1000)
    DELETE_EVENT(LW(0))
    CALL(evt_msg_print_party, STRING("stg3_tou_300"))
    WAIT_MS(300)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_snd_envoff, 512)
    CALL(evt_eff_fukidashi, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SURPRISED2"))
        WAIT_MS(1000)
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
    END_INLINE_EVENT
    CALL_CHILD(party_npc_replace)
    WAIT_MS(300)
    CALL(evt_mario_get_party, LW(3))
    SWITCH(LW(3))
        CASE_EQUAL(1)
        CALL(evt_msg_print, 0, STRING("stg3_tou_301_kur"), 0, STRING("dummy_party"))
        CASE_EQUAL(2)
        CALL(evt_msg_print, 0, STRING("stg3_tou_301_nok"), 0, STRING("dummy_party"))
        CASE_EQUAL(5)
        CALL(evt_msg_print, 0, STRING("stg3_tou_301_win"), 0, STRING("dummy_party"))
        CASE_EQUAL(4)
        CALL(evt_msg_print, 0, STRING("stg3_tou_301_yos"), 0, STRING("dummy_party"))
    END_SWITCH
    WAIT_MS(300)
    SET_VALUE(GSWF(2449), 0)
    SET_VALUE(GSWF(2450), 1)
    CALL(evt_party_flg_onoff, 0, 16777216, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_kill_party, 0)
    CALL(evt_pouch_party_left, 1)
    CALL(evt_pouch_party_left, 2)
    CALL(evt_pouch_party_left, 5)
    CALL(evt_pouch_party_left, 4)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU1"))
    CALL(evt_snd_bgmoff, 26624)
    CALL(evt_snd_envon, 272, STRING("ENV_STG3_TOU6"))
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(evt_doku_cake)
    WAIT_MS(2000)
    CALL(evt_npc_set_position, STRING("キノシコワ"), 0, 0, -180)
    CALL(evt_npc_set_ry, STRING("キノシコワ"), 270)
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, -180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
        CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        SET_VALUE(LW(1), 0)
        SPAWN_EVENT(bero_close_door_play_se)
        CALL(evt_sub_intpl_msec_init, 11, -180, 0, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
            CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_mario_set_dir_npc, STRING("キノシコワ"))
        CALL(evt_party_set_dir_npc, 0, STRING("キノシコワ"))
    END_INLINE_EVENT
    CALL(evt_npc_move_position, STRING("キノシコワ"), -1, -25, 0, FLOAT(80.0f), 1)
    CALL(evt_set_dir_to_target, STRING("キノシコワ"), STRING("mario"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_295_doku"), 0, STRING("キノシコワ"))
    CALL(evt_npc_set_ry, STRING("キノシコワ"), 90)
    WAIT_MS(300)
    CALL(evt_item_entry, STRING("doku_cake"), 66, 20, 25, -25, 17, -1, 0)
    CALL(evt_item_status_on, STRING("doku_cake"), 33554432)
    CALL(evt_run_case_evt, 9, 1, STRING("a_cake"), 0, PTR(evt_eat_doku_cake), 0)
    WAIT_MS(500)
    CALL(evt_npc_move_position, STRING("キノシコワ"), 0, -130, 0, FLOAT(80.0f), 1)
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, 180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
        CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_npc_move_position, STRING("キノシコワ"), 0, -200, 0, FLOAT(80.0f), 1)
    CALL(evt_npc_set_position, STRING("キノシコワ"), 0, -1000, 0)
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_close_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 180, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
        CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    SET_VALUE(GSWF(2449), 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_michel_1)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir_npc, STRING("マイケル"))
        CALL(evt_party_set_dir_npc, 0, STRING("マイケル"))
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, -20, 93, 290, -20, 57, -8, 1000, 11)
    CALL(evt_npc_move_position, STRING("マイケル"), 40, -80, 0, FLOAT(60.0f), 1)
    CALL(evt_msg_print, 0, STRING("stg3_npc_10_01_01"), 0, STRING("マイケル"))
    CALL(evt_cam3d_evt_off, 500, 11)
    RETURN
EVT_END()

EVT_BEGIN(evt_michel_2)
    CALL(evt_cam3d_evt_set, -45, 105, 288, -45, 60, -9, 2000, 11)
    CALL(evt_npc_move_position, STRING("マイケル"), 0, -80, 0, FLOAT(60.0f), 1)
    CALL(evt_msg_print, 0, STRING("stg3_npc_10_01_02"), 0, STRING("マイケル"))
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_npc_move_position, STRING("マイケル"), 90, -90, 0, FLOAT(60.0f), 1)
    CALL(evt_npc_set_ry, STRING("マイケル"), 270)
    RETURN
EVT_END()

EVT_BEGIN(evt_michel_3_sub)
    SET_VALUE(LW(3), LW(0))
    CALL(evt_npc_move_position, LW(3), 0, -130, 0, FLOAT(80.0f), 1)
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, 180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
        CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, LW(3), 0, -200, 0, FLOAT(80.0f), 1)
        CALL(evt_npc_set_position, LW(3), 0, -1000, 0)
    END_INLINE_EVENT
    WAIT_MS(800)
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_close_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 180, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
        CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_michel_3)
    CALL(evt_set_dir_to_target, STRING("マイケル"), STRING("mario"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_17"), 0, STRING("マイケル"))
    CALL(evt_cam3d_evt_set, 100, 93, 290, 100, 57, -8, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_18"), 0, STRING("マイケル"))
    CALL(evt_npc_set_position, STRING("ブロッツ"), 0, 0, -200)
    CALL(evt_npc_set_position, STRING("キノシコワ"), 0, 0, -170)
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_19"), 0, STRING("キノシコワ"))
    CALL(evt_cam3d_evt_set, 0, 112, 287, 0, 60, -8, 1000, 11)
    WAIT_MS(1000)
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, 180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
        CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL(evt_npc_status_onoff, 1, STRING("ブロッツ"), 2)
        CALL(evt_npc_move_position, STRING("ブロッツ"), 1, -75, 0, FLOAT(80.0f), 1)
        CALL(evt_npc_move_position, STRING("ブロッツ"), -25, -75, 0, FLOAT(80.0f), 1)
        CALL(evt_set_dir_to_target, STRING("ブロッツ"), STRING("キノシコワ"))
    END_INLINE_EVENT
    CALL(evt_npc_status_onoff, 1, STRING("キノシコワ"), 2)
    CALL(evt_npc_move_position, STRING("キノシコワ"), 1, -75, 0, FLOAT(80.0f), 1)
    CALL(evt_npc_move_position, STRING("キノシコワ"), 25, -75, 0, FLOAT(80.0f), 1)
    CALL(evt_set_dir_to_target, STRING("キノシコワ"), STRING("ブロッツ"))
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_close_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 180, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
        CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_20"), 0, STRING("キノシコワ"))
    WAIT_MS(300)
    CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("M_1"))
    WAIT_MS(600)
    CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("S_1"))
    SET_VALUE(LW(0), STRING("キノシコワ"))
    CALL_CHILD(evt_michel_3_sub)
    CALL(evt_npc_move_position, STRING("マイケル"), 25, -75, 0, FLOAT(80.0f), 1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_21"), 0, STRING("マイケル"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_22"), 0, STRING("ブロッツ"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_23"), 0, STRING("マイケル"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_23_01"), 0, STRING("ブロッツ"))
    SET_VALUE(LW(0), STRING("ブロッツ"))
    CALL_CHILD(evt_michel_3_sub)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_npc_move_position, STRING("マイケル"), 90, -90, 0, FLOAT(60.0f), 1)
    CALL(evt_npc_set_ry, STRING("マイケル"), 270)
    RETURN
EVT_END()

EVT_BEGIN(evt_michel_4)
    CALL(evt_cam3d_evt_set, -45, 105, 288, -45, 60, -9, 2000, 11)
    CALL(evt_npc_move_position, STRING("マイケル"), 0, -80, 0, FLOAT(60.0f), 1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_24"), 0, STRING("マイケル"))
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_2"), LW(0), LW(1), LW(2), 0)
        CALL(evt_eff_fukidashi, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
        CALL(evt_eff_fukidashi, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
        CALL(evt_eff_fukidashi, 1, 0, STRING("マイケル"), 0, 0, 0, 0, 0, 0, 0, 0)
    END_INLINE_EVENT
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_24_01"), 0, 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_mario_set_dir, 270, 300, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_party_set_dir, 0, 270, 300)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, -165, 105, 288, -165, 60, -9, 2000, 11)
    WAIT_MS(2000)
    CALL(evt_cam3d_evt_set, 185, 105, 288, 185, 60, -9, 15000, 11)
    CALL(evt_msg_toge, 2, -165, 50, -200)
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_24_02"), 0, 0)
    CALL(evt_msg_toge, 2, -125, 40, -200)
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_24_03"), 0, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir, 90, 300, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_set_dir, 0, 90, 300)
    END_INLINE_EVENT
    CALL(evt_msg_toge, 2, -85, 20, -200)
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_24_04"), 0, 0)
    CALL(evt_msg_toge, 2, -45, 50, -200)
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_24_05"), 0, 0)
    CALL(evt_msg_toge, 2, 0, 40, -200)
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_24_06"), 0, 0)
    CALL(evt_npc_set_ry, STRING("マイケル"), 90)
    CALL(evt_msg_toge, 2, 45, 10, -200)
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_24_07"), 0, 0)
    CALL(evt_msg_toge, 2, 85, 30, -200)
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_24_08"), 0, 0)
    CALL(evt_msg_toge, 2, 125, 40, -200)
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_24_09"), 0, 0)
    CALL(evt_msg_toge, 2, 165, 50, -200)
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_24_10"), 0, 0)
    CALL(evt_cam3d_evt_set, -45, 105, 288, -45, 60, -9, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_24_11"), 0, STRING("マイケル"))
    WAIT_MS(200)
    CALL(evt_npc_set_ry, STRING("マイケル"), 270)
    WAIT_MS(200)
    CALL(evt_set_dir_to_target, STRING("マイケル"), STRING("mario"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_24_12"), 0, STRING("マイケル"))
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_npc_move_position, STRING("マイケル"), 90, -90, 0, FLOAT(60.0f), 1)
    CALL(evt_npc_set_ry, STRING("マイケル"), 270)
    RETURN
EVT_END()

EVT_BEGIN(evt_michel_5)
    CALL(evt_cam3d_evt_set, -45, 105, 288, -45, 60, -9, 2000, 11)
    CALL(evt_npc_move_position, STRING("マイケル"), 0, -80, 0, FLOAT(60.0f), 1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_25"), 0, STRING("マイケル"))
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_npc_move_position, STRING("マイケル"), 90, -90, 0, FLOAT(60.0f), 1)
    CALL(evt_npc_set_ry, STRING("マイケル"), 270)
    RETURN
EVT_END()

EVT_BEGIN(evt_michel_6)
    CALL(evt_cam3d_evt_set, -45, 105, 288, -45, 60, -9, 2000, 11)
    CALL(evt_npc_move_position, STRING("マイケル"), 0, -80, 0, FLOAT(60.0f), 1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_26"), 0, STRING("マイケル"))
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_npc_move_position, STRING("マイケル"), 90, -90, 0, FLOAT(60.0f), 1)
    CALL(evt_npc_set_ry, STRING("マイケル"), 270)
    RETURN
EVT_END()

EVT_BEGIN(evt_michel_7)
    CALL(evt_cam3d_evt_set, -45, 105, 288, -45, 60, -9, 2000, 11)
    CALL(evt_npc_move_position, STRING("マイケル"), 0, -80, 0, FLOAT(60.0f), 1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_27"), 0, STRING("マイケル"))
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_npc_move_position, STRING("マイケル"), 90, -90, 0, FLOAT(60.0f), 1)
    CALL(evt_npc_set_ry, STRING("マイケル"), 270)
    RETURN
EVT_END()

__declspec(section ".rodata") Vec vec3_tou_0000a2b4 = { 80.0f, 0.0f, 85.0f };

EVT_BEGIN(evt_michel_8)
    CALL(evt_npc_set_position, STRING("オクトール"), 145, 0, 50)
    CALL(evt_npc_set_position, STRING("ガンガン"), 95, 0, 50)
    CALL(evt_cam3d_evt_set, 95, 109, 419, 95, 45, -6, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_28"), 0, STRING("オクトール"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_29"), 0, STRING("ガンガン"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_30"), 0, STRING("オクトール"))
    CALL(evt_cam3d_evt_set, 0, 112, 287, 0, 60, -8, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_npc_set_position, STRING("ポグ"), -1, 0, -200)
    CALL(evt_npc_set_position, STRING("キノシコワ"), 0, 0, -180)
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, -180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
        CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL(evt_npc_move_position, STRING("ポグ"), 0, -80, 0, FLOAT(80.0f), 1)
        CALL(evt_set_dir_to_target, STRING("ポグ"), STRING("キノシコワ"))
    END_INLINE_EVENT
    CALL(evt_npc_move_position, STRING("キノシコワ"), 0, -120, 0, FLOAT(80.0f), 1)
    CALL(evt_npc_move_position, STRING("キノシコワ"), 40, -100, 0, FLOAT(80.0f), 1)
    CALL(evt_set_dir_to_target, STRING("キノシコワ"), STRING("ポグ"))
    WAIT_MS(500)
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_close_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, -180, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.472f))
        CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_31"), 0, STRING("キノシコワ"))
    WAIT_MS(300)
    CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("M_1"))
    WAIT_MS(600)
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_32"), 0, STRING("キノシコワ"))
    CALL(evt_cam3d_evt_set, -70, 112, 287, -70, 60, -8, 1000, 11)
    CALL(evt_npc_move_position, STRING("キノシコワ"), -40, -100, 0, FLOAT(80.0f), 1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_33"), 0, STRING("キノシコワ"))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, -110, -70, FLOAT(60.0f))
        CALL(evt_mario_set_dir, 90, 300, 0)
    END_INLINE_EVENT
    CALL(evt_npc_move_position, STRING("ポグ"), -70, -95, 0, FLOAT(80.0f), 1)
    CALL(evt_npc_move_position, STRING("ポグ"), -70, -120, 0, FLOAT(80.0f), 1)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_34"), 0, STRING("キノシコワ"))
    CALL(evt_npc_get_position, STRING("ポグ"), LW(0), LW(1), LW(2))
    CALL(evt_npc_jump_position_nohit, STRING("ポグ"), LW(0), LW(1), LW(2), 500, 30)
    CALL_CHILD(evt_tou_monitor_on)
    WAIT_MS(800)
    CALL(evt_mapobj_get_position, STRING("gamen"), LW(0), LW(1), LW(2))
    CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_35"), 0, STRING("ガンス"))
    CALL_CHILD(evt_tou_monitor_off)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_36"), 0, STRING("キノシコワ"))
    CALL(evt_npc_get_position, STRING("ポグ"), LW(0), LW(1), LW(2))
    CALL(evt_npc_jump_position_nohit, STRING("ポグ"), LW(0), LW(1), LW(2), 500, 30)
    CALL(evt_msg_continue)
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_37"), 0, STRING("キノシコワ"))
    WAIT_MS(300)
    CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("M_1"))
    WAIT_MS(600)
    CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("S_1"))
    CALL(evt_npc_move_position, STRING("ポグ"), -70, -95, 0, FLOAT(80.0f), 1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_38"), 0, STRING("ポグ"))
    WAIT_MS(2000)
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, -180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.472f))
        CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_npc_set_position, STRING("ガードマン"), -1, 0, -170)
    CALL(evt_npc_move_position, STRING("ガードマン"), 0, -100, 0, FLOAT(80.0f), 1)
    CALL(evt_set_dir_to_target, STRING("ガードマン"), STRING("mario"))
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_close_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, -180, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.472f))
        CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_38_00"), 0, STRING("ガードマン"))
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg3_tou_93_39"), 0, STRING("キノシコワ"))
    WAIT_MS(300)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("キノシコワ"), 30, -100, 0, FLOAT(80.0f), 1)
        CALL(evt_set_dir_to_target, STRING("キノシコワ"), STRING("ポグ"))
    END_INLINE_EVENT
    CALL(evt_npc_move_position, STRING("ガードマン"), 0, -130, 0, FLOAT(80.0f), 1)
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, 180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.472f))
        CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("ガードマン"), 0, -200, 0, FLOAT(80.0f), 1)
        CALL(evt_npc_set_position, STRING("ガードマン"), 0, -1000, 0)
    END_INLINE_EVENT
    WAIT_MS(300)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("ポグ"), 0, -120, 0, FLOAT(80.0f), 1)
        CALL(evt_npc_move_position, STRING("ポグ"), 0, -200, 0, FLOAT(80.0f), 1)
        CALL(evt_npc_set_position, STRING("ポグ"), 0, -1000, 0)
    END_INLINE_EVENT
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("キノシコワ"), 0, -120, 0, FLOAT(80.0f), 1)
        CALL(evt_npc_move_position, STRING("キノシコワ"), 0, -200, 0, FLOAT(80.0f), 1)
        CALL(evt_npc_set_position, STRING("キノシコワ"), 0, -1000, 0)
    END_INLINE_EVENT
    WAIT_MS(2000)
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_close_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 180, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
        CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_npc_set_position, STRING("ガンガン"), -75, 0, 90)
    CALL(evt_npc_set_ry, STRING("ガンガン"), 90)
    CALL(evt_cam3d_evt_off, 500, 11)
    RETURN
EVT_END()

EVT_BEGIN(evt_michel_9)
    CALL(evt_cam3d_evt_set, -45, 105, 288, -45, 60, -9, 2000, 11)
    CALL(evt_npc_move_position, STRING("ポグ"), 0, -80, 0, FLOAT(60.0f), 1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_255_01"), 0, STRING("ポグ"))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("ポグ"), 90, -90, 0, FLOAT(60.0f), 1)
        CALL(evt_npc_set_ry, STRING("ポグ"), 270)
    END_INLINE_EVENT
    RETURN
EVT_END()

EVT_BEGIN(evt_mail_1)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT_GET_ID(LW(11))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_3"), LW(0), LW(1), LW(2), 0)
        CALL(evt_eff_fukidashi, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
        WAIT_MS(1500)
    END_INLINE_EVENT
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_MAIL_RECEPTION2"))
    SPAWN_EVENT_GET_ID(PTR(evt_sub_mail_vibration), LW(10))
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("stg3_tou_297_03"))
    LOOP(0)
        IS_EVENT_RUNNING(LW(11), LW(12))
        IF_INT_EQUAL(LW(12), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_balloon_bikkuri)
    LOOP(0)
        CALL(evt_key_get_buttontrg, 0, LW(0))
        IF_INT_EQUAL(LW(0), 256)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_balloon_off)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MAIL_RECEPTION1"), LW(0), LW(1), LW(2), 0)
    DELETE_EVENT(LW(10))
    WAIT_FRAMES(1)
    CALL(evt_sub_rumble_onoff, 1, 0)
    CALL(evt_mario_set_pose, STRING("M_N_9"))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MAIL_RECEPTION2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_10"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg3_tou_297_04"), 0, 0)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print_party, STRING("stg3_tou_297_05"))
    CALL(evt_pouch_receive_mail, 7)
    CALL(evt_pouch_open_mail, 7)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU1"))
    SET_VALUE(GSW(0), 139)
    RETURN
EVT_END()

EVT_BEGIN(evt_mail_2)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT_GET_ID(LW(11))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_3"), LW(0), LW(1), LW(2), 0)
        CALL(evt_eff_fukidashi, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
        WAIT_MS(1500)
    END_INLINE_EVENT
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_MAIL_RECEPTION2"))
    SPAWN_EVENT_GET_ID(PTR(evt_sub_mail_vibration), LW(10))
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("stg3_tou_297_22"))
    LOOP(0)
        IS_EVENT_RUNNING(LW(11), LW(12))
        IF_INT_EQUAL(LW(12), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_balloon_bikkuri)
    LOOP(0)
        CALL(evt_key_get_buttontrg, 0, LW(0))
        IF_INT_EQUAL(LW(0), 256)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_balloon_off)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MAIL_RECEPTION1"), LW(0), LW(1), LW(2), 0)
    DELETE_EVENT(LW(10))
    WAIT_FRAMES(1)
    CALL(evt_sub_rumble_onoff, 1, 0)
    CALL(evt_mario_set_pose, STRING("M_N_9"))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MAIL_RECEPTION2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_10"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg3_tou_297_23"), 0, 0)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print_party, STRING("stg3_tou_297_24"))
    CALL(evt_pouch_receive_mail, 9)
    CALL(evt_pouch_open_mail, 9)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU1"))
    RETURN
EVT_END()

EVT_BEGIN(evt_eat_cake)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_party_get_pos, 0, LW(3), LW(4), LW(5))
    CALL(evt_sub_get_dist, LW(0), LW(2), LW(3), LW(5), LW(10))
    IF_INT_GREATER(LW(10), 100)
        CALL(evt_party_set_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_party_set_dir_pos, 0, 20, -20)
    END_IF
    WAIT_MS(200)
    CALL(evt_msg_print_party, STRING("stg3_tou_296"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_297"), 0, 0)
    CALL(evt_msg_select, 0, STRING("stg3_tou_297_yn"))
    CALL(evt_msg_continue)
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_party_flg_onoff, 0, 16777216, 1)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        BEGIN_INLINE_EVENT
            SUBTRACT(LW(2), 30)
            CALL(evt_mario_mov_pos2, LW(0), LW(2), FLOAT(80.0f))
            CALL(evt_mario_adjust_dir)
        END_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(80.0f))
        CALL(evt_msg_print_party, STRING("stg3_tou_299"))
        CALL(evt_msg_print, 0, STRING("stg3_tou_297_1"), 0, 0)
        CALL(evt_item_delete, STRING("cake"))
        CALL(evt_party_set_breed_pose, 0, 7)
        BEGIN_INLINE_EVENT_GET_ID(LW(0))
            LOOP(0)
                CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_EAT_CAKE1"), LW(0), LW(1), LW(2), 0)
                WAIT_MS(500)
            END_LOOP
        END_INLINE_EVENT
        WAIT_MS(1000)
        DELETE_EVENT(LW(0))
        CALL(evt_pouch_mario_recovery)
        CALL(evt_pouch_all_party_recovery)
        SET_VALUE(GSWF(2447), 0)
        CALL(evt_party_flg_onoff, 0, 16777216, 0)
        CALL(evt_exit_case_evt)
    ELSE
        CALL(evt_msg_print_party, STRING("stg3_tou_298"))
    END_IF
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_cake_init)
    CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
    IF_INT_EQUAL(LW(0), 6)
        IF_INT_EQUAL(GSWF(2418), 0)
            IF_INT_EQUAL(GSWF(2447), 0)
                CALL(evt_item_entry, STRING("cake"), 160, 20, 25, -25, 17, -1, 0)
                CALL(evt_item_status_on, STRING("cake"), 33554432)
            END_IF
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_cake)
    CALL(evt_run_case_evt, 9, 1, STRING("a_cake"), 0, PTR(evt_eat_cake), 0)
    CALL(evt_cam3d_evt_set, 55, 102, 362, 55, 45, -9, 1000, 11)
    WAIT_MS(1100)
    CALL(evt_msg_print, 0, STRING("stg3_tou_295"), 0, STRING("トゲノコエース"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_off, 500, 11)
    SET_VALUE(GSWF(2447), 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_koopa_after)
    CALL(evt_msg_print_party, STRING("stg3_tou_403_11"))
    RETURN
EVT_END()

EVT_BEGIN(evt_mail_3)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT_GET_ID(LW(11))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_3"), LW(0), LW(1), LW(2), 0)
        CALL(evt_eff_fukidashi, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
        WAIT_MS(1500)
    END_INLINE_EVENT
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_MAIL_RECEPTION2"))
    SPAWN_EVENT_GET_ID(PTR(evt_sub_mail_vibration), LW(10))
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("stg3_tou_297_25"))
    LOOP(0)
        IS_EVENT_RUNNING(LW(11), LW(12))
        IF_INT_EQUAL(LW(12), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_balloon_bikkuri)
    LOOP(0)
        CALL(evt_key_get_buttontrg, 0, LW(0))
        IF_INT_EQUAL(LW(0), 256)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_balloon_off)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MAIL_RECEPTION1"), LW(0), LW(1), LW(2), 0)
    DELETE_EVENT(LW(10))
    WAIT_FRAMES(1)
    CALL(evt_sub_rumble_onoff, 1, 0)
    CALL(evt_mario_set_pose, STRING("M_N_9"))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MAIL_RECEPTION2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_10"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg3_tou_297_26"), 0, 0)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print_party, STRING("stg3_tou_297_27"))
    CALL(evt_pouch_receive_mail, 10)
    CALL(evt_pouch_open_mail, 10)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU1"))
    SET_VALUE(GSW(0), 144)
    RETURN
EVT_END()

EVT_BEGIN(evt_mail_4)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT_GET_ID(LW(11))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_3"), LW(0), LW(1), LW(2), 0)
        CALL(evt_eff_fukidashi, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
        WAIT_MS(1500)
    END_INLINE_EVENT
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_MAIL_RECEPTION2"))
    SPAWN_EVENT_GET_ID(PTR(evt_sub_mail_vibration), LW(10))
    WAIT_MS(500)
    LOOP(0)
        IS_EVENT_RUNNING(LW(11), LW(12))
        IF_INT_EQUAL(LW(12), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_balloon_bikkuri)
    LOOP(0)
        CALL(evt_key_get_buttontrg, 0, LW(0))
        IF_INT_EQUAL(LW(0), 256)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_balloon_off)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MAIL_RECEPTION1"), LW(0), LW(1), LW(2), 0)
    DELETE_EVENT(LW(10))
    WAIT_FRAMES(1)
    CALL(evt_sub_rumble_onoff, 1, 0)
    CALL(evt_mario_set_pose, STRING("M_N_9"))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MAIL_RECEPTION2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_10"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg3_tou_326_01"), 0, 0)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print_party, STRING("stg3_tou_326_02"))
    CALL(evt_pouch_receive_mail, 13)
    CALL(evt_pouch_open_mail, 13)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU1"))
    RETURN
EVT_END()

EVT_BEGIN(evt_mail_5)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT_GET_ID(LW(11))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_3"), LW(0), LW(1), LW(2), 0)
        CALL(evt_eff_fukidashi, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
        WAIT_MS(1500)
    END_INLINE_EVENT
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_MAIL_RECEPTION2"))
    SPAWN_EVENT_GET_ID(PTR(evt_sub_mail_vibration), LW(10))
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("stg3_tou_307_05"))
    LOOP(0)
        IS_EVENT_RUNNING(LW(11), LW(12))
        IF_INT_EQUAL(LW(12), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_balloon_bikkuri)
    LOOP(0)
        CALL(evt_key_get_buttontrg, 0, LW(0))
        IF_INT_EQUAL(LW(0), 256)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_balloon_off)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MAIL_RECEPTION1"), LW(0), LW(1), LW(2), 0)
    DELETE_EVENT(LW(10))
    WAIT_FRAMES(1)
    CALL(evt_sub_rumble_onoff, 1, 0)
    CALL(evt_mario_set_pose, STRING("M_N_9"))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MAIL_RECEPTION2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_10"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg3_tou_307_06"), 0, 0)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print_party, STRING("stg3_tou_307_07"))
    CALL(evt_pouch_receive_mail, 14)
    CALL(evt_pouch_open_mail, 14)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU1"))
    SET_VALUE(GSW(0), 153)
    RETURN
EVT_END()

EVT_BEGIN(evt_tamago_hello)
    CALL(evt_npc_check, STRING("たまご"), LW(10))
    IF_INT_EQUAL(LW(10), 0)
        CALL(evt_npc_entry, STRING("たまご"), STRING("c_babyyoshi"))
    END_IF
    CALL(evt_npc_set_tribe, STRING("たまご"), STRING("たまご"))
    CALL(evt_npc_set_anim, STRING("たまご"), STRING("PYS_E_2"))
    CALL(evt_npc_set_position, STRING("たまご"), -30, 20, 30)
    CALL(evt_party_set_pos, 1, 0, -1000, 0)
    CALL(evt_party_stop, 1)
    CALL(evt_party_cont_onoff, 1, 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_tamago_hello2)
    LOOP(3)
        CALL(evt_npc_get_position, STRING("たまご"), LW(0), LW(1), LW(2))
        CALL(evt_sub_random, 200, LW(3))
        ADD(LW(3), 200)
        CALL(evt_sub_random, 20, LW(4))
        ADD(LW(4), 20)
        SPAWN_EVENT(evt_tamago_scale)
        CALL(evt_npc_jump_position_nohit, STRING("たまご"), LW(0), LW(1), LW(2), LW(3), LW(4))
    END_LOOP
    CALL(evt_npc_get_position, STRING("たまご"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_position, STRING("たまご"), 0, -1000, 0)
    CALL(evt_mario_get_exparty, LW(3))
    IF_INT_NOT_EQUAL(LW(3), 0)
        CALL(evt_party_set_pos, 1, LW(0), LW(1), LW(2))
    ELSE
        CALL(evt_mario_set_party_pos, 1, 8, LW(0), LW(1), LW(2))
    END_IF
    CALL(evt_party_run, 1)
    CALL(evt_party_cont_onoff, 1, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_tou_ranking_msg)
    CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
    IF_INT_LESS(LW(0), GSW(506))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_msg_print_add, 0, STRING("stg3_rank_05"))
        ELSE
            CALL(evt_msg_print_add_insert, 0, STRING("stg3_rank_01"), LW(0))
        END_IF
        RETURN
    END_IF
    IF_INT_GREATER(LW(0), GSW(506))
        CALL(evt_msg_print_add_insert, 0, STRING("stg3_rank_04"), LW(0))
        RETURN
    END_IF
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("stg3_rank_06"))
        RETURN
    END_IF
    CALL(evt_tou_get_fighter_rank, GSW(507), LW(1))
    IF_INT_LESS(LW(0), LW(1))
        CALL(evt_msg_print_add_insert, 0, STRING("stg3_rank_03"), LW(0))
    ELSE
        CALL(evt_msg_print_add_insert, 0, STRING("stg3_rank_02"), LW(0))
    END_IF
    RETURN
EVT_END()

// evt_tou_set_ranking_win_lose: d must start at 0 and accumulate both rand()%4 terms

EVT_BEGIN(evt_tou_match_after_default)
    SET_VALUE(LF(0), 0)
    IF_INT_EQUAL(GSW(501), 10)
        CALL(chk, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            SET_VALUE(LF(0), 1)
            CALL(evt_party_set_pos, 1, 0, -1000, 0)
            CALL(evt_party_stop, 1)
            CALL(evt_party_cont_onoff, 1, 0)
        END_IF
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 137)
        IF_INT_LESS(GSW(0), 138)
            IF_INT_EQUAL(LF(0), 0)
                CALL(chk, LW(0))
                IF_INT_EQUAL(LW(0), 0)
                    CALL_CHILD(evt_tamago_hello)
                END_IF
            END_IF
        END_IF
    END_IF
    IF_INT_EQUAL(GSWF(2448), 1)
        IF_INT_EQUAL(GSWF(2451), 0)
            IF_INT_EQUAL(GSWF(2450), 0)
                CALL(evt_npc_flag_onoff, 0, STRING("ウラノコ"), 1536)
                CALL(evt_npc_set_anim, STRING("ウラノコ"), STRING("NKT_K_2"))
            ELSE
                WAIT_MS(100)
            END_IF
        END_IF
    END_IF
    CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
    IF_INT_EQUAL(LW(0), 11)
        IF_INT_EQUAL(GSWF(2413), 0)
            CALL(evt_npc_check, STRING("ガンガン"), LW(0))
            IF_INT_EQUAL(LW(0), 1)
                CALL(evt_npc_set_position, STRING("ガンガン"), 95, 0, 50)
            END_IF
        END_IF
    END_IF
    SET_VALUE(LF(0), 0)
    SET_VALUE(LF(1), 0)
    SET_VALUE(LF(2), 0)
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("tou_10"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("tou_08"))
        SET_VALUE(LF(1), 1)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("tou_07"))
        SET_VALUE(LF(2), 1)
    END_IF
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("キノシコワ"), 32)
    IF_INT_EQUAL(LF(2), 1)
        CALL(evt_mario_set_pos, 65, 0, -95)
        CALL(evt_party_set_pos, 0, 95, 0, -105)
        CALL(evt_npc_set_position, STRING("キノシコワ"), 30, 0, -95)
        CALL(evt_cam3d_evt_set, 45, 91, 266, 45, 57, -11, 0, 11)
        CALL(evt_set_dir_to_target, STRING("キノシコワ"), STRING("mario"))
        CALL(evt_mario_set_dir, 270, 0, 0)
        CALL(evt_party_set_dir, 0, 270, 0)
    ELSE
        CALL(evt_mario_set_pos, -90, 0, -95)
        CALL(evt_party_set_pos, 0, -120, 0, -105)
        CALL(evt_npc_set_position, STRING("キノシコワ"), -55, 0, -95)
        CALL(evt_cam3d_evt_set, -75, 91, 266, -75, 57, -11, 0, 11)
        CALL(evt_set_dir_to_target, STRING("キノシコワ"), STRING("mario"))
        CALL(evt_mario_set_dir, 90, 0, 0)
        CALL(evt_party_set_dir, 0, 90, 0)
    END_IF
    CALL(evt_seq_wait, 2)
    CALL(octo_stop)
    WAIT_MS(1000)
    CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
    IF_INT_EQUAL(GSWF(2442), 1)
        CALL(fight_money, LW(0), 1, LW(10))
    ELSE
        CALL(fight_money, LW(0), 0, LW(10))
    END_IF
    SET_VALUE(LF(10), 0)
    SET_VALUE(LF(11), 0)
    SET_VALUE(LF(12), 0)
    SET_VALUE(LF(13), 0)
    IF_INT_EQUAL(LF(0), 1)
        IF_INT_LESS(GSW(506), 11)
            CALL(evt_msg_print, 0, STRING("stg3_tou_114_00"), 0, STRING("キノシコワ"))
            WAIT_MS(300)
            CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("A_1"))
            WAIT_MS(300)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            ADD(LW(0), 20)
            ADD(LW(1), 20)
            CALL(evt_item_entry, STRING("fmoney"), 121, LW(0), LW(1), LW(2), 17, -1, 0)
            CALL(evt_mario_set_pose, STRING("M_C_3"))
            WAIT_MS(1000)
            CALL(evt_item_delete, STRING("fmoney"))
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            CALL(evt_msg_print_insert, 0, STRING("stg3_tou_114_01"), 0, STRING("キノシコワ"), LW(10))
            CALL_CHILD(evt_tou_ranking_msg)
            CALL(evt_pouch_add_coin, LW(10))
            WAIT_MS(300)
            CALL(evt_msg_print, 0, STRING("stg3_tou_114_02"), 0, STRING("キノシコワ"))
            WAIT_MS(300)
            CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("M_1"))
            WAIT_MS(600)
            CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("S_1"))
            SET_VALUE(LF(10), 1)
        ELSE
            CALL(evt_msg_print, 0, STRING("stg3_tou_179_00"), 0, STRING("キノシコワ"))
            WAIT_MS(300)
            CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("A_1"))
            WAIT_MS(300)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            ADD(LW(0), 20)
            ADD(LW(1), 20)
            CALL(evt_item_entry, STRING("fmoney"), 121, LW(0), LW(1), LW(2), 17, -1, 0)
            CALL(evt_mario_set_pose, STRING("M_C_3"))
            WAIT_MS(1000)
            CALL(evt_item_delete, STRING("fmoney"))
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            CALL(evt_msg_print_insert, 0, STRING("stg3_tou_179_01"), 0, STRING("キノシコワ"), LW(10))
            CALL_CHILD(evt_tou_ranking_msg)
            CALL(evt_pouch_add_coin, LW(10))
            WAIT_MS(300)
            CALL(evt_msg_print, 0, STRING("stg3_tou_179_02"), 0, STRING("キノシコワ"))
            WAIT_MS(300)
            CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("M_1"))
            WAIT_MS(600)
            CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("S_1"))
        END_IF
    END_IF
    IF_INT_EQUAL(LF(1), 1)
        SWITCH(GSW(506))
            CASE_LESS(1)
            CALL(evt_msg_print, 0, STRING("stg3_tou_365_00"), 0, STRING("キノシコワ"))
            WAIT_MS(300)
            CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("A_1"))
            WAIT_MS(300)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            ADD(LW(0), 20)
            ADD(LW(1), 20)
            CALL(evt_item_entry, STRING("fmoney"), 121, LW(0), LW(1), LW(2), 17, -1, 0)
            CALL(evt_mario_set_pose, STRING("M_C_3"))
            WAIT_MS(1000)
            CALL(evt_item_delete, STRING("fmoney"))
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            CALL(evt_msg_print_insert, 0, STRING("stg3_tou_365_01"), 0, STRING("キノシコワ"), LW(10))
            CALL_CHILD(evt_tou_ranking_msg)
            CALL(evt_pouch_add_coin, LW(10))
            WAIT_MS(300)
            CALL(evt_msg_print, 0, STRING("stg3_tou_365_02"), 0, STRING("キノシコワ"))
            WAIT_MS(300)
            CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("M_1"))
            WAIT_MS(600)
            CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("S_1"))
            SET_VALUE(LF(11), 1)
            SET_VALUE(GSWF(2505), 0)
            CASE_GREATER(10)
            CALL(evt_msg_print, 0, STRING("stg3_tou_365_03_00"), 0, STRING("キノシコワ"))
            WAIT_MS(300)
            CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("A_1"))
            WAIT_MS(300)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            ADD(LW(0), 20)
            ADD(LW(1), 20)
            CALL(evt_item_entry, STRING("fmoney"), 121, LW(0), LW(1), LW(2), 17, -1, 0)
            CALL(evt_mario_set_pose, STRING("M_C_3"))
            WAIT_MS(1000)
            CALL(evt_item_delete, STRING("fmoney"))
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            CALL(evt_msg_print_insert, 0, STRING("stg3_tou_365_03_01"), 0, STRING("キノシコワ"), LW(10))
            CALL_CHILD(evt_tou_ranking_msg)
            CALL(evt_pouch_add_coin, LW(10))
            WAIT_MS(300)
            CALL(evt_msg_print, 0, STRING("stg3_tou_365_03_02"), 0, STRING("キノシコワ"))
            WAIT_MS(300)
            CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("M_1"))
            WAIT_MS(600)
            CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("S_1"))
            SET_VALUE(LF(12), 1)
            CASE_DEFAULT
            CALL(evt_msg_print, 0, STRING("stg3_tou_180_00"), 0, STRING("キノシコワ"))
            WAIT_MS(300)
            CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("A_1"))
            WAIT_MS(300)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            ADD(LW(0), 20)
            ADD(LW(1), 20)
            CALL(evt_item_entry, STRING("fmoney"), 121, LW(0), LW(1), LW(2), 17, -1, 0)
            CALL(evt_mario_set_pose, STRING("M_C_3"))
            WAIT_MS(1000)
            CALL(evt_item_delete, STRING("fmoney"))
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            CALL(evt_msg_print_insert, 0, STRING("stg3_tou_180_01"), 0, STRING("キノシコワ"), LW(10))
            CALL_CHILD(evt_tou_ranking_msg)
            CALL(evt_pouch_add_coin, LW(10))
            WAIT_MS(300)
            CALL(evt_msg_print, 0, STRING("stg3_tou_180_02"), 0, STRING("キノシコワ"))
            WAIT_MS(300)
            CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("M_1"))
            WAIT_MS(600)
            CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("S_1"))
        END_SWITCH
    END_IF
    IF_INT_EQUAL(LF(2), 1)
        IF_INT_GREATER(GSW(506), 0)
            CALL(evt_msg_print, 0, STRING("stg3_tou_365_04_00"), 0, STRING("キノシコワ"))
            WAIT_MS(300)
            CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("A_1"))
            WAIT_MS(300)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            SUBTRACT(LW(0), 20)
            ADD(LW(1), 20)
            CALL(evt_item_entry, STRING("fmoney"), 121, LW(0), LW(1), LW(2), 17, -1, 0)
            CALL(evt_mario_set_pose, STRING("M_C_3"))
            WAIT_MS(1000)
            CALL(evt_item_delete, STRING("fmoney"))
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            CALL(evt_msg_print_insert, 0, STRING("stg3_tou_365_04_01"), 0, STRING("キノシコワ"), LW(10))
            CALL_CHILD(evt_tou_ranking_msg)
            CALL(evt_pouch_add_coin, LW(10))
            WAIT_MS(300)
            CALL(evt_msg_print, 0, STRING("stg3_tou_365_04_02"), 0, STRING("キノシコワ"))
            WAIT_MS(300)
            CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("M_1"))
            WAIT_MS(600)
            CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("S_1"))
            SET_VALUE(LF(13), 1)
        ELSE
            CALL(evt_msg_print, 0, STRING("stg3_tou_181_00"), 0, STRING("キノシコワ"))
            WAIT_MS(300)
            CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("A_1"))
            WAIT_MS(300)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            SUBTRACT(LW(0), 20)
            ADD(LW(1), 20)
            CALL(evt_item_entry, STRING("fmoney"), 121, LW(0), LW(1), LW(2), 17, -1, 0)
            CALL(evt_mario_set_pose, STRING("M_C_3"))
            WAIT_MS(1000)
            CALL(evt_item_delete, STRING("fmoney"))
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            CALL(evt_msg_print_insert, 0, STRING("stg3_tou_181_01"), 0, STRING("キノシコワ"), LW(10))
            CALL_CHILD(evt_tou_ranking_msg)
            CALL(evt_pouch_add_coin, LW(10))
            WAIT_MS(300)
            CALL(evt_msg_print, 0, STRING("stg3_tou_181_02"), 0, STRING("キノシコワ"))
            WAIT_MS(300)
            CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("M_1"))
            WAIT_MS(600)
            CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("S_1"))
        END_IF
    END_IF
    CALL(evt_npc_status_onoff, 1, STRING("キノシコワ"), 2)
    CALL(evt_npc_move_position, STRING("キノシコワ"), 0, -130, 0, FLOAT(80.0f), 1)
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, 180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
        IF_INT_EQUAL(LF(0), 1)
            CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        END_IF
        IF_INT_EQUAL(LF(1), 1)
            CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        END_IF
        IF_INT_EQUAL(LF(2), 1)
            CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        END_IF
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("キノシコワ"), 0, -200, 0, FLOAT(80.0f), 1)
        CALL(evt_npc_set_position, STRING("キノシコワ"), 0, -1000, 0)
    END_INLINE_EVENT
    WAIT_MS(800)
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_close_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 180, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
        IF_INT_EQUAL(LF(0), 1)
            CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        END_IF
        IF_INT_EQUAL(LF(1), 1)
            CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        END_IF
        IF_INT_EQUAL(LF(2), 1)
            CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        END_IF
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    IF_INT_EQUAL(GSWF(2448), 1)
        IF_INT_EQUAL(GSWF(2451), 0)
            IF_INT_EQUAL(GSWF(2450), 1)
                CALL(evt_mario_set_dir, 270, 300, 0)
                CALL(evt_party_stop, 0)
                WAIT_MS(300)
                IF_INT_EQUAL(GSWF(2442), 1)
                    CALL(evt_msg_print_party, STRING("stg3_tou_306"))
                ELSE
                    CALL(evt_msg_print_party, STRING("stg3_tou_307"))
                END_IF
                CALL(evt_pouch_party_join, 1)
                CALL(evt_pouch_party_join, 2)
                CALL(evt_pouch_party_join, 5)
                CALL(evt_pouch_party_join, 4)
                CALL(evt_party_run, 0)
            ELSE
                CALL(evt_npc_flag_onoff, 0, STRING("ウラノコ"), 1536)
                CALL(evt_npc_set_anim, STRING("ウラノコ"), STRING("NKT_K_2"))
                CALL(evt_mario_set_dir, 270, 300, 0)
                CALL(evt_party_stop, 0)
                WAIT_MS(300)
                CALL(evt_msg_print_party, STRING("stg3_tou_307_01"))
                CALL(evt_npc_get_position, STRING("ウラノコ"), LW(0), LW(1), LW(2))
                CALL(evt_cam3d_evt_set, LW(0), 90, 283, LW(0), 45, -9, 1000, 11)
                BEGIN_INLINE_EVENT
                    CALL(evt_mario_set_dir_npc, STRING("ウラノコ"))
                END_INLINE_EVENT
                BEGIN_INLINE_EVENT
                    CALL(evt_party_set_dir_npc, 0, STRING("ウラノコ"))
                END_INLINE_EVENT
                WAIT_MS(1000)
                CALL(evt_msg_print, 0, STRING("stg3_tou_307_02"), 0, STRING("ウラノコ"))
                CALL(evt_npc_change_interrupt, STRING("ウラノコ"), 6, PTR(evt_talk_doku_cake))
                CALL(evt_cam3d_evt_set, -75, 91, 266, -75, 57, -11, 500, 11)
                CALL(evt_mario_set_dir, 270, 300, 0)
                CALL(evt_party_set_dir, 0, 90, 300)
                WAIT_MS(500)
                CALL(evt_msg_print_party, STRING("stg3_tou_307_03"))
                CALL(evt_party_run, 0)
            END_IF
            SET_VALUE(GSWF(2451), 1)
        END_IF
    END_IF
    IF_INT_EQUAL(LF(10), 1)
        CALL(evt_mario_set_dir, 270, 300, 0)
        CALL(evt_msg_print_party, STRING("stg3_tou_114"))
        IF_INT_GREATER_OR_EQUAL(GSW(0), 172)
            SET_VALUE(GSWF(2505), 1)
        END_IF
    END_IF
    IF_INT_EQUAL(LF(11), 1)
        CALL(evt_mario_set_dir, 270, 300, 0)
        CALL(evt_msg_print_party, STRING("stg3_tou_365"))
        CALL(evt_pouch_remove_item, ITEM_CHAMPS_BELT, LW(0))
    END_IF
    IF_INT_EQUAL(LF(12), 1)
        CALL(evt_mario_set_dir, 270, 300, 0)
        CALL(evt_msg_print_party, STRING("stg3_tou_365_03_03"))
    END_IF
    IF_INT_EQUAL(LF(13), 1)
        CALL(evt_mario_set_dir, 270, 300, 0)
        CALL(evt_msg_print_party, STRING("stg3_tou_365_04_03"))
        CALL(evt_pouch_add_item, ITEM_CHAMPS_BELT, LW(0))
    END_IF
    SET_VALUE(LF(0), 0)
    IF_INT_EQUAL(GSW(501), 10)
        CALL(chk, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            IF_INT_GREATER_OR_EQUAL(GSW(0), 137)
                CALL_CHILD(evt_tou_chibi_yoshi)
            ELSE
                CALL_CHILD(evt_michel_9)
            END_IF
            SET_VALUE(LF(0), 1)
        END_IF
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 137)
        IF_INT_EQUAL(LF(0), 0)
            CALL(chk, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL_CHILD(evt_tamago_hello2)
            END_IF
        END_IF
    END_IF
    CALL_CHILD(evt_cake_init)
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(18)
        IF_INT_EQUAL(GSWF(2406), 0)
            SET_VALUE(GSWF(2406), 1)
            CALL_CHILD(evt_michel_2)
        END_IF
        CASE_EQUAL(17)
        IF_INT_EQUAL(GSWF(2407), 0)
            SET_VALUE(GSWF(2407), 1)
            CALL_CHILD(evt_michel_3)
        END_IF
        CASE_EQUAL(16)
        IF_INT_EQUAL(GSWF(2408), 0)
            SET_VALUE(GSWF(2408), 1)
            CALL_CHILD(evt_michel_4)
        END_IF
        CASE_EQUAL(15)
        IF_INT_EQUAL(GSWF(2409), 0)
            SET_VALUE(GSWF(2409), 1)
            CALL_CHILD(evt_michel_5)
        END_IF
        CASE_EQUAL(14)
        IF_INT_EQUAL(GSWF(2410), 0)
            SET_VALUE(GSWF(2410), 1)
            CALL_CHILD(evt_michel_6)
        END_IF
        CASE_EQUAL(13)
        IF_INT_EQUAL(GSWF(2411), 0)
            SET_VALUE(GSWF(2411), 1)
            CALL_CHILD(evt_michel_7)
        END_IF
        CASE_EQUAL(12)
        IF_INT_EQUAL(GSWF(2412), 0)
            SET_VALUE(GSWF(2412), 1)
            CALL(evt_npc_set_position, STRING("マイケル"), 0, -1000, 0)
            CALL(evt_rankingReInit)
        END_IF
        CASE_EQUAL(11)
        IF_INT_EQUAL(GSWF(2413), 0)
            SET_VALUE(GSWF(2413), 1)
            CALL_CHILD(evt_michel_8)
            CALL(evt_rankingReInit)
        END_IF
        CASE_EQUAL(10)
        CASE_EQUAL(9)
        CASE_EQUAL(8)
        IF_INT_EQUAL(GSWF(2416), 0)
            SET_VALUE(GSWF(2416), 1)
            CALL_CHILD(evt_mail_1)
        END_IF
        CASE_EQUAL(7)
        IF_INT_EQUAL(GSWF(2417), 0)
            SET_VALUE(GSWF(2417), 1)
            CALL_CHILD(evt_mail_2)
        END_IF
        CASE_EQUAL(6)
        IF_INT_EQUAL(GSWF(2418), 0)
            SET_VALUE(GSWF(2418), 1)
            CALL_CHILD(evt_cake)
        END_IF
        CASE_EQUAL(5)
        IF_INT_EQUAL(GSWF(2419), 0)
            SET_VALUE(GSWF(2419), 1)
            CALL_CHILD(evt_koopa_after)
        END_IF
        IF_INT_EQUAL(GSWF(2447), 1)
            CALL_CHILD(evt_tou_08_cake)
        END_IF
        CASE_EQUAL(4)
        IF_INT_EQUAL(GSWF(2420), 0)
            SET_VALUE(GSWF(2420), 1)
            CALL_CHILD(evt_mail_3)
        END_IF
        CASE_EQUAL(3)
        IF_INT_EQUAL(GSWF(2421), 0)
            SET_VALUE(GSWF(2421), 1)
            CALL_CHILD(evt_mail_4)
            CALL(evt_rankingReInit)
        END_IF
        CASE_EQUAL(2)
        CASE_EQUAL(1)
        IF_INT_EQUAL(GSWF(2423), 0)
            SET_VALUE(GSWF(2423), 1)
            CALL_CHILD(evt_mail_5)
        END_IF
    END_SWITCH
    CALL(octo_start)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

// TODO: type
static s32 champion_belt_onoff[1];

// TODO: type
u32 gans_wp[1];

// TODO: type
static u32 rank_work[2];

// TODO: type
static u32 disp_work[3];

// clang-format on

// Retail builds the GXColor in one scratch slot and copies it to the argument slot; returning it
// from an inline reproduces that instead of writing the channels in place.
static inline GXColor monitorColor(u8 alpha) {
    GXColor c = *(GXColor*)&dat_tou_00009c38;
    c.a = alpha;
    return c;
}

// Emit the real functions in retail order so native conversion constants share the original pool slots.
#ifdef MUST_MATCH
#pragma defer_codegen off
#endif

static USER_FUNC(tou_champion_belt_onoff) {
    champion_belt_onoff[0] = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(tou_champion_belt_27_text_34) {
    NpcEntry* npc;
    AnimationPose* pose;
    Vec* loc;
    s32 group;
    CameraEntry* cam;
    EffectEntry* handle;
    f32 angle;
    void** ud;

    npc = npcNameToPtr((const char*)evtGetValue(event, event->args[0]));

    if (isFirstCall) {
        f32 zero = 0.0f;

        handle = effStarStoneEntry(9, zero, zero, zero, 0.6f);
        animPoseWorldPositionEvalOn(npc->poseId);
        event->userdata[0] = (s32)handle;
        champion_belt_onoff[0] = 0;
    }
    pose = animPoseGetAnimPosePtr(npc->poseId);
    loc = ((PoseUser*)pose->groupWorldPos)->locators;
    group = animPoseGetGroupIdx(npc->poseId, "locator8");
    cam = camGetPtr(CAMERA_3D);
    angle = reviseAngle((f32)atan2(-(cam->cameraAt.x - cam->cameraPos.x), -(cam->cameraAt.z - cam->cameraPos.z)) *
                        57.29579162597656f);
    if (angle < 0.0f) {
        angle += 360.0f;
    }
    angle = reviseAngle(angle - pose->rotationY);
    handle = (EffectEntry*)event->userdata[0];
    ud = &handle->userdata;
    ((effStarStoneData*)*ud)->x = loc[group].x;
    ((effStarStoneData*)*ud)->y = loc[group].y;
    ((effStarStoneData*)*ud)->z = loc[group].z;
    if ((angle > 90.0f && angle <= 270.0f) ||
        (angle < -90.0f && angle >= -270.0f)) {
        ((effStarStoneData*)*ud)->rotY = 8.0f - (angle + camGetPtr(CAMERA_3D)->viewYaw);
    } else {
        cam = camGetPtr(CAMERA_3D);
        ((effStarStoneData*)*ud)->rotY = -(angle + cam->viewYaw) - 8.0f;
    }
    if (npc->position.y <= -1000.0f) {
        ((effStarStoneData*)*ud)->x = 0.0f;
        ((effStarStoneData*)*ud)->y = -1000.0f;
        ((effStarStoneData*)*ud)->z = 0.0f;
    }
    if (champion_belt_onoff[0] != 0) {
        ((effStarStoneData*)*ud)->x = 0.0f;
        ((effStarStoneData*)*ud)->y = -1000.0f;
        ((effStarStoneData*)*ud)->z = 0.0f;
    }
    return EVT_RETURN_BLOCK;
}

static f32 camGetRy(void) {
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    f32 ry = reviseAngle((f32)atan2(-(cam->cameraAt.x - cam->cameraPos.x), -(cam->cameraAt.z - cam->cameraPos.z)) *
                         57.29579162597656f);

    if (ry < 0.0f) {
        ry += 360.0f;
    }
    return ry;
}

const f32 float_0p99_tou_0000a860 = 0.99f;
const f32 float_2_tou_0000a864 = 2.0f;
const f32 float_0p5_tou_0000a868 = 0.5f;
const f32 float_5_tou_0000a86c = 5.0f;

USER_FUNC(fight_money) {
    s32* args = event->args;
    s32 a = evtGetValue(event, args[0]);
    s32 v;

    if (evtGetValue(event, args[1])) {
        if (a > 10) {
            v = (s32)(TOU_F32(float_0p99_tou_0000a860) + ((f32)(0x15 - a) * TOU_F32(float_0p5_tou_0000a868) + TOU_F32(float_2_tou_0000a864)));
        } else if (a > 0) {
            v = (s32)(TOU_F32(float_0p99_tou_0000a860) + ((f32)(0x15 - a) * TOU_F32(float_0p5_tou_0000a868) + TOU_F32(float_5_tou_0000a86c)));
        } else {
            v = 0x14;
        }
    } else {
        v = 1;
    }
    evtSetValue(event, args[2], v);
    return EVT_RETURN_DONE;
}

USER_FUNC(octo_start) {
    NpcEntry* npc = npcNameToPtr_NoAssert("オクトール");

    if (npc != NULL) {
        evtStartID(npc->regularEvtId);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(octo_stop) {
    NpcEntry* npc = npcNameToPtr_NoAssert("オクトール");

    if (npc != NULL) {
        evtStopID(npc->regularEvtId);
        npc->prevPosition = vec3_tou_0000a2b4;
        npc->position = npc->prevPosition;
        npc->direction = 270.0f;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(unk_tou_000005d0) {
    s32* args = event->args;
    if (marioKeyOffChk() || marioCtrlOffChk()) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(unk_tou_0000063c) {
    MarioWork* mario = marioGetPtr();

    if (isFirstCall) {
        event->userdata[0] = 0;
    }
    if (!marioKeyOffChk() && !marioCtrlOffChk() &&
        (mario->currMotionId == 0 || mario->currMotionId == 1 || mario->currMotionId == 2)) {
        event->userdata[0]++;
        if (event->userdata[0] > 0xf0) {
            event->userdata[0] = 0xf0;
            return EVT_RETURN_DONE;
        }
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(chk) {
    s32* args = event->args;
    evtSetValue(event, args[0], partyChkJoin(PARTNER_YOSHI) != 0);
    return EVT_RETURN_DONE;
}

static USER_FUNC(getYoshiColor) {
    s32* args = event->args;
    s32 color = pouchGetPartyColor(PARTNER_YOSHI);
    evtSetValue(event, args[0], color);
    return EVT_RETURN_DONE;
}

USER_FUNC(onoff) {
    ((GansWork*)gans_wp[0])->enabled = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

void monitor_tev(void* param) {
    MapTevCallback* p = param;
    GXColor color;

    if (((GansWork*)gans_wp[0])->texReady != 0) {
        GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
        GXLoadTexObj(smartTexObj(&((GansWork*)gans_wp[0])->texObj, (void**)((GansWork*)gans_wp[0])->texData), 1);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
        GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
        GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_TEXC, GX_CC_CPREV, GX_CC_A0, GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
        GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
        color = monitorColor(((GansWork*)gans_wp[0])->alpha);
        GXSetTevColor(GX_TEVREG0, color);
        p->tevStage = 2;
        p->texCoord = 2;
        p->texMap = 2;
    }
}

USER_FUNC(make_monitor) {
    NpcEntry* npc = npcNameToPtr_NoAssert(
        evtGetValue(NULL, GSW(0)) < 0xac ? "ガンス" : "キノシコワ");
    GansWork* work;

    if (isFirstCall) {
        work = (GansWork*)_mapAlloc(sizeof(GansWork));
        event->userdata[0] = (s32)work;
        work->enabled = 0;
        work->texReady = 0;
    }
    if (npc == NULL) {
        return EVT_RETURN_BLOCK;
    }
    work = (GansWork*)event->userdata[0];
    gans_wp[0] = (u32)work;
    work->texData = smartAlloc(GXGetTexBufferSize(0x80, 0x80, GX_TF_RGB5A3, 0, 0), 3);
    GXInitTexObj(&work->texObj, work->texData->address, 0x80, 0x80, GX_TF_RGB5A3, GX_CLAMP, GX_CLAMP, 0);
    GXInitTexObjLOD(&work->texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    if (((GansWork*)gans_wp[0])->enabled) {
        ((GansWork*)gans_wp[0])->alpha += 4;
        if (((GansWork*)gans_wp[0])->alpha > 0x80) {
            ((GansWork*)gans_wp[0])->alpha = 0x80;
        }
    } else {
        ((GansWork*)gans_wp[0])->alpha -= 4;
        if (((GansWork*)gans_wp[0])->alpha < 0) {
            ((GansWork*)gans_wp[0])->alpha = 0;
        }
    }
    dispEntry(CAMERA_SHADOW, 0.0f, 3, gans_tex, work);
    return EVT_RETURN_BLOCK;
}

void gans_tex(CameraId cameraId, void* param) {
    GansWork* work = (GansWork*)param;
    NpcEntry* npc;
    CameraEntry cam;
    f32 viewport[6];
    f32 proj[7];
    u32 x, y, wd, ht;
    Vec camPos;
    Vec camUp;
    Vec camTarget;
    Mtx44 persp;
    Mtx lookAt;
    Mtx mtx;

    GXGetViewportv(viewport);
    GXGetProjectionv(proj);
    GXGetScissor(&x, &y, &wd, &ht);
    {
        GXSetViewport(0.0f, 0.0f, 128.0f, 128.0f,
                      0.0f, 1.0f);
    }
    GXSetScissor(1, 1, 0x7e, 0x7e);
    C_MTXPerspective(persp, 25.0f, 1.0f, 1.0f,
                     1000.0f);
    GXSetProjection(persp, 0);
    camTarget = vec3_tou_00009c2c;
    camUp = vec3_tou_00009c20;
    camPos = vec3_tou_00009c14;
    C_MTXLookAt(lookAt, &camPos, &camUp, &camTarget);
    cam = *camGetPtr(cameraId);
    npc = npcNameToPtr(evtGetValue(NULL, GSW(0)) < 0xac ? "ガンス" : "キノシコワ");
    PSMTXCopy(lookAt, camGetCurPtr()->view);
    animPosePeraOff(npc->poseId);
    PSMTXIdentity(mtx);
    animPoseMain(npc->poseId);
    animPoseDrawMtx(npc->poseId, mtx, 1, 0.0f, TOU_F32(float_2_tou_0000a864));
    animPoseDrawMtx(npc->poseId, mtx, 2, 0.0f, TOU_F32(float_2_tou_0000a864));
    animPoseDrawMtx(npc->poseId, mtx, 3, 0.0f, TOU_F32(float_2_tou_0000a864));
    animPosePeraOn(npc->poseId);
    PSMTXCopy(cam.view, camGetCurPtr()->view);
    GXSetTexCopySrc(0, 0, 0x80, 0x80);
    GXSetTexCopyDst(0x80, 0x80, GX_TF_RGB5A3, GX_FALSE);
    GXCopyTex(work->texData->address, GX_TRUE);
    GXPixModeSync();
    GXSetViewport(viewport[0], viewport[1], viewport[2], viewport[3], viewport[4], viewport[5]);
    GXSetProjectionv(proj);
    GXSetScissor(x, y, wd, ht);
    work->texReady = 1;
}

const f32 float_0p125_tou_0000a888 = 0.125f;
const f32 float_300_tou_0000a88c = 300.0f;

static USER_FUNC(evt_tou_print_ranking) {
    s32 i;
    s32 c;

    if (isFirstCall) {
        memset((void*)dwp[0], 0, sizeof(DispWork));
        c = 0;
        for (i = 0; i < ((RankWork*)rank_wp[0])->count; i++) {
            u16 f = ((RankWork*)rank_wp[0])->entries[i].flags;

            if (!(f & 2)) {
                if (f & 4) {
                    break;
                }
                c++;
            }
        }
        ((DispWork*)dwp[0])->page = c / 0xb;
        {
            f32 v = (f32)(((DispWork*)dwp[0])->page * 0xdc);

            ((DispWork*)dwp[0])->targetScroll = v;
            ((DispWork*)dwp[0])->scroll = v;
        }
        evtGetValue(NULL, GSW(0));
        for (i = 0; i < ((RankWork*)rank_wp[0])->count; i++) {
        }
        return EVT_RETURN_BLOCK;
    }
    if (keyGetButtonTrg(0) & 0x200) {
        psndSFXOn((const char*)0x13);
        return EVT_RETURN_YIELD;
    }
    if (((DispWork*)dwp[0])->page > 0 && (keyGetButtonTrg(0) & 0x40)) {
        psndSFXOn((const char*)5);
        ((DispWork*)dwp[0])->page--;
    }
    if (((DispWork*)dwp[0])->page < 1 && (keyGetButtonTrg(0) & 0x20)) {
        ((DispWork*)dwp[0])->page++;
        psndSFXOn((const char*)5);
    }
    ((DispWork*)dwp[0])->targetScroll = (f32)(((DispWork*)dwp[0])->page * 0xdc);
    {
        DispWork* w = (DispWork*)dwp[0];
        // Reading scroll through dwp[0] again instead of w keeps MWCC from reordering
        // the two field loads (the natural w->scroll form lands scroll in f3 before f2's targetScroll)
        f32 t = w->targetScroll - ((DispWork*)dwp[0])->scroll;

        w->scroll = t * TOU_F32(float_0p125_tou_0000a888) + w->scroll;
    }
    dispEntry(CAMERA_2D, TOU_F32(float_300_tou_0000a88c), 0, disp_proc, NULL);
    return EVT_RETURN_BLOCK;
}

const f32 float_250_tou_0000a890 = 250.0f;
const f32 float_30_tou_0000a894 = 30.0f;
const f32 float_165_tou_0000a898 = 165.0f;
const f32 float_310_tou_0000a89c = 310.0f;
const f32 float_330_tou_0000a8a0 = 330.0f;
static const char str_00001F74[12] = "ランキング";
const f32 float_135_tou_0000a8b0 = 135.0f;
const f32 float_304_tou_0000a8b4 = 304.0f;
const f32 float_145_tou_0000a8b8 = 145.0f;
const f32 float_220_tou_0000a8bc = 220.0f;
const f32 float_10_tou_0000a8c0 = 10.0f;
const f32 float_40_tou_0000a8c4 = 40.0f;
const f32 float_0p8_tou_0000a8c8 = 0.8f;
const f32 zero_tou_0000a8cc = 0.0f;
static const char str_fmt_ss[8] = "%s%s";
const f32 float_137_tou_0000a8d8 = 137.0f;
const f32 float_228_tou_0000a8dc = 228.0f;
const f32 float_95_tou_0000a8e0 = 95.0f;
const f32 float_15_tou_0000a8e4 = 15.0f;
const f32 float_24_tou_0000a8e8 = 24.0f;
const f32 float_neg1_tou_0000a8ec = -1.0f;

static void disp_proc(CameraId cameraId, void* param) {
    FighterEntry* dt;
    f32 f31;
    u16 scW;
    u16 scX;
    s32 w;
    s32 i;
    s32 rank;
    s32 y;
    s32 off;
    f32 f27;
    f32 f26;
    Mtx scale;
    Mtx mtx;
    char buf[0x100];
    f32 c250;

    if (evtGetValue(NULL, GSW(0)) < 0xac) {
        dt = (FighterEntry*)fighterDt;
    } else {
        dt = (FighterEntry*)fighterDt_re;
    }
    {
        f32 t = TOU_F32(float_250_tou_0000a890);
        f32 h = TOU_F32(float_0p5_tou_0000a868);

        f31 = -t * h;
    }
    windowDispGX_System(0, 0xff, f31 - TOU_F32(float_30_tou_0000a894), TOU_F32(float_165_tou_0000a898),
                        TOU_F32(float_310_tou_0000a89c), TOU_F32(float_330_tou_0000a8a0));
    FontDrawStart();
    FontDrawScale(1.0f);
    w = FontGetMessageWidth(str_00001F74);
    {
        GXColor color = *(GXColor*)&dat_tou_00009904;

        FontDrawColor(&color);
    }
    FontDrawMessage(-w / 2, (s32)TOU_F32(float_135_tou_0000a8b0), str_00001F74);
    c250 = TOU_F32(float_250_tou_0000a890);
    scX = (u16)(TOU_F32(float_304_tou_0000a8b4) + f31);
    scW = (u16)c250;
    GXSetScissor(scX, (u16)TOU_F32(float_145_tou_0000a8b8), scW, (u16)TOU_F32(float_220_tou_0000a8bc));
    f27 = TOU_F32(float_10_tou_0000a8c0) + f31;
    rank = 0;
    y = 0;
    i = 0;
    off = 0;
    for (; i < ((RankWork*)rank_wp[0])->count; i++, off += 0xc) {
        f26 = ((DispWork*)dwp[0])->scroll + ((TOU_F32(float_135_tou_0000a8b0) - (f32)y) - TOU_F32(float_40_tou_0000a8c4));
        if (!(((RankEntry*)((u8*)((RankWork*)rank_wp[0])->entries + off))->flags & 2)) {
            PSMTXScale(scale, TOU_F32(float_0p8_tou_0000a8c8), TOU_F32(float_0p8_tou_0000a8c8), TOU_F32(float_0p8_tou_0000a8c8));
            if (rank != 0) {
                const char* p1 = (rank / 10) ? suuji[rank / 10] : (const char*)&(*(f32*)&TOU_F32(zero_tou_0000a8cc));

                sprintf(buf, str_fmt_ss, p1, suuji[rank % 10]);
                FontDrawStart();
                {
                    GXColor color = *(GXColor*)&dat_tou_00009908;

                    FontDrawColor(&color);
                }
                PSMTXTrans(mtx, f27, f26, 0.0f);
                PSMTXConcat(mtx, scale, mtx);
                FontDrawMessageMtx(mtx, buf);
            }
            FontDrawStart();
            if (((RankEntry*)((u8*)((RankWork*)rank_wp[0])->entries + off))->flags & 4) {
                GXColor color = *(GXColor*)&dat_tou_0000990c;

                FontDrawColor(&color);
            } else {
                GXColor color = *(GXColor*)&dat_tou_00009910;

                FontDrawColor(&color);
            }
            PSMTXTrans(mtx, TOU_F32(float_40_tou_0000a8c4) + f27, f26, 0.0f);
            PSMTXConcat(mtx, scale, mtx);
            FontDrawMessageMtx(mtx, msgSearch((const char*)((u32*)dt)[((RankEntry*)((u8*)((RankWork*)rank_wp[0])->entries + off))->id * 7 + 1]));
            rank++;
            y += 0x14;
        }
    }
    GXSetScissor(scX, (u16)(s32)TOU_F32(float_137_tou_0000a8d8), scW, (u16)(s32)TOU_F32(float_228_tou_0000a8dc));
    {
        f32 yv = TOU_F32(float_95_tou_0000a8e0) + ((DispWork*)dwp[0])->scroll;
        Vec pos = vec3_tou_00009914;

        pos.x = TOU_F32(float_15_tou_0000a8e4) + f31;
        pos.y = yv - TOU_F32(float_24_tou_0000a8e8);
        iconDispGx(TOU_F32(float_0p8_tou_0000a8c8), &pos, 0x10, 0x123);
    }
    GXSetScissor(0, 0, 0x260, 0x1e0);
    if (((DispWork*)dwp[0])->page > 0) {
        Vec pos = vec3_tou_00009920;

        pos.x = f31 + TOU_F32(float_250_tou_0000a890);
        iconDispGx(TOU_F32(float_neg1_tou_0000a8ec), &pos, 0x10, 0x205);
        {
            Vec pos = vec3_tou_0000992c;

            pos.x = f31 + TOU_F32(float_250_tou_0000a890);
            iconDispGx(0.6f, &pos, 0x10, 0x7e);
        }
    }
    if (((DispWork*)dwp[0])->page < 1) {
        Vec pos = vec3_tou_00009938;

        pos.x = f31 + TOU_F32(float_250_tou_0000a890);
        iconDispGx(1.0f, &pos, 0x10, 0x205);
        {
            Vec pos = vec3_tou_00009944;

            pos.x = f31 + TOU_F32(float_250_tou_0000a890);
            iconDispGx(0.6f, &pos, 0x10, 0x80);
        }
    }
}

static void printRanking(void) {
    s32 i;

    evtGetValue(NULL, GSW(0));
    for (i = 0; i < ((RankWork*)rank_wp[0])->count; i++) {
    }
}

static USER_FUNC(evt_tou_set_rule_result) {
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

static USER_FUNC(evt_tou_set_ranking_win_lose) {
    s32* args = event->args;
    s32 a = evtGetValue(event, args[0]);
    s32 id = evtGetValue(event, args[1]);
    s32 i;
    s16 min;
    s16 max;

    if (a == 0) {
        for (i = 0; i < ((RankWork*)rank_wp[0])->count; i++) {
            if (((RankWork*)rank_wp[0])->entries[i].id == id) {
                ((RankWork*)rank_wp[0])->entries[i].flags |= 1;
                break;
            }
        }
        swSet(0x98a);
    } else {
        for (i = 0; i < ((RankWork*)rank_wp[0])->count; i++) {
            if (((RankWork*)rank_wp[0])->entries[i].id == id) {
                ((RankWork*)rank_wp[0])->entries[i].flags &= ~1;
                break;
            }
        }
        swClear(0x98a);
    }
    min = 0x3e8;
    max = -0x3e8;
    for (i = 0; i < ((RankWork*)rank_wp[0])->count; i++) {
        if (!(((RankWork*)rank_wp[0])->entries[i].flags & 2)) {
            s32 d = 0;

            d += rand() % 4;
            d += rand() % 4;
            ((RankWork*)rank_wp[0])->entries[i].score += d - 3;
            if (((RankWork*)rank_wp[0])->entries[i].score < min) {
                min = ((RankWork*)rank_wp[0])->entries[i].score;
            }
            if (((RankWork*)rank_wp[0])->entries[i].score > max) {
                max = ((RankWork*)rank_wp[0])->entries[i].score;
            }
        }
    }
    swByteSet(0x1fb, id);
    swSet(0x9e4);
    rankingControll();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_tou_get_rule_msg2_27_text_182C) {
    s32* args = event->args;
    s32 ruleId = evtGetValue(event, args[0]);
    s32 n;
    RuleEntry* dt;
    s32 i;

    if (evtGetValue(NULL, GSW(0)) < 0xac) {
        RankWork* rw = (RankWork*)rank_wp[0];
        s32 count = 0;

        for (i = 0; i < rw->count; i++) {
            u16 f = rw->entries[i].flags;
            if (!(f & 2)) {
                if (f & 4) {
                    break;
                }
                count++;
            }
        }
        if (count > 10) {
            dt = (RuleEntry*)ruleDt_2bu;
            n = 10;
        } else {
            dt = (RuleEntry*)ruleDt_1bu;
            n = 14;
        }
    } else {
        dt = (RuleEntry*)ruleDt_re;
        n = 14;
    }

    for (i = 0; i < n; i++) {
        if (ruleId == dt[i].kind) {
            break;
        }
    }
    if (i == n) {
        i = n - 1;
    }
    evtSetValue(event, args[1], (s32)msgSearch(dt[i].msg));
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_tou_get_rule_info) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 n;
    RuleEntry* dt;
    s32 i;

    if (evtGetValue(NULL, GSW(0)) < 0xac) {
        RankWork* rw = (RankWork*)rank_wp[0];
        s32 count = 0;

        for (i = 0; i < rw->count; i++) {
            u16 f = rw->entries[i].flags;
            if (!(f & 2)) {
                if (f & 4) {
                    break;
                }
                count++;
            }
        }
        if (count > 10) {
            dt = (RuleEntry*)ruleDt_2bu;
            n = 10;
        } else {
            dt = (RuleEntry*)ruleDt_1bu;
            n = 14;
        }
    } else {
        dt = (RuleEntry*)ruleDt_re;
        n = 14;
    }

    if (type == 0) {
        for (i = 0; i < n; i++) {
            if (dt[i].kind == 6) {
                break;
            }
        }
    } else if (type == 10) {
        for (;;) {
            i = rand() % n;
            if (dt[i].kind != 0x30 && dt[i].kind != 0x20 && dt[i].kind != 0x40) {
                break;
            }
        }
    } else {
        i = rand() % n;
    }

    evtSetValue(event, args[1], (s32)dt[i].name);
    evtSetValue(event, args[2], dt[i].kind);
    evtSetValue(event, args[3], dt[i].param);
    evtSetValue(event, args[4], dt[i].field_10);
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_tou_get_fighter_battle_msg) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    FighterEntry* dt;

    if (evtGetValue(NULL, GSW(0)) < 0xac) {
        dt = (FighterEntry*)fighterDt;
    } else {
        dt = (FighterEntry*)fighterDt_re;
    }
    evtSetValue(event, args[1], (s32)dt[idx].taunt);
    evtSetValue(event, args[2], (s32)dt[idx].victory);
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_tou_get_fighter_rank) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    RankWork* rw = (RankWork*)rank_wp[0];
    s32 rank = 0;
    s32 i;

    for (i = 0; i < rw->count; i++) {
        RankEntry* e = &rw->entries[i];
        if (!(e->flags & 2)) {
            if (id == e->id) {
                break;
            }
            rank++;
        }
    }
    evtSetValue(event, args[1], rank);
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_tou_get_fighter_name2) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    FighterEntry* dt;

    if (evtGetValue(NULL, GSW(0)) < 0xac) {
        dt = (FighterEntry*)fighterDt;
    } else {
        dt = (FighterEntry*)fighterDt_re;
    }
    evtSetValue(event, args[1], (s32)msgSearch(dt[idx].title));
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_tou_get_fighter_info_27_text_1CB8) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    FighterEntry* dt;

    if (evtGetValue(NULL, GSW(0)) < 0xac) {
        dt = (FighterEntry*)fighterDt;
    } else {
        dt = (FighterEntry*)fighterDt_re;
    }
    evtSetValue(event, args[1], (s32)msgSearch(dt[idx].name));
    evtSetValue(event, args[2], ((FighterEntry*)fighterDt)[idx].setupId);
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_tou_get_next_match) {
    s32* args = event->args;
    s32 x = evtGetValue(event, 0xf840650b);
    s32 list[32];
    s32 i;
    s32 cnt = 0;
    s32 c;

    for (i = 0; i < ((RankWork*)rank_wp[0])->count; i++) {
        if (!(((RankWork*)rank_wp[0])->entries[i].flags & 2)) {
            list[cnt++] = ((RankWork*)rank_wp[0])->entries[i].id;
        }
    }
    c = 0;
    for (i = 0; i < ((RankWork*)rank_wp[0])->count; i++) {
        u16 f = ((RankWork*)rank_wp[0])->entries[i].flags;

        if (!(f & 2)) {
            if (f & 4) {
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
    evtGetValue(NULL, GSW(0));
    for (i = 0; i < ((RankWork*)rank_wp[0])->count; i++) {
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_tou_get_ranking_27_text_1ED4) {
    RankWork* rw = (RankWork*)rank_wp[0];
    s32* args = event->args;
    s32 count = 0;
    s32 i;

    for (i = 0; i < rw->count; i++) {
        u16 f = rw->entries[i].flags;
        if (!(f & 2)) {
            if (f & 4) {
                break;
            }
            count++;
        }
    }
    evtSetValue(event, args[0], count);
    return EVT_RETURN_DONE;
}

static void rankingControll(void) {
    s32 above[32];
    s32 below[32];
    s32 bi2;
    s32 bi;
    s32 nAbove;
    s32 nBelow;
    s32 i;
    s32 j;
    s32 k;
    s32 m;
    s32 n;
    s32 p;
    s32 q;
    s32 tmp;
    s32 idx;
    s32 idx2;
    s32 rank;
    RankEntry* entry;
    RankEntry* e;

    nAbove = 0;
    nBelow = 0;
    for (k = 0, e = ((RankWork*)rank_wp[0])->entries; k < ((RankWork*)rank_wp[0])->count; k++, e++) {
        if (!(e->flags & 4)) {
            if (e->flags & 1) {
                below[nBelow] = k;
                nBelow++;
            } else {
                above[nAbove] = k;
                nAbove++;
            }
        }
    }

    if (nAbove > 0) {
        for (i = 0; i < nAbove - 1; i++) {
            for (j = i + 1; j < nAbove; j++) {
                if (((RankWork*)rank_wp[0])->entries[above[i]].score < ((RankWork*)rank_wp[0])->entries[above[j]].score) {
                    tmp = above[i];
                    above[i] = above[j];
                    above[j] = tmp;
                }
            }
        }

        for (m = 0; m < nAbove - 1; m++) {
            for (n = m + 1; n < nAbove; n++) {
                FighterEntry* fj = &((FighterEntry*)fighterDt)[((RankWork*)rank_wp[0])->entries[above[n]].id];
                FighterEntry* fi = &((FighterEntry*)fighterDt)[((RankWork*)rank_wp[0])->entries[above[m]].id];
                if ((fj->flags & 5) && (fi->flags & 2)) {
                    tmp = above[m];
                    above[m] = above[n];
                    above[n] = tmp;
                }
            }
        }

        if (nAbove > 0) {
            for (idx = 0; idx < nAbove; idx++) {
                if (((u16*)fighterDt)[((RankWork*)rank_wp[0])->entries[above[idx]].id * 14] & 4) {
                    break;
                }
            }
            if (idx < nAbove) {
                doInsert(above, nAbove, idx, 0);
            }
        }

        if (nAbove > 1) {
            for (idx = 0; idx < nAbove; idx++) {
                if (((u16*)fighterDt)[((RankWork*)rank_wp[0])->entries[above[idx]].id * 14] & 0x1000) {
                    break;
                }
            }
            if (idx < nAbove) {
                doInsert(above, nAbove, idx, 1);
            }
        }

        if (nAbove > 10) {
            for (idx = 0; idx < nAbove; idx++) {
                if (((u16*)fighterDt)[((RankWork*)rank_wp[0])->entries[above[idx]].id * 14] & 0x800) {
                    break;
                }
            }
            if (idx < nAbove) {
                doInsert(above, nAbove, idx, 10);
            }
        }

        if (nAbove > 18) {
            for (idx = 0; idx < nAbove; idx++) {
                if (((u16*)fighterDt)[((RankWork*)rank_wp[0])->entries[above[idx]].id * 14] & 0x200) {
                    break;
                }
            }
            if (idx < nAbove) {
                doInsert(above, nAbove, idx, 18);
            }
        }

        if (nAbove > 19) {
            for (idx = 0; idx < nAbove; idx++) {
                if (((u16*)fighterDt)[((RankWork*)rank_wp[0])->entries[above[idx]].id * 14] & 0x400) {
                    break;
                }
            }
            if (idx < nAbove) {
                doInsert(above, nAbove, idx, 19);
            }
        }

        if (nAbove > 16) {
            for (idx = 0; idx < nAbove; idx++) {
                if (((u16*)fighterDt)[((RankWork*)rank_wp[0])->entries[above[idx]].id * 14] & 0x100) {
                    break;
                }
            }
            if (idx < nAbove) {
                doInsert(above, nAbove, idx, 14);
            }
        }

        if (swGet(0x9e4)) {
            for (idx = 0; idx < nAbove; idx++) {
                if (above[idx] == swByteGet(0x1fb)) {
                    break;
                }
            }
            if (idx < nAbove) {
                doInsert(above, nAbove, idx, nAbove - 1);
            }
        }
    }

    if (nBelow > 0) {
        for (p = 0; p < nBelow - 1; p++) {
            for (q = p + 1; q < nBelow; q++) {
                if (((RankWork*)rank_wp[0])->entries[below[p]].score < ((RankWork*)rank_wp[0])->entries[below[q]].score) {
                    tmp = below[p];
                    below[p] = below[q];
                    below[q] = tmp;
                }
            }
        }

        for (p = 0; p < nBelow - 1; p++) {
            for (q = p + 1; q < nBelow; q++) {
                FighterEntry* fj = &((FighterEntry*)fighterDt)[((RankWork*)rank_wp[0])->entries[below[q]].id];
                FighterEntry* fi = &((FighterEntry*)fighterDt)[((RankWork*)rank_wp[0])->entries[below[p]].id];
                if ((fj->flags & 5) && (fi->flags & 2)) {
                    tmp = below[p];
                    below[p] = below[q];
                    below[q] = tmp;
                }
            }
        }

        if (nBelow > 0) {
            for (idx = 0; idx < nBelow; idx++) {
                if (((u16*)fighterDt)[((RankWork*)rank_wp[0])->entries[below[idx]].id * 14] & 4) {
                    break;
                }
            }
            if (idx < nBelow) {
                doInsert(below, nBelow, idx, 0);
            }
        }

        if (nBelow > 0) {
            for (bi = 0; bi < nBelow; bi++) {
                if (((u16*)fighterDt)[((RankWork*)rank_wp[0])->entries[below[bi]].id * 14] & 0x800) {
                    break;
                }
            }
            if (bi < nBelow) {
                for (bi2 = 0; bi2 < nBelow; bi2++) {
                    if (((u16*)fighterDt)[((RankWork*)rank_wp[0])->entries[below[bi2]].id * 14] & 2) {
                        break;
                    }
                }
                if (bi2 < nBelow && bi2 > 0) {
                    doInsert(below, nBelow, bi, bi2 - 1);
                }
            }
        }

        if (swGet(0x9e4)) {
            for (idx = 0; idx < nBelow; idx++) {
                if (below[idx] == swByteGet(0x1fb)) {
                    break;
                }
            }
            if (idx < nBelow) {
                doInsert(below, nBelow, idx, 0);
            }
        }
    }

    entry = (RankEntry*)__memAlloc(HEAP_DEFAULT, sizeof(RankEntry) * ((RankWork*)rank_wp[0])->count);
    for (i = 0; i < nAbove; i++) {
        entry[i] = ((RankWork*)rank_wp[0])->entries[above[i]];
    }
    entry[nAbove].flags = 4;
    entry[nAbove].id = 0x14;
    entry[nAbove].score = 0;
    for (i = 0; i < nBelow; i++) {
        j = 1 + nAbove + i;
        entry[j] = ((RankWork*)rank_wp[0])->entries[below[i]];
    }
    memcpy(((RankWork*)rank_wp[0])->entries, entry, sizeof(RankEntry) * ((RankWork*)rank_wp[0])->count);
    __memFree(HEAP_DEFAULT, entry);

    rank = 0;
    for (i = 0; i < ((RankWork*)rank_wp[0])->count; i++) {
        if (!(((RankWork*)rank_wp[0])->entries[i].flags & 2)) {
            if (((RankWork*)rank_wp[0])->entries[i].flags & 4) {
                break;
            }
            rank++;
        }
    }
    swByteSet(0x244, rank);
}

static void insert(s32* arr, s32 unused, s32 from, s32 to) {
    s32 i;
    s32 val;

    if (from < to) {
        val = arr[from];
        for (i = from; i < to; i++) {
            arr[i] = arr[i + 1];
        }
        arr[to] = val;
    } else {
        val = arr[from];
        for (i = from; i > to; i--) {
            arr[i] = arr[i - 1];
        }
        arr[to] = val;
    }
}

USER_FUNC(unk_tou_00002e24) {
    RankEntry* e = ((RankWork*)rank_wp[0])->entries;
    s32 j;
    s32 i;

    for (i = 0; i < ((RankWork*)rank_wp[0])->count; i++, e++) {
        if (e->flags & 1) {
            swSet(e->id + 0x9a1);
        } else {
            swClear(e->id + 0x9a1);
        }
        for (j = 0; j < 2; j++) {
            swByteSet(e->id * 2 + 0x207 + j, ((u8*)e)[8 + j]);
        }
    }
    _mapFree(((RankWork*)rank_wp[0])->entries);
    pouchRemoveItem(ITEM_CHAMPS_BELT);
    {
        s32 k;
        for (k = 0; k < 0x14; k++) {
            swClear(k + 0x9a1);
        }
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

static USER_FUNC(evt_rankingReInit) {
    RankEntry* e = ((RankWork*)rank_wp[0])->entries;
    s32 j;
    s32 i;

    for (i = 0; i < ((RankWork*)rank_wp[0])->count; i++, e++) {
        if (e->flags & 1) {
            swSet(e->id + 0x9a1);
        } else {
            swClear(e->id + 0x9a1);
        }
        for (j = 0; j < 2; j++) {
            swByteSet(e->id * 2 + 0x207 + j, ((u8*)e)[8 + j]);
        }
    }
    _mapFree(((RankWork*)rank_wp[0])->entries);
    rankingInit();
    return EVT_RETURN_DONE;
}

static void rankingExit(void) {
    s32 i;
    s32 j;
    RankEntry* e = ((RankWork*)rank_wp[0])->entries;

    for (i = 0; i < ((RankWork*)rank_wp[0])->count; i++, e++) {
        if (e->flags & 1) {
            swSet(e->id + 0x9a1);
        } else {
            swClear(e->id + 0x9a1);
        }
        for (j = 0; j < 2; j++) {
            swByteSet(e->id * 2 + 0x207 + j, ((u8*)e)[8 + j]);
        }
    }
    _mapFree(((RankWork*)rank_wp[0])->entries);
}

static void rankingInit(void) {
    s32 i;
    s32 j;
    RankEntry* e;

    memset((RankWork*)rank_wp[0], 0, sizeof(RankWork));
    ((RankWork*)rank_wp[0])->count = evtGetValue(NULL, GSW(0)) < 0xac ? 0x17 : 0x15;
    ((RankWork*)rank_wp[0])->entries = _mapAlloc(((RankWork*)rank_wp[0])->count * sizeof(RankEntry));
    memset(((RankWork*)rank_wp[0])->entries, 0, ((RankWork*)rank_wp[0])->count * sizeof(RankEntry));
    e = ((RankWork*)rank_wp[0])->entries;
    for (i = 0; i < ((RankWork*)rank_wp[0])->count; i++, e++) {
        e->flags = 0;
        e->id = i;
        if (swGet(e->id + 0x9a1)) {
            e->flags |= 1;
        }
        if (swGet(0x9e1)) {
            for (j = 0; j < 2; j++) {
                ((u8*)e)[8 + j] = swByteGet(e->id * 2 + 0x207 + j);
            }
        } else {
            e->score = ((FighterEntry*)fighterDt)[e->id].defaultScore;
        }
        if (e->id == 0x14) {
            e->flags |= 4;
        }
    }
    swSet(0x9e1);
    if (evtGetValue(NULL, GSW(0)) < 0xac) {
        ((RankWork*)rank_wp[0])->entries[0x15].flags |= 2;
        ((RankWork*)rank_wp[0])->entries[0x16].flags |= 2;
        if (evtGetValue(NULL, 0xf84064ec)) {
            ((RankWork*)rank_wp[0])->entries[0x12].flags |= 2;
        }
        if (evtGetValue(NULL, 0xf84064ed)) {
            ((RankWork*)rank_wp[0])->entries[0x15].flags &= ~2;
            if (!evtGetValue(NULL, 0xf8406562)) {
                ((RankWork*)rank_wp[0])->entries[0x15].flags |= 1;
            }
            evtSetValue(NULL, 0xf8406562, 1);
        }
        if (evtGetValue(NULL, 0xf84064f5)) {
            ((RankWork*)rank_wp[0])->entries[0x0b].flags |= 2;
            ((RankWork*)rank_wp[0])->entries[0x16].flags &= ~2;
            if (!evtGetValue(NULL, 0xf8406563)) {
                ((RankWork*)rank_wp[0])->entries[0x16].flags |= 1;
            }
            evtSetValue(NULL, 0xf8406563, 1);
        }
    }
    rankingControll();
}

void _unresolved(void) {
    (void)0; // bare blr; the null statement keeps lint_patch from flagging an empty stub
}

void _epilog(void) {
    RankEntry* e = ((RankWork*)rank_wp[0])->entries;
    s32 j;
    s32 i;

    for (i = 0; i < ((RankWork*)rank_wp[0])->count; i++, e++) {
        if (e->flags & 1) {
            swSet(e->id + 0x9a1);
        } else {
            swClear(e->id + 0x9a1);
        }
        for (j = 0; j < 2; j++) {
            swByteSet(e->id * 2 + 0x207 + j, ((u8*)e)[8 + j]);
        }
    }
    _mapFree(((RankWork*)rank_wp[0])->entries);
    {
        voidfunctionptr* dtor2;
        for (dtor2 = _dtors; *dtor2 != NULL; dtor2++) {
            (*dtor2)();
        }
    }
}

void _prolog(void) {
    voidfunctionptr* ctor;

    for (ctor = _ctors; *ctor != NULL; ctor++) {
        (*ctor)();
    }
    relSetEvtAddr("tou_00", tou_00_init_evt);
    relSetEvtAddr("tou_01", tou_01_init_evt);
    relSetEvtAddr("tou_02", tou_02_init_evt);
    relSetEvtAddr("tou_04", tou_04_init_evt);
    relSetEvtAddr("tou_05", tou_05_init_evt);
    relSetEvtAddr("tou_06", tou_06_init_evt);
    relSetEvtAddr("tou_07", tou_07_init_evt);
    relSetEvtAddr("tou_08", tou_08_init_evt);
    relSetEvtAddr("tou_09", tou_09_init_evt);
    relSetEvtAddr("tou_10", tou_10_init_evt);
    relSetEvtAddr("tou_11", tou_11_init_evt);
    relSetEvtAddr("tou_12", tou_12_init_evt);
    relSetEvtAddr("tou_13", tou_13_init_evt);
    relSetEvtAddr("tou_20", tou_20_init_evt);
    rankingInit();
}

// clang-format on
