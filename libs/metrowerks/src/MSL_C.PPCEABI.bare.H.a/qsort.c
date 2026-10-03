#include "stdlib.h"

/* Heap sort. */
// NONMATCHING (95.27%): calls and immediates identical; only the callee-saved register numbering differs (the
// original keeps compare/size/base in r31/r30/r29 and the indices in r28..r26, ours puts the pointers above them).
void qsort(void* base_, size_t nmemb, size_t size, int (*compare)(const void*, const void*)) {
    char* base = base_;
    char* ip;
    char* last;
    char* parent;
    char* child;
    char* child2;
    char* q;
    char* p;
    char tmp;
    size_t i, j, n, k;

    if (nmemb < 2) {
        return;
    }
    i = (nmemb >> 1) + 1;
    n = nmemb;
    ip = base + size * (i - 1);
    last = base + size * (n - 1);
    for (;;) {
        if (i > 1) {
            ip -= size;
            i--;
        } else {
            q = last - 1;
            p = ip - 1;
            for (k = size + 1; --k;) {
                tmp = *++p;
                *p = *++q;
                *q = tmp;
            }
            if (--n == 1) {
                return;
            }
            last -= size;
        }
        j = i;
        child = base + size * (j - 1);
        while (j * 2 <= n) {
            j <<= 1;
            parent = child;
            child = base + size * (j - 1);
            if (j < n) {
                child2 = child + size;
                if (compare(child, child2) < 0) {
                    child = child2;
                    j++;
                }
            }
            if (compare(parent, child) >= 0) {
                break;
            }
            q = child - 1;
            p = parent - 1;
            for (k = size + 1; --k;) {
                tmp = *++p;
                *p = *++q;
                *q = tmp;
            }
        }
    }
}
