#include <thread>
#include <chrono>
#include <iostream>
#include <atomic>
#include <condition_variable>
using namespace std::chrono_literals;

int main2() {
	std::stop_source ss;

	std::thread t([](std::stop_token t) {
		while (!t.stop_requested()) {
			std::cout << "tick!\n";
			std::this_thread::sleep_for(1s);
		}
		std::cout << "stop requested!\n";
	}, ss.get_token());

	std::this_thread::sleep_for(3s);

	ss.request_stop();
	t.join();

	std::this_thread::sleep_for(3s);
	return 0;
}

int main3() {
	std::mutex m;
	std::atomic_bool b = false;
	std::condition_variable cv;

	std::jthread t([&](std::stop_token t) {
		std::unique_lock<std::mutex> lock(m);
		std::stop_callback cb(t, [&] {
			lock.lock();
			lock.unlock();
			cv.notify_all();
		});
		cv.wait(lock, [&]{ return b.load() || t.stop_requested(); });
		std::cout << "end";
	});

	std::this_thread::sleep_for(10s);
	return 0;
}


int main() {
	std::mutex m;
	std::atomic_bool b = false;
	std::condition_variable_any cv;

	std::jthread t([&](std::stop_token t) {
		std::unique_lock<std::mutex> lock(m);
		cv.wait(lock, t, [&] { return b.load(); });
			std::cout << "end";
	});

	std::this_thread::sleep_for(10s);
}