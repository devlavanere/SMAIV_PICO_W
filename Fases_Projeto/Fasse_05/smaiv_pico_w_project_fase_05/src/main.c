/**
 * @file main.c
 * @author Michel L. Sampaio
 * @brief Arquivo principal e orquestrador do sistema SMAIV (Core 0).
 * @version 5.0 (Versão Final com Suporte Dual-Core)
 * 
 * @copyright Copyright (c) 2025
 * 
 * @details
 * Este arquivo contém a função `main()`, que serve como ponto de entrada para o 
 * Core 0 da aplicação embarcada. A arquitetura do sistema foi projetada para 
 * utilizar os dois núcleos do microcontrolador RP2040, dividindo as tarefas para
 * otimizar o desempenho e a responsividade.
 * 
 * ### Divisão de Tarefas entre os Núcleos:
 * 
 * **Core 0 (Este arquivo):**
 * - Atua como o núcleo principal, responsável pela lógica de alto nível.
 * - Gerencia a interface com o usuário (UI), lendo botões, joystick e atualizando o display OLED.
 * - Controla a lógica de estado do sistema (monitoramento vs. alerta).
 * - Gerencia toda a conectividade de rede (Wi-Fi e MQTT).
 * - Controla os atuadores de alerta locais (LEDs, buzzer).
 * - Comunica-se com o Core 1 através de uma FIFO (First-In, First-Out) para receber
 *   os dados de nível sonoro.
 * 
 * **Core 1 (Módulo audio_processing):**
 * - Atua como um co-processador de sinal dedicado.
 * - Executa um loop infinito focado exclusivamente na aquisição de áudio via ADC
 *   e no cálculo do valor RMS (Root Mean Square).
 * - Envia o resultado do cálculo de RMS para o Core 0 de forma contínua através da FIFO.
 * 
 * Esta abordagem de processamento paralelo garante que a tarefa computacionalmente
 * intensiva e sensível ao tempo (processamento de áudio) não interfira na
 * responsividade da interface do usuário e na estabilidade da conexão de rede.
 */

// =================================================================================
// INCLUDES DE BIBLIOTECAS E MÓDULOS
// =================================================================================

// Bibliotecas padrão da Pico SDK e C
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/multicore.h"

// Arquivos de configuração e tipos compartilhados
#include "config.h"
#include "common.h"

// Inclusão das interfaces dos módulos de software
#include "modules/audio_processing/audio_processing.h"
#include "modules/ui_manager/ui_manager.h"
#include "modules/local_alerts/local_alerts.h"
#include "modules/mqtt_comm/mqtt_comm.h"

// =================================================================================
// main() - Orquestrador do Sistema SMAIAS
// =================================================================================

int main() {
    // --- 1. INICIALIZAÇÃO DO SISTEMA E HARDWARE ---

    /**
     * @brief Inicializa as bibliotecas padrão da SDK, incluindo o stdio via USB.
     */
    stdio_init_all();

    /**
     * @brief Bloqueia a execução até que a conexão serial USB seja estabelecida.
     * @details Essencial para garantir que as primeiras mensagens de log não sejam perdidas.
     */
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("\n--- SMAIV: FASE 5 - SISTEMA MODULAR INTEGRADO ---\n");

    /**
     * @brief Declara e inicializa a estrutura de estado global do sistema.
     */
    system_state_t state = {
        .current_sound_level = 0.0f,
        .sound_threshold = 150.0f,
        .current_screen = SCREEN_MAIN,
        .wifi_connected = false,
        .mqtt_connected = false,
        .alert_active = false 
    };

    /**
     * @brief Inicializa os modulo que rodam no core 0
     */
    ui_init();
    alerts_init();

    /**
     * @brief Inicializa o hardware de áudio e LANÇA o processamento no Core 1
     */
    audio_init();
    printf("Core 0: Lançando processamento de áudio no Core 1...\n");
    audio_launch_on_core1();


    // --- 2. INICIALIZAÇÃO DA CONECTIVIDADE ---

    /**
     * @brief Inicializa o chip Wi-Fi CYW43, habilitando o modo Station (STA).
     */
    if (cyw43_arch_init()) {
        printf("FATAL: Falha ao inicializar o modulo Wi-Fi.\n");
        return -1;
    }
    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi: %s...\n", WIFI_SSID);

    /**
     * @brief Tenta estabelecer a conexão com a rede Wi-Fi configurada.
     */
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("ERRO: Falha ao conectar ao Wi-Fi. Operando em modo offline.\n");
        state.wifi_connected = false;
    } else {
        printf("Wi-Fi conectado com sucesso.\n");
        state.wifi_connected = true;
        mqtt_connect(&state);
    }
    
    printf("Sistema ativo. Entrando no loop principal do Core 0.\n");

    // --- 3. LOOP OPERACIONAL INFINITO (CORE 0) ---

    /**
     * @brief Variável para detecção de borda de subida do estado de alerta.
     */
    bool last_alert_state = false;

     /**
     * @brief // Timestamp para controlar o período após um alarme ser desativado.
     */
    absolute_time_t silenced_until = get_absolute_time();

    while (true) {

        /**
         * @brief Verifica se há um novo valor de RMS enviado pelo Core 1
         */
        if (multicore_fifo_rvalid()) {
            // Lê o valor da FIFO
            uint32_t raw_value = multicore_fifo_pop_blocking();
            // Reconverte o valor para float
            state.current_sound_level = *(float*)&raw_value;
        }

        /**
         * @brief Lógica principal dividida com base no estado de alarme "travado".
         */
         if (state.alert_active) {
            // CONTEXTO: ALARME ATIVO
            // Verifica se o botão A foi pressionado para silenciar o alarme.
            if (!gpio_get(BUTTON_A_PIN)) {
                printf("Alarme silenciado pelo usuário.\n");
                state.alert_active = false;
                state.current_screen = SCREEN_MAIN;

                // Período durante o qual o alarme não pode ser reativado.
                silenced_until = make_timeout_time_ms(5000); // 5 segundos
                
                // Debounce e espera para garantir que o usuário solte o botão
                sleep_ms(500); 
            }
        } else {
            // CONTEXTO: MONITORAMENTO NORMAL
            ui_update_input(&state);
            
            // --- LÓGICA DE REATIVAÇÃO CONTROLADA ---
            // Verifica se o período de silêncio já passou.
            if (time_reached(silenced_until)) {
                last_alert_state = state.alert_active;
                state.alert_active = (state.current_sound_level > state.sound_threshold);

                // Publica o alerta apenas na transição de "normal" para "alerta".
                if (state.alert_active && !last_alert_state) {
                    mqtt_publish_alert(&state);
                }
            }
        }
        
        // --- 4. ATUALIZAÇÃO DOS ATUADORES ---
        
        alerts_update(&state);
        ui_draw(&state);

        sleep_ms(20);
    }

    return 0;
}