#include <thread>
#include <mutex>
#include <iostream>
#include <array>
#include <chrono>
#include <format>
#include <ranges>
using namespace std::chrono_literals;

int main() {
	const int n = 5;
	std::array<std::mutex, n> forks;
	
	auto philosopher = [&](int i) {
		std::mutex& left = forks[i];
		std::mutex& right = forks[(i + 1) % n];

		left.lock();
		std::this_thread::sleep_for(1s);
		right.lock();

		std::cout << std::format("Philosopher {} eating\n", i);

		left.unlock();
		right.unlock();
	};

	std::array<std::thread, n> philosophers;
	for (int i : std::views::iota(0,n)) {
		philosophers[i] = std::thread(philosopher, i);
	}
}