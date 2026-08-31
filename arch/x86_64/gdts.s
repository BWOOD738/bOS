global gdtFlush

gdtFlush:
    mov eax, [esp+4] ; grab GDT pointer from top of stack 
    lgdt [eax]
    
    jmp 0x08:.flush ; This forces the CPU to update CS with new code selector (0x08)

.flush:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret

global tssFlush
tssFlush:
    mov ax, 0x28 ; 0x2B is offset associated with task segment 
    ltr ax ; load task register 
    ret 