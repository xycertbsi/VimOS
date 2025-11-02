// keyboard.c - Keyboard input implementation
#include "include/keyboard.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

#define KEY_BUFFER_SIZE 16

static uint8_t key_buffer[KEY_BUFFER_SIZE];
static int key_buffer_start = 0;
static int key_buffer_end = 0;
static int key_buffer_count = 0;

static uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static int is_shift_pressed = 0;

void init_keyboard(void) {
    key_buffer_start = 0;
    key_buffer_end = 0;
    key_buffer_count = 0;
    is_shift_pressed = 0;
}

static void add_key_to_buffer(uint8_t scancode) {
    if (key_buffer_count < KEY_BUFFER_SIZE) {
        key_buffer[key_buffer_end] = scancode;
        key_buffer_end = (key_buffer_end + 1) % KEY_BUFFER_SIZE;
        key_buffer_count++;
    }
}

uint8_t read_scancode(void) {
    uint8_t status = inb(KEYBOARD_STATUS_PORT);
    
    if (status & 0x01) {
        uint8_t scancode = inb(KEYBOARD_DATA_PORT);
        
        // Track shift state
        if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
            is_shift_pressed = 1;
        }
        else if (scancode == (KEY_LSHIFT | 0x80) || scancode == (KEY_RSHIFT | 0x80)) {
            is_shift_pressed = 0;
        }
        
        // Only process key press (not release)
        if (!(scancode & 0x80)) {
            add_key_to_buffer(scancode);
        }
        
        return scancode;
    }
    
    return 0;
}

int has_key_event(void) {
    // Check for new scancodes
    read_scancode();
    return key_buffer_count > 0;
}

uint8_t get_key_event(void) {
    if (key_buffer_count == 0) {
        return 0;
    }
    
    uint8_t scancode = key_buffer[key_buffer_start];
    key_buffer_start = (key_buffer_start + 1) % KEY_BUFFER_SIZE;
    key_buffer_count--;
    
    return scancode;
}

char scancode_to_char(uint8_t scancode) {
    // Handle special keys
    if (scancode == KEY_ENTER) return '\n';
    if (scancode == KEY_BACKSPACE) return '\b';
    if (scancode == KEY_SPACE) return ' ';
    if (scancode == KEY_TAB) return '\t';
    if (scancode == KEY_ESC) return 27;
    
    // Regular keys without shift
    static const char scancode_to_ascii[] = {
        0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
        '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
        0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
        0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
        '*', 0, ' '
    };
    
    // Regular keys with shift
    static const char scancode_to_ascii_shift[] = {
        0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
        '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
        0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
        0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
        '*', 0, ' '
    };
    
    if (scancode < sizeof(scancode_to_ascii)) {
        if (is_shift_pressed) {
            return scancode_to_ascii_shift[scancode];
        } else {
            return scancode_to_ascii[scancode];
        }
    }
    
    return 0;
}