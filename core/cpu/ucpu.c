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
#include "cpu/uerrno.h"
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
    O_BRK, O_ORA, O_DNE, O_DNE, O_DNE, O_ORA, O_ASL, O_DNE, O_PHP, O_ORA, O_ASL, O_DNE, O_DNE, O_ORA, O_ASL, O_DNE, 
    O_BPL, O_ORA, O_DNE, O_DNE, O_DNE, O_ORA, O_ASL, O_DNE, O_CLC, O_ORA, O_DNE, O_DNE, O_DNE, O_ORA, O_ASL, O_DNE, 
    O_JSR, O_AND, O_DNE, O_DNE, O_BIT, O_AND, O_ROL, O_DNE, O_PLP, O_AND, O_ROL, O_DNE, O_BIT, O_AND, O_ROL, O_DNE, 
    O_BMI, O_AND, O_DNE, O_DNE, O_DNE, O_AND, O_ROL, O_DNE, O_SEC, O_AND, O_DNE, O_DNE, O_DNE, O_AND, O_ROL, O_DNE, 
    O_RTI, O_EOR, O_DNE, O_DNE, O_DNE, O_EOR, O_LSR, O_DNE, O_PHA, O_EOR, O_LSR, O_DNE, O_JMP, O_EOR, O_LSR, O_DNE, 
    O_BVC, O_EOR, O_DNE, O_DNE, O_DNE, O_EOR, O_LSR, O_DNE, O_CLI, O_EOR, O_DNE, O_DNE, O_DNE, O_EOR, O_LSR, O_DNE, 
    O_RTS, O_ADC, O_DNE, O_DNE, O_DNE, O_ADC, O_ROR, O_DNE, O_PLA, O_ADC, O_ROR, O_DNE, O_JMP, O_ADC, O_ROR, O_DNE, 
    O_DNE, O_ADC, O_DNE, O_DNE, O_DNE, O_ADC, O_ROR, O_DNE, O_SEI, O_ADC, O_DNE, O_DNE, O_DNE, O_ADC, O_ROR, O_DNE, 
    O_DNE, O_STA, O_DNE, O_DNE, O_STY, O_STA, O_STX, O_DNE, O_DEY, O_DNE, O_TXA, O_DNE, O_STY, O_STA, O_STX, O_DNE, 
    O_BCC, O_STA, O_DNE, O_DNE, O_STY, O_STA, O_STX, O_DNE, O_TYA, O_STA, O_TXS, O_DNE, O_DNE, O_STA, O_DNE, O_DNE, 
    O_LDY, O_LDA, O_LDX, O_DNE, O_LDY, O_LDA, O_LDX, O_DNE, O_TAY, O_LDA, O_TAX, O_DNE, O_LDY, O_LDA, O_LDX, O_DNE, 
    O_BCS, O_LDA, O_DNE, O_DNE, O_LDY, O_LDA, O_LDX, O_DNE, O_TSX, O_LDA, O_DNE, O_DNE, O_LDY, O_LDA, O_LDX, O_DNE, 
    O_CPY, O_CMP, O_DNE, O_DNE, O_CPY, O_CMP, O_DEC, O_DNE, O_INY, O_CMP, O_DEX, O_DNE, O_CPY, O_CMP, O_DEC, O_DNE, 
    O_BNE, O_CMP, O_DNE, O_DNE, O_DNE, O_CMP, O_DEC, O_DNE, O_CLD, O_CMP, O_DNE, O_DNE, O_DNE, O_CMP, O_DEC, O_DNE, 
    O_CPX, O_SBC, O_DNE, O_DNE, O_CPX, O_SBC, O_INC, O_DNE, O_INX, O_SBC, O_NOP, O_DNE, O_CPX, O_SBC, O_INC, O_DNE, 
    O_BEQ, O_SBC, O_DNE, O_DNE, O_DNE, O_SBC, O_INC, O_DNE, O_SED, O_SBC, O_DNE, O_DNE, O_DNE, O_SBC, O_INC, O_DNE
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
static bool get_nth_bit(byte_t byte, int n) {
    return !!(byte & (1u << (n)));
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
    return get_nth_bit(cpu->status, (int) flag);
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
    cpu->S = 0x01FF; // stack pointer. Stores EMULATED memory address.
    cpu->status = 16u; // status "register" -- bit 5 always set
    
    // initialize main memory pointer
    cpu->memory = ram;

    // initialize state machine logic
    cpu->accum = false;
    cpu->cycs_left = 0;
    cpu->deferred = false;
}

void push(ucpu_t *cpu, byte_t what) {
    if (cpu->S == 0x00FF) {
        UERRNO = ERR_STACK_OVERFLOW;
        return;
    }
    set_byte(cpu->memory, cpu->S, what);
    cpu->S--;
}

byte_t pop(ucpu_t *cpu) {
    if (cpu->S == 0x01FF) {
        UERRNO = ERR_STACK_UNDERFLOW;
        return 0x00;
    }
    cpu->S++;
    return get_byte(cpu->memory, cpu->S);
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
#ifdef DEBUG
        printf("OP: %x\n", op);
        dump_cpu(stdout, cpu);
        printf("\n");
#endif
        // get the actual operation class
        cpu->curr_canon = OPCODE_TO_CANONICAL[op];
        if (cpu->curr_canon == O_DNE) {
            fprintf(stderr, "Unrecognized instruction %x, exiting!\n", op);
            return 1;
        }
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
            case INDIR: {
                uaddr_t first = * (uaddr_t *) get_actual_addr(cpu->memory,
                            pack(program[cpu->PC + 2], program[cpu->PC + 1])
                );
                cpu->operand = get_actual_addr(cpu->memory, first);
                break;
            }
            case ABS: {
                cpu->operand = get_actual_addr(cpu->memory,
                            pack(program[cpu->PC + 2], program[cpu->PC + 1]) // little endian
                );
                cpu->PC += 3;
                break;
            }
            case INDIR_X: {
                cpu->operand = get_actual_addr(cpu->memory, 
                            get_byte(cpu->memory, pack(0, program[cpu->PC + 1] + cpu->X))
                );
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
                uaddr_t before_indir = get_byte(cpu->memory, pack(0, program[cpu->PC + 1]));
                uaddr_t after_indir = before_indir + cpu->Y;
                if (compare_pages(after_indir, before_indir) != 0) {
                    cpu->cycs_left++;
                }
                cpu->operand = get_actual_addr(cpu->memory, after_indir);
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
                cpu->accum = true;
                cpu->PC += 1;
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
            unsigned long add = *operand + (unsigned long) get_flag(cpu, CARRY);
            unsigned long temp = (unsigned long) cpu->A; // bad overflow detection
            cpu->A += add;
            unsigned long true_result = temp + add;
            set_flag(cpu, OVERFLOW, (true_result != cpu->A));
            set_flag(cpu, CARRY, !!(true_result >> (sizeof(uregr_t)*8)));
            set_flag(cpu, ZERO, cpu->A == 0);
            set_flag(cpu, NEGATIVE, !sign(cpu->A));
            break;
        }
        case O_AND: {
            cpu->A &= *operand;
            set_flag(cpu, ZERO, cpu->A == 0);
            set_flag(cpu, NEGATIVE, !sign(cpu->A));
            break;
        }
        case O_ASL: { // a bit messy...
            byte_t val;
            if (cpu->accum) {
                val = cpu->A;
                cpu->A = cpu->A << 1;
                set_flag(cpu, CARRY, !!(val >> 7));
                set_flag(cpu, ZERO, cpu->A == 0);
                set_flag(cpu, NEGATIVE, !sign(cpu->A));
                break;
            }
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
        case O_BIT: {
            byte_t test = cpu->A & *operand;
            set_flag(cpu, ZERO, test == 0);
            set_flag(cpu, OVERFLOW, get_nth_bit(test, 6));
            set_flag(cpu, NEGATIVE, get_nth_bit(test, 7));
            break;
        }
        case O_BMI: {
            if (get_flag(cpu, NEGATIVE) || cpu->deferred) {
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
        case O_BNE: {
            if (!get_flag(cpu, ZERO) || cpu->deferred) {
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
        case O_BPL: {
            if (!get_flag(cpu, NEGATIVE) || cpu->deferred) {
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
        case O_BVC: {
            if (!get_flag(cpu, OVERFLOW) || cpu->deferred) {
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
        case O_BVS: {
            if (get_flag(cpu, OVERFLOW) || cpu->deferred) {
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
        case O_CLC: {
            set_flag(cpu, CARRY, false);
            break;
        }
        case O_CLD: {
            set_flag(cpu, DECIMAL, false);
            break;
        }
        case O_CLI: {
            set_flag(cpu, INTERRUPT, false);
            break;
        }
        case O_CLV: {
            set_flag(cpu, OVERFLOW, false);
            break;
        }
        case O_CMP: { // Check this case later
            byte_t comparison = cpu->A - *operand;
            set_flag(cpu, CARRY, sign(comparison));
            set_flag(cpu, ZERO, comparison == 0);
            set_flag(cpu, NEGATIVE, !sign(comparison));
            break;
        }
        case O_CPX: {
            byte_t comparison = cpu->X - *operand;
            set_flag(cpu, CARRY, sign(comparison));
            set_flag(cpu, ZERO, comparison == 0);
            set_flag(cpu, NEGATIVE, !sign(comparison));
            break;
        }
        case O_CPY: {
            byte_t comparison = cpu->Y - *operand;
            set_flag(cpu, CARRY, sign(comparison));
            set_flag(cpu, ZERO, comparison == 0);
            set_flag(cpu, NEGATIVE, !sign(comparison));
            break;
        }
        case O_DEC: {
            SETS(operand, *operand - 1);
            set_flag(cpu, ZERO, *operand == 0);
            set_flag(cpu, NEGATIVE, !sign(*operand));
            break;
        }
        case O_DEX: {
            cpu->X--;
            set_flag(cpu, ZERO, cpu->X == 0);
            set_flag(cpu, NEGATIVE, !sign(cpu->X));
            break;
        }
        case O_DEY: {
            cpu->Y--;
            set_flag(cpu, ZERO, cpu->X == 0);
            set_flag(cpu, NEGATIVE, !sign(cpu->X));
            break;
        }
        case O_EOR: {
            cpu->A ^= *operand;
            set_flag(cpu, ZERO, cpu->A == 0);
            set_flag(cpu, NEGATIVE, !sign(cpu->A));
            break;
        }
        case O_INC: {
            SETS(operand, *operand + 1);
            set_flag(cpu, ZERO, *operand == 0);
            set_flag(cpu, NEGATIVE, !sign(*operand));
            break;
        }
        case O_INX: {
            cpu->X++;
            set_flag(cpu, ZERO, cpu->X == 0);
            set_flag(cpu, NEGATIVE, !sign(cpu->X));
            break;
        }
        case O_INY: {
            cpu->Y++;
            set_flag(cpu, ZERO, cpu->Y == 0);
            set_flag(cpu, NEGATIVE, !sign(cpu->Y));
            break;
        }
        case O_JMP: {
            /* 
             * Taken from nesdev.org:
             * An original 6502 has does not correctly fetch the 
             * target address if the indirect vector falls on a 
             * page boundary (e.g. $xxFF where xx is any value from 
             * $00 to $FF). In this case fetches the LSB from $xxFF 
             * as expected but takes the MSB from $xx00. This is 
             * fixed in some later chips like the 65SC02 so for 
             * compatibility always ensure the indirect vector is 
             * not at the end of the page.
             */
#ifdef DEBUG
            printf("jump to %x\n", *operand);
#endif
            cpu->PC = *operand; // TODO: check this case
            break;
        }
        case O_JSR: {
            push(cpu, cpu->PC + 2); // "+2" is NOT a typo.
            cpu->PC = *operand;
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
        case O_LDY: {
            cpu->Y = *operand;
            set_flag(cpu, ZERO, cpu->Y == 0);
            set_flag(cpu, NEGATIVE, !sign(cpu->Y));
            break;
        }
        case O_LSR: { // a bit messy...
            if (cpu->accum) {
                bool carry = cpu->A % 2;
                cpu->A = cpu->A >> 1;
                set_flag(cpu, CARRY, carry);
                set_flag(cpu, ZERO, cpu->A == 0);
                break;
            }
            bool carry = *operand % 2;
            SETS(operand, *operand >> 1);
            set_flag(cpu, CARRY, carry);
            set_flag(cpu, ZERO, *operand == 0);
            break;
        }
        case O_ORA: {
            cpu->A |= *operand;
            set_flag(cpu, ZERO, cpu->A == 0);
            set_flag(cpu, NEGATIVE, !sign(cpu->A));
            break;
        }
        case O_PHA: {
            push(cpu, cpu->A);
            break;
        }
        case O_PHP: {
            push(cpu, cpu->status);
            break;
        }
        case O_PLA: {
            cpu->A = pop(cpu);
            break;
        }
        case O_PLP: {
            cpu->status = pop(cpu);
            break;
        }
        case O_ROL: {
            uregr_t carry_r = (uregr_t) get_nth_bit(cpu->status, CARRY);
            if (cpu->accum) {
                set_flag(cpu, CARRY, get_nth_bit(cpu->A, 0));
                cpu->A <<= 1; // you can do this?! :O
                cpu->A |= carry_r;
                set_flag(cpu, NEGATIVE, !sign(cpu->A));
            } else {
                set_flag(cpu, CARRY, get_nth_bit(*operand, 0));
                SETS(operand, (*operand << 1) | carry_r);
                set_flag(cpu, NEGATIVE, !sign(*operand));
            }
            set_flag(cpu, ZERO, cpu->A == 0); // TODO: might be a typo in the spec?
            break;
        }
        case O_ROR: {
            uregr_t carry = (uregr_t) get_nth_bit(cpu->status, CARRY);
            if (cpu->accum) {
                set_flag(cpu, CARRY, get_nth_bit(cpu->A, 0));
                cpu->A >>= 1; // wheee
                cpu->A |= (carry << 7);
                set_flag(cpu, NEGATIVE, !sign(cpu->A));
            } else {
                set_flag(cpu, CARRY, get_nth_bit(*operand, 0));
                SETS(operand, (*operand >> 1) | (carry << 7));
                set_flag(cpu, NEGATIVE, !sign(*operand));
            }
            set_flag(cpu, ZERO, cpu->A == 0); // see above
            break;
        }
        case O_RTI: {
            ustat_t stat = pop(cpu);
            bool orig_brk = get_nth_bit(cpu->status, BREAK);
            cpu->status = stat;
            set_flag(cpu, BREAK, orig_brk); // ignore pulled BREAK bit
            cpu->PC = pop(cpu);
            break;
        }
        case O_RTS: {
            cpu->PC = pop(cpu) + 1;
            break;
        }
        case O_SBC: { // surely the most beautiful code ever that surely works 100% fine
            unsigned long add = *operand + (unsigned long) get_flag(cpu, CARRY);
            add = ~add + 1;
            unsigned long temp = (unsigned long) cpu->A; // bad overflow detection
            cpu->A += add;
            unsigned long true_result = temp + add;
            set_flag(cpu, OVERFLOW, (true_result != cpu->A));
            set_flag(cpu, CARRY, !!(true_result >> (sizeof(uregr_t)*8)));
            set_flag(cpu, ZERO, cpu->A == 0);
            set_flag(cpu, NEGATIVE, !sign(cpu->A));
            break;
        }
        case O_SEC: {
            set_flag(cpu, CARRY, true);
            break;
        }
        case O_SED: {
            set_flag(cpu, DECIMAL, true);
            break;
        }
        case O_SEI: {
            set_flag(cpu, INTERRUPT, true);
            break;
        }
        case O_STA: {
            SETS(operand, cpu->A);
            break;
        }
        case O_STX: {
            SETS(operand, cpu->X);
            break;
        }
        case O_STY: {
            SETS(operand, cpu->Y);
            break;
        }
        case O_TAX: {
            cpu->X = cpu->A;
            set_flag(cpu, ZERO, cpu->X == 0);
            set_flag(cpu, NEGATIVE, !sign(cpu->X));
            break;
        }
        case O_TAY: {
            cpu->X = cpu->A;
            set_flag(cpu, ZERO, cpu->X == 0);
            set_flag(cpu, NEGATIVE, !sign(cpu->X));
            break;
        }
        case O_TXA: {
            cpu->A = cpu->X;
            set_flag(cpu, ZERO, cpu->A == 0);
            set_flag(cpu, NEGATIVE, !sign(cpu->A));
            break;
        }
        case O_TXS: {
            cpu->S = cpu->X;
            break;
        }
        case O_TYA: {
            cpu->A = cpu->Y;
            set_flag(cpu, ZERO, cpu->A == 0);
            set_flag(cpu, NEGATIVE, !sign(cpu->A));
            break;
        }
        case O_BRK: {
            push(cpu, cpu->PC + 2);
            set_flag(cpu, BREAK, true);
            push(cpu, cpu->status);
            return 1; // for now, at least
        }
        default: {
            break;
        }
    }
    return 0;
}

void dump_cpu(FILE *out, ucpu_t *cpu) {
     fprintf(out, "PC: %" PRIu16 " A: %" PRIu8 " X: %" PRIu8 " "
                "Y: %" PRIu8 " S: %" PRIu16 " status: %" PRIu8 " "
                "remaining cycles: %" PRIu64 " deferred: %d\n",
                cpu->PC, cpu->A, cpu->X, cpu->Y, cpu->S, cpu->status,
                cpu->cycs_left, cpu->deferred);
}

// literally bc lldb is trash
void dump_cpu_to_stdout(ucpu_t *cpu) {
    dump_cpu(stdout, cpu);
}