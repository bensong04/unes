/**
 * @file
 * @brief
 *
 * @author Benedict Song
 */
#ifndef _CARTRIDGE_INCLUDED
#include <stdio.h>

#include "memory/umem.h"

typedef enum { MAPPER_0, MAPPER_1, MAPPER_2 } mapper_t;

typedef struct {
  byte_t *_data_ptr;
  size_t cartridge_sz;
  mapper_t mapper;
} cartridge_t;

cartridge_t mount(const char *locale);
void unmount(cartridge_t cartridge);

byte_t ask(cartridge_t cartridge, uaddr_t which);

#define _CARTRIDGE_INCLUDED
#endif