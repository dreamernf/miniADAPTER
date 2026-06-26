/*
 * defines.h
 *
 *  Created on: Sep 24, 2024
 *      Author: dreamernf
 */

#ifndef DEFINES_H_
#define DEFINES_H_

#include <stdint.h>

typedef enum {
	NO = 0,
	YES,
	OFF,
	ON,
	LED_OFF,
	LED_GREEN,
	LED_YELLOW,
	LEFT,
	RIGHT,
	START,
	END,
} COLOR_LED_STATUS_TypeDef;


typedef struct  CAR_data_t
{
	uint8_t   		led_status;
	uint8_t         left_indicator;
	uint8_t         right_indicator;
	uint8_t         left_turn_signal;
	uint8_t         right_turn_signal;
	uint16_t  		speed;
	uint8_t         command_from_external;
}CAR_DATA_t;


#endif /* DEFINES_H_ */
