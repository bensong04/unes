/**
 * @file
 * @brief
 * 
 * @author Benedict Song <benedict04song@gmail.com>
 */
#include "graphics/pixels.h"

#define PX_RED_OFF 2
#define PX_GREEN_OFF 1
#define PX_BLUE_OFF 0
#define PX_ALPHA_OFF 3

#define PX_RED_MASK (0xFFu << (8 * PX_RED_OFF))  
#define PX_GREEN_MASK (0xFFu << (8 * PX_GREEN_OFF))
#define PX_BLUE_MASK (0xFFu << (8 * PX_BLUE_OFF))

pixel_t new_pixel(uint8_t r, uint8_t g, uint8_t b) {
    return (((pixel_t) r) << (8 * PX_RED_OFF)) | 
           (((pixel_t) g) << (8 * PX_GREEN_OFF)) | 
           (((pixel_t) b) << (8 * PX_BLUE_OFF)) | 
           (((pixel_t) 0xFF) << (8 * PX_ALPHA_OFF)); 
}

uint8_t get_red(pixel_t px) {
    return (px & PX_RED_MASK) >> (8 * PX_RED_OFF);
}

uint8_t get_green(pixel_t px) {
    return (px & PX_GREEN_MASK) >> (8 * PX_GREEN_OFF);
}

uint8_t get_blue(pixel_t px) {
    return (px & PX_BLUE_MASK) >> (8 * PX_BLUE_OFF);
}