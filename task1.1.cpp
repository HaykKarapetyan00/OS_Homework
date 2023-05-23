#include <iostream>
#include <string>
#include <thread>
#include <mutex>

std::mutex mtx;

void copy_string(const std::string& src, std::string& dest, int start, int end) {
    for (int i = start; i < end; ++i) {
        dest[i] = src[i];
    }
}

int main() {
    std::string src, dest;
    std::cout << "Enter a string to copy: ";
    std::getline(std::cin, src);

    int length = src.length();
    dest.resize(length);

    int part_length = length / 4;
    int start = 0;
    int end = part_length;

    std::thread threads[4];
    for (int i = 0; i < 4; ++i) {
        threads[i] = std::thread(copy_string, std::ref(src), std::ref(dest), start, end);
        start = end;
        end += part_length;
    }

    for (int i = 0; i < 4; ++i) {
        threads[i].join();
}
std::cout << "Duplicated string: " << dest << std::endl;

return 0;
}