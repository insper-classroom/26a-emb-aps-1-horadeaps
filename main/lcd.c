#include "lcd.h"
void lcd_init(){
    // stdio_init_all();
    // gpio_init(pin_g);
    // gpio_init(pin_b);
    // gpio_init(pin_r);
    // gpio_init(pin_y);
    // gpio_set_dir(pin_g, GPIO_OUT);
    // gpio_set_dir(pin_b, GPIO_OUT);
    // gpio_set_dir(pin_r, GPIO_OUT);
    // gpio_set_dir(pin_y, GPIO_OUT);

}

void set_colour(int cor){
    int largura = 150;
    int altura =120;
    int pos_x = 105;
    int pos_y = 70;
    if(cor == 1){
        // gpio_put(pin_g,1);
        // gfx_clear();
        gfx_fillRect(pos_x,pos_y, largura, altura, 0x0000);
        gfx_fillRect(pos_x,pos_y, largura, altura, 0x07E0);

    }
    if(cor == 2){
        // gpio_put(pin_b,1);
        // gfx_clear();
        gfx_fillRect(pos_x,pos_y, largura, altura, 0x0000);
        gfx_fillRect(pos_x,pos_y, largura, altura, 0x001F);

    }
    if(cor == 3){
        // gpio_put(pin_r,1);
        gfx_fillRect(pos_x,pos_y, largura, altura, 0x0000);
        // gfx_clear();
        gfx_fillRect(pos_x,pos_y, largura, altura, 0xF800);

    }
    if(cor == 4){
        // gpio_put(pin_y,1);
        // gfx_clear();
        gfx_fillRect(pos_x,pos_y, largura, altura, 0x0000);
        gfx_fillRect(pos_x,pos_y, largura, altura, 0xFFE0);

    }
}

void lose_colour(int cor){
    if(cor == 1){
        gpio_put(pin_g,0);
    }
    if(cor == 2){
        gpio_put(pin_b,0);
    }
    if(cor == 3){
        gpio_put(pin_r,0);
    }
    if(cor == 4){
        gpio_put(pin_y,0);
    }
}