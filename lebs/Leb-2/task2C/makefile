#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: menu

# Tool invocations
# menuutable "menu" depends on the files menu.o and run.o.
menu: menu_map.o
	gcc -m32 -g -Wall -o menu menu_map.o

# Depends on the source and header files
menu_map.o: menu_map.c
	gcc -g -Wall -m32  -c -o menu_map.o menu_map.c 


#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o menu

