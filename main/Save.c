#include "Save.h"

#define FLASH_TARGET_OFFSET (256 * 1024) // posição na flash (ajuste conforme necessário)
#define FLASH_PAGE_SIZE 256              // tamanho de página da flash

// Função para salvar um valor
void salvar_valor(uint32_t valor) {
    uint8_t buffer[FLASH_PAGE_SIZE];
    // Preenche o buffer com 0xFF (estado apagado)
    for (int i = 0; i < FLASH_PAGE_SIZE; i++) buffer[i] = 0xFF;

    // Copia o valor para o início do buffer
    memcpy(buffer, &valor, sizeof(valor));

    // Desabilita interrupções durante escrita
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_PAGE_SIZE);
    flash_range_program(FLASH_TARGET_OFFSET, buffer, FLASH_PAGE_SIZE);
    restore_interrupts(ints);
}

// Função para ler o valor salvo
uint32_t ler_valor() {
    const uint8_t *flash_ptr = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);
    uint32_t valor;
    memcpy(&valor, flash_ptr, sizeof(valor));
    return valor;
}