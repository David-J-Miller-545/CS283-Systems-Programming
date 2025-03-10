#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int, int);
int  rev_chars(char *, int, int);
int  print_words(char *, int, int);
//add additional prototypes here


int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions 
    int new_word = 1;
    int done_copying = 0;

    int str_len = 0;

    int str_i = 0;
    int buff_i = 0;
    while (buff_i < len) {
        if (done_copying == 0) {
            if (*(user_str + str_i) == '\n' || *(user_str + str_i) == '\t') { // Handles other forms of whitespace in a string
                *(user_str + str_i) = ' ';
            }
            if (!(*(user_str + str_i) == ' ' && new_word == 1)) /* This if cond. takes care of duplicate spaces*/ {
                new_word = 0; // not a new word
                
                *(buff + str_len) = *(user_str + str_i);
                if (*(buff + str_len) == ' ') {
                    new_word = 1;
                }
                str_len ++; // increments length of the string in the buffer
                buff_i ++;
            }
            str_i++;
            if (*(user_str + str_i) == '\0') {
                done_copying = 1;
                if (*(buff + str_len - 1) == ' ') {
                    buff_i--;
                    str_len--;
                }
            }
        }   
        if (done_copying == 1) {
            *(buff + buff_i) = '.';
            buff_i ++;
        }        
    }
    if (done_copying) return str_len; // returns the length of the copied string in the buffer
    else return -1;
}

void print_buff(char *buff, int len){
    printf("Buffer:  [");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    printf("]\n");
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len, int isPrint){
    //YOU MUST IMPLEMENT
    if (str_len > len) {
        return -1;
    }
    else if (str_len == 0) {
        return 0;
    }

    int word_count = 0;
    int char_count = 0; // Doesn't matter if not printing
    int at_start = 1;

    for (int i = 0; i < str_len; i++) {
        char c = *(buff + i);
        if (at_start) {
            word_count++;
            at_start = 0;
            if (isPrint == 1) {
                printf("%d. ", word_count);
            }
            
        }

        if (c == ' ') {
            if (isPrint == 1) {
                printf("(%d)\n", char_count);
                char_count = 0; 
            }
            at_start = 1;
        }
        else if (isPrint == 1) {
            putchar(c);
            char_count++;
        }
    }
    if (isPrint == 1) {
        printf("(%d)\n", char_count);
        char_count = 0; 
    }
    return word_count;
}

int print_words(char *buff, int len, int str_len){
    //YOU MUST IMPLEMENT
    printf("Word Print\n----------\n");
    return count_words(buff, BUFFER_SZ, str_len, 1);
}

int rev_chars(char *buff, int len, int str_len){
    //YOU MUST IMPLEMENT
    char char_i = ' ';
    for (int i = 0; i < (int) str_len / 2; i++) {
        char_i = *(buff + i);
        *(buff + i) = *(buff + str_len - i - 1);
        *(buff + str_len - i - 1) = char_i;
    }
    return 0;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      This is safe because the first condition in the or statement verifies that there are more than 2
    //      arguments within the command, thus meaning if there wasn't, the second condition would not be checked
    //      and therefore not access bad memory.
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      The purpose of the statement below is to ensure that the command given has enough arguments to run
    //      the program.
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    buff = malloc(BUFFER_SZ*sizeof(char));

    if (buff == NULL) {
        exit(99);
    }


    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len, 0); // returns word count
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;
        case 'r':
            rc = rev_chars(buff, BUFFER_SZ, user_str_len);  // returns a return code
            if (rc < 0){
                printf("Error reversing characters, rc = %d", rc);
                exit(2);
            }
            break;
        case 'w':
            rc = print_words(buff, BUFFER_SZ, user_str_len);  // returns word count
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("\nNumber of words returned: %d\n", rc);
            break;
        case 'x':
            printf("Not Implemented!");
            exit(3);

        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    buff = NULL;
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
/*          It is good practice because we can verify that the length is less than the buff to ensure that our code
            is running as intended and that we don't start accessing bad memory.
*/