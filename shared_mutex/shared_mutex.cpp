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

int main() {
	std::string s = "          ";
	std::shared_mutex m;

	auto writer = [&] {
		while (true) {
			{
				std::unique_lock lock(m);
				for (char& c : s) {
					c = 'x';
					std::this_thread::sleep_for(50ms);
				}
			}
			std::this_thread::sleep_for(150ms);
			{
				std::unique_lock lock(m);
				for (char& c : s) {
					c = ' ';
					std::this_thread::sleep_for(50ms);
				}
			}
			std::this_thread::sleep_for(150ms);
		}
	};

	auto reader = [&](int i) {
		while (true) {
			{
				std::shared_lock lock(m);
				std::cout << std::format("{} : {}\n", i, s);
			}
			std::this_thread::sleep_for(50ms);
		}
	};

	std::jthread writerT(writer);
	std::jthread readerT(reader, 0);
	std::jthread readerT2(reader, 1);
}