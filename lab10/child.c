#include <stdlib.h>

#define SUCCESS 0
#define ERROR 1

int main(int argc, char *argv[]){
    int parameter = *argv[1] - 48;
    if (parameter == 6) abort();

    return SUCCESS;
}
