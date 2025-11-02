// fb.h - Framebuffer kezelő fejléc
#ifndef FB_H
#define FB_H

#include "stdint.h"

// Képernyő alapparaméterek
#define FB_WIDTH 320
#define FB_HEIGHT 200
#define FB_BYTES_PER_PIXEL 1
#define FB_SIZE (FB_WIDTH * FB_HEIGHT * FB_BYTES_PER_PIXEL)

// Színek (8-bit palletta indexek)
#define COLOR_BLACK 0
#define COLOR_BLUE 1
#define COLOR_GREEN 2
#define COLOR_CYAN 3
#define COLOR_RED 4
#define COLOR_MAGENTA 5
#define COLOR_BROWN 6
#define COLOR_LIGHT_GRAY 7
#define COLOR_DARK_GRAY 8
#define COLOR_LIGHT_BLUE 9
#define COLOR_LIGHT_GREEN 10
#define COLOR_LIGHT_CYAN 11
#define COLOR_LIGHT_RED 12
#define COLOR_LIGHT_MAGENTA 13
#define COLOR_YELLOW 14
#define COLOR_WHITE 15

// Framebuffer inicializálása
void fb_init();

// Pixel rajzolása
void fb_put_pixel(int x, int y, uint8_t color);

// Képernyő törlése
void fb_clear(uint8_t color);

// Vonal rajzolása
void fb_draw_line(int x0, int y0, int x1, int y1, uint8_t color);

// Téglalap rajzolása
void fb_draw_rect(int x, int y, int width, int height, uint8_t color);

// Kitöltött téglalap rajzolása
void fb_fill_rect(int x, int y, int width, int height, uint8_t color);

// Kör rajzolása
void fb_draw_circle(int x, int y, int radius, uint8_t color);

// Karakter rajzolása
void fb_draw_char(char c, int x, int y, uint8_t fg_color, uint8_t bg_color);

// Szöveg kiírása
void fb_write_string(const char* str, int x, int y, uint8_t fg_color, uint8_t bg_color);

#endif // FB_H