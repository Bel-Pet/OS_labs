#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define CLOSE_ERROR -1
#define WRITE_ERROR -1
#define READ_ERROR -1
#define SOCKET_ERROR -1
#define BIND_ERROR -1
#define LISTEN_ERROR -1
#define ACCEPT_ERROR -1

#define DEFAULT_PROTOCOL 0
#define END_READ 0

#define SUCCESS 0
#define ERROR 1

#define BUFFER_SIZE 256
#define MAX_QUEUE_LEN 1

int convert_text(int file_descriptor) {
    char buf[BUFFER_SIZE];
    int res_read = 0;

    do {
        res_read = read(file_descriptor, buf, sizeof(buf));
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

int setup_server_socket(int file_descriptor, char* address_name) {
    struct sockaddr_un address;

    memset(&address, 0, sizeof(struct sockaddr_un));

    address.sun_family = AF_UNIX;

    size_t addressLen = strlen(address_name);

    strncpy(address.sun_path, address_name, addressLen);
    /// присваиваем адрес сокету
    int res_bind = bind(file_descriptor, (struct sockaddr*) &address, sizeof(address));
    if (res_bind == BIND_ERROR){
        perror("bind");
        return ERROR;
    }
    /// выражаем готовность сокета принять соединение и задаем размер очереди
    int res_listen = listen(file_descriptor, MAX_QUEUE_LEN);
    if (res_listen == LISTEN_ERROR){
        perror("listen");
        return ERROR;
    }

    return SUCCESS;
}

int main() {

    char address_name[] = "mysocket";
    /// создаем локальный сокет
    int file_descriptor = socket(AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    if (file_descriptor == SOCKET_ERROR){
        perror("socket");
        return ERROR;
    }
    /// настраиваем сокет для принятия соединения
    int res_setup = setup_server_socket(file_descriptor, address_name);
    if (res_setup == ERROR) {
        close(file_descriptor);
        return ERROR;
    }
    /// получаем запрос от клиента и получаем дескриптор сокета соединенного с клиентом
    int res_accept = accept(file_descriptor, NULL, NULL);
    if (res_accept == ACCEPT_ERROR){
        perror("accept");
        close(file_descriptor);
        return ERROR;
    }
    /// считываем и конвертируем данные с последующим выводом
    int res_convert = convert_text(res_accept);
    if (res_convert == ERROR) {
        close(res_accept);
        close(file_descriptor);
        return ERROR;
    }
    /// закрываем дескрипторы
    int close_result = close(file_descriptor);
    if (close_result == CLOSE_ERROR) {
        perror("close");
        return ERROR;
    }
    close_result = close(res_accept);
    if (close_result == CLOSE_ERROR) {
        perror("close");
        return ERROR;
    }

    unlink(address_name);

    return SUCCESS;
}
