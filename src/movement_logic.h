#ifndef __MOVEMENT_LOGIC_H__
#define __MOVEMENT_LOGIC_H__

#include "adc.h"
#include "photoresistor.h"
#include "stepper.h"
#include "uart.h"

#define HIGH_THRESHOLD (uint16_t)0xFF00
#define LOW_THRESHOLD (uint16_t)0x2000

#define STEP_HORIZONTAL 1
#define STEP_VERTICAL 2

#define PIN_LED 26

uint8_t manual_mode = 1;

enum tracker_state { HOME, N, S, E, W, NE, SE, NW, SW } tstate;

enum joystick_state { UP_OFF, DOWN_ON, UP_ON, DOWN_OFF } jstate;

void tick_automatic() {
  const uint16_t pn = ADC_read16b(ADC_PHOTORESISTOR_N);
  const uint16_t ps = ADC_read16b(ADC_PHOTORESISTOR_S);
  const uint16_t pe = ADC_read16b(ADC_PHOTORESISTOR_E);
  const uint16_t pw = ADC_read16b(ADC_PHOTORESISTOR_W);

  const uint8_t home = adc_equal(pe, pw) && adc_equal(ps, pn);
  const uint8_t north = adc_equal(pn, pw) && pn > ps;
  const uint8_t south = adc_equal(pe, pw) && ps > pn;
  const uint8_t east = adc_equal(ps, pn) && pe > pw;
  const uint8_t west = adc_equal(ps, pn) && pw > pe;
  const uint8_t northeast = pn > ps && pe > pw;
  const uint8_t northwest = pn > ps && pw > pe;
  const uint8_t southwest = ps > pn && pw > pe;
  const uint8_t southeast = ps > pn && pe > pw;

  switch (tstate) {

  case HOME:
    if (home) {
      break;
    } else if (north) {
      tstate = N;
      rotate_by(0, STEP_VERTICAL);
    } else if (south) {
      tstate = S;
      rotate_by(0, STEP_VERTICAL * -1);
    } else if (east) {
      tstate = E;
      rotate_by(STEP_HORIZONTAL * 2, STEP_VERTICAL);
    } else if (west) {
      tstate = W;
      rotate_by(STEP_HORIZONTAL * -2, STEP_VERTICAL);
    } else if (northeast) {
      tstate = NE;
      rotate_by(STEP_HORIZONTAL, STEP_VERTICAL);
    } else if (northwest) {
      tstate = NW;
      rotate_by(STEP_HORIZONTAL * -1, STEP_VERTICAL);
    } else if (southwest) {
      tstate = SW;
      rotate_by(STEP_HORIZONTAL, STEP_VERTICAL * -1);
    } else if (southeast) {
      tstate = SE;
      rotate_by(STEP_HORIZONTAL * -1, STEP_VERTICAL * -1);
    } else {
      uprintf("%s:%d - FSM Error", __FILE__, __LINE__);
    }
    break;
  case N:
    if (!north) {
      tstate = HOME;
      rotate_by(0, -STEP_VERTICAL);
    }
    break;
  case S:
    if (!south) {
      tstate = HOME;
      rotate_by(0, STEP_VERTICAL);
      break;
    case E:
      if (!east) {
        tstate = HOME;
        rotate_by(STEP_HORIZONTAL * -2, STEP_VERTICAL * -1);
      }
      break;
    case W:
      if (!west) {
        tstate = HOME;
        rotate_by(STEP_HORIZONTAL * 2, STEP_VERTICAL * -1);
      }
      break;
    case NE:
      if (!northeast) {
        tstate = HOME;
        rotate_by(STEP_HORIZONTAL * -1, STEP_VERTICAL * -1);
      }
      break;
    case NW:
      if (!northwest) {
        tstate = HOME;
        rotate_by(STEP_HORIZONTAL, STEP_VERTICAL * -1);
      }
      break;
    case SE:
      if (!southeast) {
        tstate = HOME;
        rotate_by(STEP_HORIZONTAL, STEP_VERTICAL);
      }
      break;
    case SW:
      if (!southwest) {
        tstate = HOME;
        rotate_by(STEP_HORIZONTAL * -1, STEP_VERTICAL);
      }
      break;
    default:
      tstate = HOME;
      uprintf("%s:%d - Automatic FSM Warning", __FILE__, __LINE__);
      break;
    }
  }
}

void tick_joystick() {
  const uint16_t stick_down = ADC_read16b(ADC0_SE12_PTB2) < 200;
  switch (jstate) {
  case UP_OFF:
    if (stick_down) {
      manual_mode = 1;
      jstate = DOWN_ON;
    }
    break;
  case DOWN_ON:
    if (!stick_down) {
      jstate = UP_ON;
    }
    break;
  case UP_ON:
    if (stick_down) {
      manual_mode = 0;
      jstate = DOWN_OFF;
    }
    break;
  case DOWN_OFF:
    if (!stick_down) {
      jstate = UP_OFF;
    }
    break;
  default:
    jstate = UP_OFF;
    uprintf("%s:%d - Joystick FSM Warning", __FILE__, __LINE__);
    break;
  }
}

void tick_manual() {
  uint16_t stick_x = ADC_read16b(ADC0_SE8_PTB0);
  uint16_t stick_y = ADC_read16b(ADC0_SE9_PTB1);

  if (stick_x > HIGH_THRESHOLD) {
    rotate_one_tick(CLOCKWISE, NOT_MOVING); // Clockwise
  } else if (stick_x < LOW_THRESHOLD) {
    rotate_one_tick(COUNTER_CLOCKWISE, NOT_MOVING); // Counterclockwise
  }
  if (stick_y > HIGH_THRESHOLD) {
    rotate_one_tick(NOT_MOVING, CLOCKWISE);
  } else if (stick_y < LOW_THRESHOLD) {
    rotate_one_tick(NOT_MOVING, COUNTER_CLOCKWISE);
  }
}

#endif