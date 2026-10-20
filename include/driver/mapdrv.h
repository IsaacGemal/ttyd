#pragma once

typedef struct TEXPalette TEXPalette;

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "driver/camdrv.h"

extern f32 float_0p0001_8041985c;
extern GXColor paperCraft_ambient;

typedef struct DisplayList {
    u32 offset;      // 0x00 - Offset to vertex data
    u32 vertexCount; // 0x04 - Number of vertices
} DisplayList;

typedef struct PackedDisplayList {
    u32 offset;      // 0x00 - Offset to packed data
    u16 vertexCount; // 0x04 - Number of vertices
    u8 padding[2];   // 0x06
} PackedDisplayList;

typedef struct MapFileMesh {
    u8 unk0;              // 0x0
    u8 padding[2];        // 0x1
    u8 isPackedDisplay;   // 0x3, BOOL
    s32 displayListCount; // 0x4
    u32 elementMask;      // 0x8
    u32 vcdTableOffset;   // 0xC
  union {
    u32* normalLists[1];               // unpacked: stride 4, count = *normalLists[j]
    PackedDisplayList packedLists[1];  // packed:   stride 8, count = *(u16*)((u8*)packedLists[j].offset + 1)
} displayLists;  // at 0x10
} MapFileMesh;

typedef struct MapFileJointPart {
    void* material;    // 0x0
    MapFileMesh* mesh; // 0x4
} MapFileJointPart;

typedef struct MapJointDrawMode {
    u8 unk0[0x1 - 0x0];  // 0x0
    u8 cullMode;         // 0x1
    u8 unk2[0x8 - 0x2];  // 0x2
    s32 unk8;            // 0x8
    u8 unkC[0x10 - 0xC]; // 0xC
} MapJointDrawMode;

typedef struct MapFileJoint {
    const char* name;            // 0x0
    const char* type;            // 0x4
    struct MapFileJoint* parent; // 0x8
    struct MapFileJoint* child;  // 0xC
    struct MapFileJoint* next;   // 0x10
    struct MapFileJoint* prev;   // 0x14
    Vec scale;                   // 0x18
    Vec rotation;                // 0x24
    Vec translation;             // 0x30
    Vec bboxMin;                 // 0x3C
    Vec bboxMax;                 // 0x48
    u8 field_0x54[0x58 - 0x54];  // 0x54
    MapJointDrawMode* drawMode;  // 0x58
    s32 partCount;               // 0x5C
    MapFileJointPart parts[];    // 0x60+
} MapFileJoint;

typedef struct MapFileHeader {
    s32 fileSize;     // 0x0
    s32 dataSize;     // 0x4
    s32 relCount;     // 0x8, TODO: rename?
    s32 chunkCount;   // 0xC, TODO: rename?
    s32 unused;       // 0x10
    u8 reserved[0xC]; // 0x14
} MapFileHeader;

// entry in the chunk table
typedef struct MapFileChunk {
    u32 offset; // 0x0, relative offset into data
    u32 string; // 0x4, relative offset into strings
} MapFileChunk;

typedef struct MapFileInfo {
    const char* version; // 0x0
    MapFileJoint* joint; // 0x4
    const char* mapName; // 0x8
    const char* hitName; // 0xC
} MapFileInfo;

typedef struct MapHeader {
    MapFileHeader file;         // 0x0
    void* data;                 // 0x20, TODO re-type?
    void* rel;                  // 0x24, TODO: re-type?
    MapFileChunk* chunks;       // 0x28
    void* unused;               // 0x2C
    const char* strings;        // 0x30
    u8 field_0x34[0x3C - 0x34]; // 0x34
    s32 field_0x3C;             // 0x3C, dispFlags something?
    void* buffer;               // 0x40, the loaded map file
} MapHeader;

typedef struct MapObject {
    u32 flags;                // 0x0, the map object flag word (mapObjFlagOn, mapFlgOn)
    u8 dispMode;              // 0x4, 0-5, set from the joint's drawMode in mapEntrySub
    u8 field_0x5;             // 0x5, set to 4 in mapEntrySub, never read in mapdrv.c
    u8 field_0x6[0x8 - 0x6];  // 0x6
    MapFileJoint* joints;     // 0x8
    GXColor flushColor;       // 0xC, flush colour (mapObjSetFlushColor); alpha is byte 3
    Vec center;               // 0x10, centre of the joint's bounds
    Mtx modelWorldMtx;        // 0x1C
    Mtx localMtx;             // 0x4C, the joint's trans * rot * scale
    Mtx altLocalMtx;          // 0x7C, used instead of localMtx when flags & 0x2000
    Mtx unkAC;                // 0xAC, mapObjRotate/Translate/Scale, applied when flags & 0x200000
    s16 offScreenId;          // 0xDC, -1 when none (mapObjSetOffScreen, mapObjClearOffScreen)
    s16 entryIdx;             // 0xDE, index into mapWork[activeGroup].entries
    struct MapObject* parent; // 0xE0
    struct MapObject* child;  // 0xE4
    struct MapObject* next;   // 0xE8
    u8 field_0xEC[0xF4 - 0xEC];  // 0xEC, four u16 cleared when flags & 0x20000000
    void* dlBuf[8];           // 0xF4, display list copies (makeDisplayList)
    u32 dlSize[8];            // 0x114, display list sizes (makeDisplayList)
} MapObject;

typedef struct MapError {
    char field_0x0[32]; // 0x0
    s32 field_0x20;     // 0x20
} MapError;

typedef struct MapEntryAnimData {
    u16 flags;           // 0x0
    u8 field_0x2[6];     // 0x2
    OSTime startTime;    // 0x8, in milliseconds
    f32 frame;           // 0x10
    f32 rate;            // 0x14, mapSetPlayRate
    void* data;          // 0x18, the animation_table entry
    u8 field_0x1C[4];    // 0x1C
} MapEntryAnimData;

typedef struct MapEntry {
    s32 count;                     // 0x0
    u16 flags;                     // 0x4
    char name[16];                 // 0x6
    GXColor blend[3];              // 0x16, colour scales for flags 0x2 / 0x4 / 0x8 (mapSetBlend, mapSetBlend2)
    u8 field_0x22[0x24 - 0x22];    // 0x22
    s32 fogEnable;                 // 0x24
    s32 fogType;                   // 0x28, index into fog_type
    f32 fogStart;                  // 0x2C
    f32 fogEnd;                    // 0x30
    GXColor fogColor;              // 0x34
    MapHeader header;              // 0x38
    void* dat;                     // 0x7C, ./map/%s/map.dat
    u32 datSize;                   // 0x80
    void* tpl;                     // 0x84, ./map/%s/texture.tpl
    u32 tplSize;                   // 0x88
    u8 field_0x8C[0x94 - 0x8C];    // 0x8C
    void* textureTable;            // 0x94, "texture_table" chunk
    struct MapFileLightTable* lightTable; // 0x98, "light_table" chunk
    struct MapFileFog* fogTable;   // 0x9C, "fog_table" chunk
    void* materialNameTable;       // 0xA0, "material_name_table" chunk
    void* vcdTable;                // 0xA4, "vcd_table" chunk
    MapObject* rootMapObj;         // 0xA8
    struct HitEntry* rootHitObj;     // 0xAC
    Vec bbox[8];                   // 0xB0
    void* tevCallback[16];         // 0x110, TEV-stage callbacks (mapSetTevCallback)
    s32 numJoints;                 // 0x150, TODO: rename? see: hitNumJoints
    MapObject* objects;            // 0x154, TODO: rename? see: hitNumJoints
    s32 hitNumJoints;              // 0x158
    struct HitEntry* hitEntries;     // 0x15C
    s32 animCount;                 // 0x160, entries in animData
    MapEntryAnimData* animData;    // 0x164
    u32 dlTotal;                   // 0x168, total size of the display lists (makeDisplayList)
    void* dlData;                  // 0x16C, buffer the display lists are copied into
    u8 field_0x170[0x178 - 0x170]; // 0x170, ARAM copies (bmapLoad, mapLoad)
} MapEntry;

typedef struct MapWork {
    MapEntry entries[2]; // 0x0
    s32 field_2F0;       // 0x2F0
} MapWork;

void mapErrorEntry(s32 arg0, const char* arg1);
const char* getMapDataDvdRoot(void);
void mapDispOff(void);
void mapDispOn(void);
MapWork* mapGetWork(void);
s32 mapGetActiveGroup(void);
s32 mapGetJoints(MapFileJoint* joint);
void mapInit(void);
MapEntryAnimData* mapSearchAnmObj(const char* animName);
void mapCheckAnimation(const char* animName, BOOL* valid, f32* a3);
// more animation functions?
void mapUnLoad(void);
BOOL mapPreLoad(void);
void mapLoad(const char* map);

void mapMain(void);
void mapDisp(void);

void mapSetMaterialLight(s32 flags, Vec position);
void mapSetMaterial(MapObject* obj, void* material);
void mapSetMaterialLastStageBlend(s32 flags, GXColor color1, GXColor color2);
void mapSetMaterialFog(void);
void mapSetMaterialTev(s32 texCount, s32 drawMode, s32 flags, Mtx mtx);

MapObject* mapGetMapObj(const char* name);

void mapObjScale(const char* name, f32 x, f32 y, f32 z);
void mapObjTranslate(const char* name, f32 x, f32 y, f32 z);
void mapObjRotate(const char* name, f32 x, f32 y, f32 z);
void mapObjGetPos(const char* name, Vec* pos);
void mapObjFlagOn(const char* name, u32 flag);
void mapGrpSetOffScreen(const char* name, const char* offName);
void test_kururing_mapdisp(CameraId camId);
void mapGrpFlagOn(const char* name, u32 flag);
void mapGrpFlagOff(const char* name, u32 flag);
void mapSetTevCallback(s32 idx, void* cb);
void mapObjFlagOff(const char* name, u32 flag);
void makeDisplayList(BOOL group);
void mapGrpClearOffScreen(const char* name);
void N_mapDispOn(void);
void N_mapDispOff(void);
void mapObjGetFlushColor(const char* name, u8* r, u8* g, u8* b, u8* a);
void mapObjSetFlushColor(const char* name, u8 r, u8 g, u8 b, u8 a);
void mapGrpSetFlushColor(const char* name, u8 r, u8 g, u8 b, u8 a);
void mapObjFlushOn(const char* name);
void mapObjFlushOff(const char* name);
void mapGrpFlushOn(const char* name);
void mapGrpFlushOff(const char* name);
void mapObjClearOffScreen(const char* name);
void mapObjSetOffScreen(const char* name, const char* offName);
void mapSetPlayRate(const char* name, f32 rate);
void mapReStartAnimation(const char* name);
void mapReStartAnimationAll(void);
void mapPauseAnimation(const char* name);
void mapPauseAnimationAll(void);
void mapBlendOff(void);
void mapBlendOff2(void);
void mapFogOn(void);
void mapFogOff(void);
void mapGetFog(u32* c, f32* near, f32* far, u32* c2);
void mapSetFog(u32 color, f32 near, f32 far, GXColor color2);
void mapResetPaperAmbColor(void);
void mapResetPaperCraftAmbColor(void);
void mapGrpSetColor(const char* name, GXColor color);
void mapFlgOff(MapObject* obj, u32 flag, s32 a3);
void mapFlgOn(MapObject* obj, u32 flag, s32 a3);
void setColor(MapObject* obj, GXColor color, s32 a3);
void mapSetPolygon(MapObject* obj, MapFileMesh* mesh);
void _mapDispMapGrp(CameraId camId, MapObject* obj);
void _mapDispMapObj(CameraId camId, MapObject* obj);
void _mapDispMapGrp_NoMaterial(MapObject* obj, void* a2);
void _mapDispMapObj_NoMaterial(s32 camId, MapObject* obj);
void mapDispMapObj_bbox(s32 camId, MapObject* obj);
void _setOffScrnId(MapObject* obj, s32 id, s32 a3);
void* mapSearchDmdJointSub(void* a, void* b);
void* mapSearchDmdJointSub2(void* a, void* b, const char* c);
void mapGetJointsSub(MapFileJoint* joint, s32* numJoints);
void mapBuildTexture(MapObject* obj, TEXPalette* texture, void* table);
void _mapLoad(MapWork* wp, BOOL group, const char* map);
BOOL mapCheckData(MapHeader* header, const char* map, const char* date);
void mapSetProjMtx(Mtx mtx);
void mapSetProjTexObj(GXTexObj* obj, void* imgRef);
void mapObjSetColor(const char* name, GXColor* color);
void mapSetBlend(GXColor* color);
void mapSetBlend2(GXColor* color);
void mapPlayAnimationLv(const char* name, s32 a2, s32 a3);
void spline_maketable(s32 count, s32 a, s32* b, s32* c);
void spline_getvalue(f32* out, s32 type, s32 a, s32 b, s32 c, f32 t);
void mapSetPaperAmbColor(GXColor* color);
void mapObjGetPosSub(MapObject* obj, Vec* pos, s32* scale, void* a4);
void mapGetBoundingBox(Vec* min, Vec* max);
MapObject* _mapEnt(MapFileJoint* joint, MapObject* parent, Mtx mtx, s32 index);
MapObject* mapEntrySub(MapFileJoint* joint, MapObject* parent, Mtx mtx, BOOL isRoot, s32 index);
MapObject* mapEntry(MapFileJoint* joint, Mtx mtx, s32 index);
void bmapLoad(const char* map1, const char* map2);
void bmapUnLoad(void);
void mapGetBlend(GXColor* out);
void mapGetBlend2(GXColor* out);
#include "driver/lightdrv.h"
s32 mapSetLight(MapObject* obj, LightEntry** entries);
void mapSetPolygonVtxDesc(MapObject* obj, MapFileMesh* mesh);
void mapCalcAnimMatrix(Mtx outMtx, Mtx inMtx, MapEntryAnimData* anim, void* joint);
typedef struct MapMatTexMtx {
    f32 field_0x0;  // 0x0
    f32 field_0x4;  // 0x4
    f32 field_0x8;  // 0x8
    f32 field_0xC;  // 0xC
    f32 field_0x10; // 0x10
    f32 field_0x14; // 0x14
    f32 field_0x18; // 0x18
} MapMatTexMtx;
typedef struct MapMaterialTex {
    u8 field_0x0[0xB];        // 0x0
    u8 field_0xB;             // 0xB
    u8 field_0xC[0x2C - 0xC]; // 0xC
    MapMatTexMtx entries[];   // 0x2C
} MapMaterialTex;
void mapSetTextureMatrix(MapMaterialTex* material);
void mapReCalcMatrix(MapObject* obj, Mtx mtx, BOOL force);
BOOL mapTestXLU(u32 flags, GXColor* a, GXColor* b);
typedef struct MapTevCallback {
    MapObject* obj;  // 0x0
    void* mtx;       // 0x4
    s32 drawMode;    // 0x8
    s32 tevStage;    // 0xC
    s32 texMap;      // 0x10
    s32 texCoord;    // 0x14
    s32 numIndStage; // 0x18
} MapTevCallback;
typedef struct MapFileLight {
    const char* name; // 0x0
    const char* type; // 0x4
    Vec position;     // 0x8
    Vec rotation;     // 0x14
    Vec field_0x20;   // 0x20
    GXColor color;    // 0x2C
    f32 spotAngle;    // 0x30
    f32 attenAngle;   // 0x34
    s32 attenType;    // 0x38
    s32 field_0x3C;   // 0x3C
    u32 field_0x40;   // 0x40
} MapFileLight;
typedef struct MapFileLightTable {
    s32 count;              // 0x0
    MapFileLight* lights[]; // 0x4
} MapFileLightTable;
typedef struct MapFileFog {
    s32 enable;     // 0x0
    s32 type;       // 0x4
    f32 start;      // 0x8
    f32 end;        // 0xC
    GXColor color;  // 0x10
} MapFileFog;
typedef struct MapFileAnimEntry {
    u16 field_0x0;              // 0x0
    u8 field_0x2[0x18 - 0x2];   // 0x2
    u32 field_0x18;             // 0x18
    u8 field_0x1C[0x20 - 0x1C]; // 0x1C
} MapFileAnimEntry;
extern const char str_80419848[];
extern const char str_804198D0[];
void* mapSearchDmdJoint(void* a, s8* b);
