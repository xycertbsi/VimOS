// filesystem.h - Simple in-memory file system
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "stdint.h"

#define MAX_FILES 20
#define MAX_FILENAME 32
#define MAX_FILE_SIZE 2048

typedef struct {
    char name[MAX_FILENAME];
    char data[MAX_FILE_SIZE];
    int size;
    int used;
} File;

typedef struct {
    File files[MAX_FILES];
    int file_count;
} FileSystem;

void init_filesystem(void);
int fs_create_file(const char* name);
int fs_write_file(const char* name, const char* data, int size);
int fs_read_file(const char* name, char* buffer, int max_size);
int fs_delete_file(const char* name);
int fs_list_files(char filenames[][MAX_FILENAME], int max_files);
int fs_file_exists(const char* name);
int fs_get_file_size(const char* name);

#endif