#ifndef OS_LAB4_LIST_H
#define OS_LAB4_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERROR 1
#define SUCCESS 0
#define FIRST_SYMBOL 0
#define LAST_SYMBOL(x) (strlen(x) - 1)
#define FULL_LENGTH(x) (strlen(x) + 1)
#define SIZE_LINE 64

/* Создаем псевдоним для структуры */
typedef struct List {
    char* value;
    struct List* next;
} List;

/* Выделение памяти под элемент списка
 * Возвращает указатель на элемент списка
 * В случае ошибки возвращает NULL */
List* init_list();

/* Добавляет или изменяет строку в элементе списка и инициализирует следующий элемент
 * Возвращает указатель на следуюший элемент списка
 * В случае ошибки или передачи не инициализированного элемента списка возвращает NULL
 * Если переданая строка равна NULL никаких действий не совершается */
List* add_list(List* list, char* line);

/* Очистка памяти начиная с переданного элемента списка */
void clear_list(List* head);

/* Печать строк начиная с переданного элемента списка*/
void print_list(List* head);

/* Добавление строк из stdin в список
 * Критерий завершения записи: передача строки с первым символом '.'
 * В случае успеха возвращает SUCCESS
 * Иначе ERROR */
int entry_list(List* list);

/* Запись строки до символа '\n' или  передача строки с первым символом '.'
 * В случае успеха возвращает указатель на переданную строку
 * Иначе возвращает NULL */
char* write_line(char **result);

#endif
