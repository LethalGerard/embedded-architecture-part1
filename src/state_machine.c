#include "state_machine.h"

static AppState current_state;

void state_machine_init(void)
{
    current_state = APP_STATE_IDLE;
}

void state_machine_on_green_btn()
{
    if (current_state == APP_STATE_IDLE)
    {
        current_state = APP_STATE_INPUT_AWAIT;
    }
}

AppState state_machine_get_state(void)
{
    return current_state;
}

void state_machine_step(void)
{
    switch (current_state)
    {
    case APP_STATE_IDLE:
        break;
    case APP_STATE_ACCESS_GRANTED:
        break;
    case APP_STATE_INPUT_AWAIT:
        break;
    default:
        current_state = APP_STATE_IDLE;
        break;
    }
}