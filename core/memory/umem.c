/**
 * @file
 * @brief
 * 
 * @author Benedict Song
*/
#include "memory/umem.h"

ram_t alloc_ram(size_t how_much) {
    // Get the current program break
    ram_t where = sbrk(0);

    // Extend the program break by how_much bytes
    // The extra heap space can then be used to emulate RAM.
    sbrk(how_much);
    
    // Zero-initialize memory
    memset(where, UNIL, how_much);

    return where;
}

void set_byte(ram_t ram, uaddr_t which, byte_t what) {
    // Placeholder implementation without mirroring and all that bullshit.
    ram[which] = what;
}

void *get_actual_addr(ram_t ram, uaddr_t emulated_addr) {
    return (void *) (ram + emulated_addr); // should work?
}