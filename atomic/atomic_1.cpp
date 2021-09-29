#include <atomic>
#include <iostream>
#include <thread>
#include <chrono>
#include <format>
using namespace std::chrono_literals;

int main() {
	std::atomic<int> x;

	auto worker = [&](char n, std::chrono::milliseconds delay) {
		while (true) {
			int xx = x;
			std::cout << std::format("{} | Reading x = {}\n", n, xx);
			std::this_thread::sleep_for(delay);
			std::cout << std::format("{} | Trying to increment x\n", n);

			while (!x.compare_exchange_weak(xx, xx + 1)) {
				std::cout << std::format("{} | Failed! (x = {}) retrying...\n", n, xx);
			}

			std::cout << std::format("{} | Succeeded setting x to {}\n", n, xx + 1);
		}
	};

	std::jthread a(worker, 'A', 333ms);
	std::jthread b(worker, 'B', 1000ms);
}