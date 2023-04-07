
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include <fcntl.h>


int main(int argc, char *argv[]){

    // no arguments
    if (argc == 1){
        while(1){
        printf("Enter a binary value:");
        char *line;
        //line limit is 128
        size_t linelen = 128;
        size_t characters;


        line = (char *)malloc(linelen * sizeof(char));
        if(line == NULL)
        {
            perror("Unable to allocate buffer");
            exit(1);
        }

        characters = getline(&line, &linelen, stdin);
        if (characters > 128){
            printf("More than 128 characters entered\n");
        }

        char *hex = binary_to_hex((void *)line, (ssize_t) characters);
        printf("\nHex value: %s\n\n", hex);
        free(hex);
        }
    }
    else {
        for(char **pargv = argv + 1; *pargv != argv[argc]; pargv++){
            
            FILE *f;
            f = fopen(*pargv, "r+");


            if(f == NULL){
                printf("Error: Cannot open File: %s\n", *pargv);
                exit(2);
            }

            // get size of file
            FILE *temp = fopen(*pargv, "r");
            fseek(temp, 0, SEEK_END);
            int len = (ftell(temp)*sizeof(char)+1);
            fclose(temp);

            // read file
            char buf[len];
            fread(&buf, 1, len, f);    
            buf[len] = '\0';

            // convert to hex
            char *ret = binary_to_hex(buf, len);
            printf("%s: %s\n\n", *pargv, ret);

            fclose(f);
            free(ret);

        }

    }
}
