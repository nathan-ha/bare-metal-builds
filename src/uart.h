#ifndef __UART_H__
#define __UART_H__

#include "fsl_device_registers.h"
#include <stdarg.h>
#include <stdint.h>

// source: https://github.com/LukeBoudreau/FreescaleCar/blob/master/uart.c
#define BAUD_RATE 9600 // default baud rate
#define SYS_CLOCK 20485760 // default system clock (see DEFAULT_SYSTEM_CLOCK in system_MK64F12.c)

void UART0_init() {
  // define variables for baud rate and baud rate fine adjust
  uint16_t ubd, brfa;
  uint8_t ubdlow, ubdhigh;

  // Enable clock for UART
  SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;
  SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;

  // Configure the port control register to alternative 3 (which is UART mode
  // for K64)
  PORTB_PCR17 = PORT_PCR_MUX(3);
  PORTB_PCR16 = PORT_PCR_MUX(3);

  /*Configure the UART for establishing serial communication*/

  // Disable transmitter and receiver until proper settings are chosen for the
  // UART module
  UART0_C2 &= ~(UART_C2_TE_MASK);
  UART0_C2 &= ~(UART_C2_RE_MASK);

  // Select default transmission/reception settings for serial communication of
  // UART by clearing the control register 1
  UART0_C1 = 0x00;

  // UART Baud rate is calculated by: baud rate = UART module clock / (16 ×
  // (SBR[12:0] + BRFD)) 13 bits of SBR are shared by the 8 bits of UART3_BDL
  // and the lower 5 bits of UART3_BDH BRFD is dependent on BRFA, refer Table
  // 52-234 in K64 reference manual BRFA is defined by the lower 4 bits of
  // control register, UART3_C4

  // calculate baud rate settings: ubd = UART module clock/16* baud rate
  ubd = (uint16_t)((SYS_CLOCK) / (BAUD_RATE * 16));

  // clear SBR bits of BDH
  UART0_BDH &= ~(UART_BDH_SBR_MASK);

  // distribute this ubd in BDH and BDL
  ubdlow = (uint8_t)ubd;
  ubdhigh = (uint8_t)(ubd >> 8);

  UART0_BDH |= ubdhigh;
  UART0_BDL |= ubdlow;

  // BRFD = (1/32)*BRFA
  // make the baud rate closer to the desired value by using BRFA
  brfa = (((SYS_CLOCK * 32) / (BAUD_RATE * 16)) - (ubd * 32));

  // write the value of brfa in UART3_C4
  UART0_C4 &= ~(UART_C4_BRFA_MASK);
  UART0_C4 = UART_C4_BRFA(brfa);

  // Enable transmitter and receiver of UART
  UART0_C2 |= UART_C2_TE_MASK;
  UART0_C2 |= UART_C2_RE_MASK;
}

void uputc(char ch) {
  // Wait until transmit data register is empty
  while (!(UART0_S1 & UART_S1_TDRE_MASK));
  // Send the character
  UART0_D = ch;
}

void uprintf(const char *format, ...) {
  va_list args; // List to hold variable arguments
  va_start(args, format);

  while (*format) {
    if (*format == '%') {
      format++; // Move past '%'
      switch (*format) {
      case 'c': {                   // Character
        char c = va_arg(args, int); // Fetch next argument as a character
        uputc(c);
        break;
      }
      case 's': { // String
        char *str = va_arg(args, char *);
        while (*str) {
          uputc(*str++);
        }
        break;
      }
      case 'd': { // Decimal integer
        int num = va_arg(args, int);
        if (num < 0) {
          uputc('-');
          num = -num;
        }
        char buffer[10]; // Buffer to hold the number
        int i = 0;
        do {
          buffer[i++] = (num % 10) + '0'; // Convert digit to character
          num /= 10;
        } while (num);
        while (i--) {
          uputc(buffer[i]); // Print the number in reverse
        }
        break;
      }
      default:
        uputc('%'); // Handle unknown format specifier
        uputc(*format);
        break;
      }
    } else {
      if (*format == '\n')
        uputc('\r');
      uputc(*format); // Print normal characters
    }
    format++;
  }

  va_end(args);
}
#endif
