
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
        printf("You typed: %s\n", line);
        char *hex = binary_to_hex((void *)line, (ssize_t) characters);
        printf("\nHex value: %s\n", hex);
    }
    else {
        for(char **pargv = argv + 1; *pargv != argv[argc]; pargv++){
            
            FILE *f;
            f = fopen(*pargv, "r+");
            printf("File: %s\n", *pargv);
            if(f == NULL){
                printf("Error: Cannot open File: %s\n", *pargv);
                exit(2);
            }
            FILE *temp = fopen(*pargv, "r");

            // get size of file
            fseek(temp, 0, SEEK_END);
            int len = (ftell(temp)*sizeof(char)+1);
            printf("Length: %d\n", len);
            fclose(temp);

            // read file
            char buf[len];
            fread(&buf, 1, len, f);    
            buf[len] = '\0';
            printf("Buffer: %s\n", buf);

            // convert to hex
            char *ret = binary_to_hex(buf, len);
            printf("Hex value: %s\n", ret);

            fclose(f);
            free(ret);

        }

    }
}
