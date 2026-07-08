section .text
bits 32

extern isrExceptionHandler
extern irqHandler

%macro isr_err_stub 1
global isr_stub_%+%1
isr_stub_%+%1:
    push %1
    jmp isr_common
%endmacro

%macro isr_no_err_stub 1
global isr_stub_%+%1
isr_stub_%+%1:
    push 0
    push %1
    jmp isr_common
%endmacro

%macro irq_stub_macro 1
global irq_stub_%+%1
irq_stub_%+%1:
    push 0
    push (32 + %1)
    jmp irq_common
%endmacro


isr_common:
    pushad
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    push esp
    call isrExceptionHandler
    add esp, 4
    pop gs
    pop fs
    pop es
    pop ds
    popad
    add esp, 8
    iret

irq_common:
    pushad
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    push esp
    call irqHandler
    add esp, 4
    pop gs
    pop fs
    pop es
    pop ds
    popad
    add esp, 8
    iret

%assign i 0
%rep 32
    %if i == 8 || i == 10 || i == 11 || i == 12 || i == 13 || i == 14 || i == 17
        isr_err_stub i     ; Has error code
    %else
        isr_no_err_stub i  ; No error code
    %endif
%assign i i + 1
%endrep

%assign i 0
%rep 16
    irq_stub_macro i
%assign i i + 1
%endrep

section .data

global isr_stub_table
isr_stub_table:
%assign i 0
%rep 32
    dd isr_stub_%+i
%assign i i + 1
%endrep

global irq_stub_table
irq_stub_table:
%assign i 0
%rep 16
    dd irq_stub_%+i
%assign i i + 1
%endrep