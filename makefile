CC = gcc

INC_DIR = inc
SRC_DIR = src
SRCS = $(wildcard $(SRC_DIR)/*.c)
INCS = $(wildcard $(INC_DIR)/*.h)

EXEC = filter

CFLAGS = -g -I $(INC_DIR) -std=c23 -Wall -lm

$(EXEC): $(SRCS) $(INCS)
	$(CC) $(CFLAGS) $(SRCS) -o filter
