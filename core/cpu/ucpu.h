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
 *
 * Macros are used over enums here for reasons I will document later.
 * I've divided macros into groups of eight for the sake of readability.
 */
#define O_ADC 0x69
#define O_AND 0x29
#define O_ASL 0x0A
#define O_BCC 0x90
#define O_BCS 0xB0
#define O_BEQ 0xF0
#define O_BIT 0x24
#define O_BMI 0x30

#define O_BNE 0xD0
#define O_BPL 0x10
#define O_BRK 0x00
#define O_BVC 0x50
#define O_BVS 0x70
#define O_CLC 0x18
#define O_CLD 0xD8
#define O_CLI 0x58

#define O_CLV 0xB8
#define O_CMP 0xC9
#define O_CPX 0xE0
#define O_CPY 0xC0
#define O_DEC 0xC6
#define O_DEX 0xCA
#define O_DEY 0x88
#define O_EOR 0x49

#define O_INC 0xE6
#define O_INX 0xE8
#define O_INY 0xC8
#define O_JMP 0x4C
#define O_JSR 0x20
#define O_LDA 0xA9
#define O_LDX 0xA2
#define O_LDY 0xA0

#define O_LSR 0x4A
#define O_NOP 0xEA
#define O_ORA 0x09
#define O_PHA 0x48
#define O_PHP 0x08
#define O_PLA 0x68
#define O_PLP 0x28
#define O_ROL 0x2A

#define O_ROR 0x6A
#define O_RTI 0x40
#define O_RTS 0x60
#define O_SBC 0xE9
#define O_SEC 0x38
#define O_SED 0xF8
#define O_SEI 0x78
#define O_STA 0x85

#define O_STX 0x86
#define O_STY 0x84
#define O_TAX 0xAA
#define O_TAY 0xA8
#define O_TSX 0xBA
#define O_TXA 0x8A
#define O_TXS 0x9A
#define O_TYA 0x98

#define O_DNE 0xFF

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
    INDIR,
    INDIR_X,
    INDIR_Y,
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
 * The 6502 also possesses seven (technically six) boolean flags.
 * They can be considered as being packed into a single 8 bit
 * "register," of sorts.
 */
typedef uint8_t ustat_t;

/*
 * Alternate 8-bit-wide type to represent offsets (which are
 * obviously signed).
 */
typedef int8_t offset_t;


/*
 * Opcodes are always a byte wide.
 */
typedef uint8_t opcode_t;

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
    uaddr_t S; // Stack pointer. Stores EMULATED memory address.
    ustat_t status; // Status "register"

    /* MAIN MEMORY */

    ram_t memory;

    /* STATE MACHINE LOGIC */
    
    // These four fields allow the CPU to jump to the proper switch case
    // and perform the proper logic when it's time to actually execute the instruction.
    addr_mode_t curr_addr_mode; // we might not need this one?
    opcode_t curr_canon; // need the canonical instruction
    byte_t *operand; // store *where* the operand is
                     // so we can fiddle with memory contents
    bool accum; // we need to manually set this option
    // since the operand is actually a register

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

void init_cpu(ucpu_t *cpu, ram_t ram);

void push(ucpu_t *cpu, byte_t what);
byte_t pop(ucpu_t *cpu);



int step(ucpu_t *cpu, byte_t *program);

/* DEBUG ROUTINES */

void dump_cpu(FILE *out, ucpu_t *cpu);

#define _UCPU_INCLUDED
#endif