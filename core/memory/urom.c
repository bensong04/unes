/**
 * @file
 * @brief
 *
 * @author Benedict Song <benedict04song@gmail.com>
 */
#include "memory/urom.h"

#include <fcntl.h>
#include <string.h>

urom_t new_rom(const char *filepath, size_t rom_size) {
  urom_t rom = {0};
  rom.filepath = strdup(filepath);
  rom.locale = NULL;
  rom.rom_size = rom_size;
  return rom;
}

void mount(urom_t *rom) {
  int romfd = open(rom->filepath, O_RDONLY);
  rom->locale =
      mmap(NULL, rom->rom_size, PROT_WRITE | PROT_READ, MAP_PRIVATE, romfd, 0);
  close(romfd);
}

void unmount(urom_t *rom) { munmap(rom->locale, rom->rom_size); }