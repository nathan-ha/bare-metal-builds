# Project name
TARGET = firmware

# Directories
BUILD_DIR = build
SRC_DIR = src
INC_DIR = inc

# Toolchain definitions
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size

# MCU settings
MCU = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

# AS defines
AS_DEFS =

# C defines
C_DEFS = \
-DCPU_MK64FN1M0VLL12

# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES = \
-I$(INC_DIR) \
-I$(INC_DIR)/MK64F12/

# Compile flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) -lc -lm -lnosys -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# Default optimization level
OPT = -Os

# Source files
C_SOURCES = \
$(SRC_DIR)/main.c \
$(SRC_DIR)/syscalls.c \
$(SRC_DIR)/system_MK64F12.c

ASM_SOURCES = \
$(INC_DIR)/startup_MK64F12.s

# Linker script
LDSCRIPT = $(INC_DIR)/MK64FN1M0xxx12_flash.ld

# Object files
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))

# Default rules
all: directories $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

directories:
	@mkdir -p $(BUILD_DIR)

# Linker
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s
	$(AS) -c $(ASFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(INC_DIR)/%.s
	$(AS) -c $(ASFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(CP) -O ihex $< $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(CP) -O binary -S $< $@

flash: $(BUILD_DIR)/$(TARGET).bin
	@echo "Flashing with OpenSDA interface..."
	openocd -f interface/opensda.cfg \
		-f board/nxp_frdm-k64f.cfg \
		-c "program $(BUILD_DIR)/$(TARGET).bin verify reset exit"


clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean flash directories
