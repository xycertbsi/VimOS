[org 0x7c00]
[bits 16]

start:
    ; Set up segments
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti
    
    ; Print start message
    mov si, msg_start
    call print

    xor ax, ax
    mov dl, 0x80        
    int 0x13
    jc disk_error
    
    mov ah, 0x02        
    mov al, 32          
    mov cx, 0x0002      
    mov dh, 0x00        
    mov dl, 0x80        
    mov bx, 0x1000      
    int 0x13
    
    jc disk_error
    mov si, msg_loaded
    call print
    
    cli
    lgdt [gdt_descriptor]
    
    mov eax, cr0
    or al, 1
    mov cr0, eax
    
    jmp CODE_SEG:init_pm

disk_error:
    mov si, msg_error
    call print
.hang:
    hlt
    jmp .hang

print:
    pusha
    mov ah, 0x0E
.loop:
    lodsb
    cmp al, 0
    je .done
    int 0x10
    jmp .loop
.done:
    popa
    ret

; GDT
gdt_start:
gdt_null:
    dd 0x0
    dd 0x0

gdt_code:
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10011010b
    db 11001111b
    db 0x0

gdt_data:
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

[bits 32]
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    mov ebp, 0x90000
    mov esp, ebp
    
    jmp 0x1000

[bits 16]
msg_start: db 'VIN OS Boot', 13, 10, 0
msg_loaded: db 'Loaded', 13, 10, 0
msg_error: db 'Disk Error!', 13, 10, 0

times 510-($-$$) db 0
dw 0xAA55