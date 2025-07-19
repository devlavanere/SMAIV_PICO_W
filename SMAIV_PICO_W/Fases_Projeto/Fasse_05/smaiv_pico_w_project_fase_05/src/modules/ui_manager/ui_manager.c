/**
 * @file ui_manager.c
 * @brief Implementação do módulo de Interface com o Usuário (UI).
 * @details Gerencia o display OLED, a leitura de botões/joystick e a navegação
 *          entre as telas de monitoramento e configuração.
 */
#include "ui_manager.h"
#include "config.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include <stdio.h>

#include "ssd1306/ssd1306.h"

/**
 * @brief Instância estática do driver do display OLED.
 * @details Manter como estático encapsula o controle do display dentro deste módulo.
 */
static ssd1306_t disp;

/**
 * @brief Inicializa os periféricos da UI e exibe a tela de inicialização (splash screen).
 */
void ui_init(void) {
    // Configura o barramento I2C e os pinos para o display OLED.
    i2c_init(OLED_I2C_PORT, 400 * 1000);
    gpio_set_function(OLED_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(OLED_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(OLED_SDA_PIN);
    gpio_pull_up(OLED_SCL_PIN);
    disp.external_vcc = false;
    ssd1306_init(&disp, 128, 64, 0x3C, OLED_I2C_PORT);

    // Configura os pinos de entrada para o joystick e botões.
    adc_gpio_init(JOYSTICK_Y_PIN);
    gpio_init(JOYSTICK_SW_PIN);
    gpio_set_dir(JOYSTICK_SW_PIN, GPIO_IN);
    gpio_pull_up(JOYSTICK_SW_PIN);
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    
    // Exibe a tela de inicialização para uma experiência de boot limpa.
    ssd1306_clear(&disp);
    ssd1306_draw_string(&disp, 20, 16, 2, "SMAIAS");
    ssd1306_draw_string(&disp, 16, 40, 1, "Inicializando...");
    ssd1306_show(&disp);
}

/**
 * @brief Desenha o conteúdo da tela principal de monitoramento.
 * @param state Ponteiro para o estado atual do sistema.
 */
static void draw_main_screen(const system_state_t *state) {
    char buf[22];
    sprintf(buf, "Nivel:%.0f Lim:%.0f", state->current_sound_level, state->sound_threshold);
    ssd1306_draw_string(&disp, 0, 24, 1, buf);

    if (state->alert_active) {
        ssd1306_draw_string(&disp, 0, 48, 2, "ALERTA!");
    } else {
        sprintf(buf, "MQTT: %s", state->mqtt_connected ? "OK" : "---");
        ssd1306_draw_string(&disp, 0, 48, 1, buf);
    }
}

/**
 * @brief Desenha o conteúdo da tela de configurações.
 * @param state Ponteiro para o estado atual do sistema.
 */
static void draw_settings_screen(const system_state_t *state) {
    char buf[20];
    sprintf(buf, "Limiar: %.0f", state->sound_threshold);
    ssd1306_draw_string(&disp, 0, 24, 2, buf);
    ssd1306_draw_string(&disp, 0, 48, 1, "Joy:Muda | BtnA:Salva");
}

/**
 * @brief Renderiza a tela completa no display OLED.
 * @details Esta função é chamada a cada ciclo do loop principal para manter a UI atualizada.
 *          Ela limpa o buffer, desenha o conteúdo da tela ativa e o envia para o display.
 * @param state Ponteiro para o estado do sistema, usado para decidir qual tela desenhar.
 */
void ui_draw(const system_state_t *state) {
    ssd1306_clear(&disp);
    
    // Desenha um título comum a ambas as telas.
    const char* title = (state->current_screen == SCREEN_MAIN) ? "SMAIAS" : "Ajustes";
    ssd1306_draw_string(&disp, 0, 0, 2, title);

    // Chama a função de desenho específica para a tela atual.
    if (state->current_screen == SCREEN_MAIN) {
        draw_main_screen(state);
    } else {
        draw_settings_screen(state);
    }
    
    // Envia o conteúdo do buffer de vídeo para o hardware do display.
    ssd1306_show(&disp);
}

/**
 * @brief Processa as entradas do usuário e atualiza o estado do sistema.
 * @details Lida com a navegação entre telas (via botão do joystick) e o ajuste de
 *          parâmetros (via eixo do joystick).
 * @param state Ponteiro para a estrutura de estado, que será modificada pela função.
 */
void ui_update_input(system_state_t *state) {
    // Lê o estado dos botões (pinos são pull-up, 'false' significa pressionado).
    bool joy_sw_pressed = !gpio_get(JOYSTICK_SW_PIN);
    bool btn_a_pressed = !gpio_get(BUTTON_A_PIN);

    // Lógica de navegação de tela.
    if (state->current_screen == SCREEN_MAIN && joy_sw_pressed) {
        state->current_screen = SCREEN_SETTINGS;
        sleep_ms(200); // Debounce para evitar trocas múltiplas.
        return;
    }
    if (state->current_screen == SCREEN_SETTINGS && btn_a_pressed) {
        state->current_screen = SCREEN_MAIN;
        sleep_ms(200);
        return;
    }

    // Lógica de ajuste de valor, apenas na tela de configurações.
    if (state->current_screen == SCREEN_SETTINGS) {
        adc_select_input(0); // Canal ADC para o eixo Y do Joystick.
        uint16_t joy_y = adc_read();
        
        // Altera o limiar com base na posição do joystick.
        if (joy_y > 3000) { 
            state->sound_threshold += 5.0f;
        } else if (joy_y < 1000) { 
            state->sound_threshold -= 5.0f;
        }
        
        // Garante que o valor do limiar permaneça dentro de limites seguros.
        if (state->sound_threshold < 10.0f) state->sound_threshold = 10.0f;
        if (state->sound_threshold > 999.0f) state->sound_threshold = 999.0f;
    }
}