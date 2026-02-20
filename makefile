filter: filter.c helpers.c
	gcc -ggdb3 -O0 -std=c11 -Wall -Werror -lm -o filter filter.c helpers.c