
#include <sys/random.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>




// Hub simulation

int main(int argc, char *argv[]){
    if (argc == 2){
        int x = atoi(*(argv + 1));
        printf("x = %d\n", x);
        printf("Simulating %d devices... \n", x);

        
        int framesDenied = 0;
        int collidecount; 
        int framesSent =0;


        // loop through 1000 cycles
        int time =0;
        while(time < 1000){

            printf("\nTime: %d.\n", time);

            collidecount= 0;

            // loop through devices to check for collisions
            for(int i = 0; i<x; i++){
                // if device is supposed to send now, add to collided
                int randtemp=0;
                getrandom(&randtemp, 1, 0);
                double randdouble = round((double)randtemp/ 256);

                if(randdouble == 1){
                    printf("Device %d sent.\n", i);
                    
                    collidecount++;
                    
                    //printf("Collidecount: %d\n", collidecount);
                    if(collidecount == 2){
                        printf("Device %d collided.\n", i);
                        framesDenied+=2;
                    }
                    if(collidecount > 2){
                        printf("Device %d collided.\n", i);
                        framesDenied++;
                    }

                }
                
                
            }
            framesSent+=collidecount;

            time++;
        }
            double percentdenied = ((double)framesDenied/(double)framesSent)*100;
            printf("Frames sent: %d. Frames Denied: %d. Percentage accepted: %f%%.\n", framesSent, framesDenied, 100 - percentdenied);
            
            
            
        }

    
    else{
        printf("Enter 2 arguments\n\n");
    }
}



