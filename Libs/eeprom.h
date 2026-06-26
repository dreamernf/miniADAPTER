/**
  ******************************************************************************
  * @file           : eeprom.h
  * @brief          : Eeprom emulation driver
  * @author         : MicroTechnics (microtechnics.ru)
  ******************************************************************************
  */

#ifndef EEPROM_H
#define EEPROM_H



/* Includes ------------------------------------------------------------------*/

#include "stm32f0xx_hal.h"



/* Declarations and definitions ----------------------------------------------*/

#define PAGE_DATA_OFFSET                                                8
#define PAGE_DATA_SIZE                                                  8

#define PARAM_1                                                         0x12121212
#define PARAM_2                                                         0x34343434
#define VAR_NUM                                                         2

#define PAGE_0_ADDRESS                                                  0x08007800
#define PAGE_1_ADDRESS                                                  0x08007C00
#define PAGE_SIZE                                                       1024



typedef enum {
  PAGE_CLEARED = 0xFFFFFFFF,
  PAGE_ACTIVE = 0x00000000,
  PAGE_RECEIVING_DATA = 0x55555555,
} PageState;

typedef enum {
  PAGE_0 = 0,
  PAGE_1 = 1,
  PAGES_NUM = 2,
} PageIdx;

typedef enum {
  EEPROM_OK = 0,
  EEPROM_ERROR1 = 1,
  EEPROM_ERROR2 = 2,
  EEPROM_ERROR3 = 3,
  EEPROM_ERROR4 = 4,
  EEPROM_ERROR5 = 5,
  EEPROM_ERROR6 = 6,
} EepromResult;



/* Functions -----------------------------------------------------------------*/

EepromResult EEPROM_Init();
EepromResult EEPROM_Read(uint32_t varId, uint32_t *varValue);
EepromResult EEPROM_Write(uint32_t varId, uint32_t varValue);



#endif // #ifndef EEPROM_H
