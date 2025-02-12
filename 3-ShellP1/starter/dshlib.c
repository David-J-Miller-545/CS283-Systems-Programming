#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  remove_leading_and_trailing_chars
 *    str:              the string to modify
 *    char_to_remove:   the char the user wants to remove leading and trailing from str  
 * 
 * This function removes all leading and trailing char_to_remove from str.
 * 
 * returns: char* to the modified str
 */
char* remove_leading_and_trailing_chars(char* str, char char_to_remove)
{
    int end = strlen(str) - 1;
    while (str[end] == char_to_remove) {
        end --;
    }
    str[end + 1] = '\0';
    while (str[0] == char_to_remove && str[0] != '\0') {
        str ++;
    }
    return str;
}


/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    cmd_line = remove_leading_and_trailing_chars(cmd_line, SPACE_CHAR);
    
    char* currentCommand = strtok(cmd_line, &PIPE_STRING);\
    char* currentArgs;
    int cmdCount = 0;
    while (currentCommand != NULL) {
        cmdCount ++;
        if (cmdCount > CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }
        currentCommand = remove_leading_and_trailing_chars(currentCommand, SPACE_CHAR);
        if (strlen(currentCommand) > SH_CMD_MAX) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        currentArgs = strchr(currentCommand, SPACE_CHAR);
        if (currentArgs != NULL) {
            *currentArgs = '\0';
            currentArgs ++;
            currentArgs = remove_leading_and_trailing_chars(currentArgs, SPACE_CHAR);
        }
        if (strlen(currentCommand) > EXE_MAX) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        else if (currentArgs != NULL && strlen(currentArgs) > ARG_MAX) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        command_t newCommand;
        strcpy(newCommand.exe, currentCommand);
        if (currentArgs != NULL) strcpy(newCommand.args, currentArgs);
        else strcpy(newCommand.args, "");
        clist->commands[cmdCount - 1] = newCommand;
        currentCommand = strtok(NULL, &PIPE_STRING);
    }
    if (cmdCount == 0) return WARN_NO_CMDS;
    clist->num = cmdCount;
    return OK;
}


