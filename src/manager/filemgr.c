#include "manager/filemgr.h"
#include "dolphin/os.h"
#include "driver/animdrv.h"
#include "driver/arcdrv.h"
#include "memory.h"
#include "sdk/texPalette.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

extern OSThread dvdmgr_thread;

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

#pragma dont_inline on // keeps fileGarbageDataAdrClear a real `bl` from fileGarbageMoveMem, as retail does
// NONMATCHING (16.42%): 3 instructions differ, 350 missing, 1 extra, plus register differences.
void fileGarbageDataAdrClear(FileEntry* entry) {
	TEXPalette* tpl;
	TEXHeader* img;
	CLUTHeader* pal;
	int i;
	void* data = *entry->data;
	switch (entry->field_0x1) {
		case 4: //TPL file
			tpl = data;
			if (tpl->versionNumber != 0x0020AF30) {
				OSPanic("filemgr.c", 147, "invalid version number for texture palette");
			}
			if ((u32)tpl->descriptorArray >= (u32)tpl) {
				for (i = 0; i < tpl->numDescriptors; i++) {
					img = tpl->descriptorArray[i].textureHeader;
					if (img) {
						if (img->unpacked) {
							img->unpacked = FALSE;
							img->data -= (u32)tpl;
						}
						tpl->descriptorArray[i].textureHeader = (void*)((u32)tpl->descriptorArray[i].textureHeader - (u32)tpl);
					}
					pal = tpl->descriptorArray[i].CLUTHeader;
					if (pal) {
						if (pal->unpacked) {
							pal->unpacked = FALSE;
							pal->data -= (u32)tpl;
						}
						tpl->descriptorArray[i].CLUTHeader = (void*)((u32)tpl->descriptorArray[i].CLUTHeader - (u32)tpl);
					}
				}
				tpl->descriptorArray = (void*)((u32)tpl->descriptorArray - (u32)tpl);
			}
			break;

		default: break;
	}
	//case 0, binary/default, rel
	//case 4, TEXPalette*, state 1
	//case 5, AnimationPoseData*, state 1
}

#pragma dont_inline reset

// NONMATCHING (99.81%): case 4's loop counter `i` is r5 (retail r4) and the `(u16)i * 8` index
// temp is r4 (retail r5); the descriptorArray base is r3 as retail has it and everything else
// matches. The `img`/`pal` locals are what moved the base from r4 to r3.
void fileGarbageDataAdrSet(void* data, s32 type) {
	u16 i;
	s32 j;
	s32 count;
	TEXPalette* tpl;
	TEXHeader* img;
	CLUTHeader* pal;

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
			if (tpl->versionNumber != 0x0020AF30) {
				OSPanic("filemgr.c", 106, "invalid version number for texture palette");
			}
			if ((u32)tpl->descriptorArray < (u32)tpl) {
				tpl->descriptorArray = (TEXDescriptor*)((u32)tpl->descriptorArray + (u32)tpl);
				for (i = 0; i < tpl->numDescriptors; i++) {
					if (tpl->descriptorArray[i].textureHeader) {
						tpl->descriptorArray[i].textureHeader = (TEXHeader*)((u32)tpl->descriptorArray[i].textureHeader + (u32)tpl);
						img = tpl->descriptorArray[i].textureHeader;
						if (!img->unpacked) {
							img->data = (char*)((u32)tpl + (u32)img->data);
							tpl->descriptorArray[i].textureHeader->unpacked = TRUE;
						}
					}
					if (tpl->descriptorArray[i].CLUTHeader) {
						tpl->descriptorArray[i].CLUTHeader = (CLUTHeader*)((u32)tpl->descriptorArray[i].CLUTHeader + (u32)tpl);
						pal = tpl->descriptorArray[i].CLUTHeader;
						if (!pal->unpacked) {
							pal->data = (char*)((u32)tpl + (u32)pal->data);
							tpl->descriptorArray[i].CLUTHeader->unpacked = TRUE;
						}
					}
				}
			}
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

FileEntry* fileAllocf(u8 type, const char* format, ...) {
	va_list va;

	va_start(va, format);
	vsprintf(_filename, format, va);
	return _fileAlloc(_filename, type, NULL);
}

FileEntry* fileAlloc(const char* filename, u8 type) {
	return _fileAlloc(filename, type, NULL);
}

FileEntry* _fileAlloc(const char* filename, u8 type, void (*callback)(FileEntry*)) {
	smartEntry* smart;
	DVDEntry* entry;
	FileEntry *search, *newentry, *temp;
	void* handle;
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
		newentry->data = newentry->field_0x4;
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
		test = ((32 - (size & 31)) + size) & ((size & 31) != 0);
		if (test) {
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
	if (afp->firstused) {
		afp->lastused->next = newentry;
	}
	else {
		afp->firstused = newentry;
	}
	afp->lastused = newentry;
	return newentry;
}

// NONMATCHING (89.41%): 3 instructions differ.
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

s32 fileAsyncf(u8 type, FileCallback callback, const char* format, ...) {
	va_list va;

	va_start(va, format);
	vsprintf(_filename, format, va);
	return fileAsync(_filename, type, callback);
}

// NONMATCHING (85.73%): 9 instructions differ, 22 missing, 6 extra, plus register differences.
s32 fileAsync(const char* filename, u8 type, FileCallback callback) {
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
			else {
				return (s32)search;
			}
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
		if (afp->firstused) {
			afp->lastused->next = newentry;
		}
		else {
			afp->firstused = newentry;
		}
		afp->lastused = newentry;
		return (s32)newentry;
	}
	else {
		entry = DVDMgrOpen(filename, 2, 0);
		if (!entry) {
			return -1;
		}
		size = DVDMgrGetLength(entry);
		test = ((32 - (size & 31)) + size) & ((size & 31) != 0);
		if (test) {
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
		if (afp->firstused) {
			afp->lastused->next = newentry;
		}
		else {
			afp->firstused = newentry;
		}
		afp->lastused = newentry;
		newentry->entry = DVDMgrOpen(filename, 2u, 0);
		DVDMgrReadAsync(newentry->entry, smart->address, test, 0, dvdReadDoneCallBack);
		return 0;
	}
}

//TODO: enum of all archive types
void fileSetCurrentArchiveType(s32 type) {
	afp->archiveType = type;
}
