#ifndef MATRIZ_LED_H
#define MATRIZ_LED_H

#include <stdbool.h>
#include <stdint.h>

#define LED_CONTAGEM 25

// Declaração dos buffers para armazenar os números de 0 a 9
extern bool numero_0[LED_CONTAGEM];
extern bool numero_1[LED_CONTAGEM];
extern bool numero_2[LED_CONTAGEM];
extern bool numero_3[LED_CONTAGEM];
extern bool numero_4[LED_CONTAGEM];
extern bool numero_5[LED_CONTAGEM];
extern bool numero_6[LED_CONTAGEM];
extern bool numero_7[LED_CONTAGEM];
extern bool numero_8[LED_CONTAGEM];
extern bool numero_9[LED_CONTAGEM];

// Função para configurar os LEDs conforme um número específico
void set_one_led(uint8_t r, uint8_t g, uint8_t b, bool numero_desenhado[]);

// Função para exibir o número baseado no contador
void mostra_numero_baseado_no_contador();

#endif // MATRIZ_LED_H
