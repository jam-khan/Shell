#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include "common_structs.h" // Assuming this header defines CommandStruct and AllCommandsStruct

#ifndef COMMAND_HANDLING_H
#define COMMAND_HANDLING_H

#include "common_structs.h" // Assuming this header defines CommandStruct and AllCommandsStruct

/**
 * Checks if the given string (word) contains non-whitespace characters.
 *
 * This function evaluates each character of the input string and
 * determines whether it contains characters other than spaces (' '),
 * tabs ('\t'), or newline characters ('\n'). It is used primarily to
 * validate the presence of meaningful content in a word or token.
 *
 * @param word A pointer to a null-terminated string to be checked.
 * @return Returns 1 (true) if the word contains non-whitespace characters,
 *         0 (false) otherwise.
 */
int isValidWord(const char *word);

/**
 * Extracts a command and its arguments from a given command line string.
 *
 * Parses a single command line input, tokenizing it based on spaces
 * and newline characters. It separates the command itself from its
 * arguments, storing them in a CommandStruct structure. The structure
 * includes the command, an array of arguments, and the count of arguments.
 *
 * @param line A string containing the command line input to be parsed.
 *             It should be null-terminated with a maximum length of 1024 characters.
 * @return A CommandStruct structure containing the parsed command and its arguments.
 */
CommandStruct extractCmdArgs(char line[1024]);

/**
 * Parses a command line input with multiple commands separated by pipes ('|').
 *
 * This function processes a command line string containing multiple commands,
 * each separated by a pipe character. It breaks down the input into individual
 * commands and uses extractCmdArgs to parse each command into its components.
 * The resulting commands and their arguments are stored in an AllCommandsStruct.
 *
 * @param line A string representing the command line input with multiple commands.
 *             The string should be null-terminated with a maximum length of 1024 characters.
 * @return An AllCommandsStruct containing an array of CommandStructs, each representing
 *         an individual parsed command, along with the count of total commands extracted.
 */
AllCommandsStruct extractAllCommands(char line[1024]);

#endif // COMMAND_HANDLING_H



#endif
