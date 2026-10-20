#include "battle/battle_information.h"

#include "battle/battle.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario_pouch.h"
#include "system.h"

// dont_inline keeps the setters real `bl`s from BattleInformationInit as in retail.
#pragma dont_inline on
void BattleInformationSetMode(FieldBattleInfo* info, u32 mode) {
	info->mode = mode;
}

void BattleInformationSetParty(FieldBattleInfo* info, BattleUnitType party) {
	info->party = party;
}

void BattleInformationSetFirstAttack(FieldBattleInfo* info, u32 partyId) {
	info->partyId = partyId;
}

#pragma dont_inline reset

void BattleInformationInit(FieldBattleInfo* info) {
	BattleInformationSetMode(info, 0);
	BattleInformationSetParty(info, 0);
	BattleInformationSetFirstAttack(info, 0);
	BattleInformationSetResult(info, 0);
	info->setup = NULL;
	*(u32*)&info->field_0x14[0] = 0;
	info->field_0x14[4] = 0;
	info->field_0x14[5] = 0;
	info->field_0x14[6] = 0;
	info->field_0x14[7] = 0;
}

void BattleInfomationSetBattleSetupInfo(FieldBattleInfo* info, NpcBattleInfo* setup) {
	info->setup = setup;
}

#pragma dont_inline on
void BattleInformationSetResult(FieldBattleInfo* info, u32 result) {
	info->result = result;
}
#pragma dont_inline reset

u32 BattleInformationGetResult(FieldBattleInfo* info) {
	return info->result;
}

// One entry of a heart/flower drop parameter table (battle_heart_drop_param_default etc.),
// read in order; the first entry whose threshold is at or above the stat is used.
typedef struct DropMaterialParam {
	s32 statPercentThreshold; // 0x0, highest HP/FP percent this entry applies to
	s32 dropChancePercent;    // 0x4, chance the entry drops anything at all
	s32 dropCount;            // 0x8, how many drops to roll for
	s32 itemChancePercent;    // 0xC, chance for each of those drops
} DropMaterialParam;

// The 20 candidate items the Hog badge rolls between.
typedef struct HogTable {
	u32 items[20];
} HogTable;

// Items the Hog badge can drop when the player has no held items.
const HogTable N_item_hog_table = {
	{
		ITEM_DRIED_SHROOM, ITEM_DRIED_SHROOM, ITEM_DRIED_SHROOM, ITEM_FRIGHT_MASK,
		ITEM_MUSHROOM, ITEM_HONEY_SYRUP, ITEM_TASTY_TONIC, ITEM_FIRE_FLOWER,
		ITEM_VOLT_SHROOM, ITEM_POW_BLOCK, ITEM_NULL, ITEM_NULL,
		ITEM_NULL, ITEM_NULL, ITEM_NULL, ITEM_NULL,
		ITEM_NULL, ITEM_NULL, ITEM_NULL, ITEM_NULL,
	}
};

// The retail scheduler hoists each drop-count load (lwz r26, 0x8(rp)) above the chance comparison.
// Computing the irand() roll into `roll` before reading the count reproduces that scheduling; the plain
// `if (irand(100) < ...)` form leaves the load after the branch. Marked MUST_MATCH (see the two loops).
void BattleInformationSetDropMaterial(FieldBattleInfo* info) {
	NpcBattleInfo* setup = info->setup;
	BattleGroupSetup* group = setup->groupSetup;
	DropMaterialParam* heartParams = (DropMaterialParam*)group->heartDropParams;
	DropMaterialParam* flowerParams = (DropMaterialParam*)group->flowerDropParams;
	BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
	s32 hpPercent;
	s32 fpPercent;
	s32 roll;
	s32 i;
	s32 threshold;
	s32 j;

	hpPercent = mario->currentHp * 100 / mario->maxHp;
	if (hpPercent > 100) {
		hpPercent = 100;
	}

	setup->heartsDropped = 0;
	while ((threshold = heartParams->statPercentThreshold) != 0) {
		if (hpPercent <= threshold) {
#ifdef MUST_MATCH
			roll = irand(100);
			i = heartParams->dropCount;
			if (roll < heartParams->dropChancePercent) {
#else
			if (irand(100) < heartParams->dropChancePercent) {
				i = heartParams->dropCount;
#endif
				while (i != 0) {
					if (irand(100) < heartParams->itemChancePercent) {
						setup->heartsDropped++;
					}
					i--;
				}
			}
			break;
		}
		heartParams++;
	}

	fpPercent = mario->currentFp * 100 / mario->maxFp;
	if (fpPercent > 100) {
		fpPercent = 100;
	}

	setup->flowersDropped = 0;
	while ((threshold = flowerParams->statPercentThreshold) != 0) {
		if (fpPercent <= threshold) {
#ifdef MUST_MATCH
			roll = irand(100);
			j = flowerParams->dropCount;
			if (roll < flowerParams->dropChancePercent) {
#else
			if (irand(100) < flowerParams->dropChancePercent) {
				j = flowerParams->dropCount;
#endif
				while (j != 0) {
					if (irand(100) < flowerParams->itemChancePercent) {
						setup->flowersDropped++;
					}
					j--;
				}
			}
			break;
		}
		flowerParams++;
	}

	{
		s32 heldItemChance = group->field_0x8;
		s32 dropTableChance = group->field_0xC;
		s32 noDropChance = group->field_0x10;
		s32 band = 0;
		s32 badge;

		// Early in the game item drops are disabled.
		if (evtGetValue(0, 0xF5DE0180) <= 0x13) {
			dropTableChance = 0;
			heldItemChance = 0;
			noDropChance = 100;
		}
		badge = pouchEquipCheckBadge(ITEM_ITEM_HOG);
		if (badge >= 1) {
			heldItemChance *= badge + 1;
			dropTableChance *= badge + 1;
		}
		// Roll over the drop-table, held-item and no-drop bands in that order.
		roll = irand(dropTableChance + heldItemChance + noDropChance);

		{
			s32 diff = roll - dropTableChance;

			if (diff < 0) {
				band = 1;
			} else if (diff - heldItemChance < 0) {
				band = 2;
			}
		}

		setup->itemDropped = 0;

		switch (band) {
			case 0:
				break;
			case 1: {
				ItemDropData* p = group->enemyData->itemDropTable;
				ItemDropData* q;
				s32 total;

				q = p;
				if (p == NULL) {
					break;
				}
				total = 0;
				while (q->dropWeight > 0 || q->type != 0) {
					total += q->dropWeight;
					q++;
				}
				if (total > 0) {
					roll = irand(total);
					while (1) {
						roll -= p->dropWeight;
						if (roll < 0) {
							setup->itemDropped = p->type;
							break;
						}
						p++;
					}
				}
				break;
			}
			case 2: {
				s32 count = 0;

				for (i = 0; i < 8; i++) {
					if (setup->heldItems[i] != 0) {
						count++;
					}
				}
				if (count > 0) {
					s32 r = irand(count);

					for (i = 0; i < 8; i++) {
						ItemType item = setup->heldItems[i];

						if (item != 0) {
							if (--r < 0) {
								setup->itemDropped = item;
								break;
							}
						}
					}
					if (i >= 8) {
						setup->itemDropped = 0;
					}
				} else if (pouchEquipCheckBadge(ITEM_ITEM_HOG) >= 1) {
					HogTable itemTable = N_item_hog_table;

					setup->itemDropped = itemTable.items[irand(20)];
				}
				break;
			}
		}
	}
}
