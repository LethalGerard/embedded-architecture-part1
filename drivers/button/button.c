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