#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

typedef enum {
    APP_STATE_IDLE,
    APP_STATE_INPUT_AWAIT,
    APP_STATE_ACCESS_GRANTED
} AppState;

void state_machine_init(void);
void state_machine_step(void);

AppState state_machine_get_state(void);

#endif