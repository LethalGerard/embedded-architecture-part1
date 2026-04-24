#include "app.h"
#include "config.h"
#include "gpio.h"
#include "pins.h"
#include "uart.h"
#include "millis.h"
#include <string.h>
#include "state_machine.h"
#include "button.h"

#define CMD_BUFFER_SIZE 32

static char cmd_buffer[CMD_BUFFER_SIZE];
static unsigned char cmd_index = 0;

static void process_command(const char *cmd)
{
    if (strcmp(cmd, "led on") == 0)
    {
        gpio_pin_high(&LED_PORT, LED_PIN);
        uart_write_string("LED is ON\n");
    }
    else if (strcmp(cmd, "led off") == 0)
    {
        gpio_pin_low(&LED_PORT, LED_PIN);
        uart_write_string("LED is OFF\n");
    }
    else if (strcmp(cmd, "led toggle") == 0)
    {
        gpio_pin_toggle(&LED_PORT, LED_PIN);
        uart_write_string("LED toggled\n");
    }
    else if (strcmp(cmd, "help") == 0)
    {
        uart_write_string("Commands: help, led on, led off, led toggle\n");
    }
    else
    {
        uart_write_string("Unknown command\n");
    }
}

void app_init(void)
{
    state_machine_init();
    gpio_pin_output(&LED_DDR, LED_PIN);
    gpio_pin_low(&LED_PORT, LED_PIN);
    button_init();

    millis_init();

    uart_init(UART_BAUDRATE);
    uart_write_string("System ready\n");
    uart_write_string("Type: help\n");
    if (state_machine_get_state() == APP_STATE_IDLE)
    {
        uart_write_string("Current state: IDLE\n");
    }
}

void app_run(void)
{
    char c;
    state_machine_step();

    static uint32_t last_press_ms = 0;
    uint32_t now_ms = millis_get();
    const uint32_t DEBOUNCE_MS = 60;

    static bool prev_pressed = false;
    bool now_pressed = is_green_button_pressed();

    if (now_pressed && !prev_pressed)
    {
        if ((now_ms - last_press_ms) >= DEBOUNCE_MS)
        {
            state_machine_on_green_btn();
            last_press_ms = now_ms;
        }
    }
    if (!now_pressed && prev_pressed) // Protects from accidental press when realing button
    {
        last_press_ms = now_ms;
    }
    prev_pressed = now_pressed;

    while (uart_read_char(&c))
    {
        uart_write_char(c);

        if (c == '\r' || c == '\n')
        {
            uart_write_string("\n");

            if (cmd_index > 0)
            {
                cmd_buffer[cmd_index] = '\0';
                process_command(cmd_buffer);
                cmd_index = 0;
            }
        }
        else
        {
            if (cmd_index < (CMD_BUFFER_SIZE - 1))
            {
                cmd_buffer[cmd_index++] = c;
            }
            else
            {
                cmd_index = 0;
                uart_write_string("\nCommand too long\n");
            }
        }
    }
}