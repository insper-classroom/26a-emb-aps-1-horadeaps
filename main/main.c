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

const int botao_inicio = 9;
const int botao_g = 10;
const int botao_b = 11;
const int botao_r = 12;
const int botao_y = 13;
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

int64_t alarm_callback(alarm_id_t id, void *user_data)
{
    flag_resposta_timeout = 1;
    return 0;
}

void btn_callback(uint gpio, uint32_t events)
{
    if (events == 0x08)
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

    // area das variaveis
    int lista[100]; // usar s range com o timer entre ligar e desligar
    int cont = 0;
    int inicio = 0;
    int valendo = 0;
    nivel = 1;
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
            int pont = (nivel - 1) * 10;
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
            }
            else
            {
                erro = 1;
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
            }
            else
            {
                erro = 1;
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
            }
            else
            {
                erro = 1;
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
            }
            else
            {
                erro = 1;
            }
            flag_y = 0;
            // sleep_ms(400);
        }

        if (erro)
        {
            // Se errar zera a contagem e os niveis
            // printf("botao: %d, cor: %d \n", BTN_GAME_flag, lista[cont]);

            char pont_str[10];
            int pont = (nivel - 1) * 10;
            snprintf(pont_str, sizeof(pont_str), "%d", pont);
            gfx_clear();
            gfx_setTextSize(2);       // Tamanho 2 (12x16 pixels por caractere)
            gfx_setTextColor(0x07E0); // Verde
            gfx_drawText(106, 25, "errou");
            gfx_drawText(106, 45, "pontos");
            gfx_drawText(206, 45, pont_str);
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
        // if (BTN_GAME_flag == lista[cont])
        // { // verifica se apertou o botão correto
        //     printf("Acertou\n");
        //     cont++;
        //     sleep_ms(200);
        //     BTN_GAME_flag = 0;
        // }
        // else if (BTN_GAME_flag != lista[cont] && BTN_GAME_flag != 0)
        // { // Se errar zera a contagem e os niveis
        //     printf("botao: %d, cor: %d \n", BTN_GAME_flag, lista[cont]);

        //     char pont_str[10];
        //     int pont = (nivel-1)*10;
        //     snprintf(pont_str, sizeof(pont_str), "%d", pont);
        //     gfx_clear();
        //     gfx_setTextSize(2);       // Tamanho 2 (12x16 pixels por caractere)
        //     gfx_setTextColor(0x07E0); // Verde
        //     gfx_drawText(106, 25, "errou");
        //     gfx_drawText(106, 45, "pontos");
        //     gfx_drawText(206, 45, pont_str);
        //     printf("Errou\n");
        //     sleep_ms(200);
        //     cancel_alarm(alarm);
        //     BTN_GAME_flag = 0;
        //     cont = 0;
        //     nivel = 1;
        //     inicio = 0;
        //     sleep_ms(2000);
        // }

        if (cont == nivel)
        {
            cancel_alarm(alarm); // avança para o proximo nivel
            nivel++;
            cont = 0;
            flag_jogo = 1;
            valendo = 0;
            printf("Proximo nivel\n");
        }

        if (nivel > 100)
        {
            nivel = 1;
            cancel_alarm(alarm);
            valendo = 0;
            flag_jogo = 0; // Finaliza o jogo
            // printf("Você venceu!\n");
            gfx_setTextSize(2);       // Tamanho 2 (12x16 pixels por caractere)
            gfx_setTextColor(0x07E0); // Verde
            gfx_drawText(106, 25, "Você venceu!");
            char pont_str[10];
            int pont = (nivel - 1) * 10;
            snprintf(pont_str, sizeof(pont_str), "%d", pont);
            gfx_drawText(106, 45, "pontos");
            gfx_drawText(206, 45, pont_str);
        }

        if (flag_resposta_timeout)
        {
            gfx_clear();
            gfx_setTextSize(2);       // Tamanho 2 (12x16 pixels por caractere)
            gfx_setTextColor(0x07E0); // Verde
            gfx_drawText(106, 25, "time out");
            char pont_str[10];
            int pont = (nivel - 1) * 10;
            snprintf(pont_str, sizeof(pont_str), "%d", pont);
            gfx_drawText(106, 45, "pontos");
            gfx_drawText(206, 45, pont_str);
            flag_resposta_timeout = 0;
            nivel = 1;
            cont = 0;
            inicio = 0;
            valendo = 0;
        }
    }
}
