#include "MSL_C/mbstring.h"
#include "MSL_C/string.h"

int is_utf8_complete(const char* str, size_t n);
int mbtowc(wchar_t* pwc, const char* s, size_t n);

inline static int unicode_to_UTF8(char* s, wchar_t wchar) {
    int number_of_bytes;
    wchar_t wide_char;
    char* target_ptr;
    char first_byte_mark[4] = { 0x00, 0x00, 0xc0, 0xe0 };

    if (!s)
        return (0);

    wide_char = wchar;
    if (wide_char < 0x0080)
        number_of_bytes = 1;
    else if (wide_char < 0x0800)
        number_of_bytes = 2;
    else
        number_of_bytes = 3;

    target_ptr = s + number_of_bytes;

    switch (number_of_bytes) {
    case 3:
        *--target_ptr = (wide_char & 0x003f) | 0x80;
        wide_char >>= 6;
    case 2:
        *--target_ptr = (wide_char & 0x003f) | 0x80;
        wide_char >>= 6;
    case 1:
        *--target_ptr = wide_char | first_byte_mark[number_of_bytes];
    }

    return number_of_bytes;
}

inline static int wctomb(char* s, wchar_t wchar) {
    return (unicode_to_UTF8(s, wchar));
}

size_t wcstombs(char* s, const wchar_t* pwcs, size_t n) {
    int chars_written = 0;
    int result;
    char temp[3];
    wchar_t* source;

    if (!s || !pwcs)
        return (0);

    source = (wchar_t*)pwcs;
    while (chars_written <= n) {
        if (!*source) {
            *(s + chars_written) = '\0';
            break;
        } else {
            result = wctomb(temp, *source++);
            if ((chars_written + result) <= n) {
                strncpy(s + chars_written, temp, result);
                chars_written += result;
            } else
                break;
        }
    }

    return chars_written;
}

// NONMATCHING (98.47%): every instruction matches except the first test: the original compiles
// `if (!s)` to `bne over; b end` (result already 0 in r30), here it becomes a single `beq end`.
int mbtowc(wchar_t* pwc, const char* s, size_t n) {
    int result = 0;
    int len;
    int check;
    const char* p;

    if (s) {
        if (n == 0) {
            result = -1;
        } else {
            len = is_utf8_complete(s, n);
            if (len < 0) {
                result = -1;
            } else {
                p = s;
                switch (len) {
                    case 3:
                        result = (*p++ & 0x0F) << 6;
                    case 2:
                        result = ((result | (*p++ & 0x3F)) << 6) & 0xFFFF;
                    case 1:
                        result = (result | (*p & 0x7F)) & 0xFFFF;
                }

                if ((result & 0xFFFF) == 0)
                    check = 0;
                else if ((unsigned int)(result & 0xFFFF) < 0x80)
                    check = 1;
                else if ((unsigned int)(result & 0xFFFF) < 0x800)
                    check = 2;
                else
                    check = 3;
                if (check != len) {
                    result = -1;
                } else {
                    if (pwc)
                        *pwc = result;
                    result = len;
                }
            }
        }
    }
    return result;
}

// Keep is_utf8_complete out-of-line: mbtowc calls it in the original.
#pragma dont_inline on
int is_utf8_complete(const char* s, size_t n) {
    if (n == 0)
        return -1;

    if (s[0] == 0)
        return 0;
    if (!(s[0] & 0x80))
        return 1;

    if ((s[0] & 0xE0) == 0xC0) {
        if (n >= 2) {
            if ((s[1] & 0x80) == 0x80)
                return 2;
            return -1;
        }
        return -2;
    }

    if ((s[0] & 0xF0) == 0xE0) {
        if (n >= 3) {
            if ((s[1] & 0x80) == 0x80 && (s[2] & 0x80) == 0x80)
                return 3;
            return -1;
        }
        if ((n == 2 && (s[1] & 0x80) == 0x80) || n == 1)
            return -2;
        return -1;
    }

    return -1;
}
#pragma dont_inline off
