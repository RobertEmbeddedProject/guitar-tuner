#include <stdint.h>

//typedef creates an alias for struct
typedef struct{
    float freq_hz;
    float confidence;
    uint8_t valid;
    float cents;
} YIN_Result_t;

YIN_Result_t YIN_DetectPitch(const uint16_t *adc_buf, uint32_t n, float sample_rate_hz);
void YIN_print(YIN_Result_t r, uint32_t led);
float parab_interpolation(const float *arr, uint32_t tau);
float hz_conversion(YIN_Result_t r, uint32_t target_hz);

/*
r'_t(tau) = x[t+1] * x[t+1+tau]
          + x[t+2] * x[t+2+tau]
          + x[t+3] * x[t+3+tau]
          + ...
          + x[t+W-tau] * x[t+W]

    tau = lag
    W   = window size
*/
    
    