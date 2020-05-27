#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>

#include "header.h"

//Imprime sur stdout avec la bonne coloration en fonction du role
void ColorVerbose(int role, bool header, bool saut, char *fmt, ...)
{ 
    va_list liste_args;
    if (saut)
        printf("\n");
    switch (role)
    {
    case MAIN:
        printf("\033[0m"); //reset
        if (header)
            printf("*** MAIN : *******\n");
        break;
    case AVION:
        printf("\033[96m"); //bleu
        if (header)
            printf("*** AVION : *******\n");
        break;
    case ATIS:
        printf("\033[35m"); //magenta
        if (header)
            printf("*** ATIS : *******\n");
        break;
    case GND:
        printf("\033[92m"); //vert
        if (header)
            printf("*** GND : *******\n");
        break;
    case APP:
        printf("\033[33m"); //jaune
        if (header)
            printf("*** APP : *******\n");
        break;
    case TWR:
        printf("\033[31m"); //rouge
        if (header)
            printf("*** TWR : *******\n");
        break;
    default:
        printf("\n*** ERREUR DE ROLE *******\n");
        break;
    }

    va_start(liste_args, fmt);
    vprintf(fmt, liste_args);
    va_end(liste_args);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("\nParametres NB_AVIONS et CAPACITE_PARKING initialises a 20 et 10\n");
    }
    else{
        nbAvions = atoi(argv[1]);
        capaParking = atoi(argv[2]);
    }
    for (int i = 0; i < 6; i++)
        ColorVerbose(i,True,False,"BONJOUR\n");
    
    return 0;
}
