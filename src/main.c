#include "fsl_device_registers.h"
#include "photoresistor.h"
#include "stepper.h"
#include "adc.h"
#include "uart.h"
#include "delay.h"

void SW_DELAY(uint64_t ticks) {
  for (volatile int i = 0; i < ticks; i++);
}

void setup() {
  UART0_init();
  ADC_init();
  stepper_init();
  photoresistor_init();
  timer_init();
}

int main() {
  int32_t data1, data2;
  setup();

  // Define thresholds with a dead zone
  const uint16_t HIGH_THRESHOLD = 0xFF00; // About 75% of range
  const uint16_t LOW_THRESHOLD = 0x2000;  // About 25% of range

  for (;;) {
    data1 = ADC_read16b(ADC0_SE12_PTB2);

    if (data1 > HIGH_THRESHOLD) {
      rotate_tick(1, 0); // Clockwise
    } else if (data1 < LOW_THRESHOLD) {
      rotate_tick(0, 0); // Counterclockwise
    }
    // If in dead zone (between thresholds), motor stays still
    uprint("hello");
    delayms(200);
  }
}
