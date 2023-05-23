#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define SHM_KEY_FILE "shmkeyfile"

int main() {
    key_t key = ftok(SHM_KEY_FILE, 1);
    if (key == -1) {
        std::cerr << "Failed to generate key" << std::endl;
        return 1;
    }

    int shmId = shmget(key, 300 * sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    if (shmId == -1) {
        std::cerr << "Failed to create shared memory: ";
        if (errno == EEXIST) {
            std::cerr << "Shared memory already exists";
        } else {
            std::cerr << strerror(errno);
        }
        std::cerr << std::endl;
        return 1;
    }

    int* sharedData = static_cast<int*>(shmat(shmId, nullptr, 0));
    if (sharedData == reinterpret_cast<int*>(-1)) {
        std::cerr << "Failed to attach shared memory: " << strerror(errno) << std::endl;
        shmctl(shmId, IPC_RMID, nullptr);
        return 1;
    }

    memset(sharedData, 0, 300 * sizeof(int));

    if (shmdt(sharedData) == -1) {
        std::cerr << "Failed to detach shared memory: " << strerror(errno) << std::endl;
        shmctl(shmId, IPC_RMID, nullptr);
        return 1;
    }

    if (unlink(SHM_KEY_FILE) == -1) {
        std::cerr << "Failed to remove temporary file: " << strerror(errno) << std::endl;
        shmctl(shmId, IPC_RMID, nullptr);
        return 1;
    }

    std::cout << "Shared memory initialized successfully" << std::endl;

    return 0;
}
