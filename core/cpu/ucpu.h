/**
 * @file unes.hpp
 * @brief
 *
 * See spec here: https://www.nesdev.org/wiki/CPU
 *
 * @author Benedict Song <benedict04song@gmail.com>
 */
#ifndef _UCPU_INCLUDED

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "memory/umem.h"


/* DEFINES */

/*
 * IMPORTANT: although the 6502 has 56 *distinct* opcodes,
 * many of the 256 possible opcodes are mapped to the same opcode.
 * EX. opcodes $69 and $65 both map to ADC (add w/ carry).
 *
 * These opcode "synonyms" exist because instructions can be run
 * in various "addressing modes". Continuing the example, $69 specifies
 * that ADC be run in immediate addressing mode, while $65 requires
 * that it be run in zero page mode.
 *
 * Instead of writing a 256-case switch statement filled with
 * mostly redundant code, we shall adopt the following strategy:
 * we will store global arrays mapping opcodes to one of their variants
 * (which is fixed across individual opcodes), and switch on those
 * opcodes instead: ex. the 0x65th byte in this array would be set
 * to 0x69. Then we will switch on these opcodes in a 56-case
 * switch statement.
 */
typedef enum opcodes {
    O_ADC, 
    O_AND,
    O_ASL, 
    O_BCC, 
    O_BCS, 
    O_BEQ, 
    O_BIT, 
    O_BMI, 

    O_BNE,
    O_BPL, 
    O_BRK, 
    O_BVC, 
    O_BVS, 
    O_CLC, 
    O_CLD,
    O_CLI, 

    O_CLV, 
    O_CMP, 
    O_CPX, 
    O_CPY, 
    O_DEC, 
    O_DEX, 
    O_DEY, 
    O_EOR, 

    O_INC, 
    O_INX, 
    O_INY, 
    O_JMP, 
    O_JSR, 
    O_LDA, 
    O_LDX, 
    O_LDY, 

    O_LSR, 
    O_NOP, 
    O_ORA, 
    O_PHA, 
    O_PHP, 
    O_PLA, 
    O_PLP, 
    O_ROL, 

    O_ROR,
    O_RTI, 
    O_RTS, 
    O_SBC, 
    O_SEC, 
    O_SED, 
    O_SEI, 
    O_STA, 

    O_STX, 
    O_STY, 
    O_TAX, 
    O_TAY, 
    O_TSX, 
    O_TXA, 
    O_TXS, 
    O_TYA, 

    O_DNE
} opcode_t;

/*
 * Hardware/software interrupt vectors
 */
#define BRK_VECTOR 0xFFFE
#define IRQ_VECTOR 0xFFFE
#define RST_VECTOR 0xFFFC
#define NMI_VECTOR 0xFFFA

#define STACK_OFFSET 0x0100

/* GLOBALS */

/*
 * Enum representing all the different "addressing modes" possible
 */
typedef enum {
    DOES_NOT_EXIST,
    ACCUM,
    IMMED,
    ZPAGE,
    ZPAGE_X,
    ZPAGE_Y,
    ABS,
    ABS_X,
    ABS_Y,
    ABS_X_RO, // read-only instructions may take extra cycles
    ABS_Y_RO, // ditto
    INDIR,
    INDIR_X,
    INDIR_Y,
    INDIR_Y_RO, // note read-only instructions only take extra cycles for INDIR_Y
    REL,
    IMPL
} addr_mode_t;

/*
 * Enum representing all flags within the "status" byte of the CPU
 */
typedef enum {
    CARRY = 0,
    ZERO = 1,
    INTERRUPT = 2,
    DECIMAL = 3,
    BREAK = 4,
    FIVE = 5,
    OVERFLOW = 6,
    NEGATIVE = 7
} flag_t;

/* TYPE DEFINITIONS */

/*
 * As the 6502 processor supports a 16-bit memory space,
 * the special program counter (PC) register holds 16 bits.
 * All other registers are 8 bits wide.
 */
typedef uint8_t uregr_t; // Type of all other registers

/*
 * Alternate 8-bit-wide type to represent offsets (which are
 * obviously signed).
 */
typedef int8_t offset_t;

/*
 * This datatype should be wide enough to hold the current clock state
 * (i.e. number of cycles).
 */
typedef uint64_t clk_t;

typedef struct ucpu {

    /* REGISTERS */

    uaddr_t PC; // Program counter
    uregr_t A; // Accumulator
    uregr_t X; // Generic register
    uregr_t Y; // Ditto
    uregr_t S; // Stack pointer. Stores EMULATED memory address offset.
    ustat_t status; // Status "register"

    /* LINK TO BUS */

    buslink_t buslink;

    /* STATE MACHINE LOGIC */

    // These four fields allow the CPU to jump to the proper switch case
    // and perform the proper logic when it's time to actually execute the instruction.
    addr_mode_t curr_addr_mode; // we might not need this one?
    opcode_t curr_canon; // need the canonical instruction
    uaddr_t operand; // store *where* the operand is
                     // so we can fiddle with memory contents
    bool accum; // we need to manually set this option
    // since the operand is actually a register
    bool indir; // this is the only instruction where the operand
				// is two bytes long

    // This field is bookkeeping for when the emulator should execute
    // the instruction it's waiting on. When it hits 1, it will execute the instruction.
    clk_t cycs_left;

    // This field is bookkeeping for branch instructions that defer
    // execution by one cycle upon a successful branch.
    // Upon a successful branch, step() will set this flag, increment
    // cycs_left by 1, and return. When execution returns to the
    // branch case, step() will perform the jump unconditionally.
    // It will also reset deferred back to false.
    bool deferred;

} ucpu_t;

/* IMPORTANT CPU OPERATIONS */

void init_cpu(ucpu_t *cpu);

void push(ucpu_t *cpu, byte_t what);
byte_t pop(ucpu_t *cpu);

int step(ucpu_t *cpu);

/* DEBUG ROUTINES */

void dump_cpu(FILE *out, ucpu_t *cpu);

#define _UCPU_INCLUDED
#endif
