/**
 * @file audio_processing.c
 * @brief Implementação do módulo de aquisição e processamento de áudio no Core 1.
 */
#include "audio_processing.h"
#include "config.h"
#include "pico/multicore.h"
#include "hardware/adc.h"
#include <math.h>

#define SAMPLE_COUNT 256

/**
 * @brief Função interna que calcula o nível de ruído (RMS).
 * @return float O valor RMS calculado.
 */
static float calculate_rms_level(void) {
    uint16_t samples[SAMPLE_COUNT];
    uint32_t sum = 0;

    adc_select_input(MIC_ADC_INPUT);

    for (int i = 0; i < SAMPLE_COUNT; i++) {
        samples[i] = adc_read();
        sum += samples[i];
    }

    uint16_t dc_offset = sum / SAMPLE_COUNT;

    uint64_t sum_of_squares = 0;
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        int32_t sample_centered = samples[i] - dc_offset;
        sum_of_squares += (sample_centered * sample_centered);
    }

    return sqrtf((float)sum_of_squares / SAMPLE_COUNT);
}

/**
 * @brief Ponto de entrada para o Core 1.
 * @details Este é o loop infinito que será executado exclusivamente no Core 1.
 *          Ele continuamente calcula o nível de RMS e envia o resultado para o
 *          Core 0 através da FIFO inter-core.
 */
void core1_entry() {
    // Loop infinito de processamento de áudio no Core 1
    while (true) {
        float rms_value = calculate_rms_level();

        // Para enviar um float pela FIFO (que espera um uint32_t),
        multicore_fifo_push_blocking((uint32_t)(*(uint32_t*)&rms_value));
    }
}

/**
 * @brief Inicializa o pino do ADC conectado ao microfone.
 */
void audio_init(void) {
    adc_init();
    adc_gpio_init(MIC_ADC_PIN);
}

/**
 * @brief Lança o loop de processamento de áudio no Core 1.
 */
void audio_launch_on_core1(void) {
    // Reseta a FIFO antes de usar
    multicore_fifo_clear_irq();
    
    // Lança a função 'core1_entry' no segundo núcleo
    multicore_launch_core1(core1_entry);
}