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
#include "log.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

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



/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
CAR_DATA_t  car_data;
CAN_RX_FRAME_t rx_frame = {0,};
CAN_TX_FRAME_t tx_frame = {0,};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
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


  Set_Filter_CAN(CANBUS_CLUSTER ,CAN_RX_FIFO0, 0x531, 0x3BA, 0x5C1, 0x320, 0);
  Set_Filter_CAN(CANBUS_CLUSTER ,CAN_RX_FIFO0, 0x67C, 0x000, 0x000, 0x000, 1);
  CAN_Start(CANBUS_CLUSTER);
  HAL_TIM_Base_Start_IT(&htim1);

  logger_uart("INIT OK\n\r");


  LED_LEFT_SA_INDICATOR_ON;
  LED_RIGHT_SA_INDICATOR_ON;
  HAL_Delay(500);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	CAN_Std_Data_receive(CANBUS_CLUSTER, &rx_frame);

	//"(e*256+d)/192"
	if (rx_frame.can_id==0x320)
		{
		   car_data.speed = (rx_frame.data[3]<<8 | rx_frame.data[4])/192;
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
