cmake_minimum_required(VERSION 3.22)

#project(Guitar_Tuner C ASM)
project(Guitar_Tuner C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Expect arm-none-eabi-* tools to be on PATH (STM32CubeCLT or GNU Arm Embedded Toolchain)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SIZE arm-none-eabi-size)

set(MCU_FLAGS
    -mcpu=cortex-m7
    -mthumb
    -mfpu=fpv5-d16
    -mfloat-abi=hard
)

add_compile_definitions(
    USE_HAL_DRIVER
    STM32F767xx
    USE_FULL_ASSERT=1
)

set(COMMON_WARNINGS
    -Wall
)

set(COMMON_OPT
    -Os
    -ffunction-sections
    -fdata-sections
)

set(DEBUG_EXTRAS
    -g3
)

add_compile_options(
    ${MCU_FLAGS}
    ${COMMON_WARNINGS}
    ${COMMON_OPT}
    ${DEBUG_EXTRAS}
    $<$<COMPILE_LANGUAGE:C>:-std=gnu11>
    $<$<COMPILE_LANGUAGE:C>:-fstack-usage>
    $<$<COMPILE_LANGUAGE:C>:-fcyclomatic-complexity>
    $<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>
    $<$<COMPILE_LANGUAGE:ASM>:-Wa,-W>
    --specs=nano.specs
)

include_directories(
    ${CMAKE_SOURCE_DIR}/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F7xx/Include
    ${CMAKE_SOURCE_DIR}/Drivers/STM32F7xx_HAL_Driver/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy
    ${CMAKE_SOURCE_DIR}/Middleware/Third_Party/FreeRTOS/Source/include
    ${CMAKE_SOURCE_DIR}/Middleware/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
    ${CMAKE_SOURCE_DIR}/Middleware/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1
    ${CMAKE_SOURCE_DIR}/Middleware/Third_Party/SEGGER
)

set(PROJECT_SOURCES
    Src/main.c
    Src/IO_init.c
    Src/stm32f7xx_hal_msp.c
    Src/stm32f7xx_it.c
    Src/syscalls.c
    Src/system_stm32f7xx.c
    Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_spi.c
    Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal.c
    Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cortex.c
    Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma.c
    Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma_ex.c
    Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_eth.c
    Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_exti.c
    Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash.c
    Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash_ex.c
    Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_gpio.c
    Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_i2c.c
    Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_i2c_ex.c
    Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pcd.c
    Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pcd_ex.c
    Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr.c
    Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr_ex.c
    Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc.c
    Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc_ex.c
    Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim.c
    Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim_ex.c
    Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_uart.c
    Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_uart_ex.c
    Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_ll_usb.c
    #added ADC drivers for the guitar analog input:
    Drivers/STM32F7xx_HAL_Driver/Inc/stm32f7xx_hal_adc.c
    Drivers/STM32F7xx_HAL_Driver/Inc/stm32f7xx_hal_adc_ex.c

    Middleware/Third_Party/FreeRTOS/Source/croutine.c
    Middleware/Third_Party/FreeRTOS/Source/event_groups.c
    Middleware/Third_Party/FreeRTOS/Source/list.c
    Middleware/Third_Party/FreeRTOS/Source/queue.c
    Middleware/Third_Party/FreeRTOS/Source/stream_buffer.c
    Middleware/Third_Party/FreeRTOS/Source/tasks.c
    Middleware/Third_Party/FreeRTOS/Source/timers.c
    Middleware/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c
    Middleware/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c
    Middleware/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1/port.c

    Middleware/Third_Party/SEGGER/SEGGER_RTT.c
    Middleware/Third_Party/SEGGER/SEGGER_RTT_printf.c
    Middleware/Third_Party/SEGGER/SEGGER_SYSVIEW.c
    Middleware/Third_Party/SEGGER/SEGGER_SYSVIEW_Config_FreeRTOS.c
    Middleware/Third_Party/SEGGER/SEGGER_SYSVIEW_FreeRTOS.c
    Middleware/Third_Party/SEGGER/SEGGER_RTT_ASM_ARMv7M.S

    startup/startup_stm32f767xx.s
)

add_executable(${PROJECT_NAME}.elf ${PROJECT_SOURCES})

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F767ZI_FLASH.ld)

target_link_options(${PROJECT_NAME}.elf PRIVATE
    ${MCU_FLAGS}
    -T${LINKER_SCRIPT}
    --specs=nosys.specs
    --specs=nano.specs
    -Wl,-Map=${PROJECT_NAME}.map
    -Wl,--gc-sections
    -Wl,--start-group
    -lc
    -lm
    -Wl,--end-group
)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}.elf
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
)
