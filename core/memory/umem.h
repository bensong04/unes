/**
 * @file umem.h
 * @brief
 *
 * @author Benedict Song
 */
#pragma once
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>

#include "ppu/uppu.h"

/*
 * The NES possesses 256 pages X 256 bytes of memory.
 */
#define UNES_MEM_CAP (256 * 256)

/*
 * In real life, the NES CPU has access to about 2 KB of memory.
 */
#define UCPU_MEM_CAP (2048)

/*
 */
#define UCPU_MIRROR_RANGE 0x2000u

#define UCPU_PAGE_SZ 0x100u

// TODO: move to mappers and generalize
#define CART_ROM_START 0x8000u
#define MAPPER_0_RANGE 0x4000u

/*
 */
#define UCPU_PPU_REG_RANGE 0x4000u

/*
 * Macro for Tom Harte CPU unit tests.
 */
#define TH_UNITTEST_MEM_CAP (1024 * 64)

/*
 * The NES's memory will be filled with UNILs when it is initialized.
 */
#define UNIL 0x00

/*
 * As the 6502 processor supports a 16-bit memory space,
 * the special program counter (PC) register holds 16 bits.
 */
typedef uint16_t uaddr_t;  // Memory address type/PC register type

/*
 * Many components of the NES support an 8-flag "status" group
 * or latch. They can be considered as being packed into a single
 * 8 bit "register," of sorts.
 */
typedef uint8_t ustat_t;

/*
 * Represents a null pointer to the address bus.
 * True for the CPU, PPU, APU, etc.
 */
#define NULLPTR ((uaddr_t)0)

/*
 * The NES's memory shall be represented as an array of
 * UCPU_MEM_CAP bytes.
 */
typedef uint8_t byte_t;  // Typedef for the sake of readability
typedef byte_t *ram_t;   // Ditto

ram_t alloc_ram(size_t how_much);