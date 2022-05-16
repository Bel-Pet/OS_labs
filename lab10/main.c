#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define SUCCESS 0
#define ERROR 1
#define FORK_ERROR -1
#define CHILD_CODE 0
#define WAIT_ERROR -1
#define MIN_ARGS_NUM 2
#define PROGRAM_NAME 1
#define PROGRAM_ARGS 1

int executeOfGivenCommand(char* commandName, char* argv[]){
    pid_t child = fork();

    if(child == FORK_ERROR){
        perror("fork");
        return ERROR;
    }

    if(child == CHILD_CODE){
        execvp(commandName, argv);
        perror("execvp");
        return ERROR;
    }

    return SUCCESS;
}

int waitChildProcess(){
    int status = 0;
    pid_t pid = wait(&status);

    if(pid == WAIT_ERROR){
        perror("wait");
        return ERROR;
    }

    if(WIFSIGNALED(status)){
        printf("Child process terminated with a signal: %d\n", WTERMSIG(status));
    } else if(WIFEXITED(status)) {
        printf("Child process status: %d\n", WEXITSTATUS(status));
    }

    return SUCCESS;
}

int main(int argc, char **argv){

    if (argc < MIN_ARGS_NUM) {
        fprintf(stderr, "Not enough arguments\n");
        return ERROR;
    }

    char* commandName = argv[PROGRAM_NAME];
    char** commandArguments = &argv[PROGRAM_ARGS];

    int resultOfCreatingChild = executeOfGivenCommand(commandName, commandArguments);
    if(resultOfCreatingChild == ERROR) return ERROR;

    int resultWait = waitChildProcess();
    if(resultWait == ERROR) return ERROR;

    return SUCCESS;
}
