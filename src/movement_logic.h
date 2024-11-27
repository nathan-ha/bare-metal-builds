#ifndef __MOVEMENT_LOGIC_H__
#define __MOVEMENT_LOGIC_H__
#include <stdlib.h>

#include "adc.h"
#include "photoresistor.h"
#include "stepper.h"
#include "uart.h"

#define HIGH_THRESHOLD (uint16_t)50000
#define LOW_THRESHOLD (uint16_t)5000

#define STEP_HORIZONTAL 550
#define STEP_VERTICAL (STEP_HORIZONTAL)

#define PIN_LED 26

#define USUB(x, y) ((x) > (y) ? (x) - (y) : (y) - (x))

uint8_t manual_mode = 1;

enum tracker_state { HOME, N, S, E, W, NE, SE, NW, SW } tstate;

enum joystick_state { UP_OFF, DOWN_ON, UP_ON, DOWN_OFF } jstate;

void tick_automatic() {
  uint16_t pn = ADC_read16b(ADC_PHOTORESISTOR_N);
  uint16_t ps = ADC_read16b(ADC_PHOTORESISTOR_S);
  uint16_t pe = ADC_read16b(ADC_PHOTORESISTOR_E);
  uint16_t pw = ADC_read16b(ADC_PHOTORESISTOR_W);

  pn = map(pn, 40000, 62000, 0, 255);
  ps = map(ps, 6000, 60000, 0, 255);
  pe = map(pe, 2000, 55000, 0, 255);
  pw = map(pw, 50000, 65000, 0, 255);

  const uint8_t home =
      USUB(pe, pw) <= 10 && // East and West sensors are very close
      USUB(pn, ps) <= 10;   // North and South sensors are very close

  const uint8_t north = pn > ps && pn > pe && pn > pw;
  const uint8_t south = ps > pn && ps > pe && ps > pw;
  const uint8_t east = pe > pn && pe > ps && pe > pw;
  const uint8_t west = pw > pn && pw > ps && pw > pe;

  const uint8_t northeast =
      pn > ps && pe > pw && pn > (ps + 20) && pe > (pw + 20);

  const uint8_t northwest =
      pn > ps && pw > pe && pn > (ps + 20) && pw > (pe + 20);

  const uint8_t southeast =
      ps > pn && pe > pw && ps > (pn + 20) && pe > (pw + 20);

  const uint8_t southwest =
      ps > pn && pw > pe && ps > (pn + 20) && pw > (pe + 20);

  switch (tstate) {
  case HOME:
    if (home) {
      break;
    } else if (north) {
      tstate = N;
      rotate_by(0, STEP_VERTICAL);
      uprintf("State changed: HOME -> NORTH (pn:%d, ps:%d, pe:%d, pw:%d)\n", pn,
              ps, pe, pw);
    } else if (south) {
      tstate = S;
      rotate_by(0, STEP_VERTICAL * -1);
      uprintf("State changed: HOME -> SOUTH (pn:%d, ps:%d, pe:%d, pw:%d)\n", pn,
              ps, pe, pw);
    } else if (east) {
      tstate = E;
      rotate_by(STEP_HORIZONTAL * 2, STEP_VERTICAL);
      uprintf("State changed: HOME -> EAST (pn:%d, ps:%d, pe:%d, pw:%d)\n", pn,
              ps, pe, pw);
    } else if (west) {
      tstate = W;
      rotate_by(STEP_HORIZONTAL * -2, STEP_VERTICAL);
      uprintf("State changed: HOME -> WEST (pn:%d, ps:%d, pe:%d, pw:%d)\n", pn,
              ps, pe, pw);
    } else if (northeast) {
      tstate = NE;
      rotate_by(STEP_HORIZONTAL, STEP_VERTICAL);
      uprintf("State changed: HOME -> NORTHEAST (pn:%d, ps:%d, pe:%d, pw:%d)\n",
              pn, ps, pe, pw);
    } else if (northwest) {
      tstate = NW;
      rotate_by(STEP_HORIZONTAL * -1, STEP_VERTICAL);
      uprintf("State changed: HOME -> NORTHWEST (pn:%d, ps:%d, pe:%d, pw:%d)\n",
              pn, ps, pe, pw);
    } else if (southwest) {
      tstate = SW;
      rotate_by(STEP_HORIZONTAL, STEP_VERTICAL * -1);
      uprintf("State changed: HOME -> SOUTHWEST (pn:%d, ps:%d, pe:%d, pw:%d)\n",
              pn, ps, pe, pw);
    } else if (southeast) {
      tstate = SE;
      rotate_by(STEP_HORIZONTAL * -1, STEP_VERTICAL * -1);
      uprintf("State changed: HOME -> SOUTHEAST (pn:%d, ps:%d, pe:%d, pw:%d)\n",
              pn, ps, pe, pw);
    } else {
      tstate = HOME;
      uprintf("%s:%d - FSM Error\n\n", __FILE__, __LINE__);
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
    }
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
    uprintf("%s:%d - Automatic FSM Warning\n\n", __FILE__, __LINE__);
    break;
  }
}

void tick_joystick() {
  const uint16_t stick_down = ADC_read16b(ADC0_SE23_DAC0_OUT) < 1000;
  if (stick_down) {
    tstate = HOME;
    manual_mode = !manual_mode;
    SW_DELAY(100000);
  }
}

void tick_manual() {
  uint16_t stick_x = ADC_read16b(ADC0_SE12_PTB2);
  uint16_t stick_y = ADC_read16b(ADC0_SE13_PTB3);

  if (stick_x > HIGH_THRESHOLD) {
    rotate_one_tick(COUNTER_CLOCKWISE, NOT_MOVING); // Clockwise
  } else if (stick_x < LOW_THRESHOLD) {
    rotate_one_tick(CLOCKWISE, NOT_MOVING); // Counterclockwise
  }
  if (stick_y > HIGH_THRESHOLD) {
    rotate_one_tick(NOT_MOVING, CLOCKWISE);
  } else if (stick_y < LOW_THRESHOLD) {
    rotate_one_tick(NOT_MOVING, COUNTER_CLOCKWISE);
  }
}

#endif