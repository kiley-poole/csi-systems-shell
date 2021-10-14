#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>


#include <unistd.h>
#define MAXLINE 1000
#define MAXARGS 5
#define GREEN  "\033[0;32m"

void parse_command(char *cmd, char **argv);
void run_command(char **argv);
int get_builtin(char *argv);
void print_logo();

int main(){
    char cmd[MAXLINE];
    char *argv[MAXARGS];

    print_logo();

    while(1){
        printf("\u00A5 ");
        fgets(cmd, MAXLINE, stdin);
        if (feof(stdin)){
            printf("\n**NULL SWEAT CHUMMER**\n");
            exit(0);
        }
        parse_command(cmd, argv);
        run_command(argv);
    }

}

void parse_command(char *cmd, char **argv){
    cmd[strlen(cmd) - 1] = ' ';
    char *delim;
    int count = 0;
    while (*cmd && (*cmd == ' '))
    cmd++;
    while ((delim = strchr(cmd, ' '))) {
        argv[count++] = cmd;
        *delim = '\0';
        cmd = delim + 1;
        while (*cmd && (*cmd == ' '))
            cmd++;
    }
    argv [count] = NULL;
}

void run_command(char **argv){
    pid_t pid;
    if(get_builtin(argv[0]) == 0){
        if((pid = fork()) == 0){
            setpgid(0, 0);
            if(execvp(argv[0], argv) < 0){
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

int get_builtin(char *argv){
    for(int i = 0; argv[i]; i++){
        argv[i] = tolower(argv[i]);
    }
    if(strcmp(argv, "quit") == 0 || strcmp(argv, "exit") == 0){
        printf("\n**NULL SWEAT CHUMMER**\n");
        exit(0);
    }
    if(strcmp(argv, "samurai") == 0){
        printf("A street samurai is a shadowrunner who specializes in physical combat. \n" 
                "They are often heavily augmented with cyberware and/or bioware to "
                "enhance their combat prowess and carry a variety of weapons to deal with any threat their groups encounter.\n");
        return 1;
    }
    return 0;
}

void print_logo(){
    printf(GREEN " __   _ _______ _     _  ______  _____  _______ _     _ _______              \n");
    printf(" | \\  | |______ |     | |_____/ |     | |______ |_____| |______ |      |     \n");
    printf(" |  \\_| |______ |_____| |    \\_ |_____| ______| |     | |______ |_____ |_____\n\n");
}
