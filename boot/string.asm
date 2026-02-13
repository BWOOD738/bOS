%ifndef STRING_ASM
%define STRING_ASM

print_str:
    pusha 
    mov ah, 0xe
    jmp read_char

read_char:
    mov al, [bx]
    cmp al, 0 ; check if null

    jne print_char
    popa
    ret

print_char:
    int 0x10
    add bx, 1 ; increment to next character
    jmp read_char

%endif