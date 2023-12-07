#ifndef SHELL_FUNCTIONS_H
#define SHELL_FUNCTIONS_H

#include <sys/types.h> // For pid_t
#include "common_structs.h" // Assuming this header defines CommandStruct and ProcessInfoStruct

// Constants
#define MAX_ARGS 100 // Adjust this as per your actual definition

// Function prototypes

/**
 * Signal handler for SIGINT.
 * 
 * This function is called when a SIGINT signal is received by the process.
 * It prints a new line and the shell prompt, and flushes stdout.
 * 
 * @param sig_num Signal number.
 */
void sigint_handler(int sig_num);

/**
 * Signal handler for SIGUSR1.
 * 
 * This handler is invoked when SIGUSR1 is received. It is used to
 * synchronize child processes.
 * 
 * @param signum Signal number.
 */
void sigusr1_handler(int signum);

/**
 * Executes a command based on the given CommandStruct.
 * 
 * This method takes a pointer to a CommandStruct, extracts the command and its arguments,
 * and then uses execvp to execute the command. If execvp is successful, this method does not return.
 * 
 * @param cmd Pointer to the CommandStruct containing the command to execute.
 */
void executeCommand(CommandStruct *cmd);

/**
 * Retrieves process information for a given PID.
 * 
 * This function reads from /proc/{pid}/stat and /proc/{pid}/status to obtain various
 * statistics about the process, such as the PID, command name, state, parent PID,
 * CPU times, and context switch counts.
 * 
 * @param pid The process ID for which to retrieve information.
 * @return A ProcessInfoStruct containing various statistics about the process.
 */
ProcessInfoStruct getProcessInfo(int pid);

#endif // SHELL_FUNCTIONS_H
