#pragma once

typedef struct ExtSortEntry ExtSortEntry;

#include <dolphin/types.h>
void nannpc_ext_dispent(void);
void nannpc_ext_main(void);
void nannpc_ext_main_sub(void);
void nannpc_ext_init(void);
void nannpc_ext_shadow_disp(void);
void nannpc_ext_disp(void);
s32 nannpc_zcompare(ExtSortEntry* a, ExtSortEntry* b);
