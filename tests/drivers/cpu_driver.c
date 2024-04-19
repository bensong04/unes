/**
 * @file
 * @brief
 *
 * @author Benedict Song <benedict04song@gmail.com>
 */
#include <stdlib.h>
#include "cpu/ucpu.h"

int main(void) {
    // MASSIVE placeholders
    // We should definitely be using sbrk...
    ucpu_t *cpu = malloc(sizeof(ucpu_t));
    init_cpu(cpu);
    byte_t program[5] = {0xa9, 0xc0, 0xaa, 0xe8, 0x00};
    drive(cpu, program, 5);
    dump_cpu(stdout, cpu);
    free(cpu);
    return 0;
}
