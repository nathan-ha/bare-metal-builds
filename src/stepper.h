#ifndef __STEPPER_H__
#define __STEPPER_H__

#include "fsl_device_registers.h"

enum { A, B, C, D } STEP_HORIZONTAL = A, STEP_VERTICAL = A;
const uint8_t STEP_VALUES_HORIZONTAL[] = {0b0001, 0b0011, 0b0010, 0b0110,
                                          0b0100, 0b1100, 0b1000, 0b1001};
const uint8_t STEP_VALUES_VERTICAL[] = {0b0001 << 4, 0b0011 << 4, 0b0010 << 4,
                                        0b0110 << 4, 0b0100 << 4, 0b1100 << 4,
                                        0b1000 << 4, 0b1001 << 4};

// using port D[7:0] for stepper motor control
void stepper_init() {
  SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK; /*Enable Port D Clock Gate Control*/
  PORTD_GPCLR = 0x01FF0100;          // PD[7:0] for GPIO;
  GPIOD_PDDR = 0x000000FFFF;         // PD[15:0] output
}

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

  GPIOD_PDOR = STEP_VALUES_VERTICAL[i] | STEP_VALUES_HORIZONTAL[j];
}

#endif