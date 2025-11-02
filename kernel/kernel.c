// kernel.c - Main kernel implementation
#include "include/stdint.h"
#include "include/window.h"
#include "include/keyboard.h"
#include "include/menu.h"
#include "include/apps.h"
#include "include/filesystem.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_COLOR(fg, bg) ((bg << 4) | fg)

// VGA buffer - defined here, used everywhere
volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;

static int menu_active = 0;
static int menu_selection = 0;
static int move_mode = 0;

typedef struct {
    int window_id;
    int app_type;
    void* app_data;
} AppWindow;

static AppWindow app_windows[10];
static int app_window_count = 0;

// Helper function prototypes
static void update_calculator_display(Calculator* calc);
static void update_notepad_display(Notepad* notepad);
static void update_terminal_display(Terminal* term);
static void update_filemanager_display(FileManager* fm);
static void int_to_str(int num, char* str);

static void register_app_window(int window_id, int app_type, void* app_data) {
    if (app_window_count < 10) {
        app_windows[app_window_count].window_id = window_id;
        app_windows[app_window_count].app_type = app_type;
        app_windows[app_window_count].app_data = app_data;
        app_window_count++;
    }
}

static AppWindow* get_app_window(int window_id) {
    for (int i = 0; i < app_window_count; i++) {
        if (app_windows[i].window_id == window_id) {
            return &app_windows[i];
        }
    }
    return 0;
}

static void unregister_app_window(int window_id) {
    for (int i = 0; i < app_window_count; i++) {
        if (app_windows[i].window_id == window_id) {
            for (int j = i; j < app_window_count - 1; j++) {
                app_windows[j] = app_windows[j + 1];
            }
            app_window_count--;
            return;
        }
    }
}

static uint16_t make_vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

static void clear_screen(void) {
    for (int y = 1; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[y * VGA_WIDTH + x] = make_vga_entry(' ', VGA_COLOR(7, 1));
        }
    }
}

static void putchar_at(char c, uint8_t color, uint8_t x, uint8_t y) {
    if (x >= VGA_WIDTH || y >= VGA_HEIGHT) return;
    vga_buffer[y * VGA_WIDTH + x] = make_vga_entry(c, color);
}

static void puts_at(const char* str, uint8_t color, uint8_t x, uint8_t y) {
    for (uint8_t i = 0; str[i] != '\0'; i++) {
        putchar_at(str[i], color, x + i, y);
    }
}

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

static void draw_desktop_icons(void) {
    const char* icon1 = "[Calc]";
    const char* icon2 = "[Note]";
    const char* icon3 = "[Term]";
    const char* icon4 = "[File]";
    
    puts_at(icon1, VGA_COLOR(15, 1), 2, 2);
    puts_at(icon2, VGA_COLOR(15, 1), 2, 4);
    puts_at(icon3, VGA_COLOR(15, 1), 2, 6);
    puts_at(icon4, VGA_COLOR(15, 1), 2, 8);
    
    puts_at("F1:Menu TAB:Switch DEL:Close M:Move", VGA_COLOR(14, 1), 2, VGA_HEIGHT - 1);
}

static void update_calculator_display(Calculator* calc) {
    Window* win = get_window(calc->window_id);
    if (win == 0) return;
    
    win->text_line_count = 0;
    
    add_window_text(calc->window_id, "VIN Calculator v1.0");
    add_window_text(calc->window_id, "--------------------");
    
    char display_line[30] = "Display: ";
    int k = 9;
    for (int i = 0; i < calc->display_len && k < 28; i++) {
        display_line[k++] = calc->display[i];
    }
    display_line[k] = '\0';
    add_window_text(calc->window_id, display_line);
    
    add_window_text(calc->window_id, "");
    add_window_text(calc->window_id, "Use keyboard:");
    add_window_text(calc->window_id, "  0-9: Numbers");
    add_window_text(calc->window_id, "  +,-,*,/: Operators");
    add_window_text(calc->window_id, "  =: Calculate");
    add_window_text(calc->window_id, "  C: Clear");
}

static void update_notepad_display(Notepad* notepad) {
    Window* win = get_window(notepad->window_id);
    if (win == 0) return;
    
    win->text_line_count = 0;
    
    if (notepad->save_mode) {
        add_window_text(notepad->window_id, "Save As - Enter filename:");
        add_window_text(notepad->window_id, "");
        
        char prompt[60] = "Filename: ";
        int k = 10;
        for (int i = 0; i < notepad->save_cursor && k < 50; i++) {
            prompt[k++] = notepad->save_buffer[i];
        }
        prompt[k++] = '_';
        prompt[k] = '\0';
        add_window_text(notepad->window_id, prompt);
        add_window_text(notepad->window_id, "");
        add_window_text(notepad->window_id, "Press ENTER to save, ESC to cancel");
        return;
    }
    
    if (notepad->has_filename) {
        char title[60] = "File: ";
        int k = 6;
        for (int i = 0; notepad->filename[i] != '\0' && k < 50; i++) {
            title[k++] = notepad->filename[i];
        }
        title[k] = '\0';
        add_window_text(notepad->window_id, title);
    } else {
        add_window_text(notepad->window_id, "Unsaved Document");
    }
    
    add_window_text(notepad->window_id, "F2:Save  F3:Save As");
    add_window_text(notepad->window_id, "----------------------------");
    
    int start_line = 0;
    if (notepad->current_line > 8) {
        start_line = notepad->current_line - 8;
    }
    
    for (int i = start_line; i < notepad->line_count && i < start_line + 9; i++) {
        if (notepad->lines[i][0] == '\0') {
            add_window_text(notepad->window_id, " ");
        } else {
            add_window_text(notepad->window_id, notepad->lines[i]);
        }
    }
    
    if (notepad->current_line >= start_line && notepad->current_line < start_line + 9) {
        char cursor_line[60];
        int k = 0;
        for (int i = 0; i < notepad->cursor_pos && i < 58; i++) {
            cursor_line[k++] = notepad->lines[notepad->current_line][i];
        }
        cursor_line[k++] = '_';
        cursor_line[k] = '\0';
        
        if (notepad->current_line == notepad->line_count) {
            add_window_text(notepad->window_id, cursor_line);
        }
    }
}

static void update_terminal_display(Terminal* term) {
    Window* win = get_window(term->window_id);
    if (win == 0) return;
    
    win->text_line_count = 0;
    
    add_window_text(term->window_id, "VIN Terminal v0.4");
    add_window_text(term->window_id, "Type 'help' for commands");
    add_window_text(term->window_id, "========================");
    
    int start = 0;
    if (term->line_count > 12) {
        start = term->line_count - 12;
    }
    
    for (int i = start; i < term->line_count; i++) {
        add_window_text(term->window_id, term->lines[i]);
    }
    
    char input_line[62] = "> ";
    int k = 2;
    for (int i = 0; i < term->input_len && k < 60; i++) {
        input_line[k++] = term->input[i];
    }
    input_line[k++] = '_';
    input_line[k] = '\0';
    add_window_text(term->window_id, input_line);
}

static void update_filemanager_display(FileManager* fm) {
    Window* win = get_window(fm->window_id);
    if (win == 0) return;
    
    win->text_line_count = 0;
    
    add_window_text(fm->window_id, "VIN File Manager");
    add_window_text(fm->window_id, "--------------------------------");
    add_window_text(fm->window_id, "UP/DOWN: Navigate  ENTER: Open");
    add_window_text(fm->window_id, "DELETE: Remove file");
    add_window_text(fm->window_id, "");
    
    if (fm->file_count == 0) {
        add_window_text(fm->window_id, "No files found.");
        add_window_text(fm->window_id, "Create files in Notepad (F2)");
    } else {
        for (int i = 0; i < fm->file_count && i < 12; i++) {
            char line[60];
            if (i == fm->selected_file) {
                line[0] = '>';
                line[1] = ' ';
            } else {
                line[0] = ' ';
                line[1] = ' ';
            }
            
            int j = 2;
            for (int k = 0; fm->filenames[i][k] != '\0' && j < 50; k++) {
                line[j++] = fm->filenames[i][k];
            }
            
            int size = fs_get_file_size(fm->filenames[i]);
            if (size >= 0) {
                line[j++] = ' ';
                line[j++] = '(';
                char size_str[10];
                int_to_str(size, size_str);
                for (int k = 0; size_str[k] != '\0' && j < 55; k++) {
                    line[j++] = size_str[k];
                }
                line[j++] = 'B';
                line[j++] = ')';
            }
            
            line[j] = '\0';
            add_window_text(fm->window_id, line);
        }
    }
}

void kernel_main(void) {
    // Clear entire screen
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = make_vga_entry(' ', VGA_COLOR(7, 1));
    }
    
    // Initialize all systems
    init_keyboard();
    init_window_manager();
    init_menu();
    init_apps();
    init_filesystem();
    
    // Set up desktop
    clear_screen();
    draw_menu(-1);
    draw_desktop_icons();
    
    int redraw = 1;
    int active_count = 0;
    
    while (1) {
        if (redraw) {
            draw_all_windows();
            draw_desktop_icons();
            redraw = 0;
        }
        
        if (has_key_event()) {
            uint8_t scancode = get_key_event();
            
            if (scancode == KEY_F1) {
                menu_active = !menu_active;
                if (menu_active) {
                    menu_selection = 0;
                    draw_menu(menu_selection);
                } else {
                    draw_menu(-1);
                }
                continue;
            }
            
            if (menu_active) {
                if (scancode == KEY_LEFT && menu_selection > 0) {
                    menu_selection--;
                    draw_menu(menu_selection);
                }
                else if (scancode == KEY_RIGHT && menu_selection < 4) {
                    menu_selection++;
                    draw_menu(menu_selection);
                }
                else if (scancode == KEY_ENTER) {
                    menu_active = 0;
                    draw_menu(-1);
                    
                    int new_win = -1;
                    
                    switch (menu_selection) {
                        case 0: {
                            Calculator* calc = get_calculator_instance();
                            if (calc) {
                                new_win = launch_calculator();
                                if (new_win >= 0) {
                                    calc->window_id = new_win;
                                    register_app_window(new_win, APP_CALCULATOR, (void*)calc);
                                    update_calculator_display(calc);
                                    active_count++;
                                    focus_window(new_win);
                                    redraw = 1;
                                }
                            }
                            break;
                        }
                        case 1: {
                            Notepad* notepad = get_notepad_instance();
                            if (notepad) {
                                new_win = launch_notepad();
                                if (new_win >= 0) {
                                    notepad->window_id = new_win;
                                    register_app_window(new_win, APP_NOTEPAD, (void*)notepad);
                                    update_notepad_display(notepad);
                                    active_count++;
                                    focus_window(new_win);
                                    redraw = 1;
                                }
                            }
                            break;
                        }
                        case 2: {
                            Terminal* term = get_terminal_instance();
                            if (term) {
                                new_win = launch_terminal();
                                if (new_win >= 0) {
                                    term->window_id = new_win;
                                    register_app_window(new_win, APP_TERMINAL, (void*)term);
                                    update_terminal_display(term);
                                    active_count++;
                                    focus_window(new_win);
                                    redraw = 1;
                                }
                            }
                            break;
                        }
                        case 3: {
                            FileManager* fm = get_filemanager_instance();
                            if (fm) {
                                new_win = launch_filemanager();
                                if (new_win >= 0) {
                                    fm->window_id = new_win;
                                    register_app_window(new_win, APP_FILEMANAGER, (void*)fm);
                                    update_filemanager_display(fm);
                                    active_count++;
                                    focus_window(new_win);
                                    redraw = 1;
                                }
                            }
                            break;
                        }
                        case 4: {
                            close_all_windows();
                            active_count = 0;
                            app_window_count = 0;
                            clear_screen();
                            draw_desktop_icons();
                            redraw = 1;
                            break;
                        }
                    }
                }
                else if (scancode == KEY_ESC) {
                    menu_active = 0;
                    draw_menu(-1);
                }
                continue;
            }
            
            // M key for move mode
            char c = scancode_to_char(scancode);
            if ((c == 'm' || c == 'M')) {
                move_mode = !move_mode;
                clear_screen();
                if (move_mode) {
                    puts_at("MOVE MODE - Arrows to move, M to exit", VGA_COLOR(0, 14), 2, VGA_HEIGHT - 1);
                } else {
                    draw_desktop_icons();
                }
                redraw = 1;
                continue;
            }
            
            // Handle move mode
            if (move_mode) {
                int focused = get_focused_window();
                if (focused >= 0) {
                    Window* win = get_window(focused);
                    if (win) {
                        int moved = 0;
                        if (scancode == KEY_UP && win->y > 1) {
                            win->y--;
                            moved = 1;
                        }
                        else if (scancode == KEY_DOWN && win->y + win->height < VGA_HEIGHT - 1) {
                            win->y++;
                            moved = 1;
                        }
                        else if (scancode == KEY_LEFT && win->x > 0) {
                            win->x--;
                            moved = 1;
                        }
                        else if (scancode == KEY_RIGHT && win->x + win->width < VGA_WIDTH) {
                            win->x++;
                            moved = 1;
                        }
                        
                        if (moved) {
                            clear_screen();
                            puts_at("MOVE MODE - Arrows to move, M to exit", VGA_COLOR(0, 14), 2, VGA_HEIGHT - 1);
                            redraw = 1;
                        }
                    }
                }
                continue;
            }
            
            // DELETE closes window
            if (scancode == KEY_DELETE || scancode == KEY_F4) {
                int focused = get_focused_window();
                if (focused >= 0) {
                    unregister_app_window(focused);
                    close_window(focused);
                    active_count--;
                    if (active_count == 0) {
                        clear_screen();
                        draw_desktop_icons();
                    }
                    redraw = 1;
                }
                continue;
            }
            
            // TAB switches windows
            if (scancode == KEY_TAB) {
                cycle_focus();
                redraw = 1;
                continue;
            }
            
            // Send to active app
            int focused = get_focused_window();
            if (focused >= 0) {
                AppWindow* app_win = get_app_window(focused);
                if (app_win) {
                    if (app_win->app_type == APP_CALCULATOR) {
                        Calculator* calc = (Calculator*)app_win->app_data;
                        handle_calculator_key(calc, scancode);
                        update_calculator_display(calc);
                        redraw = 1;
                    }
                    else if (app_win->app_type == APP_NOTEPAD) {
                        Notepad* notepad = (Notepad*)app_win->app_data;
                        handle_notepad_key(notepad, scancode);
                        update_notepad_display(notepad);
                        redraw = 1;
                    }
                    else if (app_win->app_type == APP_TERMINAL) {
                        Terminal* term = (Terminal*)app_win->app_data;
                        handle_terminal_key(term, scancode);
                        update_terminal_display(term);
                        redraw = 1;
                    }
                    else if (app_win->app_type == APP_FILEMANAGER) {
                        FileManager* fm = (FileManager*)app_win->app_data;
                        handle_filemanager_key(fm, scancode);
                        fm->file_count = fs_list_files(fm->filenames, 20);
                        update_filemanager_display(fm);
                        redraw = 1;
                    }
                }
            }
        }
        
        for (volatile int i = 0; i < 100000; i++);
    }
}