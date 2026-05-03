#include <stdbool.h>
#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

typedef enum {
    APP_STATE_IDLE,
    APP_STATE_INPUT_AWAIT,
    APP_STATE_ACCESS_GRANTED
} AppState;

void state_machine_init(void);
void state_machine_step(void);
void state_machine_on_green_btn(void);

AppState state_machine_get_state(void);

bool state_machine_set_pin(const char old_pin[4], const char new_pin[4]);

#endif