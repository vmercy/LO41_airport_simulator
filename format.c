#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "header.h"


//Imprime sur stdout avec la bonne coloration en fonction du role
void ColorVerbose(int role, bool header, bool jump, char *fmt, ...)
{
    //P(SEM_PRINTF);
    va_list argsList;
    if (jump)
        printf("\n");
    switch (role)
    {
    case MAIN:
        printf("\033[0m"); //reset
        if (header)
            printf("*** MAIN : *******\n");
        break;
    case PLANE:
        printf("\033[96m"); //blue
        if (header)
            printf("*** AVION : *******\n");
        break;
    case ATIS:
        printf("\033[35m"); //magenta
        if (header)
            printf("*** ATIS : *******\n");
        break;
    case GND:
        printf("\033[92m"); //green
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

    va_start(argsList, fmt);
    vprintf(fmt, argsList);
    va_end(argsList);
    //printf("\033[0m"); //reset
    //V(SEM_PRINTF);
    fflush(stdout);
}

int nbDigits(int value){
    int res = 0;
    while (value!=0)
    {
        value/=10;
        res++;
    }
    return res;
}

char* zfill(int length, int value){ //prefix value with zeros until desired length
    char* ret = (char*)malloc(sizeof(char));
    char* res = ret;
    int space = length-nbDigits(value);
    for(int i = 0;i<space;i++){
        *res='0';
        res++;
    }
    sprintf(res,"%i",value);
    return ret;
}