/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "can_lowlevel_funcs.h"
#include "defines.h"
#include "eeprom.h"
#include "log.h"
#include "buzzer.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
  COMMAND_EMPTY = 0x00U,
  COMMAND_MINICLUSTER_TURN_LIGHTS = 0x08U,
  COMMAND_MINICLUSTER_BUZZER = 0x09U,
  COMMAND_MINICLUSTER_TESTS = 0x10U,
} TypesCommandsFiltersTypeDef;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define   LED_STATUS_SA_ACTIVE_ON   HAL_GPIO_WritePin(LED_STATUS_SA_ACTIVE_GPIO_Port, LED_STATUS_SA_ACTIVE_Pin, GPIO_PIN_SET)
#define   LED_STATUS_SA_ACTIVE_OFF  HAL_GPIO_WritePin(LED_STATUS_SA_ACTIVE_GPIO_Port, LED_STATUS_SA_ACTIVE_Pin, GPIO_PIN_RESET)

#define   LED_STATUS_SA_PASSIVE_ON   HAL_GPIO_WritePin(LED_STATUS_SA_PASSIVE_GPIO_Port, LED_STATUS_SA_PASSIVE_Pin, GPIO_PIN_SET)
#define   LED_STATUS_SA_PASSIVE_OFF  HAL_GPIO_WritePin(LED_STATUS_SA_PASSIVE_GPIO_Port, LED_STATUS_SA_PASSIVE_Pin, GPIO_PIN_RESET)

#define   LED_LEFT_SA_INDICATOR_ON   HAL_GPIO_WritePin(LED_LEFT_SA_INDICATOR_GPIO_Port, LED_LEFT_SA_INDICATOR_Pin, GPIO_PIN_SET)
#define   LED_LEFT_SA_INDICATOR_OFF  HAL_GPIO_WritePin(LED_LEFT_SA_INDICATOR_GPIO_Port, LED_LEFT_SA_INDICATOR_Pin, GPIO_PIN_RESET)

#define   LED_RIGHT_SA_INDICATOR_ON   HAL_GPIO_WritePin(LED_RIGHT_SA_INDICATOR_GPIO_Port, LED_RIGHT_SA_INDICATOR_Pin, GPIO_PIN_SET)
#define   LED_RIGHT_SA_INDICATOR_OFF  HAL_GPIO_WritePin(LED_RIGHT_SA_INDICATOR_GPIO_Port, LED_RIGHT_SA_INDICATOR_Pin, GPIO_PIN_RESET)

#define   VALUE_PARAM_INDICATION_WITH_TURN_LIGHT    ParamData[0]
#define   VALUE_PARAM_BUZZER    					ParamData[1]

#define   PARAM_INDICATION_WITH_TURN_LIGHT  0x12121212
#define   PARAM_BUZZER  					0x34343434

#define   BYTE0_STATUS_LIGHTS  		rx_frame.data[0]
#define   BYTE1_STATUS_LIGHTS  		rx_frame.data[1]
#define   HIGH_BEAM_STATUS      	0x04
#define   FOG_LIGHTS_STATUS    		0x08

#define   LEFT_TURN_SIGNAL_ON       0x11
#define   RIGHT_TURN_SIGNAL_ON      0x12
#define   ALARM_TURN_SIGNAL_ON      0x1B

#define   CNTS_PRESS_BTN_OK         5
#define   BTN_OK					0x28



/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
CAR_DATA_t  car_data;
CAN_RX_FRAME_t rx_frame = {0,};
CAN_TX_FRAME_t tx_frame = {0,};
uint32_t time  = 0;
uint32_t ParamData[2] = {0, 0};
uint32_t btn_OK = 0;
uint8_t  flag_rattling = OFF;
uint8_t  flag_saveparams = NO;
uint8_t  flag_buzzer = OFF;
uint8_t  flag_recieve_command = NO;
EepromResult  status = EEPROM_ERROR1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
int cb(const int value, const int position);
int cb_mask(const int value, const int mask);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN_Init();
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  //BUZZER_Init();
  logger_uart("START INIT\n\r");

  car_data.command_from_external = COMMAND_EMPTY;

  EEPROM_Init();

  Set_Filter_CAN(CANBUS_CLUSTER ,CAN_RX_FIFO0, 0x531, 0x3BA, 0x5C1, 0x320, 0);
  Set_Filter_CAN(CANBUS_CLUSTER ,CAN_RX_FIFO0, 0x67C, 0x000, 0x000, 0x000, 1);
  CAN_Start(CANBUS_CLUSTER);
  HAL_TIM_Base_Start_IT(&htim1);

  logger_uart("INIT OK\n\r");

  status = EEPROM_Read(PARAM_INDICATION_WITH_TURN_LIGHT, &VALUE_PARAM_INDICATION_WITH_TURN_LIGHT);
  if (status ==EEPROM_OK)
	  {
		  if (VALUE_PARAM_INDICATION_WITH_TURN_LIGHT!=ON && VALUE_PARAM_INDICATION_WITH_TURN_LIGHT!=OFF)
			{
			  VALUE_PARAM_INDICATION_WITH_TURN_LIGHT=OFF;
			  logger_uart("ERROR READ FLASH (%d)!!! Set default PARAM_INDICATION_WITH_TURN_LIGHT: OFF\n\r",VALUE_PARAM_INDICATION_WITH_TURN_LIGHT);
			} else logger_uart("PARAM_INDICATION_WITH_TURN_LIGHT: %s [%d]\n\r", VALUE_PARAM_INDICATION_WITH_TURN_LIGHT==OFF ? "OFF" : "ON", VALUE_PARAM_INDICATION_WITH_TURN_LIGHT);
	  }
  else if (status!=EEPROM_OK)
  {
	  VALUE_PARAM_INDICATION_WITH_TURN_LIGHT=OFF;
	  status = EEPROM_Write(PARAM_INDICATION_WITH_TURN_LIGHT, OFF);
	  logger_uart("ERROR READ FLASH!!! Set default PARAM_INDICATION_WITH_TURN_LIGHT: OFF [%d]\n\r", status);
  }

  status = EEPROM_Read(PARAM_BUZZER, &VALUE_PARAM_BUZZER);
  if (status ==EEPROM_OK)
	  {
		  if (VALUE_PARAM_BUZZER!=ON && VALUE_PARAM_BUZZER!=OFF)
			{
			  VALUE_PARAM_BUZZER=ON;
			  logger_uart("ERROR READ FLASH (%d)!!! Set default VALUE_PARAM_BUZZER: OFF\n\r",VALUE_PARAM_BUZZER);
			} else logger_uart("PARAM_BUZZER: %s [%d]\n\r", VALUE_PARAM_BUZZER==OFF ? "OFF" : "ON", VALUE_PARAM_BUZZER);
	  }
  else if (status!=EEPROM_OK)
  {
	  VALUE_PARAM_BUZZER=OFF;
	  status = EEPROM_Write(PARAM_BUZZER, OFF);
	  logger_uart("ERROR READ FLASH!!! Set default PARAM_BUZZER: OFF [%d]\n\r", status);
  }


  LED_LEFT_SA_INDICATOR_ON;
  LED_RIGHT_SA_INDICATOR_ON;
  HAL_Delay(500);
  LED_LEFT_SA_INDICATOR_OFF;
  LED_RIGHT_SA_INDICATOR_OFF;
  HAL_Delay(500);
  LED_STATUS_SA_ACTIVE_ON;
  LED_STATUS_SA_PASSIVE_OFF;
  HAL_Delay(500);
  LED_STATUS_SA_ACTIVE_OFF;
  LED_STATUS_SA_PASSIVE_ON;
  HAL_Delay(1000);
  LED_STATUS_SA_ACTIVE_OFF;
  LED_STATUS_SA_PASSIVE_OFF;

  //BUZZER_Go(TBUZ_20, TICK_4);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	CAN_Std_Data_receive(CANBUS_CLUSTER, &rx_frame);

	if (rx_frame.can_id==0x67C && rx_frame.anz_bytes == 2 && rx_frame.data[0]==0xEE && flag_recieve_command == NO)
	{
		flag_recieve_command = YES;
		logger_uart("Recieved external command = %02X\n\r",rx_frame.data[1]);
		car_data.command_from_external = rx_frame.data[1];
	}

	//"(e*256+d)/192"
	if (rx_frame.can_id==0x320)
		{
		   flag_recieve_command = NO;
		   car_data.speed = (rx_frame.data[3]<<8 | rx_frame.data[4])/192;
		}

	if (rx_frame.can_id==0x5C1)
	{
		if (rx_frame.data[0]==BTN_OK && flag_rattling == ON)
		{
			flag_rattling = OFF;
			btn_OK++;
			if (btn_OK>CNTS_PRESS_BTN_OK) {btn_OK = 0;};
			logger_uart("btn_OK = %d\n\r",btn_OK);
		}
		else if (rx_frame.data[0]==0x00 && flag_rattling == OFF) flag_rattling = ON;
		else if (rx_frame.data[0]!=BTN_OK && rx_frame.data[0]!=0x00)
		{
			flag_rattling = OFF;
			btn_OK = 0;
		}
	}

	if (rx_frame.can_id==0x531)
	{
		if (cb_mask(BYTE1_STATUS_LIGHTS,ALARM_TURN_SIGNAL_ON))
		{
			car_data.left_turn_signal = ON;
			car_data.right_turn_signal = ON;
		}
		else
		{
			if (cb_mask(BYTE1_STATUS_LIGHTS,LEFT_TURN_SIGNAL_ON)) car_data.left_turn_signal = ON; else  if (!cb_mask(BYTE1_STATUS_LIGHTS,LEFT_TURN_SIGNAL_ON)) car_data.left_turn_signal = OFF;
			if (cb_mask(BYTE1_STATUS_LIGHTS,RIGHT_TURN_SIGNAL_ON)) car_data.right_turn_signal = ON; else  if (!cb_mask(BYTE1_STATUS_LIGHTS,LEFT_TURN_SIGNAL_ON)) car_data.right_turn_signal = OFF;
		}


		if (((cb(BYTE0_STATUS_LIGHTS,HIGH_BEAM_STATUS)) && btn_OK == CNTS_PRESS_BTN_OK) || car_data.command_from_external == COMMAND_MINICLUSTER_TURN_LIGHTS)
		{
			 car_data.command_from_external = COMMAND_EMPTY;
			 flag_saveparams = YES;
			 btn_OK = 0;
			 status = EEPROM_Read(PARAM_INDICATION_WITH_TURN_LIGHT, &VALUE_PARAM_INDICATION_WITH_TURN_LIGHT);
			 if (status == EEPROM_OK && VALUE_PARAM_INDICATION_WITH_TURN_LIGHT==ON)
				 {
				    status = EEPROM_Write(PARAM_INDICATION_WITH_TURN_LIGHT, OFF);
				    if (status == EEPROM_OK) VALUE_PARAM_INDICATION_WITH_TURN_LIGHT = OFF;
				 }
			 else if (status == EEPROM_OK && VALUE_PARAM_INDICATION_WITH_TURN_LIGHT==OFF)
				 {
					status = EEPROM_Write(PARAM_INDICATION_WITH_TURN_LIGHT, ON);
					if (status == EEPROM_OK) VALUE_PARAM_INDICATION_WITH_TURN_LIGHT = ON;
				 }
			 if (status == EEPROM_OK)
			 {
				   LED_STATUS_SA_PASSIVE_OFF;
				   HAL_Delay(100);
				   LED_STATUS_SA_PASSIVE_ON;
				   HAL_Delay(100);
				   LED_STATUS_SA_PASSIVE_OFF;
				   HAL_Delay(100);
				   LED_STATUS_SA_PASSIVE_ON;
				   HAL_Delay(100);
				   LED_STATUS_SA_PASSIVE_OFF;
				   HAL_Delay(100);
				   LED_STATUS_SA_PASSIVE_ON;
				   HAL_Delay(100);
				   LED_STATUS_SA_PASSIVE_OFF;
				   logger_uart("[%d] SWITCH PARAM_INDICATION_WITH_TURN_LIGHT: %s\n\r", status, VALUE_PARAM_INDICATION_WITH_TURN_LIGHT==OFF ? "OFF" : "ON");
				   Send_Command_to_ExtDevice(COMMAND_MINICLUSTER_TURN_LIGHTS, VALUE_PARAM_INDICATION_WITH_TURN_LIGHT);
				   BUZZER_Go(TBUZ_200, TICK_4);
			 }
			 flag_saveparams = NO;
		}
		if (((cb(BYTE0_STATUS_LIGHTS,FOG_LIGHTS_STATUS)) && btn_OK == CNTS_PRESS_BTN_OK) || car_data.command_from_external == COMMAND_MINICLUSTER_BUZZER)
			{
				 flag_saveparams = YES;
				 car_data.command_from_external = COMMAND_EMPTY;
				 btn_OK = 0;
				 status = EEPROM_Read(PARAM_BUZZER, &VALUE_PARAM_BUZZER);
				 if (status == EEPROM_OK && VALUE_PARAM_BUZZER==ON)
					 {
					    status = EEPROM_Write(PARAM_BUZZER, OFF);
					    if (status == EEPROM_OK) VALUE_PARAM_BUZZER = OFF;
					 }
				 else if (status == EEPROM_OK && VALUE_PARAM_BUZZER==OFF)
					 {
						status = EEPROM_Write(PARAM_BUZZER, ON);
						if (status == EEPROM_OK) VALUE_PARAM_BUZZER = ON;
					 }
				 if (status == EEPROM_OK)
				 {
					   LED_STATUS_SA_PASSIVE_OFF;
					   HAL_Delay(100);
					   LED_STATUS_SA_PASSIVE_ON;
					   HAL_Delay(100);
					   LED_STATUS_SA_PASSIVE_OFF;
					   HAL_Delay(100);
					   LED_STATUS_SA_PASSIVE_ON;
					   HAL_Delay(100);
					   LED_STATUS_SA_PASSIVE_OFF;
					   HAL_Delay(100);
					   LED_STATUS_SA_PASSIVE_ON;
					   HAL_Delay(100);
					   LED_STATUS_SA_PASSIVE_OFF;
					   logger_uart("[%d] SWITCH PARAM_BUZZER: %s\n\r", status, VALUE_PARAM_BUZZER==OFF ? "OFF" : "ON");
					   Send_Command_to_ExtDevice(COMMAND_MINICLUSTER_BUZZER, VALUE_PARAM_BUZZER);
					   BUZZER_Go(TBUZ_200, TICK_4);
				 }
				 flag_saveparams = NO;
			}
	}

	if (car_data.command_from_external == COMMAND_MINICLUSTER_TESTS)
		{
		  car_data.command_from_external = COMMAND_EMPTY;
		  Send_Command_to_ExtDevice(COMMAND_MINICLUSTER_TESTS, START);
		  LED_LEFT_SA_INDICATOR_ON;
		  LED_RIGHT_SA_INDICATOR_ON;
		  HAL_Delay(1000);
		  LED_LEFT_SA_INDICATOR_OFF;
		  LED_RIGHT_SA_INDICATOR_OFF;
		  HAL_Delay(1000);

		  LED_STATUS_SA_ACTIVE_ON;
		  LED_STATUS_SA_PASSIVE_OFF;
		  HAL_Delay(1000);
		  LED_STATUS_SA_ACTIVE_OFF;
		  LED_STATUS_SA_PASSIVE_ON;
		  HAL_Delay(1000);
		  LED_STATUS_SA_ACTIVE_OFF;
		  LED_STATUS_SA_PASSIVE_OFF;

		  BUZZER_Go(TBUZ_50, TICK_12);
		  Send_Command_to_ExtDevice(COMMAND_MINICLUSTER_TESTS, END);
		}

	if (rx_frame.can_id==0x3BA)
	{
		if (rx_frame.data[2] == 0x08)
		{
			car_data.led_status = LED_YELLOW;
			car_data.left_indicator = OFF;
			car_data.right_indicator = OFF;
		}
		else if (rx_frame.data[2] == 0x10)
			   {
			        car_data.led_status = LED_GREEN;
			        if (VALUE_PARAM_INDICATION_WITH_TURN_LIGHT==ON)
			        {
				        if (rx_frame.data[3] == 0x04) car_data.left_indicator = ON;  else if (rx_frame.data[3] == 0x00) car_data.left_indicator = OFF;
				        if (rx_frame.data[5] == 0x04) car_data.right_indicator = ON; else if (rx_frame.data[5] == 0x00) car_data.right_indicator = OFF;
			        }
			   }
		else
				{
					car_data.led_status = LED_OFF;
					car_data.left_indicator = OFF;
					car_data.right_indicator = OFF;
				}
	}


	if (car_data.led_status == LED_GREEN && flag_saveparams == NO)
		{
		   LED_STATUS_SA_ACTIVE_ON;
		   LED_STATUS_SA_PASSIVE_OFF;
		}
	else if (car_data.led_status == LED_YELLOW && flag_saveparams == NO)
		{
		   LED_STATUS_SA_ACTIVE_OFF;
		   LED_STATUS_SA_PASSIVE_ON;
		}
	else if (car_data.led_status == LED_OFF && flag_saveparams == NO)
		{
		   LED_STATUS_SA_ACTIVE_OFF;
		   LED_STATUS_SA_PASSIVE_OFF;
		}

	if (car_data.left_indicator == ON && car_data.left_turn_signal==OFF)
		{
		  LED_LEFT_SA_INDICATOR_ON;
		  if (VALUE_PARAM_BUZZER==ON) flag_buzzer=ON; else  if (VALUE_PARAM_BUZZER==OFF) flag_buzzer=OFF;
		}
	else if (car_data.left_indicator == OFF)
			  {
			      LED_LEFT_SA_INDICATOR_OFF;
			  }
	if (car_data.right_indicator == ON && car_data.right_turn_signal==OFF)
		{
		   LED_RIGHT_SA_INDICATOR_ON;
		   if (VALUE_PARAM_BUZZER==ON) flag_buzzer=ON; else  if (VALUE_PARAM_BUZZER==OFF) flag_buzzer=OFF;
		}
		   else if (car_data.right_indicator == OFF)
			   {
			      LED_RIGHT_SA_INDICATOR_OFF;
			   }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
        if(htim->Instance == TIM1) //check if the interrupt comes from TIM1
        {
        	if (car_data.left_turn_signal==ON && car_data.right_turn_signal==ON)
        	{
        		LED_LEFT_SA_INDICATOR_OFF;
        		LED_RIGHT_SA_INDICATOR_OFF;
        	}
        	else
				{
					if (car_data.left_indicator == ON  && car_data.left_turn_signal==ON) HAL_GPIO_TogglePin(LED_LEFT_SA_INDICATOR_GPIO_Port, LED_LEFT_SA_INDICATOR_Pin);
					else if (car_data.left_indicator == OFF) LED_LEFT_SA_INDICATOR_OFF;

					if (car_data.right_indicator == ON  && car_data.right_turn_signal==ON) HAL_GPIO_TogglePin(LED_RIGHT_SA_INDICATOR_GPIO_Port, LED_RIGHT_SA_INDICATOR_Pin);
					else if (car_data.right_indicator == OFF) LED_RIGHT_SA_INDICATOR_OFF;
				}

        	if (flag_buzzer==ON && car_data.speed>=120)
        		{
        		   BUZZER_Go(TBUZ_50, TICK_12);
        		   flag_buzzer=OFF;
        		}

        }
}

int cb(const int value, const int position)
    {
    	return (((value & position)) != 0);
    }

int cb_mask(const int value, const int mask)
    {
       if ((mask & value) == mask) return 1; else return 0;
       return 0;
    }

///*** SYSTICK (T = 1ms) ***/
///**
//  * @brief  SYSTICK callback.
//  * @retval None
//  */
void HAL_SYSTICK_Callback(void)
{
	BUZZER_Handler();
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
