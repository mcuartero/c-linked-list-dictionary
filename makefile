CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -Iinclude

SRC := src
OBJ1 := build/dict1
OBJ2 := build/dict2

COMMON := bit.c csv.c list.c print.c read.c row.c search.c utils.c main.c
DICT1_SRCS := $(addprefix $(SRC)/,$(COMMON))
DICT2_SRCS := $(addprefix $(SRC)/,$(COMMON)) $(SRC)/patricia.c

DICT1_OBJS := $(patsubst $(SRC)/%.c,$(OBJ1)/%.o,$(DICT1_SRCS))
DICT2_OBJS := $(patsubst $(SRC)/%.c,$(OBJ2)/%.o,$(DICT2_SRCS))

all: dict1 dict2

dict1: $(DICT1_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Enable patricia.c
dict2: CFLAGS += -DENABLE_PATRICIA
dict2: $(DICT2_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ1)/%.o: $(SRC)/%.c | $(OBJ1)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ2)/%.o: $(SRC)/%.c | $(OBJ2)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ1) $(OBJ2):
	mkdir -p $@

clean:
	rm -rf build dict1 dict2 output.txt

.PHONY: all clean
