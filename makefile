CC      := gcc
CFLAGS  := -Wall -Wextra -std=c99 -O2 -Iinclude

SRC_COMMON := src/bit.c src/csv.c src/list.c src/print.c src/read.c src/row.c src/search.c src/utils.c
BUILD      := build

OBJ_COMMON := $(patsubst src/%.c,$(BUILD)/%.o,$(SRC_COMMON))
OBJ_MAIN_S1 := $(BUILD)/main.s1.o
OBJ_MAIN_S2 := $(BUILD)/main.s2.o
OBJ_PATRICIA := $(BUILD)/patricia.o

.PHONY: all clean
all: dict1 dict2

dict1: $(OBJ_COMMON) $(OBJ_MAIN_S1)
	$(CC) $(CFLAGS) -o $@ $^

dict2: $(OBJ_COMMON) $(OBJ_MAIN_S2) $(OBJ_PATRICIA)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD)/%.o: src/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_MAIN_S1): src/main.c | $(BUILD)
	$(CC) $(CFLAGS) -UENABLE_PATRICIA -c $< -o $@

$(OBJ_MAIN_S2): src/main.c | $(BUILD)
	$(CC) $(CFLAGS) -DENABLE_PATRICIA -c $< -o $@

$(OBJ_PATRICIA): src/patricia.c | $(BUILD)
	$(CC) $(CFLAGS) -DENABLE_PATRICIA -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD) dict1 dict2 output.txt

