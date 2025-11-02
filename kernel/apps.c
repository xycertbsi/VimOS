// apps.c - Application implementations
#include "include/apps.h"
#include "include/window.h"
#include "include/keyboard.h"
#include "include/filesystem.h"

#define VGA_COLOR(fg, bg) ((bg << 4) | fg)

static Calculator calculators[5];
static Notepad notepads[5];
static Terminal terminals[5];
static FileManager filemanagers[5];
static int calc_count = 0;
static int notepad_count = 0;
static int terminal_count = 0;
static int fm_count = 0;

static void str_copy(char* dest, const char* src, int max_len) {
    int i = 0;
    while (src[i] != '\0' && i < max_len - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

static int str_len(const char* str) {
    int len = 0;
    while (str[len] != '\0') len++;
    return len;
}

void init_apps(void) {
    calc_count = 0;
    notepad_count = 0;
    terminal_count = 0;
    fm_count = 0;
    
    // Initialize all instances
    for (int i = 0; i < 5; i++) {
        calculators[i].window_id = -1;
        notepads[i].window_id = -1;
        terminals[i].window_id = -1;
        filemanagers[i].window_id = -1;
    }
}

// Instance getters
Calculator* get_calculator_instance(void) {
    if (calc_count >= 5) return 0;
    return &calculators[calc_count];
}

Notepad* get_notepad_instance(void) {
    if (notepad_count >= 5) return 0;
    return &notepads[notepad_count];
}

Terminal* get_terminal_instance(void) {
    if (terminal_count >= 5) return 0;
    return &terminals[terminal_count];
}

FileManager* get_filemanager_instance(void) {
    if (fm_count >= 5) return 0;
    return &filemanagers[fm_count];
}

// Simple integer to string conversion
static void int_to_str(int num, char* str) {
    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }
    
    int is_negative = 0;
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }
    
    char temp[20];
    int i = 0;
    while (num > 0) {
        temp[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    int j = 0;
    if (is_negative) str[j++] = '-';
    
    while (i > 0) {
        str[j++] = temp[--i];
    }
    str[j] = '\0';
}

int launch_calculator(void) {
    if (calc_count >= 5) return -1;
    
    int win_id = create_window(10 + calc_count * 2, 5 + calc_count * 2, 
                               28, 12, " Calculator ", VGA_COLOR(0, 15));
    if (win_id < 0) return -1;
    
    Calculator* calc = &calculators[calc_count];
    calc->window_id = win_id;
    calc->display[0] = '0';
    calc->display[1] = '\0';
    calc->display_len = 1;
    calc->value1 = 0;
    calc->value2 = 0;
    calc->operation = 0;
    calc->new_number = 1;
    
    calc_count++;
    return win_id;
}

int launch_notepad(void) {
    if (notepad_count >= 5) return -1;
    
    int win_id = create_window(15 + notepad_count * 2, 8 + notepad_count * 2,
                               50, 15, " Notepad ", VGA_COLOR(0, 11));
    if (win_id < 0) return -1;
    
    Notepad* notepad = &notepads[notepad_count];
    notepad->window_id = win_id;
    notepad->line_count = 1;
    notepad->current_line = 0;
    notepad->cursor_pos = 0;
    notepad->has_filename = 0;
    notepad->save_mode = 0;
    notepad->save_cursor = 0;
    notepad->filename[0] = '\0';
    notepad->save_buffer[0] = '\0';
    
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 60; j++) {
            notepad->lines[i][j] = '\0';
        }
    }
    
    notepad_count++;
    return win_id;
}

int launch_terminal(void) {
    if (terminal_count >= 5) return -1;
    
    int win_id = create_window(8 + terminal_count * 2, 4 + terminal_count * 2,
                               60, 18, " VIN Terminal ", VGA_COLOR(15, 0));
    if (win_id < 0) return -1;
    
    Terminal* term = &terminals[terminal_count];
    term->window_id = win_id;
    term->line_count = 0;
    term->input_len = 0;
    
    for (int i = 0; i < 18; i++) {
        for (int j = 0; j < 60; j++) {
            term->lines[i][j] = '\0';
        }
    }
    for (int i = 0; i < 60; i++) {
        term->input[i] = '\0';
    }
    
    terminal_count++;
    return win_id;
}

int launch_filemanager(void) {
    if (fm_count >= 5) return -1;
    
    int win_id = create_window(12 + fm_count * 2, 6 + fm_count * 2,
                               55, 18, " File Manager ", VGA_COLOR(15, 3));
    if (win_id < 0) return -1;
    
    FileManager* fm = &filemanagers[fm_count];
    fm->window_id = win_id;
    fm->selected_file = 0;
    fm->file_count = fs_list_files(fm->filenames, 20);
    
    fm_count++;
    return win_id;
}

void handle_calculator_key(Calculator* calc, uint8_t scancode) {
    char c = scancode_to_char(scancode);
    
    if (c >= '0' && c <= '9') {
        if (calc->new_number) {
            calc->display[0] = c;
            calc->display[1] = '\0';
            calc->display_len = 1;
            calc->new_number = 0;
        } else if (calc->display_len < 18) {
            calc->display[calc->display_len++] = c;
            calc->display[calc->display_len] = '\0';
        }
    }
    else if (c == '+' || c == '-' || c == '*' || c == '/') {
        calc->value1 = 0;
        for (int i = 0; i < calc->display_len; i++) {
            calc->value1 = calc->value1 * 10 + (calc->display[i] - '0');
        }
        calc->operation = c;
        calc->new_number = 1;
    }
    else if (c == '=' || c == '\n') {
        calc->value2 = 0;
        for (int i = 0; i < calc->display_len; i++) {
            calc->value2 = calc->value2 * 10 + (calc->display[i] - '0');
        }
        
        int result = 0;
        if (calc->operation == '+') result = calc->value1 + calc->value2;
        else if (calc->operation == '-') result = calc->value1 - calc->value2;
        else if (calc->operation == '*') result = calc->value1 * calc->value2;
        else if (calc->operation == '/' && calc->value2 != 0) result = calc->value1 / calc->value2;
        
        int_to_str(result, calc->display);
        calc->display_len = 0;
        while (calc->display[calc->display_len] != '\0') calc->display_len++;
        calc->new_number = 1;
    }
    else if (c == 'c' || c == 'C') {
        calc->display[0] = '0';
        calc->display[1] = '\0';
        calc->display_len = 1;
        calc->value1 = 0;
        calc->value2 = 0;
        calc->operation = 0;
        calc->new_number = 1;
    }
}

void handle_notepad_key(Notepad* notepad, uint8_t scancode) {
    // F2 = Save, F3 = Save As
    if (scancode == KEY_F2) {
        if (notepad->has_filename) {
            // Save to existing file
            char buffer[2048];
            int pos = 0;
            for (int i = 0; i < notepad->line_count && pos < 2000; i++) {
                int j = 0;
                while (notepad->lines[i][j] != '\0' && pos < 2000) {
                    buffer[pos++] = notepad->lines[i][j++];
                }
                if (i < notepad->line_count - 1) {
                    buffer[pos++] = '\n';
                }
            }
            fs_write_file(notepad->filename, buffer, pos);
        } else {
            // Enter save mode
            notepad->save_mode = 1;
            notepad->save_cursor = 0;
            notepad->save_buffer[0] = '\0';
        }
        return;
    }
    
    if (scancode == KEY_F3) {
        // Save As
        notepad->save_mode = 1;
        notepad->save_cursor = 0;
        notepad->save_buffer[0] = '\0';
        return;
    }
    
    if (notepad->save_mode) {
        // Handle save dialog input
        char c = scancode_to_char(scancode);
        
        if (c == '\n') {
            // Confirm save
            if (notepad->save_cursor > 0) {
                str_copy(notepad->filename, notepad->save_buffer, 32);
                notepad->has_filename = 1;
                
                // Save file
                char buffer[2048];
                int pos = 0;
                for (int i = 0; i < notepad->line_count && pos < 2000; i++) {
                    int j = 0;
                    while (notepad->lines[i][j] != '\0' && pos < 2000) {
                        buffer[pos++] = notepad->lines[i][j++];
                    }
                    if (i < notepad->line_count - 1) {
                        buffer[pos++] = '\n';
                    }
                }
                fs_write_file(notepad->filename, buffer, pos);
            }
            notepad->save_mode = 0;
        }
        else if (c == '\b' && notepad->save_cursor > 0) {
            notepad->save_cursor--;
            notepad->save_buffer[notepad->save_cursor] = '\0';
        }
        else if (c >= 32 && c <= 126 && notepad->save_cursor < 30) {
            notepad->save_buffer[notepad->save_cursor++] = c;
            notepad->save_buffer[notepad->save_cursor] = '\0';
        }
        else if (scancode == KEY_ESC) {
            notepad->save_mode = 0;
        }
        return;
    }
    
    // Normal edit mode
    char c = scancode_to_char(scancode);
    
    if (c >= 32 && c <= 126 && notepad->cursor_pos < 55) {
        notepad->lines[notepad->current_line][notepad->cursor_pos++] = c;
        notepad->lines[notepad->current_line][notepad->cursor_pos] = '\0';
    }
    else if (c == '\b' && notepad->cursor_pos > 0) {
        notepad->cursor_pos--;
        notepad->lines[notepad->current_line][notepad->cursor_pos] = '\0';
    }
    else if (c == '\n' && notepad->current_line < 19) {
        notepad->current_line++;
        notepad->cursor_pos = 0;
        if (notepad->current_line >= notepad->line_count) {
            notepad->line_count = notepad->current_line + 1;
        }
    }
}

void handle_terminal_key(Terminal* term, uint8_t scancode) {
    char c = scancode_to_char(scancode);
    
    if (c == '\n') {
        if (term->input_len > 0) {
            if (term->line_count < 16) {
                char cmd_line[60] = "> ";
                int k = 2;
                for (int i = 0; i < term->input_len && k < 58; i++) {
                    cmd_line[k++] = term->input[i];
                }
                cmd_line[k] = '\0';
                
                for (int i = 0; cmd_line[i] != '\0' && i < 60; i++) {
                    term->lines[term->line_count][i] = cmd_line[i];
                }
                term->lines[term->line_count][term->input_len + 2] = '\0';
                term->line_count++;
            }
            
            // Command handling
            if (term->input[0] == 'h' && term->input[1] == 'e' && 
                term->input[2] == 'l' && term->input[3] == 'p') {
                if (term->line_count < 16) {
                    char* help1 = "Available commands:";
                    char* help2 = "  help  - Show this help";
                    char* help3 = "  ver   - Show version";
                    char* help4 = "  clear - Clear screen";
                    char* help5 = "  ls    - List files";
                    
                    int i;
                    for (i = 0; help1[i] != '\0'; i++) term->lines[term->line_count][i] = help1[i];
                    term->lines[term->line_count++][i] = '\0';
                    
                    for (i = 0; help2[i] != '\0'; i++) term->lines[term->line_count][i] = help2[i];
                    term->lines[term->line_count++][i] = '\0';
                    
                    for (i = 0; help3[i] != '\0'; i++) term->lines[term->line_count][i] = help3[i];
                    term->lines[term->line_count++][i] = '\0';
                    
                    for (i = 0; help4[i] != '\0'; i++) term->lines[term->line_count][i] = help4[i];
                    term->lines[term->line_count++][i] = '\0';
                    
                    for (i = 0; help5[i] != '\0'; i++) term->lines[term->line_count][i] = help5[i];
                    term->lines[term->line_count++][i] = '\0';
                }
            }
            else if (term->input[0] == 'v' && term->input[1] == 'e' && 
                     term->input[2] == 'r') {
                if (term->line_count < 16) {
                    char* ver = "VIN OS v0.4 - File System Edition";
                    int i;
                    for (i = 0; ver[i] != '\0'; i++) {
                        term->lines[term->line_count][i] = ver[i];
                    }
                    term->lines[term->line_count++][i] = '\0';
                }
            }
            else if (term->input[0] == 'l' && term->input[1] == 's') {
                char filenames[20][32];
                int count = fs_list_files(filenames, 20);
                
                if (count == 0) {
                    char* msg = "No files found.";
                    int i;
                    for (i = 0; msg[i] != '\0'; i++) {
                        term->lines[term->line_count][i] = msg[i];
                    }
                    term->lines[term->line_count++][i] = '\0';
                } else {
                    for (int f = 0; f < count && term->line_count < 16; f++) {
                        int i;
                        for (i = 0; filenames[f][i] != '\0'; i++) {
                            term->lines[term->line_count][i] = filenames[f][i];
                        }
                        term->lines[term->line_count++][i] = '\0';
                    }
                }
            }
            else if (term->input[0] == 'c' && term->input[1] == 'l' && 
                     term->input[2] == 'e' && term->input[3] == 'a' &&
                     term->input[4] == 'r') {
                term->line_count = 0;
                for (int i = 0; i < 18; i++) {
                    term->lines[i][0] = '\0';
                }
            }
            else if (term->input_len > 0) {
                if (term->line_count < 16) {
                    char* unknown = "Unknown command. Type 'help'";
                    int i;
                    for (i = 0; unknown[i] != '\0'; i++) {
                        term->lines[term->line_count][i] = unknown[i];
                    }
                    term->lines[term->line_count++][i] = '\0';
                }
            }
            
            term->input_len = 0;
            term->input[0] = '\0';
        }
    }
    else if (c == '\b' && term->input_len > 0) {
        term->input_len--;
        term->input[term->input_len] = '\0';
    }
    else if (c >= 32 && c <= 126 && term->input_len < 58) {
        term->input[term->input_len++] = c;
        term->input[term->input_len] = '\0';
    }
}

void handle_filemanager_key(FileManager* fm, uint8_t scancode) {
    if (scancode == KEY_UP) {
        if (fm->selected_file > 0) {
            fm->selected_file--;
        }
    }
    else if (scancode == KEY_DOWN) {
        if (fm->selected_file < fm->file_count - 1) {
            fm->selected_file++;
        }
    }
    else if (scancode == KEY_DELETE) {
        // Delete selected file
        if (fm->file_count > 0) {
            fs_delete_file(fm->filenames[fm->selected_file]);
            fm->file_count = fs_list_files(fm->filenames, 20);
            if (fm->selected_file >= fm->file_count && fm->file_count > 0) {
                fm->selected_file = fm->file_count - 1;
            }
        }
    }
    else if (scancode == KEY_ENTER) {
        // Open file in notepad
        if (fm->file_count > 0) {
            Notepad* notepad = get_notepad_instance();
            if (notepad) {
                int win_id = launch_notepad();
                if (win_id >= 0) {
                    // Load file content
                    char buffer[2048];
                    int size = fs_read_file(fm->filenames[fm->selected_file], buffer, 2048);
                    
                    if (size > 0) {
                        str_copy(notepad->filename, fm->filenames[fm->selected_file], 32);
                        notepad->has_filename = 1;
                        
                        // Parse buffer into lines
                        int line = 0;
                        int pos = 0;
                        for (int i = 0; i < size && line < 20; i++) {
                            if (buffer[i] == '\n') {
                                notepad->lines[line][pos] = '\0';
                                line++;
                                pos = 0;
                            } else if (pos < 59) {
                                notepad->lines[line][pos++] = buffer[i];
                            }
                        }
                        notepad->lines[line][pos] = '\0';
                        notepad->line_count = line + 1;
                        notepad->current_line = 0;
                        notepad->cursor_pos = 0;
                    }
                }
            }
        }
    }
}