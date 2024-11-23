#ifndef __DELAY_H__
#define __DELAY_H__

void timer_init() {
  SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK; 
  SIM_SCGC3 |= SIM_SCGC3_FTM3_MASK;  // FTM3 clock enable
  FTM3_MODE = 0x5;                   // Enable FTM3
  FTM3_MOD = 0xFFFF;
  FTM3_SC = 0x0E; // System clock / 64
}

void delayms(int k) {
  FTM3_C6SC = 0x1C;          // Output-compare; Set output
  FTM3_C6V = FTM3_CNT + 333; // 1 ms
  for (int i = 0; i < k; k++) {
    while (!(FTM3_C6SC & 0x80))
      ;
    FTM3_C6SC &= ~(1 << 7);
    FTM3_C6V = FTM3_CNT + 333; // 1 ms
  }
  FTM3_C6SC = 0; // Disable channel
}

#endif