#pragma once

#include "driver/camdrv.h"

#include <dolphin/gx.h>
#include "memory.h"

// One glare tap: its offset in pixels, scaled by offsetScale (envGlare divides x by fbWidth and y by efbHeight
// to make it a texture-matrix translation).
typedef struct EnvironmentGlareTexCoord {
    f32 x; //0x0
    f32 y; //0x4
} EnvironmentGlareTexCoord;

typedef struct EnvironmentGlare {
    GXBlendMode mode; //0x0
    GXBlendFactor srcFactor; //0x4
    GXBlendFactor dstFactor; //0x8
    GXTevKColorSel thresholdSel; //0xC, KONST each tap's stage subtracts from the capture (GX_TEV_SUB into TEVREG1)
    GXTevKColorSel weightSel; //0x10, KONST each tap's thresholded colour is multiplied by before it is added up
    f32 offsetScale; //0x14, multiplies every coords[] offset
    s32 count; //0x18, number of coords that follors
    EnvironmentGlareTexCoord coords[]; //0x1C
} EnvironmentGlare;

typedef struct EnvironmentGlareFilter {
    u16 flags; //0x0
    u16 type; //0x2
    u16 y0; //0x4
    u16 y1; //0x6
    u16 x0; //0x8
    u16 x1; //0xA
    f32 zscale; //0xC
} EnvironmentGlareFilter;

// envDisp_DepthOfField's two blur passes; index 0 is the first (flags 0x1), 1 the second (flags 0x2).
typedef struct EnvironmentDepthOfField {
    u16 flags; //0x0, 0x1 runs the first pass, 0x2 the second
    u8 depthThreshold[2]; //0x2, KONST red that the pass's GX_TEV_COMP_R8_GT stage tests the Z8 depth capture against
    f32 blurRadius[2]; //0x4, the four taps' offset: radius * sin / 100, radius * cos / 100 (a pass with 0 is skipped)
} EnvironmentDepthOfField;

typedef struct EnvironmentBlur {
    u16 flags; //0x0
    u8 pad_0x2[6]; //0x2
    u64 renderTime; //0x8
    s32 duration; //0x10, envBlurOn's value: ms over which envDisp_Blur fades the alpha (0 holds it at 0xA0)
    u8 pad_0x14[4]; //0x14
} EnvironmentBlur;

typedef struct EnvironmentWork {
    s32 flags; //0x0, 0x1 envCapture copies the frame into capture, 0x2 the Z8 depth into capture2, 0x4 envCapture2
               //      copies the blur frame, 0x8 a blur frame is there to draw, 0x10000000 envDisp_FF runs,
               //      0x20000000 glare filter on (envGlareFilter), 0x40000000 blur on (envBlurOn/Off),
               //      0x80000000 envDisp_DepthOfField runs
    smartEntry* capture; //0x4
    smartEntry* capture2; //0x8
    GXTexObj captureObj; //0xC
    GXTexObj captureObj2; //0x2C
    void* envBlur; //0x4C, see envBlurOn
    GXTexObj envBlurObj; //0x50
    MtxPtr viewMtx; //0x70, envAddTev stores the current camera view; envTevLoadTexMtxImm reads it
    MtxPtr modelMtx; //0x74, envAddTev stores EnvironmentYami.mtx
    MtxPtr modelViewMtx; //0x78, envAddTev stores viewMtx * modelMtx; envTevLoadTexMtxImm reads it
    f32 texMtxScale; //0x7C, set per type by envAddTev; envTevLoadTexMtxImm scales its texture matrix by it
                     //      (x by 0.5 - 0.25 * s for mode 1, 20 * s for 2, 2 * s for 3)
    f32 dirYaw; //0x80, degrees, set by envAddTev: envTevLoadTexMtxImm rotates about Y by it (mode 0) or aims the
                //      direction (cos p sin y, sin p, cos p cos y) with it (modes 2, 3)
    f32 dirPitch; //0x84, degrees, the p of that direction; mode 0 rotates about Z by -p (envAddTev spins it with retraceCount)
    EnvironmentDepthOfField depthOfField; //0x88, read by envDisp_DepthOfField
    u8 pad_0x94[0x98 - 0x94]; //0x94
    EnvironmentBlur blur; //0x98, envBlurOn / envDisp_Blur
    EnvironmentGlareFilter filter; //0xB0
    Mtx yamiView; //0xC0, envSetYamiView
} EnvironmentWork;

typedef struct EnvironmentYami {
    struct MapObject* obj; //0x0, the object being drawn (MapTevCallback.obj)
    MtxPtr mtx; //0x4, the object's model matrix (MapTevCallback.mtx; envAddTev and envSetWater multiply by it)
    s32 field_0x8; //0x8
    s32 tevStage; //0xC
    s32 texMap; //0x10
    s32 texCoord; //0x14
    s32 numIndStage; //0x18, as MapTevCallback's: the GXSetNumIndStages count (bom_01) and next ind stage
} EnvironmentYami;

void envInit(void);
void envReInit(void);
void envTexSetup(void);
void envMain(void);
void envBlurOn(s32 flag, s32 value);
void envBlurOff(void);
void envGlare(EnvironmentGlare* glare);
void envGlareFilterZ(f32 z);
void envSetYamiView(Mtx mtx);
void envGlareFilter(s32 type, s16 y0, s16 x0, s16 y1, s16 x1);
void envDisp_DepthOfField(void);
void envDisp_FF(void);
void envCapture(CameraId camId, void* param);
void envCapture2(CameraId camId, void* param);
void envDisp(CameraId camId, void* param);
void envDisp2(CameraId camId, void* param);
void envDisp_Blur(void);
void envSetWater(s32 arg0, void* arg1);
void envAddTev(s32 type, void* param);
void envTevLoadTexMtxImm(s32 type, u32 texMtx, u32 texMtx2, GXTexMtxType mtxType);
void envSetYami(EnvironmentYami* yami);
// The texture-matrix data the original keeps in .rodata at dat_802f9338: ten texture-matrix ids
// followed by five vectors.
typedef struct EnvTevMtxData {
    s32 tbl[10]; // 0x00
    Vec vec28;   // 0x28
    Vec vec34;   // 0x34
    Vec vec40;   // 0x40
    Vec vec4C;   // 0x4C
    Vec vec58;   // 0x58
} EnvTevMtxData;
