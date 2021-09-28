#include <thread>
#include <ranges>
#include <mutex>
#include <array>
#include <iostream>
#include <chrono>
#include <map>
#include <format>
#include <random>

using namespace std::chrono_literals;

class Bank {
	std::map<int, int> accounts;
	mutable std::mutex m;

public:
	void add(int acc, int val = 100) {
		accounts.insert({ acc, val });
	}

	void transfer(int a, int b, int value) {
		{
			std::scoped_lock lock(m);
			accounts[b] += value;
		//}
			std::this_thread::sleep_for(10ms);
		//{
			//std::scoped_lock lock(m);
			accounts[a] -= value;
		}
	}

	int sum() const {
		std::scoped_lock lock(m);
		int sum = 0;
		for (int v : std::views::values(accounts)) {
			sum += v;
		}
		return sum;
	}
};

int main() {
	Bank bank;
	for (int i : std::views::iota(0, 10)) {
		bank.add(i);
	}
	
	std::jthread sum_worker([&] {
		while (true) {
			std::this_thread::sleep_for(50ms);
			std::cout << std::format("Sum : {}\n", bank.sum()); 
		}
	});

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> acc(0, 10);
	std::uniform_int_distribution<> val(30, 60);

	auto random_transfer = [&] {
		int counter = 100;
		while (counter--) {
			int a = acc(gen);
			int b = acc(gen);
			int v = val(gen);
			bank.transfer(a, b, v);

			std::cout << std::format("Transfer {} -> {} ({})\n", a, b, v);
			std::this_thread::sleep_for(50ms);
		}
	};

	std::array<std::jthread, 5> transfer_workers;
	for (auto& t : transfer_workers)
		t = std::jthread(random_transfer);
}