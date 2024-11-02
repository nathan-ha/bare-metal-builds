#define STM32F303xE // using f303re
#include "include/stm32f3xx.h"

void LED_Init(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN; // Enable GPIOA clock
    GPIOA->MODER &= ~(0x3 << (5 * 2));   // Clear mode bits
    GPIOA->MODER |= (0x1 << (5 * 2));    // Set PA5 to output
}

void Delay(volatile uint32_t delay) {
    while (delay--);
}

int main(void) {
    LED_Init(); // Initialize the LED

    while (1) {
        GPIOA->ODR ^= (1 << 5); // Toggle PA5
        Delay(1000); // Simple delay
    }
}