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
#include <atomic>
#include <shared_mutex>
#include <vector>
#include <barrier>
#include <latch>
#include <future>
using namespace std::chrono_literals;

std::future<int> foo(std::chrono::seconds delay) {
    std::promise<int> promise;
    std::future<int> result = promise.get_future();

    std::thread t([=, p = std::move(promise)]() mutable {
        std::this_thread::sleep_for(delay);
        p.set_value_at_thread_exit(1);
    });
    t.detach();

    return result;
}

// packaged_task
// async

int main() {
	auto f = foo(3s);
    auto g = foo(6s);

    std::cout << "Waiting...";
    //std::cout << f.get() << g.get();
    //std::cout.operator<<(f.get()).operator<<(g.get());

    std::cout << std::format("{}{}", f.get(), g.get());
}