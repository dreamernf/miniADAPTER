#include "stm32f0xx.h"

uint32_t SystemCoreClock = 8000000U;
const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
const uint8_t APBPrescTable[8] = {0, 0, 0, 0, 1, 2, 3, 4};

void SystemInit(void)
{
}

void SystemCoreClockUpdate(void)
{
  SystemCoreClock = 8000000U;
}
