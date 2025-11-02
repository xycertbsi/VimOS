// string.h - String kezelő függvények
#ifndef STRING_H
#define STRING_H

#include "stdint.h"
#include "types.h"

// Memóriaterület nullázása
void* memset(void* s, int c, size_t n);

// Memóriamásolás
void* memcpy(void* dest, const void* src, size_t n);

// String hossza
size_t strlen(const char* s);

// String másolás
char* strcpy(char* dest, const char* src);

// String összefűzés
char* strcat(char* dest, const char* src);

// String összehasonlítás
int strcmp(const char* s1, const char* s2);

#endif // STRING_H