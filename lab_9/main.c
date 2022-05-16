#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define ERROR_FORK -1
#define ERROR_WAIT -1
#define ERROR 1
#define SUCCESS 0

int main() {
    pid_t child = fork();
    if (child == ERROR_FORK) {
        perror("fork()");
        return ERROR;
    }
    if (child == SUCCESS) {
        execlp("cat", "cat", "text.txt", NULL);
        perror("exec");
        _exit(ERROR);
    }
    pid_t pid = wait(NULL);
    if (pid == ERROR_WAIT) {
        perror("wait");
        return ERROR;
    }
    printf("I am parent\n");

    return SUCCESS;
}
