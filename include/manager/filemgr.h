#pragma once

#include "manager/dvdmgr.h"
#include <dolphin/types.h>

typedef void (*FileCallback)(struct FileEntry* file);

typedef struct FileEntry {
	u8 state; //0x0
	s8 type; //0x1, TODO enum archivetype/type; selects the relocation case in fileGarbageDataAdrSet/Clear
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
