# linux-shell

## Overview
This is a toy UNIX shell that operates like any command-line interpreter you are used to. It's essentially a really long while loop that continuously prompts you for commands that it will execute for you. Once you enter a command, the shell forks a new child process, `exec()`'s the command while the parent `wait()`'s for its completion before continuing to the next iteration.

## Interactive/Batch Modes
To run in interactive mode:
```bash
./mysh
```
Then wait for the shell to start before typing your commands.
```bash
mysh>
```

To run in batch mode:
```bash
./mysh batch_file
```

Your batch file should contain a list of commands, each on their own line, to be executed.

## Redirection
If you prefer output to be sent to a file rather than standard output you can run a command via with redirection.
```bash
./mysh ls -la > output
```
This runs the `ls -la` command and writes the output to the file `output.`

## Aliasing

To create an alias, run `alias <command> <alias-name>`. This will set up an alias between `command` and `alias-name`. If `alias-name` was previously used for another alias, it is replaced.

```bash
mysh> alias ll /bin/ls -l -a
```

To see all aliases:
```bash
mysh> alias
```

To view a single alias:
```bash
mysh> alias <alias-name>
```
If `alias-name` exists, the shell will display its replacement value.

To unalias:
```bash
mysh> unalias <alias-name>
```

### Implementation
This program was written completely in C and makes use of basic Linux functionality i.e. file descriptors, parent/child processes, fork/execv commands, and others. Aliasing was implemented using a doubly-linked list to support arbitrary deletion. Most of the shell implementation is in `mysh.c` while aliasing functionality is defined in `myalias.c`.
