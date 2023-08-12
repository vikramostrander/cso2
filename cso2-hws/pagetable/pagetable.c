#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "mlpt.h"
#include "config.h"

size_t ptbr = (size_t)NULL;
int error;

void divide(size_t va, size_t pn_arr[LEVELS]) {

    int total_size = LEVELS * (POBITS - 3);
    if ((64 - POBITS) < total_size) {
        total_size = (64 - POBITS) / LEVELS;
    }
    int size = total_size / LEVELS;

    va <<= 64 - total_size - POBITS;
    va >>= 64 - total_size;

    //  SPLITTING VPN INTO PAGE NUMBERS
    size_t tmp;
    for (int i = 0; i < LEVELS; ++i) {
        tmp = va;
        tmp <<= (i * size) + (64 - total_size);
        tmp >>= (64 - size);
        pn_arr[i] = tmp;
    }
}

size_t translate(size_t va) {

    size_t failed = (size_t)(-1);
    if (ptbr == (size_t)NULL) return failed;

    //  SAVING POBITS
    size_t po = va << (64 - POBITS);
    po >>= (64 - POBITS);

    //  SPLITTING VPN INTO COMPONENTS
    size_t pn_arr[LEVELS];
    divide(va, pn_arr);

    //  TRAVERSING PAGE TABLES
    size_t pte;
    size_t ptr = ptbr;
    for (int i = 0; i < LEVELS; ++i) {
        ptr += pn_arr[i] * 8;
        pte = *(size_t *)ptr;
        if ((pte << 63) >> 63 == 0) return failed;
        pte -= 1;
        ptr = pte;
    }
    return ptr + po;
}

void page_allocate(size_t va) {

    //  INITIALIZING FIRST PAGE TABLE
    if (ptbr == (size_t)NULL) {
        error = posix_memalign((void **)&ptbr, (size_t)(1 << POBITS), (size_t)(1 << POBITS));
        if (error != 0) {
            printf("POSIX_MEMALIGN FAILED\n");
            return;
        }
        memset((void *)ptbr, 0, (size_t)(1 << POBITS));
    }

    //  SPLITTING VPN INTO COMPONENTS
    size_t pn_arr[LEVELS];
    divide(va, pn_arr);

    //  BUILDING PAGE TABLE LEVELS
    size_t pte;
    size_t ptr = ptbr;
    size_t *tmp;
    for (int i = 0; i < LEVELS; ++i) {
        ptr += pn_arr[i] * 8;
        pte = *(size_t *)ptr;
        if ((pte << 63) >> 63 == 1) {
            ptr = pte;
            ptr -= 1;
        }
        else {
            tmp = (size_t *)ptr;
            error = posix_memalign((void **)&ptr, (size_t)(1 << POBITS), (size_t)(1 << POBITS));
            if (error != 0) {
                printf("POSIX_MEMALIGN FAILED\n");
                return;
            }
            memset((void *)ptr, 0, (size_t)(1 << POBITS));

            pte = ptr;
            pte += 1;
            *tmp = pte;
        }
    }
}

void page_deallocate(size_t ptr) {

    //  CHECK IF A PAGE EXISTS
    if (ptr == (size_t)NULL) {
        return;
    }

    //  FIND PAGES WITHIN CURRENT PAGETABLE
    size_t pte;
    for (int i = 1; i <= (1 << (POBITS - 3)); ++i) {
        pte = *(size_t *)(ptr + (i * 8));
        if((pte & 1) == 1)  {
            page_deallocate(pte - 1);
        }
    }

    //  CLEAR AND FREE PAGE
    memset((void *)ptr, 0, (size_t)(1 << POBITS));
    free((void *)ptr);
}