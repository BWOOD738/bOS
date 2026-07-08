MBALIGN equ 1<<0 ; align modules on page boundaries
MEMINFO equ 1<<1
MB_USE_GFX equ 0
MBFLAGS equ MBALIGN | MEMINFO | MB_USE_GFX
MAGIC equ 0x1BADB002 ; magic number for multiboot. Change for multiboot2
CHECKSUM equ -(MAGIC + MBFLAGS)

section .multiboot
align 4
    dd MAGIC
    dd MBFLAGS
    dd CHECKSUM
    dd 0, 0, 0, 0, 0

    ; For graphics 
    dd 0
    dd 800
    dd 600
    dd 32

section .bss 
align 16
stack_bottom:
resb 16384 ; reserve 16kb of stack space
stack_top:

section .text
global _start

extern kmain
_start:
    push ebx ; Save multiboot info pointer

    mov esp, stack_top
    and esp, 0xFFFFFFF0

    push ebx
    push MAGIC

    call kmain

    cli

.hang: hlt
    jmp .hang
.end: