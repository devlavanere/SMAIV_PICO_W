/**
 * @file config.h
 * @author Michel L. Sampaio
 * @brief Arquivo de configuração central para o projeto SMAIAS.
 * @version 5.0
 * 
 * @details
 * Este arquivo centraliza todas as constantes e definições que podem precisar
 * ser ajustadas, como mapeamento de pinos de hardware, credenciais de rede e
 * parâmetros de comportamento do sistema. Manter essas configurações em um
 * único local facilita a manutenção e a adaptação do projeto para diferentes
 * hardwares ou ambientes de rede.
 */

#ifndef CONFIG_H
#define CONFIG_H

// =================================================================================
// SEÇÃO DE MAPEAMENTO DE PINOS (HARDWARE)
// =================================================================================
// Este mapeamento corresponde à pinagem da placa de desenvolvimento BitDogLab.

// --- Periféricos de Entrada ---
#define MIC_ADC_PIN     28      ///< GPIO conectado ao microfone de eletreto.
#define MIC_ADC_INPUT   2       ///< Canal do ADC correspondente ao GPIO28.

#define JOYSTICK_Y_PIN  26      ///< GPIO para o eixo Y do joystick (usado para ajustes).
#define JOYSTICK_SW_PIN 22      ///< GPIO para o botão (switch) do joystick (entrar no menu).
#define BUTTON_A_PIN    5       ///< GPIO para o botão 'A' (salvar/resetar alarme).

// --- Periféricos de Saída ---
#define OLED_I2C_PORT   i2c1    ///< Instância do barramento I2C para o display.
#define OLED_SDA_PIN    14      ///< Pino de dados (SDA) para o display I2C.
#define OLED_SCL_PIN    15      ///< Pino de clock (SCL) para o display I2C.

#define RGB_R_PIN       13      ///< Pino para o canal Vermelho do LED RGB.
#define RGB_G_PIN       11      ///< Pino para o canal Verde do LED RGB.
#define RGB_B_PIN       12      ///< Pino para o canal Azul do LED RGB.

#define BUZZER_PIN      21      ///< GPIO conectado ao buzzer para alertas sonoros.

#define WS2812_PIN      7       ///< Pino de dados para a matriz de LEDs WS2812B.
#define WS2812_NUM_LEDS 25      ///< Número total de LEDs na matriz (5x5).


// =================================================================================
// SEÇÃO DE CONFIGURAÇÃO DE REDE
// =================================================================================

/**
 * @brief Credenciais para a rede Wi-Fi.
 * @details Substitua pelos dados da sua rede (ou do hotspot de teste).
 */
#define WIFI_SSID       "PicowTest"
#define WIFI_PASSWORD   "aa5904bed354"

/**
 * @brief Configurações para o broker MQTT.
 * @details Para testes, um broker público é utilizado. Em produção, seria um
 *          broker privado e seguro.
 */
#define MQTT_BROKER_HOST "broker.hivemq.com"                     ///< Endereço do broker MQTT.
#define MQTT_BROKER_PORT 1883                                    ///< Porta padrão para MQTT não criptografado.
#define MQTT_CLIENT_ID   "mqtt-smaias"              ///< ID único para este dispositivo no broker.
#define MQTT_TOPIC_ALERT "smaias/alerta"     ///< Tópico onde os alertas serão publicados.

#endif