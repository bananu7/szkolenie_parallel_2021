#include <iostream>
#include <string>
#include <type_traits>
#include <vector>
#include <algorithm>
#include <map>
#include <stdexcept>
#include <numeric>
#include <random>
#include <thread>
#include <mutex>
#include <chrono>
#include <omp.h>
#include <atomic>
#include <shared_mutex>
#include <vector>
#include <barrier>
using namespace std::chrono_literals;

std::string str = "          ";

std::mutex io_mtx;
std::vector<int> v;

std::shared_mutex mtx;

std::barrier sync_point(2, []() noexcept {std::cout << "------------------------------\n"; });

void writer(const char cw, std::chrono::milliseconds pause) {
    char x = cw;

    while (true) {
        {
            std::unique_lock<std::shared_mutex> lock(mtx);
            x = cw;
            for (char& c : str) {
                c = x;
                std::this_thread::sleep_for(5ms);
            }
        }
        std::this_thread::sleep_for(pause);
        {
            std::unique_lock<std::shared_mutex> lock(mtx);
            x = ' ';
            for (char& c : str) {
                c = x;
                std::this_thread::sleep_for(5ms);
            }
        }

        sync_point.arrive_and_wait();
        std::this_thread::sleep_for(pause);
    }
}

void reader(int number) {
    std::string copy = str;

    while (true) {
        {
            std::shared_lock<std::shared_mutex> lock(mtx);
            copy = str;
        }
        {
            std::lock_guard<std::mutex> lock(io_mtx);
            std::cout << number << " [" << copy << "]\n";
        }

        std::this_thread::sleep_for(50ms);
    }
}

int main()
{
    std::jthread writer_x(writer, 'x', 1000ms);
    std::jthread writer_y(writer, 'y', 100ms);
    std::jthread readerT(reader, 0);
    std::jthread readerT2(reader, 1);
}

