#include "fsl_device_registers.h"

#define ADC0_SE8_PTB0   8   // Joystick X
#define ADC0_SE9_PTB1   9   // JoyStick Y
#define ADC0_SE12_PTB2  12  // Joystick SW
#define ADC0_SE13_PTB3  13  // Photoresistor N
#define ADC0_SE14_PTC0  14  // Photoresistor S
#define ADC0_SE15_PTC1  15  // Photoresistor E
#define ADC0_SE17_PTE24  17 // Photoresistor W


void SW_DELAY(uint64_t ticks) {
  for (volatile int i = 0; i < ticks; i++);
}

uint16_t ADC_read16b(uint16_t channel) {
  // Start conversion by writing the channel to SC1A
  ADC0_SC1A = channel & 0x1F;
  while (ADC0_SC2 & ADC_SC2_ADACT_MASK); // Conversion in progress
  while (!(ADC0_SC1A & ADC_SC1_COCO_MASK)); // Wait until conversion complete
  return ADC0_RA;
}

enum { A, B, C, D } STEP_HORIZONTAL = A, STEP_VERTICAL = A;
const uint8_t STEP_VALUES_HORIZONTAL[] = {0b0001, 0b0011, 0b0010, 0b0110,
                                          0b0100, 0b1100, 0b1000, 0b1001};
const uint8_t STEP_VALUES_VERTICAL[] = {0b0001 << 4, 0b0011 << 4, 0b0010 << 4,
                                        0b0110 << 4, 0b0100 << 4, 0b1100 << 4,
                                        0b1000 << 4, 0b1001 << 4};

void setup() {
  SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;
  SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK; /*Enable Port C Clock Gate Control*/
  SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK; /*Enable Port D Clock Gate Control*/

  PORTD_GPCLR = 0x01FF0100;  // PD[7:0] for GPIO;
  GPIOD_PDDR = 0x000000FFFF; // PD[15:0] output

  // Enable clock for ADC0
  SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;
  // Configure ADC0
  ADC0_CFG1 = 0x0C; // 16-bit mode, bus clock, long sample time
  ADC0_SC1A = 0x1F; // Disable the module, ADCH = 11111 (disable channel)
}

void read_photosensor() {}

// Horizontal: 0 - counterclockwise
//			   		 1 - clockwise
//
void rotate_tick(uint8_t ROT_DIR_VERTICAL, uint8_t ROT_DIR_HORIZONTAL) {
  static int16_t i = 0; // Changed to signed int16_t
  static int16_t j = 0; // Changed to signed int16_t

  // Horizontal rotation
  if (ROT_DIR_VERTICAL) {
    i = (i + 1) % 8;
    if (i < 0)
      i += 8; // Handle negative wraparound
  } else {
    i = (i - 1) % 8;
    if (i < 0)
      i += 8; // Handle negative wraparound
  }

  // Vertical rotation
  if (ROT_DIR_HORIZONTAL) {
    j = (j + 1) % 8;
    if (j < 0)
      j += 8; // Handle negative wraparound
  } else {
    j = (j - 1) % 8;
    if (j < 0)
      j += 8; // Handle negative wraparound
  }

  GPIOD_PDOR = 0;
  GPIOD_PDOR |= STEP_VALUES_VERTICAL[i];
  GPIOD_PDOR |= STEP_VALUES_HORIZONTAL[j];
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

    SW_DELAY(3000);
  }
}
