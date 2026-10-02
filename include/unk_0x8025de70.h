#pragma once

#include <dolphin/types.h>

// The .rodata blob defined in src/unk_0x8025de70.c (0x803019E0..0x80301B78) holds the error-handler
// messages followed by the unsigned int->float conversion magic double. N_systemErrorHandler reaches
// each of them as an offset from one base register, so it views the whole blob as this struct.
typedef struct EffUltraHammerErrorStrings {
	char dsi[0x18];
	char isi[0x18];
	char program[0x1c];
	char context[0x1c];
	char gpr[0x24];
	char lr[0x20];
	char srr[0x20];
	char dsisr[0x20];
	char address[0x28];
	char backchain[0x1c];
	char instruction[0x60];
	f64 intToFloatMask;
} EffUltraHammerErrorStrings;

extern EffUltraHammerErrorStrings str_803019E0;
