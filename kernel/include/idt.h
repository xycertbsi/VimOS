// idt.h - Interrupt Descriptor Table kezelő fejléc
#ifndef IDT_H
#define IDT_H

#include "stdint.h"  // <stdint.h> helyett relatív elérési úttal

// IDT bejegyzés struktúra
typedef struct {
    uint16_t base_low;      // Handler függvény címének alsó 16 bitje
    uint16_t sel;           // Kernel szegmens szelektor
    uint8_t always0;        // Mindig 0
    uint8_t flags;          // Flags
    uint16_t base_high;     // Handler függvény címének felső 16 bitje
} __attribute__((packed)) idt_entry_t;

// IDT pointer struktúra (IDTR regiszter számára)
typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

// IDT inicializálása
void idt_init();

// IDT bejegyzés beállítása
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

// IDT betöltése (assembly függvény)
extern void idt_flush(uint32_t idt_ptr);  // Hiányzó paraméternév hozzáadva

#endif // IDT_H