#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/types.h>

#include "process_handler.h"
#include "common_structs.h"
#include "utils.h"



// below is the SIGINT handler for the parent
// it prints new line and clears out the stdout
void sigint_handler(int sig_num) {
    printf("\n## JCshell [%d] ##  ", getpid());
    fflush(stdout);

}



// This handler will be invoked when SIGUSR1 is received.
// It is used to synchronize child processes.
void sigusr1_handler(int signum) {}


// This method is used for the execution of a command
// Command is passed, and then execvp is used for execution

void executeCommand(CommandStruct *cmd) {
    
    // +2 because one for the command itself and one for the NULL terminator
    char *args[MAX_ARGS + 2];

    // First argument is the command itself
    args[0] = cmd->command;

    // Copy the arguments from the CommandStruct to the args array
    for (int i = 0; i < cmd->arg_count; i++) {
        args[i + 1] = cmd->arguments[i];
    }

    // Last element should be a NULL pointer
    args[cmd->arg_count + 1] = NULL;

    // Execute the command using execvp
    execvp(args[0], args);  // Note the change here

    // if execvp is successful, then this will not get executed.
    fprintf(stderr, "JCshell: \'%s\': %s\n", cmd->command, strerror(errno));
    
    exit(EXIT_FAILURE);
}


// Below method will take all the processinfo from /proc/{pid}/stat
// It will return ProcessInfoStruct which stores all the values
// It stores the values that are specified in the requirements

ProcessInfoStruct getProcessInfo(int pid) {
    char path[256];
    char line[256];
    unsigned long int utime, stime;
    FILE *file;

    ProcessInfoStruct pInfo = {0}; // Initialize the struct to zero

    sprintf(path, "/proc/%d/stat", pid);
    file = fopen(path, "r");

    if (!file) {
        perror("Error opening stat file");
        return pInfo;
    }


    fscanf(file, "%d %s %c %*d %d", &pInfo.pid, pInfo.cmd, &pInfo.state, &pInfo.ppid);
    for (int i = 0; i < 10; i++) { // Skip to utime
        fscanf(file, "%*s");
    }
    fscanf(file, "%lu %lu", &utime, &stime);
    
    fclose(file);

    // Convert jiffies to seconds for user_time and sys_time
    pInfo.user_time = (double) utime/sysconf(_SC_CLK_TCK);
    pInfo.sys_time = (double) stime/sysconf(_SC_CLK_TCK);


    // Reading stats from /proc/{pid}/status
    sprintf(path, "/proc/%d/status", pid);
    file = fopen(path, "r");
    if (!file) {
        perror("Error opening status file");
        return pInfo;
    }

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "voluntary_ctxt_switches: %ld", &pInfo.vctx) == 1) {
            // vctx now contains the number of voluntary context switches
        } else if (sscanf(line, "nonvoluntary_ctxt_switches: %ld", &pInfo.nvctx) == 1) {
            // nvctx now contains the number of non-voluntary context switches
        }
    }

    fclose(file);

    return pInfo;

}