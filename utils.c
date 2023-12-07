#include <stdio.h>
#include <unistd.h>
#include "sys/wait.h"
#include <stdlib.h>
#include <string.h>

#include "common_structs.h"

void addString(String stats[], int *current_stat, const char *str) {
    if (*current_stat < 5) { // Assuming the size of stats array is 5
        strncpy(stats[*current_stat].data, str, 1024 - 1);
        stats[*current_stat].data[1024 - 1] = '\0'; // Ensuring null-termination
        (*current_stat)++;
    }
}

int isCommandsValid(AllCommandsStruct commands) {

    for(int i = 0; i < commands.commandCount; i++)
        if (commands.commands[i].arg_count < 0)
            return 0;
    
    return 1;

}
