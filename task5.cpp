#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#define SHM_KEY 1234
#define SEM_EMPTY 0
#define SEM_FULL 1
#define SEM_MUTEX 2

struct SharedBuffer {
    int buffer[100];
    int count;
};

void producer(int shmId, int semId) {
    SharedBuffer* buffer = (SharedBuffer*)shmat(shmId, NULL, 0);

    for (int i = 0; i < 100; ++i) {
        semop(semId, &waitEmpty, 1);
        semop(semId, &waitMutex, 1);
        buffer->buffer[buffer->count++] = i;
        semop(semId, &signalMutex, 1);
        semop(semId, &signalFull, 1);
    }

    shmdt(buffer);
}

void consumer(int shmId, int semId) {
    SharedBuffer* buffer = (SharedBuffer*)shmat(shmId, NULL, 0);

    for (int i = 0; i < 100; ++i) {
        semop(semId, &waitFull, 1);
        semop(semId, &waitMutex, 1);
        int item = buffer->buffer[--buffer->count];
        std::cout << "Item: " << item << std::endl;
        semop(semId, &signalMutex, 1);
        semop(semId, &signalEmpty, 1);
    }

    shmdt(buffer);
}

int main() {
    int shmId = shmget(SHM_KEY, sizeof(SharedBuffer), IPC_CREAT | 0666);
    if (shmId == -1) {
        perror("shmget");
        return 1;
    }

    SharedBuffer* buffer = (SharedBuffer*)shmat(shmId, NULL, 0);
    buffer->count = 0;

    int semId = semget(SHM_KEY, 3, IPC_CREAT | 0666);
    if (semId == -1) {
        perror("semget");
        return 1;
    }

    semctl(semId, SEM_EMPTY, SETVAL, 100);
    semctl(semId, SEM_FULL, SETVAL, 0);
    semctl(semId, SEM_MUTEX, SETVAL, 1);

    pid_t producerPid = fork();
    if (producerPid == 0) {
        producer(shmId, semId);
        return 0;
    } else if (producerPid < 0) {
        perror("fork");
        return 1;
    }

    pid_t consumerPid = fork();
    if (consumerPid == 0) {
        consumer(shmId, semId);
        return 0;
    } else if (consumerPid < 0) {
        perror("fork");
        return 1;
    }

    waitpid(producerPid, NULL, 0);
    waitpid(consumerPid, NULL, 0);

    shmctl(shmId, IPC_RMID, NULL);
    semctl(semId, 0, IPC_RMID);

    return 0;
}
