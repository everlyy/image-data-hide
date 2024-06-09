NAME := imgdat

CC := gcc
CFLAGS := -Wall -Wextra -Werror -std=c2x
LIBS := -lm

SOURCES_DIR := src
BUILD_DIR := .build

OBJECTS := main.o imgdat.o stb_image.o stb_image_write.o
SOURCES := $(OBJECTS:%.o=$(SOURCES_DIR)/%.c)
OBJECTS := $(OBJECTS:%=$(BUILD_DIR)/%)

all: $(NAME)

$(NAME): $(OBJECTS)
	@$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	$(info LD	$@)

$(BUILD_DIR)/%.o: $(SOURCES_DIR)/%.c
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c -o $@ $^
	$(info CC	$@)

clean:
	@rm -rf $(BUILD_DIR)
	$(info RM	$(BUILD_DIR))
	@rm -f $(NAME)
	$(info RM	$(NAME))
