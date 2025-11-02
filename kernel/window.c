// window.c - Window management implementation
#include "include/window.h"
#include "include/vga.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_COLOR(fg, bg) ((bg << 4) | fg)

extern volatile uint16_t* vga_buffer;



static Window windows[MAX_WINDOWS];
static int window_count = 0;
static int focused_window = -1;

static uint16_t make_vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

static void putchar_at(char c, uint8_t color, uint8_t x, uint8_t y) {
    if (x >= VGA_WIDTH || y >= VGA_HEIGHT) return;
    vga_buffer[y * VGA_WIDTH + x] = make_vga_entry(c, color);
}

static void puts_at(const char* str, uint8_t color, uint8_t x, uint8_t y) {
    for (uint8_t i = 0; str[i] != '\0' && x + i < VGA_WIDTH; i++) {
        putchar_at(str[i], color, x + i, y);
    }
}

static void draw_box(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color) {
    // Top border
    putchar_at('+', color, x, y);
    for (uint8_t i = 1; i < width - 1; i++) {
        putchar_at('-', color, x + i, y);
    }
    putchar_at('+', color, x + width - 1, y);
    
    // Sides
    for (uint8_t i = 1; i < height - 1; i++) {
        putchar_at('|', color, x, y + i);
        putchar_at('|', color, x + width - 1, y + i);
        // Fill interior
        for (uint8_t j = 1; j < width - 1; j++) {
            putchar_at(' ', color, x + j, y + i);
        }
    }
    
    // Bottom border
    putchar_at('+', color, x, y + height - 1);
    for (uint8_t i = 1; i < width - 1; i++) {
        putchar_at('-', color, x + i, y + height - 1);
    }
    putchar_at('+', color, x + width - 1, y + height - 1);
}

static void str_copy(char* dest, const char* src, int max_len) {
    int i = 0;
    while (src[i] != '\0' && i < max_len - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

void init_window_manager(void) {
    window_count = 0;
    focused_window = -1;
    for (int i = 0; i < MAX_WINDOWS; i++) {
        windows[i].active = 0;
        windows[i].focused = 0;
    }
}

int create_window(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const char* title, uint8_t color) {
    if (window_count >= MAX_WINDOWS) return -1;
    
    int id = -1;
    for (int i = 0; i < MAX_WINDOWS; i++) {
        if (!windows[i].active) {
            id = i;
            break;
        }
    }
    
    if (id < 0) return -1;
    
    windows[id].x = x;
    windows[id].y = y;
    windows[id].width = width;
    windows[id].height = height;
    windows[id].color = color;
    windows[id].active = 1;
    windows[id].focused = 0;
    windows[id].text_line_count = 0;
    
    str_copy(windows[id].title, title, 32);
    
    for (int i = 0; i < MAX_WINDOW_TEXT_LINES; i++) {
        windows[id].text_lines[i][0] = '\0';
    }
    
    window_count++;
    return id;
}

void close_window(int window_id) {
    if (window_id < 0 || window_id >= MAX_WINDOWS) return;
    if (!windows[window_id].active) return;
    
    windows[window_id].active = 0;
    windows[window_id].focused = 0;
    window_count--;
    
    if (focused_window == window_id) {
        focused_window = -1;
        // Find another window to focus
        for (int i = 0; i < MAX_WINDOWS; i++) {
            if (windows[i].active) {
                focus_window(i);
                break;
            }
        }
    }
}

void close_all_windows(void) {
    for (int i = 0; i < MAX_WINDOWS; i++) {
        if (windows[i].active) {
            windows[i].active = 0;
            windows[i].focused = 0;
        }
    }
    window_count = 0;
    focused_window = -1;
}

void draw_window(int window_id) {
    if (window_id < 0 || window_id >= MAX_WINDOWS) return;
    if (!windows[window_id].active) return;
    
    Window* win = &windows[window_id];
    
    // Determine border color based on focus
    uint8_t border_color = win->focused ? VGA_COLOR(15, 4) : win->color;
    
    // Draw window box
    draw_box(win->x, win->y, win->width, win->height, border_color);
    
    // Draw title bar
    int title_x = win->x + 2;
    puts_at(win->title, border_color, title_x, win->y);
    
    // Draw window content
    int content_y = win->y + 1;
    for (int i = 0; i < win->text_line_count && i < (win->height - 2); i++) {
        puts_at(win->text_lines[i], win->color, win->x + 1, content_y + i);
    }
}

void draw_all_windows(void) {
    // Draw unfocused windows first
    for (int i = 0; i < MAX_WINDOWS; i++) {
        if (windows[i].active && !windows[i].focused) {
            draw_window(i);
        }
    }
    
    // Draw focused window last (on top)
    if (focused_window >= 0 && focused_window < MAX_WINDOWS) {
        if (windows[focused_window].active) {
            draw_window(focused_window);
        }
    }
}

void focus_window(int window_id) {
    if (window_id < 0 || window_id >= MAX_WINDOWS) return;
    if (!windows[window_id].active) return;
    
    // Unfocus all windows
    for (int i = 0; i < MAX_WINDOWS; i++) {
        windows[i].focused = 0;
    }
    
    // Focus the selected window
    windows[window_id].focused = 1;
    focused_window = window_id;
}

void cycle_focus(void) {
    if (window_count == 0) return;
    
    int start = focused_window + 1;
    if (start >= MAX_WINDOWS) start = 0;
    
    for (int i = 0; i < MAX_WINDOWS; i++) {
        int idx = (start + i) % MAX_WINDOWS;
        if (windows[idx].active) {
            focus_window(idx);
            return;
        }
    }
}

int get_focused_window(void) {
    return focused_window;
}

Window* get_window(int window_id) {
    if (window_id < 0 || window_id >= MAX_WINDOWS) return 0;
    if (!windows[window_id].active) return 0;
    return &windows[window_id];
}

void add_window_text(int window_id, const char* text) {
    if (window_id < 0 || window_id >= MAX_WINDOWS) return;
    if (!windows[window_id].active) return;
    
    Window* win = &windows[window_id];
    
    if (win->text_line_count >= MAX_WINDOW_TEXT_LINES) return;
    
    str_copy(win->text_lines[win->text_line_count], text, 80);
    win->text_line_count++;
}