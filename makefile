CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -O2 -Iinclude

SRC_DIR = src
OBJ_DIR = build

COMMON_SRCS = \
  bit.c \
  csv.c \
  list.c \
  print.c \
  read.c \
  row.c \
  search.c \
  utils.c \
  main.c

DICT1_SRCS = $(COMMON_SRCS)
DICT2_SRCS = $(COMMON_SRCS) patricia.c

COMMON_SRCS_FULL = $(addprefix $(SRC_DIR)/,$(COMMON_SRCS))
DICT1_SRCS_FULL  = $(addprefix $(SRC_DIR)/,$(DICT1_SRCS))
DICT2_SRCS_FULL  = $(addprefix $(SRC_DIR)/,$(DICT2_SRCS))

DICT1_OBJS = $(DICT1_SRCS_FULL:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DICT2_OBJS = $(DICT2_SRCS_FULL:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: dict2 dict1

dict1: $(DICT1_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Enable Patricia for dict2
dict2: CFLAGS += -DENABLE_PATRICIA
dict2: $(DICT2_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR) dict1 dict2 output.txt

.PHONY: all clean