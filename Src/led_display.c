#include "led_display.h"
#include "IO_init.h"
#include <math.h>

#define cents_width 10

//LED moving scale; suggests LED to illuminate based on pitch using frequency (cents)
uint32_t LED_tuning_ind(float cents)
{
    if (fabsf(cents) <= 5.0f)
        return 9;

    int led = 9 - (int)lroundf(cents / 10.0f);

    if (led < 0)
        led = 0;

    if (led > 18)
        led = 18;

    return (uint32_t)led;
}