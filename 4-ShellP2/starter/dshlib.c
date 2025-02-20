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

            rc = build_cmd_buff(cmd_buff, &cmd);

            if (rc == 0) {
                rc = exec_cmd(&cmd);
            }

            clear_cmd_buff(&cmd);
            free_cmd_buff(&cmd);
        
    
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
    cmd_buff->argc = 0;
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
    
    bool hasTokened = false;

    char delim[] = {SPACE_CHAR, '\0'};
    char* saveptr;
    char* currentArg;

    char quote;

    if (cmd_line[0] == '"' || cmd_line[0] == '\'') { // If a quote find the rest of the quote
        quote = cmd_line[0];
        cmd_line[0] = '\0';
        cmd_line ++;
        currentArg = cmd_line;
        cmd_line = strchr(cmd_line, quote);
        if (cmd_line == NULL) return -1; //Find the correct Error
        cmd_line[0] = '\0';
        cmd_line ++;
        while (cmd_line[0] == SPACE_CHAR && cmd_line[0] != '\0') {
            cmd_line ++;
        }
        saveptr = cmd_line;    
    }
    else {
        currentArg = strtok_r(cmd_line, delim, &saveptr);
        hasTokened = true;
    }
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
        if (saveptr != NULL && (saveptr[0] == '"' || saveptr[0] == '\'')) { // If a quote find the rest of the quote
            quote = saveptr[0];
            saveptr[0] = '\0';
            saveptr ++;
            currentArg = saveptr;
            saveptr = strchr(saveptr, quote);
            if (saveptr == NULL) return -1; //Find the correct Error
            saveptr[0] = '\0';
            saveptr ++;
            while (saveptr[0] == SPACE_CHAR && saveptr[0] != '\0') {
                saveptr ++;
            }
        }
        else {
            if (hasTokened) currentArg = strtok_r(NULL, delim, &saveptr);
            else {
                currentArg = saveptr;
                currentArg = strtok_r(currentArg, delim, &saveptr);
            }
        }
    }
    cmd_buff->argv[argCount] = NULL;
    cmd_buff->argc = argCount;
    if (argCount == 0) return WARN_NO_CMDS;
    return OK;

}

int exec_cmd(cmd_buff_t *cmd) {

    int rc = match_command(cmd->argv[0]);
    if (rc != BI_NOT_BI) {
        rc = exec_built_in_cmd(cmd);
    }
    else {
        int PID = fork();
        if (PID == 0) {
            execvp(cmd->argv[0], cmd->argv);
        }
        else {
            waitpid(PID, &rc, 0);
        }
    }
}

Built_In_Cmds match_command(const char *input) {
    if (strcmp(input, EXIT_CMD) == 0) {
        return BI_CMD_EXIT;
    }
    else if (strcmp(input, "dragon") == 0) {
        return BI_CMD_DRAGON;
    }
    else if (strcmp(input, "cd") == 0) {
        return BI_CMD_CD;
    }
    else return BI_NOT_BI; 
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    int rc = match_command(cmd->argv[0]);
    if (rc == BI_CMD_EXIT) {
        exit(0);
    }
    else if (rc == BI_CMD_DRAGON) {
        return OK;
    }
    else if (rc == BI_CMD_CD) {
        if (cmd->argc == 2) {
            rc = chdir(cmd->argv[1]);
            if (rc == -1) return ERR_CMD_ARGS_BAD;
            else return OK;
        }
    }
}

