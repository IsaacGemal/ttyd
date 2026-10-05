.include "macros.inc"
.file "targsupp.s"

# 0x8025CBF0..0x8025CC10 | size: 0x20
.text
.balign 4

# .text:0x0 | 0x8025CBF0 | size: 0x8
.fn TRKAccessFile, global
/* 8025CBF0 00259BF0  0F E0 00 00 */	twui r0, 0x0
/* 8025CBF4 00259BF4  4E 80 00 20 */	blr
.endfn TRKAccessFile

# .text:0x8 | 0x8025CBF8 | size: 0x8
.fn TRKOpenFile, global
/* 8025CBF8 00259BF8  0F E0 00 00 */	twui r0, 0x0
/* 8025CBFC 00259BFC  4E 80 00 20 */	blr
.endfn TRKOpenFile

# .text:0x10 | 0x8025CC00 | size: 0x8
.fn TRKCloseFile, global
/* 8025CC00 00259C00  0F E0 00 00 */	twui r0, 0x0
/* 8025CC04 00259C04  4E 80 00 20 */	blr
.endfn TRKCloseFile

# .text:0x18 | 0x8025CC08 | size: 0x8
.fn TRKPositionFile, global
/* 8025CC08 00259C08  0F E0 00 00 */	twui r0, 0x0
/* 8025CC0C 00259C0C  4E 80 00 20 */	blr
.endfn TRKPositionFile
