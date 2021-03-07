#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

#include "myalias.h"

FILE *stdin;
FILE *fp1;
FILE *fp2;
int fd;
int old_stdout;
struct linked_list* aliases;
char *dup_command;

// this function runs an individual line containing a user command
// takes in string of user command, parses arguments, and calls execv
void handle_command(const char *user_command) {
    // file descriptor for file to redirect to
    // make duplicate of command
    dup_command = strdup(user_command);
    // remove newline
    for (int i = 0; i < strlen(dup_command); i++) {
        if (dup_command[i] == '\n') {
            dup_command[i] = '\0';
        }
    }
    // case: valid command
    if (strlen(dup_command) > 0) {
        if (strcmp(dup_command, "alias") == 0) {
            free(dup_command);
            print_list(aliases);
            return;
        }
        // user types <alias-name>
        if (search(aliases, dup_command) != NULL) {
            char* replacement = (char*)search(aliases, dup_command)->value;
            int replace_len = strlen(replacement);
            dup_command = realloc(dup_command, replace_len*sizeof(char)+1);
            strcpy(dup_command, replacement);
        }
        char *argv_alias[100];
        bool alias_detected = false;
        if (strncmp(dup_command, "alias", 5) == 0) {
            alias_detected = true;
        }
        if (alias_detected) {
            // grab alias keyword
            char *tok_alias = strtok(dup_command, " \t");
            // grab command
            tok_alias = strtok(NULL, " \t");
            int arg_alias_index = 0;
            // set command as first argument
            argv_alias[arg_alias_index++] = tok_alias;
            if ((tok_alias = strtok(NULL, "")) != NULL) {
                argv_alias[arg_alias_index++] = tok_alias;
            }
            argv_alias[arg_alias_index] = NULL;
            // user types alias <word>: prints alias and replacement
            if (arg_alias_index == 1) {
                if (search(aliases, argv_alias[0]) != NULL) {
                    char* replace =
                        (char*)(search(aliases, argv_alias[0])->value);
                    write(STDOUT_FILENO, argv_alias[0], strlen(argv_alias[0]));
                    write(STDOUT_FILENO, " ", 1);
                    write(STDOUT_FILENO, replace, strlen(replace));
                    write(STDOUT_FILENO, "\n", 1);
                }
                return;
            }
            if (strcmp(argv_alias[0], "alias") == 0) {
                write(STDERR_FILENO,
                    "alias: Too dangerous to alias that.\n", 36);
                return;
            }
            if (strcmp(argv_alias[0], "unalias") == 0) {
                write(STDERR_FILENO,
                    "alias: Too dangerous to alias that.\n", 36);
                return;
            }
            if (strcmp(argv_alias[0], "exit") == 0) {
                write(STDERR_FILENO,
                    "alias: Too dangerous to alias that.\n", 36);
                return;
            }
            if (search(aliases, argv_alias[0]) == NULL) {
                insert_to_end(aliases, argv_alias[0], argv_alias[1]);
            } else {
                search(aliases, argv_alias[0])->value = argv_alias[1];
            }
            return;
        }
        if (strncmp(dup_command, "unalias", 7) == 0) {
            char *tok_unalias = strtok(dup_command, " \t");
            if ((tok_unalias = strtok(NULL, " \t")) == NULL) {
                // no alias name specified
                write(STDERR_FILENO,
                    "unalias: Incorrect number of arguments.\n", 40);
                free(dup_command);
                return;
            }
            char *argv_unalias[100];
            int arg_unalias_index = 0;
            argv_unalias[arg_unalias_index++] = tok_unalias;
            if ((tok_unalias = strtok(NULL, "")) != NULL) {
                write(STDERR_FILENO,
                    "unalias: Incorrect number of arguments.\n", 40);
                free(dup_command);
                return;
            }
            if (delete(aliases, argv_unalias[0]) == NULL) {
                free(dup_command);
                return;
            }
            return;
        }

        // detect redirection
        int start_of_filename = 0;
        bool redirect_detected = false;
        int redirect_index;
        for (int i = 0; i < strlen(dup_command); i++) {
            if (dup_command[i] == '>') {
                // check for multiple >'s
                if (redirect_detected) {
                    write(STDERR_FILENO, "Redirection misformatted.\n", 26);
                } else {
                    redirect_detected = true;
                    redirect_index = i;
                }
            } else if (dup_command[i] == ' ' || dup_command[i] == '\t') {
                continue;
            } else {
                // if after > symbol, mark as beginning of filename
                if (redirect_detected) {
                    if (dup_command[i] != ' ' || dup_command[i] != '\t') {
                        start_of_filename = i;
                        break;
                    }
                } else {
                    continue;
                }
            }
        }
        if (start_of_filename == 0 && redirect_detected) {
            redirect_detected = false;
            write(STDERR_FILENO, "Redirection misformatted.\n", 26);
            return;
        }
        // redirect detected, so extract name of file and handle FDs

        if (redirect_detected) {
            char filename[128];
            int i;
            // extract name of batch file
            for (i = start_of_filename; i < strlen(dup_command); i++) {
                if (dup_command[i] == ' ') {
                    write(STDERR_FILENO, "Redirection misformatted.\n", 26);
                    return;
                }
                filename[i-start_of_filename] = dup_command[i];
            }
            filename[i] = '\0';
            // attempt to open file
            fp2 = fopen(filename, "w");
            if (fp2 == NULL) {
                write(STDERR_FILENO, "Cannot write to file ", 21);
                write(STDERR_FILENO, filename, strlen(filename));
                write(STDERR_FILENO, "\n", 1);
            }
            old_stdout = dup(STDOUT_FILENO);
            if (old_stdout == -1) {
                return;
            }
            int dup2_ret = dup2(fileno(fp2), STDOUT_FILENO);
            if (dup2_ret == -1) {
                return;
            }
        }
        // beginning of argument parsing with strtok
        char *argv[100];
        if (redirect_detected) {
            dup_command[redirect_index] = '\0';
        }
        char *tok = strtok(dup_command, " \t");
        if (tok == NULL && redirect_detected) {
            write(STDERR_FILENO, "Redirection misformatted.\n", 26);
        }
        int arg_index = 0;
        // parse command, add to argv array
        while (tok != NULL) {
            argv[arg_index] = tok;
            tok = strtok(NULL, " \t");
            arg_index++;
        }
        // set last argument to NULL
        argv[arg_index] = NULL;

        pid_t pid = fork();
        if (pid == 0) {
            if (execv(argv[0], argv) == -1) {
                write(STDERR_FILENO, argv[0], strlen(argv[0]));
                write(STDERR_FILENO, ": Command not found.\n", 21);
            }
            free(dup_command);
            _exit(1);
        } else {
            free(dup_command);
            int status;
            waitpid(pid, &status, 0);
            if (redirect_detected) {
                dup2(old_stdout, STDOUT_FILENO);
                fclose(fp2);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    // set up struct for aliases
    aliases = malloc(sizeof(struct linked_list));
    aliases->head = NULL;
    aliases->tail = NULL;
    // run in interactive mode
    if (argc == 1) {
        fp1 = stdin;
        char str[128];
        while (1) {
            write(STDOUT_FILENO, "mysh> ", 6);
            if (fgets(str, 128, fp1) != NULL) {
                    if (strncmp(str, "exit", 4) == 0) {
                        break;
                    }
                    handle_command(str);
            } else {
                break;
            }
        }
    } else if (argc == 2) {
        fp1 = fopen(argv[1], "r");
        // batch file is invalid
        if (fp1 == NULL) {
            write(STDERR_FILENO, "Error: Cannot open file ", 24);
            write(STDERR_FILENO, argv[1], strlen(argv[1]));
            write(STDERR_FILENO, ".\n", 2);
            free(aliases);
            exit(1);
        } else {
            char str[128];
            char* command;
            while ((command = fgets(str, 128, fp1)) != NULL) {
                write(STDOUT_FILENO, command, strlen(command));
                if (strncmp(command, "exit", 4) == 0) {
                    break;
                }
                handle_command(str);
            }
        }
    } else {
        write(STDERR_FILENO, "Usage: mysh [batch-file]\n", 25);
        free(aliases);
        exit(1);
    }
    fclose(fp1);
    return 0;
}
