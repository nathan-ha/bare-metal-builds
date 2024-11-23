#include "fsl_device_registers.h"
#include "photoresistor.h"
#include "stepper.h"
#include "adc.h"
#include "uart.h"
#include "delay.h"
#include "movement_logic.h"

void setup() {
  UART0_init();
  ADC_init();
  stepper_init();
  photoresistor_init();
  timer_init();

  PORTE_GPCLR |= 0x00000100 | (1 << 26); // gpio on PTE26     
  GPIOE_PDDR |= (1 << 26); // pin 26 output (LED for manual mode)        

}

void tick() {
  tick_joystick();
  if (manual_mode) {
    tick_manual();
    GPIOE_PDOR |= (1 << 26);
    SW_DELAY(SW_DELAY_TICKS);
  } else {
    GPIOE_PDOR &= ~(1 << 26);
    tick_automatic();
  }
}

int main() {

  setup();

  for (;;) {
    // test 1 -- stepper rotating one tick
    rotate_one_tick(2, 2);
    // rotate_one_tick(2, 0);
    // rotate_one_tick(0, 2);
    // rotate_one_tick(0, 0);
    // rotate_one_tick(0, 1);
    // rotate_one_tick(1, 0);
    // rotate_one_tick(1, 1);

    // test 2 -- two rotations at the same time
    // rotate_by(0, 0);
    // rotate_by(1, 1);
    // rotate_by(1, -1);
    // rotate_by(-1, -1);

    // test 3
    // manual mode test -- should be default


    // last test -- actual thingy
    tick();


    SW_DELAY(SW_DELAY_TICKS);
  }
}
