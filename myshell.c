#include "source.h"

int main() {
	char last_cmd[LEN], command[LEN], temp[LEN];

    shell_pid = getpid();

	struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT, &sa, NULL);

    strcpy(prompt, "hello:");

	while (1) {
        interrupt_fgets = 0;
	    printf("%s ", prompt);
        fgets(command, LEN, stdin);
        command[strlen(command) - 1] = '\0'; 

        if (strstr(command, "if ")) { 
            strcat(command, "; ");
            while (1) {
                in_if_fgets = 1;
                printf("> ");
                fgets(temp, LEN, stdin);
                if (interrupt_fgets) break;
                in_if_fgets = 0;
                if((strstr(temp, "else") || strstr(temp, "then"))) temp[strlen(temp) -1] = ' ';
                else {
                    temp[strlen(temp) -1] = ';';
                    temp[strlen(temp)] = ' ';
                }
                strcat(command, temp);
                if(strstr(temp, "fi")) break;
                memset(temp, '\0', sizeof(temp));
            }
            if(interrupt_fgets == 1) continue;
            strcpy(last_cmd, command);
        }
        if(!(command[0] == '!' && command[1] == '!')) {
            strcpy(last_cmd, command);
        }
        else{
            strcpy(command, last_cmd);
            puts(command);
        }

        handle_argv(command);
    }

    free_vars();
	return 0;
}

/**
 * This function is responsible for handling Ctrl+C
 * Input: signal (int)
 * Output: none
*/
void sigint_handler(int signal) {
    if (getpid() == shell_pid) {
        if (in_if_fgets) interrupt_fgets = 1;
        else printf("\nYou typed Control-C!\n");
    }
    else exit(0);
}

/**
 * This function is responsible for handling the user command, ans split it to argv 2D array that will contain the command like main's argv.
 * Input: command (char*)
 * Output: none
*/
void handle_argv(char *command) {
    int i = 0, return_id = 0, is_pipe = 0, redirect = 0, is_prompt = 0, exc_mark = 0;
    char *argv[WORDS_IN_CMD], *token;

    if(strstr(command, "if ")) {
        if_func(command);
        return;
    }

    token = strtok(command," ");
    while (token != NULL) {
        argv[i] = token;
        token = strtok (NULL, " ");
        if(strcmp(argv[i] , "|") == 0) is_pipe++;
        else if(strcmp(argv[i] , ">") == 0) redirect = REDIRECT1;
        else if(strcmp(argv[i] , ">>") == 0) redirect = REDIRECT2;
        else if(strcmp(argv[i] , "2>") == 0) redirect = REDIRECT_STDERR;
        i++;
    }
    argv[i] = NULL;

    // Special cases
    if (argv[0] == NULL) return;
    else if(strcmp(argv[0], "quit") == 0) {
        free_vars();
        exit(0);
    }
    else if(strcmp(argv[0] , "prompt") == 0) {
        strcpy(prompt, argv[2]);
        return;
    }
    else if(strcmp(argv[0] , "echo") == 0) {
        echo_func(argv);
        return;
    }
    else if (strcmp(argv[0], "cd") == 0) {
        cd_command(argv);
        return;
    }
    else if(argv[0][0] == '$') {
        var_func(argv);
        return;
    }


    // Special characters
    if(is_pipe) pipe_func(argv, is_pipe);
    else if(redirect != 0) {
        switch(redirect) {
            case REDIRECT1:
                redirect1(argv);
                break;
            case REDIRECT2:
                redirect2(argv);
                break;
            case REDIRECT_STDERR:
                redirect3(argv);
                break;
        }
    }
    else run(argv); 
}

/**
 * This function is responsible for running the command, and handle the ampercend case
 * Input: argv (char*[])
 * Output: none
*/
void run(char *argv[]) { 
    int i = 0, amper = 0;
    while(argv[i] != NULL) {
        if (strcmp(argv[i], "&") == 0) {
            amper = 1;
            argv[i] = NULL;
            break;
        }
        i++;
    }
    if (fork() == 0) { 
        execvp(argv[0], argv);
    }
    if (amper == 0) wait(&status);
}

/**
 * This function is responsible for handling cd command
 * Input: argv (char*[])
 * Output: none
*/
void cd_command(char *argv[]) {
    if (argv[1] != NULL) {
        if (chdir(argv[1]) != 0) perror("cd");
    }
}

/**
 * This function is responsible for handling command with redirect characters in it, and send it to it's function
 * Input: argv (char*[])
 * Output: none
*/
void redirect_func(char *argv[]) {
    int i = 0;
    while (argv[i] != NULL) {
            if(strcmp(argv[i] , ">") == 0) {
                redirect1(argv);
                return;
            }
            else if(strcmp(argv[i] , ">>") == 0) {
                redirect2(argv);
                return;
            }
            else if(strcmp(argv[i] , "2>") == 0) {
                redirect3(argv);
                return;
            }
			i++;
	}
    run(argv);
}

/**
 * This function is responsible for handling command with pipes in it (can handle endless pipes)
 * Input: argv (char*[]), num_of_pipes (int)
 * Output: none
*/
void pipe_func(char *argv[], int num_of_pipes) {
    char *commands[num_of_pipes+1][10];
    int i = 0, j = 0, cmd_num = 0, saved_stdin = dup(STDIN_FILENO), saved_stdout = dup(STDOUT_FILENO), pipes[num_of_pipes][2];

    while(argv[i] != NULL) {
        if(strcmp(argv[i], "|") == 0) {
            commands[cmd_num][j] = NULL;
            cmd_num++;
            j = 0;
            i++;
        }
        commands[cmd_num][j] = argv[i];
        i++;
        j++;
    }
    commands[cmd_num][j] = NULL;

    for(i = 0, j = 0; i <= num_of_pipes && j <= cmd_num; i++, j++){
        if(i != 0) {
            close(pipes[i-1][1]);
            close(pipes[i-1][0]);
        }
        if(j == cmd_num || i == num_of_pipes) {
            dup2(saved_stdout, STDOUT_FILENO);
            redirect_func(commands[j]);
            break;
        }

        pipe(pipes[i]);
        dup2(pipes[i][1], STDOUT_FILENO);
        close(pipes[i][1]);
        redirect_func(commands[j]);
        dup2(pipes[i][0], STDIN_FILENO);
    }
    dup2(saved_stdin, STDIN_FILENO);
    dup2(saved_stdout, STDOUT_FILENO);
}

/**
 * This function is responsible for handling command with >
 * Input: argv (char*[])
 * Output: none
*/
void redirect1(char *argv[]) {
    int i = 0, saved_stdout = dup(STDOUT_FILENO);
    char *src[10], *target;
    while(argv[i] != NULL) {
        if(strcmp(argv[i], ">") == 0) {
            src[i] = NULL;
            i++;
            target = argv[i];
        }
        src[i] = argv[i];
        i++;
    }
    
    FILE *fp = fopen(target, "w");
    dup2(fileno(fp), STDOUT_FILENO);
    run(src);
    fclose(fp);
    fflush(stdout);
    dup2(saved_stdout, STDOUT_FILENO);
}

/**
 * This function is responsible for handling command with >>
 * Input: argv (char*[])
 * Output: none
*/
void redirect2(char *argv[]) {
    int i = 0, saved_stdout = dup(STDOUT_FILENO);
    char *src[10], *target;
    while(argv[i] != NULL) {
        if(strcmp(argv[i], ">>") == 0) {
            src[i] = NULL;
            i++;
            target = argv[i];
        }
        else src[i] = argv[i];
        i++;
    }
    
    FILE *fp = fopen(target, "a+");
    dup2(fileno(fp), STDOUT_FILENO);
    run(src);
    fclose(fp);
    fflush(stdout);
    dup2(saved_stdout, STDOUT_FILENO);
} 

/**
 * This function is responsible for handling command with 2>
 * Input: argv (char*[])
 * Output: none
*/
void redirect3(char *argv[]) {
    int i = 0, saved_stderr = dup(STDERR_FILENO);
    char *src[10], *target;
    while(argv[i] != NULL) {
        if(strcmp(argv[i], "2>") == 0) {
            src[i] = NULL;
            i++;
            target = argv[i];
        }
        else src[i] = argv[i];
        i++;
    }
    
    FILE *fp = fopen(target, "w");
    dup2(fileno(fp), STDERR_FILENO);
    run(src);
    fclose(fp);
    fflush(stderr);
    dup2(saved_stderr, STDERR_FILENO);
}

/**
 * This function is responsible for handling the echo command and it's special cases
 * Input: argv (char*[])
 * Output: none
*/
void echo_func(char *argv[]) {
    if(strcmp(argv[1] , "$?") == 0) {
        if (WIFEXITED(status)) printf("%d\n", WEXITSTATUS(status));
        else if (WIFSIGNALED(status)) printf("%d\n", WTERMSIG(status)); 
        return;
    }
    else if(argv[1][0] == '$') {
        var * temp = head;
        int  i = 1;
        while(head != NULL && argv[i] != NULL) {
            if(strcmp(head->name, argv[i]) == 0) {
                puts(head->value);
                i++;
            }
            head = head->next;
        }
        head = temp;
    }
    else if(strcmp(argv[1] , "Enter") == 0 && strcmp(argv[2] , "a") == 0 && strcmp(argv[3] , "string") == 0) {
        char input[LEN], *token, temp[LEN], var_name[LEN], var_value[LEN], *argv_temp[LEN];
        fgets(input, LEN, stdin);
        input[strlen(input) - 1] = '\0';

        for(int i = 4, j = 0; i < LEN; i++, j++) {
            var_name[j] = input[i];
            if(input[i] == '\0') break;
        }
        var_name[0] = '$';
        argv_temp[0] = var_name;

        fgets(var_value, LEN, stdin);
        var_value[strlen(var_value) - 1] = '\0';
        argv_temp[2] = var_value;
        var_func(argv_temp);
    }
    else {
        if(argv[1][0] == '"' && argv[1][strlen(argv[1])-1] == '"'){
            char temp[LEN];
            for(int i = 1, j = 0; i < strlen(argv[1]) -1; i++, j++) {
                temp[j] = argv[1][i];
            }
            argv[1] = temp;
        }
        redirect_func(argv);
    }
}

/**
 * This function is responsible for creating and adding shell's variables to linked list
 * Input: argv (char*[])
 * Output: none
*/
void var_func(char *argv[]) {
    if(head == NULL) {
        head = (var*)malloc(sizeof(var));
        strcpy(head->name, argv[0]);
        strcpy(head->value, argv[2]);
        return;
    }
    var * temp = head;
    while(head != NULL) {
        if(strcmp(head->name, argv[0]) == 0) {
            strcpy(head->next->value, argv[2]);
            break;
        }
        if(head->next == NULL) {
            head->next = (var*)malloc(sizeof(var));
            strcpy(head->next->name, argv[0]);
            strcpy(head->next->value, argv[2]);
            break;
        }
        head = head->next;
    }
    head = temp;
}

/**
 * This function is responsible for freeing the allocated memory for shell's variables
 * Input: none
 * Output: none
*/
void free_vars() {
    while(head != NULL) {
        var * temp = head;
        head = head->next;
        free(temp);
    }
}

/**
 * This function is responsible for handling the if/else command
 * Input: command (char*)
 * Output: none
*/
void if_func(char *command) {
    char condition[LEN], then_cmd[LEN], else_cmd[LEN], *token, *argv[WORDS_IN_CMD], *result_cmd[WORDS_IN_CMD];
    int is_pipe = 0, i = 0;

    if(strstr(command, "then") && strstr(command, "else")) 
        sscanf(command, "if %[^\n;]; then %[^\n;]; else %[^\n;]; fi;", condition, then_cmd, else_cmd);
    else if(strstr(command, "then")) sscanf(command, "if %[^\n;]; then %[^\n;]; fi;", condition, then_cmd);
    else if(strstr(command, "else")) sscanf(command, "if %[^\n;]; else %[^\n;]; fi;", condition, else_cmd);
    else {
        status = 2;
        printf("bash: syntax error near unexpected token `fi'\n");
        return;
    }

    token = strtok(condition," ");
    while (token != NULL) {
        argv[i] = token;
        token = strtok(NULL, " ");
        if(strcmp(argv[i] , "|") == 0) is_pipe++;
        i++;
    }
    argv[i] = NULL;

    if(is_pipe > 0) {
        FILE *original_stdout = stdout;
        freopen("/dev/null", "w", stdout);
        pipe_func(argv, is_pipe);
        freopen("/dev/tty", "w", stdout);
    }
    else run(argv);

    if(WEXITSTATUS(status) == 0) handle_argv(then_cmd);
    else handle_argv(else_cmd);
}