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

    if (shmctl(shmId, IPC_RMID, nullptr) == -1) {
        std::cerr << "Failed to mark shared memory for removal: " << strerror(errno) << std::endl;
        return 1;
    }

    if (unlink(SHM_KEY_FILE) == -1) {
        std::cerr << "Failed to remove temporary file: " << strerror(errno) << std::endl;
        return 1;
    }

    std::cout << "Shared memory destroyed successfully" << std::endl;

    return 0;
}
