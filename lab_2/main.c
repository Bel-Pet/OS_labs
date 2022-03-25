#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#define ERROR_TIME (time_t) - 1
#define ERROR 1
#define SUCCESS 0

int current_time(time_t* now) {
    // функция ищет в списке окружения строку
    // в случае успеха возвращает указатель на значение в окружении, иначе NULL
    char* tz = getenv("TZ");
    if (tz == NULL) {
        fprintf(stderr, "getenv()");
        return ERROR;
    }

    printf("timezone %s\n", tz);
    
    // функция возвращает время в секундах, прошедшее с начала этой эпохи
    // в случае ошибки возвращает (time_t)-1
    int error_fun =  time(now);
    if (error_fun == ERROR_TIME) {
        
        perror("time()");
        return ERROR;
    }

    // ctime() преобразует календарное время в переменной now в строку
    // формат строки: день недели, месяц, день, время, год
    char* result = ctime(now);
    if (result == NULL) {
        perror("ctime()");
        return ERROR;
    }

    printf("%s\n", result);
    return SUCCESS;
}

int main() {
    time_t now;

    printf("Before:\n");
    // Функция выводит текущую таймзону и время
    // В случае ошибки возвращает ERROR
    int result_current_time = current_time(&now);
    if (result_current_time == ERROR) return ERROR;

    // Изменяем перемунную среды окружения
    // Функция возвращает 0 если завершила работу корректно и -1 при ошибке
    int result_putenv = putenv("TZ=America/Los_Angeles");
    if (result_putenv != SUCCESS) {
        perror("putenv()");
        return ERROR;
    }

    // Повторно вызываем функцию для фиксирования изменений
    printf("After:\n");
    result_current_time = current_time(&now);
    if (result_current_time) return ERROR;

    return SUCCESS;
}
