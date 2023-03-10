
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include <fcntl.h>



int main(int argc, char *argv[]){
    if (argc == 1){
        printf("Enter a hex value:");
        char *line;
        size_t linelen = 32;

        line = malloc(linelen * sizeof(char));
        if(line == NULL)
        {
            perror("Unable to allocate buffer");
            exit(1);
        }

        getline(&line, &linelen, stdin);
        printf("You typed: %s\n", line);
        void *hex = hex_to_binary(line);
        printf("\n Binary value: %s\n", (char *)hex);

        free(line);
    }
    else {
        for(char **pargv = argv + 1; *pargv != argv[argc]; pargv++){
            
            FILE *f = fopen(*pargv, "r");
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
            void *buf = malloc(strlen(*pargv) + 1);
            fread(buf, 1, len, f);
            printf("buffer %s\n", (char *)buf);

            // convert to binary
            void *ret = hex_to_binary((char *)buf);
            printf("binary %s\n\n", (char *)ret);


            fclose(f);
            free(buf);
            free(ret);
        
        }



    }
}
