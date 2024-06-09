#include "memory/bus.h"

#include <inttypes.h>
#include <stdio.h>

#include "memory/umem.h"

/**
 * @brief
 *
 * @note This should be a singleton.
 */
bus_t new_bus() {
  bus_t bus = {NULL, NULL};
#ifdef CPU_TESTS
  bus.cpu_ram = alloc_ram(TH_UNITTEST_MEM_CAP);
#endif
  // not implemented
  return bus;
}

/**
 * @brief Connects a device to the bus.
 *
 * A device should pass in a reference to its buslink, as well as
 * the (singleton) bus to which it wants to link itself.
 */
void link_device(buslink_t *link, bus_t *bus) { link->bus = bus; }

/**
 * @brief Sets the byte at *emulated* address `which` to what.
 */
void set_byte(buslink_t link, uaddr_t which, byte_t what) {
  bus_t *bus = link.bus;
  switch (link.device) {
    case DEV_CPU: {
#ifdef DEBUG
      printf("Address %" PRIu16 " set to %" PRIu8 ".\n", which, what);
#endif
#ifdef CPU_TESTS
      bus->cpu_ram[which] = what;
#else
      if (which < UCPU_MIRROR_RANGE) {
        bus->cpu_ram[which % UCPU_MEM_CAP] = what;
      } else if (which < UCPU_PPU_REG_RANGE) {
        uppu_t *ppu = bus->ppu;

        // writing anything to the PPU registers (even to STATUS)
        // fills the 8-bit latch. when reading a write-only
        // register, the value of the latch is returned.
        // latch delay is unimplemented.
        bus->p_latch = what;

        // handle mirroring
        uaddr_t request = (which % 8) + PPU_CTRL_ADDR;

        switch (request) {
          case PPU_CTRL_ADDR: {
            ppu->PPU_CTRL = (ustat_t)what;
            break;
          }

          case PPU_MASK_ADDR: {
            ppu->PPU_MASK = (ustat_t)what;
            break;
          }

          case PPU_OAMA_ADDR: {
            ppu->OAM_ADDR = what;
            break;
          }

          case PPU_OAMD_ADDR: {
            ppu->OAM_DATA = what;
            break;
          }

          case PPU_SCRL_ADDR: {
            ppu->PPU_SCRL = what;
            // toggle the write latch
            ppu->w = !ppu->w;
            break;
          }

          case PPU_ADDR_ADDR: {
            ppu->PPU_ADDR = what;
            // toggle the write latch
            ppu->w = !ppu->w;
            break;
          }

          case PPU_DATA_ADDR: {
            ppu->OAM_DATA = what;
            break;
          }

          case OAM_DMA_ADDR: {
            // unimplemented for now...
            break;
          }

          default:  // do nothing
        }
      } else {
      }

      break;
#endif
    }
  }
}

/**
 * @brief Gets the byte at *emulated* address `which`.
 */
byte_t get_byte(buslink_t link, uaddr_t which) {
  bus_t *bus = link.bus;
  switch (link.device) {
    case DEV_CPU: {
#ifdef DEBUG
      printf("Read %" PRIu8 " at address %" PRIu16 ".\n", bus->cpu_ram[which],
             which);
#endif
#ifdef CPU_TESTS
      if (which > CART_ROM_START) {
        return bus->cartridge[(which - CART_ROM_START) % MAPPER_0_RANGE];
      }
      return bus->cpu_ram[which];
#endif
      if (which < UCPU_MIRROR_RANGE) {
        return bus->cpu_ram[which % UCPU_MEM_CAP];
      } else if (which < UCPU_PPU_REG_RANGE) {
      } else {
      }
      break;
    }
  }
}