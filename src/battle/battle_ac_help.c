#include "battle/battle_ac_help.h"
#include "battle/battle.h"

void BattleAcHelpSetHelp(const char* msg) {
    // field_18FF0 is where the move-described help message pointer is kept.
    *(const char**)&_battleWorkPointer->field_18FF0 = msg;
}

void BattleAcHelpInit(void) {
	//stubbed in retail
}

// NONMATCHING (4.00%): empty placeholder, not decompiled yet.
void BattleAcHelpMain(void) {

}