#include "eff/n64/eff_confetti_n64.h"
#include "driver/effdrv.h"

void effConfettiN64SetCamId(EffectEntry* entry, s32 camId) {
	((effConfettiN64Data*)entry->userdata)->camId = camId;
}
