#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>


#include <unistd.h>
#define MAXLINE 1000
#define MAXARGS 5

int main(){

    char cmd[MAXLINE];
    char *argv[]={"ls",NULL};
    int cmdErr = 0;
    pid_t pid;

    while(1){
        printf("@ ");
        fgets(cmd, MAXLINE, stdin);
        if (feof(stdin)){
            printf("\n**NULL SWEAT CHUMMER**\n");
            exit(0);
        }
        if((pid = fork()) == 0){
            if(execvp("cd", argv) < 0){
                printf("Unknown Command\n");
                exit(0);
            }
        }
        int status;
        if (waitpid(pid, &status, 0) < 0){
            fprintf(stderr, "%s: %s\n", "pid no good", strerror(errno));
        }
    }

}
