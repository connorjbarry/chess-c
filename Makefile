CC = gcc
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --libs)

CFLAGS = -Wall -Wextra -pedantic -std=c11 -O2 $(SDL_CFLAGS) -Iinclude
LDFLAGS = $(SDL_LDFLAGS) -lSDL2_image -lSDL2_ttf

SRC_DIR = src
BUILD_DIR = bin

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

TARGET = $(BUILD_DIR)/cengine

.PHONY: all clean run

all: $(TARGET)

# Link the final executable
$(TARGET): $(OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CC) -o $@ $^ $(LDFLAGS)

# Compile each .c to .o file
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) $(ARGS)

clean:
	rm -f $(BUILD_DIR)/*.o $(TARGET)

