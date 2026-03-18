/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "lcd.h"
const int botao_inicio =14;
const int botao_g =10;
const int botao_b =11;
const int botao_r =12;
const int botao_y =13;



volatile int flag_jogo = 0;
volatile int flag_resposta_timeout = 0;


int64_t alarm_callback(alarm_id_t id, void *user_data)
{
    flag_resposta_timeout = 1;
    return 0;
}

void btn_callback(uint gpio, uint32_t events)
{
    if(events==0x4){
        if(gpio == botao_inicio){
            flag_jogo=1;
        }

        if(gpio == botao_g){
            flag_jogo=1;
        }

        if(gpio == botao_b){
            flag_jogo=1;
        }

        if(gpio == botao_r){
            flag_jogo=1;
        }

        if(gpio == botao_y){
            flag_jogo=1;
        }
    }
}
int main() {
    stdio_init_all();
    lcd_init();
    gpio_init(botao_inicio);

    gpio_init(botao_g);
    gpio_init(botao_b);
    gpio_init(botao_r);
    gpio_init(botao_y);

    gpio_set_dir(botao_inicio, GPIO_IN);

    gpio_set_dir(botao_g, GPIO_IN);
    gpio_set_dir(botao_b, GPIO_IN);
    gpio_set_dir(botao_r, GPIO_IN);
    gpio_set_dir(botao_r, GPIO_IN);

    gpio_pull_up(botao_inicio);

    gpio_pull_up(botao_g);
    gpio_pull_up(botao_b);
    gpio_pull_up(botao_r);
    gpio_pull_up(botao_y);
    
    gpio_set_irq_enabled_with_callback(botao_inicio, GPIO_IRQ_EDGE_RISE |GPIO_IRQ_EDGE_FALL , true, &btn_callback);
    
    gpio_set_irq_enabled_with_callback(botao_g, GPIO_IRQ_EDGE_RISE |GPIO_IRQ_EDGE_FALL , true, &btn_callback);
    gpio_set_irq_enabled_with_callback(botao_b, GPIO_IRQ_EDGE_RISE |GPIO_IRQ_EDGE_FALL , true, &btn_callback);
    gpio_set_irq_enabled_with_callback(botao_r, GPIO_IRQ_EDGE_RISE |GPIO_IRQ_EDGE_FALL , true, &btn_callback);
    gpio_set_irq_enabled_with_callback(botao_y, GPIO_IRQ_EDGE_RISE |GPIO_IRQ_EDGE_FALL , true, &btn_callback);
    
    
    // area das variaveis
    int lista[] = {0,1,2,3};
    alarm_id_t alarm;
    while (true) {
        if(flag_jogo){
            int tamanho = (sizeof(lista)/sizeof(lista[0]));
            for(int i =0; i<tamanho; i++){
                if(i!=0){
                    lose_colour(lista[i-1]);
                }
                set_colour(lista[i]);
                sleep_ms(2000);
            }
            lose_colour(lista[tamanho-1]);
            alarm = add_alarm_in_ms(2000, alarm_callback, NULL, false);
            flag_jogo = 0;
        }

        if(flag_resposta_timeout){
            printf("se fudeu \n");
            flag_resposta_timeout=0;
        }
        sleep_ms(1000);
    }
}
