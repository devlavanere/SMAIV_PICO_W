/**
 * @file common.h
 * @brief Define tipos de dados e estruturas compartilhadas entre os módulos do sistema.
 * @details Este arquivo evita dependências circulares, permitindo que diferentes
 *          módulos compartilhem o mesmo estado do sistema.
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>

/**
 * @brief Enumeração para os diferentes estados da tela da UI.
 */
typedef enum {
    SCREEN_MAIN,     ///< Tela principal de monitoramento.
    SCREEN_SETTINGS  ///< Tela de configuração de parâmetros.
} screen_t;

/**
 * @brief Estrutura central que representa o estado completo do sistema SMAIAS.
 * @details Uma única instância desta struct é passada entre os módulos para garantir
 *          a consistência dos dados em toda a aplicação.
 */
typedef struct {
    // --- Estado do Áudio ---
    float current_sound_level; ///< Valor RMS atual do som ambiente.
    float sound_threshold;     ///< Limiar de ruído para disparo do alarme.

    // --- Estado da UI ---
    screen_t current_screen;   ///< Tela atualmente ativa no display OLED.
    
    // --- Estado da Conectividade ---
    bool wifi_connected;       ///< Flag que indica se a conexão Wi-Fi foi bem-sucedida.
    bool mqtt_connected;       ///< Flag que indica se a conexão com o broker MQTT está ativa.

    // --- Estado Geral de Alarme ---
    bool alert_active;         ///< Flag "latched" que indica se o sistema está em estado de alarme.

} system_state_t;

#endif