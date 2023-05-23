#include <iostream>
#include <cstdlib>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define SHM_KEY_FILE "shmkeyfile"

int main() {
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

    for (int i = 2; i <= 300; ++i) {
        sharedData[i] = i;
    }

    for (int i = 2; i <= 300; ++i) {
        if (sharedData[i] != 0) {
            for (int j = 2 * sharedData[i]; j <= 300; j += sharedData[i]) {
                sharedData[j] = 0;
            }
        }
    }

    if (shmdt(sharedData) == -1) {
        std::cerr << "Failed to detach shared memory: " << strerror(errno) << std::endl;
        return 1;
    }

    return 0;
}
