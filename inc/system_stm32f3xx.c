#include "stm32f3xx.h"

// Forward declaration
void SystemInit(void);

uint32_t SystemCoreClock = 8000000; // Default to 8 MHz

void SystemInit(void)
{
    // Reset the RCC clock configuration to the default reset state
    RCC->CR |= RCC_CR_HSION;  // Enable HSI
    while(!(RCC->CR & RCC_CR_HSIRDY)); // Wait for HSI ready
    
    // Reset CFGR register
    RCC->CFGR = 0x00000000;
    
    // Reset HSEON, CSSON and PLLON bits
    RCC->CR &= ~(RCC_CR_HSEON | RCC_CR_CSSON | RCC_CR_PLLON);
    
    // Reset HSEBYP bit
    RCC->CR &= ~RCC_CR_HSEBYP;
    
    // Disable all interrupts
    RCC->CIR = 0x00000000;

    // Enable FPU if needed
    #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
      SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  // set CP10 and CP11 Full Access
    #endif
}

void SystemCoreClockUpdate(void)
{
    // Add clock update code if needed
    SystemCoreClock = 8000000; // For now, just set to HSI speed
}