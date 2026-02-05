CC      ?= gcc
CFLAGS  ?= -Wall -Wextra -O1 -g
LDFLAGS ?= 

TARGET_EXEC ?= main
BUILD_DIR   ?= ./build
SRC_DIRS    ?= ./src

MKDIR_P ?= mkdir -p

# Find all source files recursively
SRCS := $(shell find $(SRC_DIRS) -name "*.c")

# Flatten object files into BUILD_DIR
OBJS := $(patsubst $(SRC_DIRS)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

# Include directories
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CPPFLAGS ?= $(INC_FLAGS) -MMD -MP

.PHONY: all clean
all: $(BUILD_DIR)/$(TARGET_EXEC)

# Link target
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Compile C files into flattened object files
$(BUILD_DIR)/%.o: $(SRC_DIRS)/%.c
	@$(MKDIR_P) $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Clean build directory
clean:
	@if [ -d "$(BUILD_DIR)" ]; then rm -rf "$(BUILD_DIR)"; fi

# Include generated dependency files
-include $(DEPS)
