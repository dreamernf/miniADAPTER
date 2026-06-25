# miniADAPTER

STM32CubeIDE / STM32Cube HAL project template for an STM32F042C6Tx mini adapter.

## Peripherals

- **CAN**: normal mode, **100 kbit/s**.
  - Clock source: APB1 at 8 MHz from HSI.
  - Bit timing: prescaler 5, SJW 1 tq, BS1 13 tq, BS2 2 tq (16 tq total).
  - Pins: PA11 = CAN_RX, PA12 = CAN_TX.
- **USART1**: asynchronous **115200 8N1**, TX/RX enabled.
  - Pins: PA9 = USART1_TX, PA10 = USART1_RX.

Open `miniADAPTER.ioc` in STM32CubeIDE, generate code if needed, then build the project.
