; entry.asm - Kernel entry (already in protected mode from bootloader)
[bits 32]

global _start
extern kernel_main

section .text
_start:
    ; We're already in protected mode with stack at 0x90000
    ; Just call the kernel
    call kernel_main
    
    ; If kernel returns, halt
    cli
.hang:
    hlt
    jmp .hang