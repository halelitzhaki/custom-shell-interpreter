default: all

all: myshell

myshell: myshell.o
	gcc -Wall -g -o myshell myshell.o

myshell.o: myshell.c source.h
	gcc -c -g myshell.c

	
clean:
	rm *.o myshell
