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

static const LED tuning_led[19] = {
    {GPIOD, GPIO_PIN_7},   // P0 Red1
    {GPIOD, GPIO_PIN_6},   // P1 Red2
    {GPIOD, GPIO_PIN_5},   // P2 Red3
    {GPIOD, GPIO_PIN_4},   // P3 Red4
    {GPIOE, GPIO_PIN_2},   // P4 Yel1
    {GPIOE, GPIO_PIN_4},   // P5 Yel2
    {GPIOE, GPIO_PIN_5},   // P6 Yel3
    {GPIOE, GPIO_PIN_6},   // P7 Yel4
    {GPIOF, GPIO_PIN_13},  // P8 Grn1
    {GPIOE, GPIO_PIN_9},   // P9 Grn2
    {GPIOE, GPIO_PIN_11},  // P10 Grn3
    {GPIOE, GPIO_PIN_13},  // P11 Yel5
    {GPIOF, GPIO_PIN_15},  // P12 Yel6
    {GPIOG, GPIO_PIN_14},  // P13 Yel7
    {GPIOG, GPIO_PIN_9},   // P14 Yel8
    {GPIOE, GPIO_PIN_14},  // P15 Red5
    {GPIOE, GPIO_PIN_15},  // P16 Red6
    {GPIOB, GPIO_PIN_10},  // P17 Red7
    {GPIOB, GPIO_PIN_11}   // P18 Red8
};

static const LED status_led[4] = {
    {GPIOD, GPIO_PIN_3},   // P0 Wht1
    {GPIOF, GPIO_PIN_8},   // P1 Wht2
    {GPIOF, GPIO_PIN_14},  // P2 Wht3
    {GPIOE, GPIO_PIN_12},  // P3 Wht4
};

void LED_ON(uint16_t position){
    HAL_GPIO_WritePin(tuning_led[position].port, tuning_led[position].pin, GPIO_PIN_SET);
}

void LED_OFF(uint16_t position){
    HAL_GPIO_WritePin(tuning_led[position].port, tuning_led[position].pin, GPIO_PIN_RESET);
}

void LED_all_off(){
    for(uint16_t i=0; i<19; i++){
        LED_OFF(i);
    }
}

void blink_status(){
    HAL_GPIO_WritePin(status_led[0].port, status_led[0].pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(status_led[1].port, status_led[1].pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(status_led[2].port, status_led[2].pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(status_led[3].port, status_led[3].pin, GPIO_PIN_SET);
    HAL_Delay(200); //vTaskDelay(20 / portTICK_PERIOD_MS);
    HAL_GPIO_WritePin(status_led[0].port, status_led[0].pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(status_led[1].port, status_led[1].pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(status_led[2].port, status_led[2].pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(status_led[3].port, status_led[3].pin, GPIO_PIN_RESET);
    HAL_Delay(200); //vTaskDelay(20 / portTICK_PERIOD_MS);
}