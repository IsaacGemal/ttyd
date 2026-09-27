#pragma once

#include <dolphin/os/OSError.h>
typedef struct OSContext OSContext;

#include <dolphin/types.h>

void setupErrorHandler(void);
void systemErrorHandler(OSError error, OSContext* context, u32 dsisr, u32 dar);
