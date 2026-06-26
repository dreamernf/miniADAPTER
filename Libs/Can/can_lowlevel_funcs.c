/*
 * can_lowlevel_funcs.c
 *
 *  Created on: Aug 11, 2023
 *      Author: dreamernf
 */

#include "can_lowlevel_funcs.h"
#include "log.h"
#include <string.h>

CAN_BUFFER_t can_buffer;
CAN_TxHeaderTypeDef _TX_msgHeader;
uint8_t _TX_msgData[8];
CAN_FilterTypeDef canFilterConfig;

extern CAN_HandleTypeDef  hcan1;


void Copy_Data_to_CAN_CLUSTER_buffer(CAN_RX_FRAME_t  _RX_Frame)
{
  uint8_t n=0;
  if(can_buffer.status==CAN_RX_EMPTY)
  {
	can_buffer.can_id=_RX_Frame.can_id;
    can_buffer.anz_bytes=_RX_Frame.anz_bytes;
    can_buffer.frame_mode=CAN_STD_DATA;

      for(n=0;n<8;n++)
      {
        if(n<can_buffer.anz_bytes)
          can_buffer.data[n]=_RX_Frame.data[n];
          else can_buffer.data[n]=0x00;
      }
    }
    can_buffer.status=CAN_RX_READY;
  }


CAN_STATUS_t CAN_Std_Data_receive(CAN_HandleTypeDef * canHandle, CAN_RX_FRAME_t *rx_frame)
{
  CAN_STATUS_t ret_wert=CAN_RX_EMPTY;
  uint8_t n;

  //CAN_CLUSTER
  if (canHandle==CANBUS_CLUSTER)
  {

	  if(can_buffer.status==CAN_RX_READY)
	  {
	    rx_frame->can_id=can_buffer.can_id;
	    rx_frame->anz_bytes=can_buffer.anz_bytes;
	    for(n=0;n<8;n++) {
	      rx_frame->data[n]=can_buffer.data[n];
	    }

	    ret_wert=CAN_RX_READY;
	    can_buffer.status=CAN_RX_EMPTY;
	  }
  }

  return(ret_wert);
}

CANErrorStatus CAN_Send_Data(CAN_HandleTypeDef * canHandle, CAN_TX_FRAME_t tx_frame)
{
  uint8_t n = 0;
  uint32_t mailBoxNum = 0;


  if(tx_frame.can_id>0x7FF) {
    return(ERROR);
  }

  if(tx_frame.anz_bytes>8) {
    return(ERROR);
  }

  _TX_msgHeader.StdId = tx_frame.can_id;
  _TX_msgHeader.ExtId = 0x00;
  _TX_msgHeader.RTR = CAN_RTR_DATA;
  _TX_msgHeader.IDE = CAN_ID_STD;
  _TX_msgHeader.DLC = tx_frame.anz_bytes;

  for(n=0;n<tx_frame.anz_bytes;n++) {
	  _TX_msgData[n] = tx_frame.data[n];
  }

  if (HAL_CAN_GetTxMailboxesFreeLevel(canHandle) != 0)
   {
	  if (HAL_CAN_AddTxMessage(canHandle, &_TX_msgHeader, _TX_msgData, &mailBoxNum)==HAL_OK)
		  return(SUCCESS);
	  else return(ERROR);
   }
  else  return(ERROR);

}


void Set_Filter_CAN(CAN_HandleTypeDef* canHandle, uint32_t FIFO_Buffer, uint32_t can_id1, uint32_t can_id2, uint32_t can_id3, uint32_t can_id4, uint32_t nmb_filter)
{

  canFilterConfig.FilterBank = nmb_filter;
  canFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;
  canFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;
  canFilterConfig.FilterIdHigh = can_id2<<5;    // ID2
  canFilterConfig.FilterIdLow = can_id1<<5;     // ID1
  canFilterConfig.FilterMaskIdHigh = can_id4<<5;// ID4
  canFilterConfig.FilterMaskIdLow =can_id3<<5;  // ID3

  canFilterConfig.FilterFIFOAssignment = FIFO_Buffer;
  canFilterConfig.FilterActivation = ENABLE;
  canFilterConfig.SlaveStartFilterBank = 14;

  if(HAL_CAN_ConfigFilter(canHandle, &canFilterConfig) != HAL_OK)
  {
	  for(;;);
  }


  if (HAL_CAN_ActivateNotification(canHandle, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
	 for(;;);
  }

}

void  CAN_Start(CAN_HandleTypeDef* canHandle)
{
	  if (HAL_CAN_Start(canHandle) != HAL_OK)
	  {
		  for(;;);
	  }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef * hcan)
{
CAN_RxHeaderTypeDef _RX_msgHeader;
uint8_t _RX_msgData[8]={0,};
CAN_RX_FRAME_t  RX_Frame;

if (hcan==CANBUS_CLUSTER)
	{
		  if (HAL_CAN_GetRxMessage(CANBUS_CLUSTER, CAN_RX_FIFO0,  &_RX_msgHeader, _RX_msgData)==HAL_OK)
		  {
			  //сохранить данные в буфер
			  RX_Frame.can_id = _RX_msgHeader.StdId;
			  RX_Frame.anz_bytes = _RX_msgHeader.DLC;
			  RX_Frame.data[0] = _RX_msgData[0];
			  RX_Frame.data[1] = _RX_msgData[1];
			  RX_Frame.data[2] = _RX_msgData[2];
			  RX_Frame.data[3] = _RX_msgData[3];
			  RX_Frame.data[4] = _RX_msgData[4];
			  RX_Frame.data[5] = _RX_msgData[5];
			  RX_Frame.data[6] = _RX_msgData[6];
			  RX_Frame.data[7] = _RX_msgData[7];

			  Copy_Data_to_CAN_CLUSTER_buffer(RX_Frame);

			  if (RX_Frame.can_id==0x3BA)  Logger_CAN_RX(YES, RX_Frame);
		  }
	}
}

void Send_Command_to_ExtDevice(uint8_t command, uint8_t status)
{
	CAN_TX_FRAME_t TxMsg;
	TxMsg.can_id=0x67C;
	TxMsg.anz_bytes = 3;
	TxMsg.data[0]=0xEE;
	TxMsg.data[1]=command;
	TxMsg.data[2]=status;
	//посылаем запрос
	CAN_Send_Data(CANBUS_CLUSTER , TxMsg);
	//Logger_CAN_TX(HAL_GetTick(), LOGGER_CAN_TX, TxMsg);
}





