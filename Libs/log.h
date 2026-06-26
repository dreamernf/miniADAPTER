/*
 * log.h
 *
 *  Created on: Oct 2, 2024
 *      Author: dreamernf
 */

#ifndef LOG_H_
#define LOG_H_

#include "can_lowlevel_funcs.h"

void logger_uart(const char* msg, ...);
void Logger_CAN_RX(uint8_t logging, CAN_RX_FRAME_t _RxMsg_);

#endif /* LOG_H_ */
