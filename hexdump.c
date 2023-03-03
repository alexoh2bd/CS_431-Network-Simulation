
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.c"

int main(int argc, char *argv[]){
    if (argc == 0){
        printf("Enter a binary value:");
        char *line = NULL;
        size_t linelen = 0;

        getline(&line, linelen, stdin);

        char *hex = binary_to_hex((void *)line, (ssize_t) linelen);
        printf("\nHex value: %s\n", hex);
    }
}
