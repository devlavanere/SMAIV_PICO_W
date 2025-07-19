#ifndef MQTT_COMM_H
#define MQTT_COMM_H

#include "common.h"

void mqtt_connect(system_state_t *state);
void mqtt_publish_alert(const system_state_t *state);
bool mqtt_is_connected(void);

#endif