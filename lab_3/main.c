#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdbool.h>

#define ERROR_SETUID -1
#define ERROR 1
#define SUCCESS 0

int print_id_and_open_file() {
    // Выводим фактичексий идентифкатор пользователя в текущем процессе
    printf("Real user ID: %ld\n", getuid());
    // Эффективный идентифкатор пользователя в текущем процессе
    printf("Effective user ID: %ld\n", geteuid());

    FILE *file = fopen("hello", "r");
    if (file == NULL) {
        perror("fopen()");
        return ERROR;
    }

    // Закрываем файл
    // В случае успеха возвращает 0, иначе EOF
    int result_fclose = fclose(file);
    if (result_fclose == EOF) {
        perror("fclose()");
        return ERROR;
    }
    
    return SUCCESS;
}

int main(int argc, char*argv[]) {

    printf("Before:\n");
    // Функция выводит фактичексий и эффективный идентифкаторы и окрывает файл для чтения
    // В случае успеха возвращает SUCCESS, иначе ERROR
    int result = print_id_and_open_file();
    if (result == ERROR) return ERROR;

    // Изменяем эффективный идентификатор процесса на фактический
    // В случае успеха возвращает 0, иначе -1
    int result_setuid = setuid(getid());
    if (result_setuid == ERROR_SETUID) {
        perror("setuid()");
        return ERROR;
    }

    printf("After:\n");
    // Повторный вызов после изменений
    result = print_id_and_open_file();
    if (result == ERROR) return ERROR;

    return SUCCESS;
}
