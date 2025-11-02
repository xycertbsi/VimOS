// filesystem.c - Simple in-memory file system implementation
#include "include/filesystem.h"

static FileSystem fs;

static void str_copy(char* dest, const char* src, int max_len) {
    int i = 0;
    while (src[i] != '\0' && i < max_len - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

static int str_compare(const char* s1, const char* s2) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) return 0;
        i++;
    }
    return s1[i] == s2[i];
}

void init_filesystem(void) {
    fs.file_count = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        fs.files[i].used = 0;
        fs.files[i].size = 0;
        fs.files[i].name[0] = '\0';
    }
}

int fs_create_file(const char* name) {
    // Check if file already exists
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].used && str_compare(fs.files[i].name, name)) {
            return i; // File already exists, return its index
        }
    }
    
    // Find empty slot
    for (int i = 0; i < MAX_FILES; i++) {
        if (!fs.files[i].used) {
            fs.files[i].used = 1;
            str_copy(fs.files[i].name, name, MAX_FILENAME);
            fs.files[i].size = 0;
            fs.file_count++;
            return i;
        }
    }
    
    return -1; // No space
}

int fs_write_file(const char* name, const char* data, int size) {
    if (size > MAX_FILE_SIZE) {
        size = MAX_FILE_SIZE;
    }
    
    int idx = fs_create_file(name);
    if (idx < 0) return -1;
    
    for (int i = 0; i < size; i++) {
        fs.files[idx].data[i] = data[i];
    }
    fs.files[idx].size = size;
    
    return size;
}

int fs_read_file(const char* name, char* buffer, int max_size) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].used && str_compare(fs.files[i].name, name)) {
            int size = fs.files[i].size;
            if (size > max_size) size = max_size;
            
            for (int j = 0; j < size; j++) {
                buffer[j] = fs.files[i].data[j];
            }
            
            return size;
        }
    }
    return -1; // File not found
}

int fs_delete_file(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].used && str_compare(fs.files[i].name, name)) {
            fs.files[i].used = 0;
            fs.files[i].size = 0;
            fs.file_count--;
            return 0;
        }
    }
    return -1; // File not found
}

int fs_list_files(char filenames[][MAX_FILENAME], int max_files) {
    int count = 0;
    for (int i = 0; i < MAX_FILES && count < max_files; i++) {
        if (fs.files[i].used) {
            str_copy(filenames[count], fs.files[i].name, MAX_FILENAME);
            count++;
        }
    }
    return count;
}

int fs_file_exists(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].used && str_compare(fs.files[i].name, name)) {
            return 1;
        }
    }
    return 0;
}

int fs_get_file_size(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].used && str_compare(fs.files[i].name, name)) {
            return fs.files[i].size;
        }
    }
    return -1;
}