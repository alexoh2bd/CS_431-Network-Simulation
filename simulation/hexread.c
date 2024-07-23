
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include <fcntl.h>



int main(int argc, char *argv[]){
    if (argc == 1){
        while(1){
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

        void *hex = hex_to_binary(line);
        printf("\n Binary value: %s\n", (char *)hex);

        free(line);
        }
    }
    else {
        for(char **pargv = argv + 1; *pargv != argv[argc]; pargv++){
            
            // open the file
            FILE *f = fopen(*pargv, "r");
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
            void *buf = malloc(strlen(*pargv) + 1);
            fread(buf, 1, len, f);

            // convert to binary
            void *ret = hex_to_binary((char *)buf);
            printf("%s:\n%s\n\n", *pargv, (char *)ret);


            fclose(f);
            free(buf);
            free(ret);
        
        }
    }
}
