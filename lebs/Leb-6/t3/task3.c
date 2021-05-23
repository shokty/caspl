#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    char *argumentsChild1[3];
    char *argumentsChild2[4];
    int pipefd[2];
    int cpid, cpid2;
    argumentsChild1[0] = "ls";
    argumentsChild1[1] = "-l";
    argumentsChild1[2] = '\0';

    argumentsChild2[0] = "tail";
    argumentsChild2[1] = "-n";
    argumentsChild2[2] = "2";
    argumentsChild2[3] = '\0';
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    cpid = fork();
    if (cpid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    //child1
    if (cpid == 0)
    {

        close(STDOUT_FILENO);
        dup(pipefd[1]); //write ends
        close(pipefd[1]);
        execvp(argumentsChild1[0], argumentsChild1);
        perror("d.r bajrno said that: ");
        exit(EXIT_FAILURE); //we will not get here
    }
    else
    {
        waitpid(cpid,NULL,0);
        close(pipefd[1]);
        //child2;
        cpid2 = fork();
        if (cpid2 == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (cpid2 == 0)
        {
            close(0);
            dup(pipefd[0]);
            close(pipefd[0]);
            execvp(argumentsChild2[0], argumentsChild2);
            perror("d.r bajrno said that: ");
            exit(EXIT_FAILURE);
        }
        else
        {
            close(pipefd[0]); // why?
            waitpid(cpid2,NULL,0);

        }
    }

    return 0;
}