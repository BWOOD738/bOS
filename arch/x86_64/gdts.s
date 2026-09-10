global gdtFlush

gdtFlush:
    ; Save current state
    push rbx
    
    ; Load new GDT
    lgdt [rdi]
    
    ; Reload data segments first (before CS)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Now reload CS using far return
    push 0x08
    lea rax, [rel .flush]
    push rax
    retfq                      
    
.flush:
    ; Reload data segments
    mov ax, 0x10              
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret

global tssFlush

tssFlush:
    mov ax, 0x28               
    ltr ax                      ; Load task register
    ret