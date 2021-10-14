#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>


#include <unistd.h>
#define MAXLINE 1000
#define MAXARGS 15
#define GREEN  "\033[0;32m"

typedef unsigned char bool;

struct command
{
    char    *argv[MAXARGS];
    bool    bg;
    bool    control_and;
    bool    control_or;
};

int parse_command(char *cmd, struct command commands[MAXARGS]);
void run_command(struct command commands[MAXARGS], int cmd_count);
int get_builtin(char *argv);
void errorP(char *errText);
void print_logo();
pid_t Fork();

void sigint_handler(int sig){
    return;
}

int main(){
    char cmd[MAXLINE];
    struct command commands[MAXARGS];

    print_logo();
    while(1){
        printf(GREEN"\n\u00A5 ");
        if (signal(SIGINT, sigint_handler) == SIG_ERR)
            errorP("signal no good");
        fgets(cmd, MAXLINE, stdin);
        if (feof(stdin)){
            printf("\n**NULL SWEAT, CHUMMER**\n");
            exit(0);
        }
        int cmd_count = parse_command(cmd, commands);
        run_command(commands, cmd_count);
    }

}

int parse_command(char *cmd, struct command commands[MAXARGS]){
    cmd[strlen(cmd) - 1] = ' ';
    char *delim;
    int arg_count = 0;
    int command_count = 0;

    while (*cmd && (*cmd == ' '))
        cmd++;

    while ((delim = strchr(cmd, ' '))) {
        if( (*cmd == '&' && *(cmd+1) == '&') || 
            (*cmd == '|' && *(cmd+1) == '|') || 
            (*cmd == ';')                    || 
            (*cmd == '&' && *(cmd+1) == ' ') )
        {

            commands[command_count].bg = 0;
            commands[command_count].argv[arg_count] = NULL;

            if(*cmd == '&' && *(cmd+1) == ' ')
                commands[command_count].bg = 1;

            arg_count = 0;
            command_count++;

            commands[command_count].control_and = 0;
            commands[command_count].control_or = 0;

            if(*cmd == '&' && *(cmd+1) == '&')
                commands[command_count].control_and = 1;

            if (*cmd == '|' && *(cmd+1) == '|')
                commands[command_count].control_or = 1;

        } else {
            commands[command_count].argv[arg_count] = cmd;
            arg_count++;
        }
        *delim = '\0';
        cmd = delim + 1;
        while (*cmd && (*cmd == ' '))
            cmd++;
    }

    commands[command_count].argv[arg_count] = NULL;
    commands[command_count].bg = 0;

    return command_count;
}

void run_command(struct command commands[MAXARGS], int cmd_count){
     int i = 0;
     bool failed = 0;
     while(i <= cmd_count){
        if(commands[i].argv[0] == NULL ||
            (i > 0 && ( 
                (commands[i].control_or == 1 && failed != commands[i].control_or) || 
                (commands[i].control_and == 1 && failed == commands[i].control_and)
            ))){
            i++;
            continue;
        }

        pid_t pid;
        int status;
        failed = 0;

        if(get_builtin(commands[i].argv[0]) == 0){
            if((pid = Fork()) == 0){
                if(commands[i].bg == 1)
                    setpgid(0, 0);
                if(execvp(commands[i].argv[0], commands[i].argv) < 0){
                    printf("Unknown Command\n");
                    exit(1);
                }
            }
            if(commands[i].bg == 0){
                if (waitpid(pid, &status, 0) < 0){
                    errorP("pid no good");
                }
                failed = WEXITSTATUS(status);
            }
        }
        i++;
     }
}

int get_builtin(char *argv){
    for(int i = 0; argv[i]; i++){
        argv[i] = tolower(argv[i]);
    }
    if(strcmp(argv, "quit") == 0 || strcmp(argv, "exit") == 0){
        printf("\n**NULL SWEAT, CHUMMER**\n");
        exit(0);
    }
    if(strcmp(argv, "cowboy") == 0){
        printf("A person especially skilled at navigating cyberspace, specifically someone employed as a digital thief, who steals money or data.\n");
        return 1;
    }
    return 0;
}

void errorP(char *errText){
    fprintf(stderr, "%s: %s\n", errText, strerror(errno));
    exit(0);
}

pid_t Fork(){
    pid_t pid;
    if((pid = fork()) < 0)
        errorP("Fork Error");
    return pid;
}

void print_logo(){
    printf(GREEN " __   _ _______ _     _  ______  _____  _______ _     _ _______              \n");
    printf(" | \\  | |______ |     | |_____/ |     | |______ |_____| |______ |      |     \n");
    printf(" |  \\_| |______ |_____| |    \\_ |_____| ______| |     | |______ |_____ |_____\n\n");
}
