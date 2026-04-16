#include "main.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>             //provides ptrs, array size utilities
#include <FreeRTOS.h>
#include <semphr.h>             //semaphore APIs
#include <stm32f7xx_hal.h>      //ST HAL
#include <task.h>               //vTask APIs
#include <SEGGER_SYSVIEW.h>
#include <SEGGER_RTT.h>

extern UART_HandleTypeDef huart6;
void UART6_Init(void);
void Error_Handler();




