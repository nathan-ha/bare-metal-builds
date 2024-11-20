#include "MK64F12.h"
// #include "fsl_device_registers.h"

#define ADC0_SE8_PTB0   8   
#define ADC0_SE9_PTB1   9   
#define ADC0_SE12_PTB2  12  
#define ADC0_SE13_PTB3  13 
#define ADC0_SE14_PTC0  14 
#define ADC0_SE15_PTC1  15 
#define ADC0_SE17_PTE24 17


uint16_t ADC_read16b(uint16_t channel) {
  // Start conversion by writing the channel to SC1A
  ADC0_SC1A = channel & 0x1F;
  while (ADC0_SC2 & ADC_SC2_ADACT_MASK); // Conversion in progress
  while (!(ADC0_SC1A & ADC_SC1_COCO_MASK)); // Wait until conversion complete
  return ADC0_RA;
}


void setup() {
  SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK; /*Enable Port C Clock Gate Control*/
  SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK; /*Enable Port D Clock Gate Control*/
  SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK; /*Enable Port E Clock Gate Control*/
  SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;  // Enable clock for ADC0

  PORTD_GPCLR = 0x01FF0100;  // PD[7:0] for GPIO;
  GPIOD_PDDR = 0x000000FFFF; // PD[15:0] output

  // Configure ADC0
  ADC0_CFG1 = 0x0C; // 16-bit mode, bus clock, long sample time
  ADC0_SC1A = 0x1F; // Disable the module, ADCH = 11111 (disable channel)
}

int main() {
  setup();
  uint16_t data1;
  for (;;) {
    data1 = ADC_read16b(ADC0_SE12_PTB2);
  }
}
