/*
 * can_lowlevel_funcs.c
 *
 *  Created on: Aug 11, 2023
 *      Author: dreamernf
 */

#include "can_lowlevel_funcs.h"
#include "log.h"
#include <string.h>

static volatile CAN_BUFFER_t can_buffer;
CAN_TxHeaderTypeDef _TX_msgHeader;
uint8_t _TX_msgData[8];
CAN_FilterTypeDef canFilterConfig;

static volatile CAN_DIAGNOSTICS_t can_diag;
static uint32_t diag_last_tx_ms;
static uint32_t diag_last_report_ms;
static uint32_t diag_reported_tx_complete;
static uint32_t diag_reported_tx_failed;
static uint32_t diag_reported_error_irq;
static uint8_t diag_sequence;

extern CAN_HandleTypeDef  hcan1;


void Copy_Data_to_CAN_COMFORT_buffer(CAN_RX_FRAME_t  _RX_Frame)
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
    can_buffer.status=CAN_RX_READY;
    can_diag.rx_received++;
    }
  else
  {
    can_diag.rx_dropped++;
  }
}


CAN_STATUS_t CAN_Std_Data_receive(CAN_HandleTypeDef * canHandle, CAN_RX_FRAME_t *rx_frame)
{
  CAN_STATUS_t ret_wert=CAN_RX_EMPTY;
  uint8_t n;

  //CANBUS_COMFORT
  if (canHandle==CANBUS_COMFORT)
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
    can_diag.tx_failed++;
    return(CAN_SEND_ERROR);
  }

  if(tx_frame.anz_bytes>8) {
    can_diag.tx_failed++;
    return(CAN_SEND_ERROR);
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
	  {
		  can_diag.tx_queued++;
		  return(CAN_SEND_OK);
	  }
	  can_diag.tx_failed++;
	  return(CAN_SEND_ERROR);
   }
  can_diag.tx_failed++;
  return(CAN_SEND_ERROR);

}


HAL_StatusTypeDef Set_Filter_CAN(CAN_HandleTypeDef* canHandle, uint32_t FIFO_Buffer, uint32_t can_id1, uint32_t can_id2, uint32_t can_id3, uint32_t can_id4, uint32_t nmb_filter)
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
	  return HAL_ERROR;
  }


  if (HAL_CAN_ActivateNotification(canHandle, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
	 return HAL_ERROR;
  }

  return HAL_OK;

}

HAL_StatusTypeDef CAN_Start(CAN_HandleTypeDef* canHandle)
{
	  return HAL_CAN_Start(canHandle);
}

HAL_StatusTypeDef CAN_Diagnostic_LoopbackSelfTest(CAN_HandleTypeDef *canHandle)
{
  CAN_FilterTypeDef filter = {0};
  CAN_TxHeaderTypeDef tx_header = {0};
  CAN_RxHeaderTypeDef rx_header = {0};
  uint8_t tx_data[2] = {0xA5, 0x5A};
  uint8_t rx_data[8] = {0};
  uint32_t mailbox;
  uint32_t started_at;
  HAL_StatusTypeDef result = HAL_ERROR;

  canHandle->Init.Mode = CAN_MODE_LOOPBACK;
  if (HAL_CAN_Init(canHandle) != HAL_OK)
  {
    goto restore_normal_mode;
  }

  filter.FilterBank = 0;
  filter.FilterMode = CAN_FILTERMODE_IDMASK;
  filter.FilterScale = CAN_FILTERSCALE_32BIT;
  filter.FilterIdHigh = (0x6FEU << 5);
  filter.FilterIdLow = 0;
  filter.FilterMaskIdHigh = (0x7FFU << 5);
  filter.FilterMaskIdLow = 0;
  filter.FilterFIFOAssignment = CAN_RX_FIFO0;
  filter.FilterActivation = ENABLE;
  filter.SlaveStartFilterBank = 14;

  if ((HAL_CAN_ConfigFilter(canHandle, &filter) != HAL_OK) ||
      (HAL_CAN_Start(canHandle) != HAL_OK))
  {
    goto stop_and_restore;
  }

  tx_header.StdId = 0x6FE;
  tx_header.IDE = CAN_ID_STD;
  tx_header.RTR = CAN_RTR_DATA;
  tx_header.DLC = 2;
  if (HAL_CAN_AddTxMessage(canHandle, &tx_header, tx_data, &mailbox) != HAL_OK)
  {
    goto stop_and_restore;
  }

  started_at = HAL_GetTick();
  while ((HAL_GetTick() - started_at) < 50U)
  {
    if (HAL_CAN_GetRxFifoFillLevel(canHandle, CAN_RX_FIFO0) != 0U)
    {
      if ((HAL_CAN_GetRxMessage(canHandle, CAN_RX_FIFO0, &rx_header, rx_data) == HAL_OK) &&
          (rx_header.StdId == 0x6FEU) && (rx_header.DLC == 2U) &&
          (rx_data[0] == 0xA5U) && (rx_data[1] == 0x5AU))
      {
        result = HAL_OK;
      }
      break;
    }
  }

stop_and_restore:
  (void)HAL_CAN_Stop(canHandle);
restore_normal_mode:
  canHandle->Init.Mode = CAN_MODE_NORMAL;
  if (HAL_CAN_Init(canHandle) != HAL_OK)
  {
    return HAL_ERROR;
  }
  return result;
}

HAL_StatusTypeDef CAN_Diagnostic_Enable(CAN_HandleTypeDef *canHandle)
{
  uint32_t notifications = CAN_IT_TX_MAILBOX_EMPTY |
                           CAN_IT_RX_FIFO0_OVERRUN |
                           CAN_IT_ERROR_WARNING |
                           CAN_IT_ERROR_PASSIVE |
                           CAN_IT_BUSOFF |
                           CAN_IT_LAST_ERROR_CODE |
                           CAN_IT_ERROR;

  return HAL_CAN_ActivateNotification(canHandle, notifications);
}

void CAN_Diagnostic_GetSnapshot(CAN_DIAGNOSTICS_t *snapshot)
{
  if (snapshot == NULL)
  {
    return;
  }

  __disable_irq();
  *snapshot = can_diag;
  __enable_irq();
}

void CAN_Diagnostic_Task(CAN_HandleTypeDef *canHandle)
{
  uint32_t now = HAL_GetTick();
  CAN_DIAGNOSTICS_t snapshot;

  if ((now - diag_last_tx_ms) >= 1000U)
  {
    CAN_TX_FRAME_t frame = {0};
    frame.can_id = 0x6FE;
    frame.anz_bytes = 8;
    frame.data[0] = 0xCA;
    frame.data[1] = 0x4E;
    frame.data[2] = diag_sequence++;
    frame.data[3] = (uint8_t)(now >> 24);
    frame.data[4] = (uint8_t)(now >> 16);
    frame.data[5] = (uint8_t)(now >> 8);
    frame.data[6] = (uint8_t)now;
    frame.data[7] = 0xD1;
    diag_last_tx_ms = now;

    if (CAN_Send_Data(canHandle, frame) == CAN_SEND_ERROR)
    {
      logger_uart_no_dma("[CAN][TX] QUEUE FAIL state=%u free=%lu err=0x%08lX\r\n",
                         (unsigned int)HAL_CAN_GetState(canHandle),
                         (unsigned long)HAL_CAN_GetTxMailboxesFreeLevel(canHandle),
                         (unsigned long)HAL_CAN_GetError(canHandle));
    }
    else
    {
      logger_uart_no_dma("[CAN][TX] queued id=6FE seq=%u\r\n", (unsigned int)frame.data[2]);
    }
  }

  CAN_Diagnostic_GetSnapshot(&snapshot);
  if (snapshot.tx_complete != diag_reported_tx_complete)
  {
    diag_reported_tx_complete = snapshot.tx_complete;
    logger_uart_no_dma("[CAN][TX] ACK OK total=%lu\r\n", (unsigned long)snapshot.tx_complete);
  }
  if (snapshot.tx_failed != diag_reported_tx_failed)
  {
    diag_reported_tx_failed = snapshot.tx_failed;
    logger_uart_no_dma("[CAN][TX] FAILED total=%lu ESR=0x%08lX\r\n",
                       (unsigned long)snapshot.tx_failed,
                       (unsigned long)canHandle->Instance->ESR);
  }
  if (snapshot.error_irq != diag_reported_error_irq)
  {
    diag_reported_error_irq = snapshot.error_irq;
    logger_uart_no_dma("[CAN][ERR] irq=%lu hal=0x%08lX ESR=0x%08lX (TEC=%lu REC=%lu)\r\n",
                       (unsigned long)snapshot.error_irq,
                       (unsigned long)snapshot.last_hal_error,
                       (unsigned long)canHandle->Instance->ESR,
                       (unsigned long)((canHandle->Instance->ESR >> 16) & 0xFFU),
                       (unsigned long)((canHandle->Instance->ESR >> 24) & 0xFFU));
    if ((snapshot.last_hal_error & HAL_CAN_ERROR_ACK) != 0U)
    {
      logger_uart_no_dma("[CAN][HINT] ACK error: MCU tried to transmit, but no other CAN node acknowledged it\r\n");
    }
    if ((snapshot.last_hal_error & HAL_CAN_ERROR_BOF) != 0U)
    {
      logger_uart_no_dma("[CAN][HINT] BUS-OFF: check CANH/CANL, transceiver power/enable and bitrate\r\n");
    }
    if ((snapshot.last_hal_error & HAL_CAN_ERROR_EPV) != 0U)
    {
      logger_uart_no_dma("[CAN][HINT] ERROR-PASSIVE: too many bus errors\r\n");
    }
    (void)HAL_CAN_ResetError(canHandle);
  }

  if ((now - diag_last_report_ms) >= 5000U)
  {
    diag_last_report_ms = now;
    logger_uart_no_dma("[CAN][STAT] state=%u queued=%lu ack=%lu fail=%lu rx=%lu drop=%lu ESR=0x%08lX\r\n",
                       (unsigned int)HAL_CAN_GetState(canHandle),
                       (unsigned long)snapshot.tx_queued,
                       (unsigned long)snapshot.tx_complete,
                       (unsigned long)snapshot.tx_failed,
                       (unsigned long)snapshot.rx_received,
                       (unsigned long)snapshot.rx_dropped,
                       (unsigned long)canHandle->Instance->ESR);
  }
}

static void CAN_Diagnostic_TxComplete(CAN_HandleTypeDef *hcan)
{
  if (hcan == CANBUS_COMFORT)
  {
    can_diag.tx_complete++;
  }
}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan) { CAN_Diagnostic_TxComplete(hcan); }
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan) { CAN_Diagnostic_TxComplete(hcan); }
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan) { CAN_Diagnostic_TxComplete(hcan); }

static void CAN_Diagnostic_TxAbort(CAN_HandleTypeDef *hcan)
{
  if (hcan == CANBUS_COMFORT)
  {
    can_diag.tx_failed++;
  }
}

void HAL_CAN_TxMailbox0AbortCallback(CAN_HandleTypeDef *hcan) { CAN_Diagnostic_TxAbort(hcan); }
void HAL_CAN_TxMailbox1AbortCallback(CAN_HandleTypeDef *hcan) { CAN_Diagnostic_TxAbort(hcan); }
void HAL_CAN_TxMailbox2AbortCallback(CAN_HandleTypeDef *hcan) { CAN_Diagnostic_TxAbort(hcan); }

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
  if (hcan == CANBUS_COMFORT)
  {
    uint32_t error = HAL_CAN_GetError(hcan);
    can_diag.error_irq++;
    can_diag.last_hal_error = error;
    if ((error & (HAL_CAN_ERROR_TX_TERR0 | HAL_CAN_ERROR_TX_TERR1 | HAL_CAN_ERROR_TX_TERR2)) != 0U)
    {
      can_diag.tx_failed++;
    }
  }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef * hcan)
{
CAN_RxHeaderTypeDef _RX_msgHeader;
uint8_t _RX_msgData[8]={0,};
CAN_RX_FRAME_t  RX_Frame;

if (hcan==CANBUS_COMFORT)
	{
		  if (HAL_CAN_GetRxMessage(CANBUS_COMFORT, CAN_RX_FIFO0,  &_RX_msgHeader, _RX_msgData)==HAL_OK)
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

			  Copy_Data_to_CAN_COMFORT_buffer(RX_Frame);
		  }
	}
}






