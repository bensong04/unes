/**
 * @file urom.h
 * @brief
 *
 * @author Benedict Song <benedict04song@gmail.com>
 */
#ifndef _UROM_INCLUDED

#include <stdlib.h>

#include "memory/umem.h"

#define STD_ROM_SIZE (256 * 1024)
#define MAX_ROM_SIZE (512 * 1024)

typedef struct urom {
  const char *filepath;
  size_t rom_size;
  byte_t *locale;
} urom_t;

urom_t new_rom(const char *filepath, size_t rom_size);

void mount(urom_t *rom);
void unmount(urom_t *rom);

#define _UROM_INCLUDED
#endif