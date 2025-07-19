#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "config.h"

// ======================================================
// SEÇÃO DE CONFIGURAÇÃO E FUNÇÕES DE ÁUDIO
// ======================================================

// -- Parâmetros do Núcleo de Monitoramento --
#define SAMPLE_COUNT 256       // Número de amostras para calcular o RMS. Um bom equilíbrio entre velocidade e escalabilidade.
#define SOUND_THRESHOLD 150.0f // Limiar de alerta. Valor para ajuste.

/**
 * @brief Coleta amostras de áudio do microfone e calcula o nível de ruído(RMS).
 * @return float O valor RMS calculado, que representa a intensidade do som.
 */
static float get_sound_level() {
    uint16_t samples[SAMPLE_COUNT];
    uint32_t sum = 0;

    // Seleciona o canal do ADC conectado ao microfone
    adc_select_input(MIC_ADC_INPUT);

    // 1. Coleta das amostras de áudio
    for(int i = 0; i < SAMPLE_COUNT; i++) {
        samples[i] = adc_read();
        sum += samples[i];
        sleep_us(10); // Delay para não amostrar muito rápido
    }

    // 2. Calcula o nível DC (a "linha de base" do sinal) para poder removê-lo
    uint16_t dc_offset = sum / SAMPLE_COUNT;

    // 3. Calcula a soma dos quadrados das diferenças (a energia do sinal de áudio)
    uint64_t sum_of_squares = 0;
    for(int i = 0; i < SAMPLE_COUNT; i++) {
        // Centraliza a amostra em zero, removendo o offset DC
        int32_t sample_centered = samples[i] - dc_offset;
        // Acumula o quadrado do valor
        sum_of_squares += (sample_centered * sample_centered);
    }

    // 4. Calcula o RMS (Root Mean Square), que é a raiz quadrada da média dos quadrados
    float rms_value = sqrtf((float)sum_of_squares / SAMPLE_COUNT);
    return rms_value;
}

int main() {
    stdio_init_all();
    
    // Espera até que o host (PC) tenha estabelecido a conexão serial USB.
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    printf("\n--- SMAIV: FASE 2 - NUCLEO DE MONITORAMENTO ---\n");
    printf("Sistema ativo. Monitorando o nivel de ruido...\n");
    printf("Limiar de Alerta (Threshold) configurado para: %.1f\n\n", SOUND_THRESHOLD);

    // --- Inicialização dos Periféricos ---
    // 1. ADC para o microfone
    adc_init();
    adc_gpio_init(MIC_ADC_PIN);

    // 2. LED RGB para servir como indicador de status
    // LED Verde para OK
    gpio_init(RGB_G_PIN);
    gpio_set_dir(RGB_G_PIN, GPIO_OUT);
    // LED vermelho para ALERTA
    gpio_init(RGB_R_PIN);
    gpio_set_dir(RGB_R_PIN, GPIO_OUT);

    // --- Loop Principal de Monitoramento ---
    while(true) {
        // 1. Lê o nível de ruído do ambiente
        float sound_level = get_sound_level();

        // 2. Compara o nível de ruído com o limiar para decidir se há um alerta
        bool alert_triggered = sound_level > SOUND_THRESHOLD;

        // 3. Imprime o status no terminal serial para depuração e calibração
        printf("\rNivel de Ruido (RMS): %-7.2f | Alerta: %s   ",
                sound_level,
                alert_triggered ? "SIM !" : "Nao");
        fflush(stdout); // Força a impressão imediata dos dados no terminal

        // 4. Atua no hardware (LEDs) com base no estado do alerta
        if (alert_triggered) {
            gpio_put(RGB_R_PIN, 1); // Liga o LED Vermelho
            gpio_put(RGB_G_PIN, 0); // Garante que o LED Verde está desligado
        } else {
            gpio_put(RGB_R_PIN, 0); // Garante que o LED Vermelho está desligado
            gpio_put(RGB_G_PIN, 1); // Liga o LED Verde
        }

        sleep_ms(100);
    }
      
    return 0;
}
