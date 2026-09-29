#include "data/mail_data.h"

typedef struct MailEntry {
    u32 flags;        // 0x0
    s32 id;           // 0x4
    const char* name; // 0x8
    const char* body; // 0xC
    u32 field_0x10;   // 0x10
} MailEntry;

MailEntry mail_data[] = {
    { 0x00010000, 0x00000000, "msg_mail_peach_stg1_name_00", "msg_mail_peach_stg1_00", 0x00000000 },
    { 0x00010000, 0x00000001, "msg_mail_peach_stg2_name_00", "msg_mail_peach_stg2_00", 0x00000000 },
    { 0x00010000, 0x00000002, "msg_mail_peach_stg3_name_00", "msg_mail_peach_stg3_00", 0x00000000 },
    { 0x00010000, 0x00000003, "msg_mail_peach_stg4_name_00", "msg_mail_peach_stg4_00", 0x00000000 },
    { 0x00010000, 0x00000004, "msg_mail_peach_stg5_name_00", "msg_mail_peach_stg5_00", 0x00000000 },
    { 0x00010000, 0x00000005, "msg_mail_peach_stg6_name_00", "msg_mail_peach_stg6_00", 0x00000000 },
    { 0x00080000, 0x00000006, "msg_mail_nazo_stg3_name_00", "msg_mail_nazo_stg3_00", 0x00000000 },
    { 0x00080000, 0x00000007, "msg_mail_nazo_stg3_name_01", "msg_mail_nazo_stg3_01", 0x00000000 },
    { 0x00080000, 0x00000008, "msg_mail_nazo_stg3_name_02", "msg_mail_nazo_stg3_02", 0x00000000 },
    { 0x00080000, 0x00000009, "msg_mail_nazo_stg3_name_03", "msg_mail_nazo_stg3_03", 0x00000000 },
    { 0x00080000, 0x0000000A, "msg_mail_nazo_stg3_name_04", "msg_mail_nazo_stg3_04", 0x00000000 },
    { 0x00080000, 0x0000000B, "msg_mail_nazo_stg3_name_05", "msg_mail_nazo_stg3_05", 0x00000000 },
    { 0x00080000, 0x0000000C, "msg_mail_nazo_stg3_name_06", "msg_mail_nazo_stg3_06", 0x00000000 },
    { 0x00080000, 0x0000000D, "msg_mail_nazo_stg3_name_07", "msg_mail_nazo_stg3_07", 0x00000000 },
    { 0x00080000, 0x0000000E, "msg_mail_nazo_stg3_name_08", "msg_mail_nazo_stg3_08", 0x00000000 },
    { 0x00080000, 0x0000000F, "msg_mail_nazo_stg3_name_09", "msg_mail_nazo_stg3_09", 0x00000000 },
    { 0x00080000, 0x0000002B, "msg_mail_nazo_stg3_name_10", "msg_mail_nazo_stg3_10", 0x00000000 },
    { 0x10000000, 0x00000010, "mail_name_00", "mail_00", 0x00000000 },
    { 0x10000000, 0x00000011, "mail_name_01", "mail_01", 0x00000001 },
    { 0x10000000, 0x00000012, "mail_name_02", "mail_02", 0x00000002 },
    { 0x10000000, 0x00000013, "mail_name_03", "mail_03", 0x00000003 },
    { 0x10000000, 0x00000014, "mail_name_04", "mail_04", 0x00000004 },
    { 0x10000000, 0x00000015, "mail_name_05", "mail_05", 0x00000005 },
    { 0x10000000, 0x00000016, "mail_name_06", "mail_06", 0x00000006 },
    { 0x10000000, 0x00000017, "mail_name_07", "mail_07", 0x00000007 },
    { 0x10000000, 0x00000018, "mail_name_08", "mail_08", 0x00000008 },
    { 0x10000000, 0x00000019, "mail_name_09", "mail_09", 0x00000009 },
    { 0x00040000, 0x0000001A, "info_mail_name_00", "info_mail_00", 0x00000000 },
    { 0x00040000, 0x0000001B, "info_mail_name_01", "info_mail_01", 0x00000000 },
    { 0x00040000, 0x0000001C, "info_mail_name_02", "info_mail_02", 0x00000000 },
    { 0x00040000, 0x0000001D, "info_mail_name_03", "info_mail_03", 0x00000000 },
    { 0x00040000, 0x0000001E, "info_mail_name_04", "info_mail_04", 0x00000000 },
    { 0x00040000, 0x0000001F, "info_mail_name_05", "info_mail_05", 0x00000000 },
    { 0x00040000, 0x00000020, "info_mail_name_06", "info_mail_06", 0x00000000 },
    { 0x00040000, 0x00000021, "info_mail_name_07", "info_mail_07", 0x00000000 },
    { 0x00000000, 0x00000022, "free_mail_name_00", "free_mail_00", 0x00000000 },
    { 0x00000000, 0x00000023, "free_mail_name_01", "free_mail_01", 0x00000000 },
    { 0x00000000, 0x00000024, "free_mail_name_02", "free_mail_02", 0x00000000 },
    { 0x00020000, 0x00000025, "irai_af_mail_00", "irai_mail_00", 0x00000000 },
    { 0x00020000, 0x00000026, "irai_af_mail_01", "irai_mail_01", 0x00000000 },
    { 0x00020000, 0x00000027, "irai_af_mail_02", "irai_mail_02", 0x00000000 },
    { 0x00020000, 0x00000028, "irai_af_mail_03", "irai_mail_03", 0x00000000 },
    { 0x00020000, 0x00000029, "irai_af_mail_04", "irai_mail_04", 0x00000000 },
    { 0x00020000, 0x0000002A, "irai_af_mail_05", "irai_mail_05", 0x00000000 },
};

void* mailGetPtr(s32 id) {
    MailEntry* p = mail_data;
    s32 i;

    for (i = 0; i < 44; i++, p++) {
        if (id == p->id) {
            break;
        }
    }
    return p;
}
