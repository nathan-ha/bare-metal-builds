#ifndef __STEPPER_H__
#define __STEPPER_H__

#include "fsl_device_registers.h"
#include "stdint.h"

#define SW_DELAY_TICKS 2500
#define COUNTER_CLOCKWISE 0
#define CLOCKWISE 1
#define NOT_MOVING 2


const uint32_t STEP_VALUES_HORIZONTAL[] = {0b0001, 0b0011, 0b0010, 0b0110,
                                          0b0100, 0b1100, 0b1000, 0b1001};
const uint32_t STEP_VALUES_VERTICAL[] = {0b0001 << 4, 0b0011 << 4, 0b0010 << 4,
                                        0b0110 << 4, 0b0100 << 4, 0b1100 << 4,
                                        0b1000 << 4, 0b1001 << 4};

void SW_DELAY(uint64_t ticks) {
  for (volatile int i = 0; i < ticks; i++);
}

// using port D[7:0] for stepper motor control
void stepper_init() {
  SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK; /*Enable Port D Clock Gate Control*/
  PORTD_GPCLR = 0x01FF0100;          // PD[7:0] for GPIO;
  GPIOD_PDDR = 0x000000FFFF;         // PD[15:0] output
}

void rotate_one_tick(uint8_t ROT_DIR_VERTICAL, uint8_t ROT_DIR_HORIZONTAL) {
  static int16_t i = 0;
  static int16_t j = 0;

  if (ROT_DIR_VERTICAL && ROT_DIR_VERTICAL != NOT_MOVING) {
    i = (i + 1) % 8;
    if (i < 0)
      i += 8;
  } else if (ROT_DIR_VERTICAL != NOT_MOVING) {
    i = (i - 1) % 8;
    if (i < 0)
      i += 8;
  }

  if (ROT_DIR_HORIZONTAL && ROT_DIR_HORIZONTAL != NOT_MOVING) {
    // clockwise
    j = (j + 1) % 8;
    if (j < 0)
      j += 8;
  } else if (ROT_DIR_HORIZONTAL != NOT_MOVING) {
    // counterclockwise
    j = (j - 1) % 8;
    if (j < 0)
      j += 8;
  }

  GPIOD_PDOR = STEP_VALUES_VERTICAL[i] | STEP_VALUES_HORIZONTAL[j];
}


void rotate_by(int32_t x, int32_t y) {
  uint8_t HORIZONTAL_CLOCKWISE = x > 0;
  uint8_t VERTICAL_FACE_DOWN = y > 0;

  if (x < 0) x = -x;
  if (y < 0) y = -y;

  while (x > 0 && y > 0) {
    rotate_one_tick(HORIZONTAL_CLOCKWISE, VERTICAL_FACE_DOWN);
    SW_DELAY(SW_DELAY_TICKS);
    x--;
    y--;
  }

  while (x > 0) {
    rotate_one_tick(HORIZONTAL_CLOCKWISE, NOT_MOVING);
    SW_DELAY(SW_DELAY_TICKS);
    x--;
  }

  while (y > 0) {
    rotate_one_tick(NOT_MOVING, VERTICAL_FACE_DOWN);
    SW_DELAY(SW_DELAY_TICKS);
    y--;
  }
}

#endif