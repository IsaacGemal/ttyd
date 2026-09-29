#include "battle/battle_party.h"
#include "battle/battle_unit.h"
#include "unit/party/unit_party_nokotarou.h"

// The unitdata_Party_* objects are defined in src/unit/party/unit_party_*.c; those translation units have no
// header declaring them yet, so declare the externs here.
extern BattleUnitKind unitdata_Party_Christine;
extern BattleUnitKind unitdata_Party_Nokotarou;
extern BattleUnitKind unitdata_Party_Yoshi;
extern BattleUnitKind unitdata_Party_Clauda;
extern BattleUnitKind unitdata_Party_Vivian;
extern BattleUnitKind unitdata_Party_Sanders;
extern BattleUnitKind unitdata_Party_Chuchurina;

// clang-format off
BattleUnitSetup entryunit_party[7] = {
    { &unitdata_Party_Christine, 0, 0x04000002, { 1000.0f, 0.0f, 1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
    { &unitdata_Party_Nokotarou, 0, 0x04000002, { 1000.0f, 0.0f, 1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
    { &unitdata_Party_Yoshi, 0, 0x04000002, { 1000.0f, 0.0f, 1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
    { &unitdata_Party_Clauda, 0, 0x04000002, { 1000.0f, 0.0f, 1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
    { &unitdata_Party_Vivian, 0, 0x04000002, { 1000.0f, 0.0f, 1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
    { &unitdata_Party_Sanders, 0, 0x04000002, { 1000.0f, 0.0f, 1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
    { &unitdata_Party_Chuchurina, 0, 0x04000002, { 1000.0f, 0.0f, 1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
};
// clang-format on

// Without this the 8-byte array lands in .sdata; the original has it in .data (see evt_memcard.c for the
// same __declspec(section ...) idiom).
__declspec(section ".data") f32 lbl_80360FE0[2] = { -1.0f, 1.0f };
