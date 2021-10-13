#include "stm32mp1xx.h"

/************************* Miscellaneous Configuration ************************/
/*!< Uncomment the following line if you need to use external SRAM mounted
     on EVAL board as data memory  */
/* #define DATA_IN_ExtSRAM */

/*!< Uncomment the following line if you need to relocate your vector Table in
     Internal SRAM. */
/* #define VECT_TAB_SRAM */
#define VECT_TAB_OFFSET  0x00 /*!< Vector Table base offset field. 
                                   This value must be a multiple of 0x400. */
/******************************************************************************/

#if !defined  (HSE_VALUE) 
#define HSE_VALUE    ((uint32_t)24000000) /*!< Value of the External oscillator in Hz : FPGA case fixed to 60MHZ */
#endif /* HSE_VALUE */

#if !defined  (HSE_STARTUP_TIMEOUT)
  #define HSE_STARTUP_TIMEOUT    ((uint32_t)100U)   /*!< Time out for HSE start up, in ms */
#endif /* HSE_STARTUP_TIMEOUT */


#if !defined  (HSI_VALUE)
  #define HSI_VALUE    ((uint32_t)64000000) /*!< Value of the Internal oscillator in Hz*/
#endif /* HSI_VALUE */

#if !defined  (CSI_VALUE)
  #define CSI_VALUE    4000000U /*!< Value of the Internal oscillator in Hz*/
#endif /* CSI_VALUE */



uint32_t SystemCoreClock = HSI_VALUE;

#if defined (DATA_IN_ExtSRAM) 
  static void SystemInit_ExtMemCtl(void); 
#endif /* DATA_IN_ExtSRAM */

/**
  * @}
  */

/** @addtogroup STM32MP1xx_System_Private_Functions
  * @{
  */

  /**
  * @brief  Setup the microcontroller system
  *         Initialize the FPU setting, vector table location and External memory 
  *         configuration.
  * @param  None
  * @retval None
  */
void SystemInit (void)
{
  /* FPU settings ------------------------------------------------------------*/
#if defined (CORE_CM4)
  #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
   SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
  #endif

  /* Configure the Vector Table location add offset address ------------------*/
#if defined (VECT_TAB_SRAM)
  SCB->VTOR = MCU_AHB_SRAM | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM */
#endif
  /* Disable all interrupts and events */
  CLEAR_REG(EXTI_C2->IMR1);
  CLEAR_REG(EXTI_C2->IMR2);
  CLEAR_REG(EXTI_C2->IMR3);
  CLEAR_REG(EXTI_C2->EMR1);
  CLEAR_REG(EXTI_C2->EMR2);
  CLEAR_REG(EXTI_C2->EMR3);
#else
#error Please #define CORE_CM4
#endif	                         
}

/**
   * @brief Update SystemCoreClock variable according to Clock Register Values.
  *         The SystemCoreClock variable contains the core clock frequency (Hz),
  *         it can be used by the user application to setup the SysTick timer or
  *         configure other parameters.
  *
  * @note   Each time the core clock changes, this function must be called to
  *         update SystemCoreClock variable value. Otherwise, any configuration
  *         based on this variable will be incorrect.
  *
  * @note   - The system frequency computed by this function is not the real
  *           frequency in the chip. It is calculated based on the predefined 
  *           constant and the selected clock source:
  *
  *           - If SYSCLK source is HSI, SystemCoreClock will contain the
  *             HSI_VALUE(*)
  *
  *           - If SYSCLK source is HSE, SystemCoreClock will contain the
  *             HSE_VALUE(**)
  *
  *           - If SYSCLK source is CSI, SystemCoreClock will contain the
  *             CSI_VALUE(***)
  *
  *           - If SYSCLK source is PLL3_P, SystemCoreClock will contain the
  *             HSI_VALUE(*) or the HSE_VALUE(*) or the CSI_VALUE(***)
  *             multiplied/divided by the PLL3 factors.
  *
  *         (*) HSI_VALUE is a constant defined in stm32mp1xx_hal_conf.h file
  *             (default value 64 MHz) but the real value may vary depending
  *             on the variations in voltage and temperature.
  *
  *         (**) HSE_VALUE is a constant defined in stm32mp1xx_hal_conf.h file
  *              (default value 24 MHz), user has to ensure that HSE_VALUE is
  *              same as the real frequency of the crystal used. Otherwise, this
  *              function may have wrong result.
  *
  *         (***) CSI_VALUE is a constant defined in stm32mp1xx_hal_conf.h file
  *              (default value 4 MHz)but the real value may vary depending
  *              on the variations in voltage and temperature.
  *
  *         - The result of this function could be not correct when using
  *           fractional value for HSE crystal.
  *
  * @param  None
  * @retval None
  */
void SystemCoreClockUpdate (void)
{
  uint32_t   pllsource, pll3m, pll3fracen;
  float fracn1, pll3vco;

  switch (RCC->MSSCKSELR & RCC_MSSCKSELR_MCUSSRC)
  {
  case 0x00:  /* HSI used as system clock source */
    SystemCoreClock = (HSI_VALUE >> (RCC->HSICFGR & RCC_HSICFGR_HSIDIV));
    break;

  case 0x01:  /* HSE used as system clock source */
    SystemCoreClock = HSE_VALUE;
    break;

  case 0x02:  /* CSI used as system clock source */
    SystemCoreClock = CSI_VALUE;
    break;

  case 0x03:  /* PLL3_P used as system clock source */
    pllsource = (RCC->RCK3SELR & RCC_RCK3SELR_PLL3SRC);
    pll3m = ((RCC->PLL3CFGR1 & RCC_PLL3CFGR1_DIVM3) >> RCC_PLL3CFGR1_DIVM3_Pos) + 1U;
    pll3fracen = (RCC->PLL3FRACR & RCC_PLL3FRACR_FRACLE) >> 16U;
    fracn1 = (float)(pll3fracen * ((RCC->PLL3FRACR & RCC_PLL3FRACR_FRACV) >> 3U));
    pll3vco = (float)((float)((RCC->PLL3CFGR1 & RCC_PLL3CFGR1_DIVN) + 1U) + (fracn1 / (float) 0x1FFF));

    if (pll3m != 0U)
    {
      switch (pllsource)
      {
        case 0x00:  /* HSI used as PLL clock source */
          pll3vco *= (float)((HSI_VALUE >> (RCC->HSICFGR & RCC_HSICFGR_HSIDIV)) / pll3m);
          break;

        case 0x01:  /* HSE used as PLL clock source */
          pll3vco *= (float)(HSE_VALUE / pll3m);
          break;

        case 0x02:  /* CSI used as PLL clock source */
          pll3vco *= (float)(CSI_VALUE / pll3m);
          break;

        case 0x03:  /* No clock source for PLL */
          pll3vco = 0;
          break;
       }
      SystemCoreClock = (uint32_t)(pll3vco/ ((float)((RCC->PLL3CFGR2 & RCC_PLL3CFGR2_DIVP) + 1U)));
    }
    else
    {
      SystemCoreClock = 0U;
    }
    break;
  }

  /* Compute mcu_ck */
  SystemCoreClock = SystemCoreClock >> (RCC->MCUDIVR & RCC_MCUDIVR_MCUDIV);
}


#ifdef DATA_IN_ExtSRAM
/**
  * @brief  Setup the external memory controller.
  *         Called in startup_stm32mp15xx.s before jump to main.
  *         This function configures the external SRAM mounted on Eval boards
  *         This SRAM will be used as program data memory (including heap and stack).
  * @param  None
  * @retval None
  */
void SystemInit_ExtMemCtl(void)
{
  
}
#endif /* DATA_IN_ExtSRAM */
  
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
