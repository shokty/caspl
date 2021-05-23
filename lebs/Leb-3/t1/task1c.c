#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void PrintLL();

typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    char *sig;
} virus;

typedef struct link link;

void list_print(link *virus_list, FILE *);

/* Print the data of every link in list to the given stream. Each item followed by a newline character. */

link *list_append(link *virus_list, virus *data);

/* Add a new link with the given data to the list
   (either at the end or the beginning, depending on what your TA tells you),
   and return a pointer to the list (i.e., the first link in the list).
   If the list is null - create a new entry and return a pointer to the entry. */

void list_free(link *virus_list); /* Free the memory allocated by the list. */


void printHEx(char *buffer, int leangth, FILE *output);

virus *readVirus(FILE *file);

void detect_virus(char *buffer, unsigned int size, link *virus_list);

void printVirus(virus *virus, FILE *output);

void pmenu(bool printnow, int sizeofarry);

void load();

void quit();

void detect();

void Fix();

void kill_virus(char *fileName, int signitureOffset, int signitureSize);

struct link {
    link *nextVirus;
    virus *vir;
};

char *infectedfile;

link *First = NULL;


struct menu {
    char *name;

    void (*fun)();
};

struct menu a[7] = {
        {"Load signatures",  load},
        {"Load signatures",  load},
        {"Print signatures", PrintLL},
        {"Detect viruses",   detect},
        {"Quit",             quit},
        {NULL, NULL}};

int main(int argc, char *argv[]) {
    int size = 0;
    while (a[size].name != NULL)
        size++;


    infectedfile = argv[1];

    FILE *input = stdin;
    char c;
    bool print = false;
    pmenu(true, size);

    while (1) {
        c = fgetc(input);
        if ((c != EOF && c != '\n')) {
            print = false;
            if (c - 48 >= 0 && c - 48 <= 6) {
                a[c - 48].fun();
                printf("%s \n \n", "DONE.");
                print = true;
            } else {
                printf("%s %c \n", "not in bound / incurrect input:  ", c);
            }
        } else if (c == '\n')
            pmenu(print, size);
    }

    return 0;
}

void load() {
    char c = fgetc(stdin);
    if (c == '\n')
        printf("%s \n", "Please enter Input File name:.");

    char t[450];
    scanf("%s", t);
    struct virus *try;
    link *Virus_Link_List = NULL;
    FILE *input = fopen(t, "r");
    if (input) {
        do {
            try = readVirus(input);
            if (try != NULL) {
                Virus_Link_List = list_append(Virus_Link_List, try);
            }
            // printVirus(try, stdout); //for me if i want to print
        } while ((try != NULL));
    }
    First = Virus_Link_List;
}

void Fix() {
//    char c1 = fgetc(stdin);
//    if (c1 == '\n')
//        printf("%s \n", "Please enter the name of the File you wish to Fix:");

    char *t1 = infectedfile;
    //  scanf("%s", t1);

    char c2 = fgetc(stdin);
    if (c2 == '\n')
        printf("%s \n", "Please enter the where is the virus located:");
    char t2[450];
    scanf("%s", t2);
    int signitureOffset = atoi(t2);

    char c3 = fgetc(stdin);
    if (c3 == '\n')
        printf("%s \n", "Please enter the where is the virus size:");
    char t3[450];
    scanf("%s", t3);
    int signitureSize = atoi(t3);
    FILE *input = fopen(t1, "r");
    if (input) {
        kill_virus(t1, signitureOffset, signitureSize);
    } else
        printf("%s %s", "No such FIle:   ", t1);
}

void kill_virus(char *fileName, int signitureOffset, int signitureSize) {
    FILE *InfectedFile = fopen(fileName, "r+");
    fseek(InfectedFile, signitureOffset, SEEK_SET);
    char *replacment = "0x90";
    fwrite(replacment, 1, signitureSize, InfectedFile);
    fclose(InfectedFile);
}

link *list_append(link *virus_list, virus *data) {
    link *start = virus_list;
    link *tmp = virus_list;
    link *toadd = calloc(1, sizeof(link));
    toadd->nextVirus = NULL;
    toadd->vir = data;

    if (start == NULL)
        return toadd;

    while (tmp->nextVirus != NULL)
        tmp = tmp->nextVirus;

    tmp->nextVirus = toadd;
    return start;
}


void printHEx(char *buffer, int leangth, FILE *output) {
    for (int i = 0; i < leangth; i++) {
        fprintf(output, "%02X %c", buffer[i] & 0XFF, ' ');
    }
    fprintf(output, "\n");
}

void PrintLL() {

    char c = fgetc(stdin);
    if (c == '\n')
        printf("%s \n", "Please enter output File name:");

    char t[450];
    scanf("%s", t);

    FILE *output;

    if (strcmp(t, "stdout") == 0)
        output = stdout;
    else
        output = fopen(t, "w");

    struct link temp = *First;
    if (output) {
        while (temp.nextVirus != NULL) {
            printVirus(temp.vir, output);
            temp = *temp.nextVirus;
        }
        printVirus(temp.vir, output);
    }
    fflush(output);
}

virus *readVirus(FILE *file) {
    virus *New_Virus = calloc(1, sizeof(virus));
    if (fread(New_Virus, 18, 1, file) == 0) {
        free(New_Virus);
        return NULL;
    }
    New_Virus->sig = calloc(1, New_Virus->SigSize);
    if (fread(New_Virus->sig, New_Virus->SigSize, 1, file) == 0) {
        free(New_Virus);
        return NULL;
    }
    return New_Virus;
}

void printVirus(virus *virus, FILE *output) {
    if (virus != NULL) {
        fprintf(output, "Virus name: %s \n", virus->virusName);
        fprintf(output, "Virus size: %d \n", virus->SigSize);
        fprintf(output, "Signature: \n");
        printHEx(virus->sig, virus->SigSize, output);
        fprintf(output, "\n");
    }
}

void pmenu(bool printnow, int sizeofarry) {
    if (printnow) {
        printf("%s", "Please choose a function:\n");
        for (int i = 1; i <= sizeofarry - 1; i++) {
            printf("%i %s %s \n", i, ") ", a[i].name);
        }
        printf("%s", "option: ");
    }
}

void quit() {
    list_free(First);
    exit(0);
}

void detect() {
    char c = fgetc(stdin);
    if (c == '\n')
        printf("%s \n", "Please enter the detect File name:.");

    char t[450];
    scanf("%s", t);

    FILE *input = fopen(t, "r");

    if (input) {
        char buffer[10000];
        size_t size = fread(buffer, sizeof(char), 10000, input);
        detect_virus(buffer, size, First);
    } else
        printf("%s %s", "No such FIle:   ", t);
}

void list_free(link *virus_list) {
    if (virus_list) {
        if (virus_list->nextVirus != NULL) {
            list_free(virus_list->nextVirus);
            free(virus_list->nextVirus);
        }
        free(virus_list->vir->sig);
        free(virus_list->vir);
    }
}


void detect_virus(char *buffer, unsigned int size, link *virus_list) {
    for (int i = 0; i < size; i++) {
        link *tmp = NULL;
        tmp = virus_list;
        while (tmp != NULL) {
            if (memcmp(tmp->vir->sig, buffer + i, tmp->vir->SigSize) == 0) {
                printf("Loction: %d \n", i);
                printf("name: %s \n", tmp->vir->virusName);
                printf("sigSize: %d \n", tmp->vir->SigSize);
                tmp = NULL;
            } else {
                tmp = tmp->nextVirus;
            }
        }
    }
}
