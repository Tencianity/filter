filter: filter.c helpers.c
	gcc filter.c helpers.c -ggdb3 -O0 -std=c11 -Wall -lm -o filter