#include "battle/battle_ac_help.h"
#include "battle/battle.h"
#include "battle/battle_audience.h"

void BattleAcHelpSetHelp(const char* msg) {
    // field_18FF0 is where the move-described help message pointer is kept.
    *(const char**)&_battleWorkPointer->field_18FF0 = msg;
}

void BattleAcHelpSetDispType(s32 type) {
    *(s32*)&_battleWorkPointer->field_18FF0[4] = type;
    switch (type) {
    case 0:
        BattleAudience_WinSetActive(1);
        break;
    default:
        BattleAudience_WinSetActive(0);
        break;
    }
}

void BattleAcHelpInit(void) {
	//stubbed in retail
}

// NONMATCHING (4.00%): empty placeholder, not decompiled yet.
void BattleAcHelpMain(void) {

}