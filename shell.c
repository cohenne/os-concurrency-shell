#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

int stat_loc;

int process_arglist(int count, char **arglist){
    pid_t child_pid;

    if(0 == strcmp(arglist[0], "exit")){
        exit(0);
    }

    if(0 == strcmp(arglist[count-1],"&")){
        child_pid = fork();

        if (child_pid < 0){
            fprintf(stderr,"\nerror in fork\n");
            return 1;
        }

        if(0 == child_pid){
            arglist[count-1] = NULL;

            if(execvp(arglist[0], arglist) < 0){
                fprintf(stderr,"\nsome error\n");
                exit(0);
            }
        }

        return 1;
    }


    child_pid = fork();

    if (child_pid < 0){
        fprintf(stderr,"\nerror in fork\n");
        return 1;
    }

    if(0 == child_pid){
        if(execvp(arglist[0], arglist) < 0){
            fprintf(stderr,"\nsome error\n");
            exit(0);}
    }

    else{
        waitpid(child_pid, &stat_loc, WUNTRACED);
    }

    return 1;
}


int main(void){
    while (1){
        char** arglist = NULL;
        char* line = NULL;
        size_t size;
        int count = 0;
        printf("----> ");

        if (-1 == getline(&line, &size, stdin)){
            free(line);
            break;
        }

        arglist = (char**) malloc(sizeof(char*));

        if (NULL == arglist) {
            printf("malloc failed: %s\n", strerror(errno));
            exit(1);
        }		arglist[0] = strtok(line, " \t\n");

        while (NULL != arglist[count]) {
            ++count;
            arglist = (char**) realloc(arglist, sizeof(char*) * (count + 1));

            if (NULL == arglist) {
                printf("realloc failed: %s\n", strerror(errno));
                exit(1);
            }

            arglist[count] = strtok(NULL, " \t\n");
        }

        if (0 != count) {
            if (!process_arglist(count, arglist)) {
                free(line);
                free(arglist);
                break;
            }
        }

        free(line);
        free(arglist);
    }
    return 0;
}