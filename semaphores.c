#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "header.h"

int initsem(key_t semkey){
    int semid_init = semget(semkey, NB_SEMAPHORES, IFLAGS);
    return semid_init;
}

/*  Pour operations P et V   */
struct sembuf sem_oper_P; /* Operation P */
struct sembuf sem_oper_V; /* Operation V */

void P(int semnum)
{
	sem_oper_P.sem_num = semnum;
	sem_oper_P.sem_op = -1;
	sem_oper_P.sem_flg = 0;
	semop(semid,&sem_oper_P,1);
}

void V(int semnum)
{
	sem_oper_V.sem_num = semnum;
	sem_oper_V.sem_op = 1;
	sem_oper_V.sem_flg = 0;
	semop(semid,&sem_oper_V,1);
}

int getVal(int semnum){ //lit la valeur du semaphore
    int retrn = semctl(semid, semnum, GETVAL);
    return retrn;
}

void initializeSemaphores(){
    semctl(semid,SEM_PRINTF,SETVAL,1);
	semctl(semid,SEM_2500_ATT,SETVAL,1);
	semctl(semid,SEM_4000_ATT,SETVAL,1);
	semctl(semid,SEM_2500_DEC,SETVAL,0);
	semctl(semid,SEM_4000_DEC,SETVAL,0);
	semctl(semid,SEM_PARKING,SETVAL,parkingCapacity);
}