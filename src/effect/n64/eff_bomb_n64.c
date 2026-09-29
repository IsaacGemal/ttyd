#include "eff/n64/eff_bomb_n64.h"
#include "driver/effdrv.h"

// Data owned by the bomb effect. Only camId (offset 0x40) is named here.
typedef struct effBombN64Data {
	u8 pad_0x0[0x40]; // 0x0
	s32 camId;        // 0x40
} effBombN64Data;

void effBombN64SetCamId(EffectEntry* entry, s32 camId) {
	effBombN64Data* data = entry->userdata;
	data->camId = camId;
}
