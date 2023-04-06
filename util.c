#include "util.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



char *binary_to_hex(void *data, ssize_t n){
    char *ret = malloc(sizeof(char) *(3 * n));
    n--;
    n*=3;
    int i = 0;
    int j = 0;
    
    while(i < n){
        
        sprintf(ret+i, "%02x", *(char *)(data+ j));
        i+=2;
        j+=1;

        if(j % 16 == 0){
            sprintf(ret+i, "\n");
            i++;
        }
        sprintf(ret + i, " ");
        i++;
        //printf("ret: %s\n", ret);
      
    }
    
    return ret;
}



void *hex_to_binary(char *hex){
    int n = strlen(hex);
    void *ret = malloc(n+1);
    int *arr= malloc(n+1);
    int i = 0;
    int s = 0;
    
    // converts hex to int array
    while(hex[i] != (char) ('\0')){
        if(hex[i] >= '0' && hex[i]<= '9'){

            arr[s] = (int)(hex[i]-'0');
            s++;
        }
        else if(hex[i] >= 'a' && hex[i]<='f'){
            arr[s] = (int)(hex[i]-'a' + 10);
            s++;
        }   
        else if(hex[i] >= 'A' && hex[i]<='F'){
            arr[s] = (int)(hex[i]-'A' + 10);
            s++;
        }
        else if(hex[i] == ' ' ||  hex[i] == '\n' || hex[i] == '/'){}
        else{
            free(ret);
            printf("Error: invalid hex");
            return NULL;
        }
        i++;        
    }

    int j = 0;
    int k = 0;
    // uses int array to set hex value
    while(j < n){
        *(char *)(ret + j) = (char)(arr[k] * 16 + arr[k+1]);
       // printf("%s", (ret+ j));
        j ++;
        k+=2;
    }
    // printf("\nret: ",ret);
    free(arr);
    return ret;

}


