#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>

FILE *stdin;
FILE *fp;

// takes in string of user command, parses arguments, and calls execv
void handle_command(const char *user_command){
//    write(STDOUT_FILENO, user_command, strlen(user_command));
    
    // make duplicate of command
    char *dup_command = strdup(user_command);  
    printf("dup command: %s\n", dup_command);  
    // remove newline
    for (int i = 0; i < strlen(dup_command); i++) {
        if (dup_command[i] == '\n') {
            dup_command[i] = '\0';
        }
    }
    // case: valid command
    if (strlen(dup_command) > 0) {
        pid_t pid = fork();
        if (pid == 0) {
            //printf("I'm a child with pid %d.\n", getpid());
            
            // detect redirection
            int start_of_filename = 0;
            for (int i = 0; i < strlen(dup_command); i++) {
                if (dup_command[i] == '>') {
                    // check for multiple >'s
                    if (i+1 < strlen(dup_command)) {
                        if (dup_command[i+1] == '>') {
                             write(STDERR_FILENO, "Redirection misformatted.\n", 26);
                        }
                    }
                    start_of_filename = i+1;
                    break;
                }
            }
            char filename[128];
            int i;
            // extract name of batch file
            for (i = start_of_filename; i < strlen(dup_command); i++) {
                filename[i-start_of_filename] = dup_command[i];
            }
            filename[i] = '\0';
            
            char *argv[100];
            char *tok = strtok(dup_command, " ");
            int arg_index = 0; 
            // parse command, add to argv array
            while (tok != NULL) {
                argv[arg_index] = tok;
                tok = strtok(NULL, " ");
                arg_index++;
            }
            // set last argument to NULL
            argv[arg_index] = NULL;
            execv(argv[0], argv);
            //printf("child: exec failed\n");
            free(dup_command);
            _exit(1);
        } else {
            free(dup_command);
            //printf("I'm a parent with pid %d.\n", getpid());
            int status;
            waitpid(pid, &status, 0);  
            //printf("parent: child process exits\n");
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
        fp = fopen(argv[1], "r");
        // batch file invalid
        if (fp == NULL) {
            write(STDERR_FILENO, "Error: Cannot open file ", 24);
            write(STDERR_FILENO, argv[1], strlen(argv[1]));
            write(STDERR_FILENO, ".\n", 2);
            exit(1);
        } else {
            char str[128];
            char* command;
            char* echo_prefix = "echo ";
            char* echo_command;
            while ((command = fgets(str, 128, fp)) != NULL) {
                echo_command = (char*) malloc(sizeof(char)*(strlen(command) + 5));
                strcat(echo_command, echo_prefix);
                strcat(echo_command, command);
                system(echo_command);
                free(echo_command);
                //write(STDOUT_FILENO, command, strlen(command));
                if (strncmp(command, "exit", 4) == 0) {
                    break;
                }
                handle_command(command);
            }   
        }
    } else {
        write(STDERR_FILENO, "Usage: mysh [batch-file]\n", 25);
        exit(1);
    }
    return 0;
}

            
    
