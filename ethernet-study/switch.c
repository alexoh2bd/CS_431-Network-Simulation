
#include <sys/random.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>




// Switch simulation

int main(int argc, char *argv[]){
    if (argc == 2){
        int x = atoi(*(argv + 1));
        printf("x = %d\n", x);
        printf("Simulating %d devices... \n", x);

        
        int framesDenied = 0;
        int framesSent =0;
                
        int collided[x];  
        // loop through 1000 cycles
        int time =0;
        while(time < 1000){

            //printf("\nTime: %d.\n", time);
              
            for(int a = 0; a < x; a++){
                collided[a] = 0;
            }

            // loop through devices to check for collisions
            for(int i = 0; i<x; i++){
                // if device is supposed to send now, add to collided
                int randtemp=0;
                getrandom(&randtemp, 1, 0);
                int randdouble = round((double)randtemp/ 256);

                // sent frame
                if(randdouble == 1){
                    
                    //printf("Device %d sent.\n", i);

                    //determine target
                    int randvar=0;
                    getrandom(&randvar, 1, 0);
                    double target = (double)x * ((double)randvar/256);

                    
                    // if the target is the receiving device
                    if((int)target == i){
                        if(target == x-1){
                            target --;
                        }
                        target ++;
                    }
                    //printf("target %i.\n", (int)target);
                    framesSent++;

                    
                    collided[(int)target]++;
                    // printf("collided %i.\n", (int)collided[(int)target]);
                }
                
            }
            //check for collisions
            for(int j = 0; j<x; j++){
                if(collided[j] > 1 ){
                    framesDenied+=collided[j];
                    // printf("Collision detected: %d\n", framesDenied);
                }
            }

            time++;
        }
            double percentdenied = ((double)framesDenied/(double)framesSent)*100;
            printf("Frames sent: %d. Frames Denied: %d. Percentage accepted: %f%%.\n", framesSent, framesDenied, 100 - percentdenied);
            
            
            
        }

    
    else{
        printf("Enter 2 arguments\n\n");
    }
}



