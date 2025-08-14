# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -O2
DEBUG_FLAGS = -g -DDEBUG

# Project files
SRC = aes.c
TARGET = aes
DEBUG_TARGET = $(TARGET)_debug

# Default target
all: $(TARGET)

# Main target
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

# Debug build
debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(DEBUG_TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean build files
clean:
	rm -f $(TARGET) $(DEBUG_TARGET)

.PHONY: all debug run clean
