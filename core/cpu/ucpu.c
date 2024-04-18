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
 */
static int min(int a, int b) {
    return (a < b ? a : b);
}

/**
 * @brief
 */
static inline uaddr_t pack(byte_t large, byte_t small) {
    return (large << 8) | (small);
}

/**
 * @brief From an emulated memory address, calculates the true pointer in the emulator's VM space.
 */
static inline void *true_ptr(ucpu_t *cpu, void *emulated_ptr) {
    // from cpu pointer offset, add the size of everything *but* the memory array
    return (void *)(cpu + sizeof(ucpu_t) - UCPU_MEM_CAP*sizeof(byte_t));
}

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

clk_t step(ucpu_t *cpu, byte_t *program, int program_size) {
    // get the current opcode
    opcode_t op = program[cpu->PC];
    // get the addressing mode
    addr_mode_t addr_mode = OPCODE_TO_ADDRMODE[op];

    // from the address mode, we can imply number of operands
    // as well as the (interpreted) operand itself
    byte_t operand = 0;
    switch (addr_mode) {
        case IMMED: {
            operand = program[cpu->PC + 1]; // assume program has been assembled properly
                                            // and that we can do this safely
            cpu->PC += 2; // remember to increment PC
            break;
        }
        case ZPAGE: {
            operand = *(byte_t *) true_ptr(cpu,
                                    program[cpu->PC + 1]);
            cpu->PC += 2;
            break;
        }
        case ZPAGE_X: {
            operand = *(byte_t *) true_ptr(cpu,
                            (program[cpu->PC + 1] + cpu->X) % 256);
            break;
        }
        case ZPAGE_Y: {
            operand = *(byte_t *) true_ptr(cpu,
                            (program[cpu->PC + 1] + cpu->Y) % 256);
            break;
        }
        case ABS: {
            operand = *(byte_t *) true_ptr(cpu,
                        pack(program[cpu->PC + 2], program[cpu->PC + 1]); // little endian
            break;
        }
        case ABS_X: {
            operand = *(byte_t *) true_ptr(cpu,
                      (pack(program[cpu->PC + 2], program[cpu->PC + 1])
                       + cpu->X) % 256);
            break;
        }
        case ABS_Y: {
            operand = *(byte_t *) true_ptr(cpu,
                      (pack(program[cpu->PC + 2], program[cpu->PC + 1])
                       + cpu->Y) % 256);
            break;
        }
        case REL: {
            operand = ...;
        }
    }
    // get the actual operation class
    opcode_t canon = OPCODE_TO_CANONICAL[op];

    switch (canon) {

    case O_ADC: {

        byte_t operand = program[cpu->PC + 1]; // note you cannot do this if there might not
                                               // be an operand (ex. ACCUM case of O_ASL)
        switch (addr_mode) {

        case IMMED: {
            // 2 bytes, 2 cycles

        }

    }

    }
}

