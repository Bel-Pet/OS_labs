#include <stdio.h>
#include "List.h"

#define ERROR 1
#define SUCCESS 0

int main() {
    List *head = init_list();
    if (head == NULL) {
        return ERROR;
    }

    int result = entry_list(head);
    if (result == ERROR) {
        clear_list(head);
        return ERROR;
    }

    print_list(head);

    clear_list(head);

    return SUCCESS;
}
