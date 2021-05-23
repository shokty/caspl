all: exec

libs: c-lib

c-lib:LineParser.c LineParser.h
	gcc -g -m32 -Wall -c -o LineParser.o LineParser.c
	
exec: libs task4.c task4.h
	gcc -g -m32 -Wall -c -o task4.o task4.c
	gcc -g -m32 -Wall -o task4 task4.o LineParser.o
	rm task4.o LineParser.o

.PHONY: clean
clean:
	rm -rf task4