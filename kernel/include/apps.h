// apps.h - Application definitions
#ifndef APPS_H
#define APPS_H

#include "stdint.h"

// App types
#define APP_CALCULATOR 1
#define APP_NOTEPAD 2
#define APP_TERMINAL 3
#define APP_FILEMANAGER 4

// Calculator state
typedef struct {
    int window_id;
    char display[20];
    int display_len;
    double value1;
    double value2;
    char operation;
    int new_number;
} Calculator;

// Notepad state
typedef struct {
    int window_id;
    char lines[20][60];
    int line_count;
    int current_line;
    int cursor_pos;
    char filename[32];
    int has_filename;
    int save_mode;  // 0=edit, 1=save dialog
    char save_buffer[32];
    int save_cursor;
} Notepad;

// Terminal state
typedef struct {
    int window_id;
    char lines[18][60];
    int line_count;
    char input[60];
    int input_len;
} Terminal;

// File Manager state
typedef struct {
    int window_id;
    int selected_file;
    int file_count;
    char filenames[20][32];
} FileManager;

// App manager
void init_apps(void);
int launch_calculator(void);
int launch_notepad(void);
int launch_terminal(void);
int launch_filemanager(void);

// Instance getters
Calculator* get_calculator_instance(void);
Notepad* get_notepad_instance(void);
Terminal* get_terminal_instance(void);
FileManager* get_filemanager_instance(void);

void handle_calculator_key(Calculator* calc, uint8_t scancode);
void handle_notepad_key(Notepad* notepad, uint8_t scancode);
void handle_terminal_key(Terminal* term, uint8_t scancode);
void handle_filemanager_key(FileManager* fm, uint8_t scancode);

#endif