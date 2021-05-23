#include <stdio.h>

#define    MAX_LEN 34            /* maximal input string size */

/* enough to get 32-bit string + '\n' + null terminator */
extern void assFunc(int x, int y);

char c_checkValidity(int x, int y) {
     if(x >= y)
    return 1;
else 
return 0;
}

int main(int argc, char **argv) {
    char buf[MAX_LEN];
    int input[2];
    input[0] = 0;
    input[1] = 0;
    int w = 0;
    for (int i = 0; i < 2; i++) {
        fgets(buf, MAX_LEN, stdin);
        while (buf[w] != '\n') {
            if (buf[w] > 47 && buf[w] < 58) {
                input[i] = input[i] * 10;
                // printf("%d  ",input[0]);
                input[i] = input[i] + (buf[w] - 48);
            }
            w++;
        }
        w = 0;
    }
  //  printf("%d \n", c_checkValidity(input[0], input[1]));
  //  printf("%d   %d",input[0],input[1]);
    assFunc(input[0], input[1]);            /* call your assembly function */
    return 0;
}
