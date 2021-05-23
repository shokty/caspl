//
// Created by shok-t on 15.5.2020.
//

#ifndef UNTITLED11_MAIN_H
#define UNTITLED11_MAIN_H
#endif //UNTITLED11_MAIN_H


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
#include <fcntl.h>

#define TERMINATED -1
#define RUNNING 1
#define SUSPENDED 0
int dbug = 0;
//START struct zone
typedef struct vardec {
    char *name;         /* var name */
    char *value;            /* var value */
    struct vardec *next; /* next vardec in chain */
} vardec;
typedef struct process {
    cmdLine *cmd;         /* the parsed command line*/
    pid_t pid;            /* the process id that is running the command*/
    int status;           /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next; /* next process in chain */
} process;
vardec *vardeclist;
process *list;
//END struct zone


//START Funck dec zone
void printProcessList(process **process_list);
void ExePipe(cmdLine *pCmdLine);
void printVardecList(vardec **var_list);
void addProcess(process **process_list, cmdLine *cmd, pid_t pid);
void execute(cmdLine *pCmdLine);
void AddVardec(vardec **var_list, cmdLine *cmd);
void freeProcessList(process *process_list);
void updateProcessList(process **process_list);
char* Find_Val(char* tofind);
void updateProcessStatus(process **process_list, int pid, int status);
void cleanshit(process *head);
// END Funck dec zone
