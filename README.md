# mysh

## Overview
Toy UNIX shell with support for interactive/batch modes, aliasing & redirection. Once you enter a command, the shell forks a new child process, `exec()`'s the command while the parent `wait()`'s for its completion before continuing to the next iteration.

## Interactive/Batch Modes
Running in interactive mode:
```bash
./mysh
```
Then wait for the shell to start before typing your commands.
```bash
mysh>
```

Running in batch mode:
```bash
./mysh batch_file
```

Your batch file should contain a list of commands, each on their own line, to be executed.

## Redirection
You can also redirect output to a file instead of stdout.
```bash
./mysh ls -la > output
```
(This runs the `ls -la` command and writes the output to the file `output.`)

## Aliasing

To create an alias, run `alias <command> <alias-name>`. If `alias-name` was previously used, it is replaced.

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

### Implementation Notes
- Aliasing was implemented using a doubly-linked list to support arbitrary deletion. 
- Most of the shell implementation is in `mysh.c` while aliasing functionality is defined in `myalias.c`.
