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
volatile int nivel = 0;
volatile int BTN_GAME_flag = 0; 

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
            nivel=1;
        }

        if(gpio == botao_g){
            BTN_GAME_flag=1;
        }

        if(gpio == botao_b){
            BTN_GAME_flag=2;
        }

        if(gpio == botao_r){
            BTN_GAME_flag=3;
        }

        if(gpio == botao_y){
            BTN_GAME_flag=4;
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
    int lista[] = {1,2,3,4};
    int cont = 0;
    nivel = 1;
    int tamanho = (sizeof(lista)/sizeof(lista[0]));

    alarm_id_t alarm;
    
    while (true) {
        if(flag_jogo){
            printf("Nivel: %d!\n",nivel);
            for(int i =0; i<nivel; i++){            //O nivel controla quantos leds vai ter a sequencia
                set_colour(lista[i]);
                sleep_ms(1000);
                lose_colour(lista[i]);
            }
            int game_time_ms=nivel*1000+1000;
            alarm = add_alarm_in_ms(game_time_ms, alarm_callback, NULL, false);
            flag_jogo = 0;
            BTN_GAME_flag=0;
            printf("Valendo!\n");
        }

        if (BTN_GAME_flag==lista[cont]){        //verifica se apertou o botão correto
            printf("Acertou\n");
            cont++;
            sleep_ms(200);
            BTN_GAME_flag = 0;
        }else if (BTN_GAME_flag!=0){             // Se errar zera a contagem e os niveis
            printf("Errou\n");
            sleep_ms(200);
            cancel_alarm(alarm);
            BTN_GAME_flag = 0;
            cont = 0;
            nivel = 1;
            sleep_ms(2000);
        }

        if(cont == nivel){   
            cancel_alarm(alarm);                     // avança para o proximo nivel
            nivel++;
            cont = 0;
            flag_jogo = 1;
            printf("Proximo nivel\n");
        }

        if(nivel > tamanho){ 
            nivel=1;    
            cancel_alarm(alarm); 
            flag_jogo = 0;                  //Finaliza o jogo
            printf("Você venceu!\n");
        }

        if(flag_resposta_timeout){
            printf("Time out error\n");
            flag_resposta_timeout = 0;
            nivel = 1;
            cont=0;

        }
    }
}
