#ifndef _UPPU_INCLUDED

#include <stdint.h>
#include <stdbool.h>

#include "memory/umem.h"

#define PPU_CTRL_ADDR 0x2000u
#define PPU_MASK_ADDR 0x2001u
#define PPU_STAT_ADDR 0x2002u
#define PPU_OAMA_ADDR 0x2003u
#define PPU_OAMD_ADDR 0x2004u
#define PPU_SCRL_ADDR 0x2005u
#define PPU_ADDR_ADDR 0x2006u
#define PPU_DATA_ADDR 0x2007u

#define OAM_DMA_ADDR 0x4014u

typedef struct uppu {

    /* The 8 standard CPU-exposed registers */

    uregr_t PPU_CTRL;
    uregr_t PPU_MASK;
    uregr_t PPU_STAT;
    uregr_t OAM_ADDR;
    uregr_t OAM_DATA;
    uregr_t PPU_SCRL;
    uregr_t PPU_ADDR;

    /* The special CPU-exposed OAMDMA register */

    uregr_t OAM_DMA;

    /* PPU internal registers */

    uaddr_t v; // current VRAM address; 15 bits
    uaddr_t t; // temporary VRAM address; 15 bits
    uregr_t x; // fine X scroll; 3 bits
    bool w; // first or second write toggle; 1 bit

} uppu_t;

#define _UPPU_INCLUDED
#endif