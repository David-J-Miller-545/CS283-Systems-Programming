#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

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
    int end = strlen(cmd_line) - 1;
    while (cmd_line[end] == SPACE_CHAR) {
        end --;
    }
    cmd_line[end] == '\0';
    while (cmd_line[0] == SPACE_CHAR && cmd_line != '\0') {
        cmd_line ++;
    }
    
    char* currentCommand = strtok(cmd_line, &PIPE_STRING);\
    char* currentArgs;
    int cmdCount = 0;
    while (currentCommand != NULL) {
        cmdCount ++;
        if (cmdCount > CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        end = strlen(currentCommand) - 1;
        while (currentCommand[end] == SPACE_CHAR) {
            end --;
        }
        currentCommand[end] == '\0';
        while (currentCommand[0] == SPACE_CHAR && currentCommand != '\0') {
            currentCommand ++;
        }

        if (strlen(currentCommand) > SH_CMD_MAX) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        currentArgs = strchr(currentCommand, SPACE_CHAR);
        if (currentArgs != NULL) {
            *currentArgs = '\0';
            currentArgs ++;
            end = strlen(currentArgs) - 1;
            while (currentArgs[end] == SPACE_CHAR) {
                end --;
            }
            currentArgs[end + 1] = '\0';
            while (currentArgs[0] == SPACE_CHAR && currentArgs != '\0') {
                currentArgs ++;
            }
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