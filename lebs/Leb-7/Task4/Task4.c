#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const **argv)
{   int i=0;
    int k=0;
    char Drbezerano[10000];
    sscanf(argv[1], "%s",Drbezerano);
    printf("%s\n",Drbezerano);
    while(Drbezerano[k]!=0)
    {
        if(Drbezerano[k]>=48&&Drbezerano[k]<=57)
        {
            i++;
        }
    k++;
    }
    printf("%d\n",i);
    return i;
}
