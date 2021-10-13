#include "stm32mp1xx.h"

int main(void)
{
  /*HW semaphore Clock enable*/
  RCC->MC_AHB3ENSETR = RCC_MC_AHB3ENSETR_HSEMEN;

  RCC->MC_AHB4ENSETR = RCC_MC_AHB4ENSETR_GPIOHEN;

  GPIOH->MODER &= GPIO_MODER_MODER7;
  GPIOH->MODER |= GPIO_MODER_MODER7_0;
  
  //BSP_LED_Init(LED7);

  while (1)
  {
	
    GPIOH->BSRR = GPIO_BSRR_BR7;
    for(uint32_t i=0; i<0x1000000; i++){}
    GPIOH->BSRR = GPIO_BSRR_BS7;
    for(uint32_t i=0; i<0x1000000; i++){}
    //BSP_LED_Toggle(LED7);
	  //HAL_Delay(1000);
  }
}

