all: lines3d

lines3d: mat4.o parse_util.o trans.o lines3d.o
	gcc -g -Wall mat4.o parse_util.o trans.o lines3d.o -lm -o lines3d

mat4.o: mat4.c mat4.h
	gcc -g -Wall -c mat4.c -o mat4.o

parse_util.o: parse_util.c parse_util.h
	gcc -g -Wall -c parse_util.c -o parse_util.o

trans.o: trans.c trans.h
	gcc -g -Wall -c trans.c -o trans.o

lines3d.o: lines3d.c 
	gcc -g -Wall -c lines3d.c -o lines3d.o

clean:
	rm *.o
	rm lines3d
