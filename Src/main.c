#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "IO_init.h"
#include "yin_tuner.h"
#include "led_display.h"
#include "main.h"
//#include "python_graph.h"

static int FindClosestGuitarString(float freq_hz);
static void ProcessAudioBlock(const uint16_t *samples, uint32_t sample_count);
static void YIN_print(YIN_Result_t r, float cents, uint32_t led);

#define SAMPLE_RATE_HZ 20000.0f
#define DMA_HALF_SAMPLES 2048
#define DMA_TOTAL_SAMPLES (2 * DMA_HALF_SAMPLES)
uint16_t adc_dma_buf[DMA_TOTAL_SAMPLES];

typedef struct
{
    const char *name;
    float freq_hz;
} GuitarString_t;

//extern cents = 1200.0f * log2f(measured_hz / target_hz);
static const GuitarString_t strings[] =
{
    {"Low E", 82.41f},
    {"A",     110.00f},
    {"D",     146.83f},
    {"G",     196.00f},
    {"B",     246.94f},
    {"High E",329.63f}
};


int main(void)
{
    HAL_Init();
    SystemClock_Config(); //added for increasing clock to 216Mhz

    //Enable peripheral clocks
    __HAL_RCC_DMA2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    //IO_init drivers
    UART6_Init();
    MX_TIM2_Init();
    MX_ADC1_Init();
    LED_Init();
    
    //DMA Driver + Error Handler
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_dma_buf, DMA_TOTAL_SAMPLES) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_Base_Start(&htim2) != HAL_OK)
    {
        Error_Handler();
    }

    // Main Loop
    while (1)
    {
            if (adc_half_ready)
        {
            adc_half_ready = 0;
            ProcessAudioBlock(&adc_dma_buf[0], DMA_HALF_SAMPLES);
        }

        if (adc_full_ready)
        {
            adc_full_ready = 0;
            ProcessAudioBlock(&adc_dma_buf[DMA_HALF_SAMPLES], DMA_HALF_SAMPLES);
        }
    }
}

static int FindClosestGuitarString(float freq_hz)
{
    int best_index = 0;
    float best_error = 999999.0f;

    for (int i = 0; i < 6; i++)
    {
        float error = fabsf(
            1200.0f * log2f(freq_hz / strings[i].freq_hz)
        );

        if (error < best_error)
        {
            best_error = error;
            best_index = i;
        }
    }

    return best_index;
}

static void ProcessAudioBlock(const uint16_t *samples, uint32_t sample_count)
{
    YIN_Result_t r = YIN_DetectPitch(samples, sample_count, SAMPLE_RATE_HZ);

    if (!r.valid)
    {
        LED_all_off();
        return;
    }

    int string_index = FindClosestGuitarString(r.freq_hz);
    float target_hz = strings[string_index].freq_hz;
    float cents = 1200.0f * log2f(r.freq_hz / target_hz);

    uint32_t led = LED_tuning_ind(cents);

    YIN_print(r, cents, led);

    LED_all_off();
    LED_ON(led);
}

//puTTy Frequency Confidence Printing for troubleshooting:
void YIN_print(YIN_Result_t r, float cents, uint32_t led)
{
    char msg[128];
    int len;

    if (r.valid)
    {
        uint32_t freq_x100 = (uint32_t)(r.freq_hz * 100.0f);
        int32_t cents_x100 = (int32_t)(cents * 100.0f);

        len = snprintf(msg, sizeof(msg),
               "freq=%lu.%02lu Hz cents=%ld.%02ld LED=P%lu\r\n",
               freq_x100 / 100,
               freq_x100 % 100,
               cents_x100 / 100,
               labs(cents_x100 % 100),
               led);
    }
    else
    {
        len = snprintf(msg, sizeof(msg),
                       "freq=--- Hz confidence=0.00 BAD\r\n");
    }

    HAL_UART_Transmit(&huart6, (uint8_t *)msg, len, HAL_MAX_DELAY);
}
