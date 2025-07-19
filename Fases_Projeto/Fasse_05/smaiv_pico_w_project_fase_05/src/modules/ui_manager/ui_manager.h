#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "common.h"

void ui_init(void);
void ui_update_input(system_state_t *state);
void ui_draw(const system_state_t *state);

#endif