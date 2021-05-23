#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

void pmenu(bool printnow,int sizeofarry);

char encrypt(char c); /* Gets a char c and returns its encrypted form by adding 3 to its value.
          If c is not between 0x20 and 0x7E it is returned unchanged */
char decrypt(char c); /* Gets a char c and returns its decrypted form by reducing 3 to its value.
            If c is not between 0x20 and 0x7E it is returned unchanged */
char dprt(char c); /* dprt prints the value of c in a decimal representation followed by a
           new line, and returns c unchanged. */
char cprt(char c); /* If c is a number between 0x20 and 0x7E, cprt prints the character of ASCII value c followed
                    by a new line. Otherwise, cprt prints the dot ('.') character. After printing, cprt returns
                    the value of c unchanged. */
char my_get(char c); /* Ignores c, reads and returns a character from stdin using fgetc. */

char quit(char c);

char censor(char c) {
    if (c == '!')
        return '.';
    else
        return c;
}

char *map(char *array, int array_length, char (*f)(char)) {
    char *mapped_array = (char *) (malloc(array_length * sizeof(char)));
    for (int i = 0; i < array_length; i++) {
        mapped_array[i] = f(array[i]);
    }
    return mapped_array;
}

struct menu {
    char *name;
    char (*fun)(char);
};

struct menu a[8] = {{"Censor",       censor},
                    {"Encrypt",      encrypt},
                    {"Decrypt",      decrypt},
                    {"Print dec",    dprt},
                    {"Print string", cprt},
                    {"Get string",   my_get},
                    {"Quit",         quit},
                    {NULL,NULL}};




int main(int argc, char **argv) {

    char commends[5];
    char *tosave = commends;

    int size = 0;

    while(a[size].name != NULL)
        size++;

    FILE *input = stdin;
    char c;
    bool print = false;
    pmenu(true,size);

    while (1) {
        c = fgetc(input);
        if ((c != EOF && c != '\n')) {
            print = false;
            if (c - 48 >= 0 && c - 48 <= 6) {
                printf("%s \n", "Within bounds");
                tosave = map(tosave, 5, a[c - 48].fun);
                printf("%s \n \n", "DONE.");
                print = true;
            }
            else{
                printf("%s %c \n", "not in bound / incurrect input:  ", c);
                exit(0);
            }
        }
        else if (c == '\n')
            pmenu(print,size);
    }
}


void pmenu(bool printnow,int sizeofarry) {
    if (printnow) {
        printf("%s", "Please choose a function:\n");
        for(int i=0; i <= sizeofarry-1; i++){
            printf("%i %s %s \n" , i ,") ", a[i].name);
        }
        printf("%s" , "option: ");
    }
}

char encrypt(char c) {
    if (0x7E >= c && c >= 0x20)
        return (c + 3);
    return c;
}

char decrypt(char c) {
    if (0x7E >= c && c >= 0x20)
        return (c - 3);
    return c;
}

char dprt(char c) {
    printf("%d\n", c);
    return c;
}

char cprt(char c) {
    if (0x7E >= c && c >= 0x20)
        printf("%c\n", c);
    else
        printf("%c\n", '.');
    return c;
}

char my_get(char c) {
    return fgetc(stdin);
}

char quit(char c) {
    if (c == 'q')
        exit(0);
    return c;
}
