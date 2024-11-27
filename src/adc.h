#ifndef __ADC_H__
#define __ADC_H__

#include "fsl_device_registers.h"

#define ADC0_SE12_PTB2 12    // Joystick X
#define ADC0_SE13_PTB3 13   // JoyStick Y
#define ADC0_SE14_PTC0 14  // Photoresistor S
#define ADC0_SE15_PTC1 15  // Photoresistor E
#define ADC0_SE17_PTE24 17 // Photoresistor W

#define ADC0_SE18_PTE25 18  // Photoresistor N
#define ADC0_SE23_DAC0_OUT   23  // Joystick SW

#define ADC_EQUAL_THRESHOLD 50

void ADC_init() {
  // Enable clock for ADC0
  SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;
  // Configure ADC0
  ADC0_CFG1 = 0x0C; // 16-bit mode, bus clock, long sample time
  ADC0_SC1A = 0x1F; // Disable the module, ADCH = 11111 (disable channel)
}

uint16_t ADC_read16b(uint16_t channel) {
  // Start conversion by writing the channel to SC1A
  ADC0_SC1A = channel & 0x1F;
  while (ADC0_SC2 & ADC_SC2_ADACT_MASK); // Conversion in progress
  while (!(ADC0_SC1A & ADC_SC1_COCO_MASK)); // Wait until conversion complete
  return ADC0_RA;
}


// Returns true if ADC values are within threshold of each other
uint8_t adc_equal(uint16_t x, uint16_t y) {
    uint16_t diff = (x > y) ? (x - y) : (y - x);
    return diff <= ADC_EQUAL_THRESHOLD;
}

uint16_t map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max) {
  x = x < in_min ? in_min : (x > in_max ? in_max : x); // squeezes x if it's out of range
  return (((uint32_t)(x - in_min) * (out_max - out_min)) / (in_max - in_min)) + out_min;
}
#endif