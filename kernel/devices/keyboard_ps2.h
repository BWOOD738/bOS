#pragma once

#define KEYBOARD_IRQ 1
#define PS2_DATA 0x60
#define PS2_CMD 0x64

void initKeyboardPS2();

void keyboardDisablePS2();
