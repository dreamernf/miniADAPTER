/*
 * can_lowlevel_funcs.h
 *
 *  Created on: Aug 11, 2023
 *      Author: dreamernf
 */

#ifndef CANFUNCS_CAN_LOWLEVEL_FUNCS_H_
#define CANFUNCS_CAN_LOWLEVEL_FUNCS_H_

#include "stm32f0xx.h"
#include "defines.h"

extern CAN_HandleTypeDef hcan1;

#define  CANBUS_CLUSTER 		&hcan1

typedef struct {
  uint32_t can_id;    // STD=11bit  EXT=29bit
  uint8_t anz_bytes;  // anzahl der daten [0...8]
  uint8_t data[8];    // datenbytes
}CAN_TX_FRAME_t;

typedef enum {
  CAN_RX_EMPTY =0,  // noch nichts empfangen
  CAN_RX_READY      // etwas wurde empfangen
}CAN_STATUS_t;

typedef enum {
  CAN_STD_DATA =0, // standard Daten-Frame
  CAN_STD_REMOTE,  // standard Remote-Frame
  CAN_EXT_DATA,    // extended Daten-Frame
  CAN_EXT_REMOTE   // extended Remote-Frame
}CAN_FRAME_MODE_t;


typedef struct {
  //CAN_FRAME_MODE_t frame_mode;    // Frame-Mode
  uint32_t can_id;    // STD=11bit  EXT=29bit
  uint8_t anz_bytes;  // anzahl der daten [0...8]
  uint8_t data[8];    // datenbytes
}CAN_RX_FRAME_t;


typedef enum {
  CAN1_STD_DATA =0, // standard Daten-Frame
  CAN1_STD_REMOTE,  // standard Remote-Frame
  CAN1_EXT_DATA,    // extended Daten-Frame
  CAN1_EXT_REMOTE,   // extended Remote-Frame
}CAN1_FRAME_MODE_t;


typedef struct {
  CAN_STATUS_t status;         // RX-Status
  CAN_FRAME_MODE_t frame_mode; // Frame-Mode
  uint32_t can_id;              // STD=11bit  EXT=29bit
  uint8_t anz_bytes;            // anzahl der daten [0...8]
  uint8_t data[8];              // datenbytes
}CAN_BUFFER_t;

typedef enum
{
  SUCCESS_OK = 0,
  ERROR_1  = -1,
  ERROR_2 = -2,
  ERROR_3 = -3,
  ERROR_4 = -4,
} CANErrorStatus;



void Copy_Data_to_CAN_CLUSTER_buffer(CAN_RX_FRAME_t  _RX_Frame);
CAN_STATUS_t CAN_Std_Data_receive(CAN_HandleTypeDef * canHandle, CAN_RX_FRAME_t *rx_frame);
CANErrorStatus CAN_Send_Data(CAN_HandleTypeDef * canHandle, CAN_TX_FRAME_t tx_frame);
void Set_Filter_CAN(CAN_HandleTypeDef* canHandle, uint32_t FIFO_Buffer, uint32_t can_id1, uint32_t can_id2, uint32_t can_id3, uint32_t can_id4, uint32_t nmb_filter);
void CAN_Start(CAN_HandleTypeDef* canHandle);
void Send_Command_to_ExtDevice(uint8_t command, uint8_t status);



#endif /* CANFUNCS_CAN_LOWLEVEL_FUNCS_H_ */
