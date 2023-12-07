#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>
#include <time.h>

#include "common_structs.h"// This is the header file for common structs
#include "command_parser.h"
#include "utils.h"
#include "process_handler.h"

// Below is the maximum length of a command keyword
// and maximum length of any argument
#define CMD_ARG_LENGTH 100 // length for storing the command in struct CommandStruct
#define MAX_ARGS 100 // length for maximum arguments for each command 
volatile sig_atomic_t child_ready = 0; // variable to inform parent that child is ready and then, send usr1 signal
int current_stat = 0; // global variable used to store the current stats index
String stats[5]; // global variable used to store the stats


// Below is the main driver function

int main() {
    char input[1024]; // stores the input from the terminal
    pid_t shell_pid = getpid(); // pid of shell
    int pipes[4][2]; // pipes for piping implementation

    signal(SIGINT, sigint_handler); // shell process (parent) gets SIGINT handler 


    while (1) {

        printf("## JCshell [%d] ##  ", shell_pid);
        if (fgets(input, sizeof(input), stdin) == NULL) {
            if (feof(stdin)) {
                // for some reason, if a weird input comes, then this will be an additional check
                printf("\nJCshell: End-of-file detected\n");
                clearerr(stdin); // clears input stream to start fresh on next input line
                continue;
            } else if (ferror(stdin)) {
                // Error in reading, possibly due to an interrupt
                perror("JCshell: Error reading input"); 
                
                // Clear the error and retry
                clearerr(stdin);
                memset(input, 0, sizeof(input));
                continue;
            }
        }

        // sets newline character at the end
        size_t len = strlen(input); 

        if (len > 0 && input[len-1] == '\n') {
            input[len-1] = '\0';  // Remove the newline character
        }


        AllCommandsStruct commands; // Assuming a maximum of 30 commands
        
        commands = extractAllCommands(input); // extracts all commands from the input line entered by user
        
        // checks if command is valid and commandscount was greater than 0
        // this is to make sure that we don't end up mentioning pipes for a single command wrong input
        if (isCommandsValid(commands) == 0 && commands.commandCount > 0) {
            printf("JCshell: should not have two | symbols without in-between command\n");
            clearerr(stdin);
            memset(input, 0, sizeof(input));
            continue;
        }

        // If more than 5 pipes are entered, then following alert is given
        if (commands.commandCount > 5){
            printf("JCshell: execution of a maximum of 5 commands is allowed. %d commands were entered\n", commands.commandCount);
            clearerr(stdin);
            memset(input, 0, sizeof(input));
            continue;
        }

        // Creates pipes for execution of the commands
        int pipe_creation_failure = 0;
        for (int i = 0; i < commands.commandCount - 1; i++){
            if (pipe(pipes[i]) < 0) {
                pipe_creation_failure = 1;
                perror("JCshell: Pipe creation failed");
                // exit(EXIT_FAILURE); 
            }
        }

        // start fresh if pipe creation was failed for some reason
        if (pipe_creation_failure == 1)
            continue;

        // array to store child_pids
        pid_t child_pids[5];
        int failureExit = 0; // this keeps track if any argument is wrong and, therefore exits and starts new shell line

        for (int i = 0; i < commands.commandCount; i++) {
            CommandStruct cmd = commands.commands[i];

            if (strcmp(cmd.command, "exit") == 0) {
                if (cmd.arg_count > 0) {
                    printf("JCshell: \"exit\" with other arguments!!\n");
                    failureExit = 1; // setting failure true before continue to continue outer loop as well
                    continue;
                } else if (cmd.arg_count == 0) {
                    printf("JCshell: Terminated\n");
                    exit(0);
                }
            }



            pid_t pid = fork(); // creating child process
            
            // VERY IMPORTANT: this helps differentiate the SIGINT handler for child and parent
            // child inherits the handler from parent, hence we ignore it

            signal(SIGINT, SIG_IGN);

            // child_pids[i] = pid;
            if (pid < 0) {
                // Error occurred
                fprintf(stderr, "fork() Failed during child proces creation");
                exit(-1);
            } else if (pid == 0) {


                child_pids[i] = getpid(); // assigning child pid in case it doesn't gets assigned under parent process

                signal(SIGUSR1, sigusr1_handler); // SIGUSR1 handler assiged
                signal(SIGINT, SIG_DFL); // DEFAULT SIGINT is assigned for the child process

                if (i != 0) {
                    close(pipes[i - 1][1]); 
                    if (dup2(pipes[i - 1][0], STDIN_FILENO) < 0){
                        perror("dup2 failed");
                    }
                }

                if (i != commands.commandCount - 1){
                    close(pipes[i][0]); 
                    if (dup2(pipes[i][1], STDOUT_FILENO) < 0) {
                        perror("dup2 failed");
                    }
                }

                sigset_t mask; // signal set variable
                sigfillset(&mask); // variable gets assigned all the signal sets
                sigdelset(&mask, SIGUSR1); // SIGUSR1 is removed from the set
                
                sigsuspend(&mask); // all the signals in the set are suspended
                // only SIGUSR1 is used

                
                executeCommand(&cmd); // command is executed finally

            } else {
                child_pids[i] = pid; // assigns child pid

                // if i > 0, then pipe needs closure as pipe[i - 1] has been used already
                if (i > 0){
                    close(pipes[i - 1][1]);
                    close(pipes[i - 1][0]);
                }
            }
        }
        // in case failure exit is true due to some child process gone wrong, then new shell line is created fresh!
        if (failureExit)
            continue;
        
        // force sleep to get all children ready
        sleep(1);

        // pipes are closed by the parent process on its side
        for (int k = 0; k < commands.commandCount - 1; k++) {
            close(pipes[k][0]);
            close(pipes[k][1]);
        }

        
        
        // Parent starts collecting childs
        for (int k = 0; k < commands.commandCount; k++) {
            int status;
            siginfo_t info;
            
            // SIGUSR1 signal is sent to child process to start its execution etc
            kill(child_pids[k], SIGUSR1);

            // waitid allows child to enter into zombie state instead of terminated state, so that we can extract process info
            int ret = waitid(P_PID, child_pids[k], &info, WNOWAIT | WEXITED);
            signal(SIGINT, sigint_handler); // handler is reassigned to parent process on safe side since we ignored on the child side
            
            
            // if ret is not -1, then it means success
            if (ret != -1) {
                ProcessInfoStruct process_info = getProcessInfo(child_pids[k]); // we extract process info of the child with child_pids[k]

                waitpid(child_pids[k], &status, 0); // process goes into terminated state, and we get the status

                // Depending on whether Process gets exited normally or a signal
                if (WIFEXITED(status) && WEXITSTATUS(status) != 1) {
                    char buffer[1024]; // buffer to store the process stats of that process
                    snprintf(buffer, 1024, "\n(PID)%d (CMD)%s (STATE)%c (EXCODE)%d (PPID)%d (USER)%.2f (SYS)%.2f (VCTX)%lu (NVCTX)%lu\n",
                        process_info.pid, process_info.cmd, process_info.state, WEXITSTATUS(status), process_info.ppid, process_info.user_time, process_info.sys_time, process_info.vctx, process_info.nvctx);
                    addString(stats, &current_stat, buffer); // process stats are added dynamically, since this is time dependent
                    // so it automatically gets ordered according to execution of the child process
                } else if (WIFSIGNALED(status)) {

                    char buffer[1024];
                    snprintf(buffer, 1024, "\n(PID)%d (CMD)%s (STATE)%c (EXSIG)%s (PPID)%d (USER)%.2f (SYS)%.2f (VCTX)%lu (NVCTX)%lu\n",
                        process_info.pid, process_info.cmd, process_info.state, strsignal(WTERMSIG(status)), process_info.ppid, process_info.user_time, process_info.sys_time, process_info.vctx, process_info.nvctx);   
                    addString(stats, &current_stat, buffer);
                }
                
            } else {
                perror("JCshell : error waitid");
                // something goes wrong, error is printed
            }
        }
        
        // Once execution is done, then we print the stats sequentially

        for (int j = 0; j < current_stat; j++)
            printf("%s", stats[j].data);
        
        // reseting stats array 
        current_stat = 0;
    }

    return 0;
}
