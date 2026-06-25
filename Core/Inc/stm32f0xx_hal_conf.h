#ifndef __STM32F0xx_HAL_CONF_H
#define __STM32F0xx_HAL_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#define HAL_MODULE_ENABLED
#define HAL_CAN_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED

#define HSE_VALUE    8000000U
#define HSI_VALUE    8000000U
#define HSI14_VALUE  14000000U
#define HSI48_VALUE  48000000U
#define LSI_VALUE    40000U
#define LSE_VALUE    32768U
#define VDD_VALUE    3300U
#define TICK_INT_PRIORITY 0U
#define USE_RTOS 0U
#define PREFETCH_ENABLE 1U

#include "stm32f0xx_hal_rcc.h"
#include "stm32f0xx_hal_gpio.h"
#include "stm32f0xx_hal_cortex.h"
#include "stm32f0xx_hal_can.h"
#include "stm32f0xx_hal_uart.h"

#ifdef __cplusplus
}
#endif

#endif /* __STM32F0xx_HAL_CONF_H */
