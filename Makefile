# Variables de compilador
CC = arm-none-eabi-gcc

CFLAGS = -I./BOARD -I./drivers -I./include -I./utilities -O0 -Wall -mthumb -mcpu=cortex-m0plus -DCPU_MKL46Z128VLH4
LDFLAGS = -O2 -Wall -Wextra -mthumb -mcpu=cortex-m0plus --specs=nosys.specs -Wl,--gc-sections,-Map=main.map -TMKL46Z256xxx4_flash.ld

OBJ_DIR = objects

STARTUP = $(OBJ_DIR)/startup_MKL46Z4.o

vpath %.c drivers utilities BOARD include

SRCS = $(notdir $(wildcard drivers/*.c utilities/*.c BOARD/*.c include/*.c))

OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(SRCS))) $(OBJ_DIR)/main.o $(STARTUP)

TARGET = main.elf

all: $(TARGET)

$(OBJ_DIR)/startup_MKL46Z4.o: startup_MKL46Z4.S
	@echo "Compilando startup $< ..."
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: %.c
	@echo "Compilando $< ..."
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	@echo "Linkando $(TARGET) ..."
	@$(CC) $(LDFLAGS) -o $@ $^

flash: $(TARGET)
	openocd -f openocd.cfg -c "program $(TARGET) verify reset exit"

clean:
	rm -rf $(OBJ_DIR)/*.o $(TARGET) main.map
