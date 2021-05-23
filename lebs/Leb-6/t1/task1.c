#include "task1.h"

char *Find_Val(char *tofind) {
    vardec *tempo = vardeclist;
    while (tempo != NULL) {
        if (strcmp(tempo->name, tofind) == 0) {
            return tempo->value;
        }
        tempo = tempo->next;
    }
    return NULL;
}

int task5stuff(cmdLine *pCmdLine) {
    int Checkerror = 0;
    if (strncmp(pCmdLine->arguments[0], "-d", 2) == 0) {
        dbug = 1;
        return 0;
    } else if (strncmp(pCmdLine->arguments[0], "procs", 5) == 0) {
        printProcessList(&list);
    } else if (strncmp(pCmdLine->arguments[0], "cd", 2) == 0) {
        if (strncmp(pCmdLine->arguments[1], "~", 1) == 0)
            chdir(getenv("HOME"));
        else Checkerror = chdir(pCmdLine->arguments[1]);
        char buff[PATH_MAX];
        getcwd(buff, PATH_MAX);
        if (Checkerror) {
            perror("error");
            exit(1);
        }
        return 0;
    } else if ((strncmp(pCmdLine->arguments[0], "kill", 4) == 0)) {
        kill(atoi(pCmdLine->arguments[1]), SIGKILL);
        updateProcessStatus(&list, atoi(pCmdLine->arguments[1]), TERMINATED);
        return 0;
    } else if ((strncmp(pCmdLine->arguments[0], "suspand", 7) == 0)) {
        kill(atoi(pCmdLine->arguments[1]), SIGSTOP);
        updateProcessStatus(&list, atoi(pCmdLine->arguments[1]), SUSPENDED);
        return 0;
    } else if ((strncmp(pCmdLine->arguments[0], "wake", 4) == 0)) {
        kill(atoi(pCmdLine->arguments[1]), SIGCONT);
        updateProcessStatus(&list, atoi(pCmdLine->arguments[1]), RUNNING);
        return 0;

    } else if ((strncmp(pCmdLine->arguments[0], "set", 3) == 0)) {
        AddVardec(&vardeclist, pCmdLine);
        return 0;
    } else if ((strncmp(pCmdLine->arguments[0], "vars", 4) == 0)) {
        printVardecList(&vardeclist);
        return 0;
    } else if ((strncmp(pCmdLine->arguments[0], "\n", 1) == 0)) {
        return 0;
    }
    return 1;
}

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

void AddVardec(vardec **var_list, cmdLine *cmd) {
    vardec *list_ptr = *var_list;
    if (list_ptr == NULL) {
        list_ptr = (vardec *) calloc(1, sizeof(vardec));
        list_ptr->name = cmd->arguments[1];
        list_ptr->value = cmd->arguments[2];
        (*var_list) = list_ptr;
    } else {
        if (strcmp(list_ptr->name, cmd->arguments[1]) == 0) {
            list_ptr->value = cmd->arguments[2];
            return;
        }
        while (list_ptr->next != NULL) {
            if (strcmp(list_ptr->name, cmd->arguments[1]) == 0) {
                list_ptr->value = cmd->arguments[2];
                return;
            }
            list_ptr = list_ptr->next;
        }
        list_ptr->next = (vardec *) calloc(1, sizeof(process));
        list_ptr = list_ptr->next;
        list_ptr->name = cmd->arguments[1];
        list_ptr->value = cmd->arguments[2];
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

void printVardecList(vardec **var_list) {
    vardec *head = *var_list;
    printf("%s\t%s\t%s\n", "pid", "commend", "status");
    while (head != NULL) {
        printf("the name is: %s\t", head->name);
        printf("the value is: %s\n", head->value);
        head = head->next;
    }
}

void execute(cmdLine *pCmdLine) {
    int cpid;
    int needfork = task5stuff(pCmdLine);
    for (int i = 0; i < pCmdLine->argCount; i++) {
        if (strncmp(pCmdLine->arguments[i], "$", 1) == 0) {
            char *var_value = Find_Val(pCmdLine->arguments[i] + 1);
            if (var_value != NULL)
                replaceCmdArg(pCmdLine, i, var_value);
            else
                perror("dr.bajrno sayed that: ");
        }
    }

    if (needfork) {
        if (!(cpid = fork())) //creating childrens in fusion way
        {
            if (pCmdLine->inputRedirect) {
                close(STDIN_FILENO); //closeing the current inputstream (STDIN 0)
                open(pCmdLine->inputRedirect, O_RDONLY);
            }
            if (pCmdLine->outputRedirect) {
                close(STDOUT_FILENO); //closeing the current outputsteam (STDOUT 1)
                open(pCmdLine->outputRedirect, O_WRONLY); //
            }

            if (pCmdLine->next)
                ExePipe(pCmdLine);
            else {
                execvp(pCmdLine->arguments[0], pCmdLine->arguments);
                perror("error");
                exit(EXIT_FAILURE);
            }
            // we did the above becouse execvp read from
            // 0 and write to 1 automaticly
            //so we change 0 to be inputRedirect and 1 to be outputRedirect if their exsist
        } else {
            addProcess(&list, pCmdLine, cpid);
            if (pCmdLine->blocking)
                waitpid(cpid, NULL, 0);
        }
    }

    if (dbug) {
        fprintf(stderr, "The commend you typed:\t%s\n", pCmdLine->arguments[0]);
        fprintf(stderr, "cpid:\t%i\n", cpid);
        if (!needfork)
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


void ExePipe(cmdLine *pCmdLine) {

    int pipefd[2];
    int cpid, cpid2;
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    //child1
    if (cpid == 0) {
        close(STDOUT_FILENO);
        dup(pipefd[1]); //write ends
        close(pipefd[1]);
        execvp(pCmdLine->arguments[0], pCmdLine->arguments);
        perror("d.r bajrno said that: ");
        exit(EXIT_FAILURE); //we will not get here
    } else {
        waitpid(cpid, NULL, 0);
        close(pipefd[1]);
        //child2;
        cpid2 = fork();
        if (cpid2 == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (cpid2 == 0) {
            close(0);
            dup(pipefd[0]);
            close(pipefd[0]);
            execvp(pCmdLine->next->arguments[0], pCmdLine->next->arguments);
            perror("d.r bajrno said that: ");
            exit(EXIT_FAILURE);
        } else {
            close(pipefd[0]);
            waitpid(cpid2, NULL, 0);
        }
    }
}
