#include <iostream>
#include <thread>
#include <mutex>

std::once_flag flag1;

std::atomic_bool flag = false;

void atomic_do_once() {
    bool expected = false;
    if (flag.compare_exchange_strong(expected, true)) {
        std::cout << "Simple example: called once\n";
    }
}

void simple_do_once()
{  
    std::call_once(flag1, []() { std::cout << "Simple example: called once\n"; });
}

int main()
{
    std::jthread st1(simple_do_once);
    std::jthread st2(simple_do_once);
    std::jthread st3(simple_do_once);
    std::jthread st4(simple_do_once);
}