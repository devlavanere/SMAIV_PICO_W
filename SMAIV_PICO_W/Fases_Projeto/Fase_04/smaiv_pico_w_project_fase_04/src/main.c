#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "config.h"

// Bibliotecas de Rede
#include "lwip/apps/mqtt.h"
#include "lwip/dns.h" 
#include "lwipopts.h"

// Biblioteca do display OLED
#include "ssd1306/ssd1306.h"

// ==================================================================
// SEÇÃO DE CONFIGURAÇÃO E ESTADO GLOBAL
// ==================================================================

// --- Estrutura e Variável Global para o Estado do MQTT ---
typedef struct MQTT_STATE_T {
    ip_addr_t remote_addr;
    mqtt_client_t *mqtt_client;
    bool connected;
} MQTT_STATE_T;
static MQTT_STATE_T mqtt_state;

// --- Estado do Sistema ---
typedef enum {
    SCREEN_MAIN,
    SCREEN_SETTINGS
} screen_t;

static screen_t current_screen = SCREEN_MAIN;
static float sound_threshold = 150.0f; // Limiar agora é uma variável para podermos ajustá-la

// -- Parâmetros de Áudio --
#define SAMPLE_COUNT 256 // Número de amostras para calcular o RMS. Um bom equilíbrio entre velocidade e escalabilidade.

// --- Objeto do Display ---
static ssd1306_t disp;

// --- DEFINIÇÃO DA FUNÇÃO DE CALLBACK PARA O SCAN ---

static int scan_result(void *env, const cyw43_ev_scan_result_t *result);

static int scan_result(void *env, const cyw43_ev_scan_result_t *result) {
    if (result) {
        printf("SSID: %-32s | RSSI: %4d | Canal: %3d | MAC: %02x:%02x:%02x:%02x:%02x:%02x | Seg: %d\n",
            result->ssid, result->rssi, result->channel,
            result->bssid[0], result->bssid[1], result->bssid[2], result->bssid[3], result->bssid[4], result->bssid[5],
            result->auth_mode);
    }
    return 0;
}

// =================================================================================
// SEÇÃO DE REDE E MQTT
// =================================================================================
static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    if (status == MQTT_CONNECT_ACCEPTED) {
        printf("MQTT: Conectado com sucesso!\n");
        mqtt_state.connected = true;
    } else {
        printf("MQTT: Falha na conexao, codigo: %d\n", status);
        mqtt_state.connected = false;
    }
}

static void dns_mqtt_found(const char *name, const ip_addr_t *ipaddr, void *arg) {
    if (!ipaddr) {
        printf("DNS: falha ao resolver %s\n", MQTT_BROKER_HOST);
        return;
    }
    printf("DNS: %s -> %s\n", name, ip4addr_ntoa(ipaddr));
    mqtt_state.remote_addr = *ipaddr;

    struct mqtt_connect_client_info_t ci;
    memset(&ci, 0, sizeof(ci));
    ci.client_id = MQTT_CLIENT_ID;
    ci.keep_alive = 60;

    mqtt_client_connect(mqtt_state.mqtt_client,
                        &mqtt_state.remote_addr,
                        MQTT_BROKER_PORT,
                        mqtt_connection_cb, NULL, &ci);
}

static void mqtt_init(void) {
    mqtt_state.mqtt_client = mqtt_client_new();
    if (mqtt_state.mqtt_client == NULL) {
        printf("Erro ao criar cliente MQTT.\n");
        return;
    }

    struct mqtt_connect_client_info_t ci;
    memset(&ci, 0, sizeof(ci));
    ci.client_id = MQTT_CLIENT_ID;
    ci.keep_alive = 60;

    err_t err = dns_gethostbyname(MQTT_BROKER_HOST, &mqtt_state.remote_addr, dns_mqtt_found, NULL);

    if (err == ERR_OK) {
        // IP está em cache DNS
        printf("DNS: em cache -> %s\n", ip4addr_ntoa(&mqtt_state.remote_addr));
        mqtt_client_connect(mqtt_state.mqtt_client,
                            &mqtt_state.remote_addr,
                            MQTT_BROKER_PORT,
                            mqtt_connection_cb, NULL, &ci);
    } else if (err == ERR_INPROGRESS) {
        printf("DNS: resolvendo %s…\n", MQTT_BROKER_HOST);
        // A conexão será feita no callback dns_mqtt_found()
    } else {
        printf("DNS: erro %d ao resolver hostname\n", err);
    }
}


static void publish_alert(float sound_level) {
    if (!mqtt_state.connected) { return; }

    char payload[64];
    snprintf(payload, sizeof(payload), "{\"device_id\":\"%s\",\"level\":%.1f}", MQTT_CLIENT_ID, sound_level);
    
    mqtt_publish(mqtt_state.mqtt_client, MQTT_TOPIC_ALERT, payload, strlen(payload), 1, 0, NULL, NULL);
    printf("MQTT: Alerta publicado.\n");
}

// =================================================================================
// SEÇÃO DE PROCESSAMENTO DE ÁUDIO (FASE 2)
// =================================================================================
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
       // Mostra o status da conexão MQTT
        if (mqtt_state.connected) {
            ssd1306_draw_string(&disp, 0, 48, 1, "Conectado");
        } else {
            ssd1306_draw_string(&disp, 0, 48, 1, "Conectando...");
        }
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

    printf("\n--- SMAIAS: FASE 4 - CONECTIVIDADE MQTT ---\n");

    // --- Inicialização dos Periféricos e Rede ---
    if (cyw43_arch_init()) {
        printf("Falha ao inicializar o modulo Wi-Fi.\n");
        return -1;
    }
    cyw43_arch_enable_sta_mode();

    printf("Tentando conectar a rede: %s ...\n", WIFI_SSID);

    int connection_status = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 15000);

    if (connection_status != 0) {
        printf("Falha ao conectar. Codigo de erro: %d\n", connection_status);
        printf("\n--- INICIANDO SCAN DE REDES WI-FI VISIVEIS ---\n");
        
        // Inicia um scan de Wi-Fi. A função 'scan_result' será chamada para cada rede encontrada.
        cyw43_wifi_scan_options_t scan_options = {0};
        int err = cyw43_wifi_scan(&cyw43_state, &scan_options, NULL, scan_result);
        
        if (err == 0) {
            printf("\nScan iniciado. Aguardando resultados...\n");
            sleep_ms(10000); // Espera 10 segundos para o scan completar
        } else {
            printf("Nao foi possivel iniciar o scan. Erro: %d\n", err);
        }
        
        printf("\n--- DIAGNOSTICO CONCLUIDO. TRAVANDO O PROGRAMA. ---\n");
        printf("Verifique na lista acima se a sua rede '%s' aparece.\n", WIFI_SSID);
        
        while(true) { // Trava o programa aqui
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
            sleep_ms(500);
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
            sleep_ms(500);
        }
    }
    // Se o código chegou até aqui, a conexão foi bem-sucedida.
    printf("Conectado ao Wi-Fi com sucesso!\n");

    printf("Aguardando estabilizacao da rede (2s)...\n");
    sleep_ms(2000);
    
    mqtt_init();

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
    
    printf("Sistema ativo. Aguardando conexao MQTT...\n");

    // --- Loop Principal ---
    
    bool alert_sent_this_event = false;

    while(true) {
        handle_input();

        // Ler sensores e atualizar a lógica
        float sound_level = get_sound_level();
        bool alert_triggered = sound_level > sound_threshold;

        if (alert_triggered) {
            // Publica o alerta apenas uma vez por evento
            if (!alert_sent_this_event) {
                publish_alert(sound_level);
                alert_sent_this_event = true;
            }
        } else {
            alert_sent_this_event = false; // Reseta o flag quando o som volta ao normal
        }

        // Atualizar atuadores (LEDs e Display)
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