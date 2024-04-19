/**
 * @file
 * @brief
 *
 * @author Benedict Song <benedict04song@gmail.com>
 */
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "cpu/ucpu.h"

/*
 * Mapping from opcode to canonical opcode (NOT FULLY IMPLEMENTED)
 * Up to AND so far...
 */
static const addr_mode_t OPCODE_TO_CANONICAL[] = {
    O_BRK, 0, 0, 0, 0, 0, O_ASL, 0, 0, 0, O_ASL, 0, 0, 0, O_ASL, 0, // 0x00 - 0x0F
    0, 0, 0, 0, 0, 0, O_ASL, 0, 0, 0, 0, 0, 0, 0, O_ASL, 0, // 0x10 - 0x1F
    0, O_AND, 0, 0, 0, O_AND, 0, 0, 0, O_AND, 0, 0, 0, O_AND, 0, 0, // 0x20 - 0x2F
    0, O_AND, 0, 0, 0, O_AND, 0, 0, 0, O_AND, 0, 0, 0, O_AND, 0, 0, // 0x30 - 0x3F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x40 - 0x4F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x50 - 0x5F
    0, O_ADC, 0, 0, 0, O_ADC, 0, 0, 0, O_ADC, 0, 0, 0, O_ADC, 0, 0, // 0x60 - 0x6F
    0, O_ADC, 0, 0, 0, O_ADC, 0, 0, 0, O_ADC, 0, 0, 0, O_ADC, 0, 0, // 0x70 - 0x7F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x80 - 0x8F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x90 - 0x9F
    0, 0, 0, 0, 0, 0, 0, 0, 0, O_LDA, O_TAX, 0, 0, 0, 0, 0, // 0xA0 - 0xAF
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xB0 - 0xBF
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xC0 - 0xCF
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xD0 - 0xDF
    0, 0, 0, 0, 0, 0, 0, 0, O_INX, 0, 0, 0, 0, 0, 0, 0, // 0xE0 - 0xEF
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // 0xF0 - 0xFF
};

/*
 * Mapping from opcode to addressing mode (NOT FULLY IMPLEMENTED)
 * Up to AND so far...
 */
static const uint8_t OPCODE_TO_ADDRMODE[] = {
    IMPL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x00 - 0x0F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x10 - 0x1F
    0, INDIR_X, 0, 0, 0, ZPAGE, 0, 0, 0, IMMED, 0, 0, 0, ABS, 0, 0, // 0x20 - 0x2F
    0, INDIR_Y, 0, 0, 0, ZPAGE_X, 0, 0, 0, ABS_Y, 0, 0, 0, ABS_X, 0, 0, // 0x30 - 0x3F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x40 - 0x4F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x50 - 0x5F
    0, INDIR_X, 0, 0, 0, ZPAGE, 0, 0, 0, IMMED, 0, 0, 0, ABS, 0, 0, // 0x60 - 0x6F
    0, INDIR_Y, 0, 0, 0, ZPAGE_X, 0, 0, 0, ABS_Y, 0, 0, 0, ABS_X, 0, 0, // 0x70 - 0x7F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x80 - 0x8F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x90 - 0x9F
    0, 0, 0, 0, 0, 0, 0, 0, 0, IMMED, IMPL, 0, 0, 0, 0, 0, // 0xA0 - 0xAF
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xB0 - 0xBF
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xC0 - 0xCF
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xD0 - 0xDF
    0, 0, 0, 0, 0, 0, 0, 0, IMPL, 0, 0, 0, 0, 0, 0, 0, // 0xE0 - 0xEF
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // 0xF0 - 0xFF
};

/*
 * Mapping from opcode to clock cycles taken
 * Up to AND so far...
 */
static const clk_t OPCODE_TO_CYCLES[] = {
    7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x00 - 0x0F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x10 - 0x1F
    0, 6, 0, 0, 0, 3, 0, 0, 0, 2, 0, 0, 0, 4, 0, 0, // 0x20 - 0x2F
    0, 5, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, // 0x30 - 0x3F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x40 - 0x4F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x50 - 0x5F
    0, 2, 0, 0, 0, 3, 0, 0, 0, 2, 0, 0, 0, 4, 0, 0, // 0x60 - 0x6F
    0, 2, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, // 0x70 - 0x7F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x80 - 0x8F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x90 - 0x9F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, // 0xA0 - 0xAF
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xB0 - 0xBF
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xC0 - 0xCF
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xD0 - 0xDF
    0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, // 0xE0 - 0xEF
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // 0xF0 - 0xFF
};

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
 * @brief
 */
static void set_flag(ucpu_t *cpu, flag_t flag, bool value) {
    if (value)
        cpu->status |= (1u << ((int) flag));
    else
        cpu->status &= ~(1u << ((int) flag));
}

/**
 * @brief
 *
 * @returns false if negative, true if positive
 */
static bool sign(byte_t u) {
    return !(u >> (sizeof(byte_t)*8 - 1));
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
    cpu->PC = 0u; // program counter
    cpu->A = 0u; // accumulator
    cpu->X = 0u; // generic register X
    cpu->Y = 0u; // generic register Y
    cpu->S = 0u; // stack pointer
    cpu->status = 16u; // status "register" -- bit 5 always set
}

int drive(ucpu_t *cpu, byte_t *program, int program_size) {
    while (1) {
        if (cpu->PC >= program_size) { // check for termination this way (for now!)
            return 0;
        }
        step(cpu, program);
    }
}

clk_t step(ucpu_t *cpu, byte_t *program) {
    // get the current opcode
    opcode_t op = program[cpu->PC];
    // get the addressing mode
    addr_mode_t addr_mode = OPCODE_TO_ADDRMODE[op];

    // from the address mode, we can imply number of operands
    // as well as the (interpreted) operand itself
    byte_t operand = 0;
    switch (addr_mode) {
        case REL:
        case IMMED: {
            operand = program[cpu->PC + 1]; // assume program has been assembled properly
                                            // and that we can do this safely
            cpu->PC += 2; // remember to increment PC
            break;
        }
        case ZPAGE: {
            operand = cpu->memory[ program[cpu->PC + 1] ];
            cpu->PC += 2;
            break;
        }
        case ZPAGE_X: {
            operand = cpu->memory [
                        (program[cpu->PC + 1] + cpu->X) % 256
            ];
            cpu->PC += 2;
            break;
        }
        case ZPAGE_Y: {
            operand = cpu->memory [
                        (program[cpu->PC + 1] + cpu->Y) % 256
            ];
            cpu->PC += 2;
            break;
        }
        case INDIR: // literally the same as ABS except bytes are interp. differently
        case ABS: {
            operand = cpu->memory [
                        pack(program[cpu->PC + 2], program[cpu->PC + 1]) // little endian
            ];
            cpu->PC += 3;
            break;
        }
        case INDIR_X: {
            // not implemented
            cpu->PC += 2;
            break;
        }
        case ABS_X: {
            operand = cpu->memory [
                      (pack(program[cpu->PC + 2], program[cpu->PC + 1])
                       + cpu->X) % 256
            ];
            cpu->PC += 3;
            break;
        }
        case INDIR_Y: {
            // not implemented
            cpu->PC += 2;
            break;
        }
        case ABS_Y: {
            operand = cpu->memory [
                      (pack(program[cpu->PC + 2], program[cpu->PC + 1])
                       + cpu->Y) % 256
            ];
            cpu->PC += 3;
            break;
        }
        default: {
            cpu->PC += 1;
        }
    }
    // get the actual operation class
    opcode_t canon = OPCODE_TO_CANONICAL[op];

    switch (canon) {
        case O_ADC: {
            // Apparently 6502 decimal mode is not supported on the NES?
            // If there are problems with ADC maybe refer back here.
            unsigned long temp = (unsigned long) cpu->A; // bad overflow detection
            cpu->A += operand;
            unsigned long true_result = temp + operand;
            set_flag(cpu, OVERFLOW, (true_result != cpu->A));
            set_flag(cpu, CARRY, !!(true_result >> (sizeof(uregr_t)*8)));
            set_flag(cpu, ZERO, cpu->A == 0);
            set_flag(cpu, NEGATIVE, !!(cpu->A >> 7));
            break;
        }
        case O_LDA: {
            cpu->A = operand;
            set_flag(cpu, ZERO, cpu->A == 0);
            set_flag(cpu, NEGATIVE, !sign(cpu->A));
            break;
        }
        case O_TAX: {
            cpu->X = cpu->A;
            set_flag(cpu, ZERO, cpu->X == 0);
            set_flag(cpu, NEGATIVE, !sign(cpu->X));
            break;
        }
        case O_INX: {
            cpu->X++;
            set_flag(cpu, ZERO, cpu->X == 0);
            set_flag(cpu, NEGATIVE, !sign(cpu->X));
            break;
        }
        case O_BRK: {
            set_flag(cpu, BREAK, true);
            break;
        }
        default: {

        }
    }
}

void dump_cpu(FILE *out, ucpu_t *cpu) {
     fprintf(out, "PC: %" PRIu16 " A: %" PRIu8 " X: %" PRIu8 " "
                "Y: %" PRIu8 " S: %" PRIu8 " status: %" PRIu8 "\n",
                cpu->PC, cpu->A, cpu->X, cpu->Y, cpu->S, cpu->status);
}

