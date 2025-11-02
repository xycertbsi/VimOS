// types.h - Alaptípusok definiálása
#ifndef TYPES_H
#define TYPES_H

#include "stdint.h"

// Boolean típus
typedef enum {
    false = 0,
    true = 1
} bool;

// NULL definíció
#define NULL ((void*)0)

// size_t típus - töröljük innen, mert már definiálva van a stdint.h-ban
// Töröld ezt a sort:
// typedef uint32_t size_t;

#endif // TYPES_H