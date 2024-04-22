/**
 * @file unes.hpp
 * @brief
 *
 * See spec here: https://www.nesdev.org/wiki/CPU
 *
 * @author Benedict Song <benedict04song@gmail.com>
 */
#ifndef UCPU_INCLUDED
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/* DEFINES */

/*
 * The 6502 possesses 256 pages X 256 bytes of memory.
 */
#define UCPU_MEM_CAP (256 * 256)

/*
 * The 6502's memory will be filled with UNILs when it is initialized.
 */
#define UNIL 0x00

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
typedef uint16_t uaddr_t; // Memory address type/PC register type
typedef uint8_t uregr_t; // Type of all other registers

/*
 * The 6502 also possesses seven (technically six) boolean flags.
 * They can be considered as being packed into a single 8 bit
 * "register," of sorts.
 */
typedef uint8_t ustat_t;

/*
 * The 6502's memory shall be represented as an array of
 * UCPU_MEM_CAP bytes.
 */
typedef uint8_t byte_t; // Typedef for the sake of readability

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
typedef uint32_t clk_t;

typedef struct ucpu {

    /* REGISTERS */

    uaddr_t PC; // Program counter
    uregr_t A; // Accumulator
    uregr_t X; // Generic register
    uregr_t Y; // Ditto
    uregr_t S; // Stack pointer
    ustat_t status; // Status "register"

    /* MAIN MEMORY */

    byte_t memory[UCPU_MEM_CAP];

} ucpu_t;

/* IMPORTANT CPU OPERATIONS */

void init_cpu(ucpu_t *cpu);

int drive(ucpu_t *cpu, byte_t *program, int program_size);

clk_t step(ucpu_t *cpu, byte_t *program);

/* DEBUG ROUTINES */

void dump_cpu(FILE *out, ucpu_t *cpu);
#define UCPU_INCLUDED
#endif