all: objects3d

objects3d: mat4.o parse_util.o trans.o objects3d.o line.o
	gcc -g -Wall mat4.o parse_util.o trans.o objects3d.o line.o -lm -o objects3d

mat4.o: mat4.c mat4.h
	gcc -g -Wall -c mat4.c -o mat4.o

parse_util.o: parse_util.c parse_util.h
	gcc -g -Wall -c parse_util.c -o parse_util.o

trans.o: trans.c trans.h
	gcc -g -Wall -c trans.c -o trans.o

objects3d.o: objects3d.c line.h
	gcc -g -Wall -c objects3d.c -o objects3d.o

line.o: drawline.c line.h
	gcc -g -Wall -c lines3d.c -o line.o

clean:
	rm *.o
	rm objects3d
