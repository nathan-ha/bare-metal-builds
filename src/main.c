// register definitions are in stm32f303xe.h
#include "../inc/main.h"

int main(void) {
    SystemInit(); // Initialize system

    // Blink
    // Initialize LED
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN; // Enable GPIOA clock
    GPIOA->MODER &= ~(0x3 << (5 * 2));   // Clear mode bits
    GPIOA->MODER |= (0x1 << (5 * 2));    // Set PA5 to output
    while (1) {
        GPIOA->ODR ^= (1 << 5); // Toggle PA5
        volatile int i = 1000000;
        while(i--);          // Simple delay
    }
}