#include "manager/filemgr.h"
#include "dolphin/os.h"
#include "driver/animdrv.h"
#include "driver/arcdrv.h"
#include "memory.h"
#include "sdk/texPalette.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>


//.bss
char _filename[0x400]; //for fileAllocf/fileAsyncf vsprintf

//.sbss
FileManagerWork fileWork;

//.sdata
FileManagerWork* afp = &fileWork;

// A file stores its pointers as offsets from the file start; these add (or take away) the base address in place.
#define FILE_RELOC(field, base) (*(u32*)&(field) += (u32)(base))
#define FILE_UNRELOC(field, base) (*(u32*)&(field) -= (u32)(base))

// Header views of the file types that fileGarbageDataAdrSet and fileGarbageDataAdrClear relocate (see FileType
// in filemgr.h).
typedef struct FileType1 {
	u8 pad_0x0[0x14C];
	u32 field_0x14C;
	u32 field_0x150;
	u32 field_0x154;
	u32 field_0x158;
	u32 field_0x15C;
	u32 field_0x160;
	u32 field_0x164;
	u32 field_0x168;
	u32 field_0x16C;
	u32 field_0x170;
	u32 field_0x174;
	u32 field_0x178;
	u32 field_0x17C;
	u32 field_0x180;
	u32 field_0x184;
	u32 field_0x188;
	u32 field_0x18C;
	u32 field_0x190;
	u32 field_0x194;
	u32 field_0x198;
	u32 field_0x19C;
	u32 field_0x1A0;
	u32 field_0x1A4;
	u32 field_0x1A8;
	u32 field_0x1AC;
} FileType1;

typedef struct FileType3 {
	u8 pad_0x0[0x48];
	u32 field_0x48;
} FileType3;

typedef struct FileType7 {
	u8 pad_0x0[0x64];
	u32 field_0x64;
	u32 field_0x68;
	u32 field_0x6C;
} FileType7;

typedef struct FileType8 {
	u8 pad_0x0[0xC8];
	u32 field_0xC8;
	u32 field_0xCC;
	u32 field_0xD0;
	u32 field_0xD4;
	u32 field_0xD8;
	u32 field_0xDC;
	u32 field_0xE0;
	u32 field_0xE4;
	u32 field_0xE8;
	u32 field_0xEC;
	u32 field_0xF0;
	u32 field_0xF4;
	u32 field_0xF8;
	u32 field_0xFC;
	u32 field_0x100;
	u32 field_0x104;
	u32 field_0x108;
	u32 field_0x10C;
	u32 field_0x110;
	u32 field_0x114;
	u32 field_0x118;
	u32 field_0x11C;
	u32 field_0x120;
} FileType8;

typedef struct FileType9 {
	u8 pad_0x0[0x58];
	u32 field_0x58;
	u32 field_0x5C;
	u32 field_0x60;
	u32 field_0x64;
} FileType9;

#define POSE(data) ((AnimationPoseData*)(data))
#define FILE_ENTRY_COUNT 0x400

void fileInit(void) {
	FileEntry* ptr;
	int i;

	afp->entries = __memAlloc(HEAP_DEFAULT, sizeof(FileEntry) * FILE_ENTRY_COUNT);
	afp->archiveType = 0;
	afp->firstused = 0;
	afp->lastused = 0;
	memset(afp->entries, 0, sizeof(FileEntry) * FILE_ENTRY_COUNT);

	for (ptr = afp->entries, i = 0; i < FILE_ENTRY_COUNT; i++, ptr++) {
		ptr->next = ptr + 1;
	}

	afp->firstavailable = afp->entries;
	afp->lastavailable = &afp->entries[FILE_ENTRY_COUNT - 1];
	afp->lastavailable->next = NULL;
}

// Clears the file-relative addresses in a TPL (texture palette); the inverse of
// fileUnpackTpl below. Kept as a separate inline because the TPL walk's loop counter and
// byte-offset only take the original's r5/r6 when the walk is not expanded inside the switch.
static inline void fileClearTpl(TEXPalette* tpl) {
	u16 i;

	if (tpl->versionNumber != 0x0020AF30) {
		OSPanic("filemgr.c", 147, "invalid version number for texture palette");
	}
	if ((u32)tpl->descriptorArray >= (u32)tpl) {
		for (i = 0; i < tpl->numDescriptors; i++) {
			TEXHeader* img = tpl->descriptorArray[i].textureHeader;
			if (img) {
				if (img->unpacked) {
					img->unpacked = FALSE;
					tpl->descriptorArray[i].textureHeader->data -= (u32)tpl;
				}
				tpl->descriptorArray[i].textureHeader = (void*)((u32)tpl->descriptorArray[i].textureHeader - (u32)tpl);
			}
			if (tpl->descriptorArray[i].CLUTHeader) {
				CLUTHeader* pal = tpl->descriptorArray[i].CLUTHeader;
				if (pal->unpacked) {
					pal->unpacked = FALSE;
					tpl->descriptorArray[i].CLUTHeader->data -= (u32)tpl;
				}
#ifdef MUST_MATCH
				// MUST_MATCH: the u32 view makes the compiler split descriptorArray (r4) and its +4 offset (r3)
				// the way the original does.
				*(u32*)&tpl->descriptorArray[i].CLUTHeader -= (u32)tpl;
#else
				tpl->descriptorArray[i].CLUTHeader = (void*)((u32)tpl->descriptorArray[i].CLUTHeader - (u32)tpl);
#endif
			}
		}
		tpl->descriptorArray = (void*)((u32)tpl->descriptorArray - (u32)tpl);
	}
}

// Turns a file's pointers back into file offsets before it moves (fileGarbageMoveMem); the inverse of
// fileGarbageDataAdrSet. The TPL case goes through the fileClearTpl inline above so its loop counter and
// byte offset take the original's r5/r6.
void fileGarbageDataAdrClear(FileEntry* entry) {
	void* data = *entry->data;

	switch (entry->type) {
		case FILE_TYPE_1:
			if (((FileType1*)data)->field_0x14C >= (u32)data) {
				((FileType1*)data)->field_0x14C -= (u32)data;
				((FileType1*)data)->field_0x150 -= (u32)data;
				((FileType1*)data)->field_0x154 -= (u32)data;
				((FileType1*)data)->field_0x158 -= (u32)data;
				((FileType1*)data)->field_0x15C -= (u32)data;
				((FileType1*)data)->field_0x160 -= (u32)data;
				((FileType1*)data)->field_0x164 -= (u32)data;
				((FileType1*)data)->field_0x168 -= (u32)data;
				((FileType1*)data)->field_0x16C -= (u32)data;
				((FileType1*)data)->field_0x170 -= (u32)data;
				((FileType1*)data)->field_0x174 -= (u32)data;
				((FileType1*)data)->field_0x178 -= (u32)data;
				((FileType1*)data)->field_0x17C -= (u32)data;
				((FileType1*)data)->field_0x180 -= (u32)data;
				((FileType1*)data)->field_0x184 -= (u32)data;
				((FileType1*)data)->field_0x188 -= (u32)data;
				((FileType1*)data)->field_0x18C -= (u32)data;
				((FileType1*)data)->field_0x190 -= (u32)data;
				((FileType1*)data)->field_0x194 -= (u32)data;
				((FileType1*)data)->field_0x198 -= (u32)data;
				((FileType1*)data)->field_0x19C -= (u32)data;
				((FileType1*)data)->field_0x1A0 -= (u32)data;
				((FileType1*)data)->field_0x1A4 -= (u32)data;
				((FileType1*)data)->field_0x1A8 -= (u32)data;
				((FileType1*)data)->field_0x1AC -= (u32)data;
			}
			break;

		case FILE_TYPE_2:
			if ((u32)((AnimData*)data)->clip >= (u32)data) {
				FILE_UNRELOC(((AnimData*)data)->clip, data);
				FILE_UNRELOC(((AnimData*)data)->frames, data);
				FILE_UNRELOC(((AnimData*)data)->posData, data);
				FILE_UNRELOC(((AnimData*)data)->nrmData, data);
				FILE_UNRELOC(((AnimData*)data)->texData, data);
				FILE_UNRELOC(((AnimData*)data)->visData, data);
				FILE_UNRELOC(((AnimData*)data)->nodeData, data);
				FILE_UNRELOC(((AnimData*)data)->frames2, data);
			}
			break;

		case FILE_TYPE_3:
			if (((FileType3*)data)->field_0x48 >= (u32)data) {
				((FileType3*)data)->field_0x48 -= (u32)data;
			}
			break;

		case FILE_TYPE_TPL:
			fileClearTpl(data);
			break;

		case FILE_TYPE_ANIMPOSE: {
			s32 i;
			s32 count;

			if ((u32)POSE(data)->shapes >= (u32)data) {
				count = POSE(data)->mAnimCount;
				for (i = 0; i < count; i++) {
					AnimTableEntry* elem = &POSE(data)->mpAnims[i];
					AnimData* anim = elem->data;

					FILE_UNRELOC(elem->data, data);
					if ((u32)anim->clip >= (u32)anim) {
						FILE_UNRELOC(anim->clip, anim);
						FILE_UNRELOC(anim->frames, anim);
						FILE_UNRELOC(anim->posData, anim);
						FILE_UNRELOC(anim->nrmData, anim);
						FILE_UNRELOC(anim->texData, anim);
						FILE_UNRELOC(anim->visData, anim);
						FILE_UNRELOC(anim->nodeData, anim);
						FILE_UNRELOC(anim->frames2, anim);
					}
				}
				FILE_UNRELOC(POSE(data)->shapes, data);
				FILE_UNRELOC(POSE(data)->drawCalls, data);
				FILE_UNRELOC(POSE(data)->bufferVtxPos, data);
				FILE_UNRELOC(POSE(data)->posIndex, data);
				FILE_UNRELOC(POSE(data)->nrmVtxBuffer, data);
				FILE_UNRELOC(POSE(data)->nrmIndex, data);
				FILE_UNRELOC(POSE(data)->clr0VtxBuffer, data);
				FILE_UNRELOC(POSE(data)->clr0Index, data);
				FILE_UNRELOC(POSE(data)->texIndex[0], data);
				FILE_UNRELOC(POSE(data)->texIndex[1], data);
				FILE_UNRELOC(POSE(data)->texIndex[2], data);
				FILE_UNRELOC(POSE(data)->texIndex[3], data);
				FILE_UNRELOC(POSE(data)->texIndex[4], data);
				FILE_UNRELOC(POSE(data)->texIndex[5], data);
				FILE_UNRELOC(POSE(data)->texIndex[6], data);
				FILE_UNRELOC(POSE(data)->texIndex[7], data);
				FILE_UNRELOC(POSE(data)->texVtxBuffer, data);
				FILE_UNRELOC(POSE(data)->mBufferTexAnim, data);
				FILE_UNRELOC(POSE(data)->texData, data);
				FILE_UNRELOC(POSE(data)->texBinds, data);
				FILE_UNRELOC(POSE(data)->draws, data);
				FILE_UNRELOC(POSE(data)->mBufferVisibility, data);
				FILE_UNRELOC(POSE(data)->mBufferNode, data);
				FILE_UNRELOC(POSE(data)->groups, data);
				FILE_UNRELOC(POSE(data)->mpAnims, data);
			}
			break;
		}
		case FILE_TYPE_6:
			if (((FileType3*)data)->field_0x48 >= (u32)data) {
				((FileType3*)data)->field_0x48 -= (u32)data;
			}
			break;

		case FILE_TYPE_7:
			if (((FileType7*)data)->field_0x64 >= (u32)data) {
				((FileType7*)data)->field_0x64 -= (u32)data;
				((FileType7*)data)->field_0x68 -= (u32)data;
				((FileType7*)data)->field_0x6C -= (u32)data;
			}
			break;

		case FILE_TYPE_8:
			if (((FileType8*)data)->field_0xC8 >= (u32)data) {
				((FileType8*)data)->field_0xC8 -= (u32)data;
				((FileType8*)data)->field_0xCC -= (u32)data;
				((FileType8*)data)->field_0xD0 -= (u32)data;
				((FileType8*)data)->field_0xD4 -= (u32)data;
				((FileType8*)data)->field_0xD8 -= (u32)data;
				((FileType8*)data)->field_0xDC -= (u32)data;
				((FileType8*)data)->field_0xE0 -= (u32)data;
				((FileType8*)data)->field_0xE4 -= (u32)data;
				((FileType8*)data)->field_0xE8 -= (u32)data;
				((FileType8*)data)->field_0xEC -= (u32)data;
				((FileType8*)data)->field_0xF0 -= (u32)data;
				((FileType8*)data)->field_0xF4 -= (u32)data;
				((FileType8*)data)->field_0xF8 -= (u32)data;
				((FileType8*)data)->field_0xFC -= (u32)data;
				((FileType8*)data)->field_0x100 -= (u32)data;
				((FileType8*)data)->field_0x104 -= (u32)data;
				((FileType8*)data)->field_0x108 -= (u32)data;
				((FileType8*)data)->field_0x10C -= (u32)data;
				((FileType8*)data)->field_0x110 -= (u32)data;
				((FileType8*)data)->field_0x114 -= (u32)data;
				((FileType8*)data)->field_0x118 -= (u32)data;
				((FileType8*)data)->field_0x11C -= (u32)data;
				((FileType8*)data)->field_0x120 -= (u32)data;
			}
			break;

		case FILE_TYPE_9:
			if (((FileType9*)data)->field_0x58 >= (u32)data) {
				((FileType9*)data)->field_0x58 -= (u32)data;
				((FileType9*)data)->field_0x5C -= (u32)data;
				((FileType9*)data)->field_0x60 -= (u32)data;
				((FileType9*)data)->field_0x64 -= (u32)data;
			}
			break;

		case FILE_TYPE_10:
			if (((FileType3*)data)->field_0x48 >= (u32)data) {
				((FileType3*)data)->field_0x48 -= (u32)data;
			}
			break;

		default: break;
	}
}

// The FILE_TYPE_TPL walk lives in this helper: as a separate inline its loop counter
// colours to the original's r4 (with the body inline it takes r5).
static inline void fileUnpackTpl(TEXPalette* tpl) {
	u16 i;
	if (tpl->versionNumber != 0x0020AF30) {
		OSPanic("filemgr.c", 106, "invalid version number for texture palette");
	}
	if ((u32)tpl->descriptorArray < (u32)tpl) {
		tpl->descriptorArray = (TEXDescriptor*)((u32)tpl->descriptorArray + (u32)tpl);
		for (i = 0; i < tpl->numDescriptors; i++) {
			if (tpl->descriptorArray[i].textureHeader) {
				tpl->descriptorArray[i].textureHeader = (TEXHeader*)((u32)tpl->descriptorArray[i].textureHeader + (u32)tpl);
				if (!tpl->descriptorArray[i].textureHeader->unpacked) {
					tpl->descriptorArray[i].textureHeader->data = (char*)((u32)tpl + (u32)tpl->descriptorArray[i].textureHeader->data);
					tpl->descriptorArray[i].textureHeader->unpacked = TRUE;
				}
			}
			if (tpl->descriptorArray[i].CLUTHeader) {
				tpl->descriptorArray[i].CLUTHeader = (CLUTHeader*)((u32)tpl->descriptorArray[i].CLUTHeader + (u32)tpl);
				if (!tpl->descriptorArray[i].CLUTHeader->unpacked) {
					tpl->descriptorArray[i].CLUTHeader->data = (char*)((u32)tpl + (u32)tpl->descriptorArray[i].CLUTHeader->data);
					tpl->descriptorArray[i].CLUTHeader->unpacked = TRUE;
				}
			}
		}
	}
}

void fileGarbageDataAdrSet(void* data, s32 type) {
	u16 i;
	s32 j;
	s32 count;
	TEXPalette* tpl;

	switch (type) {
		case FILE_TYPE_1:
			if (((FileType1*)data)->field_0x14C < (u32)data) {
				((FileType1*)data)->field_0x14C += (u32)data;
				((FileType1*)data)->field_0x150 += (u32)data;
				((FileType1*)data)->field_0x154 += (u32)data;
				((FileType1*)data)->field_0x158 += (u32)data;
				((FileType1*)data)->field_0x15C += (u32)data;
				((FileType1*)data)->field_0x160 += (u32)data;
				((FileType1*)data)->field_0x164 += (u32)data;
				((FileType1*)data)->field_0x168 += (u32)data;
				((FileType1*)data)->field_0x16C += (u32)data;
				((FileType1*)data)->field_0x170 += (u32)data;
				((FileType1*)data)->field_0x174 += (u32)data;
				((FileType1*)data)->field_0x178 += (u32)data;
				((FileType1*)data)->field_0x17C += (u32)data;
				((FileType1*)data)->field_0x180 += (u32)data;
				((FileType1*)data)->field_0x184 += (u32)data;
				((FileType1*)data)->field_0x188 += (u32)data;
				((FileType1*)data)->field_0x18C += (u32)data;
				((FileType1*)data)->field_0x190 += (u32)data;
				((FileType1*)data)->field_0x194 += (u32)data;
				((FileType1*)data)->field_0x198 += (u32)data;
				((FileType1*)data)->field_0x19C += (u32)data;
				((FileType1*)data)->field_0x1A0 += (u32)data;
				((FileType1*)data)->field_0x1A4 += (u32)data;
				((FileType1*)data)->field_0x1A8 += (u32)data;
				((FileType1*)data)->field_0x1AC += (u32)data;
			}
			break;
		case FILE_TYPE_2:
			if ((u32)((AnimData*)data)->clip < (u32)data) {
				FILE_RELOC(((AnimData*)data)->clip, data);
				FILE_RELOC(((AnimData*)data)->frames, data);
				FILE_RELOC(((AnimData*)data)->posData, data);
				FILE_RELOC(((AnimData*)data)->nrmData, data);
				FILE_RELOC(((AnimData*)data)->texData, data);
				FILE_RELOC(((AnimData*)data)->visData, data);
				FILE_RELOC(((AnimData*)data)->nodeData, data);
				FILE_RELOC(((AnimData*)data)->frames2, data);
			}
			break;
		case FILE_TYPE_3:
			if (((FileType3*)data)->field_0x48 < (u32)data) {
				((FileType3*)data)->field_0x48 += (u32)data;
			}
			break;
		case FILE_TYPE_TPL:
			tpl = (TEXPalette*)data;
			fileUnpackTpl(tpl);
			break;
		case FILE_TYPE_ANIMPOSE:
			if ((u32)POSE(data)->shapes < (u32)data) {
				FILE_RELOC(POSE(data)->shapes, data);
				FILE_RELOC(POSE(data)->drawCalls, data);
				FILE_RELOC(POSE(data)->bufferVtxPos, data);
				FILE_RELOC(POSE(data)->posIndex, data);
				FILE_RELOC(POSE(data)->nrmVtxBuffer, data);
				FILE_RELOC(POSE(data)->nrmIndex, data);
				FILE_RELOC(POSE(data)->clr0VtxBuffer, data);
				FILE_RELOC(POSE(data)->clr0Index, data);
				FILE_RELOC(POSE(data)->texIndex[0], data);
				FILE_RELOC(POSE(data)->texIndex[1], data);
				FILE_RELOC(POSE(data)->texIndex[2], data);
				FILE_RELOC(POSE(data)->texIndex[3], data);
				FILE_RELOC(POSE(data)->texIndex[4], data);
				FILE_RELOC(POSE(data)->texIndex[5], data);
				FILE_RELOC(POSE(data)->texIndex[6], data);
				FILE_RELOC(POSE(data)->texIndex[7], data);
				FILE_RELOC(POSE(data)->texVtxBuffer, data);
				FILE_RELOC(POSE(data)->mBufferTexAnim, data);
				FILE_RELOC(POSE(data)->texData, data);
				FILE_RELOC(POSE(data)->texBinds, data);
				FILE_RELOC(POSE(data)->draws, data);
				FILE_RELOC(POSE(data)->mBufferVisibility, data);
				FILE_RELOC(POSE(data)->mBufferNode, data);
				FILE_RELOC(POSE(data)->groups, data);
				FILE_RELOC(POSE(data)->mpAnims, data);
				count = POSE(data)->mAnimCount;
				for (j = 0; j < count; j++) {
					AnimData* anim;
					u32 off = (u32)POSE(data)->mpAnims[j].data;
					POSE(data)->mpAnims[j].data = (AnimData*)(off + (u32)data);
					anim = POSE(data)->mpAnims[j].data;
					if ((u32)anim->clip < (u32)anim) {
						FILE_RELOC(anim->clip, anim);
						FILE_RELOC(anim->frames, anim);
						FILE_RELOC(anim->posData, anim);
						FILE_RELOC(anim->nrmData, anim);
						FILE_RELOC(anim->texData, anim);
						FILE_RELOC(anim->visData, anim);
						FILE_RELOC(anim->nodeData, anim);
						FILE_RELOC(anim->frames2, anim);
					}
				}
			}
			break;
		case FILE_TYPE_6:
			if (((FileType3*)data)->field_0x48 < (u32)data) {
				((FileType3*)data)->field_0x48 += (u32)data;
			}
			break;
		case FILE_TYPE_7:
			if (((FileType7*)data)->field_0x64 < (u32)data) {
				((FileType7*)data)->field_0x64 += (u32)data;
				((FileType7*)data)->field_0x68 += (u32)data;
				((FileType7*)data)->field_0x6C += (u32)data;
			}
			break;
		case FILE_TYPE_8:
			if (((FileType8*)data)->field_0xC8 < (u32)data) {
				((FileType8*)data)->field_0xC8 += (u32)data;
				((FileType8*)data)->field_0xCC += (u32)data;
				((FileType8*)data)->field_0xD0 += (u32)data;
				((FileType8*)data)->field_0xD4 += (u32)data;
				((FileType8*)data)->field_0xD8 += (u32)data;
				((FileType8*)data)->field_0xDC += (u32)data;
				((FileType8*)data)->field_0xE0 += (u32)data;
				((FileType8*)data)->field_0xE4 += (u32)data;
				((FileType8*)data)->field_0xE8 += (u32)data;
				((FileType8*)data)->field_0xEC += (u32)data;
				((FileType8*)data)->field_0xF0 += (u32)data;
				((FileType8*)data)->field_0xF4 += (u32)data;
				((FileType8*)data)->field_0xF8 += (u32)data;
				((FileType8*)data)->field_0xFC += (u32)data;
				((FileType8*)data)->field_0x100 += (u32)data;
				((FileType8*)data)->field_0x104 += (u32)data;
				((FileType8*)data)->field_0x108 += (u32)data;
				((FileType8*)data)->field_0x10C += (u32)data;
				((FileType8*)data)->field_0x110 += (u32)data;
				((FileType8*)data)->field_0x114 += (u32)data;
				((FileType8*)data)->field_0x118 += (u32)data;
				((FileType8*)data)->field_0x11C += (u32)data;
				((FileType8*)data)->field_0x120 += (u32)data;
			}
			break;
		case FILE_TYPE_9:
			if (((FileType9*)data)->field_0x58 < (u32)data) {
				((FileType9*)data)->field_0x58 += (u32)data;
				((FileType9*)data)->field_0x5C += (u32)data;
				((FileType9*)data)->field_0x60 += (u32)data;
				((FileType9*)data)->field_0x64 += (u32)data;
			}
			break;
		case FILE_TYPE_10:
			if (((FileType3*)data)->field_0x48 < (u32)data) {
				((FileType3*)data)->field_0x48 += (u32)data;
			}
			break;
	}
}

void fileGarbageMoveMem(void* data, FileEntry* file) {
	if (file->state == FILE_STATE_ASYNC) {
		if (!file->entry) {
			fileGarbageDataAdrClear(file);
		}
	} else {
		fileGarbageDataAdrClear(file);
	}
	memmove(data, file->data[0], (u32)file->data[1]);
	file->data[0] = data;
	if (file->state == FILE_STATE_ASYNC) {
		if (file->entry) {
			file->entry->address = data;
		} else {
			fileGarbageDataAdrSet(file->data[0], file->type);
		}
	} else {
		fileGarbageDataAdrSet(file->data[0], file->type);
	}
}

void _fileGarbage(BOOL a1) {
	FileEntry* file;
	FileEntry* prev;
	FileEntry* last;
	FileEntry* next;

	if (a1 == 0) {
		for (file = afp->firstused; file; file = file->next) {
			if (file->state == FILE_STATE_ASYNC) {
				if (file->entry) {
					if (OSGetCurrentThread() == &dvdmgr_thread) {
						file->entry->status = DVDMGR_CLOSED;
					} else {
						while (file->entry) {
							OSYieldThread();
						}
					}
				}
				file->state = FILE_STATE_GARBAGE;
			}
		}
	} else if (a1 == 1) {
		for (file = afp->firstused; file; file = file->next) {
			if (file->state == FILE_STATE_ASYNC) {
				if (file->entry) {
					if (OSGetCurrentThread() == &dvdmgr_thread) {
						continue;
					}
					while (file->entry) {
						OSYieldThread();
					}
				} else {
					file->references--;
					if (file->references <= -2) {
						file->state = FILE_STATE_GARBAGE;
					}
				}
			}
		}
	}

	prev = NULL;
	last = NULL;
	for (file = afp->firstused; file; file = next) {
		next = file->next;
		if (file->state == FILE_STATE_GARBAGE) {
			file->state = FILE_STATE_FREE;
			if (&file->inlineData != file->data) {
				smartFree((smartEntry*)file->data);
			}
			if (prev) {
				prev->next = file->next;
			}
			if (!afp->firstavailable) {
				afp->firstavailable = file;
			} else {
				afp->lastavailable->next = file;
			}
			file->next = NULL;
			afp->lastavailable = file;
		} else {
			if (!prev) {
				last = file;
			}
			prev = file;
		}
	}
	afp->firstused = last;
	afp->lastused = prev;
}

FileEntry* fileAllocf(s32 type, const char* format, ...) {
	va_list va;

	va_start(va, format);
	vsprintf(_filename, format, va);
	return _fileAlloc(_filename, type, NULL);
}

FileEntry* fileAlloc(const char* filename, s32 type) {
	return _fileAlloc(filename, type, NULL);
}

FileEntry* _fileAlloc(const char* filename, s32 type, FileCallback callback) {
	FileEntry *search, *newentry, *temp;
	void* handle;
	DVDEntry* entry;
	smartEntry* smart;
	u32 size, test;

	for (search = afp->firstused; search; search = search->next) {
		if (search->state && !strcmp(search->filename, filename)) {
			while (search->entry) {;} //infinite loop????
			if (search->state == FILE_STATE_ASYNC) {
				search->state = FILE_STATE_ALLOCATED;
				search->references = 0;
			}
			else if (search->state == FILE_STATE_GARBAGE) {
				search->state = FILE_STATE_ALLOCATED;
			}
			search->references++;
			return search;
		}
	}
	newentry = afp->firstavailable;
	if (!newentry) {
		_fileGarbage(1);
		newentry = afp->firstavailable;
	}
	if (!newentry) {
		_fileGarbage(0);
		newentry = afp->firstavailable;
	}

	handle = NULL;
	temp = newentry->next;

	switch (afp->archiveType) {
		case 0:
			handle = arcOpen(filename, NULL, NULL);
			break;
		case 1:
			handle = arcOpen(filename, NULL, NULL);
			if (!handle) {
				handle = arcOpen(filename, NULL, NULL);
			}
			if (!handle) {
				handle = arcOpen(filename, NULL, NULL);
			}
			break;
		case 2:
			handle = arcOpen(filename, NULL, NULL);
			if (!handle) {
				handle = arcOpen(filename, NULL, NULL);
			}
			if (!handle) {
				handle = arcOpen(filename, NULL, NULL);
			}
			break;
	}
	if (handle) {
		fileGarbageDataAdrSet(handle, type);
	}
	if (handle) {
		newentry->state = FILE_STATE_ALLOCATED;
		newentry->data = (void**)&newentry->inlineData;
		*newentry->data = handle;
		newentry->references = 1;
		newentry->type = type;
		newentry->next = NULL;
		strcpy(newentry->filename, filename);
		newentry->entry = NULL;
	}
	else {
		entry = DVDMgrOpen(filename, 2, 0);
		if (!entry) {
			return NULL;
		}
		size = DVDMgrGetLength(entry);
		test = size + ((size & 31) ? 32 - (size & 31) : 0);
		if (test == 0) {
			DVDMgrClose(entry);
			return NULL;
		}
		if (test & 31) {
			test += 32 - (test & 31);
		}
		smart = smartAlloc(test, 0);
		smart->file = newentry;
		DVDMgrRead(entry, smart->address, test, 0);
		DVDMgrClose(entry);
		newentry->data = &smart->address;
		newentry->state = FILE_STATE_ALLOCATED;
		newentry->references = 1;
		newentry->type = type;
		newentry->next = NULL;
		strcpy(newentry->filename, filename);
		newentry->entry = NULL;
		fileGarbageDataAdrSet(smart->address, type);
	}
	afp->firstavailable = temp;
	if (!afp->firstavailable) {
		afp->lastavailable = NULL;
	}
	if (!afp->firstused) {
		afp->firstused = newentry;
		afp->lastused = newentry;
	}
	else {
		afp->lastused->next = newentry;
		afp->lastused = newentry;
	}
	return newentry;
}

void fileFree(FileEntry* handle) {
	if (handle) {
		if (handle->data) {
			if (handle->state == FILE_STATE_ALLOCATED) {
				handle->references--;
				if (!handle->references) {
					handle->state = FILE_STATE_GARBAGE;
				}
			}
		}
	}
}

void dvdReadDoneCallBack(s32 error, DVDFileInfo* info) {
	DVDEntry* entry;
	FileEntry* file;

	for (file = afp->firstused; file; file = file->next) {
		if (&file->entry->info == info) {
			break;
		}
	}
	entry = file->entry;
	if (error == DVD_RESULT_CANCELED) {
		file->entry = NULL;
		file->state = FILE_STATE_GARBAGE;
		file->type = FILE_TYPE_CANCELED;
		DVDMgrClose(entry);
	} else {
		file->entry = NULL;
		fileGarbageDataAdrSet(*file->data, file->type);
		DVDMgrClose(entry);
		if (file->callback) {
			file->callback(file);
		}
	}
}

s32 fileAsyncf(s32 type, FileCallback callback, const char* format, ...) {
	va_list va;

	va_start(va, format);
	vsprintf(_filename, format, va);
	return fileAsync(_filename, type, callback);
}

s32 fileAsync(const char* filename, s32 type, FileCallback callback) {
	FileEntry *search, *newentry, *temp;
	smartEntry* smart;
	DVDEntry* entry;
	void* handle;
	u32 size, test;

	for (search = afp->firstused; search; search = search->next) {
		if (search->state && !strcmp(search->filename, filename)) {
			if (search->entry) return 0;
			if (search->state == FILE_STATE_ASYNC) return (s32)search;
			if (search->state == FILE_STATE_GARBAGE) {
				search->state = FILE_STATE_ASYNC;
			}
			return (s32)search;
		}
	}

	newentry = afp->firstavailable;
	if (!newentry) {
		_fileGarbage(1);
		newentry = afp->firstavailable;
	}
	if (!newentry) {
		_fileGarbage(0);
		newentry = afp->firstavailable;
	}

	handle = NULL;
	temp = newentry->next;

	switch (afp->archiveType) {
		case 0:
			handle = arcOpen(filename, NULL, NULL);
			break;
		case 1:
			handle = arcOpen(filename, NULL, NULL);
			if (!handle) {
				handle = arcOpen(filename, NULL, NULL);
			}
			if (!handle) {
				handle = arcOpen(filename, NULL, NULL);
			}
			break;
		case 2:
			handle = arcOpen(filename, NULL, NULL);
			if (!handle) {
				handle = arcOpen(filename, NULL, NULL);
			}
			if (!handle) {
				handle = arcOpen(filename, NULL, NULL);
			}
			break;
	}

	if (handle) {
		fileGarbageDataAdrSet(handle, type);
	}
	
	if (handle) {
		newentry->state = FILE_STATE_ASYNC;
		newentry->data = &newentry->inlineData;
		*newentry->data = handle;
		newentry->references = 0;
		newentry->type = type;
		newentry->next = 0;
		strcpy(newentry->filename, filename);
		newentry->entry = NULL;
		afp->firstavailable = afp->firstavailable->next;
		if (!afp->firstavailable) {
			afp->lastavailable = NULL;
		}
		if (!afp->firstused) {
			afp->firstused = newentry;
			afp->lastused = newentry;
		}
		else {
			afp->lastused->next = newentry;
			afp->lastused = newentry;
		}
		return (s32)newentry;
	}
	else {
		entry = DVDMgrOpen(filename, 2, 0);
		if (!entry) {
			return -1;
		}
		size = DVDMgrGetLength(entry);
		test = size + ((size & 31) ? 32 - (size & 31) : 0);
		if (test == 0) {
			DVDMgrClose(entry);
			return 0;
		}
		DVDMgrClose(entry);
		if (test & 31) {
			test += 32 - (test & 31);
		}
		smart = smartAlloc(test, 0);
		smart->file = newentry;
		newentry->data = &smart->address;
		newentry->state = FILE_STATE_ASYNC;
		newentry->references = 0;
		newentry->type = type;
		newentry->next = NULL;
		newentry->callback = callback;
		strcpy(newentry->filename, filename);
		afp->firstavailable = temp;
		if (!afp->firstavailable) {
			afp->lastavailable = NULL;
		}
		if (!afp->firstused) {
			afp->firstused = newentry;
			afp->lastused = newentry;
		}
		else {
			afp->lastused->next = newentry;
			afp->lastused = newentry;
		}
		newentry->entry = DVDMgrOpen(filename, 2u, 0);
		DVDMgrReadAsync(newentry->entry, smart->address, test, 0, dvdReadDoneCallBack);
		return 0;
	}
}

//TODO: enum of all archive types
void fileSetCurrentArchiveType(s32 type) {
	afp->archiveType = type;
}
