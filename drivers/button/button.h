#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>

void button_init(void);
bool is_green_button_pressed(void);
void keypad_init(void);
char keypad_scan(void);

#endif