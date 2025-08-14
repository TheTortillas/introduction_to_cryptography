# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -O2
DEBUG_FLAGS = -g -DDEBUG

# Project files
SRC1 = hillCypher.c
SRC2 = hillModesImage.c
TARGET1 = hillCypher
TARGET2 = hillModesImage
TARGETS = $(TARGET1) $(TARGET2)

# Default target - builds both programs
all: $(TARGETS)

# Individual targets
$(TARGET1): $(SRC1)
	$(CC) $(CFLAGS) $(SRC1) -o $(TARGET1)

$(TARGET2): $(SRC2)
	$(CC) $(CFLAGS) $(SRC2) -o $(TARGET2)

# Build specific program
hill: $(TARGET1)

modes: $(TARGET2)

# Run programs
run-hill: $(TARGET1)
	./$(TARGET1)

run-modes: $(TARGET2)
	./$(TARGET2)

# Run both (you can choose which one when prompted)
run: all
	@echo "Available programs:"
	@echo "1. hillCypher"
	@echo "2. hillModesImage"
	@echo "Use 'make run-hill' or 'make run-modes' to run specific program"

# Clean build files
clean:
	rm -f $(TARGETS)

.PHONY: all hill modes run run-hill run-modes clean


# Compilar ambos programas: 
# make

# Ejecutar hillCypher:
# make run-hill

# Ejecutar hillModesImage:
# make run-modes

# Limpiar ejecutables cuando termines:
# make clean
