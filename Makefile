# Compiler and flags
CC = gcc
CFLAGS = -Og -Wall -Wno-missing-braces -Iinclude
LDFLAGS = -Llib/raylib -lraylib -lopengl32 -lgdi32 -lwinmm

# Target executable
TARGET = main.exe

# Source files
SRC = main.c

# Default target
all: $(TARGET)

# Compile target
$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

# Clean build files
clean:
	rm -f $(TARGET)
