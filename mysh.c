#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>

FILE *stdin;
FILE *fp;

void handle_command(const char *user_command){
    write(STDOUT_FILENO, user_command, strlen(user_command));
    
    // make copy
    char *dup_command = strdup(user_command);    
    // remove newline
    for (int i = 0; i < strlen(dup_command); i++) {
        if (dup_command[i] == '\n') {
            dup_command[i] = '\0';
        }
    }
    if (strlen(dup_command) > 0) {
        pid_t pid = fork();
        if (pid == 0) {
            //printf("I'm a child with pid %d.\n", getpid());
            char *argv[100];
            char *tok = strtok(dup_command, " ");
            int arg_index = 0;
            char path[128];
            strcat(path, "/bin/");
            while (tok != NULL) {
                if (arg_index == 0) {
                    argv[arg_index] = strcat(path, tok);
                } else {
                    argv[arg_index] = tok;
                }
                tok = strtok(NULL, " ");
                arg_index++;
            }
            argv[arg_index] = NULL;
            execv(argv[0], argv);
            
            for (int i = 0; i <= arg_index; i++) {
                printf("arg: %s\n", argv[i]);
            }
            //execv(argv[0], argv);
                /*
            if (strcmp(dup_command, "ls") == 0) {
                char *args[2] = {"/bin/ls", NULL};
                execv(args[0], args);
            }*/
            printf("child: exec failed\n");
            _exit(1);
        } else {
            //printf("I'm a parent with pid %d.\n", getpid());
            int status;
            waitpid(pid, &status, 0);  
            printf("parent: child process exits\n");
        }
    }
}

int main(int argc, char *argv[]) {
    // run in interactive mode
    if (argc == 1) {
        fp = stdin;
        char str[128]; 
        while (1) {
            write(STDOUT_FILENO, "mysh> ", 6);
            if (fgets(str, 128, fp) != NULL) {
                    if (strncmp(str, "exit", 4) == 0) {
                        break;
                    }
                    handle_command(str);
            } else {
                break;
            }
        }
    }
     // run in batch mode
    else if (argc == 2) {
        printf("will run in batch mode\n");
        fp = fopen(argv[1], "r");
        if (fp == NULL) {
            write(STDOUT_FILENO, "Error: Cannot open file ", 24);
            write(STDOUT_FILENO, argv[1], strlen(argv[1]));
            write(STDOUT_FILENO, "\n", 1);
            exit(1);
        } else {
            char str[128];
            char* command;
            while ((command = fgets(str, 128, fp)) != NULL) {
                //write(STDOUT_FILENO, command, strlen(command));
                handle_command(command);
            }   
        }
    } else {
        write(STDOUT_FILENO, "Usage: mysh[batch-file]\n", 24);
        exit(1);
    }
    return 0;
}

            
    
