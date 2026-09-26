#pragma once

// Code that exists only to reproduce the original bytes (a fake cast or temporary, an odd expression order)
// goes inside #ifdef MUST_MATCH, with the natural form under #else where there is one. configure.py defines
// MUST_MATCH for matching builds; --non-matching builds leave it out, so the natural code is what they compile.

// Reserve stack space the original function has but our code doesn't otherwise use.
#define PAD_STACK(bytes)                \
    do {                                \
        unsigned char _pad[(bytes)];    \
        (void)_pad;                     \
    } while (0)
