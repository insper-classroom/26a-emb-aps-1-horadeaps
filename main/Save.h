#ifndef SAVE_H
#define SAVE_H

#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include <stdio.h>
#include <string.h>


#define FLASH_TARGET_OFFSET (256 * 1024) // posição na flash (ajuste conforme necessário)
#define FLASH_PAGE_SIZE 256              // tamanho de página da flash

void salvar_valor(uint32_t valor);
uint32_t ler_valor();
#endif
