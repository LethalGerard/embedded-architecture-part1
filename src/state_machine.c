#include "state_machine.h"
#include "button.h"
#include "pins.h"
#include "gpio.h"
#include "millis.h"
#include <stdint.h>
#include <stdbool.h>
#include <avr/eeprom.h>

static void handle_idle(void);
static void handle_input_await(void);
static void handle_access_granted(void);

static AppState current_state;
static char pin_input[4];
static uint8_t pin_idx = 0;

static char current_pin[4] = {'1', '7', '7', '2'};
static uint8_t EEMEM eeprom_pin[4] = {'1', '7', '7', '2'};

void state_machine_init(void)
{
        current_state = APP_STATE_IDLE;
    eeprom_read_block((void*)current_pin, (const void*)eeprom_pin, 4);

    bool valid = true;
    for (uint8_t i = 0; i < 4; i++)
    {
        if (current_pin[i] < '0' || current_pin[i] > '9')
        {
            valid = false;
            break;
        }
    }

    if (!valid)
    {
        current_pin[0] = '1';
        current_pin[1] = '7';
        current_pin[2] = '7';
        current_pin[3] = '2';
        eeprom_update_block((const void*)current_pin, (void*)eeprom_pin, 4);
    }
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

    if (!entered)
    {
        entered = true;
        entered_ms = now_ms;
        last_blink_ms = now_ms;
        pin_idx = 0;
    }

    char key = keypad_scan();

    if (key != 0 && pin_idx < 4)
    {
        pin_input[pin_idx++] = key;

        gpio_pin_high(&GREEN_LED_PORT, GREEN_LED_PIN);
        green_pulse_active = true;
        green_pulse_start_ms = now_ms;
    }

    if (green_pulse_active && (now_ms - green_pulse_start_ms) >= 150U)
    {
        gpio_pin_low(&GREEN_LED_PORT, GREEN_LED_PIN);
        green_pulse_active = false;
    }

    if (pin_idx == 4)
    {
        bool pin_ok = true;
        for (uint8_t i = 0; i < 4; i++)
        {
            if (pin_input[i] != current_pin[i])
            {
                pin_ok = false;
                break;
            }
        }

        if (pin_ok)
        {
            current_state = APP_STATE_ACCESS_GRANTED;
        }
        else
        {
            current_state = APP_STATE_IDLE;
        }

        entered = false;
        pin_idx = 0;
        return;
    }

    if ((now_ms - entered_ms) >= 5000U)
    {
        current_state = APP_STATE_IDLE;
        entered = false;
        pin_idx = 0;
        return;
    }

    if (!green_pulse_active)
    {
        gpio_pin_low(&GREEN_LED_PORT, GREEN_LED_PIN);
    }

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

bool state_machine_set_pin(const char old_pin[4], const char new_pin[4])
{
        if (current_state != APP_STATE_IDLE)
    {
        return false;
    }

    for (uint8_t i = 0; i < 4; i++)
    {
        if (current_pin[i] != old_pin[i])
        {
            return false;
        }
    }

    for (uint8_t i = 0; i < 4; i++)
    {
        current_pin[i] = new_pin[i];
    }

    eeprom_update_block((const void*)current_pin, (void*)eeprom_pin, 4);
    return true;
}