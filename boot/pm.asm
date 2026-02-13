[bits 16]

mov ax, 0
mov ds, ax
mov es, ax

pm_switch:
    cli 
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    jmp CODE_SEG:pm_init

[bits 32]

pm_init:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax 

    mov ebp, 0x90000
    mov esp, ebp

    call pm_begin ; Entry to 32 bit code 