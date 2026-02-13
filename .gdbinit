set disassembly-flavor intel 

target remote localhost:1234

layout asm 
layout reg

b* 0x7c00

set architecture i8086 