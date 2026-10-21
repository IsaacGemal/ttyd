// fontlenpitch, shared by the evt_yuuranking units of gor and yuu. Included where it sits in the unit's function order
// (it sets the .text layout).
#include "driver/msgdrv.h"
#include "manager/fontmgr.h"

static s32 fontlenpitch(const s8* msg, s32 pitch) {
    s32 i;
    s32 len;

    if (pitch != 0) {
        i = 0;
        len = 0;
        while (msg[i] != 0) {
            if (_ismbblead(msg[i])) {
                len++;
                i++;
            } else {
                len++;
            }
            i++;
        }
        return len * pitch;
    }
    return (u16)FontGetMessageWidth((const char*)msg);
}
