#pragma once

#include <dolphin/os.h>
#include "driver/seqdrv.h"
#include "manager/filemgr.h"

typedef struct seq_e3_work {
    FileEntry* handle; // 0x00
    u32 showMenu;      // 0x04, e3Disp draws the Press Start screen at 0 and the scenario menu at 1 (the CAMERA_OFFSCREEN2
                       //       pass draws the other)
    u32 nextShowMenu;  // 0x08, 1 on Start or A, 0 on B or the idle timeout; e3DispOffscreen copies it into showMenu under the fade
    s32 scenario;      // 0x0C
    f32 curX;          // 0x10
    f32 curY;          // 0x14
    f32 dstX;          // 0x18
    f32 dstY;          // 0x1C
    OSAlarm alarm;     // 0x20
    OSTime initTime;   // 0x48
    u8 pad_0x50[0x64 - 0x50]; // 0x50
    f32 field_0x64;    // 0x64, seq_e3Init sets it to 270, nothing reads it
} seq_e3_work;

void seq_e3Init(SequenceWork* work);
void seq_e3Main(SequenceWork* work);
void seq_e3Exit(SequenceWork* work);
