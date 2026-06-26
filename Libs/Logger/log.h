//
// Created by naoki on 19/03/09.
//

#ifndef HAL_DMA_PRINTF_LOG_H
#define HAL_DMA_PRINTF_LOG_H

#include "defines.h"
#include "can_lowlevel_funcs.h"

void  Init_Logger(void);


void logger_uart(const char* msg, ...);
void logger_uart_no_dma(const char* msg, ...);
void logger_message(uint8_t logging, const char* msg, ...);
void Logger_CAN_RX(uint8_t logging, CAN_RX_FRAME_t _RxMsg_);



#endif //HAL_DMA_PRINTF_LOG_H
