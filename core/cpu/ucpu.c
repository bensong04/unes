/**
 * @file
 * @brief
 *
 * @author Benedict Song <benedict04song@gmail.com>
 */
#include <stdlib.h>
#include "cpu/ucpu.h"

/**
 * @brief
 *
 * This pointer should be `sbrk`ed and not `malloc`'ed.
 * `sbrk` is preferable over `malloc` since we know *exactly*
 * how much memory will be used by our emulator ahead of time.
 *
 * @param[out] Pointer to where the cpu struct should be written.
 */
void init_cpu(ucpu_t *cpu) {
    // zero-initialize memory
    memset(cpu->memory, UNIL, UCPU_MEM_CAP);
    // initialize all registers (placeholders for now)
    cpu->PC = 0; // program counter
    cpu->A = 0; // accumulator
    cpu->X = 0; // generic register X
    cpu->Y = 0; // generic register Y
    cpu->S = 0; // stack pointer
    cpu->status = 0; // status "register"
}

bool emulate(ucpu_t *cpu, byte_t *program, int program_size) {

}

