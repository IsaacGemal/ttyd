#pragma once

#include <dolphin/types.h>

// _btlcmd_SetAttackEvent's caller passes unit->unitId, but btlataudGetMoveEventPtr's
// definition in battle_attack_audience.c takes no argument. This header carries the caller's
// ABI view; it is kept separate because battle_attack_audience.c reaches battle_seq_command.h
// through battle.h and would otherwise see a conflicting prototype.
void* btlataudGetMoveEventPtr(s32 unitId);
