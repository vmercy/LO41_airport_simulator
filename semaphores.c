#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include "header.h"
/* 
int initsem(key_t semkey, int role)
{
	switch (role)
	{
	case MAIN:
		semid_init = semget(semkey, NB_SEMAPHORES, IFLAGS);
		break;

	default:
		semid_init = semget(semkey, NB_SEMAPHORES, SEMPERM);
		break;
	}
	if (semid_init < 0)
	{
		printf("\nErreur en creant les semaphores\n");
	}
	return semid_init;
}

//Pour operations P et V
struct sembuf sem_oper_P; // Operation P
struct sembuf sem_oper_V; // Operation V

void P(int semnum)
{
	sem_oper_P.sem_num = semnum;
	sem_oper_P.sem_op = -1;
	sem_oper_P.sem_flg = 0;
	int err = 0;
	if ((err = semop(semid, &sem_oper_P, 1)) < 0)
	{
		printf("\nErreur operation P : %i\n", err);
		exit(EXIT_FAILURE);
	}
}

void V(int semnum)
{
	sem_oper_V.sem_num = semnum;
	sem_oper_V.sem_op = 1;
	sem_oper_V.sem_flg = 0;
	int err = 0;
	if ((err = semop(semid, &sem_oper_V, 1)) < 0)
	{
		printf("\nErreur operation V : %i\n", err);
		exit(EXIT_FAILURE);
	}
}

int getVal(int semnum)
{ //lit la valeur du semaphore
	int retrn = semctl(semid, semnum, GETVAL);
	return retrn;
}

void initializeSemaphores()
{
	semctl(semid, SEM_PRINTF, SETVAL, 1);
	semctl(semid, SEM_2500_ATT, SETVAL, 1);
	semctl(semid, SEM_4000_ATT, SETVAL, 1);
	semctl(semid, SEM_2500_DEC, SETVAL, 0);
	semctl(semid, SEM_4000_DEC, SETVAL, 0);
	semctl(semid, SEM_PARKING, SETVAL, parkingCapacity);
} */