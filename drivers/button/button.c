#include "gpio.h"
#include "button.h"
#include "pins.h"

void button_init()
{
    gpio_pin_input_pullup(&GREEN_BTN_DDR, &GREEN_BTN_PORT, GREEN_BTN_PIN);
}

bool is_green_button_pressed()
{
    return (GREEN_BTN_PINR &(1 << GREEN_BTN_PIN)) == 0;
}

void keypad_init(void)
{
    DDRB |= (1U << PB0) | (1U <<PB1) | (1U << PB2) | (1U << PB3);
    PORTB |= (1U << PB0) | (1U <<PB1) | (1U << PB2) | (1U << PB3);

    DDRD &= ~((1U <<PD7) | (1U << PD6) | (1U << PD5) | (1U <<PD4));
    PORTD |= (1U <<PD7) | (1U << PD6) | (1U << PD5) | (1U <<PD4);
}

static const char keymap[4][4] = {
    { '1', '2', '3', 'A'},
    { '4', '5', '6', 'B'},
    { '7', '8', '9', 'C'},
    { '*', '0', '#', 'D'}
};

char keypad_scan(void)
{
    static char last_key = 0;
    char detected = 0;

    for (uint8_t row = 0; row < 4; row++)
    {
        PORTB |= (1U << PB0) | (1U << PB1) | (1U << PB2) | (1U <<PB3);

        PORTB &= ~(1U << (PB3 - row));

        if (!(PIND & (1U << PD7))) { detected = keymap[row][0]; break; }
        if (!(PIND & (1U << PD6))) { detected = keymap[row][1]; break; }
        if (!(PIND & (1U << PD5))) { detected = keymap[row][2]; break; }
        if (!(PIND & (1U << PD4))) { detected = keymap[row][3]; break; }
    }

    if (detected != 0 && last_key == 0)
    {
        last_key = detected;
        return detected;
    }

    if (detected == 0)
    {
        last_key = 0;
    }

    return 0;
}