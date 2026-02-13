[org 0x7c00] 

[bits 16]

mov [BOOT_DRIVE], dl

mov bp, 0x9000
xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax 
mov sp, bp

mov bx, MSG_REAL_MODE
call print_str

call pm_switch ; We will never return from here 

jmp $

%include "string.asm"
%include "print_hex.asm"
%include "gdt.asm"
%include "string_pm.asm"
%include "pm.asm"

DISK_ERROR_MSG db "Disk read error !" , 0

load_disk:
    push dx 
    mov ah, 0x02 ; BIOS read sector function
    mov al, dh ; Read DH sectors
    mov ch, 0x00 ; Select cylinder 0
    mov dh, 0x00 ; Select head 0
    mov cl, 0x02 ; Start reading from second sector after boot sector 
    
    int 0x13 
    jc disk_error 
    
    pop dx 
    cmp al, dh ; if sectors read  != sectors expected 
    
    jne disk_error 

    ret
    
disk_error :
    mov bx , DISK_ERROR_MSG
    call print_str
    jmp $

[bits 32]

pm_begin:
    mov ebx, MSG_PRT_MODE
    call print_string_pm

    jmp $


MSG_REAL_MODE db "Started in 16 bit mode", 0
MSG_PRT_MODE db "Loaded into 32 bit mode", 0

BOOT_DRIVE: db 0 ; Global variable. 

times 510-($-$$) db 0
dw 0xAA55
