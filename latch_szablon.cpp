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
#include <atomic>
#include <shared_mutex>
#include <vector>
#include <barrier>
#include <latch>
using namespace std::chrono_literals;

int main() {
	int x = 0;
	int y = 0;

	std::jthread yt([&] {
		std::this_thread::sleep_for(1s);
		x = 1;
	});

	std::jthread xt([&] {
		std::this_thread::sleep_for(1s);
		y = 1;
	});

	std::cout << "Waiting...\n";

	std::cout << std::format("x: {}, y: {}\n", x, y);
}