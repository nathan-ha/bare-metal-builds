# Compiler and Tools
CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# Directories
CMSIS_DIR = path/to/CMSIS
DEVICE_HEADERS_DIR = path/to/device_headers
SRC_DIR = .
BUILD_DIR = build

# Source files
SRC = $(SRC_DIR)/main.c $(SRC_DIR)/syscalls.c
OBJ = $(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Output files
TARGET = main.elf
HEXCOPY = $(BUILD_DIR)/main.hex

# Flags
CFLAGS = -mcpu=cortex-m4 -mthumb -I$(CMSIS_DIR) -I$(DEVICE_HEADERS_DIR) -Wall $(EXTRA_CFLAGS)
LDFLAGS = -T STM32F303RETX_FLASH.ld -nostartfiles -nostdlib --specs nano.specs -lc -lgcc -Wl,--gc-sections -Wl,-Map=$@.map

EXTRA_CFLAGS = -W -Wall -Wextra -Wundef -Wshadow -Wdouble-promotion \
               -Wformat-truncation -fno-common -Wconversion \
               -g3 -Os -ffunction-sections -fdata-sections -I. \
               -mfloat-abi=hard -mfpu=fpv4-sp-d16 # -Werror

# Rules
all: $(TARGET) $(HEXCOPY)

$(TARGET): $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(HEXCOPY): $(TARGET)
	$(OBJCOPY) -O ihex $(TARGET) $(HEXCOPY)

flash: $(TARGET)
	st-flash --reset write $(TARGET) 0x8000000

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(HEXCOPY)
	rm -f $(OBJ) *.map

.PHONY: all clean flash
