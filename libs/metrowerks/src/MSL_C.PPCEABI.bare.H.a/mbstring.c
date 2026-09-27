#include "MSL_C/mbstring.h"
#include "MSL_C/string.h"

int is_utf8_complete(const char* str, size_t n);
int mbtowc(wchar_t* pwc, const char* s, size_t n);

// `(wchar_t)r4 >> 6` (below) and the `& 0xFFFF` masks in mbtowc are not
// behaviour changes: the values always fit in 16 bits, but the explicit 16-bit
// type makes MWCC emit the original's rlwinm/extrwi instead of a 32-bit shift.
static size_t unicode_to_UTF8(char* r3, unsigned int r4) {
    const unsigned char lut[] = { 0, 0, 0xC0, 0xE0 };
    int r5;
    char* r6;

    if (!r3)
        return 0;

    if (r4 < 0x80)
        r5 = 1;
    else if (r4 < 0x800)
        r5 = 2;
    else
        r5 = 3;

    r6 = r3 + r5;
    switch (r5) {
        case 3:
            *--r6 = (r4 & 0x3F) | 0x80;
            r4 = (wchar_t)r4 >> 6;
        case 2:
            *--r6 = (r4 & 0x3F) | 0x80;
            r4 = (wchar_t)r4 >> 6;
        case 1:
            *--r6 = r4 | lut[r5];
    }
    return r5;
}

// NONMATCHING (98.36%): the inlined unicode_to_UTF8 adds one clrlwi that
// converts the wchar_t argument to the helper's unsigned int parameter, and the
// `i + num > n` test uses r3 instead of r31.
size_t wcstombs(char* s, const wchar_t* pwcs, size_t n) {
    char decoded[4];
    wchar_t w;
    size_t num;
    int i = 0;
    const wchar_t* p;

    if (!s || !pwcs)
        return 0;

    p = pwcs;
    while (i <= n) {
        w = *p;

        if (!w) {
            s[i] = 0;
            break;
        }
        ++p;
        num = unicode_to_UTF8(decoded, w);

        if (i + num > n)
            break;

        strncpy(s + i, decoded, num);
        i += num;
    }
    return i;
}

// NONMATCHING (95.62%): every other instruction matches; the original funnels
// all returns through r30 (`li r30,-1`, then `mr r30,r3`/`mr r3,r30`), the
// compiler emits `li r3,-1` and returns r3.
int mbtowc(wchar_t* pwc, const char* s, size_t n) {
    int result = 0;
    int len;
    int check;
    const char* p;

    if (!s)
        return result;
    if (n == 0) {
        result = -1;
        return result;
    }

    len = is_utf8_complete(s, n);
    if (len < 0) {
        result = -1;
        return result;
    }

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
        return result;
    }

    if (pwc)
        *pwc = result;
    result = len;
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
