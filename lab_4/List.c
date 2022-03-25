#include "List.h"

List* init_list() {
    List *list = (List *) malloc(sizeof(List));
    if (!list) {
        perror("malloc()");
        return NULL;
    }
    list->value = NULL;
    list->next = NULL;

    return list;
}

List* add_list(List *list, char *line) {
    if (!line || !list) return list;

    list->value = (char*)calloc(FULL_LENGTH(line), sizeof(char));
    if (!list->value) {
        perror("calloc()");
        return NULL;
    }

    strcpy(list->value, line);

    if (!list->next) {
        list->next = init_list();
        if (list->next == NULL) return NULL;
    }

    return list->next;
}

void clear_list(List* head) {
    List* next;
    while (head) {
        next = head->next;
        free(head->value);
        free(head);
        head = next;
    }
}

void print_list(List* head) {
    while (head->value) {
        printf("%s", head->value);
        head = head->next;
    }
}

char* write_line(char **result) {
    char buffer[SIZE_LINE];
    char *res_fgets = fgets(buffer, sizeof(buffer), stdin);
    if (!res_fgets) {
        perror("fgets()");
        return NULL;
    }

    *result = (char*) calloc(FULL_LENGTH(buffer), sizeof(char));
    if (!*result) {
        perror("calloc(2)");
        return NULL;
    }

    if (FULL_LENGTH(buffer) * sizeof(char) <= sizeof(buffer)) {
        fprintf(stderr, "strcpy()");
        return NULL;
    }
    strcpy(*result, buffer);

    char *res_realloc;
    size_t new_size;
    while (buffer[FIRST_SYMBOL] != '.' && buffer[LAST_SYMBOL(buffer)] != '\n') {
        res_fgets = fgets(buffer, sizeof(buffer), stdin);
        if (!res_fgets) {
            perror("fgets()");
            return NULL;
        }

        new_size = (strlen(*result) + FULL_LENGTH(buffer)) * sizeof(char);
        res_realloc = (char*) realloc(*result, new_size);
        if (!res_realloc) {
            perror("realloc()");
            return NULL;
        }

        strcat(*result, buffer);
    }

    return *result;
}

int entry_list(List* list) {
    char *line = NULL;
    char *res_write_line;
    do {
        list = add_list(list, line);
        if (!list) {
            free(line);
            return ERROR;
        }

        res_write_line = write_line(&line);
        if(!res_write_line) {
            free(line);
            return ERROR;
        }

    } while (line[FIRST_SYMBOL] != '.');

    free(line);
    return SUCCESS;
}
