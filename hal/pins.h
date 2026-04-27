#ifndef PINS_H
#define PINS_H

#include <avr/io.h>

#define GREEN_LED_DDR   DDRD
#define GREEN_LED_PORT  PORTD
#define GREEN_LED_PIN   PD2

#define RED_LED_DDR     DDRD
#define RED_LED_PORT    PORTD
#define RED_LED_PIN     PD3

#define GREEN_BTN_DDR DDRB
#define GREEN_BTN_PORT PORTB
#define GREEN_BTN_PINR PINB
#define GREEN_BTN_PIN PB4

#endif