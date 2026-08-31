section .text
bits 64

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
    push 0             ; Dummy error code
    push %1            ; Interrupt number
    jmp isr_common
%endmacro

%macro irq_stub_macro 1
global irq_stub_%+%1
irq_stub_%+%1:
    push 0             ; Dummy error code
    push (32 + %1)     ; Interrupt number
    jmp irq_common
%endmacro

isr_common:
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rbp
    push rdi
    push rsi
    push rdx
    push rcx
    push rbx
    push rax
    
 
    mov rdi, rsp        
    call isrExceptionHandler
    
    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    pop rbp
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15
    
    ; Remove error code and interrupt number
    add rsp, 16
    
    iretq

irq_common:
    ; Save all GPRs in order matching iregs_t structure
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rbp
    push rdi
    push rsi
    push rdx
    push rcx
    push rbx
    push rax
    
    mov rdi, rsp        ; First argument = pointer to register frame
    call irqHandler
    
    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    pop rbp
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15
    
    ; remove error code and interrupt number
    add rsp, 16
    
    iretq


%assign i 0
%rep 32
    %if i == 8 || i == 10 || i == 11 || i == 12 || i == 13 || i == 14 || i == 17
        isr_err_stub i
    %else
        isr_no_err_stub i
    %endif
%assign i i+1
%endrep

%assign i 0
%rep 16
    irq_stub_macro i
%assign i i+1
%endrep

section .data

global isr_stub_table
isr_stub_table:
%assign i 0
%rep 32
    dq isr_stub_%+i
%assign i i+1
%endrep

global irq_stub_table
irq_stub_table:
%assign i 0
%rep 16
    dq irq_stub_%+i
%assign i i+1
%endrep