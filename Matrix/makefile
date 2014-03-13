all: lines3d

lines3d: mat4.o parse_util.o trans.o lines3d.o
	gcc -g -02 -Wall mat4.o parse_util.o trans.o lines3d.o -lm -o lines3d

mat4.o: mat4.c mat4.h
	gcc -g -02 -Wall -c mat4.c -o mat4.o

parse_util.o: parse_util.c parse_util.h
	gcc -g -02 -Wall -c parse_util.c -o parse_util.o

trans.o: trans.c trans.h
	gcc -g -02 -Wall -c trans.c -o trans.o

lines3d.o: lines3d.c 
	gcc -g -02 -Wall -c lines3d.c -o lines3d.o

clean:
	rm *.o
	rm lines3d
