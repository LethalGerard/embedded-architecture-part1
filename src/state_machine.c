#include "state_machine.h"
#include "button.h"
#include "pins.h"
#include "gpio.h"
#include "millis.h"
#include <stdint.h>
#include <stdbool.h>

static void handle_idle(void);
static void handle_input_await(void);
static void handle_access_granted(void);

static AppState current_state;
static char pin_input[4];
static uint8_t pin_idx = 0;

static const char correct_pin[4] = { '1', '7', '7', '2'};

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
            handle_idle();
        break;
    case APP_STATE_ACCESS_GRANTED:
            handle_access_granted();
        break;
    case APP_STATE_INPUT_AWAIT:
            handle_input_await();
        break;
    default:
        current_state = APP_STATE_IDLE;
        break;
    }
}

static void handle_idle(void)
{
    gpio_pin_high(&RED_LED_PORT, RED_LED_PIN);
    gpio_pin_low(&GREEN_LED_PORT, GREEN_LED_PIN);
}

static void handle_input_await(void)
{
    static bool green_pulse_active = false;
    static uint32_t green_pulse_start_ms = 0;
    static uint32_t last_blink_ms = 0;
    const uint32_t BLINK_PERIOD_MS = 300;
    uint32_t now_ms = millis_get();
    static bool entered = false;
    static uint32_t entered_ms = 0;
    char key = keypad_scan();

    if (key != 0 && pin_idx < 4)
    {
        pin_input[pin_idx++] = key;

        gpio_pin_high(&GREEN_LED_PORT, GREEN_LED_PIN);
        green_pulse_active = true;
        green_pulse_start_ms = now_ms;

        if (green_pulse_active && (now_ms - green_pulse_start_ms) >= 150U)
        {
            gpio_pin_low(&GREEN_LED_PORT, GREEN_LED_PIN);
            green_pulse_active = false;
        }
    }

    if (pin_idx == 4)
    {
        bool pin_ok = true;
        for (uint8_t i = 0; i < 4; i++)
        {
            if (pin_input[i] != correct_pin[i])
            {
                pin_ok = false;
                break;
            }
        }

        if (pin_ok)
        {
            current_state = APP_STATE_ACCESS_GRANTED;
        } else {
            current_state = APP_STATE_IDLE;
        }

        entered = false;
        pin_idx = 0;
        return;
    }

    if (!entered)
    {
        entered = true;
        entered_ms = now_ms;
        last_blink_ms = now_ms;
        pin_idx = 0;
    }

    if ((now_ms - entered_ms) >= 5000U)
    {
        current_state = APP_STATE_IDLE;
        entered = false;
        pin_idx = 0;
        return;
    }

    gpio_pin_low(&GREEN_LED_PORT, GREEN_LED_PIN);

    if ((now_ms - last_blink_ms) >= BLINK_PERIOD_MS)
    {
        gpio_pin_toggle(&RED_LED_PORT, RED_LED_PIN);
        last_blink_ms = now_ms;
    }
}

static void handle_access_granted(void)
{
    static bool entered = false;
    static uint32_t start_ms = 0;
    uint32_t now_ms = millis_get();

    gpio_pin_low(&RED_LED_PORT, RED_LED_PIN);
    gpio_pin_high(&GREEN_LED_PORT, GREEN_LED_PIN);
    
    if (!entered)
    {
        entered = true;
        start_ms = now_ms;
    }

    if ((now_ms - start_ms) >= 3000U)
    {
        current_state = APP_STATE_IDLE;
        entered = false;
    }
}