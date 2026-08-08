set disassembly-flavor intel 

file bOS.elf

target remote localhost:1234

layout asm 
layout reg

set architecture i386:x86-64