/**
 * @file
 * @brief
 *
 * @author Benedict Song
 */
#ifdef DEBUG
#include <stdio.h>
#include <inttypes.h>
#elifdef CPU_TESTS
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
 * @brief
 *
 * @note This should be a singleton.
 */
bus_t new_bus() {
	bus_t bus = { NULL, NULL };
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
void link_device(buslink_t *link, bus_t *bus) {
    link->bus = bus;
}

/**
 * @brief Sets the byte at *emulated* address `which` to what.
 */
void set_byte(buslink_t link, uaddr_t which, byte_t what) {
    bus_t *bus = link.bus;
    switch (link.device) {
        case DEV_CPU: {
#ifdef CPU_TESTS
            printf("Address %" PRIu16 " set to %" PRIu8 ".\n",
                    which, what);
#endif
            bus->cpu_ram[which] = what;
            break;
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
#ifdef CPU_TESTS
            printf("Read %" PRIu8 " at address %" PRIu16 ".\n",
                    bus->cpu_ram[which], which);
#endif
            return bus->cpu_ram[which];
            break;
        }
    }
}
