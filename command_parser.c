
#include <stdio.h>
#include <unistd.h>
#include "sys/wait.h"
#include <stdlib.h>
#include <string.h>

#include "common_structs.h"



// When a command and its arguments are broken down,
// then, this method is used for validation of a specific word
// in order to avoid empty values
int isValidWord(const char *word) {
    while (*word) {
        if (*word != ' ' && *word != '\t' && *word != '\n') {
            return 1; // means true
        }
        word++;
    }
    return 0; // returns false

}

// This takes the command line input and breaks down seperate commands
// It returns AllComandsStruct which stores an array of individual commands
// It splits line around '|' for parsing

CommandStruct extractCmdArgs(char line[1024]) {
    CommandStruct cmd = {0};  // Initialize the struct to zero

    char *words[124];  // Temporary storage for tokenized words
    int i = 0;
    char *token = strtok(line, " \n");  // Added \n to delimiters to handle newline characters

    while ((token != NULL) && i < 124) {  // Checking i to avoid overflow
        if (isValidWord(token)) {
            words[i] = token;
            i++;
        }
        token = strtok(NULL, " \n");
    }

    if (i > 0) {
        strncpy(cmd.command, words[0], sizeof(cmd.command) - 1);
        cmd.command[sizeof(cmd.command) - 1] = '\0';  // Ensure null termination
    }

    // i counts commands + arguments
    // arguments will be i - 1 (not counting the command)
    cmd.arg_count = i - 1;

    for (int j = 1; j < i; j++) {
        strncpy(cmd.arguments[j - 1], words[j], sizeof(cmd.arguments[j - 1]) - 1);
        cmd.arguments[j - 1][sizeof(cmd.arguments[j - 1]) - 1] = '\0';  // Ensure null termination
    }

    return cmd;
}

AllCommandsStruct extractAllCommands(char line[1024]) {
    AllCommandsStruct cmds = {0};
    cmds.commandCount = 0;
    char *cmdInputs[30];
    char* token = strtok(line, "|");
    int i = 0;

    while ((token != NULL) & (i < 30)) {
        cmdInputs[i] = token;
        i++;
        token = strtok(NULL, "|");
    }

    for(int j = 0; j < i; j++)
        cmds.commands[j] = extractCmdArgs(cmdInputs[j]);
    cmds.commandCount = i;
    
    return cmds;
}



