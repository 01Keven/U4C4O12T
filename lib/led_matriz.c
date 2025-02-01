#include "led_matriz.h"        // Inclui o cabeçalho para a biblioteca de controle de LEDs
#include "hardware/pio.h"      // Inclui a biblioteca para controlar o PIO (Programmable IO)
#include "ws2812.pio.h"        // Inclui o código específico para controle de LEDs WS2812

// Função inline para enviar um pixel (cor no formato GRB) ao PIO
static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);  // Envia o valor do pixel para o PIO
}

// Função que converte as componentes RGB (vermelho, verde, azul) para um valor de 32 bits
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    // A ordem dos bytes é invertida para o formato GRB necessário para os LEDs WS2812
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

// Definindo os números de 0 a 9, onde cada número é representado por um vetor de LEDs
// LED_CONTAGEM é o número total de LEDs para desenhar cada número
bool numero_0[LED_CONTAGEM] = {
    0, 1, 1, 1, 1, 
    1, 0, 0, 1, 0, 
    0, 1, 0, 0, 1, 
    1, 0, 0, 1, 0, 
    0, 1, 1, 1, 1
};

bool numero_1[LED_CONTAGEM] = {
    0, 0, 1, 0, 0, 
    0, 0, 1, 0, 0, 
    0, 0, 1, 0, 1, 
    0, 1, 1, 0, 0, 
    0, 0, 1, 0, 0
};

bool numero_2[LED_CONTAGEM] = {
    0, 1, 1, 1, 1, 
    1, 0, 0, 0, 0, 
    0, 1, 1, 1, 1, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 1
};

bool numero_3[LED_CONTAGEM] = {
    0, 1, 1, 1, 1, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 1, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 1
};

bool numero_4[LED_CONTAGEM] = {
    0, 1, 0, 0, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 1, 
    1, 0, 0, 1, 0, 
    0, 1, 0, 0, 1
};

bool numero_5[LED_CONTAGEM] = {
    0, 1, 1, 1, 1, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 1, 
    1, 0, 0, 0, 0, 
    0, 1, 1, 1, 1
};

bool numero_6[LED_CONTAGEM] = {
    0, 1, 1, 1, 1, 
    1, 0, 0, 1, 0, 
    0, 1, 1, 1, 1, 
    1, 0, 0, 0, 0, 
    0, 1, 1, 1, 1
};

bool numero_7[LED_CONTAGEM] = {
    0, 1, 0, 0, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 0, 0, 1, 
    1, 0, 0, 1, 0, 
    0, 1, 1, 1, 1
};

bool numero_8[LED_CONTAGEM] = {
    0, 1, 1, 1, 1, 
    1, 0, 0, 1, 0, 
    0, 1, 1, 1, 1, 
    1, 0, 0, 1, 0, 
    0, 1, 1, 1, 1
};

bool numero_9[LED_CONTAGEM] = {
    0, 1, 0, 0, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 1, 
    1, 0, 0, 1, 0, 
    0, 1, 1, 1, 1
};

// Função para acender os LEDs de um número com uma cor específica (RGB) em cada posição
// A função recebe a cor (r, g, b) e o vetor que representa o número a ser desenhado
void set_one_led(uint8_t r, uint8_t g, uint8_t b, bool numero_desenhado[])
{
    // Converte a cor RGB para o formato esperado pelos LEDs (GRB)
    uint32_t color = urgb_u32(r, g, b);
    
    // Percorre todos os LEDs do número e acende-os de acordo com a definição do vetor
    for (int i = 0; i < LED_CONTAGEM; i++)
    {
        // Se o valor no vetor for '1', acende o LED com a cor definida
        // Caso contrário, apaga o LED
        if (numero_desenhado[i])
        {
            put_pixel(color);
        }
        else
        {
            put_pixel(0);  // Desliga o LED se o valor for '0'
        }
    }
}
