/**
 * @file umem.h
 * @brief
 * 
 * @author Benedict Song
 */
#ifndef _UMEM_INCLUDED
#include <stdint.h>
#include <unistd.h>

/*
 * The NES possesses 256 pages X 256 bytes of memory.
 */
#define UNES_MEM_CAP (256 * 256)

/*
 * The NES's memory will be filled with UNILs when it is initialized.
 */
#define UNIL 0x00

/*
 * As the 6502 processor supports a 16-bit memory space,
 * the special program counter (PC) register holds 16 bits.
 */
typedef uint16_t uaddr_t; // Memory address type/PC register type

/*
 * The NES's memory shall be represented as an array of
 * UCPU_MEM_CAP bytes.
 */
typedef uint8_t byte_t; // Typedef for the sake of readability
typedef byte_t *ram_t; // Ditto

ram_t alloc_ram(size_t how_much);

void set_byte(uaddr_t which, byte_t what);

void *get_actual_addr(ram_t ram, uaddr_t *emulated_addr);

#define _UMEM_INCLUDED
#endif