SRC_DIR := src
INC_DIR := include
OBJ_DIR := obj

CC := gcc
CFLAGS := -Wall -Wextra -pedantic -O2
INCLUDES := -I$(INC_DIR)

SRCS := $(wildcard $(SRC_DIR)/*.c)

OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

all: $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

test: test.c $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(OBJS) test.c

clean:
	rm -rf $(OBJ_DIR)
	rm -rf test

.PHONY: all clean
