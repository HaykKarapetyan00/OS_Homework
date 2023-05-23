#include <iostream>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <signal.h>

#define SEM_KEY 1234

struct Signal {
    int smokerId;
    bool hasTobacco;
    bool hasPaper;
    bool hasMatches;
};

int semaphoreId;

void handleSIGTERM(int signal) {
    semctl(semaphoreId, 0, IPC_RMID);
    exit(0);
}

void smoker(int smokerId, int itemIndex) {
    Signal signal;

    while (true) {
        semop(semaphoreId, NULL, 0);
        semop(semaphoreId, &signal, 1);

        if (itemIndex == smokerId && !signal.hasTobacco && !signal.hasPaper && !signal.hasMatches) {
            std::cout << (char)('T' + smokerId - 1) << std::endl;
        }

        semop(semaphoreId, NULL, 1);
    }
}

int main() {
    semaphoreId = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    if (semaphoreId == -1) {
        perror("semget");
        return 1;
    }

    Signal signal;
    signal.smokerId = 0;
    signal.hasTobacco = false;
    signal.hasPaper = false;
    signal.hasMatches = false;

    signal(SIGTERM, handleSIGTERM);

    for (int i = 1; i <= 3; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            smoker(i, i);
            return 0;
        } else if (pid < 0) {
            perror("fork");
            return 1;
        }
    }

    while (true) {
        char item;
        std::cin >> item;

        semop(semaphoreId, NULL, -1);

        switch (item) {
            case 't':
                signal.hasTobacco = true;
                break;
            case 'p':
                signal.hasPaper = true;
                break;
            case 'm':
                signal.hasMatches = true;
                break;
        }

        for (int i = 1; i <= 3; ++i) {
            signal.smokerId = i;
            semop(semaphoreId, &signal, 1);
        }

        if (!signal.hasTobacco && !signal.hasPaper && !signal.hasMatches) {
            kill(0, SIGTERM);
            return 0;
        }

        semop(semaphoreId, NULL, 1);
    }

    return 0;
}
