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
#include <condition_variable>

using namespace std::chrono_literals;

int main() {
	std::string s = "          ";
	
	bool writerWaiting;
	std::shared_mutex m;
	
	// reader (low-priority): wait([](w == false)), lock(M), ..., unlock(M)
	// writer (high-priority): w=true, lock(M),  ..., unlock(M), w=false

	std::condition_variable_any cv;

	auto writer = [&] {
		while (true) {
			writerWaiting = true;
			{
				std::unique_lock lock(m);
				for (char& c : s) {
					c = 'x';
					std::this_thread::sleep_for(50ms);
				}
				writerWaiting = false;
			}
			cv.notify_all();
			std::this_thread::sleep_for(150ms);

			writerWaiting = true;
			{
				std::unique_lock lock(m);
				for (char& c : s) {
					c = ' ';
					std::this_thread::sleep_for(50ms);
				}
				writerWaiting = false;
			}
			cv.notify_all();
			std::this_thread::sleep_for(150ms);
		}
	};

	auto reader = [&](int i) {
		while (true) {
			{
				std::shared_lock lock(m);
				cv.wait(lock, [&] {return !writerWaiting; });

				std::cout << std::format("{} : {}\n", i, s);
				std::this_thread::sleep_for(50ms);
			}
		}
	};

	std::jthread writerT(writer);
	std::jthread readerT(reader, 0);
	std::jthread readerT2(reader, 1);
}