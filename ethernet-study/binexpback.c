
#include <sys/random.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct device {
    int timecycle;
    int sendnext;
};



// Binary Exponential Backoff



int main(int argc, char *argv[]){
    if (argc == 2){
        int x = atoi(*(argv + 1));
        printf("x = %d\n", x);
        printf("Simulating %d devices... \n", x);

        // initialize devices
        struct device devices[x];
        for(int i = 0; i<x; i++){
            devices[i].sendnext = 0;
            devices[i].timecycle = 0;
        }
        
        int collidecount; 
        int devicesSent =0;


        // loop through time until devices are all sent
        int time =0;
        while(devicesSent < x){

            printf("\nTime: %d.\n", time);

            int collided[x];
            collidecount= 0;

            // loop through devices to check for collisions
            for(int i = 0; i<x; i++){
                // if device is supposed to send now, add to collided
                if(devices[i].sendnext == time){
                    printf("Device %d sent.\n", i);
                    collided[i] = 1;
                    collidecount++;
                    //printf("Collidecount: %d\n", collidecount);
                }
                else{
                    collided[i] = 0;
                }
            }
            
            
            // loop through devices to reset time cycle
            for(int j = 0; j < x; j++){
                // if no collision and device is supposed to send now, free device 
                if(collided[j] == 1 && collidecount <=1){
                    devices[j].sendnext = -1;
                    devicesSent ++;
                    printf("Device %d has been received without collisions. Timecycle #%d.\n", j, devices[j].timecycle);
                }
                // if collided at current timestep, randomize collision time, and increment timecycle
                else if(collided[j] == 1 && collidecount >= 2){
                    int randtemp=0;
                    getrandom(&randtemp, 1, 0);
                    // printf("Random number %d\n", randtemp);
                    //int twopower = pow((double)2, (double)devices[j].timecycle);
                    // printf("twopower %d\n", twopower);
                    double randdouble = (double)randtemp/ 256;
                    // printf("Random double %f\n", randdouble);
                    double range = pow(2, devices[j].timecycle);
                    // printf("timecycle %d\n", devices[j].timecycle;
                    randdouble = round(randdouble * range);

                    devices[j].sendnext = devices[j].sendnext + 1 + (int)randdouble;

                    printf("Random collision: device %d, waiting %d timesteps. Range: %d.\n", j, (int)randdouble, (int)pow(2, devices[j].timecycle));

                    devices[j].timecycle++;
                }
            }
            
            
            time++;
            // if(time == 5){
            //     break;
            // }
        }
        printf("All devices sent\n\n");
    }
    else{
        printf("Enter 2 arguments\n\n");
    }
}




 