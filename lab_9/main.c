#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define ERROR_FORK -1
#define ERROR_WAIT -1
#define ERROR 1
#define SUCCESS 0

int main() {
    /* Создаем дочерний процесс
     * В случае ошибки возвращается -1 */
    pid_t child = fork();
    if (child == ERROR_FORK) {
        perror("fork()");
        return ERROR;
    }
    /* В случае успеха родительскому процессу возвращается pid потомка,
     * а потомку возвращается 0 */
    if (child == SUCCESS) {
    /* Замещяем тело дочернего процесса
     * В случае ошибки замещения не произойдет и выполнится возврат с ошибкой */
        execlp("cat", "cat", "text.txt", NULL);
        perror("exec: cat");
        _exit(ERROR);
    }
    /* Приостанавливаем процесс до выполнения дочернего процесса
       В случае ошибки возвращается -1*/
    //pid_t res_wait = wait(NULL);
    //if (res_wait == ERROR_WAIT) {
    //    perror("wait()");
    //    return ERROR;
    //}
    printf("I am parent\n");

    return SUCCESS;
}
