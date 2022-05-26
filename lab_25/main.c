#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

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
#define READ_ERROR -1
#define END_READ 0

int sending_child(int file_descriptors[NUMBER_OF_DESCRIPTORS]) {
    int res_fork = fork();
    if (res_fork == FORK_ERROR) {
        perror("fork");

        return ERROR;
    }

    /// родительский процесс завершает свою работу в функции после удачного создания потомка
    if (res_fork > CHILD_CODE) return res_fork;

    /// закрываем конец отвечающего за запись в программный канала
    int res_close = close(file_descriptors[FIRST_DESCRIPTOR]);
    if (res_close == CLOSE_ERROR) {
        perror("close");
        exit(ERROR);
    }

    char buf[BUFFER_SIZE];
    int res_read = 0;

    do {
        res_read = read(STDIN_FILENO, buf, BUFFER_SIZE);
        if (res_read == READ_ERROR) {
            perror("read");
            exit(ERROR);
        }

        int res_write = write(file_descriptors[SECOND_DESCRIPTOR], buf, res_read);
        if (res_write == WRITE_ERROR) {
            perror("write to pipe");
            exit(ERROR);
        }
    } while (res_read > END_READ);

    /// закрываем оставшийся конец канала
    res_close = close(file_descriptors[SECOND_DESCRIPTOR]);
    if (res_close == CLOSE_ERROR) {
        perror("close");
        exit(ERROR);
    }

    exit(SUCCESS);
}

int receiving_child(int file_descriptors[NUMBER_OF_DESCRIPTORS]) {
    int res_fork = fork();
    if (res_fork == FORK_ERROR) {
        perror("fork");
        return ERROR;
    }

    /// родительский процесс завершает свою работу в функции после удачного создания потомка
    if (res_fork > CHILD_CODE) return SUCCESS;

    /// закрываем конец отвечающего за запись в программный канала
    int res_close = close(file_descriptors[SECOND_DESCRIPTOR]);
    if (res_close == CLOSE_ERROR) {
        perror("close");
        return ERROR;
    }

    char buf[BUFFER_SIZE];
    int res_read = 0;

    do {
        res_read = read(file_descriptors[FIRST_DESCRIPTOR], buf, sizeof(buf));
        if (res_read == READ_ERROR) {
            perror("read to pipe");
            exit(ERROR);
        }

        for (int i = 0; i < res_read; i++){
            buf[i] = toupper(buf[i]);
        }

        int res_write = write(STDOUT_FILENO, buf, res_read);
        if (res_write == WRITE_ERROR) {
            perror("write");
            exit(ERROR);
        }
    } while (res_read > END_READ);

    res_close = close(file_descriptors[FIRST_DESCRIPTOR]);
    if (res_close == CLOSE_ERROR) {
        perror("close");
        exit(ERROR);
    }

    exit(SUCCESS);
}

int main() {
    int file_descriptors[NUMBER_OF_DESCRIPTORS];

    int res_pipe = pipe(file_descriptors);
    if (res_pipe == PIPE_ERROR) {
        perror("pipe");
        return ERROR;
    }

    int res_sending_child = sending_child(file_descriptors);
    if (res_sending_child == ERROR) return ERROR;

    int res_receiving_child =  receiving_child(file_descriptors);
    if (res_receiving_child == ERROR) return ERROR;

    int res_close = close(file_descriptors[FIRST_DESCRIPTOR]);
    if (res_close == CLOSE_ERROR) {
        perror("close");
        return ERROR;
    }

    res_close = close(file_descriptors[SECOND_DESCRIPTOR]);
    if (res_close == CLOSE_ERROR) {
        perror("close");
        return ERROR;
    }
    wait(NULL);

    return SUCCESS;
}
