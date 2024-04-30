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
#include "memory/umem.h"

#define DEFER(cpu, cycs) {\
    if (!cpu->deferred) { \
        cpu->cycs_left += cycs; \
        cpu->deferred = true; \
        return 0; \
    }\
}

#define SETS(where, what) sets(cpu->memory, where, what);

/*
 * Mapping from opcode to canonical opcode
 */
static const addr_mode_t OPCODE_TO_CANONICAL[] = {
    O_BRK, O_ORA, 0, 0, 0, O_ORA, O_ASL, 0, O_PHP, O_ORA, O_ASL, 0, 0, O_ORA, O_ASL, 0, 
    O_BPL, O_ORA, 0, 0, 0, O_ORA, O_ASL, 0, O_CLC, O_ORA, 0, 0, 0, O_ORA, O_ASL, 0, 
    O_JSR, O_AND, 0, 0, O_BIT, O_AND, O_ROL, 0, O_PLP, O_AND, O_ROL, 0, O_BIT, O_AND, O_ROL, 0, 
    O_BMI, O_AND, 0, 0, 0, O_AND, O_ROL, 0, O_SEC, O_AND, 0, 0, 0, O_AND, O_ROL, 0, 
    O_RTI, O_EOR, 0, 0, 0, O_EOR, O_LSR, 0, O_PHA, O_EOR, O_LSR, 0, O_JMP, O_EOR, O_LSR, 0, 
    O_BVC, O_EOR, 0, 0, 0, O_EOR, O_LSR, 0, O_CLI, O_EOR, 0, 0, 0, O_EOR, O_LSR, 0, 
    O_RTS, O_ADC, 0, 0, 0, O_ADC, O_ROR, 0, O_PLA, O_ADC, O_ROR, 0, O_JMP, O_ADC, O_ROR, 0, 
    0, O_ADC, 0, 0, 0, O_ADC, O_ROR, 0, O_SEI, O_ADC, 0, 0, 0, O_ADC, O_ROR, 0, 
    0, O_STA, 0, 0, O_STY, O_STA, O_STX, 0, O_DEY, 0, O_TXA, 0, O_STY, O_STA, O_STX, 0, 
    O_BCC, O_STA, 0, 0, O_STY, O_STA, O_STX, 0, O_TYA, O_STA, O_TXS, 0, 0, O_STA, 0, 0, 
    O_LDY, O_LDA, O_LDX, 0, O_LDY, O_LDA, O_LDX, 0, O_TAY, O_LDA, O_TAX, 0, O_LDY, O_LDA, O_LDX, 0, 
    O_BCS, O_LDA, 0, 0, O_LDY, O_LDA, O_LDX, 0, O_TSX, O_LDA, 0, 0, O_LDY, O_LDA, O_LDX, 0, 
    O_CPY, O_CMP, 0, 0, O_CPY, O_CMP, O_DEC, 0, O_INY, O_CMP, O_DEX, 0, O_CPY, O_CMP, O_DEC, 0, 
    O_BNE, O_CMP, 0, 0, 0, O_CMP, O_DEC, 0, O_CLD, O_CMP, 0, 0, 0, O_CMP, O_DEC, 0, 
    O_CPX, O_SBC, 0, 0, O_CPX, O_SBC, O_INC, 0, O_INX, O_SBC, O_NOP, 0, O_CPX, O_SBC, O_INC, 0, 
    O_BEQ, O_SBC, 0, 0, 0, O_SBC, O_INC, 0, O_SED, O_SBC, 0, 0, 0, O_SBC, O_INC, 0
};

/*
 * Mapping from opcode to addressing mode
 */
static const uint8_t OPCODE_TO_ADDRMODE[] = {
    IMPL, INDIR_X, 0, 0, 0, ZPAGE, ZPAGE, 0, IMPL, IMMED, ACCUM, 0, 0, ABS, ABS, 0, 
    REL, INDIR_Y, 0, 0, 0, ZPAGE_X, ZPAGE_X, 0, IMPL, ABS_Y, 0, 0, 0, ABS_X, ABS_X, 0, 
    ABS, INDIR_X, 0, 0, ZPAGE, ZPAGE, ZPAGE, 0, IMPL, IMMED, ACCUM, 0, ABS, ABS, ABS, 0, 
    REL, INDIR_Y, 0, 0, 0, ZPAGE_X, ZPAGE_X, 0, IMPL, ABS_Y, 0, 0, 0, ABS_X, ABS_X, 0, 
    IMPL, INDIR_X, 0, 0, 0, ZPAGE, ZPAGE, 0, IMPL, IMMED, ACCUM, 0, ABS, ABS, ABS, 0, 
    REL, INDIR_Y, 0, 0, 0, ZPAGE_X, ZPAGE_X, 0, IMPL, ABS_Y, 0, 0, 0, ABS_X, ABS_X, 0, 
    IMPL, INDIR_X, 0, 0, 0, ZPAGE, ZPAGE, 0, IMPL, IMMED, ACCUM, 0, INDIR, ABS, ABS, 0, 
    0, INDIR_Y, 0, 0, 0, ZPAGE_X, ZPAGE_X, 0, IMPL, ABS_Y, 0, 0, 0, ABS_X, ABS_X, 0, 
    0, INDIR_X, 0, 0, ZPAGE, ZPAGE, ZPAGE, 0, IMPL, 0, IMPL, 0, ABS, ABS, ABS, 0, 
    REL, INDIR_Y, 0, 0, ZPAGE_X, ZPAGE_X, ZPAGE_Y, 0, IMPL, ABS_Y, IMPL, 0, 0, ABS_X, 0, 0, 
    IMMED, INDIR_X, IMMED, 0, ZPAGE, ZPAGE, ZPAGE, 0, IMPL, IMMED, IMPL, 0, ABS, ABS, ABS, 0, 
    REL, INDIR_Y, 0, 0, ZPAGE_X, ZPAGE_X, ZPAGE_Y, 0, IMPL, ABS_Y, 0, 0, ABS_X, ABS_X, ABS_Y, 0, 
    IMMED, INDIR_X, 0, 0, ZPAGE, ZPAGE, ZPAGE, 0, IMPL, IMMED, IMPL, 0, ABS, ABS, ABS, 0, 
    REL, INDIR_Y, 0, 0, 0, ZPAGE_X, ZPAGE_X, 0, IMPL, ABS_Y, 0, 0, 0, ABS_X, ABS_X, 0, 
    IMMED, INDIR_X, 0, 0, ZPAGE, ZPAGE, ZPAGE, 0, IMPL, IMMED, IMPL, 0, ABS, ABS, ABS, 0, 
    REL, INDIR_Y, 0, 0, 0, ZPAGE_X, ZPAGE_X, 0, IMPL, ABS_Y, 0, 0, 0, ABS_X, ABS_X, 0
};

/*
 * Mapping from opcode to clock cycles taken
 */
static const clk_t OPCODE_TO_CYCLES[] = {
    7, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 0, 4, 6, 0, 
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
    6, 6, 0, 0, 3, 3, 5, 0, 4, 2, 2, 0, 4, 4, 6, 0, 
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
    6, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 3, 4, 6, 0, 
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
    6, 6, 0, 0, 0, 3, 5, 0, 4, 2, 2, 0, 5, 4, 6, 0, 
    0, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
    0, 6, 0, 0, 3, 3, 3, 0, 2, 0, 2, 0, 4, 4, 4, 0, 
    2, 6, 0, 0, 4, 4, 4, 0, 2, 5, 2, 0, 0, 5, 0, 0, 
    2, 6, 2, 0, 3, 3, 3, 0, 2, 2, 2, 0, 4, 4, 4, 0, 
    2, 5, 0, 0, 4, 4, 4, 0, 2, 4, 0, 0, 4, 4, 4, 0, 
    2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0, 
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
    2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0, 
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0
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
 */
static bool get_flag(ucpu_t *cpu, flag_t flag) {
    return !!(cpu->status &= (1u << ((int) flag)));
}

/**
 * @brief
 * 
 * @returns -1 if page(ptr1) < page(ptr2), +1 if page(ptr1) > page(ptr2), 0 if equal.
 */
static int compare_pages(uaddr_t ptr1, uaddr_t ptr2) {
    uaddr_t page1 = (ptr1 >> 8); // get pages
    uaddr_t page2 = (ptr2 >> 8); // ditto
    if (page1 < page2) return -1;
    else if (page1 == page2) return 0;
    else if (page1 > page2) return 1;
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
 * @param[out] Pointer to where the cpu struct should be written.
 */
void init_cpu(ucpu_t *cpu, ram_t ram) {
    // initialize all registers (placeholders for now)
    cpu->PC = 0u; // program counter
    cpu->A = 0u; // accumulator
    cpu->X = 0u; // generic register X
    cpu->Y = 0u; // generic register Y
    cpu->S = 0u; // stack pointer
    cpu->status = 16u; // status "register" -- bit 5 always set
    
    // initialize main memory pointer
    cpu->memory = ram;

    // initialize state machine logic
    cpu->accum = false;
    cpu->cycs_left = 0;
    cpu->deferred = false;
}

int drive(ucpu_t *cpu, byte_t *program, int program_size) {
    while (1) {
        if (cpu->PC >= program_size) { // check for termination this way (for now!)
            return 0;
        }
        step(cpu, program);
    }
}

/**
 * @brief
 * 
 * @returns 1 if program terminates, 0 otherwise.
 */
int step(ucpu_t *cpu, byte_t *program) {
    // check if CPU is currently waiting on clock
    if (cpu->cycs_left > 1) {
        cpu->cycs_left--; // indicate one more cycle has passed
        return 0;
    } else if (cpu->cycs_left == 0) {
        // interpret the next instruction and reset the state machine 
        // get the current opcode
        opcode_t op = program[cpu->PC];
        // get the actual operation class
        cpu->curr_canon = OPCODE_TO_CANONICAL[op];

        // initialize number of cycles
        cpu->cycs_left = OPCODE_TO_CYCLES[op] - 1; // subtract 1 for current cycle

        // get the addressing mode
        cpu->curr_addr_mode = OPCODE_TO_ADDRMODE[op]; 

        cpu->operand = NULL;
        cpu->accum = false;

        // calculate everything
        // note we also increment the PC here too
        switch (cpu->curr_addr_mode) {
            case REL:
            case IMMED: {
                cpu->operand = &program[cpu->PC + 1]; // assume program has been assembled properly
                                                // and that we can do this safely
                cpu->PC += 2; // remember to increment PC
                break;
            }
            case ZPAGE: {
                cpu->operand = get_actual_addr(cpu->memory, program[cpu->PC + 1]);
                cpu->PC += 2;
                break;
            }
            case ZPAGE_X: {
                cpu->operand = get_actual_addr(cpu->memory, 
                            (program[cpu->PC + 1] + cpu->X) % 256
                );
                cpu->PC += 2;
                break;
            }
            case ZPAGE_Y: {
                cpu->operand = get_actual_addr(cpu->memory,
                            (program[cpu->PC + 1] + cpu->Y) % 256
                );
                cpu->PC += 2;
                break;
            }
            case INDIR: // literally the same as ABS except bytes are interp. differently
            case ABS: {
                cpu->operand = get_actual_addr(cpu->memory,
                            pack(program[cpu->PC + 2], program[cpu->PC + 1]) // little endian
                );
                cpu->PC += 3;
                break;
            }
            case INDIR_X: {
                // not implemented
                cpu->PC += 2;
                break;
            }
            case ABS_X: {
                uaddr_t packed_addr = pack(program[cpu->PC + 2], program[cpu->PC + 1]);
                cpu->operand = get_actual_addr(cpu->memory,
                            packed_addr + cpu->X
                );
                if (program[cpu->PC + 1] + cpu->X > 255) { // page crossing
                    cpu->cycs_left++;
                }
                cpu->PC += 3;
                break;
            }
            case INDIR_Y: {
                // not implemented
                cpu->PC += 2;
                break;
            }
            case ABS_Y: {
                uaddr_t packed_addr = pack(program[cpu->PC + 2], program[cpu->PC + 1]);
                cpu->operand = get_actual_addr(cpu->memory, 
                            packed_addr + cpu->Y
                );
                if (program[cpu->PC + 1] + cpu->Y > 255) { // page crossing
                    cpu->cycs_left++;
                }
                cpu->PC += 3;
                break;
            }
            case ACCUM: {
                cpu->operand = &cpu->A;
                break; 
            }
            default: {
                cpu->PC += 1;
            }
        }
        // wait the necessary number of cycles
        return 0;
    }
    // set clock to 0
    cpu->cycs_left--;

    // execute the instruction we were waiting on
    // read the state machine!

    // get the current opcode
    opcode_t op = cpu->curr_canon;

    // get the addressing mode
    addr_mode_t addr_mode = cpu->curr_addr_mode;
    byte_t *operand = cpu->operand; 
    bool accum = cpu->accum; 

    switch (op) {
        case O_ADC: {
            // Apparently 6502 decimal mode is not supported on the NES?
            // If there are problems with ADC maybe refer back here.
            unsigned long temp = (unsigned long) cpu->A; // bad overflow detection
            cpu->A += *operand;
            unsigned long true_result = temp + *operand;
            set_flag(cpu, OVERFLOW, (true_result != cpu->A));
            set_flag(cpu, CARRY, !!(true_result >> (sizeof(uregr_t)*8)));
            set_flag(cpu, ZERO, cpu->A == 0);
            set_flag(cpu, NEGATIVE, !!(cpu->A >> 7));
            break;
        }
        case O_AND: {
            cpu->A &= *operand;
            set_flag(cpu, ZERO, cpu->A == 0);
            set_flag(cpu, NEGATIVE, !sign(cpu->A));
            break;
        }
        case O_ASL: { // this instruction modifies memory
            byte_t val;
            val = *operand;
            SETS(operand, (*operand << 1));
            set_flag(cpu, CARRY, !!(val >> 7));
            set_flag(cpu, ZERO, cpu->A == 0);
            set_flag(cpu, NEGATIVE, !sign(*operand));
            break;
        }
        case O_BCC: {
            // if carry bit clear... <==> cpu->C == 0??
            if (!get_flag(cpu, CARRY) || cpu->deferred) {
                offset_t off = (offset_t) *operand;
                clk_t cycs = compare_pages(cpu->PC, cpu->PC + off) == 0 ?
                                1 : 2;
                DEFER(cpu, cycs); // defer 1 cycle on successful branch
                cpu->PC += off; // play around with this line
                                // do we have to subtract back 2
                                // for the initial addition to PC?
                cpu->deferred = false;
            }
            break;
        }
        case O_BCS: {
            // if carry bit clear... <==> cpu->C == 0??
            if (get_flag(cpu, CARRY) || cpu->deferred) {
                offset_t off = (offset_t) *operand;
                clk_t cycs = compare_pages(cpu->PC, cpu->PC + off) == 0 ?
                                1 : 2;
                DEFER(cpu, cycs); // defer 1 cycle on successful branch
                cpu->PC += off; // play around with this line
                                // do we have to subtract back 2
                                // for the initial addition to PC?
                cpu->deferred = false;
            }
            break;
        }
        case O_BEQ: {
            // if carry bit clear... <==> cpu->C == 0??
            if (get_flag(cpu, ZERO) || cpu->deferred) {
                offset_t off = (offset_t) *operand;
                clk_t cycs = compare_pages(cpu->PC, cpu->PC + off) == 0 ?
                                1 : 2;
                DEFER(cpu, cycs); // defer 1 cycle on successful branch
                cpu->PC += off; // play around with this line
                                // do we have to subtract back 2
                                // for the initial addition to PC?
                cpu->deferred = false;
            }
            break;
        }
        case O_DEC: {
            SETS(operand, *operand - 1);
            set_flag(cpu, ZERO, *operand == 0);
            set_flag(cpu, NEGATIVE, !sign(*operand));
            break;
        }
        case O_LDA: {
            cpu->A = *operand;
            set_flag(cpu, ZERO, cpu->A == 0);
            set_flag(cpu, NEGATIVE, !sign(cpu->A));
            break;
        }
        case O_LDX: {
            cpu->X = *operand;
            set_flag(cpu, ZERO, cpu->X == 0);
            set_flag(cpu, NEGATIVE, !sign(cpu->X));
            break;
        }
        case O_STA: {
            SETS(operand, cpu->A);
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
            return 1;
        }
        default: {
            break;
        }
    }
    return 0;
}

void dump_cpu(FILE *out, ucpu_t *cpu) {
     fprintf(out, "PC: %" PRIu16 " A: %" PRIu8 " X: %" PRIu8 " "
                "Y: %" PRIu8 " S: %" PRIu8 " status: %" PRIu8 " "
                "remaining cycles: %" PRIu64 " deferred: %d\n",
                cpu->PC, cpu->A, cpu->X, cpu->Y, cpu->S, cpu->status,
                cpu->cycs_left, cpu->deferred);
}

// literally bc lldb is trash
void dump_cpu_to_stdout(ucpu_t *cpu) {
    dump_cpu(stdout, cpu);
}