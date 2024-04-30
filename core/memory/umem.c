/**
 * @file
 * @brief
 * 
 * @author Benedict Song
 */
#ifdef DEBUG
#include <stdio.h>
#include <inttypes.h>
#endif DEBUG
#include "memory/umem.h"

ram_t alloc_ram(size_t how_much) {
    // Map how_much bytes to the emulating process.
    // The extra heap space can then be used to emulate RAM.
    // Note that MAP_ANON zero-initializes everything.
    ram_t where = mmap(NULL, how_much, PROT_WRITE | PROT_READ, 
                        MAP_PRIVATE | MAP_ANON, -1, 0); 

    return where;
}

void set_byte(ram_t ram, uaddr_t which, byte_t what) {
    // Placeholder implementation without mirroring and all that bullshit.
#ifdef DEBUG
    printf("Address %" PRIu16 " set to %" PRIu8 ".\n");
#endif
    ram[which] = what;
}

void *get_actual_addr(ram_t ram, uaddr_t emulated_addr) {
    return (void *) (ram + emulated_addr); // should work?
}