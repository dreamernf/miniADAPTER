/*
 * log.c
 *
 *  Created on: Oct 2, 2024
 *      Author: dreamernf
 */


#include "log.h"
#include "main.h"
#include "defines.h"
#include "can_lowlevel_funcs.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

extern UART_HandleTypeDef huart2;


void logger_uart(const char* msg, ...)
{
	    char buff[256];
	    va_list args;
	    va_start(args, msg);
	    vsnprintf(buff, sizeof(buff), msg, args);
	    HAL_UART_Transmit(&huart2, (uint8_t*)buff, strlen((char *)buff), HAL_MAX_DELAY);
	    va_end(args);
}


void Logger_CAN_RX( uint8_t logging, CAN_RX_FRAME_t _RxMsg_)
{
if (logging == YES)
  {
			switch(_RxMsg_.anz_bytes)
				{
					case 1: logger_uart("<-- [%03X] %02X\n\r",  _RxMsg_.can_id,_RxMsg_.data[0]); break;
					case 2: logger_uart("<-- [%03X] %02X %02X\n\r",  _RxMsg_.can_id,_RxMsg_.data[0],_RxMsg_.data[1]); break;
					case 3: logger_uart("<-- [%03X] %02X %02X %02X\n\r",  _RxMsg_.can_id,_RxMsg_.data[0],_RxMsg_.data[1],_RxMsg_.data[2]); break;
					case 4: logger_uart("<-- [%03X] %02X %02X %02X %02X\n\r",  _RxMsg_.can_id,_RxMsg_.data[0],_RxMsg_.data[1],_RxMsg_.data[2],_RxMsg_.data[3]); break;
					case 5: logger_uart("<-- [%03X] %02X %02X %02X %02X %02X\n\r",  _RxMsg_.can_id,_RxMsg_.data[0],_RxMsg_.data[1],_RxMsg_.data[2],_RxMsg_.data[3], _RxMsg_.data[4]); break;
					case 6: logger_uart("<-- [%03X] %02X %02X %02X %02X %02X %02X\n\r",  _RxMsg_.can_id,_RxMsg_.data[0],_RxMsg_.data[1],_RxMsg_.data[2],_RxMsg_.data[3], _RxMsg_.data[4],_RxMsg_.data[5]); break;
					case 7: logger_uart("<-- [%03X] %02X %02X %02X %02X %02X %02X %02X\n\r",  _RxMsg_.can_id,_RxMsg_.data[0],_RxMsg_.data[1],_RxMsg_.data[2],_RxMsg_.data[3], _RxMsg_.data[4],_RxMsg_.data[5],_RxMsg_.data[6]); break;
					case 8: logger_uart("<-- [%03X] %02X %02X %02X %02X %02X %02X %02X %02X\n\r",  _RxMsg_.can_id,_RxMsg_.data[0],_RxMsg_.data[1],_RxMsg_.data[2],_RxMsg_.data[3], _RxMsg_.data[4],_RxMsg_.data[5],_RxMsg_.data[6],_RxMsg_.data[7]); break;
				}
  }
}
