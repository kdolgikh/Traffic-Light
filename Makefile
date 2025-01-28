CC = gcc
CFLAGS = -Wall -Wextra -O2
DEBUG_FLAGS = -g -DDEBUG

TARGET = traffic_controller
SRC_DIR = src
INC_DIR = inc
BUILD_DIR = build
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

.PHONY: all clean debug

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(OBJS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

debug: CFLAGS += $(DEBUG_FLAGS)
debug: all

clean:
	rm -rf $(BUILD_DIR)
