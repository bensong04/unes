/**
 * @file
 * @brief
 *
 * @author Benedict Song <benedict04song@gmail.com>
 */
#include <stdlib.h>
#include <stdio.h>
#include "cpu/ucpu.h"
#include "memory/umem.h"

static const char *USAGE = "Placeholder help text\n";

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
    // 1000 byte program for now
    byte_t *rom = mmap(NULL, 1000, PROT_WRITE | PROT_READ, 
                        MAP_PRIVATE | MAP_ANON, -1, 0);  

    FILE *progfile = fopen(program_path, "rb");
    if (progfile == NULL) {
        fprintf(stderr, "No file \"%s\" found", program_path);
        exit(1);
    }
    long filelen;
    fseek(progfile, 0, SEEK_END);
    filelen = ftell(progfile);
    rewind(progfile);

    fread(rom, filelen, 1, progfile);
    fclose(progfile);

    // bad, should rewrite
    while (cpu.PC <= filelen) {
        master_clock++;
        if (step(&cpu, rom) == 1) {
            break;
        }
        if (master_clock > 10000) {
            perror("Warning: infinite loop\n");
            exit(1);
        }
    }

    printf("MASTER CLOCK: %llu\n", master_clock);
    dump_cpu(stdout, &cpu);
    exit(0);
}
