#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;
bool printerAvailable = true;

void print(const std::string& message) {
    std::unique_lock<std::mutex> lock(mtx);

    while (!printerAvailable) {
        cv.wait(lock);
    }

    printerAvailable = false;
    std::cout << "Printing: " << message << std::endl;
    printerAvailable = true;

    cv.notify_all();
}

int main() {
    std::thread threads[5];

    for (int i = 0; i < 5; ++i) {
        threads[i] = std::thread(print, "Thread " + std::to_string(i));
    }

    for (int i = 0; i < 5; ++i) {
        threads[i].join();
    }

    return 0;
}
