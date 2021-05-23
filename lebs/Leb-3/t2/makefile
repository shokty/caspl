#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: task2b

# Tool invocations
# task2butable "task2b" depends on the files task2b.o and run.o.
task2b: task2b.o
	gcc -m32 -g -Wall -o task2b task2b.o

# Depends on the source and header files
task2b.o: task2b.c
	gcc -g -Wall -m32  -c -o task2b.o task2b.c 


#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o task2b

