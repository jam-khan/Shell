#ifndef SHELL_STRUCTS_H
#define SHELL_STRUCTS_H

/**
 * Struct representing a command and its arguments.
 *
 * This struct is used to store a single command with its arguments.
 * It includes an array to hold the command, a 2D array to hold
 * each argument, and an integer to count the number of arguments.
 */
typedef struct {
    char command[100];           // The command string
    char arguments[100][100];    // Array of arguments for the command
    int arg_count;               // Number of arguments
} CommandStruct;

/**
 * Struct representing a collection of commands.
 *
 * This struct is used to store multiple commands, each of which
 * is a CommandStruct. It includes an array of CommandStructs and
 * an integer to count the total number of commands.
 * The array size is set to 30 to accommodate a maximum of 5 expected commands,
 * with additional space for safety.
 */
typedef struct {
    CommandStruct commands[30];  // Array of CommandStructs
    int commandCount;            // Total number of commands
} AllCommandsStruct;

/**
 * Struct for storing process information.
 *
 * This struct includes various fields that represent different
 * statistics and information about a process, such as its PID,
 * command name, state, parent PID, CPU times, and context switch counts.
 */
typedef struct {
    int pid;                     // Process ID
    char cmd[256];               // Command executed by the process
    char state;                  // Current state of the process
    int ppid;                    // Parent Process ID
    double user_time;            // User CPU time used
    double sys_time;             // System CPU time used
    long int vctx;               // Number of voluntary context switches
    long int nvctx;              // Number of nonvoluntary context switches
} ProcessInfoStruct;

/**
 * Struct for storing process statistics.
 *
 * This struct is used to store statistical data about processes
 * in a string format. It includes a large character array to hold
 * the data.
 */
typedef struct {
    char data[1024];             // String to hold process statistics
} String;

#endif // SHELL_STRUCTS_H
