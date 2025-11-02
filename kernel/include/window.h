// window.h - Window management system
#ifndef WINDOW_H
#define WINDOW_H

#include "stdint.h"

#define MAX_WINDOWS 10
#define MAX_WINDOW_TEXT_LINES 20

typedef struct {
    uint8_t x, y;           // Position
    uint8_t width, height;  // Dimensions
    char title[32];         // Window title
    uint8_t color;          // Window color
    int active;             // Is window active
    int focused;            // Is window focused
    char text_lines[MAX_WINDOW_TEXT_LINES][80];
    int text_line_count;    // Number of text lines
} Window;

void init_window_manager(void);
int create_window(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const char* title, uint8_t color);
void close_window(int window_id);
void close_all_windows(void);
void draw_window(int window_id);
void draw_all_windows(void);
void focus_window(int window_id);
void cycle_focus(void);
int get_focused_window(void);
Window* get_window(int window_id);
void add_window_text(int window_id, const char* text);

#endif