/**
 * @file umem.h
 * @brief
 * 
 * @author Benedict Song
 */
#ifndef _UMEM_INCLUDED
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>

/*
 * The NES possesses 256 pages X 256 bytes of memory.
 */
#define UNES_MEM_CAP (256 * 256)

/*
 * In real life, the NES CPU has access to about 2 KB of memory.
 */
#define UCPU_MEM_CAP (2048)

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
 * Represents a null pointer to the address bus.
 * True for the CPU, PPU, APU, etc.
 */
#define NULLPTR ((uaddr_t) 0)

/*
 * The NES's memory shall be represented as an array of
 * UCPU_MEM_CAP bytes.
 */
typedef uint8_t byte_t; // Typedef for the sake of readability
typedef byte_t *ram_t; // Ditto

/*
 * The "bus" itself. Exists as a way to programmatically enable connections
 * between different devices, including exposing PPU registers to the CPU,
 * enabling hardware interrupts, etc.
 */
typedef struct bus {
    ram_t cpu_ram;
    ram_t ppu_ram; // will add more later
} bus_t;

/*
 * Depending on who's reading or writing, the behavior of the bus may change.
 */
typedef enum device {
    DEV_CPU,
    DEV_PPU,
    DEV_APU,
    DEV_ROM
} device_t;

/*
 * Represents a device-specific "link" onto the bus.
 * Depending on this link, byte setting and getting may produce different behavior.
 */
typedef struct buslink {
    device_t device;
    bus_t *bus;
} buslink_t;

bus_t new_bus();

void link(buslink_t *link, bus_t *bus);

void set_byte(buslink_t link, uaddr_t which, byte_t what);
byte_t get_byte(buslink_t link, uaddr_t which);

#define _UMEM_INCLUDED
#endif
