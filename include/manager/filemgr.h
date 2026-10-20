#pragma once

#include "manager/dvdmgr.h"
#include <dolphin/types.h>

typedef void (*FileCallback)(struct FileEntry* file);

// What a file holds, which picks how fileGarbageDataAdrSet/Clear relocate its file-relative pointers.
// Only RAW, TPL and ANIMPOSE are passed by any caller in this build; the numbering matches
// Super Paper Mario's filemgr (FILETYPE_GENERIC/TPL/ANIMPOSE), which leaves 1-3 and 6-10 unnamed too.
typedef enum FileType {
	FILE_TYPE_RAW = 0, // no relocation: map data (b/first, the area files), a/vivian.bin, a/ag2tg.bin, mono.dat
	FILE_TYPE_1 = 1, // 25 pointers at 0x14C-0x1AC: AnimationPoseData's table without the per-animation walk
	FILE_TYPE_2 = 2, // 8 pointers at 0x24-0x40: the layout ANIMPOSE relocates in each AnimTableEntry's data
	FILE_TYPE_3 = 3, // 1 pointer at 0x48
	FILE_TYPE_TPL = 4, // TEXPalette (.tpl textures)
	FILE_TYPE_ANIMPOSE = 5, // AnimationPoseData (a/<name>, animPoseEntry's model files)
	FILE_TYPE_6 = 6, // 1 pointer at 0x48
	FILE_TYPE_7 = 7, // 3 pointers at 0x64-0x6C
	FILE_TYPE_8 = 8, // 23 pointers at 0xC8-0x120
	FILE_TYPE_9 = 9, // 4 pointers at 0x58-0x64
	FILE_TYPE_10 = 10, // 1 pointer at 0x48
	FILE_TYPE_CANCELED = 11, // set by dvdReadDoneCallBack when the read was canceled; no case relocates it
} FileType;

typedef struct FileEntry {
	u8 state; //0x0
	s8 type; //0x1, FileType
	s16 references; //0x2
	void* inlineData; //0x4, the handle when arcOpen opened the file (data points here)
	u8 field_0x8[0x20 - 0x8]; //0x8
	char filename[64]; //0x20
	u8 field_0x60[0xA0 - 0x60]; //0x60
	void** data; //0xA0
	struct FileEntry* next; //0xA4
	FileCallback callback; //0xA8
	DVDEntry* entry; //0xAC
} FileEntry;

typedef struct FileManagerWork {
	FileEntry* entries; //0x0
	s32 archiveType; //0x4
	FileEntry* firstused; //0x8
	FileEntry* lastused; //0xC
	FileEntry* firstavailable; //0x10
	FileEntry* lastavailable; //0x14
} FileManagerWork;

void fileInit(void);
void fileGarbageMoveMem(void* data, FileEntry* file);
void _fileGarbage(BOOL a1);
FileEntry* fileAllocf(s32 type, const char* format, ...);
FileEntry* fileAlloc(const char* filename, s32 type);
void fileFree(FileEntry* handle);
s32 fileAsyncf(s32 type, FileCallback callback, const char* format, ...);
void fileSetCurrentArchiveType(s32 type);
void fileGarbageDataAdrSet(void* data, s32 type);
void fileGarbageDataAdrClear(FileEntry* entry);
void dvdReadDoneCallBack(s32 error, DVDFileInfo* info);
s32 fileAsync(const char* filename, s32 type, FileCallback callback);
FileEntry* _fileAlloc(const char* filename, s32 type, void (*callback)(FileEntry*));
