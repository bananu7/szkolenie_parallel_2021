#include <atomic>
#include <iostream>
#include <thread>
#include <chrono>
#include <format>
using namespace std::chrono_literals;

template<typename T>
class lock_free_stack
{
private:
	struct node
	{
		T data;
		node* next;
		node(T data) : data(std::move(data)) { }
	};
	std::atomic<node*> head;
public:
	void push(T data)
	{
		node* n = new node(std::move(data));
		n->next = head;
		while (!head.compare_exchange_weak(n->next, n));
	}

	T pop(std::chrono::milliseconds delay) {
		node* h = head;

		// if null, it means stack empty
		if (not h) {
			throw std::runtime_error("empty stack");
		}

		std::cout << std::format("Trying to pop {}\n", h->data);

		std::this_thread::sleep_for(delay);

		while (!head.compare_exchange_weak(h, h->next)) {
			std::cout << std::format("Failed, next to pop is {}\n", h->data);
			// if failed, it means another thread either popped or pushed
			if (not h)
				throw std::runtime_error("empty stack");
		}

		std::cout << std::format("Preparing to delete {}\n", h->data);
		T t = std::move(h->data);
		delete h;
		return t;
	}
};

int main() {
	lock_free_stack<int> s;
	s.push(1);
	s.push(2);
	s.push(3);

	std::jthread a([&] {
		int x = s.pop(100ms);
	});

	std::jthread b([&] {
		int x = s.pop(10s);
	});
}