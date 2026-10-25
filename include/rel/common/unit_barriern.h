#pragma once

#include <dolphin/types.h>

// Shared by the Yux units: unit_barriern (v1, v2), unit_barriern_z and aji's unit_barriern_custom, and their satellite
// units. The functions in rel/common/unit_barriern/ compile to the same code in each unit that has them; a unit
// includes each where it sits in its function order, after the file-scope data they use (zanzou_count, the *_scale
// tables, the spline tables).
//
// MWCC gives an included file's functions their own .text section and links the unit's own section ahead of them, so
// a unit can take a function from there only if all its own functions come after it (before it in mri, built without
// deferred inlining). unit_barriern_custom, unit_barriern_z and the satellite units take all of theirs from there; v1
// takes the barriern_zanzou entry points; v2 (mri) battleGetBarriernSatelliteInitPos. The rest of v1 and v2 differs.

typedef struct BarriernZanzouScale {
    f32 value[5];
} BarriernZanzouScale;

// A satellite's move along a spline, from barriernSatelliteTo_Init (0x5C bytes, in unit work[10 + satellite]) to
// barriernSatelliteTo_Main. The array members are spline_maketable's and spline_getvalue's tables.
typedef struct BarriernSatelliteTo {
    s32 type;         // 0x00, spline_getvalue's type (the point count, 5)
    Vec* points;      // 0x04, the spline table (a splineTableIn*/Out* array of five Vecs) spline_maketable builds from
    f32 knots[5];     // 0x08, spline_maketable's knots
    Vec tangents[5];  // 0x1C, spline_maketable's tangents
    s32 frame;        // 0x58, Main's intplGetValue step over 0x3C frames; the move ends at 0x3C
} BarriernSatelliteTo;
