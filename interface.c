#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main(){
    pid_t target_pid = 0;
    printf("\nEntrez PID console : ");
    scanf("%i",&target_pid);

    if(kill(target_pid,SIGUSR1)!=0){
        printf("\nErreur de PID\n");
        exit(EXIT_FAILURE);
    }

    

    printf("\nSignal de synchronisation envoye\n");
}