#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "tlb.h"

struct block {
    int valid;
    size_t tag;
    size_t ppn;
};

struct set {
    struct block blocks[4];
    int lru[4];
};

struct set cache[16];

//size_t translate(size_t va) { return va < 0x1234000 ? va + 0x20000 : -1; }

void tlb_clear() {
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 4; ++j) {
            cache[i].blocks[j].valid = 0;
            cache[i].lru[j] = 4 - j;
        }
    }
}

// DOES THIS NEED TO UPDATE LRU ?
int tlb_peek(size_t va) {
    size_t vpn = va >> POBITS;
    size_t index = vpn & 0xF;
    size_t tag = vpn >> 4;

    for (int i = 0; i < 4; ++i) {
        if (cache[index].blocks[i].valid == 1 && cache[index].blocks[i].tag == tag) {
            return cache[index].lru[i];
        }
    }

    return 0;
}

size_t tlb_translate(size_t va) {
    size_t offset = va & ((1 << POBITS) - 1);
    size_t vpn = va >> POBITS;
    size_t index = vpn & 0xF;
    size_t tag = vpn >> 4;

    int lru;

    for (int i = 0; i < 4; ++i) {
        if (cache[index].blocks[i].valid == 1 && cache[index].blocks[i].tag == tag) {

            lru = cache[index].lru[i];
            for (int j = 0; j < 4; ++j) {
                if (j == i) {
                    cache[index].lru[j] = 1;
                }
                else if (cache[index].lru[j] < lru) {
                    cache[index].lru[j] += 1;
                }
            }

            // RETURN ADDR
            return cache[index].blocks[i].ppn + offset;
        }
    }

    va = (va >> POBITS) << POBITS;
    size_t ppn = translate(va);

    if (ppn == -1) return -1;

    for (int i = 0; i < 4; ++i) {
        if(cache[index].lru[i] == 4) lru = i + 1;
    }

    cache[index].blocks[lru - 1].valid = 1;
    cache[index].blocks[lru - 1].tag = tag;
    cache[index].blocks[lru - 1].ppn = ppn;

    for (int i = 0; i < 4; ++i) {
        if (i == lru - 1) cache[index].lru[i] = 1;
        else cache[index].lru[i] += 1;
    }

    return ppn + offset;
}

/*int main() {
    tlb_clear();
    assert(tlb_peek(0) == 0);
    assert(tlb_translate(0) == 0x20000);
    assert(tlb_peek(0) == 1);
    assert(tlb_translate(0x200) == 0x20200);
    assert(tlb_peek(0) == 1);
    assert(tlb_peek(0x200) == 1);
    assert(tlb_translate(0x1200) == 0x21200);
    assert(tlb_translate(0x5200) == 0x25200);
    assert(tlb_translate(0x8200) == 0x28200);
    assert(tlb_translate(0x2200) == 0x22200);
    assert(tlb_peek(0x1000) == 1);
    assert(tlb_peek(0x5000) == 1);
    assert(tlb_peek(0x8000) == 1);
    assert(tlb_peek(0x2000) == 1);
    assert(tlb_peek(0x0000) == 1);
    assert(tlb_translate(0x101200) == 0x121200);
    assert(tlb_translate(0x801200) == 0x821200);
    assert(tlb_translate(0x301200) == 0x321200);
    assert(tlb_translate(0x501200) == 0x521200);
    assert(tlb_translate(0xA01200) == 0xA21200);
    assert(tlb_translate(0xA0001200) == -1);
    assert(tlb_peek(0x001200) == 0);
    assert(tlb_peek(0x101200) == 0);
    assert(tlb_peek(0x301200) == 3);
    assert(tlb_peek(0x501200) == 2);
    assert(tlb_peek(0x801200) == 4);
    assert(tlb_peek(0xA01200) == 1);
    assert(tlb_translate(0x301800) == 0x321800);
    assert(tlb_peek(0x001000) == 0);
    assert(tlb_peek(0x101000) == 0);
    assert(tlb_peek(0x301000) == 1);
    assert(tlb_peek(0x501000) == 3);
    assert(tlb_peek(0x801000) == 4);
    assert(tlb_peek(0xA01000) == 2);
}*/