/**
 * @file
 * @brief
 * 
 * @author Benedict Song <benedict04song@gmail.com>
 */
#ifndef _GFX_PIXELS_INCLUDED
#include <stdint.h>

/**
 * @brief Implement a pixel as an unsigned int, 122 style ;)  
 */
typedef uint32_t pixel_t;

pixel_t new_pixel(uint8_t r, uint8_t g, uint8_t b);

uint8_t get_red(pixel_t px);
uint8_t get_green(pixel_t px);
uint8_t get_blue(pixel_t px);

#define _GFX_PIXELS_INCLUDED
#endif