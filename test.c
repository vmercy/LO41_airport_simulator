#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void traitantSIGUSR1(int num)
{
}

int main(int argc, char *argv[])
{
    char string[100];
    sprintf(string,"bonjour");
    printf("%s\n",string);
    sprintf(string,"bonjourkngioejvo");
    printf("%s\n",string);
    /* pid_t pid = fork();
    switch (pid)
    {
    case 0: //fils
        signal(SIGUSR1,traitantSIGUSR1);
        int i = 0;
        for (i = 0; i < 5; i++)
        {
            printf("\nFils : i = %i",i);
            pause();
        }
        exit(0);
        break;

    default: //pere
        while (1)
        {
            int rep = 0;
            printf("\nEntrer 1 pour continuer");
            while (rep!=1)
            {
                scanf("%i",&rep);
                kill(pid,SIGUSR1);
            }
        }
        wait(NULL);
        break;
    } */
    return 0;
}