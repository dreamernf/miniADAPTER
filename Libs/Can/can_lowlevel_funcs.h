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

#define  CANBUS_COMFORT 		&hcan1

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
  CAN_SEND_ERROR = 0,
  CAN_SEND_OK = 1
} CANErrorStatus;

typedef struct {
  uint32_t tx_queued;
  uint32_t tx_complete;
  uint32_t tx_failed;
  uint32_t rx_received;
  uint32_t rx_dropped;
  uint32_t error_irq;
  uint32_t last_hal_error;
} CAN_DIAGNOSTICS_t;



void Copy_Data_to_CAN_COMFORT_buffer(CAN_RX_FRAME_t  _RX_Frame);
CAN_STATUS_t CAN_Std_Data_receive(CAN_HandleTypeDef * canHandle, CAN_RX_FRAME_t *rx_frame);
CANErrorStatus CAN_Send_Data(CAN_HandleTypeDef * canHandle, CAN_TX_FRAME_t tx_frame);
HAL_StatusTypeDef Set_Filter_CAN(CAN_HandleTypeDef* canHandle, uint32_t FIFO_Buffer, uint32_t can_id1, uint32_t can_id2, uint32_t can_id3, uint32_t can_id4, uint32_t nmb_filter);
HAL_StatusTypeDef CAN_Start(CAN_HandleTypeDef* canHandle);

/* Diagnostic helpers. All UART output is produced from the main context. */
HAL_StatusTypeDef CAN_Diagnostic_LoopbackSelfTest(CAN_HandleTypeDef *canHandle);
HAL_StatusTypeDef CAN_Diagnostic_Enable(CAN_HandleTypeDef *canHandle);
void CAN_Diagnostic_Task(CAN_HandleTypeDef *canHandle);
void CAN_Diagnostic_GetSnapshot(CAN_DIAGNOSTICS_t *snapshot);


#endif /* CANFUNCS_CAN_LOWLEVEL_FUNCS_H_ */
