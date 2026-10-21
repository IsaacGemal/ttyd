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
    u32 nbytes;      // 0x04, GXCallDisplayList's nbytes (mapdrv)
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

typedef struct MapMatTexRef {
    void* field_0x0;  // 0x0, points at the texture name/info block
    u8 field_0x4[4];  // 0x4
    u8 wrapS;         // 0x8
    u8 wrapT;         // 0x9
    u8 field_0xA;     // 0xA, 1 or 2 picks the joint's render mode 1 or 2 (_mapEnt), as MapMaterial.field_0xA
} MapMatTexRef;

typedef struct MapMatTexMtx {
    f32 transX;     // 0x0
    f32 transY;     // 0x4, negated before the matrix is built
    f32 scaleX;     // 0x8
    f32 scaleY;     // 0xC
    f32 rotation;   // 0x10, degrees, negated before the matrix is built
    f32 field_0x14; // 0x14
    f32 field_0x18; // 0x18
} MapMatTexMtx;

typedef struct MapMaterial {
    u8 field_0x0[4];          // 0x0
    GXColor color;            // 0x4
    u8 field_0x8;             // 0x8
    u8 field_0x9;             // 0x9
    u8 field_0xA;             // 0xA, 1 or 2 picks the joint's render mode 1 or 2 (_mapEnt)
    u8 texCount;              // 0xB
    MapMatTexRef* texRefs[8]; // 0xC
    MapMatTexMtx entries[8];  // 0x2C, texture matrices (mapSetTextureMatrix)
    GXColor field_0x10C;      // 0x10C, last-stage blend colour (mapSetMaterialLastStageBlend)
    u8* field_0x110;          // 0x110
} MapMaterial;

typedef struct MapFileJointPart {
    MapMaterial* material; // 0x0
    MapFileMesh* mesh;     // 0x4
} MapFileJointPart;

typedef struct MapJointDrawMode {
    u8 unk0;             // 0x0, picks the render mode (_mapEnt)
    u8 cullMode;         // 0x1
    u8 unk2;             // 0x2, flag bits read by _mapEnt and the display-list setup
    u8 unk3[0x4 - 0x3];  // 0x3
    u32 unk4;            // 0x4, display lists are only built when 0
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

// MapObject.flags bits whose effect is visible in mapdrv.c; the rest stay hex at their uses.
enum MapObjectFlags {
    MAPOBJ_FLAG_HIDDEN = 0x1,          // mapDisp skips the normal draw (still drawn offscreen with 0x80)
    MAPOBJ_FLAG_CLIP = 0x2,            // bounds-clip in mapDisp; _mapEnt sets it for small joints
    MAPOBJ_FLAG_GROUP = 0x10,          // group root: drawn with the _mapDispMapGrp variants
    MAPOBJ_FLAG_IN_GROUP = 0x20,       // under a group root; _mapDispMapGrp also walks next
    MAPOBJ_FLAG_COLOR = 0x40,          // scale by color (mapObjSetColor, setColor)
    MAPOBJ_FLAG_OFFSCREEN = 0x80,      // also drawn to offScreenId (_setOffScrnId, mapObjSetOffScreen)
    MAPOBJ_FLAG_DISPLAY_LIST = 0x100,  // draw the prebuilt dlBuf lists; only set when dlSize[0] != 0
    MAPOBJ_FLAG_DIRTY = 0x200,         // modelWorldMtx needs recalculating (mapReCalcMatrix clears it)
    MAPOBJ_FLAG_NO_BLEND = 0x800,      // ignore MapEntry blend[0] and blend[2]
    MAPOBJ_FLAG_NO_BLEND1 = 0x1000,    // ignore MapEntry blend[1]
    MAPOBJ_FLAG_ANIM_MTX = 0x2000,     // use animLocalMtx instead of localMtx
    MAPOBJ_FLAG_NO_DISP = 0x4000,      // mapDisp skips the object entirely
    MAPOBJ_FLAG_NO_FOG = 0x80000,      // mapDrawObjectFog leaves fog off
    MAPOBJ_FLAG_FLUSH = 0x100000,      // TEV flush with color (mapObjFlushOn)
    MAPOBJ_FLAG_USER_MTX = 0x200000,   // multiply in unkAC (mapObjRotate/Translate/Scale)
    MAPOBJ_TEV_CALLBACK_MASK = 0x0F000000, // index into MapEntry tevCallback
    MAPOBJ_FLAG_BBOX = 0x20000000,     // capture the screen bounds into bboxLeft..bboxBottom (mapDispMapObj_bbox)
};

typedef struct MapObject {
    u32 flags;                // 0x0, MapObjectFlags
    u8 renderMode;            // 0x4, dispEntry render mode 0-5, chosen in _mapEnt from the joint's drawMode
    u8 cameraId;              // 0x5, dispEntry camera, CAMERA_3D from _mapEnt (lect_map_cam_chg_sub retargets it)
    u8 field_0x6[0x8 - 0x6];  // 0x6
    MapFileJoint* joints;     // 0x8
    GXColor color;            // 0xC, colour scale with MAPOBJ_FLAG_COLOR, flush colour with MAPOBJ_FLAG_FLUSH
    Vec center;               // 0x10, centre of the joint's bounds
    Mtx modelWorldMtx;        // 0x1C
    Mtx localMtx;             // 0x4C, the joint's trans * rot * scale
    Mtx animLocalMtx;         // 0x7C, localMtx with the matrix animation applied (mapCalcAnimMatrix)
    Mtx unkAC;                // 0xAC, mapObjRotate/Translate/Scale, applied with MAPOBJ_FLAG_USER_MTX
    s16 offScreenId;          // 0xDC, -1 when none (mapObjSetOffScreen, mapObjClearOffScreen)
    s16 entryIdx;             // 0xDE, index into mapWork[activeGroup].entries
    struct MapObject* parent; // 0xE0
    struct MapObject* child;  // 0xE4
    struct MapObject* next;   // 0xE8
    u16 bboxLeft;             // 0xEC, screen bounds from GXReadBoundingBox (mapDispMapObj_bbox),
    u16 bboxTop;              // 0xEE  cleared each frame with MAPOBJ_FLAG_BBOX
    u16 bboxRight;            // 0xF0
    u16 bboxBottom;           // 0xF2
    void* dlBuf[8];           // 0xF4, display list copies (makeDisplayList)
    u32 dlSize[8];            // 0x114, display list sizes (makeDisplayList)
} MapObject;

typedef struct MapError {
    char field_0x0[32]; // 0x0
    s32 field_0x20;     // 0x20
} MapError;

// Animation tracks of an "animation_table" entry. Each key holds a time and Hermite channels (value and the two
// tangents mapCalcAnimMatrix and mapMain's steps read; a nonzero flag holds the value without interpolating).
typedef struct MapAnimChannel {
    f32 value;  // 0x0
    f32 unk4;   // 0x4
    f32 unk8;   // 0x8
    f32 unkC;   // 0xC
    s32 flag;   // 0x10
} MapAnimChannel;

typedef struct MapAnimKey {
    f32 time;                 // 0x0
    MapAnimChannel chan[9];   // 0x4
    u8 unk[0xF0];             // 0xB8
} MapAnimKey;

// A joint's matrix track (mapCalcAnimMatrix): channels 0-2 translate, 3-5 rotate, 6-8 scale.
typedef struct MapAnimData {
    const char* name; // 0x0, the joint (mapGetMapObj, hitNameToPtr)
    f32 base[6];      // 0x4
    f32 div[3];       // 0x1C
    u8 unk28[0x30];   // 0x28
    s32 count;        // 0x58
    MapAnimKey key[]; // 0x5C
} MapAnimData;

// mapAnimColorObj's track: channels 0-4 go to the material's texture matrix field_0x4 (MapMatTexMtx transX..rotation).
typedef struct MapAnimTexMtxKey {
    f32 time;               // 0x0
    MapAnimChannel chan[5]; // 0x4
} MapAnimTexMtxKey;

typedef struct MapAnimTexMtxItem {
    const char* name;       // 0x0, the material (mapSearchAnimObj)
    s32 field_0x4;          // 0x4, index into MapMaterial.entries
    f32 field_0x8;          // 0x8, stored to that entry's field_0x14
    f32 field_0xC;          // 0xC, stored to that entry's field_0x18
    s32 count;              // 0x10
    MapAnimTexMtxKey key[]; // 0x14
} MapAnimTexMtxItem;

// mapAnimAlphaObj's track: channel 0 goes to the material's field_0x10C.r.
typedef struct MapAnimAlphaKey {
    f32 time;                      // 0x0
    MapAnimChannel chan[1];        // 0x4
    u8 field_0x18[0x54 - 0x18];    // 0x18
} MapAnimAlphaKey;

typedef struct MapAnimAlphaItem {
    const char* name;      // 0x0, the material (mapSearchAnimObj)
    s32 count;             // 0x4
    MapAnimAlphaKey key[]; // 0x8
} MapAnimAlphaItem;

// mapAnimLightPos's track: channels 0-2 are the light's position (x10), 3-5 its rotation.
typedef struct MapAnimLightPosKey {
    f32 time;               // 0x0
    MapAnimChannel chan[9]; // 0x4
} MapAnimLightPosKey;

typedef struct MapAnimLightPosItem {
    const char* name;         // 0x0, the light (lightNameToPtr)
    s32 count;                // 0x4
    MapAnimLightPosKey key[]; // 0x8
} MapAnimLightPosItem;

// mapAnimLightColor's track: channels 0-2 are the light's colour, 3 its attenAngle, 4 its spotAngle.
typedef struct MapAnimLightColorKey {
    f32 time;               // 0x0
    MapAnimChannel chan[5]; // 0x4
} MapAnimLightColorKey;

typedef struct MapAnimLightColorItem {
    const char* name;           // 0x0, the light (lightNameToPtr)
    s32 count;                  // 0x4
    MapAnimLightColorKey key[]; // 0x8
} MapAnimLightColorItem;

// The track lists of an "animation_table" entry: a count, then the tracks.
typedef struct MapAnimJointList {
    s32 count;              // 0x0
    MapAnimData* entries[]; // 0x4
} MapAnimJointList;

typedef struct MapAnimTexMtxList {
    s32 count;                    // 0x0
    MapAnimTexMtxItem* entries[]; // 0x4
} MapAnimTexMtxList;

typedef struct MapAnimAlphaList {
    s32 count;                   // 0x0
    MapAnimAlphaItem* entries[]; // 0x4
} MapAnimAlphaList;

typedef struct MapAnimLightPosList {
    s32 count;                      // 0x0
    MapAnimLightPosItem* entries[]; // 0x4
} MapAnimLightPosList;

typedef struct MapAnimLightColorList {
    s32 count;                        // 0x0
    MapAnimLightColorItem* entries[]; // 0x4
} MapAnimLightColorList;

// An "animation_table" chunk entry in the map file.
typedef struct MapAnimTableEntry {
    const char* name;                 // 0x0, matched against the animation name (mapSearchAnmObj)
    u8 field_0x4[4];                  // 0x4
    f32 field_0x8;                    // 0x8, in frames; the play frame wraps at it (mapMain)
    MapAnimJointList* field_0xC;      // 0xC, joint tracks for mapCalcAnimMatrix (mapMain, hitdrv)
    MapAnimTexMtxList* field_0x10;     // 0x10, mapAnimColorObj
    MapAnimAlphaList* field_0x14;     // 0x14, mapAnimAlphaObj
    MapAnimLightPosList* field_0x18;  // 0x18, mapAnimLightPos
    MapAnimLightColorList* field_0x1C; // 0x1C, mapAnimLightColor
} MapAnimTableEntry;

typedef struct MapEntryAnimData {
    u16 flags;           // 0x0
    u8 field_0x2[6];     // 0x2
    OSTime startTime;    // 0x8, in milliseconds
    f32 frame;           // 0x10
    f32 rate;            // 0x14, mapSetPlayRate
    MapAnimTableEntry* data; // 0x18
    u8 field_0x1C[4];    // 0x1C
} MapEntryAnimData;

// The "vcd_table" chunk: vertex fraction bits for the packed position and texture coordinates.
typedef struct MapVtxFmt {
    u8 field_0x0[0x44]; // 0x0
    u32 field_0x44;     // 0x44, position fraction bits (mapSetPolygonVtxDesc, hitdrv)
    u32 field_0x48[8];  // 0x48, texture coordinate fraction bits
} MapVtxFmt;

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
    void* chunkBuf;                // 0x8C, buffer for the chunk data, freed on unload
    u32 chunkBufSize;              // 0x90
    void* textureTable;            // 0x94, "texture_table" chunk
    struct MapFileLightTable* lightTable; // 0x98, "light_table" chunk
    struct MapFileFog* fogTable;   // 0x9C, "fog_table" chunk
    void* materialNameTable;       // 0xA0, "material_name_table" chunk
    MapVtxFmt* vcdTable;           // 0xA4, "vcd_table" chunk
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
    void* field_0x170;             // 0x170, ARAM copies (bmapLoad, mapLoad)
    void* field_0x174;             // 0x174
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
BOOL mapPreLoad(const char* map);
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
void mapSetTextureMatrix(MapMaterial* material);
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
extern const char str_s_s[];
void* mapSearchDmdJoint(void* a, s8* b);
