#ifndef SOURCE_H
#define SOURCE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define LEN 1024
#define WORDS_IN_CMD 500

#define REDIRECT1 1 // >
#define REDIRECT2 2 // >>
#define REDIRECT_STDERR 3 // 2>

typedef struct Variable {
    char name[1024];
    char value[1024];
    struct Variable * next;
    
}var;

var * head = NULL; // Head of variables' linked list
char prompt[LEN]; // The shell's prompt
int status = 0; // Store every command exiting status
pid_t shell_pid; // Contains he shell process id
int in_if_fgets = 0; // State to know when pressing Ctrl+C not to print "You types Control-C"
int interrupt_fgets = 0; // To stop fgets in if command if Ctrl+C is pressed

void sigint_handler(int); // Handle Ctrl+c press
void handle_argv(char*); // Organizing the command to argv arry
void run(char*[]); // Executing the command
void pipe_func(char*[], int); // Runs shell command with endless |
void redirect1(char*[]); // Runs shell command with >
void redirect2(char*[]); // Runs shell command with >>
void redirect3(char*[]); // Runs shell command with 2>
void cd_command(char*[]); // Runs cd command
void redirect_func(char*[]); // Addressing the command to it's redirect function
void echo_func(char*[]); // Handle the echo command and it's special cases
void var_func(char*[]); // Create and add the shell's variables to linked list
void free_vars(); // Free all the allocated space for the variables' linked list
void if_func(char*); // Handle the if/else command


#endif