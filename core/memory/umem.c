/**
 * @file
 * @brief
 * 
 * @author Benedict Song
 */
#ifdef DEBUG
#include <stdio.h>
#include <inttypes.h>
#endif
#include "memory/umem.h"

ram_t alloc_ram(size_t how_much) {
    // Map how_much bytes to the emulating process.
    // The extra heap space can then be used to emulate RAM.
    // Note that MAP_ANON zero-initializes everything.
    ram_t where = mmap(NULL, how_much, PROT_WRITE | PROT_READ, 
                        MAP_PRIVATE | MAP_ANON, -1, 0); 

    return where;
}

/**
 * @brief Sets the byte at *emulated* address `which` to what.
 */
void set_byte(ram_t ram, uaddr_t which, byte_t what) {
    // Placeholder implementation without mirroring and all that bullshit.
#ifdef DEBUG
    printf("Address %" PRIu16 " set to %" PRIu8 ".\n", which, what);
#endif
    ram[which] = what;
}

/**
 * @brief Sets the byte at the *actual* address `which` to what.
 * 
 * The advantage of using `sets` over pure dereferencing is that
 * this function wraps `set_byte`, which performs bookkeeping,
 * memory mirroring, etc.
 */
void sets(ram_t ram, void *actual, byte_t what) {
    set_byte(ram, get_emul_addr(ram, actual), what);
}

uaddr_t get_emul_addr(ram_t ram, void *actual_addr) {
    return (uaddr_t) ((size_t) actual_addr - (size_t) ram);
}

void *get_actual_addr(ram_t ram, uaddr_t emulated_addr) {
    return (void *) (ram + emulated_addr); // should work?
}