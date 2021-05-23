all: exec

libs: c-lib

c-lib:LineParser.c LineParser.h
	gcc -g -m32 -Wall -c -o LineParser.o LineParser.c
	
exec: libs task2.c task2.h
	gcc -g -m32 -Wall -c -o task2.o task2.c
	gcc -g -m32 -Wall -o task2 task2.o LineParser.o
	rm task2.o LineParser.o

.PHONY: clean
clean:
	rm -rf task2