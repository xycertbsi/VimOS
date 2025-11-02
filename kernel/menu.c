// menu.c - Menu bar implementation
#include "include/menu.h"
#include "include/stdint.h"
#include "include/vga.h"
extern volatile uint16_t* vga_buffer;


static const char* menu_items[MENU_ITEMS] = {
    "Calculator",
    "Notepad",
    "Terminal",
    "Files",
    "Exit"
};

static uint16_t make_vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

static void putchar_at(char c, uint8_t color, uint8_t x, uint8_t y) {
    if (x >= VGA_WIDTH) return;
    vga_buffer[y * VGA_WIDTH + x] = make_vga_entry(c, color);
}

static void puts_at(const char* str, uint8_t color, uint8_t x, uint8_t y) {
    for (uint8_t i = 0; str[i] != '\0'; i++) {
        putchar_at(str[i], color, x + i, y);
    }
}

static void fill_line(uint8_t y, char ch, uint8_t color) {
    for (uint8_t x = 0; x < VGA_WIDTH; x++) {
        putchar_at(ch, color, x, y);
    }
}

static int str_len(const char* str) {
    int len = 0;
    while (str[len] != '\0') len++;
    return len;
}

void init_menu(void) {
    // Menu initialized on first draw
}

void draw_menu(int selected) {
    // Draw menu bar background
    fill_line(0, ' ', VGA_COLOR(0, 7));
    
    if (selected < 0) {
        // No selection, just show VIN OS title
        puts_at("VIN OS v0.4.METAL.DEFA", VGA_COLOR(1, 7), 2, 0);
        puts_at("Press F1 for Menu", VGA_COLOR(8, 7), VGA_WIDTH - 19, 0);
        return;
    }
    
    // Draw menu items
    int x_pos = 2;
    for (int i = 0; i < MENU_ITEMS; i++) {
        uint8_t color;
        if (i == selected) {
            color = VGA_COLOR(15, 1); // White on blue for selected
        } else {
            color = VGA_COLOR(0, 7);  // Black on light gray
        }
        
        putchar_at(' ', color, x_pos++, 0);
        puts_at(menu_items[i], color, x_pos, 0);
        x_pos += str_len(menu_items[i]);
        putchar_at(' ', color, x_pos++, 0);
        
        putchar_at(' ', VGA_COLOR(0, 7), x_pos++, 0);
    }
}