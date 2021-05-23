//
// Created by shok-t on 25.3.2020.
//

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

void ToUpper(FILE *input);

void printsaharf();

void solodebugg(FILE *input, bool tofileonly);

void Encrypt(char *argv[], FILE *input, bool debugg, bool tofile, int argnum);

int main(int argc, char *argv[]) {
    FILE *input = stdin;
    remove("saharfile.txt");
    int encryptargs = 1;
    int filname = 1;
    int fromfilenum = 1;

    bool debug = false; // -D flag.
    bool encrypt = false; // +e / -e flag
    bool tofile = false; // -o flag
    bool fromfile = false; // -i flag


    if (argc == 1) {
        while (!feof(stdin)) {
            ToUpper(input);
        }
    } //only encoder was putted.

    else {
        for (int i = 1; i < argc; i++) {
            if (!debug)
                debug = ((strcmp(argv[i], "-D") == 0));
            if (!encrypt) {
                encrypt = (argv[i][1] == 'e' && ((argv[i][0] == '+') || argv[i][0] == '-'));
                encryptargs = i;
            }
            if (!tofile) {
                tofile = (argv[i][0] == '-' && argv[i][1] == 'o');
                filname = i;
            }
            if (!fromfile) {
                fromfile = (argv[i][0] == '-' && argv[i][1] == 'i');
                fromfilenum = i;
            }
        } //chack flag

        if (fromfile) {
            if (fopen(argv[fromfilenum] + 2, "r") != NULL)
                input = fopen(argv[fromfilenum] + 2, "r");
            else
            {
                perror("Error printed by perror");
                exit(0); //print error and exit
            }
        }

        if (encrypt) {
            Encrypt(argv, input, debug, tofile, encryptargs);
            if (tofile)
                rename("saharfile.txt", argv[filname] + 2);


        } else if (debug) {
            solodebugg(input, tofile);
            if (tofile)
                rename("saharfile.txt", argv[filname] + 2);
        }
    }


    remove("saharfile.txt");
    return 0;
}


void solodebugg(FILE *input, bool tofileonly) {
    char addcur;
    char curchar;
    while (curchar != EOF) {
        FILE *endfile = fopen("saharfile.txt", "a");
        curchar = fgetc(input);
        addcur = curchar;
        if ((int) curchar >= 97 && (int) curchar <= 122) {
            addcur = (char) ((int) curchar - 32); //change it to upper
            fprintf(stderr, "%d %s %d %c", curchar, "        ", addcur, '\n'); // print at Err
        } else if (curchar != '\n' && curchar != EOF) {
            fprintf(stderr, "%d %s %d %c", curchar, "        ", addcur, '\n');
        } else if (curchar != EOF && !tofileonly) {
            printf("%c", '\n');
            printsaharf();
        }
        fputc(addcur, endfile); //Add a the change\unchanged char to the new file
        fflush(endfile);
    }
}

void printsaharf() {
    FILE *toprint = fopen("saharfile.txt", "r");
    char toprintt = fgetc(toprint);
    while (toprintt != EOF && toprintt != '\n') {
        printf("%c", toprintt);
        toprintt = fgetc(toprint);
    }
    remove("saharfile.txt");
    printf("%c", '\n');
}

void ToUpper(FILE *input) {
    char curchar;
    FILE *endfile = fopen("saharfile.txt", "a");
    while (curchar != EOF) {
        curchar = fgetc(input);
        if ((int) curchar >= 97 && (int) curchar <= 122) {
            curchar = (char) ((int) curchar - 32);
            fputc(curchar, endfile);
        } else if (curchar != '\n')
            fputc(curchar, endfile);
        else {
            printsaharf();
            break;
        }
        fflush(endfile);
    }
}

void Encrypt(char *argv[], FILE *input, bool debugg, bool tofile, int argnum) {

    int size = strlen(argv[argnum]);
    int whattoadd[1000];
    if (argv[argnum][0] == '+') {
        for (int i = 2; i < size; i++) {
            whattoadd[i - 2] = (int) argv[argnum][i] - 48;
        }
    } else if (argv[argnum][0] == '-') {
        for (int i = 2; i < size; i++) {
            whattoadd[i - 2] = -(int) argv[argnum][i] + 48;
        }
    } else {
        printf("%s", argv[1]);
        printf("%s\n", " wtf?");
    }
    //start the int arry that we gonna round robin around it.

    int t = 0;
    char toadd;
    char before;

    while (toadd != EOF) {
        FILE *endfile = fopen("saharfile.txt", "a");
        toadd = fgetc(input);
        before = toadd;
        if (toadd != EOF && toadd != '\n') {
            if (t < size - 2) {
                toadd = (char) ((int) toadd + whattoadd[t]);
                if (debugg)
                    fprintf(stderr, "%d %s %d %c", before, "        ", toadd, '\n');
                putc(toadd, endfile);
                fflush(endfile);
                t++;
            } else {
                t = 0;
                toadd = (char) ((int) toadd + whattoadd[t]);
                if (debugg)
                    fprintf(stderr, "%d %s %d %c", before, "        ", toadd, '\n');
                putc(toadd, endfile);
                fflush(endfile);
                t++;
            }
        } else if (toadd != EOF) {

            if (!tofile)
                printsaharf();

            fputc('\n', endfile);
            fflush(endfile);
            t = 0;
        }
    }
}
