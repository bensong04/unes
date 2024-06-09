/**
 * @file
 * @brief
 *
 * @author Benedict Song
 */
#ifdef DEBUG
#include <inttypes.h>
#include <stdio.h>
#elifdef CPU_TESTS
#include <inttypes.h>
#include <stdio.h>
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