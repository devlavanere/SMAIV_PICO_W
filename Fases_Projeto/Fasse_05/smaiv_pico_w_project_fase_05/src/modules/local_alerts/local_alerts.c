/**
 * @file local_alerts.c
 * @brief Gerencia todos os atuadores de alerta locais (LEDs, Buzzer).
 */

#include "local_alerts.h"
#include "config.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

// --- Definições e Funções Internas do Módulo ---
static PIO pio = pio0;
static uint sm = 0;

static inline void ws2812_program_init(PIO pio_instance, uint sm_instance, uint offset, uint pin, float freq, bool rgbw) {
    pio_gpio_init(pio_instance, pin);
    pio_sm_set_consecutive_pindirs(pio_instance, sm_instance, pin, 1, true);
    pio_sm_config c = ws2812_program_get_default_config(offset);
    sm_config_set_sideset_pins(&c, pin);
    sm_config_set_out_shift(&c, false, true, rgbw ? 32 : 24);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);
    int cycles_per_bit = ws2812_T1 + ws2812_T2 + ws2812_T3;
    float div = clock_get_hz(clk_sys) / (freq * cycles_per_bit);
    sm_config_set_clkdiv(&c, div);
    pio_sm_init(pio_instance, sm_instance, offset, &c);
    pio_sm_set_enabled(pio_instance, sm_instance, true);
}

static void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio, sm, pixel_grb << 8u);
}

static void fill_matrix(uint32_t color) {
    for (int i = 0; i < WS2812_NUM_LEDS; ++i) {
        put_pixel(color);
    }
}

/**
 * @brief Inicializa os pinos e periféricos para todos os atuadores de alerta.
 */
void alerts_init(void) {
    // LED RGB
    gpio_init(RGB_R_PIN); gpio_set_dir(RGB_R_PIN, GPIO_OUT);
    gpio_init(RGB_G_PIN); gpio_set_dir(RGB_G_PIN, GPIO_OUT);
    gpio_init(RGB_B_PIN); gpio_set_dir(RGB_B_PIN, GPIO_OUT);

    // --- INICIALIZAÇÃO DO BUZZER ---
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    
    // Configura a frequência do PWM
    // Usando a configuração que validamos na Fase 1
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 100.0f); // Define o divisor de clock
    pwm_config_set_wrap(&config, 500);      // Define o período (wrap) -> Frequência de 2.5kHz
    
    // Aplica a configuração e já habilita o slice de PWM
    pwm_init(slice_num, &config, true);
    
    // Garante que o som comece desligado (duty cycle 0%)
    pwm_set_gpio_level(BUZZER_PIN, 0);

    // Matriz WS2812
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, false);
    fill_matrix(0);
}

/**
 * @brief Atualiza o estado de todos os alertas com base no estado do sistema.
 * @details Esta função é chamada a cada ciclo do loop principal para garantir que os
 *          alertas visuais e sonoros reflitam consistentemente o estado atual.
 * @param state Ponteiro para a estrutura de estado do sistema.
 */
void alerts_update(const system_state_t *state) {
    if (state->alert_active) {
        // Alerta ATIVO
        gpio_put(RGB_R_PIN, 1); gpio_put(RGB_G_PIN, 0); gpio_put(RGB_B_PIN, 0); // Vermelho
        
        // Liga o som ajustando o duty cycle para 50%
        pwm_set_gpio_level(BUZZER_PIN, 250); // 250 é 50% de 500 (o valor do wrap)
        
        // Acende a matriz com vermelho
        fill_matrix(0x008000); // Formato GRB
    } else {
        // Alerta INATIVO
        pwm_set_gpio_level(BUZZER_PIN, 0); // Desliga o som (duty cycle 0%)
        fill_matrix(0); // Desliga a matriz

        // Lógica de status do LED RGB
        if (state->mqtt_connected) {
            gpio_put(RGB_R_PIN, 0); gpio_put(RGB_G_PIN, 1); gpio_put(RGB_B_PIN, 0); // Verde
        } else if (state->wifi_connected) {
            gpio_put(RGB_R_PIN, 0); gpio_put(RGB_G_PIN, 0); gpio_put(RGB_B_PIN, 1); // Azul
        } else {
            // Pisca amarelo se não houver conexão Wi-Fi
            bool led_state = (to_ms_since_boot(get_absolute_time()) / 500) % 2;
            gpio_put(RGB_R_PIN, led_state);
            gpio_put(RGB_G_PIN, led_state);
            gpio_put(RGB_B_PIN, 0);
        }
    }
}