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

# ST-LINK utilities
STFLASH = st-flash
STUTIL = st-util
STINFO = st-info

# MCU settings
MCU = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

# AS defines
AS_DEFS =

# C defines
C_DEFS = \
-DSTM32F303xE

# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES = \
-I$(INC_DIR)

# Compile flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) -lc -lm -lnosys -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# Default optimization level
OPT = -Os

# Source files
C_SOURCES = \
$(SRC_DIR)/main.c \
$(INC_DIR)/system_stm32f3xx.c

ASM_SOURCES = \
$(INC_DIR)/startup_stm32f303xe.s

# Linker script
LDSCRIPT = $(INC_DIR)/STM32F303RETX_FLASH.ld

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

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(CP) -O ihex $< $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(CP) -O binary -S $< $@

# Pattern rules for building objects
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(INC_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s
	$(AS) -c $(ASFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(INC_DIR)/%.s
	$(AS) -c $(ASFLAGS) $< -o $@

# Flash the device
flash: $(BUILD_DIR)/$(TARGET).bin
	@echo "Flashing $(TARGET).bin to STM32F303RE..."
	$(STFLASH) write $(BUILD_DIR)/$(TARGET).bin 0x8000000

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean flash directories