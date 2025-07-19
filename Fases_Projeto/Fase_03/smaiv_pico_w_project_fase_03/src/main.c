#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "config.h"

// Biblioteca do display OLED
#include "ssd1306/ssd1306.h"

// ==================================================================
// SEÇÃO DE CONFIGURAÇÃO E ESTADO GLOBAL
// ==================================================================

// --- Estado do Sistema ---
typedef enum {
    SCREEN_MAIN,
    SCREEN_SETTINGS
} screen_t;

static screen_t current_screen = SCREEN_MAIN;
static float sound_threshold = 150.0f; // Limiar agora é uma variável para podermos ajustá-la

// -- Parâmetros de Áudio --
#define SAMPLE_COUNT 256 // Número de amostras para calcular o RMS.

// --- Objeto do Display ---
static ssd1306_t disp;

// ======================================================================
// SEÇÃO DE PROCESSAMENTO DE ÁUDIO (FASE 2)
// ======================================================================
static float get_sound_level() {
    uint16_t samples[SAMPLE_COUNT];
    uint32_t sum = 0;
    adc_select_input(MIC_ADC_INPUT);

    for(int i = 0; i < SAMPLE_COUNT; i++) {
        samples[i] = adc_read();
        sum += samples[i];
    }

    uint16_t dc_offset = sum / SAMPLE_COUNT;

    uint64_t sum_of_squares = 0;
    for(int i = 0; i < SAMPLE_COUNT; i++) {
        int32_t sample_centered = samples[i] - dc_offset;
        sum_of_squares += (sample_centered * sample_centered);
    }
    return sqrtf((float)sum_of_squares / SAMPLE_COUNT);
}

// =================================================================================
// SEÇÃO DA INTERFACE DO USUÁRIO (UI) - FASE 3
// =================================================================================

/**
 * @brief Inicializa os perifericos da UI: OLED e pinos de input.
 */
static void ui_init() {
    // 1. Inicializa o display OLED via I2C
    i2c_init(OLED_I2C_PORT, 400 * 1000);
    gpio_set_function(OLED_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(OLED_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(OLED_SDA_PIN);
    gpio_pull_up(OLED_SCL_PIN);
    disp.external_vcc = false;
    ssd1306_init(&disp, 128, 64, 0x3c, OLED_I2C_PORT);

    // 2. Inicializa os pinos de input (joystick e botões)
    // Joystick eixo Y
    adc_gpio_init(JOYSTICK_Y_PIN);
    // Botão Joystick
    gpio_init(JOYSTICK_SW_PIN);
    gpio_set_dir(JOYSTICK_SW_PIN, GPIO_IN);
    gpio_pull_up(JOYSTICK_SW_PIN);
    // Botão A
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN); 
}

/**
 * @brief Desenha a tela principal de monitoramento
 */
static void draw_main_screen(float current_sound_level, bool is_alert_active) {
    char buf[22];
    ssd1306_draw_string(&disp, 0, 0, 2, "SMAIAS");
    sprintf(buf, "Nivel:%.0f Lim:%.0f", current_sound_level, sound_threshold);
    ssd1306_draw_string(&disp, 0, 24, 1, buf);
    if (is_alert_active) {
        ssd1306_draw_string(&disp, 0, 48, 2, "ALERTA!");
    } else {
        ssd1306_draw_string(&disp, 0, 48, 1, "Monitorando...");
    }
}

/**
 * @brief Desenha a tela de configurações para ajustar o limiar.
 */
static void draw_settings_screen() {
    char buf[20];
    ssd1306_draw_string(&disp, 0, 0, 2, "Ajustes");
    sprintf(buf, "Limiar: %.0f", sound_threshold);
    ssd1306_draw_string(&disp, 0, 24, 2, buf);
    ssd1306_draw_string(&disp, 0, 48, 1, "Joy:Muda | BtnA:Salva");
}

/**
 * @brief Lê os inputs e atualiza o estado do sistema (troca de tela, ajusta limiar).
 */
static void handle_input() {
    // Os pinos são pull-up, então '0' (false) significa pressionado.
    bool joy_sw_pressed = !gpio_get(JOYSTICK_SW_PIN);
    bool btn_a_pressed = !gpio_get(BUTTON_A_PIN);

    // Lógica de navegação entre telas
    if (current_screen == SCREEN_MAIN && joy_sw_pressed) {
        current_screen = SCREEN_SETTINGS;
        sleep_ms(200); // Debounce simples para evitar múltiplas leituras
        return;
    }
    if (current_screen == SCREEN_SETTINGS && btn_a_pressed) {
        current_screen = SCREEN_MAIN;
        printf("\nNovo limiar salvo: %.1f\n", sound_threshold);
        sleep_ms(200); // Debounce
        return;
    }

    // Lógica de ajuste de valor na tela de configurações
    if (current_screen == SCREEN_SETTINGS) {
        adc_select_input(0); // ADC0 corresponde ao GPIO26 (Joy Y na BitDogLab v1)
        uint16_t joy_y = adc_read();

        // Valores típicos do ADC de 12 bits (0 a 4095)
        if (joy_y > 3000) { // Joystick para cima
            sound_threshold += 5.0f;
            if (sound_threshold > 1000.0f) sound_threshold = 1000.0f; // Limite máximo
        } else if (joy_y < 1000) { // Joystick para baixo
            sound_threshold -= 5.0f;
            if (sound_threshold < 10.0f) sound_threshold = 10.0f; // Limite mínimo
        }
    }
}

// =================================================================================
// FUNÇÃO PRINCIPAL (main)
// =================================================================================

int main() {
    stdio_init_all();
    
    // Espera até que o host (PC) tenha estabelecido a conexão serial USB.
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    printf("\n--- SMAIAS: FASE 3 - INTERFACE DE USUARIO ---\n");

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

    // Inicializa a UI
    ui_init();
    
    printf("Sistema ativo. UI e monitoramento de audio operacionais.\n");

    // --- Loop Principal de Monitoramento ---
    while(true) {
        // 1. Processar entradas do usuário 
        handle_input();

        // 2. Ler sensores e atualizar a lógica
        float sound_level = get_sound_level();
        bool alert_triggered = sound_level > sound_threshold;

        // 3. Atualizar atuadores (LEDs e Display)
        gpio_put(RGB_R_PIN, alert_triggered);
        gpio_put(RGB_G_PIN, !alert_triggered);
        
        ssd1306_clear(&disp);
        if (current_screen == SCREEN_MAIN) {
            draw_main_screen(sound_level, alert_triggered);
        } else {
            draw_settings_screen();
        }
        ssd1306_show(&disp);

        // Pequeno delay para estabilizar o loop
        sleep_ms(50);
    }
      
    return 0;
}
