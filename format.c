#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "header.h"

//Imprime sur stdout avec la bonne coloration en fonction du role
void ColorVerbose(int role, bool header, bool jump, char *fmt, ...)
{
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
    case AVION:
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
    printf("\033[0m"); //reset
}

