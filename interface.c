#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define BREAKDOWN 1
#define ATIS 2
#define EXIT 3

int main()
{
    pid_t target_pid = 0;
    printf("\nEntrez Main PID : ");
    scanf("%i", &target_pid);

    if (kill(target_pid, SIGUSR1) != 0)
    {
        printf("\nErreur de PID\n");
        exit(EXIT_FAILURE);
    }

    printf("\nSignal de synchronisation envoye\n");

    int chx = 0;
    printf("\nChoisir commande :\n1 - Declarer une panne avant atterissage \n2 - Changer l'ATIS\n3 - Fermer la console de controle\nVotre choix : ");
    scanf("%i", &chx);
    while (chx != EXIT)
    {
        switch (chx)
        {
        case BREAKDOWN:
            {
            pid_t BDownTargetPid = 0;
            printf("\nEntrez le PID de l'avion à déclarer en panne : ");
            scanf("%i", &BDownTargetPid);
            if (kill(BDownTargetPid, SIGUSR1) != 0)
            {
                printf("\nErreur de PID ou l'avion cible a deja atterri\n");
                exit(EXIT_FAILURE);
            }
            printf("\nSignal de panne envoye avec succes\n");
            break;
            }
        case ATIS:
            {
            if (kill(target_pid, SIGUSR2) != 0)
            {
                printf("\nErreur de PID\n");
                exit(EXIT_FAILURE);
            }
            printf("\nRequete de regeneration d'ATIS envoyee avec succes\n");
            break;
            }
        default:
            printf("Choix invalide, veuillez recommencer\n");
            break;
        }
        printf("\nChoisir commande :\n1 - Declarer une panne avant atterissage \n2 - Changer l'ATIS\n3 - Fermer la console de controle\nVotre choix : ");
        scanf("%i", &chx);
    }

    printf("\nFermeture de la console de controle\n");
}