#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

static int last_rc = 0;

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
    command_list_t clist;

    while(1){
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
            printf("\n");
            break;
        }
        else {
            cmd_buff[strcspn(cmd_buff,"\n")] = '\0';

            rc = build_cmd_list(cmd_buff, &clist);

            if (rc == OK) {
                if (clist.num > 1) rc = execute_pipeline(&clist);
                else rc = exec_cmd(&(clist.commands[0]));
                if (rc == OK_EXIT) return OK;
            }

            free_cmd_list(&clist);
        
    
        }
    }

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

char* search_for_quotes(char* str) {
    while (*str != '\'' && *str != '"') {
        if (*str == '\0') return NULL;
        else str++;
    }
    return str;
}

char* find_end_quote(char *address_of_start_quote, char quote_char) {
    if (*address_of_start_quote == quote_char) address_of_start_quote++;
    while (*address_of_start_quote != quote_char) {
        if (*address_of_start_quote == '\0') return NULL;
        else address_of_start_quote++;
    }
    return address_of_start_quote;
}

char* extract_tok_from_quotes(char **saveptr) {
    char* tok;
    char quote = (*saveptr)[0];

    quote = (*saveptr)[0];
    (*saveptr)[0] = '\0';
    (*saveptr) ++;
    tok = (*saveptr);
    (*saveptr) = strchr((*saveptr), quote);
    if ((*saveptr) == NULL) return NULL; //Find the correct Error
    (*saveptr)[0] = '\0';
    (*saveptr) ++;
    while ((*saveptr)[0] == SPACE_CHAR && (*saveptr)[0] != '\0') {
        (*saveptr) ++;
    }
    return tok;
}

char not_in_quotes(char* left_bound, char* currentLoc) {
    //printf("Init Left Bound: %u\n", left_bound);
    //printf("Delim Loc: %u\n", currentLoc);
    while (left_bound != NULL && left_bound < currentLoc) {
        //printf("Loop can run\n");
        left_bound = search_for_quotes(left_bound);
        //printf("Left Bound #1: %u\n", left_bound);
        if (left_bound == NULL) return 1;
        left_bound = find_end_quote(left_bound, *left_bound);
        //printf("Left Bound #2: %u\n", left_bound);
        left_bound ++;
    }
    //printf("Leave loop\n");
    if (left_bound == NULL) return 0;
    else return 1;
}

char* cmdtok(char** saveptr, char delim) {
    char* strStart = *saveptr;
    if (*strStart == '\0') {
        return NULL;
    }
    char* delimLoc = strchr(*saveptr, delim);
    if (delimLoc != NULL) {
        while (not_in_quotes(strStart, delimLoc) == 0) {
            //printf("check");
            if (*delimLoc != '\0') {
                //printf("test1");
                delimLoc++;
                *saveptr = delimLoc;
                //printf("test2");
                delimLoc = strchr(*saveptr, delim);
                //printf("test");
            }
            else return strStart;
        }
        *delimLoc = '\0';
        *saveptr = delimLoc + 1;
    }
    else {
        delimLoc = *saveptr;
        while(*delimLoc != '\0') {
            delimLoc++;
        }
        *saveptr = delimLoc;
    }
    return strStart;
}

int build_cmd_list(char *cmd_line, command_list_t *clist) {
    int rc = OK;
    int cmdNum = 0;
    char* saveptr = cmd_line;
    char* currentCommand;

    //currentCommand = cmdtok(&saveptr, PIPE_CHAR);
    currentCommand = strtok_r(cmd_line, PIPE_STRING, &saveptr);
    while (currentCommand != NULL) {
        cmdNum ++;
        if (cmdNum > CMD_MAX) return ERR_TOO_MANY_COMMANDS;
        rc = build_cmd_buff(currentCommand, &(clist->commands[cmdNum - 1]));
        //printf("Command %d: ", cmdNum);
        //for (int i = 0; i < clist->commands[cmdNum - 1].argc; i++) 
        //    printf("%s ", clist->commands[cmdNum - 1].argv[i]);
        //printf("\n");
        if (rc != OK && rc != WARN_NO_CMDS) return rc;
        //currentCommand = cmdtok(&saveptr, PIPE_CHAR);
        currentCommand = strtok_r(NULL, PIPE_STRING, &saveptr);
    }

    clist->num = cmdNum;
    return OK;
}

int free_cmd_list(command_list_t *cmd_lst) {
    int rc = OK;
    for (int i = 0; i < cmd_lst->num; i++) {
        rc = clear_cmd_buff(&(cmd_lst->commands[i]));
        if (rc != OK) return rc;
        rc = free_cmd_buff(&(cmd_lst->commands[i]));
    }
    return rc;
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
    char* saveptr = cmd_buff->_cmd_buffer;
    char* currentArg;

    if (saveptr[0] == '"' || saveptr[0] == '\'') { // If a quote find the rest of the quote
       currentArg = extract_tok_from_quotes(&saveptr);
       if (currentArg == NULL) return -1; //Find the correct Error
    }
    else {
        currentArg = strtok_r(cmd_buff->_cmd_buffer, delim, &saveptr);
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
            currentArg = extract_tok_from_quotes(&saveptr);
            if (currentArg == NULL) return -1; //Find the correct Error
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
        if (PID == -1) {
            return ERR_MEMORY;
        }
        else if (PID == 0) {
            execvp(cmd->argv[0], cmd->argv);
            exit(errno);
        }
        else {
            waitpid(PID, &rc, 0);
            if (WIFEXITED(rc)) {
                rc = WEXITSTATUS(rc);
                if (rc != 0) printf("%s\n", strerror(rc));
            }
            else if (WIFSIGNALED(rc)) rc = WTERMSIG(rc);
            last_rc = rc;
        }
    }
    last_rc = rc;
    return rc;
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
    else if (strcmp(input, "rc") == 0) {
        return BI_RC;
    }
    else return BI_NOT_BI; 
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    int rc = match_command(cmd->argv[0]);
    if (rc == BI_CMD_EXIT) {
        printf("exiting...\n");
        return OK_EXIT;
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
    else if (rc == BI_RC) {
        printf("%d\n", last_rc);
    }
}

int execute_pipeline(command_list_t *clist) {
    int pipes[clist->num - 1][2];  // Array of pipes
    pid_t pids[clist->num];        // Array to store process IDs
    int rc = OK;

    // Create all necessary pipes
    for (int i = 0; i < clist->num - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return ERR_EXEC_CMD;
        }
    }

    // Create processes for each command
    for (int i = 0; i < clist->num; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork");
            return ERR_EXEC_CMD;
        }

        if (pids[i] == 0) {  // Child process
            // Set up input pipe for all except first process
            if (i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO);
            }

            // Set up output pipe for all except last process
            if (i < clist->num - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            // Close all pipe ends in child
            for (int j = 0; j < clist->num - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // Execute command
            rc = match_command(clist->commands[0].argv[0]);
            if (rc != BI_NOT_BI) {
                rc = exec_built_in_cmd(&(clist->commands[i]));
                exit(rc);
            }
            else {
                execvp(clist->commands[i].argv[0], clist->commands[i].argv);
                exit(errno);
            }
        }
    }
    
    // Parent process: close all pipe ends
    for (int i = 0; i < clist->num - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for all children
    for (int i = 0; i < clist->num; i++) {
        waitpid(pids[i], &rc, 0);
        if (WIFEXITED(rc)) {
            rc = WEXITSTATUS(rc);
            if (rc != 0) printf("%s\n", strerror(rc));
        }
        else if (WIFSIGNALED(rc)) rc = WTERMSIG(rc);
        last_rc = rc;
    }

    return last_rc;
}

