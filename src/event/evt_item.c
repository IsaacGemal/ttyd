/* "evt_item" - Item-related Event Script Functions
 * Status: In progress.
 */
#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"
#include "driver/itemdrv.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "system.h"

// Prototypes not yet in a shared header (itemdrv.h only declares itemInit/itemReInit/
// itemMain/itemNameToPtr). All are defined in src/driver/itemdrv.c.
ItemEntry* itemEntry(const char* name, ItemType type, s32 a3, s32 evtId, s32 a5, f32 a6, f32 a7, f32 a8);
s32 itemStatus(ItemEntry* item, u32 mask);
void itemStatusOn(ItemEntry* item, u32 mask);
void itemFlagOff(ItemEntry* item, u16 flag);
void itemFlagOn(ItemEntry* item, u16 flag);
void itemModeChange(ItemEntry* item, u16 mode);
void itemForceGet(ItemEntry* item);
void itemDelete(char* name);

// This declaration differs from src/driver/itemdrv.c (which says `void itemDelete`) only in
// return type; see REPORT.md. It is declared void here as the definition is void.

USER_FUNC(evt_item_delete) {
	s32* args = event->args;

	itemDelete((char*)evtGetValue(event, args[0]));
	return EVT_RETURN_DONE;
}
