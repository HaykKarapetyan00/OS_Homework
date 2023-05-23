#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#define SHARED_MEMORY_KEY 1234
#define SEMAPHORE_KEY 5678

void increment(int* sharedNumber, int semId) {
    struct sembuf semaphoreOperation;
    semaphoreOperation.sem_num = 0;
    semaphoreOperation.sem_op = -1;
    semaphoreOperation.sem_flg = 0;

    for (int i = 0; i < 10000; ++i) {
        semop(semId, &semaphoreOperation, 1);
        (*sharedNumber)++;
        semaphoreOperation.sem_op = 1;
        semop(semId, &semaphoreOperation, 1);
    }
}

int main() {
    int shmid = shmget(SHARED_MEMORY_KEY, sizeof(int), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        return 1;
    }
    int* sharedNumber = (int*)shmat(shmid, NULL, 0);
    if (sharedNumber == (int*)-1) {
        perror("shmat");
        return 1;
    }

    *sharedNumber = 0;

    int semId = semget(SEMAPHORE_KEY, 1, IPC_CREAT | 0666);
    if (semId == -1) {
        perror("semget");
        return 1;
    }

    semctl(semId, 0, SETVAL, 1);

    for (int i = 0; i < 2; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            increment(sharedNumber, semId);
            return 0;
        } else if (pid < 0) {
            perror("fork");
            return 1;
        }
    }

    for (int i = 0; i < 2; ++i) {
        wait(NULL);
    }

    std::cout << "Final value: " << *sharedNumber << std::endl;

    shmdt(sharedNumber);

    shmctl(shmid, IPC_RMID, NULL);

    semctl(semId, 0, IPC_RMID);

    return 0;
}
