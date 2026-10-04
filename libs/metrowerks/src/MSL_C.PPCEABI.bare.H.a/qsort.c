#include "stdlib.h"

/* Heap sort. */
void qsort(void* base, size_t nmemb, size_t size, int (*compare)(const void*, const void*)) {
    size_t i;
    size_t n;
    size_t j;
    char* ip;
    char* last;
    char* parent;
    char* child;
    char* child2;

    if (nmemb < 2) {
        return;
    }
    i = (nmemb >> 1) + 1;
    n = nmemb;
    ip = (char*)base + size * (i - 1);
    last = (char*)base + size * (n - 1);
    for (;;) {
        if (i > 1) {
            ip -= size;
            i--;
        } else {
            char* q = last - 1;
            char* p = ip - 1;
            size_t k;
            int tmp;

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
        child = (char*)base + size * (j - 1);
        while (j * 2 <= n) {
            j <<= 1;
            parent = child;
            child = (char*)base + size * (j - 1);
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
            {
                char* q = child - 1;
                char* p = parent - 1;
                size_t k;
                int tmp;

                for (k = size + 1; --k;) {
                    tmp = *++p;
                    *p = *++q;
                    *q = tmp;
                }
            }
        }
    }
}
