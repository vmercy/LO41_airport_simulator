#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>

#define MAX_VERBOSE_SIZE 100

#define MAIN 0 //noir
#define AVION 1 //bleu
#define ATIS 2 //violet
#define GND 3 //vert
#define APP 4 //jaune
#define TWR 5 //rouge

int main(int argc, char *argv[]){
    time_t now = time(NULL);
    struct tm ETA = *localtime(&now);
    printf("Temps : %s\n",asctime(&ETA));

    ColorVerbose(MAIN,1,"BONJOUR %i %i %i",0,1,78);
    ColorVerbose(ATIS,1,"BONJOUR %i %i %i",0,1,78);
    /* char buffer[50];
    strcpy(buffer,"JE SUIS UNE LIGNE \nJE SUIS UNE AUTRE LIGNE"); 
    printf("%s",buffer); */

    return 0;
}