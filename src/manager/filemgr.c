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

//local prototypes




void fileInit(void) {
	FileEntry* ptr;
	int i;

	afp->entries = __memAlloc(HEAP_DEFAULT, sizeof(FileEntry) * 0x400);
	afp->archiveType = 0;
	afp->firstused = 0;
	afp->lastused = 0;
	memset(afp->entries, 0, sizeof(FileEntry) * 0x400);

	for (ptr = afp->entries, i = 0; i < 0x400; i++, ptr++) {
		ptr->next = ptr + 1;
	}

	afp->firstavailable = afp->entries;
	afp->lastavailable = &afp->entries[0x3FF];
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

// Cases 1,2,3,5,6,7,8,9,10 and the type-5 loop match exactly. Case 4 (TPL) goes through the
// fileClearTpl inline above so its loop counter/byte offset take the original's r5/r6. The CLUT
// pointer fixup's u32 view (see the MUST_MATCH below) gives the original's r4/r3 operand split.
void fileGarbageDataAdrClear(FileEntry* entry) {
	void* data = *entry->data;
	u8* d = data;

	switch (entry->field_0x1) {
		case 1:
			if (*(u32*)(d + 0x14C) >= (u32)data) {
				*(u32*)(d + 0x14C) -= (u32)data;
				*(u32*)(d + 0x150) -= (u32)data;
				*(u32*)(d + 0x154) -= (u32)data;
				*(u32*)(d + 0x158) -= (u32)data;
				*(u32*)(d + 0x15C) -= (u32)data;
				*(u32*)(d + 0x160) -= (u32)data;
				*(u32*)(d + 0x164) -= (u32)data;
				*(u32*)(d + 0x168) -= (u32)data;
				*(u32*)(d + 0x16C) -= (u32)data;
				*(u32*)(d + 0x170) -= (u32)data;
				*(u32*)(d + 0x174) -= (u32)data;
				*(u32*)(d + 0x178) -= (u32)data;
				*(u32*)(d + 0x17C) -= (u32)data;
				*(u32*)(d + 0x180) -= (u32)data;
				*(u32*)(d + 0x184) -= (u32)data;
				*(u32*)(d + 0x188) -= (u32)data;
				*(u32*)(d + 0x18C) -= (u32)data;
				*(u32*)(d + 0x190) -= (u32)data;
				*(u32*)(d + 0x194) -= (u32)data;
				*(u32*)(d + 0x198) -= (u32)data;
				*(u32*)(d + 0x19C) -= (u32)data;
				*(u32*)(d + 0x1A0) -= (u32)data;
				*(u32*)(d + 0x1A4) -= (u32)data;
				*(u32*)(d + 0x1A8) -= (u32)data;
				*(u32*)(d + 0x1AC) -= (u32)data;
			}
			break;

		case 2:
			if (*(u32*)(d + 0x24) >= (u32)data) {
				*(u32*)(d + 0x24) -= (u32)data;
				*(u32*)(d + 0x28) -= (u32)data;
				*(u32*)(d + 0x2C) -= (u32)data;
				*(u32*)(d + 0x30) -= (u32)data;
				*(u32*)(d + 0x34) -= (u32)data;
				*(u32*)(d + 0x38) -= (u32)data;
				*(u32*)(d + 0x3C) -= (u32)data;
				*(u32*)(d + 0x40) -= (u32)data;
			}
			break;

		case 3:
			if (*(u32*)(d + 0x48) >= (u32)data) {
				*(u32*)(d + 0x48) -= (u32)data;
			}
			break;

		case 4:
			fileClearTpl(data);
			break;

		case 5: { // AnimationPoseData?
			s32 i;
			s32 count;

			if (*(u32*)(d + 0x14C) >= (u32)data) {
				count = *(s32*)(d + 0x148);
				for (i = 0; i < count; i++) {
					u32 elem = *(u32*)(d + 0x1AC) + i * 0x40;
					u32 ptr = *(u32*)(elem + 0x3C);

					*(u32*)(elem + 0x3C) = ptr - (u32)data;
					if (*(u32*)(ptr + 0x24) >= ptr) {
						*(u32*)(ptr + 0x24) -= ptr;
						*(u32*)(ptr + 0x28) -= ptr;
						*(u32*)(ptr + 0x2C) -= ptr;
						*(u32*)(ptr + 0x30) -= ptr;
						*(u32*)(ptr + 0x34) -= ptr;
						*(u32*)(ptr + 0x38) -= ptr;
						*(u32*)(ptr + 0x3C) -= ptr;
						*(u32*)(ptr + 0x40) -= ptr;
					}
				}
				*(u32*)(d + 0x14C) -= (u32)data;
				*(u32*)(d + 0x150) -= (u32)data;
				*(u32*)(d + 0x154) -= (u32)data;
				*(u32*)(d + 0x158) -= (u32)data;
				*(u32*)(d + 0x15C) -= (u32)data;
				*(u32*)(d + 0x160) -= (u32)data;
				*(u32*)(d + 0x164) -= (u32)data;
				*(u32*)(d + 0x168) -= (u32)data;
				*(u32*)(d + 0x16C) -= (u32)data;
				*(u32*)(d + 0x170) -= (u32)data;
				*(u32*)(d + 0x174) -= (u32)data;
				*(u32*)(d + 0x178) -= (u32)data;
				*(u32*)(d + 0x17C) -= (u32)data;
				*(u32*)(d + 0x180) -= (u32)data;
				*(u32*)(d + 0x184) -= (u32)data;
				*(u32*)(d + 0x188) -= (u32)data;
				*(u32*)(d + 0x18C) -= (u32)data;
				*(u32*)(d + 0x190) -= (u32)data;
				*(u32*)(d + 0x194) -= (u32)data;
				*(u32*)(d + 0x198) -= (u32)data;
				*(u32*)(d + 0x19C) -= (u32)data;
				*(u32*)(d + 0x1A0) -= (u32)data;
				*(u32*)(d + 0x1A4) -= (u32)data;
				*(u32*)(d + 0x1A8) -= (u32)data;
				*(u32*)(d + 0x1AC) -= (u32)data;
			}
			break;
		}
		case 6:
			if (*(u32*)(d + 0x48) >= (u32)data) {
				*(u32*)(d + 0x48) -= (u32)data;
			}
			break;

		case 7:
			if (*(u32*)(d + 0x64) >= (u32)data) {
				*(u32*)(d + 0x64) -= (u32)data;
				*(u32*)(d + 0x68) -= (u32)data;
				*(u32*)(d + 0x6C) -= (u32)data;
			}
			break;

		case 8:
			if (*(u32*)(d + 0xC8) >= (u32)data) {
				*(u32*)(d + 0xC8) -= (u32)data;
				*(u32*)(d + 0xCC) -= (u32)data;
				*(u32*)(d + 0xD0) -= (u32)data;
				*(u32*)(d + 0xD4) -= (u32)data;
				*(u32*)(d + 0xD8) -= (u32)data;
				*(u32*)(d + 0xDC) -= (u32)data;
				*(u32*)(d + 0xE0) -= (u32)data;
				*(u32*)(d + 0xE4) -= (u32)data;
				*(u32*)(d + 0xE8) -= (u32)data;
				*(u32*)(d + 0xEC) -= (u32)data;
				*(u32*)(d + 0xF0) -= (u32)data;
				*(u32*)(d + 0xF4) -= (u32)data;
				*(u32*)(d + 0xF8) -= (u32)data;
				*(u32*)(d + 0xFC) -= (u32)data;
				*(u32*)(d + 0x100) -= (u32)data;
				*(u32*)(d + 0x104) -= (u32)data;
				*(u32*)(d + 0x108) -= (u32)data;
				*(u32*)(d + 0x10C) -= (u32)data;
				*(u32*)(d + 0x110) -= (u32)data;
				*(u32*)(d + 0x114) -= (u32)data;
				*(u32*)(d + 0x118) -= (u32)data;
				*(u32*)(d + 0x11C) -= (u32)data;
				*(u32*)(d + 0x120) -= (u32)data;
			}
			break;

		case 9:
			if (*(u32*)(d + 0x58) >= (u32)data) {
				*(u32*)(d + 0x58) -= (u32)data;
				*(u32*)(d + 0x5C) -= (u32)data;
				*(u32*)(d + 0x60) -= (u32)data;
				*(u32*)(d + 0x64) -= (u32)data;
			}
			break;

		case 10:
			if (*(u32*)(d + 0x48) >= (u32)data) {
				*(u32*)(d + 0x48) -= (u32)data;
			}
			break;

		default: break;
	}
}

// case 4's TPL walk lives in this helper: as a separate inline its loop counter
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
		case 1:
			if (*(u32*)((u8*)data + 0x14C) < (u32)data) {
				*(u32*)((u8*)data + 0x14C) += (u32)data;
				*(u32*)((u8*)data + 0x150) += (u32)data;
				*(u32*)((u8*)data + 0x154) += (u32)data;
				*(u32*)((u8*)data + 0x158) += (u32)data;
				*(u32*)((u8*)data + 0x15C) += (u32)data;
				*(u32*)((u8*)data + 0x160) += (u32)data;
				*(u32*)((u8*)data + 0x164) += (u32)data;
				*(u32*)((u8*)data + 0x168) += (u32)data;
				*(u32*)((u8*)data + 0x16C) += (u32)data;
				*(u32*)((u8*)data + 0x170) += (u32)data;
				*(u32*)((u8*)data + 0x174) += (u32)data;
				*(u32*)((u8*)data + 0x178) += (u32)data;
				*(u32*)((u8*)data + 0x17C) += (u32)data;
				*(u32*)((u8*)data + 0x180) += (u32)data;
				*(u32*)((u8*)data + 0x184) += (u32)data;
				*(u32*)((u8*)data + 0x188) += (u32)data;
				*(u32*)((u8*)data + 0x18C) += (u32)data;
				*(u32*)((u8*)data + 0x190) += (u32)data;
				*(u32*)((u8*)data + 0x194) += (u32)data;
				*(u32*)((u8*)data + 0x198) += (u32)data;
				*(u32*)((u8*)data + 0x19C) += (u32)data;
				*(u32*)((u8*)data + 0x1A0) += (u32)data;
				*(u32*)((u8*)data + 0x1A4) += (u32)data;
				*(u32*)((u8*)data + 0x1A8) += (u32)data;
				*(u32*)((u8*)data + 0x1AC) += (u32)data;
			}
			break;
		case 2:
			if (*(u32*)((u8*)data + 0x24) < (u32)data) {
				*(u32*)((u8*)data + 0x24) += (u32)data;
				*(u32*)((u8*)data + 0x28) += (u32)data;
				*(u32*)((u8*)data + 0x2C) += (u32)data;
				*(u32*)((u8*)data + 0x30) += (u32)data;
				*(u32*)((u8*)data + 0x34) += (u32)data;
				*(u32*)((u8*)data + 0x38) += (u32)data;
				*(u32*)((u8*)data + 0x3C) += (u32)data;
				*(u32*)((u8*)data + 0x40) += (u32)data;
			}
			break;
		case 3:
			if (*(u32*)((u8*)data + 0x48) < (u32)data) {
				*(u32*)((u8*)data + 0x48) += (u32)data;
			}
			break;
		case 4:
			tpl = (TEXPalette*)data;
			fileUnpackTpl(tpl);
			break;
		case 5:
			if (*(u32*)((u8*)data + 0x14C) < (u32)data) {
				*(u32*)((u8*)data + 0x14C) += (u32)data;
				*(u32*)((u8*)data + 0x150) += (u32)data;
				*(u32*)((u8*)data + 0x154) += (u32)data;
				*(u32*)((u8*)data + 0x158) += (u32)data;
				*(u32*)((u8*)data + 0x15C) += (u32)data;
				*(u32*)((u8*)data + 0x160) += (u32)data;
				*(u32*)((u8*)data + 0x164) += (u32)data;
				*(u32*)((u8*)data + 0x168) += (u32)data;
				*(u32*)((u8*)data + 0x16C) += (u32)data;
				*(u32*)((u8*)data + 0x170) += (u32)data;
				*(u32*)((u8*)data + 0x174) += (u32)data;
				*(u32*)((u8*)data + 0x178) += (u32)data;
				*(u32*)((u8*)data + 0x17C) += (u32)data;
				*(u32*)((u8*)data + 0x180) += (u32)data;
				*(u32*)((u8*)data + 0x184) += (u32)data;
				*(u32*)((u8*)data + 0x188) += (u32)data;
				*(u32*)((u8*)data + 0x18C) += (u32)data;
				*(u32*)((u8*)data + 0x190) += (u32)data;
				*(u32*)((u8*)data + 0x194) += (u32)data;
				*(u32*)((u8*)data + 0x198) += (u32)data;
				*(u32*)((u8*)data + 0x19C) += (u32)data;
				*(u32*)((u8*)data + 0x1A0) += (u32)data;
				*(u32*)((u8*)data + 0x1A4) += (u32)data;
				*(u32*)((u8*)data + 0x1A8) += (u32)data;
				*(u32*)((u8*)data + 0x1AC) += (u32)data;
				count = *(s32*)((u8*)data + 0x148);
				for (j = 0; j < count; j++) {
					AnimData* anim;
					u32 off = (u32)((AnimTableEntry*)*(u32*)((u8*)data + 0x1AC))[j].data;
					((AnimTableEntry*)*(u32*)((u8*)data + 0x1AC))[j].data = (AnimData*)(off + (u32)data);
					anim = ((AnimTableEntry*)*(u32*)((u8*)data + 0x1AC))[j].data;
					if (*(u32*)((u8*)anim + 0x24) < (u32)anim) {
						*(u32*)((u8*)anim + 0x24) += (u32)anim;
						*(u32*)((u8*)anim + 0x28) += (u32)anim;
						*(u32*)((u8*)anim + 0x2C) += (u32)anim;
						*(u32*)((u8*)anim + 0x30) += (u32)anim;
						*(u32*)((u8*)anim + 0x34) += (u32)anim;
						*(u32*)((u8*)anim + 0x38) += (u32)anim;
						*(u32*)((u8*)anim + 0x3C) += (u32)anim;
						*(u32*)((u8*)anim + 0x40) += (u32)anim;
					}
				}
			}
			break;
		case 6:
			if (*(u32*)((u8*)data + 0x48) < (u32)data) {
				*(u32*)((u8*)data + 0x48) += (u32)data;
			}
			break;
		case 7:
			if (*(u32*)((u8*)data + 0x64) < (u32)data) {
				*(u32*)((u8*)data + 0x64) += (u32)data;
				*(u32*)((u8*)data + 0x68) += (u32)data;
				*(u32*)((u8*)data + 0x6C) += (u32)data;
			}
			break;
		case 8:
			if (*(u32*)((u8*)data + 0xC8) < (u32)data) {
				*(u32*)((u8*)data + 0xC8) += (u32)data;
				*(u32*)((u8*)data + 0xCC) += (u32)data;
				*(u32*)((u8*)data + 0xD0) += (u32)data;
				*(u32*)((u8*)data + 0xD4) += (u32)data;
				*(u32*)((u8*)data + 0xD8) += (u32)data;
				*(u32*)((u8*)data + 0xDC) += (u32)data;
				*(u32*)((u8*)data + 0xE0) += (u32)data;
				*(u32*)((u8*)data + 0xE4) += (u32)data;
				*(u32*)((u8*)data + 0xE8) += (u32)data;
				*(u32*)((u8*)data + 0xEC) += (u32)data;
				*(u32*)((u8*)data + 0xF0) += (u32)data;
				*(u32*)((u8*)data + 0xF4) += (u32)data;
				*(u32*)((u8*)data + 0xF8) += (u32)data;
				*(u32*)((u8*)data + 0xFC) += (u32)data;
				*(u32*)((u8*)data + 0x100) += (u32)data;
				*(u32*)((u8*)data + 0x104) += (u32)data;
				*(u32*)((u8*)data + 0x108) += (u32)data;
				*(u32*)((u8*)data + 0x10C) += (u32)data;
				*(u32*)((u8*)data + 0x110) += (u32)data;
				*(u32*)((u8*)data + 0x114) += (u32)data;
				*(u32*)((u8*)data + 0x118) += (u32)data;
				*(u32*)((u8*)data + 0x11C) += (u32)data;
				*(u32*)((u8*)data + 0x120) += (u32)data;
			}
			break;
		case 9:
			if (*(u32*)((u8*)data + 0x58) < (u32)data) {
				*(u32*)((u8*)data + 0x58) += (u32)data;
				*(u32*)((u8*)data + 0x5C) += (u32)data;
				*(u32*)((u8*)data + 0x60) += (u32)data;
				*(u32*)((u8*)data + 0x64) += (u32)data;
			}
			break;
		case 10:
			if (*(u32*)((u8*)data + 0x48) < (u32)data) {
				*(u32*)((u8*)data + 0x48) += (u32)data;
			}
			break;
	}
}

void fileGarbageMoveMem(void* data, FileEntry* file) {
	if (file->state == 3) {
		if (!file->entry) {
			fileGarbageDataAdrClear(file);
		}
	} else {
		fileGarbageDataAdrClear(file);
	}
	memmove(data, file->data[0], (u32)file->data[1]);
	file->data[0] = data;
	if (file->state == 3) {
		if (file->entry) {
			file->entry->address = data;
		} else {
			fileGarbageDataAdrSet(file->data[0], file->field_0x1);
		}
	} else {
		fileGarbageDataAdrSet(file->data[0], file->field_0x1);
	}
}

void _fileGarbage(BOOL a1) {
	FileEntry* file;
	FileEntry* prev;
	FileEntry* last;
	FileEntry* next;

	if (a1 == 0) {
		for (file = afp->firstused; file; file = file->next) {
			if (file->state == 3) {
				if (file->entry) {
					if (OSGetCurrentThread() == &dvdmgr_thread) {
						file->entry->status = 8;
					} else {
						while (file->entry) {
							OSYieldThread();
						}
					}
				}
				file->state = 2;
			}
		}
	} else if (a1 == 1) {
		for (file = afp->firstused; file; file = file->next) {
			if (file->state == 3) {
				if (file->entry) {
					if (OSGetCurrentThread() == &dvdmgr_thread) {
						continue;
					}
					while (file->entry) {
						OSYieldThread();
					}
				} else {
					(*(s16*)((u8*)file + 0x2))--; // references is s16 in the original (lha/sth)
					if (*(s16*)((u8*)file + 0x2) <= -2) {
						file->state = 2;
					}
				}
			}
		}
	}

	prev = NULL;
	last = NULL;
	for (file = afp->firstused; file; file = next) {
		next = file->next;
		if (file->state == 2) {
			file->state = 0;
			if (&file->field_0x4 != file->data) {
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

FileEntry* _fileAlloc(const char* filename, s32 type, void (*callback)(FileEntry*)) {
	FileEntry *search, *newentry, *temp;
	void* handle;
	DVDEntry* entry;
	smartEntry* smart;
	u32 size, test;

	for (search = afp->firstused; search; search = search->next) {
		if (search->state && !strcmp(search->filename, filename)) {
			while (search->entry) {;} //infinite loop????
			if (search->state == 3) {
				search->state = 1;
				search->references = 0;
			}
			else if (search->state == 2) {
				search->state = 1;
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
		newentry->state = 1;
		newentry->data = (void**)&newentry->field_0x4;
		*newentry->data = handle;
		newentry->references = 1;
		newentry->field_0x1 = type;
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
		smart->field_0x8 = newentry;
		DVDMgrRead(entry, smart->address, test, 0);
		DVDMgrClose(entry);
		newentry->data = &smart->address;
		newentry->state = 1;
		newentry->references = 1;
		newentry->field_0x1 = type;
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
			if (handle->state == 1) {
				handle->references--;
				if (!handle->references) {
					handle->state = 2;
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
	if (error == -3) {
		file->entry = NULL;
		file->state = 2;
		file->field_0x1 = 0xB;
		DVDMgrClose(entry);
	} else {
		file->entry = NULL;
		fileGarbageDataAdrSet(*file->data, file->field_0x1);
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
			if (search->state == 3) return (s32)search;
			if (search->state == 2) {
				search->state = 3;
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
		newentry->state = 3;
		newentry->data = &newentry->field_0x4;
		*newentry->data = handle;
		newentry->references = 0;
		newentry->field_0x1 = type;
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
		smart->field_0x8 = newentry;
		newentry->data = &smart->address;
		newentry->state = 3;
		newentry->references = 0;
		newentry->field_0x1 = type;
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
