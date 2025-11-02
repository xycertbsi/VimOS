// vga.h - VGA buffer shared declaration
#ifndef VGA_H
#define VGA_H

#include "stdint.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_COLOR(fg, bg) ((bg << 4) | fg)

// Shared VGA buffer - defined in kernel.c
extern volatile uint16_t* vga_buffer;

#endif