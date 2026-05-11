#pragma once
#include <stdint.h>

typedef struct
{
    float freq_hz;
    float confidence;
    uint8_t valid;
} YIN_Result_t;

float parab_interpolation(const float *arr, uint32_t tau);
YIN_Result_t YIN_DetectPitch(const uint16_t *adc_buf,
                             uint32_t n,
                             float sample_rate_hz);

void YIN_print(YIN_Result_t r, float cents, uint32_t led);