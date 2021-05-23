all: exec

libs: c-lib

c-lib:LineParser.c LineParser.h
	gcc -g -m32 -Wall -c -o LineParser.o LineParser.c
	
exec: libs task1.c task1.h
	gcc -g -m32 -Wall -c -o task1.o task1.c
	gcc -g -m32 -Wall -o task1 task1.o LineParser.o
	rm task1.o LineParser.o

.PHONY: clean
clean:
	rm -rf task1