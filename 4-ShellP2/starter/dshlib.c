#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
int exec_local_cmd_loop()
{
    char *cmd_buff = malloc(CMD_MAX * SH_CMD_MAX);
    if (cmd_buff == NULL) {
        return ERR_MEMORY;
    }
    int rc = 0;
    cmd_buff_t cmd;

    // TODO IMPLEMENT MAIN LOOP
    while(1){
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
            printf("\n");
            break;
        }
        else {
            //remove the trailing \n from cmd_buff
            cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
        
    

    // TODO IMPLEMENT parsing input to cmd_buff_t *cmd_buff

        }
    }
    // TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
    // the cd command should chdir to the provided directory; if no directory is provided, do nothing

    // TODO IMPLEMENT if not built-in command, fork/exec as an external command
    // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"

    return OK;
}

int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i] = malloc(ARG_MAX);
        if (cmd_buff->argv[i] == 0) {
            return ERR_MEMORY;
        }
    }
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX);
    if (cmd_buff->_cmd_buffer == NULL) {
        return ERR_MEMORY;
    }
    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff) {
    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        free(cmd_buff->argv[i]);
    }
    free(cmd_buff->_cmd_buffer);
    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->argc = NULL;
    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i] = NULL;
    }
    cmd_buff->_cmd_buffer = NULL;
    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    int end = strlen(cmd_line) - 1;
    while (cmd_line[end] == SPACE_CHAR) {
        end --;
    }
    cmd_line[end + 1] = '\0';
    while (cmd_line[0] == SPACE_CHAR && cmd_line[0] != '\0') {
        cmd_line ++;
    }

    if (strlen(cmd_line) > SH_CMD_MAX) {
        return ERR_CMD_OR_ARGS_TOO_BIG;
    }
    
    int rc = OK;
    rc = alloc_cmd_buff(cmd_buff);
    if (rc != OK) {
        return rc;
    } 

    strcpy(cmd_buff->_cmd_buffer, cmd_line);

    char* delim = {SPACE_CHAR};
    char* currentArg = strtok(cmd_line, &delim);
    int argCount = 0;
    if (currentArg != NULL && strlen(currentArg) > EXE_MAX) {
        return ERR_CMD_OR_ARGS_TOO_BIG;
    }
    while (currentArg != NULL) {
        argCount ++;
        if (argCount > CMD_ARGV_MAX) {
            return -1; //Find the correct error
        }
        if (strlen(currentArg) > ARG_MAX) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        strcpy(cmd_buff->argv[argCount - 1], currentArg);
    }
    if (argCount == 0) return WARN_NO_CMDS;
    return OK;

}
