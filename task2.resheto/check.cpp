#include <iostream>
#include <cstdlib>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <cmath>

#define SHM_KEY_FILE "shmkeyfile"

bool isPrime(int number) {
    if (number <= 1)
        return false;

    int sqrtNum = static_cast<int>(std::sqrt(number));

    for (int i = 2; i <= sqrtNum; ++i) {
        if (number % i == 0)
            return false;
    }

    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <number>" << std::endl;
        return 1;
    }

    int number = std::atoi(argv[1]);

    key_t key = ftok(SHM_KEY_FILE, 1);
    if (key == -1) {
        std::cerr << "Failed to generate key" << std::endl;
        return 1;
    }

    int shmId = shmget(key, 0, 0);
    if (shmId == -1) {
        std::cerr << "Failed to get shared memory: " << strerror(errno) << std::endl;
        return 1;
    }

    int* sharedData = static_cast<int*>(shmat(shmId, nullptr, 0));
    if (sharedData == reinterpret_cast<int*>(-1)) {
        std::cerr << "Failed to attach shared memory: " << strerror(errno) << std::endl;
        return 1;
    }

    bool isNumberPrime = isPrime(number);

    if (isNumberPrime) {
        std::cout << "Yes" << std::endl;
    } else {
        std::cout << "No" << std::endl;
    }

    if (shmdt(sharedData) == -1) {
        std::cerr << "Failed to detach shared memory: " << strerror(errno) << std::endl;
        return 1;
    }

    return 0;
}
