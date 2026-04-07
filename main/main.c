/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "lcd.h"
#include "tft_lcd_ili9341/gfx/gfx_ili9341.h"
#include "tft_lcd_ili9341/ili9341/ili9341.h"
#include "tft_lcd_ili9341/touch_resistive/touch_resistive.h"

#include "hardware/irq.h"  // interrupts
#include "hardware/pwm.h"  // pwm 
#include "hardware/sync.h" // wait for interrupt 
#include "hardware/clocks.h" // redefined set_sys_clock_khz() 

#include "fahhhhh.h"
#include "Green.h"
#include "Blue.h"
#include "Red.h"
#include "Yellow.h"
#include "Win.h"

const int botao_inicio = 9;
const int botao_g = 10;
const int botao_b = 11;
const int botao_r = 12;
const int botao_y = 13;
const int AUDIO_PIN=28;
#define SCREEN_ROTATION 1 // 0 = RETRATO, 1 = PAISAGEM
const int width = 320;    // Variável global definida em gfx_ili9341.c que armazena a largura da tela
const int height = 240;

volatile int flag_jogo = 0;
volatile int flag_g = 0;
volatile int flag_b = 0;
volatile int flag_r = 0;
volatile int flag_y = 0;

volatile int flag_resposta_timeout = 0;
volatile int nivel = 0;
volatile int BTN_GAME_flag = 0;

int wav_position=0;
int len_audio=0;
int *p_len=&len_audio;
uint8_t *p_audio;

int64_t alarm_callback(alarm_id_t id, void *user_data)
{
    flag_resposta_timeout = 1;
    return 0;
}

void btn_callback(uint gpio, uint32_t events)
{
    if (events == 0x04)
    {
        if (gpio == botao_inicio)
        {
            flag_jogo = 1;
            nivel = 1;
        }

        if (gpio == botao_g)
        {
            flag_g = 1;
            BTN_GAME_flag = 1;
        }

        if (gpio == botao_b)
        {
            flag_b = 1;
            BTN_GAME_flag = 2;
        }

        if (gpio == botao_r)
        {
            flag_r = 1;
            BTN_GAME_flag = 3;
        }

        if (gpio == botao_y)
        {
            flag_y = 1;
            BTN_GAME_flag = 4;
        }
    }
}

void gera_lista(int lista[])
{
    for (int i = 0; i < 100; i++)
    {
        lista[i] = (rand() % 4) + 1;
        printf("%d", lista[i]);
    }
}

void pwm_interrupt_handler() {
    pwm_clear_irq(pwm_gpio_to_slice_num(AUDIO_PIN));    
    if (wav_position < (*p_len<<3) - 1) { 
        // set pwm level 
        // allow the pwm value to repeat for 8 cycles this is >>3 
        pwm_set_gpio_level(AUDIO_PIN, p_audio[wav_position>>3]);  
        wav_position++;
    }
}

int main()
{
    stdio_init_all();
    // lcd_init();
    LCD_initDisplay();
    gfx_init();
    gfx_clear();

    gpio_init(botao_inicio);
    LCD_initDisplay();
    LCD_setRotation(SCREEN_ROTATION);
    gpio_init(botao_g);
    gpio_init(botao_b);
    gpio_init(botao_r);
    gpio_init(botao_y);

    gpio_set_dir(botao_inicio, GPIO_IN);

    gpio_set_dir(botao_g, GPIO_IN);
    gpio_set_dir(botao_b, GPIO_IN);
    gpio_set_dir(botao_r, GPIO_IN);
    gpio_set_dir(botao_y, GPIO_IN);

    gpio_pull_up(botao_inicio);

    gpio_pull_up(botao_g);
    gpio_pull_up(botao_b);
    gpio_pull_up(botao_r);
    gpio_pull_up(botao_y);

    gpio_set_irq_enabled_with_callback(botao_inicio, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    gpio_set_irq_enabled_with_callback(botao_g, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    gpio_set_irq_enabled_with_callback(botao_b, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    gpio_set_irq_enabled_with_callback(botao_r, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    gpio_set_irq_enabled_with_callback(botao_y, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    //Declara e configura o PWM
    set_sys_clock_khz(176000, true); 
    gpio_set_function(AUDIO_PIN, GPIO_FUNC_PWM);
    int audio_pin_slice = pwm_gpio_to_slice_num(AUDIO_PIN);

    pwm_clear_irq(audio_pin_slice);
    pwm_set_irq_enabled(audio_pin_slice, true);

    irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_interrupt_handler);  
    irq_set_enabled(PWM_IRQ_WRAP, true);


    pwm_config config = pwm_get_default_config();

    pwm_config_set_clkdiv(&config, 8.0f); 
    pwm_config_set_wrap(&config, 250); 
    pwm_init(audio_pin_slice, &config, true);

    pwm_set_gpio_level(AUDIO_PIN, 0);

    // area das variaveis
    int lista[100]; // usar s range com o timer entre ligar e desligar
    int cont = 0;
    int inicio = 0;
    int valendo = 0;
    nivel = 1;
    int recorde = 0;
    int pont = 0;
    alarm_id_t alarm;
    int erro = 0;
    while (true)
    {
        if (flag_jogo)
        {

            if (inicio == 0)
            {
                int seed = to_us_since_boot(get_absolute_time());
                srand(seed);
                gera_lista(lista);
                // tamanho = (sizeof(lista) / sizeof(lista[0]));
                gfx_clear();
                gfx_setTextSize(2);       // Tamanho 2 (12x16 pixels por caractere)
                gfx_setTextColor(0x07E0); // Verde
                gfx_drawText(106, 25, "inicio de jogo");
                sleep_ms(300);
            }

            char nivel_str[10];
            char pont_str[10];
            pont = (nivel - 1) * 10;
            snprintf(nivel_str, sizeof(nivel_str), "%d", nivel);
            snprintf(pont_str, sizeof(nivel_str), "%d", pont);

            gfx_clear();
            gfx_setTextSize(2);       // Tamanho 2 (12x16 pixels por caractere)
            gfx_setTextColor(0x07E0); // Verde
            gfx_drawText(106, 25, "nivel");
            gfx_drawText(206, 25, nivel_str);
            gfx_drawText(106, 45, "pontos");
            gfx_drawText(206, 45, pont_str);

            sleep_ms(200);
            printf("Nivel: %d!\n", nivel);
            for (int i = 0; i < nivel; i++)
            { // O nivel controla quantos leds vai ter a sequencia
                set_colour(lista[i]);
                if (lista[i]==1){
                    wav_position=0;
                    len_audio=Green_DATA_LENGTH;
                    p_audio=Green_DATA;
                }else if (lista[i]==2){
                    wav_position=0;
                    len_audio=Blue_DATA_LENGTH;
                    p_audio=Blue_DATA;
                }else if (lista[i]==3){
                    wav_position=0;
                    len_audio=Red_DATA_LENGTH;
                    p_audio=Red_DATA;
                }else if (lista[i]==4){
                    wav_position=0;
                    len_audio=Yellow_DATA_LENGTH;
                    p_audio=Yellow_DATA;
                }
                
                
                
                sleep_ms(300);
                // lose_colour(lista[i]);
            }
            int game_time_ms = nivel * 3000 + 1000;
            alarm = add_alarm_in_ms(game_time_ms, alarm_callback, NULL, false);
            flag_jogo = 0;
            BTN_GAME_flag = 0;
            inicio = 1;
            gfx_clear();
            gfx_setTextSize(2);       // Tamanho 2 (12x16 pixels por caractere)
            gfx_setTextColor(0x07E0); // Verde
            gfx_drawText(106, 25, "valendo");
            flag_g=0;
            flag_b=0;
            flag_r=0;
            flag_y=0;
            valendo = 1;
            sleep_ms(200);
            // printf("Valendo!\n");
        }
        if (flag_g && valendo)
        {
            // printf("cor: %d, botao: 1", lista[cont]);
            if (lista[cont] == 1)
            { // verifica se apertou o botão correto
                cont++;
                wav_position=0;
                len_audio=Green_DATA_LENGTH;
                p_audio=Green_DATA;
                sleep_ms(500);
            }
            else
            {
                erro = 1;
                wav_position=0;
                len_audio=FAHHHHH_DATA_LENGTH;
                p_audio=FAHHHHH_DATA;
            }
            flag_g = 0;
            // sleep_ms(400);
        }

        if (flag_b && valendo)
        {
            // printf("cor: %d, botao: 2", lista[cont]);
            if (lista[cont] == 2)
            { // verifica se apertou o botão correto
                cont++;
                wav_position=0;
                len_audio=Blue_DATA_LENGTH;
                p_audio=Blue_DATA;
                sleep_ms(500);
            }
            else
            {
                erro = 1;
                wav_position=0;
                len_audio=FAHHHHH_DATA_LENGTH;
                p_audio=FAHHHHH_DATA;
            }
            flag_b = 0;
            // sleep_ms(400);
        }
        if (flag_r && valendo)
        {
            // printf("cor: %d, botao: 3", lista[cont]);
            if (lista[cont] == 3)
            { // verifica se apertou o botão correto
                cont++;
                wav_position=0;
                len_audio=Red_DATA_LENGTH;
                p_audio=Red_DATA;
                sleep_ms(500);
            }
            else
            {
                erro = 1;
                wav_position=0;
                len_audio=FAHHHHH_DATA_LENGTH;
                p_audio=FAHHHHH_DATA;
            }
            flag_r = 0;
            // sleep_ms(400);
        }

        if (flag_y && valendo)
        {
            // printf("cor: %d, botao: 4", lista[cont]);
            if (lista[cont] == 4)
            { // verifica se apertou o botão correto
                cont++;
                wav_position=0;
                len_audio=Yellow_DATA_LENGTH;
                p_audio=Yellow_DATA;
                sleep_ms(500);
            }
            else
            {
                erro = 1;
                wav_position=0;
                len_audio=FAHHHHH_DATA_LENGTH;
                p_audio=FAHHHHH_DATA;
            }
            flag_y = 0;
            // sleep_ms(400);
        }
        
        if (erro)
        {
            // Se errar zera a contagem e os niveis
            // printf("botao: %d, cor: %d \n", BTN_GAME_flag, lista[cont]);
            
            char pont_str[10];
            pont = (nivel - 1) * 10;
            snprintf(pont_str, sizeof(pont_str), "%d", pont);
            gfx_clear();
            gfx_setTextSize(2);       // Tamanho 2 (12x16 pixels por caractere)
            gfx_setTextColor(0x07E0); // Verde
            gfx_drawText(106, 25, "Errou");
            gfx_drawText(106, 45, "Pontos");
            gfx_drawText(206, 45, pont_str);
            
            char rec_str[10];
            snprintf(rec_str, sizeof(rec_str), "%d", recorde);
            gfx_drawText(106, 65, "Recorde");
            gfx_drawText(206, 65, rec_str);
            
            printf("Errou\n");
            sleep_ms(200);
            cancel_alarm(alarm);
            // BTN_GAME_flag = 0;
            
            flag_b = 0;
            flag_g = 0;
            flag_y = 0;
            flag_r = 0;
            
            erro = 0;
            cont = 0;
            nivel = 1;
            inicio = 0;
            valendo = 0;
            sleep_ms(2000);
        }
        
        
        if (cont == nivel)
        {
            cancel_alarm(alarm); // avança para o proximo nivel
            nivel++;
            cont = 0;
            flag_jogo = 1;
            valendo = 0;
            printf("Proximo nivel\n");
            
        }
        
        if (nivel > 4)
        {
            cancel_alarm(alarm);
            valendo = 0;
            flag_jogo = 0; // Finaliza o jogo
            gfx_clear();
            
            // printf("Você venceu!\n");
            gfx_setTextSize(2);       // Tamanho 2 (12x16 pixels por caractere)
            gfx_setTextColor(0x07E0); // Verde
            gfx_drawText(106, 25, "Voce venceu!");
            char pont_str[10];
            pont = (nivel - 1) * 10;
            nivel = 1;
            snprintf(pont_str, sizeof(pont_str), "%d", pont);
            gfx_drawText(106, 45, "Pontos");
            gfx_drawText(206, 45, pont_str);
            recorde=pont;
            char rec_str[10];
            snprintf(rec_str, sizeof(rec_str), "%d", recorde);
            gfx_drawText(106, 65, "Recorde");
            gfx_drawText(206, 65, rec_str);
            wav_position=0;
            len_audio=Win_DATA_LENGTH;
            p_audio=Win_DATA;
        }
        
        if (flag_resposta_timeout)
        {
            gfx_clear();
            gfx_setTextSize(2);       // Tamanho 2 (12x16 pixels por caractere)
            gfx_setTextColor(0x07E0); // Verde
            gfx_drawText(106, 25, "Time out");
            char pont_str[10];
            pont = (nivel - 1) * 10;
            snprintf(pont_str, sizeof(pont_str), "%d", pont);
            gfx_drawText(106, 45, "Pontos");
            gfx_drawText(206, 45, pont_str);
            char rec_str[10];
            snprintf(rec_str, sizeof(rec_str), "%d", recorde);
            gfx_drawText(106, 65, "Recorde");
            gfx_drawText(206, 65, rec_str);
            
            flag_resposta_timeout = 0;
            nivel = 1;
            cont = 0;
            inicio = 0;
            valendo = 0;
            
            wav_position=0;
            len_audio=FAHHHHH_DATA_LENGTH;
            p_audio=FAHHHHH_DATA;
        }

        if(pont>recorde){
            recorde = pont;
        }
    }
}
