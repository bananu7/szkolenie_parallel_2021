#include <vector>
#include <future>
#include <memory>
#include <chrono>
#include <iostream>
#include <format>
#include <latch>
using namespace std::chrono_literals;

std::future<int> parallel_sum(std::vector<int> const& v) {
	const int num_threads = 2;

	class sum_state {
	public:
		int sum = 0;
		std::vector<int>::const_iterator currentItem;
		std::vector<int>::const_iterator end;
		std::mutex iter_mutex, sum_mutex;
		std::latch latch, thread_exit_latch;
		bool finished = false;

		sum_state(std::vector<int> const& v)
			: currentItem(v.begin())
			, end(v.end())
			, latch(v.size())
			, thread_exit_latch(num_threads)
		{
		}
	};

	auto state = std::make_unique<sum_state>(v);
	auto statePtr = state.get();

	std::promise<int> p;
	std::future<int> f = p.get_future();
	auto coordinator = [](std::unique_ptr<sum_state> state, std::promise<int> p) {
		state->latch.wait();
		p.set_value(state->sum);
		state->thread_exit_latch.wait();
	};

	auto worker = [](sum_state* state) {
		while (true) {
			int x;
			{
				std::unique_lock lock(state->iter_mutex);
				if (state->currentItem == state->end) {
					lock.unlock();
					state->thread_exit_latch.count_down();
					break;
				}

				x = *(state->currentItem)++;
				lock.unlock();
			}
			std::this_thread::sleep_for(1s);
			{
				std::scoped_lock lock(state->sum_mutex);
				std::cout << std::format("Adding {}\n", x);
				state->sum += x;
			}
			state->latch.count_down();
		}
	};

	std::thread t1(worker, statePtr);
	t1.detach();
	std::thread t2(worker, statePtr);
	t2.detach();

	std::thread tc(coordinator, std::move(state), std::move(p));
	tc.detach();

	return f;
}

int main() {
	auto v = std::vector<int>{ 1,43,13,33,11,9 };
	auto sum = parallel_sum(v);
	std::cout << sum.get() << '\n';
}