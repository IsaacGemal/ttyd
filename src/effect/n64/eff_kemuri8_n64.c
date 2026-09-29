#include "eff/n64/eff_kemuri8_n64.h"

void effKemuri8N64SetCamId(EffectEntry* entry, s32 camId) {
	((effKemuri8Data*)entry->userdata)->camId = camId;
}
