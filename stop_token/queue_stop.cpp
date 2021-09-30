#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <utility>
#include <stop_token>
#include <iostream>
#include <format>
#include <chrono>
using namespace std::chrono_literals;

template<class T>
class queue {
	std::mutex m;
	std::condition_variable_any emptiness;
	std::queue<T> q;
public:
	void push(T t) {
		{
			std::scoped_lock lock(m);
			q.push(std::move(t));
		}
		emptiness.notify_one();
	}

	T wait_and_pop(std::stop_token st) {
		std::unique_lock lock(m);
		if (emptiness.wait(lock, st, [this] { return not q.empty(); })) {
			auto const e = std::move_if_noexcept(q.front());
			q.pop();
			return e;
		}
		else {
			throw std::runtime_error("interrupted!");
		}
	}

	std::optional<T> try_pop() {
		std::scoped_lock lock(m);
		if (q.empty)
			return std::nullopt;
		else {
			auto const e = std::move_if_noexcept(q.front());
			q.pop();
			return e;
		}
	}

	bool empty() {
		std::scoped_lock lock(m);
		return q.empty();
	}
};

int main() {
	queue<int> q;
	std::jthread producer_thread([&](std::stop_token st) {
		while (!st.stop_requested()) {
			q.push(1);
			std::this_thread::sleep_for(1s);
		}
	});

	std::jthread consumer_thread([&](std::stop_token st) {
		unsigned counter = 0;
		try {
			while (true) {
				int x = q.wait_and_pop(st);
				std::cout << std::format("{}\n", x);
				counter++;
			}
		}
		catch (...) {
			std::cout << std::format("Interrupted! printed {} elements\n", counter);
		}
	});

	std::this_thread::sleep_for(15s);
}