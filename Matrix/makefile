all: trans

trans: mat4.o parse_util.o trans.o line.o
	gcc -g -02 -Wall mat4.o parse_util.o trans.o line.o -lm -o trans

mat4.o: mat4.c mat4.h
	gcc -g -02 -Wall -c mat4.c -o mat4.o

parse_util.o: parse_util.c parse_util.h
	gcc -g -02 -Wall -c parse_util.c -o parse_util.o

trans.o: trans.c trans.h
	gcc -g -02 -Wall -c trans.c -o trans.o

line.o: line.c 
	gcc -g -02 -Wall -c line.c -o line.o

clean:
	rm *.o
	rm trans
