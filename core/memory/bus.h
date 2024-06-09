#pragma once
#include "memory/umem.h"

/*
 * The "bus" itself. Exists as a way to programmatically enable connections
 * between different devices, including exposing PPU registers to the CPU,
 * enabling hardware interrupts, etc.
 */
typedef struct bus {
  ram_t cpu_ram;
  ram_t ppu_ram;  // will add more later
  void *cpu;
  void *ppu;
  byte_t *cartridge;  // assume mapper 0 for now
  ustat_t p_latch;
} bus_t;

/*
 * Depending on who's reading or writing, the behavior of the bus may change.
 */
typedef enum device { DEV_CPU, DEV_PPU, DEV_APU, DEV_ROM } device_t;

/*
 * Represents a device-specific "link" onto the bus.
 * Depending on this link, byte setting and getting may produce different
 * behavior.
 */
typedef struct buslink {
  device_t device;
  bus_t *bus;
} buslink_t;

bus_t new_bus();

void link_device(buslink_t *link, bus_t *bus);

void set_byte(buslink_t link, uaddr_t which, byte_t what);
byte_t get_byte(buslink_t link, uaddr_t which);