#ifndef LCD_H
#define LCD_H

#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define pin_g 2
#define pin_b 3
#define pin_r 4
#define pin_y 5

void lcd_init();
void set_colour(int cor);
void lose_colour(int cor);
#endif
