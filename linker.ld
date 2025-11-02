/* linker.ld - Linker script for VIN OS kernel */
OUTPUT_FORMAT("binary")
ENTRY(_start)

SECTIONS
{
    /* Kernel loads at 0x1000 */
    . = 0x1000;

    .text : {
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
        *(COMMON)
    }

    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.note.GNU-stack)
    }
}