#include "MSL_C/scanf.h"
#include "MSL_C/ansi_fp.h"
#include "MSL_C/MSL_Common/float.h"
#include "MSL_C/ctype.h"
#include "limits.h"
#include "errno.h"
#include "MSL_C/MSL_Common_Embedded/Math/fdlibm.h"

extern double __dec2num(const decimal* d);
extern const char str_803025CC[12];

extern int __extended_min[];
extern int __extended_max[];

#define TARGET_FLOAT_BITS 64
#define TARGET_FLOAT_BYTES (TARGET_FLOAT_BITS / 8)
#define TARGET_FLOAT_MAX_EXP LDBL_MAX_EXP
#define TARGET_FLOAT_MANT_DIG LDBL_MANT_DIG
#define TARGET_FLOAT_IMPLICIT_J_BIT 1
#define TARGET_FLOAT_MANT_BITS (TARGET_FLOAT_MANT_DIG - TARGET_FLOAT_IMPLICIT_J_BIT)
#define TARGET_FLOAT_EXP_BITS (TARGET_FLOAT_BITS - TARGET_FLOAT_MANT_BITS - 1)

enum scan_states {
    start = 0x0001,
    sig_start = 0x0002,
    leading_sig_zeroes = 0x0004,
    int_digit_loop = 0x0008,
    frac_start = 0x0010,
    frac_digit_loop = 0x0020,
    sig_end = 0x0040,
    exp_start = 0x0080,
    leading_exp_digit = 0x0100,
    leading_exp_zeroes = 0x0200,
    exp_digit_loop = 0x0400,
    finished = 0x0800,
    failure = 0x1000,
    nan_state = 0x2000,
    infin_state = 0x4000,
    hex_state = 0x8000
};

enum hex_scan_states {
    not_hex = 0x0000,
    hex_start = 0x0001,
    hex_leading_sig_zeroes = 0x0002,
    hex_int_digit_loop = 0x0004,
    hex_frac_digit_loop = 0x0008,
    hex_sig_end = 0x0010,
    hex_exp_start = 0x0020,
    hex_leading_exp_digit = 0x0040,
    hex_leading_exp_zeroes = 0x0080,
    hex_exp_digit_loop = 0x0100
};

#define final_state(scan_state) (scan_state & (finished | failure))
#define success(scan_state) \
    (scan_state &           \
     (leading_sig_zeroes | int_digit_loop | frac_digit_loop | leading_exp_zeroes | exp_digit_loop | finished))
#define hex_success(count, scan_state)                                                                  \
    (count - 1 > 2 && scan_state & (hex_leading_sig_zeroes | hex_int_digit_loop | hex_frac_digit_loop | \
                                    hex_leading_exp_zeroes | hex_exp_digit_loop))

#define fetch() (count++, (*ReadProc)(ReadProcArg, 0, __GetAChar))
#define unfetch(c) (*ReadProc)(ReadProcArg, c, __UngetAChar)

// The block-scoped `int ui` in the hex digit loops keeps the `_toupper` result (r3) apart from the
// converted digit (r5). The `num` union keeps `result`/`mantissa` in the original's aliased 0x18(r1) slot.
long double __strtold(int max_width, int (*ReadProc)(void*, int, int), void* ReadProcArg, int* chars_scanned,
                      int* overflow) {
    int scan_state = start;
    int hex_scan_state = not_hex;
    int count = 0;
    int spaces = 0;
    int c;
    decimal d = {0, 0, 0, {0, ""}};
    int sig_negative = 0;
    int exp_negative = 0;
    long exp_value = 0;
    int exp_adjust = 0;
    int sign_detected = 0;
    int expsign = 0;

    union {
        long double result;
        unsigned char mantissa[TARGET_FLOAT_BYTES];
    } num;
#define result num.result
#define mantissa num.mantissa
    unsigned char* chptr = mantissa;
    int NibbleIndex;
    int exp_digits = 0;
    unsigned intdigits = 0;
    short exponent = 0;
    int radix_marker = *(unsigned char*)(__lconv).decimal_point;

    unsigned mantissa_digits;
    unsigned char uch, uch1;

    *overflow = 0;
    c = fetch();

    while (count <= max_width && c != -1 && !final_state(scan_state)) {
        switch (scan_state) {
            case start:
                if (_isspace(c)) {
                    c = fetch();
                    count--;
                    spaces++;
                    break;
                }

                switch (_toupper(c)) {
                    case '-':
                        sig_negative = 1;

                    case '+':
                        c = fetch();
                        sign_detected = 1;
                        break;
                    case 'I':
                        c = fetch();
                        scan_state = infin_state;
                        break;

                    case 'N':
                        c = fetch();
                        scan_state = nan_state;
                        break;

                    default:
                        scan_state = sig_start;
                        break;
                }
                break;

            case infin_state: {
                int i = 1;
                // Copy through the named object so it stays a 12-byte entry in .rodata.
                typedef struct { char b[9]; } model_t;
                char model[9];
                *(model_t*)model = *(model_t*)str_803025CC;

                while ((i < 8) && (_toupper(c) == model[i])) {
                    i++;
                    c = fetch();
                }

                if ((i == 3) || (i == 8)) {
                    if (sig_negative) {
                        result = (float)-HUGE_VALF;
                    } else {
                        result = HUGE_VALF;
                    }

                    *chars_scanned = spaces + i + sign_detected;
                    return result;
                } else {
                    scan_state = failure;
                }

                break;
            }

            case nan_state: {
                int i = 1, j = 0;
                char model[] = "NAN(";
                char nan_arg[32]; // write-only, so the original leaves it uninitialised
                while ((i < 4) && (_toupper(c) == model[i])) {
                    i++;
                    c = fetch();
                }

                if ((i == 3) || (i == 4)) {
                    if (i == 4) {
                        while ((j < 32) && (_isdigit(c) || _isalpha(c))) {
                            nan_arg[j++] = (char)c;
                            c = fetch();
                        }

                        if (c != ')') {
                            scan_state = failure;
                            break;
                        } else {
                            j++;
                        }
                    }
                    nan_arg[j] = '\0';

                    if (sig_negative) {
                        result = (float)-NAN;
                    } else {
                        result = NAN;
                    }

                    *chars_scanned = spaces + i + j + sign_detected;
                    return result;
                } else {
                    scan_state = failure;
                }
                break;
            }

            case sig_start:
                if (c == radix_marker) {
                    scan_state = frac_start;
                    c = fetch();
                    break;
                }
                if (!_isdigit(c)) {
                    scan_state = failure;
                    break;
                }

                if (c == '0') {
                    c = fetch();
                    if (_toupper(c) == 'X') {
                        scan_state = hex_state;
                        hex_scan_state = hex_start;
                    } else {
                        scan_state = leading_sig_zeroes;
                    }
                    break;
                }

                scan_state = int_digit_loop;
                break;

            case leading_sig_zeroes:
                if (c == '0') {
                    c = fetch();

                    break;
                }
                scan_state = int_digit_loop;
                break;

            case int_digit_loop:
                if (!_isdigit(c)) {
                    if (c == radix_marker) {
                        scan_state = frac_digit_loop;
                        c = fetch();
                    } else {
                        scan_state = sig_end;
                    }
                    break;
                }
                if (d.sig.length < 20) {
                    d.sig.text[d.sig.length++] = (unsigned char)c;
                } else {
                    exp_adjust++;
                }

                c = fetch();
                break;

            case frac_start:
                if (!_isdigit(c)) {
                    scan_state = failure;
                    break;
                }

                scan_state = frac_digit_loop;
                break;

            case frac_digit_loop:
                if (!_isdigit(c)) {
                    scan_state = sig_end;
                    break;
                }

                if (d.sig.length < 20) {
                    if (c != '0' || d.sig.length) {
                        d.sig.text[d.sig.length++] = (unsigned char)c;
                    }

                    exp_adjust--;
                }
                c = fetch();
                break;

            case sig_end:
                if (_toupper(c) == 'E') {
                    scan_state = exp_start;
                    c = fetch();
                    break;
                }
                scan_state = finished;
                break;

            case exp_start:
                if (c == '+') {
                    c = fetch();
                } else if (c == '-') {
                    c = fetch();
                    exp_negative = 1;
                }

                scan_state = leading_exp_digit;
                break;

            case leading_exp_digit:
                if (!_isdigit(c)) {
                    scan_state = failure;
                    break;
                }

                if (c == '0') {
                    scan_state = leading_exp_zeroes;
                    c = fetch();
                    break;
                }

                scan_state = exp_digit_loop;
                break;

            case leading_exp_zeroes:
                if (c == '0') {
                    c = fetch();
                    break;
                }

                scan_state = exp_digit_loop;
                break;

            case exp_digit_loop:
                if (!_isdigit(c)) {
                    scan_state = finished;
                    break;
                }

                exp_value = exp_value * 10 + (c - '0');
                if (exp_value > SHRT_MAX) {
                    *overflow = 1;
                }

                c = fetch();
                break;

            case hex_state: {
                switch (hex_scan_state) {
                    case hex_start:
                        mantissa[0] = 0;
                        mantissa[1] = 0;
                        mantissa[2] = 0;
                        mantissa[3] = 0;
                        mantissa[4] = 0;
                        mantissa[5] = 0;
                        mantissa[6] = 0;
                        mantissa[7] = 0;
                        NibbleIndex = 2;
                        hex_scan_state = hex_leading_sig_zeroes;
                        c = fetch();
                        break;

                    case hex_leading_sig_zeroes:
                        if (c == '0') {
                            c = fetch();
                        } else {
                            hex_scan_state = hex_int_digit_loop;
                        }
                        break;

                    case hex_int_digit_loop:
                        if (!_isxdigit(c)) {
                            if (c == radix_marker) {
                                hex_scan_state = hex_frac_digit_loop;
                                c = fetch();
                            } else {
                                hex_scan_state = hex_sig_end;
                            }
                        } else if (NibbleIndex < 17) {
                            intdigits++;
                            uch = *(chptr + NibbleIndex / 2);
                            {
                                int ui = _toupper(c);
                                if (ui >= 'A') {
                                    ui = ui - 'A' + 10;
                                } else {
                                    ui -= '0';
                                }
                                uch1 = (unsigned char)ui;
                            }
                            if ((NibbleIndex % 2) != 0) {
                                uch |= uch1;
                            } else {
                                uch |= uch1 << 4;
                            }
                            *(chptr + NibbleIndex++ / 2) = uch;
                            c = fetch();
                        } else {
                            c = fetch();
                        }
                        break;

                    case hex_frac_digit_loop:
                        if (!_isxdigit(c)) {
                            hex_scan_state = hex_sig_end;
                        } else if (NibbleIndex < 17) {
                            uch = *(chptr + NibbleIndex / 2);
                            {
                                int ui = _toupper(c);
                                if (ui >= 'A') {
                                    ui = ui - 'A' + 10;
                                } else {
                                    ui -= '0';
                                }
                                uch1 = (unsigned char)ui;
                            }
                            if ((NibbleIndex % 2) != 0) {
                                uch |= uch1;
                            } else {
                                uch |= uch1 << 4;
                            }
                            *(chptr + NibbleIndex++ / 2) = uch;
                            c = fetch();
                        } else {
                            c = fetch();
                        }
                        break;

                    case hex_sig_end:
                        if (_toupper(c) == 'P') {
                            hex_scan_state = hex_exp_start;
                            exp_digits++;
                            c = fetch();
                        } else {
                            scan_state = finished;
                        }
                        break;

                    case hex_exp_start:
                        exp_digits++;
                        if (c == '-') {
                            expsign = 1;
                        } else if (c != '+') {
                            c = unfetch(c);
                            exp_digits--;
                        }
                        hex_scan_state = hex_leading_exp_digit;
                        c = fetch();
                        break;

                    case hex_leading_exp_digit:
                        if (!_isdigit(c)) {
                            scan_state = failure;
                        } else if (c == '0') {
                            hex_scan_state = hex_leading_exp_zeroes;
                            exp_digits++;
                            c = fetch();
                        } else {
                            hex_scan_state = hex_exp_digit_loop;
                        }
                        break;

                    case hex_exp_digit_loop:
                        if (!_isdigit(c)) {
                            scan_state = finished;
                        } else {
                            exponent = exponent * 10 + (c - '0');
                            if (exp_value > SHRT_MAX) {
                                *overflow = 1;
                            }
                            exp_digits++;
                            c = fetch();
                        }
                        break;
                }
            } break;
        }
    }

    if (!success(scan_state)) {
        *chars_scanned = 0;
    } else {
        *chars_scanned = count - 1 + spaces;
    }

    unfetch(c);

    if (hex_scan_state == not_hex) {
        if (exp_negative) {
            exp_value = -exp_value;
        }

        {
            int n = d.sig.length;
            unsigned char* p = &d.sig.text[n];

            while (n-- && *--p == '0') {
                exp_adjust++;
            }

            d.sig.length = (unsigned char)(n + 1);

            if (d.sig.length == 0) {
                d.sig.text[d.sig.length++] = '0';
            }
        }

        exp_value += exp_adjust;

        if (exp_value < -0x8000 || exp_value > 0x7FFF) {
            *overflow = 1;
        }

        if (*overflow) {
            if (exp_negative) {
                return 0.0;
            }
            if (sig_negative) {
                return -*(double*)__double_huge;
            }
            return *(double*)__double_huge;
        }

        d.exp = (short)exp_value;

        result = __dec2num(&d);

        if (result != 0.0 && result < *(double*)__extended_min) {
            *overflow = 1;
        } else if (result > *(double*)__extended_max) {
            *overflow = 1;
            result = *(double*)__double_huge;
        }

        if (sig_negative && success(scan_state)) {
            result = -result;
        }

        return result;
    } else {
        long double* dbl = (long double*)mantissa;

        if (*dbl) {
            if (expsign) {
                exponent = -exponent;
            }

            while ((*(short*)mantissa & 0xF0) != 0x10) {
                exponent++;
                *(unsigned long long*)mantissa >>= 1;
            }

            exponent += (intdigits - 1) * 4;
            *(short*)mantissa &= 0xF;
            *(short*)mantissa |= (exponent + 0x3FF) << 4;
            *chars_scanned = spaces + sign_detected + NibbleIndex + 1 + exp_digits;

            if (*dbl != 0.0 && *dbl < *(double*)__extended_min) {
                *dbl = 0.0;
                *overflow = 1;
            } else if (*dbl > *(double*)__extended_max) {
                *overflow = 1;
                *dbl = *(double*)__double_huge;
            }

            if (sig_negative) {
                *(short*)mantissa |= 0x8000;
            }
        } else {
            *dbl = 0.0;
        }

        return *dbl;
    }
}
#undef result
#undef mantissa

// The "INFINITY" string literal, emitted as a 12-byte .rodata object.
#pragma force_active on
const __declspec(section ".rodata") char str_803025CC[12] = "INFINITY";
#pragma force_active reset
