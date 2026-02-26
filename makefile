filter: filter.c helpers.c bmp.h
	gcc filter.c helpers.c -ggdb3 -O0 -std=c11 -Wall -lm -o filter