#include <stdlib.h>
#include <stdio.h>
#include "header.h"

int CountReportingPoints(route Route)
{
    int i = 0;
    while (i < ROUTE_MAXLENGTH && Route.point_indexes[i] != -1)
        i++;
    /* printf("\033[31mNb Points Report : %i\033[0m", i); */
    return i;
}

//checks that route padding with -1 is correct (all -1 must be grouped at the end)
bool checkRoutePadding(route Route)
{
    int i = 0;
    while (Route.point_indexes[i] != -1 && i != (ROUTE_MAXLENGTH - 1)) //finds the first -1
        i++;
    if (i < ROUTE_MAXLENGTH - 1) //if some -1 were found before the last element
    {
        for (int j = i; j < ROUTE_MAXLENGTH; j++)
        {
            if (Route.point_indexes[j] != -1)
                return False;
        }
    }
    return True;
}

//positive modulo
unsigned int pos_modulo( int value, unsigned m) {
    int mod = value % (int)m;
    if (value < 0) {
        mod += m;
    }
    return mod;
}

//performs a circular permutation to the right until all -1 are grouped at the end
route circularPermutation(route Route)
{
    route newRoute;
    for (int i = 0; i < ROUTE_MAXLENGTH; i++)
    {
        newRoute.point_indexes[i] = Route.point_indexes[pos_modulo(i - 1,ROUTE_MAXLENGTH)];
    }
    return newRoute;
}

route reverseRoute(route Route)
{
    for (int i = 0; i < ROUTE_MAXLENGTH / 2; i++)
    {
        if (Route.point_indexes[i] != Route.point_indexes[ROUTE_MAXLENGTH - 1 - i])
        { //we swap each element that are at equal distance from the middle
            int temp = Route.point_indexes[i];
            Route.point_indexes[i] = Route.point_indexes[ROUTE_MAXLENGTH - 1 - i];
            Route.point_indexes[ROUTE_MAXLENGTH - 1 - i] = temp;
        }
    }

    while (!checkRoutePadding(Route))
        Route = circularPermutation(Route);

    return Route;
}

void PrintRoute(route Route)
{
    int nb_pts = CountReportingPoints(Route);
    for (int i = 0; i < nb_pts; i++)
    {
        ColorVerbose(MAIN, False, False, "%s", BSL_reporting_points[Route.point_indexes[i]].id);
        if (i != (nb_pts - 1))
            printf("-");
    }
}

report_pt ReportPointAtIndex(int point_index, route Route){
    for(int i = 0;i<CountReportingPoints(Route);i++){
        if(Route.point_indexes[i]==point_index)
            return BSL_reporting_points[i];
    }
    return out;
}

/* void PrintRouteRaw(route Route)
{
    for (int i = 0; i < ROUTE_MAXLENGTH; i++)
    {
        if (Route.point_indexes[i] == -1)
            ColorVerbose(MAIN, False, False, "-1");
        else
            ColorVerbose(MAIN, False, False, BSL_reporting_points[Route.point_indexes[i]].id);
        if (i != ROUTE_MAXLENGTH - 1)
            printf("|");
    }
    printf(".. Is valid : %i\n",checkRoutePadding(Route));
} */
