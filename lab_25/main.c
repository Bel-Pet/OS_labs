#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 256
#define SUCCESS 0
#define ERROR 1
#define PIPE_ERROR -1
#define FORK_ERROR -1
#define CHILD_CODE 0
#define NUMBER_OF_DESCRIPTORS 2
#define FIRST_DESCRIPTOR 0
#define SECOND_DESCRIPTOR 1
#define CLOSE_ERROR -1
#define WRITE_ERROR -1
#define TRUE 1
#define READ_ERROR -1
#define FILE_END 0


int transmitting_child(int file_descriptors[NUMBER_OF_DESCRIPTORS]) {
    int res_fork = fork();
    if (res_fork == FORK_ERROR) {
        perror("fork");
        return ERROR;
    }

    if (res_fork == CHILD_CODE) {
        char data[] = "The fork system call is used to create a new processes\n";

        int res_close = close(file_descriptors[FIRST_DESCRIPTOR]);
        if (res_close == CLOSE_ERROR) {
            perror("close");
            return ERROR;
        }

        int res_write = write(file_descriptors[SECOND_DESCRIPTOR], data, sizeof(data));
        if (res_write == WRITE_ERROR) {
            perror("write to pipe");
            return ERROR;
        }

        res_close = close(file_descriptors[SECOND_DESCRIPTOR]);
        if (res_close == CLOSE_ERROR) {
            perror("close");
            return ERROR;
        }

        exit(SUCCESS);
    }

    return SUCCESS;
}

int receiving_child(int file_descriptors[NUMBER_OF_DESCRIPTORS]) {
    int res_fork = fork();
    if (res_fork == FORK_ERROR) {
        perror("fork");
        return ERROR;
    }

    if (res_fork == CHILD_CODE) {
        int res_close = close(file_descriptors[SECOND_DESCRIPTOR]);
        if (res_close == CLOSE_ERROR) {
            perror("close");
            return ERROR;
        }

        char buf[BUFFER_SIZE];

        int res_read = read(file_descriptors[FIRST_DESCRIPTOR], buf, sizeof(buf));
        if (res_read == READ_ERROR) {
            perror("read");
            return ERROR;
        }

        for (int i = 0; i < res_read; i++){
            buf[i] = toupper(buf[i]);
        }

        int res_write = write(STDOUT_FILENO, buf, res_read);
        if (res_write == WRITE_ERROR) {
            perror("write");
            return ERROR;
        }

        res_close = close(file_descriptors[FIRST_DESCRIPTOR]);
        if (res_close == CLOSE_ERROR) {
            perror("close");
            return ERROR;
        }

        exit(SUCCESS);
    }

    return SUCCESS;
}

int main() {
    int file_descriptors[NUMBER_OF_DESCRIPTORS];

    int res_pipe = pipe(file_descriptors);
    if (res_pipe == PIPE_ERROR) {
        perror("pipe");
        return ERROR;
    }

    int res_transmitting_child = transmitting_child(file_descriptors);
    if (res_transmitting_child == ERROR) return ERROR;

    int res_receiving_child =  receiving_child(file_descriptors);
    if (res_receiving_child == ERROR) return ERROR;

    close(file_descriptors[FIRST_DESCRIPTOR]);
    close(file_descriptors[SECOND_DESCRIPTOR]);

    return SUCCESS;
}
