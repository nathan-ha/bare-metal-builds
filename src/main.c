#define STM32F303xE // using f303re
#include "../inc/stm32f3xx.h"


void LED_Init(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN; // Enable GPIOA clock
    GPIOA->MODER &= ~(0x3 << (5 * 2));   // Clear mode bits
    GPIOA->MODER |= (0x1 << (5 * 2));    // Set PA5 to output
}

void Delay(volatile uint32_t delay) {
    while (delay--);
}

// Add vector table handler
void NMI_Handler(void) { }
void HardFault_Handler(void) { while (1); }
void MemManage_Handler(void) { while (1); }
void BusFault_Handler(void) { while (1); }
void UsageFault_Handler(void) { while (1); }
void SVC_Handler(void) { }
void DebugMon_Handler(void) { }
void PendSV_Handler(void) { }
void SysTick_Handler(void) { }

int main(void) {
    // Enable FPU if needed
    #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
        SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));
    #endif

    SystemInit(); // Initialize system
    LED_Init();   // Initialize LED

    while (1) {
        GPIOA->ODR ^= (1 << 5); // Toggle PA5
        Delay(100000);          // Simple delay
    }
}