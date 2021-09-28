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

std::future<int> foo2(std::chrono::seconds delay) {
    std::packaged_task task([=] {
        std::this_thread::sleep_for(delay);
        return 1;
    });
    auto result = task.get_future();
    std::thread t(std::move(task));
    t.detach();

    return result;
}

std::future<int> foo3(std::chrono::seconds delay) {
    return std::async(std::launch::async, [=] {
        std::this_thread::sleep_for(delay);
        return 1;
    });
}

int main() {
	auto f = foo3(1s);
    auto g = foo3(2s);

    std::cout << "Waiting...";
    //std::cout << f.get() << g.get();
    //std::cout.operator<<(f.get()).operator<<(g.get());

    std::cout << std::format("{}{}", f.get(), g.get());

    /*
    {
        std::promise<void> p;
        std::future<void> f = p.get_future();

        p.set_value();
        f.wait();
    }
    {
        std::promise<void> p;
        std::future<void> f = p.get_future();

        std::shared_future<void> sf(std::move(f));

        auto sf2 = sf;
        auto sf3 = sf;

        sf2.get();
    }*/
}
