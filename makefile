CC = gcc

INC_DIR = inc
SRC_DIR = src
SRCS = $(wildcard $(SRC_DIR)/*.c)
INCS = $(wildcard $(INC_DIR)/*.h)

EXEC = filter
CFLAGS1 = -g -O1 -I $(INC_DIR) -std=c23 -Wall -lm -lz
CFLAGS2 = -g -I $(INC_DIR) -std=c23 -Wall -lm -lz

$(EXEC): $(SRCS) $(INCS)
	$(CC) $(CFLAGS1) $(SRCS) -o $(EXEC)

safe: $(SRCS) $(INCS)
	$(CC) $(CFLAGS2) $(SRCS) -o $(EXEC)-safe
