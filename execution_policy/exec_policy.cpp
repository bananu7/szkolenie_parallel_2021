#include <algorithm>
#include <thread>
#include <array>
#include <iostream>
#include <execution>
#include <set>
#include <random>

using namespace std::chrono_literals;

const std::size_t n = 100000;

struct entry {
	std::thread::id thread_id;
	int thread_counter;
};

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dist(0, 1000);

entry foo(int) {
	thread_local int thread_counter = 0;

	auto ms = std::chrono::milliseconds(dist(gen));
	if (ms > 999ms)
		std::this_thread::sleep_for(ms);

	return { std::this_thread::get_id(), thread_counter++ };
}

int main() {
	std::vector<int> input(n);
	std::vector<entry> output(n);
	
	std::transform(std::execution::par, input.begin(), input.end(), output.begin(), foo);

	for (auto const& entry : output) {
		std::cout << entry.thread_id << " " << entry.thread_counter << '\n';
	}
	return 0;
}

/*
int main() {
	std::atomic<int> sum = 0;

	std::array<int, n> input;

	std::for_each(std::execution::par, input.begin(), input.end(),
		[&](int) { sum += 1; }
	);

	std::cout << sum << '\n';
}
*/