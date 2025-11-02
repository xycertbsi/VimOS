// gdt.h - Global Descriptor Table kezelő fejléc
#ifndef GDT_H
#define GDT_H

#include "stdint.h"  // <stdint.h> helyett relatív elérési úttal

// GDT bejegyzés struktúra
typedef struct {
    uint16_t limit_low;     // Limit (0-15 bitek)
    uint16_t base_low;      // Base (0-15 bitek)
    uint8_t base_middle;    // Base (16-23 bitek)
    uint8_t access;         // Access byte
    uint8_t granularity;    // Limit (16-19 bitek) + flags
    uint8_t base_high;      // Base (24-31 bitek)
} __attribute__((packed)) gdt_entry_t;

// GDT pointer struktúra (GDTR regiszter számára)
typedef struct {
    uint16_t limit;         // GDT mérete (bájtokban - 1)
    uint32_t base;          // GDT báziscíme
} __attribute__((packed)) gdt_ptr_t;

// GDT inicializálása
void gdt_init();

// GDT bejegyzés beállítása
void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

// GDT betöltése (assembly függvény)
extern void gdt_flush(uint32_t gdt_ptr);  // Hiányzó paraméternév hozzáadva

#endif // GDT_H