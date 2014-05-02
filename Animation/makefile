all: animate

animate: mat4.o parse_util.o trans.o animate.o line.o
	gcc -g -Wall mat4.o parse_util.o trans.o animate.o line.o -lm -o animate

mat4.o: mat4.c mat4.h
	gcc -g -Wall -c mat4.c -o mat4.o

parse_util.o: parse_util.c parse_util.h
	gcc -g -Wall -c parse_util.c -o parse_util.o

trans.o: trans.c trans.h
	gcc -g -Wall -c trans.c -o trans.o

animate.o: animate.c line.h
	gcc -g -Wall -c animate.c -o animate.o

line.o: drawline.c line.h
	gcc -g -Wall -c lines3d.c -o line.o

clean:
	rm *.o
	rm animate
