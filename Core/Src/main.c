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
#define   OUT2_ON   HAL_GPIO_WritePin(OUT2_GPIO_Port, OUT2_Pin, GPIO_PIN_SET)
#define   OUT2_OFF  HAL_GPIO_WritePin(OUT2_GPIO_Port, OUT2_Pin, GPIO_PIN_RESET)



/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
CAN_RX_FRAME_t rx_frame = {0,};
CAN_TX_FRAME_t tx_frame = {0,};
static uint32_t led_last_toggle_ms;
static GPIO_PinState button_last_state = GPIO_PIN_SET;
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
  MX_USART2_UART_Init();
  MX_CAN_Init();
  /* USER CODE BEGIN 2 */

  logger_uart_no_dma("\r\n=== miniADAPTER CAN diagnostic ===\r\n");
  logger_uart_no_dma("UART: 115200 8N1 | CAN: PCLK/30/16TQ = 100 kbit/s\r\n");
  logger_uart_no_dma("Test frame: STD 0x6FE every 1000 ms\r\n");

  if (CAN_Diagnostic_LoopbackSelfTest(CANBUS_COMFORT) == HAL_OK)
  {
    logger_uart_no_dma("[CAN][SELFTEST] PASS - MCU CAN controller and software path work\r\n");
  }
  else
  {
    logger_uart_no_dma("[CAN][SELFTEST] FAIL - MCU clock/config/controller problem\r\n");
  }

  if (Set_Filter_CAN(CANBUS_COMFORT, CAN_RX_FIFO0, 0x2C1, 0x2C3, 0x5C1, 0x320, 0) != HAL_OK)
  {
    logger_uart_no_dma("[CAN][INIT] FILTER FAIL err=0x%08lX\r\n", (unsigned long)HAL_CAN_GetError(CANBUS_COMFORT));
    Error_Handler();
  }
  if (CAN_Diagnostic_Enable(CANBUS_COMFORT) != HAL_OK)
  {
    logger_uart_no_dma("[CAN][INIT] NOTIFICATION FAIL err=0x%08lX\r\n", (unsigned long)HAL_CAN_GetError(CANBUS_COMFORT));
    Error_Handler();
  }
  if (CAN_Start(CANBUS_COMFORT) != HAL_OK)
  {
    logger_uart_no_dma("[CAN][INIT] START FAIL err=0x%08lX\r\n", (unsigned long)HAL_CAN_GetError(CANBUS_COMFORT));
    Error_Handler();
  }
  logger_uart_no_dma("[CAN][INIT] OK state=%u BTR=0x%08lX\r\n",
                     (unsigned int)HAL_CAN_GetState(CANBUS_COMFORT),
                     (unsigned long)(CANBUS_COMFORT)->Instance->BTR);
  logger_uart_no_dma("Interpretation: SELFTEST PASS + repeated ACK error => check transceiver/wiring/termination/peer node\r\n");

  OUT2_OFF;
  HAL_Delay(500);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	if (CAN_Std_Data_receive(CANBUS_COMFORT, &rx_frame) == CAN_RX_READY)
	{
	  Logger_CAN_RX(YES, rx_frame);
	}

	CAN_Diagnostic_Task(CANBUS_COMFORT);

	if ((HAL_GetTick() - led_last_toggle_ms) >= 500U)
	{
	  HAL_GPIO_TogglePin(OUT2_GPIO_Port, OUT2_Pin);
	  led_last_toggle_ms = HAL_GetTick();
	}

	{
	  GPIO_PinState button_state = HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin);
	  if (button_state != button_last_state)
	  {
		button_last_state = button_state;
		logger_uart_no_dma("[GPIO] BUTTON %s\r\n", button_state == GPIO_PIN_RESET ? "PRESSED" : "RELEASED");
	  }
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
#ifdef USE_FULL_ASSERT
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
