/**
 * @file
 * @brief
 *
 * @author Benedict Song <benedict04song@gmail.com>
 */
#include <stdlib.h>
#include <stdio.h>

#include "cpu/ucpu.h"
#include "cpu/uerrno.h"
#include "memory/umem.h"
#include "memory/urom.h"

static const char *USAGE = "Placeholder help text\n";

uerrno_t UERRNO;

int main(int argc, char **argv) {
    // MASSIVE placeholders to follow
    // error handling
    if (argc != 2) {
        perror(USAGE);
        exit(1);
    }

    // setup everything
    ram_t ram = alloc_ram(UNES_MEM_CAP);
    ucpu_t cpu;
    init_cpu(&cpu, ram);
    clk_t master_clock = 0;

    const char *program_path = argv[1];
    urom_t rom = new_rom(program_path, STD_ROM_SIZE);
    mount(&rom);

    while (cpu.PC <= rom.rom_size) {
        master_clock++;
        if (step(&cpu, rom.locale) == 1) {
            break;
        }
    }
    printf("OUTPUT: %s", (const char *) (rom.locale + 6000));

    printf("MASTER CLOCK: %llu\n", master_clock);
    dump_cpu(stdout, &cpu);

    unmount(&rom);
    exit(0);
}
