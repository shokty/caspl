#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#include "LineParser.h"
#include <linux/limits.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define TERMINATED -1
#define RUNNING 1
#define SUSPENDED 0
int dbug = 0;

void execute(cmdLine *pCmdLine);

typedef struct process {
    cmdLine *cmd;         /* the parsed command line*/
    pid_t pid;            /* the process id that is running the command*/
    int status;           /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next; /* next process in chain */
} process;
process *list;

void addProcess(process **process_list, cmdLine *cmd, pid_t pid);

void printProcessList(process **process_list);

void freeProcessList(process *process_list);

void updateProcessList(process **process_list);

void updateProcessStatus(process **process_list, int pid, int status);

void cleanshit(process *head);

void updateProcessStatus(process **process_list, int pid, int status) {
    process *process_ptr = *process_list;
    while (process_ptr != NULL) {
        if (process_ptr->pid == pid) {
            process_ptr->status = status;
            break;
        }
        process_ptr = process_ptr->next;
    }
}

void updateProcessList(process **process_list) {
    int status;
    process *list_ptr = *process_list;
    int w;
    while (list_ptr != NULL) {
        w = waitpid(list_ptr->pid, &status, WNOHANG | WUNTRACED | WCONTINUED);

        if (w == -1) {
            updateProcessStatus(process_list, list_ptr->pid, TERMINATED);
            perror("error");
        } else if (list_ptr->pid == w) {
            if (WIFEXITED(status))
                updateProcessStatus(&list_ptr, list_ptr->pid, TERMINATED);
            else if (WIFSTOPPED(status))
                updateProcessStatus(&list_ptr, list_ptr->pid, SUSPENDED);
            else if (WIFCONTINUED(status))
                updateProcessStatus(&list_ptr, list_ptr->pid, RUNNING);
            else
                updateProcessStatus(&list_ptr, list_ptr->pid, TERMINATED);
        }
        list_ptr = list_ptr->next;
    }
}

void freeProcessList(process *process_list) {
    if (process_list != NULL) {
        freeCmdLines(process_list->cmd);
        freeProcessList(process_list->next); //clean the list recursively
        free(process_list);
    }
}

void addProcess(process **process_list, cmdLine *cmd, pid_t pid) {
    process *list_ptr = *process_list;
    if (list_ptr == NULL) {
        list_ptr = (process *) calloc(1, sizeof(process));
        list_ptr->cmd = cmd;
        list_ptr->pid = pid;
        list_ptr->status = RUNNING;
        (*process_list) = list_ptr;
    } else {
        while (list_ptr->next != NULL)
            list_ptr = list_ptr->next;
        list_ptr->next = (process *) calloc(1, sizeof(process));
        list_ptr->next->cmd = cmd;
        list_ptr->next->pid = pid;
        list_ptr->next->status = RUNNING;
    }
}

void printProcessList(process **process_list) {
    updateProcessList(process_list);
    process *head = *process_list;
    printf("%s\t%s\t%s\n", "pid", "commend", "status");
    while (head != NULL) {
        printf("%d\t", head->pid);
        printf("%s\t", head->cmd->arguments[0]);
        printf("%d\n", head->status);
        head = head->next;
    }
    head = *process_list;
    while (head != NULL && head->status == -1) {
        freeCmdLines(head->cmd);
        head->cmd = NULL;
        head = head->next;
        free(*process_list);
        *process_list = head;
    }
    cleanshit(head);
}

void cleanshit(process *head) {
    if (head != NULL && head->next != NULL) {
        process *prvlink = head;
        head = head->next;
        if (head->status == -1) {
            freeCmdLines(head->cmd);
            prvlink->next = head->next;
            cleanshit(head->next);
            free(head);
        } else
            cleanshit(head);
    }
}


void execute(cmdLine *pCmdLine) {
    int cpid;
    int Checkerror = 0;
    int todele = 0;
    if (strncmp(pCmdLine->arguments[0], "-d", 2) == 0) {
        dbug = 1;
        todele = 1;
    } else if (strncmp(pCmdLine->arguments[0], "procs", 5) == 0) {
        printProcessList(&list);
        todele =1;
    } else if (strncmp(pCmdLine->arguments[0], "cd", 2) == 0) {
        Checkerror = chdir(pCmdLine->arguments[1]);
        char buff[PATH_MAX];
        getcwd(buff, PATH_MAX);
        if (Checkerror) {
            perror("error");
            exit(1);
        }
        todele =1;
    } else if ((strncmp(pCmdLine->arguments[0], "kill", 4) == 0)) {
        kill(atoi(pCmdLine->arguments[1]), SIGKILL);
        updateProcessStatus(&list, atoi(pCmdLine->arguments[1]), TERMINATED);
        todele =1;
    } else if ((strncmp(pCmdLine->arguments[0], "suspand", 7) == 0)) {
        kill(atoi(pCmdLine->arguments[1]), SIGSTOP);
        updateProcessStatus(&list, atoi(pCmdLine->arguments[1]), SUSPENDED);
        todele =1;

    } else if ((strncmp(pCmdLine->arguments[0], "wake", 4) == 0)) {
        kill(atoi(pCmdLine->arguments[1]), SIGCONT);
        updateProcessStatus(&list, atoi(pCmdLine->arguments[1]), RUNNING);
        todele =1;

    } else if ((strncmp(pCmdLine->arguments[0], "\n", 1) == 0))
        todele = 1;

    else {
        if (!(cpid = fork())) //creating childrens in fusion way
        {
            execvp(pCmdLine->arguments[0], pCmdLine->arguments);
            perror("error");
            exit(1);
        } else {
            addProcess(&list, pCmdLine, cpid);
            if (pCmdLine->blocking)
                waitpid(cpid, NULL, 0);
        }
    }
    if (dbug) {
        fprintf(stderr, "The commend you typed:\t%s\n", pCmdLine->arguments[0]);
        fprintf(stderr, "cpid:\t%i\n", cpid);
        if (todele)
        freeCmdLines(pCmdLine);
    }
    
}

int main(int argc, char const **argv) {
    char buff[PATH_MAX];
    char input[2048];
    while (1) {
        getcwd(buff, PATH_MAX);
        fgets(input, 2048, stdin);
        if (strncmp(input, "quit", 4) == 0) {
            freeProcessList(list);
            exit(0);
        }
        cmdLine *cmd = parseCmdLines(input);
        execute(cmd);
        // freeCmdLines(cmd);
    }
    return 0;
}
