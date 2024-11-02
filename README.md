# All in one thing to build code (no HAL) for an stm32

The main.c code, linker script (.ld), and makefile (C_DEFS) are currently targeted for the stm32f303re. 

Also, all of the includes are currently just for stm32f3. If you have an f4 or something, google the header files (should be from cmsis github) and paste them in here.

Instructions:
In the terminal, run:
```py
make # builds the project (make sure you have Arm Embedded Toolchain installed)
make flash # flashes it (using st-link)
```