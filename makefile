# Makefile (minimal, clean)

CC      := gcc
CFLAGS  := -Wall -Wextra -std=c99 -O2 -Iinclude
SRC     := src/bit.c src/csv.c src/list.c src/print.c src/read.c src/row.c src/search.c src/utils.c src/main.c
SRC_P   := src/patricia.c

# Separate build dirs to avoid mixing objects compiled with/without ENABLE_PATRICIA
BUILD1  := build1
BUILD2  := build2

OBJ1 := $(patsubst src/%.c,$(BUILD1)/%.o,$(SRC))
OBJ2 := $(patsubst src/%.c,$(BUILD2)/%.o,$(SRC))
OBJ2_P := $(BUILD2)/patricia.o

.PHONY: all clean
all: dict1 dict2

# Stage 1 (NO Patricia)
dict1: $(OBJ1)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD1)/%.o: src/%.c | $(BUILD1)
	$(CC) $(CFLAGS) -UENABLE_PATRICIA -c $< -o $@

# Stage 2 (WITH Patricia)
dict2: $(OBJ2) $(OBJ2_P)
	$(CC) $(CFLAGS) -DENABLE_PATRICIA -o $@ $^

$(BUILD2)/%.o: src/%.c | $(BUILD2)
	$(CC) $(CFLAGS) -DENABLE_PATRICIA -c $< -o $@

$(OBJ2_P): $(SRC_P) | $(BUILD2)
	$(CC) $(CFLAGS) -DENABLE_PATRICIA -c $< -o $@

$(BUILD1) $(BUILD2):
	mkdir -p $@

clean:
	rm -rf $(BUILD1) $(BUILD2) dict1 dict2 output.txt
