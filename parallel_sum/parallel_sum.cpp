#include <vector>
#include <future>
#include <memory>
#include <chrono>
#include <iostream>
#include <format>
using namespace std::chrono_literals;

std::future<int> parallel_sum(std::vector<int> const& v) {
	class sum_state {
	public:
		int sum = 0;
		std::vector<int>::const_iterator currentItem;
		std::vector<int> const& v;
		std::mutex iter_mutex, sum_mutex;
		std::promise<int> p;
		bool finished = false;

		sum_state(std::vector<int> const& v)
			: currentItem(v.begin())
			, v(v)
		{
		}

		auto [[nodiscard]] get_future() { return p.get_future(); }
	};

	auto state = std::make_shared<sum_state>(v);

	auto worker = [=]() mutable {
		while (true) {
			int x;
			{
				std::scoped_lock lock(state->iter_mutex);
				x = *(state->currentItem)++;
			}
			std::this_thread::sleep_for(1s);
			{
				std::scoped_lock lock(state->sum_mutex);
				std::cout << std::format("Adding {}\n", x);
				state->sum += x;
			}
			{
				std::scoped_lock lock(state->iter_mutex);
				if (state->currentItem == state->v.end()) {
					std::cout << "Reached the end\n";

					if (not state->finished) {
						std::cout << "First to finish, waiting for all sums to complete\n";
						std::scoped_lock sum_lock(state->sum_mutex);
						std::cout << "Setting promise result\n";
						state->p.set_value_at_thread_exit(state->sum);
						state->finished = true;
					}
					break;
				}
			}
		}
	};

	std::thread t1(worker);
	t1.detach();
	std::thread t2(worker);
	t2.detach();

	return state->get_future();
}

int main() {
	auto v = std::vector<int>{ 1,43,13,33,11,9 };
	auto sum = parallel_sum(v);
	std::cout << sum.get() << '\n';
}