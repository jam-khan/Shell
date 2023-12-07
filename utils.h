#ifndef SHELL_UTILS_H
#define SHELL_UTILS_H

#include "common_structs.h" // Include this if it defines String and AllCommandsStruct

/**
 * Adds a string to the stats array.
 *
 * This function appends a string to the stats array at the position
 * specified by the value pointed to by current_stat, then increments
 * current_stat. If current_stat is greater than or equal to 5, the
 * function does nothing (assuming the stats array size is 5).
 *
 * @param stats Array of String structs to which the string is added.
 * @param current_stat Pointer to the current index in the stats array.
 * @param str The string to be added to the stats array.
 */
void addString(String stats[], int *current_stat, const char *str);

/**
 * Checks the validity of commands in an AllCommandsStruct.
 *
 * This function checks if each CommandStruct inside the AllCommandsStruct
 * is valid. A CommandStruct is considered invalid if its arg_count is less
 * than zero, which indicates improper use of the pipe '|' symbol.
 *
 * @param commands The AllCommandsStruct instance to be checked.
 * @return 1 (true) if all commands are valid, 0 (false) otherwise.
 */
int isCommandsValid(AllCommandsStruct commands);

#endif // SHELL_UTILS_H
