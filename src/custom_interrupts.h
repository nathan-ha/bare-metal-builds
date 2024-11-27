#ifndef __CUSTOM_INTERRUPTS_H__
#define __CUSTOM_INTERRUPTS_H__

#include "fsl_device_registers.h"
#include "movement_logic.h"
#include "uart.h"

#define SYSTEM_CLOCK_FREQ_MHZ 48
#define DESIRED_INTERRUPT_FREQ_HZ 100

void init_custom_interrupts() {
  // Clock gating
  SIM_SCGC3 |= SIM_SCGC3_FTM3_MASK;

  // Calculate timer period
  uint32_t clock_freq = SYSTEM_CLOCK_FREQ_MHZ * 1000000;
  uint32_t prescaler = 64;
  uint32_t timer_clock = clock_freq / prescaler;
  uint32_t mod_value = (timer_clock / DESIRED_INTERRUPT_FREQ_HZ) - 1;

  // FTM3 configurations
  FTM3_MODE =
      FTM_MODE_WPDIS_MASK | FTM_MODE_FTMEN_MASK; // Enhanced configuration
  FTM3_MOD = mod_value; // Set counter modulo for desired frequency

  // Configure timer
  FTM3_SC = (FTM_SC_CLKS(1)     // System clock
             | FTM_SC_PS(6)     // Prescaler of 64 
             | FTM_SC_TOIE_MASK // Timer overflow interrupt enable
  );

  FTM3_SC &= ~FTM_SC_TOF_MASK;

  NVIC_EnableIRQ(FTM3_IRQn);
}

void FTM3_IRQHandler(void) {
  volatile uint32_t SC_VAL = FTM3_SC;
	FTM3_SC &= 0x7F; // clear TOF

  tick_joystick();
}

#endif
