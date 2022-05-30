#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>


#define CLOSE_ERROR -1
#define WRITE_ERROR -1
#define READ_ERROR -1
#define SOCKET_ERROR -1
#define CONNECT_ERROR -1

#define END_READ 0
#define DEFAULT_PROTOCOL 0

#define SUCCESS 0
#define ERROR 1

#define BUFFER_SIZE 256

void handler(int sig) {
    char buf[] = "Server unreachable\n";
    size_t len = strlen(buf);
    write(STDERR_FILENO, buf, len);
}

int transfer_text(int file_descriptor) {
    char buf[BUFFER_SIZE];
    int res_read = 0;

    do {
        res_read = read(STDIN_FILENO, buf, BUFFER_SIZE);
        if (res_read == READ_ERROR) {
            perror("read");
            return ERROR;
        }

        int res_write = write(file_descriptor, buf, res_read);
        if (res_write == WRITE_ERROR) {
            perror("write to pipe");
            return ERROR;
        }
    } while (res_read > END_READ);

    return SUCCESS;
}

int connect_server(int file_descriptor) {
    /// создаем структуру описывающую адресс сокета
    struct sockaddr_un address;
    /// заполняем память нулями для корректной работы
    memset(&address, 0, sizeof(address));
    /// записываем тип коммуникации
    address.sun_family = AF_UNIX;
    /// записываем имя сервера
    char ADDRESS[] = "mysocket";
    size_t address_len = strlen(ADDRESS);
    strncpy(address.sun_path, ADDRESS, address_len);
    /// устанавливаем соединение с сервером
    int res_connect = connect(file_descriptor, (struct sockaddr*) &address, sizeof(address));
    if (res_connect == CONNECT_ERROR){
        perror("connect");
        close(file_descriptor);
        return ERROR;
    }

    return SUCCESS;
}

int main() {
    /// обрабатываем SIGPIPE в случае завершения сервера до завершения клиента
    void* sigset_result = signal(SIGPIPE, handler);
    if (sigset_result == SIG_ERR){
        perror("signal");
        return ERROR;
    }
    /// создаем локальный сокет
    int file_descriptor = socket(AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    if (file_descriptor == SOCKET_ERROR){
        perror("socket");
        return ERROR;
    }
    /// подключаемся к серверу
    int res_connect = connect_server(file_descriptor);
    if (res_connect == ERROR) {
        close(file_descriptor);
        return ERROR;
    }
    /// считываем и записываем данные в дескриптор сокета сервера
    int res_transfer = transfer_text(file_descriptor);
    if (res_transfer == ERROR) {
        close(file_descriptor);
        return ERROR;
    }
    /// закрываем дескриптор
    int close_result = close(file_descriptor);
    if (close_result == CLOSE_ERROR) {
        perror("close");
        return ERROR;
    }

    return SUCCESS;
}
