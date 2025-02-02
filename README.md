# Controle de Matriz de LEDs com Raspberry Pi Pico W  

Este projeto implementa um sistema para controle de uma matriz de LEDs utilizando um Raspberry Pi Pico W. O código permite a exibição de números de 0 a 9 na matriz, que podem ser incrementados e decrementados usando botões físicos. Além disso, um LED vermelho pisca continuamente 5 vezes por segundo, e um botão de joystick pode ser utilizado para reiniciar o dispositivo.

*Extra: Ao chegar no ultimo número (9), o sistema retorna para (0) para que seja possivel fazer a contagem de forma mais rápida, o mesmo acontece se descrementar do numero (0), assim ele vai para a ultima posição númerica que é o (9)*

---

## Vídeo Apresentação (Baixe o vídeo para melhor resolução)
- https://drive.google.com/file/d/19z6hVeYqy2P-gc60m-JFPZrAgenY0aw1/view?usp=sharing

---

## 📜 Sumário

- [📌 Descrição do Projeto](#-descrição-do-projeto)  
- [⚙️ Funcionamento](#️-funcionamento)  
- [🔧 Hardware Utilizado](#-hardware-utilizado)  
- [📂 Estrutura do Projeto](#-estrutura-do-projeto)
- [💻 Explicação do Código](#explicação-do-código)
  - [Definição de Constantes](#definição-de-constantes)
  - [Inicialização de GPIOs](#inicialização-de-gpios)
  - [Manipulação de Interrupções](#manipulação-de-interrupções)
  - [Exibição de Números na Matriz de LEDs](#exibição-de-números-na-matriz-de-leds)
  - [Loop Principal](#loop-principal)
- [🚀 Como Executar o Projeto](#-como-executar-o-projeto)  
- [📜 Licença](#-licença)  

---

## 📌 Descrição do Projeto  

O código utiliza interrupções para detectar o pressionamento dos botões e atualizar a matriz de LEDs de acordo com o contador interno. As principais funcionalidades são:  

✅ Incrementar e decrementar o valor exibido na matriz de LEDs.  
✅ Reiniciar o dispositivo ao pressionar o botão do joystick.  
✅ Piscar o LED vermelho continuamente 5 vezes por segundo.  
✅ Enviar mensagens para o monitor serial para depuração.  

---

## ⚙️ Funcionamento  

- **Botão A (GPIO 5)** → Incrementa o número exibido (0 a 9).  
- **Botão B (GPIO 6)** → Decrementa o número exibido (0 a 9).  
- **Botão Joystick (GPIO 22)** → Reinicia o Raspberry Pi Pico w.  
- **LED Vermelho (GPIO 13)** → Pisca a cada 100 ms.  
- **Matriz de LEDs (GPIO 7)** → Exibe os números controlados pelos botões.  

Os números são desenhados na matriz utilizando LEDs WS2812B, que são controlados via PIO (Programmable I/O).  

---

## 🔧 Hardware Utilizado  

| Componente       | Descrição                                  |
|-----------------|------------------------------------------|
| Raspberry Pi Pico W | Microcontrolador utilizado no projeto  |
| Matriz WS2812  | Matriz de LEDs endereçáveis             |
| Botões          | Três botões para controle do sistema    |
| LED Vermelho    | LED indicador de status                 |

---

## 📂 Estrutura do Projeto  

```
├── lib/
│   ├── led_matriz.c          # Implementação das funções para controlar a matriz de LEDs
│   ├── led_matriz.h          # Cabeçalho da biblioteca de controle da matriz de LEDs
│
├── u4_t2_interrupcoes.c      # Código principal do projeto
├── CMakeLists.txt            # Arquivo de configuração para compilação com CMake
├── README.md                 # Documentação do projeto
├── ws2812.pio                 # Código que gera omunicação via PIO com os LEDs WS2812
```

---

## 🚀 Como Executar o Projeto  

### 1️⃣ Clonar o Repositório  

```sh
git clone [https://github.com/seu-usuario/nome-do-repositorio.git](https://github.com/01Keven/U4C4O12T.git)
```

### 2️⃣ Compilar o Código  

Para compilar o código, utilize o SDK do Raspberry Pi Pico W e o CMake. Caso ainda não tenha o SDK configurado, siga [este guia oficial](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf). Se tiver o SDK, compile o codigo normalmente.

![image](https://github.com/user-attachments/assets/54ce2b09-d0aa-4871-af25-1c102d5c256c)


```sh
mkdir build
cd build
cmake ..
make
```

### 3️⃣ Gerar e Enviar o Arquivo `.uf2`  

Após a compilação, conecte o Kit de desenvolvimento BitDogLab ao computador em **modo bootloader** (pressionando o botão BOOTSEL ao conectar o cabo USB).  

Depois, copie o arquivo gerado para a unidade do Pico. Ou simplesmente arraste e solte o arquivo `.uf2` na unidade correspondente.  

---

## **Explicação do Código**

### **Definição de Constantes**
```c
// Inclusão das bibliotecas necessárias para o funcionamento do código
#include <stdio.h>
#include "pico/stdlib.h"      // Biblioteca padrão do Raspberry Pi Pico
#include "hardware/pio.h"     // Biblioteca para controle do PIO (Programmable IO)
#include "hardware/clocks.h"  // Biblioteca para controle de clocks
#include "pico/bootrom.h"     // Biblioteca para reiniciar o dispositivo
#include "lib/led_matriz.h"   // Biblioteca personalizada para controle da matriz de LEDs
#include "ws2812.pio.h"       // Biblioteca gerada para controlar LEDs WS2812 via PIO

// Definição das constantes relacionadas aos pinos e LEDs
#define LED_CONTAGEM 25       // Número de LEDs na matriz
#define LED_RED 13            // Pino do LED vermelho
#define MATRIZ_LED_PIN 7      // Pino para a matriz de LEDs
#define BUTTON_A 5            // Pino do botão A
#define BUTTON_B 6            // Pino do botão B
#define BUTTON_JOYSTICK 22    // Pino do botão Joystick
#define DEBOUNCE_DELAY_MS 200 // Tempo de debounce em milissegundos para evitar múltiplas leituras rápidas

// Variáveis globais para controle do estado
static volatile int decrementa_incrementa_led = 0;       // Contador de LEDs (decrementa ou incrementa)
static volatile bool atualiza_leds = false;               // Flag para sinalizar se a matriz de LEDs precisa ser atualizada
static volatile uint32_t ultima_alteracao_led_vermelho = 0; // Controle de tempo para piscar o LED vermelho

// Variáveis de controle do tempo de interrupção dos botões (para debounce)
volatile uint32_t ultimo_tempo_button_a = 0;  // Tempo da última interrupção do botão A
volatile uint32_t ultimo_tempo_button_b = 0;  // Tempo da última interrupção do botão B
---

### **Inicialização de GPIOs**
```c
// Função para inicializar os LEDs e botões, configurando os pinos adequadamente
void inicializar_leds_e_botoes() {
    gpio_init(BUTTON_A);            // Inicializa o pino do botão A
    gpio_set_dir(BUTTON_A, GPIO_IN); // Define o pino do botão A como entrada
    gpio_pull_up(BUTTON_A);         // Ativa o resistor de pull-up para o botão A

    gpio_init(BUTTON_B);            // Inicializa o pino do botão B
    gpio_set_dir(BUTTON_B, GPIO_IN); // Define o pino do botão B como entrada
    gpio_pull_up(BUTTON_B);         // Ativa o resistor de pull-up para o botão B

    gpio_init(BUTTON_JOYSTICK);     // Inicializa o pino do botão Joystick
    gpio_set_dir(BUTTON_JOYSTICK, GPIO_IN); // Define o pino do botão Joystick como entrada
    gpio_pull_up(BUTTON_JOYSTICK);  // Ativa o resistor de pull-up para o botão Joystick

    gpio_init(LED_RED);             // Inicializa o pino do LED vermelho
    gpio_set_dir(LED_RED, GPIO_OUT); // Define o pino do LED vermelho como saída
    gpio_put(LED_RED, false);       // Desliga o LED vermelho inicialmente
}
```
- Configura os pinos dos botões como **entradas** com resistores `pull-up`.
- Configura o **LED vermelho** como saída.

---

### **Manipulação de Interrupções**
```c
// Função de interrupção que trata os eventos gerados pelos botões
static void gpio_irq_handler(uint gpio, uint32_t events) {
    // Verifica qual botão gerou a interrupção e realiza a ação correspondente
    if (gpio == BUTTON_A) {
        uint32_t tempo_atual = time_us_32() / 1000; // Captura o tempo atual em milissegundos

        // Verifica o debounce do botão A para evitar múltiplas leituras rápidas
        if (tempo_atual - ultimo_tempo_button_a < DEBOUNCE_DELAY_MS) return;
        ultimo_tempo_button_a = tempo_atual;

        // Depuração: Exibe mensagem quando o botão A é pressionado
        printf("Botão A pressionado. Incrementando contador. Novo valor: %d\n", decrementa_incrementa_led);

        // Incrementa o contador de LEDs, garantindo que ele nunca ultrapasse 9
        decrementa_incrementa_led = (decrementa_incrementa_led + 1) % 10;
        atualiza_leds = true; // Sinaliza que a matriz de LEDs precisa ser atualizada

    } else if (gpio == BUTTON_B) {
        uint32_t tempo_atual = time_us_32() / 1000; // Captura o tempo atual em milissegundos

        // Verifica o debounce do botão B para evitar múltiplas leituras rápidas
        if (tempo_atual - ultimo_tempo_button_b < DEBOUNCE_DELAY_MS) return;
        ultimo_tempo_button_b = tempo_atual;

        // Depuração: Exibe mensagem quando o botão B é pressionado
        printf("Botão B pressionado. Decrementando contador. Novo valor: %d\n", decrementa_incrementa_led);

        // Decrementa o contador de LEDs, garantindo que ele nunca seja menor que 0
        decrementa_incrementa_led = (decrementa_incrementa_led == 0) ? 9 : decrementa_incrementa_led - 1;
        atualiza_leds = true; // Sinaliza que a matriz de LEDs precisa ser atualizada

    } else if (gpio == BUTTON_JOYSTICK) {
        // Depuração: Exibe mensagem quando o botão Joystick é pressionado
        printf("Botão Joystick pressionado. Resetando o boot.\n");
        reset_usb_boot(0, 0); // Reinicia o dispositivo
    }
}
```
- Quando `BUTTON_A` é pressionado, o contador de LEDs é **incrementado**.
- Quando `BUTTON_B` é pressionado, o contador é **decrementado**.
- Pressionar `BUTTON_JOYSTICK` **reinicia** o Raspberry Pi Pico.
- Usa debounce para evitar leituras múltiplas rápidas.

---

### **Exibição de Números na Matriz de LEDs**
```c
// Função para exibir um número na matriz de LEDs baseado no valor do contador
void mostra_numero_baseado_no_contador() {
    // A função seleciona a cor e o número a ser exibido com base no contador
    switch (decrementa_incrementa_led) {
    case 0:
        set_one_led(255, 0, 0, numero_0); // Exibe o número 0 em vermelho
        break;
    case 1:
        set_one_led(0, 0, 255, numero_1); // Exibe o número 1 em azul
        break;
    case 2:
        set_one_led(255, 0, 0, numero_2); // Exibe o número 2 em vermelho
        break;
    case 3:
        set_one_led(0, 0, 255, numero_3); // Exibe o número 3 em azul
        break;
    case 4:
        set_one_led(255, 0, 0, numero_4); // Exibe o número 4 em vermelho
        break;
    case 5:
        set_one_led(0, 0, 255, numero_5); // Exibe o número 5 em azul
        break;
    case 6:
        set_one_led(255, 0, 0, numero_6); // Exibe o número 6 em vermelho
        break;
    case 7:
        set_one_led(0, 0, 255, numero_7); // Exibe o número 7 em azul
        break;
    case 8:
        set_one_led(255, 0, 0, numero_8); // Exibe o número 8 em vermelho
        break;
    case 9:
        set_one_led(0, 0, 255, numero_9); // Exibe o número 9 em azul
        break;
    }
}
```
- Exibe o número correspondente na matriz de LEDs, alternando entre cores vermelha e azul.
- ![image](https://github.com/user-attachments/assets/f2181523-cb45-45ac-adf8-07291e923833)
- ![image](https://github.com/user-attachments/assets/8170c369-6b88-4597-aedf-364791097141)

---

### **Função Principal**
```c
    // Função principal do programa
int main() {
    stdio_init_all(); // Inicializa a comunicação serial
    inicializar_leds_e_botoes(); // Inicializa os LEDs e os botões

    PIO pio = pio0;  // Seleciona o PIO 0
    int sm = 0;      // Seleciona o estado da máquina de estado PIO
    uint offset = pio_add_program(pio, &ws2812_program); // Adiciona o programa de controle de LEDs WS2812 no PIO

    ws2812_program_init(pio, sm, offset, MATRIZ_LED_PIN, 800000, false); // Inicializa o programa de controle de LEDs

    // Registra as interrupções para os botões A, B e Joystick
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_JOYSTICK, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // Loop principal do programa
    while (true) {
        uint32_t tempo_atual = time_us_32(); // Captura o tempo atual em microssegundos

        // Atualiza a matriz de LEDs se necessário
        if (atualiza_leds) {
            printf("Atualizando LEDs. Valor atual do contador: %d\n", decrementa_incrementa_led);
            mostra_numero_baseado_no_contador(); // Atualiza a exibição dos LEDs conforme o contador
            atualiza_leds = false;  // Reseta a flag de atualização
        }

        // Pisca o LED vermelho 5 vezes por segundo
        if (tempo_atual - ultima_alteracao_led_vermelho >= 100000) {  // 100ms = 100000µs
            gpio_put(LED_RED, !gpio_get(LED_RED)); // Inverte o estado do LED vermelho
            ultima_alteracao_led_vermelho = tempo_atual; // Atualiza o tempo da última alteração do LED vermelho

            // Depuração: Exibe o estado do LED vermelho
            printf("LED vermelho alternado. Novo estado: %d\n", gpio_get(LED_RED));
        }
    }

}
```
- Atualiza a matriz de LEDs quando necessário.
- Alterna o LED vermelho a cada `100ms`.
