#include <util.h>
#include <sys/types.h>
#include <unistd.h>


int main(int argc, char){

    char * hex = binary_to_hex("HELLO", 5);
    printf("Hex: %s\n", hex);
    
    free(hex);
}


char *binary_to_hex(void *data, ssize_t n){
    char *ret = malloc(sizeof(char) * 2 * n);
    int i = 0;
   
    while(i < n){
        *(ret + i) = "%02x", data[i];
        i += 2;
        *(ret + i) = ' ';
        i ++;
        if(i % 16 == 0){
            *(ret + i) = '\n';
        }
        
    }

    return ret;
}



void *hex_to_binary(char *hex){

}