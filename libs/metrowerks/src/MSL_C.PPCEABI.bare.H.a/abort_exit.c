#include "libc/stdlib.h"

void _ExitProcess();
void __destroy_global_chain();
void __kill_critical_regions();
void __begin_critical_region(int region);
void __end_critical_region(int region);

extern void (*_dtors[])(void);

int __aborting = 0;

void (*__atexit_funcs[64])(void);
int __atexit_curr_func = 0;

void (*__stdio_exit)(void) = 0;
void (*__console_exit)(void) = 0;

void exit(int status) {
    int i;
    void (**dtor)(void);

    if (!__aborting) {
        __begin_critical_region(0);
        __end_critical_region(0);

        __destroy_global_chain();

        dtor = _dtors;
        while (*dtor != NULL) {
            (*dtor)();
            dtor++;
        }

        if (__stdio_exit != NULL) {
            __stdio_exit();
            __stdio_exit = NULL;
        }
    }

    __begin_critical_region(0);
    while (__atexit_curr_func > 0)
        __atexit_funcs[--__atexit_curr_func]();
    __end_critical_region(0);
    __kill_critical_regions();

    if (__console_exit != NULL) {
        __console_exit();
        __console_exit = NULL;
    }

    _ExitProcess();
}
