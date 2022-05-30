#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#define PIPE_ERROR -1
#define FORK_ERROR -1
#define CLOSE_ERROR -1
#define WRITE_ERROR -1
#define READ_ERROR -1
#define WAIT_ERROR -1

#define END_READ 0
#define CHILD_CODE 0

#define NUMBER_OF_DESCRIPTORS 2
#define FIRST_DESCRIPTOR 0
#define SECOND_DESCRIPTOR 1

#define SUCCESS 0
#define ERROR 1

#define BUFFER_SIZE 256

int transfer_text(int file_descriptors[NUMBER_OF_DESCRIPTORS]) {
    char buf[BUFFER_SIZE];
    int res_read = 0;

    do {
        res_read = read(STDIN_FILENO, buf, BUFFER_SIZE);
        if (res_read == READ_ERROR) {
            perror("read");
            return ERROR;
        }

        int res_write = write(file_descriptors[SECOND_DESCRIPTOR], buf, res_read);
        if (res_write == WRITE_ERROR) {
            perror("write to pipe");
            return ERROR;
        }
    } while (res_read > END_READ);

    return SUCCESS;
}

int create_and_start_sending_child(int file_descriptors[NUMBER_OF_DESCRIPTORS]) {
    int res_fork = fork();
    if (res_fork == FORK_ERROR) {
        perror("fork");
        return ERROR;
    }

    /// родительский процесс завершает свою работу в функции после удачного создания потомка
    if (res_fork > CHILD_CODE) return SUCCESS;

    /// закрываем конец отвечающий за запись в программный канала
    int res_close = close(file_descriptors[FIRST_DESCRIPTOR]);
    if (res_close == CLOSE_ERROR) {
        perror("close");
        exit(ERROR);
    }

    /// считываем текст и передаем его через программный канала
    int res_transfer = transfer_text(file_descriptors);
    if (res_transfer == ERROR) {
        close(file_descriptors[SECOND_DESCRIPTOR]);
        exit(ERROR);
    }

    /// закрываем оставшийся конец канала
    res_close = close(file_descriptors[SECOND_DESCRIPTOR]);
    if (res_close == CLOSE_ERROR) {
        perror("close");
        exit(ERROR);
    }

    exit(SUCCESS);
}

int convert_text(int file_descriptors[NUMBER_OF_DESCRIPTORS]) {
    char buf[BUFFER_SIZE];
    int res_read = 0;

    do {
        res_read = read(file_descriptors[FIRST_DESCRIPTOR], buf, sizeof(buf));
        if (res_read == READ_ERROR) {
            perror("read to pipe");
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
    } while (res_read > END_READ);

    return SUCCESS;
}

int create_and_start_receiving_child(int file_descriptors[NUMBER_OF_DESCRIPTORS]) {
    int res_fork = fork();
    if (res_fork == FORK_ERROR) {
        perror("fork");
        return ERROR;
    }

    /// родительский процесс завершает свою работу в функции после удачного создания потомка
    if (res_fork > CHILD_CODE) return SUCCESS;

    /// закрываем конец отвечающий за запись в программный канала
    int res_close = close(file_descriptors[SECOND_DESCRIPTOR]);
    if (res_close == CLOSE_ERROR) {
        perror("close");
        exit(ERROR);
    }

    /// считываем и преобразуем текст, затем выводим его
    int res_convert = convert_text(file_descriptors);
    if (res_convert == ERROR) {
        close(file_descriptors[FIRST_DESCRIPTOR]);
        exit(ERROR);
    }

    /// закрываем оставшийся конец канала
    res_close = close(file_descriptors[FIRST_DESCRIPTOR]);
    if (res_close == CLOSE_ERROR) {
        perror("close");
        exit(ERROR);
    }

    exit(SUCCESS);
}

int close_descriptors(int file_descriptors[NUMBER_OF_DESCRIPTORS]) {
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
    return SUCCESS;
}

int main() {
    /// создаем массив для заполнения его концами межпроцессорного канала
    int file_descriptors[NUMBER_OF_DESCRIPTORS];
    /// создаем межпроцессорный канал
    int res_pipe = pipe(file_descriptors);
    if (res_pipe == PIPE_ERROR) {
        perror("pipe");
        return ERROR;
    }

    /// создаем дочерний процесс, передающий текс по межпроцессорному каналу
    int res_sending_child = create_and_start_sending_child(file_descriptors);
    if (res_sending_child == ERROR) {
        close_descriptors(file_descriptors);
        return ERROR;
    }
    /// создаем дочерний процесс, принимающий текс по межпроцессорному каналу
    int res_receiving_child =  create_and_start_receiving_child(file_descriptors);
    if (res_receiving_child == ERROR) {
        close_descriptors(file_descriptors);
        return ERROR;
    }

    /// закрываем дескрипторы
    int res_close = close_descriptors(file_descriptors);
    if (res_close == ERROR) return ERROR;

    /// ожидаем завершения дочернего процесса
    int res_wait = wait(NULL);
    if (res_wait == WAIT_ERROR) return ERROR;

    return SUCCESS;
}
