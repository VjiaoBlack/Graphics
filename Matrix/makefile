all: trans

trans: mat4.o parse_util.o trans.o
	gcc -g -02 -Wall mat4.o parse_util.o trans.o -lm -o trans

mat4.o: mat4.c mat4.h
	gcc -g -02 -Wall -c mat4.c -o mat4.o

parse_util.o: parse_util.c parse_util.h
	gcc -g -02 -Wall -c parse_util.c -o parse_util.o

trans.o: trans.c
	gcc -g -02 -Wall -c trans.c -o trans.o
