#pragma once

typedef struct PartyEntry PartyEntry;

#include <dolphin/types.h>
#include <dolphin/mtx.h>

// This unit's .rodata zero vectors (defined in party_chuchu.c).
extern const Vec vec3_802f8288;
extern const Vec vec3_802f839c;

struct MarioWork;
struct ChuchuSearchEntry;

// Loop state for chuchu_searchObject. The four fields are held in one aggregate so
// MWCC emits the target's `i = found` register copy instead of folding it to a
// second `li`; see the MUST_MATCH note above the function.
typedef struct ChuchuSearchState {
    struct ChuchuSearchEntry* entry;
    struct MarioWork* loopPlayer;
    BOOL found;
    u32 i;
} ChuchuSearchState;

BOOL chuchu_searchObject(void);
void chuchu_exit(PartyEntry* party);
void chuchu_init(PartyEntry* party);
void chuchu_move(PartyEntry* party);
void chuchu_use(PartyEntry* party);
void chuchu_reinit(PartyEntry* party);
