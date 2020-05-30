#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>

#define LONGMAX_INDICATIF_PT_REPORT 2 //longueur maximum d'un indicatif de point de report
#define LONGMAX_ITINERAIRE 3          //nombre maximum de points de report pour un itineraire

#define KMTONM 0.539957 //convert factor kilometers -> nautic miles

int kmToNM(int km){return KMTONM*km;}

int main(int argc, char *argv[])
{
    printf("km : %i -> NM : %i",187,kmToNM(187));
    return 0;
}