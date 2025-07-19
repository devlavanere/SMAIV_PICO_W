/**
 * @file mqtt_comm.c
 * @brief Gerencia a conectividade de rede e a comunicação via protocolo MQTT.
 * @details Este módulo encapsula a lógica de conexão Wi-Fi, resolução de DNS assíncrona
 *          e a publicação de mensagens MQTT para o broker.
 */
#include "mqtt_comm.h"
#include "config.h"
#include "lwip/apps/mqtt.h"
#include "lwip/dns.h"
#include <string.h>

/**
 * @brief Estrutura interna para manter o estado do cliente LwIP MQTT.
 */
typedef struct MQTT_STATE_T {
    ip_addr_t remote_addr;      ///< Endereço IP resolvido do broker.
    mqtt_client_t *mqtt_client; ///< Ponteiro para a instância do cliente MQTT da LwIP.
} MQTT_STATE_T;
static MQTT_STATE_T internal_state; ///< Instância estática interna do estado MQTT.

/**
 * @brief Ponteiro para a estrutura de estado global do sistema.
 * @details Usado para atualizar o status da conexão MQTT no estado principal da aplicação.
 */
static system_state_t *global_state_ptr = NULL;

/**
 * @brief Callback invocado pela LwIP quando o estado da conexão MQTT muda.
 * @param client Instância do cliente MQTT.
 * @param arg Argumento opcional passado durante a conexão (não utilizado aqui).
 * @param status Novo status da conexão (ex: MQTT_CONNECT_ACCEPTED).
 */
static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    if (status == MQTT_CONNECT_ACCEPTED) {
        printf("MQTT: Conectado com sucesso!\n");
        if(global_state_ptr) global_state_ptr->mqtt_connected = true;
    } else {
        printf("MQTT: Falha na conexao, codigo: %d\n", status);
        if(global_state_ptr) global_state_ptr->mqtt_connected = false;
    }
}

/**
 * @brief Callback invocado pela LwIP quando a resolução de DNS é concluída.
 * @details Esta abordagem assíncrona garante que a tentativa de conexão MQTT só ocorra
 *          após o endereço IP do broker ser obtido com sucesso.
 * @param name Nome do host que foi resolvido.
 * @param ipaddr Ponteiro para o endereço IP encontrado.
 * @param arg Argumento opcional (não utilizado).
 */
static void dns_found_cb(const char *name, const ip_addr_t *ipaddr, void *arg) {
    // Aborta se a resolução de DNS falhar.
    if (ipaddr == NULL) { 
        printf("DNS: Falha ao resolver o hostname '%s'.\n", MQTT_BROKER_HOST);
        return; 
    }
    
    // Armazena o IP resolvido e inicia a conexão MQTT.
    internal_state.remote_addr = *ipaddr;
    struct mqtt_connect_client_info_t client_info;
    memset(&client_info, 0, sizeof(client_info));
    client_info.client_id = MQTT_CLIENT_ID;
    client_info.keep_alive = 60;
    
    mqtt_client_connect(internal_state.mqtt_client, &internal_state.remote_addr, MQTT_BROKER_PORT, mqtt_connection_cb, NULL, &client_info);
}

/**
 * @brief Inicia o processo de conexão MQTT.
 * @details Cria o cliente MQTT e dispara a resolução de nome DNS. A conexão
 *          real é estabelecida dentro do callback `dns_found_cb`.
 * @param state Ponteiro para o estado global do sistema.
 */
void mqtt_connect(system_state_t *state) {
    global_state_ptr = state; // Salva o ponteiro para o estado global para uso nos callbacks.
    internal_state.mqtt_client = mqtt_client_new();
    if (internal_state.mqtt_client == NULL) { return; }
    
    // Dispara a requisição de DNS. É uma operação não-bloqueante.
    dns_gethostbyname(MQTT_BROKER_HOST, &internal_state.remote_addr, dns_found_cb, NULL);
}

/**
 * @brief Publica uma mensagem de alerta no tópico MQTT configurado.
 * @param state Ponteiro para o estado do sistema, de onde o nível de som é lido.
 */
void mqtt_publish_alert(const system_state_t *state) {
    if (!state->mqtt_connected) { return; }
    
    char payload[128];
    snprintf(payload, sizeof(payload), "{\"message\":\"ALERTA DE SOM ALTO DETECTADO!\", \"sound_level\":%.1f, \"threshold\":%.1f}", 
             state->current_sound_level, 
             state->sound_threshold);
    
    // Publica a mensagem com QoS 1 para garantir pelo menos uma entrega.
    mqtt_publish(internal_state.mqtt_client, MQTT_TOPIC_ALERT, payload, strlen(payload), 1, 0, NULL, NULL);
    printf("MQTT: Alerta publicado.\n");
}

/**
 * @brief Retorna o status atual da conexão MQTT.
 * @return true se o cliente MQTT estiver conectado, false caso contrário.
 */
bool mqtt_is_connected(void){
    // Verifica se o ponteiro global é válido antes de acessá-lo.
    return global_state_ptr ? global_state_ptr->mqtt_connected : false;
}