/**
 * @file
 * @brief
 *
 * The following C code is objectively awful. Avert your eyes when necessary.
 *
 * @author Benedict Song <benedict04song@gmail.com>
 */
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include "cpu/ucpu.h"
#include "cpu/uerrno.h"
#include "memory/umem.h"
#include "memory/urom.h"

static const char *USAGE = "Placeholder help text\n";

uerrno_t UERRNO;

typedef struct cpu_state {
    uaddr_t PC;
    uregr_t A;
    uregr_t X;
    uregr_t Y;
    uregr_t S;
    ustat_t status;
} cpu_state_t;

void dump_cpu_state(FILE *out, cpu_state_t stat) {
     fprintf(out, "PC: %" PRIu16 " A: %" PRIu8 " X: %" PRIu8 " "
                "Y: %" PRIu8 " S: %" PRIu8 " status: %" PRIu8 "\n",
                stat.PC, stat.A, stat.X, stat.Y, stat.S, stat.status
            );
}

bool check_stats_equal(ucpu_t cpu, cpu_state_t state) {
    return (cpu.PC == state.PC) &&
           (cpu.A == state.A) &&
           (cpu.X == state.X) &&
           (cpu.Y == state.Y) &&
           (cpu.S == state.S) &&
           (cpu.status == state.status)
    ;
}

void preset_bus_byte(bus_t *bus, uaddr_t which, byte_t what) {
    bus->cpu_ram[which] = what;
}

void preset_cpu_state(ucpu_t *cpu, cpu_state_t state) {
    cpu->PC = state.PC;
    cpu->A = state.A;
    cpu->X = state.X;
    cpu->Y = state.Y;
    cpu->S = state.S;
    cpu->status = state.status;
}

int main(int argc, char **argv) {
    // MASSIVE placeholders to follow
    // error handling
    if (argc != 2) {
        perror(USAGE);
        exit(1);
    }

    FILE *unittest = fopen(argv[1], "rb");

    fseek(unittest, 0, SEEK_END);
    size_t unitlen = ftell(unittest);
    rewind(unittest);

    if (unitlen < sizeof(cpu_state_t)*2 + 2) {
        // insufficient to setup the CPU
        perror("Cannot initialize CPU with file < 18 bytes");
        exit(1);
    }

    // setup everything
    ucpu_t cpu;
    init_cpu(&cpu);

	bus_t bus = new_bus();

	link_device(&cpu.buslink, &bus); // this is fine I think?

    byte_t cycs_raw = 0;
    fread(&cycs_raw, 1, 1, unittest);
    clk_t cycs = (clk_t) cycs_raw;

    uint16_t memcontent_size = 0;
    fread(&memcontent_size, 2, 1, unittest);
    uint16_t *mem_validation = malloc(memcontent_size * sizeof(uint16_t) * 2);

    cpu_state_t stat, stat_expected = { 0 };
    // yucky
    fread(&stat.PC, 2, 1, unittest);
    fread(&stat_expected.PC, 2, 1, unittest);

    fread(&stat.A, 1, 1, unittest);
    fread(&stat_expected.A, 1, 1, unittest);

    fread(&stat.X, 1, 1, unittest);
    fread(&stat_expected.X, 1, 1, unittest);

    fread(&stat.Y, 1, 1, unittest);
    fread(&stat_expected.Y, 1, 1, unittest);

    fread(&stat.status, 1, 1, unittest);
    fread(&stat_expected.status, 1, 1, unittest);

    fread(&stat.S, 1, 1, unittest);
    fread(&stat_expected.S, 1, 1, unittest);

    preset_cpu_state(&cpu, stat);

    printf("INITIAL: \n");
    dump_cpu_state(stdout, stat);
    printf("FINAL: \n");
    dump_cpu_state(stdout, stat_expected);
    printf("\n");

    size_t mem_idx = 0;
    while (ftell(unittest) + 2*sizeof(uaddr_t) + 2*sizeof(uint16_t) <= unitlen) {
        uaddr_t byte_where;
        fread(&byte_where, sizeof(uaddr_t), 1, unittest);
        fread(mem_validation + mem_idx, sizeof(uaddr_t), 1, unittest);

        uint16_t byte_what;
        fread(&byte_what, sizeof(uint16_t), 1, unittest);
        fread(mem_validation + mem_idx + 1, sizeof(uaddr_t), 1, unittest);

        preset_bus_byte(&bus, byte_where, (byte_t) byte_what);
        mem_idx += 2;
    }

    printf("Cycling CPU %llu times.\n\n", cycs);

    clk_t cyc;
    for (cyc = 0; cyc < cycs; cyc++) {
        if (step(&cpu) != 0) break;
    }

    // check results
    bool passed = true;
    if (cyc < cycs - 1) {
        printf("CYCLE ACCURACY: expected %llu cycles but only took %llu.\n",
                cycs, cyc);
        passed = false;
    }
    printf("\nEXPECTED: \n");
    dump_cpu_state(stdout, stat_expected);
    printf("ACTUAL: \n");
    dump_cpu(stdout, &cpu);

    if (!check_stats_equal(cpu, stat_expected)) {
        printf("The CPU is in an unexpected state.\n");
        passed = false;
    }

    for (size_t midx; midx < memcontent_size * 2; midx += 2) {
        uaddr_t mloc = mem_validation[midx];
        byte_t mexpect = (byte_t) mem_validation[midx + 1];
        if (get_byte(cpu.buslink, mloc) != mexpect) {
            printf("\nMemory contents differ: ADDRESS %" PRIu16 " EXPECTED %" PRIu8
                   " GOT %" PRIu8 ".", mloc, mexpect, get_byte(cpu.buslink, mloc));
            passed = false;
        }
    }
    if (passed)
        printf("Passed test!");
    printf("\n");

    fclose(unittest);

    exit(passed ? 0 : 1);
}
