/**
 * @file uerrno.h
 * @brief Error-handling routines for the 6502.
 * 
 * @author Benedict Song <benedict04song@gmail.com>
 */
#ifndef _UERRNO_INCLUDED

typedef enum uerrno {
    ERR_STACK_OVERFLOW,
    ERR_STACK_UNDERFLOW
} uerrno_t;

extern uerrno_t UERRNO;

#define _UERRNO_INCLUDED
#endif